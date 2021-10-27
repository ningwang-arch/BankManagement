#ifndef _FUNCTIONS_H_

#define _FUNCTIONS_H_

#include <string>
#include <time.h>

#include "MyDatabase.h"
using namespace std;

class Functions
{
private:
    MyDatabase mdb;
    MYSQL_RES *res;

public:
    Functions(/* args */);
    bool login(const string username, const string encode_passwd, const int role);
    string register_(const string username, const string encode_passwd, const int role);
    map<string, string> get_card_list(const string sql);
    MYSQL_RES *query_by_id(const string id);
    bool delete_(const string id);
    MYSQL_RES *get_log(const string sql);
    bool add_card_operation(const string optor, const string card_num, const string operation);
    bool update(const string sql);
    int get_count_by_query(const string sql);
    bool query_if_exist(const string id, const string pwd_encode);
    const string build_update_sql(const string id, const int role, const string pwd, const string username);
    MYSQL_RES *get_res();
    ~Functions();
};

#endif // !_FUNCTIONS_H_