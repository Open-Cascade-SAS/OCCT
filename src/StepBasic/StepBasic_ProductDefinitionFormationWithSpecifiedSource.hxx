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

#ifndef _StepBasic_ProductDefinitionFormationWithSpecifiedSource_HeaderFile
#define _StepBasic_ProductDefinitionFormationWithSpecifiedSource_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_Source.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
class TCollection_HAsciiString;
class StepBasic_Product;

class StepBasic_ProductDefinitionFormationWithSpecifiedSource;
DEFINE_STANDARD_HANDLE(StepBasic_ProductDefinitionFormationWithSpecifiedSource,
                       StepBasic_ProductDefinitionFormation)

class StepBasic_ProductDefinitionFormationWithSpecifiedSource
    : public StepBasic_ProductDefinitionFormation
{

public:
  //! Returns a ProductDefinitionFormationWithSpecifiedSource
  Standard_EXPORT StepBasic_ProductDefinitionFormationWithSpecifiedSource();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aId,
                            const Handle(TCollection_HAsciiString)& aDescription,
                            const Handle(StepBasic_Product)&        aOfProduct,
                            const StepBasic_Source                  aMakeOrBuy);

  Standard_EXPORT void SetMakeOrBuy(const StepBasic_Source aMakeOrBuy);

  Standard_EXPORT StepBasic_Source MakeOrBuy() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_ProductDefinitionFormationWithSpecifiedSource,
                          StepBasic_ProductDefinitionFormation)

protected:
private:
  StepBasic_Source makeOrBuy;
};

#endif // _StepBasic_ProductDefinitionFormationWithSpecifiedSource_HeaderFile
