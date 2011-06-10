#!/usr/bin/env python

# run.py: Start up script for DAQ-Components.
#         This script parses config.xml file, gets IP addrese of CPU DAQs and
#         CPU UI, and starts DAQ-Components on remote/local PCs via network.

import errno
import sys
import os
import time
import subprocess
import signal
import socket
import re
from optparse import OptionParser

# Python interpreter is at least version 2.5.0
if sys.hexversion >= 0x020500F0:
    import xml.etree.ElementTree as Etree
# Python interpreter is before version 2.5.0
else:
    import elementtree.ElementTree as Etree

#
# constants and parameters
#

# adjust the daq_lib_path to your own environment.
daq_lib_path = '/usr/lib/daqmw:/kensdaq/root/lib'
confFile = ''

# directory of Components log files
daqmw_dir = '/tmp/daqmw'

# xinetd port for remote booting
xinetdPort = 50000

# omniNames port
nsport = 9876

def am_i_rhel_derived_and_running_on_vmware():
    redhat_release = '/etc/redhat-release'
    proc_ide_cdrom = '/proc/ide/ide1/hdc/model'

    if os.path.isfile(redhat_release) == False:
        return False
    if os.path.isfile(proc_ide_cdrom) == True:
        grep_command = '/bin/grep -q -i vmware %s' % (proc_ide_cdrom)
        if os.system(grep_command) == 0:
            return True
    else:
        return False

def opt():
    global confFile
    global schemaFile
    global operator
    global console
    global localBoot
    global mydisp
    global verbose
    global comps_invoke_interval

    usage = "run.py [OPTIONS] [CONFIG_FILE]"
    parser = OptionParser(usage)
    parser.set_defaults(console=False)
    parser.set_defaults(local=False)
    parser.set_defaults(verbose=False)
    parser.set_defaults(schema='/usr/share/daqmw/conf/config.xsd')
    parser.set_defaults(operator='/usr/libexec/daqmw/DaqOperatorComp')
    parser.set_defaults(comps_invoke_interval='0.0')

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
    parser.add_option("-w", "--wait", dest="comps_invoke_interval",
                      help="sleep specified interval seconds between each component invoking")

    (options, args) = parser.parse_args()
    if len(args) != 1:
        parser.error("ERROR: not specified config file")

    confFile   = args[0]
    schemaFile = options.schema
    operator   = options.operator
    console    = options.console
    localBoot  = options.local
    mydisp     = options.display
    verbose    = options.verbose
    
    # XXX
    # We have to sleep some seconds not to cause core file
    # when running on RHEL derived OS (SL, CentOS etc)
    # AND running on VMware Player.
    if options.comps_invoke_interval == '0.0':
        if am_i_rhel_derived_and_running_on_vmware():
            options.comps_invoke_interval = '1'
    comps_invoke_interval = float(options.comps_invoke_interval)

    if os.path.exists(confFile):
        print 'Use config file ' + confFile
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
    if comps_invoke_interval > 0:
        print "Comps invoke interval: %4.1f sec" % (comps_invoke_interval)

def getVal(confile, path):
    dom = Etree.parse(confile)
    myVal = ''
    myVal = dom.find(path)
    if myVal == None:
        return myVal
    else:
        return myVal.text

def getVals(confile, path):
    vals = []
    dom = Etree.parse(confile)
    vals = dom.findall(path)
    myVals = []
    index = 0
    for i in vals:
        if i != None:
            myVals.append(i.text)
    return myVals

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

def remove_file(file_path):
    if os.path.isfile(file_path):
        try:
            os.remove(file_path)
        except os.error:
            return False
    return True

def write_file(file_path, mytext):
    ret = True
    remove_file(file_path)
    ret = True
    outf = open(file_path, 'w')
    try:
        outf.write(mytext)
        outf.close()
    except IOError, (errno, strerror):
        outf.close()
        ret = False
    return ret

