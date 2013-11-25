// -*- C++ -*-
/*!
 * @file ConfFileParser.cpp
 * @brief Configuration File Parsing class implementation
 * @date 1-January-2008
 * @author Kazuo Nakayoshi <kazuo.nakayoshi@kek.jp>
 *
 * Copyright (C) 2008-2011
 *     Kazuo Nakayoshi
 *     High Energy Accelerator Research Organization (KEK), Japan.
 *     All rights reserved.
 *
 */


#include "ConfFileParser.h"
using namespace xercesc;

ConfFileParser::ConfFileParser()
    : m_comp_num(0), m_sitcp_num(0), m_debug(false)
{

     TAG_root         = XMLString::transcode("configInfo");
     TAG_group        = XMLString::transcode("daqGroup");
     TAG_groups       = XMLString::transcode("daqGroups");
     TAG_groupId      = XMLString::transcode("gid");
     TAG_components   = XMLString::transcode("components");
     TAG_component    = XMLString::transcode("component");
     TAG_compId       = XMLString::transcode("cid");
     TAG_compHostAddr = XMLString::transcode("hostAddr");
     TAG_compHostPort = XMLString::transcode("hostPort");
     TAG_compInstName = XMLString::transcode("instName");
     TAG_compExecPath = XMLString::transcode("execPath");
     TAG_compConfFile = XMLString::transcode("confFile");
     TAG_compStartOrd = XMLString::transcode("startOrd");
     TAG_compOutPort  = XMLString::transcode("outPort");
     TAG_compInPort   = XMLString::transcode("inPort");
     TAG_compFromOut  = XMLString::transcode("from");
     TAG_compBufferLength = XMLString::transcode("buffer_length");
     TAG_compBufferReadTimeout  = XMLString::transcode("buffer_read_timeout");
     TAG_compBufferWriteTimeout = XMLString::transcode("buffer_write_timeout");
     TAG_compBufferReadEmptyPolicy = XMLString::transcode("buffer_read_empty_policy");
     TAG_compBufferWriteFullPolicy = XMLString::transcode("buffer_write_full_policy");
     TAG_paramId      = XMLString::transcode("pid");
     TAG_params       = XMLString::transcode("params");
     TAG_param        = XMLString::transcode("param");

     m_xercesDomParser = new XercesDOMParser;
}

ConfFileParser::~ConfFileParser()
{
    try {
        XMLString::release( &TAG_root );
        XMLString::release( &TAG_groups );
        XMLString::release( &TAG_group );
        XMLString::release( &TAG_groupId );
        XMLString::release( &TAG_components );
        XMLString::release( &TAG_component );
        XMLString::release( &TAG_compId );
        XMLString::release( &TAG_compHostAddr );
        XMLString::release( &TAG_compHostPort );
        XMLString::release( &TAG_compInstName );
        XMLString::release( &TAG_compExecPath );
        XMLString::release( &TAG_compConfFile );
        XMLString::release( &TAG_compStartOrd );
        XMLString::release( &TAG_compOutPort );
        XMLString::release( &TAG_compInPort );
        XMLString::release( &TAG_compFromOut );
        XMLString::release( &TAG_compBufferLength );
        XMLString::release( &TAG_compBufferReadTimeout );
        XMLString::release( &TAG_compBufferWriteTimeout );
        XMLString::release( &TAG_compBufferReadEmptyPolicy );
        XMLString::release( &TAG_compBufferWriteFullPolicy );
        XMLString::release( &TAG_paramId );
        if (TAG_params) {
            XMLString::release( &TAG_params );
        }
        if (TAG_param) {
            XMLString::release( &TAG_param );
        }
   }
   catch( ... )
   {
       std::cerr << "### ERROR: Unknown exception encountered in TagNamesdtor" 
                 << std::endl;
   }
}

/**
 *   Test the specified xml file. If not ok, throw exceptions. 
 */
