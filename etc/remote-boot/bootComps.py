#!/usr/bin/env python

import errno
import re
import os
import sys
import time # for sleep
import signal
import subprocess
#import ProcUtils
import glob
import shutil
#import ExistOkMkdir

class MyProcUtil:

    def __init__(self):
        self.script_langs = [
            'python',
            'python2',
            'python2.4',
            'python2.5',
            'python2.6',
            'python2.7',
            'perl',
            'ruby',
            'sh',
            'bash',
            'tcsh',
            'zsh'
        ]

    def get_all_pids(self):
        return [ int(x) for x in os.listdir('/proc') if x.isdigit() ]
    
    def get_cmdline(self, pid):
        filename = '/proc/%s/cmdline' % (pid)
        try:
            f = open(filename, 'r')
        except IOError, (en, msg):
            if en == errno.ENOENT:
                return []
            else:
                raise IOError('pid %s: %s' % (pid, msg))
        try:
            rv =  [ x for x in f.read().split('\x00') if x ]
        except IOError, (en, msg):
            if en == errno.ESRCH:
                return []
            else:
                raise IOError('remote-boot/bootComps.py: pid %s: %s' % (pid, msg))
            
        f.close()

        return rv

    def get_pids_by_proc_name(self, proc_name):
        rv_pids = []
        for pid in self.get_all_pids():
            cmdline = self.get_cmdline(pid)
            if (len(cmdline) > 0):
                basename = os.path.basename(cmdline[0])
                if basename in self.script_langs:
                    if len(cmdline) == 1:
                        # if cmdline is for example '/bin/zsh' only
                        basename = cmdline[0]
                    else:
                        basename = os.path.basename(cmdline[1])
                if proc_name == basename:
                    rv_pids.append(pid)
        return rv_pids

def get_pids_exact(proc_name):
    p = MyProcUtil()
    pid_list = p.get_pids_by_proc_name(proc_name)
    return tuple(pid_list)

def start_comp(command_line, log='', env='', foreground='no', no_stdin = 'yes'):
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

    #command = [ path ]
    #if options:
    #    command += options

    # Copy environment variables from xinetd (env += keyword.  See xinetd.conf manual page)
    my_env = os.environ
    if env != '': # -d option specified when invoking run.py (run.py -d 1.2.3.4)
        env_list = env.split('\t')
        my_env[env_list[0]] = env_list[1]

    try:
        p = subprocess.Popen(proc_title_argv, shell = False,
                            # stdin  = subprocess.PIPE,\
                            #stdin  = None,
                            stdin  = my_stdin,
                            stdout = my_stdout,
                            stderr = my_stderr,#)
                            env = my_env)
    except OSError, (errno, strerror):
        #sys.exit('cannot execute %s: %s' % (path, strerror))
        print 'cannot execute %s: %s' % (real_program, strerror)
        raise
    except ValueError, strerror:
        #sys.exit('subprocess.Popen value error: %s' %strerror)
        print 'subprocess.Popen value error: %s' % (strerror)
        raise

    #proc_name = os.path.basename(path)
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

def kill_proc_exact(proc_name, sleep_sec = 1, max_retry = 60):
    """
    Send SIGTERM signal to proc_name.  Information of process id
    will be get from get_pids_exact() that use MyProcUtil.
    Process lookup will be done based on exact match of the proc_name.
    For example,

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

def can_find_all_shared_libs(command_path):
    ldd = '/usr/bin/ldd'
    command = []
    command.append(ldd)
    command.append(command_path)

    try:
        p = subprocess.Popen(command, shell = False, stdout = subprocess.PIPE,\
                             stderr = subprocess.PIPE)
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

# input from remote host
#
# exec:/usr/local/bin/somecomp -f rtc.conf:logfile
# exec:/usr/local/bin/somecomp -f rtc.conf
# kill:somecomp
#

# command format
# command:arg0:arg1
# arg0 and arg1 are optional.  The meaing of the args depend on commmand.
# 
# command: exec
# arg0:    comamnd path with args
# arg1:    specify log files if desired (optional)
# 
# command: kill
# arg0:    process name to kill
# arg1:    none
# copyglob   src directory ' ' dst directory             
# copyfile   src file ' ' dst directory

def main():
    #sys.stderr.write('connect')
    input_line = sys.stdin.readline().rstrip().split('\t', 3)
    if len(input_line) < 2:
        sys.exit('-1 need more info.')

    command = input_line[0]

    if command == 'exec':
        log = ''
        env = ''

        if len(input_line) >= 3:
            log = input_line[2]

        if len(input_line) >= 4:
            env = input_line[3]
            if len(input_line) == 5:
                disp_no = input_line[4]
                env = '%s\t%s' % (env, disp_no)
        try:

            start_comp(input_line[1], log, env)
        except OSError, (errno, strerror):
            print '-1 %s %s' % (program, strerror)
            sys.exit(strerror)
        except IOError, e:
            print '-1 %s %s' % (program, e)
            sys.exit(e)
        except:
            print '-1 unknown error in bootComps2 main()'

    elif command == 'kill':
        if os.path.basename(input_line[1].split()[0]) == 'taskset':
            proc_title_argv_0 = input_line[1].split()[3]
        else:
            proc_title_argv_0 = input_line[1].split()[0]

        try:
            #ProcUtils.kill_proc_exact(os.path.basename(input_line[1]))
            kill_proc_exact(os.path.basename(proc_title_argv_0))
        except OSError, (errno, strerror):
            print '-1 %s %s' % (input_line[1], strerror)
    elif command == 'copyglob':
        orig, dest = input_line[1].split()
        copy_list = glob.glob('%s/*' % orig)
        for f in copy_list:
            shutil.copy2(f, dest)
    elif command == 'copyfile':
        orig, dest = input_line[1].split()
        shutil.copy2(orig, dest)
    elif command == 'createfile':
        file_path = input_line[1]
        create_file(file_path)
    elif command == 'catfile':
        file_path = input_line[1]
        cat_file(file_path)
    else:
        print '-1 unknown command'

def create_file(file_path):
    dir_path = os.path.dirname(file_path)
    try:
        #ExistOkMkdir.exist_ok_makedirs(dir_path, 0755)
        exist_ok_makedirs(dir_path, 0755)
    except Exception, e:
        sys.exit(e)

    try:
        f = open(file_path, 'w')
    except Exception, e:
        sys.exit(e)

    content = []
    while True:
        line = sys.stdin.readline()
        if re.match('^\.', line):
            break
        line = line.rstrip('\n')
        line = line.rstrip('\r')
        content.append(line)

    lines = '\n'.join(content)
    lines = lines + '\n'
    try:
        f.write(lines)
    except Exception, e:
        sys.exit(e)
    
    try:
        f.close()
    except Exception, e:
        sys.exit(e)
    
    sys.exit(0)

def cat_file(file_path):
    try:
        f = open(file_path, 'r')
    except IOError, e:
        sys.exit(e)

    lines = f.readlines()
    for line in lines:
        print line,

if __name__ == '__main__':
    main()
