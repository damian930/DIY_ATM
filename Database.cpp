#include "Database.h"
#include <iostream>
#include <regex>
#include <nlohmann/json.hpp>
using namespace std;

Database::Database(const string& dbName)
{
    int exit = sqlite3_open(dbName.c_str(), &db);
    if (exit)
    {
        cerr << "Error opening DB: " << sqlite3_errmsg(db) << endl;
    }
    else
    {
        cout << "Opened Database Successfully!" << endl;
    }
}

Database::~Database()
{
    close();
}

void Database::createCardsTable()
{
    const char* sql = "CREATE TABLE IF NOT EXISTS Cards("
        "CardNumber TEXT NOT NULL PRIMARY KEY, "
        "PIN TEXT NOT NULL, "
        "CurrentBalance INTEGER NOT NULL);";
    char* errorMessage = 0;
    int exit = sqlite3_exec(db, sql, 0, 0, &errorMessage);
    if (exit != SQLITE_OK)
    {
        cerr << "Error creating table: " << errorMessage << endl;
        sqlite3_free(errorMessage);
        assert(false);
    }
}

bool Database::insertCard(const string& cardNumber, const string& pin, double dbalance)
{
    long long balance = long long(dbalance * 100);
    // Validate card number and PIN
    regex cardNumberPattern(R"(\d{16})"); // 16 digits
    regex pinPattern(R"(\d{4})");         // 4 digits

    if (!regex_match(cardNumber, cardNumberPattern))
    {
        cerr << "Error: Card number must be exactly 16 digits." << endl;
        assert(false);
    }
    if (!regex_match(pin, pinPattern))
    {
        cerr << "Error: PIN must be exactly 4 digits." << endl;
        assert(false);
    }

    // Check if the card already exists
    std::string sqlCheck = "SELECT COUNT(*) FROM Cards WHERE CardNumber = '" + cardNumber + "';";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sqlCheck.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int count = sqlite3_column_int(stmt, 0);
            if (count > 0)
            {
                cerr << "Error: Card already exists." << endl;
                sqlite3_finalize(stmt);
                return false;
            }
        }
    }
    else
    {
        cerr << "Error checking for existing card: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        assert(false);
    }
    sqlite3_finalize(stmt);

    string sql = "INSERT INTO Cards (CardNumber, PIN, CurrentBalance) VALUES ('" + cardNumber + "', '" + pin + "', " + std::to_string(balance) + ");";
    char* errorMessage = nullptr;
    int exit = sqlite3_exec(db, sql.c_str(), 0, 0, &errorMessage);
    if (exit != SQLITE_OK)
    {
        cerr << "Error inserting card: " << errorMessage << endl;
        sqlite3_free(errorMessage);
        assert(false);
    }
    return true;
}

bool Database::removeCard(const string& cardNumber) {
    // Validate card number
    regex cardNumberPattern(R"(\d{16})"); // 16 digits

    if (!regex_match(cardNumber, cardNumberPattern))
    {
        cerr << "Error: Card number must be exactly 16 digits." << endl;
        assert(false);
    }
    
    // Check if the card already exists
    std::string sqlCheck = "SELECT COUNT(*) FROM Cards WHERE CardNumber = '" + cardNumber + "';";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sqlCheck.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int count = sqlite3_column_int(stmt, 0);
            if (count == 0)
            {
                cerr << "Error: Card doesnt exist." << endl;
                sqlite3_finalize(stmt);
                return false;
            }
        }
    }
    else
    {
        cerr << "Error checking for existing card: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        assert(false);
    }
    sqlite3_finalize(stmt);
    string sql = "DELETE FROM Cards WHERE CardNumber = '" + cardNumber + "';";
    char* errorMessage = nullptr;
    int exit = sqlite3_exec(db, sql.c_str(), 0, 0, &errorMessage);
    if (exit != SQLITE_OK)
    {
        cerr << "Error deleting card: " << errorMessage << endl;
        sqlite3_free(errorMessage);
        assert(false);
    }
    return true;
}

