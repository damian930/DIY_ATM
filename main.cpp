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
			html_content = load_html("index.html");
		}
		catch (invalid_argument) {
			return crow::response(500, "");
		}
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
	CROW_ROUTE(app, "/authorization").methods("POST"_method)([&diyATM](const crow::request& req) {
		// Parse the JSON body
		auto data = json::parse(req.body); 
		std::string card_number = data["cardNumber"]; 
		std::string card_pin = data["pin"];

		int res = diyATM.authenticator(card_number, card_number);

		bool isCardNumberValid = res == 1? false: true;
		bool isPinValid = res == 2? false :true;
		//res == 3 //card was blocked

		// Respond back to the client 
		json response;
		response["cardNumber"] = isCardNumberValid;
		response["pin"] = isPinValid;

		return crow::response(200, response.dump()); // Send the JSON response
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