# generate rtc.conf for remote CPU for DAQ-Components
def genConfFileForCpudaq(addr, operatorAddr, nsport, mydir):
    rtc_conf_template = """\
#rtc.conf
corba.nameservers: %s:9876
naming.formats: %s.host_cxt/%%n.rtc
logger.log_level: TRACE
logger.enable: NO
exec_cxt.periodic.rate: 1000000000
corba.endpoint: %s:\
"""
    rtc_conf = rtc_conf_template % \
            (
                operatorAddr,
                addr,
                addr
            )
    return rtc_conf

# generate rtc.conf for DaqOperator
def genConfFileForOperator(mydir, confOperatorPath, operatorAddr):
    op_rtc_conf_template = """\
#rtc.conf
corba.nameservers: %s:9876
naming.formats: %s.host_cxt/%%n.rtc
logger.log_level: TRACE
logger.enable: NO
exec_cxt.periodic.rate: 1000000000
"""
    op_rtc_conf = op_rtc_conf_template % \
            (
                operatorAddr,
                operatorAddr
            )

    file_path = '%s/rtc.conf' % (mydir)
    ret = write_file(file_path, op_rtc_conf)
    return ret

def getExecPath(confFile):
    dom = Etree.parse(confFile)
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
    command = []
    command.append('/usr/bin/xmllint')
    command.append('-noout')
    command.append('--schema')
    command.append(schemaFile)
    command.append(confFile)
    #print 'Validation command: ',command

    try:
        p = subprocess.Popen(command, shell = False, stdout = subprocess.PIPE, stderr = subprocess.PIPE, close_fds=True)
    except OSError, e:
        sys.exit(e)
    except IOError, e:
        sys.exit(e)
    except:
        sys.exit('xmllint command error')

    p.wait()

    validated_return_mess = confFile + ' validates'

    out, err = p.communicate()

    if (err.strip() == validated_return_mess):
        return True, err
    else:
        return False, err

def kill_proc_exact(proc_name, sleep_sec = 1, max_retry = 60):
    """
    Send SIGTERM signal to proc_name.  Information of process id
    will be get from pgrep(1) output.  Process lookup will be done
    based on exact match of the proc_name.  For example,
    kill_proc_exact('SomeComp') will send SIGTERM to

    /usr/local/bin/SomeComp -f rtc.conf
    ./SomeComp -f rtc.conf
    /bin/sh SomeComp

    but does not send to

    /some/command -f SomeComp
    /some/command -f SomeCompFile

    If try to kill the other person's process, kill_proc_exact will send
    exception (OSError).  If trying max_ertry times and the process is still
    exist, sys.exit().
    """
    retry = 0
    # cannot write while (pids = get_pids_exact('SomeComp')):
    while True:
        pids = get_pids_exact(proc_name)
        if len(pids) == 0:
            break
        if retry == max_retry:
            sys.exit('cannot kill all specified processes after %d trial' % retry)
        else:
            if retry > 0:
                # print 'retry %d' % (retry)
                time.sleep(sleep_sec)
            retry += 1
            for pid in pids:
                try:
                    os.kill(int(pid), signal.SIGTERM)
                except OSError, (my_errno, strerror):
                    # 'No such process' may ignore safely (we got that pid
                    # but exit by previos kill already after we get_pids_exact())
                    # If other error like 'Operation not permitted', exit.
                    if my_errno == errno.ESRCH:
                        # print 'info: try another process if exists'
                        pass
                    else:
                        # XXX
                        #sys.exit('%s %s' % (proc_name, strerror))
                        #sys.stderr.write('err\n')
                        #sys.stderr.write('error: %s %s' % (proc_name, strerror))
                        raise

def exist_ok_mkdir (path, mode=0777):
    """Create a directory, but report no error if it already exists.

    This is the same as os.mkdir except it doesn't complain if the directory
    already exists.  It works by calling os.mkdir.  In the event of an error,
    it checks if the requested path is a directory and suppresses the error
    if so.
    """
    try:
        os.mkdir (path, mode)
    except OSError:
        if not os.path.isdir (path):
            raise

def exist_ok_makedirs (path, mode=0777):
    """Create a directory recursively, reporting no error if it already exists.

    This is like os.makedirs except it doesn't complain if the specified
    directory or any of its ancestors already exist.  This works by
    essentially re-implementing os.makedirs but calling the mkdir in this
    module instead of os.mkdir.  This also corrects a race condition in
    os.makedirs.
    """
    if not os.path.isdir (path):
        head, tail = os.path.split (path)
        if not tail:
            head, tail = os.path.split (head)
        if head and tail:
            exist_ok_makedirs (head, mode)
        exist_ok_mkdir (path, mode)

