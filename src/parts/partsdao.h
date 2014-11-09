#ifndef PARTSDAO_H
#define PARTSDAO_H

#include <QString>

class PartsDAO
{
public:
    PartsDAO();
    /**
     * @brief addStock Add a new entry in the stock change table for the addition of more stock
     * @param partId The id of the part
     * @param quantity Quantity of parts add
     * @param price Price of a single part
     * @param comment A brief comment for the change
     * @return
     */
    static bool addStock(QVariant partId, int quantity, double price, const QString & comment);
    static bool removeStock(QVariant partId, int quantity, const QString & comment);    
};

#endif // PARTSDAO_H
