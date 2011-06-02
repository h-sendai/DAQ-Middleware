#!/usr/bin/env python

"""Write local data for the server program, daq.py
"""
__author__   = 'Yoshiji Yasu'
__version__  = '1.0'
__date__     = '1-January-2008'

from mod_python import apache
file = "/var/www/html/daqmw/operatorPanel/runNumber.txt"

def SaveRunNumber(req, RunNumber):
       """Save run number into the file."""
       fileObj = open(file, "w+")
       fileObj.write(RunNumber)
       req.write("OK")
       return
