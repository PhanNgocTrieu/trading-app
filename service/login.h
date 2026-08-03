#ifndef LOGGIN_SERVICE_H
#define LOGGIN_SERVICE_H

#include <iostream>
#include "status.h"
#include "logger.hpp"

namespace Service {
    class LoginService {

        public:
            static LoginService& getInstance() {
                static LoginService instance;
                return instance;
            }

            bool getLoginStatus() const {
                return m_loginStatus;
            }

            virtual ~LoginService() = default;

            
            LOGIN_STATUS requestLogin();
            

        private:
            LoginService() = default;
            LoginService(const LoginService&) = delete;
            LoginService& operator=(const LoginService&) = delete;

            LOGIN_STATUS login(const std::string& username, const std::string& password);
            LOGIN_STATUS logout(const std::string& username);

            bool m_loginStatus = false;

    };
}

#endif