def get_pids_exact(proc_name):
    """
    Return process id tuple if 'proc_name' process(es) exist(s).
    Return empty tuple if there is no proc_name.
    If the program is a shell script, python script etc, then
    the first argument of the output of pgrep is /bin/sh,
    /usr/bin/python (or python if we use #!/usr/bin/env python).
    This function lookup if the program is written in script language.
    """

    pgrep_command = []
    pgrep_command.append('pgrep')
    pgrep_command.append('-fl')
    pgrep_command.append(proc_name)

    try:
        p = subprocess.Popen(pgrep_command, shell = False,
                             stdin  = subprocess.PIPE,
                             stdout = subprocess.PIPE,
                             stderr = subprocess.PIPE)
    except OSError, (my_errno, strerror):
        sys.stderr.write('%s: %s' % ('pgrep', strerror))
        raise
        #sys.exit(strerror)
    except:
        sys.stderr.write('get_pid_exact(): unknown error')
        raise

    p.wait()
    candidate = []
    for pgrep_line in p.stdout:
        candidate.append(pgrep_line.rstrip())

    return_pids = []
    for line in candidate:
        cols = line.split()
        pid = cols[0]

        if len(cols) >= 3:
            if is_script_lang(cols[1]):
                command = os.path.basename(cols[2])
            else:
                command = os.path.basename(cols[1])
        else:
            command = os.path.basename(cols[1])

        if command == proc_name:
            return_pids.append(pid)

    return tuple(return_pids)

def is_script_lang(elem_1):
    """
    Return True if elem_1 is script languages.
    """
    script_langs = [ 'sh', 'bash', 'csh', 'tcsh', 'zsh', 'perl', 'python', 'php', 'ruby']
    basename = os.path.basename(elem_1)
    if basename in script_langs:
        return True
    else:
        return False

def can_find_all_shared_libs(command_path):
    ldd = '/usr/bin/ldd'
    command = []
    command.append(ldd)
    command.append(command_path)

    try:
        p = subprocess.Popen(command, shell = False, stdout = subprocess.PIPE, stderr = subprocess.PIPE)
    except OSError, e:
        sys.exit(e)
    except IOError, e:
        sys.exit(e)
    except:
        sys.exit('ldd command error')

    p.wait()

    n_not_found_libs = 0
    for line in p.stdout:
        if re.search('not found', line):
            print line,
            n_not_found_libs += 1

    if n_not_found_libs > 0:
        raise IOError, 'Above shared libraries not found'

