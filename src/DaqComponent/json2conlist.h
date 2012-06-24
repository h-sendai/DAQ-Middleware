// -*- C++ -*-
/*!
 * @file json2conlist.h
 * @brief json2conlist class
 * @date 1-January-2009
 * @author Yoshiji Yasu (yoshiji.yasu@kek.jp)
 *
 * Copyright (C) 2009-2011
 *     Yoshiji Yasu
 *     High Energy Accelerator Organization (KEK), Japan
 *     All rights reserved.
 *
 */

#ifndef JSON2CONLIST_H
#define JSON2CONLIST_H

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include "json_spirit.h"
#include <cassert>
#include <algorithm>
#include <fstream>
#include <boost/bind.hpp>

using namespace std;
using namespace boost;
using namespace json_spirit;

typedef std::map< std::string, std::string > conList;
typedef std::pair< std::string, std::string > conPair;
typedef std::map< std::string, std::string >::iterator conIt;

/*!
 * @class Json2ConList
 * @brief Json2ConList class
 * 
 * 
 *
 */
class Json2ConList {

 private:
  bool m_first_call;

 public:
  Json2ConList() : m_first_call(true) {
  }
  ~Json2ConList() {
  }

  bool makeConList(string file, conList* cList) {
    //  cout << "makeConList is called" << endl;
    cList->clear();
    
    ifstream is(file.c_str());
    if (!is.is_open()) {
      return false;
    }
    
    json_spirit::Value v;
    bool b = json_spirit::read(is, v);
    if (b == false) {
      return false;
    }
    
    json_spirit::Object o = v.get_obj();
    std::string namestr = "";
    makeConList_obj(o, namestr, cList);
    
//    print(cList);
    return true;
  }

 private:
  void makeConList_obj(Object& o, std::string& namestr, conList* cList) {
    //  cout << "makeConList_obj is called" << endl;
    std::string name = namestr;
    std::vector< Pair >::iterator it;
    for(it=o.begin(); it != o.end(); it++) {
      //    cout << "makeConList_obj:for loop" << endl;
      std::string s= (*it).name_;
      if (s[0] == '@') {
        makeConList_it(it, name, cList);
      }
    }
    
    for(it=o.begin(); it != o.end(); it++) {
      //    cout << "makeConList_obj:for loop" << endl;
      std::string s= (*it).name_;
      if (s[0] != '@') {
        makeConList_it(it, name, cList);
      }
    }
  }

  void makeConList_it(std::vector< Pair >::iterator it, std::string& namestr, conList* cList) {
      std::string name = namestr;
      std::string s;
      if(m_first_call) {
	name = "";
	s = "";
	m_first_call = false;
      } else 
	s= (*it).name_;
      Value v1 = (*it).value_;
      Value_type t = v1.type();
      if(s[0] == '@') {
	if(t == str_type) {
	  std::string s1 = v1.get_str();
	  name += s1+"_";
	}
	if(t== int_type) {
	  int i = v1.get_int();
	  std::stringstream ss;
	  ss << i;
	  name += ss.str()+"_";
	}
      } else { 
	if(s!="" && s[0]!='#')
	  name += s + "_";
      }
      if(s[0]=='@') {
	namestr = name;
	return;
      }
      if(t == obj_type) {
	Object o1 = v1.get_obj();
	makeConList_obj(o1, name, cList);
      } else {
	std::string cName;
	std::string cValue;
	
	cName = name.substr(0,name.size()-1); // remove last "_"
	if(t == str_type) {
	  std::string s = v1.get_str();
	  cValue = s;
	} else if (t == int_type) {
	  int i = v1.get_int();
	  std::stringstream ss;
	  ss << i;
	  cValue = ss.str();
	} else if(t== array_type) {
//	  cout << "value = array type" << endl;
	  Array array = v1.get_array();
	  for (int i = 0; i < (int)array.size(); ++i) {
	    Object o2 = array[i].get_obj();
	    makeConList_obj(o2, name, cList);
	  }
	  return;
	} else if(t==bool_type)
	  cout << "value = bool type" << endl;
	else if(t==real_type) {
	  //cout << "value = real type" << endl;
	  double dv = v1.get_real();
	  std::stringstream ss;
	  ss << dv;
	  cValue = ss.str();
	  // cValue may look like integer (for example "100").
	  // We have to express it as floating number. 
	  if (cValue.find('.') == std::string::npos) { // if '.' not found
	    cValue += ".0";
	  }
	}
	else
	  cout << "value = null type or illegal" << endl;
	
	cList->insert(conPair(cName, cValue));
      }
  }

  void print(conList* cList) {
    cout << "* json2conlist::print()_start" << endl;
    for (conIt it = cList->begin(); it != cList->end(); ++it) {
      std::string first = (std::string)(it->first);
      std::string second = (std::string)(it->second);
      cout << "(" << first << ", " << second << ")" << endl;
    }
    cout << "* json2conlist::print()_end" << endl;
  }
};
#endif // JSON2CONLIST_H

