#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "array.h"

#define EXIT_SUCCESS 0
#define EXIT_FAIL 1

const int bSize = 100; // the initial size of the buffer holding the triangle data

typedef struct pointSet // a set of points. 
{
    double *X;
    double *Y;
    int numberOfPoints;
}pointSet;

typedef int int3[3];

struct _triangulation
{
    int3 *triangles; // the integers of a triangle refer to the indexes of the nodes
    // so if triangles[0] = {4,8,11}, then this triangle is spanned by (nodes.X[4],nodes.Y[4]),
    // (nodes.X[8],nodes.Y[8]) and (nodes.X[11],nodes.Y[11])
    int countTriangles;
    int3 *children; // an array of equal length as the "triangles" array. Each triangle in the
    // "triangles" array can have up to three children. For a detailed explanation of why this is needed and
    // how it works, see RD...
    int bufferSize;
    array *triangleMembershipLookupTable;
    // "triangleMembershipLookupTable" is a nested array, whereby the outer array has a length equal to the
    // number of nodes. Each node can be a member of multiple triangles, which is recorded in the inner array
    pointSet nodes; 
};

struct _triangulation triangulation;

int findMaxMin(double *array, int arraySize, double *max, int *argmax, double *min, int *argmin)
{
    *max = -1E6;
    *min = 1E6;
    int count;

    for(count=0;count<arraySize;count++)
    {
	if (array[count] > *max)
	{
	    if (max != NULL)			
		*max = array[count];
 			
	    if (argmax != NULL)
		*argmax = count;
	}

	if (array[count] < *min)
	{
	    if (min != NULL)
		*min = array[count];
			
	    if (argmin != NULL)			
		*argmin = count;
	}
    }
    
    return EXIT_SUCCESS;
}

int addContainingTriangle(pointSet p, pointSet *pNew)
{
    // find the triangle that conatins all the points and add its vertices to the pointSet
    // as the first thee entries
    double xMax, xMin, yMax, yMin;
    double basisLengthInitTriangle;
    double basisMidpointInitTriangle;
    double heightInitTriangle;
    double xt1, yt1, xt2, yt2, xt3, yt3; // the vertices of the inital triangle
    double *xNew, *yNew;
    int count;
    
    findMaxMin(p.X,p.numberOfPoints,&xMax,NULL,&xMin,NULL);
    findMaxMin(p.Y,p.numberOfPoints,&yMax,NULL,&yMin,NULL);
    
    basisLengthInitTriangle = 1.5*(xMax-xMin);
    basisMidpointInitTriangle = 0.5*(xMax+xMin);
    heightInitTriangle = 3*(yMax-yMin);
    
    xt1 = basisMidpointInitTriangle - 0.5*basisLengthInitTriangle;
    yt1 = yMin-0.1;
    
    xt2 = basisMidpointInitTriangle + 0.5*basisLengthInitTriangle;
    yt2 = yMin-0.1;
    
    xt3 = basisMidpointInitTriangle;
    yt3 = heightInitTriangle + yMin-0.1;
    
    xNew = (double*)malloc((p.numberOfPoints+3)*sizeof(double));
    yNew = (double*)malloc((p.numberOfPoints+3)*sizeof(double));
    
    xNew[0] = xt1;
    xNew[1] = xt2;
    xNew[2] = xt3;
    yNew[0] = yt1;
    yNew[1] = yt2;
    yNew[2] = yt3;
    
    for (count=3;count<p.numberOfPoints+3;count++)
    {
        xNew[count] = p.X[count-3];
        yNew[count] = p.Y[count-3];
    }
    
    pNew->X=xNew;
    pNew->Y=yNew;
    pNew->numberOfPoints = p.numberOfPoints + 3;
    
    return EXIT_SUCCESS;
    
}

int inTriangle(double x0, double y0, int triangleIndex)
{
    int idxNode1 = triangulation.triangles[triangleIndex][0];
    int idxNode2 = triangulation.triangles[triangleIndex][1];
    int idxNode3 = triangulation.triangles[triangleIndex][2];
    
    double x[] = {triangulation.nodes.X[idxNode1], triangulation.nodes.X[idxNode2], triangulation.nodes.X[idxNode3], \
                    triangulation.nodes.X[idxNode1]};
    double y[] = {triangulation.nodes.Y[idxNode1], triangulation.nodes.Y[idxNode2], triangulation.nodes.Y[idxNode3], \
                    triangulation.nodes.Y[idxNode1]};
    
    int count;
    double v1x, v1y, v2x, v2y;
    double cross = 0;
    
    for(count=0;count<3;count++)
    {
        v1x = x0 - x[count];
        v1y = y0 - y[count];
    
        v2x = x[count+1] - x[count];
        v2y = y[count+1] - y[count];
        
        if ( (v1x*v2y - v2x*v1y) * cross >= 0)
            cross = v1x*v2y - v2x*v1y;
        else
            return 0;
    }
    
    return 1;
}

