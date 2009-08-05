/****************************************************************
 *                                                              *
 * train.cpp - the training of the general constituent parser.  *
 *                                                              *
 * Author: Yue Zhang                                            *
 *                                                              *
 * Computing Laboratory, Oxford. 2007.8                         *
 *                                                              *
 ****************************************************************/

#include "definitions.h"
#include "conparser.h"
#include "reader.h"
#include "writer.h"

#include <cstring>

using namespace TARGET_LANGUAGE;

/*===============================================================
 *
 * auto_train - train by the parser itself, black-box training
 *
 *===============================================================*/

void auto_train(const string &sOutputFile, const string &sFeatureFile) {

   cout << "Training iteration is started ... " << endl ; cout.flush();

   CConParser parser(sFeatureFile, true);

   ifstream is(sOutputFile.c_str());
   assert(is.is_open());

   CSentenceParsed ref_sent; 

   int nCount=0;
   
   is >> ref_sent;
   while( ! ref_sent.empty() ) {
      cout << "Sentence " << nCount << " ... ";
      nCount ++;
      parser.train( ref_sent, nCount );
      cout << "done." << endl;
      is >> ref_sent;
   }

   parser.finishtraining();

   cout << "Done. " << endl;

}

/*===============================================================
 *
 * main
 *
 *==============================================================*/

int main(int argc, char* argv[]) {

   const string hint = " training_input_file feature_file number_of_iterations \n\
";

   if (argc < 4) {
      cout << "\nUsage: " << argv[0] << hint << endl;
      return 1;
   }

   if (argc>4) {
      for (int i=4; i<argc; i++) {
         if (argv[i][0]!='-') { cout << "\nUsage: " << argv[0] << hint << endl ; return 1; }
         switch (argv[i][1]) {
            default:
               cout << "\nUsage: " << argv[0] << hint << endl ;
               return 1;
         }
      }
   }

   int training_rounds = atoi(argv[3]);
   if (training_rounds < 1) {
      cout << "\nThe number of training iterations is incorrect. " << endl;
      return 1;
   }

   cout << "Training started." << endl;
   int time_start = clock();
   for (int i=0; i<training_rounds; ++i) {
      try {
         auto_train(argv[1], argv[2]);
      }
      catch(const string &e) {
         cerr << "Exception: " << e << " Stop." << endl;
         exit(1);
      }
   }
   cout << "Training has finished successfully. Total time taken is: " << double(clock()-time_start)/CLOCKS_PER_SEC << endl;

   return 0;

}

