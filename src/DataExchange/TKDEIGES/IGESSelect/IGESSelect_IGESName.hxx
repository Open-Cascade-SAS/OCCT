// Created on: 1994-12-21
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

#ifndef _IGESSelect_IGESName_HeaderFile
#define _IGESSelect_IGESName_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_Signature.hxx>
#include <Standard_CString.hxx>
class Standard_Transient;
class Interface_InterfaceModel;

//! IGESName is a Signature specific to IGESNorm :
//! it considers the Name of an IGESEntity as being its ShortLabel
//! (some sending systems use name, not to identify entities, but
//! ratjer to classify them)
class IGESSelect_IGESName : public IFSelect_Signature
{

public:
  //! Creates a Signature for IGES Name (reduced to ShortLabel,
  //! without SubscriptLabel or Long Name)
  Standard_EXPORT IGESSelect_IGESName();

  //! Returns the ShortLabel as being the Name of an IGESEntity
  //! If <ent> has no name, it returns empty string ""
  Standard_EXPORT const char*
    Value(const occ::handle<Standard_Transient>&       ent,
          const occ::handle<Interface_InterfaceModel>& model) const override;

  DEFINE_STANDARD_RTTIEXT(IGESSelect_IGESName, IFSelect_Signature)

};

#endif // _IGESSelect_IGESName_HeaderFile