int isConvex(int node1, int node2, int node3, int node4)
{
	double x[] = {triangulation.nodes.X[node1], triangulation.nodes.X[node2], triangulation.nodes.X[node3], triangulation.nodes.X[node4]};
	double y[] = {triangulation.nodes.Y[node1], triangulation.nodes.Y[node2], triangulation.nodes.Y[node3], triangulation.nodes.Y[node4]};

	double v1x, v1y, v2x, v2y;

	double c;
	double cross = 0;
	
	int i;
	for (i=0;i<4;i++)
	{
		if (i < 2)
		{
			v1x = x[i+1] - x[i];
			v1y = y[i+1] - y[i];
			v2x = x[i+2] - x[i+1];
			v2y = y[i+2] - y[i+1];
		}
		if (i == 2)
		{
			v1x = x[i+1] - x[i];
			v1y = y[i+1] - y[i];
			v2x = x[0] - x[i+1];
			v2y = y[0] - y[i+1];
		}
		if (i == 3)
		{
			v1x = x[0] - x[3];
			v1y = y[0] - y[3];
			v2x = x[1] - x[0];
			v2y = y[1] - y[0];
		}

		c = v2x*v1y - v1x*v2y;

		if (c*cross < 0)
			return 0;
		else
			cross = c;
	}

	return 1;
}

double findMinAngle(int node1, int node2, int node3)
{
	double x1 = triangulation.nodes.X[node1];
	double y1 = triangulation.nodes.Y[node1];

	double x2 = triangulation.nodes.X[node2];
	double y2 = triangulation.nodes.Y[node2];

	double x3 = triangulation.nodes.X[node3];
	double y3 = triangulation.nodes.Y[node3];
	
	double v1x = x2-x1;
	double v1y = y2-y1;

	double v2x = x3-x2;
	double v2y = y3-y2;

	double v3x = x1-x3;
	double v3y = y1-y3;

	double lV1Sqr = v1x*v1x + v1y*v1y;
	double lV2Sqr = v2x*v2x + v2y*v2y;
	double lV3Sqr = v3x*v3x + v3y*v3y;

	double cosOfAngle;

	// suppose we have a triangle with sides a,b,c and c is the shortest side, the smallest 
	// angle alpha is the one opposing c. Further, c^2 = a^2 + b^2 - 2*a*b*cos(alpha)
	// cos(alpha) = ( (a^2+b^2)  - c^2 ) / (2*a*b) 
	// first lets find the shortest side

	double a2,b2,c2;
	
	if (lV1Sqr < ( (lV2Sqr < lV3Sqr) ? lV2Sqr : lV3Sqr ) )
	{
		c2 = lV1Sqr;
		a2 = lV2Sqr;
		b2 = lV3Sqr;
	}

	if (lV2Sqr < ( (lV1Sqr < lV3Sqr) ? lV1Sqr : lV3Sqr ) )
	{
		c2 = lV2Sqr;
		a2 = lV3Sqr;
		b2 = lV1Sqr;
	}
	
	if (lV3Sqr < ( (lV1Sqr < lV2Sqr) ? lV1Sqr : lV2Sqr ) )
	{
		c2 = lV3Sqr;
		a2 = lV1Sqr;
		b2 = lV2Sqr;
	}

	cosOfAngle = (a2+b2-c2)/(2*sqrt(a2*b2)); 

	return acos(cosOfAngle);

}

