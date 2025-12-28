// Created on: 1998-07-29
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _STEPEdit_EditContext_HeaderFile
#define _STEPEdit_EditContext_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_Editor.hxx>
#include <Standard_Integer.hxx>
class TCollection_AsciiString;
class IFSelect_EditForm;
class TCollection_HAsciiString;
class Standard_Transient;
class Interface_InterfaceModel;

//! EditContext is an Editor fit for
//! Product Definition Context (one per Model) , i.e. :
//! - ProductDefinition
//! - ApplicationProtocolDefinition
//! - ProductRelatedProductCategory
class STEPEdit_EditContext : public IFSelect_Editor
{

public:
  Standard_EXPORT STEPEdit_EditContext();

  Standard_EXPORT TCollection_AsciiString Label() const override;

  Standard_EXPORT bool Recognize(const occ::handle<IFSelect_EditForm>& form) const override;

  Standard_EXPORT occ::handle<TCollection_HAsciiString> StringValue(
    const occ::handle<IFSelect_EditForm>& form,
    const int                             num) const override;

  Standard_EXPORT bool Apply(const occ::handle<IFSelect_EditForm>&        form,
                             const occ::handle<Standard_Transient>&       ent,
                             const occ::handle<Interface_InterfaceModel>& model) const override;

  Standard_EXPORT bool Load(const occ::handle<IFSelect_EditForm>&        form,
                            const occ::handle<Standard_Transient>&       ent,
                            const occ::handle<Interface_InterfaceModel>& model) const override;

  DEFINE_STANDARD_RTTIEXT(STEPEdit_EditContext, IFSelect_Editor)
};

#endif // _STEPEdit_EditContext_HeaderFile
