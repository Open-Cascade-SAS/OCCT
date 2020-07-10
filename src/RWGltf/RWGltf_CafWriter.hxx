// Copyright (c) 2017-2019 OPEN CASCADE SAS
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

#ifndef _RWGltf_CafWriter_HeaderFiler
#define _RWGltf_CafWriter_HeaderFiler

#include <TColStd_IndexedDataMapOfStringString.hxx>
#include <TColStd_MapOfAsciiString.hxx>
#include <TDF_LabelSequence.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <RWGltf_GltfBufferView.hxx>
#include <RWGltf_GltfFace.hxx>
#include <RWGltf_WriterTrsfFormat.hxx>
#include <RWMesh_CoordinateSystemConverter.hxx>
#include <XCAFPrs_Style.hxx>

#include <memory>

class Message_ProgressRange;
class RWMesh_FaceIterator;
class RWGltf_GltfOStreamWriter;
class RWGltf_GltfMaterialMap;
class RWGltf_GltfSceneNodeMap;
class TDocStd_Document;

//! glTF writer context from XCAF document.
class RWGltf_CafWriter : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(RWGltf_CafWriter, Standard_Transient)
public:

  //! Main constructor.
  //! @param theFile     [in] path to output glTF file
  //! @param theIsBinary [in] flag to write into binary glTF format (.glb)
  Standard_EXPORT RWGltf_CafWriter (const TCollection_AsciiString& theFile,
                                    Standard_Boolean theIsBinary);

  //! Destructor.
  Standard_EXPORT virtual ~RWGltf_CafWriter();

  //! Return transformation from OCCT to glTF coordinate system.
  const RWMesh_CoordinateSystemConverter& CoordinateSystemConverter() const { return myCSTrsf; }

  //! Return transformation from OCCT to glTF coordinate system.
  RWMesh_CoordinateSystemConverter& ChangeCoordinateSystemConverter() { return myCSTrsf; }

  //! Set transformation from OCCT to glTF coordinate system.
  void SetCoordinateSystemConverter (const RWMesh_CoordinateSystemConverter& theConverter) { myCSTrsf = theConverter; }

  //! Return flag to write into binary glTF format (.glb), specified within class constructor.
  bool IsBinary() const { return myIsBinary; }

  //! Return preferred transformation format for writing into glTF file; RWGltf_WriterTrsfFormat_Compact by default.
  RWGltf_WriterTrsfFormat TransformationFormat() const { return myTrsfFormat; }

  //! Set preferred transformation format for writing into glTF file.
  void SetTransformationFormat (RWGltf_WriterTrsfFormat theFormat) { myTrsfFormat = theFormat; }

  //! Return TRUE to export UV coordinates even if there are no mapped texture; FALSE by default.
  bool IsForcedUVExport() const { return myIsForcedUVExport; }

  //! Set flag to export UV coordinates even if there are no mapped texture; FALSE by default.
  void SetForcedUVExport (bool theToForce) { myIsForcedUVExport = theToForce; }

  //! Return default material definition to be used for nodes with only color defined.
  const XCAFPrs_Style& DefaultStyle() const { return myDefaultStyle; }

  //! Set default material definition to be used for nodes with only color defined.
  void SetDefaultStyle (const XCAFPrs_Style& theStyle) { myDefaultStyle = theStyle; }

  //! Write glTF file and associated binary file.
  //! Triangulation data should be precomputed within shapes!
  //! @param theDocument    [in] input document
  //! @param theRootLabels  [in] list of root shapes to export
  //! @param theLabelFilter [in] optional filter with document nodes to export,
  //!                            with keys defined by XCAFPrs_DocumentExplorer::DefineChildId() and filled recursively
  //!                            (leaves and parent assembly nodes at all levels);
  //!                            when not NULL, all nodes not included into the map will be ignored
  //! @param theFileInfo    [in] map with file metadata to put into glTF header section
  //! @param theProgress    [in] optional progress indicator
  //! @return FALSE on file writing failure
  Standard_EXPORT virtual bool Perform (const Handle(TDocStd_Document)& theDocument,
                                        const TDF_LabelSequence& theRootLabels,
                                        const TColStd_MapOfAsciiString* theLabelFilter,
                                        const TColStd_IndexedDataMapOfStringString& theFileInfo,
                                        const Message_ProgressRange& theProgress);

  //! Write glTF file and associated binary file.
  //! Triangulation data should be precomputed within shapes!
  //! @param theDocument    [in] input document
  //! @param theFileInfo    [in] map with file metadata to put into glTF header section
  //! @param theProgress    [in] optional progress indicator
  //! @return FALSE on file writing failure
  Standard_EXPORT virtual bool Perform (const Handle(TDocStd_Document)& theDocument,
                                        const TColStd_IndexedDataMapOfStringString& theFileInfo,
                                        const Message_ProgressRange& theProgress);

