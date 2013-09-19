#from elementtree.ElementTree import *
import sys
try:
    import elementtree.ElementTree as xml
except ImportError,e:
    try:
        import xml.etree.ElementTree as xml
    except ImportError, e:
        sys.exit(e)

import signal
import types
from subprocess import Popen, PIPE

import time,datetime, os, sys

class Operator:
    def __init__(self, addr):
        self.addr = addr

class Component:
    def __init__(self, name):
        self.name = name
        self.inp  = []
        self.outp = []
        self.ofrom =[]
        self.params=[]
        self.sofrder = -1

    def set_param(self, pkey, pval):
        mylist = [pkey, pval]
        self.params.append(mylist)

    def set_inPort(self, inp, from_comp, from_pname):
        self.inp.append(inp)
        myfrom = from_comp + "0" + ":" + from_pname
        self.ofrom.append(myfrom)

    def set_outPort(self, outp):
        self.outp.append(outp)

    def show(self):
        print "name:", self.name
        print "inp:",  self.inp
        print "outp:", self.outp
        print "ofrom:",self.ofrom

class Source(Component):
    def __init__(self, name):
        self.sorder = 4
        Component.__init__(self, name)

class Dispatcher(Component):
    def __init__(self, name):
        self.sorder = 3
        Component.__init__(self, name)

class Sink(Component):
    def __init__(self, name):
        self.sorder = 1
        Component.__init__(self, name)

class ConfigWriter:
    def __init__(self):
        self.root = xml.Element('configInfo')

        self.op = xml.Element('daqOperator')
        self.root.append(self.op)
        self.dgs = xml.Element('daqGroups')
        self.root.append(self.dgs)
        dg = xml.Element('daqGroup')
        dg.attrib["gid"] = "group0"
        self.dgs.append(dg)

        self.cps = xml.Element('components')
        dg.append(self.cps)

    def add_elem(self, parent, tag):
        myelem = xml.Element(tag)
        parent.append(myelem)

    def add_elem_text(self, parent, tag, text):
        myelem = xml.Element(tag)
        myelem.text = text
        parent.append(myelem)

    def add_elem_attr(self, parent, tag, text, attr, aval):
        myelem = xml.Element(tag)
        myelem.text = text
        myelem.attrib[attr] = aval;
        parent.append(myelem)

    def create_ports(self, parent, comp):
        parent.append(self.inps)
        parent.append(self.outps)

        for index in range(len(comp.outp)):
            self.add_elem_text(self.outps, "outPort", comp.outp[index])

        for index in range(len(comp.inp)):
            self.add_elem_attr(self.inps, "inPort", comp.inp[index], "from", comp.ofrom[index])

    def add_op_elem(self, hostaddr):
        self.add_elem_text(self.op, "hostAddr", hostaddr)

    def add_comp_elem(self, comp, hostaddr, execpath, sorder, hostport="50000"):
        compel = xml.Element('component')
        compel.attrib['cid'] = comp.name + '0'

        self.add_elem_text(compel, "hostAddr", hostaddr)
        self.add_elem_text(compel, "hostPort", hostport)
        instname = comp.name + "0" + "." + "rtc"
        self.add_elem_text(compel, "instName", instname)
        self.add_elem_text(compel, "execPath", execpath)
        self.add_elem_text(compel, "confFile", "/tmp/daqmw/rtc.conf")
        self.add_elem_text(compel, "startOrd", sorder)
        inps = xml.Element("inPorts")
        outps = xml.Element("outPorts")
        params = xml.Element("params")

        for index in range(len(comp.outp)):
            self.add_elem_text(outps, "outPort", comp.outp[index])

        for index in range(len(comp.inp)):
            self.add_elem_attr(inps, "inPort", comp.inp[index], "from",
                               comp.ofrom[index])

        for index in range(len(comp.params)):
            self.add_elem_attr(params, "param", comp.params[index][1], "pid",
                               comp.params[index][0])

        compel.append(inps)
        compel.append(outps)
        compel.append(params)

        self.cps.append(compel)

    def signal_handler(self, *args):
        print 'Got signal Alarm since Popen will deadlock:%s' %str(args)

    def run_command_with_popen(self, cmd, **kwargs):
        signal.signal(signal.SIGALRM, self.signal_handler)
        signal.alarm(3)
        wait = kwargs.pop('wait')
        p = Popen(cmd, **kwargs)
        if wait:
            p.wait()
            #print 'return code: %s' % p.returncode
        if type(kwargs.get('stdout')) == types.FileType:
            kwargs['stdout'].seek(0)
            out = kwargs['stdout'].read()
        else:
            out, err = p.communicate()
            #print 'out size: %s' % len(out)
            signal.alarm(0)
        return out

    def write_to_file(self, fname):
        tmpname = '/tmp/mytmp.%s.xml' % os.getpid()
        tmpfile = open(tmpname, 'w')
        xml.ElementTree(self.root).write(tmpfile)
        tmpfile.close()

        mycmd = ('/usr/bin/xmllint', '--format', tmpname)
        try:
            out = self.run_command_with_popen(mycmd, wait=True, stdout=PIPE)
        except OSError, err:
            print 'Exception occured'
            print err.args

        if fname:
            outfile = open(fname, 'w')
            outfile.write(out)
            return
        else:
            return out

if __name__ == "__main__":
    src = Source("SampleReader")
    src.set_outPort("samplereader_out")
    src.set_param("srcAddr", "127.0.0.1")
    src.set_param("srcPort", "2222")

    sink = Sink("SampleLogger")
    sink.set_inPort("samplelogger_in", "SampleReader", "samplereader_out")
    sink.set_param("dirName", "/tmp")
    sink.set_param("isLogging", "yes")
    sink.set_param("maxFileSizeInMegaByte", "10")

    cw = ConfigWriter()
    cw.add_op_elem("127.0.0.1")
    cw.add_comp_elem(src,  "127.0.0.1", "/home/nakayosi/work-1.0.0/DAQ-Middleware-Logger/examples/SampleReader/SampleReaderComp","2")
    cw.add_comp_elem(sink, "127.0.0.1", "/home/nakayosi/work-1.0.0/DAQ-Middleware-Logger/examples/SampleLogger/SampleLoggerComp","1")
    #cw.write_to_file("mytest.xml")
    cw.write_to_file("")

