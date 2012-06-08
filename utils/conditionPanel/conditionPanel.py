#!/usr/bin/python

# Contributed by Nakatani san.

import datetime, urllib, os, sys
from optparse import OptionParser

try:
    import wx
except ImportError, e:
    sys.stderr.write('This program requires wx module.\n')
    sys.exit(e)

try:
    import elementtree.ElementTree as Etree
except ImportError, e:
    try:
        import xml.etree.ElementTree as Etree
    except ImportError, e:
        std.stderr.write('cannot import elemenetree.ElementTree or xml.etree.ElementTree')
        sys.exit(e)

class DaqmwSmpl(wx.Frame):
    def __init__(self, condition_xml_path):
        self.condition_xml_path = condition_xml_path
        wx.Frame.__init__(self,None,title="DAQ-MW Sample GUI", size=(300,300))
        self.CreateStatusBar()

        panel = wx.Panel(self, -1)
        exit = wx.Button(panel, wx.ID_EXIT,'',(10,5))
        self.Bind(wx.EVT_BUTTON, self.OnExit, id=wx.ID_EXIT)
        putBegin = wx.Button(panel, -1, 'Begin', (10,40),(80,30))
        self.Bind(wx.EVT_BUTTON, self.OnBegin, id=putBegin.GetId())
        putEnd = wx.Button(panel, -1, 'End', (100,40),(80,30))
        self.Bind(wx.EVT_BUTTON, self.OnEnd, id=putEnd.GetId())

        # read condition.xml
        self.tree=Etree.parse(self.condition_xml_path)
        self.elem=self.tree.getroot()

        # entry input value items
        labelBin = wx.StaticText(panel, -1, "Number of Hist Bin", (10,70))
        self.setBin=wx.TextCtrl(panel,-1,self.elem.find(".//hist_bin").text,(170,70),(120,-1),style=wx.ALIGN_RIGHT)
        labelMin = wx.StaticText(panel, -1, "Hist Min", (10,100))
        self.setMin=wx.TextCtrl(panel,-1,self.elem.find(".//hist_min").text,(170,100),(120,-1),style=wx.ALIGN_RIGHT)
        labelMax = wx.StaticText(panel, -1, "Hist Max", (10,130))
        self.setMax=wx.TextCtrl(panel,-1,self.elem.find(".//hist_max").text,(170,130),(120,-1),style=wx.ALIGN_RIGHT)
        labelRate = wx.StaticText(panel, -1, "Monitor Update [sec]", (10,160))
        self.setRate=wx.TextCtrl(panel,-1,self.elem.find(".//monitor_update_rate").text,(170,160),(120,-1),style=wx.ALIGN_RIGHT)

        # entry component status items
        self.compName=[]
        self.compState=[]
        self.eventNum=[]
        self.compStatus=[]
        for i in range(2):  # number of componets = 2
            y=190+i*40
            self.compName.append(wx.StaticText(panel,-1,"",(10,y)))
            self.compState.append(wx.StaticText(panel,-1,"",(200,y)))
            self.eventNum.append(wx.StaticText(panel,-1,"",(10,y+20)))
            self.compStatus.append(wx.StaticText(panel,-1,"",(200,y+20)))

        # set timer
        self.timer=wx.Timer(self)
        self.Bind(wx.EVT_TIMER,self.OnTimer)
        # update time 1[sec]
        self.timer.Start(1000)

        self.Show(True)
        #self.url="http://localhost/daqmw/operatorPanel/daq.py/"
        self.url="http://localhost/daqmw/scripts/daq.py/"

        # configure
        params=urllib.urlencode({"cmd":"<?xml version='1.0' encoding='UTF-8' standalone='no' ?><request><params>sample.xml</params></request>"})
        urllib.urlopen(self.url+"Params",params)

    def OnExit(self, event):
        # unconfigure
        params=urllib.urlencode({"cmd":""})
        urllib.urlopen(self.url+"ResetParams",params)
        self.Close()

    def OnBegin(self, event):
        # create condition.xml
        self.elem.find(".//hist_bin").text=self.setBin.GetValue()
        self.elem.find(".//hist_min").text=self.setMin.GetValue()
        self.elem.find(".//hist_max").text=self.setMax.GetValue()
        self.elem.find(".//monitor_update_rate").text=self.setRate.GetValue()
        self.tree.write(self.condition_xml_path)

        # create condition.json
        convert_command = 'condition_xml2json ' + self.condition_xml_path
        os.system(convert_command)
        # begin
        params=urllib.urlencode({"cmd":"<?xml version='1.0' encoding='UTF-8' standalone='no' ?><request><runNo>0</runNo></request>"})
        urllib.urlopen(self.url+"Begin",params)

    def OnEnd(self, event):
        # end
        params=urllib.urlencode({"cmd":""})
        urllib.urlopen(self.url+"End",params)

    def OnTimer(self,event):
        # update clock
        d=datetime.datetime.today()
        now="%04d/%02d/%02d %02d:%02d:%02d" % (d.year,d.month,d.day,d.hour,d.minute,d.second)

        #update DAQ-MW status
        f=urllib.urlopen(self.url+"Status")
        elem=Etree.fromstring(f.read())
        self.SetStatusText(now+"-"+elem.find(".//devStatus").find(".//status").text)

        # update comp status
        f=urllib.urlopen(self.url+"Log")
        elem=Etree.fromstring(f.read())
        i=0
        for s in elem.findall(".//compName"):
            self.compName[i].SetLabel(s.text)
            i+=1
        i=0
        for s in elem.findall(".//state"):
            self.compState[i].SetLabel(s.text)
            i+=1
        i=0
        for s in elem.findall(".//eventNum"):
            self.eventNum[i].SetLabel(s.text)
            i+=1
        i=0
        for s in elem.findall(".//compStatus"):
            self.compStatus[i].SetLabel(s.text)
            i+=1

def main():
    parser = OptionParser()
    parser.add_option('-c', '--condition',
                      action = 'store',
                      type   = 'string',
                      dest   = 'condition_xml_path')
    parser.set_defaults(condition_xml_path='/home/daq/condition.xml')
    (options, args) = parser.parse_args()

    app = wx.App()
    DaqmwSmpl(options.condition_xml_path)
    app.MainLoop()

if __name__ == '__main__':
    main()
