// Created on: 1993-07-23
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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



#include <BRepBuilderAPI_MakeSolid.ixx>
#include <TopoDS.hxx>


//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid()
{
}

//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid(const TopoDS_CompSolid& S)
: myMakeSolid(S)
{
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid(const TopoDS_Shell& S)
: myMakeSolid(S)
{
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid(const TopoDS_Shell& S1, 
				     const TopoDS_Shell& S2)
: myMakeSolid(S1,S2)
{
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid(const TopoDS_Shell& S1, 
				     const TopoDS_Shell& S2, 
				     const TopoDS_Shell& S3)
: myMakeSolid(S1,S2,S3)
{
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid(const TopoDS_Solid& So)
: myMakeSolid(So)
{
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}


//=======================================================================
//function : BRepBuilderAPI_MakeSolid
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::BRepBuilderAPI_MakeSolid(const TopoDS_Solid& So, 
				     const TopoDS_Shell& S)
: myMakeSolid(So,S)
{
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  BRepBuilderAPI_MakeSolid::Add(const TopoDS_Shell& S)
{
  myMakeSolid.Add(S);
  if ( myMakeSolid.IsDone()) {
    Done();
    myShape = myMakeSolid.Shape();
  }
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepBuilderAPI_MakeSolid::IsDone() const
{
  return myMakeSolid.IsDone();
}


//=======================================================================
//function : Solid
//purpose  : 
//=======================================================================

const TopoDS_Solid&  BRepBuilderAPI_MakeSolid::Solid()const 
{
  return myMakeSolid.Solid();
}



//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeSolid::operator TopoDS_Solid() const
{
  return Solid();
}

//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================

Standard_Boolean BRepBuilderAPI_MakeSolid::IsDeleted (const TopoDS_Shape& S) 

{
  if(S.ShapeType() == TopAbs_FACE) {
    BRepLib_ShapeModification aStatus = myMakeSolid.FaceStatus(TopoDS::Face(S));
   
    if(aStatus == BRepLib_Deleted) return Standard_True;

  }
    
  return Standard_False;
}

