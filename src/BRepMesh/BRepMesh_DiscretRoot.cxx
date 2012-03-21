// Created on: 2008-04-10
// Created by: Peter KURNEV
// Copyright (c) 2008-2012 OPEN CASCADE SAS
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
