import xml.etree.cElementTree
import csv
import math
#import numpy as np
#import pandas as pd

e = xml.etree.cElementTree.parse('sumoTrace.xml').getroot() 
#data= pd.read_csv('selectedCellTowersOfProvider.csv', sep=',',quotechar='"')

nearestLatitudes = []
nearestLongitudes = []
earfcn_frequency=[]
LAT_LIST=[]
LON_LIST=[]
EARFCN_LIST=[]
comb_lat_long=[]
new_array=[]
lat_long_new_array=[]

with open('selectedCellTowersOfProvider.csv','rb') as fin:
	reader = csv.DictReader(fin,delimiter=',',quotechar='"')	
	
	
	#get the latitude and longitude of all cellTowers
	latitude = []
	longitude = []
	for row in reader:
			latitude.append(float(row ['LATITUDE']))
			longitude.append(float(row ['LONGITUDE']))
	
	print 'Extracting now cell towers along the drive routes of the vehicles'
	for neighbor1 in e.iter('vehicle'):
		x= float(neighbor1.get('x'))
		y= float(neighbor1.get('y'))
		currentShortestDistance = 100000000;
		currentNearestLatitude = 0;
		currentNearestLongitude = 0;
		#print x,y
		for index , currentLatitude in enumerate(latitude):
			currentDistance = (math.sqrt(math.pow((currentLatitude-y),2) + math.pow((longitude[index]-x),2)))
			#print currentDistance
			if(currentDistance<currentShortestDistance):
				 currentShortestDistance=currentDistance
				 #print currentShortestDistance
				 currentNearestLatitude = currentLatitude
				 currentNearestLongitude = longitude[index]
		#print currentNearestLongitude, currentNearestLatitude
		if currentNearestLatitude not in nearestLatitudes and currentNearestLongitude not in nearestLongitudes:
			nearestLatitudes.append(currentNearestLatitude)
			nearestLongitudes.append(currentNearestLongitude)
		
		#print x,y
#open reader again to write output file

#print nearestLatitudes
#print nearestLongitudes

with open('selectedCellTowersOfProvider.csv','rb') as anotherRun:
	anotherReader = csv.DictReader(anotherRun,delimiter=',',quotechar='"')
	for row in anotherReader:
		if float(row ['LATITUDE']) in nearestLatitudes and float(row ['LONGITUDE']) in nearestLongitudes:
			NEW_LATITUDE = float(row['LATITUDE'])
			NEW_LONGITUDE = float(row['LONGITUDE'])
			NEW_EARFCN = float(row['TRANSMIT_FREQ'])
			LAT_LIST.append(NEW_LATITUDE)
			LON_LIST.append(NEW_LONGITUDE)
			EARFCN_LIST.append(NEW_EARFCN)
	comb_lat_long = zip(LAT_LIST,LON_LIST,EARFCN_LIST)
	LENGTH = len(comb_lat_long)
	
	for i in xrange (0, LENGTH):
		if (comb_lat_long[i][0],comb_lat_long[i][1]) not in lat_long_new_array:
			lat_long_list=(comb_lat_long[i][0],comb_lat_long[i][1])
			lat_long_new_array.append(lat_long_list)
			combined_list = (comb_lat_long[i][0],comb_lat_long[i][1],comb_lat_long[i][2])
			new_array.append(combined_list)
			
			for j in xrange (0,LENGTH):
				if i != j:
					if comb_lat_long[i][0]==comb_lat_long[j][0] and comb_lat_long[i][1]==comb_lat_long[j][1] and comb_lat_long[i][2]==comb_lat_long[j][2]:
						new_lat_long_list=(comb_lat_long[j][0],comb_lat_long[j][1])
						lat_long_new_array.append(new_lat_long_list)
						new_combined_list=(comb_lat_long[j][0],comb_lat_long[j][1],comb_lat_long[j][2])
						new_array.append (new_combined_list)
						
	unique_list=list(set(new_array))
	list1,list2,list3 = zip( *unique_list)
	unique_length= len(unique_list)	
with open('selectedCellTowersOfProvider.csv','rb') as nextrun, open ('cellTowersForRoute.csv','w') as output:
	nextreader = csv.DictReader(nextrun,delimiter=',',quotechar='"')
	headers = nextreader.fieldnames
	new_writer = csv.DictWriter(output, fieldnames=headers,delimiter=',',quotechar='"')
	new_writer.writeheader()
	for row in nextreader:
		for m in xrange(0,unique_length):
			if float(row['LATITUDE']) == list1[m] and float(row['LONGITUDE'])== list2[m] and float(row['TRANSMIT_FREQ'])== list3[m]:
				new_writer.writerow(row)

	
	








