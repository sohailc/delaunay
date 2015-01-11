import numpy
import pylab
import ctypes
import time

class pointSet(ctypes.Structure):
    _fields_ = [("X", ctypes.POINTER(ctypes.c_double)), \
                ("Y", ctypes.POINTER(ctypes.c_double)), \
                ("numberOfPoints", ctypes.c_int)]

def findContainingTriangle(x,y):

	basisLen = 1.5*(max(x) - min(x))
	basisMid = 0.5*(max(x) + min(x))
	h = 3*(max(y)-min(y))
	
	x1 = basisMid - 0.5*basisLen
	y1 = min(y)

	x2 = basisMid + 0.5*basisLen
	y2 = min(y)

	x3 = basisMid
	y3 = h + min(y)

	print "p1=%f,%f\n"%(x1, y1)
	print "p2=%f,%f\n"%(x2, y2)
	print "p3=%f,%f\n"%(x3, y3)

def main():
    
    delauneyLib = ctypes.CDLL("./delaunay.so")
    triangulate = delauneyLib.triangulate
    
    x = numpy.random.normal(0.0,20,150)
    y = numpy.random.normal(0.0,20,150)

    x_c = (ctypes.c_double*len(x))(*x)
    y_c = (ctypes.c_double*len(y))(*y)
    p = pointSet(x_c,y_c,len(x))
    
    int3 = ctypes.c_int*3
    ft = ctypes.POINTER(int3)()
    n = pointSet()
    countNoKids = ctypes.c_int()
    
    tb = time.time()
    triangulate(p,ctypes.byref(ft),ctypes.byref(countNoKids), ctypes.byref(n))
    te = time.time()

    print "this took %.3f s\n"%(te-tb)

    fig = pylab.figure()
    ax = fig.add_subplot(111)
    
    for i in range(0,n.numberOfPoints):

        x = n.X[i]
        y = n.Y[i]
        ax.scatter(x,y)
        
    
    c = ['b','g','r','k','m','c']*10000 

    for i in range(0,countNoKids.value):
        #print ft[i][0], ft[i][1], ft[i][2]
        for j in range(0,3):
            idx = ft[i][j]
            x1 = n.X[idx]
            y1 = n.Y[idx]
            
            if (j != 2):
                idx = ft[i][j+1]
                x2 = n.X[idx]
                y2 = n.Y[idx]
            else:
                idx = ft[i][0]
                x2 = n.X[idx]
                y2 = n.Y[idx]
            
            ax.plot([x1,x2],[y1,y2],c[i])
            
    ax.set_aspect(1)
    pylab.show()
    
if __name__ == '__main__':
    main()

