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

#ifndef _DEOBJ_ConfigurationNode_HeaderFile
#define _DEOBJ_ConfigurationNode_HeaderFile

#include <DE_ConfigurationNode.hxx>
#include <RWMesh_CoordinateSystem.hxx>

//! The purpose of this class is to configure the transfer process for OBJ format
//! Stores the necessary settings for DEOBJ_Provider.
//! Configures and creates special provider to transfer OBJ files.
//!
//! Nodes grouped by Vendor name and Format type.
//! The Vendor name is "OCC"
//! The Format type is "OBJ"
//! The supported CAD extension is ".obj"
//! The import process is supported.
//! The export process is supported.
class DEOBJ_ConfigurationNode : public DE_ConfigurationNode
{
  DEFINE_STANDARD_RTTIEXT(DEOBJ_ConfigurationNode, DE_ConfigurationNode)
public:
  //! Initializes all field by default
  Standard_EXPORT DEOBJ_ConfigurationNode();

  //! Copies values of all fields
  //! @param[in] theNode object to copy
  Standard_EXPORT DEOBJ_ConfigurationNode(const Handle(DEOBJ_ConfigurationNode)& theNode);

  //! Updates values according the resource
  //! @param[in] theResource input resource to use
  //! @return true if theResource loading has ended correctly
  Standard_EXPORT virtual bool Load(const Handle(DE_ConfigurationContext)& theResource)
    Standard_OVERRIDE;

  //! Writes configuration to the string
  //! @return result resource string
  Standard_EXPORT virtual TCollection_AsciiString Save() const Standard_OVERRIDE;

  //! Copies values of all fields
  //! @return new object with the same field values
  Standard_EXPORT virtual Handle(DE_ConfigurationNode) Copy() const Standard_OVERRIDE;

  //! Creates new provider for the own format
  //! @return new created provider
  Standard_EXPORT virtual Handle(DE_Provider) BuildProvider() Standard_OVERRIDE;

public:
  //! Checks the import supporting
  //! @return true if import is supported
  Standard_EXPORT virtual bool IsImportSupported() const Standard_OVERRIDE;

  //! Checks the export supporting
  //! @return true if export is supported
  Standard_EXPORT virtual bool IsExportSupported() const Standard_OVERRIDE;

  //! Gets CAD format name of associated provider
  //! @return provider CAD format
  Standard_EXPORT virtual TCollection_AsciiString GetFormat() const Standard_OVERRIDE;

  //! Gets provider's vendor name of associated provider
  //! @return provider's vendor name
  Standard_EXPORT virtual TCollection_AsciiString GetVendor() const Standard_OVERRIDE;

  //! Gets list of supported file extensions
  //! @return list of extensions
  Standard_EXPORT virtual TColStd_ListOfAsciiString GetExtensions() const Standard_OVERRIDE;

public:
  struct RWObj_InternalSection
  {
    // Common
    // clang-format off
    double FileLengthUnit = 1.; //!< File length units to convert from while reading the file, defined as scale factor for m (meters)
    RWMesh_CoordinateSystem SystemCS = RWMesh_CoordinateSystem_Zup; //!< System origin coordinate system to perform conversion into during read
    RWMesh_CoordinateSystem FileCS = RWMesh_CoordinateSystem_Yup; //!< File origin coordinate system to perform conversion during read
    // Reading
    bool ReadSinglePrecision = false; //!< Flag for reading vertex data with single or double floating point precision
    bool ReadCreateShapes = false;  //!< Flag for create a single triangulation
    TCollection_AsciiString ReadRootPrefix; //!< Root folder for generating root labels names
    bool ReadFillDoc = true; //!< Flag for fill document from shape sequence
    bool ReadFillIncomplete = true; //!< Flag for fill the document with partially retrieved data even if reader has failed with error
    // clang-format on
    int ReadMemoryLimitMiB = -1; //!< Memory usage limit
    // Writing
    TCollection_AsciiString WriteComment; //!< Export special comment
    TCollection_AsciiString WriteAuthor;  //!< Author of exported file name
  } InternalParameters;
};

#endif // _DEOBJ_ConfigurationNode_HeaderFile
