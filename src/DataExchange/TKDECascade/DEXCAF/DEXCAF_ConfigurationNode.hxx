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

#ifndef _DEXCAF_ConfigurationNode_HeaderFile
#define _DEXCAF_ConfigurationNode_HeaderFile

#include <DE_ConfigurationNode.hxx>
#include <PCDM_ReaderFilter.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_List.hxx>

//! The purpose of this class is to configure the transfer process for XDE Documents
//! Stores the necessary settings for DEXCAF_Provider.
//! Configures and creates special provider to transfer XDE Documents.
//!
//! Nodes grouped by Vendor name and Format type.
//! The Vendor name is "OCC"
//! The Format type is "XCAF"
//! The supported CAD extension is ".xbf"
//! The import process is supported.
//! The export process is supported.
class DEXCAF_ConfigurationNode : public DE_ConfigurationNode
{
  DEFINE_STANDARD_RTTIEXT(DEXCAF_ConfigurationNode, DE_ConfigurationNode)
public:
  //! Initializes all field by default
  Standard_EXPORT DEXCAF_ConfigurationNode();

  //! Copies values of all fields
  //! @param[in] theNode object to copy
  Standard_EXPORT DEXCAF_ConfigurationNode(const occ::handle<DEXCAF_ConfigurationNode>& theNode);

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
  struct XCAFDoc_InternalSection
  {
    // Read
    // clang-format off
    PCDM_ReaderFilter::AppendMode ReadAppendMode = PCDM_ReaderFilter::AppendMode::AppendMode_Forbid; //!< Setting up the append mode
    NCollection_List<TCollection_AsciiString> ReadSkipValues; //!< Overwrites the existing attributes by the loaded ones
    NCollection_List<TCollection_AsciiString> ReadValues; //!< Adds sub-tree path or adds attribute to read by typename
    // clang-format on

  } InternalParameters;
};

#endif // _DEXCAF_ConfigurationNode_HeaderFile
