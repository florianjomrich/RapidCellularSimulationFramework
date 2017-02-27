import urllib
import shell_command
import PositionSolverClass
import WGS84Coordinate

import subprocess
import folium
import os

import csv

from geopy.geocoders import Nominatim

###################################

sumoMap = 'map.net.xml'
celltowerPositions = 'celltowerPositions.csv'

startPoints = []
endPoints = []
noOfVehicles = []
oneTrip = []

###################################

#get name from geopy:
scenario_file= open('Scenario.txt', 'r+')
scenario_txt = scenario_file.read();
name_of_area=scenario_txt.split(';')[0].split('=')[1]
print "Name of area: "+ name_of_area
geolocator = Nominatim()
mylocation = geolocator.geocode(name_of_area)

pathToMapFile = scenario_txt.split(';')[1].split('=')[1]
wanted_Providers = scenario_txt.split(';')[2].split('=')[1].split(',')
print wanted_Providers;

#extract wanted providers out of base file:
print "Extracting Celltower Positions right now"

with open('celltowerPositions.csv','rb') as all_celltower_postion_file, open('selectedCellTowersOfProvider.csv','w') as current_celltower_position_file:
	
	cellTowerReader = csv.DictReader(all_celltower_postion_file,delimiter=',',quotechar='"')	
	headers = cellTowerReader.fieldnames
	cellTowerWriter = csv.DictWriter(current_celltower_position_file, fieldnames=headers,delimiter=',',quotechar='"')
	cellTowerWriter.writeheader()
    	for row in cellTowerReader:
		 try:
			 if row['LICENSEE'] in wanted_Providers and ((700 <= float(row['TRANSMIT_LOWER']) <= 800) or (1700 <= float(row['TRANSMIT_LOWER']) <= 4000)) and (abs(float(row['LONGITUDE']) - mylocation.longitude)<2) and (abs(float(row['LATITUDE']) - mylocation.latitude)<2):#LICENSEE and ruff frequency band estimation for lte and a bounding box arround the location
				cellTowerWriter.writerow(row) 
		 except: 
			 print 'Cannot convert these given values'
			 print row['TRANSMIT_LOWER']


#convert selected providers to geojson for visualisation
current_celltower_as_geojson = open("geoPositionsOfCelltowersOfScenario.geojson", "w")
geojson_line_counter=0;
csv_to_geojson_file =  'selectedCellTowersOfProvider.csv'

current_celltower_as_geojson.write("{\n\"type\": \"FeatureCollection\",\n\n\"features\": [\n")


with open(csv_to_geojson_file) as csvfile:
	reader = csv.DictReader(csvfile,delimiter=',',quotechar='"')
	first_line = 1;                
	for row in reader:
			try:
		 		if(first_line):
					first_line=0;
					geojson_line_counter=geojson_line_counter+1;
	  				current_celltower_as_geojson.write("{ \"type\": \"Feature\", \"id\": "+str(geojson_line_counter)+", \"geometry\": { \"type\": \"Point\", \"coordinates\": [ "+str(row['LONGITUDE'])+","+ str(row['LATITUDE'])+" ] } }\n")
				else:
					geojson_line_counter=geojson_line_counter+1;
	  				current_celltower_as_geojson.write(",\n{ \"type\": \"Feature\", \"id\": "+str(geojson_line_counter)+", \"geometry\": { \"type\": \"Point\", \"coordinates\": [ "+str(row['LONGITUDE'])+","+ str(row['LATITUDE'])+" ] } }\n")		
			except: 
				print 'No Latitude or Longitude given by point'
			

current_celltower_as_geojson.write("]\n}")
current_celltower_as_geojson.close()

#setup map using folium
#map_osm = folium.Map(location=[mylocation.latitude,mylocation.longitude])
#map_osm.click_for_marker(popup='Waypoint')

#add markers:
#folium.Marker([45.3288, -121.6625], popup='Mt. Hood Meadows').add_to(map_osm)

#map_osm.create_map(path='osm_map.html')

#now modify the leaflet html file regarding our new position
osm_map_file= open('osm_map.html', 'r+')
map_content = osm_map_file.read();

beginning_content = map_content.split('center:[')[0];
end_content = map_content.split('center:[')[1].split('zoom: 10,')[1];
new_content = beginning_content+"center:["+str(mylocation.latitude)+","+str(mylocation.longitude)+"],\n\t\t\t\t\tzoom: 10,"+end_content ;


