#include "ATM.h"

ATM::ATM(Database& db)
	:currentState(State::Idle), bank(db) {}


nlohmann::json ATM::getAccInfo(const string& cardNum)
{
	return bank.getCardDetails(cardNum);
}

bool ATM::start()
{
	if (getState() == ATM::State::Idle)
	{
		setState(ATM::State::Authorization);
		return true;
	}
	else 
		return false;
}

int ATM::authenticator(const string& cardNum, const string& pin)
{
	if (getState() != ATM::State::Authorization) return 4;

	if (bank.isCardValid(cardNum) == false)
	{
		setState(State::Idle);
		return 1; // no such a card
	}
	if (bank.isPinCorrect(cardNum, pin) == false)
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

void ATM::endSession()
{
	// return_the_card()
	setState(State::Idle);
	delete currentSession;
	currentSession = nullptr;
}

bool ATM::Session::withdraw(double amount)
{
	if (_atm.bank.getCardBalance(_cardNum) < amount) return false;
	_atm.bank.removeMoney(_cardNum, amount);
	// give_cash()
	return true;
}

void ATM::Session::deposit(double amount)
{
	// should have read amount from money-reader but we don't have it
	_atm.bank.addMoney(_cardNum, amount);
}

int ATM::Session::transfer(const string& recipient, double amount)
{
	if (_atm.checkInDB(recipient) == false)
		return 1; // no such a card
	if (_atm.bank.getCardBalance(_cardNum) < amount)
		return 2; // not enough money

	_atm.bank.removeMoney(_cardNum, amount);
	_atm.bank.addMoney(recipient, amount);

	return 0;
}

int ATM::Session::paymentMenu(const string& recipient, const string& userID, double amount)
{
	if (_atm.bank.getCardBalance(_cardNum) < amount) return 2;
	_atm.bank.removeMoney(_cardNum, amount);
	// send a check to the recipient
	return 0;
}