protected:

  //! Write binary data file with triangulation data.
  //! Triangulation data should be precomputed within shapes!
  //! @param theDocument    [in] input document
  //! @param theRootLabels  [in] list of root shapes to export
  //! @param theLabelFilter [in] optional filter with document nodes to export
  //! @param theProgress    [in] optional progress indicator
  //! @return FALSE on file writing failure
  Standard_EXPORT virtual bool writeBinData (const Handle(TDocStd_Document)& theDocument,
                                             const TDF_LabelSequence& theRootLabels,
                                             const TColStd_MapOfAsciiString* theLabelFilter,
                                             const Message_ProgressRange& theProgress);

  //! Write JSON file with glTF structure (should be called after writeBinData()).
  //! @param theDocument    [in] input document
  //! @param theRootLabels  [in] list of root shapes to export
  //! @param theLabelFilter [in] optional filter with document nodes to export
  //! @param theFileInfo    [in] map with file metadata to put into glTF header section
  //! @param theProgress    [in] optional progress indicator
  //! @return FALSE on file writing failure
  Standard_EXPORT virtual bool writeJson (const Handle(TDocStd_Document)& theDocument,
                                          const TDF_LabelSequence& theRootLabels,
                                          const TColStd_MapOfAsciiString* theLabelFilter,
                                          const TColStd_IndexedDataMapOfStringString& theFileInfo,
                                          const Message_ProgressRange& theProgress);

protected:

  //! Return TRUE if face mesh should be skipped (e.g. because it is invalid or empty).
  Standard_EXPORT virtual Standard_Boolean toSkipFaceMesh (const RWMesh_FaceIterator& theFaceIter);

  //! Write mesh nodes into binary file.
  //! @param theGltfFace [out] glTF face definition
  //! @param theBinFile  [out] output file to write into
  //! @param theFaceIter [in]  current face to write
  //! @param theAccessorNb [in] [out] last accessor index
  Standard_EXPORT virtual void saveNodes (RWGltf_GltfFace& theGltfFace,
                                          std::ostream& theBinFile,
                                          const RWMesh_FaceIterator& theFaceIter,
                                          Standard_Integer& theAccessorNb) const;

  //! Write mesh normals into binary file.
  //! @param theGltfFace [out] glTF face definition
  //! @param theBinFile  [out] output file to write into
  //! @param theFaceIter [in]  current face to write
  //! @param theAccessorNb [in] [out] last accessor index
  Standard_EXPORT virtual void saveNormals (RWGltf_GltfFace& theGltfFace,
                                            std::ostream& theBinFile,
                                            RWMesh_FaceIterator& theFaceIter,
                                            Standard_Integer& theAccessorNb) const;

  //! Write mesh texture UV coordinates into binary file.
  //! @param theGltfFace [out] glTF face definition
  //! @param theBinFile  [out] output file to write into
  //! @param theFaceIter [in]  current face to write
  //! @param theAccessorNb [in] [out] last accessor index
  Standard_EXPORT virtual void saveTextCoords (RWGltf_GltfFace& theGltfFace,
                                               std::ostream& theBinFile,
                                               const RWMesh_FaceIterator& theFaceIter,
                                               Standard_Integer& theAccessorNb) const;

  //! Write mesh indexes into binary file.
  //! @param theGltfFace [out] glTF face definition
  //! @param theBinFile  [out] output file to write into
  //! @param theFaceIter [in]  current face to write
  //! @param theAccessorNb [in] [out] last accessor index
  Standard_EXPORT virtual void saveIndices (RWGltf_GltfFace& theGltfFace,
                                            std::ostream& theBinFile,
                                            const RWMesh_FaceIterator& theFaceIter,
                                            Standard_Integer& theAccessorNb);

