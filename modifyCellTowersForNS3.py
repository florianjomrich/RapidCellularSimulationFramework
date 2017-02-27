import csv
import math


bandwidth_mhz = [1.4,3,5,10,15,20]
bandwidth_prbs = [6,15,25,50,75,100]

fdl_dow_low = [1930,2110,2620,734,2570,2300,3400]
n_offs_down = [600,1950,2750,5730,37750,38650,41590]

fdl_up_low = [1850,1710,2500,704,2570,2300,3400]
n_offs_up = [18600,19950,20750,23730,37750,38650,41590]


def getPrbBandwidth(current_bandwidth_mhz):
	closest_bandwidth_mhz = min(bandwidth_mhz, key=lambda x:abs(x-current_bandwidth_mhz))
	current_index = bandwidth_mhz.index(closest_bandwidth_mhz)	
	return bandwidth_prbs[current_index]


def calculate_EARFCN(first_frequency,second_frequency):# source: http://analog.intgckts.com/lte-carrier-frequency-and-earfcn/

# at least Telus and Rogers have switched there values for Transmit and Recieve Frequency - therefore select the best fitting frequency first: means shortest total distance
#for the sum of download and upload frequencies to there bottom part
      	first_diff = abs( first_frequency - min(fdl_dow_low, key=lambda x:abs(x-first_frequency)) ) + abs( second_frequency - min(fdl_up_low, key=lambda x:abs(x-second_frequency)) )
	second_diff = abs( second_frequency - min(fdl_dow_low, key=lambda x:abs(x-second_frequency)) ) + abs(first_frequency - min(fdl_up_low, key=lambda x:abs(x-first_frequency)) )

	if( first_diff < second_diff):
		#print first_diff,"<",second_diff
		f_dl = first_frequency
		f_up = second_frequency

	else:
		#print first_diff,">",second_diff
	  	f_dl = second_frequency
	  	f_up = first_frequency	
	

	#now calculate the earfcn: 

	closest_fdl_dow_low = min(fdl_dow_low, key=lambda x:abs(x-f_dl))
	current_index_dl = fdl_dow_low.index(closest_fdl_dow_low)
	n_offset_down = n_offs_down[current_index_dl]
	n_dl = n_offset_down + (f_dl-closest_fdl_dow_low )/0.1

	closest_fdl_up_low = min(fdl_up_low, key=lambda x:abs(x-f_up))
	current_index_up = fdl_up_low.index(closest_fdl_up_low)
	n_offset_up = n_offs_up[current_index_up]
	n_up = n_offset_up + (f_up-closest_fdl_up_low )/0.1

	return n_dl,n_up


def calculate_Correct_Power(power_in_watt):
	power_in_dBm = 10 * math.log10(1000 * power_in_watt)
	return power_in_dBm
	
	


with open('cellTowersForRouteCorrectPosition.csv','rb') as fin, open ('cellTowersForNS3.csv','w') as fout:
	reader = csv.DictReader(fin,delimiter=',',quotechar='"')
	headers = reader.fieldnames
	writer = csv.DictWriter(fout, fieldnames=headers,delimiter=',',quotechar='"')
	writer.writeheader()
	
	
	for row in reader:
		#get the bandwidth_prbs_used:
		prb_bandwidth = getPrbBandwidth(float(row['TRANSMIT_BW']))
		row['TRANSMIT_BW']=str(prb_bandwidth);


		#get the earfcn used for frequency:
		earfcn_down,earfcn_up = calculate_EARFCN(float(row['TRANSMIT_FREQ']),float(row['RECEIVE_FREQ']))
		row['TRANSMIT_FREQ']=str(earfcn_down);
		row['RECEIVE_FREQ']=str(earfcn_up);

		
		#calculate the correct transmission power
		row['TX_PWR'] = str(calculate_Correct_Power(float(row['TX_PWR'])))
	
		writer.writerow(row)
	
	








