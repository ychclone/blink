#ifndef ISTORAGE_HANDLER_H
#define ISTORAGE_HANDLER_H

#include "Utils/commonType.h"

// interface for storage handler

class IStorageHandler {
public:
    IStorageHandler() {};
    virtual ~IStorageHandler() {};
    
    virtual BYTE loadFromFile(const QString& filename, QMap<QString, CProfileItem>& profileList, QMap<QString, CGroupItem>& groupList) = 0;
    virtual BYTE saveToFile(const QString& filename, const QMap<QString, CProfileItem>& profileList, const QMap<QString, CGroupItem>& groupList) = 0;
};

#endif // ISTORAGE_HANDLER_H


