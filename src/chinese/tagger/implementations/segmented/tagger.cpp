/****************************************************************
 *                                                              *
 * segmented.cpp - the Chinese segmented sentence tagger impls. *
 *                                                              *
 * Author: Yue Zhang                                            *
 *                                                              *
 * Computing Laboratory, Oxford. 2007.5                         *
 *                                                              *
 ****************************************************************/

#include "tagger.h"

using namespace chinese;
using namespace chinese::tagger;

static CWord g_emptyWord("");

/*===============================================================
 *
 * CTagger - the detail coding for the tagger based on words
 *
 *==============================================================*/

/*---------------------------------------------------------------
 *
 * getLocalScore - get the local score for a word in sentence
 *
 * When bigram is needed from the beginning of sentence, the
 * -BEGIN- tag and the empty word are used. 
 *
 * This implies that empty words should not be used in other 
 * situations. 
 *
 *--------------------------------------------------------------*/

SCORE_TYPE CTagger::getLocalScore( const CSentenceRaw * sentence, CStateItem * item , int index ) {

   static SCORE_TYPE nReturn ; 
   static unsigned int last_start , last_length ;
   static unsigned int start , end , length ; 

   // about the words
   start = item->getWordStart( index ) ;
   end = item->getWordEnd( index ) ;
   length = item->getWordLength( index ) ; 
   last_start = index > 0 ? item->getWordStart( index-1 ) : 999999 ;
   last_length = index > 0 ? item->getWordLength( index-1 ) : 99999 ;
   const CWord &word = m_WordCache.find( start , end , sentence ) ; 
   const CWord &last_word = index > 0 ? m_WordCache.find( last_start , start-1 , sentence ) : g_emptyWord ; 

   // about the chars
   const CWord &first_char = m_WordCache.find( start , start , sentence ) ;
   const CWord &last_char = m_WordCache.find( end , end , sentence ) ;
   const CWord &first_char_last_word = index > 0 ? m_WordCache.find( last_start , last_start , sentence ) : g_emptyWord ;
   const CWord &last_char_last_word = index > 0 ? m_WordCache.find( start-1 , start-1 , sentence) : g_emptyWord;
   const CWord &first_char_next_word = end+1 < sentence->size() ? m_WordCache.find( end+1 , end+1 , sentence) : g_emptyWord ;
   const CWord &first_twochar = start+1 < sentence->size() ? m_WordCache.find( start , start+1 , sentence ) : g_emptyWord ;
   const CWord &last_twochar_last_word = start>1 ? m_WordCache.find( start-2 , start-1 , sentence ) : g_emptyWord ;
   const CWord &two_char = index > 0 ? m_WordCache.find( start-1 , start, sentence) : g_emptyWord;
   const CWord &lastword_firstchar = index > 0 ? m_WordCache.find( last_start , start , sentence ) : g_emptyWord ; 
   const CWord &currentword_lastchar = index > 0 ? m_WordCache.find( start-1 , end , sentence) : g_emptyWord ;
   const CWord &currentword_lasttwochar = start > 1 ? m_WordCache.find( start-2 , end , sentence ) : g_emptyWord ;
   const CWord &lastword_firsttwochar = index > 0 && start+1 < sentence->size() ? m_WordCache.find( last_start , start+1 , sentence ) : g_emptyWord ;
   const CWord &three_char = length == 1 && start > 0 && end < sentence->size()-1 ? m_WordCache.find( start-1 , end+1 , sentence ) : g_emptyWord ;

   CTwoWords two_word;

   // about the tags 
   const unsigned &tag = item->getTag(index);
   const unsigned &last_tag = index>0 ? item->getTag(index-1) : PENN_TAG_BEGIN;
   const unsigned last_two_tag = index>1 ? joinTwoTags(item->getTag(index-2), last_tag) : joinTwoTags(PENN_TAG_BEGIN, last_tag);
   static CTaggedWord<CTag> wt1, wt2;
   static CTwoTaggedWords wt12;

   int first_char_cat = m_weights->m_mapCharTagDictionary.lookup(first_char) | (1<<tag) ;
   int last_char_cat = m_weights->m_mapCharTagDictionary.lookup(last_char) | (1<<tag) ;

   nReturn = m_weights->m_mapCurrentTag.getScore( make_pair(word, tag) , m_nScoreIndex ) ; 
   nReturn += m_weights->m_mapLastTagByTag.getScore( make_pair(tag, last_tag) , m_nScoreIndex ) ;
   nReturn += m_weights->m_mapLastTwoTagsByTag.getScore( make_pair(tag, last_two_tag) , m_nScoreIndex ) ;
   if ( start > 0 ) {
      if ( last_length <= 2 ) nReturn += m_weights->m_mapTagByLastWord.getScore( make_pair(last_word, tag) , m_nScoreIndex ) ;
      if ( length <= 2 ) nReturn += m_weights->m_mapLastTagByWord.getScore( make_pair(word, last_tag) , m_nScoreIndex ) ;
      if ( length <= 2 ) nReturn += m_weights->m_mapTagByWordAndPrevChar.getScore( make_pair(currentword_lastchar, tag) , m_nScoreIndex ) ;
      if ( last_length <= 2 ) nReturn += m_weights->m_mapTagByWordAndNextChar.getScore( make_pair(lastword_firstchar, last_tag) , m_nScoreIndex) ;

   }
   if ( length == 1 ) {
      if ( start > 0 && end < sentence->size()-1 )
         nReturn += m_weights->m_mapTagOfOneCharWord.getScore( make_pair(three_char, tag) , m_nScoreIndex ) ;
   }
   else {
      nReturn += m_weights->m_mapTagByFirstChar.getScore( make_pair(first_char, tag) , m_nScoreIndex ) ; 
      nReturn += m_weights->m_mapTagByLastChar.getScore( make_pair(last_char, tag) , m_nScoreIndex ) ;
      nReturn += m_weights->m_mapTagByFirstCharCat.getScore( make_pair(first_char_cat, tag) , m_nScoreIndex ) ; 
      nReturn += m_weights->m_mapTagByLastCharCat.getScore( make_pair(last_char_cat, tag) , m_nScoreIndex ) ;

      for ( int j = 0 ; j < item->getWordLength( index ) ; j ++ ) {
         if ( j > 0 && j < item->getWordLength( index )-1 )
            nReturn += m_weights->m_mapTagByChar.getScore( make_pair(m_WordCache.find(start+j, start+j, sentence), tag) , m_nScoreIndex );
         if ( j > 0 ) {
            wt1.load( m_WordCache.find(start+j, start+j, sentence), tag ); wt2.load( first_char ); wt12.refer(&wt1, &wt2); nReturn += m_weights->m_mapTaggedCharByFirstChar.getScore( wt12, m_nScoreIndex );
            if ( m_WordCache.find(start+j, start+j, sentence) == m_WordCache.find(start+j-1, start+j-1, sentence)) 
               nReturn += m_weights->m_mapRepeatedCharByTag.getScore( make_pair(m_WordCache.find(start+j, start+j, sentence), tag) , m_nScoreIndex );
         }
         if ( j < item->getWordLength( index )-1 ) {
            wt1.load( m_WordCache.find(start+j, start+j, sentence), tag ); wt2.load( last_char ); wt12.refer(&wt1, &wt2); nReturn += m_weights->m_mapTaggedCharByLastChar.getScore( wt12 , m_nScoreIndex );
         }
      }
   }

   return nReturn;
}

