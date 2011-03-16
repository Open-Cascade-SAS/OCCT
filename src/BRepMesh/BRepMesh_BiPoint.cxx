// File:	BRepMesh_BiPoint.cxx
// Created:	Thu Sep 18 10:34:24 1997
// Author:	Christophe MARION
//		<cma@partox.paris1.matra-dtv.fr>

//#define No_Exception

#include <BRepMesh_BiPoint.ixx>

#define PntX1  myCoordinates[0]
#define PntY1  myCoordinates[1]
#define PntX2  myCoordinates[2]
#define PntY2  myCoordinates[3]
#define VectX  myCoordinates[4]
#define VectY  myCoordinates[5]

#define MinSg  myIndices[0]
#define MaxSg  myIndices[1]

//=======================================================================
//function : BRepMesh_BiPoint
//purpose  : 
//=======================================================================

BRepMesh_BiPoint::BRepMesh_BiPoint (const Standard_Real X1,
				    const Standard_Real Y1,
				    const Standard_Real X2,
				    const Standard_Real Y2)
{
  PntX1 = X1;
  PntY1 = Y1;
  PntX2 = X2;
  PntY2 = Y2;
}

