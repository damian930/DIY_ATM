#include "ATM.h"

ATM::ATM(Database& db)
	:currentState(State::Idle), bank(db) {}

ATM::~ATM()
{
	delete currentSession;
}

const auto& ATM::getAccInfo(const string& cardNum)
{
	return cardNum;
}

bool ATM::changeAccBalance(const string& cardNum, double)
{
	return false;
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
		// ++wrong_pin
		if (true /*<3*/)
		{
			return 2; // wrong pin
		}
		else
		{
			setState(State::Idle);
			return 3; // the card was blocked
		}
	}
	currentSession = new Session(*this, cardNum);
	setState(State::ActionMenu);
	return 0; // everything is great
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
	return _cardNum;
}

inline void ATM::Session::exit()
{
	_atm.closeSession();
}

inline bool ATM::Session::withdraw(double amount)
{
	// give the cash
	return _atm.changeAccBalance(_cardNum, -amount);
}

inline void ATM::Session::deposit(double amount)
{
	// should have read amount from money-reader but we don't have it
	_atm.changeAccBalance(_cardNum, amount);
	return;
}

int ATM::Session::transfer(const string& recipient, double amount)
{
	if (_atm.checkInDB(recipient) == false)
	{
		return 1; // no such a card
	}
	if (_atm.changeAccBalance(_cardNum, -amount) == false)
		return 2; // not enough money
	if (_atm.changeAccBalance(recipient, amount) == false)
		return 3; // huh?

	return 0;
}

int ATM::Session::paymentMenu(ATM::Session::PayMenu act, const string& id, double amount)
{
	if (_atm.changeAccBalance(_cardNum, -amount) == false)
		return 2; // not enough money
	// send a check to the owner
	return 0;
}