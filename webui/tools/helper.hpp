#pragma once
#include "tools.hpp"
#include <boost/format.hpp>
#include "crow_all.h"
#include "Functions.h"
using namespace std;

#define CTX(app, mw, rq) app.get_context<mw>(rq)

#define THEAD R"(<thead><tr><th class="table-title" id="bwhuq">ID</th><th class="table-type" id="eryio">用户名</th>%s<th class="table-set">操作</th></tr></thead>)"
#define THEAD_U R"(<thead><tr><th class="table-title" id="bwhuq">ID</th>%s<th class="table-set">操作</th></tr></thead>)"
#define BASE R"(<tr><td>%s</td><td>%s</td>)"
#define BASE_U R"(<tr><td onclick="card_log(this,'',2);">%s</td>)"
#define BUTTON R"(<td><div class="am-btn-toolbar"><div class="am-btn-group am-btn-group-xs"><button class="am-btn am-btn-default am-btn-xs am-text-secondary" onclick="edit(this);"><span></span> 编辑</button><button class="am-btn am-btn-default am-btn-xs am-text-danger am-hide-sm-only" onclick="delete_(this);"><span ></span> 删除</button></div></div></td></tr>)"
#define BUTTON_M R"(<td><div class="am-btn-toolbar"><div class="am-btn-group am-btn-group-xs"><button class="am-btn am-btn-default am-btn-xs am-text-secondary" onclick="edit(this);"><span></span> 编辑</button></div></div></td></tr>)"
#define BUTTON_U R"(<td><div class="dropdown"><button class="dropbtn">操作</button><div class="dropdown-content" id="group"><a class="btn" data-method="add" href="#">存款</a><a class="btn" data-method="sub" href="#">取款</a></div></div></td></tr>)"
#define ITEM_R BASE BUTTON
#define ITEM_M BASE R"(<td class="am-hide-sm-only" onclick="user_info(this,'',2);">%s</td>)" BUTTON_M
#define ITEM_U BASE_U R"(<td class="am-hide-sm-only">%s</td><td>%s</td>)" BUTTON_U
#define TBODY "<tbody>%s</tbody>"

#define LOG R"(<tr><td>%s</td><td>%s</td><td>%s</td></tr>)"

#define INFO R"(<tr><td>%s</td><td>%s</td><td><div class="dropdown"><button class="dropbtn">操作</button><div class="dropdown-content" id="group"><a class="btn" data-method="%s" href="#">%s</a><a class="btn" data-method="lost" href="#">挂失</a></div></div></td></tr>)"

/*

            <tr><td>%s</td><td>%s</td><td><div class="dropdown"><button class="dropbtn">操作</button><div class="dropdown-content" id="group"><a class="btn" data-method="%s" href="#">%s</a><a class="btn" data-method="sub" href="#">挂失</a></div></div></td></tr>

*/

string ClearHeadTail(string str)
{
    if (str.empty())
    {
        return str;
    }

    str.erase(0, str.find_first_not_of("\""));
    str.erase(str.find_last_not_of("\"") + 1);
    return str;
}

string build_query_sql(const int role, const string id)
{
    string sql;
    if (role == 0)
    {
        sql = "select card_num from cards where user='" + id + "'";
    }
    else if (role == 1 || role == 2)
    {
        sql = "select id from users where role<'" + to_string(role) + "'";
    }
    else
    {
        sql = "";
    }
    return sql;
}