protected:

  //! Write bufferView for vertex positions within RWGltf_GltfRootElement_Accessors section
  //! @param theGltfFace [in] face definition to write
  Standard_EXPORT virtual void writePositions (const RWGltf_GltfFace& theGltfFace);

  //! Write bufferView for vertex normals within RWGltf_GltfRootElement_Accessors section
  //! @param theGltfFace [in] face definition to write
  Standard_EXPORT virtual void writeNormals (const RWGltf_GltfFace& theGltfFace);

  //! Write bufferView for vertex texture coordinates within RWGltf_GltfRootElement_Accessors section
  //! @param theGltfFace [in] face definition to write
  Standard_EXPORT virtual void writeTextCoords (const RWGltf_GltfFace& theGltfFace);

  //! Write bufferView for triangle indexes within RWGltf_GltfRootElement_Accessors section.
  //! @param theGltfFace [in] face definition to write
  Standard_EXPORT virtual void writeIndices (const RWGltf_GltfFace& theGltfFace);

protected:

  //! Write RWGltf_GltfRootElement_Accessors section.
  //! @param theSceneNodeMap [in] ordered map of scene nodes
  Standard_EXPORT virtual void writeAccessors (const RWGltf_GltfSceneNodeMap& theSceneNodeMap);

  //! Write RWGltf_GltfRootElement_Animations section (reserved).
  Standard_EXPORT virtual void writeAnimations();

  //! Write RWGltf_GltfRootElement_Asset section.
  //! @param theFileInfo [in] optional metadata to write into file header
  Standard_EXPORT virtual void writeAsset (const TColStd_IndexedDataMapOfStringString& theFileInfo);

  //! Write RWGltf_GltfRootElement_BufferViews section.
  //! @param theBinDataBufferId [in] index of binary buffer with vertex data
  Standard_EXPORT virtual void writeBufferViews (const Standard_Integer theBinDataBufferId);

  //! Write RWGltf_GltfRootElement_Buffers section.
  Standard_EXPORT virtual void writeBuffers();

  //! Write RWGltf_GltfRootElement_ExtensionsUsed/RWGltf_GltfRootElement_ExtensionsRequired sections (reserved).
  Standard_EXPORT virtual void writeExtensions();

  //! Write RWGltf_GltfRootElement_Images section.
  //! @param theSceneNodeMap [in] ordered map of scene nodes
  //! @param theMaterialMap [out] map of materials, filled with image files used by textures
  Standard_EXPORT virtual void writeImages (const RWGltf_GltfSceneNodeMap& theSceneNodeMap,
                                            RWGltf_GltfMaterialMap& theMaterialMap);

  //! Write RWGltf_GltfRootElement_Materials section.
  //! @param theSceneNodeMap [in] ordered map of scene nodes
  //! @param theMaterialMap [out] map of materials, filled with materials
  Standard_EXPORT virtual void writeMaterials (const RWGltf_GltfSceneNodeMap& theSceneNodeMap,
                                               RWGltf_GltfMaterialMap& theMaterialMap);

  //! Write RWGltf_GltfRootElement_Meshes section.
  //! @param theSceneNodeMap [in] ordered map of scene nodes
  //! @param theMaterialMap  [in] map of materials
  Standard_EXPORT virtual void writeMeshes (const RWGltf_GltfSceneNodeMap& theSceneNodeMap,
                                            const RWGltf_GltfMaterialMap& theMaterialMap);

  //! Write RWGltf_GltfRootElement_Nodes section.
  //! @param theDocument     [in] input document
  //! @param theRootLabels   [in] list of root shapes to export
  //! @param theLabelFilter  [in] optional filter with document nodes to export
  //! @param theSceneNodeMap [in] ordered map of scene nodes
  //! @param theSceneRootNodeInds [out] sequence of scene nodes pointing to root shapes (to be used for writeScenes())
  Standard_EXPORT virtual void writeNodes (const Handle(TDocStd_Document)&  theDocument,
                                           const TDF_LabelSequence&         theRootLabels,
                                           const TColStd_MapOfAsciiString*  theLabelFilter,
                                           const RWGltf_GltfSceneNodeMap&   theSceneNodeMap,
                                           NCollection_Sequence<Standard_Integer>& theSceneRootNodeInds);

  //! Write RWGltf_GltfRootElement_Samplers section.
  Standard_EXPORT virtual void writeSamplers (const RWGltf_GltfMaterialMap& theMaterialMap);

  //! Write RWGltf_GltfRootElement_Scene section.
  //! @param theDefSceneId [in] index of default scene (0)
  Standard_EXPORT virtual void writeScene (const Standard_Integer theDefSceneId);

  //! Write RWGltf_GltfRootElement_Scenes section.
  //! @param theSceneRootNodeInds [in] sequence of scene nodes pointing to root shapes
  Standard_EXPORT virtual void writeScenes (const NCollection_Sequence<Standard_Integer>& theSceneRootNodeInds);

  //! Write RWGltf_GltfRootElement_Skins section (reserved).
  Standard_EXPORT virtual void writeSkins();

  //! Write RWGltf_GltfRootElement_Textures section.
  //! @param theSceneNodeMap [in] ordered map of scene nodes
  //! @param theMaterialMap [out] map of materials, filled with textures
  Standard_EXPORT virtual void writeTextures (const RWGltf_GltfSceneNodeMap& theSceneNodeMap,
                                              RWGltf_GltfMaterialMap& theMaterialMap);

