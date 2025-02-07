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

#include <Message_ProgressScope.hxx>
#include <NCollection_DataMap.hxx>
#include <TColStd_IndexedDataMapOfStringString.hxx>
#include <TColStd_MapOfAsciiString.hxx>
#include <TDF_LabelSequence.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <RWGltf_DracoParameters.hxx>
#include <RWGltf_GltfArrayType.hxx>
#include <RWGltf_GltfBufferView.hxx>
#include <RWGltf_GltfFace.hxx>
#include <RWGltf_WriterTrsfFormat.hxx>
#include <RWMesh_CoordinateSystemConverter.hxx>
#include <RWMesh_NameFormat.hxx>
#include <XCAFPrs_DocumentNode.hxx>
#include <XCAFPrs_Style.hxx>
#include <Poly_Triangle.hxx>

#include <memory>

class Message_ProgressRange;
class RWMesh_ShapeIterator;
class RWMesh_FaceIterator;
class RWMesh_EdgeIterator;
class RWMesh_VertexIterator;
class RWGltf_GltfOStreamWriter;
class RWGltf_GltfMaterialMap;
class RWGltf_GltfSceneNodeMap;
class TDataStd_NamedData;
class TDocStd_Document;

//! glTF writer context from XCAF document.
class RWGltf_CafWriter : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(RWGltf_CafWriter, Standard_Transient)
public:
  //! Mesh
  struct Mesh
  {
    std::vector<Graphic3d_Vec3> NodesVec;     //!< vector for mesh nodes
    std::vector<Graphic3d_Vec3> NormalsVec;   //!< vector for mesh normals
    std::vector<Graphic3d_Vec2> TexCoordsVec; //!< vector for mesh texture UV coordinates
    std::vector<Poly_Triangle>  IndicesVec;   //!< vector for mesh indices
  };

  //! Main constructor.
  //! @param[in] theFile      path to output glTF file
  //! @param[in] theIsBinary  flag to write into binary glTF format (.glb)
  Standard_EXPORT RWGltf_CafWriter(const TCollection_AsciiString& theFile,
                                   Standard_Boolean               theIsBinary);

  //! Destructor.
  Standard_EXPORT virtual ~RWGltf_CafWriter();

  //! Return transformation from OCCT to glTF coordinate system.
  const RWMesh_CoordinateSystemConverter& CoordinateSystemConverter() const { return myCSTrsf; }

  //! Return transformation from OCCT to glTF coordinate system.
  RWMesh_CoordinateSystemConverter& ChangeCoordinateSystemConverter() { return myCSTrsf; }

  //! Set transformation from OCCT to glTF coordinate system.
  void SetCoordinateSystemConverter(const RWMesh_CoordinateSystemConverter& theConverter)
  {
    myCSTrsf = theConverter;
  }

  //! Return flag to write into binary glTF format (.glb), specified within class constructor.
  bool IsBinary() const { return myIsBinary; }

  //! Return preferred transformation format for writing into glTF file;
  //! RWGltf_WriterTrsfFormat_Compact by default.
  RWGltf_WriterTrsfFormat TransformationFormat() const { return myTrsfFormat; }

  //! Set preferred transformation format for writing into glTF file.
  void SetTransformationFormat(RWGltf_WriterTrsfFormat theFormat) { myTrsfFormat = theFormat; }

  //! Return name format for exporting Nodes; RWMesh_NameFormat_InstanceOrProduct by default.
  RWMesh_NameFormat NodeNameFormat() const { return myNodeNameFormat; }

  //! Set name format for exporting Nodes.
  void SetNodeNameFormat(RWMesh_NameFormat theFormat) { myNodeNameFormat = theFormat; }

  //! Return name format for exporting Meshes; RWMesh_NameFormat_Product by default.
  RWMesh_NameFormat MeshNameFormat() const { return myMeshNameFormat; }

  //! Set name format for exporting Meshes.
  void SetMeshNameFormat(RWMesh_NameFormat theFormat) { myMeshNameFormat = theFormat; }

  //! Return TRUE to export UV coordinates even if there are no mapped texture; FALSE by default.
  bool IsForcedUVExport() const { return myIsForcedUVExport; }

  //! Set flag to export UV coordinates even if there are no mapped texture; FALSE by default.
  void SetForcedUVExport(bool theToForce) { myIsForcedUVExport = theToForce; }

  //! Return default material definition to be used for nodes with only color defined.
  const XCAFPrs_Style& DefaultStyle() const { return myDefaultStyle; }

  //! Set default material definition to be used for nodes with only color defined.
  void SetDefaultStyle(const XCAFPrs_Style& theStyle) { myDefaultStyle = theStyle; }

  //! Return flag to write image textures into GLB file (binary gltf export); TRUE by default.
  //! When set to FALSE, texture images will be written as separate files.
  //! Has no effect on writing into non-binary format.
  Standard_Boolean ToEmbedTexturesInGlb() { return myToEmbedTexturesInGlb; }

  //! Set flag to write image textures into GLB file (binary gltf export).
  void SetToEmbedTexturesInGlb(Standard_Boolean theToEmbedTexturesInGlb)
  {
    myToEmbedTexturesInGlb = theToEmbedTexturesInGlb;
  }

  //! Return flag to merge faces within a single part; FALSE by default.
  bool ToMergeFaces() const { return myToMergeFaces; }

  //! Set flag to merge faces within a single part.
  //! May reduce JSON size thanks to smaller number of primitive arrays.
  void SetMergeFaces(bool theToMerge) { myToMergeFaces = theToMerge; }

  //! Return flag to prefer keeping 16-bit indexes while merging face; FALSE by default.
  bool ToSplitIndices16() const { return myToSplitIndices16; }

  //! Set flag to prefer keeping 16-bit indexes while merging face.
  //! Has effect only with ToMergeFaces() option turned ON.
  //! May reduce binary data size thanks to smaller triangle indexes.
  void SetSplitIndices16(bool theToSplit) { myToSplitIndices16 = theToSplit; }

  //! Return TRUE if multithreaded optimizations are allowed; FALSE by default.
  bool ToParallel() const { return myToParallel; }

  //! Setup multithreaded execution.
  void SetParallel(bool theToParallel) { myToParallel = theToParallel; }

  //! Return Draco parameters
  const RWGltf_DracoParameters& CompressionParameters() const { return myDracoParameters; }

  //! Set Draco parameters
  void SetCompressionParameters(const RWGltf_DracoParameters& theDracoParameters)
  {
    myDracoParameters = theDracoParameters;
  }

  //! Write glTF file and associated binary file.
  //! Triangulation data should be precomputed within shapes!
  //! @param[in] theDocument     input document
  //! @param[in] theRootLabels   list of root shapes to export
  //! @param[in] theLabelFilter  optional filter with document nodes to export,
  //!                            with keys defined by XCAFPrs_DocumentExplorer::DefineChildId() and
  //!                            filled recursively (leaves and parent assembly nodes at all
  //!                            levels); when not NULL, all nodes not included into the map will be
  //!                            ignored
  //! @param[in] theFileInfo     map with file metadata to put into glTF header section
  //! @param[in] theProgress     optional progress indicator
  //! @return FALSE on file writing failure
  Standard_EXPORT virtual bool Perform(const Handle(TDocStd_Document)&             theDocument,
                                       const TDF_LabelSequence&                    theRootLabels,
                                       const TColStd_MapOfAsciiString*             theLabelFilter,
                                       const TColStd_IndexedDataMapOfStringString& theFileInfo,
                                       const Message_ProgressRange&                theProgress);

  //! Write glTF file and associated binary file.
  //! Triangulation data should be precomputed within shapes!
  //! @param[in] theDocument     input document
  //! @param[in] theFileInfo     map with file metadata to put into glTF header section
  //! @param[in] theProgress     optional progress indicator
  //! @return FALSE on file writing failure
  Standard_EXPORT virtual bool Perform(const Handle(TDocStd_Document)&             theDocument,
                                       const TColStd_IndexedDataMapOfStringString& theFileInfo,
                                       const Message_ProgressRange&                theProgress);

