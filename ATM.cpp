#include "ATM.h"

ATM::ATM()
	:currentState(State::Idle) {}

ATM::~ATM()
{
	delete currentSession;
}

ATM::AccInfo ATM::getAccInfo(long long cardNum)
{
	return { 1111222233334444, 1111, "Yurii", 10.50 };
}

void ATM::setAccBalance(long long cardNum, float)
{
	return;
}

int ATM::authenticator(long long cardNum, int pin)
{
	if (checkInDB(cardNum) == false)
	{
		setState(State::Idle);
		return 1; // no such a card
	}
	if (pin != getAccInfo(cardNum).PIN)
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
	currentSession = new Session(*this, new AccInfo(getAccInfo(cardNum)) );
	setState(State::ActionMenu);
	return 0; // everything is great
}

inline void ATM::closeSession()
{
	// return the card command
	setState(State::Idle);
	delete currentSession;
}

ATM::Session::~Session() { delete _info; }

inline const ATM::AccInfo& ATM::Session::accInfo()
{
	return *_info;
}

inline void ATM::Session::exit()
{
	_atm.closeSession();
}

bool ATM::Session::withdraw(float amount)
{
	if (_atm.getAccInfo(_info->cardNum).balance >= amount) // strict db check
	{
		_atm.setAccBalance(_info->cardNum, _info->balance - amount);
		// give the cash
		return true;
	}
	return false;
}

void ATM::Session::deposit()
{
	return;
}

int ATM::Session::transfer(long long recipient, float amount)
{
	if (_atm.checkInDB(recipient) == false)
	{
		return 1; // no such a card
	}
	if (_atm.getAccInfo(_info->cardNum).balance < amount) // strict db check
	{
		return 2; // not enough money
	}
	_atm.setAccBalance(_info->cardNum, _info->balance - amount);
	_atm.setAccBalance(recipient, _info->balance + amount);

	return 0;
}

int ATM::Session::paymentMenu(ATM::Session::PayMenu act, string id, float amount)
{
	if (_atm.getAccInfo(_info->cardNum).balance < amount) // strict db check
	{
		return 2; // not enough money
	}
	_atm.setAccBalance(_info->cardNum, _info->balance - amount);
	// send a check to owner
	return 0;
}