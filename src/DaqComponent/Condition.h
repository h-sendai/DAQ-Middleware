// -*- C++ -*-
/*!
 * @file Condition.h
 * @brief Base class of Condition database
 * @date 1-January-2009
 * @author Yoshiji Yasu <yoshiji.yasu@kek.jp>\n
 * 9-September-2009 modified by HOSOYA Takaaki (university of ibaraki)
 * @version 1.0
 *
 * Copyright (C) 2009-2011
 *		 Yoshiji Yasu
 *		 High Energy Accelerator Organization (KEK), Japan
 *		 All rights reserved.
 *
 */

#ifndef CONDITION_H
#define CONDITION_H

#include <ctype.h>
#include "json2conlist.h"
#include <cstdlib>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

/*!
 * @class Condition
 * @brief Condition class
 * 
 * This is default condition class. User uses the class as base class.
 *
 */
class Condition
{
public:
	/*
	 * @brief Constructor
	 * 
	 * Constructor.
	 * Initialize the condition class.
	 *
	 */
	Condition()
		: reg_pos_int("[0-9]+"),
		reg_neg_int("-[0-9]+"),
		reg_hex_int("0[xX][0-9A-Fa-f]+"),
		reg_float("-?[0-9]+\\.[0-9]+([Ee]-?[0-9]+)?") {;}
	/*
	 * @brief Virtual destractor
	 * 
	 * Virtual destractor
	 *
	 */ 
	virtual ~Condition() {;}

public:
	void init(conList* cList) {
		m_cList = cList;
		m_prefix = "";
	}
	void setPrefix(const std::string& prefix) {
		m_prefix = prefix;
	}
	// Proposal by HOSOYA Takaaki (2009.9.9)
	//		find key and get the value as "string"
	bool find_as_string(const std::string& key, std::string& value)
	{
		std::cerr << "key: " << m_prefix << key << std::endl;
		conIt it = m_cList->find(m_prefix+key);
		if (it == m_cList->end()) {
			//cerr << "not find !" << endl;
			return false;
		}
		value = it->second;
		return true;
	}
	bool find(const std::string& key, std::string* value)
	{
		return find_as_string(key, *value);
	}
	// Proposal by HOSOYA Takaaki (2009.9.9)
	//		find key and get the value as "int"
	bool find_as_int(const std::string& key, int& value)
	{
		std::string result;
		if (!find_as_string(key, result)) {
			return false;
		}
		if (boost::regex_match(result, reg_pos_int) ||		// positive value
			boost::regex_match(result, reg_neg_int))		// negative value
		{
			value = boost::lexical_cast<int>(result);
		}
		else if (boost::regex_match(result, reg_hex_int)) {
			char* e;
			value = static_cast<int>(std::strtoul(result.c_str(), &e, 0));
		}
		else {
			value = 0;
			return false;
		}
		return true;
	}
	bool find(const std::string& key, int* value)
	{
		return find_as_int(key, *value);
	}
	// Proposal by HOSOYA Takaaki (2009.9.9)
	//		find key and get the value as "unsigned int"
	bool find_as_uint(const std::string& key, unsigned int& value)
	{
		std::string result;
		if (!find_as_string(key, result)) {
			return false;
		}
		if (boost::regex_match(result, reg_pos_int)) {
			value = boost::lexical_cast<unsigned int>(result);
		}
		else if (boost::regex_match(result, reg_hex_int)) {
			char* e;
			value = static_cast<unsigned int>(std::strtoul(result.c_str(), &e, 0));
		}
		else {
			value = 0;
			return false;
		}
		return true;
	}
	bool find(const std::string& key, unsigned int* value)
	{
		return find_as_uint(key, *value);
	}
	// Proposal by HOSOYA Takaaki (2009.9.9)
	//		find key and get the value as "double"
	bool find_as_double(const std::string& key, double& value)
	{
		std::string result;
		if (!find_as_string(key, result)) {
			return false;
		}
		if (boost::regex_match(result, reg_pos_int) ||		// positive value
			boost::regex_match(result, reg_neg_int))		// negative value
		{
			value = boost::lexical_cast<double>(result);
		}
		else if (boost::regex_match(result, reg_hex_int)) {
			char* e;
			value = static_cast<double>(std::strtoul(result.c_str(), &e, 0));
		}
		else if (boost::regex_match(result, reg_float)) {
			value = boost::lexical_cast<double>(result);
		}
		else {
			value = 0.0;
			return false;
		}
		return true;
	}
	bool find(const std::string& key, double* value)
	{
		return find_as_double(key, *value);
	}

	bool find(const std::string& key, long long* value)
	{
		std::string result;
		if (!find_as_string(key, result)) {
			return false;
		}
		if (boost::regex_match(result, reg_pos_int) ||		// positive value
			boost::regex_match(result, reg_neg_int))		// negative value
		{
			*value = boost::lexical_cast<long long>(result);
		}
		else if (boost::regex_match(result, reg_hex_int)) {
			char* e;
			*value = static_cast<long long>(std::strtoul(result.c_str(), &e, 0));
		}
		else {
			value = 0;
			return false;
		}
		return true;
	}

protected:
	void printInt(const std::string& name, unsigned int value) {
				cout << name << ":" << value << endl;
	}
	void printString(const std::string& name, const std::string& value) {
					cout << name << ":" << value << endl;
	}

private:
	conList* m_cList;
	std::string m_prefix;
	boost::regex reg_pos_int;
	boost::regex reg_neg_int;
	boost::regex reg_hex_int;
	boost::regex reg_float;
};

#endif // CONDITION_H
