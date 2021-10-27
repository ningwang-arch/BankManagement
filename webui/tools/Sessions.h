#pragma once

#include "crow_all.h"
#include <string>
#include <utility>
#include <unordered_map>

namespace app
{
    namespace tools
    {
        class Session
        {
        public:
            using Key = std::string;
            using Value = crow::json::wvalue;
            using State = crow::json::wvalue;

        protected:
            State state = {};

        public:
            Session() = default;
            Session(const Session &) = delete; //because crow::json::wvalue is not copyable
            Session(Session &&) = default;

            bool has(const Key &key) { return this->state.count(key) > 0; }
            Value &get(const Key &key) { return this->state[key]; }

            template <class T>
            Session &set(const Key &key, const T &value)
            {
                this->state[key] = /*std::forward<T&&>(*/ value /*)*/;
                return *this;
            }

            Session &remove(const Key &key)
            {
                this->state[key] = nullptr;
                return *this;
            }

            const State &json() const
            {
                /*State ret;
                    ret["session"] = crow::json::dump(this->state);
                    return std::move(ret);*/
                return this->state;
            }
        };

        class Sessions
        {
        protected:
            Sessions() {}

        public:
            static Sessions &repo()
            {
                static Sessions instance;
                return instance;
            }

            using Key = std::string;
            using Value = std::shared_ptr<Session>;
            using map = std::unordered_map<Key, Value>;

            Sessions(const Sessions &) = default; //Value non copyable
            Sessions(Sessions &&) = default;

        protected:
            map sessions = map{};

        public:
            bool has(const Key &key) const
            {
                auto it = this->sessions.find(key);
                return it != this->sessions.end();
            }

            const Value &get(const Key &key) const
            {
                return this->sessions.at(key);
            }

            Value &get(const Key &key)
            {
                return this->sessions.at(key);
            }

            Sessions &set(const Key &key, const Value &value)
            {
                this->sessions.emplace(key, value);
                return *this;
            }

            Sessions &newSessionFor(const Key &key)
            {
                return this->set(key, std::make_shared<Session>(Session{}));
            }

            Sessions &remove(const Key &key)
            {
                this->sessions.erase(key);
                return *this;
            }

            const Value &operator[](const Key &key) const
            {
                return this->get(key);
            }

            Value &operator[](const Key &key)
            {
                return this->get(key);
            }
        };
    }
}