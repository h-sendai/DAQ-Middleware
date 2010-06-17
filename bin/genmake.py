#!/usr/bin/env python
# -*- coding: euc-jp -*-

##
# @file genmake.py
# @brief Makefile generator using config.xml
# @date 
# @author Kazuo Nakayoshi

import sys, os
import time
import re
import getopt
from string import Template

def print_usage():
    mess = """\
Usage: genmake.py [-f] [-d] [-o]
 -f: configuration file name and its path (eg. ./config.xml)
 -d: directory path of DAQ-Components (e.g. /home/daq/DaqComponents/src/mlf)
"""
    print mess

def mycommand2(cmd):
    child = os.popen(cmd)
    time.sleep(0.1)
    data  = child.read()
    #print 'mycommand2: data: ', data
    err   = child.close()
    if err:
        return 'Runtime Error: ', err
    data = data.split('\n')
    return data[0]

def searchSrcDir(searchterm, file):
    try:
        fileToSearch = open( file, 'r' )
    except IOError:
        print "No such file"
        sys.exit(2)

    # Well, we've got this far - the file must exist!
    data = fileToSearch.read()
    data = data.split('\n')

    patternprog = re.compile( searchterm )

    srcDir = []

    start = 0
    stop  = 0

    leading_char = '>'
    tail_char = '<'

    for line in data:
        a_match = patternprog.search( line )
        if ( a_match ):
            #print line
            loop  = 0
            findcnt = 0

            for c in line:
                loop+=1
                if (c == leading_char):
                    if (findcnt == 0):
                        start = loop
                        findcnt +=1
                if (c == tail_char):
                    if (findcnt == 1):
                        stop = loop - 1
                        srcDir.append(line[start:stop])
                        findcnt = 0

    fileToSearch.close()
    #print srcDir

    return srcDir


def searchComps(searchterm, file):
    #if len(arguments) != 3:
    #    print_usage()
    #    sys.exit(1)

    #searchterm = arguments[1]
    #file = arguments[2]

    print "Searching for %s in %s" % ( searchterm, file )

    try:
        fileToSearch = open( file, 'r' )
    except IOError:
        print "No such file"
        sys.exit(2)

    # Well, we've got this far - the file must exist!
    data = fileToSearch.read()
    data = data.split('\n')

    patternprog = re.compile( searchterm )

    mycomps = []

    start = 0
    stop  = 0

    leading_char = '"'

    for line in data:
        a_match = patternprog.search( line )
        if ( a_match ):
            #print line
            loop  = 0
            findcnt = 0            
            for c in line:
                loop+=1
                if (c == leading_char):
                    if (findcnt == 1):
                        stop = loop - 2
                        #print line[start:stop]
                        mycomps.append(line[start:stop])
                        #print start, stop
                    elif (findcnt == 0):
                        start = loop
                        findcnt +=1                        

    #print mycomps
    fileToSearch.close()
    return mycomps
    #sys.exit( 0 )

def find_path(comps, mytop):
    mypath = []
    for comp in comps:
        mycom  = 'find ' + mytop + ' -name ' + comp+ ' -print'
        #print mycom
        mypath.append( mycommand2(mycom) )
        #print mypath
    return mypath
        
if __name__ == "__main__":
    arguments = sys.argv    
    if len(arguments) < 3:
        print_usage()
        sys.exit(1)
    try:
        opts, args = getopt.getopt(sys.argv[1:], "f:d:o:h")
    except getopt.GetoptError:
        sys.stdout = sys.stderr

    confile = ''
    ofile   = 'Makefile'
    for o, a in opts:
        if o == "-h":
            self.usage();
            sys.exit(1)
        if o == "-f":
            print 'conf file:',a
            confile = a
        if o == "-d":
            print 'search dir:',a
            searchdir = a            
        if o == "-o":
            ofile = a

    print 'confile:',confile

    dirs = searchSrcDir("sourcePath", confile)
    print len(dirs)
    for mypath in dirs:
        print mypath

    if ( len(dirs) == 0):
        comps = searchComps("cid", confile)
        dirs = find_path(comps, searchdir)
        t  = Template('\tcd $dir && $$(MAKE)\n')
        t2 = Template('\tcd $dir && $$(MAKE) clean\n')
        t3 = Template('\tcd $dir && $$(MAKE) install\n')        
        #t = Template('TARGET = all\n$(TARGET)\n\tcd $dir && $$(MAKE)\nclean:\n\tcd $dir && $$(MAKE) clean\n')
        #print comps
        #print dirs
    else:
        t = Template('\tcd $dir && $$(MAKE)\n')
#        t = Template('TARGET = all\n$(TARGET)\n\tcd $dir && $$(MAKE)\nclean:\n\tcd $dir && $$(MAKE) clean\n')
        t2 = Template('\tcd $dir && $$(MAKE) clean\n')
        t3 = Template('\tcd $dir && $$(MAKE) install\n')        

    ### generate Makefile ###
    makefileheader = """\
# -*- Makefile -*-
#
# @file  Makefile
# @brief makefile for DaqComponents sub directory
# @date  $Date$
#
"""
#TARGET = DaqComponents

#$(TARGET):

    target_all   = "TARGET = all\n$(TARGET):\n"
    target_clean = "clean:\n"
    target_install = "install:\n"

    outf = open(ofile, 'w')
    outf.write(makefileheader)
    
    outf.write(target_all)    
    for i in dirs:
        s = t.substitute(dir=i)
        outf.write(s)

    outf.write(target_clean)            
    for i in dirs:
        s2 = t2.substitute(dir=i)
        outf.write(s2)        

    outf.write(target_install)            
    for i in dirs:
        s3 = t3.substitute(dir=i)
        outf.write(s3)   

    outf.close()
    print

    
