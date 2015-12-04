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

#ifndef _AIS_EllipseRadiusDimension_HeaderFile
#define _AIS_EllipseRadiusDimension_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Elips.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <AIS_Relation.hxx>
#include <AIS_KindOfDimension.hxx>
#include <AIS_KindOfSurface.hxx>
class Geom_OffsetCurve;
class TopoDS_Shape;
class TCollection_ExtendedString;
class Geom_Surface;


class AIS_EllipseRadiusDimension;
DEFINE_STANDARD_HANDLE(AIS_EllipseRadiusDimension, AIS_Relation)


//! Computes  geometry  (  basis  curve  and  plane  of  dimension)
//! for  input  shape  aShape  from  TopoDS
//! Root  class  for MinRadiusDimension  and  MaxRadiusDimension
class AIS_EllipseRadiusDimension : public AIS_Relation
{

public:

  
  Standard_EXPORT virtual AIS_KindOfDimension KindOfDimension() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean IsMovable() const Standard_OVERRIDE;
  
  Standard_EXPORT void ComputeGeometry();




  DEFINE_STANDARD_RTTIEXT(AIS_EllipseRadiusDimension,AIS_Relation)

protected:

  
  Standard_EXPORT AIS_EllipseRadiusDimension(const TopoDS_Shape& aShape, const TCollection_ExtendedString& aText);

  gp_Elips myEllipse;
  Standard_Real myFirstPar;
  Standard_Real myLastPar;
  Standard_Boolean myIsAnArc;
  Handle(Geom_OffsetCurve) myOffsetCurve;
  Standard_Real myOffset;
  Standard_Boolean myIsOffset;


private:

  
  Standard_EXPORT void ComputeFaceGeometry();
  
  Standard_EXPORT void ComputeCylFaceGeometry (const AIS_KindOfSurface aSurfType, const Handle(Geom_Surface)& aSurf, const Standard_Real Offset);
  
  Standard_EXPORT void ComputePlanarFaceGeometry();
  
  Standard_EXPORT void ComputeEdgeGeometry();



};







#endif // _AIS_EllipseRadiusDimension_HeaderFile
