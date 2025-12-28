// Created on: 1991-05-27
// Created by: Isabelle GRIGNON
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _IntPatch_WLine_HeaderFile
#define _IntPatch_WLine_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IntPatch_Point.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Real.hxx>
#include <IntPatch_PointLine.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntSurf_Situation.hxx>
#include <IntSurf_TypeTrans.hxx>

class IntPatch_Point;
class IntSurf_PntOn2S;
class gp_Pnt2d;
class gp_Pnt;

//! Definition of set of points as a result of the intersection
//! between 2 parametrised patches.
class IntPatch_WLine : public IntPatch_PointLine
{

public:
  //! Enumeration of ways of WLine creation.
  enum IntPatch_WLType
  {
    IntPatch_WLUnknown,
    IntPatch_WLImpImp,
    IntPatch_WLImpPrm,
    IntPatch_WLPrmPrm
  };

  //! Creates a WLine as an intersection when the
  //! transitions are In or Out.
  Standard_EXPORT IntPatch_WLine(const occ::handle<IntSurf_LineOn2S>& Line,
                                 const bool                           Tang,
                                 const IntSurf_TypeTrans              Trans1,
                                 const IntSurf_TypeTrans              Trans2);

  //! Creates a WLine as an intersection when the
  //! transitions are Touch.
  Standard_EXPORT IntPatch_WLine(const occ::handle<IntSurf_LineOn2S>& Line,
                                 const bool                           Tang,
                                 const IntSurf_Situation              Situ1,
                                 const IntSurf_Situation              Situ2);

  //! Creates a WLine as an intersection when the
  //! transitions are Undecided.
  Standard_EXPORT IntPatch_WLine(const occ::handle<IntSurf_LineOn2S>& Line, const bool Tang);

  //! Adds a vertex in the list. If theIsPrepend == TRUE the new
  //! vertex will be added before the first element of vertices sequence.
  //! Otherwise, to the end of the sequence
  void AddVertex(const IntPatch_Point& Pnt, const bool theIsPrepend = false) override;

  //! Set the Point of index <Index> in the LineOn2S
  Standard_EXPORT void SetPoint(const int Index, const IntPatch_Point& Pnt);

  //! Replaces the element of range Index in the list
  //! of points.
  //! The exception OutOfRange is raised when
  //! Index <= 0 or Index > NbVertex.
  void Replace(const int Index, const IntPatch_Point& Pnt);

  void SetFirstPoint(const int IndFirst);

  void SetLastPoint(const int IndLast);

  //! Returns the number of intersection points.
  int NbPnts() const override;

  //! Returns the intersection point of range Index.
  const IntSurf_PntOn2S& Point(const int Index) const override;

  //! Returns True if the line has a known First point.
  //! This point is given by the method FirstPoint().
  bool HasFirstPoint() const;

  //! Returns True if the line has a known Last point.
  //! This point is given by the method LastPoint().
  bool HasLastPoint() const;

  //! Returns the Point corresponding to the FirstPoint.
  const IntPatch_Point& FirstPoint() const;

  //! Returns the Point corresponding to the LastPoint.
  const IntPatch_Point& LastPoint() const;

  //! Returns the Point corresponding to the FirstPoint.
  //! Indfirst is the index of the first in the list
  //! of vertices.
  const IntPatch_Point& FirstPoint(int& Indfirst) const;

  //! Returns the Point corresponding to the LastPoint.
  //! Indlast is the index of the last in the list
  //! of vertices.
  const IntPatch_Point& LastPoint(int& Indlast) const;

  //! Returns number of vertices (IntPatch_Point) of the line
  int NbVertex() const override;

  //! Returns the vertex of range Index on the line.
  const IntPatch_Point& Vertex(const int Index) const override;

  //! Returns the vertex of range Index on the line.
  IntPatch_Point& ChangeVertex(const int Index) override;

