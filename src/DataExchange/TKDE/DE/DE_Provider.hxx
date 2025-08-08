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

#ifndef _DE_Provider_HeaderFile
#define _DE_Provider_HeaderFile

#include <Message_ProgressRange.hxx>
#include <NCollection_List.hxx>
#include <Standard_IStream.hxx>
#include <Standard_OStream.hxx>

class DE_ConfigurationNode;
class TopoDS_Shape;
class XSControl_WorkSession;
class TDocStd_Document;

//! Base class to make transfer process.
//! Reads or Writes specialized CAD files into/from OCCT.
//! Each operation needs the Configuration Node.
//!
//! Providers are grouped by Vendor's name and Format type.
//! The Vendor name is not defined by default.
//! The Format type is not defined by default.
//! The import process is not supported.
//! The export process is not supported.
//!
//! The algorithm for standalone transfer operation:
//! 1) Create new empty Provider object
//! 2) Configure the current object by special Configuration Node (::SetNode)
//! 3) Initiate the transfer process:
//!   3.1) Call the required Read method (if Read methods are implemented)
//!   3.2) Call the required Write method (if Write methods are implemented)
//! 4) Validate the output values
class DE_Provider : public Standard_Transient
{
public:
  DEFINE_STANDARD_RTTIEXT(DE_Provider, Standard_Transient)

  //! Node to store write stream information
  //! Contains relative path and reference to output stream
  struct WriteStreamNode
  {
    TCollection_AsciiString Path;   //!< Relative path to the output file
    Standard_OStream&       Stream; //!< Reference to output stream

    //! Constructor
    WriteStreamNode(const TCollection_AsciiString& thePath, Standard_OStream& theStream)
        : Path(thePath),
          Stream(theStream)
    {
    }
  };

  //! Node to store read stream information
  //! Contains relative path and reference to input stream
  struct ReadStreamNode
  {
    TCollection_AsciiString Path;   //!< Relative path to the input file
    Standard_IStream&       Stream; //!< Reference to input stream

    //! Constructor
    ReadStreamNode(const TCollection_AsciiString& thePath, Standard_IStream& theStream)
        : Path(thePath),
          Stream(theStream)
    {
    }
  };

public:
  //! List to store write stream nodes
  //! First element is the main stream, others are for internal referencing
  using WriteStreamList = NCollection_List<WriteStreamNode>;

  //! List to store read stream nodes
  //! First element is the main stream, others are for internal referencing
  using ReadStreamList = NCollection_List<ReadStreamNode>;

public:
  //! Default constructor
  //! Configure translation process with global configuration
  Standard_EXPORT DE_Provider();

