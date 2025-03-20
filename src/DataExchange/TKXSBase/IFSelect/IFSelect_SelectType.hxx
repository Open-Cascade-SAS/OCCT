// Created on: 1992-11-18
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IFSelect_SelectType_HeaderFile
#define _IFSelect_SelectType_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Type.hxx>
#include <IFSelect_SelectAnyType.hxx>
class TCollection_AsciiString;

class IFSelect_SelectType;
DEFINE_STANDARD_HANDLE(IFSelect_SelectType, IFSelect_SelectAnyType)

//! A SelectType keeps or rejects Entities of which the Type
//! is Kind of a given Cdl Type
class IFSelect_SelectType : public IFSelect_SelectAnyType
{

public:
  //! Creates a SelectType. Default is no filter
  Standard_EXPORT IFSelect_SelectType();

  //! Creates a SelectType for a given Type
  Standard_EXPORT IFSelect_SelectType(const Handle(Standard_Type)& atype);

  //! Sets a TYpe for filter
  Standard_EXPORT void SetType(const Handle(Standard_Type)& atype);

  //! Returns the Type to be matched for select : this is the type
  //! given at instantiation time
  Standard_EXPORT Handle(Standard_Type) TypeForMatch() const Standard_OVERRIDE;

  //! Returns a text defining the criterium.
  //! (should by gotten from Type of Entity used for instantiation)
  Standard_EXPORT TCollection_AsciiString ExtractLabel() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectType, IFSelect_SelectAnyType)

protected:
private:
  Handle(Standard_Type) thetype;
};

#endif // _IFSelect_SelectType_HeaderFile
