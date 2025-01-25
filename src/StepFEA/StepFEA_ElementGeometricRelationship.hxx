// Created on: 2003-02-04
// Created by: data exchange team
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

#ifndef _StepFEA_ElementGeometricRelationship_HeaderFile
#define _StepFEA_ElementGeometricRelationship_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepFEA_ElementOrElementGroup.hxx>
#include <StepElement_ElementAspect.hxx>
#include <Standard_Transient.hxx>
class StepElement_AnalysisItemWithinRepresentation;

class StepFEA_ElementGeometricRelationship;
DEFINE_STANDARD_HANDLE(StepFEA_ElementGeometricRelationship, Standard_Transient)

//! Representation of STEP entity ElementGeometricRelationship
class StepFEA_ElementGeometricRelationship : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepFEA_ElementGeometricRelationship();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const StepFEA_ElementOrElementGroup&                        aElementRef,
                            const Handle(StepElement_AnalysisItemWithinRepresentation)& aItem,
                            const StepElement_ElementAspect&                            aAspect);

  //! Returns field ElementRef
  Standard_EXPORT StepFEA_ElementOrElementGroup ElementRef() const;

  //! Set field ElementRef
  Standard_EXPORT void SetElementRef(const StepFEA_ElementOrElementGroup& ElementRef);

  //! Returns field Item
  Standard_EXPORT Handle(StepElement_AnalysisItemWithinRepresentation) Item() const;

  //! Set field Item
  Standard_EXPORT void SetItem(const Handle(StepElement_AnalysisItemWithinRepresentation)& Item);

  //! Returns field Aspect
  Standard_EXPORT StepElement_ElementAspect Aspect() const;

  //! Set field Aspect
  Standard_EXPORT void SetAspect(const StepElement_ElementAspect& Aspect);

  DEFINE_STANDARD_RTTIEXT(StepFEA_ElementGeometricRelationship, Standard_Transient)

protected:
private:
  StepFEA_ElementOrElementGroup                        theElementRef;
  Handle(StepElement_AnalysisItemWithinRepresentation) theItem;
  StepElement_ElementAspect                            theAspect;
};

#endif // _StepFEA_ElementGeometricRelationship_HeaderFile
