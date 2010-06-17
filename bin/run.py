#!/usr/bin/env python

# run.py: Start up script for DAQ-Components.
#         This script parses config.xml file, gets IP addrese of CPU DAQs and
#         CPU UI, and starts DAQ-Components on remote/local PCs via network.

import sys, os, time, getopt
import popen2
import socket
from xml.dom.ext.reader import PyExpat
from xml.xpath import Evaluate
from xml.dom.minidom import parse

# adjust the daq_lib_path to your own environment. 
daq_lib_path = '/home/daq/lib:/kensdaq/root/lib'
confFile = ''

# xinetd port
xinetdPort = 50000

#omniNames port
nsport = 9876

def usage():
    usage_message = """\
Usage: run.py [-h] [-c] [-f] [-x]
-h: Print usage and exit
-c: DAQ-Operator Console mode for debugging
-f: configuration file name with abs. path
-x: XML schema file with abs. path
"""
    sys.stderr.write(usage_message)
    
def getVal(confile, path):
    reader = PyExpat.Reader()
    text = open(confile).read()
    dom = reader.fromString(text)
    for node in Evaluate(path, dom.documentElement):
        print node.firstChild.nodeValue
    return node.firstChild.nodeValue

def getVals(confile, path):
    vals = []
    reader = PyExpat.Reader()
    text = open(confile).read()
    dom = reader.fromString(text)
    for node in Evaluate(path, dom.documentElement):
        vals.append(node.firstChild.nodeValue)
    return vals

def mycommand2(cmd):
    child = os.popen(cmd)
    time.sleep(0.1)
    data  = child.read()
    print 'mycommand2: data: ', data
    err   = child.close()
    if err:
        return 'Runtime Error: ', err

    
def mycommand(cmd):
    cp = popen2.Popen3(cmd,1)
    cp.tochild.close()
    child_stdout_fp = cp.fromchild
    child_stderr_fp = cp.childerr
    ret = cp.wait()

    if ( ret == 0):
        mess =  child_stdout_fp.read()
        mess += child_stderr_fp.read()
        #print 'mycommand mess:', mess
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
    os.system('pwd')
    fr = open(fname, 'r')
    text = fr.read()
    return text
    
def genConfFileForCpudaq(addr, operatorAddr, nsport, mydir):
    fname = str(mydir) + '/rtc.conf.' + str(addr)
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
    conf += 'corba.endpoints: ' + str(addr) + ':\n'
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

def genRunCompsFile(bindir, confdir, mydir):
    fname = str(mydir) + '/run-comps.sh'
    com = 'rm -f ' + fname
    os.system(com)
    fw = open(fname, 'w')
    runcomps = ''
    runcomps += '#comps.sh\n'
    runcomps += 'pkill -f Comp\n'
    runcomps += 'export MANYOLIB=/home/daq/manyo\n'
    runcomps += 'export SOCKLIB=/home/daq/lib\n'
    runcomps += 'export LD_LIBRARY_PATH=$MANYOLIB:$SOCKLIB:$LD_LIBRARY_PATH:\n'    
    fw.write(conf)
    fw.close()

def getExecPath(confFile):
    dom = parse(confFile)
    root = dom.documentElement
    glist = root.getElementsByTagName('daqGroup')
    allExecPaths = []

    for g in glist:
        epaths = g.getElementsByTagName('execPath')
        execPaths = []
        if len(epaths) > 0:
            for epath in epaths:
                epathdat = epath.childNodes
                for dat in epathdat:
                    if dat.nodeType == dat.TEXT_NODE:
                        print ' ', dat.nodeValue
                        execPaths.append(dat.nodeValue)
        allExecPaths.append(execPaths)
        
    dom.unlink()
    return allExecPaths


def opt():
    global confFile;
    global schemaFile;
    try:
        opts, args = getopt.getopt(sys.argv[1:], "chf:x:")
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
#hostAddrPath = '/configInfo/daqGroups/daqGroup[@gid]/components/component[@cid="Gatherer0"]/hostAddr'
hostAddrPath = '/configInfo/daqGroups/daqGroup[@gid]/components/component[@cid]/hostAddr'
compExecPath = '/configInfo/daqGroups/daqGroup[@gid]/components/component[@cid]/execPath'
confPath     = '/configInfo/daqGroups/daqGroup[@gid]/components/component[@cid]/confFile'

consMode = opt()
print "cons_mode:", str(consMode)

