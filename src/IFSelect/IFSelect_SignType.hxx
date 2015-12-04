// Created on: 1996-01-29
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

#ifndef _IFSelect_SignType_HeaderFile
#define _IFSelect_SignType_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <IFSelect_Signature.hxx>
#include <Standard_CString.hxx>
class Standard_Transient;
class Interface_InterfaceModel;


class IFSelect_SignType;
DEFINE_STANDARD_HANDLE(IFSelect_SignType, IFSelect_Signature)

//! This Signature returns the cdl Type of an entity, under two
//! forms :
//! - complete dynamic type (package and class)
//! - class type, without package name
class IFSelect_SignType : public IFSelect_Signature
{

public:

  
  //! Returns a SignType
  //! <nopk> false (D) : complete dynamic type (name = Dynamic Type)
  //! <nopk> true : class type without pk (name = Class Type)
  Standard_EXPORT IFSelect_SignType(const Standard_Boolean nopk = Standard_False);
  
  //! Returns the Signature for a Transient object, as its Dynamic
  //! Type, with or without package name, according starting option
  Standard_EXPORT Standard_CString Value (const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IFSelect_SignType,IFSelect_Signature)

protected:




private:


  Standard_Boolean thenopk;


};







#endif // _IFSelect_SignType_HeaderFile
