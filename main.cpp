#include <crow.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp> 
#include <cassert>
#include "DataBase.h"
#include "ATM.h"
#include "Test_ATM.h"

using namespace std;
using json = nlohmann::json;

string load_html(const string& path);

int main() {
	crow::SimpleApp app;

	Database diyDB("diyDB2.db");
	initialiseDatabase(diyDB);

	// testing right before letting someone use it
	Test_ATM test(diyDB);
	test.test_all();
	//============================================

	ATM diyATM(diyDB);

	
	// First page the user sees
	CROW_ROUTE(app, "/")([]() {
		string html_content;

		// check if can read the file
		try {
			html_content = load_html("start.html");
		}
		catch (invalid_argument) {
			return crow::response(500, "");
		}
		return crow::response(200, html_content);
	});

	// Route to serve Authorization HTML page
	CROW_ROUTE(app, "/authorization")([&diyATM]() {
		string html_content;

		// checks if can read the file
		try {
			html_content = load_html("authorization.html");
		}
		catch (invalid_argument) {
			return crow::response(500, "");
		}
		// changes ATM state to Authorization when loading main page 
		ATM::Error res = diyATM.start();
		return crow::response(200, html_content);
	});

	// Route to serve Main HTML page
	CROW_ROUTE(app, "/main")([&diyATM]() {
		string html_content;

		// check if can read the file
		try {
			html_content = load_html("main.html");
		}
		catch (invalid_argument) {
			return crow::response(500, "");
		}
		return crow::response(200, html_content);
	});
	
	// Gets images for UI
	CROW_ROUTE(app, "/static/<string>")([](const crow::request& req, std::string filename) {
		std::ifstream file("static/" + filename, std::ios::binary);
		if (!file.is_open()) {
			return crow::response(404);
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		return crow::response(buffer.str());
	});

	// Route to handle form submissions
	CROW_ROUTE(app, "/authorization/send").methods("POST"_method)([&diyATM](const crow::request& req) {
		// Parse the JSON body
		auto data = json::parse(req.body);
		std::string card_number = data["cardNumber"];
		std::string card_pin = data["pin"];

		ATM::Error res = diyATM.authenticator(card_number, card_pin);

		// Respond back to the client 
		json response;
		response["cardNumber"] = res != ATM::Error::WrongCardNum;
		response["pin"] = res != ATM::Error::WrongPIN;
		response["notBlocked"] = res != ATM::Error::CardWasBlocked;

		return crow::response(200, response.dump()); // Send the JSON response
	});

	// Route to handle exit
	CROW_ROUTE(app, "/exit")([&diyATM]() {
		// Parse the JSON body

		if (diyATM.getSession() == nullptr)
			return crow::response(400);
		ATM::Error res = diyATM.endSession();

		return crow::response(200);
	});

	// Route to handle acc_info
	CROW_ROUTE(app, "/acc_info")([&diyATM]() {

		if (diyATM.getSession() == nullptr)
			return crow::response(400);

		nlohmann::json accInfo = diyATM.getSession()->accInfo();
		//double accInfo = diyATM.getSession()->accInfo();

		// Respond back to the client 
		json response;
		response["cardNumber"] = accInfo["cardNumber"];
		response["balance"] = round(double(accInfo["balance"]) * 100)/100;

		return crow::response(200, response.dump());

	});

	// Route to handle withdraw
	CROW_ROUTE(app, "/withdraw").methods("POST"_method)([&diyATM](const crow::request& req) {
		// Parse the JSON body
		auto data = json::parse(req.body);
		double amount = std::stod(data["amount"].get<nlohmann::json::string_t>());

		if (diyATM.getSession() == nullptr)
			return crow::response(400);
		ATM::Error success = diyATM.getSession()->withdraw(amount);
		// can throw ATM::Error::BadMoneyValue

		// Respond back to the client 
		json response;
		response["enoughMoney"] = success != ATM::Error::NotEnoughMoney;

		return crow::response(200, response.dump());

	});

	// Route to handle transferring to another card
	CROW_ROUTE(app, "/transfer_to_card").methods("POST"_method)([&diyATM](const crow::request& req)
		{
			// Parse the JSON body
			auto data = json::parse(req.body);
			std::string card_number_to = data["cardNumber_to"];
			double amount = std::stod(data["amount"].get<nlohmann::json::string_t>());

			if (diyATM.getSession() == nullptr)
				return crow::response(400);
			ATM::Error success = diyATM.getSession()->transfer(card_number_to, amount);
			// can throw ATM::Error::BadMoneyValue

			// Respond back to the client 
			json response;
			response["cardNumber"] = success != ATM::Error::WrongCardNum;
			response["enoughMoney"] = success != ATM::Error::NotEnoughMoney;

			return crow::response(200, response.dump());
	});

	// Route to handle topping up the card
	CROW_ROUTE(app, "/top_up_the_card").methods("POST"_method)([&diyATM](const crow::request& req)
		{
			// Parse the JSON body
			auto data = json::parse(req.body);
			double amount = std::stod(data["amount"].get<nlohmann::json::string_t>());

			if (diyATM.getSession() == nullptr)
				return crow::response(400);
			ATM::Error res = diyATM.getSession()->deposit(amount);
			// can throw ATM::Error::BadMoneyValue

			json response;
			response["enoughMoney"] = true;
			return crow::response(200, response.dump());
	});

	// Route to handle payments
	CROW_ROUTE(app, "/payments").methods("POST"_method)([&diyATM](const crow::request& req)
		{
			// Parse the JSON body
			auto data = json::parse(req.body);
			string recipient = data["recipient"];
			string userID = data["user_id"];
			double amount = std::stod(data["amount"].get<nlohmann::json::string_t>());

			if (diyATM.getSession() == nullptr)
				return crow::response(400);
			ATM::Error res = diyATM.getSession()->paymentMenu(recipient, userID, amount);
			// can throw ATM::Error::BadMoneyValue

			// Respond back to the client 
			json response;
			response["enoughMoney"] = res != ATM::Error::NotEnoughMoney;
			response["correctID"] = res != ATM::Error::WrongCardNum;

			return crow::response(200, response.dump());
	});

	app.port(8000).multithreaded().run();

	return 0;
}

string load_html(const string& path) {
	ifstream file(path);
	if (!file.is_open()) {
		invalid_argument("");
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}