/*---------------------------------------------------------------
 *
 * updateScores - update the score vector by input
 *                this is used in training to adjust params
 *
 * Inputs: the tagged and the correct example
 *
 * Affects: m_bScoreModified, which leads to saveScores on destructor
 *
 *--------------------------------------------------------------*/

void CTagger::updateScores(const CSentenceTagged* tagged, const CSentenceTagged* correct, int round) {

   static int i , j ;

   if ( *tagged != *correct ) {
      for (i=0; i<tagged->size(); ++i) 
         updateLocalFeatureVector(eSubtract, tagged, i, round);
      for (i=0; i<correct->size(); ++i)
         updateLocalFeatureVector(eAdd, correct, i, round);
   }

   if (round>m_nNumberOfCurrentTrainingExample) {
      //
      // Updates that are common for all example
      //
      m_nNumberOfCurrentTrainingExample = round;
      for (i=0; i<correct->size(); i++) {
         CWord word = correct->at(i).first;
         CTag tag(correct->at(i).second);
         CSentenceRaw chars;
         chars.clear(); getCharactersFromUTF8String(correct->at(i).first, &chars);
         m_weights->m_mapWordFrequency[word]++;
         m_weights->m_mapTagDictionary.add(word, tag);
         if (m_weights->m_mapWordFrequency[word]>m_weights->m_nMaxWordFrequency) m_weights->m_nMaxWordFrequency = m_weights->m_mapWordFrequency[word];
         for ( j = 0 ; j < chars.size() ; j++ ) m_weights->m_mapCharTagDictionary.add(chars[j], tag) ;
      }
   }
}

