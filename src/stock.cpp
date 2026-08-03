#include "stock.h"

Stock::Stock(const std::string& symbol, const std::string& name, double price)
: m_stockSymbol(symbol), m_stockName(name), m_stockPrice(price)
{
}

void Stock::updatePrice(double price)
{
    m_stockPrice = price;
}