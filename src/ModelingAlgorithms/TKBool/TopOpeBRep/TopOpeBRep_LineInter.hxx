// Created on: 1993-11-10
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRep_LineInter_HeaderFile
#define _TopOpeBRep_LineInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TopOpeBRep_TypeLineCurve.hxx>
#include <TopOpeBRep_WPointInter.hxx>
#include <TopOpeBRep_VPointInter.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TopoDS_Face.hxx>
#include <TopOpeBRepDS_Transition.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Real.hxx>
#include <IntSurf_TypeTrans.hxx>
#include <IntSurf_Situation.hxx>
#include <Standard_OStream.hxx>
class IntPatch_Line;
class IntPatch_ALine;
class IntPatch_RLine;
class IntPatch_WLine;
class IntPatch_GLine;
class BRepAdaptor_Surface;
class TopOpeBRep_VPointInter;
class Geom_Curve;
class TCollection_AsciiString;
class TopOpeBRep_Bipoint;

class TopOpeBRep_LineInter
{
public:
  DEFINE_STANDARD_ALLOC

  TopOpeBRep_LineInter();

  Standard_EXPORT void SetLine(const occ::handle<IntPatch_Line>& L,
                               const BRepAdaptor_Surface&        S1,
                               const BRepAdaptor_Surface&        S2);

  void SetFaces(const TopoDS_Face& F1, const TopoDS_Face& F2);

  TopOpeBRep_TypeLineCurve TypeLineCurve() const;

  int NbVPoint() const;

  Standard_EXPORT const TopOpeBRep_VPointInter& VPoint(const int I) const;

  Standard_EXPORT TopOpeBRep_VPointInter& ChangeVPoint(const int I);

  Standard_EXPORT void SetINL();

  bool INL() const;

  Standard_EXPORT void SetIsVClosed();

  bool IsVClosed() const;

  Standard_EXPORT void SetOK(const bool B);

  bool OK() const;

  Standard_EXPORT void SetHasVPonR();

  bool HasVPonR() const;

  Standard_EXPORT void SetVPBounds();

  Standard_EXPORT void VPBounds(int& f, int& l, int& n) const;

  Standard_EXPORT bool IsPeriodic() const;

  Standard_EXPORT double Period() const;

  Standard_EXPORT void Bounds(double& f, double& l) const;

  Standard_EXPORT bool HasVInternal();

  Standard_EXPORT int NbWPoint() const;

  Standard_EXPORT const TopOpeBRep_WPointInter& WPoint(const int I);

  IntSurf_TypeTrans TransitionOnS1() const;

  IntSurf_TypeTrans TransitionOnS2() const;

  IntSurf_Situation SituationS1() const;

  IntSurf_Situation SituationS2() const;

  Standard_EXPORT occ::handle<Geom_Curve> Curve() const;

  Standard_EXPORT occ::handle<Geom_Curve> Curve(const double parmin, const double parmax) const;

  //! returns the edge of a RESTRICTION line (or a null edge).
  Standard_EXPORT const TopoDS_Shape& Arc() const;

  //! returns true if Arc() edge (of a RESTRICTION line) is
  //! an edge of the original face <Index> (1 or 2).
  Standard_EXPORT bool ArcIsEdge(const int I) const;

  const occ::handle<IntPatch_WLine>& LineW() const;

  const occ::handle<IntPatch_GLine>& LineG() const;

  const occ::handle<IntPatch_RLine>& LineR() const;

  Standard_EXPORT bool HasFirstPoint() const;

  Standard_EXPORT bool HasLastPoint() const;

  Standard_EXPORT void ComputeFaceFaceTransition();

  Standard_EXPORT const TopOpeBRepDS_Transition& FaceFaceTransition(const int I) const;

  void Index(const int I);

  int Index() const;

  Standard_EXPORT void DumpType() const;

  Standard_EXPORT void DumpVPoint(const int                      I,
                                  const TCollection_AsciiString& s1,
                                  const TCollection_AsciiString& s2) const;

  Standard_EXPORT void DumpBipoint(const TopOpeBRep_Bipoint&      B,
                                   const TCollection_AsciiString& s1,
                                   const TCollection_AsciiString& s2) const;

  Standard_EXPORT void SetTraceIndex(const int exF1, const int exF2);

  Standard_EXPORT void GetTraceIndex(int& exF1, int& exF2) const;

  Standard_EXPORT Standard_OStream& DumpLineTransitions(Standard_OStream& OS) const;

private:
  bool                                                     myOK;
  int                                                      myIndex;
  int                                                      myNbVPoint;
  bool                                                     myIsVClosed;
  bool                                                     myHasVPonR;
  bool                                                     myINL;
  bool                                                     myVPBDefined;
  int                                                      myVPF;
  int                                                      myVPL;
  int                                                      myVPN;
  TopOpeBRep_TypeLineCurve                                 myTypeLineCurve;
  occ::handle<IntPatch_Line>                               myIL;
  occ::handle<IntPatch_ALine>                              myILA;
  occ::handle<IntPatch_RLine>                              myILR;
  occ::handle<IntPatch_WLine>                              myILW;
  occ::handle<IntPatch_GLine>                              myILG;
  TopOpeBRep_WPointInter                                   myCurrentWP;
  occ::handle<NCollection_HArray1<TopOpeBRep_VPointInter>> myHAVP;
  TopoDS_Face                                              myF1;
  TopoDS_Face                                              myF2;
  TopOpeBRepDS_Transition                                  myLineTonF1;
  TopOpeBRepDS_Transition                                  myLineTonF2;
  TopoDS_Shape                                             myNullShape;
  int                                                      myexF1;
  int                                                      myexF2;
};

#include <TopOpeBRep_LineInter.lxx>

#endif // _TopOpeBRep_LineInter_HeaderFile
