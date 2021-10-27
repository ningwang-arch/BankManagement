#ifndef _CONFIG_H
#define _CONFIG_H

#include <string>
#include <iostream>
#include <fstream>
#include <json/json.h>
using namespace std;

class Config
{
private:
    string host;
    string username;
    string passwd;
    string db;

public:
    Config();
    void load(string config_file);
    string get_host();
    string get_username();
    string get_passwd();
    string get_db();
    ~Config();
};

string SHA224(const string message);

std::string md5(const std::string str);

string encode_passwd(const string passwd);

string decode_passwd(const string encode_passwd);

string get_encode_pwd_from_decoded(const string encode_passwd);

bool check_is_root(const string id);

#endif // !_CONFIG_H