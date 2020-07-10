// Author: Kirill Gavrilov
// Copyright (c) 2016-2019 OPEN CASCADE SAS
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

#ifndef _RWMesh_CafReader_HeaderFile
#define _RWMesh_CafReader_HeaderFile

#include <Message_ProgressRange.hxx>
#include <NCollection_IndexedMap.hxx>
#include <RWMesh_CoordinateSystemConverter.hxx>
#include <RWMesh_NodeAttributes.hxx>
#include <TColStd_IndexedDataMapOfStringString.hxx>
#include <TDF_Label.hxx>
#include <TopTools_SequenceOfShape.hxx>

class TDocStd_Document;
class XCAFDoc_ShapeTool;
class XCAFDoc_ColorTool;
class XCAFDoc_VisMaterialTool;

//! Extended status bits.
enum RWMesh_CafReaderStatusEx
{
  RWMesh_CafReaderStatusEx_NONE    = 0,    //!< empty status
  RWMesh_CafReaderStatusEx_Partial = 0x01, //!< partial read (due to unexpected EOF, syntax error, memory limit)
};

//! The general interface for importing mesh data into XDE document.
//!
//! The tool implements auxiliary structures for creating an XDE document in two steps:
//! 1) Creating TopoDS_Shape hierarchy (myRootShapes)
//!    and Shape attributes (myAttribMap) separately within performMesh().
//!    Attributes include names and styles.
//! 2) Filling XDE document from these auxiliary structures.
//!    Named elements are expanded within document structure, while Compounds having no named children will remain collapsed.
//!    In addition, unnamed nodes can be filled with generated names like "Face", "Compound" via generateNames() method,
//!    and the very root unnamed node can be filled from file name like "MyModel.obj".
class RWMesh_CafReader : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(RWMesh_CafReader, Standard_Transient)
public:

  //! Structure holding tools for filling the document.
  struct CafDocumentTools
  {
    Handle(XCAFDoc_ShapeTool)       ShapeTool;
    Handle(XCAFDoc_ColorTool)       ColorTool;
    Handle(XCAFDoc_VisMaterialTool) VisMaterialTool;
    NCollection_DataMap<TopoDS_Shape, TDF_Label, TopTools_ShapeMapHasher> ComponentMap;
  };

public:

  //! Empty constructor.
  Standard_EXPORT RWMesh_CafReader();

  //! Destructor.
  Standard_EXPORT virtual ~RWMesh_CafReader();

  //! Return target document.
  const Handle(TDocStd_Document)& Document() const { return myXdeDoc; }

  //! Set target document.
  void SetDocument (const Handle(TDocStd_Document)& theDoc) { myXdeDoc = theDoc; }

  //! Return prefix for generating root labels names.
  const TCollection_AsciiString& RootPrefix() const { return myRootPrefix; }

  //! Set prefix for generating root labels names
  void SetRootPrefix (const TCollection_AsciiString& theRootPrefix) { myRootPrefix = theRootPrefix; }

  //! Flag indicating if partially read file content should be put into the XDE document, TRUE by default.
  //!
  //! Partial read means unexpected end of file, critical parsing syntax errors in the middle of file, or reached memory limit
  //! indicated by performMesh() returning FALSE.
  //! Partial read allows importing a model even in case of formal reading failure,
  //! so that it will be up to user to decide if processed data has any value.
  //!
  //! In case of partial read (performMesh() returns FALSE, but there are some data that could be put into document),
  //! Perform() will return TRUE and result flag will have failure bit set.
  //! @sa MemoryLimitMiB(), ExtraStatus().
  Standard_Boolean ToFillIncompleteDocument() const { return myToFillIncomplete; }

  //! Set flag allowing partially read file content to be put into the XDE document.
  void SetFillIncompleteDocument (Standard_Boolean theToFillIncomplete) { myToFillIncomplete = theToFillIncomplete; }

  //! Return memory usage limit in MiB, -1 by default which means no limit.
  Standard_Integer MemoryLimitMiB() const { return myMemoryLimitMiB; }

  //! Set memory usage limit in MiB; can be ignored by reader implementation!
  void SetMemoryLimitMiB (Standard_Integer theLimitMiB) { myMemoryLimitMiB = theLimitMiB; }

