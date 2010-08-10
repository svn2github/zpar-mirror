/**************************************************************** 
 *                                                              *
 * Specific implementation by agenda                            *
 *                                                              *
 * Yue Zhang, 2007                                              *
 *                                                              *
 ****************************************************************/

#ifndef SEGMENTOR_AGENDA_IMPL_H
#define SEGMENTOR_AGENDA_IMPL_H

const int CHAR_TYPE_SIZE = 4;
const unsigned DEFAULT_TABLE_SIZE = 65536;

#define iterate_templates(left,right) \
   left(m_weights.m_mapCharUnigram)right\
   left(m_weights.m_mapCharBigram)right\
   left(m_weights.m_mapCharTrigram)right\
   left(m_weights.m_mapSeenWords)right\
   left(m_weights.m_mapLastWordByWord)right\
   left(m_weights.m_mapWordAndNextChar)right\
   left(m_weights.m_mapWordAndPrevChar)right\
   left(m_weights.m_mapFirstCharLastWordByWord)right\
   left(m_weights.m_mapLastWordByLastChar)right\
   left(m_weights.m_mapSeparateChars)right\
   left(m_weights.m_mapConsecutiveChars)right\
   left(m_weights.m_mapFirstAndLastChars)right\
   left(m_weights.m_mapFirstCharAndChar)right\
   left(m_weights.m_mapOneCharWord)right\
   left(m_weights.m_mapLengthByFirstChar)right\
   left(m_weights.m_mapLengthByLastChar)right\
   left(m_weights.m_mapLengthByLastWord)right\
   left(m_weights.m_mapLastLengthByWord)right\
   left(m_weights.m_mapLexiconWord)right\
   left(m_weights.m_mapCharCatUnigram)right\
   left(m_weights.m_mapCharCatBigram)right\
   left(m_weights.m_mapCharCatTrigram)right

/*===============================================================
 *
 * CWeight - the definition of weights, in feature
 *
 *==============================================================*/

class CWeight {

public:
   typedef CScoreMapType< CWord, SCORE_TYPE > CWordMap;
   typedef CScoreMapType< CTwoWords, SCORE_TYPE > CTwoWordsMap;
   typedef CScoreMapType< pair<int, int>, SCORE_TYPE > CPairIntMap;
   typedef CScoreMapType< pair<CWord, int>, SCORE_TYPE > CWordIntMap;
   typedef CScoreMapType< pair<CTwoWords, int>, SCORE_TYPE > CTwoWordsIntMap;
   typedef CScoreMapType< unsigned long int, SCORE_TYPE > CIntMap;

   // char based features
   CWordIntMap m_mapCharUnigram;
   CWordIntMap m_mapCharBigram;
   CWordIntMap m_mapCharTrigram;

   // word based features
   CWordMap m_mapSeenWords;
   CTwoWordsMap m_mapLastWordByWord;
   CTwoWordsMap m_mapWordAndNextChar;
   CTwoWordsMap m_mapWordAndPrevChar;
   CTwoWordsMap m_mapFirstCharLastWordByWord;
   CTwoWordsMap m_mapLastWordByLastChar;
   CWordMap m_mapSeparateChars;
   CWordMap m_mapConsecutiveChars;
   CTwoWordsMap m_mapFirstAndLastChars;
   CTwoWordsMap m_mapFirstCharAndChar;
   CWordMap m_mapOneCharWord;
   CWordIntMap m_mapLengthByFirstChar;
   CWordIntMap m_mapLengthByLastChar;
   CWordIntMap m_mapLengthByLastWord;
   CWordIntMap m_mapLastLengthByWord;

   // word knowledge features
   CPairIntMap m_mapLexiconWord;
   CPairIntMap m_mapCharCatUnigram;
   CPairIntMap m_mapCharCatBigram;
   CPairIntMap m_mapCharCatTrigram;

public:
   CWeight() : m_mapCharUnigram("CharUnigram", DEFAULT_TABLE_SIZE),
               m_mapCharBigram("CharBigram", DEFAULT_TABLE_SIZE),
               m_mapCharTrigram("CharTrigram", DEFAULT_TABLE_SIZE),
               m_mapSeenWords("SeenWords", DEFAULT_TABLE_SIZE),
               m_mapLastWordByWord("LastWordByWord", DEFAULT_TABLE_SIZE),
               m_mapWordAndNextChar("WordAndNextChar", DEFAULT_TABLE_SIZE),
               m_mapWordAndPrevChar("WordAndPrevChar", DEFAULT_TABLE_SIZE),
               m_mapFirstCharLastWordByWord("FirstCharLastWordByWord", DEFAULT_TABLE_SIZE),
               m_mapLastWordByLastChar("LastWordByLastChar", DEFAULT_TABLE_SIZE),
               m_mapSeparateChars("SeparateChars", DEFAULT_TABLE_SIZE),
               m_mapConsecutiveChars("ConsecutiveChars", DEFAULT_TABLE_SIZE),
               m_mapFirstAndLastChars("FirstAndLastChars", DEFAULT_TABLE_SIZE),
               m_mapFirstCharAndChar("FirstCharAndChar", DEFAULT_TABLE_SIZE),
               m_mapOneCharWord("OneCharWord", DEFAULT_TABLE_SIZE),
               m_mapLengthByFirstChar("LengthByFirstChar", DEFAULT_TABLE_SIZE),
               m_mapLengthByLastChar("LengthByLastChar", DEFAULT_TABLE_SIZE),
               m_mapLengthByLastWord("LengthByLastWord", DEFAULT_TABLE_SIZE),
               m_mapLastLengthByWord("LastLengthByWord", DEFAULT_TABLE_SIZE),
               m_mapLexiconWord("LexiconWord", 7), 
               m_mapCharCatUnigram("CharCatUnigram", 997),
               m_mapCharCatBigram("CharCatBigram", 997), 
               m_mapCharCatTrigram("CharCatTrigram", 997) {
   }
};

/*===============================================================
 *
 * The implementation specific part of segmentor is defined here. 
 *
 *==============================================================*/

class CSegmentorImpl {
};

//===============================================================

inline int normalizeLength(const int &length) {
   return length<LENGTH_MAX-1?length:LENGTH_MAX-1;
}

#endif