int ConfFileParser::checkXmlFile(const char* xmlFile)
{
   // Test to see if the file is ok.
   struct stat fileStatus;

   int iretStat = stat(xmlFile, &fileStatus);
   if ( iretStat == ENOENT )
       throw ( std::runtime_error(
                   "Path file_name does not exist, or path is an empty string."));
   else if ( iretStat == ENOTDIR )
       throw ( std::runtime_error("A component of the path is not a directory."));
   else if ( iretStat == ELOOP )
       throw ( std::runtime_error(
                   "Too many symbolic links encountered while traversing the path."));
   else if ( iretStat == EACCES )
       throw ( std::runtime_error("Permission denied."));
   else if ( iretStat == ENAMETOOLONG )
       throw ( std::runtime_error("File can not be read\n"));

   // Configure DOM parser.
   //m_xercesDomParser->setValidationScheme( XercesDOMParser::Val_Never );
   m_xercesDomParser->setValidationScheme( XercesDOMParser::Val_Always );
   m_xercesDomParser->setDoNamespaces( false );
   //m_xercesDomParser->setDoSchema( false );
   m_xercesDomParser->setDoSchema( true );
   m_xercesDomParser->setExternalNoNamespaceSchemaLocation("config.xsd");

   m_xercesDomParser->setLoadExternalDTD( false );

   //ErrorHandler* errHandler;
   m_errHandler = (ErrorHandler*) new HandlerBase();
   m_xercesDomParser->setErrorHandler(m_errHandler);

   return 0;
}

/**
 *  read conf.xml file and parse it.  output after parsing is XPath-like format.
 */
int ConfFileParser::readConfFile(const char* xmlFile, bool isConfigure)
{
    if (m_debug) {
        std::cerr << "***** readConfFile: " << xmlFile << std::endl;
    }
    // Test to see if the file is ok.
    try {
        checkXmlFile(xmlFile);

        m_xercesDomParser->parse(xmlFile);

        DOMDocument* xmlDoc = m_xercesDomParser->getDocument();

        DOMElement* root = xmlDoc->getDocumentElement();
        if ( !root ) {
            throw(std::runtime_error( "empty XML document" ));
        }

        DOMNodeList*      group = root->getElementsByTagName(TAG_group);

        std::string myroot = "root";

        /// loop for daqGroup nodes
        for(int gindex = 0; gindex < (int)group->getLength(); gindex++ ) {

            CompInfoList compList;
            ComponentGroup compGroup;

            DOMElement* gElem = static_cast<DOMElement*> (group->item(gindex));

            std::string myKey;

            std::string myKey0 = makeXPath(myroot, TAG_groups, 0);
            myKey  = makeXPath(myKey0,  TAG_group, gindex);

            DOMAttr* attr = gElem->getAttributeNode(TAG_groupId);
            char* gid = XMLString::transcode(attr->getValue());
            std::string groupId = gid;
            if (m_debug) {
                std::cerr << "++++++ groupID: " << groupId << std::endl;
            }
            XMLString::release(&gid);
            compGroup.setGroupId(groupId);
            if (m_debug) {
                std::cerr << "  valu: "
                          << XMLString::transcode(attr->getValue())
                          << std::endl;
            }
            DOMElement*  ele  = (DOMElement*)group->item(gindex);
            DOMNodeList* comp = ele->getElementsByTagName(TAG_component);

            int compNum = comp->getLength();
            m_comp_num += compNum;

            /// loop for Component nodes
            for(int m = 0; m < compNum; m++){
                ComponentInfoContainer compCont;
                DOMElement* ele = (DOMElement*)comp->item(m);
                // comp id
                std::string key1;
                if (isConfigure) {
                    std::string key0 = makeXPath(myKey, TAG_components, 0);
                    key1 = makeXPath(key0,  TAG_component, m);
                    std::string key2 = makeXPath(key1,  TAG_compId);
                    //std::cerr << "name: " << key2;
                }

                DOMAttr* cattr = ele->getAttributeNode(TAG_compId);
                char* compId = XMLString::transcode(cattr->getValue());
                if (m_debug) {
                    std::cerr << "compId:" << compId << std::endl;
                }
                compCont.setId(compId);

                std::string addr = getElementByTagName(ele, TAG_compHostAddr, key1); ///get host address
                std::string port = getElementByTagName(ele, TAG_compHostPort, key1); ///get host port
                std::string inst = getElementByTagName(ele, TAG_compInstName, key1); ///get instance name
                std::string exec = getElementByTagName(ele, TAG_compExecPath, key1); ///get comp exec. path
                std::string conf = getElementByTagName(ele, TAG_compConfFile, key1); ///get rtc.conf path
                std::string orde = getElementByTagName(ele, TAG_compStartOrd, key1); ///get start up order
                compCont.setAddress(addr);
                compCont.setPort(port);
                compCont.setName(inst);
                compCont.setExec(exec);
                compCont.setConf(conf);
                compCont.setStartupOrder(orde);

                getElementsFromParent(ele, TAG_compInPort,  key1, groupId, &compCont); ///get inPorts
                getElementsFromParent(ele, TAG_compOutPort, key1, groupId, &compCont); ///get outPorts

                if (isConfigure) {
                    std::string key3 = makeXPath(key1, TAG_params, 0);
                    std::string mycompId = groupId + ":" + compId;
                    getParams(ele, TAG_param, key3, mycompId, &compCont); ///get param
                }

                XMLString::release(&compId);
                compList.push_back(compCont);
            } // for comp
            compGroup.setCompInfoList(compList);
            m_groupList.push_back(compGroup);
        } // for group
    } catch(const XMLException& e) {
        char* message = XMLString::transcode(e.getMessage());
        std::cerr << "### ERROR: readConfFile" << std::endl;
        std::cerr << message << std::endl;
        XMLString::release(&message);
        throw;
    } catch(const DOMException& e) {
        char* message = XMLString::transcode(e.msg);
        std::cerr << "### ERROR: readConfFile" << std::endl;
        std::cerr << message << std::endl;
        XMLString::release(&message);
        throw;
    } catch(const SAXException& e) {
        char* message = XMLString::transcode(e.getMessage());
        std::cerr << "### ERROR: readConfFile" << std::endl;
        std::cerr << message << std::endl;
        XMLString::release(&message);
        std::cerr << "### Check your config.xml file\n";
        //throw;
        exit(1);
    } catch (std::exception& e) {
        std::cerr << "### ERROR: readConfFile" << std::endl;;
        std::cerr << e.what() << std::endl;
        throw;
    } catch(...) {
        std::cerr << "### ERROR: readConfFile" << std::endl;;
        std::cerr << "Unexpected Exception in ConfFileParser" << std::endl;
        throw;
    }

    delete m_xercesDomParser;
    delete m_errHandler;

    if (m_debug) {
        std::cerr << "readConfFile() exit\n";
    }
    return m_comp_num;
}