  //! Set the parameters of all the vertex on the line.
  //! if a vertex is already in the line,
  //! its parameter is modified
  //! else a new point in the line is inserted.
  Standard_EXPORT void ComputeVertexParameters(const double Tol);

  //! Returns set of intersection points
  Standard_EXPORT occ::handle<IntSurf_LineOn2S> Curve() const override;

  //! Returns TRUE if theP is out of the box built from
  //! the points on 1st surface
  bool IsOutSurf1Box(const gp_Pnt2d& theP) const override { return curv->IsOutSurf1Box(theP); }

  //! Returns TRUE if theP is out of the box built from
  //! the points on 2nd surface
  bool IsOutSurf2Box(const gp_Pnt2d& theP) const override { return curv->IsOutSurf2Box(theP); }

  //! Returns TRUE if theP is out of the box built from 3D-points.
  bool IsOutBox(const gp_Pnt& theP) const override { return curv->IsOutBox(theP); }

  Standard_EXPORT void SetPeriod(const double pu1,
                                 const double pv1,
                                 const double pu2,
                                 const double pv2);

  Standard_EXPORT double U1Period() const;

  Standard_EXPORT double V1Period() const;

  Standard_EXPORT double U2Period() const;

  Standard_EXPORT double V2Period() const;

  Standard_EXPORT void SetArcOnS1(const occ::handle<Adaptor2d_Curve2d>& A);

  Standard_EXPORT bool HasArcOnS1() const;

  Standard_EXPORT const occ::handle<Adaptor2d_Curve2d>& GetArcOnS1() const;

  Standard_EXPORT void SetArcOnS2(const occ::handle<Adaptor2d_Curve2d>& A);

  Standard_EXPORT bool HasArcOnS2() const;

  Standard_EXPORT const occ::handle<Adaptor2d_Curve2d>& GetArcOnS2() const;

  //! Removes vertices from the line (i.e. cleans svtx member)
  void ClearVertexes() override;

  //! Removes single vertex from the line
  void RemoveVertex(const int theIndex) override;

  void InsertVertexBefore(const int theIndex, const IntPatch_Point& thePnt);

  //! if (theMode == 0) then prints the information about WLine
  //! if (theMode == 1) then prints the list of 3d-points
  //! if (theMode == 2) then prints the list of 2d-points on the 1st surface
  //! Otherwise, prints list of 2d-points on the 2nd surface
  Standard_EXPORT void Dump(const int theMode) const;

  //! Allows or forbids purging of existing WLine
  void EnablePurging(const bool theIsEnabled) { myIsPurgerAllowed = theIsEnabled; }

  //! Returns TRUE if purging is allowed or forbidden for existing WLine
  bool IsPurgingAllowed() { return myIsPurgerAllowed; }

  //! Returns the way of <*this> creation.
  IntPatch_WLType GetCreatingWay() const { return myCreationWay; }

  //! Sets the info about the way of <*this> creation.
  void SetCreatingWayInfo(IntPatch_WLType theAlgo) { myCreationWay = theAlgo; }

  DEFINE_STANDARD_RTTIEXT(IntPatch_WLine, IntPatch_PointLine)

private:
  occ::handle<IntSurf_LineOn2S>        curv;
  bool                                 fipt;
  bool                                 lapt;
  int                                  indf;
  int                                  indl;
  NCollection_Sequence<IntPatch_Point> svtx;
  double                               u1period;
  double                               v1period;
  double                               u2period;
  double                               v2period;
  bool                                 hasArcOnS1;
  occ::handle<Adaptor2d_Curve2d>       theArcOnS1;
  bool                                 hasArcOnS2;
  occ::handle<Adaptor2d_Curve2d>       theArcOnS2;
  bool                                 myIsPurgerAllowed;

  //! identifies the way of <*this> creation
  IntPatch_WLType myCreationWay;
};

#include <IntPatch_WLine.lxx>

#endif // _IntPatch_WLine_HeaderFile
