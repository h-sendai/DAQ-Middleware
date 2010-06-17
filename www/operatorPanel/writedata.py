from mod_python import apache
file = "/home/daq/www/operatorPanel/runNumber.txt"

def SaveRunNumber(req, RunNumber):
       fileObj = open(file, "w+")
       fileObj.write(RunNumber)
       req.write("OK")
       return
