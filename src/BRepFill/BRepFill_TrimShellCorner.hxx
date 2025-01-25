// Created on: 2003-10-21
// Created by: Mikhail KLOKOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _BRepFill_TrimShellCorner_HeaderFile
#define _BRepFill_TrimShellCorner_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepFill_TransitionStyle.hxx>
#include <gp_Ax2.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_HArray2OfShape.hxx>
#include <TopTools_HArray1OfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BOPDS_PDS.hxx>

//! Trims sets of faces in the corner to make proper parts of pipe
class BRepFill_TrimShellCorner
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor: takes faces to intersect,
  //! type of transition (it can be RightCorner or RoundCorner)
  //! and axis of bisector plane
  //! theIntersectPointCrossDirection : prev path direction at the origin point of theAxeOfBisPlane
  //! cross next path direction at the origin point of theAxeOfBisPlane. used when EE has more than
  //! one vertices
  Standard_EXPORT BRepFill_TrimShellCorner(const Handle(TopTools_HArray2OfShape)& theFaces,
                                           const BRepFill_TransitionStyle         theTransition,
                                           const gp_Ax2&                          theAxeOfBisPlane,
                                           const gp_Vec& theIntPointCrossDir);

  Standard_EXPORT void AddBounds(const Handle(TopTools_HArray2OfShape)& Bounds);

  Standard_EXPORT void AddUEdges(const Handle(TopTools_HArray2OfShape)& theUEdges);

  Standard_EXPORT void AddVEdges(const Handle(TopTools_HArray2OfShape)& theVEdges,
                                 const Standard_Integer                 theIndex);

  Standard_EXPORT void Perform();

  Standard_EXPORT Standard_Boolean IsDone() const;

  Standard_EXPORT Standard_Boolean HasSection() const;

  Standard_EXPORT void Modified(const TopoDS_Shape& S, TopTools_ListOfShape& theModified);

protected:
private:
  Standard_Boolean MakeFacesSec(const Standard_Integer theIndex,
                                const BOPDS_PDS&       theDS,
                                const Standard_Integer theFaceIndex1,
                                const Standard_Integer theFaceIndex2,
                                const Standard_Integer theSSInterfIndex);

  Standard_Boolean MakeFacesNonSec(const Standard_Integer theIndex,
                                   const BOPDS_PDS&       theDS,
                                   const Standard_Integer theFaceIndex1,
                                   const Standard_Integer theFaceIndex2);

  Standard_Boolean ChooseSection(const TopoDS_Shape&  Comp,
                                 const TopoDS_Vertex& theFirstVertex,
                                 const TopoDS_Vertex& theLastVertex,
                                 TopoDS_Shape&        resWire,
                                 gp_Pln&              resPlane,
                                 Standard_Boolean&    IsSingular);

  BRepFill_TransitionStyle           myTransition;
  gp_Ax2                             myAxeOfBisPlane;
  gp_Vec                             myIntPointCrossDir;
  TopoDS_Shape                       myShape1;
  TopoDS_Shape                       myShape2;
  Handle(TopTools_HArray2OfShape)    myBounds;
  Handle(TopTools_HArray2OfShape)    myUEdges;
  Handle(TopTools_HArray1OfShape)    myVEdges;
  Handle(TopTools_HArray2OfShape)    myFaces;
  Standard_Boolean                   myDone;
  Standard_Boolean                   myHasSection;
  TopTools_DataMapOfShapeListOfShape myHistMap;
};

#endif // _BRepFill_TrimShellCorner_HeaderFile
