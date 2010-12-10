// Copyright (C) University of Oxford 2010
/****************************************************************
 *                                                              *
 * tagset.h - the definitions for general tag set.              *
 *                                                              *
 * Author: Yue Zhang                                            *
 *                                                              *
 * Computing Laboratory, Oxford. 2009.8                         *
 *                                                              *
 ****************************************************************/

#ifndef _TAGSET_H
#define _TAGSET_H

/*===============================================================
 *
 * definitions 
 *
 *==============================================================*/

template<typename CTag, unsigned long size>
class CTagSet {

protected:
   unsigned long m_nHash;

public:
   CTagSet() : m_nHash(0) { }
   CTagSet(const unsigned long hash) : m_nHash(hash) { assert(hash>>(CTag::SIZE*size)==0); }
   virtual ~CTagSet() {}

private:
   void operator += (const CTag &i) { 
      m_nHash = (m_nHash<<CTag::SIZE) | i.code();
   }
   unsigned long getTag(const unsigned long &tc) const {
      return tc & ((1<<(CTag::SIZE-1))-1);
   }

public:
   virtual const unsigned long &hash() const { return m_nHash; }
   virtual bool operator == (const CTagSet &set) const { 
      return m_nHash == set.m_nHash; 
   }
   virtual bool operator != (const CTagSet &set) const { 
      return ! ((*this) == set); 
   }
   virtual bool operator < (const CTagSet &set) const { 
      return m_nHash < set.m_nHash; 
   }
   void operator = (const CTagSet &set) { 
      m_nHash = set.m_nHash; 
   }
   const std::string str() const { 
      std::string retval = "";
      unsigned long hs = m_nHash;
      unsigned long tc;
      for (unsigned long i=0; i<size; ++i) {
         if (retval.empty()==false) retval = " " + retval;
         tc = hs & ((1 << CTag::SIZE) -1);
         retval = CTag(tc).str() + retval;
         hs >>= CTag::SIZE;
      }
      return retval; 
   }
   void load(const std::string &s) {
      clear();
      std::istringstream iss(s);
      static std::string t;
      iss >> t;
      for (unsigned long i=0; i<size; ++i) {
         assert(iss.good());
         *this += CTag(t).code();
         iss >> t;
      }
   }
   void load(const unsigned long &i) { m_nHash = i; }
   void clear() { m_nHash = 0; }
};

//===============================================================

template<typename CTag, unsigned long size>
inline const unsigned long &hash(const CTagSet<CTag, size> &set) {return set.hash();}

//===============================================================

template<typename CTag>
inline unsigned long encodeTags(const CTag &tag1, const CTag &tag2) {
   return (tag1.code()<<CTag::SIZE) | tag2.code();
}

template<typename CTag>
inline unsigned long encodeTags(const CTag &tag1, const CTag &tag2, const CTag &tag3) {
   return (tag1.code()<<(CTag::SIZE*2)) | (tag2.code()<<CTag::SIZE) | tag3.code() ;
}

template<typename CTag>
inline unsigned long encodeTags(const CTag &tag1, const CTag &tag2, const CTag &tag3, const CTag &tag4) {
   assert( CTag::SIZE*4<sizeof(unsigned long)*8 ); 
   return (tag1.code()<<CTag::SIZE*3) | (tag2.code()<<CTag::SIZE*2) |
          (tag3.code()<<CTag::SIZE) | tag4.code() ;
}

//===============================================================

template<typename CTag, unsigned long size>
inline std::istream & operator >> (std::istream &is, CTagSet<CTag, size> &c) {

   std::string s;
   std::string t;
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

template<typename CTag, unsigned long size>
inline std::ostream & operator << (std::ostream &os, const CTagSet<CTag, size> &c) {

   os << "[ ";
   os << c.str();
   os << " ]";

   return os;
}

//=============================================================================
//
// a set of tags
//
//=============================================================================

template<typename CTag>
class CSetOfTags {

protected:
   unsigned long long m_code;

public:
   CSetOfTags() : m_code(0) {}

public:
   void add(const CTag &tag) { m_code |= (1LL<<tag.code()); }
   void remove(const CTag &tag) { m_code &= ~(1LL<<tag.code()); }
   void clear() { m_code = 0LL; }

public:
   bool contains(const CTag &tag) const { return m_code & (1LL<<tag.code()); }

public:
   const unsigned long long &code() const { return m_code; }
   const unsigned long long &hash() const { return m_code; }

public:
   bool operator == (const CSetOfTags &s) const { return m_code == s.m_code; }
};

//==============================================================================

template<typename CTag>
inline std::istream & operator >> (std::istream &is, CSetOfTags<CTag> &c) {

   std::string s;
   CTag t;
   is >> s;
   assert(s=="[");

   is >> s;
   while (s!="]") {
     t.load(s);
     ASSERT(t.code()<sizeof(unsigned long long)*8, "tagset.h: The number of tagsexceeds the capacity of CSetOfTags.");
     c.add(t);
     is >> s;
   }

   return is;
}

template<typename CTag>
inline std::ostream & operator << (std::ostream &os, const CSetOfTags<CTag> &c) {

   os << "[ ";
   for (unsigned i=0; i<sizeof(unsigned long long)*8; ++i) {
      static CTag t;
      t.load(i);
      if (c.contains(t)) os << t << ' ';
   }
   os << "]";

   return os;
}

#endif

