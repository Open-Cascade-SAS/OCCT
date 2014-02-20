// Created on: 1997-09-18
// Created by: Christophe MARION
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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

