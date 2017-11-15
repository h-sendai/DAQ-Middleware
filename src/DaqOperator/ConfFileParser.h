// -*- C++ -*-
/*!
 * @file ConfFileParser.h
 * @brief Configuration File Parsing class
 * @date 1-January-2008
 * @author Kazuo Nakayoshi <kazuo.nakayoshi@kek.jp>
 *
 * Copyright (C) 2008-2011
 *     Kazuo Nakayoshi
 *     High Energy Accelerator Research Organization (KEK), Japan.
 *     All rights reserved.
 *
 */

#ifndef CONFFILEPASER_H
#define CONFFILEPASER_H

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <stdexcept>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include "ComponentInfoContainer.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "DAQServiceSVC_impl.h"

typedef std::map<std::string, std::string > ParamMap;

struct componentInfo {
    std::string compId;
    std::string compHostAddr;
    std::string compHostPort;
    std::string compInstName;
    std::string compOutPort;
    std::string compInPort;
    std::string compFromOut;
    ParamMap paramMap;
};

/*!
 * @class ConfFileParser
 * @brief ConfFileParser class
 * 
 * 
 *
 */
class ConfFileParser
{
public:
    ConfFileParser();
    virtual ~ConfFileParser();

    int readConfFile(const char* xmlFile, bool isConfigure);
    CompGroupList getGroupList();
    CompInfoList  getCompList();
    ParamList     getParamList();
    void setList(std::vector<NameValue>& list, char* name, char* value);
    void setSeq(std::vector<NameValue> vec, NVList& seq);

private:
    int checkXmlFile(const char* xmlFile);
    std::string getElementByTagName(xercesc::DOMElement* ele, 
				    XMLCh* chName,
				    std::string xpath);

    int getElementsFromParent(xercesc::DOMElement* myEle, 
			      XMLCh* chName,
			      std::string xpath);
    int getElementsFromParent(xercesc::DOMElement* myEle, 
			      XMLCh* chName,
			      std::string xpath,
			      std::string gid,
			      ComponentInfoContainer* compCont);

    int getParams(xercesc::DOMElement* myEle, 
		  XMLCh* chName,
		  std::string xpath,
		  ///char* compId,
		  std::string compId,
		  ComponentInfoContainer* compCont);

    std::string makeXPath(std::string path1, std::string path2, int index);
    std::string makeXPath(std::string path1, XMLCh* path2, int index);
    std::string makeXPath(std::string path1, XMLCh* path2);
    std::string makeXPath(std::string path1, xercesc::DOMAttr* path2);

    xercesc::XercesDOMParser *m_xercesDomParser;
    xercesc::ErrorHandler* m_errHandler;

    CompGroupList m_groupList;
    CompInfoList  m_compList;
    ParamList     m_paramList;

    int   m_comp_num;
    int   m_sitcp_num;
    bool  m_debug;

    XMLCh* TAG_root;
    XMLCh* TAG_groups;
    XMLCh* TAG_group;
    XMLCh* TAG_groupId;
    XMLCh* TAG_components;
    XMLCh* TAG_component;
    XMLCh* TAG_compId;
    XMLCh* TAG_compHostAddr;
    XMLCh* TAG_compHostPort;
    XMLCh* TAG_compInstName;
    XMLCh* TAG_compExecPath;
    XMLCh* TAG_compConfFile;
    XMLCh* TAG_compStartOrd;
    XMLCh* TAG_compOutPort;
    XMLCh* TAG_compInPort;
    XMLCh* TAG_compFromOut;
    XMLCh* TAG_compBufferLength;
    XMLCh* TAG_compBufferReadTimeout;
    XMLCh* TAG_compBufferWriteTimeout;
    XMLCh* TAG_compBufferReadEmptyPolicy;
    XMLCh* TAG_compBufferWriteFullPolicy;
    XMLCh* TAG_paramId;
    XMLCh* TAG_params;
    XMLCh* TAG_param;
    XMLCh* TAG_select;
}; 
#endif
