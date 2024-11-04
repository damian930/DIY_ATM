#pragma once
#include <sqlite3.h>
#include <string>
#include <nlohmann/json.hpp> 
using std::string;

class Database {
public:
    Database(const string& dbName);
    ~Database();
    bool createCardsTable();
    bool insertCard(const string& cardNumber, const string& pin, double balance);
    nlohmann::json getCardDetails(const string& cardNumber);
    double getCardBalance(const string& cardNumber);
    bool addMoney(const string& cardNumber, double amount);
    bool removeMoney(const string& cardNumber, double amount);
    bool isCardValid(const string& cardNumber);
    bool isPinCorrect(const string& cardNumber, const string& pin);
    void close();

private:
    sqlite3* db;
};

