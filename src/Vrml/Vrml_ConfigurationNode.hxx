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

#ifndef _Vrml_ConfigurationNode_HeaderFile
#define _Vrml_ConfigurationNode_HeaderFile

#include <DE_ConfigurationNode.hxx>
#include <RWMesh_CoordinateSystem.hxx>

//! The purpose of this class is to configure the transfer process for VRML format
//! Stores the necessary settings for Vrml_Provider.
//! Configures and creates special provider to transfer VRML files.
//!
//! Nodes grouped by Vendor name and Format type.
//! The Vendor name is "OCC"
//! The Format type is "VRML"
//! The supported CAD extensions are ".vrml", ".wrl"
//! The import process is supported.
//! The export process is supported.
class Vrml_ConfigurationNode : public DE_ConfigurationNode
{
  DEFINE_STANDARD_RTTIEXT(Vrml_ConfigurationNode, DE_ConfigurationNode)
public:

  //! Initializes all field by default
  Standard_EXPORT Vrml_ConfigurationNode();

  //! Copies values of all fields
  //! @param[in] theNode object to copy
  Standard_EXPORT Vrml_ConfigurationNode(const Handle(Vrml_ConfigurationNode)& theNode);

  //! Updates values according the resource
  //! @param[in] theResource input resource to use
  //! @return true if theResource loading has ended correctly
  Standard_EXPORT virtual bool Load(const Handle(DE_ConfigurationContext)& theResource) Standard_OVERRIDE;

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
  enum WriteMode_WriterVersion
  {
    WriteMode_WriterVersion_1 = 1,
    WriteMode_WriterVersion_2
  };
  enum WriteMode_RepresentationType
  {
    WriteMode_RepresentationType_Shaded = 0,
    WriteMode_RepresentationType_Wireframe,
    WriteMode_RepresentationType_Both
  };

  struct Vrml_InternalSection
  {
    // Read
    double ReadFileUnit = 1.; //<! file length units to convert from while reading the file, defined as scale factor for meters
    RWMesh_CoordinateSystem ReadFileCoordinateSys = RWMesh_CoordinateSystem_Yup; //<! coordinate system defined by Vrml file
    RWMesh_CoordinateSystem ReadSystemCoordinateSys = RWMesh_CoordinateSystem_Zup; //<! result coordinate system 
    bool ReadFillIncomplete = true; //<! fill the document with partially retrieved data even if reader has failed with error

    // Write
    WriteMode_WriterVersion WriterVersion = WriteMode_WriterVersion_2; //!< Setting up writer version (1/2)
    WriteMode_RepresentationType WriteRepresentationType = WriteMode_RepresentationType_Wireframe; //!< Setting up representation (shaded/wireframe/both) 

  } InternalParameters;
};

#endif // _Vrml_ConfigurationNode_HeaderFile
