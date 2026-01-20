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

#ifndef _StepElement_SurfaceSectionFieldConstant_HeaderFile
#define _StepElement_SurfaceSectionFieldConstant_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepElement_SurfaceSectionField.hxx>
class StepElement_SurfaceSection;

//! Representation of STEP entity SurfaceSectionFieldConstant
class StepElement_SurfaceSectionFieldConstant : public StepElement_SurfaceSectionField
{

public:
  //! Empty constructor
  Standard_EXPORT StepElement_SurfaceSectionFieldConstant();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<StepElement_SurfaceSection>& aDefinition);

  //! Returns field Definition
  Standard_EXPORT occ::handle<StepElement_SurfaceSection> Definition() const;

  //! Set field Definition
  Standard_EXPORT void SetDefinition(const occ::handle<StepElement_SurfaceSection>& Definition);

  DEFINE_STANDARD_RTTIEXT(StepElement_SurfaceSectionFieldConstant, StepElement_SurfaceSectionField)

private:
  occ::handle<StepElement_SurfaceSection> theDefinition;
};

#endif // _StepElement_SurfaceSectionFieldConstant_HeaderFile
