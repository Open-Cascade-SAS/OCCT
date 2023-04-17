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

#ifndef _DE_ConfigurationNode_HeaderFile
#define _DE_ConfigurationNode_HeaderFile

#include <TColStd_ListOfAsciiString.hxx>

class DE_ConfigurationContext;
class DE_Provider;
class NCollection_Buffer;

//! Base class to work with CAD transfer properties.
//! Stores the necessary settings for a single Provider type.
//! Configures and creates special provider to transfer CAD files.
//!
//! Nodes are grouped by Vendor's name and Format type.
//! The Vendor name is not defined by default.
//! The Format type is not defined by default.
//! The supported CAD extensions are not defined by default.
//! The import process is not supported.
//! The export process is not supported.
//!
//! The algorithm for standalone transfer operation:
//! 1) Create new empty Node object
//! 2) Configure the current Node
//!   2.1) Use the external resource file to configure (::Load)
//!   2.2) Change the internal parameters directly:
//!     2.2.1) Change field values of "GlobalParameters"
//!     2.2.2) Change field values of "InternalParameters"
//! 3) Create one-time transfer provider (::BuildProvider)
//! 4) Initiate the transfer process:
//!   4.1) Import (if "::IsImportSupported: returns TRUE)
//!     4.1.1) Validate the support of input format (::CheckContent or ::CheckExtension)
//!     4.1.2) Use created provider's "::Read" method
//!   4.2) Export (if "::IsExportSupported: returns TRUE)
//!     4.2.1) Use created provider's "::Write" method
//! 5) Check the provider's output
class DE_ConfigurationNode : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(DE_ConfigurationNode, Standard_Transient)
public:

  //! Initializes all field by default
  Standard_EXPORT DE_ConfigurationNode();

  //! Copies values of all fields
  //! @param[in] theConfigurationNode object to copy
  Standard_EXPORT DE_ConfigurationNode(const Handle(DE_ConfigurationNode)& theConfigurationNode);

  //! Updates values according the resource file
  //! @param[in] theResourcePath file path to resource
  //! @return True if Load was successful
  Standard_EXPORT virtual bool Load(const TCollection_AsciiString& theResourcePath = "");

  //! Updates values according the resource
  //! @param[in] theResource input resource to use
  //! @return True if Load was successful
  Standard_EXPORT virtual bool Load(const Handle(DE_ConfigurationContext)& theResource) = 0;

  //! Writes configuration to the resource file
  //! @param[in] theResourcePath file path to resource
  //! @return True if Save was successful
  Standard_EXPORT bool Save(const TCollection_AsciiString& theResourcePath) const;

  //! Writes configuration to the string
  //! @return result resource string
  Standard_EXPORT virtual TCollection_AsciiString Save() const = 0;

  //! Creates new provider for the own format
  //! @return new created provider
  Standard_EXPORT virtual Handle(DE_Provider) BuildProvider() = 0;

  //! Copies values of all fields
  //! @return new object with the same field values
  Standard_EXPORT virtual Handle(DE_ConfigurationNode) Copy() const = 0;

  //! Update loading status. Checking for the ability to read and write.
  //! @param[in] theToImport flag to updates for import. Standard_True-import, Standard_False-export
  //! @param[in] theToKeep flag to save update result
  //! @return Standard_True, if node can be used
  Standard_EXPORT virtual bool UpdateLoad(const Standard_Boolean theToImport,
                                          const Standard_Boolean theToKeep);

public:

  //! Checks the import supporting
  //! @return Standard_True if import is support
  Standard_EXPORT virtual bool IsImportSupported() const;

  //! Checks the export supporting
  //! @return Standard_True if export is support
  Standard_EXPORT virtual bool IsExportSupported() const;

  //! Gets CAD format name of associated provider
  //! @return provider CAD format
  Standard_EXPORT virtual TCollection_AsciiString GetFormat() const = 0;

  //! Gets provider's vendor name of associated provider
  //! @return provider's vendor name
  Standard_EXPORT virtual TCollection_AsciiString GetVendor() const = 0;

  //! Gets list of supported file extensions
  //! @return list of extensions
  Standard_EXPORT virtual TColStd_ListOfAsciiString GetExtensions() const = 0;

  //! Checks the file extension to verify a format
  //! @param[in] theExtension input file extension
  //! @return Standard_True if file is supported by a current provider
  Standard_EXPORT virtual bool CheckExtension(const TCollection_AsciiString& theExtension) const;

  //! Checks the file content to verify a format
  //! @param[in] theBuffer read stream buffer to check content
  //! @return Standard_True if file is supported by a current provider
  Standard_EXPORT virtual bool CheckContent(const Handle(NCollection_Buffer)& theBuffer) const;

public:

  //! Gets the provider loading status
  //! @return Standard_True if the load is correct
  Standard_Boolean IsEnabled() const
  {
    return myIsEnabled;
  }

  //! Sets the provider loading status
  //! @param[in] theIsLoaded input load status
  void SetEnabled(const Standard_Boolean theIsLoaded)
  {
    myIsEnabled = theIsLoaded;
  }

public:

  //!< Internal parameters for transfer process
  struct DE_SectionGlobal
  {
    Standard_Real LengthUnit = 1.0; //!< Scale length unit value from MM, default 1.0 (MM)
  } GlobalParameters;

private:

  Standard_Boolean myIsEnabled; //!< Flag to use a current provider for Read or Write process via DE_Wrapper

};

#endif // _DE_ConfigurationNode_HeaderFile
