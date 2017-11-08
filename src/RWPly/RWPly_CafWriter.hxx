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

#ifndef _RWPly_CafWriter_HeaderFiler
#define _RWPly_CafWriter_HeaderFiler

#include <TColStd_IndexedDataMapOfStringString.hxx>
#include <TColStd_MapOfAsciiString.hxx>
#include <TDF_LabelSequence.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <RWMesh_CoordinateSystemConverter.hxx>
#include <XCAFPrs_Style.hxx>

#include <memory>

class Message_ProgressRange;
class RWMesh_FaceIterator;
class TDocStd_Document;

class Message_LazyProgressScope;
class RWPly_PlyWriterContext;

//! PLY writer context from XCAF document.
class RWPly_CafWriter : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(RWPly_CafWriter, Standard_Transient)
public:

  //! Main constructor.
  //! @param[in] theFile path to output PLY file
  Standard_EXPORT RWPly_CafWriter (const TCollection_AsciiString& theFile);

  //! Destructor.
  Standard_EXPORT virtual ~RWPly_CafWriter();

  //! Return transformation from OCCT to PLY coordinate system.
  const RWMesh_CoordinateSystemConverter& CoordinateSystemConverter() const { return myCSTrsf; }

  //! Return transformation from OCCT to PLY coordinate system.
  RWMesh_CoordinateSystemConverter& ChangeCoordinateSystemConverter() { return myCSTrsf; }

  //! Set transformation from OCCT to PLY coordinate system.
  void SetCoordinateSystemConverter (const RWMesh_CoordinateSystemConverter& theConverter) { myCSTrsf = theConverter; }

  //! Return default material definition to be used for nodes with only color defined.
  const XCAFPrs_Style& DefaultStyle() const { return myDefaultStyle; }

  //! Set default material definition to be used for nodes with only color defined.
  void SetDefaultStyle (const XCAFPrs_Style& theStyle) { myDefaultStyle = theStyle; }

public:

  //! Return TRUE if vertex position should be stored with double floating point precision; FALSE by default.
  bool IsDoublePrecision() const { return myIsDoublePrec; }

  //! Set if vertex position should be stored with double floating point precision.
  void SetDoublePrecision (bool theDoublePrec) { myIsDoublePrec = theDoublePrec; }

  //! Return TRUE if normals should be written; TRUE by default.
  bool HasNormals() const { return myHasNormals; }

  //! Set if normals are defined.
  void SetNormals (const bool theHasNormals) { myHasNormals = theHasNormals; }

  //! Return TRUE if UV / texture coordinates should be written; FALSE by default.
  bool HasTexCoords() const { return myHasTexCoords; }

  //! Set if UV / texture coordinates should be written.
  void SetTexCoords (const bool theHasTexCoords) { myHasTexCoords = theHasTexCoords; }

  //! Return TRUE if point colors should be written; TRUE by default.
  bool HasColors() const { return myHasColors; }

  //! Set if point colors should be written.
  void SetColors (bool theToWrite) { myHasColors = theToWrite; }

  //! Return TRUE if part Id should be written as element attribute; TRUE by default.
  bool HasPartId() const { return myHasPartId; }

  //! Set if part Id should be written as element attribute; FALSE by default.
  //! Cannot be combined with HasFaceId().
  void SetPartId (bool theSurfId)
  {
    myHasPartId = theSurfId;
    myHasFaceId = myHasFaceId && !myHasPartId;
  }

  //! Return TRUE if face Id should be written as element attribute; FALSE by default.
  bool HasFaceId() const { return myHasFaceId; }

  //! Set if face Id should be written as element attribute; FALSE by default.
  //! Cannot be combined with HasPartId().
  void SetFaceId (bool theSurfId)
  {
    myHasFaceId = theSurfId;
    myHasPartId = myHasPartId && !myHasFaceId;
  }

