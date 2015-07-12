// Created on: 1993-01-21
// Created by: Peter KURNEV
// Copyright (c) 1993-1999 Matra Datavision
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

// File:	BRepClass3d.cxx
// Created:	Thu Sep 20 10:05:46 2012
// Author:	
//		<pkv@PETREX>

#include <BRep_Builder.hxx>
#include <BRepClass3d.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_State.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>

static 
  Standard_Boolean IsInternal(const TopoDS_Shell& aSx);

//=======================================================================
//function : OuterShell
//purpose  : 
//=======================================================================
TopoDS_Shell BRepClass3d::OuterShell(const TopoDS_Solid& aSolid)
{
  Standard_Boolean bFound;
  Standard_Real aTol;
  TopoDS_Solid aSDx;
  TopoDS_Shell aShell, aDummy;
  TopoDS_Iterator aIt;
  BRep_Builder aBB;
  BRepClass3d_SolidClassifier aSC;
  //
  if (aSolid.IsNull()) {
    return aDummy;
  }
  //
  aTol=1.e-7;
  bFound=Standard_False;
  //
  // if solid has one shell, it will return, without checking orientation 
  Standard_Integer aShellCounter = 0;
  for (aIt.Initialize(aSolid); aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSx=aIt.Value();
    if (aSx.ShapeType()==TopAbs_SHELL) {
      aShell=*((TopoDS_Shell*)&aSx);
      aShellCounter++;
      if (aShellCounter >= 2)
        break;
    }
  }
  if (aShellCounter == 0) {
    return aDummy;
  }
  else if (aShellCounter == 1) {
    return aShell;
  }
  //
  for (aIt.Initialize(aSolid); aIt.More(); aIt.Next()) { 
    const TopoDS_Shape& aSx=aIt.Value();
    if (aSx.ShapeType()==TopAbs_SHELL) {
      aShell=*((TopoDS_Shell*)&aSx);
      if (!IsInternal(aShell)) {
	aSDx=aSolid;
	aSDx.EmptyCopy();
	aBB.Add(aSDx, aShell);
	//
	aSC.Load(aSDx);
	aSC.PerformInfinitePoint(aTol);
	if(aSC.State()==TopAbs_OUT) {
	  bFound=Standard_True;
	  break;
	}
      }
    }
  }
  //
  if (!bFound) {
    return aDummy;
  } 
  //
  return aShell;
}

//=======================================================================
//function : IsInternal
//purpose  : 
//=======================================================================
Standard_Boolean IsInternal(const TopoDS_Shell& aSx)
{
  Standard_Boolean bInternal;
  TopAbs_Orientation aOr;
  TopoDS_Iterator aIt; 
  //
  bInternal=Standard_False;
  //
  aIt.Initialize(aSx);
  if (aIt.More()) {
    const TopoDS_Shape& aSy=aIt.Value();
    aOr=aSy.Orientation();
    bInternal=(aOr==TopAbs_INTERNAL);
  }	
  //
  return bInternal;
}