void Database::addMoney(const string& cardNumber, double damount)
{
    long long amount = long long(damount * 100);
    if (amount <= 0)
    {
        cerr << "Error: Amount must be positive." << endl;
        assert(false);
    }

    // Update the balance by adding the amount
    string sql = "UPDATE Cards SET CurrentBalance = CurrentBalance + " + to_string(amount) + " WHERE CardNumber = " + cardNumber + ";";
    char* errorMessage = nullptr;
    int exit = sqlite3_exec(db, sql.c_str(), 0, 0, &errorMessage);
    if (exit != SQLITE_OK)
    {
        cerr << "Error adding money: " << errorMessage << endl;
        sqlite3_free(errorMessage);
        assert(false);
    }
}

void Database::removeMoney(const string& cardNumber, double damount)
{
    long long amount = long long(damount * 100);
    if (amount <= 0)
    {
        cerr << "Error: Amount must be positive." << endl;
        assert(false);
    }

    // Check the current balance first
    double currentBalance;
    string sqlCheck = "SELECT CurrentBalance FROM Cards WHERE CardNumber = " + cardNumber + ";";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sqlCheck.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            currentBalance = sqlite3_column_double(stmt, 0);
        }
        else
        {
            cerr << "Error: Card not found." << endl;
            sqlite3_finalize(stmt);
            assert(false);
        }
    }
    else
    {
        cerr << "Error checking balance: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        assert(false);
    }
    sqlite3_finalize(stmt);

    if (currentBalance < amount)
    {
        cerr << "Error: Insufficient funds." << endl;
        assert(false);
    }

    // Update the balance by subtracting the amount
    string sql = "UPDATE Cards SET CurrentBalance = CurrentBalance - " + to_string(amount) + " WHERE CardNumber = " + cardNumber + ";";
    char* errorMessage = nullptr;
    int exit = sqlite3_exec(db, sql.c_str(), 0, 0, &errorMessage);
    if (exit != SQLITE_OK)
    {
        cerr << "Error withdrawing money: " << errorMessage << endl;
        sqlite3_free(errorMessage);
        assert(false);
    }
}

bool Database::isCardValid(const string& cardNumber)
{
    // Validate the card number format
    regex cardNumberPattern(R"(\d{16})"); // 16 digits
    if (!regex_match(cardNumber, cardNumberPattern))
    {
        cerr << "Error: Card number must be exactly 16 digits." << endl;
        return false;
    }

    // Check if the card number exists in the database
    string sql = "SELECT COUNT(*) FROM Cards WHERE CardNumber = '" + cardNumber + "';";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int count = sqlite3_column_int(stmt, 0);
            sqlite3_finalize(stmt);
            return count > 0; // Returns true if the card exists
        }
    }
    else
    {
        cerr << "Error checking card validity: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    return false; // Card does not exist or query failed
}

bool Database::isPinCorrect(const string& cardNumber, const string& pin)
{
    // Validate the PIN format
    regex pinPattern(R"(\d{4})"); // 4 digits
    if (!regex_match(pin, pinPattern))
    {
        cerr << "Error: PIN must be exactly 4 digits." << endl;
        return false;
    }

    // Check if the provided PIN matches the stored PIN for the given card number
    string sql = "SELECT COUNT(*) FROM Cards WHERE CardNumber = '" + cardNumber + "' AND PIN = '" + pin + "';";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int count = sqlite3_column_int(stmt, 0);
            sqlite3_finalize(stmt);
            return count > 0; // Returns true if the PIN is correct
        }
    }
    else
    {
        cerr << "Error checking PIN: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    return false; // PIN does not match or query failed
}

double Database::getCardBalance(const string& cardNumber)
{
    string sql = "SELECT CurrentBalance FROM Cards WHERE CardNumber = " + cardNumber + ";";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
        assert(false);
    }


    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        double balance = sqlite3_column_double(stmt, 0);
        return double(balance)/100.0;
    }
    else
    {
        assert(false);
    }

    sqlite3_finalize(stmt);
    assert(false); // No record found
}

nlohmann::json Database::getCardDetails(const string& cardNumber)
{
    string sql = "SELECT * FROM Cards WHERE CardNumber = " + cardNumber + ";";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
        assert(false);
    }

    nlohmann::json jsonData;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        string cardNumber = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string pin = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        long long balance = sqlite3_column_double(stmt, 2);

        jsonData["cardNumber"] = cardNumber;
        jsonData["pin"] = pin;
        jsonData["balance"] = double(balance)/100.0;
    }
    else
    {
        assert(false);
    }

    sqlite3_finalize(stmt);
    return jsonData; // No record found
}

void Database::close()
{
    sqlite3_close(db);
}