operatorAddr = getVal(confFile, operatorPath)
cpudaqAddrs  = getVals(confFile, hostAddrPath)
#execPaths    = getVals(confFile, compExecPath)
confPaths    = getVals(confFile, confPath)

for i in cpudaqAddrs:
    print "CPU DAQ IP address: ", i

cpudaqAddr = []
cpudaqAddr = list(set(cpudaqAddrs))

print "$$$ cpu daq addr:", cpudaqAddr

#directory where temp files are created
mydir = '.'
print 'confFile:', confFile
#cmd = 'xmlwf -s ' + mydir + '/config.xsd ' + confFile
#cmd = 'xmllint --noout --schema ' + mydir + '/config.xsd ' + confFile
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

time.sleep(2)

print 'start new naming service and wait for booting'
cmd = 'OMNIORB_USEHOSTNAME=' + str(operatorAddr) + ' rtm-naming ' +  str(nsport) + ' -logdir /tmp'

os.system( cmd )

time.sleep(2)

print 'generate rtc.conf for each CPU DAQ'
for addr in cpudaqAddr:
    print 'CPUDAQ addr:', addr
    conf = genConfFileForCpudaq(addr, operatorAddr, nsport, mydir)
    sendData(addr, xinetdPort, conf)
        
print 'generate a rtc.conf for DaqOperator'
genConfFileForOperator(operatorAddr, nsport, mydir)

#fname = 'run-comps.sh' 
#dat = readFile(fname)

ldpath = os.getenv('LD_LIBRARY_PATH')
if (ldpath):
    mypath = ':' + daq_lib_path
    ldpath += mypath
else:
    ldpath = daq_lib_path

#print 'CURRENT DIR: ', os.getcwd()
print 'ldpath: ', ldpath

myldpath = 'LD_LIBRARY_PATH=' + ldpath + ' '

execPaths = getExecPath(confFile)

print 'execPaths: ', execPaths
print 'confPaths: ', confPaths
print 'cpudaqAddr:', cpudaqAddr

index = 0
for addr in cpudaqAddr:
    runcomps = ''
    print '*********************************'
    runcomps += '#comps.sh\n'
    runcomps += 'pkill -f Comp\n'
#    runcomps += '#export MANYOLIB=/home/daq/manyo\n'
#    runcomps += 'export ROOTSYS=/kensdaq/root\n'
#    runcomps += 'export SOCKLIB=/home/daq/lib\n'
#    runcomps += 'export ROOTLIB=/kensdaq/root/lib\n'
    mydisp = ''
    mydisp = 'export DISPLAY=' + addr + ':0\n'

#    runcomps += 'export DISPLAY=192.168.1.207:0\n'
#    runcomps += 'export DISPLAY=192.168.1.203:0\n'
    
    runcomps += mydisp
    runcomps += 'export LD_LIBRARY_PATH=$MANYOLIB:$SOCKLIB:$ROOTLIB:$LD_LIBRARY_PATH:\n'
    
    for i in execPaths[index]:
        print i
#        logfile = '/tmp/log.' + os.path.basename(execPaths[index])
        logfile = '/tmp/log.' + os.path.basename(i)
#        comp = myldpath + i + ' -f ' + confPaths[index] + '.' + addr + ' > ' + logfile + ' 2>&1 &\n'
        comp = myldpath + i + ' -f ' + confPaths[index] + ' > ' + logfile + ' 2>&1 &\n'        
        runcomps += comp
    index += 1
    
    print 'booting the DAQ-Components on each CPU DAQ'
    #for addr in cpudaqAddr:
    print '  send to ', addr
    #sendData(addr[index], xinetdPort, runcomps)
    sendData(addr, xinetdPort, runcomps)
    print runcomps
    print '*********************************'
time.sleep(6)

print 'booting the DAQ-Operator'

ldpath = os.getenv('LD_LIBRARY_PATH')
if (ldpath):
    mypath = ':' + daq_lib_path
    ldpath += mypath
else:
    ldpath = daq_lib_path

#print 'confFile:', confFile
#com = 'LD_LIBRARY_PATH=' + ldpath + ' ' + mydir + '/bin/DaqOperatorComp -h ' + str(operatorAddr) + ' -p ' + str(nsport) + ' ' + '-x' + confFile + ' ' + str(consMode)
com = 'LD_LIBRARY_PATH=' + ldpath + ' ' + mydir + '/bin/DaqOperatorComp -h ' + str(operatorAddr) + ' -p ' + str(nsport) + ' ' + '-x' + confFile + ' ' + str(consMode)

print com
os.system( com )


    