def start_comp(command_line, log='', foreground='no', no_stdin = 'yes'):
    """
    Execute component binary.

    The program is executed in background if we don't specify
    foreground = 'yes'.  If foreground = 'yes' is specified, 
    the process will be wait()'ed in this function.

    command_line is string that is a command and options to the command.
    For example:
    command_line = '/path/to/component -f /tmp/daqmw/rtc.conf'.

    If we have to log in a file, specify log argument.  Standard and
    standard error will be redirect to the file (Optional).
    If we don't specify log, the starndard output and standard error
    output will be inherited from the parent.  If you don't want to log,
    specify /dev/null as: log='/dev/null'.

    If execution fails (program file does not exist, excution bit is not
    set etc), start_comp() will sys.exit(sterror).  Execution success is
    verified by using get_pids_exact().

    Example:
        program = '/usr/local/bin/SomeComp'

        # standard output and standard eror output will be printed
        # on the termninal.  program will be executed in background.
        start_comp(program)

        # log to /tmp/daqmw/log.SomeComp
        log_file = '/tmp/daqmw/log.SomeComp'
        start_comp(program, log = log_file)

        # Specify options.  exec. '/usr/local/bin/SompeComp -a argument_of_a_option'
        execpath = '/usr/bin/somecomp'
        rtc_conf = '/tmp/daqmw/rtc.conf'
        command_line = '%s -f %s' % (execpath, rtc_conf)
        start_comp(command_line, log = log_file)

        # Foreground.  For example, DaqOperator console mode.
        command_line = '%s -c' % ('/usr/libexec/daqmwp/DaqOperatorComp')
        start_comp(command_line, foreground = 'yes')
    """
    proc_title_argv = command_line.split()

    if proc_title_argv[0] == 'taskset':
        real_program = proc_title_argv[3]
    else:
        real_program = proc_title_argv[0]

    # first test shared library link
    try:
        can_find_all_shared_libs(real_program)
    except IOError, e:
        print e;
        raise

    my_stdout = None
    my_stderr = None
    my_stdin  = None
    if (no_stdin == 'yes'):
        my_stdin  = open('/dev/null', 'r')

    if log:
        dir = os.path.dirname(log)
        if dir:
            try:
                #ExistOkMkdir.exist_ok_makedirs(dir, 0777)
                exist_ok_makedirs(dir, 0777)
            except OSError, (errno, strerror):
                sys.stderr.write('%s: %s\n' % (dir, strerror))
                raise
        try:
            log_fd = open(log, "w")
        except IOError, (errno, strerror):
            print 'cannot open %s: %s' % (log, strerror)
            raise
        else:
            my_stdout = log_fd
            my_stderr = subprocess.STDOUT

    try:
        p = subprocess.Popen(proc_title_argv, shell = False,
                            stdin  = my_stdin,
                            stdout = my_stdout,
                            stderr = my_stderr)
    except OSError, (errno, strerror):
        print 'cannot execute %s: %s' % (real_program, strerror)
        raise
    except ValueError, strerror:
        print 'subprocess.Popen value error: %s' % (strerror)
        raise

    if proc_title_argv[0] == 'taskset':
        try:
            proc_name = os.path.basename(proc_title_argv[3])
        except IndexError,e:
            print "path: ", path
            sys.exit(e)
    else:
        proc_name = os.path.basename(proc_title_argv[0])

    max_retry = 20
    retry = 0
    while True:
        if retry == max_retry:
            sys.exit('cannot exec. %s' % proc_name)

        #if kill_proc_exact.lookup_process_exact(proc):
        if get_pids_exact(proc_name):
            break;
        else:
            time.sleep(0.1)
            retry += 1

    if foreground == 'yes':
        try:
            p.wait()
        except KeyboardInterrupt, strerror:
            pass
        p.wait()

__author__ = 'Hiroshi Sendai'
__date__   = 'Sept. 2010'


def remove_omni_logs(omni_log_dir = ''):
    my_hostname = socket.gethostname()
    if not omni_log_dir:
        omni_log_dir = '.'
    omni_log_path        = '%s/omninames-%s.log' % (omni_log_dir, my_hostname)
    omni_log_backup_path = '%s/omninames-%s.bak' % (omni_log_dir, my_hostname)

    if os.path.isfile(omni_log_path):
        try:
            os.remove(omni_log_path)
        except OSError, (errno, strerror):
            sys.exit('%s: cannot remove %s: %s' % progname, omni_log_path, strerror)
    if os.path.isfile(omni_log_backup_path):
        try:
            os.remove(omni_log_backup_path)
        except OSError, (errno, strerror):
            sys.exit('%s: cannot remove %s: %s' % progname, omni_log_backup_path, strerror)


def run_omniNames(omni_log_dir = '', omni_port=nsport):
    # /usr/sbin/../bin/omniNames -start 1234 -logdir /home/sendai/app/py/invoke-comps

    command = 'omniNames'
    try:
        kill_proc_exact(command, sleep_sec = 0.2)
    except OSError, (errno, strerror):
        sys.exit('%s: cannot kill current running omniNames: %s' % (progname, strerror))

    if omni_log_dir == '' or omni_log_dir == '.':
        omni_log_dir = os.getcwd()
    options = []

    omni_command_line = "%s -start %s -logdir %s" % (command, omni_port, omni_log_dir)
    #print 'omni_command_line: ', omni_command_line
    prev_handler = signal.signal(signal.SIGINT, signal.SIG_IGN)
    start_comp(omni_command_line, log = '/dev/null')
    next_handler = signal.signal(signal.SIGINT, prev_handler)
    #omni_command_line += '>/dev/null 2>&1 < /dev/null &'
    #os.system(omni_command_line)