int ConfFileParser::getElementsFromParent(
    xercesc::DOMElement* myEle, XMLCh* chName, std::string xpath)
{

    DOMNodeList* nodeList = myEle->getElementsByTagName(chName);
    int nodeLen = (signed)nodeList->getLength();

    for(int index = 0; index < nodeLen; index++) {
        DOMElement* nodeEle = (DOMElement*)nodeList->item(index);
        if (nodeEle) {
            DOMNode* myNode = nodeEle->getFirstChild();
            char *textCont = XMLString::transcode(myNode->getTextContent());
            std::string name = makeXPath(xpath, chName, index);
            XMLString::release(&textCont);
        }
    }
    return 0;
}


int ConfFileParser::getElementsFromParent(
    xercesc::DOMElement* myEle, XMLCh* chName, std::string xpath, std::string gid, ComponentInfoContainer* compCont)
{
    DOMNodeList* nodeList = myEle->getElementsByTagName(chName);
    char* tagName = XMLString::transcode(chName);
    int nodeLen = (signed)nodeList->getLength();

    for(int index = 0; index < nodeLen; index++) {

        DOMElement* nodeEle = (DOMElement*)nodeList->item(index);
        if (nodeEle) {
            DOMNode* myNode = nodeEle->getFirstChild();
            char* textCont = XMLString::transcode(myNode->getTextContent());
            std::string myport = textCont;

            std::string name = makeXPath(xpath, chName, index);

            if (strcmp(tagName, "inPort") == 0) {
                // from attribute
                char* from 
                    = XMLString::transcode(nodeEle->getAttributeNode(TAG_compFromOut)->getValue() );
                ///std::cerr << "#### from: " << XMLString::transcode(
                ///    nodeEle->getAttributeNode(TAG_compFromOut)->getValue() ) << std::endl;
                ///std::string inport  = gid + ":" + myport;
                ///std::string outfrom = gid + ":" + from;

                //// buffer_length attribute ////
                std::string buffer_length;
                if (nodeEle->getAttributeNode(TAG_compBufferLength)) {
                    char *buf_len
                        = XMLString::transcode(nodeEle->getAttributeNode(TAG_compBufferLength)->getValue() );
                    buffer_length = buf_len;
                    XMLString::release(&buf_len);
                }
                else {
                    buffer_length = "256";
                }

                //// buffer_read_timeout attribute ////
                std::string buffer_read_timeout;
                if (nodeEle->getAttributeNode(TAG_compBufferReadTimeout)) {
                    char *r_timeout
                        = XMLString::transcode(nodeEle->getAttributeNode(TAG_compBufferReadTimeout)->getValue() );
                    buffer_read_timeout = r_timeout;
                    XMLString::release(&r_timeout);
                }
                else {
                    buffer_read_timeout = "0.005"; // 5 milli seconds
                }

                //// buffer_write_timeout attribute ////
                std::string buffer_write_timeout;
                if (nodeEle->getAttributeNode(TAG_compBufferWriteTimeout)) {
                    char *w_timeout
                        = XMLString::transcode(nodeEle->getAttributeNode(TAG_compBufferWriteTimeout)->getValue() );
                    buffer_write_timeout = w_timeout;
                    XMLString::release(&w_timeout);
                }
                else {
                    buffer_write_timeout = "0.005"; // 5 milli seconds
                }

                //// buffer_read_empty_policy attribute ////
                std::string buffer_read_empty_policy;
                if (nodeEle->getAttributeNode(TAG_compBufferReadEmptyPolicy)) {
                    char *r_empty
                        = XMLString::transcode(nodeEle->getAttributeNode(TAG_compBufferReadEmptyPolicy)->getValue() );
                    buffer_read_empty_policy = r_empty;
                    XMLString::release(&r_empty);
                }
                else {
                    buffer_read_empty_policy = "block"; // block 
                }

                //// buffer_write_full_policy attribute ////
                std::string buffer_write_full_policy;
                if (nodeEle->getAttributeNode(TAG_compBufferWriteFullPolicy)) {
                    char *w_full
                        = XMLString::transcode(nodeEle->getAttributeNode(TAG_compBufferWriteFullPolicy)->getValue() );
                    buffer_write_full_policy = w_full;
                    XMLString::release(&w_full);
                }
                else {
                    buffer_write_full_policy = "block"; // block 
                }

                std::string inport  = myport;
                std::string outfrom = from;
                ///std::cerr << "$$$$$ outport: " << inport << std::endl;
                compCont->setInport(inport);
                compCont->setFromOutPort(outfrom);
                compCont->setBufferLength(buffer_length);
                compCont->setBufferReadTimeout(buffer_read_timeout);
                compCont->setBufferWriteTimeout(buffer_write_timeout);
                compCont->setBufferReadEmptyPolicy(buffer_read_empty_policy);
                compCont->setBufferWriteFullPolicy(buffer_write_full_policy);
                XMLString::release(&from);
            }
            else if (strcmp(tagName, "outPort") == 0) {
                ///std::string outport = gid + ":" + myport;
                std::string outport = myport;
                ///std::cerr << "$$$$$ outport: " << outport << std::endl;
                compCont->setOutport(outport);
            }
            else {
                std::cerr << "### ERROR: ConfFileParser::getElementsFromParent(): Bad Tag in configuration file\n";
            }
            XMLString::release(&textCont);
        }//if
    }//for

    //m_paramList.push_back(compParam);

    XMLString::release(&tagName);
    return 0;
}

