#comps.sh

#echo "*** run-comps start ***"

pkill Comp

#DIR_COMPS=/home/daq/DaqComponents
DIR_COMPS=/home/nakayosi/work-1.0/DAQ-Middleware-1st
DIR_CONF=/tmp

#. $DIR_COMPS/setup.sh
export MANYOLIB=/home/daq/manyo
export SOCKLIB=/home/daq/lib
export LD_LIBRARY_PATH=$MANYOLIB:$SOCKLIB:$LD_LIBRARY_PATH:

cd $DIR_COMPS

#$DIR_COMPS/bin/GathererComp   -f ${DIR_CONF}/rtc.conf < /dev/null > /dev/null 2>&1 &
#$DIR_COMPS/bin/DispatcherComp -f ${DIR_CONF}/rtc.conf < /dev/null > /dev/null 2>&1 &
#$DIR_COMPS/bin/LoggerComp     -f ${DIR_CONF}/rtc.conf < /dev/null > /dev/null 2>&1 &
#$DIR_COMPS/bin/MonitorComp    -f ${DIR_CONF}/rtc.conf < /dev/null > /dev/null 2>&1 &

$DIR_COMPS/bin/GathererComp   -f ${DIR_CONF}/rtc.conf < /dev/null > /tmp/gath.log 2>&1 &
$DIR_COMPS/bin/DispatcherComp -f ${DIR_CONF}/rtc.conf < /dev/null > /tmp/disp.log 2>&1 &
$DIR_COMPS/bin/LoggerComp     -f ${DIR_CONF}/rtc.conf < /dev/null > /tmp/logg.log 2>&1 &
$DIR_COMPS/bin/MonitorComp    -f ${DIR_CONF}/rtc.conf < /dev/null > /tmp/moni.log 2>&1 &
$DIR_COMPS/bin/GatenetComp    -f ${DIR_CONF}/rtc.conf < /dev/null > /tmp/gate.log 2>&1 &


echo "*** run-comps finished ***"

