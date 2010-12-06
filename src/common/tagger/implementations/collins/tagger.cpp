// Copyright (C) University of Oxford 2010
/****************************************************************
 *                                                              *
 * agenda.cpp - the agenda pos tagger implementation.           *
 *                                                              *
 * Author: Yue Zhang                                            *
 *                                                              *
 * Computing Laboratory, Oxford. 2007.10                        *
 *                                                              *
 ****************************************************************/

#include "tagger.h"

using namespace TARGET_LANGUAGE;
using namespace TARGET_LANGUAGE::tagger;


/*---------------------------------------------------------------
 *
 * getLocalScore - get the local score for a word in sentence
 *
 * When bigram is needed from the beginning of sentence, the
 * -SENTENCE_BEGIN- tag and the empty word are used. 
 *
 * This implies that empty words should not be used in other 
 * situations. 
 *
 *--------------------------------------------------------------*/

void TARGET_LANGUAGE::CTagger::getLocalScore( CPackedScoreType<SCORE_TYPE, CTag::MAX_COUNT> &retval, const CStringVector *sentence, const CStateItem *item , const unsigned long &index ) {
   const static CWord g_emptyWord("");
   const static CScore<SCORE_TYPE> g_zeroScore;
   const CWord &word = m_Cache[index]; 
   const CWord &prev_word = index>0 ? m_Cache[index-1] : g_emptyWord; 
   const CWord &second_prev_word = index>1 ? m_Cache[index-2] : g_emptyWord;
   const CWord &next_word = index<m_Cache.size()-1 ? m_Cache[index+1] : g_emptyWord;
   const CWord &second_next_word = index<m_Cache.size()-2 ? m_Cache[index+2] : g_emptyWord;
   CTag prev_tag = item ? item->tag : CTag::SENTENCE_BEGIN;
   CTag second_prev_tag = (item && item->prev) ?  item->prev->tag : CTag::SENTENCE_BEGIN;

   static int i;
   static int word_size;
   static char letter;
   static bool bContainHyphen;
   static bool bContainNumber;
   static bool bContainCapitalLetter;
   static std::string prefix, suffix;

   m_weights->m_mapCurrentTag.getScore(retval, word, m_nScoreIndex) ; 
   m_weights->m_mapLastTagByTag.getScore(retval, prev_tag, m_nScoreIndex) ; 
   m_weights->m_mapLastTwoTagsByTag.getScore(retval, CTagSet<CTag, 2>(encodeTags(prev_tag, second_prev_tag)), m_nScoreIndex) ; 

   if (index>0) m_weights->m_mapTagByPrevWord.getScore(retval, prev_word, m_nScoreIndex) ; 
   if (index<m_Cache.size()-1) m_weights->m_mapTagByNextWord.getScore(retval, next_word, m_nScoreIndex) ; 
   if (index>1) m_weights->m_mapTagBySecondPrevWord.getScore(retval, second_prev_word, m_nScoreIndex) ; 
   if (index<m_Cache.size()-2) m_weights->m_mapTagBySecondNextWord.getScore(retval, second_next_word, m_nScoreIndex) ; 

   bContainHyphen = false;
   bContainNumber = false;
   bContainCapitalLetter = false;
   word_size = sentence->at(index).size();
   for ( i=0; i<word_size; ++i ) {
      letter = sentence->at(index)[i];
      if ( letter == '-' ) bContainHyphen = true;
      if ( letter >= '0' && letter <= '9' ) bContainNumber = true;
      if ( letter >= 'A' && letter <= 'Z' ) bContainCapitalLetter = true;
   }
   if (bContainHyphen) m_weights->m_mapContainHyphen.getScore(retval, 1, m_nScoreIndex);
   if (bContainNumber) m_weights->m_mapContainNumber.getScore(retval, 1, m_nScoreIndex);
   if (bContainCapitalLetter) m_weights->m_mapContainCapitalLetter.getScore(retval, 1, m_nScoreIndex);

   prefix.clear();
   prefix += sentence->at(index)[0]; m_weights->m_mapTagByPrefix.getScore(retval, prefix, m_nScoreIndex);
   if ( word_size>1 ) prefix += sentence->at(index)[1]; m_weights->m_mapTagByPrefix.getScore(retval, prefix, m_nScoreIndex);
   if ( word_size>2 ) prefix += sentence->at(index)[2]; m_weights->m_mapTagByPrefix.getScore(retval, prefix, m_nScoreIndex);
   if ( word_size>3 ) prefix += sentence->at(index)[3]; m_weights->m_mapTagByPrefix.getScore(retval, prefix, m_nScoreIndex);

   // the processing of suffix is tricky - we are storing the revert of suffix!
   suffix.clear();
   suffix += sentence->at(index)[word_size-1]; m_weights->m_mapTagBySuffix.getScore(retval, suffix, m_nScoreIndex);
   if ( word_size>1 ) suffix += sentence->at(index)[word_size-2]; m_weights->m_mapTagBySuffix.getScore(retval, suffix, m_nScoreIndex);
   if ( word_size>2 ) suffix += sentence->at(index)[word_size-3]; m_weights->m_mapTagBySuffix.getScore(retval, suffix, m_nScoreIndex);
   if ( word_size>3 ) suffix += sentence->at(index)[word_size-4]; m_weights->m_mapTagBySuffix.getScore(retval, suffix, m_nScoreIndex);
}