public:

  //! Return coordinate system converter.
  const RWMesh_CoordinateSystemConverter& CoordinateSystemConverter() const { return myCoordSysConverter; }

  //! Set coordinate system converter.
  void SetCoordinateSystemConverter (const RWMesh_CoordinateSystemConverter& theConverter) { myCoordSysConverter = theConverter; }

  //! Return the length unit to convert into while reading the file, defined as scale factor for m (meters);
  //! -1.0 by default, which means that NO conversion will be applied.
  Standard_Real SystemLengthUnit() const { return myCoordSysConverter.OutputLengthUnit(); }

  //! Set system length units to convert into while reading the file, defined as scale factor for m (meters).
  void SetSystemLengthUnit (Standard_Real theUnits) { myCoordSysConverter.SetOutputLengthUnit (theUnits); }

  //! Return TRUE if system coordinate system has been defined; FALSE by default.
  Standard_Boolean HasSystemCoordinateSystem() const { return myCoordSysConverter.HasOutputCoordinateSystem(); }

  //! Return system coordinate system; UNDEFINED by default, which means that no conversion will be done.
  const gp_Ax3& SystemCoordinateSystem() const { return myCoordSysConverter.OutputCoordinateSystem(); }

  //! Set system origin coordinate system to perform conversion into during read.
  void SetSystemCoordinateSystem (const gp_Ax3& theCS) { myCoordSysConverter.SetOutputCoordinateSystem (theCS); }

  //! Set system origin coordinate system to perform conversion into during read.
  void SetSystemCoordinateSystem (RWMesh_CoordinateSystem theCS) { myCoordSysConverter.SetOutputCoordinateSystem (theCS); }

  //! Return the length unit to convert from while reading the file, defined as scale factor for m (meters).
  //! Can be undefined (-1.0) if file format is unitless.
  Standard_Real FileLengthUnit() const { return myCoordSysConverter.InputLengthUnit(); }

  //! Set (override) file length units to convert from while reading the file, defined as scale factor for m (meters).
  void SetFileLengthUnit (Standard_Real theUnits) { myCoordSysConverter.SetInputLengthUnit (theUnits); }

  //! Return TRUE if file origin coordinate system has been defined.
  Standard_Boolean HasFileCoordinateSystem() const { return myCoordSysConverter.HasInputCoordinateSystem(); }

  //! Return file origin coordinate system; can be UNDEFINED, which means no conversion will be done.
  const gp_Ax3& FileCoordinateSystem() const { return myCoordSysConverter.InputCoordinateSystem(); }

  //! Set (override) file origin coordinate system to perform conversion during read.
  void SetFileCoordinateSystem (const gp_Ax3& theCS) { myCoordSysConverter.SetInputCoordinateSystem (theCS); }

  //! Set (override) file origin coordinate system to perform conversion during read.
  void SetFileCoordinateSystem (RWMesh_CoordinateSystem theCS) { myCoordSysConverter.SetInputCoordinateSystem (theCS); }

public:

  //! Read the data from specified file.
  //! The Document instance should be set beforehand.
  bool Perform (const TCollection_AsciiString& theFile,
                const Message_ProgressRange& theProgress)
  {
    return perform (theFile, theProgress, Standard_False);
  }

  //! Return extended status flags.
  //! @sa RWMesh_CafReaderStatusEx enumeration.
  Standard_Integer ExtraStatus() const { return myExtraStatus; }

public:

  //! Return result as a single shape.
  Standard_EXPORT TopoDS_Shape SingleShape() const;

  //! Return the list of complementary files - external references (textures, data, etc.).
  const NCollection_IndexedMap<TCollection_AsciiString>& ExternalFiles() const { return myExternalFiles; }

  //! Return metadata map.
  const TColStd_IndexedDataMapOfStringString& Metadata() const { return myMetadata; }

  //! Read the header data from specified file without reading entire model.
  //! The main purpose is collecting metadata and external references - for copying model into a new location, for example.
  //! Can be NOT implemented (unsupported by format / reader).
  Standard_Boolean ProbeHeader (const TCollection_AsciiString& theFile,
                                const Message_ProgressRange& theProgress = Message_ProgressRange())
  {
    return perform (theFile, theProgress, Standard_True);
  }

