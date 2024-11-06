#pragma once
#include <sqlite3.h>
#include <string>
#include <nlohmann/json.hpp> 
using std::string;

class Database {
public:
    Database(const string& dbName);
    ~Database();
    void createCardsTable();
    bool insertCard(const string& cardNumber, const string& pin, double balance);
    bool removeCard(const string& cardNumber);
    double getCardBalance(const string& cardNumber);
    nlohmann::json getCardDetails(const string& cardNumber);
    void addMoney(const string& cardNumber, double amount);
    void removeMoney(const string& cardNumber, double amount);
    bool isCardValid(const string& cardNumber);
    bool isPinCorrect(const string& cardNumber, const string& pin);
    void close();

private:
    sqlite3* db;
};

