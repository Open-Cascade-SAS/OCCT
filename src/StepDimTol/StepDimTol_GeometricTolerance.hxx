// Created on: 2003-06-04
// Created by: Galina KULIKOVA
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _StepDimTol_GeometricTolerance_HeaderFile
#define _StepDimTol_GeometricTolerance_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <StepDimTol_GeometricToleranceTarget.hxx>
class TCollection_HAsciiString;
class StepBasic_MeasureWithUnit;
class StepDimTol_GeometricToleranceTarget;
class StepRepr_ShapeAspect;


class StepDimTol_GeometricTolerance;
DEFINE_STANDARD_HANDLE(StepDimTol_GeometricTolerance, Standard_Transient)

//! Representation of STEP entity GeometricTolerance
class StepDimTol_GeometricTolerance : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepDimTol_GeometricTolerance();
  
  //! Initialize all fields (own and inherited) AP214
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theName, 
                            const Handle(TCollection_HAsciiString)& theDescription, 
                            const Handle(StepBasic_MeasureWithUnit)& theMagnitude, 
                            const Handle(StepRepr_ShapeAspect)& theTolerancedShapeAspect);

    //! Initialize all fields (own and inherited) AP242
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theName, 
                            const Handle(TCollection_HAsciiString)& theDescription, 
                            const Handle(StepBasic_MeasureWithUnit)& theMagnitude, 
                            const StepDimTol_GeometricToleranceTarget& theTolerancedShapeAspect);
  
  //! Returns field Name
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;
  
  //! Set field Name
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& theName);
  
  //! Returns field Description
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;
  
  //! Set field Description
  Standard_EXPORT void SetDescription (const Handle(TCollection_HAsciiString)& theDescription);
  
  //! Returns field Magnitude
  Standard_EXPORT Handle(StepBasic_MeasureWithUnit) Magnitude() const;
  
  //! Set field Magnitude
  Standard_EXPORT void SetMagnitude (const Handle(StepBasic_MeasureWithUnit)& theMagnitude);
  
  //! Returns field TolerancedShapeAspect
  //! Note: in AP214(203) type of this attribute can be only StepRepr_ShapeAspect
  Standard_EXPORT StepDimTol_GeometricToleranceTarget TolerancedShapeAspect() const;
  
  //! Set field TolerancedShapeAspect AP214
  Standard_EXPORT void SetTolerancedShapeAspect (const Handle(StepRepr_ShapeAspect)& theTolerancedShapeAspect);

  //! Set field TolerancedShapeAspect AP242
  Standard_EXPORT void SetTolerancedShapeAspect (const StepDimTol_GeometricToleranceTarget& theTolerancedShapeAspect);




  DEFINE_STANDARD_RTTIEXT(StepDimTol_GeometricTolerance,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) myName;
  Handle(TCollection_HAsciiString) myDescription;
  Handle(StepBasic_MeasureWithUnit) myMagnitude;
  StepDimTol_GeometricToleranceTarget myTolerancedShapeAspect;


};







#endif // _StepDimTol_GeometricTolerance_HeaderFile
