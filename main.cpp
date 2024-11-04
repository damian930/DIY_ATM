#include <crow.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp> 
#include <cassert>
#include "DataBase.h"
#include "ATM.h"

using namespace std;
using json = nlohmann::json;

void initialiseDatabase(Database& database);
string load_html(const string& path);


int main() {
	crow::SimpleApp app;

	Database diyDB("diyDB.db");
	initialiseDatabase(diyDB);

	ATM diyATM(diyDB);

	/// First page the user sees
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

        // check if can read the file
        try {
            html_content = load_html("authorization.html");
        }
        catch (invalid_argument) {
            return crow::response(500, "");
        }
        return crow::response(200, html_content);
        });

	    // Route to serve Main HTML page
    CROW_ROUTE(app, "/main")([]() {
        string html_content;

        // check if can read the file
        try {
            html_content = load_html("main.html");
        }
        catch (invalid_argument) {
            return crow::response(500, "");
        }
		// start atm when loading main page 
		diyATM.start();
        return crow::response(200, html_content);
        });
	
	// get images 
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

		int res = diyATM.authenticator(card_number, card_number);

		// Respond back to the client 
		json response;
		response["cardNumber"] = res == 1 ? false : true;
		response["pin"] = res == 2 ? false : true;
		response["notBlocked"] = res == 3 ? false : true;

		return crow::response(200, response.dump()); // Send the JSON response
		});

	// Route to handle exit
	CROW_ROUTE(app, "/exit")([&diyATM]() {
		// Parse the JSON body

		if (diyATM.getSession() == nullptr)
			return crow::response(400);
		diyATM.endSession();

		return crow::response(200);
		});

	// Route to handle acc_info
	CROW_ROUTE(app, "/acc_info")([&diyATM]() {

		if (diyATM.getSession() == nullptr)
			return crow::response(400);
		nlohmann::json accInfo = diyATM.getSession()->accInfo();

		// Respond back to the client 
		json response;
		response["cardNumber"] = accInfo["cardNumber"];
		response["balance"] = accInfo["balance"];

		return crow::response(200, response.dump());

		});

	// Route to handle withdraw
	CROW_ROUTE(app, "/withdraw").methods("POST"_method)([&diyATM](const crow::request& req) {
		// Parse the JSON body
		auto data = json::parse(req.body);
		double amount = std::stod(data["amount"].get<nlohmann::json::string_t>());

		if (diyATM.getSession() == nullptr)
			return crow::response(400);
		bool success = diyATM.getSession()->withdraw(amount);
		// true if good, false if not enough money

		// Respond back to the client 
		json response;
		response["enoughMoney"] = success;

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
			int success = diyATM.getSession()->transfer(card_number_to, amount);
			// 0 - good; 1 - no card; 2 - no money

			// Respond back to the client 
			json response;
			response["cardNumber"] = success != 1;
			response["enoughMoney"] = success != 2;

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
			diyATM.getSession()->deposit(amount);
			crow::response res(200);
			return res;
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
			int res = diyATM.getSession()->paymentMenu(recipient, userID, amount);
			// true if good, false if not enough money

			// Respond back to the client 
			json response;
			response["enoughMoney"] = res != 2;
			response["correctID"] = res != 1;

			return crow::response(200, response.dump());
		});

	app.port(8000).multithreaded().run();

	return 0;
}

void initialiseDatabase(Database& database) {
	database.createCardsTable();

	database.insertCard("1111111111111111", "1111", 111.1);
	database.insertCard("2222222222222222", "2222", 222.2);
	database.insertCard("3333333333333333", "3333", 333.3);
	if (database.insertCard("1111111111111111", "1111", 111.1))
		assert(false);

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