protected:

  TCollection_AsciiString                       myFile;              //!< output glTF file
  TCollection_AsciiString                       myBinFileNameFull;   //!< output file with binary data (full  path)
  TCollection_AsciiString                       myBinFileNameShort;  //!< output file with binary data (short path)
  RWGltf_WriterTrsfFormat                       myTrsfFormat;        //!< transformation format to write into glTF file
  Standard_Boolean                              myIsBinary;          //!< flag to write into binary glTF format (.glb)
  Standard_Boolean                              myIsForcedUVExport;  //!< export UV coordinates even if there are no mapped texture
  RWMesh_CoordinateSystemConverter              myCSTrsf;            //!< transformation from OCCT to glTF coordinate system
  XCAFPrs_Style                                 myDefaultStyle;      //!< default material definition to be used for nodes with only color defined

  opencascade::std::shared_ptr<RWGltf_GltfOStreamWriter>
                                                myWriter;            //!< JSON writer
  RWGltf_GltfBufferView                         myBuffViewPos;       //!< current buffer view with nodes positions
  RWGltf_GltfBufferView                         myBuffViewNorm;      //!< current buffer view with nodes normals
  RWGltf_GltfBufferView                         myBuffViewTextCoord; //!< current buffer view with nodes UV coordinates
  RWGltf_GltfBufferView                         myBuffViewInd;       //!< current buffer view with triangulation indexes
  NCollection_DataMap<TopoDS_Shape, RWGltf_GltfFace,
                      TopTools_ShapeMapHasher>  myBinDataMap;        //!< map for TopoDS_Face to glTF face (merging duplicates)
  int64_t                                       myBinDataLen64;      //!< length of binary file

};

#endif // _RWGltf_CafWriter_HeaderFiler
