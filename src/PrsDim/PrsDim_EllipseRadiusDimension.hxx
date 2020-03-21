// Created on: 1998-01-22
// Created by: Sergey ZARITCHNY
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _PrsDim_EllipseRadiusDimension_HeaderFile
#define _PrsDim_EllipseRadiusDimension_HeaderFile

#include <gp_Elips.hxx>
#include <PrsDim_Relation.hxx>
#include <PrsDim_KindOfSurface.hxx>

class Geom_OffsetCurve;
class TopoDS_Shape;
class TCollection_ExtendedString;
class Geom_Surface;

DEFINE_STANDARD_HANDLE(PrsDim_EllipseRadiusDimension, PrsDim_Relation)

//! Computes  geometry  (  basis  curve  and  plane  of  dimension)
//! for  input  shape  aShape  from  TopoDS
//! Root  class  for MinRadiusDimension  and  MaxRadiusDimension
class PrsDim_EllipseRadiusDimension : public PrsDim_Relation
{
  DEFINE_STANDARD_RTTIEXT(PrsDim_EllipseRadiusDimension, PrsDim_Relation)
public:

  virtual PrsDim_KindOfDimension KindOfDimension() const Standard_OVERRIDE { return PrsDim_KOD_ELLIPSERADIUS; }
  
  virtual Standard_Boolean IsMovable() const Standard_OVERRIDE { return Standard_True; }
  
  Standard_EXPORT void ComputeGeometry();

protected:

  Standard_EXPORT PrsDim_EllipseRadiusDimension(const TopoDS_Shape& aShape, const TCollection_ExtendedString& aText);

protected:

  gp_Elips myEllipse;
  Standard_Real myFirstPar;
  Standard_Real myLastPar;
  Standard_Boolean myIsAnArc;
  Handle(Geom_OffsetCurve) myOffsetCurve;
  Standard_Real myOffset;
  Standard_Boolean myIsOffset;

private:

  Standard_EXPORT void ComputeFaceGeometry();
  
  Standard_EXPORT void ComputeCylFaceGeometry (const PrsDim_KindOfSurface aSurfType, const Handle(Geom_Surface)& aSurf, const Standard_Real Offset);
  
  Standard_EXPORT void ComputePlanarFaceGeometry();
  
  Standard_EXPORT void ComputeEdgeGeometry();

};

#endif // _PrsDim_EllipseRadiusDimension_HeaderFile