int ConfFileParser::getParams(xercesc::DOMElement* myEle, XMLCh* chName, std::string xpath, std::string compId, ComponentInfoContainer* compCont)
{
    DOMNodeList* nodeList = myEle->getElementsByTagName(chName);
    char* tagName = XMLString::transcode(chName);
    int nodeLen = (signed)nodeList->getLength();

    ComponentParam compParam;

    compParam.setId(compId);
    NVList nvList;

    nvList.length(nodeLen*2);

    int pindex = 0;
    for(int index = 0; index < nodeLen; index++) {

        DOMElement* nodeEle = (DOMElement*)nodeList->item(index);

        if (nodeEle) {
            DOMNodeList* myNodes = nodeEle->getChildNodes();
            if( myNodes->getLength() == 0 ) {
                std::cerr << "#### ERROR: ConfFileParser::getParams(): empty node found\n";
                std::cerr << "#### check config.xml file\n";
                throw;
            }
            DOMNode* myNode = nodeEle->getFirstChild();
            char* textCont = XMLString::transcode(myNode->getTextContent());

            std::string val = textCont;
            std::string name = makeXPath(xpath, chName, index);

            if (strcmp(tagName, "param") == 0) {
                char* paramid ;

                if (nodeEle->getAttributeNode(TAG_paramId) != 0) {
                    paramid = XMLString::transcode(nodeEle->getAttributeNode(TAG_paramId)->getValue() );
                }
                else {
                    std::cerr << "### ERROR: Bad attribute name in param element\n";
                    std::cerr << "### Check the configuration file\n";
                    throw;
                }

                std::string paramId = paramid;
                std::string name1   = name + "/@pid";

                if (m_debug) {
                    std::cerr << "name1: " << name1;
                    std::cerr << " valu: " << paramid << std::endl;
                    std::cerr << "name2: " << name;
                    std::cerr << " valu: " << textCont << std::endl;
                }
                nvList[pindex].name  = name1.c_str();
                nvList[pindex].value = paramId.c_str();
                nvList[pindex + 1].name  = name.c_str();
                nvList[pindex + 1].value = val.c_str();

                //std::cerr << "### pindex = " << pindex << std::endl;

                pindex += 2;
                XMLString::release(&paramid);
            }
            else {
                std::cerr << "### ERROR: ConfFileParser::getParameters(): Bad Tag in conf file\n";
            }
            compParam.setList(nvList);
            XMLString::release(&textCont);
        }//if
    }//for
    m_paramList.push_back(compParam);
    XMLString::release(&tagName);
    return 0;
}

