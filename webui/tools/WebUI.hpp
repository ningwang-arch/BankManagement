#define CROW_MAIN

#include <iostream>
#include "crow_all.h"
#include <string>
#include <sstream>
#include <fstream>
#include <exception>
#include <iterator>
#include <stdlib.h>
#include <time.h>
#include <boost/format.hpp>
#include "Functions.h"
#include "tools.hpp"
#include "middlewares.hpp"
#include "helper.hpp"

#define CROW_STATIC_DIRECTORY "static/"

#define DEFAULT_PORT 11223

#define CROW_STATIC_ENDPOINT "/static/<path>/<path>"
namespace mw = app::middlewares;

class WebUI
{
private:
    crow::App<crow::CookieParser, mw::Session> app;
    Functions fun;
    int port;

public:
    WebUI() { this->port = DEFAULT_PORT; }
    WebUI(int port);
    void init();
    void start();
    ~WebUI();
};

WebUI::WebUI(int port)
{
    this->port = port;
}

void WebUI::init()
{
    CROW_ROUTE(app, "/")
    ([]
     { return crow::mustache::load("login.html").render(); });

    CROW_ROUTE(app, "/login").methods("POST"_method, "GET"_method)([&](const crow::request &req, crow::response &res)
                                                                   {
                                                                       crow::mustache::context ctx;
                                                                       string message = R"(<script>alert("%s")</script>)";
                                                                       boost::format formater(message);
                                                                       if (req.method == "GET"_method)
                                                                       {
                                                                           ctx["message"] = (formater % "请先登录").str();
                                                                           res.write(crow::mustache::load("login.html").render(ctx));
                                                                           res.end();
                                                                       }
                                                                       map<std::string, std::string> post_body = parse_crow_post_data(req.body);

                                                                       if (post_body.count("user") == 0 || post_body.count("passwd") == 0 || post_body.count("role") == 0)
                                                                       {
                                                                           ctx["message"] = (formater % "用户信息不完整").str();
                                                                           res.write(crow::mustache::load("login.html").render(ctx));
                                                                           res.end();
                                                                       }
                                                                       std::string username = post_body["user"];
                                                                       std::string pwd = post_body["passwd"];
                                                                       int role;
                                                                       if (check_is_root(username))
                                                                       {
                                                                           role = 2;
                                                                       }
                                                                       else
                                                                           role = std::stoi(post_body["role"]);
                                                                       bool result = fun.login(username, pwd, role);
                                                                       if (result)
                                                                       {
                                                                           MYSQL_RES *result = fun.get_res();
                                                                           MYSQL_ROW row = mysql_fetch_row(result);
                                                                           auto session = CTX(app, mw::Session, req).session;
                                                                           session->set("role", role);
                                                                           session->set("id", row[0]);
                                                                           res.redirect("/dashboard");
                                                                           res.end();
                                                                       }
                                                                       else
                                                                       {
                                                                           ctx["message"] = (formater % "用户信息有误,请检查后重新登录").str();
                                                                           res.write(crow::mustache::load("login.html").render(ctx));
                                                                           res.end();
                                                                       } });

    CROW_ROUTE(app, "/register").methods("POST"_method, "GET"_method)([&](const crow::request &req, crow::response &res)
                                                                      {
                                                                          if (req.method == "GET"_method)
                                                                          {
                                                                              res.redirect("/");
                                                                              res.end();
                                                                          }
                                                                          map<std::string, std::string> post_body = parse_crow_post_data(req.body);
                                                                          if (post_body.count("user") == 0 || post_body.count("passwd") == 0)
                                                                          {
                                                                              res.code = 400;
                                                                              res.end();
                                                                          }
                                                                          std::string username = post_body["user"];
                                                                          std::string pwd = post_body["passwd"];
                                                                          int role = 0;
                                                                          std::string re_pwd = post_body["re_passwd"];
                                                                          if (pwd != re_pwd)
                                                                          {
                                                                              res.write("两次输入密码不一致");
                                                                              res.end();
                                                                          }
                                                                          string id = fun.register_(username, pwd, role);
                                                                          if (id != "")
                                                                          {
                                                                              auto session = CTX(app, mw::Session, req).session;
                                                                              session->set("id", id);
                                                                              session->set("role", role);
                                                                              res.redirect("/dashboard");
                                                                              res.end();
                                                                          }
                                                                          else
                                                                          {
                                                                              res.code = 400;
                                                                              res.end();
                                                                          } });

    CROW_ROUTE(app, "/dashboard").methods("GET"_method, "POST"_method)([&](const crow::request &req)
                                                                       {
                                                                           auto session = CTX(app, mw::Session, req).session;
                                                                           crow::mustache::context ctx;
                                                                           if (session->has("id"))
                                                                           {
                                                                               std::string id = ClearHeadTail(session->get("id").dump());
                                                                               MYSQL_RES *result = fun.query_by_id(id);
                                                                               MYSQL_ROW row = mysql_fetch_row(result);
                                                                               ctx["username"] = row[1];
                                                                               return crow::mustache::load("dashboard.html").render(ctx);
                                                                           }
                                                                           else
                                                                           {
                                                                               return crow::mustache::load("login.html").render();
                                                                           } });
    CROW_ROUTE(app, "/index.html").methods(crow::HTTPMethod::Get, crow::HTTPMethod::POST)([&](const crow::request &req)
                                                                                          {
                                                                                              crow::mustache::context ctx;

                                                                                              std::string fmt = R"(<div class="user-infos"><div class="account-type">%s</div><div class="username"><div class="fullname">%s</div><div class="pseudo">id: %s</div></div><div class="user-stats"><div class="stat projects"><div class="stat-title">Register Date</div><div class="stat-val">%s</div></div><div class="stat projects"></div><div class="stat followers"><div class="stat-title">%s</div><div class="stat-val">%d</div></div></div></div>)";
                                                                                              char info[1024];
                                                                                              std::string role;
                                                                                              auto session = CTX(app, mw::Session, req).session;
                                                                                              if (!session->has("id"))
                                                                                              {
                                                                                                  return crow::mustache::load("login.html").render();
                                                                                              }

                                                                                              std::string id = ClearHeadTail(session->get("id").dump());
                                                                                              int role_num = std::stoi(session->get("role").dump());

                                                                                              string sql = build_query_sql(role_num, id);
                                                                                              MYSQL_RES *result = fun.query_by_id(id);
                                                                                              MYSQL_ROW row = mysql_fetch_row(result);
                                                                                              if (role_num == 0)
                                                                                              {
                                                                                                  role = "user";
                                                                                              }
                                                                                              else if (role_num == 1)
                                                                                              {
                                                                                                  role = "manager";
                                                                                              }
                                                                                              else
                                                                                              {
                                                                                                  role = "root";
                                                                                              }
                                                                                              string labels = role_num > 0 ? "Users" : "Cards";
                                                                                              int rows = fun.get_count_by_query(sql);
                                                                                              sprintf(info, fmt.c_str(), role.c_str(), row[1], row[0], row[4], labels.c_str(), rows);
                                                                                              ctx["info"] = info;

                                                                                              return crow::mustache::load("index.html").render(ctx); });

    CROW_ROUTE(app, "/info.html")
    ([&](const crow::request &req)
     {
         auto session = CTX(app, mw::Session, req).session;
         if (!session->has("id"))
         {
             return crow::mustache::load("login.html").render();
         }
         crow::mustache::context ctx = build_ctx(session);
         return crow::mustache::load("info.html").render(ctx); });

    CROW_ROUTE(app, "/updateInfo.html").methods(crow::HTTPMethod::Get, crow::HTTPMethod::POST)([&](const crow::request &req)
                                                                                               {
                                                                                                   crow::mustache::context ctx;
                                                                                                   if (req.method == crow::HTTPMethod::Get)
                                                                                                       return crow::mustache::load("updateInfo.html").render();
                                                                                                   auto session = CTX(app, mw::Session, req).session;
                                                                                                   if (!session->has("id"))
                                                                                                   {
                                                                                                       return crow::mustache::load("login.html").render();
                                                                                                   }
                                                                                                   map<std::string, std::string> post_body = parse_crow_post_data(req.body);
                                                                                                   string new_name = "";
                                                                                                   if (post_body.count("name_new") == 1)
                                                                                                   {
                                                                                                       new_name = post_body["name_new"];
                                                                                                   }
                                                                                                   string script = R"(<script>alert("%s")</script>)";

                                                                                                   boost::format formater(script);
                                                                                                   string old_pwd = post_body["pwd_old"];
                                                                                                   string new_pwd = post_body["password"];
                                                                                                   string new_pwd_re = post_body["pwd"];
                                                                                                   string id = ClearHeadTail(session->get("id").dump());
                                                                                                   if (!fun.query_if_exist(id, old_pwd))
                                                                                                   {
                                                                                                       formater % "原密码错误";
                                                                                                       ctx["script"] = formater.str();
                                                                                                       return crow::mustache::load("updateInfo.html").render(ctx);
                                                                                                   }
                                                                                                   if (!fun.update(fun.build_update_sql(id, std::stoi(session->get("role").dump()), new_pwd, new_name)))
                                                                                                   {
                                                                                                       formater % "更新失败";
                                                                                                       ctx["script"] = formater.str();
                                                                                                       return crow::mustache::load("updateInfo.html").render(ctx);
                                                                                                   }
                                                                                                   formater % "更新成功";
                                                                                                   ctx["script"] = formater.str();
                                                                                                   return crow::mustache::load("updateInfo.html").render(ctx); });

    CROW_ROUTE(app, "/new_manager").methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST)([&](const crow::request &req)
                                                                                           {
                                                                                               map<std::string, std::string> post_body = parse_crow_post_data(req.body);
                                                                                               string user_name = post_body["username"];
                                                                                               string pwd = post_body["pwd"];
                                                                                               int role = 1;
                                                                                               string id = fun.register_(user_name, pwd, role);
                                                                                               string script = R"(<script>alert("%s")</script>)";
                                                                                               crow::mustache::context ctx;
                                                                                               auto session = CTX(app, mw::Session, req).session;
                                                                                               ctx = build_ctx(session);
                                                                                               boost::format formater(script);
                                                                                               if (id != "")
                                                                                               {
                                                                                                   // do something
                                                                                                   ctx["message"] = (formater % "添加成功").str();
                                                                                               }
                                                                                               else
                                                                                               {
                                                                                                   ctx["message"] = (formater % "添加失败").str();
                                                                                               }
                                                                                               return crow::mustache::load("info.html").render(ctx); });
    CROW_ROUTE(app, "/logout")
        .methods(crow::HTTPMethod::Get, crow::HTTPMethod::POST)([&](const crow::request &req, crow::response &res)
                                                                {
                                                                    auto session = CTX(app, mw::Session, req).session;
                                                                    session->remove("id");
                                                                    res.redirect("/");
                                                                    res.end(); });

    CROW_ROUTE(app, "/edit").methods(crow::HTTPMethod::Get, crow::HTTPMethod::POST)([&](const crow::request &req)
                                                                                    {
                                                                                        map<std::string, std::string> post_body = parse_crow_post_data(req.body);
                                                                                        string user_name = post_body["username"];
                                                                                        string pwd = post_body["pwd"];
                                                                                        string pwd_encode=get_encode_pwd_from_decoded(pwd);
                                                                                        string sql = "update users set passwd='" + pwd_encode + "' where id='" + user_name + "'";
                                                                                        bool ret = fun.update(sql);
                                                                                        string script = R"(<script>alert("%s")</script>)";
                                                                                        crow::mustache::context ctx;
                                                                                        auto session = CTX(app, mw::Session, req).session;
                                                                                        ctx = build_ctx(session);
                                                                                        boost::format formater(script);
                                                                                        if (ret)
                                                                                        {
                                                                                            // do something
                                                                                            ctx["message"] = (formater % "修改成功").str();
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                            ctx["message"] = (formater % "修改失败").str();
                                                                                        }
                                                                                        return crow::mustache::load("info.html").render(ctx); });
    CROW_ROUTE(app, "/delete").methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST)([&](const crow::request &req)
                                                                                      {
        auto info = crow::json::load(req.body);
        bool ret = fun.delete_(info["id"].s());
        crow::json::wvalue res;
        if (ret)
        {
            res["success"] = "True";
            return res;
        } });

    CROW_ROUTE(app, "/card_about").methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST)([&](const crow::request &req)
                                                                                          {
                                                                                              auto info = crow::json::load(req.body);
                                                                                              auto session = CTX(app, mw::Session, req).session;

                                                                                              string operater = get_operater(session);

                                                                                              string id = info["id"].s(), status = info["opt"].s();
                                                                                              crow::json::wvalue response;
                                                                                              string sql;
                                                                                              if (status != "new")
                                                                                              {
                                                                                                  sql = "update cards set status='" + status + "' where card_num='" + id + "';";
                                                                                                  bool ret = fun.update(sql);
                                                                                                  if (ret)
                                                                                                      response["ok"] = true;
                                                                                              }
                                                                                              else
                                                                                              {
                                                                                                  string card_num = card_generate();
                                                                                                  sql = "insert into cards value('" + card_num + "','" + id + "',0,'Normal');";
                                                                                                  id = card_num;
                                                                                              }
                                                                                              bool ret = fun.update(sql);
                                                                                              fun.add_card_operation(operater, id, status);
                                                                                              if (ret)
                                                                                                  response["ok"] = true;
                                                                                              return response; });

    CROW_ROUTE(app, "/money_about").methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST)([&](const crow::request &req)
                                                                                           {
                                                                                               auto session = CTX(app, mw::Session, req).session;
                                                                                               crow::json::wvalue response;
                                                                                               auto info = crow::json::load(req.body);
                                                                                               string operater = get_operater(session);
                                                                                               string card_num = info["id"].s();
                                                                                               string msg = get_response_msg(info);
                                                                                               string opeartion = get_opeartion(info);
                                                                                               fun.add_card_operation(operater, card_num, opeartion);
                                                                                               response["msg"] = msg;
                                                                                               return response; });
    CROW_ROUTE(app, "/userInfo/<string>")
    ([&](const crow::request &req, string id)
     {
         crow::mustache::context ctx;
         string sql = "select card_num,status from cards where user='" + id + "'";
         map<string, string> list = fun.get_card_list(sql);
         string info = info_table_build(list);
         ctx["info"] = info;
         return crow::mustache::load("card.html").render(ctx); });
    CROW_ROUTE(app, "/card_log/<string>")
    ([](const crow::request &req, string card_num)
     {
         crow::mustache::context ctx;
         string sql = "select operator, operation,opt_time from cardopt where card_num='" + card_num + "' order by opt_time desc;";
         string info = buid_log_table(sql);
         ctx["info"] = info;
         return crow::mustache::load("log.html").render(ctx); });
}

void WebUI::start()
{
    cout << "Serving HTTP on 0.0.0.0 port " + to_string(this->port) + " (http://0.0.0.0:" + to_string(this->port) + "/)" << endl;
    app.loglevel(crow::LogLevel::Warning);
    app.port(this->port).multithreaded().run();
}

WebUI::~WebUI()
{
}
