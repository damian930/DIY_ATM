#pragma once
#include "IDataBase.h"

class ATM
{
public:
	class Session;
	enum class State;
	enum class Result;

	ATM(IDataBase& db);
	~ATM() { delete currentSession; };

	ATM::Result start();
	ATM::Result authenticator(const std::string& cardNum, const std::string& pin);
	Session* const getSession() { return currentSession; };
	ATM::Result endSession();

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
	ATM::Result withdraw(double);
	ATM::Result deposit(double);
	ATM::Result transfer(const std::string&, double);
	ATM::Result paymentMenu(const std::string&, const std::string&, double);
	
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

enum class ATM::Result
{
	EverythingIsFine = 0,
	WrongATMState,
	WrongCardNum,
	WrongPIN,
	CardWasBlocked,
	BadMoneyValue,
	NotEnoughMoney
};