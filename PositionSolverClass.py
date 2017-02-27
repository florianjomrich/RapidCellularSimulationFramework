import sumolib


class PositionSolver:
	# parse the net
	def getLaneIDFromGPSForStartAndEnd(self,mapName,startLatitude,startLongitude,endLatitude,endLongitude):
		print 'parsing the net'
		net = sumolib.net.readNet(mapName)

		#startLatitude = startLatitude.replace(",", ".")
		#startLongitude = startLongitude.replace(",", ".")
		#endLatitude = endLatitude.replace(",", ".")
		#endLongitude = endLongitude.replace(",", ".")

		#print startLatitude,startLongitude,endLatitude, endLongitude

		#startEdge = self.getEdgeFromNet(net,float(startLatitude),float(startLongitude))
		startEdge = self.getEdgeFromNet(net,startLatitude,startLongitude)
		#print 'startEdge',startEdge
		#endEdge = self.getEdgeFromNet(net,float(endLatitude),float(endLongitude))
		endEdge = self.getEdgeFromNet(net,endLatitude,endLongitude)
		#print 'endEdge',endEdge
		return startEdge, endEdge

	def getEdgeFromNet(self,net,latitude,longitude):
		radius = 200 #in Meter
		#print 'latitude,longitude',latitude,longitude
		x, y = net.convertLonLat2XY(longitude, latitude)
		#print 'x and y:',x,y
		edges = net.getNeighboringEdges(x, y, radius)
		# pick the closest edge
		if len(edges) > 0:
		  distancesAndEdges = sorted([(dist, edge) for edge, dist in edges])
		  dist, closestEdge = distancesAndEdges[0]
		  return closestEdge	
		return 'NO_EDGE'
		#x=6466.11
		#y=14352.05
		#lat,lon = net.convertXY2LonLat(x, y)
		#print lat,lon


