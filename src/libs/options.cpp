/****************************************************************
 *                                                              *
 * options.cpp - command line options                           *
 *                                                              *
 * Author: Yue Zhang                                            *
 *                                                              *
 * Computing Laboratory, Oxford. 2009.7                         *
 *                                                              *
 ****************************************************************/

#include "options.h"

COptions::COptions(int argc, char* args[], const string& char_options, const vector<string>& str_options) {
   for ( unsigned i=0; i<argc; ++i ) {
      string arg( args[i] );
      string name;
      string value;
      if ( arg[0] == '-' ) {
         if ( arg.size() == 1 ) {
            valid = false;
            err = "invalid option: -";
            return;
         }
         if ( arg[1] == '-' ) { // --name value
            if ( arg.size() == 2 ) {
               valid = false;
               err = "invalid option: --";
               return;
            }
            name = arg.substr(2);
            value = args[++i];
         }
         else { // -[name]value
            name = arg[1];
            value = arg.substr(2);
         }
         opts.push_back(make_pair(name, value));
      }
      else {
         (*this).args.push_back(arg);
      }
   } // for
   valid = true;
}
      
void CConfigurations::defineConfiguration(const string &name, const string &argument, const string &type, const string &message) {
   // check for duplication
   for (int i=0; i<configurations.size(); ++i) {
      if (configurations[i].name == name)
         THROW("duplicate definitions of configuration item: " << name);
   }
   // ensure none empty
   if (name.empty())
      THROW("configuration name is not allowed to be empty in definition");
   // add item
   configurations.push_back(CCon(name, argument, type, message));
}

string CConfigurations::loadConfigurations(const vector< pair< string, string > > &options) {
   vector< string > ignored;
   bool bFound;
   // insert define
   for (int i=0; i<options.size(); ++i) {
      bFound = false;
      for (int j=0; j<configurations.size(); ++j) {
         if (configurations[j].name == options[i].first) {
            if (configurations[i].argument.empty()) {
               if (!options[i].second.empty()) THROW("configuration "<<configurations[i].name<<" does not take any arguments, but "<<options[i].second<<" given");
               configurations[j].value = "true";
            }
            else
               configurations[j].value = options[i].second;
            bFound = true;
         }
      }
      if (!bFound) {
         ignored.push_back(options[i].first);
      }
   }
   // report 
   ostringstream os;
   if (ignored.size()) {
      os << "The following options are ignored: ";
      for (int i=0; i<ignored.size(); ++i) 
         os << ignored[i] << " ";
   }
   return os.str();
}

string CConfigurations::getConfiguration(const string &name) {
   for (int i=0; i<configurations.size(); ++i) {
      if (configurations[i].name == name)
         return configurations[i].value;
   }
   THROW("configuration required but not defined: " << name);
}

string CConfigurations::message() {
   ostringstream os;
   os << "Options: " << endl;
   for (int i=0; i<configurations.size(); ++i) {
      os << " ";
      if (configurations[i].name.size()==1) {
         os << '-' << configurations[i].name << configurations[i].argument;
      }
      else {
         assert(configurations[i].name.size() > 1);
         os << "--" << configurations[i].name << " " << configurations[i].argument;
      }
      os << ": " << configurations[i].message << ". Default: " << (!configurations[i].value.empty()?configurations[i].value:"unset")<<";" << endl;
   }
   return os.str();
}
