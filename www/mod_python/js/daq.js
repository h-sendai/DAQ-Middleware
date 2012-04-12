// -*- Javascript -*-
/**
 * @file daq.js
 * @brief methods for DAQ Panel
 * @date 25-May-2011
 * @author Yoshiji Yasu
 * @author Hiroshi Sendai
 * @modified 13-January-2012
 *
 * Copyright (C) 2008-2011
 *     High Energy Accelerator Research Organization (KEK), Japan.
 *     All rights reserved.
 *
 */

  /**
   *  setReflesh();
   *  clearReflesh();
   *    calls "getstate(./daq.py/Log')"
   */
  var timeID;
  function clearReflesh() {
    clearInterval(timeID);
  }
  function timerFunc() {
    getstate('./daq.py/Log');
  }
  function setReflesh() {
    clearInterval(timeID);
    timeID = setInterval(timerFunc, 2000); // 2 sec.
  }

  /**
   * change to Configured state
   */
  function change2Configured() {
    var inputs = document.getElementsByTagName("input");
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
    inputs = null;
  }

  /**
   * change to Running state
   */
  function change2Running() {
    var inputs = document.getElementsByTagName("input");
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
    inputs = null;
  }

  /**
   * change to Paused state
   */
  function change2Paused() {
    var inputs = document.getElementsByTagName("input");
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
    inputs = null;
  }

  /**
   * change to Loaded state
   */
  function change2Loaded() {
    var inputs = document.getElementsByTagName("input");
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
    inputs = null;
  }

  /**
   * get state from the url
   */
  function onSuccessFunc(httpObj) {
//        alert("OK: "+ httpObj.responseText);
        var res = httpObj.responseXML;
//        alert("OK: "+ httpObj.responseXML);
    var compNames = res.getElementsByTagName('compName');
    var status = res.getElementsByTagName('status');
    var value = status[0].firstChild.nodeValue;
    $("commandStatus").innerHTML = value;
    value = null;
    var states = res.getElementsByTagName('state');
    var currentstate = states[0].firstChild.nodeValue;
    var compStatus = res.getElementsByTagName('compStatus');
    var eventnums = res.getElementsByTagName('eventNum');
    var length = states.length;
    var text="";
    var len = compNames.length;
    for(i=0; i<length;i+=len) {
      text +="<table><tr><th></th><th>State</th><th>Status</th><th>Total Byte Counts</th></tr>";
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
    res = null;
    compNames = null;
    status = null;
    states = null;
    compStatus = null;
    eventnums = null;
    //alert(text);
    $("DAQStatus").innerHTML = text;
    text = null;

    if(currentstate == "LOADED")
      change2Loaded();
    else if (currentstate == "CONFIGURED")
      change2Configured();
    else if (currentstate == "RUNNING")
      change2Running();
    else if (currentstate == "PAUSED")
      change2Paused();
    currentstate = null;
  }

  function onCreateFunc(httpObj) {
//        alert(httpObj+" was got.");
  }

  function onFailureFunc(httpObj){
//        alert("Bad: "+ httpObj.responseText);
  }

  function getstate(url) {
    var date = new Date();
    var params = "cache" + date.getTime();
    var myAjax = new Ajax.Request(url, {
      method: "get",
      parameters: params,
      onCreate: onCreateFunc,
      onSuccess: onSuccessFunc,
      onFailure: onFailureFunc
    });
  }