/*---------------------------------------------------------------
 *
 * updateLocalFeatureVector - update the given feature vector with
 *                            the local feature vector for a given
 *                            sentence. This is a private member only 
 *                            used by updateGlobalFeatureVector and is
 *                            only used for training. 
 *
 *--------------------------------------------------------------*/

void CTagger :: updateLocalFeatureVector( SCORE_UPDATE method , const CSentenceTagged * sentence , int index , int round ) { 
   // about words              
   CWord word = sentence->at( index ).first ;
   CWord last_word = index > 0 ? sentence->at( index - 1 ).first : g_emptyWord ;
   CWord next_word = index < sentence->size() - 1 ? sentence->at( index + 1 ).first : g_emptyWord ;
   CSentenceRaw chars , last_chars ;
   chars.clear() ; getCharactersFromUTF8String( sentence->at(index).first , &chars ) ;
   last_chars.clear() ; if ( index > 0 ) getCharactersFromUTF8String( sentence->at( index - 1 ).first , &last_chars ) ;
   // about length
   int length = chars.size() ; //if ( length > LENGTH_MAX-1 ) length = LENGTH_MAX-1 ;
   int last_length = last_chars.size() ; //if ( last_length > LENGTH_MAX-1 ) last_length = LENGTH_MAX-1 ;
   // about chars  
   CWord first_char = chars[ 0 ];
   CWord last_char = chars[ chars.size() - 1 ];
   CWord first_char_last_word = index > 0 ? last_chars[ 0 ] : g_emptyWord;
   CWord last_char_last_word = index > 0 ? last_chars[ last_chars.size() - 1 ] : g_emptyWord;
   CWord first_char_next_word = index + 1 < sentence->size() ? getFirstCharFromUTF8String( sentence->at( index + 1 ).first ) : g_emptyWord ;
   CWord last_twochar_last_word = last_chars.size() > 1 ? last_chars[ last_chars.size() - 2 ] + last_chars[ last_chars.size() - 1] 
                                                        : ( index > 1 ? getLastCharFromUTF8String(sentence->at(index-2).first) + last_chars[ 0 ] : g_emptyWord );
   CWord first_twochar = chars.size() > 1 ? chars[ 0 ] + chars [ 1 ] : ( index + 1 <sentence->size() ? chars[ 0 ] + getFirstCharFromUTF8String( sentence->at( index + 1 ).first ) : g_emptyWord );
   CWord currentword_lasttwochar = index > 1 ? last_twochar_last_word.str() + word.str() : g_emptyWord ;
   CWord lastword_firsttwochar = index > 0 && index+1 < sentence->size() ? last_word.str() + first_twochar.str() : g_emptyWord ; 

   CWord two_char = index > 0 ? last_char_last_word.str() + first_char.str() : g_emptyWord ;
   CWord lastword_firstchar = index > 0 ? last_word.str() + first_char.str() : g_emptyWord ; 
   CWord currentword_lastchar = index > 0 ? last_char_last_word.str() + word.str() : g_emptyWord ;
   CWord three_char = length == 1 ? last_char_last_word.str() + word.str() + first_char_next_word.str() : g_emptyWord ;

   CTwoWords two_word ; 

   // about tags
   unsigned tag = CTag( sentence->at(index).second ).code() ; 
   unsigned last_tag = index > 0 ? CTag( sentence->at( index-1 ).second).code() : PENN_TAG_BEGIN ;
   unsigned last_two_tag = index > 1 ? joinTwoTags( CTag( sentence->at( index-2 ).second ).code() , last_tag ) : joinTwoTags( PENN_TAG_BEGIN , last_tag ) ;
   CTaggedWord<CTag> wt1, wt2;
   CTwoTaggedWords wt12;
   
   // about the char categories 
   int first_char_cat = m_weights->m_mapCharTagDictionary.lookup(first_char) | (1<<tag) ; 
   int last_char_cat = m_weights->m_mapCharTagDictionary.lookup(last_char) | (1<<tag) ; 
   SCORE_TYPE amount = method == eAdd ? 1 : -1 ; 

   m_weights->m_mapCurrentTag[ make_pair(word, tag) ].updateCurrent( amount , round ) ;
   m_weights->m_mapLastTagByTag[ make_pair(tag, last_tag) ].updateCurrent( amount , round ) ;
   m_weights->m_mapLastTwoTagsByTag[ make_pair(tag, last_two_tag) ].updateCurrent( amount , round ) ;
   if ( index > 0 ) {
      if ( last_length <= 2 ) m_weights->m_mapTagByLastWord[ make_pair(last_word, tag) ].updateCurrent( amount , round ) ;
      if ( length <= 2 ) m_weights->m_mapLastTagByWord[ make_pair(word, last_tag) ].updateCurrent( amount , round ) ;
      if ( length <= 2 ) m_weights->m_mapTagByWordAndPrevChar[ make_pair(currentword_lastchar, tag) ].updateCurrent( amount , round ) ;
      if ( last_length <= 2 ) m_weights->m_mapTagByWordAndNextChar[ make_pair(lastword_firstchar, last_tag) ].updateCurrent( amount , round ) ;
   }
   if ( length == 1 ) {
      if ( index > 0 && index < sentence->size() - 1 )
         m_weights->m_mapTagOfOneCharWord[ make_pair(three_char, tag) ].updateCurrent( amount , round ) ; 
   }
   else {
      m_weights->m_mapTagByFirstChar[ make_pair(first_char, tag) ].updateCurrent( amount , round ) ; 
      m_weights->m_mapTagByLastChar[ make_pair(last_char, tag) ].updateCurrent( amount , round ) ;                    //
      m_weights->m_mapTagByFirstCharCat[ make_pair(first_char_cat, tag) ].updateCurrent( amount , round ) ; 
      m_weights->m_mapTagByLastCharCat[ make_pair(last_char_cat, tag) ].updateCurrent( amount , round ) ;
      for ( int j = 0 ; j < chars.size() ; j ++ ) {
         if ( j > 0 && j < chars.size() - 1 )                                                                
            m_weights->m_mapTagByChar[ make_pair(CWord(chars[j]), tag) ].updateCurrent( amount , round ) ;
         if ( j > 0 ) {
            wt1.load(chars[j], tag); wt2.load(first_char); wt12.allocate(wt1, wt2); m_weights->m_mapTaggedCharByFirstChar[ wt12 ].updateCurrent( amount , round ) ;
            if ( chars[j] == chars[j-1] ) m_weights->m_mapRepeatedCharByTag[ make_pair(CWord(chars[j]), tag) ].updateCurrent( amount , round ) ; //
         }
         if (j<chars.size()-1) {
            wt1.load(chars[j], tag); wt2.load(last_char); wt12.allocate(wt1, wt2); m_weights->m_mapTaggedCharByLastChar[ wt12 ].updateCurrent(amount, round);
         }
      }
   }

}

