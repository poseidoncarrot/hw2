#include "mydatastore.h"
#include "util.h"

using namespace std;

MyDataStore::MyDataStore() {

}

MyDataStore::~MyDataStore() {
  for (vector<Product*>::iterator it = products_.begin(); it != products_.end(); it++) {
    delete *it;
  }
  for (vector<User*>::iterator it = users_.begin(); it != users_.end(); it++) {
    delete *it;
  }
}

void MyDataStore::addProduct(Product* p) {
  if (p == NULL) {
    return;
  }
  products_.push_back(p);
  set<string> productKeywords = p->keywords();
  for (set<string>::iterator it = productKeywords.begin(); it != productKeywords.end(); it++) {
    string keyword = convToLower(*it);
    keywordIndex_[keyword].insert(p);
  }
}

void MyDataStore::addUser(User* u) {
  if (u == NULL) {
    return;
  }
  users_.push_back(u);
  string username = convToLower(u->getName());
  userMap_[username] = u;
  carts_[username] = vector<Product*>();
}

vector<Product*> MyDataStore::search(vector<string>& terms, int type) {
  set<Product*> result;
  if (terms.size() == 0) {
    return vector<Product*>();
  }
  string firstTerm = convToLower(terms[0]);
  map<string, set<Product*> >::iterator firstIt = keywordIndex_.find(firstTerm);
  if(firstIt != keywordIndex_.end()) {
    result = firstIt->second;
  }
  for (unsigned int i = 1; i < terms.size(); i++) {
    string term = convToLower(terms[i]);
    set<Product*> currentProducts;
    map<string, set<Product*> >::iterator found = keywordIndex_.find(term);
    if (found != keywordIndex_.end()) {
      currentProducts = found->second;
    }
    if (type == 0) {
      result = setIntersection(result, currentProducts);
    }
    else {
      result = setUnion(result, currentProducts);
    }
  }
  vector<Product*> hits(result.begin(), result.end());
  return hits;
}

void MyDataStore::dump(ostream& ofile) {
  ofile << "<products>" << endl;
  for (vector<Product*>::iterator it = products_.begin(); it != products_.end(); it++) {
    (*it)->dump(ofile);
  }
  ofile << "</products>" << endl;
  ofile << "<users>" << endl;
  for (vector<User*>::iterator it = users_.begin(); it != users_.end(); it++) {
    (*it)->dump(ofile);
  }
  ofile << "</users>" << endl;
}

bool MyDataStore::addToCart(const string& username, Product* product) {
  string lowercaseUsername = convToLower(username);
  map<string, User*>::iterator userIt = userMap_.find(lowercaseUsername);
  if (userIt == userMap_.end() || product == NULL) {
    return false;
  }
  carts_[lowercaseUsername].push_back(product);
  return true;
}

bool MyDataStore::viewCart(const string& username, ostream& os) const {
  string lowercaseUsername = convToLower(username);
  map<string, User*>::const_iterator userIt = userMap_.find(lowercaseUsername);
  if (userIt == userMap_.end()) {
    return false;
  }
  map<string, vector<Product*> >::const_iterator cartIt = carts_.find(lowercaseUsername);
  if(cartIt == carts_.end()) {
    return true;
  }
  for (unsigned int i = 0; i < cartIt->second.size(); i++) {
    os << "Item " << i + 1 << endl;
    os << cartIt->second[i]->displayString() << endl;
  }
  return true;
}

bool MyDataStore::buyCart(const string& username) {
  string lowercaseUsername = convToLower(username);
  map<string, User*>::iterator userIt = userMap_.find(lowercaseUsername);
  if (userIt == userMap_.end()) {
    return false;
  }
  User* user = userIt->second;
  vector<Product*>& cart = carts_[lowercaseUsername];
  vector<Product*> remainingItems;
  for (vector<Product*>::iterator it = cart.begin(); it != cart.end(); it++) {
    Product* product = *it;
    bool inStock = product->getQty() > 0;
    bool enoughCredit = user->getBalance() >= product->getPrice();
    if (inStock && enoughCredit) {
      product->subtractQty(1);
      user->deductAmount(product->getPrice());
    }
    else {
      remainingItems.push_back(product);
    }
  }
  cart = remainingItems;
  return true;
}