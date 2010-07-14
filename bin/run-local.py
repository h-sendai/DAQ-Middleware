#!/usr/bin/env python

# run.py: Start up script for DAQ-Components on a local host.
#         This script parses config.xml file, gets IP addrese of CPU DAQs and
#         CPU UI, and starts DAQ-Components on the local PC.

import sys, os, time, getopt
import popen2
import socket
from xml.dom.ext.reader import PyExpat
from xml.xpath import Evaluate

# adjust the daq_lib_path to your own environment. 
daq_lib_path = '/usr/lib/daqmw:$ROOTSYS/lib'
confFile = ''

# xinetd port
xinetdPort = 50000

#omniNames port
nsport = 9876

def usage():
    usage_message = """\
Usage: run.py [-h] [-c] [-d] [-f] [-x]
-h: Print usage and exit
-c: DAQ-Operator Console mode for debugging
-d: Specify a directory of DaqOperator
-f: configuration file name with abs. path
-x: schema file name with abs. path
"""
    sys.stderr.write(usage_message)
    
def getVal(confile, path):
    reader = PyExpat.Reader()
    text = open(confile).read()
    dom = reader.fromString(text)
    myValue = ''
    for node in Evaluate(path, dom.documentElement):
        myValue = node.firstChild.nodeValue
        print node.firstChild.nodeValue
    #return node.firstChild.nodeValue
    return myValue

def getVals(confile, path):
    vals = []
    reader = PyExpat.Reader()
    text = open(confile).read()
    dom = reader.fromString(text)
    for node in Evaluate(path, dom.documentElement):
        vals.append(node.firstChild.nodeValue)
    return vals

def mycommand(cmd):
    cp = popen2.Popen3(cmd,1)
    cp.tochild.close()
    child_stdout_fp = cp.fromchild
    child_stderr_fp = cp.childerr
    ret = cp.wait()

    if ( ret == 0):
        mess =  child_stdout_fp.read()
        mess += child_stderr_fp.read()
    else:
        mess = ''
        #mess =  child_stdout_fp.read()
    return mess

def sendData(addr, port, data):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect( (addr, port) )
    s.send(data)
    s.shutdown(socket.SHUT_WR)
    rdata = s.recv(1024)
    s.close()
    return rdata

def readFile(fname):
    fr = open(fname, 'r')
    text = fr.read()
    return text
    
def genConfFileForCpudaq(addr, operatorAddr, nsport, mydir):
    #fname = str(mydir) + '/rtc.conf.' + str(addr)
    fname = str(mydir) + '.' + str(addr)
    com = 'rm -f ' + fname
    os.system(com)
    fw = open(fname, 'w')
    conf = ''
    conf += '#rtc.conf\n'
    conf += 'corba.nameservers: ' + str(operatorAddr) + ':' + str(nsport) + '\n'
    conf += 'naming.formats: ' + str(addr) + '.host_cxt/%n.rtc\n'
    conf += 'logger.log_level: TRACE\n'
    conf += 'logger.enable: NO\n'
    conf += 'exec_cxt.periodic.rate: 1000000000\n'
    conf += 'corba.endpoint: ' + str(addr) + ':\n'
    fw.write(conf)
    fw.close()
    return conf

def genConfFileForOperator(operatorAddr, nsport, mydir):
    fname = str(mydir) + '/rtc.conf'
    com = 'rm -f ' + fname
    os.system(com)
    fw = open(fname, 'w')
    conf = ''
    conf += '#rtc.conf\n'    
    conf += 'corba.nameservers: ' + str(operatorAddr) + ': ' + str(nsport) + '\n'
    conf += 'naming.formats: '    + str(operatorAddr) + '.host_cxt/%n.rtc\n'
    conf += 'logger.log_level: TRACE\n'
    conf += 'logger.enable: NO\n'
    conf += 'exec_cxt.periodic.rate: 1000000000\n'
#    conf += 'corba.endpoint: ' + str(operatorAddr) + ':\n'
    fw.write(conf)
    fw.close()

def checkIpAddress(addr):
    awkcommand = "awk -F '[ |:]' '{print $13}'"
    mycomm = '/sbin/ifconfig -a|grep "inet " | ' + awkcommand
    addrsInIfconfig = []
    addrsInIfconfig = mycommand(mycomm)

    foundAddr = False
#    for i in addrs:
#        ret =  addrsInIfconfig.find(i)
#        if ret != -1:
#            foundAddr = True
    ret =  addrsInIfconfig.find(addr)
    if ret != -1:
        foundAddr = True

    return foundAddr
    