protected:
  //! Write binary data file with triangulation data.
  //! Triangulation data should be precomputed within shapes!
  //! @param[in] theDocument     input document
  //! @param[in] theRootLabels   list of root shapes to export
  //! @param[in] theLabelFilter  optional filter with document nodes to export
  //! @param[in] theProgress     optional progress indicator
  //! @return FALSE on file writing failure
  Standard_EXPORT virtual bool writeBinData(const Handle(TDocStd_Document)& theDocument,
                                            const TDF_LabelSequence&        theRootLabels,
                                            const TColStd_MapOfAsciiString* theLabelFilter,
                                            const Message_ProgressRange&    theProgress);

  //! Write JSON file with glTF structure (should be called after writeBinData()).
  //! @param[in] theDocument     input document
  //! @param[in] theRootLabels   list of root shapes to export
  //! @param[in] theLabelFilter  optional filter with document nodes to export
  //! @param[in] theFileInfo     map with file metadata to put into glTF header section
  //! @param[in] theProgress     optional progress indicator
  //! @return FALSE on file writing failure
  Standard_EXPORT virtual bool writeJson(const Handle(TDocStd_Document)&             theDocument,
                                         const TDF_LabelSequence&                    theRootLabels,
                                         const TColStd_MapOfAsciiString*             theLabelFilter,
                                         const TColStd_IndexedDataMapOfStringString& theFileInfo,
                                         const Message_ProgressRange&                theProgress);

