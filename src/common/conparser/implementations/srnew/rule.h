#ifndef _COMMON_CON_PARSER_RULE
#define _COMMON_CON_PARSER_RULE

/*===============================================================
 *
 * Rules
 *
 *==============================================================*/

#include "tuple2.h"

namespace TARGET_LANGUAGE {

namespace conparser {

class CRule {

protected:
   CHashMap< CTuple2<CConstituent, CConstituent>, vector<CAction> > *m_mapBinaryRules;
   CHashMap< CConstituent, vector<CAction> > *m_mapUnaryRules;
   const vector< vector<CConstituent> > *m_LexConstituents;

public:
   CRule() : m_mapBinaryRules(0), m_mapUnaryRules(0), m_LexConstituents(0) {}
   virtual ~CRule() {
      if (m_mapBinaryRules) 
         delete m_mapBinaryRules; 
      if (m_mapUnaryRules) 
         delete m_mapUnaryRules;
   }

public:
   void getActions(const CStateItem &item, vector<CAction> &actions) {
      assert(!item.IsTerminated());
      actions.clear();

      static CAction action;
      const unsigned &stack_size = item.stack.size();
      const unsigned &length = item.sent->size();

      // finish parsing
      if (item.IsComplete()) { 
         action.encodeReduceRoot();
         actions.push_back(action);
      }
      // shift
      if ( item.current_word < length ) { 
         // do not shift for head right tmp item
         if (stack_size>0 && 
             item.nodes[item.stack.back()].temp &&
             item.nodes[item.stack.back()].head_left()==false) {
         }
         else {
             getShiftRules(item, actions);
         }
      }
      // reduce bin
      if ( stack_size > 1 ) {
         getBinaryRules(item, actions);
      }
      // reduce unary
      if ( stack_size && item.unary_reduce<UNARY_MOVES &&
           !item.nodes[item.stack.back()].temp ) {
         getUnaryRules(item, actions);
      }
   }

protected:
   void getShiftRules(const CStateItem &item, vector<CAction> &actions) {
      static CAction action;
      // the rules onto lexical item constituents
      if (m_LexConstituents) {
         for (int i=0; i<m_LexConstituents->at(item.current_word).size(); ++i) {
            action.encodeShift(m_LexConstituents->at(item.current_word).at(i).code());
            actions.push_back(action);
         }
         return;
      }
      // the default rules
      action.encodeShift();
      actions.push_back(action);
   }
   void getBinaryRules(const CStateItem &item, vector<CAction> &actions) {
      static CAction action;
      const unsigned &stack_size = item.stack.size();
      const CStateNode &right = item.nodes[item.stack.back()];
      const CStateNode  &left = item.nodes[item.stack[stack_size-2]];
      // specified rules
      if (m_mapBinaryRules) {
         static CTuple2<CConstituent, CConstituent> tuple2;
         tuple2.refer(&(left.constituent), &(right.constituent));
         const vector<CAction> &result = m_mapBinaryRules->find(tuple2, vector<CAction>());
         actions.insert(actions.end(), result.begin(), result.end());
         return;
      }
      // the normal method
      const bool prev_temp = stack_size>2 ? item.nodes[item.stack[stack_size-3]].temp:false;
      for (unsigned long constituent=CConstituent::FIRST; constituent<CConstituent::COUNT; ++constituent) {
         for (unsigned i=0; i<=1; ++i) {
            for (unsigned j=0; j<=1; ++j) {
	       const bool &head_left = static_cast<bool>(i);
               const bool &temporary = static_cast<bool>(j);
               assert( item.stack.back() == item.context->s0 );
               assert( item.stack[stack_size-2] == item.context ->s1 );
               const CWord &head_wd = item.sent->at( (head_left?left:right).lexical_head );
               if ( ( !left.temp || !right.temp ) &&
                     ( !(stack_size==2 && item.current_word==item.sent->size()) || !temporary ) &&
                     ( !(stack_size==2) || (!temporary||head_left) ) &&
                     ( !(prev_temp && item.current_word==item.sent->size()) || !temporary ) &&
                     ( !(prev_temp) || (!temporary||head_left) ) &&
                     ( !left.temp || (head_left&&constituent==left.constituent.code()) ) &&
                     ( !right.temp || (!head_left&&constituent==right.constituent.code()) ) //&&
//                     ( !temporary || CConstituent::canBeTemporary(constituent) ) 
                 ) {
                        action.encodeReduce(constituent, false, head_left, temporary);
                        actions.push_back(action);
                  }
               } // for j
            } // for i
         } // for constituent
   }