  //! Configure translation process
  //! @param[in] theNode object to copy
  Standard_EXPORT DE_Provider(const Handle(DE_ConfigurationNode)& theNode);

public:
  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theDocument document to save result
  //! @param[in] theWS current work session
  //! @param[in] theProgress progress indicator
  //! @return True if Read was successful
  Standard_EXPORT virtual Standard_Boolean Read(
    const TCollection_AsciiString&  thePath,
    const Handle(TDocStd_Document)& theDocument,
    Handle(XSControl_WorkSession)&  theWS,
    const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Writes a CAD file, according internal configuration
  //! @param[in] thePath path to the export CAD file
  //! @param[out] theDocument document to export
  //! @param[in] theWS current work session
  //! @param[in] theProgress progress indicator
  //! @return True if Write was successful
  Standard_EXPORT virtual Standard_Boolean Write(
    const TCollection_AsciiString&  thePath,
    const Handle(TDocStd_Document)& theDocument,
    Handle(XSControl_WorkSession)&  theWS,
    const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Reads streams according to internal configuration
  //! @param[in] theStreams streams to read from
  //! @param[out] theDocument document to save result
  //! @param[in] theWS current work session
  //! @param[in] theProgress progress indicator
  //! @return True if Read was successful
  Standard_EXPORT virtual Standard_Boolean Read(
    ReadStreamList&                 theStreams,
    const Handle(TDocStd_Document)& theDocument,
    Handle(XSControl_WorkSession)&  theWS,
    const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Writes streams according to internal configuration
  //! @param[in] theStreams streams to write to
  //! @param[out] theDocument document to export
  //! @param[in] theWS current work session
  //! @param[in] theProgress progress indicator
  //! @return True if Write was successful
  Standard_EXPORT virtual Standard_Boolean Write(
    WriteStreamList&                theStreams,
    const Handle(TDocStd_Document)& theDocument,
    Handle(XSControl_WorkSession)&  theWS,
    const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theDocument document to save result
  //! @param[in] theProgress progress indicator
  //! @return True if Read was successful
  Standard_EXPORT virtual Standard_Boolean Read(
    const TCollection_AsciiString&  thePath,
    const Handle(TDocStd_Document)& theDocument,
    const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Writes a CAD file, according internal configuration
  //! @param[in] thePath path to the export CAD file
  //! @param[out] theDocument document to export
  //! @param[in] theProgress progress indicator
  //! @return True if Write was successful
  Standard_EXPORT virtual Standard_Boolean Write(
    const TCollection_AsciiString&  thePath,
    const Handle(TDocStd_Document)& theDocument,
    const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Reads streams according to internal configuration
  //! @param[in] theStreams streams to read from
  //! @param[out] theDocument document to save result
  //! @param[in] theProgress progress indicator
  //! @return True if Read was successful
  Standard_EXPORT virtual Standard_Boolean Read(
    ReadStreamList&                 theStreams,
    const Handle(TDocStd_Document)& theDocument,
    const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Writes streams according to internal configuration
  //! @param[in] theStreams streams to write to
  //! @param[out] theDocument document to export
  //! @param[in] theProgress progress indicator
  //! @return True if Write was successful
  Standard_EXPORT virtual Standard_Boolean Write(
    WriteStreamList&                theStreams,
    const Handle(TDocStd_Document)& theDocument,
    const Message_ProgressRange&    theProgress = Message_ProgressRange());

  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theShape shape to save result
  //! @param[in] theWS current work session
  //! @param[in] theProgress progress indicator
  //! @return True if Read was successful
  Standard_EXPORT virtual Standard_Boolean Read(
    const TCollection_AsciiString& thePath,
    TopoDS_Shape&                  theShape,
    Handle(XSControl_WorkSession)& theWS,
    const Message_ProgressRange&   theProgress = Message_ProgressRange());

  //! Writes a CAD file, according internal configuration
  //! @param[in] thePath path to the export CAD file
  //! @param[out] theShape shape to export
  //! @param[in] theWS current work session
  //! @param[in] theProgress progress indicator
  //! @return True if Write was successful
  Standard_EXPORT virtual Standard_Boolean Write(
    const TCollection_AsciiString& thePath,
    const TopoDS_Shape&            theShape,
    Handle(XSControl_WorkSession)& theWS,
    const Message_ProgressRange&   theProgress = Message_ProgressRange());

  //! Reads streams according to internal configuration
  //! @param[in] theStreams streams to read from
  //! @param[out] theShape shape to save result
  //! @param[in] theWS current work session
  //! @param[in] theProgress progress indicator
  //! @return True if Read was successful
  Standard_EXPORT virtual Standard_Boolean Read(
    ReadStreamList&                theStreams,
    TopoDS_Shape&                  theShape,
    Handle(XSControl_WorkSession)& theWS,
    const Message_ProgressRange&   theProgress = Message_ProgressRange());

  //! Writes streams according to internal configuration
  //! @param[in] theStreams streams to write to
  //! @param[out] theShape shape to export
  //! @param[in] theWS current work session
  //! @param[in] theProgress progress indicator
  //! @return True if Write was successful
  Standard_EXPORT virtual Standard_Boolean Write(
    WriteStreamList&               theStreams,
    const TopoDS_Shape&            theShape,
    Handle(XSControl_WorkSession)& theWS,
    const Message_ProgressRange&   theProgress = Message_ProgressRange());

  //! Reads a CAD file, according internal configuration
  //! @param[in] thePath path to the import CAD file
  //! @param[out] theShape shape to save result
  //! @param[in] theProgress progress indicator
  //! @return True if Read was successful
  Standard_EXPORT virtual Standard_Boolean Read(
    const TCollection_AsciiString& thePath,
    TopoDS_Shape&                  theShape,
    const Message_ProgressRange&   theProgress = Message_ProgressRange());

  //! Writes a CAD file, according internal configuration
  //! @param[in] thePath path to the export CAD file
  //! @param[out] theShape shape to export
  //! @param[in] theProgress progress indicator
  //! @return True if Write was successful
  Standard_EXPORT virtual Standard_Boolean Write(
    const TCollection_AsciiString& thePath,
    const TopoDS_Shape&            theShape,
    const Message_ProgressRange&   theProgress = Message_ProgressRange());

  //! Reads streams according to internal configuration
  //! @param[in] theStreams streams to read from
  //! @param[out] theShape shape to save result
  //! @param[in] theProgress progress indicator
  //! @return True if Read was successful
  Standard_EXPORT virtual Standard_Boolean Read(
    ReadStreamList&              theStreams,
    TopoDS_Shape&                theShape,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Writes streams according to internal configuration
  //! @param[in] theStreams streams to write to
  //! @param[out] theShape shape to export
  //! @param[in] theProgress progress indicator
  //! @return True if Write was successful
  Standard_EXPORT virtual Standard_Boolean Write(
    WriteStreamList&             theStreams,
    const TopoDS_Shape&          theShape,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

public:
  //! Gets CAD format name of associated provider
  //! @return provider CAD format
  Standard_EXPORT virtual TCollection_AsciiString GetFormat() const = 0;

  //! Gets provider's vendor name of associated provider
  //! @return provider's vendor name
  Standard_EXPORT virtual TCollection_AsciiString GetVendor() const = 0;

  //! Gets internal configuration node
  //! @return configuration node object
  Handle(DE_ConfigurationNode) GetNode() const { return myNode; }

  //! Sets internal configuration node
  //! @param[in] theNode configuration node to set
  void SetNode(const Handle(DE_ConfigurationNode)& theNode) { myNode = theNode; }

private:
  Handle(DE_ConfigurationNode) myNode; //!< Internal configuration for the own format
};

#endif // _DE_Provider_HeaderFile