def getCompInfoFromXml():
    global compInfo_list
    global operatorAddr
    global cpudaqAddrs
    global execPaths
    global confPaths
    global confOperatorPath

    hostAddrPath = '//component/hostAddr'
    compExecPath = '//component/execPath'
    confPath     = '//component/confFile'
    confOperatorPath = '//daqOperator/hostAddr'

    operatorAddr = getVal(confFile, confOperatorPath)
    cpudaqAddrs  = getVals(confFile, hostAddrPath)
    execPaths    = getVals(confFile, compExecPath)
    confPaths    = getVals(confFile, confPath)

    compInfo_list = []
    n_of_comps  = len(execPaths)

    for i in range(n_of_comps):
        mycomp = dict()
        mycomp['compName'] = os.path.basename(execPaths[i])
        mycomp['compAddr'] = cpudaqAddrs[i]
        mycomp['execPath'] = execPaths[i]
        mycomp['confPath'] = confPaths[i]
        compInfo_list.append(mycomp)

#def boot_comps_or_die(ip_address, portno, execpath, rtc_conf_path, log):
def boot_comps_or_die(ip_address, portno, execpath, rtc_conf_path, log, env):
    # kill
    command_line = 'kill:%s\n' % execpath
    if send_and_recv_or_die(ip_address, portno, command_line) != True:
        print 'ERROR: kill component failed', command_line
        return False

    # boot
    #command_line = 'exec:%s -f %s:%s' %(execpath, rtc_conf_path, log)
    command_line = 'exec:%s -f %s:%s:%s' %(execpath, rtc_conf_path, log, env)

    #print 'boot_comps_or_die: ', command_line
    if send_and_recv_or_die(ip_address, portno, command_line) != True:
        print 'ERROR: boot comp failed:', command_line
        return False
    return True

def send_and_recv_or_die(ip_address, portno, command_line):
    try:
        so = socket.socket()
        so.connect((ip_address, portno))
    except socket.error, message:
        sys.exit(message)

    if command_line[len(command_line) - 1] != '\n':
        command_line += '\n'

    so.send(command_line)
    recvline = so.recv(1024)
    if recvline != '':
        #sys.exit('XXX' + ip_address + recvline)
        print 'ERROR: command failed: %s %s' %(ip_address, recvline)
        return False
    return True

def send_file_content(ip_address, portno, file_path, content):
    try:
        so = socket.socket()
        so.connect((ip_address, portno))
    except socket.error, e:
        #sys.exit(e)
        print 'ERROR: Socket connect', socket.error, e
        return False

    command_line = 'createfile:%s\n' % (file_path)

    try:
        so.send(command_line)
    except Exception, e:
        #sys.exit(e)
        print 'ERROR: Socket send', e
        return False

    for line in content.split('\n'):
        line = line + '\n'
        so.send(line)
    so.send('.\n')

    recvline = so.recv(1024)
    if recvline != '':
        msg = 'ERROR: %s: %s' % (ip_address, recvline)
        #sys.exit(msg)
        return False
    return True

def localCompsBooting():
    conf = genConfFileForCpudaq(cpudaqAddrs[0], operatorAddr, nsport, daqmw_dir)

    exist_ok_mkdir(daqmw_dir)

    fname = str(daqmw_dir) + '/rtc.conf'
    fw = open(fname, 'w')
    fw.write(conf)
    fw.close()

    for compInfo in compInfo_list:
        kill_proc_exact(os.path.basename(compInfo['execPath']))
        command_line = '%s -f %s' %(compInfo['execPath'], compInfo['confPath'])
        log_file = daqmw_dir +'/log.' + os.path.basename(compInfo['execPath'])
        start_comp(command_line, log = log_file)
        if comps_invoke_interval > 0:
            print 'sleeping %4.1f sec' % (comps_invoke_interval)
            time.sleep(comps_invoke_interval)

