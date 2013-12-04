// Created on: 2008-04-10
// Created by: Peter KURNEV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BRepMesh_DiscretRoot.ixx>

//=======================================================================
//function : BRepMesh_DiscretRoot
//purpose  :
//=======================================================================
BRepMesh_DiscretRoot::BRepMesh_DiscretRoot()
: myDeflection (0.001),
  myAngle (0.1),
  myIsDone (Standard_False)
{
  //
}

//=======================================================================
//function : ~
//purpose  :
//=======================================================================
BRepMesh_DiscretRoot::~BRepMesh_DiscretRoot()
{
}

//=======================================================================
//function : SetDeflection
//purpose  :
//=======================================================================
void BRepMesh_DiscretRoot::SetDeflection (const Standard_Real theDeflection)
{
  myDeflection = theDeflection;
}

//=======================================================================
//function : Deflection
//purpose  : 
//=======================================================================
Standard_Real BRepMesh_DiscretRoot::Deflection() const
{
  return myDeflection;
}

//=======================================================================
//function : SetAngle
//purpose  :
//=======================================================================
void BRepMesh_DiscretRoot::SetAngle (const Standard_Real theAngle)
{
  myAngle = theAngle;
}

//=======================================================================
//function : Angle
//purpose  :
//=======================================================================
Standard_Real BRepMesh_DiscretRoot::Angle() const
{
  return myAngle;
}

//=======================================================================
//function : SetShape
//purpose  :
//=======================================================================
void BRepMesh_DiscretRoot::SetShape (const TopoDS_Shape& theShape)
{
  myShape = theShape;
}

//=======================================================================
//function : Shape
//purpose  :
//=======================================================================
const TopoDS_Shape& BRepMesh_DiscretRoot::Shape() const
{
  return myShape;
}

//=======================================================================
//function : IsDone
//purpose  :
//=======================================================================
Standard_Boolean BRepMesh_DiscretRoot::IsDone() const
{
  return myIsDone;
}

//=======================================================================
//function : SetDone
//purpose  :
//=======================================================================
void BRepMesh_DiscretRoot::SetDone()
{
  myIsDone = Standard_True;
}

//=======================================================================
//function : SetNotDone
//purpose  :
//=======================================================================
void BRepMesh_DiscretRoot::SetNotDone()
{
  myIsDone = Standard_False;
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void BRepMesh_DiscretRoot::Init()
{
  //
}
