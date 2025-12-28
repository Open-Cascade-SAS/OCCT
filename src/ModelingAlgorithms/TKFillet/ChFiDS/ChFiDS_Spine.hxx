// Created on: 1993-11-09
// Created by: Laurent BOURESCHE
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

#ifndef _ChFiDS_Spine_HeaderFile
#define _ChFiDS_Spine_HeaderFile

#include <BRepAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <GeomAbs_CurveType.hxx>
#include <ChFiDS_ChamfMode.hxx>
#include <ChFiDS_ErrorStatus.hxx>
#include <ChFiDS_ElSpine.hxx>
#include <NCollection_List.hxx>
#include <ChFiDS_State.hxx>
#include <ChFiDS_TypeOfConcavity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>

class TopoDS_Edge;
class gp_Lin;
class gp_Circ;
class TopoDS_Vertex;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

//! Contains information necessary for construction of
//! a 3D fillet or chamfer:
//!
//! - guideline composed of edges of the solid, tangents
//! between them, and borders by faces tangents
//! between them.
//!
//! Tools for construction of the Sp
//! by propagation from an edge of solid
//! are provided in the Builder of Fil3d.
//!
//! The Spine contains among others the
//! information about the nature of extremities
//! of the fillet ( on free border , on section or closed ).
//!
//! IMPORTANT NOTE: the guideline represented
//! in this way is not C2, although the path
//! claims it. Several palliative workarounds
//! (see the methods at the end) are planned,
//! but they are not enough. It is necessary to change
//! the approach and double the Spine of line C2 with
//! the known consequences for management of
//! interactions between KPart Blend in Fil3d.
class ChFiDS_Spine : public Standard_Transient
{

public:
  Standard_EXPORT ChFiDS_Spine();

  Standard_EXPORT ChFiDS_Spine(const double Tol);

  //! store edges composing the guideline
  void SetEdges(const TopoDS_Edge& E);

  //! store offset edges composing the offset guideline
  void SetOffsetEdges(const TopoDS_Edge& E);

  //! store the edge at the first position before all others
  void PutInFirst(const TopoDS_Edge& E);

  //! store the offset edge at the first position before all others
  void PutInFirstOffset(const TopoDS_Edge& E);

  int NbEdges() const;

  const TopoDS_Edge& Edges(const int I) const;

  const TopoDS_Edge& OffsetEdges(const int I) const;

  //! stores if the start of a set of edges starts on a
  //! section of free border or forms a closed contour
  void SetFirstStatus(const ChFiDS_State S);

  //! stores if the end of a set of edges starts on a
  //! section of free border or forms a closed contour
  void SetLastStatus(const ChFiDS_State S);

  Standard_EXPORT virtual void AppendElSpine(const occ::handle<ChFiDS_ElSpine>& Els);

  Standard_EXPORT virtual void AppendOffsetElSpine(const occ::handle<ChFiDS_ElSpine>& Els);

  Standard_EXPORT occ::handle<ChFiDS_ElSpine> ElSpine(const int IE) const;

  Standard_EXPORT occ::handle<ChFiDS_ElSpine> ElSpine(const TopoDS_Edge& E) const;

  Standard_EXPORT occ::handle<ChFiDS_ElSpine> ElSpine(const double W) const;

  Standard_EXPORT NCollection_List<occ::handle<ChFiDS_ElSpine>>& ChangeElSpines();

  Standard_EXPORT NCollection_List<occ::handle<ChFiDS_ElSpine>>& ChangeOffsetElSpines();

  Standard_EXPORT virtual void Reset(const bool AllData = false);

  Standard_EXPORT bool SplitDone() const;

  Standard_EXPORT void SplitDone(const bool B);

  //! prepare the guideline depending on the edges that
  //! are elementary arks (take parameters from
  //! a single curvilinear abscissa); to be able to call
  //! methods on the geometry (first,last,value,d1,d2)
  //! it is necessary to start with preparation otherwise an
  //! exception will be raised
  Standard_EXPORT void Load();

  Standard_EXPORT double Resolution(const double R3d) const;

  Standard_EXPORT bool IsClosed() const;

  Standard_EXPORT double FirstParameter() const;

  Standard_EXPORT double LastParameter() const;

  Standard_EXPORT void SetFirstParameter(const double Par);

  Standard_EXPORT void SetLastParameter(const double Par);

  //! gives the total length of all arcs before the
  //! number IndexSp
  Standard_EXPORT double FirstParameter(const int IndexSpine) const;

  //! gives the total length till the ark with number
  //! IndexSpine (inclus)
  Standard_EXPORT double LastParameter(const int IndexSpine) const;

  //! gives the length of ark with number IndexSp
  Standard_EXPORT double Length(const int IndexSpine) const;

  Standard_EXPORT bool IsPeriodic() const;

  Standard_EXPORT double Period() const;

  Standard_EXPORT double Absc(const double U);

  Standard_EXPORT double Absc(const double U, const int I);

