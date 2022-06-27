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

#ifndef _StepElement_SurfaceSection_HeaderFile
#define _StepElement_SurfaceSection_HeaderFile

#include <Standard.hxx>

#include <StepElement_MeasureOrUnspecifiedValue.hxx>
#include <Standard_Transient.hxx>


class StepElement_SurfaceSection;
DEFINE_STANDARD_HANDLE(StepElement_SurfaceSection, Standard_Transient)

//! Representation of STEP entity SurfaceSection
class StepElement_SurfaceSection : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepElement_SurfaceSection();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const StepElement_MeasureOrUnspecifiedValue& aOffset, const StepElement_MeasureOrUnspecifiedValue& aNonStructuralMass, const StepElement_MeasureOrUnspecifiedValue& aNonStructuralMassOffset);
  
  //! Returns field Offset
  Standard_EXPORT StepElement_MeasureOrUnspecifiedValue Offset() const;
  
  //! Set field Offset
  Standard_EXPORT void SetOffset (const StepElement_MeasureOrUnspecifiedValue& Offset);
  
  //! Returns field NonStructuralMass
  Standard_EXPORT StepElement_MeasureOrUnspecifiedValue NonStructuralMass() const;
  
  //! Set field NonStructuralMass
  Standard_EXPORT void SetNonStructuralMass (const StepElement_MeasureOrUnspecifiedValue& NonStructuralMass);
  
  //! Returns field NonStructuralMassOffset
  Standard_EXPORT StepElement_MeasureOrUnspecifiedValue NonStructuralMassOffset() const;
  
  //! Set field NonStructuralMassOffset
  Standard_EXPORT void SetNonStructuralMassOffset (const StepElement_MeasureOrUnspecifiedValue& NonStructuralMassOffset);




  DEFINE_STANDARD_RTTIEXT(StepElement_SurfaceSection,Standard_Transient)

protected:




private:


  StepElement_MeasureOrUnspecifiedValue theOffset;
  StepElement_MeasureOrUnspecifiedValue theNonStructuralMass;
  StepElement_MeasureOrUnspecifiedValue theNonStructuralMassOffset;


};







#endif // _StepElement_SurfaceSection_HeaderFile
