#include <crow.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp> 
#include "UserDataBase.h"
#include "DIY_Error.h"

using namespace std;
using json = nlohmann::json;

void initialiseDatabase(UserDatabase& database);
string load_html(const string& path);


int main() {
    UserDatabase database;
    initialiseDatabase(database);

    crow::SimpleApp app;

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
    CROW_ROUTE(app, "/authorization").methods("POST"_method)([](const crow::request& req) {
        // Parse the JSON body
        auto data = json::parse(req.body); 
        std::string card_number = data["cardNumber"]; 
        std::string card_pin = data["pin"];

        bool isCardNumberValid = (card_number == "1234567812345678"); // Example validation
        bool isPinValid = (card_pin == "1234");

        // Respond back to the client 
        json response;
        response["cardNumber"] = isCardNumberValid;
        response["pin"] = isPinValid;

        return crow::response(200, response.dump()); // Send the JSON response
    });

    app.port(8000).multithreaded().run();

    return 0;
}

void initialiseDatabase(UserDatabase& database) {
    database.addCard("1111111111111111", "1111");
    database.addCard("2222222222222222", "2222");
    database.addCard("3333333333333333", "3333");
    database.addCard("4444444444444444", "4444");
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
