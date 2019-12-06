/*
***************************************************************************  
**  Program  : DSMReditor.js, part of DSMRloggerWS
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
	let activeTab = "none";
	let monthType = "D";
  let singlePair;
  let onePair;
	let settingBgColor = 'deepskyblue';
	let settingFontColor = 'white'
  
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
    document.getElementById('bTerug').addEventListener('click',function()
    																						{location.href = "/";});
    document.getElementById('bEditMonths').addEventListener('click',function()
    																						{openTab('tabMonths');});
    document.getElementById('bEditSettings').addEventListener('click',function() 
    																						{openTab('tabSettings');});
    document.getElementById('bEditColors').addEventListener('click',function() 
    																						{openTab('tabColors');});
    document.getElementById('bUndo').addEventListener('click',function() 
    																						{undoReload();});
    document.getElementById('bSave').addEventListener('click',function() 
    																						{saveData();});
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
    //---- update buttons in navigation bar ---
 		let x = document.getElementsByClassName("editButton");
 		for (var i = 0; i < x.length; i++) {
			x[i].style.background			= settingBgColor;
 			x[i].style.border					= 'none';
 			x[i].style.textDecoration = 'none';  
 			x[i].style.outline    		= 'none';  
 			x[i].style.boxShadow 			= 'none';
 		}

		document.getElementById("DT1").addEventListener('focus',			function() {hoverOn('DT1');});
		document.getElementById("DT1").addEventListener('blus',				function() {hoverOn('DT1');});
		document.getElementById("DT1").addEventListener('mouseover',	function() {hoverOn('DT1');});
		document.getElementById("DT1").addEventListener('mouseout',		function() {hoverOff('DT1');});
		
		document.getElementById("DT2").addEventListener('focus',			function() {hoverOn('DT2');});
		document.getElementById("DT2").addEventListener('blur',				function() {hoverOff('DT2');});
		document.getElementById("DT2").addEventListener('mouseover',	function() {hoverOn('DT2');});
		document.getElementById("DT2").addEventListener('mouseout',		function() {hoverOff('DT2');});

		document.getElementById("RT1").addEventListener('focus',			function() {hoverOn('RT1');});
		document.getElementById("RT1").addEventListener('blur',				function() {hoverOff('RT1');});
		document.getElementById("RT1").addEventListener('mouseover',	function() {hoverOn('RT1');});
		document.getElementById("RT1").addEventListener('mouseout',		function() {hoverOff('RT1');});

		document.getElementById("RT2").addEventListener('focus',			function() {hoverOn('RT2');});
		document.getElementById("RT2").addEventListener('blur',				function() {hoverOff('RT2');});
		document.getElementById("RT2").addEventListener('mouseover',	function() {hoverOn('RT2');});
		document.getElementById("RT2").addEventListener('mouseout',		function() {hoverOff('RT2');});
		
		document.getElementById("ENBK").addEventListener('focus',			function() {hoverOn('ENBK');});
		document.getElementById("ENBK").addEventListener('blur',			function() {hoverOff('ENBK');});
		document.getElementById("ENBK").addEventListener('mouseover',	function() {hoverOn('ENBK');});
		document.getElementById("ENBK").addEventListener('mouseout',	function() {hoverOff('ENBK');});
		
		document.getElementById("GAST").addEventListener('focus',			function() {hoverOn('GAST');});
		document.getElementById("GAST").addEventListener('blur',			function() {hoverOff('GAST');});
		document.getElementById("GAST").addEventListener('mouseover',	function() {hoverOn('GAST');});
		document.getElementById("GAST").addEventListener('mouseout',	function() {hoverOff('GAST');});
		
		document.getElementById("GNBK").addEventListener('focus',			function() {hoverOn('GNBK');});
		document.getElementById("GNBK").addEventListener('blur',			function() {hoverOff('GNBK');});
		document.getElementById("GNBK").addEventListener('mouseover',	function() {hoverOn('GNBK');});
		document.getElementById("GNBK").addEventListener('mouseout',	function() {hoverOff('GNBK');});


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
//  openTab("tabMonths");
//	addLogLine("send(sendMonths)");
//	webSocketConn.send("sendMonths");
   	needReload	= false;

  }; 
  
  webSocketConn.onclose 		= function () { 
   	addLogLine(" ");
   	addLogLine("Disconnected!");
   	addLogLine(" ");
   	needReload	= true;
   	let redirectButton = "<p></p><hr><p></p><p></p>"; 
   	redirectButton    += "<style='font-size: 50px;'>Disconneted from DSMRlogger"; 
   	redirectButton    += "<input type='submit' value='re-Connect' "; 
    redirectButton    += " onclick='window.location=\"/\";' />  ";     
   	redirectButton    += "<p></p><p></p><hr><p></p>"; 
		document.getElementById("Maanden").innerHTML = redirectButton;

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
    
 		let bID = "b" + tabName;
    let i;
    //---- update buttons in navigation bar ---
		console.log("openTab: First set all fields in [tabName] to none ..");
 		let x = document.getElementsByClassName("tabName");	// set all fields to "none"
 		for (i = 0; i < x.length; i++) {
 			console.log("Field["+i+"] set to none");
 			x[i].style.display    = "none";  
			x[i].style.background			= settingBgColor;
 			x[i].style.border					= 'none';
 			x[i].style.textDecoration = 'none';  
 			x[i].style.outline    		= 'none';  
 			x[i].style.boxShadow 			= 'none';
 		}
 		//--- hide all tab's -------
// 		x = document.getElementsByClassName("tabName");
// 		for (i = 0; i < x.length; i++) {
// 			x[i].style.display    = "none";  
// 		}
 		//--- and set active tab to 'block'
		console.log("openTab: now set all fields in ["+tabName+"] to block ..");
    document.getElementById(tabName).style.background='white';
 		document.getElementById(tabName).style.display = "block";  
    if (tabName == "tabMonths") {
 			addLogLine("newTab: tabMonths");
 					addLogLine("send(sendMonths)");
 					webSocketConn.send("sendMonths");
 		} else if (tabName == "tabSettings") {
 			addLogLine("newTab: tabSettings");
 					webSocketConn.send("sendSettings");
 		} else if (tabName == "tabColors") {
 			addLogLine("newTab: tabColors");
 					webSocketConn.send("sendColors");
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
  			document.getElementById(onePair[0].trim()).innerHTML = onePair[1].trim();
  			if (onePair[0].trim() == "settingBgColor") 		settingBgColor 		= onePair[1].trim();
  			if (onePair[0].trim() == "settingFontColor") 	settingFontColor 	= onePair[1].trim();
  		}
     	document.getElementById('maandenTable').style.background = settingBgColor;
 			css_getclass('.flexLabel200').style.background = settingBgColor; 
 			css_getclass('.flexLabel200').style.color = settingFontColor; 
 		  css_getclass('.flexLabel300').style.background = settingBgColor; 
 		  css_getclass('.flexLabel300').style.color = settingFontColor; 
 			css_getclass('.flexValue250').style.background = settingBgColor; 
 			css_getclass('.flexValue250').style.color = settingFontColor; 
 			css_getclass('.editButton').style.background = settingBgColor; 
 			css_getclass('.editButton a:hover').style.background = 'gray'; 
 			css_getclass('.nav-item').style.color = settingFontColor; 
 			css_getclass('.nav-item').style.background = settingBgColor; 
 			css_getclass('.nav-clock').style.background = settingBgColor; 
 			css_getclass('.nav-clock').style.color = settingFontColor; 
 			css_getclass('.nav-container').style.background = settingBgColor;
 			if (settingFontColor == 'black') {
 				css_getclass('.nav-container a:hover').style.color = 'white';
 			} else {
 				css_getclass('.nav-container a:hover').style.color = 'black';
 			}

 		} else if (msgType[1] == "editMonthsHeaders") {
 			addLogLine("message received ["+msgType[1]+"]");
 			editMonthsHeaders();

  	} else if (msgType[1] == "editMonthsRow") {
 			addLogLine("message received ["+msgType[1]+"]");
 			editMonthsTable(payload);

  	} else if (msgType[1] == "settings") {
 			addLogLine("message received ["+msgType[1]+"]");
 			editSettings(payload);

  	} else if (msgType[1] == "colors") {
 			addLogLine("message received ["+msgType[1]+"]");
 			editColors(payload);
      
 	  } else if (msgType[1] == "clock") {
 			onePair = singlePair[1].split("=");
 			if (onePair[0].trim() == "DT") {
 				document.getElementById("theTime").innerHTML = onePair[1].trim();
 			}
  	} else {
  		addLogLine("parsePayload(): Don't know: [" + payload + "]\r\n");
  	}
  };	// parsePayload()
 	  
  
  function editMonthsHeaders() {
  	addLogLine("editMonthsHeaders()");
    // Find a <table> element with id="last24MonthsTable":
    var table = document.getElementById("maandenTable");
    table.innerHTML = "";
    //while(table.rows.length > 0) {table.deleteRow(0);}

   	var headers1 = [" ", "Slimme Meter Standen" ];
    if (monthType == "D")
    	var headers2 = ["R","Jaar", "Maand", "Verbruik<br>Tarief 1", "Verbruik<BR>Tarief 2"];
    else if (monthType == "R")
    	var headers2 = ["R","Jaar", "Maand", "Teruglevering<br>Tarief 1", "Teruglevering<BR>Tarief 2"];
    else if (monthType == "G")
    	var headers2 = ["R","Jaar", "Maand", "Gas Verbruik<br>Tarief p/m3"];
    else
    	var headers2 = ["R","Jaar", "Maand", "error", "error"];
    
   	if (!existingId("MH1")) {
   		var tableHeader = document.createElement("thead");
   		table.appendChild(tableHeader);
   		var tableHeaderRow = document.createElement("tr");
   		tableHeaderRow.setAttribute("id", "MH1", 0);
   		table.appendChild(tableHeaderRow);
   		var tableHeader = document.createElement("th");
      tableHeader.colSpan = 3;
   		tableHeaderRow.appendChild(tableHeader);
   		tableHeader.innerHTML = headers1[0]
   		tableHeader.style.border = "none";
    	var tableHeader = document.createElement("th");
      tableHeader.colSpan = 3;
      tableHeaderRow.appendChild(tableHeader);
      tableHeader.innerHTML = headers1[1]
      tableHeader.style.border = "none";
      tableHeader.style.borderLeft = "thin solid black";
      tableHeader.style.color = settingFontColor;
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
        tableHeader.style.color = "black";
        if (headers2[i] == "R" || headers2[i] == "Jaar" || headers2[i] == "Maand") {
        	tableHeader.style.textAlign = "center";
          //tableHeader.style.borderRight = "thick solid black";
        
        } else {
        	tableHeader.style.textAlign = "right";
          tableHeader.style.borderLeft = "thin solid black";
        }
      }
    }
    for ( var i = 0; i < 25; i++ ) {
    	if (existingId("MR"+i)) {
    			continue;
    	}
      var row = table.insertRow(2);
      row.setAttribute("id", "MR" + i, 0);
      var cell0 = row.insertCell(0);				// RecNo
   		cell0 = setStyle(cell0, 10, "center", "thin", (25-i));
      var cell1 = row.insertCell(1);				// Year
   		cell1 = setStyle(cell1, 80, "center", "thin", i);
      var cell2 = row.insertCell(2);				// Month
   		cell2 = setStyle(cell2, 70, "center", "thin", i);
			var cell3 = row.insertCell(3);				// Tarief 1
   		cell3 = setStyle(cell3, 120, "right", "thin", i);
   		if (monthType != "G") {
   			var cell4 = row.insertCell(4);				// Tarief 2
   			cell4 = setStyle(cell4, 120, "right", "thin", i);
   		}
    }
 		var table = document.getElementById("maandenTable");
    // table rows
    for(var r = 1; r < table.rows.length; r++) {
      table.rows[r].cells[0].style.color = "black";
      // row cells
      for(var c = 1; c < table.rows[r].cells.length; c++) {
        //console.log("Row : ["+i+"] , Cell : ["+j+"]");
        table.rows[r].cells[c].style.color = settingFontColor;
      }
    }

		console.log("editMonthsRow,r=25");
		webSocketConn.send("editMonthsRow,r=25");	// Bottum UP!

  }	// update24MonthsHeaders()
 	  
  
  function editMonthsTable(payload) {
  	singlePair   = payload.split(",");
  	var rowNr = singlePair[1].split("=");
  	var uRow = 0;
  	var tdID;
  	if (parseInt(rowNr[1]) == 99) {
  		uRow = 1;
  	} else {
  		uRow  = parseInt(rowNr[1]) + 1;
  	}

  	addLogLine("editMonthsTable("+rowNr[1]+"->["+uRow+"])");
    // Find a <table> element with id="last24MonthsTable":
    var table = document.getElementById("maandenTable");
    var tr = table.getElementsByTagName('tr')[uRow];
    var td;
    for ( var i = 0; i < singlePair.length; i++ ) {
      
    	// Add some text to the new cells:
    	var Data = singlePair[i].split("=");
    	addLogLine("["+i+"]"+Data[0]+"="+Data[1]);
    	if (Data[0] == "R") {
    		td = tr.getElementsByTagName('td')[0];
    		td.innerHTML = Data[1];
    		td.setAttribute("id", "R" + (uRow - 1));
    	} else if (Data[0] == "Y") {
    		td = tr.getElementsByTagName('td')[1];
    		td.innerHTML = Data[1];
    		td.setAttribute("id", "Y-" + (uRow - 1));
    		if (uRow > 2)	{
    			td.addEventListener('mouseover',function() {hoverOn('Y-'+(uRow + 1));});
    			td.addEventListener('mouseout',	function() {hoverOff('Y-'+(uRow + 1));});
    			td.addEventListener('click',		function() {editValue('Y', (uRow + 1), 0);});
    		}
    	} else if (Data[0] == "M") {
    		td = tr.getElementsByTagName('td')[2];
    		td.innerHTML = Data[1];
    		td.setAttribute("id", "M-" + (uRow - 1));
    		if (uRow > 2)	{
    			td.addEventListener('mouseover',function() {hoverOn('M-'+(uRow + 1));});
    			td.addEventListener('mouseout',	function() {hoverOff('M-'+(uRow + 1));});
    			td.addEventListener('click',		function() {editValue('M', (uRow + 1), 0);});
    		}
    	}
    	if (monthType == "D") {
    		if (Data[0] == "EDT1") {
    			td = tr.getElementsByTagName('td')[3];
    			td.innerHTML = Data[1];
    			td.setAttribute("id", "EDT1-" + (uRow - 1));
    			if (uRow > 2)	{
    				td.addEventListener('mouseover',function() {hoverOn('EDT1-'+(uRow + 1));});
    				td.addEventListener('mouseout',	function() {hoverOff('EDT1-'+(uRow + 1));});
    				td.addEventListener('click',		function() {editValue('EDT1', (uRow + 1), 3);});
    			}
    		} else if (Data[0] == "EDT2") {
    			td = tr.getElementsByTagName('td')[4];
    			td.innerHTML = Data[1];
    			td.setAttribute("id", "EDT2-" + (uRow - 1));
    			if (uRow > 2) {
    				td.addEventListener('mouseover',function() {hoverOn('EDT2-'+(uRow + 1));});
    				td.addEventListener('mouseout',	function() {hoverOff('EDT2-'+(uRow + 1));});
    				td.addEventListener('click',		function() {editValue('EDT2', (uRow + 1), 3);});
    			}
    		}
    	} else if (monthType == "R") {
    		if (Data[0] == "ERT1") {
    			td = tr.getElementsByTagName('td')[3];
    			td.innerHTML = Data[1];
    			td.setAttribute("id", "ERT1-" + (uRow - 1));
    			if (uRow > 2)	{
    				td.addEventListener('mouseover',function() {hoverOn('ERT1-'+(uRow + 1));});
    				td.addEventListener('mouseout',	function() {hoverOff('ERT1-'+(uRow + 1));});
    				td.addEventListener('click',		function() {editValue('ERT1', (uRow + 1), 3);});
    			}
    		} else if (Data[0] == "ERT2") {
    			td = tr.getElementsByTagName('td')[4];
    			td.innerHTML = Data[1];
    			td.setAttribute("id", "ERT2-" + (uRow - 1));
    			if (uRow > 2)	{
    				td.addEventListener('mouseover',function() {hoverOn('ERT2-'+(uRow + 1));});
    				td.addEventListener('mouseout',	function() {hoverOff('ERT2-'+(uRow + 1));});
    				td.addEventListener('click',		function() {editValue('ERT2', (uRow + 1), 3);});
    			}
    		}
    	} else if (monthType == "G") {	
    		if (Data[0] == "GAS") {
    			td = tr.getElementsByTagName('td')[3];
    			td.innerHTML = Data[1];
    			td.setAttribute("id", "GAS-" + (uRow - 1));
    			if (uRow > 2)	{
    				td.addEventListener('mouseover',function() {hoverOn('GAS-'+(uRow + 1));});
    				td.addEventListener('mouseout',	function() {hoverOff('GAS-'+(uRow + 1));});
    				td.addEventListener('click',		function() {editValue('GAS', (uRow + 1), 3);});
    			}
    		}
    	}
    }
    
    if (parseInt(rowNr[1]) != 99) {
    	uRow = parseInt(rowNr[1]) - 1;	// bottum Up!
    	if (uRow > 0 && uRow <= 24) {
    		console.log("editMonthsRow?r="+ uRow);
    		webSocketConn.send("editMonthsRow?r="+ uRow);
    	}
    }

  }	// update24MonthsTable()

  
  function editSettings(payload) {
  	singlePair   = payload.split(",");
		for ( var i = 1; i < singlePair.length; i++ ) {
			onePair = singlePair[i].split("=");
			console.log("setting ["+onePair[0]+"] value ["+onePair[1]+"]");
			if (   onePair[0].trim() == "DT1" || onePair[0].trim() == "DT2"
					|| onePair[0].trim() == "RT1" || onePair[0].trim() == "RT2"				
					|| onePair[0].trim() == "GAST" 			
					|| onePair[0].trim() == "ENBK" || onePair[0].trim() == "GNBK"	
					|| onePair[0].trim() == "Interval" || onePair[0].trim() == "SleepTime")
						document.getElementById(onePair[0].trim()).value = onePair[1].trim() * 1;
			else	document.getElementById(onePair[0].trim()).value = onePair[1].trim();
    	document.getElementById(onePair[0].trim()).style.color = settingFontColor;

		}

  }	// editSettings

  
  function editColors(payload) {
  	singlePair   = payload.split(",");
		for ( var i = 1; i < singlePair.length; i++ ) {
			onePair = singlePair[i].split("=");
			console.log("color ["+onePair[0]+"] value ["+onePair[1]+"]");
			document.getElementById(onePair[0].trim()).value = onePair[1].trim();
    	document.getElementById(onePair[0].trim()).style.color = settingFontColor;

		}

  }	// editColors


 	function editValue(Field, Row, Dec) {
 		console.log("editValue(): Field["+Field+"] Row["+Row+"]");

 		var tdID 		= Field + "-" + Row;
 		var fieldName = "";
		var Value 	= document.getElementById(tdID).innerText; 
    document.getElementById(tdID).style.color="black";	// does not work (until after prompt()
    
    var Year  	= document.getElementById("Y-"+Row).innerText;
    var Month  	= document.getElementById("M-"+Row).innerText;
    if (Field == "EDT1") fieldName = "meterstand Energy Delivered Tarief 1";
    if (Field == "EDT2") fieldName = "meterstand Energy Delivered Tarief 2";
    if (Field == "ERT1") fieldName = "meterstand Energy Returned Tarief 1";
    if (Field == "ERT2") fieldName = "meterstand Energy Returned Tarief 2";
    if (Field == "GAS")  fieldName = "meterstand GAS per/m3";
    if (Field == "Y")    fieldName = "Jaar (EEJJ)";
    if (Field == "M")    fieldName = "Maand (0-12)";
    	
    var message = "Voer de "+fieldName+" in voor " + Year + "-" + Month;
  //var newVal 	= prompt("Nieuw waarde "+tdID+" "+Month+" "+Year, Value);
 		var newVal 	= prompt(message, (Value * 1));
 		if (newVal == null)	newVal = Value;
    document.getElementById(tdID).style.color = settingFontColor;
    var fixVal 	= (newVal*1).toFixed(Dec);
    //if (fixVal !=  0) {
    	document.getElementById(tdID).innerText = fixVal;
    	if (fixVal != Value) {
    		document.getElementById(tdID).style.color = "gray";
    	}
    //}
    checkValidity();

 	}	// editValue()
	 	
 	
 	function saveData() {
  	if (activeTab == "tabMonths") {
  		saveMonths();
  	} else if (activeTab == "tabSettings") {
  		console.log("saveData(): save Settings..");
  		saveSettings();
	   	webSocketConn.send("getDevInfo");
  		webSocketConn.send("sendSettings");

  	} else if (activeTab == "tabColors") {
  		console.log("saveData(): save Colors..");
  		saveColors();
	   	webSocketConn.send("getDevInfo");
  		webSocketConn.send("sendColors");

  	} else {
  		console.log("saveData(): I don't know what to do ..");
  	}
 		
 	}	// saveData()

 	
 	function saveMonths() {
 		var Y, M, ET1, ET2, GAS;
 		var save = false;
 		var table = document.getElementById("maandenTable");
 		var nr = table.rows.length;
 		console.log("There are ["+nr+"] rows in this table");
    // table rows
    for(var i = 1; i < table.rows.length; i++) {	// i = recnr
    	console.log("Processing row ["+i+"]");
      // row cells
   		save = false;
      for(var j = 1; j < table.rows[i].cells.length; j++) {
        //console.log("Row : ["+i+"] , Cell : ["+j+"]");
        if (table.rows[i].cells[j].style.color != settingFontColor) {
        	save 	= true;
        	Y 		= document.getElementById( "Y-"+(i-1) ).innerText;
        	M 		= document.getElementById( "M-"+(i-1) ).innerText;
        	if (monthType == "D") {
        		ET1 	= document.getElementById( "EDT1-"+(i-1) ).innerText;
        		ET2		= document.getElementById( "EDT2-"+(i-1) ).innerText;
        	} else if (monthType == "R") {
        		ET1 	= document.getElementById( "ERT1-"+(i-1) ).innerText;
        		ET2		= document.getElementById( "ERT2-"+(i-1) ).innerText;
        	} else if (monthType == "G") {
        		GAS 	= document.getElementById( "GAS-"+(i-1) ).innerText;
        	}
        	table.rows[i].cells[j].style.color = settingFontColor;	// change back
        }	// color != settingFontColor...
      }	// for cells ..
      if (save) {
       	if (monthType == "D") {
       		console.log("updateMonth?r="+(i-1)+",Y="+Y+",M="+M+",EDT1="+ET1+",EDT2="+ET2);	
       		webSocketConn.send("updateMonth?r="+(i-1)+",Y="+Y+",M="+M+",EDT1="+ET1+",EDT2="+ET2);
       	} else if (monthType == "R") {
       		console.log("updateMonth?r="+(i-1)+",Y="+Y+",M="+M+",ERT1="+ET1+",ERT2="+ET2);	
       		webSocketConn.send("updateMonth?r="+(i-1)+",Y="+Y+",M="+M+",ERT1="+ET1+",ERT2="+ET2);
       	} else if (monthType == "G") {
       		console.log("updateMonth?r="+(i-1)+",Y="+Y+",M="+M+",GAS="+GAS);	
       		webSocketConn.send("updateMonth?r="+(i-1)+",Y="+Y+",M="+M+",GAS="+GAS);
       	}
   		}	// save
    }	// for .. rows
 	}	// saveMonths()
	
 	
 	function checkValidity() {
 		var EEYYMMp=0,EEYYMM=0,Y=0, M=0, EDT1=0, EDT2=0, ERT1=0, ERT2=0, GD=0;
 		var table = document.getElementById("maandenTable");
 		var nr = table.rows.length;
 		//console.log("There are ["+nr+"] rows in this table");
    // table rows
      for(var i = (table.rows.length - 2); i >= 2; i--) {
    	  console.log("checkValidity(): Processing row ["+i+"]");

        EEYYMM = document.getElementById(table.rows[i].cells[1].id).innerText * 1;
        EEYYMM = (EEYYMM * 100) + document.getElementById(table.rows[i].cells[2].id).innerText * 1;
        
       	if (EEYYMMp > EEYYMM) {
       		console.log("EEYYMMp["+EEYYMMp+"] > than ["+EEYYMM+"]");
       		table.rows[i].cells[1].style.color = "red";	// error
       		table.rows[i].cells[2].style.color = "red";	// error 
       	}
       	if (existingId("EDT1-"+(i))) {
     			console.log("EDT1p["+EDT1+"] ? EDT1-"+(i)+ " value ["+document.getElementById("EDT1-"+(i)).innerText+"]");
       		if (EDT1 > (document.getElementById("EDT1-"+(i)).innerText * 1)) {
       			console.log("EDT1p["+EDT1+"] > than ["+document.getElementById("EDT1-"+(i)).innerText+"]");
       			document.getElementById("EDT1-"+(i)).style.color = "red";	// error
       		}
       		if (EDT2 > (document.getElementById("EDT2-"+(i)).innerText * 1)) {
       			console.log("EDT2p["+EDT2+"] > than ["+document.getElementById("EDT2-"+(i)).innerText+"]");
       			document.getElementById("EDT2-"+(i)).style.color = "red";	// error 
       		}
       	}
       	if (existingId("ERT1-"+i)) {
       		if (ERT1 > document.getElementById("ERT1-"+i).innerText) {
       			console.log("ERT1p["+ERT1+"] > than ["+document.getElementById("ERT1-"+i).innerText+"]");
       			document.getElementById("ERT1-"+(i)).style.color = "red";	// error 
       		}
       		if (ERT2 > document.getElementById("ERT2-"+(i)).innerText) {
       			console.log("ERT2p["+ERT2+"] > than ["+document.getElementById("ERT2-"+(i)).innerText+"]");
       			document.getElementById("ERT2-"+(i)).style.color = "red";	// error 
       		}
       	}
       	if (existingId("GAS-"+i)) {
       		if (GD > document.getElementById("GAS-"+i).innerText) {
       			console.log("GDp["+GD+"] > than ["+document.getElementById("GAS-"+i).innerText+"]");
       			document.getElementById("GAS-"+i).style.color = "red";	// error 
       		}
        }
        EEYYMMp = EEYYMM * 1;
        if (monthType == "D") {
        	EDT1 = document.getElementById("EDT1-"+i).innerText * 1;
        	EDT2 = document.getElementById("EDT2-"+i).innerText * 1;
        } else if (monthType == "R") {
        	ERT1 = document.getElementById("ERT1-"+i).innerText * 1;
        	ERT2 = document.getElementById("ERT2-"+i).innerText * 1;
        } else if (monthType == "G") {
        	GD = document.getElementById("GAS-"+i).innerText * 1;
        }
//    }
    }
 	}	// check validity
 	
 	
 	function saveSettings() {
		//console.log("saveSettings() ...");
    var fields = ["DT1",   "DT2",  "RT1",  "RT2",  "GAST", "ENBK", "GNBK"												//  7
    	           ,"BgColor", "FontColor", "Interval", "SleepTime"														 	  // 11
				   ,"MQTTbroker", "MQTTuser", "MQTTpasswd", "MQTTtopTopic", "MQTTinterval"
				   ,"MindergasAuthtoken" ];	    // 17

    var allFields = "";
 		for(var i=0;i<fields.length;i++){
 			//console.log("setting"+fields[i]);
 			allFields += ","+fields[i]+"="+document.getElementById(fields[i]).value;
 		}
		console.log("saveSettings"+allFields);	
		webSocketConn.send("saveSettings"+allFields);

 	}	// saveSettings()
 	
 	
 	function saveColors() {
 		//console.log("saveColors() ...");
    var fields = ["LEDC",  "BEDC", "LERC", "BERC", "LGDC", "BGDC", "LED2C", "BED2C"							//  8
    						 ,"LER2C", "BER2C", "LGD2C", "BGD2C"																						// 12
    					   ,"LPD1C", "BPD1C", "LPD2C", "BPD2C", "LPD3C", "BPD3C", "LPR123C", "BPR123C" ];	// 20

    var allFields = "";
 		for(var i=0;i<fields.length;i++){
 			//console.log("color"+fields[i]);
 			allFields += ","+fields[i]+"="+document.getElementById(fields[i]).value;
 		}
		console.log("saveColors"+allFields);	
		webSocketConn.send("saveColors"+allFields);

 	}	// saveColors()
 	
 	
  function undoReload() {
  	if (activeTab == "tabMonths") {
  		webSocketConn.send("sendMonths");

  	} else if (activeTab == "tabSettings") {
  		console.log("undoReload(): reload Settings..");
			webSocketConn.send("sendSettings");

  	} else {
  		console.log("undoReload(): I don't knwo what to do ..");
  	}

 	}	// undoRelaod()
 	
 	
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
   
  
  function validateNumber(field) {
  	console.log("validateNumber(): ["+field+"]");
  	if (field == "EDT1" || field == "EDT2" || field == "ERT1" || field == "ERT2" || field == "GAS") {
  		var pattern = /^\d{1,1}(\.\d{1,5})?$/;
  		var max = 1.99999;
  	} else {
  		var pattern = /^\d{1,2}(\.\d{1,2})?$/;
  		var max = 99.99;
  	}
  	var newVal = document.getElementById(field).value;
  	newVal = newVal.replace( /[^0-9.]/g, '' );
    if (!pattern.test(newVal)) {
    	document.getElementById(field).style.color = 'orange';
  		console.log("wrong format");
  	} else {
    	document.getElementById(field).style.color = settingFontColor;
  		console.log("valid number!");
  	}
  	if (newVal > max) {
  		console.log("Number to big!");
  		document.getElementById(field).style.color = 'orange';
  		newVal = max;
  	}
 		document.getElementById(field).value = newVal * 1;
  	
  }	// validateNumber()
  
  function setEditType(eType) {
  	if (eType == "D")	{
  		console.log("Edit Energy Delivered!");
  		monthType = eType;
  	} else if (eType == "R") {
  		console.log("Edit Energy Returned!");
  		monthType = eType;
  	} else if (eType == "G") {
  		console.log("Edit Gas Delivered!");
  		monthType = eType;
    } else {
    	console.log("setEditType to ["+eType+"] is quit shitty!");
  		monthType = "";
    }
		webSocketConn.send("sendMonths");

  }	// setEditType()

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

  function hoverOn(field) {
  	document.getElementById(field).style.background = "lightgray";
  }	// hoverOn()
  function hoverOff(field) {
  	document.getElementById(field).style.background = settingBgColor;
  }	// hoverOff()

  function existingId(elementId) {
    if(document.getElementById(elementId)){
      return true;
    } 
  //console.log("cannot find elementId [" + elementId + "]");
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
