#include "user.h"

User::User(std::string name)
: m_userName(name), m_phoneNumber(""), m_balance(0.0)
{

}

User::User(const std::string& name, const std::string& phoneNumber, double balance)
: m_userName(name), m_phoneNumber(phoneNumber), m_balance(balance)
{

}

void User::rename(const std::string& newName)
{
    m_userName = newName;
}