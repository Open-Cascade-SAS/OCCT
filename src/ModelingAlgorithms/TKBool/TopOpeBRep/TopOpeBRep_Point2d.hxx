// Created on: 1998-10-29
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _TopOpeBRep_Point2d_HeaderFile
#define _TopOpeBRep_Point2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IntRes2d_IntersectionPoint.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepDS_Transition.hxx>
#include <Standard_Integer.hxx>
#include <TopOpeBRep_P2Dstatus.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <TopOpeBRepDS_Config.hxx>
class TopOpeBRep_Hctxff2d;
class TopOpeBRep_Hctxee2d;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

class TopOpeBRep_Point2d
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRep_Point2d();

  Standard_EXPORT void Dump(const int ie1 = 0, const int ie2 = 0) const;

  void SetPint(const IntRes2d_IntersectionPoint& P);

  bool HasPint() const;

  const IntRes2d_IntersectionPoint& Pint() const;

  void SetIsVertex(const int I, const bool B);

  bool IsVertex(const int I) const;

  void SetVertex(const int I, const TopoDS_Vertex& V);

  Standard_EXPORT const TopoDS_Vertex& Vertex(const int I) const;

  void SetTransition(const int I, const TopOpeBRepDS_Transition& T);

  Standard_EXPORT const TopOpeBRepDS_Transition& Transition(const int I) const;

  Standard_EXPORT TopOpeBRepDS_Transition& ChangeTransition(const int I);

  void SetParameter(const int I, const double P);

  double Parameter(const int I) const;

  void SetIsPointOfSegment(const bool B);

  bool IsPointOfSegment() const;

  void SetSegmentAncestors(const int IP1, const int IP2);

  bool SegmentAncestors(int& IP1, int& IP2) const;

  void SetStatus(const TopOpeBRep_P2Dstatus S);

  TopOpeBRep_P2Dstatus Status() const;

  void SetIndex(const int X);

  int Index() const;

  void SetValue(const gp_Pnt& P);

  const gp_Pnt& Value() const;

  void SetValue2d(const gp_Pnt2d& P);

  const gp_Pnt2d& Value2d() const;

  void SetKeep(const bool B);

  bool Keep() const;

  void SetEdgesConfig(const TopOpeBRepDS_Config C);

  TopOpeBRepDS_Config EdgesConfig() const;

  void SetTolerance(const double T);

  double Tolerance() const;

  void SetHctxff2d(const occ::handle<TopOpeBRep_Hctxff2d>& ff2d);

  occ::handle<TopOpeBRep_Hctxff2d> Hctxff2d() const;

  void SetHctxee2d(const occ::handle<TopOpeBRep_Hctxee2d>& ee2d);

  occ::handle<TopOpeBRep_Hctxee2d> Hctxee2d() const;

  friend class TopOpeBRep_EdgesIntersector;

private:
  IntRes2d_IntersectionPoint  mypint;
  bool            myhaspint;
  bool            myisvertex1;
  TopoDS_Vertex               myvertex1;
  TopOpeBRepDS_Transition     mytransition1;
  double               myparameter1;
  bool            myisvertex2;
  TopoDS_Vertex               myvertex2;
  TopOpeBRepDS_Transition     mytransition2;
  double               myparameter2;
  bool            myispointofsegment;
  int            myips1;
  int            myips2;
  bool            myhasancestors;
  TopOpeBRep_P2Dstatus        mystatus;
  int            myindex;
  gp_Pnt                      mypnt;
  gp_Pnt2d                    mypnt2d;
  bool            mykeep;
  TopOpeBRepDS_Config         myedgesconfig;
  double               mytolerance;
  occ::handle<TopOpeBRep_Hctxff2d> myctxff2d;
  occ::handle<TopOpeBRep_Hctxee2d> myctxee2d;
};

#include <TopOpeBRep_Point2d.lxx>

#endif // _TopOpeBRep_Point2d_HeaderFile
