// -*- C++ -*-
/*!
 * @file CreateDom.cpp
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
#include <iostream>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>

#include "CreateDom.h"

using namespace DAQMW;

class XStr
{
public :
    XStr(const char* const toTranscode)
    {
        fUnicodeForm = XMLString::transcode(toTranscode);
    }

    ~XStr()
    {
        XMLString::release(&fUnicodeForm);
    }

    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }

private :
    XMLCh*   fUnicodeForm;
};

#define X(str) XStr(str).unicodeForm()

CreateDom::CreateDom(): m_debug(false)
{
}

CreateDom::~CreateDom()
{
}

std::string CreateDom::getOK(std::string command)
{
	makeRoot();
	makeMethodnName(command);
	makeReturnvalue();
	makeResultOK();

	std::string buf = getBuffer();
	return buf;
}

std::string CreateDom::getNG(std::string command, int code, 
		std::string methodName, std::string messageEng, std::string messageJpn)
{
	char str[8];
	sprintf(str, "%d", code);
	
	Result result;
	result.code = str;
	result.className = "DAQ";
	result.methodName = methodName;
	result.messageEng = messageEng;
	result.messageJpn = messageJpn;

	makeRoot();
	makeMethodnName(command);
	makeReturnvalue();
	makeResultNG(result);

	std::string buf = getBuffer();
	return buf;
}

std::string CreateDom::getParams(std::string command, NVList* list)
{
	if(m_debug) {
		std::cerr << "getParams: enter" << std::endl;
	}

	///m_str_sitcp = "sitcp";

	makeRoot();
	makeMethodnName(command);
	makeReturnvalue();
	makeResultOK();
	  //makeParams(m_returnElem, list);
	makeParams();

	std::string buf = getBuffer();
	return buf;
}

std::string CreateDom::getStatus(std::string command, DAQLifeCycleState state)
{
    ///m_str_sitcp = "sitcp";

	makeRoot();
	makeMethodnName(command);
	makeReturnvalue();
	makeResultOK();
	makeDevStatus();
	makeState(state);
	//makeParams(m_devStatusElem, list);
	makeParams();

	std::string buf = getBuffer();
	//std::cerr << "CreateDom::getStatus():" << buf << std::endl;
	return buf;
}

std::string CreateDom::getLog(std::string command, groupStatusList status_list)
{
	makeRoot();
	makeMethodnName(command);
	makeReturnvalue();
	makeResultOK();
	makeLogs();

	for (std::vector< groupStatus >::iterator p = status_list.begin(); 
	     p != status_list.end(); ++p) {
	    makeLog(*p);
	}

	std::string buf = getBuffer();
	return buf;
}

std::string CreateDom::getLog(std::string command, groupStatusList status_list, 
			      std::string err_msg)
{
	makeRoot();
	makeMethodnName(command);
	makeReturnvalue();

	Result result;
	result.status = true;
	result.code = "0";
	result.messageEng = err_msg;
	std::cerr << "getLog: " << err_msg << std::endl;
	makeResult(result);
	makeLogs();
	
	for (std::vector< groupStatus >::iterator p = status_list.begin(); 
	     p != status_list.end(); ++p) {
	    makeLog(*p);
	}

	std::string buf = getBuffer();
	return buf;
}

std::string CreateDom::getState(DAQLifeCycleState state, bool flag)
{
    const char *str1[] = {"", "LOADED", "CONFIGURED", "RUNNING", "PAUSED", "FIX", "STOP"};
    const char *str2[] = {"", "Ready",  "Parameter Set", "Acquiring", "Paused"};

	int idx = 0;
	switch(state) {
	case(LOADED):
		idx = 1;
		break;
	case(CONFIGURED):
		idx = 2;
		break;
	case(RUNNING):
		idx = 3;
		break;
	case(PAUSED):
		idx = 4;
		break;
	case(ERROR):
		idx = 5;
		break;
	case(STOP):
		idx = 6;
		break;
	default:
		idx = 0;
		break;
	}

	std::string text;
	if (flag == true) {
		text = str1[idx];
	} else {
		text = str2[idx];
	}
	return text;
}

void CreateDom::makeRoot()
{
	m_impl =  DOMImplementationRegistry::getDOMImplementation(X("Core"));

	//response
	m_doc = m_impl->createDocument(0, X("response"), 0);
//	m_doc->setStandalone("yes");

	m_rootElem = m_doc->getDocumentElement();
}

void CreateDom::makeMethodnName(std::string name)
{
	// methodname
	DOMElement* methodElem = m_doc->createElement(X("methodName"));
	m_rootElem->appendChild(methodElem);

	DOMText* methodText = m_doc->createTextNode(X(name.c_str()));
	methodElem->appendChild(methodText);
}

void CreateDom::makeReturnvalue()
{
	// returnvalue
	m_returnElem = m_doc->createElement(X("returnValue"));
	m_rootElem->appendChild(m_returnElem);
}

void CreateDom::makeResultOK()
{
	Result result;
	result.status = true;
	result.code = "0";

	makeResult(result);
}

void CreateDom::makeResultNG(Result result)
{
	result.status = false;

	makeResult(result);
}

void CreateDom::makeResult(Result result)
{
	// result
	DOMElement* resultElem = m_doc->createElement(X("result"));
	m_returnElem->appendChild(resultElem);

	// status
	if (result.status == true) {
		make(resultElem, "status", "OK");
	} else {
		make(resultElem, "status", "NG");
	}

	// code
	make(resultElem, "code", result.code);

	// className
	make(resultElem, "className", result.className);

	// name
	make(resultElem, "name", result.name);

	// methodName
	make(resultElem, "methodName", result.methodName);

	// messageEng
	std::cerr << "makeResult:" << result.messageEng << std::endl;///
	make(resultElem, "messageEng", result.messageEng);

	// messageJpn
	make(resultElem, "messageJpn", result.messageJpn);
}

void CreateDom::makeDevStatus()
{
	// devStatus
	m_devStatusElem = m_doc->createElement(X("devStatus"));
	m_returnElem->appendChild(m_devStatusElem);

	// name
	make(m_devStatusElem, "name", "DAQ");
}

void CreateDom::make(DOMElement* ele, std::string tag, std::string text)
{
	DOMElement* valElem = m_doc->createElement(X(tag.c_str()));
	ele->appendChild(valElem);

	if (text.length() > 0) {
		DOMText* valText= m_doc->createTextNode(X(text.c_str()));
		valElem->appendChild(valText);
	}
}

void CreateDom::makeState(DAQLifeCycleState state)
{
	std::string text = getState(state, false);

	//make(m_devStatusElem, "state", text); // commented out 09/07/27
	                                        // pointed out by Nakatani.
	make(m_devStatusElem, "status", text);
}

void CreateDom::makeParams()
{
	if(m_debug) {
		std::cerr << "makeParams: enter" << std::endl;
	}

	// params
	//m_paramsElem = m_doc->createElement(X("params"));
	make(m_devStatusElem, "params", "");
	return;
}

/*
void CreateDom::makeName(int cnt, std::string value)
{
    ///makeSitcp(cnt);

	DOMElement* nameElem = m_doc->createElement(X("name"));
	m_sitcpElem->appendChild(nameElem);

	DOMText* nameText = m_doc->createTextNode(X(value.c_str()));
	nameElem->appendChild(nameText);
}
*/