   void getUnaryRules(const CStateItem &item, vector<CAction> &actions) {
      const CStateNode &child = item.nodes[item.stack.back()];
      // input rule list
      if (m_mapUnaryRules) {
         const vector<CAction> &result = m_mapUnaryRules->find(child.constituent, vector<CAction>());
         actions.insert(actions.end(), result.begin(), result.end());
         return;
      }
      // the normal rules
      static CAction action;
      const unsigned &stack_size = item.stack.size();
      for (unsigned long constituent=CConstituent::FIRST; constituent<CConstituent::COUNT; ++constituent){
         assert(item.context->s0==item.stack.back());
         if (constituent != child.constituent.code()) { 
            action.encodeReduce(constituent, true, false, false);
            actions.push_back(action);
         }
      } // for constituent
   }

public:
   void loadRules(ifstream &is) {
      // initialization
      if (!is.is_open()) {
         return;
      }
      static string s;
      getline(is, s);
      // binary rules
      ASSERT(s=="Binary rules" or s=="Free binary rules", "Binary rules not found from model.");
      if (s=="Free binary rules")
         return;
      ASSERT(!m_mapBinaryRules, "Binary rules already loaded");
      m_mapBinaryRules = new CHashMap< CTuple2<CConstituent, CConstituent>, vector<CAction> >;
      is >> (*m_mapBinaryRules);
      // unary rules
      getline(is, s);
      ASSERT(s=="Unary rules" or s=="Free unary rules", "Unary rules not found from model.");
      if (s=="Free unary rules")
         return;
      ASSERT(!m_mapUnaryRules, "Unary rules already loaded");
      m_mapUnaryRules = new CHashMap< CConstituent, vector<CAction> >;
      is >> (*m_mapUnaryRules);
   }

   void saveRules(ofstream &os) {
      ASSERT(os.is_open(), "Cannot save rules possibly because the output file is not accessible.");
      if (m_mapBinaryRules) {
         os << "Binary rules" << endl;
         os << (*m_mapBinaryRules);
      }
      else {
         os << "Free binary rules" << endl;
       }
      if (m_mapUnaryRules) {
         os << "Unary rules" << endl;
         os << (*m_mapUnaryRules);
      }
      else { os << "Free unary rules" << endl; }
      os << endl;
   }

   void LoadBinaryRules(ifstream &is) {
      if (!is.is_open()) {
         THROW("Loading binary rules failed possibly becaues file not exists.");
      }
      m_mapBinaryRules = new CHashMap< CTuple2<CConstituent, CConstituent>, vector<CAction> >;
      is >> (*m_mapBinaryRules);
   }

   void LoadUnaryRules(ifstream &is) {
      if (!is.is_open()) {
         THROW("Loading unary rules failed possibly becaues file not exists.");
      }
      m_mapUnaryRules = new CHashMap< CConstituent, vector<CAction> >;
      is >> (*m_mapUnaryRules);
   }

   void SetLexConstituents(const vector<vector<CConstituent> > &con_input) {
      m_LexConstituents = &con_input;
   }
   void UnsetLexConstituents() {
      m_LexConstituents=0;
   }

};

} // namespace conparser

}

#endif
