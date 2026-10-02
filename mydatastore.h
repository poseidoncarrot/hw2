#ifndef MYDATASTORE_H
#define MYDATASTORE_H

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "datastore.h"

class MyDataStore : public DataStore {
  public:
    MyDataStore();
    virtual ~MyDataStore();
    virtual void addProduct(Product* p);
    virtual void addUser(User* u);
    virtual std::vector<Product*> search(std::vector<std::string>& terms, int type);
    virtual void dump(std::ostream& ofile);
    bool addToCart(const std::string& username, Product* product);
    bool viewCart(const std::string& username, std::ostream& os) const;
    bool buyCart(const std::string& username);
  private:
    std::vector<Product*> products_;
    std::vector<User*> users_;
    std::map<std::string, User*> userMap_;
    std::map<std::string, std::set<Product*> > keywordIndex_;
    std::map<std::string, std::vector<Product*> > carts_;
};

#endif