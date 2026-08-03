#include "user.h"

User::User(std::string name)
: m_userName(name), m_phoneNumber(""), m_balance(0.0)
{

}

User::User(std::string name, std::string phoneNumer = "", double balance = 0.0)
: m_userName(name), m_phoneNumber(phoneNumer), m_balance(balance)
{

}