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

#ifndef _RWGltf_Provider_HeaderFile
#define _RWGltf_Provider_HeaderFile

#include <DE_Provider.hxx>
#include <RWGltf_CafReader.hxx>
#include <RWGltf_ConfigurationNode.hxx>

//! The class to transfer glTF files.
//! Reads and Writes any glTF files into/from OCCT.
//! Each operation needs configuration node.
//!
//! Providers grouped by Vendor name and Format type.
//! The Vendor name is "OCC"
//! The Format type is "GLTF"
//! The import process is supported.
//! The export process is supported.
class RWGltf_Provider : public DE_Provider
{
public:
  DEFINE_STANDARD_RTTIEXT(RWGltf_Provider, DE_Provider)

public:

  //! Default constructor
  //! Configure translation process with global configuration
  Standard_EXPORT RWGltf_Provider();

  //! Configure translation process
  //! @param[in] theNode object to copy
  Standard_EXPORT RWGltf_Provider(const Handle(DE_ConfigurationNode)& theNode);

public:

  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theDocument document to save result
  //! @param[in] theWS current work session
  //! @param theProgress[in] progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT virtual bool Read(const TCollection_AsciiString& thePath,
                                    const  Handle(TDocStd_Document)& theDocument,
                                    Handle(XSControl_WorkSession)& theWS,
                                    const Message_ProgressRange& theProgress = Message_ProgressRange()) Standard_OVERRIDE;

  //! Writes a CAD file, according internal configuration
  //! @param[in] thePath path to the export CAD file
  //! @param[out] theDocument document to export
  //! @param[in] theWS current work session
  //! @param theProgress[in] progress indicator
  //! @return true if Write operation has ended correctly
  Standard_EXPORT virtual bool Write(const TCollection_AsciiString& thePath,
                                     const Handle(TDocStd_Document)& theDocument,
                                     Handle(XSControl_WorkSession)& theWS,
                                     const Message_ProgressRange& theProgress = Message_ProgressRange()) Standard_OVERRIDE;

  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theDocument document to save result
  //! @param theProgress[in] progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT virtual bool Read(const TCollection_AsciiString& thePath,
                                    const Handle(TDocStd_Document)& theDocument,
                                    const Message_ProgressRange& theProgress = Message_ProgressRange()) Standard_OVERRIDE;

  //! Writes a CAD file, according internal configuration
  //! @param[in] thePath path to the export CAD file
  //! @param[out] theDocument document to export
  //! @param theProgress[in] progress indicator
  //! @return true if Write operation has ended correctly
  Standard_EXPORT virtual bool Write(const TCollection_AsciiString& thePath,
                                     const Handle(TDocStd_Document)& theDocument,
                                     const Message_ProgressRange& theProgress = Message_ProgressRange()) Standard_OVERRIDE;

  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theShape shape to save result
  //! @param[in] theWS current work session
  //! @param theProgress[in] progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT virtual bool Read(const TCollection_AsciiString& thePath,
                                    TopoDS_Shape& theShape,
                                    Handle(XSControl_WorkSession)& theWS,
                                    const Message_ProgressRange& theProgress = Message_ProgressRange()) Standard_OVERRIDE;

  //! Writes a CAD file, according internal configuration
  //! @param[in] thePath path to the export CAD file
  //! @param[out] theShape shape to export
  //! @param[in] theWS current work session
  //! @param theProgress[in] progress indicator
  //! @return true if Write operation has ended correctly
  Standard_EXPORT virtual bool Write(const TCollection_AsciiString& thePath,
                                     const TopoDS_Shape& theShape,
                                     Handle(XSControl_WorkSession)& theWS,
                                     const Message_ProgressRange& theProgress = Message_ProgressRange()) Standard_OVERRIDE;

  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theShape shape to save result
  //! @param theProgress[in] progress indicator
  //! @return true if Read operation has ended correctly
  Standard_EXPORT virtual bool Read(const TCollection_AsciiString& thePath,
                                    TopoDS_Shape& theShape,
                                    const Message_ProgressRange& theProgress = Message_ProgressRange()) Standard_OVERRIDE;

  //! Writes a CAD file, according internal configuration
  //! @param[in] thePath path to the export CAD file
  //! @param[out] theShape shape to export
  //! @param theProgress[in] progress indicator
  //! @return true if Write operation has ended correctly
  Standard_EXPORT virtual bool Write(const TCollection_AsciiString& thePath,
                                     const TopoDS_Shape& theShape,
                                     const Message_ProgressRange& theProgress = Message_ProgressRange()) Standard_OVERRIDE;

public:

  //! Gets CAD format name of associated provider
  //! @return provider CAD format
  Standard_EXPORT virtual TCollection_AsciiString GetFormat() const Standard_OVERRIDE;

  //! Gets provider's vendor name of associated provider
  //! @return provider's vendor name
  Standard_EXPORT virtual TCollection_AsciiString GetVendor() const Standard_OVERRIDE;

};

#endif // _RWGltf_Provider_HeaderFile
