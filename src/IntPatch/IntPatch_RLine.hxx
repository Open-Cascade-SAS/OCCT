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
#include <IntPatch_SequenceOfPoint.hxx>
#include <IntPatch_PointLine.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntSurf_Situation.hxx>
#include <IntSurf_TypeTrans.hxx>

class IntPatch_Point;
class IntSurf_PntOn2S;


class IntPatch_RLine;
DEFINE_STANDARD_HANDLE(IntPatch_RLine, IntPatch_PointLine)

//! Implementation of an intersection line described by a
//! restriction line on one of the surfaces.
class IntPatch_RLine : public IntPatch_PointLine
{

public:

  
  //! Creates a restriction as an intersection line
  //! when the transitions are In or Out.
  Standard_EXPORT IntPatch_RLine(const Standard_Boolean Tang, const IntSurf_TypeTrans Trans1, const IntSurf_TypeTrans Trans2);
  
  //! Creates a restriction as an intersection line
  //! when the transitions are Touch.
  Standard_EXPORT IntPatch_RLine(const Standard_Boolean Tang, const IntSurf_Situation Situ1, const IntSurf_Situation Situ2);
  
  //! Creates a restriction as an intersection line
  //! when the transitions are Undecided.
  Standard_EXPORT IntPatch_RLine(const Standard_Boolean Tang);
  
  //! Adds a vertex in the list. If theIsPrepend == TRUE the new
  //! vertex will be added before the first element of vertices sequence.
  //! Otherwise, to the end of the sequence
  virtual void AddVertex (const IntPatch_Point& Pnt,
                    const Standard_Boolean theIsPrepend = Standard_False) Standard_OVERRIDE;
  
  //! Replaces the element of range Index in the list
  //! of points.
    void Replace (const Standard_Integer Index, const IntPatch_Point& Pnt);
  
    void SetFirstPoint (const Standard_Integer IndFirst);
  
    void SetLastPoint (const Standard_Integer IndLast);
  
    void Add (const Handle(IntSurf_LineOn2S)& L);
  
  //! Returns True if the intersection is on the domain of the
  //! first patch.
  //! Returns False if the intersection is on the domain of
  //! the second patch.
    Standard_Boolean IsArcOnS1() const;
  
  //! Returns True if the intersection is on the domain of the
  //! first patch.
  //! Returns False if the intersection is on the domain of
  //! the second patch.
    Standard_Boolean IsArcOnS2() const;
  
  Standard_EXPORT void SetArcOnS1 (const Handle(Adaptor2d_Curve2d)& A);
  
  Standard_EXPORT void SetArcOnS2 (const Handle(Adaptor2d_Curve2d)& A);
  
  Standard_EXPORT void SetParamOnS1 (const Standard_Real p1, const Standard_Real p2);
  
  Standard_EXPORT void SetParamOnS2 (Standard_Real& p1, Standard_Real& p2);
  
  //! Returns the concerned arc.
    const Handle(Adaptor2d_Curve2d)& ArcOnS1() const;
  
  //! Returns the concerned arc.
    const Handle(Adaptor2d_Curve2d)& ArcOnS2() const;
  
  Standard_EXPORT void ParamOnS1 (Standard_Real& p1, Standard_Real& p2) const;
  
  Standard_EXPORT void ParamOnS2 (Standard_Real& p1, Standard_Real& p2) const;
  
  //! Returns True if the line has a known First point.
  //! This point is given by the method FirstPoint().
    Standard_Boolean HasFirstPoint() const;
  
  //! Returns True if the line has a known Last point.
  //! This point is given by the method LastPoint().
    Standard_Boolean HasLastPoint() const;
  
  //! Returns the IntPoint corresponding to the FirstPoint.
  //! An exception is raised when HasFirstPoint returns False.
    const IntPatch_Point& FirstPoint() const;
  
