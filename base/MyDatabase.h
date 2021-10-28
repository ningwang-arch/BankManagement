#ifndef _MYDATABASE_H_

#define _MYDATABASE_H_

#include "Config.h"
#include <string>
#include <mysql/mysql.h>
using namespace std;
class MyDatabase
{
private:
    Config config;
    MYSQL *mysql;      //连接mysql句柄指针
    MYSQL_RES *result; //指向查询结果的指针
    MYSQL_ROW row;     //按行返回的查询信息

public:
    MyDatabase();
    bool initDB();
    bool execSQL(string sql, MYSQL_RES **res);
    ~MyDatabase();
};

#endif // !_MYDATABASE_H_