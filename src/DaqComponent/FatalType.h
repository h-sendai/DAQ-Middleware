// -*- C++ -*-
/*!
 * @file FatalType.h
 * @brief Definition of Fata errors
 * @date 2008-1-1
 * @author Kazuo Nakayoshi (kazuo.nakayoshi@kek.jp)
 *
 * Copyright (C) 2008-2011
 *     Kazuo Nakayoshi
 *     High Energy Accelerator Research Organization (KEK), Japan.
 *     All rights reserved.
 *
 */
#ifndef FATALTYPE_H
#define FATALTYPE_H

/*!
 * @namespace DAQMW
 * @brief common namespace of DAQ-Middleware
 */
namespace DAQMW
{
  /*!
   * @class FatalType
   * @brief FatalType class
   * 
   * 
   *
   */
    namespace FatalType
    {
        enum Enum
        {
            ///DAQ-Middleware defined fatal error
            ///use following function
            ///fatal_error_report(FatalTypes types, int code)
            /// e.g. fatal_error_report(HEADER_DATA_MISMATCH, -1)

            ///header, footer error
            HEADER_DATA_MISMATCH,
            FOOTER_DATA_MISMATCH,
            SEQUENCE_NUM_MISMATCH,

            ///configuration file
            CANNOT_OPEN_CONFIGFILE,
            CONFIGFILE_PARSE_ERROR,
            NO_CONFIG_PARAMS,

            ///condition file
            CANNOT_OPEN_COND_FILE,
            COND_FILE_PARSE_ERROR,

            ///command/status path error
            CANNOT_CONNECT_COMMANDPATH,
            COMMANDPATH_DISCONNECTED,

            ///data path error
            CANNOT_CONNECT_DATAPATH,
            DATAPATH_DISCONNECTED,

            ///InPort/OutPort error
            INPORT_ERROR,
            OUTPORT_ERROR,

            ///wrong parameters, such as command line options, etc.
            BAD_PARAMETER,

            ///readout module-related error
            CANNOT_CONNECT_DATA_SRC,
            TOO_MANY_DATA_FROM_DATA_SRC,
            READOUT_ERROR,

            ///file I/O error
            BAD_DIR,
            CANNOT_MAKE_DIR,
            CANNOT_OPEN_FILE,
            CANNOT_WRITE_DATA,

            ///user defined fatal error (user defined error1 - error20)
            ///users can choose a below error and its description by string.
            ///fatal_error_report(FatalTypes types, std::string desc, int code)
            /// e.g. 
            /// fatal_error_report(USER_DEFINED_ERROR1,
            ///                                "My fatal error detail", -1)
            USER_DEFINED_ERROR1,
            USER_DEFINED_ERROR2,
            USER_DEFINED_ERROR3,
            USER_DEFINED_ERROR4,
            USER_DEFINED_ERROR5,
            USER_DEFINED_ERROR6,
            USER_DEFINED_ERROR7,
            USER_DEFINED_ERROR8,
            USER_DEFINED_ERROR9,
            USER_DEFINED_ERROR10,
            USER_DEFINED_ERROR11,
            USER_DEFINED_ERROR12,
            USER_DEFINED_ERROR13,
            USER_DEFINED_ERROR14,
            USER_DEFINED_ERROR15,
            USER_DEFINED_ERROR16,
            USER_DEFINED_ERROR17,
            USER_DEFINED_ERROR18,
            USER_DEFINED_ERROR19,
            USER_DEFINED_ERROR20,
            
            ///reboot_message
            REBOOT,
            REBOOT_REQUEST,

            ///unknown error
            UNKNOWN_FATAL_ERROR,
        };

        static const char* toString(Enum fatalTypes)
        {
            const char* strType[] = {
                "HEADER_DATA_MISMATCH",
                "FOOTER_DATA_MISMATCH",
                "SEQUENCE_NUM_MISMATCH",

                "CANNOT_OPEN_CONFIGFILE",
                "CONFIGFILE_PARSE_ERROR",
                "NO_CONFIG_PARAMS",

                "CANNOT_OPEN_COND_FILE",
                "COND_FILE_PARSE_ERROR",

                "CANNOT_CONNECT_COMMANDPATH",
                "COMMANDPATH_DISCONNECTED",

                "CANNOT_CONNECT_DATAPATH",
                "DATAPATH_DISCONNECTED",

                "INPORT_ERROR",
                "OUTPORT_ERROR",

                "BAD_PARAMETER",

                "CANNOT_CONNECT_DATA_SRC",
                "TOO_MANY_DATA_FROM_DATA_SRC",
                "READOUT_ERROR",

                "BAD_DIR",
                "CANNOT_MAKE_DIR",
                "CANNOT_OPEN_FILE",
                "CANNOT_WRITE_DATA",

                "USER_DEFINED_ERROR1",
                "USER_DEFINED_ERROR2",
                "USER_DEFINED_ERROR3",
                "USER_DEFINED_ERROR4",
                "USER_DEFINED_ERROR5",
                "USER_DEFINED_ERROR6",
                "USER_DEFINED_ERROR7",
                "USER_DEFINED_ERROR8",
                "USER_DEFINED_ERROR9",
                "USER_DEFINED_ERROR10",
                "USER_DEFINED_ERROR11",
                "USER_DEFINED_ERROR12",
                "USER_DEFINED_ERROR13",
                "USER_DEFINED_ERROR14",
                "USER_DEFINED_ERROR15",
                "USER_DEFINED_ERROR16",
                "USER_DEFINED_ERROR17",
                "USER_DEFINED_ERROR18",
                "USER_DEFINED_ERROR19",
                "USER_DEFINED_ERROR20",

                "REBOOT",
                "REBOOT_REQUEST",

                "UNKNOWN_FATAL_ERROR",
            };
            return strType[fatalTypes];
        }
    }
}
#endif
