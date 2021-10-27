#include "Config.h"
#include "md5.h"
#include <openssl/evp.h>

#define ROOT_ID "9250707949178507"

using namespace std;
Config::Config()
{
}

void Config::load(string config_file)
{
    Json::CharReaderBuilder builder;
    Json::Value root;
    builder["collectComments"] = true;
    JSONCPP_STRING errs;
    ifstream ifs(config_file, ios::in);
    if (!ifs.is_open())
    {
        cout << "Open config file failed" << endl;
        return;
    }
    if (parseFromStream(builder, ifs, &root, &errs))
    {
        this->host = root["host"].asString();
        this->username = root["username"].asString();
        this->passwd = root["passwd"].asString();
        this->db = root["db"].asString();
    }
    else
    {
        ifs.close();
        cout << "parse error" << endl;
        return;
    }
    ifs.close();
}

string Config::get_host()
{
    return this->host;
}

string Config::get_username()
{
    return this->username;
}

string Config::get_passwd()
{
    return this->passwd;
}

string Config::get_db()
{
    return this->db;
}

Config::~Config()
{
}

string SHA224(const string message)
{
    uint8_t digest[EVP_MAX_MD_SIZE];
    char mdString[(EVP_MAX_MD_SIZE << 1) + 1];
    unsigned int digest_len;
    EVP_MD_CTX *ctx;
    if ((ctx = EVP_MD_CTX_new()) == nullptr)
    {
        cout << "Could not create hash context" << endl;
        exit(-1);
    }
    if (!EVP_DigestInit_ex(ctx, EVP_sha224(), nullptr))
    {
        EVP_MD_CTX_free(ctx);
        cout << "Could not initialize hash context" << endl;
        exit(-1);
    }
    if (!EVP_DigestUpdate(ctx, message.c_str(), message.length()))
    {
        EVP_MD_CTX_free(ctx);
        cout << "Could not update hash" << endl;
        exit(-1);
    }
    if (!EVP_DigestFinal_ex(ctx, digest, &digest_len))
    {
        EVP_MD_CTX_free(ctx);
        cout << "Could not output hash" << endl;
        exit(-1);
    }
    for (unsigned int i = 0; i < digest_len; ++i)
    {
        sprintf(mdString + (i << 1), "%02x", (unsigned int)digest[i]);
    }
    mdString[digest_len << 1] = '\0';
    EVP_MD_CTX_free(ctx);
    return string(mdString);
}

std::string md5(const std::string str)
{
    MD5 md5 = MD5(str);
    return md5.hexdigest();
}

string encode_passwd(const string passwd)
{
    return SHA224(passwd);
}

string decode_passwd(const string passwd)
{

    string passwd_decode = passwd;

    // do something to decode

    return passwd_decode;
}

string get_encode_pwd_from_decoded(const string passwd_encode)
{
    string pwd = passwd_encode;
    return encode_passwd(decode_passwd(pwd));
}

bool check_is_root(const string id)
{
    // change check method here
    if (id == ROOT_ID)
    {
        return true;
    }
    return false;
}