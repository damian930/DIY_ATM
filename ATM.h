#pragma once
#include "DataBase.h"

class ATM
{
public:
	class Session;
	enum class State;

	ATM(Database& db);
	~ATM();

	int authenticator(const string& cardNum, const string& pin);
	Session& getSession();
	void closeSession();

private:
	void setState(State state) { currentState = state; }
	State getState() { return currentState; }

	bool checkInDB(const string& cardNum) { return bank.isCardValid(cardNum);};
	const auto& getAccInfo(const string& cardNum);

	int wrongPINtimes = 0;
	Database& bank;
	Session* currentSession = nullptr;
	State currentState;
};

class ATM::Session
{
	friend int ATM::authenticator(const string& cardNum, const string& pin);
	friend Session& ATM::getSession();

public:
	enum class PayMenu;
	~Session();
	void exit();
	const auto& accInfo();
	bool withdraw(double);
	void deposit(double);
	int transfer(const string&, double);
	int paymentMenu(/*PayMenu,*/ const string&, double);
	
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

enum class ATM::Session::PayMenu
{
	UtilityServices,
	TravelCard,
	Charity,
	InfoTeleCom,
	Steam,
	Gambling
};