void enlargeBuffers()
{
    int currentBufferSize = triangulation.bufferSize;
    
    if (currentBufferSize == 0)
    {
        triangulation.triangles = (int3*)malloc(bSize*sizeof(int3));
        triangulation.children = (int3*)malloc(bSize*sizeof(int3));
    }else{
        triangulation.triangles = (int3*)realloc(triangulation.triangles,(currentBufferSize+bSize)*sizeof(int3));
        triangulation.children = (int3*)realloc(triangulation.children,(currentBufferSize+bSize)*sizeof(int3));
    }
    
    triangulation.bufferSize += bSize;
    
    int count;
    
    for (count=currentBufferSize;count<triangulation.bufferSize;count++)
    {
        triangulation.triangles[count][0] = -1;
        triangulation.triangles[count][1] = -1;
        triangulation.triangles[count][2] = -1;

        triangulation.children[count][0] = -1;
        triangulation.children[count][1] = -1;
        triangulation.children[count][2] = -1;
    }
}

void makeTriangle(int idxNode1, int idxNode2, int idxNode3, int idxMom, int idxDad, int *idxNewTri)
{
    // a new triangle always has a "mom" as a parent. It can also have a "dad". If it does not, then 
    // "idxDad = -1"
    // Scenario in which we only have a mom: We add a node d to the triangulation and find that it falls 
    // inside a triangle with index "idxTri1" with nodes {a,b,c}. We have to make three new triangles with 
    // nodes {d,a,b}, {d,a,c} and {d,b,c}. The old triangle with index "idxTri1" will be the sole parent 
    // (mother) of the new triangles. 
    // Scenario in which we have a mom and a dad: We test for a triangle {a,b,c} with index idxTr1 if it 
    // is optimally triangulated with respect to its neighbours. Suppose we find that it is not optimally 
    // triangulated with respect to the triangle {d,b,c} with index idxTri2 and that we need to make new 
    // triangles {a,d,b} and {a,d,c}. These new triangles will have both a mom (=idxTr1) and a dad (=idxTri2)

    if ((triangulation.countTriangles)%bSize == 0)
    {
            enlargeBuffers();
    }
    
    triangulation.triangles[triangulation.countTriangles][0] = idxNode1;
    triangulation.triangles[triangulation.countTriangles][1] = idxNode2;
    triangulation.triangles[triangulation.countTriangles][2] = idxNode3;
    
    int count;
    
    for(count=0;count<3;count++)
    {
	if (triangulation.children[idxMom][count] == -1)
	{
	    triangulation.children[idxMom][count] = triangulation.countTriangles;
	    break;
	}
    } 

    if (idxDad != -1)
    {
	for(count=0;count<3;count++)
        {
	    if (triangulation.children[idxDad][count] == -1)
	    {
	         triangulation.children[idxDad][count] = triangulation.countTriangles;
		 break;
	    }
        }
    }

    // nodes can NEVER be members of triangles with children
    deleteValueFromArray(&(triangulation.triangleMembershipLookupTable[idxNode1]), idxMom);
    deleteValueFromArray(&(triangulation.triangleMembershipLookupTable[idxNode2]), idxMom);
    deleteValueFromArray(&(triangulation.triangleMembershipLookupTable[idxNode3]), idxMom);

    if (idxDad != -1)
    {
    	deleteValueFromArray(&(triangulation.triangleMembershipLookupTable[idxNode1]), idxDad); 
    	deleteValueFromArray(&(triangulation.triangleMembershipLookupTable[idxNode2]), idxDad);
    	deleteValueFromArray(&(triangulation.triangleMembershipLookupTable[idxNode3]), idxDad);
    }

    *idxNewTri = triangulation.countTriangles;

    addToArray(&(triangulation.triangleMembershipLookupTable[idxNode1]),*idxNewTri); // we say that the node with
    // node number "idxNode1" is a member of the triangle with index "*idxNewTri"
    addToArray(&(triangulation.triangleMembershipLookupTable[idxNode2]),*idxNewTri);
    addToArray(&(triangulation.triangleMembershipLookupTable[idxNode3]),*idxNewTri);
    
    triangulation.countTriangles++;
    
}

