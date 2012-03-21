// Created on: 1997-09-18
// Created by: Christophe MARION
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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

