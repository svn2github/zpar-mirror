/****************************************************************
 *                                                              *
 * deplabeler.cpp - the naive labeler implementation            *
 *                                                              *
 * Author: Yue Zhang                                            *
 *                                                              *
 * Computing Laboratory, Oxford. 2007.12                        *
 *                                                              *
 ****************************************************************/

#include "deplabeler.h"
#include "weight.h"

using namespace TARGET_LANGUAGE;
using namespace TARGET_LANGUAGE::deplabeler;

const CWord g_emptyWord("");
const CTaggedWord<CTag, TAG_SEPARATOR> g_emptyTaggedWord("", CTag::NONE);
const CTag g_noneTag(CTag::NONE);
const CTag g_beginTag(CTag::SENTENCE_BEGIN);
//const CScore<SCORE_TYPE> g_zeroScore;

#define cast_weights static_cast<CWeight*>(m_weights)
#define refer_or_allocate_tuple3(x, o1, o2, o3) { if (amount == 0) x.refer(o1, o2, o3); else x.allocate(o1, o2, o3); }
#define refer_or_allocate_tuple4(x, o1, o2, o3, o4) { if (amount == 0) x.refer(o1, o2, o3, o4); else x.allocate(o1, o2, o3, o4); }

/*===============================================================
 *
 * CDepLabeler - the deplabeler for TARGET_LANGUAGE 
 *
 *==============================================================*/

/*---------------------------------------------------------------
 *
 * getOrUpdateArcLabelScore - the label score of dependency link
 *
 *---------------------------------------------------------------*/

inline SCORE_TYPE CDepLabeler::getOrUpdateArcLabelScore( const int &head_index, const int &dep_index, const CDependencyLabel &label, SCORE_TYPE amount, int round) {
   const CTaggedWord<CTag, TAG_SEPARATOR> &head_word_tag = m_lCache[head_index];
   const CTaggedWord<CTag, TAG_SEPARATOR> &dep_word_tag = m_lCache[dep_index];
   const CWord &head_word = static_cast<const CWord&>(head_word_tag);
   const CWord &dep_word = static_cast<const CWord&>(dep_word_tag);
   const CTag &head_tag = head_word_tag.tag;
   const CTag &dep_tag = dep_word_tag.tag;

   const CTag &head_tag_l = ( head_index > 0 ) ? m_lCache[ head_index-1 ].tag : g_beginTag ;
   const CTag &head_tag_r = ( head_index+1 < m_lCache.size() ) ? m_lCache[ head_index+1 ].tag : g_beginTag ;
   const CTag &dep_tag_l = ( dep_index > 0 ) ? m_lCache[ dep_index-1 ].tag : g_beginTag ;
   const CTag &dep_tag_r = ( dep_index+1 < m_lCache.size() ) ? m_lCache[ dep_index+1 ].tag : g_beginTag ;

   static CTagSet<CTag, 3> head_tag_lm, head_tag_mr, head_tag_lmr, dep_tag_lm, dep_tag_mr, dep_tag_lmr;

   head_tag_lm.load(encodeTags(head_tag_l, head_tag, g_noneTag));
   head_tag_mr.load(encodeTags(g_noneTag, head_tag, head_tag_r));
   head_tag_lm.load(encodeTags(head_tag_l, head_tag, head_tag_r));
   dep_tag_lm.load(encodeTags(dep_tag_l, dep_tag, g_noneTag));
   dep_tag_mr.load(encodeTags(g_noneTag, dep_tag, dep_tag_r));
   dep_tag_lm.load(encodeTags(dep_tag_l, dep_tag, dep_tag_r));

   static CTuple3<CWord, CDependencyLabel, int> word_label_int;
   static CTuple3<CTag, CDependencyLabel, int> tag_label_int;
   static CTuple3<CTaggedWord<CTag, TAG_SEPARATOR>, CDependencyLabel, int> taggedword_label_int;
   static CTuple3<CTagSet<CTag, 3>, CDependencyLabel, int> tagset3_label_int;

#include "templates/labeled.h"

   const int link_distance = getLinkSizeAndDirection(head_index, dep_index) ;
   const int link_direction = getLinkDirectionEncode(head_index, dep_index);

   static SCORE_TYPE retval;

   retval = 0;

   getOrUpdateLabeledScoreTemplate(link_distance);
   getOrUpdateLabeledScoreTemplate(link_direction);

   return retval;
}

/*---------------------------------------------------------------
 *
 * initCaches - initialize cache
 *
 *--------------------------------------------------------------*/

void CDepLabeler::initCaches( const CDependencyTree *sentence ) {
}

/*---------------------------------------------------------------
 *
 * work - the working process shared by training and parsing
 *
 * Returns: makes a new instance of CLabeledDependencyTree 
 *
 *--------------------------------------------------------------*/

void CDepLabeler::work( const CDependencyTree &sentence , CLabeledDependencyTree *retval , const CLabeledDependencyTree *correct , const unsigned long &index ) {

   const unsigned &length = sentence.size() ; 
   static unsigned temp_i ;

   assert(length<MAX_SENTENCE_SIZE);

   // initialise word cache
   m_lCache.clear();
   for ( temp_i=0; temp_i<length; temp_i++ )
      m_lCache.push_back( CTaggedWord<CTag, TAG_SEPARATOR>(sentence[temp_i].word , sentence[temp_i].tag) );

   if (m_bTrain) {
   } 

}

/*---------------------------------------------------------------
 *
 * label - do dependency parsing to a sentence
 *
 *--------------------------------------------------------------*/

void CDepLabeler::label( const CDependencyTree &sentence , CLabeledDependencyTree *retval ) {

   for (unsigned long i=0; i<sentence.size(); ++i)
      work( sentence, retval, 0, i ) ;

}

/*---------------------------------------------------------------
 *
 * train - train the models with an example
 *
 *---------------------------------------------------------------*/

void CDepLabeler::train( const CLabeledDependencyTree &correct ) {

   static CDependencyTree parse ;
   static CLabeledDependencyTree label ; 

   UnlabelSentence( &correct, &parse ) ;

   // The following code does update for each processing stage

   for (unsigned long i=0; i<correct.size(); ++i) {
      ++m_nTrainingRound ;
      work( parse , &label , &correct , i ) ; 
   }

};


