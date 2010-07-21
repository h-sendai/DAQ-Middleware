#!/usr/bin/env python

# run.py: Start up script for DAQ-Components.
#         This script parses config.xml file, gets IP addrese of CPU DAQs and
#         CPU UI, and starts DAQ-Components on remote/local PCs via network.

import sys, os, time
import popen2
import socket
from optparse import OptionParser
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

def opt():
    global confFile
    global schemaFile
    global operator
    global console
    global localBoot
    global mydisp
    global verbose

    usage = "run.py [OPTIONS] [CONFIG_FILE]"
    parser = OptionParser(usage)
    parser.set_defaults(console=False)
    parser.set_defaults(local=False)
    parser.set_defaults(verbose=False)
    parser.set_defaults(schema='/usr/share/daqmw/conf/config.xsd')
    parser.set_defaults(operator='/usr/libexec/daqmw/DaqOperatorComp')

    parser.add_option("-c", "--console",
                      action="store_true", dest="console", help="console mode. default is HTTP mode")
    parser.add_option("-l", "--local",
                      action="store_true", dest="local", help="local boot mode. default is remote boot")
    parser.add_option("-v", "--verbose",
                      action="store_true", dest="verbose", help="verbose mode on. default is off" )
    parser.add_option("-s", "--schema", dest="schema",
                      help="specify XML schema file with abs. path")
    parser.add_option("-o", "--operator", dest="operator",
                      help="specify DaqOperatorComp with abs. path")
    parser.add_option("-d", "--display", dest="display",
                      help="specify DISPLAY env. val for X apps")

    (options, args) = parser.parse_args()

    confFile   = args[0]
    schemaFile = options.schema
    operator   = options.operator
    console    = options.console
    localBoot  = options.local
    mydisp     = options.display
    verbose    = options.verbose

    if len(args) != 1:
        parser.error("ERROR: not specified config file")
    if os.path.exists(confFile):
        print 'Use config file ' + confFile + ' for configuration'
    else:
        print 'ERROR: config file ' + confFile + ' not exists. exit.'
        sys.exit(-1)
    if os.path.exists(schemaFile):
        print 'Use ' + schemaFile + ' for XML schema'
    else:
        print 'ERROR: schema ' + options.schemaFile + ' not exists. exit.'
        sys.exit(-1)
    if os.path.exists(options.operator):
        print 'Use ' + options.operator + ' for DAQ-Operator'
    else:
        print 'ERROR: ' + options.operator + ' not exists. exit.'
        sys.exit(-1)

def getVal(confile, path):
    reader = PyExpat.Reader()
    text = open(confile).read()
    dom = reader.fromString(text)
    myVal = ''
    for node in Evaluate(path, dom.documentElement):
        myVal = node.firstChild.nodeValue
        if verbose:
            print node.firstChild.nodeValue
    return myVal

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
    return mess

def sendData(addr, port, data):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    rdata = ''
    try:
        s.connect( (addr, port) )
        s.send(data)
        s.shutdown(socket.SHUT_WR)
        rdata = s.recv(1024)
        if verbose:
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
                        if verbose:
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
        return True
    else:
        print '### ERROR: Validation failed:', confFile
        print '### Check the', confFile
        os.system(cmd) # for error message
        return False

def localCompsBooting():
    execPaths    = getVals(confFile, compExecPath)
    mypkill = 'pkill -f "^[^ ]+Comp "'
    os.system(mypkill)  # kill previous components
    index = 0
    for i in execPaths:
        #print "booting: ", i
        logfile = daqmw_dir +'/log.' + os.path.basename(execPaths[index])
        #comm = myldpath + i + ' -f ' + confPaths[index] + '.' + operatorAddr + ' > ' + logfile + ' 2>&1 &'
        comm = myldpath + i + ' -f ' + confPaths[index] + ' > ' + logfile + ' 2>&1 &'
        if verbose:
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
        mydisp = 'export DISPLAY=' + str(mydisp) + '\n'
        runcomps += mydisp

        for i in execPaths[index]:
            logfile = daqmw_dir + '/log.' + os.path.basename(i)
            comp = myldpath + i + ' -f ' + confPaths[index] + ' > ' + logfile + ' 2>&1 &\n'
            runcomps += comp
        index += 1

        print 'booting the DAQ-Components on each CPU DAQ'
        rdata = sendData(addr, xinetdPort, runcomps)

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

    if verbose:
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

    print 'start new naming service and wait for booting...'
    cmd = 'OMNIORB_USEHOSTNAME=' + str(operatorAddr) + ' rtm-naming ' +  str(nsport) + ' -logdir /tmp'

    os.system( cmd )

    time.sleep(2)

    # generate a rtc.conf for DaqOperator
    conf_path_operator = '.'
    genConfFileForOperator(operatorAddr, nsport, conf_path_operator)

    ldpath = os.getenv('LD_LIBRARY_PATH')
    if (ldpath):
        mypath = ':' + daq_lib_path
        ldpath += mypath
    else:
        ldpath = daq_lib_path

    #print 'CURRENT DIR: ', os.getcwd()
    if verbose:
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
            #print 'CPUDAQ addr:', addr
            conf = genConfFileForCpudaq(addr, operatorAddr, nsport, daqmw_dir)
            sendData(addr, xinetdPort, conf)
        remoteCompsBooting()
    print 'booting the DAQ-Componets, wait...'
    time.sleep(8)
    print 'booting the DAQ-Operator'

    ldpath = os.getenv('LD_LIBRARY_PATH')

    if (ldpath):
        mypath = ':' + daq_lib_path
        ldpath += mypath
    else:
        ldpath = daq_lib_path

    consMode = ''
    if console:
        consMode = '-c'

    ld_lib_path   = 'LD_LIBRARY_PATH=' + ldpath + ' '
    #rtc_conf_path = daqmw_dir + '/rtc.conf.operator '
    rtc_conf_path = conf_path_operator + '/rtc.conf '
    operator_path = operator + ' -f ' + rtc_conf_path
    operator_addr = ' -h ' + str(operatorAddr) + ' '
    ns_port       = ' -p ' + str(nsport) + ' '
    config_file   = ' -x ' + confFile + ' '
    console_mode  = str(consMode)

    # execute DaqOperator
    com = ld_lib_path + operator_path + operator_addr + ns_port + config_file + console_mode
    #com = 'LD_LIBRARY_PATH=' + ldpath + ' ' + operator + ' -h ' + str(operatorAddr) + ' -p ' + str(nsport) + ' ' + '-x ' + confFile + ' ' + str(consMode)
    if verbose:
        print com
    os.system( com )

if __name__ == "__main__":
    main()