def remoteCompsBooting():
    for compInfo in compInfo_list:
        compAddr = compInfo['compAddr']
        log_file  = '%s/log.%s' % (daqmw_dir, compInfo['compName'])
        execPath  = compInfo['execPath']
        compPath  = '%s -f %s > %s 2>&1 &' % \
                    (execPath,compInfo['confPath'],log_file)

        conf = genConfFileForCpudaq(compAddr, operatorAddr, nsport, daqmw_dir)
        ret = send_file_content(compAddr, xinetdPort, compInfo['confPath'], conf)

        if ret != True:
            print 'rtc.conf file creation failed:', \
                  compAddr, xinetdPort, compInfo['confPath'], conf
            return False

        env = ''
        if mydisp != None:
            mydisp_list = mydisp.split(':')

            if len(mydisp_list) == 1:
                env = '%s\t%s:0' % ('DISPLAY', mydisp)
            elif len(mydisp_list) == 2:
                env = '%s\t%s' % ('DISPLAY', mydisp)
            else:
                print 'ERROR: Invalid DISPLAY env. val', env
                return False

        ret = boot_comps_or_die(compAddr, xinetdPort, compInfo['execPath'],\
                                #compInfo['confPath'], log_file)
                                compInfo['confPath'], log_file, env)

        if ret != True:
            print 'Remote booting failed:'
            print ' IP addr  : %s\n Port No  : %s\n Exe Path : %s\n Conf Path: %s\n Log File : %s\n Env Vals : %s\n' % \
                  (compAddr, xinetdPort,compInfo['execPath'], compInfo['confPath'], log_file, env)
            return False
        if comps_invoke_interval > 0:
            time.sleep(compos_invoke_interval)
    return True

def DaqOperatorBooting():
    if operatorAddr == None:
        print 'ERROR: not specified IP address of DaqOperator in config file'
        return False

    exist_ok_mkdir(daqmw_dir)  # create /tmp/daqmw for DAQ-Operator

    # generate a rtc.conf for DaqOperator
    conf_path_op = '.'
    ret = genConfFileForOperator(conf_path_op, confOperatorPath, operatorAddr)
    if ret == False:
        print 'ERROR: creation of conf file for DaqOperator failed'
        return False

    conf_path_op = '.'
    command_line = '%s -f %s/rtc.conf -h %s -p %s -x %s'\
                   % (operator, conf_path_op, str(operatorAddr), str(nsport), confFile)

    kill_proc_exact(os.path.basename(operator))

    if console:
        command_line = command_line + ' -c'
        try:
            start_comp(command_line, foreground = 'yes', no_stdin = 'no')
        except IOError, e:
            sys.exit(1)
        except OSError, e:
            sys.exit(1)
        except:
            sys.exit('error in run_daq_operator')
    else:
        try:
            start_comp(command_line, log = '/dev/null')
        except IOError, e:
            sys.exit(1)
        except OSError, e:
            sys.exit(1)
        except:
            sys.exit('error in run_daq_operator')
    return True

def main():
    #
    # get command line options
    #
    opt()

    #
    # validation of configuration file using XML schema config.xsd
    #
    ret, err = validateConfigFile(confFile, schemaFile)
    if ret:
        print 'Conf file validated:', confFile
    else:
        print '### ERROR: Conf file validation failed. Check the ', confFile
        print err
        sys.exit(-1)

    #
    # Boot omni naming service
    #
    print 'start new naming service...',
    remove_omni_logs()
    run_omniNames()
    print 'done'

    # parse config file
    getCompInfoFromXml()

    #
    # Boot DAQ-Components
    #

    # local DAQ-Components booting
    if localBoot:
        print 'Local Comps booting...',
        localCompsBooting()
        print 'done'
    # remote DAQ-Components booting
    else:
        print 'Remote Comps booting...',
        ret = remoteCompsBooting()
        if ret != True:
            print 'Remote Comps booting failed. Check remote hosts'
            sys.exit(-1)
        else:
            print 'done'

    #
    # Boot DAQ-Operator
    #
    if console == False:
        print 'Now booting the DAQ-Operator...',
    ret = DaqOperatorBooting()
    if ret != True:
        print 'DaqOperator booting failed'
        sys.exit(-1)
    else:
        if console == False:
            print 'done'
if __name__ == "__main__":
    main()
