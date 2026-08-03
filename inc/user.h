#ifndef USER_H
#define USER_H

#include <iostream>

class User {
    private:
        int             m_id;
        std::string     m_userName;
        std::string     m_passWord;
        std::string     m_phoneNumer;
        double          m_balance;

    public:
        User(std::string name);
        User(std::string name, std::string number = 0, double balance = 0.0);
        void rename(const std::string&);
        
};


#endif