#overwrite the file:
osm_map_file.seek(0)
osm_map_file.write(str(new_content));

osm_map_file.truncate()

#get the map id
#map_id = split_content[1].split('.addTo(')
#map_id = map_id[1].split(');')
#map_id = map_id[0]
#print map_id

#overwrite the file:
#osm_map_file.seek(0)
#osm_map_file.write(str(untouched_content));


#osm_map_file.write('\tvar myString = \'\';\n');
#osm_map_file.write('\tvar filString =\'scenario_trips.rec\';\n');
#osm_map_file.write('\n');
#osm_map_file.write('\t\tfunction download(filename, text) {');
#osm_map_file.write('\t\tvar element = document.createElement(\'a\');\n');
#osm_map_file.write('\t\telement.setAttribute(\'href\', \'data:text/plain;charset=utf-8,\' + encodeURIComponent(myString));\n');
#osm_map_file.write('\t\telement.setAttribute(\'download\', filString);\n');
#osm_map_file.write('\n');
#osm_map_file.write('\t\telement.style.display = \'none\';\n');
#osm_map_file.write('\t\tdocument.body.appendChild(element);\n');
#osm_map_file.write('\n');
#osm_map_file.write('\t\telement.click();\n');
#osm_map_file.write('\n');
#osm_map_file.write('\t\tdocument.body.removeChild(element);');
#osm_map_file.write('\t\t};\n');
#osm_map_file.write('\n');            
#osm_map_file.write('\tfunction newMarker(e){\n');
#osm_map_file.write('\t\tvar new_mark = L.marker().setLatLng(e.latlng).addTo('+str(map_id)+');\n');
#osm_map_file.write('\t\tnew_mark.on(\'dblclick\', function(e){\n');
#osm_map_file.write('\t\t'+str(map_id)+'.removeLayer(e.target)\n');
#osm_map_file.write('\t\tvar lat = e.latlng.lat.toFixed(6),\n');
#osm_map_file.write('\t\tlng = e.latlng.lng.toFixed(6);\n');
#osm_map_file.write('\t\tmyString += \'remove;\'+lat+\';\'+lng+\'\\n\';})\n');
#osm_map_file.write('\t\tvar lat = e.latlng.lat.toFixed(6),\n');
#osm_map_file.write('\t\tlng = e.latlng.lng.toFixed(6);\n');
#osm_map_file.write('\t\tmyString += \'add;\'+lat+\';\'+lng+\'\\n\';\n');
#osm_map_file.write('\t\tnew_mark.bindPopup("Waypoint");\n');
#osm_map_file.write('\t\t};\n');
#osm_map_file.write('\t'+str(map_id)+'.on(\'click\', newMarker);\n');
#osm_map_file.write('\t'+str(map_id)+'.on(\'contextmenu\', download);\n');
#osm_map_file.write('</script>')            
        
        
        

#osm_map_file.truncate()



#open the webbrowser showing the osm map
p = subprocess.Popen(["firefox", "osm_map.html"])
p.wait(); # wait until the browser is closed - the user is finished then
#os.system("firefox osm_map.html")

#readParameters from the scenarioFile

scenarioFile = open('scenario_trips.rec', 'r');

startPoint =1;

for line in scenarioFile:
	oneTrip = line.split(';');
	#print oneTrip[0]

	if(startPoint):
			startPoint = WGS84Coordinate.WGS84(oneTrip[1],oneTrip[2]);
			startPoints.append(startPoint)
			startPoint=0;
	else:
			endPoint = WGS84Coordinate.WGS84(oneTrip[1],oneTrip[2]);
			endPoints.append(endPoint)
			noOfVehiclesForThisTrip = oneTrip[3]
			noOfVehicles.append(noOfVehiclesForThisTrip)
			startPoint=1;
			
		

#after all the points exist in array
allLatitudes =[];
allLongitudes =[];
for point in startPoints:
	allLatitudes.append(float(point.getLatitude()));
	allLongitudes.append(float(point.getLongitude()));
for point in endPoints:
	allLatitudes.append(float(point.getLatitude()));
	allLongitudes.append(float(point.getLongitude()));

maxLatitude = (float(max(allLatitudes))+0.05);
maxLongitude = (float(max(allLongitudes))+0.05);

