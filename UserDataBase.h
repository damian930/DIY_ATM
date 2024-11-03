#pragma once
#include <iostream>
#include <map>
#include <string>
#include <set>
using namespace std;



class UserDatabase {
private:
    map<string, string> cardDatabase; 

    static const set<char> validChars;
    bool validateString(const string& str) {
        for (char c : str) {
            auto it = validChars.find(c);
            if (it == validChars.end())
                return false;
        }
        return true;
    }

public:
    static enum class Error {
        InvalidCardNumberLength,
        InvalidCardPinLength,
        CardAlreadyInUse,
        CardNumberIsNotInt,
        CardPinIsNotInt,
        CardNotExistant
    };

    UserDatabase() {}

    UserDatabase(const map<string, string>& other): 
        cardDatabase(other) {}



    void addCard(const string& cardNumber, const string& pin) {
        /*if (cardNumber.length() != 16) {
            throw Error::InvalidCardNumberLength;
        }

        if (pin.length() != 4) {
            throw Error::InvalidCardPinLength;
        }

        if (cardDatabase.find(cardNumber) != cardDatabase.end()) {
            throw Error::CardAlreadyInUse;
        }

        try {
            stoi(cardNumber);
        }
        catch(invalid_argument ex) { 
            throw Error::CardNumberIsNotInt;
        }

        try {
            stoi(pin);
        }
        catch (invalid_argument ex) {
            throw Error::CardPinIsNotInt;
        }*/

        if (cardNumber.length() != 16) {
            throw invalid_argument("");
        }

        if (pin.length() != 4) {
            throw invalid_argument("");
        }

        if (cardDatabase.find(cardNumber) != cardDatabase.end()) {
            throw invalid_argument("");
        }

        if (!validateString(cardNumber)) {
            throw invalid_argument("");
        }

        if (!validateString(pin)) {
            throw invalid_argument("");
        }

        cardDatabase[cardNumber] = pin;
    }

    // Subscript operator to access pin by card number
    string& operator[](const string& cardNumber) {
        auto it = cardDatabase.find(cardNumber);
        if (it == cardDatabase.end()) {
            //throw Error::CardNotExistant;
            throw invalid_argument("");
        }
        return it->second; // Return reference to the pin
    }

    // Const version of the subscript operator for read-only access
    const string& operator[](const string& cardNumber) const {
        auto it = cardDatabase.find(cardNumber);
        if (it == cardDatabase.end()) {
            //throw Error::CardNotExistant;
            throw invalid_argument("");
        }
        return it->second; // Return reference to the pin
    }

    // Method to display all cards (for debugging)
    void displayAllCards() const {
        for (const auto& entry : cardDatabase) {
            cout << "Card Number: " << entry.first << ", PIN: " << entry.second << endl;
        }
    }

};

const set<char> UserDatabase::validChars =
{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };




