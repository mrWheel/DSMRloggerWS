#!/usr/bin/env python
#
# This program is tailered to the Windows envirionment
# It will upload an arduino compiled sketch
# to a WiFi connected device
#
#------ where pyton is located -----------------------------
PYTHON='C:\python27\python.exe'
#
#------ this is the Sketch Location (see preferences.txt) --
BUILDPATH = "C:\Documents and Settings\(YourLoginName)\Local Settings\Temp\Build"
#
#------ Edit this ESPOTAPY to point to the location --------
#------ where your espota.py file is located        --------
ESPOTAPY = "\"C:/Documents and Settings/(YourLoginName)\Local Settings/Application Data/Arduino15/packages/esp8266/hardware/esp8266/2.5.0/tools/espota.py\""
#
#------ do not change anything below this line! ------------
#
import os  
import sys
import glob
import stat
import time
#
##print sys.argv
MODNAME   = os.path.basename(sys.argv[0])
#
#------ yes/no question ------------------------------------
def yes_or_no(question):
        reply = str(raw_input(MODNAME+' : '+question+' (y/N)? ')).lower().strip()
        if reply[:1] == 'y':
            return True
        return False


#------ check number of arguments on command line ----------
arguments = sys.argv[1:]
count = len(arguments)
if (count <> 1):
   print "Error: use", MODNAME, "<IPaddress device>"
   sys.exit(1)
#
#------ first argument must be IP address of the device ----
IPADDRESS = sys.argv[1]
#
# where are we?
CWD = os.getcwd()
##print MODNAME,": Started in [", CWD, "]"
#
#------ does build-path exists -----------------------------
if ( not os.path.exists(BUILDPATH)):
   print MODNAME,": Build path does not exist"
   sys.exit(2)
#
#------ change directory to BUILDPATH ----------------------
os.chdir(BUILDPATH)
##print MODNAME,": Changed to [", os.getcwd(), "]"
#
#------ now check if there is only 1 bin file --------------
noBins = 0
for binFile in glob.glob('*.*.bin'):
    noBins += 1
    print MODNAME,": [", noBins, "] [", binFile, "]"
#for binFile in glob.glob('*.*.bin'):
#    noBins += 1
#    print MODNAME,": [", noBins, "] [", binFile, "]"
#
if (noBins > 1):
    print MODNAME,": There are more than one (1) *.*.bin files!"
    print MODNAME,": Abort"
    sys.exit(3)
#
if (noBins < 1):
    print MODNAME,": There are no *.*.bin files!"
    print MODNAME,": Abort"
    sys.exit(3)
#
#------ ok, all went well .. -------------------------------
print MODNAME,": bin File Found ", binFile
#
fileStatsObj = os.stat ( binFile )
modificationTime = time.ctime ( fileStatsObj [ stat.ST_MTIME ] )
print MODNAME,": Last Modified  ", modificationTime
#
if (not yes_or_no('Continue')):
    sys.exit(4)
#
#------ build the espota command line and execute ----------
ESPOTA = PYTHON + ' ' + ESPOTAPY + ' -i ' + IPADDRESS + ' -p 8266 --auth= -f ' + binFile
##print MODNAME,': [', ESPOTA, ']'
#
print '----------------------------------------------------------------------------'
print MODNAME,': espota -i ', IPADDRESS,' -f ', binFile
os.system(ESPOTA)
#
# go back to CWD
os.chdir(CWD)
##print MODNAME,": .. and we'r back at [", os.getcwd(), "]"
print "Done .."
