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

#ifndef _IntPatch_GLine_HeaderFile
#define _IntPatch_GLine_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Ax2.hxx>
#include <Standard_Integer.hxx>
#include <IntPatch_Point.hxx>
#include <NCollection_Sequence.hxx>
#include <IntPatch_Line.hxx>
#include <IntSurf_TypeTrans.hxx>
#include <IntSurf_Situation.hxx>
#include <gp_Circ.hxx>
class gp_Lin;
class gp_Circ;
class gp_Elips;
class gp_Parab;
class gp_Hypr;
class IntPatch_Point;

//! Implementation of an intersection line represented
//! by a conic.
class IntPatch_GLine : public IntPatch_Line
{

public:
  //! Creates a Line as intersection line
  //! when the transitions are In or Out.
  Standard_EXPORT IntPatch_GLine(const gp_Lin&           L,
                                 const bool  Tang,
                                 const IntSurf_TypeTrans Trans1,
                                 const IntSurf_TypeTrans Trans2);

  //! Creates a Line as intersection line
  //! when the transitions are Touch.
  Standard_EXPORT IntPatch_GLine(const gp_Lin&           L,
                                 const bool  Tang,
                                 const IntSurf_Situation Situ1,
                                 const IntSurf_Situation Situ2);

  //! Creates a Line as intersection line
  //! when the transitions are Undecided.
  Standard_EXPORT IntPatch_GLine(const gp_Lin& L, const bool Tang);

  //! Creates a circle as intersection line
  //! when the transitions are In or Out.
  Standard_EXPORT IntPatch_GLine(const gp_Circ&          C,
                                 const bool  Tang,
                                 const IntSurf_TypeTrans Trans1,
                                 const IntSurf_TypeTrans Trans2);

  //! Creates a circle as intersection line
  //! when the transitions are Touch.
  Standard_EXPORT IntPatch_GLine(const gp_Circ&          C,
                                 const bool  Tang,
                                 const IntSurf_Situation Situ1,
                                 const IntSurf_Situation Situ2);

  //! Creates a circle as intersection line
  //! when the transitions are Undecided.
  Standard_EXPORT IntPatch_GLine(const gp_Circ& C, const bool Tang);

  //! Creates an ellipse as intersection line
  //! when the transitions are In or Out.
  Standard_EXPORT IntPatch_GLine(const gp_Elips&         E,
                                 const bool  Tang,
                                 const IntSurf_TypeTrans Trans1,
                                 const IntSurf_TypeTrans Trans2);

  //! Creates an ellispe as intersection line
  //! when the transitions are Touch.
  Standard_EXPORT IntPatch_GLine(const gp_Elips&         E,
                                 const bool  Tang,
                                 const IntSurf_Situation Situ1,
                                 const IntSurf_Situation Situ2);

  //! Creates an ellipse as intersection line
  //! when the transitions are Undecided.
  Standard_EXPORT IntPatch_GLine(const gp_Elips& E, const bool Tang);

  //! Creates a parabola as intersection line
  //! when the transitions are In or Out.
  Standard_EXPORT IntPatch_GLine(const gp_Parab&         P,
                                 const bool  Tang,
                                 const IntSurf_TypeTrans Trans1,
                                 const IntSurf_TypeTrans Trans2);

  //! Creates a parabola as intersection line
  //! when the transitions are Touch.
  Standard_EXPORT IntPatch_GLine(const gp_Parab&         P,
                                 const bool  Tang,
                                 const IntSurf_Situation Situ1,
                                 const IntSurf_Situation Situ2);

  //! Creates a parabola as intersection line
  //! when the transitions are Undecided.
  Standard_EXPORT IntPatch_GLine(const gp_Parab& P, const bool Tang);

  //! Creates an hyperbola as intersection line
  //! when the transitions are In or Out.
  Standard_EXPORT IntPatch_GLine(const gp_Hypr&          H,
                                 const bool  Tang,
                                 const IntSurf_TypeTrans Trans1,
                                 const IntSurf_TypeTrans Trans2);

  //! Creates an hyperbola as intersection line
  //! when the transitions are Touch.
  Standard_EXPORT IntPatch_GLine(const gp_Hypr&          H,
                                 const bool  Tang,
                                 const IntSurf_Situation Situ1,
                                 const IntSurf_Situation Situ2);

  //! Creates an hyperbola as intersection line
  //! when the transitions are Undecided.
  Standard_EXPORT IntPatch_GLine(const gp_Hypr& H, const bool Tang);

  //! To add a vertex in the list.
  Standard_EXPORT void AddVertex(const IntPatch_Point& Pnt);

  //! To replace the element of range Index in the list
  //! of points.
  Standard_EXPORT void Replace(const int Index, const IntPatch_Point& Pnt);

  void SetFirstPoint(const int IndFirst);

  void SetLastPoint(const int IndLast);

  //! Returns the Lin from gp corresponding to the intersection
  //! when ArcType returns IntPatch_Line.
  gp_Lin Line() const;

  //! Returns the Circ from gp corresponding to the intersection
  //! when ArcType returns IntPatch_Circle.
  gp_Circ Circle() const;

  //! Returns the Elips from gp corresponding to the intersection
  //! when ArcType returns IntPatch_Ellipse.
  gp_Elips Ellipse() const;

  //! Returns the Parab from gp corresponding to the intersection
  //! when ArcType returns IntPatch_Parabola.
  gp_Parab Parabola() const;

  //! Returns the Hypr from gp corresponding to the intersection
  //! when ArcType returns IntPatch_Hyperbola.
  gp_Hypr Hyperbola() const;

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

  int NbVertex() const;

  //! Returns the vertex of range Index on the line.
  const IntPatch_Point& Vertex(const int Index) const;

  //! Set the parameters of all the vertex on the line.
  //! if a vertex is already in the line,
  //! its parameter is modified
  //! else a new point in the line is inserted.
  Standard_EXPORT void ComputeVertexParameters(const double Tol);

  DEFINE_STANDARD_RTTIEXT(IntPatch_GLine, IntPatch_Line)

private:
  gp_Ax2                   pos;
  double            par1;
  double            par2;
  bool         fipt;
  bool         lapt;
  int         indf;
  int         indl;
  NCollection_Sequence<IntPatch_Point> svtx;
};

#include <IntPatch_GLine.lxx>

#endif // _IntPatch_GLine_HeaderFile
