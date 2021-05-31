// Copyright (c) 2015-2021 OPEN CASCADE SAS
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

#ifndef _RWObj_CafWriter_HeaderFiler
#define _RWObj_CafWriter_HeaderFiler

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
class RWObj_ObjWriterContext;
class RWObj_ObjMaterialMap;

//! OBJ writer context from XCAF document.
class RWObj_CafWriter : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(RWObj_CafWriter, Standard_Transient)
public:

  //! Main constructor.
  //! @param theFile [in] path to output OBJ file
  Standard_EXPORT RWObj_CafWriter (const TCollection_AsciiString& theFile);

  //! Destructor.
  Standard_EXPORT virtual ~RWObj_CafWriter();

  //! Return transformation from OCCT to OBJ coordinate system.
  const RWMesh_CoordinateSystemConverter& CoordinateSystemConverter() const { return myCSTrsf; }

  //! Return transformation from OCCT to OBJ coordinate system.
  RWMesh_CoordinateSystemConverter& ChangeCoordinateSystemConverter() { return myCSTrsf; }

  //! Set transformation from OCCT to OBJ coordinate system.
  void SetCoordinateSystemConverter (const RWMesh_CoordinateSystemConverter& theConverter) { myCSTrsf = theConverter; }

  //! Return default material definition to be used for nodes with only color defined.
  const XCAFPrs_Style& DefaultStyle() const { return myDefaultStyle; }

  //! Set default material definition to be used for nodes with only color defined.
  void SetDefaultStyle (const XCAFPrs_Style& theStyle) { myDefaultStyle = theStyle; }

  //! Write OBJ file and associated MTL material file.
  //! Triangulation data should be precomputed within shapes!
  //! @param theDocument    [in] input document
  //! @param theRootLabels  [in] list of root shapes to export
  //! @param theLabelFilter [in] optional filter with document nodes to export,
  //!                            with keys defined by XCAFPrs_DocumentExplorer::DefineChildId() and filled recursively
  //!                            (leaves and parent assembly nodes at all levels);
  //!                            when not NULL, all nodes not included into the map will be ignored
  //! @param theFileInfo    [in] map with file metadata to put into OBJ header section
  //! @param theProgress    [in] optional progress indicator
  //! @return FALSE on file writing failure
  Standard_EXPORT virtual bool Perform (const Handle(TDocStd_Document)& theDocument,
                                        const TDF_LabelSequence& theRootLabels,
                                        const TColStd_MapOfAsciiString* theLabelFilter,
                                        const TColStd_IndexedDataMapOfStringString& theFileInfo,
                                        const Message_ProgressRange& theProgress);

  //! Write OBJ file and associated MTL material file.
  //! Triangulation data should be precomputed within shapes!
  //! @param theDocument    [in] input document
  //! @param theFileInfo    [in] map with file metadata to put into glTF header section
  //! @param theProgress    [in] optional progress indicator
  //! @return FALSE on file writing failure
  Standard_EXPORT virtual bool Perform (const Handle(TDocStd_Document)& theDocument,
                                        const TColStd_IndexedDataMapOfStringString& theFileInfo,
                                        const Message_ProgressRange& theProgress);

protected:

  //! Return TRUE if face mesh should be skipped (e.g. because it is invalid or empty).
  Standard_EXPORT virtual Standard_Boolean toSkipFaceMesh (const RWMesh_FaceIterator& theFaceIter);

  //! Collect face triangulation info.
  //! @param theFace [in] face to process
  //! @param theNbNodes [in] [out] overall number of triangulation nodes (should be appended)
  //! @param theNbElems [in] [out] overall number of triangulation elements (should be appended)
  //! @param theNbProgressSteps [in] [out] overall number of progress steps (should be appended)
  //! @param theToCreateMatFile [in] [out] flag to create material file or not (should be appended)
  Standard_EXPORT virtual void addFaceInfo (const RWMesh_FaceIterator& theFace,
                                            Standard_Integer& theNbNodes,
                                            Standard_Integer& theNbElems,
                                            Standard_Real& theNbProgressSteps,
                                            Standard_Boolean& theToCreateMatFile);

  //! Write the shape.
  //! @param theWriter  [in] OBJ writer context
  //! @param theMatMgr  [in] OBJ material map
  //! @param thePSentry [in] progress sentry
  //! @param theLabel   [in] document label to process
  //! @param theParentTrsf  [in] parent node transformation
  //! @param theParentStyle [in] parent node style
  //! @param theName    [in] node name
  Standard_EXPORT virtual bool writeShape (RWObj_ObjWriterContext&        theWriter,
                                           RWObj_ObjMaterialMap&          theMatMgr,
                                           Message_LazyProgressScope&     thePSentry,
                                           const TDF_Label&               theLabel,
                                           const TopLoc_Location&         theParentTrsf,
                                           const XCAFPrs_Style&           theParentStyle,
                                           const TCollection_AsciiString& theName);

  //! Write face triangle vertex positions.
  //! @param theWriter  [in] OBJ writer context
  //! @param thePSentry [in] progress sentry
  //! @param theFace    [in] current face
  //! @return FALSE on writing file error
  Standard_EXPORT virtual bool writePositions (RWObj_ObjWriterContext&    theWriter,
                                               Message_LazyProgressScope& thePSentry,
                                               const RWMesh_FaceIterator& theFace);

  //! Write face triangle vertex normals.
  //! @param theWriter  [in] OBJ writer context
  //! @param thePSentry [in] progress sentry
  //! @param theFace    [in] current face
  //! @return FALSE on writing file error
  Standard_EXPORT virtual bool writeNormals (RWObj_ObjWriterContext&    theWriter,
                                             Message_LazyProgressScope& thePSentry,
                                             const RWMesh_FaceIterator& theFace);

  //! Write face triangle vertex texture coordinates.
  //! @param theWriter  [in] OBJ writer context
  //! @param thePSentry [in] progress sentry
  //! @param theFace    [in] current face
  //! @return FALSE on writing file error
  Standard_EXPORT virtual bool writeTextCoords (RWObj_ObjWriterContext&    theWriter,
                                                Message_LazyProgressScope& thePSentry,
                                                const RWMesh_FaceIterator& theFace);

  //! Write face triangles indices.
  //! @param theWriter  [in] OBJ writer context
  //! @param thePSentry [in] progress sentry
  //! @param theFace    [in] current face
  //! @return FALSE on writing file error
  Standard_EXPORT virtual bool writeIndices (RWObj_ObjWriterContext&    theWriter,
                                             Message_LazyProgressScope& thePSentry,
                                             const RWMesh_FaceIterator& theFace);


protected:

  TCollection_AsciiString          myFile;         //!< output OBJ file
  RWMesh_CoordinateSystemConverter myCSTrsf;       //!< transformation from OCCT to OBJ coordinate system
  XCAFPrs_Style                    myDefaultStyle; //!< default material definition to be used for nodes with only color defined

};

#endif // _RWObj_CafWriter_HeaderFiler