protected:
  //! Return TRUE if face shape should be skipped (e.g. because it is invalid or empty).
  Standard_EXPORT virtual Standard_Boolean toSkipShape(
    const RWMesh_ShapeIterator& theShapeIter) const;

  //! Return TRUE if shape has triangulation (not vertex or edge).
  Standard_EXPORT virtual Standard_Boolean hasTriangulation(const RWGltf_GltfFace& theShape) const;

  //! Generate name for specified labels.
  //! @param[in] theFormat   name format to apply
  //! @param[in] theLabel    instance label
  //! @param[in] theRefLabel product label
  Standard_EXPORT virtual TCollection_AsciiString formatName(RWMesh_NameFormat theFormat,
                                                             const TDF_Label&  theLabel,
                                                             const TDF_Label&  theRefLabel) const;

  //! Write mesh nodes into binary file.
  //! @param[out] theGltfFace  glTF face definition
  //! @param[out] theBinFile   output file to write into
  //! @param[in] theShapeIter  current shape to write
  //! @param[in][out] theAccessorNb   last accessor index
  //! @param[in][out] theMesh   mesh
  Standard_EXPORT virtual void saveNodes(
    RWGltf_GltfFace&                               theGltfFace,
    std::ostream&                                  theBinFile,
    const RWMesh_ShapeIterator&                    theShapeIter,
    Standard_Integer&                              theAccessorNb,
    const std::shared_ptr<RWGltf_CafWriter::Mesh>& theMesh) const;

  //! Write mesh normals into binary file.
  //! @param[out] theGltfFace  glTF face definition
  //! @param[out] theBinFile   output file to write into
  //! @param[in] theFaceIter   current face to write
  //! @param[in][out] theAccessorNb   last accessor index
  //! @param[in][out] theMesh   mesh
  Standard_EXPORT virtual void saveNormals(
    RWGltf_GltfFace&                               theGltfFace,
    std::ostream&                                  theBinFile,
    const RWMesh_FaceIterator&                     theFaceIter,
    Standard_Integer&                              theAccessorNb,
    const std::shared_ptr<RWGltf_CafWriter::Mesh>& theMesh) const;

  //! Write mesh texture UV coordinates into binary file.
  //! @param[out] theGltfFace  glTF face definition
  //! @param[out] theBinFile   output file to write into
  //! @param[in] theFaceIter   current face to write
  //! @param[in][out] theAccessorNb   last accessor index
  //! @param[in][out] theMesh   mesh
  Standard_EXPORT virtual void saveTextCoords(
    RWGltf_GltfFace&                               theGltfFace,
    std::ostream&                                  theBinFile,
    const RWMesh_FaceIterator&                     theFaceIter,
    Standard_Integer&                              theAccessorNb,
    const std::shared_ptr<RWGltf_CafWriter::Mesh>& theMesh) const;

  //! Write mesh indexes into binary file.
  //! @param[out] theGltfFace  glTF face definition
  //! @param[out] theBinFile   output file to write into
  //! @param[in] theShapeIter  current shape to write
  //! @param[in][out] theAccessorNb   last accessor index
  //! @param[in][out] theMesh   mesh
  Standard_EXPORT virtual void saveIndices(RWGltf_GltfFace&            theGltfFace,
                                           std::ostream&               theBinFile,
                                           const RWMesh_ShapeIterator& theShapeIter,
                                           Standard_Integer&           theAccessorNb,
                                           const std::shared_ptr<RWGltf_CafWriter::Mesh>& theMesh);

  //! Write triangle indexes into binary file.
  //! @param[out] theGltfFace  glTF face definition
  //! @param[out] theBinFile   output file to write into
  //! @param[in] theFaceIter   current face to write
  //! @param[in][out] theMesh   mesh
  Standard_EXPORT virtual void saveTriangleIndices(
    RWGltf_GltfFace&                               theGltfFace,
    std::ostream&                                  theBinFile,
    const RWMesh_FaceIterator&                     theFaceIter,
    const std::shared_ptr<RWGltf_CafWriter::Mesh>& theMesh);

  //! Write edge indexes into binary file.
  //! @param[out] theGltfFace  glTF face definition
  //! @param[out] theBinFile   output file to write into
  //! @param[in] theEdgeIter   current edge to write
  Standard_EXPORT virtual void saveEdgeIndices(RWGltf_GltfFace&           theGltfFace,
                                               std::ostream&              theBinFile,
                                               const RWMesh_EdgeIterator& theEdgeIter);

  //! Write vertex indexes into binary file.
  //! @param[out] theGltfFace  glTF face definition
  //! @param[out] theBinFile   output file to write into
  //! @param[in] theVertexIter current vertex to write
  Standard_EXPORT virtual void saveVertexIndices(RWGltf_GltfFace&             theGltfFace,
                                                 std::ostream&                theBinFile,
                                                 const RWMesh_VertexIterator& theVertexIter);

