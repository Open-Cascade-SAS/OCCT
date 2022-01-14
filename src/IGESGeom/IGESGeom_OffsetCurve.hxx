// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( TCD )
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

#ifndef _IGESGeom_OffsetCurve_HeaderFile
#define _IGESGeom_OffsetCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Vec;


class IGESGeom_OffsetCurve;
DEFINE_STANDARD_HANDLE(IGESGeom_OffsetCurve, IGESData_IGESEntity)

//! defines IGESOffsetCurve, Type <130> Form <0>
//! in package IGESGeom
//! An OffsetCurve entity contains the data necessary to
//! determine the offset of a given curve C. This entity
//! points to the base curve to be offset and contains
//! offset distance and other pertinent information.
class IGESGeom_OffsetCurve : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESGeom_OffsetCurve();
  
  //! This method is used to set the fields of the class
  //! OffsetCurve
  //! - aBaseCurve         : The curve entity to be offset
  //! - anOffsetType       : Offset distance flag
  //! 1 = Single value, uniform distance
  //! 2 = Varying linearly
  //! 3 = As a specified function
  //! - aFunction          : Curve entity, one coordinate of which
  //! describes offset as a function of its
  //! parameter (0 unless OffsetType = 3)
  //! - aFunctionCoord     : Particular coordinate of curve
  //! describing offset as function of its
  //! parameters. (used if OffsetType = 3)
  //! - aTaperedOffsetType : Tapered offset type flag
  //! 1 = Function of arc length
  //! 2 = Function of parameter
  //! (Only used if OffsetType = 2 or 3)
  //! - offDistance1       : First offset distance
  //! (Only used if OffsetType = 1 or 2)
  //! - arcLength1         : Arc length or parameter value of
  //! first offset distance
  //! (Only used if OffsetType = 2)
  //! - offDistance2       : Second offset distance
  //! - arcLength2         : Arc length or parameter value of
  //! second offset distance
  //! (Only used if OffsetType = 2)
  //! - aNormalVec         : Unit vector normal to plane containing
  //! curve to be offset
  //! - anOffsetParam      : Start parameter value of offset curve
  //! - anotherOffsetParam : End parameter value of offset curve
  Standard_EXPORT void Init (const Handle(IGESData_IGESEntity)& aBaseCurve, const Standard_Integer anOffsetType, const Handle(IGESData_IGESEntity)& aFunction, const Standard_Integer aFunctionCoord, const Standard_Integer aTaperedOffsetType, const Standard_Real offDistance1, const Standard_Real arcLength1, const Standard_Real offDistance2, const Standard_Real arcLength2, const gp_XYZ& aNormalVec, const Standard_Real anOffsetParam, const Standard_Real anotherOffsetParam);
  
  //! returns the curve to be offset
  Standard_EXPORT Handle(IGESData_IGESEntity) BaseCurve() const;
  
  //! returns the offset distance flag
  //! 1 = Single value offset (uniform distance)
  //! 2 = Offset distance varying linearly
  //! 3 = Offset distance specified as a function
  Standard_EXPORT Standard_Integer OffsetType() const;
  
  //! returns the function defining the offset if at all the offset
  //! is described as a function or Null Handle.
  Standard_EXPORT Handle(IGESData_IGESEntity) Function() const;
  
  //! returns True if function defining the offset is present.
  Standard_EXPORT Standard_Boolean HasFunction() const;
  
  //! returns particular coordinate of the curve which describes offset
  //! as a function of its parameters. (only used if OffsetType() = 3)
  Standard_EXPORT Standard_Integer FunctionParameter() const;
  
  //! returns tapered offset type flag (only used if OffsetType() = 2 or 3)
  //! 1 = Function of arc length
  //! 2 = Function of parameter
  Standard_EXPORT Standard_Integer TaperedOffsetType() const;
  
  //! returns first offset distance (only used if OffsetType() = 1 or 2)
  Standard_EXPORT Standard_Real FirstOffsetDistance() const;
  
  //! returns arc length or parameter value (depending on value of
  //! offset distance flag) of first offset distance
  //! (only used if OffsetType() = 2)
  Standard_EXPORT Standard_Real ArcLength1() const;
  
  //! returns the second offset distance
  Standard_EXPORT Standard_Real SecondOffsetDistance() const;
  
  //! returns arc length or parameter value (depending on value of
  //! offset distance flag) of second offset distance
  //! (only used if OffsetType() = 2)
  Standard_EXPORT Standard_Real ArcLength2() const;
  
  //! returns unit vector normal to plane containing curve to be offset
  Standard_EXPORT gp_Vec NormalVector() const;
  
  //! returns unit vector normal to plane containing curve to be offset
  //! after applying Transf. Matrix
  Standard_EXPORT gp_Vec TransformedNormalVector() const;
  
  Standard_EXPORT void Parameters (Standard_Real& StartParam, Standard_Real& EndParam) const;
  
  //! returns Start Parameter value of the offset curve
  Standard_EXPORT Standard_Real StartParameter() const;
  
  //! returns End   Parameter value of the offset curve
  Standard_EXPORT Standard_Real EndParameter() const;




  DEFINE_STANDARD_RTTIEXT(IGESGeom_OffsetCurve,IGESData_IGESEntity)

protected:




private:


  Handle(IGESData_IGESEntity) theBaseCurve;
  Standard_Integer theOffsetType;
  Handle(IGESData_IGESEntity) theFunction;
  Standard_Integer theFunctionCoord;
  Standard_Integer theTaperedOffsetType;
  Standard_Real theOffsetDistance1;
  Standard_Real theArcLength1;
  Standard_Real theOffsetDistance2;
  Standard_Real theArcLength2;
  gp_XYZ theNormalVector;
  Standard_Real theOffsetParam1;
  Standard_Real theOffsetParam2;


};







#endif // _IGESGeom_OffsetCurve_HeaderFile
