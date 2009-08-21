/****************************************************************
 *                                                              *
 * tagdict.h - the tag dictionary implementation.               *
 *                                                              *
 * Author: Yue Zhang                                            *
 *                                                              *
 * Computing Laboratory, Oxford. 2007.10                        *
 *                                                              *
 ****************************************************************/

#ifndef _TAGDICT_H
#define _TAGDICT_H

#include "hash.h"

/*===============================================================
 *
 * CTagDict - the tagdict
 *
 *==============================================================*/

template <typename CTag>
class CTagDict {

protected:

   CHashMap<CWord, unsigned long> m_mapItems;
   unsigned m_nTotalNumberOfTags;

public:

   CTagDict(unsigned nTotalNumberOfTags) { 
      m_nTotalNumberOfTags = nTotalNumberOfTags ;
      assert( nTotalNumberOfTags < sizeof(unsigned long) * 8 ); 
   }

   virtual ~CTagDict() { }

public:

   unsigned long lookup(const CWord &word) const {
      return m_mapItems.find(word, 0);
   }

   // whether the word and tag pair is in the dictionary
   bool lookup(const CWord &word, const CTag &tag) const {
      return m_mapItems.find(word, 0) & (static_cast<unsigned long int>(1)<<tag.code()) ;
   }

   // add a word and tag pair into entries
   void add(const CWord &word, const CTag &tag) {
      m_mapItems[word] |= (static_cast<unsigned long>(1)<<tag.code());
   }

public:

   // load tag dictionary from a stream
   void load(istream &is) {
      string s, word, tag ;
      unsigned int i ;
      getline(is, s);
      while( is && !(s.empty()) ) {
         istringstream iss(s);
         iss >> word >> tag ;
         i = CTag(tag).code() ; 
//         if ( i==0 ) {
//            throw("unrecognized tag in the dictionary file: " + tag + ".");
//         }
         m_mapItems[word] |= (static_cast<unsigned long int>(1)<<i) ;
         getline(is, s);
      }
   }

   // load tag dictionary from a file
   void load(const string &sFileName) {
      ifstream ifs ;

      ifs.open( sFileName.c_str() ) ;
      assert( ifs.is_open() ) ;
      load(ifs) ;
      ifs.close() ;
   }

   // save tag dictionary to a stream
   void save(ostream &os) {
      CHashMap<CWord, unsigned long>::iterator it;
      unsigned long i, j ; 
      it = m_mapItems.begin() ;
      while (it != m_mapItems.end()) {
         j = it.second();
         for (i = 0; i < m_nTotalNumberOfTags; i++) {
            if ( j & (static_cast<unsigned long int>(1)<<i) ) 
               os << it.first().str() << "\t" << CTag(i).str() << endl ;
         }
         ++it ; 
      }
      os << endl;
   }

   // save tag dictionary to a file
   void save(const string &sFileName) {
      ofstream ofs ;
      ofs.open( sFileName.c_str() ) ;
      assert( ofs.is_open() ) ;
      save(ofs);
      ofs.close() ;
   }

};

//================================================================

template<typename CTag>
istream & operator >> (istream &is, CTagDict<CTag> &dict) {
   dict.load(is);
   return is;
}

template<typename CTag>
ostream & operator << (ostream &os, CTagDict<CTag> &dict) {
   dict.save(os);
   return os;
}

#endif