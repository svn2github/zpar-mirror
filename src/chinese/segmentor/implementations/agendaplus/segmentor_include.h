#include "agenda.h"
#include "pair_stream.h"
#include "linguistics/word_tokenized.h"
#include "learning/perceptron/hashmap_score_nnf.h"

#ifdef NO_NEG_FEATURE
#define CScoreMapType CScoreMapNoNegFeatures
#else
#define CScoreMapType CScoreMap
#endif

typedef double SCORE_TYPE;

/*===============================================================
 *
 * Configuration constants
 *
 *==============================================================*/
namespace chinese {

namespace segmentor {
//
// Beam segmentor specific item
//
const int BEAM_SIZE = 16;
//
// All segmentor must define the following
//
const int MAX_SENTENCE_SIZE = 512; // restrict size of sentences 
const int MAX_WORD_SIZE = 128; // restrict size of words

//const int LENGTH_MAX_BITS = 3;
const int LENGTH_MAX_BITS = 4;
const int LENGTH_MAX = 1<<LENGTH_MAX_BITS;

};
};

