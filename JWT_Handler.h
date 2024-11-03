//#pragma once
//#include <iostream>
//#include <vector>
//#include <random>
//#include <map>
//#include <string>
//#include <chrono>
//#include <jwt-cpp/jwt.h>
//using namespace std;
//
//
//class JWT_Handler {
//private:
//	static const int key_length = 100;
//	static const vector<unsigned char>* secret_key;
//	static const map<string, string>* user_jwt_map;
//	static const long jwt_life_time = 60000;
//
//	static vector<unsigned char>* generateSecretKey();
//
//public:
//	JWT_Handler() = default;
//	~JWT_Handler();
//
//    static std::string generateToken(const std::string& username) {
//        // Get the current time
//        auto current_time = chrono::system_clock::now();
//        auto issued_at = chrono::system_clock::to_time_t(current_time);
//        auto expires_at = chrono::system_clock::to_time_t(current_time + chrono::seconds(jwt_life_time));
//
//        // Create the JWT token
//        std::string token = jwt::create()
//            .set_issued_at(issuedAt)
//            .set_expires_at(expiresAt)
//            .set_payload_claim("username", jwt::claim(username))
//            .sign(jwt::algorithm::hs256{ secret_key });
//
//        return token;
//    }
//};