void CreateDom::makeValue(DOMElement* ele, int index, std::string value)
{
	DOMElement* valElem = m_doc->createElement(X("val"));
	ele->appendChild(valElem);

	char id[8];
	sprintf(id, "%d", index);
	valElem->setAttribute(X("id"), X(id));

	DOMText* valText= m_doc->createTextNode(X(value.c_str()));
	valElem->appendChild(valText);
}

void CreateDom::makeLogs()
{
	m_logsElem = m_doc->createElement(X("logs"));
	m_returnElem->appendChild(m_logsElem);
}

///void CreateDom::makeLog(Status status)
void CreateDom::makeLog(groupStatus status) 
{
	m_logElem = m_doc->createElement(X("log"));
	m_logsElem->appendChild(m_logElem);

	// compName
	//std::string name = getCompName(status.comp_name);
	std::string name = status.groupId;
	make(m_logElem, "compName", name);

	// state
	//std::string state = getState(status.state, true);
	std::string state = getState(status.comp_status.state, true);
	make(m_logElem, "state", state);

	// event_num
    // max unsigned long long int is 18446_74407_37095_51615 (20 digits)
	char num[21];
	sprintf(num, "%llu", (long long unsigned int)status.comp_status.event_size);
	make(m_logElem, "eventNum", num);

	// component status
	std::string comp_status;
	//switch (status.comp_status) {
	switch (status.comp_status.comp_status) {
	    //case COMP_OK:
	    //comp_status = "OK";
	    //break;
	case COMP_WORKING:
	  comp_status = "WORKING";
	  break;
	case COMP_FINISHED:
	  comp_status = "FINISHED";
	  break;
	case COMP_WARNING:
	  comp_status = "WARNING";
	  break;
	case COMP_FATAL:
	  comp_status = "FATAL";
	  break;
	}

	make(m_logElem, "compStatus", comp_status);

	return;
}