/*---------------------------------------------------------------
 *
 * updateScoreVector - update the score std::vector by input
 *                     this is used in training to adjust params
 *
 * Inputs: the tagged and the correct example
 *
 * Affects: m_bScoreModified, which leads to saveScores on destructor
 *
 *--------------------------------------------------------------*/

void TARGET_LANGUAGE::CTagger::updateScoreVector(const CTwoStringVector* tagged, const CTwoStringVector* correct, int round) {
   THROW("Not implemented");
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

void TARGET_LANGUAGE::CTagger :: updateLocalFeatureVector( SCORE_UPDATE method , const CTwoStringVector * sentence , int index , int round ) { 
   const CWord g_emptyWord("");
   const CScore<SCORE_TYPE> g_zeroScore;
   const CWord &word = m_Cache[index]; 
   const CWord &prev_word = index>0 ? m_Cache[index-1] : g_emptyWord; 
   const CWord &second_prev_word = index>1 ? m_Cache[index-2] : g_emptyWord;
   const CWord &next_word = index<m_Cache.size()-1 ? m_Cache[index+1] : g_emptyWord;
   const CWord &second_next_word = index<m_Cache.size()-2 ? m_Cache[index+2] : g_emptyWord;
   unsigned tag = CTag(sentence->at(index).second).code();
   CTag prev_tag = index>0 ? CTag(sentence->at(index-1).second) : CTag::SENTENCE_BEGIN;
   CTag second_prev_tag = index>1 ? CTag(sentence->at(index-2).second) : CTag::SENTENCE_BEGIN;

   static int i;
   static int word_size;
   static char letter;
   static bool bContainHyphen;
   static bool bContainNumber;
   static bool bContainCapitalLetter;
   static std::string prefix, suffix;

   SCORE_TYPE amount = method==eAdd ? 1 : -1;

   m_weights->m_mapCurrentTag.updateScore( word, tag, amount, round ) ;
   m_weights->m_mapLastTagByTag.updateScore( prev_tag, tag, amount, round ) ;
   m_weights->m_mapLastTwoTagsByTag.updateScore( CTagSet<CTag, 2>(encodeTags(prev_tag, second_prev_tag)), tag, amount, round ) ;

   if (index>0) m_weights->m_mapTagByPrevWord.updateScore( prev_word, tag, amount, round ) ;
   if (index<m_Cache.size()-1) m_weights->m_mapTagByNextWord.updateScore( next_word, tag, amount, round ) ;
   if (index>1) m_weights->m_mapTagBySecondPrevWord.updateScore( second_prev_word, tag, amount, round ) ;
   if (index<m_Cache.size()-2) m_weights->m_mapTagBySecondNextWord.updateScore( second_next_word, tag, amount, round ) ;

   bContainHyphen = false;
   bContainNumber = false;
   bContainCapitalLetter = false;
   word_size = sentence->at(index).first.size();
   for ( i=0; i<word_size; ++i ) {
      letter = sentence->at(index).first[i];
      if ( letter == '-' ) bContainHyphen = true;
      if ( letter >= '0' && letter <= '9' ) bContainNumber = true;
      if ( letter >= 'A' && letter <= 'Z' ) bContainCapitalLetter = true;
   }
   if (bContainHyphen) m_weights->m_mapContainHyphen.updateScore( 1, tag, amount, round );
   if (bContainNumber) m_weights->m_mapContainNumber.updateScore( 1, tag, amount, round );
   if (bContainCapitalLetter) m_weights->m_mapContainCapitalLetter.updateScore( 1, tag, amount, round );

   prefix.clear();
   prefix += sentence->at(index).first[0]; m_weights->m_mapTagByPrefix.updateScore( prefix, tag, amount, round );
   if ( word_size>1 ) prefix += sentence->at(index).first[1]; m_weights->m_mapTagByPrefix.updateScore( prefix, tag, amount, round );
   if ( word_size>2 ) prefix += sentence->at(index).first[2]; m_weights->m_mapTagByPrefix.updateScore( prefix, tag, amount, round );
   if ( word_size>3 ) prefix += sentence->at(index).first[3]; m_weights->m_mapTagByPrefix.updateScore( prefix, tag, amount, round );

   // the processing of suffix is tricky - we are storing the revert of suffix!
   suffix.clear();
   suffix += sentence->at(index).first[word_size-1]; m_weights->m_mapTagBySuffix.updateScore( suffix, tag, amount, round );
   if ( word_size>1 ) suffix += sentence->at(index).first[word_size-2]; m_weights->m_mapTagBySuffix.updateScore( suffix, tag, amount, round );
   if ( word_size>2 ) suffix += sentence->at(index).first[word_size-3]; m_weights->m_mapTagBySuffix.updateScore( suffix, tag, amount, round );
   if ( word_size>3 ) suffix += sentence->at(index).first[word_size-4]; m_weights->m_mapTagBySuffix.updateScore( suffix, tag, amount, round );

}

/*---------------------------------------------------------------
 *
 * loadScores - load scores from the file specified at constructor
 *              currently this uses database, but it can be modified
 *         
 * Affects: m_bScoreModified, clearing it. 
 *
 *--------------------------------------------------------------*/

void TARGET_LANGUAGE::CTagger::loadScores() {
   m_weights->loadScores();
   m_bScoreModified = false;
}

/*---------------------------------------------------------------
 *
 * saveScores - save scores back to database
 *
 * This method is called by the destructor is m_bScoreModified
 * is true.
 *
 *--------------------------------------------------------------*/

void TARGET_LANGUAGE::CTagger::saveScores() {
   m_weights->saveScores();
   m_bScoreModified = false;
}

/*---------------------------------------------------------------
 *
 * finishTraining - compute averaged scores and finish training
 *
 *--------------------------------------------------------------*/

void TARGET_LANGUAGE::CTagger::finishTraining() {
   m_weights->computeAverageFeatureWeights(m_nTrainingRound);
   saveScores();
}

/*===============================================================
 *
 * CTagger - the tagger for Chinese 
 *
 *==============================================================*/

/*---------------------------------------------------------------
 *
 * getPossibleTagsForWord - get possible pos
 *
 *--------------------------------------------------------------*/

unsigned long long TARGET_LANGUAGE::CTagger::getPossibleTagsForWord( const CWord &word ) {
   static unsigned long long possible_tags;
   possible_tags = m_TagDict ? m_TagDict->lookup(word) : 0LL;
   // when there is not an open tag for this word
   //if (possible_tags & m_opentags==0) possible_tags |= m_opentags;
   possible_tags |= m_opentags;
//#ifdef _ENGLISH_TAGS_H
//   possible_tags |= getPossibleTagsBySuffix( word.str() );
//   possible_tags |= PENN_TAG_MUST_SEE ;
//#endif
   assert(possible_tags!=0);
   return possible_tags;
}

/*---------------------------------------------------------------
 *
 * train - train the module auto
 *
 * Since we rely on external trainer, this method is empty
 *
 *--------------------------------------------------------------*/

bool TARGET_LANGUAGE::CTagger::train( const CTwoStringVector * correct ) {
   static int i;
   static CTwoStringVector tagged;

   static CStringVector sentence;
   static bool bDicOOV;
   static unsigned long long possible_tags, current_tag;

   UntagSentence( correct, &sentence );

   ++m_nTrainingRound;

   tag( &sentence , &tagged , 1 , NULL );
   if ( tagged != *correct ) {
      for (i=0; i<tagged.size(); ++i) {
         bDicOOV = false;
	 if (m_TagDict) {
	    possible_tags = getPossibleTagsForWord(correct->at(i).first);
	    current_tag = (1LL<<CTag(correct->at(i).second).code()) ;
	    if ( ( possible_tags & current_tag ) == 0 ) {
	       WARNING("dictionary does not have the example word " << correct->at(i).first << " with tag " << correct->at(i).second);
               bDicOOV = true;
	    }
	 }
         if (!bDicOOV) {
            updateLocalFeatureVector(eSubtract, &tagged, i, m_nTrainingRound);
            updateLocalFeatureVector(eAdd, correct, i, m_nTrainingRound);
            m_bScoreModified = true;
         }
      }
      return true;
   }

   return false;
}

/*---------------------------------------------------------------
 *
 * tag - assign POS tags to a sentence
 *
 * Returns: makes a new instance of CTwoStringVector 
 *
 *--------------------------------------------------------------*/

void TARGET_LANGUAGE::CTagger::tag( CStringVector * sentence , CTwoStringVector * vReturn , int nBest , double * out_scores ) {
   clock_t total_start_time = clock();;
   // initialise the return value, the agenda and cache
   TRACE("Initialising the tagging process...");
   const int length = sentence->size() ;
   static int index, temp_index, j;
   static unsigned tag, last_tag;
   static CPackedScoreType<SCORE_TYPE, CTag::MAX_COUNT> scores;
   const CStateItem *pGenerator;
   static CStateItem best_bigram[1<<CTag::SIZE][1<<CTag::SIZE];
   static int done_bigram[1<<CTag::SIZE][1<<CTag::SIZE];
   static unsigned long long possible_tags; // possible tags for a word
   static CStateItem temp;

   assert(vReturn!=NULL); 
   vReturn->clear();

   if (length+3>m_nMaxSentenceSize) {
      while (length+3>m_nMaxSentenceSize) {
         m_nMaxSentenceSize *= 2;
      }
      delete []stateindice;
      delete []stateitems;
      stateitems = new CStateItem[AGENDA_SIZE*m_nMaxSentenceSize];
      stateindice = new unsigned[m_nMaxSentenceSize];
   }
//   if (length>=MAX_SENTENCE_SIZE) {
//      WARNING("The input sentence is longer than the maximum " << MAX_SENTENCE_SIZE << "supported by the current setting."<<std::endl<<"Sentence: "<<sentence);
//      return;
//   }
//
   if (length == 0) {
      TRACE("Empty input.");
      return;
   }

   m_Cache.clear();
   for ( index=0; index<length; ++index )
      m_Cache.push_back(CWord(sentence->at(index)));

   // start tag
   TRACE("Tagging started"); 
   m_Agenda->clear();

   // the first step
   stateindice[0] = 0;
   stateindice[1] = 0;
   temp.prev = 0;
   possible_tags = getPossibleTagsForWord(m_Cache[0]);
   scores.reset();
   getLocalScore(scores, sentence, 0, 0);
   for (tag=0; tag<CTag::COUNT; ++tag) {
     if ( possible_tags & (1LL<<tag) ) {
        temp.tag = tag;
        temp.m_nScore = scores[tag] ;
        m_Agenda->insertItem(&temp);
     }
   }
   for (temp_index=0; temp_index<m_Agenda->size(); ++temp_index) {
      stateitems[stateindice[1]] = *(m_Agenda->item(temp_index));
      ++stateindice[1];
   }
   stateindice[2] = stateindice[1];

   if (nBest == 1) {
//      for ( tag=0; tag<CTag::COUNT; ++tag )
//         for ( last_tag=0; last_tag<CTag::COUNT; ++last_tag )
//            done_bigram[last_tag][tag] = -1;
      memset(done_bigram, 0, (1<<CTag::SIZE)*(1<<CTag::SIZE)*sizeof(int));
   }

   for ( index=1; index<length; index++ ) {

      m_Agenda->clear();
      for ( j=stateindice[index-1]; j<stateindice[index]; ++j ) {

         pGenerator = &stateitems[j];
         last_tag = pGenerator->tag;

         // lookup dictionary
         possible_tags = getPossibleTagsForWord(m_Cache[index]);
         scores.reset();
         getLocalScore(scores, sentence, pGenerator, index);

         for ( tag=CTag::FIRST; tag<CTag::COUNT; ++tag ) {
            if ( possible_tags & (1LL<<tag) ) {
               temp.prev = pGenerator; temp.tag = tag;
//               temp.m_nScore = pGenerator->m_nScore + getLocalScore(sentence, &temp, index); 
               temp.m_nScore = pGenerator->m_nScore + scores[tag]; 
               if (nBest==1) {
                  if ( done_bigram[last_tag][tag] != index || temp.m_nScore > best_bigram[last_tag][tag].m_nScore ) {
                     done_bigram[last_tag][tag] = index;
                     best_bigram[last_tag][tag] = temp ;
                  }
               }
               else {
                  m_Agenda->insertItem(&temp);
               }
            }
         }//assert(bDone);

      }//for pGenerator

      if (nBest==1) {
         for ( tag=CTag::FIRST; tag<CTag::COUNT; ++tag ) {
            for ( last_tag=0; last_tag<CTag::COUNT; ++last_tag ) {
               if ( done_bigram[last_tag][tag]==index ) {
                  m_Agenda->insertItem(&best_bigram[last_tag][tag]);
               }
            }
         }
      }

      for (temp_index=0; temp_index<m_Agenda->size(); ++temp_index) {
         stateitems[stateindice[index+1]] = *(m_Agenda->item(temp_index));
         ++stateindice[index+1];
      }
      stateindice[index+2] = stateindice[index+1];
//      TRACE("The time for iteration" << index << ":was " << double(clock() - total_start_time)/CLOCKS_PER_SEC);
   }

   // outout 
   TRACE("Outputing sentence");
   m_Agenda->sortItems();
   for ( temp_index = 0 ; temp_index < std::min(nBest, m_Agenda->size()) ; ++ temp_index ) {
      vReturn[temp_index].resize(length); 
      pGenerator = m_Agenda->item(temp_index);
      for (j=0; j<length; ++j) { 
         vReturn[temp_index][length-j-1].first = sentence->at(length-j-1);
         vReturn[temp_index][length-j-1].second = CTag(pGenerator->tag).str();
         pGenerator = pGenerator->prev;
      }
      assert(pGenerator==0);
      if (out_scores)
         out_scores[temp_index] = m_Agenda->item(temp_index)->m_nScore;
   }
   TRACE("Done, the highest score is: " << m_Agenda->item(0)->m_nScore);
   TRACE("The total time spent: " << double(clock() - total_start_time)/CLOCKS_PER_SEC);
}


