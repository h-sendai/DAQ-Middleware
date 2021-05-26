#!/usr/bin/env python

"""Server program based on mod_wsgi for DaqOperator

The Server program supports DaqOperator commands with XML/HTTP protocol.
"""
__author__   = 'Yoshiji Yasu (yoshiji.yasu@kek.jp)'
__version__  = '0.96'
__date__     = '11-April-2012'

import os
import sys
import cgi
import ParameterClient

host = "localhost"
port = 30000

xml_ver = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
xml_style = "<?xml-stylesheet href=\"../../operatorPanel/style.xsl\" type=\"text/xsl\" ?>"

def putFunc(method, cmd):
	"""Internal function for HTTP POST"""
	client = ParameterClient.ParameterClient(host, port)
	result = client.new_strp()
	ret = client.put(method, cmd, result)
	
	output = xml_ver + xml_style
	output += client.strp_value(result)
	
	response_headers = [('Content-type', 'text/xml'),
			    ('Content-Length', str(len(output)))]
	return response_headers, output

def getFunc(method):
	"""Internal function for HTTP GET"""
	client = ParameterClient.ParameterClient(host, port)
	result = client.new_strp()
	ret = client.get(method, result)

	output = xml_ver + xml_style
	output += client.strp_value(result)

	response_headers = [('Content-type', 'text/xml'),
			    ('Pragma', 'no-cache'),
			    ('Cache-Control', 'no-cache'),
			    ('Expires', '-1'),
			    ('Content-Length', str(len(output)))]
        return response_headers, output

def Status(req, cmd=None):
	"""Status command"""
	if req == 'GET':
		return getFunc("Status")

def Params(req, cmd=None):
	"""Set Parameter command of HTTP POST
	Get Parametger command of HTTP GET
	"""
	if req == 'POST':
		return putFunc("Params", cmd)
	if req == 'GET':
		return getFunc(req, "Params")

def StopParamsSet(req, cmd=None):
	"""StopParameterSet command"""
	if req == 'POST':
		return putFunc("StopParamsSet", cmd)

def Reset(req, cmd=None):
	"""Reset command"""
	if req == 'POST':
		return putFunc("Reset", cmd)

def ResetParams(req, cmd=None):
	if req == 'POST':
		return putFunc("ResetParams", cmd)

def Begin(req, cmd=None):
	"""Begin or Start of DAQ command"""
	if req == 'POST':
		return putFunc("Begin", cmd)

def End(req, cmd=None):
	"""End or Stop of DAQ command"""
	if req == 'POST':
		return putFunc("End", cmd)

def Pause(req, cmd=None):
	"""Pause of DAQ command"""
	if req == 'POST':
		return putFunc("Pause", cmd)

def Save(req, cmd=None):
	"""Save command"""
	if req == 'POST':
		return putFunc("Save", cmd)

def Restart(req, cmd=None):
	"""Restart or Resume of DAQ command"""
	if req == 'POST':
		return putFunc("Restart", cmd)

def Abort(req, cmd=None):
	"""Abort command"""
	if req == 'POST':
		return putFunc("Abort", cmd)

def ConfirmEnd(req, cmd=None):
	"""ConfirmEnd command"""
	if req == 'POST':
		return putFunc("ConfirmEnd", cmd)

def Log(req, cmd=None):
	"""Log command"""
	if req == 'GET':
		return getFunc("Log")

def ConfirmConnection(req, cmd=None):
	"""ConfirmConnection command"""
	if req == 'POST':
		return putFunc("ConfirmConnection", cmd)

def application(environ, start_response):
	subroutine = {
                'Status': Status,
                'Params': Params,
                'Begin': Begin,
                'End' : End,
                'Restart' : Restart,
                'Pause' : Pause,
                'Log' : Log,
                'StopParamsSet': StopParamsSet,
                'Reset': Reset,
                'ResetParams' : ResetParams,
                'Save' : Save,
                'Abort' : Abort,
                'ConfirmEnd' : ConfirmEnd,
                'ConfirmConnection' : ConfirmConnection
		}
	status = '200 OK'
	req = environ['REQUEST_METHOD']
	path = environ['PATH_INFO']
	method = path[1:]  # extract '/'
	os.environ['PARAMETER_CLIENT_TIMEOUT'] = environ.get('PARAMETER_CLIENT_TIMEOUT', '20')
	# print >>  sys.stderr, "request = ", req, "  method = ", method
	if req == "GET":
		params = cgi.parse_qsl(environ['QUERY_STRING'])
	if req == "POST":
		wsgi_input     = environ['wsgi.input']
		content_length = int(environ.get('CONTENT_LENGTH', 0))
		params = cgi.parse_qsl(wsgi_input.read(content_length))
	cmd =''
	# print >> sys.stderr, "params ", params
	if params != []:
		for param in params:
			# print >> sys.stderr, "param[0] ", param[0]
			if param[0] == 'cmd':
				cmd = param[1]
				# print >> sys.stderr,"OK ", cmd
	response_headers, output =  subroutine[method](req, cmd)
	start_response(status, response_headers)
	return [output]
