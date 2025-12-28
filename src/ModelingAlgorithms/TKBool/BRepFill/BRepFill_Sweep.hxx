// Created on: 1997-11-21
// Created by: Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _BRepFill_Sweep_HeaderFile
#define _BRepFill_Sweep_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomFill_ApproxStyle.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <BRepFill_TransitionStyle.hxx>
class BRepFill_LocationLaw;
class BRepFill_SectionLaw;
class TopoDS_Edge;

//! Topological Sweep Algorithm
//! Computes an Sweep shell using a generating
//! wire, an SectionLaw and an LocationLaw.
class BRepFill_Sweep
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepFill_Sweep(const occ::handle<BRepFill_SectionLaw>&  Section,
                                 const occ::handle<BRepFill_LocationLaw>& Location,
                                 const bool                               WithKPart);

  Standard_EXPORT void SetBounds(const TopoDS_Wire& FirstShape, const TopoDS_Wire& LastShape);

  //! Set Approximation Tolerance
  //! Tol3d : Tolerance to surface approximation
  //! Tol2d : Tolerance used to perform curve approximation
  //! Normally the 2d curve are approximated with a
  //! tolerance given by the resolution on support surfaces,
  //! but if this tolerance is too large Tol2d is used.
  //! TolAngular : Tolerance (in radian) to control the angle
  //! between tangents on the section law and
  //! tangent of iso-v on approximated surface
  Standard_EXPORT void SetTolerance(const double Tol3d,
                                    const double BoundTol   = 1.0,
                                    const double Tol2d      = 1.0e-5,
                                    const double TolAngular = 1.0e-2);

  //! Tolerance  To controle Corner management.
  //!
  //! If the discontinuity is lesser than <AngleMin> in radian The
  //! Transition Performed will be always "Modified"
  Standard_EXPORT void SetAngularControl(const double AngleMin = 0.01, const double AngleMax = 6.0);

  //! Set the flag that indicates attempt to approximate
  //! a C1-continuous surface if a swept surface proved
  //! to be C0.
  Standard_EXPORT void SetForceApproxC1(const bool ForceApproxC1);

  //! Build the Sweep Surface
  //! Transition define Transition strategy
  //! Approx define Approximation Strategy
  //! - GeomFill_Section : The composed Function Location X Section
  //! is directly approximated.
  //! - GeomFill_Location : The location law is approximated, and the
  //! SweepSurface builds an algebraic composition
  //! of approximated location law and section law
  //! This option is Ok, if Section.Surface() methode
  //! is effective.
  //! Continuity : The continuity in v waiting on the surface
  //! Degmax     : The maximum degree in v required on the surface
  //! Segmax     : The maximum number of span in v required on
  //! the surface.
  Standard_EXPORT void Build(NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& ReversedEdges,
                             NCollection_DataMap<TopoDS_Shape,
                                                 occ::handle<NCollection_HArray2<TopoDS_Shape>>,
                                                 TopTools_ShapeMapHasher>&           Tapes,
                             NCollection_DataMap<TopoDS_Shape,
                                                 occ::handle<NCollection_HArray2<TopoDS_Shape>>,
                                                 TopTools_ShapeMapHasher>&           Rails,
                             const BRepFill_TransitionStyle Transition = BRepFill_Modified,
                             const GeomAbs_Shape            Continuity = GeomAbs_C2,
                             const GeomFill_ApproxStyle     Approx     = GeomFill_Location,
                             const int                      Degmax     = 11,
                             const int                      Segmax     = 30);

  //! Say if the Shape is Build.
  Standard_EXPORT bool IsDone() const;

  //! Returns the Sweeping Shape
  Standard_EXPORT TopoDS_Shape Shape() const;

  //! Get the Approximation error.
  Standard_EXPORT double ErrorOnSurface() const;

  Standard_EXPORT occ::handle<NCollection_HArray2<TopoDS_Shape>> SubShape() const;

  Standard_EXPORT occ::handle<NCollection_HArray2<TopoDS_Shape>> InterFaces() const;

  Standard_EXPORT occ::handle<NCollection_HArray2<TopoDS_Shape>> Sections() const;

  //! Returns the Tape corresponding to Index-th edge of section
  Standard_EXPORT TopoDS_Shape Tape(const int Index) const;

protected:
  Standard_EXPORT bool CorrectApproxParameters();

  Standard_EXPORT bool BuildWire(const BRepFill_TransitionStyle Transition);

  Standard_EXPORT bool BuildShell(
    const BRepFill_TransitionStyle                          Transition,
    const int                                               Vf,
    const int                                               Vl,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& ReversedEdges,
    NCollection_DataMap<TopoDS_Shape,
                        occ::handle<NCollection_HArray2<TopoDS_Shape>>,
                        TopTools_ShapeMapHasher>&           Tapes,
    NCollection_DataMap<TopoDS_Shape,
                        occ::handle<NCollection_HArray2<TopoDS_Shape>>,
                        TopTools_ShapeMapHasher>&           Rails,
    const double                                            ExtendFirst = 0.0,
    const double                                            ExtendLast  = 0.0);

  Standard_EXPORT bool PerformCorner(const int                      Index,
                                     const BRepFill_TransitionStyle Transition,
                                     const occ::handle<NCollection_HArray2<TopoDS_Shape>>& Bounds);

  Standard_EXPORT double EvalExtrapol(const int                      Index,
                                      const BRepFill_TransitionStyle Transition) const;

  Standard_EXPORT bool MergeVertex(const TopoDS_Shape& V1, TopoDS_Shape& V2) const;

  Standard_EXPORT void UpdateVertex(const int     Ipath,
                                    const int     Isec,
                                    const double  Error,
                                    const double  Param,
                                    TopoDS_Shape& V) const;

  Standard_EXPORT void RebuildTopOrBottomEdge(
    const TopoDS_Edge&                                      aNewEdge,
    TopoDS_Edge&                                            anEdge,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& ReversedEdges) const;

private:
  bool                                                                     isDone;
  bool                                                                     KPart;
  double                                                                   myTol3d;
  double                                                                   myBoundTol;
  double                                                                   myTol2d;
  double                                                                   myTolAngular;
  double                                                                   myAngMin;
  double                                                                   myAngMax;
  GeomFill_ApproxStyle                                                     myApproxStyle;
  GeomAbs_Shape                                                            myContinuity;
  int                                                                      myDegmax;
  int                                                                      mySegmax;
  bool                                                                     myForceApproxC1;
  TopoDS_Shape                                                             myShape;
  occ::handle<BRepFill_LocationLaw>                                        myLoc;
  occ::handle<BRepFill_SectionLaw>                                         mySec;
  occ::handle<NCollection_HArray2<TopoDS_Shape>>                           myUEdges;
  occ::handle<NCollection_HArray2<TopoDS_Shape>>                           myVEdges;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> myVEdgesModified;
  occ::handle<NCollection_HArray2<TopoDS_Shape>>                           myFaces;
  NCollection_List<TopoDS_Shape>                                           myAuxShape;
  occ::handle<NCollection_HArray1<TopoDS_Shape>>                           myTapes;
  double                                                                   Error;
  TopoDS_Wire                                                              FirstShape;
  TopoDS_Wire                                                              LastShape;
};

#endif // _BRepFill_Sweep_HeaderFile
