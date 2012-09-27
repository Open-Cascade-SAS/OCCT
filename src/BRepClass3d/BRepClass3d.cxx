// Created on: 1993-01-21
// Created by: Peter KURNEV
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

// File:	BRepClass3d.cxx
// Created:	Thu Sep 20 10:05:46 2012
// Author:	
//		<pkv@PETREX>

#include <BRepClass3d.ixx>


#include <TopAbs_State.hxx>
#include <TopAbs_Orientation.hxx>

#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Iterator.hxx>

#include <BRep_Builder.hxx>
#include <BRepClass3d_SolidClassifier.hxx>


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
  aIt.Initialize(aSolid);
  for (; aIt.More(); aIt.Next()) { 
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
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSy=aIt.Value();
    aOr=aSy.Orientation();
    bInternal=(aOr==TopAbs_INTERNAL);
    break;
  }	
  //
  return bInternal;
}