def opt():
    global operatorDir;
    global confFile;
    global schemaFile;
    try:
        opts, args = getopt.getopt(sys.argv[1:], "chd:f:x:")
    except getopt.GetoptError:
        usage();
        sys.exit(1)
    consMode = ''

    for o, a in opts:
        if o == "-c":
            consMode = '-c'
            print "Console mode "
        if o == "-h":
            usage();
            sys.exit(1)
        if o == "-d":
            operatorDir = a
            print "Operator Dir:", operatorDir
        if o == "-f":
            confFile = a
            print "confFile:", confFile
        if o == "-x":
            schemaFile = a
            print "schema File:", schemaFile
            
    if(confFile == ""):
        confFile = "./config.xml"
    return consMode
            
groupPath    = '/configInfo/daqGroups/daqGroup'
operatorPath = '/configInfo/daqOperator/hostAddr'
hostAddrPath = '/configInfo/daqGroups/daqGroup[@gid]/components/component[@cid]/hostAddr'
execPath     = '/configInfo/daqGroups/daqGroup[@gid]/components/component[@cid]/execPath'
confPath     = '/configInfo/daqGroups/daqGroup[@gid]/components/component[@cid]/confFile'

consMode = opt()

operatorAddr = getVal(confFile,  operatorPath)
cpudaqAddr   = getVals(confFile, hostAddrPath)
execPaths    = getVals(confFile, execPath)
confPaths    = getVals(confFile, confPath)

ret = False
ret = checkIpAddress(operatorAddr)
if ret == False:
    print 'IP Address for DAQ Operator is wrong. Not found in ifconfig: ', operatorAddr
    sys.exit(1)
else:
    print 'DAQ Operator addr', operatorAddr

for i in cpudaqAddr:
    ret = checkIpAddress(i)
    if ret == False:
        print 'IP Address for CPU DAQ  is wrong. Not found in ifconfig: ', i
        sys.exit(1)



for i in cpudaqAddr:
    print "CPU DAQ IP address: ", i

for i in execPaths:
    print "Exec Paths: ", i    

for i in confPaths:
    print "Config Paths: ", i        

#directory where temp files are created

mydir = '.'
print 'confFile:', confFile
cmd = 'xmllint --noout --schema ' + schemaFile + ' ' + confFile

print 'Validation command: ',cmd
ret = mycommand(cmd)
mess = ret.strip()

validated = confFile + ' validates'

if (mess == validated):
    print 'Validated:', confFile
else:
    print '### Validation failed:', confFile
    print '### Check the', confFile
    os.system(cmd)
    sys.exit(1)

ret = mycommand('killall omniNames')
ret = mycommand('pkill Comp')

time.sleep(2)

print 'start new naming service and wait for booting'
cmd = 'OMNIORB_USEHOSTNAME=' + str(operatorAddr) + ' rtm-naming ' +  str(nsport) + ' -logdir /tmp'
#print '======= operatorAddr', str(operatorAddr) 

os.system( cmd )

#os.chdir( '/home/daq/DaqComponents' )
time.sleep(2)

print 'generate rtc.conf for each CPU DAQ'
#confdir = '/tmp'

index = 0
for addr in cpudaqAddr:
    print 'CPUDAQ addr:', addr
    confdir = confPaths[index]
    print 'confdir',confdir
    conf = genConfFileForCpudaq(addr, operatorAddr, nsport, confdir)
    index += 1
#    print conf
        
print 'generate a rtc.conf for DaqOperator'
genConfFileForOperator(operatorAddr, nsport, mydir)


ldpath = os.getenv('LD_LIBRARY_PATH')
if (ldpath):
    mypath = ':' + daq_lib_path
    ldpath += mypath
else:
    ldpath = daq_lib_path

print 'CURRENT DIR: ', os.getcwd()

myldpath = 'LD_LIBRARY_PATH=' + ldpath + ' '

index = 0
for i in execPaths:
    print "booting: ", i
    #comm = myldpath + i + ' -f ' + confPaths[index] + '.' + operatorAddr + ' > /tmp/tt.log 2>&1 &'
    logfile = '/tmp/log.' + os.path.basename(execPaths[index])
    comm = myldpath + i + ' -f ' + confPaths[index] + '.' + operatorAddr + ' > ' + logfile + ' 2>&1 &'    
    os.system(comm)
    index+=1

time.sleep(6)

print 'booting the DAQ-Operator'

#print 'confFile:', confFile

com = 'LD_LIBRARY_PATH=' + ldpath + ' ' + str(operatorDir) + '/DaqOperatorComp -h ' + str(operatorAddr) + ' -p ' + str(nsport) + ' ' + '-x' + confFile + ' ' + str(consMode)

print 'command:', com
os.system( com )
