// Created on: 1994-05-31
// Created by: Modelistation
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

#ifndef _IGESSelect_IGESTypeForm_HeaderFile
#define _IGESSelect_IGESTypeForm_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_Signature.hxx>
#include <Standard_CString.hxx>
class Standard_Transient;
class Interface_InterfaceModel;

// Avoid possible conflict with SetForm macro defined by windows.h
#ifdef SetForm
  #undef SetForm
#endif

class IGESSelect_IGESTypeForm;
DEFINE_STANDARD_HANDLE(IGESSelect_IGESTypeForm, IFSelect_Signature)

//! IGESTypeForm is a Signature specific to the IGES Norm :
//! it gives the signature under two possible forms :
//! - as "mmm nnn", with "mmm" as IGES Type Number, and "nnn"
//! as IGES From Number (even if = 0)  [Default]
//! - as "mmm" alone, which gives only the IGES Type Number
class IGESSelect_IGESTypeForm : public IFSelect_Signature
{

public:
  //! Creates a Signature for IGES Type & Form Numbers
  //! If <withform> is False, for IGES Type Number only
  Standard_EXPORT IGESSelect_IGESTypeForm(const Standard_Boolean withform = Standard_True);

  //! Changes the mode for giving the Form Number
  Standard_EXPORT void SetForm(const Standard_Boolean withform);

  //! Returns the signature for IGES, "mmm nnn" or "mmm" according
  //! creation choice (Type & Form or Type only)
  Standard_EXPORT Standard_CString
    Value(const Handle(Standard_Transient)&       ent,
          const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IGESSelect_IGESTypeForm, IFSelect_Signature)

protected:
private:
  Standard_Boolean theform;
};

#endif // _IGESSelect_IGESTypeForm_HeaderFile
