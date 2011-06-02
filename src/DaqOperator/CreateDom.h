// -*- C++ -*-
/*!
 * @file CreateDom.h
 * @brief 
 * @date 1-January-2008
 * @author Kazuo Nakayoshi <kazuo.nakayoshi@kek.jp>
 *
 * Copyright (C) 2008-2011
 *     Kazuo Nakayoshi
 *     High Energy Accelerator Research Organization (KEK), Japan.
 *     All rights reserved.
 *
 */

#ifndef CREATEDOM_H
#define CREATEDOM_H

#include <string>
#include <vector>

#include <stdio.h>

#include "ComponentInfoContainer.h"

using namespace xercesc;

/*!
 * @namespace DAQMW
 * @brief common namespace of DAQ-Middleware
 */
namespace DAQMW {

/*!
 * @class CreateDom
 * @brief CreateDom class
 * 
 * 
 *
 */
class CreateDom
{
	struct Result {
		bool status;
		std::string code;
		std::string className;
		std::string name;
		std::string methodName;
		std::string messageEng;
		std::string messageJpn;
	};

public:
	CreateDom();
	~CreateDom();

	std::string getOK(std::string command);
	std::string getNG(std::string command, int code, 
			  std::string methodName, std::string messageEng, 
			  std::string messageJpn);
	std::string getParams(std::string command, NVList* list);
        std::string getStatus(std::string command, DAQLifeCycleState state);
        std::string getLog(std::string command, groupStatusList status_list);
        std::string getLog(std::string command, groupStatusList status_list,
			   std::string err_msg);
	std::string getState(DAQLifeCycleState state, bool flag);
	
private:
	void makeRoot();
	void makeMethodnName(std::string name);
	void makeReturnvalue();
	void makeResultOK();
	void makeResultNG(Result result);
	void makeResult(Result result);
	void makeDevStatus();
	void make(DOMElement* ele, std::string tag, std::string text);
	void makeState(DAQLifeCycleState state);
	void makeParams();
	void makeValue(DOMElement* ele, int index, std::string value);
	void makeLogs();
	void makeLog(groupStatus groupStatus);
	std::string getBuffer();

private:
	DOMImplementation* m_impl;
	DOMDocument* m_doc;
	DOMElement* m_rootElem;
	DOMElement* m_returnElem;
	DOMElement* m_devStatusElem;
	DOMElement* m_paramsElem;
	DOMElement* m_logsElem;
	DOMElement* m_logElem;
	
	bool  m_debug;
};

}//namespace
#endif // CREATEDOM_H
