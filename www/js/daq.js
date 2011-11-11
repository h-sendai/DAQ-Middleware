// -*- Javascript -*-
/**
 * @file daq.js
 * @brief methods for DAQ Panel
 * @date 25-May-2011
 * @author Yoshiji Yasu
 *        
 *
 * Copyright (C) 2008-2011
 *     High Energy Accelerator Research Organization (KEK), Japan.
 *     All rights reserved.
 *
 */

  /**
   * repeatGetState1 "getstate(./daq.py/Log')" wtih the specified count, times.
   */
  function repeatGetState(times) {
    var i = 0;
    setInterval((
      function() {
        getstate('./daq.py/Log');
        i++;
        if ((times != 0) && (i == times)) {
          clearInterval();
        }
      }
    ), 2000); // 2 sec.
  }

  /**
   * change to Configured state
   */
  function change2Configured() {
    inputs = document.getElementsByTagName("input");
    inputs[3].src = "../parts/configurationbuttonlight.jpg";
    inputs[5].src = "../parts/beginbutton.jpg";
    inputs[7].src = "../parts/endbuttonlight.jpg";
    inputs[9].src = "../parts/unconfigurebutton.jpg";
    inputs[11].src = "../parts/pausebuttonlight.jpg";
    inputs[13].src = "../parts/restartbuttonlight.jpg";
    $("configId").disabled = true;
    $("beginId").disabled = false;
    $("endId").disabled = true;
    $("unconfigId").disabled = false;
    $("pauseId").disabled = true;
    $("restartId").disabled = true;
    repeatGetState(5);
  }

  /**
   * change to Running state
   */
  function change2Running() {
    inputs = document.getElementsByTagName("input");
    inputs[3].src = "../parts/configurationbuttonlight.jpg";
    inputs[5].src = "../parts/beginbuttonlight.jpg";
    inputs[7].src = "../parts/endbutton.jpg";
    inputs[9].src = "../parts/unconfigurebuttonlight.jpg";
    inputs[11].src = "../parts/pausebutton.jpg";
    inputs[13].src = "../parts/restartbuttonlight.jpg";
    $("configId").disabled = true;
    $("beginId").disabled = true;
    $("endId").disabled = false;
    $("unconfigId").disabled = true;
    $("pauseId").disabled = false;
    $("restartId").disabled = true;
    repeatGetState(0);
  }

  /**
   * change to Paused state
   */
  function change2Paused() {
    inputs = document.getElementsByTagName("input");
    inputs[3].src = "../parts/configurationbuttonlight.jpg";
    inputs[5].src = "../parts/beginbuttonlight.jpg";
    inputs[7].src = "../parts/endbuttonlight.jpg";
    inputs[9].src = "../parts/unconfigurebuttonlight.jpg";
    inputs[11].src = "../parts/pausebuttonlight.jpg";
    inputs[13].src = "../parts/restartbutton.jpg";
    $("configId").disabled = true;
    $("beginId").disabled = false;
    $("endId").disabled = true;
    $("unconfigId").disabled = true;
    $("pauseId").disabled = true;
    $("restartId").disabled = false;
    repeatGetState(5);
  }

  /**
   * change to Loaded state
   */
  function change2Loaded() {
    inputs = document.getElementsByTagName("input");
    inputs[3].src = "../parts/configurationbutton.jpg";
    inputs[5].src = "../parts/beginbuttonlight.jpg";
    inputs[7].src = "../parts/endbuttonlight.jpg";
    inputs[9].src = "../parts/unconfigurebuttonlight.jpg";
    inputs[11].src = "../parts/pausebuttonlight.jpg";
    inputs[13].src = "../parts/restartbuttonlight.jpg";
    $("configId").disabled = false;
    $("beginId").disabled = true;
    $("endId").disabled = true;
    $("unconfigId").disabled = true;
    $("pauseId").disabled = true;
    $("restartId").disabled = true;
    repeatGetState(5);
  }

  /**
   * get state from the url
   */
  function getstate(url) {
    var msec = (new Date()).getTime();
    new Ajax.Request(url, {
      method: "get",
      parameters: "cache="+msec,
      onCreate:function(httpObj){
//        alert(httpObj+" was got.");
      },
      onSuccess:function(httpObj){
//        alert("OK: "+ httpObj.responseText);
        var res = httpObj.responseXML;
//        alert("OK: "+ httpObj.responseXML);
	var compNames = res.getElementsByTagName('compName');
        var status = res.getElementsByTagName('status');
        $("commandStatus").innerHTML = status[0].firstChild.nodeValue;
        var states = res.getElementsByTagName('state');
        var currentstatus = states[0].firstChild.nodeValue;
        var compStatus = res.getElementsByTagName('compStatus');
        var eventnums = res.getElementsByTagName('eventNum');
        var length = states.length;
        text="";
        var len = compNames.length;
        for(i=0; i<length;i+=len) {
          text +="<table><tr><th></th><th>State</th><th>Status</th><th>Event Counts</th></tr>";
          for(j = 0; j< len; j++) {
            text +="<tr><td>";
            text += compNames[i+j].firstChild.nodeValue;
            text += "</td><td class='compState'>";
            text += states[i+j].firstChild.nodeValue;
            text += "</td><td class='compStatus'>";
            text +=compStatus[i+j].firstChild.nodeValue;
            text +="</td><td class='eventCount'>";
            text += eventnums[i+j].firstChild.nodeValue;
            text +="</td></tr>";
          }
          text +="</table>";
          //alert(text);
        }
        //alert(text);
        $("DAQStatus").innerHTML = text;

        if(currentstate == "LOADED")
          change2Loaded1();
        else if (currentstate == "CONFIGURED")
          change2Configured1();
        else if (currentstat == "RUNNING")
          change2Running1();
        else if (currentstat == "PAUSED")
          change2Paused1();
      },
      onFailure:function(httpObj){
//        alert("Bad: "+ httpObj.responseText);
      }
    });
  }
