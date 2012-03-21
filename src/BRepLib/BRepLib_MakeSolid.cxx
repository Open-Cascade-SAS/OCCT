// Created on: 1995-01-04
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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



#include <BRepLib_MakeSolid.ixx>

#include <TopoDS.hxx>
#include <BRep_Builder.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=======================================================================
//function : BRepLib_MakeSolid
//purpose  : 
//=======================================================================

BRepLib_MakeSolid::BRepLib_MakeSolid()
{
  BRep_Builder B;
  B.MakeSolid(TopoDS::Solid(myShape));
  Done();
}

//=======================================================================
//function : BRepLib_MakeSolid
//purpose  : 
//=======================================================================

BRepLib_MakeSolid::BRepLib_MakeSolid(const TopoDS_CompSolid& S)
{
  BRep_Builder B;
  B.MakeSolid(TopoDS::Solid(myShape));

  TopExp_Explorer ex1, ex2;
  TopTools_MapOfShape aMapOfFaces;
  for (ex1.Init(S,TopAbs_SHELL); ex1.More(); ex1.Next()) {
    for (ex2.Init(ex1.Current(),TopAbs_FACE); ex2.More(); ex2.Next()) {
      if(!aMapOfFaces.Add(ex2.Current())) {
	myDeletedFaces.Append(ex2.Current());
	aMapOfFaces.Remove(ex2.Current());
      }
    }    
  }

  TopoDS_Shape aShell;
  B.MakeShell(TopoDS::Shell(aShell));

  TopTools_MapIteratorOfMapOfShape aFaceIter(aMapOfFaces);
  for(; aFaceIter.More(); aFaceIter.Next()) {
    B.Add(aShell, aFaceIter.Key());
  }

  B.Add(myShape,aShell);

  Done();
}


//=======================================================================
//function : BRepLib_MakeSolid
//purpose  : 
//=======================================================================

BRepLib_MakeSolid::BRepLib_MakeSolid(const TopoDS_Shell& S)
{
  BRep_Builder B;
  B.MakeSolid(TopoDS::Solid(myShape));
  B.Add(myShape,S);
  Done();
}


//=======================================================================
//function : BRepLib_MakeSolid
//purpose  : 
//=======================================================================

BRepLib_MakeSolid::BRepLib_MakeSolid(const TopoDS_Shell& S1, const TopoDS_Shell& S2)
{
  BRep_Builder B;
  B.MakeSolid(TopoDS::Solid(myShape));
  B.Add(myShape,S1);
  B.Add(myShape,S2);
  Done();
}


//=======================================================================
//function : BRepLib_MakeSolid
//purpose  : 
//=======================================================================

BRepLib_MakeSolid::BRepLib_MakeSolid(const TopoDS_Shell& S1, const TopoDS_Shell& S2, const TopoDS_Shell& S3)
{
  BRep_Builder B;
  B.MakeSolid(TopoDS::Solid(myShape));
  B.Add(myShape,S1);
  B.Add(myShape,S2);
  B.Add(myShape,S3);
  Done();
}


//=======================================================================
//function : BRepLib_MakeSolid
//purpose  : 
//=======================================================================

BRepLib_MakeSolid::BRepLib_MakeSolid(const TopoDS_Solid& So)
{
  BRep_Builder B;
  B.MakeSolid(TopoDS::Solid(myShape));
  TopExp_Explorer ex;
  for (ex.Init(So,TopAbs_SHELL); ex.More(); ex.Next())
    B.Add(myShape,ex.Current());
  Done();
}


//=======================================================================
//function : BRepLib_MakeSolid
//purpose  : 
//=======================================================================

BRepLib_MakeSolid::BRepLib_MakeSolid(const TopoDS_Solid& So, const TopoDS_Shell& S)
{
  BRep_Builder B;
  B.MakeSolid(TopoDS::Solid(myShape));
  TopExp_Explorer ex;
  for (ex.Init(So,TopAbs_SHELL); ex.More(); ex.Next())
    B.Add(myShape,ex.Current());
  B.Add(myShape,S);
  Done();
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  BRepLib_MakeSolid::Add(const TopoDS_Shell& S)
{
  BRep_Builder B;
  B.Add(myShape,S);
}


//=======================================================================
//function : Solid
//purpose  : 
//=======================================================================

const TopoDS_Solid&  BRepLib_MakeSolid::Solid()const 
{
  return TopoDS::Solid(Shape());
}



//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepLib_MakeSolid::operator TopoDS_Solid() const
{
  return TopoDS::Solid(Shape());
}

//=======================================================================
//function : FaceStatus
//purpose  : 
//=======================================================================

BRepLib_ShapeModification BRepLib_MakeSolid::FaceStatus
  (const TopoDS_Face& F) const
{
  BRepLib_ShapeModification myStatus = BRepLib_Preserved;
  TopTools_ListIteratorOfListOfShape anIter(myDeletedFaces);

  for(; anIter.More(); anIter.Next()) {
    if(F.IsSame(anIter.Value())) {
      myStatus = BRepLib_Deleted;
      break;
    }
  }
  
  return myStatus;
}