std::string ConfFileParser::makeXPath(std::string path1, XMLCh* path2, int index)
{
    char* cpath2 = XMLString::transcode(path2);

    std::stringstream xpath;
    if (path1 == "root") {
        if (index == -1) {
            xpath << "@" << cpath2;
        } else {
            xpath << cpath2 << "[" << index << "]";
        }
    } else {
        if (index == -1) {
            xpath << path1 << "/" << "@" << cpath2;
        } else {
            xpath << path1 << "/" << cpath2 << "[" << index << "]";
        }
    }
    XMLString::release(&cpath2);
    //std::cerr << "name: " << xpath.str() << std::endl;
    return xpath.str();
}

std::string ConfFileParser::makeXPath(std::string path1, XMLCh* path2)
{
    char* cpath2 = XMLString::transcode(path2);

    std::stringstream xpath;
    if (path1 == "root") {
        xpath << "@" << cpath2;
    } else {
        xpath << path1 << "/" << "@" << cpath2;
    }
    XMLString::release(&cpath2);
    //std::cerr << "name: " << xpath.str() << std::endl;
    return xpath.str();
}

std::string ConfFileParser::makeXPath(std::string path1, DOMAttr* path2)
{
    char* cpath2 = XMLString::transcode(path2->getValue() );

    std::stringstream xpath;
    if (path1 == "root") {
        xpath << "@" << cpath2;

    } else {
        xpath << path1 << "/" << "@" << cpath2;
    }
    XMLString::release(&cpath2);
    //std::cerr << "name: " << xpath.str() << std::endl;
    return xpath.str();
}

std::string ConfFileParser::makeXPath(std::string path1, std::string path2, int index)
{
    std::stringstream xpath;
    if (path1 == "root") {
        if (index == -1) {
            xpath << "@" << path2;
        } else {
            xpath << path2 << "[" << index << "]";
        }

    } else {
        if (index == -1) {
            xpath << path1 << "/" << "@" << path2;
        } else {
            xpath << path1 << "/" << path2 << "[" << index << "]";
        }
    }
    //std::cerr << "name: " << xpath.str();
    return xpath.str();
}

std::string ConfFileParser::getElementByTagName(DOMElement* ele, XMLCh* chName, std::string xpath)
{
    std::string val;
    DOMNodeList* list = ele->getElementsByTagName(chName);
    DOMElement* ch_ele = (DOMElement*)list->item(0);
    if (ch_ele) {
        DOMNode* ch_node = ch_ele->getFirstChild();
        char *ch = XMLString::transcode(ch_node->getTextContent());
        if (m_debug) {
            //std::cerr << "  ch:" << ch << std::endl;
        }

        int index = 0;
        std::string name = makeXPath(xpath, chName, index);
        val = ch;
        XMLString::release(&ch);
    }
    return val;
}

CompGroupList ConfFileParser::getGroupList()
{
   return m_groupList;
}

CompInfoList ConfFileParser::getCompList()
{
   return m_compList;
}

ParamList ConfFileParser::getParamList()
{
   return m_paramList;
}

void ConfFileParser::setSeq(std::vector<NameValue> vec, NVList& seq)
{
    if (m_debug) {
        std::cerr << "setSeq: enter" << std::endl;
    }
    int size = vec.size();

    for (int i = 0; i < size; ++i) {
        NameValue nv = vec.at(i);
        seq[i].name =  nv.name;
        seq[i].value = nv.value;
        if (m_debug) {
            std::cerr << "  nv( " << seq[i].name << ", " << seq[i].value 
                      << ")"<< std::endl;
        }
    }
}
