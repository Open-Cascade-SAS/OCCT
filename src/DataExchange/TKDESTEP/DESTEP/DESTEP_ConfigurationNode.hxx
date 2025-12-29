// Copyright (c) 2022 OPEN CASCADE SAS
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

#ifndef _DESTEP_ConfigurationNode_HeaderFile
#define _DESTEP_ConfigurationNode_HeaderFile

#include <DESTEP_Parameters.hxx>
#include <DE_ShapeFixConfigurationNode.hxx>
#include <Resource_FormatType.hxx>
#include <STEPControl_StepModelType.hxx>
#include <UnitsMethods_LengthUnit.hxx>

//! The purpose of this class is to configure the transfer process for STEP format
//! Stores the necessary settings for DESTEP_Provider.
//! Configures and creates special provider to transfer STEP files.
//!
//! Nodes grouped by Vendor name and Format type.
//! The Vendor name is "OCC"
//! The Format type is "STEP"
//! The supported CAD extensions are ".stp", ".step", ".stpz"
//! The import process is supported.
//! The export process is supported.
class DESTEP_ConfigurationNode : public DE_ShapeFixConfigurationNode
{
  DEFINE_STANDARD_RTTIEXT(DESTEP_ConfigurationNode, DE_ShapeFixConfigurationNode)
public:
  //! Initializes all field by default
  Standard_EXPORT DESTEP_ConfigurationNode();

  //! Copies values of all fields
  //! @param[in] theNode object to copy
  Standard_EXPORT DESTEP_ConfigurationNode(const occ::handle<DESTEP_ConfigurationNode>& theNode);

  //! Updates values according the resource
  //! @param[in] theResource input resource to use
  //! @return true if theResource loading has ended correctly
  Standard_EXPORT bool Load(const occ::handle<DE_ConfigurationContext>& theResource) override;

  //! Writes configuration to the string
  //! @return result resource string
  Standard_EXPORT TCollection_AsciiString Save() const override;

  //! Copies values of all fields
  //! @return new object with the same field values
  Standard_EXPORT occ::handle<DE_ConfigurationNode> Copy() const override;

  //! Creates new provider for the own format
  //! @return new created provider
  Standard_EXPORT occ::handle<DE_Provider> BuildProvider() override;

public:
  //! Checks the import supporting
  //! @return true if import is supported
  Standard_EXPORT bool IsImportSupported() const override;

  //! Checks the export supporting
  //! @return true if export is supported
  Standard_EXPORT bool IsExportSupported() const override;

  //! Checks for stream support.
  //! @return true if streams are supported
  Standard_EXPORT bool IsStreamSupported() const override;

  //! Gets CAD format name of associated provider
  //! @return provider CAD format
  Standard_EXPORT TCollection_AsciiString GetFormat() const override;

  //! Gets provider's vendor name of associated provider
  //! @return provider's vendor name
  Standard_EXPORT TCollection_AsciiString GetVendor() const override;

  //! Gets list of supported file extensions
  //! @return list of extensions
  Standard_EXPORT NCollection_List<TCollection_AsciiString> GetExtensions() const override;

  //! Checks the file content to verify a format
  //! @param[in] theBuffer read stream buffer to check content
  //! @return true if file is supported by a current provider
  Standard_EXPORT bool CheckContent(
    const occ::handle<NCollection_Buffer>& theBuffer) const override;

public:
  DESTEP_Parameters InternalParameters;
};

#endif // _DESTEP_ConfigurationNode_HeaderFile
