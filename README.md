# Introduction Delaunay Triangulation

This project implements the [Bowyer Watson algorith](https://en.wikipedia.org/wiki/Bowyer%E2%80%93Watson_algorithm) to calculate a Delaunay triangulation. Please note that this is still work in progress, although it already produces correct triangulation results. 

In an example application, we can place random dots in the window with mouse clicks. If three or more dots are drawn, the triangulation will start. A screen shot is seen below. 

![alt text](https://github.com/sohailc/delaunay/blob/master/delaunay_in_action.png)

The example application depends on QT 5 to work, although it should be straightforward to develop another example application using the same triangulation implementation with another GUI toolkit. This project is mostly implemented in C++ although there is also some lagacy C code that will be replaced in coming iterations.  

