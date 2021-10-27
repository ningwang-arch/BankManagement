#include "Functions.h"

Functions::Functions()
{
    if (!mdb.initDB())
    {
        cout << "connect mysql error" << endl;
        exit(-1);
    }
}

Functions::~Functions()
{
}

MYSQL_RES *Functions::get_res()
{
    return this->res;
}

MYSQL_RES *Functions::get_log(const string sql)
{
    mdb.execSQL(sql, &this->res);
    return res;
}

bool Functions::login(const string username, const string encode_pwd, const int role)
{
    string passwd_encode = get_encode_pwd_from_decoded(encode_pwd);
    string sql_id = "select * from users where id='" + username + "' and passwd='" + passwd_encode + "' and role=" + to_string(role) + ";";
    this->mdb.execSQL(sql_id, &this->res);
    int num_rows = mysql_num_rows(this->res);
    if (num_rows >= 1)
        return true;
    return false;
}

string Functions::register_(const string username, const string encode_pwd, const int role)
{
    MYSQL_RES *res;
    string passwd_encode = get_encode_pwd_from_decoded(encode_pwd);
    string id;
    int count = 0;
    while (true)
    {
        for (int i = 0; i < 16; i++)
        {
            id += (rand() % 10 + '0');
        }
        count++;
        string query_sql = "select * from users where id='" + id + "'";
        mdb.execSQL(query_sql, &res);
        int num_rows = mysql_num_rows(res);
        if (num_rows == 0 || count >= 20)
        {
            break;
        }
    }
    time_t t = time(0);
    tm *ltm = localtime(&t);
    char time[1024];
    sprintf(time, "%d-%d-%d %d:%d:%d", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday, ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    string insert_sql = "insert into users values('" + id + "','" + username + "','" + passwd_encode + "','" + to_string(role) + "','" + time + "')";
    if (mdb.execSQL(insert_sql, &res))
        return id;
    else
    {
        delete_(id);
        return "";
    }
}

MYSQL_RES *Functions::query_by_id(const string id)
{
    string query_sql = "select * from users where id='" + id + "'";
    mdb.execSQL(query_sql, &res);
    return this->res;
}

bool Functions::delete_(const string id)
{
    MYSQL_RES *res;
    string sql = "delete from users where id='" + id + "'";

    if (mdb.execSQL(sql, &res) && (mysql_num_rows(res) > 0))
    {
        return true;
    }
    return false;
}

bool Functions::update(string sql)
{
    MYSQL_RES *res;
    if (mdb.execSQL(sql, &res) && (mysql_num_rows(res) > 0))
    {
        return true;
    }

    return false;
}

int Functions::get_count_by_query(const string sql)
{
    mdb.execSQL(sql, &this->res);
    int num_rows = mysql_num_rows(this->res);
    return num_rows;
}

bool Functions::add_card_operation(const string optor, const string card_num, const string operation)
{
    MYSQL_RES *res;
    time_t t = time(0);
    tm *ltm = localtime(&t);
    char time[1024];
    sprintf(time, "%d-%d-%d %d:%d:%d", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday, ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    string sql = "insert into cardopt values (NULL,'" + card_num + "','" + optor + "','" + operation + "','" + time + "');";
    return mdb.execSQL(sql, &res);
}

map<string, string> Functions::get_card_list(const string sql)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    mdb.execSQL(sql, &res);
    map<string, string> list;
    int num_rows = mysql_num_rows(res);
    for (int i = 0; i < num_rows; i++)
    {
        row = mysql_fetch_row(res);
        list.insert(pair<string, string>(row[0], row[1]));
    }
    return list;
}

bool Functions::query_if_exist(const string id, const string pwd_encode)
{
    MYSQL_RES *res;
    string passwd_encode = get_encode_pwd_from_decoded(pwd_encode);
    string sql_id = "select * from users where id='" + id + "' and passwd='" + passwd_encode + "';";
    mdb.execSQL(sql_id, &res);
    if (mysql_num_rows(this->res) > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

const string Functions::build_update_sql(const string id, const int role, string pwd, const string username)
{
    // UPDATE table_name SET field1=new-value1, field2=new-value2 [WHERE Clause]
    string sql;
    string preffix = "update users set ";
    string clause = "where id='" + id + "' and role=" + to_string(role) + ";";
    decode_passwd(pwd);
    string pwd_encode = get_encode_pwd_from_decoded(pwd);
    string middle = "passwd='" + pwd_encode + "' ";
    if (username != "")
    {
        middle += ",username='" + username + "' ";
    }
    sql = preffix + middle + clause;
    return sql;
}