#ifdef MLF
bool CreateDom::check(std::string name, int *cnt, int *type, int *index)
{
	std::string::size_type n, nb = 0;
	if (name.find(m_str_sitcp, 0) != 0) {
		return false;
	}
	nb = m_str_sitcp.length();

	n = name.find('_', nb);
	if (n == std::string::npos) {
		return false;
	}

	int d = checkDigit(name, nb, n-nb);
	if (d == -1) {
		return false;
	}
	*cnt = d;

	nb = n+1;
	*type = -1;
	if (name.find(m_str_ip, nb) == nb) {
		*type = 0;
		nb += m_str_ip.length();
	}
	if (name.find(m_str_name, nb) == nb) {
		*type = 1;
		nb += m_str_name.length();
	}
	if (name.find(m_str_level, nb) == nb) {
		*type = 2;
		nb += m_str_level.length();
	}
	if (name.find(m_str_co, nb) == nb) {
		*type = 3;
		nb += m_str_co.length();
	}
	if (name.find(m_str_dead, nb) == nb) {
		*type = 4;
		nb += m_str_dead.length();
	}

	if (*type == -1) {
		return false;
	}

	n = name.size();
	if (*type == 0 || *type == 1) {
		if (nb != n) {
			return false;
		}
	} else if (2 <= *type && *type <= 4) {	
		if (nb == n) {
			return false;
		}

		d = checkDigit(name, nb, n-nb);
		if (d == -1) {
			return false;
		}
		*index = d;
	}

	return true;
}


int CreateDom::checkDigit(std::string name, int index, int cnt)
{
	for (int i = index; i <index+cnt; ++i) {
		if (isdigit(name.at(i)) == false) {
			return (-1);
		}
	}
	std::string sub  = name.substr(index, cnt);
	int d = atoi(sub.c_str());

	return d;
}
#endif

std::string CreateDom::getBuffer()
{
	if (m_impl == NULL) {
		return NULL;
	}

	MemBufFormatTarget target;

#if _XERCES_VERSION < 30000
    DOMWriter* writer = ((DOMImplementationLS*)m_impl)->createDOMWriter();
    writer->writeNode(&target, *m_rootElem);
    // for debug
    // LocalFileFormatTarget file("debug.xml");
    // writer->writeNode(&file, *m_doc);
#else
	DOMLSSerializer* writer = ((DOMImplementationLS*)m_impl)->createLSSerializer();
    DOMLSOutput *theOutputDesc = ((DOMImplementationLS*)m_impl)->createLSOutput();
    // set user specified output encoding
    XMLCh encodeStr[100];
    XMLString::transcode("UTF-8", encodeStr, 99);
    theOutputDesc->setEncoding(encodeStr);
    theOutputDesc->setByteStream(&target);
    writer->write(m_rootElem, theOutputDesc);

    theOutputDesc->release();
#endif
    writer->release();
	//delete writer;

	m_doc->release();

	return (char*)target.getRawBuffer();
}

