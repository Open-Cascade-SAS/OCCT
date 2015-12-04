// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen (Kiran)
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

#ifndef _IGESGeom_Boundary_HeaderFile
#define _IGESGeom_Boundary_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESData_IGESEntity;
class IGESBasic_HArray1OfHArray1OfIGESEntity;
class Standard_DimensionMismatch;
class Standard_OutOfRange;


class IGESGeom_Boundary;
DEFINE_STANDARD_HANDLE(IGESGeom_Boundary, IGESData_IGESEntity)

//! defines IGESBoundary, Type <141> Form <0>
//! in package IGESGeom
//! A boundary entity identifies a surface boundary consisting
//! of a set of curves lying on the surface
class IGESGeom_Boundary : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESGeom_Boundary();
  
  //! This method is used to set the fields of the class
  //! Boundary
  //! - aType              : Type of bounded surface representation
  //! - aPreference        : Preferred representation of
  //! Trimming Curve
  //! - aSurface           : Untrimmed surface to be bounded
  //! - allModelCurves     : Model Space Curves
  //! - allSenses          : Orientation flags of all Model Space
  //! Curves
  //! - allParameterCurves : Parameter Space Curves
  //! raises exception if allSenses, allModelCurves and
  //! allParameterCurves do not have same dimensions
  Standard_EXPORT void Init (const Standard_Integer aType, const Standard_Integer aPreference, const Handle(IGESData_IGESEntity)& aSurface, const Handle(IGESData_HArray1OfIGESEntity)& allModelCurves, const Handle(TColStd_HArray1OfInteger)& allSenses, const Handle(IGESBasic_HArray1OfHArray1OfIGESEntity)& allParameterCurves);
  
  //! returns type of bounded surface representation
  //! 0 = Boundary entities may only reference model space trimming
  //! curves. Associated surface representation may be parametric
  //! 1 = Boundary entities must reference model space curves and
  //! associated parameter space curve collections. Associated
  //! surface must be a parametric representation
  Standard_EXPORT Standard_Integer BoundaryType() const;
  
  //! returns preferred representation of trimming curves
  //! 0 = Unspecified
  //! 1 = Model space
  //! 2 = Parameter space
  //! 3 = Representations are of equal preference
  Standard_EXPORT Standard_Integer PreferenceType() const;
  
  //! returns the surface to be bounded
  Standard_EXPORT Handle(IGESData_IGESEntity) Surface() const;
  
  //! returns the number of model space curves
  Standard_EXPORT Standard_Integer NbModelSpaceCurves() const;
  
  //! returns Model Space Curve
  //! raises exception if Index <= 0 or Index > NbModelSpaceCurves()
  Standard_EXPORT Handle(IGESData_IGESEntity) ModelSpaceCurve (const Standard_Integer Index) const;
  
  //! returns the sense of a particular model space curve
  //! 1 = model curve direction does not need reversal
  //! 2 = model curve direction needs to be reversed
  //! raises exception if Index <= 0 or Index > NbModelSpaceCurves()
  Standard_EXPORT Standard_Integer Sense (const Standard_Integer Index) const;
  
  //! returns the number of parameter curves associated with one
  //! model space curve referred to by Index
  //! raises exception if Index <= 0 or Index > NbModelSpaceCurves()
  Standard_EXPORT Standard_Integer NbParameterCurves (const Standard_Integer Index) const;
  
  //! returns an array of parameter space curves associated with
  //! a model space curve referred to by the Index
  //! raises exception if Index <= 0 or Index > NbModelSpaceCurves()
  Standard_EXPORT Handle(IGESData_HArray1OfIGESEntity) ParameterCurves (const Standard_Integer Index) const;
  
  //! returns an individual parameter curve
  //! raises exception if Index or Num is out of range
  Standard_EXPORT Handle(IGESData_IGESEntity) ParameterCurve (const Standard_Integer Index, const Standard_Integer Num) const;




  DEFINE_STANDARD_RTTIEXT(IGESGeom_Boundary,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theType;
  Standard_Integer thePreference;
  Handle(IGESData_IGESEntity) theSurface;
  Handle(IGESData_HArray1OfIGESEntity) theModelCurves;
  Handle(TColStd_HArray1OfInteger) theSenses;
  Handle(IGESBasic_HArray1OfHArray1OfIGESEntity) theParameterCurves;


};







#endif // _IGESGeom_Boundary_HeaderFile
