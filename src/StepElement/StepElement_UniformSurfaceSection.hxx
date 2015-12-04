// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepElement_UniformSurfaceSection_HeaderFile
#define _StepElement_UniformSurfaceSection_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <StepElement_MeasureOrUnspecifiedValue.hxx>
#include <StepElement_SurfaceSection.hxx>
class StepElement_MeasureOrUnspecifiedValue;


class StepElement_UniformSurfaceSection;
DEFINE_STANDARD_HANDLE(StepElement_UniformSurfaceSection, StepElement_SurfaceSection)

//! Representation of STEP entity UniformSurfaceSection
class StepElement_UniformSurfaceSection : public StepElement_SurfaceSection
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepElement_UniformSurfaceSection();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const StepElement_MeasureOrUnspecifiedValue& aSurfaceSection_Offset, const StepElement_MeasureOrUnspecifiedValue& aSurfaceSection_NonStructuralMass, const StepElement_MeasureOrUnspecifiedValue& aSurfaceSection_NonStructuralMassOffset, const Standard_Real aThickness, const StepElement_MeasureOrUnspecifiedValue& aBendingThickness, const StepElement_MeasureOrUnspecifiedValue& aShearThickness);
  
  //! Returns field Thickness
  Standard_EXPORT Standard_Real Thickness() const;
  
  //! Set field Thickness
  Standard_EXPORT void SetThickness (const Standard_Real Thickness);
  
  //! Returns field BendingThickness
  Standard_EXPORT StepElement_MeasureOrUnspecifiedValue BendingThickness() const;
  
  //! Set field BendingThickness
  Standard_EXPORT void SetBendingThickness (const StepElement_MeasureOrUnspecifiedValue& BendingThickness);
  
  //! Returns field ShearThickness
  Standard_EXPORT StepElement_MeasureOrUnspecifiedValue ShearThickness() const;
  
  //! Set field ShearThickness
  Standard_EXPORT void SetShearThickness (const StepElement_MeasureOrUnspecifiedValue& ShearThickness);




  DEFINE_STANDARD_RTTIEXT(StepElement_UniformSurfaceSection,StepElement_SurfaceSection)

protected:




private:


  Standard_Real theThickness;
  StepElement_MeasureOrUnspecifiedValue theBendingThickness;
  StepElement_MeasureOrUnspecifiedValue theShearThickness;


};







#endif // _StepElement_UniformSurfaceSection_HeaderFile
