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
#include <TopOpeBRepDS_Interference.hxx>
#include <NCollection_List.hxx>
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
                              const occ::handle<TopOpeBRepDS_HDataStructure>& HDS);

  Standard_EXPORT void Face(const int I, const TopoDS_Shape& F);

  Standard_EXPORT const TopoDS_Shape& Face(const int I) const;

private:
  Standard_EXPORT bool GetGeometry(NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator& IT,
                                               const TopOpeBRep_Point2d&                      P,
                                               int&                              G,
                                               TopOpeBRepDS_Kind& K) const;

  Standard_EXPORT bool MakeGeometry(const TopOpeBRep_Point2d& P,
                                                int&         G,
                                                TopOpeBRepDS_Kind&        K) const;

  Standard_EXPORT void SetShapeTransition(const TopOpeBRep_Point2d& P,
                                          TopOpeBRepDS_Transition&  T1,
                                          TopOpeBRepDS_Transition&  T2) const;

  Standard_EXPORT occ::handle<TopOpeBRepDS_Interference> StorePI(const TopOpeBRep_Point2d&      P,
                                                            const TopOpeBRepDS_Transition& T,
                                                            const int         EI,
                                                            const int         PI,
                                                            const double            p,
                                                            const int         IE);

  Standard_EXPORT occ::handle<TopOpeBRepDS_Interference> StoreVI(const TopOpeBRep_Point2d&      P,
                                                            const TopOpeBRepDS_Transition& T,
                                                            const int         EI,
                                                            const int         VI,
                                                            const bool         VB,
                                                            const TopOpeBRepDS_Config      C,
                                                            const double            p,
                                                            const int         IE);

  Standard_EXPORT bool ToRecompute(const TopOpeBRep_Point2d&                P,
                                               const occ::handle<TopOpeBRepDS_Interference>& I,
                                               const int                   IEmother);

  Standard_EXPORT void StoreRecompute(const occ::handle<TopOpeBRepDS_Interference>& I,
                                      const int                   IEmother);

  Standard_EXPORT void RecomputeInterferences(const TopoDS_Edge&               E,
                                              NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LOI);

  TopoDS_Edge                         myE1;
  TopoDS_Edge                         myE2;
  TopoDS_Face                         myF1;
  TopoDS_Face                         myF2;
  occ::handle<TopOpeBRepDS_HDataStructure> myHDS;
  TopOpeBRepDS_PDataStructure         myPDS;
  TopOpeBRep_PEdgesIntersector        myPEI;
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>     myLI1;
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>     myLI2;
};

#endif // _TopOpeBRep_EdgesFiller_HeaderFile
