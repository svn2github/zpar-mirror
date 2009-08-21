/****************************************************************
 *                                                              *
 * writer.cpp - the sentence writer classes definition          *
 *                                                              *
 * Author: Yue Zhang                                            *
 *                                                              *
 * Computing Laboratory, Oxford. 2006.10                        *
 *                                                              *
 ****************************************************************/

#include "writer.h"

/*===============================================================
 *
 * CSentenceWriter - write sentence
 *
 *==============================================================*/

void CSentenceWriter::writeSentence(const CSentenceRaw* sentence) {
   for (int i=0; i<sentence->size(); i++)
      (*m_oStream) << sentence->at(i) << ' ';
   (*m_oStream) << endl;
};

void CSentenceWriter::writeSentence(const CSentenceTagged* sentence, const char separator) {
   for (int i=0; i<sentence->size(); i++) {
      if (i>0)
         m_oStream->put(' ');
      for (int j=0; j<sentence->at(i).first.length(); j++)
         m_oStream->put(sentence->at(i).first[j]);
      m_oStream->put(separator);
      for (int j=0; j<sentence->at(i).second.length(); j++)
         m_oStream->put(sentence->at(i).second[j]);
   }
   (*m_oStream) << endl;
};
