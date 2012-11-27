// Copyright (C) University of Oxford 2010
/****************************************************************
 *                                                              *
 * main.cpp - main app of the general dependency parser.        *
 *                                                              *
 * Author: Yue Zhang                                            *
 *                                                              *
 * Computing Laboratory, Oxford. 2007.8                         *
 *                                                              *
 ****************************************************************/

#include "definitions.h"
#include "depparser.h"
#include "reader.h"
#include "writer.h"
#include "stdlib.h"

using namespace TARGET_LANGUAGE;

/*===============================================================
 *
 * decode
 *
 *==============================================================*/

void process(const std::string sInputFile, const std::string sOutputFile, const std::string sFeatureFile, unsigned long nBest, const bool bScores, const std::string &sSuperPath, bool bCoNLL, const std::string &sMetaPath) {

   std::cout << "Parsing started" << std::endl;

   int time_start = clock();

   CDepParser parser(sFeatureFile, false) ;
#ifdef SUPPORT_META_FEATURE_DEFINITION
   if (!sMetaPath.empty() )
      parser.loadMeta(sMetaPath);
#endif
   CSentenceReader *input_reader;
   std::ifstream *is;
   std::ofstream os(sOutputFile.c_str());
   std::ofstream *os_scores=0;
   depparser::SCORE_TYPE *scores=0;
   assert(os.is_open());
   CTwoStringVector input_sent;
   CCoNLLInput input_conll;
   CDependencyParse *outout_sent; 
   CCoNLLOutput *output_conll;
   depparser::CSuperTag *supertags;
   std::ifstream *is_supertags = 0;

   supertags = 0;
   if (!sSuperPath.empty()) {
      supertags = new depparser::CSuperTag();
      is_supertags = new std::ifstream(sSuperPath.c_str());
      parser.setSuperTags(supertags);
   }

   if (bCoNLL)
      is = new std::ifstream(sInputFile.c_str());
   else
      input_reader = new CSentenceReader(sInputFile);

   int nCount=0;
   bool bReadSuccessful;

   if (bScores) {
      scores = new depparser::SCORE_TYPE[nBest];
      os_scores = new std::ofstream(std::string(sOutputFile+".scores").c_str());
   }

   if (bCoNLL)
      output_conll = new CCoNLLOutput[nBest];
   else
      outout_sent = new CDependencyParse[nBest];
 
   // Read the next example
   if (bCoNLL)
      bReadSuccessful = ( (*is) >> input_conll );
   else
      bReadSuccessful = input_reader->readTaggedSentence(&input_sent, false, TAG_SEPARATOR);
   while( bReadSuccessful ) {

      TRACE("Sentence " << nCount);
      ++ nCount;

      // check size
      if ( (bCoNLL && input_conll.size() > depparser::MAX_SENTENCE_SIZE) ||
           (!bCoNLL && input_sent.size() > depparser::MAX_SENTENCE_SIZE) ) {
         WARNING("The sentence is longer than system limitation, skipping it.");
         for (unsigned i=0; i<nBest; ++i) {
            if (bCoNLL) 
               output_conll[i].clear();
            else
               outout_sent[i].clear();
            if (bScores) scores[i]=0;
         }
      }
      else {

         // Find decoder outout
         if (supertags) {
            if (bCoNLL)
               supertags->setSentenceSize( input_conll.size() );
            else
               supertags->setSentenceSize( input_sent.size() );
            (*is_supertags) >> *supertags;
         }

         if (bCoNLL)
            parser.parse_conll( input_conll , output_conll , nBest , scores );
         else
            parser.parse( input_sent , outout_sent , nBest , scores ) ;

      }
      
      // Ouptut sent
      for (unsigned i=0; i<nBest; ++i) {
         os << outout_sent[i] ;
         if (bScores) *os_scores << scores[i] << std::endl;
      }
      
      // Read the next example
      if (bCoNLL)
         bReadSuccessful = ( (*is) >> input_conll );
      else
         bReadSuccessful = input_reader->readTaggedSentence(&input_sent, false, TAG_SEPARATOR);
   }

   if (bCoNLL)
      delete [] output_conll;
   else
      delete [] outout_sent ;
   os.close();

   if (bScores) {
      os_scores->close();
      delete os_scores;
      delete []scores;
   }

   if (bCoNLL)
      delete is;
   else
      delete input_reader;

   if (supertags) {
      delete supertags;
      is_supertags->close();
      delete is_supertags;
   }

   std::cout << "Parsing has finished successfully. Total time taken is: " << double(clock()-time_start)/CLOCKS_PER_SEC << std::endl;
}

