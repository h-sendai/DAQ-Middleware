// -*- C++ -*-
#ifndef DAQCOMPEXCEPTION_H
#define DAQCOMPEXCEPTION_H

#include <exception>
#include "FatalType.h"

namespace DAQMW
{
    class DaqComponentException 
	: public std::exception
    {
    public:
	DaqComponentException()
	    : m_code(-1), m_daqFatalTypes(FatalType::UNKNOWN_FATAL_ERROR), 
	      m_msg(""), m_date("")
	{

	}

	explicit DaqComponentException(const FatalType::Enum fatalTypes, int code = -1)
	    : m_code(code), m_daqFatalTypes(fatalTypes), m_msg(""), m_date("")
	{
	    //m_msg = DAQMW::FatalType::toString(fatalTypes);
	    time_t now = time(0);
	    tm* local = localtime(&now);
	    m_date = asctime(local);
	    m_date.replace(m_date.find('\n'),1,"");
	}

	explicit DaqComponentException(const FatalType::Enum fatalTypes, std::string desc,
				       int code = -1)
	    : m_code(code), m_daqFatalTypes(fatalTypes), m_msg(desc), m_date("")
	{
	    //(const)m_msg = FatalType::toString(fatalTypes);
	    time_t now = time(0);
	    tm* local = localtime(&now);
	    m_date = asctime(local);
	    m_date.replace(m_date.find('\n'),1,"");
	}

	virtual ~DaqComponentException() throw()
	{

	}

	virtual const char* what() const throw()
	{
	    //m_msg = 
	    //std::cerr << "m_msg: " << m_msg << std::endl;
	    //m_msg = msg;
	    //std::cerr << ">>>> " << FatalType::toString(m_daqFatalTypes) << std::endl;

	    return FatalType::toString(m_daqFatalTypes);
	}

	FatalType::Enum type() const throw()
	{
	    return m_daqFatalTypes;
	}

	int reason() const throw() 
	{
	    return m_code;
	}

    protected:
	int m_code;
	FatalType::Enum m_daqFatalTypes;
	const std::string m_msg;
	std::string m_date;
    };

    class DaqCompDefinedException : public DaqComponentException
    {
    public:
	DaqCompDefinedException(const FatalType::Enum fatalTypes, int code = -1)
	    : DaqComponentException(fatalTypes, code)
	{

	}
    };

    class DaqCompUserException : public DaqComponentException
    {
    public:
	DaqCompUserException(const FatalType::Enum fatalTypes, const char* description, 
			     int code = -1)

	    : DaqComponentException(fatalTypes, description, code)
	{

	}

	virtual const char* what() const throw()
	{
	    return m_msg.c_str();
	}
    };
}
#endif
