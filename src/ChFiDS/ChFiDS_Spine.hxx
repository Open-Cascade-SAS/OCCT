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
#include <ChFiDS_ListOfHElSpine.hxx>
#include <ChFiDS_State.hxx>
#include <ChFiDS_TypeOfConcavity.hxx>
#include <ChFiDS_ElSpine.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopTools_SequenceOfShape.hxx>

class TopoDS_Edge;
class gp_Lin;
class gp_Circ;
class TopoDS_Vertex;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

class ChFiDS_Spine;
DEFINE_STANDARD_HANDLE(ChFiDS_Spine, Standard_Transient)

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
//! IMPORTANT NOTE  :    the guideline
//! represented in this way is not C2, although the path
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
  
  Standard_EXPORT ChFiDS_Spine(const Standard_Real Tol);
  
  //! store edges composing the guideline
    void SetEdges (const TopoDS_Edge& E);
  
  //! store offset edges composing the offset guideline
    void SetOffsetEdges (const TopoDS_Edge& E);
  
  //! store the edge at the first position before all others
    void PutInFirst (const TopoDS_Edge& E);
  
  //! store the offset edge at the first position before all others
    void PutInFirstOffset (const TopoDS_Edge& E);
  
    Standard_Integer NbEdges() const;
  
    const TopoDS_Edge& Edges (const Standard_Integer I) const;
  
    const TopoDS_Edge& OffsetEdges (const Standard_Integer I) const;
  
  //! stores if the start of a set of edges starts on a
  //! section of free border or forms  a closed contour
    void SetFirstStatus (const ChFiDS_State S);
  
  //! stores if the end of a set of edges starts on a
  //! section of free border or forms  a closed contour
    void SetLastStatus (const ChFiDS_State S);
  
  Standard_EXPORT virtual void AppendElSpine (const Handle(ChFiDS_ElSpine)& Els);
  
  Standard_EXPORT virtual void AppendOffsetElSpine (const Handle(ChFiDS_ElSpine)& Els);
  
  Standard_EXPORT Handle(ChFiDS_ElSpine) ElSpine (const Standard_Integer IE) const;
  
  Standard_EXPORT Handle(ChFiDS_ElSpine) ElSpine (const TopoDS_Edge& E) const;
  
  Standard_EXPORT Handle(ChFiDS_ElSpine) ElSpine (const Standard_Real W) const;
  
  Standard_EXPORT ChFiDS_ListOfHElSpine& ChangeElSpines();
  
  Standard_EXPORT ChFiDS_ListOfHElSpine& ChangeOffsetElSpines();
  
  Standard_EXPORT virtual void Reset (const Standard_Boolean AllData = Standard_False);
  
  Standard_EXPORT Standard_Boolean SplitDone() const;
  
  Standard_EXPORT void SplitDone (const Standard_Boolean B);
  
  //! prepare the guideline depending on the edges that
  //! are elementary arks (take parameters from
  //! a single curvilinear abscissa); to be able to call
  //! methods on the geometry (first,last,value,d1,d2)
  //! it is necessary to start with preparation otherwise an
  //! exception will be raised
  Standard_EXPORT void Load();
  
  Standard_EXPORT Standard_Real Resolution (const Standard_Real R3d) const;
  
  Standard_EXPORT Standard_Boolean IsClosed() const;
  
  Standard_EXPORT Standard_Real FirstParameter() const;
  
  Standard_EXPORT Standard_Real LastParameter() const;
  
  Standard_EXPORT void SetFirstParameter (const Standard_Real Par);
  
  Standard_EXPORT void SetLastParameter (const Standard_Real Par);
  
  //! gives the total length of all arcs before the
  //! number IndexSp
  Standard_EXPORT Standard_Real FirstParameter (const Standard_Integer IndexSpine) const;
  
  //! gives the total length till the ark with number
  //! IndexSpine (inclus)
  Standard_EXPORT Standard_Real LastParameter (const Standard_Integer IndexSpine) const;
  
  //! gives the length of ark with number IndexSp
  Standard_EXPORT Standard_Real Length (const Standard_Integer IndexSpine) const;
  
  Standard_EXPORT Standard_Boolean IsPeriodic() const;
  
  Standard_EXPORT Standard_Real Period() const;
  
  Standard_EXPORT Standard_Real Absc (const Standard_Real U);
  
  Standard_EXPORT Standard_Real Absc (const Standard_Real U, const Standard_Integer I);
  
  Standard_EXPORT void Parameter (const Standard_Real AbsC, Standard_Real& U, const Standard_Boolean Oriented = Standard_True);
  
  Standard_EXPORT void Parameter (const Standard_Integer Index, const Standard_Real AbsC, Standard_Real& U, const Standard_Boolean Oriented = Standard_True);
  
  Standard_EXPORT gp_Pnt Value (const Standard_Real AbsC);
  
  Standard_EXPORT void D0 (const Standard_Real AbsC, gp_Pnt& P);
  
  Standard_EXPORT void D1 (const Standard_Real AbsC, gp_Pnt& P, gp_Vec& V1);
  
  Standard_EXPORT void D2 (const Standard_Real AbsC, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2);
  
  Standard_EXPORT void SetCurrent (const Standard_Integer Index);
  
  //! sets the current curve and returns it
  Standard_EXPORT const BRepAdaptor_Curve& CurrentElementarySpine (const Standard_Integer Index);
  
    Standard_Integer CurrentIndexOfElementarySpine() const;
  
  Standard_EXPORT GeomAbs_CurveType GetType() const;
  
  Standard_EXPORT gp_Lin Line() const;
  
  Standard_EXPORT gp_Circ Circle() const;
  
  //! returns if the set of edges starts on a free boundary
  //! or if the first vertex is a breakpoint or if the set is
  //! closed
    ChFiDS_State FirstStatus() const;
  
  //! returns the state at the end of the set
    ChFiDS_State LastStatus() const;
  
    ChFiDS_State Status (const Standard_Boolean IsFirst) const;
  
  //! returns the type of concavity in the connection
    ChFiDS_TypeOfConcavity GetTypeOfConcavity() const;
  
    void SetStatus (const ChFiDS_State S, const Standard_Boolean IsFirst);
  
  //! sets the type of concavity in the connection
    void SetTypeOfConcavity (const ChFiDS_TypeOfConcavity theType);
  
  //! returns   if the  set  of  edges starts (or   end) on
  //! Tangency point.
    Standard_Boolean IsTangencyExtremity (const Standard_Boolean IsFirst) const;
  
    void SetTangencyExtremity (const Standard_Boolean IsTangency, const Standard_Boolean IsFirst);
  
  Standard_EXPORT Standard_Real Absc (const TopoDS_Vertex& V) const;
  
  Standard_EXPORT TopoDS_Vertex FirstVertex() const;
  
  Standard_EXPORT TopoDS_Vertex LastVertex() const;
  
  Standard_EXPORT void SetFirstTgt (const Standard_Real W);
  
  Standard_EXPORT void SetLastTgt (const Standard_Real W);
  
  Standard_EXPORT Standard_Boolean HasFirstTgt() const;
  
  Standard_EXPORT Standard_Boolean HasLastTgt() const;
  
  //! set a parameter reference for the approx.
  Standard_EXPORT void SetReference (const Standard_Real W);
  
  //! set  a  parameter  reference  for  the approx,  at the
  //! middle  of edge I.
  Standard_EXPORT void SetReference (const Standard_Integer I);
  
  Standard_EXPORT Standard_Integer Index (const Standard_Real W, const Standard_Boolean Forward = Standard_True) const;
  
  Standard_EXPORT Standard_Integer Index (const TopoDS_Edge& E) const;
  
  Standard_EXPORT void UnsetReference();
  
  Standard_EXPORT void SetErrorStatus (const ChFiDS_ErrorStatus state);
  
  Standard_EXPORT ChFiDS_ErrorStatus ErrorStatus() const;

  //! Return the mode of chamfers used
  Standard_EXPORT ChFiDS_ChamfMode Mode() const;

  //! Return tolesp parameter
  Standard_EXPORT Standard_Real GetTolesp() const;



  DEFINE_STANDARD_RTTIEXT(ChFiDS_Spine,Standard_Transient)

