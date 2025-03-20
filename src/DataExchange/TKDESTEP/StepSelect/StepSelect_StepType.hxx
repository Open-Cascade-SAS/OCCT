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

#ifndef _StepSelect_StepType_HeaderFile
#define _StepSelect_StepType_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepData_WriterLib.hxx>
#include <IFSelect_Signature.hxx>
#include <Standard_CString.hxx>
class StepData_Protocol;
class Interface_Protocol;
class Standard_Transient;
class Interface_InterfaceModel;

class StepSelect_StepType;
DEFINE_STANDARD_HANDLE(StepSelect_StepType, IFSelect_Signature)

//! StepType is a Signature specific to Step definitions : it
//! considers the type as defined in STEP Schemas, the same which
//! is used in files.
//! For a Complex Type, if its definition is known, StepType
//! produces the list of basic types, separated by commas, the
//! whole between brackets : "(TYPE1,TYPE2..)".
//! If its precise definition is not known (simply it is known as
//! Complex, it can be recognised, but the list is produced at
//! Write time only), StepType produces : "(..COMPLEX TYPE..)"
class StepSelect_StepType : public IFSelect_Signature
{

public:
  //! Creates a Signature for Step Type. Protocol is undefined here,
  //! hence no Signature may yet be produced. The StepType signature
  //! requires a Protocol before working
  Standard_EXPORT StepSelect_StepType();

  //! Sets the StepType signature to work with a Protocol : this
  //! initialises the library
  Standard_EXPORT void SetProtocol(const Handle(Interface_Protocol)& proto);

  //! Returns the Step Type defined from the Protocol (see above).
  //! If <ent> is not recognised, produces "..NOT FROM SCHEMA <name>.."
  Standard_EXPORT Standard_CString
    Value(const Handle(Standard_Transient)&       ent,
          const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(StepSelect_StepType, IFSelect_Signature)

protected:
  StepData_WriterLib thelib;

private:
  Handle(StepData_Protocol) theproto;
};

#endif // _StepSelect_StepType_HeaderFile
