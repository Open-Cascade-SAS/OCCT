// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _StepBasic_ProductConceptContext_HeaderFile
#define _StepBasic_ProductConceptContext_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_ApplicationContextElement.hxx>
class TCollection_HAsciiString;
class StepBasic_ApplicationContext;

class StepBasic_ProductConceptContext;
DEFINE_STANDARD_HANDLE(StepBasic_ProductConceptContext, StepBasic_ApplicationContextElement)

//! Representation of STEP entity ProductConceptContext
class StepBasic_ProductConceptContext : public StepBasic_ApplicationContextElement
{

public:
  //! Empty constructor
  Standard_EXPORT StepBasic_ProductConceptContext();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const Handle(TCollection_HAsciiString)&     aApplicationContextElement_Name,
    const Handle(StepBasic_ApplicationContext)& aApplicationContextElement_FrameOfReference,
    const Handle(TCollection_HAsciiString)&     aMarketSegmentType);

  //! Returns field MarketSegmentType
  Standard_EXPORT Handle(TCollection_HAsciiString) MarketSegmentType() const;

  //! Set field MarketSegmentType
  Standard_EXPORT void SetMarketSegmentType(
    const Handle(TCollection_HAsciiString)& MarketSegmentType);

  DEFINE_STANDARD_RTTIEXT(StepBasic_ProductConceptContext, StepBasic_ApplicationContextElement)

protected:
private:
  Handle(TCollection_HAsciiString) theMarketSegmentType;
};

#endif // _StepBasic_ProductConceptContext_HeaderFile