  Standard_EXPORT void Parameter(const double AbsC, double& U, const bool Oriented = true);

  Standard_EXPORT void Parameter(const int    Index,
                                 const double AbsC,
                                 double&      U,
                                 const bool   Oriented = true);

  Standard_EXPORT gp_Pnt Value(const double AbsC);

  Standard_EXPORT void D0(const double AbsC, gp_Pnt& P);

  Standard_EXPORT void D1(const double AbsC, gp_Pnt& P, gp_Vec& V1);

  Standard_EXPORT void D2(const double AbsC, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2);

  Standard_EXPORT void SetCurrent(const int Index);

  //! sets the current curve and returns it
  Standard_EXPORT const BRepAdaptor_Curve& CurrentElementarySpine(const int Index);

  int CurrentIndexOfElementarySpine() const;

  Standard_EXPORT GeomAbs_CurveType GetType() const;

  Standard_EXPORT gp_Lin Line() const;

  Standard_EXPORT gp_Circ Circle() const;

  //! returns if the set of edges starts on a free boundary
  //! or if the first vertex is a breakpoint or if the set is
  //! closed
  ChFiDS_State FirstStatus() const;

  //! returns the state at the end of the set
  ChFiDS_State LastStatus() const;

  ChFiDS_State Status(const bool IsFirst) const;

  //! returns the type of concavity in the connection
  ChFiDS_TypeOfConcavity GetTypeOfConcavity() const;

  void SetStatus(const ChFiDS_State S, const bool IsFirst);

  //! sets the type of concavity in the connection
  void SetTypeOfConcavity(const ChFiDS_TypeOfConcavity theType);

  //! returns if the set of edges starts (or end) on
  //! Tangency point.
  bool IsTangencyExtremity(const bool IsFirst) const;

  void SetTangencyExtremity(const bool IsTangency, const bool IsFirst);

  Standard_EXPORT double Absc(const TopoDS_Vertex& V) const;

  Standard_EXPORT TopoDS_Vertex FirstVertex() const;

  Standard_EXPORT TopoDS_Vertex LastVertex() const;

  Standard_EXPORT void SetFirstTgt(const double W);

  Standard_EXPORT void SetLastTgt(const double W);

  Standard_EXPORT bool HasFirstTgt() const;

  Standard_EXPORT bool HasLastTgt() const;

  //! set a parameter reference for the approx.
  Standard_EXPORT void SetReference(const double W);

  //! set a parameter reference for the approx, at the
  //! middle of edge I.
  Standard_EXPORT void SetReference(const int I);

  Standard_EXPORT int Index(const double W, const bool Forward = true) const;

  Standard_EXPORT int Index(const TopoDS_Edge& E) const;

  Standard_EXPORT void UnsetReference();

  Standard_EXPORT void SetErrorStatus(const ChFiDS_ErrorStatus state);

  Standard_EXPORT ChFiDS_ErrorStatus ErrorStatus() const;

  //! Return the mode of chamfers used
  Standard_EXPORT ChFiDS_ChamfMode Mode() const;

  //! Return tolesp parameter
  Standard_EXPORT double GetTolesp() const;

  DEFINE_STANDARD_RTTIEXT(ChFiDS_Spine, Standard_Transient)

protected:
  bool                                          splitdone;
  NCollection_List<occ::handle<ChFiDS_ElSpine>> elspines;
  NCollection_List<occ::handle<ChFiDS_ElSpine>> offset_elspines;
  ChFiDS_ChamfMode                              myMode;

private:
  Standard_EXPORT void Prepare(double& L, int& Index) const;

  BRepAdaptor_Curve                        myCurve;
  BRepAdaptor_Curve                        myOffsetCurve;
  int                                      indexofcurve;
  ChFiDS_TypeOfConcavity                   myTypeOfConcavity;
  ChFiDS_State                             firstState;
  ChFiDS_State                             lastState;
  NCollection_Sequence<TopoDS_Shape>       spine;
  NCollection_Sequence<TopoDS_Shape>       offsetspine;
  occ::handle<NCollection_HArray1<double>> abscissa;
  occ::handle<NCollection_HArray1<double>> offset_abscissa;
  double                                   tolesp;
  double                                   firstparam;
  double                                   lastparam;
  bool                                     firstprolon;
  bool                                     lastprolon;
  bool                                     firstistgt;
  bool                                     lastistgt;
  double                                   firsttgtpar;
  double                                   lasttgtpar;
  bool                                     hasfirsttgt;
  bool                                     haslasttgt;
  gp_Pnt                                   firstori;
  gp_Pnt                                   lastori;
  gp_Vec                                   firsttgt;
  gp_Vec                                   lasttgt;
  double                                   valref;
  bool                                     hasref;
  ChFiDS_ErrorStatus                       errorstate;
};

#include <ChFiDS_Spine.lxx>

#endif // _ChFiDS_Spine_HeaderFile