public:

  //! Write PLY file and associated MTL material file.
  //! Triangulation data should be precomputed within shapes!
  //! @param[in] theDocument    input document
  //! @param[in] theRootLabels  list of root shapes to export
  //! @param[in] theLabelFilter optional filter with document nodes to export,
  //!                           with keys defined by XCAFPrs_DocumentExplorer::DefineChildId() and filled recursively
  //!                           (leaves and parent assembly nodes at all levels);
  //!                           when not NULL, all nodes not included into the map will be ignored
  //! @param[in] theFileInfo    map with file metadata to put into PLY header section
  //! @param[in] theProgress    optional progress indicator
  //! @return FALSE on file writing failure
  Standard_EXPORT virtual bool Perform (const Handle(TDocStd_Document)& theDocument,
                                        const TDF_LabelSequence& theRootLabels,
                                        const TColStd_MapOfAsciiString* theLabelFilter,
                                        const TColStd_IndexedDataMapOfStringString& theFileInfo,
                                        const Message_ProgressRange& theProgress);

  //! Write PLY file and associated MTL material file.
  //! Triangulation data should be precomputed within shapes!
  //! @param[in] theDocument input document
  //! @param[in] theFileInfo map with file metadata to put into PLY header section
  //! @param[in] theProgress optional progress indicator
  //! @return FALSE on file writing failure
  Standard_EXPORT virtual bool Perform (const Handle(TDocStd_Document)& theDocument,
                                        const TColStd_IndexedDataMapOfStringString& theFileInfo,
                                        const Message_ProgressRange& theProgress);

protected:

  //! Return TRUE if face mesh should be skipped (e.g. because it is invalid or empty).
  Standard_EXPORT virtual Standard_Boolean toSkipFaceMesh (const RWMesh_FaceIterator& theFaceIter);

  //! Collect face triangulation info.
  //! @param[in] theFace face to process
  //! @param[in,out] theNbNodes overall number of triangulation nodes (should be appended)
  //! @param[in,out] theNbElems overall number of triangulation elements (should be appended)
  Standard_EXPORT virtual void addFaceInfo (const RWMesh_FaceIterator& theFace,
                                            Standard_Integer& theNbNodes,
                                            Standard_Integer& theNbElems);

  //! Write the shape.
  //! @param[in] theWriter      PLY writer context
  //! @param[in] thePSentry     progress sentry
  //! @param[in] theWriteStep   export step, 0 for vertex attributes, 1 for elements
  //! @param[in] theLabel       document label to process
  //! @param[in] theParentTrsf  parent node transformation
  //! @param[in] theParentStyle parent node style
  Standard_EXPORT virtual bool writeShape (RWPly_PlyWriterContext& theWriter,
                                           Message_LazyProgressScope& thePSentry,
                                           const Standard_Integer theWriteStep,
                                           const TDF_Label& theLabel,
                                           const TopLoc_Location& theParentTrsf,
                                           const XCAFPrs_Style& theParentStyle);

  //! Write face triangle vertices and attributes.
  //! @param[in] theWriter  PLY writer context
  //! @param[in] thePSentry progress sentry
  //! @param[in] theFace    current face
  //! @return FALSE on writing file error
  Standard_EXPORT virtual bool writeNodes (RWPly_PlyWriterContext& theWriter,
                                           Message_LazyProgressScope& thePSentry,
                                           const RWMesh_FaceIterator& theFace);

  //! Write face triangles indices.
  //! @param[in] theWriter  PLY writer context
  //! @param[in] thePSentry progress sentry
  //! @param[in] theFace    current face
  //! @return FALSE on writing file error
  Standard_EXPORT virtual bool writeIndices (RWPly_PlyWriterContext& theWriter,
                                             Message_LazyProgressScope& thePSentry,
                                             const RWMesh_FaceIterator& theFace);


protected:

  TCollection_AsciiString          myFile;         //!< output PLY file
  RWMesh_CoordinateSystemConverter myCSTrsf;       //!< transformation from OCCT to PLY coordinate system
  XCAFPrs_Style                    myDefaultStyle; //!< default material definition to be used for nodes with only color defined
  Standard_Boolean                 myIsDoublePrec;
  Standard_Boolean                 myHasNormals;
  Standard_Boolean                 myHasColors;
  Standard_Boolean                 myHasTexCoords;
  Standard_Boolean                 myHasPartId;
  Standard_Boolean                 myHasFaceId;

};

#endif // _RWPly_CafWriter_HeaderFiler
