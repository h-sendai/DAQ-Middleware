/* File : ParameterClient.i */
%module ParameterClient

%{
#include "ParameterClient.h"
%}

%include stl.i
/*
%include "typemaps.i"
%include "std_string.i"
%include "cpointer.i"
*/

/* Let's just grab the original header file here */
/*
%apply string *OUTPUT {std::string* value};

%pointer_functions(int, intp);
%pointer_functions(string, strp);
*/

%include "ParameterClient.h"

extern int ParameterClient::put2(std::string id, std::string value, std::string& result);

