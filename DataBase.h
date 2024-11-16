#pragma once
#include <sqlite3.h>
#include "IDataBase.h"

class Database : public IDataBase {
    friend void initialiseDatabase(Database& database);
public:
    Database(const std::string& dbName);
    ~Database();

private:
    void createCardsTable();
    bool insertCard(const std::string& cardNumber, const std::string& pin, double balance);
    bool removeCard(const std::string& cardNumber);
    double getCardBalance(const std::string& cardNumber);
    nlohmann::json getCardDetails(const std::string& cardNumber);
    void addMoney(const std::string& cardNumber, double amount);
    void removeMoney(const std::string& cardNumber, double amount);
    bool isCardValid(const std::string& cardNumber);
    bool isPinCorrect(const std::string& cardNumber, const std::string& pin);
    void close();

private:
    sqlite3* db;
};

void initialiseDatabase(Database& database);

