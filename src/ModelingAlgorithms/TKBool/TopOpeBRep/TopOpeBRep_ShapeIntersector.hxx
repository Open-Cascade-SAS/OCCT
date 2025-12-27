// Created on: 1993-05-07
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

#ifndef _TopOpeBRep_ShapeIntersector_HeaderFile
#define _TopOpeBRep_ShapeIntersector_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepTool_ShapeExplorer.hxx>
#include <TopOpeBRep_ShapeScanner.hxx>
#include <TopOpeBRep_FacesIntersector.hxx>
#include <TopOpeBRep_EdgesIntersector.hxx>
#include <TopOpeBRep_FaceEdgeIntersector.hxx>
#include <TopoDS_Face.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
class TopOpeBRepTool_HBoxTool;

//! Intersect two shapes.
//!
//! A GeomShape is a shape with a geometric domain, i.e.
//! a Face or an Edge.
//!
//! The purpose of the ShapeIntersector is to find
//! couples of intersecting GeomShape in two Shapes
//! (which can be any kind of topologies : Compound,
//! Solid, Shell, etc... )
//!
//! It is in charge of exploration of the shapes and
//! rejection. For this it is provided with two tools:
//!
//! - ShapeExplorer from TopOpeBRepTool.
//! - ShapeScanner from TopOpeBRep which implements bounding boxes.
//!
//! Let S1,S2 the shapes sent to InitIntersection(S1,S2) method:
//! - S1 is always SCANNED by a ShapeScanner from TopOpeBRep.
//! - S2 is always EXPLORED by a ShapeExplorer from TopOpeBRepTool.
class TopOpeBRep_ShapeIntersector
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRep_ShapeIntersector();

  //! Initialize the intersection of shapes S1,S2.
  Standard_EXPORT void InitIntersection(const TopoDS_Shape& S1, const TopoDS_Shape& S2);

  //! Initialize the intersection of shapes S1,S2.
  Standard_EXPORT void InitIntersection(const TopoDS_Shape& S1,
                                        const TopoDS_Shape& S2,
                                        const TopoDS_Face&  F1,
                                        const TopoDS_Face&  F2);

  //! return the shape <Index> ( = 1 or 2) given to
  //! InitIntersection().
  //! Index = 1 will return S1, Index = 2 will return S2.
  Standard_EXPORT const TopoDS_Shape& Shape(const int Index) const;

  //! returns True if there are more intersection
  //! between two the shapes.
  Standard_EXPORT bool MoreIntersection() const;

  //! search for the next intersection between the two shapes.
  Standard_EXPORT void NextIntersection();

  //! return the current intersection of two Faces.
  Standard_EXPORT TopOpeBRep_FacesIntersector& ChangeFacesIntersector();

  //! return the current intersection of two Edges.
  Standard_EXPORT TopOpeBRep_EdgesIntersector& ChangeEdgesIntersector();

  //! return the current intersection of a Face and an Edge.
  Standard_EXPORT TopOpeBRep_FaceEdgeIntersector& ChangeFaceEdgeIntersector();

  //! return geometric shape <Index> ( = 1 or 2 ) of
  //! current intersection.
  Standard_EXPORT const TopoDS_Shape& CurrentGeomShape(const int Index) const;

  //! return MAX of intersection tolerances with
  //! which FacesIntersector from TopOpeBRep was working.
  Standard_EXPORT void GetTolerances(double& tol1, double& tol2) const;

  Standard_EXPORT void DumpCurrent(const int K) const;

  Standard_EXPORT int Index(const int K) const;

  Standard_EXPORT void RejectedFaces(const TopoDS_Shape&   anObj,
                                     const TopoDS_Shape&   aReference,
                                     NCollection_List<TopoDS_Shape>& aListOfShape);

private:
  Standard_EXPORT void Reset();

  Standard_EXPORT void Init(const TopoDS_Shape& S1, const TopoDS_Shape& S2);

  Standard_EXPORT void SetIntersectionDone();

  Standard_EXPORT void InitFFIntersection();

  Standard_EXPORT void FindFFIntersection();

  Standard_EXPORT bool MoreFFCouple() const;

  Standard_EXPORT void NextFFCouple();

  Standard_EXPORT void InitEEFFIntersection();

  Standard_EXPORT void FindEEFFIntersection();

  Standard_EXPORT bool MoreEEFFCouple() const;

  Standard_EXPORT void NextEEFFCouple();

  Standard_EXPORT void InitFEIntersection();

  Standard_EXPORT void FindFEIntersection();

  Standard_EXPORT bool MoreFECouple() const;

  Standard_EXPORT void NextFECouple();

  Standard_EXPORT void InitEFIntersection();

  Standard_EXPORT void FindEFIntersection();

  Standard_EXPORT bool MoreEFCouple() const;

  Standard_EXPORT void NextEFCouple();

  Standard_EXPORT void InitEEIntersection();

  Standard_EXPORT void FindEEIntersection();

  Standard_EXPORT bool MoreEECouple() const;

  Standard_EXPORT void NextEECouple();

  TopoDS_Shape                    myShape1;
  TopoDS_Shape                    myShape2;
  occ::handle<TopOpeBRepTool_HBoxTool> myHBoxTool;
  TopOpeBRepTool_ShapeExplorer    myFaceExplorer;
  TopOpeBRep_ShapeScanner         myFaceScanner;
  TopOpeBRep_FacesIntersector     myFFIntersector;
  bool                myFFSameDomain;
  TopOpeBRepTool_ShapeExplorer    myEdgeExplorer;
  TopOpeBRep_ShapeScanner         myEdgeScanner;
  TopOpeBRep_EdgesIntersector     myEEIntersector;
  TopOpeBRep_FaceEdgeIntersector  myFEIntersector;
  TopoDS_Face                     myEEFace1;
  TopoDS_Face                     myEEFace2;
  bool                myIntersectionDone;
  double                   myTol1;
  double                   myTol2;
  bool                myFFDone;
  bool                myEEFFDone;
  bool                myEFDone;
  bool                myFEDone;
  bool                myEEDone;
  bool                myFFInit;
  bool                myEEFFInit;
  bool                myEFInit;
  bool                myFEInit;
  bool                myEEInit;
};

#endif // _TopOpeBRep_ShapeIntersector_HeaderFile
