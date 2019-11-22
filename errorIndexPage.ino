/*
***************************************************************************  
**  Program  : errorIndexPage, part of DSMRloggerWS
**  Version  : v1.0.4
**
**  Copyright (c) 2019 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

//===========================================================================================
void handleErrorIndexPage() {                    // HTML error index.html
//===========================================================================================

  String errIndexHTML = "";

  DebugTf("FreeHeap [%d]\r\n", ESP.getFreeHeap() );

  errIndexHTML = "<!DOCTYPE HTML><html lang='en'>\r\n"
   "<head>"
   "<meta charset='UTF-8'>"
   "<style type='text/css'>"
     "body {background-color: lightgray;}"
   "</style>"
   "</head>\r\n"
   "<body>\n<h1>" + String(_HOSTNAME) + "</h1>"
 
   "<h2>This page should not look like this</h2>"
   "<br>missing DSMRlogger.html page<br>"
   "<hr><h3>Have you uploaded the 'data' dir to the ESP8266? </h3>"
  
   "<hr>"
   "<div style='width: 30%'>"
     "<form style='float: right;' action='/FSexplorer' method='POST'>"
       "<input type='submit' class='button' name='SUBMIT' value='Go to FSexplorer'>"
       "</form>"
   "</div>"
   "<div style='width: 40%'>&nbsp;</div>"
   "</body></html>\r\n";

  httpServer.send(200, "text/html", errIndexHTML);
  
} // handleErrorIndexPage()


/***************************************************************************
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
***************************************************************************/
