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
daq_lib_path = '/usr/lib/daqmw:/kensdaq/root/lib'
confFile = ''

# xinetd port for remote booting
xinetdPort = 50000

# omniNames port
nsport = 9876

# Options
opt_args = ["help",
            "console",
            "local",
            "file_of_config=",
            "schema=",
            "operator=",
            "display="]

def usage():
    print """
Usage: run.py [OPTIONS]

  -h, --help                      Print usage.
  -c, --console                   Console mode. Default setting is HTTP mode.
  -l, --local                     Local booting mode. Default is remote booting mode.
  -f, --file_of_config=conf_file  Specify config file with full path.
  -s, --schema=schema_file        Specify XML schema file for config with full path.
  -o, --operator=operator_path    Specify DaqOperatorComp with full path.
  -d, --display=display_env       Specify DISPLAY env. for X apps, such as ROOT for monitoring.

Examples:

  run.py -f xxx/conf.xml       : remote booting using config file xxx/conf.xml with http mode
  run.py -f xxx/conf.xml -c    : remote booting using config file xxx/conf.xml with console mode
  run.py -f xxx/conf.xml -l    : local booting using config file xxx/conf.xml with http mode
  run.py -f xxx/conf.xml -c -l : local booting using config file xxx/conf.xml with console mode
"""

def opt():
    global opt_args
    global consoleMode
    global localBoot
    global confFile
    global schemaFile
    global operator
    global mydisplay

    consoleMode = False
    localBoot   = False
    schemaFile  = '/usr/share/daqmw/conf/config.xsd'
    operator    = '/usr/libexec/daqmw/DaqOperatorComp'

    try:
        opts, args = getopt.getopt(sys.argv[1:], "hclf:s:o:d:", opt_args)
    except getopt.GetoptError:
        print "Error: Invalid option."
        usage();
        sys.exit(-1)

    for o, a in opts:
        if o in ("-h", "--help"):
            usage();
            sys.exit(0)
        if o in ("-c", "--console"):
            consoleMode = True
            print "Console mode "
        if o in ("-l", "--local"):
            localBoot = True
            print "Local boot mode"
        if o in ("-f", "--file_of_config"):
            confFile  = a
            print confFile
        if o in ("-s", "--schema"):
            schemaFile = a
        if o in ("-o", "--operator"):
            operator = a
        if o in ("-d", "--display"):
            mydisplay = a

    if not opts:
        usage()
        sys.exit(-1)

    if confFile == '':
        print 'ERROR: Configuration file not specified.'
        sys.exit(-1)
    if os.path.exists(confFile):
        print 'Use ' + confFile + ' for configuration'
    else:
        print 'ERROR: ' + confFile + ' not exists. exit.'
        sys.exit(-1)
    if os.path.exists(schemaFile):
        print 'Use ' + schemaFile + ' for XML schema'
    else:
        print 'ERROR: ' + schemaFile + ' not exists. exit.'
        sys.exit(-1)
    if os.path.exists(operator):
        print 'Use ' + operator + ' for DAQ-Operator'
    else:
        print 'ERROR: ' + operator + ' not exists. exit.'
        sys.exit(-1)

def isFileExist(fname):
    if os.path.exists(schemaFile):
        return True
    else:
        return False

def getVal(confile, path):
    reader = PyExpat.Reader()
    text = open(confile).read()
    dom = reader.fromString(text)
    myVal = ''
    for node in Evaluate(path, dom.documentElement):
        myVal = node.firstChild.nodeValue
        print node.firstChild.nodeValue
    #return node.firstChild.nodeValue
    return myVal

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
    else:
        mess = ''
    return mess

def sendData(addr, port, data):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    rdata = ''
    try:
        s.connect( (addr, port) )
        s.send(data)
        s.shutdown(socket.SHUT_WR)
        rdata = s.recv(1024)
        print "received data:", rdata
        s.close()
    except socket.error, msg:
        s.close()
        print 'socket error occured'
    return rdata

def readFile(fname):
    os.system('pwd')
    fr = open(fname, 'r')
    text = fr.read()
    return text

# generate rtc.conf for remote CPU for DAQ-Components
def genConfFileForCpudaq(addr, operatorAddr, nsport, mydir):
    #fname = str(mydir) + '/rtc.conf.' + str(addr)
    fname = str(mydir) + '/rtc.conf'
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

# generate rtc.conf for DaqOperator
def genConfFileForOperator(operatorAddr, nsport, mydir):
    fname = str(mydir) + '/rtc.conf.operator'
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

def validateConfigFile(confFile, schemaFile):
    cmd = 'xmllint --noout --schema ' + schemaFile + ' ' + confFile

    print 'Validation command: ',cmd
    ret = mycommand(cmd)
    mess = ret.strip()

    validated = confFile + ' validates'

    if (mess == validated):
        #print 'Validated:', confFile
        return True
    else:
        print '### ERROR: Validation failed:', confFile
        print '### Check the', confFile
        os.system(cmd) # for error message
        #sys.exit(-1)
        return False

def localCompsBooting():
    execPaths    = getVals(confFile, compExecPath)
    mypkill = 'pkill -f "^[^ ]+Comp "'
    os.system(mypkill)  # kill previous components
    index = 0
    for i in execPaths:
        print "booting: ", i
        logfile = daqmw_dir +'/log.' + os.path.basename(execPaths[index])
        #comm = myldpath + i + ' -f ' + confPaths[index] + '.' + operatorAddr + ' > ' + logfile + ' 2>&1 &'
        comm = myldpath + i + ' -f ' + confPaths[index] + ' > ' + logfile + ' 2>&1 &'
        print comm
        os.system(comm)
        index+=1

