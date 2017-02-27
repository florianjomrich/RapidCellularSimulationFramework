import commands
import os
import subprocess
randomtrip_path="/usr/share/sumo/tools/trip/randomTrips.py"
map_path=""
trace_path=""
netconvert_path=""
duarouter_path=""
sumo_path=""

def convert_map():
 subprocess.call(["netconvert","--osm-files","map.osm","-o","map.net.xml"])
 print "Mapped sucessfully converted to SUMO format"
def get_custom_traffic():
 try:
  subprocess.call(["duarouter","--trip-files","custom_trips.trips.xml","--net-file","map.net.xml","--output-file","map.rou.xml"])
 except:
  print "Please enter valid starting and ending lane"
 print "Custom Traffic successfully generated "
def get_random_traffic(number_veh):
 subprocess.call(["python",randomtrip_path,"-n","map.net.xml","-r","map.rou.xml","-e",number_veh,"-l"])
 print "Random Traffic successfully generated "
def get_geo_trace():
 try:
   subprocess.call(["sumo","-c","map.sumo.cfg","--fcd-output","sumoTrace.xml"])
   print "SUMO Geo successfully generated "
 except:
   print("Error in generating trace")
def open_sumo_app():
 subprocess.call(["sumo-gui","map.sumo.cfg"])
 
#subprocess.call(["netconvert","--osm-files","map.osm","-o","map.net.xml"])
#subprocess.call(["duarouter","--trip-files","custom_trips.trips.xml","--net-file","map.net.xml","--output-file","map.rou.xml"])
#for custoom traffic 
#subprocess.call(["python",randomtrip_path,"-n","map.net.xml","-r","map.rou.xml","-e",random_number_veh,"-l"])
#for random traffic 
#subprocess.call(["sumo","-c","map.sumo.cfg","--fcd-output","sumoTrace.xml"])

