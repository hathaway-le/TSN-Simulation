#coding=utf-8
#!/usr/bin/python
import re
import sys
import matplotlib.pyplot as plt
from numpy import mean, std
from decimal import *
import numpy as np

def gen_plot(file):

	result = {}
	RANGE1 = [0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]
	RANGE2 = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
	y_factor=1e6#转换为us

	for x in RANGE2:
		result[x] = {'mean':[], 'min':[], 'max':[], 'stddev':[]}

	with open(file, 'r') as f:
		for line in f:
			lineArr = line.split(",")#默认逗号分隔	
			if len(lineArr) == 5 and lineArr[0] != "File":#避免第一行干扰

				#m = re.search("-\w+\-", lineArr[0])
				n = re.search("-\w+.sca", lineArr[0])
				#print "%s" %(lineArr[0][m.start()+1:m.end()-1])
				print "%s" %(lineArr[0][n.start()+1:n.end()-4])
				#down = int(lineArr[0][m.start()+1:m.end()-1])
				up = int(lineArr[0][n.start()+1:n.end()-4])
				
				if lineArr[3] == "time_deviation_histogram:mean":
					result[up]['mean'].append(float(lineArr[4][:-2]))
				if lineArr[3] == "time_deviation_histogram:min":
					result[up]['min'].append(float(lineArr[4][:-2]))
				if lineArr[3] == "time_deviation_histogram:max":
					#import pdb  
					#pdb.set_trace()   
					result[up]['max'].append(float(lineArr[4][:-2]))
	
	
	plt.scatter(RANGE1, [max(result[i]['max'])*y_factor for i in RANGE2],c="r",marker="o")
#	plt.scatter(RANGE1, [mean(result[i]['mean'])*y_factor for i in RANGE2],c="b",marker="o")	
if __name__ == "__main__":

	if len(sys.argv) < 2:
	    sys.stderr.write('Usage: gen_plot.py <*.csv>\n')
	    sys.exit(1)
#	import pdb  
#	pdb.set_trace()    
	gen_plot(file=sys.argv[1])
	
	plt.xlim(0, 1.0001)
	my_x_ticks = np.arange(0, 1.0001, 0.1)
	plt.xticks(my_x_ticks)
#	plt.ylim(0.000015 * y_factor, 0.00006 * y_factor)
	plt.xlabel("lineload")
	plt.ylabel("Time deviation (us)")
	plt.title("maximum value")
#	plt.title("average value")	
	plt.legend(loc=9)#'upper center'
	
	fig = plt.gcf()
	fig.savefig('max.png', dpi=100)
#	fig.savefig('mean.png', dpi=100)
	
	plt.show()