  //! Returns the IntPoint corresponding to the LastPoint.
  //! An exception is raised when HasLastPoint returns False.
    const IntPatch_Point& LastPoint() const;
  
  //! Returns number of vertices (IntPatch_Point) of the line
  virtual Standard_Integer NbVertex() const Standard_OVERRIDE;
  
  //! Returns the vertex of range Index on the line.
  virtual const IntPatch_Point& Vertex (const Standard_Integer Index) const Standard_OVERRIDE;

  //! Returns the vertex of range Index on the line.
  virtual IntPatch_Point& ChangeVertex (const Standard_Integer Index) Standard_OVERRIDE;
  
  //! Removes single vertex from the line
  virtual void RemoveVertex (const Standard_Integer theIndex) Standard_OVERRIDE;

    Standard_Boolean HasPolygon() const;
  
  //! Returns the number of intersection points.
  virtual Standard_Integer NbPnts() const Standard_OVERRIDE;
  
  //! Returns the intersection point of range Index.
  virtual const IntSurf_PntOn2S& Point (const Standard_Integer Index) const Standard_OVERRIDE;
  
  //! Set the Point of index <Index> in the LineOn2S
  Standard_EXPORT void SetPoint (const Standard_Integer Index, const IntPatch_Point& Pnt);
  
  //! Set the parameters of all the vertex on the line.
  //! if a vertex is already in the line,
  //! its parameter is modified
  //! else a new point in the line is inserted.
  Standard_EXPORT void ComputeVertexParameters (const Standard_Real Tol);

  //! Returns set of intersection points
  virtual Handle(IntSurf_LineOn2S) Curve() const Standard_OVERRIDE;

  //! Returns TRUE if theP is out of the box built from
  //! the points on 1st surface
  virtual Standard_Boolean IsOutSurf1Box(const gp_Pnt2d& theP) const Standard_OVERRIDE
  {
    return curv->IsOutSurf1Box(theP);
  }

  //! Returns TRUE if theP is out of the box built from
  //! the points on 2nd surface
  virtual Standard_Boolean IsOutSurf2Box(const gp_Pnt2d& theP) const Standard_OVERRIDE
  {
    return curv->IsOutSurf2Box(theP);
  }

  //! Returns TRUE if theP is out of the box built from 3D-points.
  virtual Standard_Boolean IsOutBox(const gp_Pnt& theP) const Standard_OVERRIDE
  {
    return curv->IsOutBox(theP);
  }

  //! Removes vertices from the line (i.e. cleans svtx member)
  virtual void ClearVertexes() Standard_OVERRIDE
  {
    svtx.Clear();
  }

  void SetCurve(const Handle(IntSurf_LineOn2S)& theNewCurve)
  {
    curv = theNewCurve;
  }

  //! if (theMode == 0) then prints the information about WLine
  //! if (theMode == 1) then prints the list of 3d-points
  //! if (theMode == 2) then prints the list of 2d-points on the 1st surface
  //! Otherwise,             prints list of 2d-points on the 2nd surface
  Standard_EXPORT void Dump(const Standard_Integer theMode) const;

  DEFINE_STANDARD_RTTIEXT(IntPatch_RLine,IntPatch_PointLine)

protected:




private:


  Handle(Adaptor2d_Curve2d) theArcOnS1;
  Handle(Adaptor2d_Curve2d) theArcOnS2;
  Standard_Boolean onS1;
  Standard_Boolean onS2;
  Standard_Real ParamInf1;
  Standard_Real ParamSup1;
  Standard_Real ParamInf2;
  Standard_Real ParamSup2;
  Handle(IntSurf_LineOn2S) curv;
  Standard_Boolean fipt;
  Standard_Boolean lapt;
  Standard_Integer indf;
  Standard_Integer indl;
  IntPatch_SequenceOfPoint svtx;


};


#include <IntPatch_RLine.lxx>





#endif // _IntPatch_RLine_HeaderFile
