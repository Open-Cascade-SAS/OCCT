// Created on: 1993-01-28
// Created by: Laurent BUCHARD
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

#ifndef _IntPatch_PrmPrmIntersection_HeaderFile
#define _IntPatch_PrmPrmIntersection_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <IntPatch_Line.hxx>
#include <NCollection_Sequence.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <NCollection_List.hxx>

class Adaptor3d_TopolTool;
class IntPatch_Polyhedron;
class IntPatch_PrmPrmIntersection_T3Bits;
class IntSurf_LineOn2S;

//! Implementation of the Intersection between two bi-parametrised surfaces.
//!
//! To avoid multiple constructions of the approximated
//! polyhedron of the surfaces, the algorithm can be
//! called with the two surfaces and their associated polyhedron.
class IntPatch_PrmPrmIntersection
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty Constructor
  Standard_EXPORT IntPatch_PrmPrmIntersection();

  //! Performs the intersection between <Caro1> and
  //! <Caro2>. Associated Polyhedrons <Polyhedron1>
  //! and <Polyhedron2> are given.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Surface>&   Caro1,
                               const IntPatch_Polyhedron&         Polyhedron1,
                               const occ::handle<Adaptor3d_TopolTool>& Domain1,
                               const occ::handle<Adaptor3d_Surface>&   Caro2,
                               const IntPatch_Polyhedron&         Polyhedron2,
                               const occ::handle<Adaptor3d_TopolTool>& Domain2,
                               const double                TolTangency,
                               const double                Epsilon,
                               const double                Deflection,
                               const double                Increment);

  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Surface>&   Caro1,
                               const IntPatch_Polyhedron&         Polyhedron1,
                               const occ::handle<Adaptor3d_TopolTool>& Domain1,
                               const double                TolTangency,
                               const double                Epsilon,
                               const double                Deflection,
                               const double                Increment);

  //! Performs the intersection between <Caro1> and
  //! <Caro2>. The method computes the polyhedron on
  //! each surface.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Surface>&   Caro1,
                               const occ::handle<Adaptor3d_TopolTool>& Domain1,
                               const occ::handle<Adaptor3d_Surface>&   Caro2,
                               const occ::handle<Adaptor3d_TopolTool>& Domain2,
                               const double                TolTangency,
                               const double                Epsilon,
                               const double                Deflection,
                               const double                Increment,
                               const bool             ClearFlag = true);

  //! Performs the intersection between <Caro1> and
  //! <Caro2>. The method computes the polyhedron on
  //! each surface.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Surface>&   Caro1,
                               const occ::handle<Adaptor3d_TopolTool>& Domain1,
                               const occ::handle<Adaptor3d_Surface>&   Caro2,
                               const occ::handle<Adaptor3d_TopolTool>& Domain2,
                               const double                TolTangency,
                               const double                Epsilon,
                               const double                Deflection,
                               const double                Increment,
                               NCollection_List<IntSurf_PntOn2S>&             ListOfPnts);

  //! Performs the intersection between <Caro1> and
  //! <Caro2>. The method computes the polyhedron on
  //! each surface.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Surface>&   Caro1,
                               const occ::handle<Adaptor3d_TopolTool>& Domain1,
                               const occ::handle<Adaptor3d_Surface>&   Caro2,
                               const occ::handle<Adaptor3d_TopolTool>& Domain2,
                               const double                U1,
                               const double                V1,
                               const double                U2,
                               const double                V2,
                               const double                TolTangency,
                               const double                Epsilon,
                               const double                Deflection,
                               const double                Increment);

  //! Performs the intersection between <Caro1> and
  //! <Caro2>. The method computes the polyhedron on
  //! each surface.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Surface>&   Caro1,
                               const occ::handle<Adaptor3d_TopolTool>& Domain1,
                               const double                TolTangency,
                               const double                Epsilon,
                               const double                Deflection,
                               const double                Increment);

  //! Performs the intersection between <Caro1> and
  //! <Caro2>.
  //!
  //! The polyhedron which approximates <Caro2>,
  //! <Polyhedron2> is given. The other one is
  //! computed.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Surface>&   Caro1,
                               const occ::handle<Adaptor3d_TopolTool>& Domain1,
                               const occ::handle<Adaptor3d_Surface>&   Caro2,
                               const IntPatch_Polyhedron&         Polyhedron2,
                               const occ::handle<Adaptor3d_TopolTool>& Domain2,
                               const double                TolTangency,
                               const double                Epsilon,
                               const double                Deflection,
                               const double                Increment);

  //! Performs the intersection between <Caro1> and
  //! <Caro2>.
  //!
  //! The polyhedron which approximates <Caro1>,
  //! <Polyhedron1> is given. The other one is
  //! computed.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Surface>&   Caro1,
                               const IntPatch_Polyhedron&         Polyhedron1,
                               const occ::handle<Adaptor3d_TopolTool>& Domain1,
                               const occ::handle<Adaptor3d_Surface>&   Caro2,
                               const occ::handle<Adaptor3d_TopolTool>& Domain2,
                               const double                TolTangency,
                               const double                Epsilon,
                               const double                Deflection,
                               const double                Increment);

  //! Returns true if the calculus was successful.
  bool IsDone() const;

  //! Returns true if the is no intersection.
  bool IsEmpty() const;

  //! Returns the number of intersection lines.
  int NbLines() const;

  //! Returns the line of range Index.
  //! An exception is raised if Index<=0 or Index>NbLine.
  const occ::handle<IntPatch_Line>& Line(const int Index) const;

  //! Computes about <NbPoints> Intersection Points on
  //! the Line <IndexLine> between the Points of Index
  //! <LowPoint> and <HighPoint>.
  //!
  //! All the points of the line of index <IndexLine>
  //! with an index between <LowPoint> and <HighPoint>
  //! are in the returned line. New Points are inserted
  //! between existing points if those points are not
  //! too closed.
  //!
  //! An exception is raised if Index<=0 or Index>NbLine.
  //! or if IsDone returns False
  Standard_EXPORT occ::handle<IntPatch_Line> NewLine(const occ::handle<Adaptor3d_Surface>& Caro1,
                                                const occ::handle<Adaptor3d_Surface>& Caro2,
                                                const int           IndexLine,
                                                const int           LowPoint,
                                                const int           HighPoint,
                                                const int           NbPoints) const;

  int GrilleInteger(const int ix,
                                 const int iy,
                                 const int iz) const;

  void IntegerGrille(const int t,
                     int&      ix,
                     int&      iy,
                     int&      iz) const;

  int DansGrille(const int t) const;

  int NbPointsGrille() const;

  Standard_EXPORT void RemplitLin(const int              x1,
                                  const int              y1,
                                  const int              z1,
                                  const int              x2,
                                  const int              y2,
                                  const int              z2,
                                  IntPatch_PrmPrmIntersection_T3Bits& Map) const;

  Standard_EXPORT void RemplitTri(const int              x1,
                                  const int              y1,
                                  const int              z1,
                                  const int              x2,
                                  const int              y2,
                                  const int              z2,
                                  const int              x3,
                                  const int              y3,
                                  const int              z3,
                                  IntPatch_PrmPrmIntersection_T3Bits& Map) const;

  Standard_EXPORT void Remplit(const int              a,
                               const int              b,
                               const int              c,
                               IntPatch_PrmPrmIntersection_T3Bits& Map) const;

  int CodeReject(const double x1,
                              const double y1,
                              const double z1,
                              const double x2,
                              const double y2,
                              const double z2,
                              const double x3,
                              const double y3,
                              const double z3) const;

  Standard_EXPORT void PointDepart(occ::handle<IntSurf_LineOn2S>&        LineOn2S,
                                   const occ::handle<Adaptor3d_Surface>& S1,
                                   const int           SU1,
                                   const int           SV1,
                                   const occ::handle<Adaptor3d_Surface>& S2,
                                   const int           SU2,
                                   const int           SV2) const;

private:
  bool        done;
  bool        empt;
  NCollection_Sequence<occ::handle<IntPatch_Line>> SLin;
};

#include <IntPatch_PrmPrmIntersection.lxx>

#endif // _IntPatch_PrmPrmIntersection_HeaderFile
