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

#ifndef _DEBREP_ConfigurationNode_HeaderFile
#define _DEBREP_ConfigurationNode_HeaderFile

#include <DE_ConfigurationNode.hxx>

#include <BinTools_FormatVersion.hxx>
#include <TopTools_FormatVersion.hxx>

//! The purpose of this class is to configure the transfer process for BRep format
//! Stores the necessary settings for DEBREP_Provider.
//! Configures and creates special provider to transfer BRep files.
//!
//! Nodes grouped by Vendor name and Format type.
//! The Vendor name is "OCC"
//! The Format type is "BREP"
//! The supported CAD extension is ".brep"
//! The import process is supported.
//! The export process is supported.
class DEBREP_ConfigurationNode : public DE_ConfigurationNode
{
  DEFINE_STANDARD_RTTIEXT(DEBREP_ConfigurationNode, DE_ConfigurationNode)
public:
  //! Initializes all field by default
  Standard_EXPORT DEBREP_ConfigurationNode();

  //! Copies values of all fields
  //! @param[in] theNode object to copy
  Standard_EXPORT DEBREP_ConfigurationNode(const occ::handle<DEBREP_ConfigurationNode>& theNode);

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
  struct DEBRep_InternalSection
  {
    // Write
    bool WriteBinary = true;          //!< Defines the binary file format
                                      // clang-format off
    BinTools_FormatVersion WriteVersionBin = BinTools_FormatVersion_CURRENT; //!< Defines the writer version for the binary format
    TopTools_FormatVersion WriteVersionAscii = TopTools_FormatVersion_CURRENT; //!< Defines the writer version for the ASCII format
                                      // clang-format on
    bool WriteTriangles = true; //!< Defines the flag for storing shape with(without) triangles
    bool WriteNormals   = true; //!< Defines the flag for storing shape with(without) normals

  } InternalParameters;
};

#endif // _DEBREP_ConfigurationNode_HeaderFile
