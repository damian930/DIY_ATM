#pragma once
#include <string>
#include <nlohmann/json.hpp> 

class IDataBase {
    friend class ATM;
public:
    virtual ~IDataBase() = default;
private:
    virtual void createCardsTable() = 0;
    virtual bool insertCard(const std::string& cardNumber, const std::string& pin, double balance) = 0;
    virtual bool removeCard(const std::string& cardNumber) = 0;
    virtual double getCardBalance(const std::string& cardNumber) = 0;
    virtual nlohmann::json getCardDetails(const std::string& cardNumber) = 0;
    virtual void addMoney(const std::string& cardNumber, double amount) = 0;
    virtual void removeMoney(const std::string& cardNumber, double amount) = 0;
    virtual bool isCardValid(const std::string& cardNumber) = 0;
    virtual bool isPinCorrect(const std::string& cardNumber, const std::string& pin) = 0;
    virtual void close() = 0;
};