def remoteCompsBooting():
    execPaths = getExecPath(confFile)

    index = 0
    for addr in cpudaqAddr:
        runcomps = ''
        runcomps += '#comps.sh\n'
        runcomps += 'pkill -f "^[^ ]+Comp "\n'
        mydisp = 'export DISPLAY=' + mydisplay + '\n'
        runcomps += mydisp
#        runcomps += 'export HOME=/home/nakayosi\n'
#        runcomps += 'export LD_LIBRARY_PATH=$MANYOLIB:$SOCKLIB:$ROOTLIB:/usr/lib/daqmw:$LD_LIBRARY_PATH:\n'

        for i in execPaths[index]:
            print i
            logfile = daqmw_dir + '/log.' + os.path.basename(i)
            comp = myldpath + i + ' -f ' + confPaths[index] + ' > ' + logfile + ' 2>&1 &\n'
            runcomps += comp
        index += 1

        print 'booting the DAQ-Components on each CPU DAQ'
        #for addr in cpudaqAddr:
        print '  send to ', addr
        rdata = sendData(addr, xinetdPort, runcomps)
        #print rdata
        #print runcomps

def main():
    global compExecPath
    global operatorAddr
    global cpudaqAddr
    global confPaths
    global daqmw_dir

    groupPath    = '/configInfo/daqGroups/daqGroup'
    operatorPath = '/configInfo/daqOperator/hostAddr'
    hostAddrPath = '/configInfo/daqGroups/daqGroup[@gid]/components/component[@cid]/hostAddr'
    compExecPath = '/configInfo/daqGroups/daqGroup[@gid]/components/component[@cid]/execPath'
    confPath     = '/configInfo/daqGroups/daqGroup[@gid]/components/component[@cid]/confFile'

    # get command line options
    opt()

    operatorAddr = getVal(confFile, operatorPath)

    #operatorAddrs = getVals(confFile, operatorPath)

    if operatorAddr == '':
        print 'ERROR: not specified IP address of DaqOperator in configuration file'
        sys.exit(-1)

    cpudaqAddrs  = getVals(confFile, hostAddrPath)
    confPaths    = getVals(confFile, confPath)

    for i in cpudaqAddrs:
        print "CPU DAQ IP address: ", i

    cpudaqAddr = list(set(cpudaqAddrs))

    daqmw_dir = '/tmp/daqmw'

    if os.path.exists(daqmw_dir) == False:
        os.mkdir(daqmw_dir)
        print '/tmp/daqmw was created'
    else:
        print '/tmp/daqmw is exist'

    # validation of configuration file using XML schema config.xsd
    if validateConfigFile(confFile, schemaFile):
        print 'Validated:', confFile
    else:
        print '### ERROR: Validation failed:', confFile
        print '### Check the', confFile
        os.system(cmd) # for error message
        sys.exit(-1)

    # kill old naming service
    ret = mycommand('killall omniNames')
    time.sleep(2)

    print 'start new naming service and wait for booting'
    cmd = 'OMNIORB_USEHOSTNAME=' + str(operatorAddr) + ' rtm-naming ' +  str(nsport) + ' -logdir /tmp'

    os.system( cmd )

    time.sleep(2)

    # generate a rtc.conf for DaqOperator
    genConfFileForOperator(operatorAddr, nsport, daqmw_dir)

    ldpath = os.getenv('LD_LIBRARY_PATH')
    if (ldpath):
        mypath = ':' + daq_lib_path
        ldpath += mypath
    else:
        ldpath = daq_lib_path

    #print 'CURRENT DIR: ', os.getcwd()
    print 'ldpath: ', ldpath

    global myldpath
    myldpath = 'LD_LIBRARY_PATH=' + ldpath + ' '

    if localBoot: # local DAQ-Components booting
        print 'local booting'
        conf = genConfFileForCpudaq(cpudaqAddr[0], operatorAddr, nsport, daqmw_dir)
        localCompsBooting()
    else:         # remote DAQ-Components booting
        print 'remote booting'

        print 'generate rtc.conf for each CPU DAQ'
        for addr in cpudaqAddr:
            print 'CPUDAQ addr:', addr
            conf = genConfFileForCpudaq(addr, operatorAddr, nsport, daqmw_dir)
            sendData(addr, xinetdPort, conf)
        remoteCompsBooting()

    time.sleep(7)
    print 'booting the DAQ-Operator'

    ldpath = os.getenv('LD_LIBRARY_PATH')
    print ldpath

    if (ldpath):
        mypath = ':' + daq_lib_path
        ldpath += mypath
    else:
        ldpath = daq_lib_path

    consMode = ''
    if consoleMode:
        consMode = '-c'

    # execute DaqOperator
    com = 'LD_LIBRARY_PATH=' + ldpath + ' ' + operator + ' -f ' + daqmw_dir + '/rtc.conf.operator' + ' -h ' + str(operatorAddr) + ' -p ' + str(nsport) + ' ' + '-x ' + confFile + ' ' + str(consMode)

    print com
    os.system( com )

if __name__ == "__main__":
    main()
