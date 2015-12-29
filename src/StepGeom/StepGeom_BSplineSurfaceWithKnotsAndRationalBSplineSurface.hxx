// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
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

#ifndef _StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface_HeaderFile
#define _StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_BSplineSurface.hxx>
#include <Standard_Integer.hxx>
#include <StepGeom_HArray2OfCartesianPoint.hxx>
#include <StepGeom_BSplineSurfaceForm.hxx>
#include <StepData_Logical.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <StepGeom_KnotType.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <Standard_Real.hxx>
class StepGeom_BSplineSurfaceWithKnots;
class StepGeom_RationalBSplineSurface;
class TCollection_HAsciiString;


class StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface;
DEFINE_STANDARD_HANDLE(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface, StepGeom_BSplineSurface)


class StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface : public StepGeom_BSplineSurface
{

public:

  
  //! Returns a BSplineSurfaceWithKnotsAndRationalBSplineSurface
  Standard_EXPORT StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Standard_Integer aUDegree, const Standard_Integer aVDegree, const Handle(StepGeom_HArray2OfCartesianPoint)& aControlPointsList, const StepGeom_BSplineSurfaceForm aSurfaceForm, const StepData_Logical aUClosed, const StepData_Logical aVClosed, const StepData_Logical aSelfIntersect, const Handle(StepGeom_BSplineSurfaceWithKnots)& aBSplineSurfaceWithKnots, const Handle(StepGeom_RationalBSplineSurface)& aRationalBSplineSurface);
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Standard_Integer aUDegree, const Standard_Integer aVDegree, const Handle(StepGeom_HArray2OfCartesianPoint)& aControlPointsList, const StepGeom_BSplineSurfaceForm aSurfaceForm, const StepData_Logical aUClosed, const StepData_Logical aVClosed, const StepData_Logical aSelfIntersect, const Handle(TColStd_HArray1OfInteger)& aUMultiplicities, const Handle(TColStd_HArray1OfInteger)& aVMultiplicities, const Handle(TColStd_HArray1OfReal)& aUKnots, const Handle(TColStd_HArray1OfReal)& aVKnots, const StepGeom_KnotType aKnotSpec, const Handle(TColStd_HArray2OfReal)& aWeightsData);
  
  Standard_EXPORT void SetBSplineSurfaceWithKnots (const Handle(StepGeom_BSplineSurfaceWithKnots)& aBSplineSurfaceWithKnots);
  
  Standard_EXPORT Handle(StepGeom_BSplineSurfaceWithKnots) BSplineSurfaceWithKnots() const;
  
  Standard_EXPORT void SetRationalBSplineSurface (const Handle(StepGeom_RationalBSplineSurface)& aRationalBSplineSurface);
  
  Standard_EXPORT Handle(StepGeom_RationalBSplineSurface) RationalBSplineSurface() const;
  
  Standard_EXPORT void SetUMultiplicities (const Handle(TColStd_HArray1OfInteger)& aUMultiplicities);
  
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) UMultiplicities() const;
  
  Standard_EXPORT Standard_Integer UMultiplicitiesValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbUMultiplicities() const;
  
  Standard_EXPORT void SetVMultiplicities (const Handle(TColStd_HArray1OfInteger)& aVMultiplicities);
  
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) VMultiplicities() const;
  
  Standard_EXPORT Standard_Integer VMultiplicitiesValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbVMultiplicities() const;
  
  Standard_EXPORT void SetUKnots (const Handle(TColStd_HArray1OfReal)& aUKnots);
  
  Standard_EXPORT Handle(TColStd_HArray1OfReal) UKnots() const;
  
  Standard_EXPORT Standard_Real UKnotsValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbUKnots() const;
  
  Standard_EXPORT void SetVKnots (const Handle(TColStd_HArray1OfReal)& aVKnots);
  
  Standard_EXPORT Handle(TColStd_HArray1OfReal) VKnots() const;
  
  Standard_EXPORT Standard_Real VKnotsValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbVKnots() const;
  
  Standard_EXPORT void SetKnotSpec (const StepGeom_KnotType aKnotSpec);
  
  Standard_EXPORT StepGeom_KnotType KnotSpec() const;
  
  Standard_EXPORT void SetWeightsData (const Handle(TColStd_HArray2OfReal)& aWeightsData);
  
  Standard_EXPORT Handle(TColStd_HArray2OfReal) WeightsData() const;
  
  Standard_EXPORT Standard_Real WeightsDataValue (const Standard_Integer num1, const Standard_Integer num2) const;
  
  Standard_EXPORT Standard_Integer NbWeightsDataI() const;
  
  Standard_EXPORT Standard_Integer NbWeightsDataJ() const;




  DEFINE_STANDARD_RTTIEXT(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface,StepGeom_BSplineSurface)

protected:




private:


  Handle(StepGeom_BSplineSurfaceWithKnots) bSplineSurfaceWithKnots;
  Handle(StepGeom_RationalBSplineSurface) rationalBSplineSurface;


};







#endif // _StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface_HeaderFile
