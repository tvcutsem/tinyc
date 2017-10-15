#ifndef UNDEFINED_H
#define UNDEFINED_H

using namespace std;

/**
 * A class denoting any reference to an identifier which has
 * not previously been declared
 */
class UndefinedIdentifier {

  public:

    /**
     * @param name the name of the undefined identifier, shared in string pool
     */
    UndefinedIdentifier(string* name): name_(name) { }

    string* name() const { return name_; }

  private:
    string* name_;

};

#endif
