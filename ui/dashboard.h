#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <iostream>

class Dashboard {
    public:
        static Dashboard& getInstance(const std::string& title = "Dashboard") {
            static Dashboard instance;
            instance.m_dashboardTitle = title;
            return instance;
        }
        
        void showLoginDashboard() {
            std::cout << "=== " << m_dashboardTitle << " ===" << std::endl;
            std::cout << "Please log in to access the dashboard." << std::endl;
            std::cout << "=== End of Dashboard ===" << std::endl;
        }

        void showDashboard();

        void setTitle(const std::string& title) {
            m_dashboardTitle = title;
        }

        void actionDashboard();

    private:
        Dashboard() = default;
        Dashboard(const Dashboard&) = delete;
        Dashboard& operator=(const Dashboard&) = delete;
        
        void drawHeader();
        void drawFooter();
        void drawContent();

        std::string m_dashboardTitle;

};



#endif