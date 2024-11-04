#pragma once
#include "DataBase.h"

class ATM
{
public:
	class Session;
	enum class State;

	ATM(Database& db);
	~ATM() { delete currentSession; };

	bool start();
	int authenticator(const string& cardNum, const string& pin);
	Session* const getSession() { return currentSession; };
	void endSession();

private:
	void setState(State state) { currentState = state; }
	State getState() { return currentState; }

	bool checkInDB(const string& cardNum) { return bank.isCardValid(cardNum);};
	nlohmann::json getAccInfo(const string& cardNum);

	int wrongPINtimes = 0;
	Database& bank;
	Session* currentSession = nullptr;
	State currentState;
};

class ATM::Session
{
	friend class ATM;

public:
	~Session() {};
	nlohmann::json accInfo() { return _atm.getAccInfo(_cardNum); };
	bool withdraw(double);
	void deposit(double);
	int transfer(const string&, double);
	int paymentMenu(const string&, const string&, double);
	
private:
	Session(ATM& myatm, const string& info) :_atm(myatm), _cardNum(info) {}

	ATM& _atm;
	const string& _cardNum;
};

enum class ATM::State
{
	Mntnc = 0,
	Idle,
	Authorization,
	ActionMenu
};