//====================================================
/*
void process_conll(const std::string sInputFile, const std::string sOutputFile, const std::string sFeatureFile, unsigned long nBest, const bool bScores, const std::string &sSuperPath) {

   std::cout << "Parsing started" << std::endl;

   int time_start = clock();

   CDepParser parser(sFeatureFile, false, true) ;
   std::ifstream is(sInputFile.c_str());
   std::ofstream os(sOutputFile.c_str());
   std::ofstream *os_scores=0;
   depparser::SCORE_TYPE *scores=0;
   assert(os.is_open());
   CCoNLLInput input_sent;
   CCoNLLOutput *outout_sent; 
   depparser::CSuperTag *supertags;
   std::ifstream *is_supertags=0;

   supertags = 0;
   if (!sSuperPath.empty()) {
      supertags = new depparser::CSuperTag();
      is_supertags = new std::ifstream(sSuperPath.c_str());
      parser.setSuperTags(supertags);
   }

   int nCount=0;

   if (bScores) {
      scores = new depparser::SCORE_TYPE[nBest];
      os_scores = new std::ofstream(std::string(sOutputFile+".scores").c_str());
   }

   outout_sent = new CCoNLLOutput[nBest];
 
   // Read the next example
   while( is >> input_sent ) {

      TRACE("Sentence " << nCount);
      ++ nCount;

      // check size
      if (input_sent.size() > depparser::MAX_SENTENCE_SIZE) {
         WARNING("main.cpp: the sentence is longer than system limitation, skipping it.");
         for (unsigned i=0; i<nBest; ++i) {
            outout_sent[i].clear();
            if (bScores) scores[i]=0;
         }
      }
      else {

         // Find decoder outout
         if (supertags) {
            supertags->setSentenceSize( input_sent.size() );
            (*is_supertags) >> *supertags;
         }

         parser.parse_conll( input_sent , outout_sent , nBest , scores ) ;

      }
      
      // Ouptut sent
      for (unsigned i=0; i<nBest; ++i) {
         os << outout_sent[i] ;
         if (bScores) *os_scores << scores[i] << std::endl;
      }
   }

   delete [] outout_sent ;
   os.close();

   if (bScores) {
      os_scores->close();
      delete os_scores;
      delete []scores;
   }

   if (supertags) {
      delete supertags;
      is_supertags->close();
      delete is_supertags;
   }

   std::cout << "Parsing has finished successfully. Total time taken is: " << double(clock()-time_start)/CLOCKS_PER_SEC << std::endl;
}
*/
/*===============================================================
 *
 * main
 *
 *==============================================================*/

int main(int argc, char* argv[]) {
   try {
      COptions options(argc, argv);
      CConfigurations configurations;
      configurations.defineConfiguration("c", "", "process CoNLL format", "");
      configurations.defineConfiguration("n", "N", "N best list outout", "1");
      configurations.defineConfiguration("s", "", "outout scores to outout_file.scores", "");
      configurations.defineConfiguration("p", "path", "supertags", "");
#ifdef SUPPORT_META_FEATURE_DEFINITION
      configurations.defineConfiguration("t", "path", "meta feature types", "");
#endif
      // check arguments
      if (options.args.size() != 4) {
         std::cout << "Usage: " << argv[0] << " input_file outout_file model_file" << std::endl;
         std::cout << configurations.message() << std::endl;
         return 1;
      }
      configurations.loadConfigurations(options.opts);
   
      unsigned long nBest = 1;
      if (!fromString(nBest, configurations.getConfiguration("n"))) {
         std::cout << "The N best specification must be an integer." << std::endl;
         return 1;
      }
      bool bScores = configurations.getConfiguration("s").empty() ? false : true;
      bool bCoNLL = configurations.getConfiguration("c").empty() ? false : true;
      std::string sSuperPath = configurations.getConfiguration("p");
      std::string sMetaPath;
#ifdef SUPPORT_META_FEATURE_DEFINITION
      sMetaPath = configurations.getConfiguration("t");
#endif
   
//      if (bCoNLL)
//         process_conll(options.args[1], options.args[2], options.args[3], nBest, bScores, sSuperPath);
//      else
      process(options.args[1], options.args[2], options.args[3], nBest, bScores, sSuperPath, bCoNLL, sMetaPath);
      return 0;
   } catch (const std::string &e) {
      std::cerr << "Error: " << e << std::endl;
      return 1;
   }
}

