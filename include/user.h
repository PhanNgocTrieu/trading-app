#ifndef USER_H
#define USER_H

#include <iostream>

class User {
    private:
        int             m_id;
        std::string     m_userName;
        std::string     m_passWord;
        std::string     m_phoneNumber;
        double          m_balance;

    public:
        User(std::string name);
        User(const std::string& name, const std::string& phoneNumber = "", double balance = 0.0);
        void rename(const std::string&);
        
};


#endif