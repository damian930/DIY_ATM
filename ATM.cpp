#include "ATM.h"

ATM::ATM(Database& db)
	:currentState(State::Idle), bank(db) {}

ATM::~ATM()
{
	delete currentSession;
}

const auto& ATM::getAccInfo(const string& cardNum)
{
	return bank.getCardDetails(cardNum);
}

bool ATM::changeAccBalance(const string& cardNum, double amount)
{
	if (amount >= 0)
	{
		 bank.addMoney(cardNum, amount);
	}
	else
	{
		return bank.removeMoney(cardNum, -amount);
	}
}

int ATM::authenticator(const string& cardNum, const string& pin)
{
	if (bank.isCardValid(cardNum) == false)
	{
		setState(State::Idle);
		return 1; // no such a card
	}
	if (bank.isPinCorrect(cardNum, pin))
	{
		++wrongPINtimes;
		if (wrongPINtimes < 3)
		{
			return 2; // wrong pin
		}
		else
		{
			// eat_the_card()
			setState(State::Idle);
			return 3; // the card was blocked
		}
	}
	wrongPINtimes = 0;
	currentSession = new Session(*this, cardNum);
	setState(State::ActionMenu);
	return 0; // everything is great
}

ATM::Session& ATM::getSession()
{
	assert(currentSession == nullptr);
	return *currentSession;
}

inline void ATM::closeSession()
{
	// return the card command
	setState(State::Idle);
	delete currentSession;
}

ATM::Session::~Session() {}

inline const auto& ATM::Session::accInfo()
{
	return _atm.getAccInfo(_cardNum);
}

inline void ATM::Session::exit()
{
	_atm.closeSession();
}

bool ATM::Session::withdraw(double amount)
{
	if (_atm.bank.getCardBalance(_cardNum) < amount) return false;
	bool asrt = _atm.bank.removeMoney(_cardNum, amount);
	assert(!asrt);
	// give_cash()
	return true;
}

void ATM::Session::deposit(double amount)
{
	// should have read amount from money-reader but we don't have it
	bool asrt = _atm.bank.addMoney(_cardNum, amount);
	assert(asrt);
}

int ATM::Session::transfer(const string& recipient, double amount)
{
	if (_atm.checkInDB(recipient) == false)
		return 1; // no such a card
	if (_atm.bank.getCardBalance(_cardNum) < amount)
		return 2; // not enough money

	bool a = _atm.bank.removeMoney(_cardNum, amount);
	bool b = _atm.bank.addMoney(recipient, amount);
	assert(a && b);

	return 0;
}

int ATM::Session::paymentMenu(ATM::Session::PayMenu act, const string& id, double amount)
{
	if (_atm.changeAccBalance(_cardNum, -amount) == false)
		return 2; // not enough money
	// send a check to the owner
	return 0;
}