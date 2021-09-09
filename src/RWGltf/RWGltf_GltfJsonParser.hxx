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

#ifndef _RWGltf_GltfJsonParser_HeaderFile
#define _RWGltf_GltfJsonParser_HeaderFile

#include <Graphic3d_Vec.hxx>
#include <Message_Gravity.hxx>
#include <Message_ProgressScope.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <RWGltf_GltfAccessor.hxx>
#include <RWGltf_GltfPrimArrayData.hxx>
#include <RWGltf_GltfLatePrimitiveArray.hxx>
#include <RWGltf_GltfBufferView.hxx>
#include <RWGltf_GltfRootElement.hxx>
#include <RWGltf_MaterialCommon.hxx>
#include <RWGltf_MaterialMetallicRoughness.hxx>
#include <RWMesh_CoordinateSystemConverter.hxx>
#include <RWMesh_NodeAttributes.hxx>
#include <TColStd_IndexedDataMapOfStringString.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_SequenceOfShape.hxx>

// workaround name collisions with XLib
#ifdef None
  #undef None
#endif
#ifdef Bool
  #undef Bool
#endif

#ifdef HAVE_RAPIDJSON
  //#define RAPIDJSON_ASSERT
  #include <rapidjson/document.h>
  #include <rapidjson/prettywriter.h>
  #include <rapidjson/stringbuffer.h>
  #include <rapidjson/istreamwrapper.h>
  #include <rapidjson/ostreamwrapper.h>

  typedef rapidjson::Document::ValueType RWGltf_JsonValue;
#endif

class Message_ProgressIndicator;

