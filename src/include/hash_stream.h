// Copyright (C) University of Oxford 2010
#ifndef _HASH_STREAM_DEF_H
#define _HASH_STREAM_DEF_H

template <typename K, typename V>
inline
std::istream & operator >> (std::istream &is, CHashMap<K, V> &table) {
   K key ;
//   V value ;
   char c ;
   std::string s ; 
   getline(is, s);
   while (is && !(s.empty())) {
      std::istringstream iss(s) ; 
//      iss >> key >> c >> value ;
      iss >> key;
      iss >> c;
      ASSERT( c == ':' , "Hash map does not match key : value format") ;
      iss >> table[key] ;
//      table.insert(key,value);
//TRACE("Read " << key << " : " << table[key]);
      getline(is, s);
   }
   return is;
}

template <typename K, typename V>
inline
std::ostream & operator << (std::ostream &os, CHashMap<K, V> &table) {
   typename CHashMap<K, V>::iterator it = table.begin() ;
   while (it != table.end()) {
      os << it.first() << "\t:\t" << it.second() << std::endl ;
      ++ it;
   }
   os << std::endl ;
   return os ;
}


#endif
