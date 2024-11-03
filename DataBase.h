#include <sqlite3.h>
#include <string>
using namespace std;

class Database {
public:
    Database(const string& dbName);
    ~Database();
    bool createCardsTable();
    bool insertCard(const string& cardNumber, const string& pin, double balance);
    bool getCardDetails(int cardId, string& cardNumber, string& pin, double& balance);
    bool addMoney(int cardId, double amount);
    bool withdrawMoney(int cardId, double amount);
    bool isCardValid(const string& cardNumber); 
    bool isPinCorrect(const string& cardNumber, const string& pin);
    void close();

private:
    sqlite3* db;
};