protected:


  Standard_Boolean splitdone;
  ChFiDS_ListOfHElSpine elspines;
  ChFiDS_ListOfHElSpine offset_elspines;
  ChFiDS_ChamfMode   myMode;


private:

  
  Standard_EXPORT void Prepare (Standard_Real& L, Standard_Integer& Index) const;

  BRepAdaptor_Curve myCurve;
  BRepAdaptor_Curve myOffsetCurve;
  Standard_Integer indexofcurve;
  ChFiDS_TypeOfConcavity myTypeOfConcavity;
  ChFiDS_State firstState;
  ChFiDS_State lastState;
  TopTools_SequenceOfShape spine;
  TopTools_SequenceOfShape offsetspine;
  Handle(TColStd_HArray1OfReal) abscissa;
  Handle(TColStd_HArray1OfReal) offset_abscissa;
  Standard_Real tolesp;
  Standard_Real firstparam;
  Standard_Real lastparam;
  Standard_Boolean firstprolon;
  Standard_Boolean lastprolon;
  Standard_Boolean firstistgt;
  Standard_Boolean lastistgt;
  Standard_Real firsttgtpar;
  Standard_Real lasttgtpar;
  Standard_Boolean hasfirsttgt;
  Standard_Boolean haslasttgt;
  gp_Pnt firstori;
  gp_Pnt lastori;
  gp_Vec firsttgt;
  gp_Vec lasttgt;
  Standard_Real valref;
  Standard_Boolean hasref;
  ChFiDS_ErrorStatus errorstate;


};


#include <ChFiDS_Spine.lxx>





#endif // _ChFiDS_Spine_HeaderFile