int findTriWithCommonFace(int triIdx, int node1, int node2, int *idxTriWCommonFace, int *opposingNodeIndex)
{
	// find a triangle with node1 and node2 as members but who is not equal to the triangle with index triIdx
	// Return the index of this triangle and also the third node, other then node1 and node2, of the 
	// neighbouring triangle. If no such triangle is found, we set "idxTriWCommonFace" to -1

	array node1Membership = triangulation.triangleMembershipLookupTable[node1];

	int count;
	int memberTri;

	*idxTriWCommonFace = -1;
	*opposingNodeIndex = -1;

    	for(count=node1Membership.next[0];count!=-1;count=node1Membership.next[count+1])
    	{
		memberTri = node1Membership.data[count];

		if (memberTri == triIdx)
			continue;

		if (triangulation.triangles[memberTri][0] == node2)
		{
			if (triangulation.triangles[memberTri][1] == node1)
			{
				*opposingNodeIndex = triangulation.triangles[memberTri][2];
			}else{
				*opposingNodeIndex = triangulation.triangles[memberTri][1];
			}
			*idxTriWCommonFace = memberTri;
			break;
		}

		if (triangulation.triangles[memberTri][1] == node2)
		{
			if (triangulation.triangles[memberTri][0] == node1)
			{
				*opposingNodeIndex = triangulation.triangles[memberTri][2];
			}else{
				*opposingNodeIndex = triangulation.triangles[memberTri][0];
			}
			*idxTriWCommonFace = memberTri;
			break;
		}

		if (triangulation.triangles[memberTri][2] == node2)
		{
			if (triangulation.triangles[memberTri][0] == node1)
			{
				*opposingNodeIndex = triangulation.triangles[memberTri][1];
			}else{
				*opposingNodeIndex = triangulation.triangles[memberTri][0];
			}
			*idxTriWCommonFace = memberTri;
			break;
		}	
    	}

	return 0;
}

void assessDelauneyCriterion(int triIndex)
{
	// loop through the neighbours of the triangle. We do this as follows: 
	// for each node in the current triangle, find out which other triangles
	// this node is a member of and also shares a face with the current triangle. 
	// Suppose this triangle has nodes {a,b,c} and a neighbouring triangle has 
	// nodes {d,b,c}. Now assess if the set {a,b,d,c} is convex. If not, move on to the next neighbour
	// If the set is convex assess with the min-max criterion if the current
	// triangulation meets the delauney criterium. We do this as follows: calculate the 
	// minimum angle in the triangles {a,b,c} and {d,b,c} (original triangulation). 
	// Also calculate the minimum angle in the triangles {a,d,b} and {a,d,c} (alternative
	// triangulation). Chose the triangulation which maximizes the minimum angle.
	// if the original triangulation meets the criterium (largest minimum angle), move on to the next
	// neighbour. If the alternative triangulation is optimal:
	// 1) make two new triangles {a,d,b} and {a,d,c} and make them the children of the triangles {a,b,c} 
        //   (mom) and {d,b,c} (dad). Lets suppose these new triangles have indexes idx1, and idx2
	// 2) recursively call this function for the new triangles, that is: 
	//    "assessDelauneyCriterion(idx1)" and "assessDelauneyCriterion(idx2)"
	// 3) move to the next neighbour

	int *memberNodes = triangulation.triangles[triIndex];
	int nodeIdx, m1, m2;
	
	nodeIdx = memberNodes[0];
	m1 = memberNodes[1];
	m2 = memberNodes[2];

	int idxTriNeighbour;
	int idxOpposingNode;
        int idxNewTri1, idxNewTri2; 
	
	double tmp, minAngleSet1, minAngleSet2;

	findTriWithCommonFace(triIndex,m1,m2,&idxTriNeighbour,&idxOpposingNode);
	if (idxTriNeighbour == -1)
		return;

	if (isConvex( nodeIdx, m1, idxOpposingNode, m2 ) == 0) 
		return;

	minAngleSet1 = findMinAngle( nodeIdx, m1, m2 );
	tmp = findMinAngle( idxOpposingNode, m1, m2 );
		
	if (tmp < minAngleSet1) // original triangulation
		minAngleSet1 = tmp;

	minAngleSet2 = findMinAngle( nodeIdx, idxOpposingNode, m1 );
	tmp = findMinAngle( nodeIdx, idxOpposingNode, m2 );
		
	if (tmp < minAngleSet2) // alternative triangulation
		minAngleSet2 = tmp;

	if (minAngleSet1 > minAngleSet2) // if true the original triangulation maximizes all angles
		return;

	// If the alternative triangulation is optimal!
	// make triangle {a,d,b}
	makeTriangle(nodeIdx, idxOpposingNode, m1, triIndex, idxTriNeighbour, &idxNewTri1);

	// make triangle {a,d,c}
    	makeTriangle(nodeIdx, idxOpposingNode, m2, triIndex, idxTriNeighbour, &idxNewTri2);
	
	assessDelauneyCriterion(idxNewTri1);
	
	if (triangulation.children[idxNewTri2][0] == -1)
	    assessDelauneyCriterion(idxNewTri2);
	
	return;
}

