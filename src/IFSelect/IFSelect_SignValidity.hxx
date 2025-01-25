// Created on: 1996-01-26
// Created by: Christian CAILLET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _IFSelect_SignValidity_HeaderFile
#define _IFSelect_SignValidity_HeaderFile

#include <Standard.hxx>

#include <IFSelect_Signature.hxx>
class Standard_Transient;
class Interface_InterfaceModel;

class IFSelect_SignValidity;
DEFINE_STANDARD_HANDLE(IFSelect_SignValidity, IFSelect_Signature)

//! This Signature returns the Validity Status of an entity, as
//! deducted from data in the model : it can be
//! "OK" "Unknown" "Unloaded" "Syntactic Fail"(but loaded)
//! "Syntactic Warning" "Semantic Fail" "Semantic Warning"
class IFSelect_SignValidity : public IFSelect_Signature
{

public:
  //! Returns a SignValidity
  Standard_EXPORT IFSelect_SignValidity();

  //! Returns the Signature for a Transient object, as a validity
  //! deducted from data (reports) stored in the model.
  //! Class method, can be called by any one
  Standard_EXPORT static Standard_CString CVal(const Handle(Standard_Transient)&       ent,
                                               const Handle(Interface_InterfaceModel)& model);

  //! Returns the Signature for a Transient object, as a validity
  //! deducted from data (reports) stored in the model
  //! Calls the class method CVal
  Standard_EXPORT Standard_CString
    Value(const Handle(Standard_Transient)&       ent,
          const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IFSelect_SignValidity, IFSelect_Signature)

protected:
private:
};

#endif // _IFSelect_SignValidity_HeaderFile
