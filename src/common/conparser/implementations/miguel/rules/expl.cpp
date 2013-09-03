 //"S|SQ|SINV < (NP=target <+(NP) EX)"
     bool buildExpl1() {
        if (node.constituent==PENN_CON_S || node.constituent==PENN_CON_SQ || node.constituent==PENN_CON_SINV) {
     	   CStateNodeList* childsS=node.m_umbinarizedSubNodes;
     	   while (childsS!=0) {
     		   const CStateNode* npTarg=childsS->node;
     		   if (npTarg->constituent==PENN_CON_NP && (!isLinked(&node, npTarg))){
     		   CStateNodeList* childsNp=npTarg->m_umbinarizedSubNodes;

				while(childsNp!=0) {
					const CStateNode* npChild=childsNp->node;//zero or more REMEMBER, this is why we can have this case.
					if ((*words)[npChild->lexical_head].tag.code()==PENN_TAG_EX) {
						CDependencyLabel* label=new CDependencyLabel(STANFORD_DEP_EXPL);
						if (buildStanfordLink(label, npTarg->lexical_head, node.lexical_head)){
    		      				addLinked(&node,npTarg);
    		      				//std::cout<<"expl"<<" (head: "<<node.lexical_head<<")"<<"(npTarg->lexical_head<<")\n";
    		      				return true;
    		      			}
					}
					else if (npChild->constituent==PENN_CON_NP) { //zero or more REMEMBER, this is why we have the thing shown above.
						CStateNodeList* npsChain=new CStateNodeList();
      						  //std::cout<<"findingchain\n";
      					findChainTargetPos(PENN_CON_NP,PENN_TAG_EX,npChild,npsChain);
      					if (npsChain->node==0) {
      						npsChain->clear();
      					        npsChain=0;
      					}
						while(npsChain!=0) {
							const CStateNode* npCh=npsChain->node;
							if ((*words)[npChild->lexical_head].tag.code()==PENN_TAG_EX) {
								CDependencyLabel* label=new CDependencyLabel(STANFORD_DEP_EXPL);
								if (buildStanfordLink(label, npTarg->lexical_head, node.lexical_head)){
    		      						addLinked(&node,npTarg);
    		      						//std::cout<<"expl"<<" (head: "<<node.lexical_head<<")"<<"(npTarg->lexical_head<<")\n";
    		      						return true;
								}
							}
							npsChain=npsChain->next;
						}
					}
     			}
				childsNp=childsNp->next;
     		   }
     		   childsS=childsS->next;
     	   }
        }
     return false;
     }
