#ifndef _DEPPARSER_INCLUDE_H
#define _DEPPARSER_INCLUDE_H

#include "bigram.h"
#include "linguistics/word_tokenized.h"
#include "tags.h"
#include "linguistics/taggedword.h"
#include "agenda.h"
#include "pair_stream.h"

#include "learning/perceptron/score.h"
#include "learning/perceptron/hashmap_score.h"

typedef CBigram< CTaggedWord<TARGET_LANGUAGE::CTag> > CTwoTaggedWords; 

typedef CDependencyTree CSentenceParsed;

namespace chinese {

namespace depparser {

#include "macros.h"

}

}

#endif