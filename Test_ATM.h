#pragma once
#include <cassert>
#include "ATM.h"

class Test_ATM {
private:
	Database* _database;
	ATM _atm;
public:
	Test_ATM(Database& database) : _database(&database), _atm(database) {}

	void test_all() {
		// correct type of usage testing
		test_Booting_up_AND_autorising_AND_depositing_money();
		test_Booting_up_AND_autorising_AND_withdrawing_cash();
		test_Booting_up_AND_autorising_AND_transfering_money();
		test_Booting_up_AND_autorising_AND_sending_money_via_extra_payment_menu();

		// incorrect type of usage testing -> edge casing
		test_Using_ATM_before_booting_up_the_system();
		test_Authorising_with_invalid_data();
		test_Withdraw_edge_cases();
		test_Deposit_edge_cases();
		test_Transfer_edge_cases();
		test_Extra_Payement_Menu_edge_cases();

		std::cout << "All Tests Passed!" << std::endl;
	}

	// ================================================================================
	// Testing correct ATM usage 

	void test_Booting_up_AND_autorising_AND_depositing_money() {
		// boot up first, then initialize with 1 try, then add money
		double money_to_deposit = 1000.5;

		_atm.start();
		_atm.authenticator("1111111111111111", "1111");

		double initiali_balance = _atm.getSession()->accBalance();
		_atm.getSession()->deposit(money_to_deposit);
		double new_balance = _atm.getSession()->accBalance();

		assert((initiali_balance + money_to_deposit) == new_balance);

		_atm.endSession();
	}

	void test_Booting_up_AND_autorising_AND_withdrawing_cash() {
		// boot up first, then initialize with 1 try, then withdraw money
		double money_to_withdraw = 1000.5;

		_atm.start();
		_atm.authenticator("1111111111111111", "1111");

		double initiali_balance = _atm.getSession()->accBalance();
		_atm.getSession()->withdraw(money_to_withdraw);
		double new_balance = _atm.getSession()->accBalance();

		assert((initiali_balance - money_to_withdraw) == new_balance);

		_atm.endSession();
	}

	void test_Booting_up_AND_autorising_AND_transfering_money() {
		// boot up first, then initialize with 1 try, then transfer money
		double money_to_transfer = 100;

		_atm.start();
		_atm.authenticator("2222222222222222", "2222");
		double card_2_initial_balance = _atm.getSession()->accBalance();
		_atm.endSession();
		
		_atm.start();
		_atm.authenticator("1111111111111111", "1111");

		double card_1_initial_balance = _atm.getSession()->accBalance();
		_atm.getSession()->deposit(money_to_transfer);
		_atm.getSession()->transfer("2222222222222222", money_to_transfer);
		double card_1_new_balance = _atm.getSession()->accBalance();
		_atm.endSession();

		assert(card_1_initial_balance  == card_1_new_balance);

		_atm.start();
		_atm.authenticator("2222222222222222", "2222");
		double card_2_new_balance = _atm.getSession()->accBalance();
		_atm.endSession();

		assert((card_2_initial_balance + money_to_transfer) == card_2_new_balance);

		_atm.endSession();
	}

	void test_Booting_up_AND_autorising_AND_sending_money_via_extra_payment_menu() {
		// boot up first, then initialize with 1 try,then send money via extra payment menu
		int money_to_send = 500;

		_atm.start();
		_atm.authenticator("1111111111111111", "1111");

		double initiali_balance = _atm.getSession()->accBalance();
		_atm.getSession()->deposit(money_to_send);
		_atm.getSession()->paymentMenu("placeholder", "username", money_to_send);
		double new_balance = _atm.getSession()->accBalance();

		assert(initiali_balance == new_balance);

		_atm.endSession();
	}

	// =========================================================================

	void test_Using_ATM_before_booting_up_the_system() {
		// cant start using ATM without booting up the system
		assert(_atm.authenticator("wrongstate", "wrongstate") == ATM::Result::WrongATMState);
	}

	void test_Authorising_with_invalid_data() {
		_atm.start();
		assert(_atm.authenticator("0000000000000000", "1111") == ATM::Result::WrongCardNum);
		_atm.start();
		assert(_atm.authenticator("1111111111111111", "2222") == ATM::Result::WrongPIN);
		assert(_atm.authenticator("1111111111111111", "2222") == ATM::Result::WrongPIN);
		assert(_atm.authenticator("1111111111111111", "2222") == ATM::Result::CardWasBlocked);
		_atm.endSession();
	}

	void test_Withdraw_edge_cases() {
		// not enough money
		_atm.start();
		_atm.authenticator("1111111111111111", "1111");
		double card_original_balance = _atm.getSession()->accBalance();
		// >10k operations are forbidden now
		//assert(_atm.getSession()->withdraw(card_original_balance + 0.01) == ATM::Result::NotEnoughMoney); 
		assert(_atm.getSession()->accBalance() == card_original_balance);
		_atm.endSession();
	}

	void test_Deposit_edge_cases() {
		// function doesn't have any edge cases
	}

	void test_Transfer_edge_cases() {
		_atm.start();
		_atm.authenticator("1111111111111111", "1111");
		
		// invalid card
		assert(_atm.getSession()->transfer("1111111999111111", 999) == ATM::Result::WrongCardNum);

		// not enough money
		double card_original_balance = _atm.getSession()->accBalance();
		// >10k operations are forbidden now
		//assert(_atm.getSession()->transfer("1111111111111111", card_original_balance + 0.01) == ATM::Result::NotEnoughMoney);

		_atm.endSession();
	}

	void test_Extra_Payement_Menu_edge_cases() {
		_atm.start();
		_atm.authenticator("1111111111111111", "1111");

		// not enough money
		double card_original_balance = _atm.getSession()->accBalance();
		// >10k operations are forbidden now
		//assert(_atm.getSession()->paymentMenu("placeholder", "placeholder", card_original_balance + 0.01) == ATM::Result::NotEnoughMoney); 

		_atm.endSession();
	}

};