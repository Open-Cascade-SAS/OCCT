// Created on: 1994-12-22
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _StepSelect_ModelModifier_HeaderFile
#define _StepSelect_ModelModifier_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_Modifier.hxx>
#include <Standard_Boolean.hxx>
class StepData_StepModel;
class StepData_Protocol;
class IFSelect_ContextModif;
class Interface_InterfaceModel;
class Interface_Protocol;
class Interface_CopyTool;

class StepSelect_ModelModifier;
DEFINE_STANDARD_HANDLE(StepSelect_ModelModifier, IFSelect_Modifier)

class StepSelect_ModelModifier : public IFSelect_Modifier
{

public:
  //! The inherited Perform does the required cast (and refuses to
  //! go further if cast has failed) then calls the instantiated
  //! Performing
  Standard_EXPORT void Perform(IFSelect_ContextModif&                  ctx,
                               const Handle(Interface_InterfaceModel)& target,
                               const Handle(Interface_Protocol)&       protocol,
                               Interface_CopyTool&                     TC) const Standard_OVERRIDE;

  //! Specific Perform with Protocol. It is defined to let the
  //! Protocol unused and to call Performing without Protocol
  //! (most current case). It can be redefined if specific action
  //! requires Protocol.
  Standard_EXPORT virtual void PerformProtocol(IFSelect_ContextModif&            ctx,
                                               const Handle(StepData_StepModel)& target,
                                               const Handle(StepData_Protocol)&  proto,
                                               Interface_CopyTool&               TC) const;

  //! Specific Perform, without Protocol. If Performing with
  //! Protocol is redefined, Performing without Protocol must
  //! though be defined to do nothing (not called, but demanded
  //! by the linker)
  Standard_EXPORT virtual void Performing(IFSelect_ContextModif&            ctx,
                                          const Handle(StepData_StepModel)& target,
                                          Interface_CopyTool&               TC) const = 0;

  DEFINE_STANDARD_RTTI_INLINE(StepSelect_ModelModifier, IFSelect_Modifier)

protected:
  //! Calls inherited Initialize, transmits to it the information
  //! <maychangegraph>
  Standard_EXPORT StepSelect_ModelModifier(const Standard_Boolean maychangegraph);

private:
};

#endif // _StepSelect_ModelModifier_HeaderFile