minLatitude = (float(min(allLatitudes))-0.05);
minLongitude = (float(min(allLongitudes))-0.05);

#maxLatitude = max(allLatitudes);
#maxLongitude = max(allLongitudes);

#minLatitude = min(allLatitudes);
#minLongitude = min(allLongitudes);
		
#Download OSM Map of Scenario file:
#testfile = urllib.URLopener()
#testfile.retrieve("http://overpass-api.de/api/map?bbox=-97.19729,49.85375,-97.18111,49.86001", "map.osm")
#print "Downloading map now."
#print "http://overpass-api.de/api/map?bbox="+str(maxLongitude)+","+str(minLatitude)+","+str(minLongitude)+","+str(maxLatitude)
#testfile.retrieve("http://overpass-api.de/api/map?bbox="+str(maxLongitude)+","+str(minLatitude)+","+str(minLongitude)+","+str(maxLatitude), "map.osm")


#convert osm file according to the bounding box via osmosis out of whole canada file
#print "./osmosis/osmosis-latest/bin/osmosis --read-pbf file=\"/home/ubuntu/canada-latest.osm.pbf\" --bounding-box top="+str(maxLatitude)+" left="+str(minLongitude)+" bottom="+str(minLatitude)+" right="+str(maxLongitude)+" --write-xml file=\"map.osm\"";
#os.system("./osmosis/osmosis-latest/bin/osmosis --read-pbf file=\"canada-latest.osm.pbf\" --bounding-box top="+str(maxLatitude)+" left="+str(minLongitude)+" bottom="+str(minLatitude)+" right="+str(maxLongitude)+" --write-xml file=\"map.osm\"")

#convert osm file according to the bounding box via osmconvert out of whole canada file
print "./osmconvert64 "+pathToMapFile+" b="+str(minLongitude)+","+str(minLatitude)+","+str(maxLongitude)+","+str(maxLatitude)+" -o=map.osm \n"
print "Extracting scenario area now"
#os.system("./osmconvert64 /home/ubuntu/canada-latest.osm.pbf -b="+str(minLongitude)+","+str(minLatitude)+","+str(maxLongitude)+","+str(maxLatitude)+" -o=map.osm")
os.system("./osmconvert64 "+pathToMapFile+" -b="+str(minLongitude)+","+str(minLatitude)+","+str(maxLongitude)+","+str(maxLatitude)+" -o=map.osm")

#Convert .osm to .net.xml for sumo
shell_command.convert_map() #convert map

#Get Edges for Sumo out of gps position
solver = PositionSolverClass.PositionSolver()
edgeCounter =0;
carCounter = 0;
tripFile = open('custom_trips.trips.xml', 'w');
tripFile.write("<?xml version=\"1.0\"?>\n<trips>\n");
	
for i in startPoints:
	startEdge, endEdge = solver.getLaneIDFromGPSForStartAndEnd(sumoMap,startPoints[edgeCounter].getLatitude(),startPoints[edgeCounter].getLongitude(),endPoints[edgeCounter].getLatitude(), endPoints[edgeCounter].getLongitude())
	#print startPoints[counter].getLatitude(),startPoints[counter].getLongitude(),endPoints[counter].getLatitude(), endPoints[counter].getLongitude()
	print startEdge, endEdge
	startEdge = str(startEdge).split("\"")[1]
	endEdge = str(endEdge).split("\"")[1]
	
	for trip_ID in range(int(noOfVehicles[edgeCounter])):
		tripFile.write("<trip id=\""+str(carCounter)+"\" depart=\"0.00\" from=\""+str(startEdge)+"\" to=\""+str(endEdge)+"\"/>\n");
		carCounter = carCounter+1;

	edgeCounter=edgeCounter+1;

tripFile.write("</trips>")
tripFile.close()
	
#generate traffic out of trip file
shell_command.get_custom_traffic() 

#simulate with sumo and generate geo sumoTrace.xml file
shell_command.get_geo_trace()

#extract only the cell towers along sid the route:
os.system('python nearestcell.py')

#generate tcl file from geo sumoTrace.xml file
os.system('./modifiedtraceexporter.py --fcd-input sumoTrace.xml --ns2mobility-output mobsmple.tcl')

#generate final cellTower.csv to be used by ns3:
os.system('python modifyCellTowersForNS3.py')

#all finished
print 'All finished!'

