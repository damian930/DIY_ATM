#include "ATM.h"

ATM::ATM()
	:currentState(State::Idle) {}

ATM::~ATM()
{
	delete currentSession;
}


ATM::AccInfo ATM::getAccInfo(int cardNum)
{
	return { 1111, 1111, "Yurii", 10.50 };
}

void ATM::setAccInfo(ATM::AccInfo info, int cardNum)
{
	return;
}

ATM::Session::~Session() { delete _info; }

void ATM::Session::authenticator()
{

	// asks card num
	// asks pin
	int cardNum = 0;
	int pin = 0;

	if (pin != atm.getAccInfo(cardNum).PIN)
	{
		// +1
		atm.setState(State::Idle);
	}
	_info = new AccInfo(atm.getAccInfo(cardNum));
	atm.setState(State::ActionMenu);
}

void ATM::Session::accInfo()
{
	// show(info)
}

void ATM::Session::exit()
{
	// return card
	atm.setState(State::Idle);
}

void ATM::Session::withdraw()
{
	return;
}

void ATM::Session::deposit()
{
	return;
}

void ATM::Session::transfer()
{
	return;
}

void ATM::Session::paymentMenu()
{
	return;
}