void makeChildren(int nodeNumber, int triIdx)
{
    int *memberNodes = triangulation.triangles[triIdx];
    int node1 = memberNodes[0];
    int node2 = memberNodes[1];
    int node3 = memberNodes[2];
    
    // We now know that the node with "nodeNumber" at d is in the triangle with index "triIdx"
    // Suppose this triangle has nodes {a,b,c} with node numbers "node1", "node2" and "node3"
    // respectively. We now want to add a point d such that we make three new triangles with nodes
    // {d,a,b}, {d,b,c} and {d,c,a}. However, these triangles are provisionary. We need to see if 
    // this triangulation meets the delauney criterium

    int idxNewTri1, idxNewTri2, idxNewTri3;

    makeTriangle(nodeNumber, node1, node2, triIdx, -1 ,&idxNewTri1);
    makeTriangle(nodeNumber, node2, node3, triIdx, -1, &idxNewTri2);
    makeTriangle(nodeNumber, node1, node3, triIdx, -1, &idxNewTri3);

    assessDelauneyCriterion(idxNewTri1);
    assessDelauneyCriterion(idxNewTri2);
    assessDelauneyCriterion(idxNewTri3);
    
}

int triangulate(pointSet p, int3 **finalTriangulation, int *countTriWithNoChildren, pointSet *nodes)
{
    triangulation.bufferSize = 0;
    triangulation.countTriangles = 0;
    
    enlargeBuffers();
    
    addContainingTriangle(p, nodes);
    triangulation.nodes = *nodes;
    
    triangulation.triangleMembershipLookupTable = (array*)malloc(nodes->numberOfPoints*sizeof(array));
    int count;
    
    for (count=0;count<nodes->numberOfPoints;count++)
    {
	triangulation.triangleMembershipLookupTable[count].data = NULL;
	triangulation.triangleMembershipLookupTable[count].len = 0;
	triangulation.triangleMembershipLookupTable[count].next = NULL;
    }
    
    // the first triangle is the one containig all the nodes...
    triangulation.triangles[0][0] = 0;
    triangulation.triangles[0][1] = 1;
    triangulation.triangles[0][2] = 2;
    triangulation.countTriangles++;
    addToArray(&(triangulation.triangleMembershipLookupTable[0]),0); // the first node is a member of the first triangle
    addToArray(&(triangulation.triangleMembershipLookupTable[1]),0); // so are the second and thrid nodes. 
    addToArray(&(triangulation.triangleMembershipLookupTable[2]),0);
    
    int countNode;
    int countChild;
    int triIdx = 0;
    int childIdx = 0;
    
    double nodeX, nodeY;
    
    for (countNode=3;countNode<triangulation.nodes.numberOfPoints;countNode++)
    {
        nodeX = triangulation.nodes.X[countNode];
        nodeY = triangulation.nodes.Y[countNode];
        
        triIdx = 0;
        while (triangulation.children[triIdx][0] != -1 || \
		triangulation.children[triIdx][1] != -1 || \
		triangulation.children[triIdx][2] != -1)
        {
            for (countChild=0;countChild<3;countChild++)
            {
                childIdx = triangulation.children[triIdx][countChild];
                
                if (inTriangle(nodeX,nodeY,childIdx))
                {
                    triIdx = childIdx;
                    break;
                }
            }
        }
        
	makeChildren(countNode, triIdx);
    }
    
    *countTriWithNoChildren = 0;
    
    for (count=0;count<triangulation.countTriangles;count++)
    {   
        if (triangulation.children[count][0] == -1)
            (*countTriWithNoChildren)++; 
    }
    
    *finalTriangulation = (int3*)malloc((*countTriWithNoChildren)*sizeof(int3));
    
    *countTriWithNoChildren = 0;
    for (count=0;count<triangulation.countTriangles;count++)
    {
        if (triangulation.children[count][0] == -1)
        {
            (*finalTriangulation)[*countTriWithNoChildren][0] = triangulation.triangles[count][0];
            (*finalTriangulation)[*countTriWithNoChildren][1] = triangulation.triangles[count][1];
            (*finalTriangulation)[*countTriWithNoChildren][2] = triangulation.triangles[count][2];
            (*countTriWithNoChildren)++;
        }
    }
    
    free(triangulation.triangles);
    free(triangulation.children);
    
    return EXIT_SUCCESS;
}