/*---------------------------------------------------------------
 *
 * train - train the module auto
 *
 * Since we rely on external trainer, this method is empty
 *
 *--------------------------------------------------------------*/

void CTagger::train( const CSentenceRaw * sentence , const CSentenceTagged * correct , int round ) {
   cerr << "Not implemented" << endl;
   assert( 0 == 1 );
}

/*---------------------------------------------------------------
 *
 * generate - helper function that generates tagged output
 *
 *--------------------------------------------------------------*/

void generate(const CStateItem *stateItem, CSentenceRaw *sentence, CSentenceTagged *vReturn) {
   string s;
   for (int j=0; j<stateItem->size(); j++) { 
      s.clear();
      for (int k=stateItem->getWordStart(j); k<stateItem->getWordEnd(j)+1; k++)
         s += sentence->at(k);
      vReturn->push_back(make_pair(s, CTag(stateItem->getTag(j)).str()));
   }
}

/*---------------------------------------------------------------
 *
 * tag - assign POS tags to a sentence
 *
 * Returns: makes a new instance of CSentenceTagged 
 *
 *--------------------------------------------------------------*/

void CTagger::tag( const CSentenceRaw * sentence , CSentenceTagged * vReturn , SCORE_TYPE * out_scores , int nBest , const CBitArray * word_ends ) {
   clock_t total_start_time = clock();;
   const int length = sentence->size() ;

   static int end_index, temp_index, start_index, word_index, j;
   static int tag, last_tag;
   tagger::CStateItem *pGenerator;
   tagger::CStateItem *pCandidate;
   static CStateItem best_bigram[PENN_TAG_COUNT][PENN_TAG_COUNT];
   static int done_bigram[PENN_TAG_COUNT][PENN_TAG_COUNT];
   static SCORE_TYPE current_score;
   static unsigned long int possible_tags; // possible tags for a word

   if (length>=m_nMaxSentSize) 
      THROW("the length of the sentence is bigger than the maximum sentence size "<<m_nMaxSentSize<<"; try changing the option");
   assert(vReturn!=NULL); 
   assert(word_ends!=NULL);

   // initialise the return value, the agenda and cache
   TRACE("Initialising the tagging process...");
   vReturn->clear();

   m_Agenda.clear();
   pCandidate = m_Agenda.candidateItem(); // make the first item
   pCandidate->clear();                   // restore state using clean
   m_Agenda.pushCandidate();              // and push it back
   m_Agenda.nextRound();                  // as the generator item

   m_WordCache.clear() ; 

   for ( tag=0; tag<PENN_TAG_COUNT; tag++ )
      for ( last_tag=0; last_tag<PENN_TAG_COUNT; last_tag++ )
         done_bigram[last_tag][tag] = -1;

   TRACE("Tagging started"); 

   end_index = 0; word_index = 0;
   while ( end_index<length && word_ends->isset(end_index)==false ) ++end_index ;
   while ( end_index<length ) {
      // generate new state itmes for each character
      pGenerator = m_Agenda.generatorStart();
      start_index = word_index==0 ? 0 : pGenerator->getWordEnd(word_index-1)+1;

      for ( j=0; j<m_Agenda.generatorSize(); ++j ) {
         last_tag = word_index>0 ? pGenerator->getTag(word_index-1) : PENN_TAG_BEGIN ;
         pGenerator->append( end_index);

         bool bDone = false;
         for ( tag=PENN_TAG_FIRST; tag<PENN_TAG_COUNT; ++tag ) {
            if ( ( m_weights->m_mapWordFrequency.find( m_WordCache.find(start_index, end_index, sentence) , 0 ) < 
                   m_weights->m_nMaxWordFrequency/5000+5 && 
                   PENN_TAG_CLOSED[ tag ] == false ) ||
                 m_weights->m_mapTagDictionary.lookup( m_WordCache.find(start_index, end_index, sentence) , tag ) ) {
               bDone = true;
               pGenerator->setTag( word_index, tag );
               current_score = pGenerator->score + getLocalScore(sentence, pGenerator, word_index); 
               if (nBest==1) {
                  if ( done_bigram[last_tag][tag] != word_index || current_score > best_bigram[last_tag][tag].score ) {
                     done_bigram[last_tag][tag] = word_index;
                     best_bigram[last_tag][tag].copy( pGenerator );
                     best_bigram[last_tag][tag].score = current_score;
                  }
               }
               else {
                  pCandidate = m_Agenda.candidateItem();
                  pCandidate->copy( pGenerator );
                  pCandidate->score = current_score;
                  m_Agenda.pushCandidate();
               }
            }
         }assert(bDone);

         pGenerator = m_Agenda.generatorNext();  // next generator
      }

      if (nBest==1) {
         for ( tag=PENN_TAG_FIRST; tag<PENN_TAG_COUNT; ++tag )
            for ( last_tag=PENN_TAG_BEGIN; last_tag<PENN_TAG_COUNT; ++last_tag )
               if ( done_bigram[last_tag][tag]==word_index ) {
                  pCandidate = m_Agenda.candidateItem();
                  pCandidate->copy( &(best_bigram[last_tag][tag]) );
                  m_Agenda.pushCandidate();
               }
      }

      m_Agenda.nextRound(); // move round
      ++end_index;
      ++word_index;
      while ( end_index<length && word_ends->isset(end_index)==false ) ++end_index ;
   }

   // now generate output sentence
   // n-best list will be stored in array
   // from the addr vReturn
   TRACE("Outputing sentence");
   m_Agenda.sortGenerators();
   for (int k=0; k<nBest; ++k) {
      // clear
      vReturn[k].clear();
      if (out_scores!=0) 
         out_scores[k] = 0;
      // assign retval
      if (k<m_Agenda.generatorSize()) {
         pGenerator = m_Agenda.generator(k);
         for (j=0; j<pGenerator->size(); j++) {
            string temp = "";
            for (int l = pGenerator->getWordStart(j); l <= pGenerator->getWordEnd(j); ++l) {
               assert(sentence->at(l)!=" "); // [SPACE]
               temp += sentence->at(l);
            }
            vReturn[k].push_back( make_pair(temp, CTag(pGenerator->getTag(j)).str()) );
         }
         if (out_scores!=0)
            out_scores[k] = pGenerator->score;
      }
   }

   TRACE("Done, the highest score is: " << m_Agenda.bestGenerator()->score);
   TRACE("The total time spent: " << double(clock() - total_start_time)/CLOCKS_PER_SEC);
}

