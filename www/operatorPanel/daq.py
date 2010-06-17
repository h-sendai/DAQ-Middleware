from mod_python import apache
import ParameterClient

file = "/home/daq/www/operatorPanel/client.conf"
host = "localhost"
port = 30000

xml_ver = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
xml_style = "<?xml-stylesheet href=\"style.xsl\" type=\"text/xsl\" ?>"

def putFunc(req, method, cmd):
	fileObj = open(file, "r")
	fileText = fileObj.readline().rstrip("\n")
	if fileText:
		host1 = fileText
		host = host1[5:]
	fileText = fileObj.readline().rstrip("\n")
	if fileText:
		port1 = fileText
		port = int(port1[5:])
	
	client = ParameterClient.ParameterClient(host, port)
	result = client.new_strp()
	ret = client.put2(method, cmd, result)
	
	status = xml_ver
#	req.content_type = "text/xml"
	if (req.headers_in.has_key('User-Agent')):
		status += xml_style
	status += client.strp_value(result)
	
	req.write(status)
	return

def getFunc(req, method):
	fileObj = open(file, "r")
	fileText = fileObj.readline().rstrip("\n")
	if fileText:
		host1 = fileText
		host = host1[5:]
	fileText = fileObj.readline().rstrip("\n")
	if fileText:
		port1 = fileText
		port = int(port1[5:])
	
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
	if req.method == 'GET':
		getFunc(req, "Status")

def Params(req, cmd=None):
	if req.method == 'POST':
		putFunc(req, "Params", cmd)
	if req.method == 'GET':
		getFunc(req, "Params")

def StopParamsSet(req, cmd=None):
	if req.method == 'POST':
		putFunc(req, "StopParamsSet", cmd)

def Reset(req, cmd=None):
	if req.method == 'POST':
		putFunc(req, "Reset", cmd)

def ResetParams(req, cmd=None):
	if req.method == 'POST':
		putFunc(req, "ResetParams", cmd)

def Begin(req, cmd=None):
	if req.method == 'POST':
		putFunc(req, "Begin", cmd)

def End(req, cmd=None):
	if req.method == 'POST':
		putFunc(req, "End", cmd)

def Pause(req, cmd=None):
	if req.method == 'POST':
		putFunc(req, "Pause", cmd)

def Save(req, cmd=None):
	if req.method == 'POST':
		putFunc(req, "Save", cmd)

def Restart(req, cmd=None):
	if req.method == 'POST':
		putFunc(req, "Restart", cmd)

def Abort(req, cmd=None):
	if req.method == 'POST':
		putFunc(req, "Abort", cmd)

def ConfirmEnd(req, cmd=None):
	if req.method == 'POST':
		putFunc(req, "ConfirmEnd", cmd)

def Log(req, cmd=None):
	if req.method == 'GET':
		getFunc(req, "Log")

def ConfirmConnection(req, cmd=None):
	if req.method == 'POST':
		putFunc(req, "ConfirmConnection", cmd)

