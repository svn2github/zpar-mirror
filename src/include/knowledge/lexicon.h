/****************************************************************
 *                                                              *
 * lexicon.h - the lexicon dictionary equals to a set of words. *
 *                                                              *
 * Author: Yue Zhang                                            *
 *                                                              *
 * Computing Laboratory, Oxford. 2008.03                        *
 *                                                              *
 ****************************************************************/

#ifndef _DICT_LEXICON_H
#define _DICT_LEXICON_H

#include "hash.h"
#include "linguistics/word.h"

class CLexiconSet : public CHashMap<CWord, unsigned long> {
public:
   void add(const CWord &word) { (*this)[word]=1; }
   bool contains(const CWord &word) { return static_cast<bool>(find(word, 0)); }
};

inline
istream & operator >> (istream &is, CLexiconSet &dict) {
   if (!is) return is;
   string s ;
   string ln;
   getline(is, ln);
   while (is && !(ln.empty())) {
      istringstream iss(ln);
      iss >> s; 
      dict.add(CWord(s));
      getline(is, ln);
   }
   return is;
}

inline
ostream & operator << (ostream &os, CLexiconSet &dict) {
   CLexiconSet::iterator it = dict.begin();
   while (it != dict.end()) {
      os << it.first().str() << endl;
      ++it;
   }
   os << endl;
   return os;
}

#endif
