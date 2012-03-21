// Created on: 1994-12-02
// Created by: Jacques GOUSSARD
// Copyright (c) 1994-1999 Matra Datavision
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



#include <BRepBuilderAPI_ModifyShape.ixx>


#include <Standard_NullObject.hxx>

//=======================================================================
//function : BRepBuilderAPI_ModifyShape
//purpose  : 
//=======================================================================

BRepBuilderAPI_ModifyShape::BRepBuilderAPI_ModifyShape () {}


//=======================================================================
//function : BRepBuilderAPI_ModifyShape
//purpose  : 
//=======================================================================

BRepBuilderAPI_ModifyShape::BRepBuilderAPI_ModifyShape (const TopoDS_Shape& S):
   myModifier(S), myInitialShape(S)
{}


//=======================================================================
//function : BRepBuilderAPI_ModifyShape
//purpose  : 
//=======================================================================

BRepBuilderAPI_ModifyShape::BRepBuilderAPI_ModifyShape 
    (const Handle(BRepTools_Modification)& M)
{
  myModification = M;
}


//=======================================================================
//function : BRepBuilderAPI_ModifyShape
//purpose  : 
//=======================================================================

BRepBuilderAPI_ModifyShape::BRepBuilderAPI_ModifyShape 
  (const TopoDS_Shape& S,
   const Handle(BRepTools_Modification)& M): myModifier(S),myInitialShape(S)
{
  myModification = M;
  DoModif();
}


//=======================================================================
//function : DoModif
//purpose  : 
//=======================================================================

void BRepBuilderAPI_ModifyShape::DoModif ()
{
  if (myInitialShape.IsNull() || myModification.IsNull()) {
    Standard_NullObject::Raise();
  }
  myModifier.Perform(myModification);
  if (myModifier.IsDone()) {
    Done();
    myShape = myModifier.ModifiedShape(myInitialShape);
  }
  else {
    NotDone();
  }
}


//=======================================================================
//function : DoModif
//purpose  : 
//=======================================================================

void BRepBuilderAPI_ModifyShape::DoModif (const TopoDS_Shape& S)
{
  if (!S.IsEqual(myInitialShape) || !IsDone()) {
    myInitialShape = S;
    myModifier.Init(S);
    DoModif();
  }
}


//=======================================================================
//function : DoModif
//purpose  : 
//=======================================================================

void BRepBuilderAPI_ModifyShape::DoModif (const Handle(BRepTools_Modification)& M)
{
  myModification = M;
  DoModif();
}


//=======================================================================
//function : DoModif
//purpose  : 
//=======================================================================

void BRepBuilderAPI_ModifyShape::DoModif (const TopoDS_Shape& S,
				   const Handle(BRepTools_Modification)& M)
{
  myInitialShape = S;
  myModifier.Init(S);
  myModification = M;
  DoModif();
}


//=======================================================================
//function : ModifiedShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepBuilderAPI_ModifyShape::ModifiedShape
  (const TopoDS_Shape& S) const
{
  return myModifier.ModifiedShape(S);
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepBuilderAPI_ModifyShape::Modified
  (const TopoDS_Shape& F)
{
  myGenerated.Clear();
  myGenerated.Append(myModifier.ModifiedShape(F));
  return myGenerated;
}
