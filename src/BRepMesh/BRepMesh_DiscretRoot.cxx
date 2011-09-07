// File:    BRepMesh_DiscretRoot.cxx
// Created: Thu Apr 10 10:08:44 2008
// Author:  Peter KURNEV <pkv@irinox>

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
