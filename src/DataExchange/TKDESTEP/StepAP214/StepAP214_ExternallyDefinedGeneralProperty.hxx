// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _StepAP214_ExternallyDefinedGeneralProperty_HeaderFile
#define _StepAP214_ExternallyDefinedGeneralProperty_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_GeneralProperty.hxx>
#include <Standard_Boolean.hxx>
class StepBasic_ExternallyDefinedItem;
class TCollection_HAsciiString;
class StepBasic_SourceItem;
class StepBasic_ExternalSource;

//! Representation of STEP entity ExternallyDefinedGeneralProperty
class StepAP214_ExternallyDefinedGeneralProperty : public StepBasic_GeneralProperty
{

public:
  //! Empty constructor
  Standard_EXPORT StepAP214_ExternallyDefinedGeneralProperty();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aGeneralProperty_Id,
                            const occ::handle<TCollection_HAsciiString>& aGeneralProperty_Name,
                            const bool                  hasGeneralProperty_Description,
                            const occ::handle<TCollection_HAsciiString>& aGeneralProperty_Description,
                            const StepBasic_SourceItem&             aExternallyDefinedItem_ItemId,
                            const occ::handle<StepBasic_ExternalSource>& aExternallyDefinedItem_Source);

  //! Returns data for supertype ExternallyDefinedItem
  Standard_EXPORT occ::handle<StepBasic_ExternallyDefinedItem> ExternallyDefinedItem() const;

  //! Set data for supertype ExternallyDefinedItem
  Standard_EXPORT void SetExternallyDefinedItem(
    const occ::handle<StepBasic_ExternallyDefinedItem>& ExternallyDefinedItem);

  DEFINE_STANDARD_RTTIEXT(StepAP214_ExternallyDefinedGeneralProperty, StepBasic_GeneralProperty)

private:
  occ::handle<StepBasic_ExternallyDefinedItem> theExternallyDefinedItem;
};

#endif // _StepAP214_ExternallyDefinedGeneralProperty_HeaderFile
