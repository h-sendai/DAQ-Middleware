#! /usr/bin/python

import os,sys

com_create_rtcconf = '#rtc.conf'
com_boot_comps     = '#comps.sh'
rtc_conf_file_path = "/tmp/rtc.conf"
rtc_boot_comp_path = "/tmp/run-comps.sh"

mymessage = sys.stdin.read()
mycommand = mymessage[0:9]
#print mycommand

if mycommand == com_create_rtcconf :
    print "create /tmp/rtc.conf"
    rtc_conf = open(rtc_conf_file_path,'w')
    rtc_conf.write(mymessage)
    rtc_conf.close()    
    
elif mycommand == com_boot_comps :
    boot_comps = open(rtc_boot_comp_path,'w')
    boot_comps.write(mymessage)
    boot_comps.close()
    comm = 'sh ' + rtc_boot_comp_path + ' 2>&1 &'
    
    
#    comm = '/home/nakayosi/work/DaqComponents/run-comps.sh' + ' 2>&1 &'
#    comm = mymessage + ' 2>&1 &'
    ret = os.system(comm)
#    ret = os.system(boot_comps)

    if ret != 0:
	print "Error occured"
else:
    print "Bad command: " + mycommand
    
