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
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <IntPatch_SequenceOfPoint.hxx>
#include <Bnd_Box2d.hxx>
#include <Bnd_Box.hxx>
#include <Standard_Real.hxx>
#include <IntPatch_PointLine.hxx>
#include <IntSurf_TypeTrans.hxx>
#include <IntSurf_Situation.hxx>
class IntSurf_LineOn2S;
class Adaptor2d_HCurve2d;
class Standard_OutOfRange;
class Standard_DomainError;
class IntPatch_Point;
class IntSurf_PntOn2S;
class gp_Pnt2d;
class gp_Pnt;


class IntPatch_WLine;
DEFINE_STANDARD_HANDLE(IntPatch_WLine, IntPatch_PointLine)

//! Definition of set of points as a result of the intersection
//! between 2 parametrised patches.
class IntPatch_WLine : public IntPatch_PointLine
{

public:

  
  //! Creates a WLine as an intersection when the
  //! transitions are In or Out.
  Standard_EXPORT IntPatch_WLine(const Handle(IntSurf_LineOn2S)& Line, const Standard_Boolean Tang, const IntSurf_TypeTrans Trans1, const IntSurf_TypeTrans Trans2);
  
  //! Creates a WLine as an intersection when the
  //! transitions are Touch.
  Standard_EXPORT IntPatch_WLine(const Handle(IntSurf_LineOn2S)& Line, const Standard_Boolean Tang, const IntSurf_Situation Situ1, const IntSurf_Situation Situ2);
  
  //! Creates a WLine as an intersection when the
  //! transitions are Undecided.
  Standard_EXPORT IntPatch_WLine(const Handle(IntSurf_LineOn2S)& Line, const Standard_Boolean Tang);
  
  //! Adds a vertex in the list.
    void AddVertex (const IntPatch_Point& Pnt);
  
  //! Set the Point of index <Index> in the LineOn2S
  Standard_EXPORT void SetPoint (const Standard_Integer Index, const IntPatch_Point& Pnt);
  
  //! Replaces the element of range Index in the list
  //! of points.
  //! The exception OutOfRange is raised when
  //! Index <= 0 or Index > NbVertex.
    void Replace (const Standard_Integer Index, const IntPatch_Point& Pnt);
  
    void SetFirstPoint (const Standard_Integer IndFirst);
  
    void SetLastPoint (const Standard_Integer IndLast);
  
  //! Returns the number of intersection points.
    Standard_Integer NbPnts() const;
  
  //! Returns the intersection point of range Index.
    const IntSurf_PntOn2S& Point (const Standard_Integer Index) const;
  
  //! Returns True if the line has a known First point.
  //! This point is given by the method FirstPoint().
    Standard_Boolean HasFirstPoint() const;
  
  //! Returns True if the line has a known Last point.
  //! This point is given by the method LastPoint().
    Standard_Boolean HasLastPoint() const;
  
  //! Returns the Point corresponding to the FirstPoint.
    const IntPatch_Point& FirstPoint() const;
  
  //! Returns the Point corresponding to the LastPoint.
    const IntPatch_Point& LastPoint() const;
  
  //! Returns the Point corresponding to the FirstPoint.
  //! Indfirst is the index of the first in the list
  //! of vertices.
    const IntPatch_Point& FirstPoint (Standard_Integer& Indfirst) const;
  
  //! Returns the Point corresponding to the LastPoint.
  //! Indlast is the index of the last in the list
  //! of vertices.
    const IntPatch_Point& LastPoint (Standard_Integer& Indlast) const;
  
    Standard_Integer NbVertex() const;
  
  //! Returns the vertex of range Index on the line.
    const IntPatch_Point& Vertex (const Standard_Integer Index) const;
  
  //! Set the parameters of all the vertex on the line.
  //! if a vertex is already in the line,
  //! its parameter is modified
  //! else a new point in the line is inserted.
  Standard_EXPORT void ComputeVertexParameters (const Standard_Real Tol, const Standard_Boolean hasBeenAdded = Standard_False);
  
  Standard_EXPORT Handle(IntSurf_LineOn2S) Curve() const;
  
  Standard_EXPORT Standard_Boolean IsOutSurf1Box (const gp_Pnt2d& P1);
  
  Standard_EXPORT Standard_Boolean IsOutSurf2Box (const gp_Pnt2d& P1);
  
  Standard_EXPORT Standard_Boolean IsOutBox (const gp_Pnt& P);
  
  Standard_EXPORT void SetPeriod (const Standard_Real pu1, const Standard_Real pv1, const Standard_Real pu2, const Standard_Real pv2);
  
  Standard_EXPORT Standard_Real U1Period() const;
  
  Standard_EXPORT Standard_Real V1Period() const;
  
  Standard_EXPORT Standard_Real U2Period() const;
  
  Standard_EXPORT Standard_Real V2Period() const;
  
  Standard_EXPORT void SetArcOnS1 (const Handle(Adaptor2d_HCurve2d)& A);
  
  Standard_EXPORT Standard_Boolean HasArcOnS1() const;
  
  Standard_EXPORT const Handle(Adaptor2d_HCurve2d)& GetArcOnS1() const;
  
  Standard_EXPORT void SetArcOnS2 (const Handle(Adaptor2d_HCurve2d)& A);
  
  Standard_EXPORT Standard_Boolean HasArcOnS2() const;
  
  Standard_EXPORT const Handle(Adaptor2d_HCurve2d)& GetArcOnS2() const;
  
  Standard_EXPORT void ClearVertexes();
  
  Standard_EXPORT void RemoveVertex (const Standard_Integer theIndex);
  
  Standard_EXPORT void InsertVertexBefore (const Standard_Integer theIndex, const IntPatch_Point& thePnt);
  
  //! if (theMode == 0) then prints the information about WLine
  //! if (theMode == 1) then prints the list of 3d-points
  //! if (theMode == 2) then prints the list of 2d-points on the 1st surface
  //! Otherwise,             prints list of 2d-points on the 2nd surface
  Standard_EXPORT void Dump(const Standard_Integer theMode) const;




  DEFINE_STANDARD_RTTI(IntPatch_WLine,IntPatch_PointLine)

protected:




private:


  Handle(IntSurf_LineOn2S) curv;
  Standard_Boolean fipt;
  Standard_Boolean lapt;
  Standard_Integer indf;
  Standard_Integer indl;
  IntPatch_SequenceOfPoint svtx;
  Bnd_Box2d Buv1;
  Bnd_Box2d Buv2;
  Bnd_Box Bxyz;
  Standard_Real u1period;
  Standard_Real v1period;
  Standard_Real u2period;
  Standard_Real v2period;
  Standard_Boolean hasArcOnS1;
  Handle(Adaptor2d_HCurve2d) theArcOnS1;
  Standard_Boolean hasArcOnS2;
  Handle(Adaptor2d_HCurve2d) theArcOnS2;


};


#include <IntPatch_WLine.lxx>





#endif // _IntPatch_WLine_HeaderFile
