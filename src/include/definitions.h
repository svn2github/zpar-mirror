/****************************************************************
 *                                                              *
 * definitions.h - the general definitions                      *
 *                                                              *
 * Author: Yue Zhang                                            *
 *                                                              *
 * Computing Laboratory, Oxford. 2006.10                        *
 *                                                              *
 ****************************************************************/

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/*===============================================================
 *
 * The things that should be included 
 *  
 *==============================================================*/

#include <cstdlib>                              // define NULL
#include <utility>                              // define pair
#include <cassert>                              // assert
#include <ctime>                                // clock(); CLK_TCK
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stack>
#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>

using namespace std;

/*===============================================================
 *
 * debugging utilities
 *  
 *==============================================================*/

// throw
#define THROW(x) { ostringstream __tmp_os; __tmp_os << x; throw(__tmp_os.str()); }

// error report tool
#define REPORT(x) { cerr << endl << "In " << __FILE__ << ", line " << __LINE__ << ": " << endl << x << endl; cerr.flush(); }

// trace tool
#ifdef DEBUG 
#define TRACE(x) { cout << x << endl; cout.flush(); }
#else
#define TRACE(x) 
#endif

// conditional compiling
#ifdef DEBUG
#define WHEN_DEBUGGING(x) x
#else
#define WHEN_DEBUGGING(x) 
#endif

/*===============================================================
 *
 * definitions about sentence structures 
 *  
 *==============================================================*/

#include "linguistics/sentence_string.h"

#endif