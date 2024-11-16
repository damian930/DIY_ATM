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

ATM::Error ATM::start()
{
	if (getState() == ATM::State::Idle)
	{
		setState(ATM::State::Authorization);
		return ATM::Error::EverythingIsFine;
	}
	else 
		return ATM::Error::WrongATMState;
}

ATM::Error ATM::authenticator(const string& cardNum, const string& pin)
{
	if (getState() != ATM::State::Authorization) return ATM::Error::WrongATMState;

	if (bank.isCardValid(cardNum) == false)
	{
		setState(State::Idle);
		return ATM::Error::WrongCardNum; // no such a card
	}
	if (bank.isPinCorrect(cardNum, pin) == false)
	{
		++wrongPINtimes;
		if (wrongPINtimes < 3)
		{
			return ATM::Error::WrongPIN; // wrong pin
		}
		else
		{
			// cardReder->eatTheCard()
			setState(State::Idle);
			return ATM::Error::CardWasBlocked; // the card was blocked
		}
	}
	wrongPINtimes = 0;
	currentSession = new Session(*this, cardNum);
	setState(State::ActionMenu);
	return ATM::Error::EverythingIsFine; // everything is great
}

ATM::Error ATM::endSession()
{
	if (getState() != ATM::State::ActionMenu) return ATM::Error::WrongATMState;
	// cardReder->returnTheCard()
	setState(State::Idle);
	delete currentSession;
	currentSession = nullptr;
	return ATM::Error::EverythingIsFine;
}

bool isMoney(double val)
{
	if (val < 0) return false;
	if (abs(val * 100 - int(val * 100)) > 0) return false;
	if (val > 10'000) return false;
}

ATM::Error ATM::Session::withdraw(double amount)
{
	if (!isMoney(amount)) return ATM::Error::BadMoneyValue;
	if (_atm.bank.getCardBalance(_cardNum) < amount) return ATM::Error::NotEnoughMoney;
	_atm.bank.removeMoney(_cardNum, amount);
	// Dispenser->giveCash()
	return ATM::Error::EverythingIsFine;
}

ATM::Error ATM::Session::deposit(double amount)
{
	// should have read amount from money-reader but we don't have it
	if (!isMoney(amount)) return ATM::Error::BadMoneyValue;
	if (accBalance() + amount > 1'000'000'000) return ATM::Error::BadMoneyValue;
	_atm.bank.addMoney(_cardNum, amount);
	return ATM::Error::EverythingIsFine;
}

ATM::Error ATM::Session::transfer(const string& recipient, double amount)
{
	if (!isMoney(amount)) return ATM::Error::BadMoneyValue;
	if (!_atm.checkInDB(recipient)) return ATM::Error::WrongCardNum;
	if (_atm.bank.getCardBalance(_cardNum) < amount) return ATM::Error::NotEnoughMoney;
	if (_atm.getAccBalance(recipient) + amount > 1'000'000'000) return ATM::Error::BadMoneyValue;

	_atm.bank.removeMoney(_cardNum, amount);
	_atm.bank.addMoney(recipient, amount);

	return ATM::Error::EverythingIsFine;
}

ATM::Error ATM::Session::paymentMenu(const string& recipient, const string& userID, double amount)
{
	if (!isMoney(amount)) return ATM::Error::BadMoneyValue;
	if (_atm.bank.getCardBalance(_cardNum) < amount) return ATM::Error::NotEnoughMoney;
	_atm.bank.removeMoney(_cardNum, amount);
	// send a check to the recipient
	return ATM::Error::EverythingIsFine;
}