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

	AccInfo getAccInfo(int cardNum);
	void setAccInfo(AccInfo info, int cardNum);


private:
	class Session;

	Session* currentSession = nullptr;
	State currentState;
};

class ATM::Session
{
public:

	Session(ATM& myatm) :atm(myatm), _info(nullptr) {}
	~Session();

	void authenticator();

	void exit();
	void accInfo();
	void withdraw();
	void deposit();
	void transfer();
	void paymentMenu();
	
private:

	ATM& atm;
	AccInfo* _info;
};

struct ATM::AccInfo
{
	int cardNum;
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