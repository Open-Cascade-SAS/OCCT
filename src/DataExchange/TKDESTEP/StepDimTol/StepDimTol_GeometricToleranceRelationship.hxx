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

#ifndef _StepDimTol_GeometricToleranceRelationship_HeaderFile
#define _StepDimTol_GeometricToleranceRelationship_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepDimTol_GeometricTolerance;

class StepDimTol_GeometricToleranceRelationship;
DEFINE_STANDARD_HANDLE(StepDimTol_GeometricToleranceRelationship, Standard_Transient)

//! Representation of STEP entity GeometricToleranceRelationship
class StepDimTol_GeometricToleranceRelationship : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepDimTol_GeometricToleranceRelationship();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const Handle(TCollection_HAsciiString)&      theName,
    const Handle(TCollection_HAsciiString)&      theDescription,
    const Handle(StepDimTol_GeometricTolerance)& theRelatingGeometricTolerance,
    const Handle(StepDimTol_GeometricTolerance)& theRelatedGeometricTolerance);

  //! Returns field Name
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;

  //! Set field Name
  Standard_EXPORT void SetName(const Handle(TCollection_HAsciiString)& theName);

  //! Returns field Description
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;

  //! Set field Description
  Standard_EXPORT void SetDescription(const Handle(TCollection_HAsciiString)& theDescription);

  //! Returns field RelatingGeometricTolerance
  Standard_EXPORT Handle(StepDimTol_GeometricTolerance) RelatingGeometricTolerance() const;

  //! Set field RelatingGeometricTolerance
  Standard_EXPORT void SetRelatingGeometricTolerance(
    const Handle(StepDimTol_GeometricTolerance)& theRelatingGeometricTolerance);

  //! Returns field RelatedGeometricTolerance
  Standard_EXPORT Handle(StepDimTol_GeometricTolerance) RelatedGeometricTolerance() const;

  //! Set field RelatedGeometricTolerance
  Standard_EXPORT void SetRelatedGeometricTolerance(
    const Handle(StepDimTol_GeometricTolerance)& theRelatedGeometricTolerance);

  DEFINE_STANDARD_RTTIEXT(StepDimTol_GeometricToleranceRelationship, Standard_Transient)

protected:
private:
  Handle(TCollection_HAsciiString)      myName;
  Handle(TCollection_HAsciiString)      myDescription;
  Handle(StepDimTol_GeometricTolerance) myRelatingGeometricTolerance;
  Handle(StepDimTol_GeometricTolerance) myRelatedGeometricTolerance;
};

#endif // _StepDimTol_GeometricToleranceRelationship_HeaderFile
