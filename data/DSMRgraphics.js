/*
***************************************************************************  
**  Program  : DSMRgraphics.js, part of DSMRloggerWS
**  Version  : v0.4.4
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
var Label   	 	= [];
var Delivered  	= [];
var Returned 	 	= []; 
var Gas     	 	= []; 
var Delivered2 	= [];
var Returned2	 	= []; 
var Gas2    	 	= []; 
var DeliveredL1	= [];
var DeliveredL2	= [];
var DeliveredL3	= [];
let TimerActual;

var ctx = document.getElementById('chartEnergy').getContext('2d');
//var myChart = new Chart(ctx, options);
var myChart = new Chart(ctx, {
    type: chartType,
    data: {},
    options: {}
  });

chartInit();

function chartInit() {
  myChart.destroy();
  // Chart declaration:
  if (graphType == "A") {						// actual
  	myChart = new Chart(ctx, {
  			type: chartType,
  			data: {
  				labels: Label,
  				datasets: [
  					{
  						label: 'Teruggave (L1+L2+L3)',
  						stack: 'L123',
  						data: Returned,
  						fill: false,
  						borderColor: settingLinePR123C,
  						backgroundColor: settingBackPR123C,
  						borderWidth: 2
  					},
  					{
  						label: 'L1',
  						stack: 'L123',
  						data: DeliveredL1,
  						fill: false,
  						borderColor: settingLinePD1C,
  						backgroundColor: settingBackPD1C,
  						borderWidth: 2
  					},
  					{
  						label: 'L2',
  						stack: 'L123',
  						data: DeliveredL2,
  						fill: false,
  						borderColor: settingLinePD2C,
  						backgroundColor: settingBackPD2C,
  						borderWidth: 2
  					},
  					{
  						label: 'L3',
  						stack: 'L123',
  						data: DeliveredL3,
  						fill: false,
  						borderColor: settingLinePD3C,
  						backgroundColor: settingBackPD3C,
  						borderWidth: 2
  					}
  				]
  			},
  			options: {
  				responsive: true,
  				maintainAspectRatio: true,
  				scales: {
            yAxes: [{
                scaleLabel: {
                	display: true,
                	labelString: 'Watt'
                }
            }]
          }
        }
  	});
  } else if (graphType == "Y") {		// Year 
  	myChart = new Chart(ctx, {
  			type: chartType,
  			data: {
  				labels: Label,
  				datasets: [
  					{
  						label: 'Verbruik deze periode',
  						stack: 'thisYear',
  						data: Delivered,
  						fill: false,
  						borderColor: settingLineEDC,
  						backgroundColor: settingBackEDC,
  						borderWidth: 2
  					},
  					{
  						label: 'Teruggave deze periode',
  						stack: 'thisYear',
  						data: Returned,
  						fill: false,
  						borderColor: settingLineERC,
  						backgroundColor: settingBackERC,
  						borderWidth: 2
  					},
  					{
  						label: 'Verbruik vorige periode',
  						stack: 'lastYear',
  						data: Delivered2,
  						fill: false,
  						borderColor: settingLineED2C,
  						backgroundColor: settingBackED2C,
  						borderWidth: 2
  					},
  					{
  						label: 'Teruggave vorige periode',
  						stack: 'lastYear',
  						data: Returned2,
  						fill: false,
  						borderColor: settingLineER2C,
  						backgroundColor: settingBackER2C,
  						borderWidth: 2
  					}
  				]
  			},
  			options: {
  				responsive: true,
  				maintainAspectRatio: true,
  				scales: {
            yAxes: [{
                scaleLabel: {
                	display: true,
                	labelString: 'kWh'
                }
            }]
          }
        }
  	});
  } else if (graphType == "W") {			// Day
  	myChart = new Chart(ctx, {
  			type: chartType,
  			data: {
  				labels: Label,
  				datasets: [
  					{
  						label: 'Levering',
  						stack: 'Electricity',
  						data: Delivered,
  						fill: false,
  						backgroundColor: settingBackEDC,
  						borderColor: settingLineEDC,
  						borderWidth: 2
  					},
  					{
  						label: 'Teruggave',
  						stack: 'Electricity',
  						data: Returned,
  						fill: false,
  						backgroundColor: settingBackERC,
  						borderColor: settingLineERC,
  						borderWidth: 2
  					}
  				]
  			},
  			options: {
  				responsive: true,
  				maintainAspectRatio: true,
  				scales: {
            yAxes: [{
                scaleLabel: {
                	display: true,
                	labelString: 'kWh'
                }
            }]
          }
        }
  	});

  } else if (graphType == "D") {			// Hours
  	myChart = new Chart(ctx, {
  			type: chartType,
  			data: {
  				labels: Label,
  				datasets: [
  					{
  						label: 'Levering',
  						stack: 'Electricity',
  						data: Delivered,
  						fill: false,
  						backgroundColor: settingBackEDC,
  						borderColor: settingLineEDC,
  						borderWidth: 2
  					},
  					{
  						label: 'Teruggave',
  						stack: 'Electricity',
  						data: Returned,
  						fill: false,
  						backgroundColor: settingBackERC,
  						borderColor: settingLineERC,
  						borderWidth: 2
  					}
  				]
  			},
  			options: {
  				responsive: true,
  				maintainAspectRatio: true,
  				scales: {
            yAxes: [{
                scaleLabel: {
                	display: true,
                	labelString: 'Watt/uur'
                }
            }]
          }
        }
  	});
  } else if (graphType == "F") {			// Financials
  	myChart = new Chart(ctx, {
  			type: chartType,
  			data: {
  				labels: Label,
  				datasets: [
  					{
  						label: 'Verbruik deze periode',
  						stack: 'ThisYear',
  						data: Delivered,
  						fill: false,
  						backgroundColor: settingBackEDC,
  						borderColor: settingLineEDC,
  						borderWidth: 2
  					},
  					{
  						label: 'Terug deze periode',
  						stack: 'ThisYear',
  						data: Returned,
  						fill: false,
  						backgroundColor: settingBackERC,
  						borderColor: settingLineERC,
  						borderWidth: 2
  					},
  					{
  						label: 'Gas deze periode',
  						stack: 'ThisYear',
  						data: Gas,
  						fill: false,
  						backgroundColor: settingBackGDC,
  						borderColor: settingLineGDC,
  						borderWidth: 2
  					},
  					{
  						label: 'Verbruik vorige periode',
  						stack: 'LastYear',
  						data: Delivered2,
  						fill: false,
  						backgroundColor: settingBackED2C,
  						borderColor: settingLineED2C,
  						borderWidth: 2
  					},
  					{
  						label: 'Terug vorige periode',
  						stack: 'LastYear',
  						data: Returned2,
  						fill: false,
  						backgroundColor: settingBackER2C,
  						borderColor: settingLineER2C,
  						borderWidth: 2
  					},
  					{
  						label: 'Gas vorige periode',
  						stack: 'LastYear',
  						data: Gas2,
  						fill: false,
  						backgroundColor: settingBackGD2C,
  						borderColor: settingLineGD2C,
  						borderWidth: 2
  					}
  				]
  			},
  			options: {        
  				scales: {
                yAxes: [{
                	scaleLabel: {
                		display: true,
                		labelString: "in Euro's",
                	}
                }]
          },
  				tooltips: {
  					mode: 'index',
  					callbacks: {
  						// Use the footer callback to display the sum of the items showing in the tooltip
  						footer: function(tooltipItems, data) {
  							var sumTy = 0;
  							var sumPy = 0;
  							tooltipItems.forEach(function(tooltipItem) {
  									//console.log(tooltipItem.datasetIndex);
  									if (tooltipItem.datasetIndex == 0 ||tooltipItem.datasetIndex == 1 ||tooltipItem.datasetIndex == 2) {
  										sumTy += parseFloat(data.datasets[tooltipItem.datasetIndex].data[tooltipItem.index]);
  									} else {
  										sumPy += parseFloat(data.datasets[tooltipItem.datasetIndex].data[tooltipItem.index]);
  									}
  							});
 							  return 'Dit Jaar € ' + parseFloat(sumTy).toFixed(2) + ', Vorig Jaar € ' + parseFloat(sumPy).toFixed(2);
							},
						},
						footerFontStyle: 'normal'
					}        
				}
  	});
  }
}		// chartInit()

function graphStart() {
	Label.length 				= 0;
	Delivered.length 		= 0;
	Returned.length 		= 0;
	Gas.length 					= 0;
	Delivered2.length		= 0;
	Returned2.length 		= 0;
	Gas2.length 				= 0;
	DeliveredL1.length	= 0;
	DeliveredL2.length	= 0;
	DeliveredL3.length	= 0;
	clearInterval(TimerActual);  

  chartInit();

	if (graphType == 'Y') {
		console.log("graphStart: graphYearRow,r=1");
		webSocketConn.send("graphYearRow,r=1");
    TimerActual=setInterval(function(){
    			webSocketConn.send("tabGraphics");
	    },120000);
	} else if (graphType == 'W') {
		console.log("graphStart: graphWeekRow,r=1");
		webSocketConn.send("graphWeekRow,r=1");
    TimerActual=setInterval(function(){
    			webSocketConn.send("tabGraphics");
	    },120000);
	} else if (graphType == 'D') {
		console.log("graphStart: graphDayRow,r=1");
		webSocketConn.send("graphDayRow,r=1");
    TimerActual=setInterval(function(){
    			webSocketConn.send("tabGraphics");
	    },120000);
	} else if (graphType == 'A') {
    console.log("graphStart: graphActualNext");
    webSocketConn.send("graphActualNext");
    TimerActual=setInterval(function(){
    			webSocketConn.send("graphActualNext");
      },9900);
	} else if (graphType == 'F') {
    console.log("graphStart: graphFinancialRow");
    //webSocketConn.send("graphFinancialRow,r=1");
    webSocketConn.send("graphFinancialRow,r=1");
    TimerActual=setInterval(function(){
    			webSocketConn.send("tabGraphics");
      },120000);
  }	
}

function graphRow(payload) {
	if (activeTab != "Graphics") {
  		clearInterval(TimerActual);  
  		return;
  }
  singlePair   = payload.split(",");
  var rowNr = singlePair[1].split("=");
  var uRow = 1;
  if (parseInt(rowNr[1]) == 99) {
  	uRow = 1;
  } else {
  	uRow  = parseInt(rowNr[1]); // - 1;
  }
  
  let doUnshift = true;
  
  for ( var i = 0; i < singlePair.length; i++ ) {
  	var Data = singlePair[i].split("=");
    addLogLine("graphRow["+i+"]"+Data[0]+"="+Data[1]);
    if (Data[0] == "A")	doUnshift = false;
    if (Data[0] == "M" || Data[0] == "D" || Data[0] == "DH" || Data[0] == "T") {
    //---> if (Data[0] == "M" || Data[0] == "D" || Data[0] == "H" || Data[0] == "T") {
    	if (doUnshift) 		Label.unshift(Data[1]);
    	else	      			Label.push(Data[1]);
    } else if (Data[0] == "ED1" || Data[0] == "ED" || Data[0] == "ED1C") {
    	if (doUnshift) 		Delivered.unshift(Data[1]);
    	else							Delivered.push(Data[1]);
    } else if (Data[0] == "ER1" || Data[0] == "ER" || Data[0] == "ER1C") {
    	if (doUnshift) 		Returned.unshift((Data[1] * -1));
    	else							Returned.push((Data[1] * -1));
    } else if (Data[0] == "ED2" || Data[0] == "ED2C" ) {
    	if (doUnshift) 		Delivered2.unshift(Data[1]);
    	else							Delivered2.push(Data[1]);
    } else if (Data[0] == "ER2" || Data[0] == "ER2C") {
    	if (doUnshift) 		Returned2.unshift((Data[1] * -1));
    	else							Returned2.push((Data[1] * -1));
    } else if (Data[0] == "EDL1") {
    			DeliveredL1.push(Data[1]);
    } else if (Data[0] == "EDL2") {
    			DeliveredL2.push(Data[1]);
    } else if (Data[0] == "EDL3") {
    			DeliveredL3.push(Data[1]);
    } else if (Data[0] == "GD1C") {
    	if (doUnshift) 		Gas.unshift(Data[1]);
    	else							Gas.push(Data[1]);
    } else if (Data[0] == "GD2C") {
    	if (doUnshift) 		Gas2.unshift(Data[1]);
    	else							Gas2.push(Data[1]);
    }
  }	// for ..
  
  if (Label.length > 100)	{
  	if (doUnshift) {
  		Label.length 			= 100;
  		Delivered.length 	= 100;
  		Returned.length 	= 100;
  		Gas.length				= 100;
  	} else {
  		Label.shift();
  		Delivered.shift();
  		Returned.shift();
  		Gas.shift();
  		DeliveredL1.shift();
  		DeliveredL2.shift();
  		DeliveredL3.shift();
  	}
  }
  
  myChart.update();
  
  if (parseInt(rowNr[1]) != 99) {
   	//uRow = parseInt(rowNr[1]) + 1;
   	//console.log("rowNr["+rowNr[1]+"] ===> uRow["+uRow+"]");
   	if (graphType == 'Y') {
    	uRow = parseInt(rowNr[1]) + 1;
   		console.log("graphYearRow?r="+ uRow);
   		webSocketConn.send("graphYearRow?r="+ uRow);
   	} else if (graphType == 'W') {
    	uRow = parseInt(rowNr[1]) + 1;
   		console.log("graphWeekRow?r="+ uRow);
   		webSocketConn.send("graphWeekRow?r="+ uRow);
   	} else if (graphType == 'D') {
    	uRow = parseInt(rowNr[1]) + 1;
   		console.log("graphDayRow?r="+ uRow);
   		webSocketConn.send("graphDayRow?r="+ uRow);
   	} else if (graphType == 'A') {
    	uRow = parseInt(rowNr[1]) + 1;
   		console.log("graphActualNext");
   	} else if (graphType == 'F') {
    	uRow = parseInt(rowNr[1]) + 1;
   		console.log("graphFinancialRow?r="+ uRow);
   		webSocketConn.send("graphFinancialRow?r="+ uRow);
   	}
  }
}	// graphRow()


function setGraphType(gPeriod) {
	graphType = gPeriod;
	console.log("setGraphType to ["+graphType+"]");
	graphStart();
}		// setGraphType()

function setChartType(gType) {
	if (gType == "B")	chartType = 'bar';
	else chartType = 'line';
	console.log("setChartType to ["+chartType+"]");
  chartInit();
}		// setChartType()

function currencyFormat(number, decimals, dec_point, thousands_sep) {
// *     example: number_format(1234.56, 2, ',', ' ');
// *     return: '1 234,56'
    number = (number + '').replace(',', '').replace(' ', '');
    var n = !isFinite(+number) ? 0 : +number,
            prec = !isFinite(+decimals) ? 0 : Math.abs(decimals),
            sep = (typeof thousands_sep === 'undefined') ? ',' : thousands_sep,
            dec = (typeof dec_point === 'undefined') ? '.' : dec_point,
            s = '',
            toFixedFix = function (n, prec) {
                var k = Math.pow(10, prec);
                return '' + Math.round(n * k) / k;
            };
    // Fix for IE parseFloat(0.55).toFixed(0) = 0;
    s = (prec ? toFixedFix(n, prec) : '' + Math.round(n)).split('.');
    if (s[0].length > 3) {
        s[0] = s[0].replace(/\B(?=(?:\d{3})+(?!\d))/g, sep);
    }
    if ((s[1] || '').length < prec) {
        s[1] = s[1] || '';
        s[1] += new Array(prec - s[1].length + 1).join('0');
    }
    return s.join(dec);
}	// currencyFormat()


function sumArrays(array1, array2, array3) {
  var result = [];
  var ctr = 0;
  var x=0;

 	while (ctr < array1.length && ctr < array2.length && ctr < array3.length) {
    result.push(  parseFloat(array1[ctr]) 
				 				+ parseFloat(array2[ctr]) 
				 				+ parseFloat(array3[ctr]));
    ctr++;
  }
  return result;
  
}		// sumArrays()

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
