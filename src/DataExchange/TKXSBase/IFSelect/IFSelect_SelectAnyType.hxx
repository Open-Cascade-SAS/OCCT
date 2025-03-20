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

#ifndef _IFSelect_SelectAnyType_HeaderFile
#define _IFSelect_SelectAnyType_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectExtract.hxx>
#include <Standard_Type.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class Interface_InterfaceModel;

class IFSelect_SelectAnyType;
DEFINE_STANDARD_HANDLE(IFSelect_SelectAnyType, IFSelect_SelectExtract)

//! A SelectAnyType sorts the Entities of which the Type is Kind
//! of a given Type : this Type for Match is specific of each
//! class of SelectAnyType
class IFSelect_SelectAnyType : public IFSelect_SelectExtract
{

public:
  //! Returns the Type which has to be matched for select
  Standard_EXPORT virtual Handle(Standard_Type) TypeForMatch() const = 0;

  //! Returns True for an Entity (model->Value(num)) which is kind
  //! of the chosen type, given by the method TypeForMatch.
  //! Criterium is IsKind.
  Standard_EXPORT Standard_Boolean
    Sort(const Standard_Integer                  rank,
         const Handle(Standard_Transient)&       ent,
         const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectAnyType, IFSelect_SelectExtract)
};

#endif // _IFSelect_SelectAnyType_HeaderFile
