// Created on: 1992-04-06
// Created by: Jacques GOUSSARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IntPatch_RLine_HeaderFile
#define _IntPatch_RLine_HeaderFile

#include <Standard.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <IntPatch_Point.hxx>
#include <NCollection_Sequence.hxx>
#include <IntPatch_PointLine.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntSurf_Situation.hxx>
#include <IntSurf_TypeTrans.hxx>

class IntPatch_Point;
class IntSurf_PntOn2S;

//! Implementation of an intersection line described by a
//! restriction line on one of the surfaces.
class IntPatch_RLine : public IntPatch_PointLine
{

public:
  //! Creates a restriction as an intersection line
  //! when the transitions are In or Out.
  Standard_EXPORT IntPatch_RLine(const bool              Tang,
                                 const IntSurf_TypeTrans Trans1,
                                 const IntSurf_TypeTrans Trans2);

  //! Creates a restriction as an intersection line
  //! when the transitions are Touch.
  Standard_EXPORT IntPatch_RLine(const bool              Tang,
                                 const IntSurf_Situation Situ1,
                                 const IntSurf_Situation Situ2);

  //! Creates a restriction as an intersection line
  //! when the transitions are Undecided.
  Standard_EXPORT IntPatch_RLine(const bool Tang);

  //! Adds a vertex in the list. If theIsPrepend == TRUE the new
  //! vertex will be added before the first element of vertices sequence.
  //! Otherwise, to the end of the sequence
  void AddVertex(const IntPatch_Point& Pnt, const bool theIsPrepend = false) override;

  //! Replaces the element of range Index in the list
  //! of points.
  void Replace(const int Index, const IntPatch_Point& Pnt);

  void SetFirstPoint(const int IndFirst);

  void SetLastPoint(const int IndLast);

  void Add(const occ::handle<IntSurf_LineOn2S>& L);

  //! Returns True if the intersection is on the domain of the
  //! first patch.
  //! Returns False if the intersection is on the domain of
  //! the second patch.
  bool IsArcOnS1() const;

  //! Returns True if the intersection is on the domain of the
  //! first patch.
  //! Returns False if the intersection is on the domain of
  //! the second patch.
  bool IsArcOnS2() const;

  Standard_EXPORT void SetArcOnS1(const occ::handle<Adaptor2d_Curve2d>& A);

  Standard_EXPORT void SetArcOnS2(const occ::handle<Adaptor2d_Curve2d>& A);

  Standard_EXPORT void SetParamOnS1(const double p1, const double p2);

  Standard_EXPORT void SetParamOnS2(double& p1, double& p2);

  //! Returns the concerned arc.
  const occ::handle<Adaptor2d_Curve2d>& ArcOnS1() const;

  //! Returns the concerned arc.
  const occ::handle<Adaptor2d_Curve2d>& ArcOnS2() const;

  Standard_EXPORT void ParamOnS1(double& p1, double& p2) const;

  Standard_EXPORT void ParamOnS2(double& p1, double& p2) const;

  //! Returns True if the line has a known First point.
  //! This point is given by the method FirstPoint().
  bool HasFirstPoint() const;

  //! Returns True if the line has a known Last point.
  //! This point is given by the method LastPoint().
  bool HasLastPoint() const;

  //! Returns the IntPoint corresponding to the FirstPoint.
  //! An exception is raised when HasFirstPoint returns False.
  const IntPatch_Point& FirstPoint() const;

  //! Returns the IntPoint corresponding to the LastPoint.
  //! An exception is raised when HasLastPoint returns False.
  const IntPatch_Point& LastPoint() const;

  //! Returns number of vertices (IntPatch_Point) of the line
  int NbVertex() const override;

  //! Returns the vertex of range Index on the line.
  const IntPatch_Point& Vertex(const int Index) const override;

  //! Returns the vertex of range Index on the line.
  IntPatch_Point& ChangeVertex(const int Index) override;

  //! Removes single vertex from the line
  void RemoveVertex(const int theIndex) override;

  bool HasPolygon() const;

  //! Returns the number of intersection points.
  int NbPnts() const override;

  //! Returns the intersection point of range Index.
  const IntSurf_PntOn2S& Point(const int Index) const override;

  //! Set the Point of index <Index> in the LineOn2S
  Standard_EXPORT void SetPoint(const int Index, const IntPatch_Point& Pnt);

  //! Set the parameters of all the vertex on the line.
  //! if a vertex is already in the line,
  //! its parameter is modified
  //! else a new point in the line is inserted.
  Standard_EXPORT void ComputeVertexParameters(const double Tol);

  //! Returns set of intersection points
  occ::handle<IntSurf_LineOn2S> Curve() const override;

  //! Returns TRUE if theP is out of the box built from
  //! the points on 1st surface
  bool IsOutSurf1Box(const gp_Pnt2d& theP) const override { return curv->IsOutSurf1Box(theP); }

  //! Returns TRUE if theP is out of the box built from
  //! the points on 2nd surface
  bool IsOutSurf2Box(const gp_Pnt2d& theP) const override { return curv->IsOutSurf2Box(theP); }

  //! Returns TRUE if theP is out of the box built from 3D-points.
  bool IsOutBox(const gp_Pnt& theP) const override { return curv->IsOutBox(theP); }

  //! Removes vertices from the line (i.e. cleans svtx member)
  void ClearVertexes() override { svtx.Clear(); }

  void SetCurve(const occ::handle<IntSurf_LineOn2S>& theNewCurve) { curv = theNewCurve; }

  //! if (theMode == 0) then prints the information about WLine
  //! if (theMode == 1) then prints the list of 3d-points
  //! if (theMode == 2) then prints the list of 2d-points on the 1st surface
  //! Otherwise, prints list of 2d-points on the 2nd surface
  Standard_EXPORT void Dump(const int theMode) const;

  DEFINE_STANDARD_RTTIEXT(IntPatch_RLine, IntPatch_PointLine)

private:
  occ::handle<Adaptor2d_Curve2d>       theArcOnS1;
  occ::handle<Adaptor2d_Curve2d>       theArcOnS2;
  bool                                 onS1;
  bool                                 onS2;
  double                               ParamInf1;
  double                               ParamSup1;
  double                               ParamInf2;
  double                               ParamSup2;
  occ::handle<IntSurf_LineOn2S>        curv;
  bool                                 fipt;
  bool                                 lapt;
  int                                  indf;
  int                                  indl;
  NCollection_Sequence<IntPatch_Point> svtx;
};

#include <IntPatch_RLine.lxx>

#endif // _IntPatch_RLine_HeaderFile
