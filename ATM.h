#pragma once
#include <iostream>
using std::string;

class ATM
{
public:
	enum class State;
	struct AccInfo;

	ATM();
	~ATM();

	void setState(State state) { currentState = state; }
	State getState() { return currentState; }

	bool checkInDB(long long cardNum) { return true; };
	AccInfo getAccInfo(long long cardNum);
	void setAccBalance(long long cardNum, float);

	int authenticator(long long cardNum, int pin);
	void closeSession();

private:
	class Session;

	Session* currentSession = nullptr;
	State currentState;
};

class ATM::Session
{
	friend int ATM::authenticator(long long cardNum, int pin);

public:
	enum class PayMenu;
	~Session();

	void exit();
	const ATM::AccInfo& accInfo();
	bool withdraw(float);
	void deposit();
	int transfer(long long, float);
	int paymentMenu(PayMenu, string, float);
	
private:
	Session(ATM& myatm, ATM::AccInfo* info) :_atm(myatm), _info(info) {}

	ATM& _atm;
	ATM::AccInfo* _info;
};

struct ATM::AccInfo
{
	long long cardNum;
	int PIN;
	string fullName;
	float balance;
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