/****************************************************************
 *                                                              *
 * cfgset.h - the definitions for Chinese cfg constituent-set.  *
 *                                                              *
 * Author: Yue Zhang                                            *
 *                                                              *
 * Computing Laboratory, Oxford. 2008.12                        *
 *                                                              *
 ****************************************************************/

#ifndef _CFGSET_H
#define _CFGSET_H

#define CFGSET_SIZE 9

#include "hash.h"

//=============================================================

inline unsigned encodeTorC(const unsigned &tagorconstituent, const bool &tag) {
   assert((tagorconstituent&(1<<(PENN_CON_TAG_COUNT_BITS-1)))==0);
   return tagorconstituent | (tag?(1<<(PENN_CON_TAG_COUNT_BITS-1)):0);
}
inline unsigned long encodeTorCs(const unsigned long &toc1, const unsigned &toc2) {
   assert((toc2>>PENN_CON_TAG_COUNT_BITS)==0);
   return (toc1<<PENN_CON_TAG_COUNT_BITS) | toc2;
}

inline unsigned long encodeTorCs(const unsigned long &toc1, const unsigned &toc2, const unsigned &toc3) {
   assert((toc2>>PENN_CON_TAG_COUNT_BITS)==0);
   assert((toc3>>PENN_CON_TAG_COUNT_BITS)==0);
   return (toc1<<(PENN_CON_TAG_COUNT_BITS*2)) | (toc2<<PENN_CON_TAG_COUNT_BITS) | toc3;
}

/*===============================================================
 *
 * definitions about constituent set 
 *
 *==============================================================*/

class CCFGSet {

protected:
   unsigned long int m_nHash;

public:
   CCFGSet() { clear(); }
   virtual ~CCFGSet() {}

public:
   void operator += (const unsigned &i) { 
      assert(i>>PENN_CON_TAG_COUNT_BITS==0);
      assert(m_nHash>>(PENN_CON_TAG_COUNT_BITS*(CFGSET_SIZE-1))==0);
      m_nHash = (m_nHash<<PENN_CON_TAG_COUNT_BITS)|i;
   }

private:
   bool isTag(const unsigned &tc) const {
      return tc & (1<<(PENN_CON_TAG_COUNT_BITS-1));
   }
   unsigned getTag(const unsigned &tc) const {
      return tc & ((1<<(PENN_CON_TAG_COUNT_BITS-1))-1);
   }

public:
   virtual unsigned long int hash() const { return m_nHash; }
   virtual bool operator == (const CCFGSet &set) const { return m_nHash == set.m_nHash; }
   virtual bool operator != (const CCFGSet &set) const { return m_nHash != set.m_nHash; }
   virtual bool operator < (const CCFGSet &set) const { return m_nHash < set.m_nHash; }
   void operator = (const CCFGSet &set) { m_nHash = set.m_nHash; }
   const string str() const { 
      string retval = "";
      unsigned long hs = m_nHash;
      unsigned tc;
      while (hs) {
         if (retval.empty()==false) retval = " " + retval;
         tc = hs & ((1 << PENN_CON_TAG_COUNT_BITS) -1);
         if (isTag(tc))
            retval = "."+CTag(getTag(tc)).str()+retval;
         else
            retval = CConstituent(tc).str() + retval;
         hs >>= PENN_CON_TAG_COUNT_BITS;
      }
      return retval; 
   }
   void load(const string &s) {
      clear();
      istringstream iss(s);
      static string t;
      iss >> t;
      while (iss.good()) {
         if (t[0]=='.') 
            *this += CTag(t.substr(1)).code()|(1<<(PENN_CON_TAG_COUNT_BITS-1));
         else
            *this += CConstituent(t).code();
         iss >> t;
      }
   }
   bool empty() { return m_nHash==0 ; }
   bool clear() { m_nHash = 0; }
};

//===============================================================

inline istream & operator >> (istream &is, CCFGSet &c) {

   string s;
   string t;
   is >> t;
   assert(t=="[");

   is >> t;
   s = "";
   while (t!="]") {
     s+=t;
     s+=" ";
     is >> t;
   }

   c.load(s);

   return is;
}

inline ostream & operator << (ostream &os, const CCFGSet &c) {

   os << "[ ";
   os << c.str();
   os << " ]";

   return os;
}

inline unsigned long int hash(const CCFGSet &set) {return set.hash();}

#endif
