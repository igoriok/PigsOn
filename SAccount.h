#ifndef SACCOUNT_H
#define SACCOUNT_H

#include <QtCore/QString>

struct Account {
    Account() {}
    Account(const QString &user, const QString &pass) {
        this->user = user;
        this->pass = pass;
    }
    QString user;
    QString pass;
    bool isValid() const {
        if (user.isEmpty() || pass.isEmpty()) return false;
        else return true;
    }
    void clear() {
        user.clear();
        pass.clear();
    }
};

#endif // SACCOUNT_H
