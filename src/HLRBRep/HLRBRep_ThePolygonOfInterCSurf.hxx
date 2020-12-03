// Created on: 1992-10-14
// Created by: Christophe MARION
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

#ifndef _HLRBRep_ThePolygonOfInterCSurf_HeaderFile
#define _HLRBRep_ThePolygonOfInterCSurf_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Bnd_Box.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_Array1OfReal.hxx>
class Standard_OutOfRange;
class gp_Lin;
class HLRBRep_LineTool;
class Bnd_Box;
class gp_Pnt;



class HLRBRep_ThePolygonOfInterCSurf 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT HLRBRep_ThePolygonOfInterCSurf(const gp_Lin& Curve, const Standard_Integer NbPnt);
  
  Standard_EXPORT HLRBRep_ThePolygonOfInterCSurf(const gp_Lin& Curve, const Standard_Real U1, const Standard_Real U2, const Standard_Integer NbPnt);
  
  Standard_EXPORT HLRBRep_ThePolygonOfInterCSurf(const gp_Lin& Curve, const TColStd_Array1OfReal& Upars);
  
  //! Give the bounding box of the polygon.
  const Bnd_Box& Bounding() const { return TheBnd; }

  Standard_Real DeflectionOverEstimation() const { return TheDeflection; }

  void SetDeflectionOverEstimation (const Standard_Real x)
  {
    TheDeflection = x;
    TheBnd.Enlarge (TheDeflection);
  }

  void Closed (const Standard_Boolean flag) { ClosedPolygon = flag; }

  Standard_Boolean Closed() const { return Standard_False; } // -- Voir si le cas Closed est traitable

  //! Give the number of Segments in the polyline.
  Standard_Integer NbSegments() const { return NbPntIn - 1; }

  //! Give the point of range Index in the Polygon.
  const gp_Pnt& BeginOfSeg (const Standard_Integer theIndex) const { return ThePnts (theIndex); }

  //! Give the point of range Index in the Polygon.
  const gp_Pnt& EndOfSeg (const Standard_Integer theIndex) const { return ThePnts (theIndex + 1); }

  //! Returns the parameter (On the curve)
  //! of the first point of the Polygon
  Standard_Real InfParameter() const { return Binf; }

  //! Returns the parameter (On the curve)
  //! of the last point of the Polygon
  Standard_Real SupParameter() const { return Bsup; }

  //! Give an approximation of the parameter on the curve
  //! according to the discretization of the Curve.
  Standard_EXPORT Standard_Real ApproxParamOnCurve (const Standard_Integer Index, const Standard_Real ParamOnLine) const;
  
  Standard_EXPORT void Dump() const;

protected:

  Standard_EXPORT void Init (const gp_Lin& Curve);

  Standard_EXPORT void Init (const gp_Lin& Curve, const TColStd_Array1OfReal& Upars);

private:

  Bnd_Box TheBnd;
  Standard_Real TheDeflection;
  Standard_Integer NbPntIn;
  TColgp_Array1OfPnt ThePnts;
  Standard_Boolean ClosedPolygon;
  Standard_Real Binf;
  Standard_Real Bsup;
  Handle(TColStd_HArray1OfReal) myParams;

};

#endif // _HLRBRep_ThePolygonOfInterCSurf_HeaderFile
