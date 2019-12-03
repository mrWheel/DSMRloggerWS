/*
***************************************************************************  
**  Program  : DSMRlogger.js, part of DSMRloggerWS
**  Version  : v1.0.4
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
  
"use strict";

	let webSocketConn;
	let needReload 	= true;
  let validJson  	= false;
  let activeTab   = "none";
  let jsonMessage;   
  let singlePair;
  let onePair;
  let daysMaxRows, hoursMaxRows, monthsMaxRows;
  let TimerTab;
	let chartType 				= 'bar';
	let graphType 				= 'W';
	let settingBgColor 	 	= 'deepskyblue';
	let settingFontColor 	= 'white'
  let settingBackEDC 		= "red";
  let settingLineEDC 		= "red";
  let settingBackERC 		= "green";
  let settingLineERC 		= "green";
  let settingBackGDC 		= "blue";
  let settingLineGDC 		= "blue";
  let settingBackED2C 	= "tomato";
  let settingLineED2C 	= "tomato";
  let settingBackER2C 	= "lightgreen";
  let settingLineER2C 	= "lightgreen";
  let settingBackGD2C 	= "lightblue";
  let settingLineGD2C 	= "lightblue";
  let settingBackPR123C	= "green";
  let settingLinePR123C = "black";
  let settingBackPD1C 	= "yellow";
  let settingLinePD1C 	= "black";
  let settingBackPD2C 	= "lightgreen";
  let settingLinePD2C 	= "black";
  let settingBackPD3C 	= "lime";
  let settingLinePD3C 	= "black";

  window.onload=bootsTrap;
  window.onfocus = function() {
  	if (needReload) {
  		window.location.reload(true);
  	}
  };
  	
  
  function bootsTrap() {
		addLogLine("bootsTrap()");
		needReload = true;
		
		document.getElementById('debug').checked = false;
    document.getElementById('logWindow').style.display = "none";
    document.getElementById('bActual').addEventListener('click',function()
    																						{openTab('Actual');});
    document.getElementById('bLastHours').addEventListener('click',function() 
    																						{openTab('LastHours');});
    document.getElementById('bLastDays').addEventListener('click',function() 
    																						{openTab('LastDays');});
    document.getElementById('bLastMonths').addEventListener('click',function() 
    																						{openTab('LastMonths');});
    document.getElementById('bGraphics').addEventListener('click',function() 
    																						{openTab('Graphics');});
    document.getElementById('bSysInfo').addEventListener('click',function() 
    																						{openTab('SysInfo');});
    document.getElementById('FSexplorer').addEventListener('click',function() 
    																						{ addLogLine("newTab: goFSexplorer");
    																						  location.href = "/FSexplorer";
    																						});
/***
    let supportsWebSockets = 'WebSocket' in window || 'MozWebSocket' in window;
    if (!supportsWebSockets) {  
  	  alert("Your browser doen NOT support websockets!")
    }
***/    
    let count = 0;
    while (document.getElementById('devVersion').value == "[version]") {
    	count++;
    	addLogLine("count ["+count+"] devVersion is ["+document.getElementById('devVersion').value+"]");
    	if (count > 10) {
    		alert("Count="+count+" => reload from server!");
    		window.location.reload(true);
    	}
    	setTimeout("", 500);
    }

  } // bootsTrap()
  
  webSocketConn	= new WebSocket('ws://'+location.host+':81/', ['arduino']);
  addLogLine(" ");
  addLogLine("WebSocket('ws://"+location.host+":81/', ['arduino'])");
  console.log("WebSocket('ws://"+location.host+":81/', ['arduino'])");
  
  webSocketConn.onopen 		= function () { 
  	addLogLine("Connected!");
   	webSocketConn.send('Connect ' + new Date()); 
   	addLogLine("getDevInfo");
   	webSocketConn.send("getDevInfo");
		addLogLine("newTab: tabSettings");
		webSocketConn.send("sendSettings");

		addLogLine("initTab: Actual");
  	//webSocketConn.send("tabActual");
		openTab("Actual");
   	needReload	= false;

  }; 
  
  webSocketConn.onclose 		= function () { 
   	addLogLine(" ");
   	addLogLine("Disconnected!");
   	addLogLine(" ");
   	needReload	= true;
   	openTab('Actual')
   	let redirectButton = "<p></p><hr><p></p><p></p>"; 
   	redirectButton    += "<style='font-size: 50px;'>Disconneted from DSMRlogger"; 
   	redirectButton    += "<input type='submit' value='re-Connect' "; 
    redirectButton    += " onclick='window.location=\"/\";' />  ";     
   	redirectButton    += "<p></p><p></p><hr><p></p>"; 
		document.getElementById("Actual").innerHTML = redirectButton;

  }; 
  webSocketConn.onerror 	= function (error) 	{ 
   	addLogLine("Error: " + error);
   	console.log('WebSocket Error ', error);
  };
  webSocketConn.onmessage	= function (e) {
  	needReload = false;
    parsePayload(e.data); 
  };
  

 	function openTab(tabName) {
 		
 		activeTab = tabName;
    clearInterval(TimerTab);  
    
 		let bID = "b" + tabName;
    let i;
    //---- update buttons in navigation bar ---
 		let x = document.getElementsByClassName("tabButton");
 		for (i = 0; i < x.length; i++) {
			x[i].style.background			= settingBgColor;
 			x[i].style.border					= 'none';
 			x[i].style.textDecoration = 'none';  
 			x[i].style.outline    		= 'none';  
 			x[i].style.boxShadow 			= 'none';
 		}
 		//--- hide all tab's -------
 		x = document.getElementsByClassName("tabName");
 		for (i = 0; i < x.length; i++) {
 			x[i].style.display    = "none";  
 		}
 		//--- and set active tab to 'block'
		console.log("now set ["+bID+"] to block ..");
    document.getElementById(bID).style.background='lightgray';
 		document.getElementById(tabName).style.display = "block";  
    if (tabName == "Actual") {
 			addLogLine("newTab: Actual");
 			webSocketConn.send("tabActual");
 	    TimerTab=setInterval(function(){
                    webSocketConn.send("tabActual");
                  },10000);

 		} else if (tabName == "LastHours") {
 			addLogLine("newTab: LastHours");
 			webSocketConn.send("tabLastHours");
 	    TimerTab=setInterval(function(){
                    webSocketConn.send("tabLastHours");
                  },90000);

 		} else if (tabName == "LastDays") {
 			addLogLine("newTab: LastDays");
 			webSocketConn.send("tabLastDays");
 	    TimerTab=setInterval(function(){
                    webSocketConn.send("tabLastDays");
                  },120000);

 		} else if (tabName == "LastMonths") {
 			addLogLine("newTab: LastMonths");
 			webSocketConn.send("tabLastMonths");
 	    TimerTab=setInterval(function(){
                    webSocketConn.send("tabLastMonths");
                  },120000);
 		
 		} else if (tabName == "SysInfo") {
 			addLogLine("newTab: SysInfo");
 			webSocketConn.send("tabSysInfo");
 	    TimerTab=setInterval(function(){
                    webSocketConn.send("tabSysInfo");
                  },20000);

 		} else if (tabName == "Graphics") {
 			addLogLine("newTab: Graphics");
 			webSocketConn.send("tabGraphics");
 			//TimerChart = setInterval(plotChart, 1000, chart);
 		}
 	}	// openTab()

		
	function parsePayload(payload) {
		console.log("parsePayload("+payload+")");

 		singlePair   = payload.split(",");
 		var msgType = singlePair[0].split("=");

    if (msgType[1] == "devInfo") {
      	console.log("devInfo: "+devName+","+devVersion);
  			for ( var i = 1; i < singlePair.length; i++ ) {
  				onePair = singlePair[i].split("=");
  				console.log("set["+onePair[0].trim()+"] to["+onePair[1].trim()+"]");
  				document.getElementById(onePair[0].trim()).innerHTML = onePair[1].trim();
  				if (onePair[0].trim() == "settingBgColor") 		settingBgColor 		= onePair[1].trim();
  				if (onePair[0].trim() == "settingFontColor") 	settingFontColor 	= onePair[1].trim();
  			}
      //console.log("devInfo: BgColor["+settingBgColor+"],FontColor["+settingFontColor+"]");
      	document.getElementById('lastHoursTable').style.background 	= settingBgColor;
      	document.getElementById('lastDaysTable').style.background 	= settingBgColor;
      	document.getElementById('lastMonthsTable').style.background = settingBgColor;
  			css_getclass('.flexLabel200').style.background 							= settingBgColor; 
  			css_getclass('.flexLabel200').style.color 									= settingFontColor; 
  		//css_getclass('.flexLabel300').style.background = settingBgColor; 
  		//css_getclass('.flexLabel300').style.color = settingFontColor; 
  			css_getclass('.flexValue250').style.background 							= settingBgColor; 
  			css_getclass('.flexValue250').style.color 									= settingFontColor; 
  			css_getclass('.editButton a:hover').style.background 				= settingBgColor; 
  			css_getclass('.nav-item').style.color 											= settingFontColor; 
  			css_getclass('.nav-clock').style.background 								= settingBgColor; 
  			css_getclass('.nav-clock').style.color 											= settingFontColor; 
  			css_getclass('.nav-container').style.background 						= settingBgColor;
  			if (settingFontColor == 'black') {
  				css_getclass('.nav-container a:hover').style.color = 'white';
  			} else {
  				css_getclass('.nav-container a:hover').style.color = 'black';
  			}
  			openTab("Actual");
      
  	} else if (msgType[1] == "settings") {
  			for ( var i = 1; i < singlePair.length; i++ ) {
  				onePair = singlePair[i].split("=");
  				console.log("setting ["+onePair[0]+"] value ["+onePair[1]+"]");
  				if ( onePair[0].trim() == "LEDC" )		settingLineEDC = onePair[1].trim();
  				if ( onePair[0].trim() == "BEDC" )		settingBackEDC = onePair[1].trim();
  				if ( onePair[0].trim() == "LERC" )		settingLineERC = onePair[1].trim();
  				if ( onePair[0].trim() == "BERC" )		settingBackERC = onePair[1].trim();
  				if ( onePair[0].trim() == "LGDC" )		settingLineGDC = onePair[1].trim();
  				if ( onePair[0].trim() == "BGDC" )		settingBackGDC = onePair[1].trim();
  				if ( onePair[0].trim() == "LED2C" )		settingLineED2C = onePair[1].trim();
  				if ( onePair[0].trim() == "BED2C" )		settingBackED2C = onePair[1].trim();
  				if ( onePair[0].trim() == "LER2C" )		settingLineER2C = onePair[1].trim();
  				if ( onePair[0].trim() == "BER2C" )		settingBackER2C = onePair[1].trim();
  				if ( onePair[0].trim() == "LGD2C" )		settingLineGD2C = onePair[1].trim();
  				if ( onePair[0].trim() == "BGD2C" )		settingBackGD2C = onePair[1].trim();
  				if ( onePair[0].trim() == "LPD1C" )		settingLinePD1C = onePair[1].trim();
  				if ( onePair[0].trim() == "BPD1C" )		settingBackPD1C = onePair[1].trim();
  				if ( onePair[0].trim() == "LPD2C" )		settingLinePD2C = onePair[1].trim();
  				if ( onePair[0].trim() == "BPD2C" )		settingBackPD2C = onePair[1].trim();
  				if ( onePair[0].trim() == "LPD3C" )		settingLinePD3C = onePair[1].trim();
  				if ( onePair[0].trim() == "BPD3C" )		settingBackPD3C = onePair[1].trim();
  				if ( onePair[0].trim() == "LPR123C" )	settingLinePR123C = onePair[1].trim();
  				if ( onePair[0].trim() == "BPR123C" )	settingBackPR123C = onePair[1].trim();
  			}
  			
  	} else if (msgType[1] == "Actual") {
 				addLogLine("message received ["+msgType[1]+"]");
  			for ( var i = 1; i < singlePair.length; i++ ) {
  				onePair = singlePair[i].split("=");
  				if (onePair[0].trim() == "theTime")
  							var field = "theTime";
  				else	var field = "a" + onePair[0].trim();
  				document.getElementById(field).innerHTML = onePair[1].trim();
  			}

  		} else if (msgType[1] == "lastHoursHeaders") {
 				addLogLine("message received ["+msgType[1]+"]");
 				onePair = singlePair[1].split("=");
 				update24HoursHeaders(onePair[1]);

  		} else if (msgType[1] == "lastHoursRow") {
 				addLogLine("message received ["+msgType[1]+"]");
 				update24HoursTable(payload);
  		
  		} else if (msgType[1] == "lastDaysHeaders") {
 				addLogLine("message received ["+msgType[1]+"]");
 				onePair = singlePair[1].split("=");
 				updateDaysHeaders(onePair[1]);

  		} else if (msgType[1] == "lastDaysRow") {
 				addLogLine("message received ["+msgType[1]+"]");
 				updateDaysTable(payload);

  		} else if (msgType[1] == "lastMonthsHeaders") {
 				addLogLine("message received ["+msgType[1]+"]");
 				onePair = singlePair[1].split("=");
 				update24MonthsHeaders(onePair[1]);

  		} else if (msgType[1] == "lastMonthsRow") {
 				addLogLine("message received ["+msgType[1]+"]");
 				update24MonthsTable(payload);
  		
  		} else if (msgType[1] == "graphStart") {
 				addLogLine("message received ["+msgType[1]+"]");
 				graphStart();

  		} else if (msgType[1] == "graphRow") {
 				addLogLine("message received ["+msgType[1]+"]");
 				graphRow(payload);

 			} else if (msgType[1] == "sysInfo") {
 				addLogLine("message received ["+msgType[1]+"]");
  			for ( var i = 1; i < singlePair.length; i++ ) {
  				onePair = singlePair[i].split("=");
  		  //console.log(onePair[0].trim()+"=["+ onePair[1].trim()+"]");
  				document.getElementById(onePair[0].trim()).innerHTML = onePair[1].trim() + " ";
  				if (onePair[0].trim() == "FreeHeap") {
  					console.log("Found FreeHeap!!!");
  					if (parseInt(onePair[1]) < 8000) {
  						document.getElementById(onePair[0].trim()).innerHTML = onePair[1].trim() + " (te?) laag! ";
  						document.getElementById(onePair[0].trim()).style.color = "red";
  						document.getElementById(onePair[0].trim()).style.fontWeight = "bold";
  					} else {
  						document.getElementById(onePair[0].trim()).style.color = settingFontColor;
  						document.getElementById(onePair[0].trim()).style.fontWeight = "normal";
  					}
  				}
  			}
  	  } else if (msgType[1] == "clock") {
  			onePair = singlePair[1].split("=");
  			if (onePair[0].trim() == "DT") {
  				document.getElementById("theTime").innerHTML = onePair[1].trim();
  			}
  		} else {
  			addLogLine("parsePayload(): Don't know: [" + payload + "]\r\n");
  		}
  };	// parsePayload()
 	  
  
  function update24HoursHeaders(numRows) {
  	addLogLine("update24HoursHeaders("+numRows+")");
  	hoursMaxRows = numRows;

    // Find a <table> element with id="lastMonthsTable":
    var table = document.getElementById("lastHoursTable");
    
    //while(table.rows.length > 0) {table.deleteRow(0);}

    var headers = ["Uur (van-tot)", "Energie&nbsp;Verbruik (Wh)", 
    							 "Energie&nbsp;Teruglevering (Wh)", "Gas&nbsp;Verbruik (m3)", 
                   "Kosten&nbsp;in Euro" ];

   	if (!existingId("HH1")) {
   		var tableHeader = document.createElement("thead");
   		table.appendChild(tableHeader);
   		var tableHeaderRow = document.createElement("tr");
   		tableHeaderRow.setAttribute("id", "HH1", 0);
   		table.appendChild(tableHeaderRow);

   		for(var i=0;i<headers.length;i++){
        var tableHeader = document.createElement("th");
        //tableHeader.colSpan = 1;
        tableHeaderRow.appendChild(tableHeader);
        tableHeader.innerHTML = headers[i]
        tableHeader.style.border = "none";
        tableHeader.style.borderLeft = "thin solid black";
        tableHeader.style.borderBottom = "thin solid black";
        tableHeader.style.color = settingFontColor;
        if (i == 0) {
        	tableHeader.style.textAlign = "center";
        } else {
        	tableHeader.style.textAlign = "right";
        }
      }
    }
    for ( var i = 1; i < hoursMaxRows; i++ ) {
    	if (existingId("HR"+i)) {
    			continue;
    	}
      var row = table.insertRow(1);
      row.setAttribute("id", "HR" + i, 0);
      var cell0 = row.insertCell(0);				// van-tot
   		cell0 = setStyle(cell0, 120, "center", "thin", i);
      var cell1 = row.insertCell(1);				// gebruik
   		cell1 = setStyle(cell1, 80, "right", "thin", i);
			var cell2 = row.insertCell(2);				// levering
   		cell2 = setStyle(cell2, 120, "right", "thin", i);
      var cell3 = row.insertCell(3);				// gas
   		cell3 = setStyle(cell3, 80, "right", "thin", i);
      var cell4 = row.insertCell(4);				// Kosten
   		cell4 = setStyle(cell4, 80, "right", "thin", i);
    }

		webSocketConn.send("lastHoursRow,r=1");

  }	// update24HoursHeaders()
 	  
  
  function update24HoursTable(payload) {
  	if (activeTab != "LastHours") {
  		return;
  	}
  	singlePair   = payload.split(",");
  	var rowNr = singlePair[1].split("=");
  	var uRow = 1;
  	if (parseInt(rowNr[1]) == 99) {
  		uRow = hoursMaxRows - 1;
  	} else {
  		uRow  = parseInt(rowNr[1]); // - 1;
  	}

  	addLogLine("update24HoursTable("+rowNr[1]+"->["+uRow+"])"); 
    // Find a <table> element with id="lastMonthsTable":
    var table = document.getElementById("lastHoursTable");
    var tr = table.getElementsByTagName('tr')[uRow];
    var td;
    for ( var i = 0; i < singlePair.length; i++ ) {
    	var Data = singlePair[i].split("=");
    	addLogLine("update24HoursTable["+i+"]"+Data[0]+"="+Data[1]);
    	
    	if (Data[0] == "H") {
    		td = tr.getElementsByTagName('td')[0];
    		td.innerHTML = Data[1];
    	} else if (Data[0] == "ED") {
    		td = tr.getElementsByTagName('td')[1];
    		td.innerHTML = Data[1];
    	} else if (Data[0] == "ER") {
    		td = tr.getElementsByTagName('td')[2];
    		td.innerHTML = Data[1];
    	} else if (Data[0] == "GD") {
    		td = tr.getElementsByTagName('td')[3];
    		td.innerHTML = Data[1];
    	} else if (Data[0] == "COSTS") {
    		td = tr.getElementsByTagName('td')[4];
    		td.innerHTML = Data[1];
    	}
    	
    }
    
    if (parseInt(rowNr[1]) != 99) {
    	uRow = parseInt(rowNr[1]) + 1;
    	console.log("rowNr["+rowNr[1]+"] ===> uRow["+uRow+"]");
    	if (uRow > 0 && uRow < hoursMaxRows) {
    		webSocketConn.send("lastHoursRow?r="+ uRow);
    	}
    }

  }	// update24HoursTable()
 	  
  
  function updateDaysHeaders(numRows) {
  	addLogLine("updateDaysHeaders("+numRows+")");
  	if (activeTab != "LastDays") {
  		console.log("updateDaysHeaders(): activeTab changed to ["+activeTab+"]");
  		return;
  	}
  	daysMaxRows = numRows;
    // Find a <table> element with id="lastDaysTable":
    var table = document.getElementById("lastDaysTable");
    
    //while(table.rows.length > 0) {table.deleteRow(0);}

    var headers = ["Dag", "Energie Verbruik<br>(kWh)", "Energie Teruglevering<br>(kWh)", 
                   "Gas Verbruik<br>(m3)", "Kosten<br>in Euro" ];

   	if (!existingId("WH1")) {
   		var tableHeader = document.createElement("thead");
   		table.appendChild(tableHeader);
   		var tableHeaderRow = document.createElement("tr");
   		tableHeaderRow.setAttribute("id", "WH1", 0);
   		table.appendChild(tableHeaderRow);

   		for(var i=0;i<headers.length;i++){
        var tableHeader = document.createElement("th");
        //tableHeader.colSpan = 1;
        tableHeaderRow.appendChild(tableHeader);
        tableHeader.innerHTML = headers[i]
        tableHeader.style.border = "none";
        tableHeader.style.borderLeft = "thin solid black";
        tableHeader.style.borderBottom = "thin solid black";
        tableHeader.style.color = settingFontColor;
        if (i == 0) {
        	tableHeader.style.textAlign = "right";
        } else {
        	tableHeader.style.textAlign = "right";
        }
      }
    }
    for ( var i = 1; i < daysMaxRows; i++ ) {
    	if (existingId("WR"+i)) {
    			continue;
    	}
      var row = table.insertRow(1);
      row.setAttribute("id", "WR" + i, 0);
      var cell0 = row.insertCell(0);				// van-tot
   		cell0 = setStyle(cell0, 150, "right", "thin", i);
      var cell1 = row.insertCell(1);				// gebruik
   		cell1 = setStyle(cell1, 80, "right", "thin", i);
			var cell2 = row.insertCell(2);				// levering
   		cell2 = setStyle(cell2, 120, "right", "thin", i);
      var cell3 = row.insertCell(3);				// gas
   		cell3 = setStyle(cell3, 80, "right", "thin", i);
      var cell4 = row.insertCell(4);				// kosten
   		cell4 = setStyle(cell4, 80, "right", "thin", i);
    }

		console.log("send(lastDaysRow,r=1)");
		webSocketConn.send("lastDaysRow,r=1");

  }	// updateDaysHeaders()
 	  
  
  function updateDaysTable(payload) {
  	if (activeTab != "LastDays") {
  		console.log("updateDaysTable(): activeTab changed to ["+activeTab+"]");
  		return;
  	}
  	singlePair   = payload.split(",");
  	var rowNr = singlePair[1].split("=");
  	var uRow = 1;
  	if (parseInt(rowNr[1]) == 99) {
  		uRow = 1;
  	} else {
  		uRow  = parseInt(rowNr[1]); 
  	}

  	addLogLine("updateDaysTable("+rowNr[1]+"->["+uRow+"])"); 
    // Find a <table> element with id="lastDaysTable":
    var table = document.getElementById("lastDaysTable");
    var tr = table.getElementsByTagName('tr')[uRow];
    var td;
    for ( var i = 0; i < singlePair.length; i++ ) {
    	var Data = singlePair[i].split("=");
    	addLogLine("updateDaysTable["+i+"]"+Data[0]+"="+Data[1]);
    	
    	if (Data[0] == "D") {
    		td = tr.getElementsByTagName('td')[0];
    		td.innerHTML = Data[1];
    	} else if (Data[0] == "ED") {
    		td = tr.getElementsByTagName('td')[1];
    		td.innerHTML = Data[1];
    	} else if (Data[0] == "ER") {
    		td = tr.getElementsByTagName('td')[2];
    		td.innerHTML = Data[1];
    	} else if (Data[0] == "GD") {
    		td = tr.getElementsByTagName('td')[3];
    		td.innerHTML = Data[1];
    	} else if (Data[0] == "COSTS") {
    		td = tr.getElementsByTagName('td')[4];
    		td.innerHTML = Data[1];
    	}
    	
    }
    
    if (parseInt(rowNr[1]) != 99) {
    	uRow = parseInt(rowNr[1]) + 1;
    	console.log("rowNr["+rowNr[1]+"] ===> uRow["+uRow+"]");
    	if (uRow > 0 && uRow < daysMaxRows) {
    		webSocketConn.send("lastDaysRow?r="+ uRow);
    	}
    }

  }	// updateDayssTable()
 	  
  
  function update24MonthsHeaders(numRows) {
  	addLogLine("update24MonthsHeaders("+numRows+")");
  	monthsMaxRows = numRows;

    // Find a <table> element with id="lastMonthsTable":
    var table = document.getElementById("lastMonthsTable");
    
    //while(table.rows.length > 0) {table.deleteRow(0);}

    var headers1 = [" ", "Energie Verbruik", "Energie Teruglevering", "Gas Verbruik" ];
    var headers2 = ["Maand", "Jaar", "(kWh)", "Jaar", "(kWh)", "Jaar", "(kWh)", "Jaar", "(kWh)", "Jaar", "(m3)", "Jaar", "(m3)" ];

   	if (!existingId("MH1")) {
   		var tableHeader = document.createElement("thead");
   		table.appendChild(tableHeader);
   		var tableHeaderRow = document.createElement("tr");
   		tableHeaderRow.setAttribute("id", "MH1", 0);
   		table.appendChild(tableHeaderRow);
   		var tableHeader = document.createElement("th");
   		tableHeaderRow.appendChild(tableHeader);
   		tableHeader.innerHTML = headers1[0]
   		tableHeader.style.border = "none";

   		for(var i=1;i<headers1.length;i++){
      	 var tableHeader = document.createElement("th");
        tableHeader.colSpan = 4;
        tableHeaderRow.appendChild(tableHeader);
        tableHeader.innerHTML = headers1[i]
        tableHeader.style.border = "none";
        tableHeader.style.borderLeft = "thick solid black";
        tableHeader.style.color = settingFontColor;
      }
    }

   	if (!existingId("MH2")) {
   		var tableHeader = document.createElement("thead");
   		table.appendChild(tableHeader);
   		var tableHeaderRow = document.createElement("tr");
   		tableHeaderRow.setAttribute("id", "MH2", 0);
   		table.appendChild(tableHeaderRow);
   		for(var i=0;i<headers2.length;i++){
        var tableHeader = document.createElement("th");
        tableHeaderRow.appendChild(tableHeader);
        tableHeader.innerHTML = headers2[i]
        tableHeader.style.border = "none";
        tableHeader.style.borderBottom = "thin solid black";
        tableHeader.style.borderLeft = "thin solid black";
        tableHeader.style.color = settingFontColor;
        if (headers2[i] == "Maand") {
        	tableHeader.style.textAlign = "right";
          //tableHeader.style.borderRight = "thick solid black";
        }
        else if (i == 1 || i == 5 || i == 9) {
        	tableHeader.style.textAlign = "center";
          tableHeader.style.borderLeft = "thick solid black";
        }
        else {
        	tableHeader.style.textAlign = "right";
        }
      }
    }
    for ( var i = 0; i < monthsMaxRows; i++ ) {
    	if (existingId("MR"+i)) {
    			continue;
    	}
      var row = table.insertRow(2);
      row.setAttribute("id", "MR" + i, 0);
      var cell0 = row.insertCell(0);				// month
   		cell0 = setStyle(cell0, 150, "right", "thin", i);
      var cell1 = row.insertCell(1);				// year
   		cell1 = setStyle(cell1, 80, "center", "thick", i);
			var cell2 = row.insertCell(2);				// Energy
   		cell2 = setStyle(cell2, 120, "right", "thin", i);
      var cell3 = row.insertCell(3);				// year
   		cell3 = setStyle(cell3, 80, "center", "thin", i);
      var cell4 = row.insertCell(4);				// energy
   		cell4 = setStyle(cell4, 120, "right", "thin", i);
      var cell5 = row.insertCell(5);				// year
   		cell5 = setStyle(cell5, 80, "center", "thick", i);
      var cell6 = row.insertCell(6);
   		cell6 = setStyle(cell6, 120, "right", "thin", i);
      var cell7 = row.insertCell(7);
   		cell7 = setStyle(cell7, 80, "center", "thin", i);
      var cell8 = row.insertCell(8);
   		cell8 = setStyle(cell8, 120, "right", "thin", i);
      var cell9 = row.insertCell(9);
   		cell9 = setStyle(cell9, 80, "center", "thick", i);
      var cell10 = row.insertCell(10);
   		cell10 = setStyle(cell10, 100, "right", "thin", i);
      var cell11 = row.insertCell(11);
   		cell11 = setStyle(cell11, 80, "center", "thin", i);
      var cell12 = row.insertCell(12);
   		cell12 = setStyle(cell12, 100, "right", "thin", i);
    }

		console.log("lastMonthsRow,r=12");
		webSocketConn.send("lastMonthsRow,r=12");	// Bottum UP!

  }	// update24MonthsHeaders()
 	  
  
  function update24MonthsTable(payload) {
  	if (activeTab != "LastMonths") {
  		return;
  	}
  	singlePair   = payload.split(",");
  	var rowNr = singlePair[1].split("=");
  	var uRow = 0;
  	if (parseInt(rowNr[1]) == 99) {
  		uRow = 1;
  	} else {
  		uRow  = parseInt(rowNr[1]) + 1;
  	}

  	addLogLine("update24MonthsTable("+rowNr[1]+"->["+uRow+"])");
    // Find a <table> element with id="lastMonthsTable":
    var table = document.getElementById("lastMonthsTable");
    var tr = table.getElementsByTagName('tr')[uRow];
    var td;
    for ( var i = 0; i < singlePair.length; i++ ) {
      
    	// Add some text to the new cells:
    	var Data = singlePair[i].split("=");
    	addLogLine("["+i+"]"+Data[0]+"="+Data[1]);
    	if (Data[0] == "M") {
    		td = tr.getElementsByTagName('td')[0];
    		td.innerHTML = Data[1];
    	} else if (Data[0] == "Y1") {
    		td = tr.getElementsByTagName('td')[1];
    		td.innerHTML = Data[1];
    		td = tr.getElementsByTagName('td')[5];
    		td.innerHTML = Data[1];
    		td = tr.getElementsByTagName('td')[9];
    		td.innerHTML = Data[1];
    	} else if (Data[0] == "Y2") {
    		td = tr.getElementsByTagName('td')[3];
    		td.innerHTML = Data[1];
    		td = tr.getElementsByTagName('td')[7];
    		td.innerHTML = Data[1];
    		td = tr.getElementsByTagName('td')[11];
    		td.innerHTML = Data[1];
    	} else if (Data[0] == "ED1") {
    		td = tr.getElementsByTagName('td')[2];
    		td.innerHTML = Data[1];
    	} else if (Data[0] == "ED2") {
    		td = tr.getElementsByTagName('td')[4];
    		td.innerHTML = Data[1];
    	} else if (Data[0] == "ER1") {
    		td = tr.getElementsByTagName('td')[6];
    		td.innerHTML = Data[1];
    	} else if (Data[0] == "ER2") {
    		td = tr.getElementsByTagName('td')[8];
    		td.innerHTML = Data[1];
    	} else if (Data[0] == "GD1") {
    		td = tr.getElementsByTagName('td')[10];
    		td.innerHTML = Data[1];
    	} else if (Data[0] == "GD2") {
    		td = tr.getElementsByTagName('td')[12];
    		td.innerHTML = Data[1];
    	}
    }
    
    if (parseInt(rowNr[1]) != 99) {
    	uRow = parseInt(rowNr[1]) - 1;	// bottum Up!
    	if (uRow > 0 && uRow <= monthsMaxRows) {
    		console.log("lastMonthsRow?r="+ uRow);
    		webSocketConn.send("lastMonthsRow?r="+ uRow);
    	}
    }

  }	// update24MonthsTable()

 	
  function setDebugMode() {
  	if (document.getElementById('debug').checked)  {
  		addLogLine("DebugMode checked!");
  		document.getElementById('logWindow').style.display = "block";
  	} else {
  		addLogLine("DebugMode unchecked");
  		document.getElementById('logWindow').style.display = "none";
  	}
  }	// setDebugMode()
  
  function addLogLine(text) {
  	if (document.getElementById('debug').checked) {
  		let logWindow = document.getElementById("logWindow");
  		let myTime = new Date().toTimeString().replace(/.*(\d{2}:\d{2}:\d{2}).*/, "$1");
  		let addText = document.createTextNode("["+myTime+"] "+text+"\r\n");
  		logWindow.appendChild(addText);
  		document.getElementById("logWindow").scrollTop = document.getElementById("logWindow").scrollHeight 
  	} 
  }	// addLogLine()
  
  function round(value, precision) {
    var multiplier = Math.pow(10, precision || 0);
    return Math.round(value * multiplier) / multiplier;
  }

  function setStyle(cell, width, Align, Border, Val) {
    cell.style.width = width + 'px';
    cell.style.textAlign = Align;
    cell.style.border = "none";
    cell.style.borderLeft = Border + " solid black";
    cell.style.color = settingFontColor;
    cell.style.fontWeight = "normal";
		cell.innerHTML = Val;
		return cell;
		
  };	// setStyle()

  function cssrules() {
  		var rules = {};
  		for (var i=0; i<document.styleSheets.length; ++i) {
        var cssRules = document.styleSheets[i].cssRules;
        for (var j=0; j<cssRules.length; ++j)
          rules[cssRules[j].selectorText] = cssRules[j];
      }
      return rules;
  }	// cssrules()

  function css_getclass(name) {
    var rules = cssrules();
    if (!rules.hasOwnProperty(name))
        throw 'TODO: deal_with_notfound_case';
    return rules[name];
  }	// css_getclass()
  
  function existingId(elementId) {
    if(document.getElementById(elementId)){
      return true;
    } 
    console.log("cannot find elementId [" + elementId + "]");
    return false;
    
  } // existingId()
  
/*
***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************
*/
