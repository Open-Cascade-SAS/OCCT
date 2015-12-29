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

#ifndef _StepBasic_ProductDefinitionContext_HeaderFile
#define _StepBasic_ProductDefinitionContext_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_ApplicationContextElement.hxx>
class TCollection_HAsciiString;
class StepBasic_ApplicationContext;


class StepBasic_ProductDefinitionContext;
DEFINE_STANDARD_HANDLE(StepBasic_ProductDefinitionContext, StepBasic_ApplicationContextElement)


class StepBasic_ProductDefinitionContext : public StepBasic_ApplicationContextElement
{

public:

  
  //! Returns a ProductDefinitionContext
  Standard_EXPORT StepBasic_ProductDefinitionContext();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepBasic_ApplicationContext)& aFrameOfReference, const Handle(TCollection_HAsciiString)& aLifeCycleStage);
  
  Standard_EXPORT void SetLifeCycleStage (const Handle(TCollection_HAsciiString)& aLifeCycleStage);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) LifeCycleStage() const;




  DEFINE_STANDARD_RTTIEXT(StepBasic_ProductDefinitionContext,StepBasic_ApplicationContextElement)

protected:




private:


  Handle(TCollection_HAsciiString) lifeCycleStage;


};







#endif // _StepBasic_ProductDefinitionContext_HeaderFile
