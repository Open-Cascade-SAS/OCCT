// Created on: 1995-06-06
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepApprox_TheMultiLineOfApprox_HeaderFile
#define _BRepApprox_TheMultiLineOfApprox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Address.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Approx_Status.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfVec2d.hxx>
class BRepApprox_ApproxLine;
class ApproxInt_SvSurfaces;



class BRepApprox_TheMultiLineOfApprox 
{
public:

  DEFINE_STANDARD_ALLOC
  
  Standard_EXPORT BRepApprox_TheMultiLineOfApprox();
  
  //! The class SvSurfaces is used when the approximation algorithm 
  //! needs some extra points on the line <line>. 
  //! A New line  is then created which shares the same surfaces and functions.
  //! SvSurfaces is a deferred class which allows several implementations of
  //! this  algorithm with different surfaces (bi-parametric ones, or 
  //! implicit and biparametric ones)
  Standard_EXPORT BRepApprox_TheMultiLineOfApprox(const Handle(BRepApprox_ApproxLine)& line,
                                                  const Standard_Address PtrSvSurfaces,
                                                  const Standard_Integer NbP3d,
                                                  const Standard_Integer NbP2d,
                                                  const Standard_Boolean ApproxU1V1,
                                                  const Standard_Boolean ApproxU2V2,
                                                  const Standard_Real xo,
                                                  const Standard_Real yo,
                                                  const Standard_Real zo,
                                                  const Standard_Real u1o,
                                                  const Standard_Real v1o,
                                                  const Standard_Real u2o,
                                                  const Standard_Real v2o,
                                                  const Standard_Boolean P2DOnFirst,
                                                  const Standard_Integer IndMin = 0,
                                                  const Standard_Integer IndMax = 0);

  //! No Extra points will be added on the current line
  Standard_EXPORT BRepApprox_TheMultiLineOfApprox(const Handle(BRepApprox_ApproxLine)& line,
                                                  const Standard_Integer NbP3d,
                                                  const Standard_Integer NbP2d,
                                                  const Standard_Boolean ApproxU1V1,
                                                  const Standard_Boolean ApproxU2V2,
                                                  const Standard_Real xo,
                                                  const Standard_Real yo,
                                                  const Standard_Real zo,
                                                  const Standard_Real u1o,
                                                  const Standard_Real v1o,
                                                  const Standard_Real u2o,
                                                  const Standard_Real v2o,
                                                  const Standard_Boolean P2DOnFirst,
                                                  const Standard_Integer IndMin = 0,
                                                  const Standard_Integer IndMax = 0);

  Standard_EXPORT Standard_Integer FirstPoint() const;
  
  Standard_EXPORT Standard_Integer LastPoint() const;

  //! Returns the number of 2d points of a TheLine.
  Standard_EXPORT Standard_Integer NbP2d() const;

  //! Returns the number of 3d points of a TheLine.
  Standard_EXPORT Standard_Integer NbP3d() const;

  Standard_EXPORT Approx_Status WhatStatus() const;
  
  //! Returns the 3d points of the multipoint <MPointIndex> when only 3d points exist.
  Standard_EXPORT void Value (const Standard_Integer MPointIndex, TColgp_Array1OfPnt& tabPt) const;

  //! Returns the 2d points of the multipoint <MPointIndex> when only 2d points exist.
  Standard_EXPORT void Value (const Standard_Integer MPointIndex, TColgp_Array1OfPnt2d& tabPt2d) const;

  //! Returns the 3d and 2d points of the multipoint <MPointIndex>.
  Standard_EXPORT void Value (const Standard_Integer MPointIndex, TColgp_Array1OfPnt& tabPt, TColgp_Array1OfPnt2d& tabPt2d) const;

  //! Returns the 3d tangency points of the multipoint <MPointIndex> only when 3d points exist.
  Standard_EXPORT Standard_Boolean Tangency (const Standard_Integer MPointIndex, TColgp_Array1OfVec& tabV) const;
  
  //! Returns the 2d tangency points of the multipoint <MPointIndex> only when 2d points exist.
  Standard_EXPORT Standard_Boolean Tangency (const Standard_Integer MPointIndex, TColgp_Array1OfVec2d& tabV2d) const;
  
  //! Returns the 3d and 2d points of the multipoint <MPointIndex>.
  Standard_EXPORT Standard_Boolean Tangency (const Standard_Integer MPointIndex, TColgp_Array1OfVec& tabV, TColgp_Array1OfVec2d& tabV2d) const;

  //! Tries to make a sub-line between <Low> and <High> points of this line
  //! by adding <NbPointsToInsert> new points
  Standard_EXPORT BRepApprox_TheMultiLineOfApprox MakeMLBetween (const Standard_Integer Low,
                                                                 const Standard_Integer High,
                                                                 const Standard_Integer NbPointsToInsert) const;
  
  //! Tries to make a sub-line between <Low> and <High> points of this line
  //! by adding one more point between (indbad-1)-th and indbad-th points
  Standard_EXPORT Standard_Boolean MakeMLOneMorePoint (const Standard_Integer Low,
                                                       const Standard_Integer High,
                                                       const Standard_Integer indbad,
                                                       BRepApprox_TheMultiLineOfApprox& OtherLine) const;

  //! Dump of the current multi-line.
  Standard_EXPORT void Dump() const;

protected:

private:
  Standard_Address PtrOnmySvSurfaces;
  Handle(BRepApprox_ApproxLine) myLine;
  Standard_Integer indicemin;
  Standard_Integer indicemax;
  Standard_Integer nbp3d;
  Standard_Integer nbp2d;
  Standard_Boolean myApproxU1V1;
  Standard_Boolean myApproxU2V2;
  Standard_Boolean p2donfirst;
  Standard_Real Xo;
  Standard_Real Yo;
  Standard_Real Zo;
  Standard_Real U1o;
  Standard_Real V1o;
  Standard_Real U2o;
  Standard_Real V2o;

};

#endif // _BRepApprox_TheMultiLineOfApprox_HeaderFile
