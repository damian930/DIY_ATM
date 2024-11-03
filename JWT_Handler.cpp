//#include "JWT_Handler.h"
//using namespace std;
//
//// class static constants
//const vector<unsigned char>* JWT_Handler::secret_key = JWT_Handler::generateSecretKey();
//const map<string, string>* JWT_Handler::user_jwt_map = new map<string, string>;
//
//// class methods
//
//JWT_Handler::~JWT_Handler() {
//	delete this->secret_key;
//	delete this->user_jwt_map;
//}
//
//
//vector<unsigned char>* JWT_Handler::generateSecretKey() {
//	vector<unsigned char>* arr = new vector<unsigned char>();
//	random_device rd;
//	generate(arr->begin(), arr->end(), ref(rd));
//	return arr;
//}
//
//
//
