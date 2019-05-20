## Integratie DSMR-logger met Domoticz
`Michel Groen` heeft een script voor 
<a href="http://www.domoticz.com/" target="_blank">Domoticz</a>
geschreven waarmee data uit de DSMR-logger in Domoticz kan worden weergegeven.

![](img/Logger-Domoticz1.png)

Je kunt het script vinden in de `DSMRloggerWS` repository op github, in de
sub-map <a href="https://github.com/mrWheel/DSMRloggerWS/tree/master/Domoticz" target="_blank">
`Domoticz`
</a>.

```
-------------------------------------------------------------------------------------------
-- script_time_esp-dsmr-logger.lua
-------------------------------------------------------------------------------------------
-- Dit script leest de JSON waardes van DSMR Slimme Meter Logger en vult Dummy 
-- switchen in Domoticz. 
-- Het script werkt met firmware DSMRlogger2HTTP & DSMRloggerWS en is getest met 
-- hardware V3. 

-- Bij script parameters - vul je de gegevens in van dsmr-logger hardware. 
-- Bij Domoticz IDX Instellingen - dien je voor elke optie die je wilt gebruiken 
-- eerst een nieuwe dummy switch aan te maken in Domoticz en verwijder je de 
-- twee '--' ervoor.
-- Bij Dummy switchen bijwerken - Verwijder de twee '--' om een dummy te laten bijwerken.

-------------------------------------------------------------------------------------------
-- Script parameters
-------------------------------------------------------------------------------------------
local DSMR_IP = "DSMR-WS.local"      -- Ip-Adres ESP-DSMR Slimme Meter Logger of gebruik: 
                                     -- DSMRlogger2HTTP : ESP01-DSMR.local of 
                                     -- DSMRloggerWS    : DSMR-WS.local
Firmware      = "DSMRlogger2HTTP"    -- Vul in welke firmware je gebruikt. 'DSMRlogger2HTTP'  
                                     -- of 'DSMRloggerWS'
Debug         = "Ja"                 -- Zet Debug op Ja om in Domoticz Log alle JSON waardes te 
                                     -- registreren. Keuze  "Ja" of "Nee" (hoofdlettergevoelig)
                                     -- Zet Debug op Nee voor een eenvoudig melding in de log.
-------------------------------------------------------------------------------------------

commandArray={}

-------------------------------------------------------------------------------------------
-- Domoticz IDX instellingen voor leveren stroom & Gas
-- Je kunt zelf bepalen welke dummy switchen je wilt bijwerken in Domoticz door de '--' 
-- ervoor te verwijderen, de betreffende dummy switchen aan te maken in Domoticz (achter 
-- elk item staat beschreven welke optie je moet kiezen bij het aanmaken van de dummy) en 
-- het IDX nummer in te vullen.
-------------------------------------------------------------------------------------------
--Timestamp_IDX =                   -- Dummy = Text
--Energy_Delivered_IDX =            -- Dummy = Counter Incremental --> Engergy
--Gas_Delivered_IDX =               -- Dummy = Counter Incremental --> Gas
--Energy_Delivered_Tariff1_IDX =    -- Dummy = Counter Incremental --> Engergy
--Energy_Delivered_Tariff2_IDX =    -- Dummy = Counter Incremental --> Engergy
--Power_Delivered_IDX =             -- Dummy - Electric
--Voltage_l1_IDX =                  -- Dummy - Voltage
--Current_l1_IDX =                  -- Dummy - Amperage
--Voltage_l2_IDX =                  -- Dummy - Voltage
--Current_l2_IDX =                  -- Dummy - Amperage
--Voltage_l3_IDX =                  -- Dummy - Voltage
--Current_l3_IDX =                  -- Dummy - Amperage
--Power_Delivered_l1_IDX =          -- Dummy = Electric
--Power_Delivered_l2_IDX =          -- Dummy = Electric
--Power_Delivered_l3_IDX =          -- Dummy = Electric

-------------------------------------------------------------------------------------------
-- Instellingen voor terugleveren van stroom. Als je niet teruglevert kun je dit uit laten.
-- Je kunt zelf bepalen welke dummy switchen je wilt bijwerken in Domoticz door de '--' ervoor te verwijderen, 
-- de betreffende dummy switchen aan te maken in Domoticz (achter elk item staat beschreven welke optie je moet kiezen bij het aanmaken van de dummy) en het IDX nummer in te vullen.
-------------------------------------------------------------------------------------------
--Energy_Returned_IDX =             -- Dummy = Counter Incremental --> Engergy 
--Energy_Returned_Tariff1_IDX =     -- Dummy = Counter Incremental --> Engergy 
--Energy_Returned_Tariff2_IDX =     -- Dummy = Counter Incremental --> Engergy
--Power_Returned_IDX =              -- Dummy - Electric 
--Power_Returned_l1_IDX =           -- Dummy = Electric
--Power_Returned_l2_IDX =           -- Dummy = Electric 
--Power_Returned_l3_IDX =           -- Dummy = Electric
-------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------
-- Instellingen voor Firmware DSMRlogger2HTTP - Deze instellingen niet aanpassen !!
-------------------------------------------------------------------------------------------

if Firmware=="DSMRlogger2HTTP" then
  json = (loadfile "/home/pi/domoticz/scripts/lua/JSON.lua")()

    local jsondata    = assert(io.popen('curl http://'..DSMR_IP..'/getActual.json'))
    local jsondevices = jsondata:read('*all')
  jsondata:close()
    local jsonCPM = json:decode(jsondevices)

        Timestamp = jsonCPM.Timestamp
        Energy_Delivered = jsonCPM.Energy_Delivered
        Gas_Delivered = jsonCPM.Gas_Delivered
        Energy_Delivered_Tariff1 = jsonCPM.Energy_Delivered_Tariff1
        Energy_Delivered_Tariff2 = jsonCPM.Energy_Delivered_Tariff2
        Power_Delivered =  jsonCPM.Power_Delivered
        Voltage_l1 = jsonCPM.Voltage_l1
        Current_l1 = jsonCPM.Current_l1
        Voltage_l2 = jsonCPM.Voltage_l2
        Current_l2 = jsonCPM.Current_l2
        Voltage_l3 = jsonCPM.Voltage_l3
        Current_l3 = jsonCPM.Current_l3
        Power_Delivered_l1 = jsonCPM.Power_Delivered_l1
        Power_Delivered_l2 = jsonCPM.Power_Delivered_l2
        Power_Delivered_l3 = jsonCPM.Power_Delivered_l3

        Energy_Returned = jsonCPM.Energy_Returned
        Energy_Returned_Tariff1 = jsonCPM.Energy_Returned_Tariff1
        Energy_Returned_Tariff2 = jsonCPM.Energy_Returned_Tariff2
        Power_Returned = jsonCPM.Power_Returned
        Power_Returned_l1 = jsonCPM.Power_Returned_l1
        Power_Returned_l2 = jsonCPM.Power_Returned_l2
        Power_Returned_l3 = jsonCPM.Power_Returned_l3
-------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------
-- Instellingen voor Firmware DSMRloggerWS - Deze instellingen niet aanpassen !!
-------------------------------------------------------------------------------------------

elseif Firmware=="DSMRloggerWS" then
  json = (loadfile "/home/pi/domoticz/scripts/lua/JSON.lua")()

    local jsondata    = assert(io.popen('curl http://'..DSMR_IP..'/restAPI?get=Actueel'))
    local jsondevices = jsondata:read('*all')
  jsondata:close()
    local jsonCPM = json:decode(jsondevices)

        Timestamp = jsonCPM.Timestamp
        Energy_Delivered = jsonCPM.Energy_Delivered
        Gas_Delivered = jsonCPM.Gas_Delivered
        Energy_Delivered_Tariff1 = jsonCPM.Energy_Delivered_Tariff1
        Energy_Delivered_Tariff2 = jsonCPM.Energy_Delivered_Tariff2
        Power_Delivered =  jsonCPM.Power_Delivered
        Voltage_l1 = jsonCPM.Voltage_l1
        Current_l1 = jsonCPM.Current_l1
        Voltage_l2 = jsonCPM.Voltage_l2
        Current_l2 = jsonCPM.Current_l2
        Voltage_l3 = jsonCPM.Voltage_l3
        Current_l3 = jsonCPM.Current_l3
        Power_Delivered_l1 = jsonCPM.Power_Delivered_l1
        Power_Delivered_l2 = jsonCPM.Power_Delivered_l2
        Power_Delivered_l3 = jsonCPM.Power_Delivered_l3

        Energy_Returned = jsonCPM.Energy_Returned
        Energy_Returned_Tariff1 = jsonCPM.Energy_Returned_Tariff1
        Energy_Returned_Tariff2 = jsonCPM.Energy_Returned_Tariff2
        Power_Returned = jsonCPM.Power_Returned
        Power_Returned_l1 = jsonCPM.Power_Returned_l1
        Power_Returned_l2 = jsonCPM.Power_Returned_l2
        Power_Returned_l3 = jsonCPM.Power_Returned_l3
-------------------------------------------------------------------------------------------
end


-------------------------------------------------------------------------------------------
-- Uitgebreide Log instellingen voor leveren stroom & Gas
-------------------------------------------------------------------------------------------
if Debug=="Ja" then

  print ('ESP-DSMR Slimme Meter Logger voor leveren stroom & Gas')
  print ('Timestamp: ' ..Timestamp.. '')
  print ('Energie Verbruik (kWh): ' ..Energy_Delivered.. '')
  print ('Gas Verbruik (m3)): ' ..Gas_Delivered.. '')
  print ('Engergie Verbruik T1 (kWh)): ' ..Energy_Delivered_Tariff1.. '')
  print ('Engergie Verbruik T2 (kWh)): ' ..Energy_Delivered_Tariff2.. '')
  print ('Totaal Verbruik (kW)): ' ..Power_Delivered.. '')
  print ('Voltage L1 (Volt)): ' ..Voltage_l1.. '')
  print ('Current L1 (Amp)): ' ..Current_l1.. '')
  print ('Voltage L2 (Volt)): ' ..Voltage_l2.. '')
  print ('Current L2 (Amp)): ' ..Current_l2.. '')
  print ('Voltage L3 (Volt)): ' ..Voltage_l3.. '')
  print ('Current L3 (Amp)): ' ..Current_l3.. '')
  print ('Verbruik L2 (Watt))): ' ..Power_Delivered_l2.. '')
  print ('Verbruik L3 (Watt))): ' ..Power_Delivered_l3.. '')
  print ('Verbruik L1 (Watt))): ' ..Power_Delivered_l1.. '')

-------------------------------------------------------------------------------------------
-- Log instellingen voor terugleveren van stroom. Deze kun je aanzetten als je ook 
-- stroom teruglevert. Je kunt zelf bepalen welke dummy switchen je wilt bijwerken 
-- in Domoticz door de '--' ervoor te verwijderen.
-------------------------------------------------------------------------------------------
  --print ('ESP-DSMR Slimme Meter Logger voor terugleveren stroom')
  --print ('Terug Levering (kWh): ' ..Energy_Returned.. '')
  --print ('Terug Levering T1 (kWh)): ' ..Energy_Returned_Tariff1.. '')
  --print ('Terug Levering T2 (kWh)): ' ..Energy_Returned_Tariff2.. '')
  --print ('Totaal Terug Levering (kW)): ' ..Power_Returned.. '')
  --print ('Terug Levering L1 (Watt)): ' ..Power_Returned_l1.. '')
  --print ('Terug Levering L2 (Watt)): ' ..Power_Returned_l2.. '')
  --print ('Terug Levering L3 (Watt)): ' ..Power_Returned_l3.. '')
end
-------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------
-- Eenvoudige logging in Domoticz zodat je weet dat het script wordt uitgevoerd.
-------------------------------------------------------------------------------------------
if Debug=="Nee" then

  print ('ESP-DSMR Slimme Meter Logger')
  print ('JSON Informatie opgehaald en dummy switchen bijgewerkt')
end
-------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------
-- Dummy switchen bijwerken
-- Welke dummy switchen moeten worden bijgewerkt in Domoticz voor leveren 
-- stroom & Gas (Alleen voor stroomafname). Je kunt zelf bepalen welke dummy switchen 
-- je wilt bijwerken in Domoticz door de '--' ervoor te verwijderen.
-------------------------------------------------------------------------------------------
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Timestamp_IDX..'|0|'..Timestamp..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Energy_Delivered_IDX..'|0|'..Energy_Delivered..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Gas_Delivered_IDX..'|0|'..Gas_Delivered..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Energy_Delivered_Tariff1_IDX..'|0|'..Energy_Delivered_Tariff1..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Energy_Delivered_Tariff2_IDX..'|0|'..Energy_Delivered_Tariff2..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Power_Delivered_IDX..'|0|'..Power_Delivered..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Voltage_l1_IDX..'|0|'..Voltage_l1..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Current_l1_IDX..'|0|'..Current_l1..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Voltage_l2_IDX..'|0|'..Voltage_l2..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Current_l2_IDX..'|0|'..Current_l2..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Voltage_l3_IDX..'|0|'..Voltage_l3..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Current_l3_IDX..'|0|'..Current_l3..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Power_Delivered_l1_IDX..'|0|'..Power_Delivered_l1..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Power_Delivered_l2_IDX..'|0|'..Power_Delivered_l2..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Power_Delivered_l3_IDX..'|0|'..Power_Delivered_l3..''}
-------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------
-- Welke dummy switchen moeten worden bijgewerkt in Domoticz voor terugleveren stroom (Als 
-- je ook stroom teruglevert). Je kunt zelf bepalen welke dummy switchen je wilt bijwerken 
-- in Domoticz door de '--' ervoor te verwijderen.
-------------------------------------------------------------------------------------------
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Energy_Returned_IDX..'|0|'..Energy_Returned..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Energy_Returned_Tariff1_IDX..'|0|'..Energy_Returned_Tariff1..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Energy_Returned_Tariff2_IDX..'|0|'..Energy_Returned_Tariff2..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Power_Returned_IDX..'|0|'..Power_Returned..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Power_Returned_l1_IDX..'|0|'..Power_Returned_l1..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Power_Returned_l2_IDX..'|0|'..Power_Returned_l2..''}
--commandArray[#commandArray+1] = {['UpdateDevice'] = ''..Power_Returned_l3_IDX..'|0|'..Power_Returned_l3..''}
-------------------------------------------------------------------------------------------

return commandArray

```



<br>
---
<center style="font-size: 70%;">[Domoticz]</center><br>

![](img/Logger-Domoticz2.png)


