// Created on: 1995-01-04
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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


#include <BRep_Builder.hxx>
#include <BRepLib_MakeSolid.hxx>
#include <StdFail_NotDone.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

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
  aShell.Closed (BRep_Tool::IsClosed (aShell));

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

const TopoDS_Solid&  BRepLib_MakeSolid::Solid()
{
  return TopoDS::Solid(Shape());
}



//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepLib_MakeSolid::operator TopoDS_Solid()
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