bool url_decode(const std::string &in, std::string &out)
{
    out.clear();
    out.reserve(in.size());
    for (std::size_t i = 0; i < in.size(); ++i)
    {
        if (in[i] == '%')
        {
            if (i + 3 <= in.size())
            {
                int value = 0;
                std::istringstream is(in.substr(i + 1, 2));
                if (is >> std::hex >> value)
                {
                    out += static_cast<char>(value);
                    i += 2;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        else if (in[i] == '+')
        {
            out += ' ';
        }
        else
        {
            out += in[i];
        }
    }
    return true;
}

map<std::string, std::string> parse_crow_post_data(const string &req_body)
{
    map<std::string, std::string> body;

    vector<string> vec;
    string tmp;
    bool result = url_decode(req_body, tmp);
    if (result)
    {
        boost::algorithm::split(vec, tmp, [](char x)
                                { return x == '&'; });
        for (auto &it : vec)
        {
            auto pos = it.find("=");
            if (pos != string::npos)
            {
                body[it.substr(0, pos)] = it.substr(pos + 1);
            }
            else
            {
                break;
            }
        }
    }
    return body;
}

// user---<id,id_str>   manager------<role,role_num>
const string table_build(map<std::string, std::string> info)
{
    boost::format format_header;
    boost::format format_item;
    boost::format format_body(TBODY);
    Functions fun;
    string body, tbody;
    if (info.count("id") == 0)
    {
        format_header.parse(THEAD);
        int role = stoi(info["role"]);
        if (role == 1)
        {
            body += (format_header % R"(<th class="table-author am-hide-sm-only">卡数</th>)").str();
            format_item.parse(ITEM_M);
        }
        else
        {
            body += (format_header % "").str();
            format_item.parse(ITEM_R);
        }
        string sql = "select id,username from users where role=" + to_string(role - 1);
        int count = fun.get_count_by_query(sql);
        MYSQL_RES *res = fun.get_res();
        for (int i = 0; i < count; i++)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            string user_id = row[0], username = row[1];
            sql = "select card_num from cards where user='" + user_id + "';";
            int cnt = fun.get_count_by_query(sql);
            string str;
            if (role == 1)
            {
                str = (format_item % user_id % username % to_string(cnt)).str();
            }
            else
            {
                str = (format_item % user_id % username).str();
            }
            tbody += str;
        }
    }
    else
    {
        format_header.parse(THEAD_U);
        string str = R"(<th class="table-author am-hide-sm-only">余额</th><th class="table-type">状态</th>)";
        body += (format_header % str).str();
        string sql = "select card_num,balance,status from cards where user='" + info["id"] + "'";
        int count = fun.get_count_by_query(sql);
        MYSQL_RES *res = fun.get_res();
        format_item.parse(ITEM_U);
        for (int i = 0; i < count; i++)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            string card_num = row[0], balance = row[1], status = row[2];
            tbody += (format_item % card_num % balance % status).str();
        }
    }
    body += tbody;
    return body;
}

crow::mustache::context build_ctx(app::tools::Sessions::Value session)
{
    map<string, string> info;
    crow::mustache::context ctx;
    std::string id = ClearHeadTail(session->get("id").dump());
    int role_num = std::stoi(session->get("role").dump());
    if (role_num == 0)
    {
        ctx["user_card"] = crow::mustache::load("money_templates.html").render();
        ctx["user"] = true;
        info.insert(std::pair<string, string>("id", id));
    }
    else
    {
        info.insert(std::pair<string, string>("role", to_string(role_num)));
        ctx["label"] = "true";
        if (role_num == 1 || role_num == 2)
        {
            ctx["edit"] = crow::mustache::load("edit_templates.html").render();
            if (role_num == 2)
            {
                ctx["nuser"] = "true";
                ctx["new"] = crow::mustache::load("new_templates.html").render();
            }
        }
    }
    const string table = table_build(info);
    ctx["table"] = table;
    return ctx;
}

const string card_generate()
{
    string head[] = {"436745", "622280", "458123", "521899", "622260", "402674", "622892", "622188", "602969", "622760", "623668"};
    int size = sizeof(head) / sizeof(string);
    //银行类型的6位标识数字
    srand((unsigned int)time(0));
    string card = head[rand() % size];
    //取12位随机数字
    for (int i = 0; i < 12; i++)
    {
        string buf = to_string(1 + rand() % 9);
        card += buf;
    }

    int s = 0;
    int num = 0;
    int len = card.length() + 1;

    //校验位前一位是偶数位因此i初始化为2
    for (int i = 2; i <= len; i++)
    {
        //依次从右往左取出数字
        char buff = card[len - i];
        num = atoi(&buff);

        //判断当前在奇数位还是偶数位
        if (i % 2 == 0)
        {
            if ((num * 2) >= 10)
            {
                //偶数位*2大于10, 大于10则为两位数

                string buf = to_string(num * 2);
                //个位数与十位数相加
                s += (int(buf[0]) - 48) + (int(buf[1]) - 48);
            }
            else
                //偶数位*2小于10则不用十位各位相加直接取出
                s += num * 2;
        }
        else
            //奇数位直接取出
            s += num;
    }

    //判断s+几才能被10整除
    for (int i = 0; i < 10; i++)
    {
        if ((s + i) % 10 == 0)
        {
            card += to_string(i);
            break;
        }
    }

    //输出有效卡号
    return card;
}

string info_table_build(map<string, string> list)
{
    boost::format formater(INFO);
    map<string, string>::iterator it;
    string tbody;
    for (it = list.begin(); it != list.end(); it++)
    {
        string id = it->first;
        string status = it->second;
        string method = "freeze", text = "冻结";
        if (status == "Frozen")
        {
            method = "unfreeze", text = "解冻";
        }
        tbody += (formater % id % status % method % text).str();
    }
    return tbody;
}

string get_opeartion(crow::json::rvalue info)
{
    string card_id = info["id"].s();
    string type = info["type"].s();
    int amount = info["amount"].i();
    string operation;
    if (type == "add")
    {
        operation = "+" + to_string(amount) + " 元";
    }
    else
    {
        operation = "-" + to_string(amount) + " 元";
    }
    return operation;
}

string get_response_msg(crow::json::rvalue info)
{
    Functions fun;
    string msg;
    string card_id = info["id"].s();
    string type = info["type"].s();
    int amount = info["amount"].i();
    if (amount < 0)
    {
        msg = "Illegal input";
    }
    else
    {
        string sql = "select balance,status from cards where card_num='" + card_id + "'";
        map<string, string> list = fun.get_card_list(sql);
        map<string, string>::iterator it;
        int balance;
        string status;
        for (it = list.begin(); it != list.end(); it++)
        {
            balance = stoi(it->first);
            string status = it->second;
        }
        if (status == "Frozen" || status == "Lost")
        {
            msg = "The card status is abnormal and the business is cancelled";
        }
        else
        {
            if (type == "sub")
            {
                if (amount > balance)
                {
                    msg = "Insufficient balance, withdrawal failed";
                }
                else
                {
                    string sql = "update cards set balance=balance-" + to_string(amount) + " where card_num='" + card_id + "';";
                    if (fun.update(sql))
                    {
                        msg = "Successful withdrawal";
                    }
                    else
                    {
                        msg = "Withdrawal failed";
                    }
                }
            }
            else if (type == "add")
            {
                string sql = "update cards set balance=balance+" + to_string(amount) + " where card_num='" + card_id + "';";
                if (fun.update(sql))
                {
                    msg = "Successful withdrawal";
                }
                else
                {
                    msg = "Withdrawal failed";
                }
            }
            else
            {
                msg = "Invalid operation";
            }
        }
    }
    return msg;
}

string get_operater(app::tools::Sessions::Value session)
{
    Functions fun;
    std::string id = ClearHeadTail(session->get("id").dump());
    MYSQL_RES *res = fun.query_by_id(id);
    MYSQL_ROW row = mysql_fetch_row(res);

    string user_name = row[1];
    return user_name;
}

string buid_log_table(const string sql)
{
    Functions fun;
    boost::format formater(LOG);
    string table;
    MYSQL_RES *res;
    res = fun.get_log(sql);
    int num_rows = mysql_num_rows(res);
    for (int i = 0; i < num_rows; i++)
    {
        MYSQL_ROW row = mysql_fetch_row(res);
        // row[0]--operator  row[1]--operation row[2]--opt_time         opeartor, operation,opt_time
        table += (formater % row[0] % row[1] % row[2]).str();
    }
    return table;
}