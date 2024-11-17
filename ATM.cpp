#include "ATM.h"
using std::string;

ATM::ATM(IDataBase& db)
	:currentState(State::Idle), bank(db) {}


nlohmann::json ATM::getAccInfo(const string& cardNum)
{
	return bank.getCardDetails(cardNum);
}

double ATM::getAccBalance(const string& cardNum)
{
	return bank.getCardBalance(cardNum);
}

ATM::Result ATM::start()
{
	if (getState() == ATM::State::Idle)
	{
		setState(ATM::State::Authorization);
		return ATM::Result::EverythingIsFine;
	}
	else 
		return ATM::Result::WrongATMState;
}

ATM::Result ATM::authenticator(const string& cardNum, const string& pin)
{
	if (getState() != ATM::State::Authorization) return ATM::Result::WrongATMState;

	if (bank.isCardValid(cardNum) == false)
	{
		setState(State::Idle);
		return ATM::Result::WrongCardNum; // no such a card
	}
	if (bank.isPinCorrect(cardNum, pin) == false)
	{
		++wrongPINtimes;
		if (wrongPINtimes < 3)
		{
			return ATM::Result::WrongPIN; // wrong pin
		}
		else
		{
			// cardReder->eatTheCard()
			setState(State::Idle);
			return ATM::Result::CardWasBlocked; // the card was blocked
		}
	}
	wrongPINtimes = 0;
	currentSession = new Session(*this, cardNum);
	setState(State::ActionMenu);
	return ATM::Result::EverythingIsFine; // everything is great
}

ATM::Result ATM::endSession()
{
	if (getState() != ATM::State::ActionMenu) return ATM::Result::WrongATMState;
	// cardReder->returnTheCard()
	setState(State::Idle);
	delete currentSession;
	currentSession = nullptr;
	return ATM::Result::EverythingIsFine;
}

bool isMoney(double val)
{
	if (val <= 0) return false;
	if (abs(val * 100 - int(val * 100)) > 0) return false;
	if (val > 10'000) return false;
}

ATM::Result ATM::Session::withdraw(double amount)
{
	if (!isMoney(amount)) return ATM::Result::BadMoneyValue;
	if (_atm.bank.getCardBalance(_cardNum) < amount) return ATM::Result::NotEnoughMoney;
	_atm.bank.removeMoney(_cardNum, amount);
	// Dispenser->giveCash()
	return ATM::Result::EverythingIsFine;
}

ATM::Result ATM::Session::deposit(double amount)
{
	// should have read amount from money-reader but we don't have it
	if (!isMoney(amount)) return ATM::Result::BadMoneyValue;
	if (accBalance() + amount > 1'000'000'000) return ATM::Result::BadMoneyValue;
	_atm.bank.addMoney(_cardNum, amount);
	return ATM::Result::EverythingIsFine;
}

ATM::Result ATM::Session::transfer(const string& recipient, double amount)
{
	if (!isMoney(amount)) return ATM::Result::BadMoneyValue;
	if (!_atm.checkInDB(recipient)) return ATM::Result::WrongCardNum;
	if (_atm.bank.getCardBalance(_cardNum) < amount) return ATM::Result::NotEnoughMoney;
	if (_atm.getAccBalance(recipient) + amount > 1'000'000'000) return ATM::Result::BadMoneyValue;

	_atm.bank.removeMoney(_cardNum, amount);
	_atm.bank.addMoney(recipient, amount);

	return ATM::Result::EverythingIsFine;
}

ATM::Result ATM::Session::paymentMenu(const string& recipient, const string& userID, double amount)
{
	if (!isMoney(amount)) return ATM::Result::BadMoneyValue;
	if (_atm.bank.getCardBalance(_cardNum) < amount) return ATM::Result::NotEnoughMoney;
	_atm.bank.removeMoney(_cardNum, amount);
	// send a check to the recipient
	return ATM::Result::EverythingIsFine;
}