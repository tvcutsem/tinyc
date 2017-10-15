#ifndef STRINGPOOL_H
#define STRINGPOOL_H

#include <string>
#include <set>

using namespace std;

/**
 * Used as comparator for string pointers in the stringpool
 */
struct StringCmp {
  bool operator() (string* elt1, string* elt2) const {
    return ((*elt1) < (*elt2));
  }
};

typedef set<string*>::iterator StringPoolit;

/**
 * Class providing standard string sharing facilities.
 *
 * A call to getString is guaranteed to return a pointer to a string, which
 * will be the only string for this character sequence allocated in this pool
 * StringPool is a singleton, use instance() to access its sole instance
 */
class StringPool {

   public:

      /**
       * returns the only instance of this class.
       */
      static StringPool* instance();

      ~StringPool();

      /**
       * destroys the singleton
       */
      static void destroy();

      /**
       * convenience method
       */
      string* getString(const char* name) {
	return getString(string(name));
      }

      /**
       * Returns a unique string allocated in the pool.
       * If the string is not found in the pool yet, it is
       * inserted.
       */
      string* getString(string name);

      /**
       * @return number of strings in the pool
       */
      unsigned int size() const { return pool_->size(); }

   protected:
      //constructor made protected to ensure a unique instance
      StringPool(): pool_(new set<string*,StringCmp>) { }

      //the only instance of this class
      static StringPool* instance_;

   private:

      string* find(string& name);
      set<string*,StringCmp>* pool_;

};

#endif