protected:
  //! Write bufferView for vertex positions within RWGltf_GltfRootElement_Accessors section
  //! @param[in] theGltfFace  face definition to write
  Standard_EXPORT virtual void writePositions(const RWGltf_GltfFace& theGltfFace);

  //! Write bufferView for vertex normals within RWGltf_GltfRootElement_Accessors section
  //! @param[in] theGltfFace  face definition to write
  Standard_EXPORT virtual void writeNormals(const RWGltf_GltfFace& theGltfFace);

  //! Write bufferView for vertex texture coordinates within RWGltf_GltfRootElement_Accessors
  //! section
  //! @param[in] theGltfFace  face definition to write
  Standard_EXPORT virtual void writeTextCoords(const RWGltf_GltfFace& theGltfFace);

  //! Write bufferView for triangle indexes within RWGltf_GltfRootElement_Accessors section.
  //! @param[in] theGltfFace  face definition to write
  Standard_EXPORT virtual void writeIndices(const RWGltf_GltfFace& theGltfFace);

protected:
  //! Write RWGltf_GltfRootElement_Accessors section.
  //! @param[in] theSceneNodeMap  ordered map of scene nodes
  Standard_EXPORT virtual void writeAccessors(const RWGltf_GltfSceneNodeMap& theSceneNodeMap);

  //! Write RWGltf_GltfRootElement_Animations section (reserved).
  Standard_EXPORT virtual void writeAnimations();

  //! Write RWGltf_GltfRootElement_Asset section.
  //! @param[in] theFileInfo  optional metadata to write into file header
  Standard_EXPORT virtual void writeAsset(const TColStd_IndexedDataMapOfStringString& theFileInfo);

  //! Write RWGltf_GltfRootElement_BufferViews section.
  //! @param[in] theBinDataBufferId  index of binary buffer with vertex data
  Standard_EXPORT virtual void writeBufferViews(const Standard_Integer theBinDataBufferId);

  //! Write RWGltf_GltfRootElement_Buffers section.
  Standard_EXPORT virtual void writeBuffers();

  //! Write RWGltf_GltfRootElement_ExtensionsUsed/RWGltf_GltfRootElement_ExtensionsRequired sections
  //! (reserved).
  Standard_EXPORT virtual void writeExtensions();

  //! Write RWGltf_GltfRootElement_Images section.
  //! @param[in] theSceneNodeMap  ordered map of scene nodes
  //! @param[out] theMaterialMap  map of materials, filled with image files used by textures
  Standard_EXPORT virtual void writeImages(const RWGltf_GltfSceneNodeMap& theSceneNodeMap);

  //! Write RWGltf_GltfRootElement_Materials section.
  //! @param[in] theSceneNodeMap  ordered map of scene nodes
  //! @param[out] theMaterialMap  map of materials, filled with materials
  Standard_EXPORT virtual void writeMaterials(const RWGltf_GltfSceneNodeMap& theSceneNodeMap);

  //! Write RWGltf_GltfRootElement_Materials section.
  //! @param[in]  theShapeIter         Shape iterator to traverse shapes
  //! @param[out] theIsStarted         Flag indicating that writing material has been started
  Standard_EXPORT virtual void writeMaterial(RWMesh_ShapeIterator& theShapeIter,
                                             Standard_Boolean&     theIsStarted);

  //! Write RWGltf_GltfRootElement_Meshes section.
  //! @param[in] theSceneNodeMap  ordered map of scene nodes
  //! @param[in] theMaterialMap   map of materials
  Standard_EXPORT virtual void writeMeshes(const RWGltf_GltfSceneNodeMap& theSceneNodeMap);

  //! Write a primitive array to RWGltf_GltfRootElement_Meshes section.
  //! @param[in]     theGltfFace     face to write
  //! @param[in]     theName         primitive array name
  //! @param[in]     theDracoBufInd  draco buffer index
  //! @param[in,out] theToStartPrims flag indicating that primitive array has been started
  Standard_EXPORT virtual void writePrimArray(const RWGltf_GltfFace&         theGltfFace,
                                              const TCollection_AsciiString& theName,
                                              const int                      theDracoBufInd,
                                              bool&                          theToStartPrims);

  //! Write RWGltf_GltfRootElement_Nodes section.
  //! @param[in] theDocument      input document
  //! @param[in] theRootLabels    list of root shapes to export
  //! @param[in] theLabelFilter   optional filter with document nodes to export
  //! @param[in] theSceneNodeMap  ordered map of scene nodes
  //! @param[out] theSceneRootNodeInds  sequence of scene nodes pointing to root shapes (to be used
  //! for writeScenes())
  Standard_EXPORT virtual void writeNodes(
    const Handle(TDocStd_Document)&         theDocument,
    const TDF_LabelSequence&                theRootLabels,
    const TColStd_MapOfAsciiString*         theLabelFilter,
    const RWGltf_GltfSceneNodeMap&          theSceneNodeMap,
    NCollection_Sequence<Standard_Integer>& theSceneRootNodeInds);

  //! Write RWGltf_GltfRootElement_Samplers section.
  Standard_EXPORT virtual void writeSamplers();

  //! Write RWGltf_GltfRootElement_Scene section.
  //! @param[in] theDefSceneId  index of default scene (0)
  Standard_EXPORT virtual void writeScene(const Standard_Integer theDefSceneId);

  //! Write RWGltf_GltfRootElement_Scenes section.
  //! @param[in] theSceneRootNodeInds  sequence of scene nodes pointing to root shapes
  Standard_EXPORT virtual void writeScenes(
    const NCollection_Sequence<Standard_Integer>& theSceneRootNodeInds);

  //! Write RWGltf_GltfRootElement_Skins section (reserved).
  Standard_EXPORT virtual void writeSkins();

  //! Write RWGltf_GltfRootElement_Textures section.
  //! @param[in] theSceneNodeMap  ordered map of scene nodes
  //! @param[out] theMaterialMap  map of materials, filled with textures
  Standard_EXPORT virtual void writeTextures(const RWGltf_GltfSceneNodeMap& theSceneNodeMap);

  //! Write nodes.extras section with key-value attributes.
  //! @param[in] theNamedData  attributes map to process.
  Standard_EXPORT virtual void writeExtrasAttributes(
    const Handle(TDataStd_NamedData)& theNamedData);

  //! Dispatch shapes
  //! @param[in] theDocNode         Document node containing shape data
  //! @param[in] thePSentryBin      Progress scope for the operation
  //! @param[in,out] theMergedFaces Data map to store merged faces
  //! @param[in,out] theShapeIter   Shape iterator to traverse shapes
  Standard_EXPORT virtual void dispatchShapes(
    const XCAFPrs_DocumentNode&                                  theDocNode,
    const Message_ProgressScope&                                 thePSentryBin,
    NCollection_DataMap<XCAFPrs_Style, Handle(RWGltf_GltfFace)>& theMergedFaces,
    RWMesh_ShapeIterator&                                        theShapeIter);

  //! Write shape into binary file
  //! @param[out] theGltfFace      glTF face definition
  //! @param[out] theBinFile       Output file to write into
  //! @param[in] theShapeIter      Current shape iterator
  //! @param[in,out] theAccessorNb Last accessor index
  //! @param[in,out] theMesh       Mesh data
  //! @param[in] theArrType        Array type for glTF
  //! @param[in] thePSentryBin     Progress scope for the operation
  //! @return True if shapes were successfully written to the binary file, false otherwise
  Standard_EXPORT bool writeShapesToBin(RWGltf_GltfFace&      theGltfFace,
                                        std::ostream&         theBinFile,
                                        RWMesh_ShapeIterator& theShapeIter,
                                        Standard_Integer&     theAccessorNb,
                                        const std::shared_ptr<RWGltf_CafWriter::Mesh>& theMesh,
                                        const RWGltf_GltfArrayType                     theArrType,
                                        const Message_ProgressScope& thePSentryBin);

  //! Write shapes to RWGltf_GltfRootElement_Meshes section
  //! @param[in] theShapeIter          Shape iterator to traverse shapes
  //! @param[in,out] theDracoBufInd    Draco buffer index
  //! @param[in,out] theToStartPrims   Flag to indicate if primitives should be started
  //! @param[in] theNodeName           Name of the current node
  //! @param[in,out] theWrittenShapes  Map to store written shapes
  //! @param[in,out] theDracoBufIndMap Map to store Draco buffer indices
  Standard_EXPORT virtual void writeShapes(
    RWMesh_ShapeIterator&                         theShapeIter,
    Standard_Integer&                             theDracoBufInd,
    Standard_Boolean&                             theToStartPrims,
    const TCollection_AsciiString&                theNodeName,
    NCollection_Map<Handle(RWGltf_GltfFaceList)>& theWrittenShapes,
    NCollection_IndexedDataMap<int, int>&         theDracoBufIndMap);

