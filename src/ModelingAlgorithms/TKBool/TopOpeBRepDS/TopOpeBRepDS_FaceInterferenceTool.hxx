// Created on: 1994-11-08
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _TopOpeBRepDS_FaceInterferenceTool_HeaderFile
#define _TopOpeBRepDS_FaceInterferenceTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepDS_PDataStructure.hxx>
#include <TopAbs_Orientation.hxx>
#include <Standard_Integer.hxx>
#include <TopTrans_SurfaceTransition.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>
class TopOpeBRepDS_Interference;
class TopOpeBRepDS_Curve;

//! a tool computing complex transition on Face.
class TopOpeBRepDS_FaceInterferenceTool
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepDS_FaceInterferenceTool(const TopOpeBRepDS_PDataStructure& P);

  //! Eisnew = true if E is a new edge built on edge I->Geometry()
  //! false if E is shape <=> I->Geometry()
  Standard_EXPORT void Init(const TopoDS_Shape&                      FI,
                            const TopoDS_Shape&                      E,
                            const Standard_Boolean                   Eisnew,
                            const Handle(TopOpeBRepDS_Interference)& I);

  //! Eisnew = true if E is a new edge built on edge I->Geometry()
  //! false if E is shape <=> I->Geometry()
  Standard_EXPORT void Add(const TopoDS_Shape&                      FI,
                           const TopoDS_Shape&                      F,
                           const TopoDS_Shape&                      E,
                           const Standard_Boolean                   Eisnew,
                           const Handle(TopOpeBRepDS_Interference)& I);

  Standard_EXPORT void Add(const TopoDS_Shape&                      E,
                           const TopOpeBRepDS_Curve&                C,
                           const Handle(TopOpeBRepDS_Interference)& I);

  Standard_EXPORT void SetEdgePntPar(const gp_Pnt& P, const Standard_Real par);

  Standard_EXPORT void GetEdgePntPar(gp_Pnt& P, Standard_Real& par) const;

  Standard_EXPORT Standard_Boolean IsEdgePntParDef() const;

  Standard_EXPORT void Transition(const Handle(TopOpeBRepDS_Interference)& I) const;

protected:
private:
  TopOpeBRepDS_PDataStructure myPBDS;
  Standard_Boolean            myrefdef;
  TopAbs_Orientation          myFaceOrientation;
  Standard_Integer            myFaceOriented;
  TopTrans_SurfaceTransition  myTool;
  TopoDS_Shape                myEdge;
  Standard_Boolean            isLine;
  gp_Pnt                      myPntOnEd;
  Standard_Real               myParOnEd;
  Standard_Boolean            myOnEdDef;
  Standard_Real               myTole;
};

#endif // _TopOpeBRepDS_FaceInterferenceTool_HeaderFile
