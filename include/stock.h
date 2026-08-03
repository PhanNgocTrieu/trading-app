#ifndef STOCK_H
#define STOCK_H

#include <iostream>

class Stock {
    
    private:
        std::string     m_stockSymbol;
        std::string     m_stockName;
        double          m_stockPrice;

    public:
        Stock(const std::string& symbol, const std::string& name, double price);
        void updatePrice(double price);
};


#endif