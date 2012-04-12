#!/usr/bin/env python

"""Server program based on mod_python for DaqOperator

The Server program supports DaqOperator commands with XML/HTTP protocol.
"""
__author__   = 'Yoshiji Yasu (yoshiji.yasu@kek.jp)'
__version__  = '1.0'
__date__     = '26-May-2011'

from mod_python import apache
import ParameterClient

host = "localhost"
port = 30000

xml_ver = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
xml_style = "<?xml-stylesheet href=\"style.xsl\" type=\"text/xsl\" ?>"

def putFunc(req, method, cmd):
	"""Internal function for HTTP POST"""
	client = ParameterClient.ParameterClient(host, port)
	result = client.new_strp()
	ret = client.put(method, cmd, result)
	
	status = xml_ver
	if (req.headers_in.has_key('User-Agent')):
		status += xml_style
	status += client.strp_value(result)

	req.write(status)
	return

def getFunc(req, method):
	"""Internal function for HTTP GET"""
	client = ParameterClient.ParameterClient(host, port)
	result = client.new_strp()
	ret = client.get(method, result)

	status = xml_ver
	req.content_type = "text/xml"
	req.headers_out['Pragma'] = 'no-cache'
	req.headers_out['Cache-Control'] = 'no-cache'
	req.headers_out['Expires'] = '-1'

	if (req.headers_in.has_key('User-Agent')):
		status += xml_style
	status += client.strp_value(result)

	req.write(status)
	return

def Status(req, cmd=None):
	"""Status command"""
	if req.method == 'GET':
		getFunc(req, "Status")

def Params(req, cmd=None):
	"""Set Parameter command of HTTP POST
	Get Parametger command of HTTP GET
	"""
	if req.method == 'POST':
		putFunc(req, "Params", cmd)
	if req.method == 'GET':
		getFunc(req, "Params")

def StopParamsSet(req, cmd=None):
	"""StopParameterSet command"""
	if req.method == 'POST':
		putFunc(req, "StopParamsSet", cmd)

def Reset(req, cmd=None):
	"""Reset command"""
	if req.method == 'POST':
		putFunc(req, "Reset", cmd)

def ResetParams(req, cmd=None):
	if req.method == 'POST':
		putFunc(req, "ResetParams", cmd)

def Begin(req, cmd=None):
	"""Begin or Start of DAQ command"""
	if req.method == 'POST':
		putFunc(req, "Begin", cmd)

def End(req, cmd=None):
	"""End or Stop of DAQ command"""
	if req.method == 'POST':
		putFunc(req, "End", cmd)

def Pause(req, cmd=None):
	"""Pause of DAQ command"""
	if req.method == 'POST':
		putFunc(req, "Pause", cmd)

def Save(req, cmd=None):
	"""Save command"""
	if req.method == 'POST':
		putFunc(req, "Save", cmd)

def Restart(req, cmd=None):
	"""Restart or Resume of DAQ command"""
	if req.method == 'POST':
		putFunc(req, "Restart", cmd)

def Abort(req, cmd=None):
	"""Abort command"""
	if req.method == 'POST':
		putFunc(req, "Abort", cmd)

def ConfirmEnd(req, cmd=None):
	"""ConfirmEnd command"""
	if req.method == 'POST':
		putFunc(req, "ConfirmEnd", cmd)

def Log(req, cmd=None):
	"""Log command"""
	if req.method == 'GET':
		getFunc(req, "Log")

def ConfirmConnection(req, cmd=None):
	"""ConfirmConnection command"""
	if req.method == 'POST':
		putFunc(req, "ConfirmConnection", cmd)
