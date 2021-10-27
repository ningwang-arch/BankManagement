#include "MyDatabase.h"

MyDatabase::MyDatabase()
{
    config.load(CONFIG_FILE);
    mysql = mysql_init(NULL);
    if (mysql == NULL)
    {
        cout << "MySQL init error " << mysql_error(mysql) << endl;
        exit(-1);
    }
}

bool MyDatabase::initDB()
{
    mysql = mysql_real_connect(mysql, config.get_host().c_str(), config.get_username().c_str(), config.get_passwd().c_str(), config.get_db().c_str(), 0, NULL, 0);
    if (mysql == NULL)
    {
        cout << "MySQL connect error " << mysql_error(mysql) << endl;
        exit(-1);
    }
    return true;
}

bool MyDatabase::execSQL(string sql, MYSQL_RES **res)
{
    // mysql_real_query: Returns zero on success, otherwise non zero.
    if (mysql_real_query(mysql, sql.c_str(), (unsigned long)strlen(sql.c_str())))
    {
        cout << "Execute sql " << sql << " error" << endl;
        return false;
    }
    else
    {
        result = mysql_store_result(mysql);
        if (result)
        {

            *res = result;
        }
        else
        {
            if (mysql_field_count(mysql) == 0) //代表执行的是update,insert,delete类的非查询语句
            {
                // (it was not a SELECT)
                int num_rows = mysql_affected_rows(mysql); //返回update,insert,delete影响的行数
            }
            else // error
            {
                cout << "Get result error: " << mysql_error(mysql);
                return false;
            }
        }
    }
    return true;
}

MyDatabase::~MyDatabase()
{
    if (mysql != NULL)
    {
        mysql_close(mysql);
    }
}