protected:
  //! Shape + Style pair.
  struct RWGltf_StyledShape
  {
    TopoDS_Shape  Shape;
    XCAFPrs_Style Style;

    RWGltf_StyledShape() {}

    explicit RWGltf_StyledShape(const TopoDS_Shape& theShape)
        : Shape(theShape)
    {
    }

    explicit RWGltf_StyledShape(const TopoDS_Shape& theShape, const XCAFPrs_Style& theStyle)
        : Shape(theShape),
          Style(theStyle)
    {
    }

    bool operator==(const RWGltf_StyledShape& theStyledShape) const
    {
      return Shape.IsSame(theStyledShape.Shape) && Style.IsEqual(theStyledShape.Style);
    }
  };

  struct Hasher
  {
    size_t operator()(const RWGltf_StyledShape& theShape) const noexcept
    {
      return std::hash<TopoDS_Shape>{}(theShape.Shape);
    }

    bool operator()(const RWGltf_StyledShape& theShape1,
                    const RWGltf_StyledShape& theShape2) const noexcept
    {
      return theShape1 == theShape2;
    }
  };

  typedef NCollection_IndexedDataMap<RWGltf_StyledShape, Handle(RWGltf_GltfFaceList), Hasher>
    ShapeToGltfFaceMap;

protected:
  TCollection_AsciiString myFile;           //!< output glTF file
                                            // clang-format off
  TCollection_AsciiString                       myBinFileNameFull;   //!< output file with binary data (full  path)
  TCollection_AsciiString                       myBinFileNameShort;  //!< output file with binary data (short path)
  RWGltf_WriterTrsfFormat                       myTrsfFormat;        //!< transformation format to write into glTF file
  RWMesh_NameFormat                             myNodeNameFormat;    //!< name format for exporting Nodes
  RWMesh_NameFormat                             myMeshNameFormat;    //!< name format for exporting Meshes
  Standard_Boolean                              myIsBinary;          //!< flag to write into binary glTF format (.glb)
  Standard_Boolean                              myIsForcedUVExport;  //!< export UV coordinates even if there are no mapped texture
  Standard_Boolean                              myToEmbedTexturesInGlb; //!< flag to write image textures into GLB file
  Standard_Boolean                              myToMergeFaces;      //!< flag to merge faces within a single part
  Standard_Boolean                              myToSplitIndices16;  //!< flag to prefer keeping 16-bit indexes while merging face
  RWMesh_CoordinateSystemConverter              myCSTrsf;            //!< transformation from OCCT to glTF coordinate system
  XCAFPrs_Style                                 myDefaultStyle;      //!< default material definition to be used for nodes with only color defined

  std::shared_ptr<RWGltf_GltfOStreamWriter>     myWriter;            //!< JSON writer
  Handle(RWGltf_GltfMaterialMap)                myMaterialMap;       //!< map of defined materials
  RWGltf_GltfBufferView                         myBuffViewPos;       //!< current buffer view with nodes positions
  RWGltf_GltfBufferView                         myBuffViewNorm;      //!< current buffer view with nodes normals
  RWGltf_GltfBufferView                         myBuffViewTextCoord; //!< current buffer view with nodes UV coordinates
  RWGltf_GltfBufferView                         myBuffViewInd;       //!< current buffer view with triangulation indexes
  ShapeToGltfFaceMap                            myBinDataMap;        //!< map for TopoDS_Face to glTF face (merging duplicates)
  int64_t                                       myBinDataLen64;      //!< length of binary file

  std::vector<RWGltf_GltfBufferView>            myBuffViewsDraco;    //!< vector of buffers view with compression data
  Standard_Boolean                              myToParallel;        //!< flag to use multithreading; FALSE by default
                                            // clang-format on
  RWGltf_DracoParameters myDracoParameters; //!< Draco parameters
};

#endif // _RWGltf_CafWriter_HeaderFiler
