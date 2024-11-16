#pragma once
#include "IDataBase.h"

class ATM
{
public:
	class Session;
	enum class State;
	enum class Error;

	ATM(IDataBase& db);
	~ATM() { delete currentSession; };

	ATM::Error start();
	ATM::Error authenticator(const std::string& cardNum, const std::string& pin);
	Session* const getSession() { return currentSession; };
	ATM::Error endSession();

private:
	void setState(State state) { currentState = state; }
	State getState() { return currentState; }

	bool checkInDB(const std::string& cardNum) { return bank.isCardValid(cardNum);};
	nlohmann::json getAccInfo(const std::string& cardNum);
	double getAccBalance(const std::string& cardNum);

	int wrongPINtimes = 0;
	IDataBase& bank;
	Session* currentSession = nullptr;
	State currentState;
};

class ATM::Session
{
	friend class ATM;

public:
	~Session() {};
	nlohmann::json accInfo() { return _atm.getAccInfo(_cardNum); };
	double accBalance() { return _atm.getAccBalance(_cardNum); };
	ATM::Error withdraw(double);
	ATM::Error deposit(double);
	ATM::Error transfer(const std::string&, double);
	ATM::Error paymentMenu(const std::string&, const std::string&, double);
	
private:
	Session(ATM& myatm, const std::string& info) :_atm(myatm), _cardNum(info) {}

	ATM& _atm;
	const std::string _cardNum;
};

enum class ATM::State
{
	Mntnc = 0,
	Idle,
	Authorization,
	ActionMenu
};

enum class ATM::Error
{
	EverythingIsFine = 0,
	WrongATMState,
	WrongCardNum,
	WrongPIN,
	CardWasBlocked,
	BadMoneyValue,
	NotEnoughMoney
};