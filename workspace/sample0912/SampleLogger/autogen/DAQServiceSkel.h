// -*- C++ -*-
/*!
 *
 * THIS FILE IS GENERATED AUTOMATICALLY!! DO NOT EDIT!!
 *
 * @file DAQServiceSkel.h 
 * @brief DAQService server skeleton header wrapper code
 * @date Fri Sep  8 03:38:30 2017 
 *
 */

#ifndef _DAQSERVICESKEL_H
#define _DAQSERVICESKEL_H



#include <rtm/config_rtc.h>
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION

#if   defined ORB_IS_TAO
#  include "DAQServiceC.h"
#  include "DAQServiceS.h"
#elif defined ORB_IS_OMNIORB
#  if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#    undef USE_stub_in_nt_dll
#  endif
#  include "DAQService.hh"
#elif defined ORB_IS_MICO
#  include "DAQService.h"
#elif defined ORB_IS_ORBIT2
#  include "/DAQService-cpp-stubs.h"
#  include "/DAQService-cpp-skels.h"
#elif defined ORB_IS_RTORB
#  include "DAQService.h"
#else
#  error "NO ORB defined"
#endif

#endif // _DAQSERVICESKEL_H