//! INTERNAL tool for parsing glTF document (JSON structure).
class RWGltf_GltfJsonParser
#ifdef HAVE_RAPIDJSON
: public rapidjson::Document
#endif
{
public:

#ifdef HAVE_RAPIDJSON
  //! Auxiliary method for formatting error code.
  Standard_EXPORT static const char* FormatParseError (rapidjson::ParseErrorCode theCode);
#endif

public:

  //! Empty constructor.
  Standard_EXPORT RWGltf_GltfJsonParser (TopTools_SequenceOfShape& theRootShapes);

  //! Set file path.
  Standard_EXPORT void SetFilePath (const TCollection_AsciiString& theFilePath);

  //! Set flag for probing file without complete reading.
  void SetProbeHeader (bool theToProbe) { myToProbeHeader = theToProbe; }

  //! Return prefix for reporting issues.
  const TCollection_AsciiString& ErrorPrefix() const { return myErrorPrefix; }

  //! Set prefix for reporting issues.
  void SetErrorPrefix (const TCollection_AsciiString& theErrPrefix) { myErrorPrefix = theErrPrefix; }

  //! Set map for storing node attributes.
  void SetAttributeMap (RWMesh_NodeAttributeMap& theAttribMap) { myAttribMap = &theAttribMap; }

  //! Set list for storing external files.
  void SetExternalFiles (NCollection_IndexedMap<TCollection_AsciiString>& theExternalFiles) { myExternalFiles = &theExternalFiles; }

  //! Set metadata map.
  void SetMetadata (TColStd_IndexedDataMapOfStringString& theMetadata) { myMetadata = &theMetadata; }

  //! Return transformation from glTF to OCCT coordinate system.
  const RWMesh_CoordinateSystemConverter& CoordinateSystemConverter() const { return myCSTrsf; }

  //! Set transformation from glTF to OCCT coordinate system.
  void SetCoordinateSystemConverter (const RWMesh_CoordinateSystemConverter& theConverter) { myCSTrsf = theConverter; }

  //! Initialize binary format.
  void SetBinaryFormat (int64_t theBinBodyOffset,
                        int64_t theBinBodyLen)
  {
    myIsBinary      = true;
    myBinBodyOffset = theBinBodyOffset;
    myBinBodyLen    = theBinBodyLen;
  }

  //! Set flag to ignore nodes without Geometry, TRUE by default.
  void SetSkipEmptyNodes (bool theToSkip) { myToSkipEmptyNodes = theToSkip; }

  //! Set flag to flag to load all scenes in the document, FALSE by default which means only main (default) scene will be loaded.
  void SetLoadAllScenes (bool theToLoadAll) { myToLoadAllScenes = theToLoadAll; }

  //! Set flag to use Mesh name in case if Node name is empty, TRUE by default.
  void SetMeshNameAsFallback (bool theToFallback) { myUseMeshNameAsFallback = theToFallback; }

  //! Parse glTF document.
  Standard_EXPORT bool Parse (const Message_ProgressRange& theProgress);

  //! Return face list for loading triangulation.
  NCollection_Vector<TopoDS_Face>& FaceList() { return myFaceList; }

protected:
#ifdef HAVE_RAPIDJSON
  //! Search mandatory root elements in the document.
  //! Return FALSE if some mandatory element is missing.
  Standard_EXPORT bool gltfParseRoots();

  //! Parse default scene.
  Standard_EXPORT bool gltfParseScene (const Message_ProgressRange& theProgress);

  //! Parse document metadata.
  Standard_EXPORT void gltfParseAsset();

protected:

  //! Parse materials defined in the document.
  Standard_EXPORT void gltfParseMaterials();

  //! Parse standard material.
  Standard_EXPORT bool gltfParseStdMaterial (Handle(RWGltf_MaterialCommon)& theMat,
                                             const RWGltf_JsonValue& theMatNode);

  //! Parse pbrMetallicRoughness material.
  Standard_EXPORT bool gltfParsePbrMaterial (Handle(RWGltf_MaterialMetallicRoughness)& theMat,
                                             const RWGltf_JsonValue& theMatNode);

  //! Parse common material (KHR_materials_common extension).
  Standard_EXPORT bool gltfParseCommonMaterial (Handle(RWGltf_MaterialCommon)& theMat,
                                                const RWGltf_JsonValue& theMatNode);

  //! Parse texture definition.
  Standard_EXPORT bool gltfParseTexture (Handle(Image_Texture)& theTexture,
                                         const RWGltf_JsonValue* theTextureId);

  //! Parse texture definition in binary buffer of GLB file.
  Standard_EXPORT bool gltfParseTexturInGlbBuffer (Handle(Image_Texture)& theTexture,
                                                   const RWGltf_JsonValue& theBinVal,
                                                   const TCollection_AsciiString& theBufferViewId,
                                                   const RWGltf_JsonValue& theBufferViewName);

  //! Parse texture definition in binary buffer of glTF file.
  Standard_EXPORT bool gltfParseTextureInBufferView (Handle(Image_Texture)& theTexture,
                                                     const TCollection_AsciiString& theSourceId,
                                                     const TCollection_AsciiString& theBufferViewhId,
                                                     const RWGltf_JsonValue& theBufferView);

  //! Bind material definition to the map.
  Standard_EXPORT void gltfBindMaterial (const Handle(RWGltf_MaterialMetallicRoughness)& theMatPbr,
                                         const Handle(RWGltf_MaterialCommon)& theMatCommon);

protected:

  //! Parse scene array of nodes recursively.
  Standard_EXPORT bool gltfParseSceneNodes (TopTools_SequenceOfShape& theShapeSeq,
                                            const RWGltf_JsonValue& theSceneNodes,
                                            const Message_ProgressRange& theProgress);

  //! Parse scene node recursively.
  Standard_EXPORT bool gltfParseSceneNode (TopoDS_Shape& theNodeShape,
                                           const TCollection_AsciiString& theSceneNodeId,
                                           const RWGltf_JsonValue& theSceneNode,
                                           const Message_ProgressRange& theProgress);

  //! Parse mesh element.
  Standard_EXPORT bool gltfParseMesh (TopoDS_Shape& theMeshShape,
                                      const TCollection_AsciiString& theMeshId,
                                      const RWGltf_JsonValue& theMesh);

  //! Parse primitive array.
  Standard_EXPORT bool gltfParsePrimArray (TopoDS_Shape& thePrimArrayShape,
                                           const TCollection_AsciiString& theMeshId,
                                           const TCollection_AsciiString& theMeshName,
                                           const RWGltf_JsonValue& thePrimArray);

  //! Parse accessor.
  Standard_EXPORT bool gltfParseAccessor (const Handle(RWGltf_GltfLatePrimitiveArray)& theMeshData,
                                          const TCollection_AsciiString& theName,
                                          const RWGltf_JsonValue& theAccessor,
                                          const RWGltf_GltfArrayType theType,
                                          const RWGltf_JsonValue* theCompBuffView);

  //! Parse buffer view.
  Standard_EXPORT bool gltfParseBufferView (const Handle(RWGltf_GltfLatePrimitiveArray)& theMeshData,
                                            const TCollection_AsciiString& theName,
                                            const RWGltf_JsonValue& theBufferView,
                                            const RWGltf_GltfAccessor& theAccessor,
                                            const RWGltf_GltfArrayType theType);

  //! Parse buffer.
  Standard_EXPORT bool gltfParseBuffer (const Handle(RWGltf_GltfLatePrimitiveArray)& theMeshData,
                                        const TCollection_AsciiString& theName,
                                        const RWGltf_JsonValue& theBuffer,
                                        const RWGltf_GltfAccessor&   theAccessor,
                                        const RWGltf_GltfBufferView& theView,
                                        const RWGltf_GltfArrayType   theType);

protected:

  //! Read vec4 from specified item.
  static bool gltfReadVec4 (Graphic3d_Vec4d& theVec4,
                            const RWGltf_JsonValue* theVal)
  {
    if (theVal == NULL
    || !theVal->IsArray()
    ||  theVal->Size() != 4)
    {
      return false;
    }

    for (int aCompIter = 0; aCompIter < 4; ++aCompIter)
    {
      const RWGltf_JsonValue& aGenVal = (*theVal)[aCompIter];
      if (!aGenVal.IsNumber())
      {
        return false;
      }
      theVec4[aCompIter] = aGenVal.GetDouble();
    }
    return true;
  }

  //! Validate color
  static bool validateColor4 (const Graphic3d_Vec4d& theVec)
  {
    return theVec.r() >= 0.0 && theVec.r() <= 1.0
        && theVec.g() >= 0.0 && theVec.g() <= 1.0
        && theVec.b() >= 0.0 && theVec.b() <= 1.0
        && theVec.a() >= 0.0 && theVec.a() <= 1.0;
  }

  //! Read vec3 from specified item.
  static bool gltfReadVec3 (Graphic3d_Vec3d& theVec3,
                            const RWGltf_JsonValue* theVal)
  {
    if (theVal == NULL
    || !theVal->IsArray()
    ||  theVal->Size() != 3)
    {
      return false;
    }

    for (int aCompIter = 0; aCompIter < 3; ++aCompIter)
    {
      const RWGltf_JsonValue& aGenVal = (*theVal)[aCompIter];
      if (!aGenVal.IsNumber())
      {
        return false;
      }
      theVec3[aCompIter] = aGenVal.GetDouble();
    }
    return true;
  }

  //! Validate color
  static bool validateColor3 (const Graphic3d_Vec3d& theVec)
  {
    return theVec.r() >= 0.0 && theVec.r() <= 1.0
        && theVec.g() >= 0.0 && theVec.g() <= 1.0
        && theVec.b() >= 0.0 && theVec.b() <= 1.0;
  }

protected:

  //! Groups for re-using shapes.
  enum ShapeMapGroup
  {
    ShapeMapGroup_Nodes,     //!< nodes
    ShapeMapGroup_Meshes,    //!< meshes
    ShapeMapGroup_PrimArray, //!< primitive array
  };

  //! Bind name attribute.
  void bindNodeShape (TopoDS_Shape& theShape,
                      const TopLoc_Location& theLoc,
                      const TCollection_AsciiString& theNodeId,
                      const RWGltf_JsonValue* theUserName)
  {
    bindNamedShape (theShape, ShapeMapGroup_Nodes, theLoc, theNodeId, theUserName);
  }

  //! Bind name attribute.
  void bindMeshShape (TopoDS_Shape& theShape,
                      const TCollection_AsciiString& theMeshId,
                      const RWGltf_JsonValue* theUserName)
  {
    bindNamedShape (theShape, ShapeMapGroup_Meshes, TopLoc_Location(), theMeshId, theUserName);
  }

  //! Find named shape.
  bool findNodeShape (TopoDS_Shape& theShape,
                      const TCollection_AsciiString& theNodeId) const
  {
    return findNamedShape (theShape, ShapeMapGroup_Nodes, theNodeId);
  }

  //! Find named shape.
  bool findMeshShape (TopoDS_Shape& theShape,
                      const TCollection_AsciiString& theMeshId) const
  {
    return findNamedShape (theShape, ShapeMapGroup_Meshes, theMeshId);
  }

  //! Bind name attribute.
  Standard_EXPORT void bindNamedShape (TopoDS_Shape& theShape,
                                       ShapeMapGroup theGroup,
                                       const TopLoc_Location& theLoc,
                                       const TCollection_AsciiString& theId,
                                       const RWGltf_JsonValue* theUserName);

  //! Find named shape.
  bool findNamedShape (TopoDS_Shape& theShape,
                       ShapeMapGroup theGroup,
                       const TCollection_AsciiString& theId) const
  {
    return myShapeMap[theGroup].Find (theId, theShape);
  }

  //! Return the string representation of the key.
  static TCollection_AsciiString getKeyString (const RWGltf_JsonValue& theValue)
  {
    if (theValue.IsString())
    {
      return TCollection_AsciiString (theValue.GetString());
    }
    else if (theValue.IsInt())
    {
      return TCollection_AsciiString (theValue.GetInt());
    }
    return TCollection_AsciiString();
  }

protected:

  //! Auxiliary structure for fast look-up of document sub-nodes of specified node.
  class GltfElementMap
  {
  public:

    //! Empty constructor.
    GltfElementMap() : myRoot (NULL) {}

    //! Return TRUE if this element is NULL.
    bool IsNull() const { return myRoot == NULL; }

    //! Access this node.
    const RWGltf_JsonValue* Root() const { return myRoot; }

    //! Find the child node with specified key.
    const RWGltf_JsonValue* FindChild (const TCollection_AsciiString& theKey)
    {
      const RWGltf_JsonValue* aNode = NULL;
      return myChildren.Find (theKey, aNode)
           ? aNode
           : NULL;
    }

    //! Find the child node with specified key.
    const RWGltf_JsonValue* FindChild (const RWGltf_JsonValue& theKey)
    {
      const TCollection_AsciiString aKey = getKeyString (theKey);
      if (aKey.IsEmpty())
      {
        return NULL;
      }

      const RWGltf_JsonValue* aNode = NULL;
      return myChildren.Find (aKey, aNode)
           ? aNode
           : NULL;
    }

    //! Initialize the element.
    void Init (const TCollection_AsciiString& theRootName,
               const RWGltf_JsonValue* theRoot);

  private:

    NCollection_DataMap<TCollection_AsciiString, const RWGltf_JsonValue*, TCollection_AsciiString> myChildren;
    const RWGltf_JsonValue* myRoot;

  };
#endif
protected:

  //! Print message about invalid glTF syntax.
  void reportGltfSyntaxProblem (const TCollection_AsciiString& theMsg, Message_Gravity theGravity);

protected:

  TopTools_SequenceOfShape*        myRootShapes;    //!< sequence of result root shapes
  RWMesh_NodeAttributeMap*         myAttribMap;     //!< shape attributes
  NCollection_IndexedMap<TCollection_AsciiString>*
                                   myExternalFiles; //!< list of external file references
  RWMesh_CoordinateSystemConverter myCSTrsf;        //!< transformation from glTF to OCCT coordinate system
  TColStd_IndexedDataMapOfStringString* myMetadata; //!< file metadata

  NCollection_DataMap<TCollection_AsciiString, Handle(RWGltf_MaterialMetallicRoughness)> myMaterialsPbr;
  NCollection_DataMap<TCollection_AsciiString, Handle(RWGltf_MaterialCommon)> myMaterialsCommon;
  NCollection_DataMap<TCollection_AsciiString, Handle(XCAFDoc_VisMaterial)> myMaterials;
  NCollection_DataMap<TCollection_AsciiString, TopoDS_Shape> myShapeMap[3];

  NCollection_DataMap<TCollection_AsciiString, bool> myProbedFiles;
  NCollection_DataMap<TCollection_AsciiString, Handle(NCollection_Buffer)> myDecodedBuffers;
  NCollection_Vector<TopoDS_Face> myFaceList; //!< face list for loading triangulation

  TCollection_AsciiString   myFilePath;       //!< file path
  TCollection_AsciiString   myFolder;         //!< folder
  TCollection_AsciiString   myErrorPrefix;    //!< invalid syntax error prefix
  int64_t                   myBinBodyOffset;  //!< offset to binary body
  int64_t                   myBinBodyLen;     //!< binary body length
  bool                      myIsBinary;       //!< binary document
  bool                      myIsGltf1;        //!< obsolete glTF 1.0 version format
  bool                      myToSkipEmptyNodes; //!< ignore nodes without Geometry
  bool                      myToLoadAllScenes;  //!< flag to load all scenes in the document, FALSE by default
  bool                      myUseMeshNameAsFallback; //!< flag to use Mesh name in case if Node name is empty, TRUE by default
  bool                      myToProbeHeader;  //!< flag to probe header without full reading, FALSE by default

#ifdef HAVE_RAPIDJSON
  GltfElementMap myGltfRoots[RWGltf_GltfRootElement_NB]; //!< glTF format root elements
#endif

};

#endif // _RWGltf_GltfJsonParser_HeaderFile