protected:

  //! Read the data from specified file.
  //! Default implementation calls performMesh() and fills XDE document from collected shapes.
  //! @param theFile    file to read
  //! @param optional   progress indicator
  //! @param theToProbe flag indicating that mesh data should be skipped and only basing information to be read
  Standard_EXPORT virtual Standard_Boolean perform (const TCollection_AsciiString& theFile,
                                                    const Message_ProgressRange& theProgress,
                                                    const Standard_Boolean theToProbe);

  //! Read the mesh from specified file - interface to be implemented by sub-classes.
  Standard_EXPORT virtual Standard_Boolean performMesh (const TCollection_AsciiString& theFile,
                                                        const Message_ProgressRange& theProgress,
                                                        const Standard_Boolean theToProbe) = 0;

//! @name tools for filling XDE document
protected:

  //! Fill document with new root shapes.
  Standard_EXPORT void fillDocument();

  //! Append new shape into the document (recursively).
  Standard_EXPORT Standard_Boolean addShapeIntoDoc (CafDocumentTools& theTools,
                                                    const TopoDS_Shape& theShape,
                                                    const TDF_Label& theLabel,
                                                    const TCollection_AsciiString& theParentName);

  //! Append new sub-shape into the document (recursively).
  Standard_EXPORT Standard_Boolean addSubShapeIntoDoc (CafDocumentTools& theTools,
                                                       const TopoDS_Shape& theShape,
                                                       const TDF_Label& theParentLabel,
                                                       const RWMesh_NodeAttributes& theAttribs);

  //! Put name attribute onto the label.
  Standard_EXPORT void setShapeName (const TDF_Label& theLabel,
                                     const TopAbs_ShapeEnum theShapeType,
                                     const TCollection_AsciiString& theName,
                                     const TDF_Label& theParentLabel,
                                     const TCollection_AsciiString& theParentName);

  //! Put color and material attributes onto the label.
  Standard_EXPORT void setShapeStyle (const CafDocumentTools& theTools,
                                      const TDF_Label& theLabel,
                                      const XCAFPrs_Style& theStyle);

  //! Put name data (metadata) attribute onto the label.
  Standard_EXPORT void setShapeNamedData (const CafDocumentTools& theTools,
                                          const TDF_Label& theLabel,
                                          const Handle(TDataStd_NamedData)& theNameData);

  //! Generate names for root labels starting from specified index.
  Standard_EXPORT void generateNames (const TCollection_AsciiString& theFile,
                                      const Standard_Integer theRootLower,
                                      const Standard_Boolean theWithSubLabels);

  //! Return shape type as string.
  //! @sa TopAbs::ShapeTypeToString()
  static TCollection_AsciiString shapeTypeToString (TopAbs_ShapeEnum theType)
  {
    TCollection_AsciiString aString = TopAbs::ShapeTypeToString (theType);
    aString.Capitalize();
    return aString;
  }

protected:

  Handle(TDocStd_Document)  myXdeDoc;            //!< target document

  TColStd_IndexedDataMapOfStringString
                            myMetadata;          //!< metadata map
  NCollection_IndexedMap<TCollection_AsciiString>
                            myExternalFiles;     //!< the list of complementary files - external references (textures, data, etc.)
  TCollection_AsciiString   myRootPrefix;        //!< root folder for generating root labels names
  TopTools_SequenceOfShape  myRootShapes;        //!< sequence of result root shapes
  RWMesh_NodeAttributeMap   myAttribMap;         //!< map of per-shape attributes

  RWMesh_CoordinateSystemConverter
                            myCoordSysConverter; //!< coordinate system converter
  Standard_Boolean          myToFillDoc;         //!< fill document from shape sequence
  Standard_Boolean          myToFillIncomplete;  //!< fill the document with partially retrieved data even if reader has failed with error
  Standard_Integer          myMemoryLimitMiB;    //!< memory usage limit
  Standard_Integer          myExtraStatus;       //!< extra status bitmask

};

#endif // _RWMesh_CafReader_HeaderFile
