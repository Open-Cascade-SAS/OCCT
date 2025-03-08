// Created on: 1994-10-12
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

#ifndef _TopOpeBRep_EdgesFiller_HeaderFile
#define _TopOpeBRep_EdgesFiller_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopOpeBRepDS_PDataStructure.hxx>
#include <TopOpeBRep_PEdgesIntersector.hxx>
#include <Standard_Integer.hxx>
#include <TopOpeBRepDS_ListIteratorOfListOfInterference.hxx>
#include <TopOpeBRepDS_Kind.hxx>
#include <TopOpeBRepDS_Config.hxx>
class TopOpeBRepDS_HDataStructure;
class TopoDS_Shape;
class TopOpeBRep_Point2d;
class TopOpeBRepDS_Transition;
class TopOpeBRepDS_Interference;

//! Fills a TopOpeBRepDS_DataStructure with Edge/Edge
//! intersection data described by TopOpeBRep_EdgesIntersector.
class TopOpeBRep_EdgesFiller
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRep_EdgesFiller();

  Standard_EXPORT void Insert(const TopoDS_Shape&                        E1,
                              const TopoDS_Shape&                        E2,
                              TopOpeBRep_EdgesIntersector&               EI,
                              const Handle(TopOpeBRepDS_HDataStructure)& HDS);

  Standard_EXPORT void Face(const Standard_Integer I, const TopoDS_Shape& F);

  Standard_EXPORT const TopoDS_Shape& Face(const Standard_Integer I) const;

protected:
private:
  Standard_EXPORT Standard_Boolean GetGeometry(TopOpeBRepDS_ListIteratorOfListOfInterference& IT,
                                               const TopOpeBRep_Point2d&                      P,
                                               Standard_Integer&                              G,
                                               TopOpeBRepDS_Kind& K) const;

  Standard_EXPORT Standard_Boolean MakeGeometry(const TopOpeBRep_Point2d& P,
                                                Standard_Integer&         G,
                                                TopOpeBRepDS_Kind&        K) const;

  Standard_EXPORT void SetShapeTransition(const TopOpeBRep_Point2d& P,
                                          TopOpeBRepDS_Transition&  T1,
                                          TopOpeBRepDS_Transition&  T2) const;

  Standard_EXPORT Handle(TopOpeBRepDS_Interference) StorePI(const TopOpeBRep_Point2d&      P,
                                                            const TopOpeBRepDS_Transition& T,
                                                            const Standard_Integer         EI,
                                                            const Standard_Integer         PI,
                                                            const Standard_Real            p,
                                                            const Standard_Integer         IE);

  Standard_EXPORT Handle(TopOpeBRepDS_Interference) StoreVI(const TopOpeBRep_Point2d&      P,
                                                            const TopOpeBRepDS_Transition& T,
                                                            const Standard_Integer         EI,
                                                            const Standard_Integer         VI,
                                                            const Standard_Boolean         VB,
                                                            const TopOpeBRepDS_Config      C,
                                                            const Standard_Real            p,
                                                            const Standard_Integer         IE);

  Standard_EXPORT Standard_Boolean ToRecompute(const TopOpeBRep_Point2d&                P,
                                               const Handle(TopOpeBRepDS_Interference)& I,
                                               const Standard_Integer                   IEmother);

  Standard_EXPORT void StoreRecompute(const Handle(TopOpeBRepDS_Interference)& I,
                                      const Standard_Integer                   IEmother);

  Standard_EXPORT void RecomputeInterferences(const TopoDS_Edge&               E,
                                              TopOpeBRepDS_ListOfInterference& LOI);

  TopoDS_Edge                         myE1;
  TopoDS_Edge                         myE2;
  TopoDS_Face                         myF1;
  TopoDS_Face                         myF2;
  Handle(TopOpeBRepDS_HDataStructure) myHDS;
  TopOpeBRepDS_PDataStructure         myPDS;
  TopOpeBRep_PEdgesIntersector        myPEI;
  TopOpeBRepDS_ListOfInterference     myLI1;
  TopOpeBRepDS_ListOfInterference     myLI2;
};

#endif // _TopOpeBRep_EdgesFiller_HeaderFile
