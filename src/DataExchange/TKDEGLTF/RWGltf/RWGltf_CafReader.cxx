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

#include <RWGltf_CafReader.hxx>

#include "RWGltf_GltfJsonParser.hxx"
#include <RWGltf_TriangulationReader.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressScope.hxx>
#include <OSD_CachedFileSystem.hxx>
#include <OSD_FileSystem.hxx>
#include <OSD_ThreadPool.hxx>
#include <RWGltf_GltfLatePrimitiveArray.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeMapTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(RWGltf_CafReader, RWMesh_CafReader)

//! Abstract base functor for parallel execution of glTF data loading.
class RWGltf_CafReader::CafReader_GltfBaseLoadingFunctor
{
public:
  //! Main constructor.
  CafReader_GltfBaseLoadingFunctor(NCollection_Vector<TopoDS_Face>& theFaceList,
                                   const Message_ProgressRange&     theProgress,
                                   const OSD_ThreadPool::Launcher&  theThreadPool)
      : myFaceList(&theFaceList),
        myProgress(theProgress, "Loading glTF triangulation", std::max(1, theFaceList.Size())),
        myThreadPool(theThreadPool)
  {
    //
  }

  //! Execute task for a face with specified index.
  void operator()(int theThreadIndex, int theFaceIndex) const
  {
    TopLoc_Location                       aDummyLoc;
    TopoDS_Face&                          aFace = myFaceList->ChangeValue(theFaceIndex);
    Handle(RWGltf_GltfLatePrimitiveArray) aLateData =
      Handle(RWGltf_GltfLatePrimitiveArray)::DownCast(BRep_Tool::Triangulation(aFace, aDummyLoc));
    Handle(Poly_Triangulation) aPolyData = loadData(aLateData, theThreadIndex);
    if (!aPolyData.IsNull())
    {
      BRep_Builder aBuilder;
      // clang-format off
      aBuilder.UpdateFace (aFace, aPolyData); // replace all "proxy"-triangulations of face by loaded active one.
      // clang-format on
    }
    if (myThreadPool.HasThreads())
    {
      std::lock_guard<std::mutex> aLock(myMutex);
      myProgress.Next();
    }
    else
    {
      myProgress.Next();
    }
  }

protected:
  //! Load primitive array.
  virtual Handle(Poly_Triangulation) loadData(
    const Handle(RWGltf_GltfLatePrimitiveArray)& theLateData,
    int                                          theThreadIndex) const = 0;

protected:
  NCollection_Vector<TopoDS_Face>* myFaceList;
  mutable std::mutex               myMutex;
  mutable Message_ProgressScope    myProgress;
  const OSD_ThreadPool::Launcher&  myThreadPool;
};

//! Functor for parallel execution of all glTF data loading.
class RWGltf_CafReader::CafReader_GltfFullDataLoadingFunctor
    : public RWGltf_CafReader::CafReader_GltfBaseLoadingFunctor
{
public:
  struct GltfReaderTLS
  {
    Handle(OSD_FileSystem) FileSystem;
  };

  //! Main constructor.
  CafReader_GltfFullDataLoadingFunctor(RWGltf_CafReader*                myCafReader,
                                       NCollection_Vector<TopoDS_Face>& theFaceList,
                                       const Message_ProgressRange&     theProgress,
                                       const OSD_ThreadPool::Launcher&  theThreadPool)
      : CafReader_GltfBaseLoadingFunctor(theFaceList, theProgress, theThreadPool),
        myCafReader(myCafReader),
        myTlsData(theThreadPool.LowerThreadIndex(), theThreadPool.UpperThreadIndex())
  {
    //
  }

protected:
  //! Load primitive array.
  virtual Handle(Poly_Triangulation) loadData(
    const Handle(RWGltf_GltfLatePrimitiveArray)& theLateData,
    int                                          theThreadIndex) const Standard_OVERRIDE
  {
    GltfReaderTLS& aTlsData = myTlsData.ChangeValue(theThreadIndex);
    if (aTlsData.FileSystem.IsNull())
    {
      aTlsData.FileSystem = new OSD_CachedFileSystem();
    }
    // Load stream data if exists
    if (Handle(Poly_Triangulation) aStreamLoadedData = theLateData->LoadStreamData())
    {
      return aStreamLoadedData;
    }
    // Load file data
    if (myCafReader->ToKeepLateData())
    {
      theLateData->LoadDeferredData(aTlsData.FileSystem);
      return Handle(Poly_Triangulation)();
    }
    return theLateData->DetachedLoadDeferredData(aTlsData.FileSystem);
  }

private:
  RWGltf_CafReader*                         myCafReader;
  mutable NCollection_Array1<GltfReaderTLS> myTlsData;
};

//! Functor for parallel execution of loading of only glTF data saved in stream buffers.
class RWGltf_CafReader::CafReader_GltfStreamDataLoadingFunctor
    : public RWGltf_CafReader::CafReader_GltfBaseLoadingFunctor
{
public:
  //! Main constructor.
  CafReader_GltfStreamDataLoadingFunctor(NCollection_Vector<TopoDS_Face>& theFaceList,
                                         const Message_ProgressRange&     theProgress,
                                         const OSD_ThreadPool::Launcher&  theThreadPool)
      : CafReader_GltfBaseLoadingFunctor(theFaceList, theProgress, theThreadPool)
  {
    //
  }

protected:
  //! Load primitive array.
  virtual Handle(Poly_Triangulation) loadData(
    const Handle(RWGltf_GltfLatePrimitiveArray)& theLateData,
    int                                          theThreadIndex) const Standard_OVERRIDE
  {
    (void)theThreadIndex;
    return theLateData->LoadStreamData();
  }
};

//=================================================================================================

RWGltf_CafReader::RWGltf_CafReader()
    : myToParallel(false),
      myToSkipEmptyNodes(true),
      myToLoadAllScenes(false),
      myUseMeshNameAsFallback(true),
      myIsDoublePrecision(false),
      myToSkipLateDataLoading(false),
      myToKeepLateData(true),
      myToPrintDebugMessages(false),
      myToApplyScale(true)
{
  myCoordSysConverter.SetInputLengthUnit(1.0); // glTF defines model in meters
  myCoordSysConverter.SetInputCoordinateSystem(RWMesh_CoordinateSystem_glTF);
}

//=================================================================================================

Standard_Boolean RWGltf_CafReader::performMesh(std::istream&                  theStream,
                                               const TCollection_AsciiString& theFile,
                                               const Message_ProgressRange&   theProgress,
                                               const Standard_Boolean         theToProbe)
{
  Message_ProgressScope aPSentry(theProgress, "Reading glTF", 2);
  aPSentry.Show();

  if (!theStream.good())
  {
    Message::SendFail(TCollection_AsciiString("File '") + theFile + "' is not found");
    return false;
  }

  bool isBinaryFile   = false;
  char aGlbHeader[12] = {};
  theStream.read(aGlbHeader, sizeof(aGlbHeader));
  int64_t aBinBodyOffset  = 0;
  int64_t aBinBodyLen     = 0;
  int64_t aJsonBodyOffset = 0;
  int64_t aJsonBodyLen    = 0;
  if (::strncmp(aGlbHeader, "glTF", 4) == 0)
  {
    isBinaryFile         = true;
    const uint32_t* aVer = (const uint32_t*)(aGlbHeader + 4);
    const uint32_t* aLen = (const uint32_t*)(aGlbHeader + 8);
    if (*aVer == 1)
    {
      if (*aLen < 20)
      {
        Message::SendFail(TCollection_AsciiString("File '") + theFile + "' has broken glTF format");
        return false;
      }

      char aHeader1[8] = {};
      theStream.read(aHeader1, sizeof(aHeader1));

      const uint32_t* aSceneLen    = (const uint32_t*)(aHeader1 + 0);
      const uint32_t* aSceneFormat = (const uint32_t*)(aHeader1 + 4);
      aJsonBodyOffset              = 20;
      aJsonBodyLen                 = int64_t(*aSceneLen);

      aBinBodyOffset = aJsonBodyOffset + aJsonBodyLen;
      aBinBodyLen    = int64_t(*aLen) - aBinBodyOffset;
      if (*aSceneFormat != 0)
      {
        Message::SendFail(TCollection_AsciiString("File '") + theFile
                          + "' is written using unsupported Scene format");
        return false;
      }
    }
    else // if (*aVer == 2)
    {
      if (*aVer != 2)
      {
        Message::SendWarning(TCollection_AsciiString("File '") + theFile
                             + "' is written using unknown version " + int(*aVer));
      }

      for (int aChunkIter = 0; !theStream.eof() && aChunkIter < 2; ++aChunkIter)
      {
        char aChunkHeader2[8] = {};
        if (int64_t(theStream.tellg()) + int64_t(sizeof(aChunkHeader2)) > int64_t(*aLen))
        {
          break;
        }

        theStream.read(aChunkHeader2, sizeof(aChunkHeader2));
        if (!theStream.good())
        {
          Message::SendFail(TCollection_AsciiString("File '") + theFile
                            + "' is written using unsupported format");
          return false;
        }

        const uint32_t* aChunkLen  = (const uint32_t*)(aChunkHeader2 + 0);
        const uint32_t* aChunkType = (const uint32_t*)(aChunkHeader2 + 4);
        if (*aChunkType == 0x4E4F534A)
        {
          aJsonBodyOffset = int64_t(theStream.tellg());
          aJsonBodyLen    = int64_t(*aChunkLen);
        }
        else if (*aChunkType == 0x004E4942)
        {
          aBinBodyOffset = int64_t(theStream.tellg());
          aBinBodyLen    = int64_t(*aChunkLen);
        }
        if (*aChunkLen != 0)
        {
          theStream.seekg(*aChunkLen, std::ios_base::cur);
        }
      }

      theStream.seekg((std::streamoff)aJsonBodyOffset, std::ios_base::beg);
    }
  }
  else
  {
    theStream.seekg(0, std::ios_base::beg);
  }

  TCollection_AsciiString anErrPrefix =
    TCollection_AsciiString("File '") + theFile + "' defines invalid glTF!\n";
  RWGltf_GltfJsonParser aDoc(myRootShapes);
  aDoc.SetFilePath(theFile);
  aDoc.SetProbeHeader(theToProbe);
  aDoc.SetExternalFiles(myExternalFiles);
  aDoc.SetMetadata(myMetadata);
  aDoc.SetErrorPrefix(anErrPrefix);
  aDoc.SetCoordinateSystemConverter(myCoordSysConverter);
  aDoc.SetSkipEmptyNodes(myToSkipEmptyNodes);
  aDoc.SetLoadAllScenes(myToLoadAllScenes);
  aDoc.SetMeshNameAsFallback(myUseMeshNameAsFallback);
  if (!theToProbe)
  {
    aDoc.SetAttributeMap(myAttribMap);
  }
  if (isBinaryFile)
  {
    aDoc.SetBinaryFormat(aBinBodyOffset, aBinBodyLen);
  }
  myShapeScaleMap = new NCollection_DataMap<TopoDS_Shape, gp_XYZ, TopTools_ShapeMapHasher>();
  aDoc.SetToApplyScale(myToApplyScale);
  aDoc.SetScaleMap(*myShapeScaleMap);

#ifdef HAVE_RAPIDJSON
  rapidjson::ParseResult    aRes;
  rapidjson::IStreamWrapper aFileStream(theStream);
  if (isBinaryFile)
  {
    aRes = aDoc.ParseStream<rapidjson::kParseStopWhenDoneFlag,
                            rapidjson::UTF8<>,
                            rapidjson::IStreamWrapper>(aFileStream);
  }
  else
  {
    aRes = aDoc.ParseStream(aFileStream);
  }
  if (aRes.IsError())
  {
    if (aRes.Code() == rapidjson::kParseErrorDocumentEmpty)
    {
      Message::SendFail(TCollection_AsciiString("File '") + theFile + "' is empty");
      return false;
    }
    TCollection_AsciiString anErrDesc(RWGltf_GltfJsonParser::FormatParseError(aRes.Code()));
    Message::SendFail(TCollection_AsciiString("File '") + theFile
                      + "' defines invalid JSON document!\n" + anErrDesc + " [at offset "
                      + (int)aRes.Offset() + "].");
    return false;
  }
#endif

  if (!aDoc.Parse(aPSentry.Next()))
  {
    return false;
  }

  if (!theToProbe && !readLateData(aDoc.FaceList(), theFile, aPSentry.Next()))
  {
    return false;
  }

  return true;
}

//=================================================================================================

Handle(RWMesh_TriangulationReader) RWGltf_CafReader::createMeshReaderContext() const
{
  Handle(RWGltf_TriangulationReader) aReader = new RWGltf_TriangulationReader();
  aReader->SetDoublePrecision(myIsDoublePrecision);
  aReader->SetCoordinateSystemConverter(myCoordSysConverter);
  aReader->SetToSkipDegenerates(false);
  aReader->SetToPrintDebugMessages(myToPrintDebugMessages);
  return aReader;
}

//=================================================================================================

Standard_Boolean RWGltf_CafReader::readLateData(NCollection_Vector<TopoDS_Face>& theFaces,
                                                const TCollection_AsciiString&   theFile,
                                                const Message_ProgressRange&     theProgress)
{
  Handle(RWGltf_TriangulationReader) aReader =
    Handle(RWGltf_TriangulationReader)::DownCast(createMeshReaderContext());
  aReader->SetFileName(theFile);
  updateLateDataReader(theFaces, aReader);

  if (myToSkipLateDataLoading)
  {
    // Load glTF data encoded in base64. It should not be skipped and saved in "proxy" object to be
    // loaded later.
    const Handle(OSD_ThreadPool)& aThreadPool = OSD_ThreadPool::DefaultPool();
    const int                     aNbThreads =
      myToParallel ? std::min(theFaces.Size(), aThreadPool->NbDefaultThreadsToLaunch()) : 1;
    OSD_ThreadPool::Launcher               aLauncher(*aThreadPool, aNbThreads);
    CafReader_GltfStreamDataLoadingFunctor aFunctor(theFaces, theProgress, aLauncher);
    aLauncher.Perform(theFaces.Lower(), theFaces.Upper() + 1, aFunctor);

    return Standard_True;
  }

  aReader->StartStatistic();

  const Handle(OSD_ThreadPool)& aThreadPool = OSD_ThreadPool::DefaultPool();
  const int                     aNbThreads =
    myToParallel ? std::min(theFaces.Size(), aThreadPool->NbDefaultThreadsToLaunch()) : 1;
  OSD_ThreadPool::Launcher aLauncher(*aThreadPool, aNbThreads);

  CafReader_GltfFullDataLoadingFunctor aFunctor(this, theFaces, theProgress, aLauncher);
  aLauncher.Perform(theFaces.Lower(), theFaces.Upper() + 1, aFunctor);

  aReader->PrintStatistic();
  aReader->StopStatistic();

  return Standard_True;
}

//=================================================================================================

void RWGltf_CafReader::updateLateDataReader(
  NCollection_Vector<TopoDS_Face>&          theFaces,
  const Handle(RWMesh_TriangulationReader)& theReader) const
{
  TopLoc_Location aDummyLoc;
  for (NCollection_Vector<TopoDS_Face>::Iterator aFaceIter(theFaces); aFaceIter.More();
       aFaceIter.Next())
  {
    const TopoDS_Face& aFace = aFaceIter.Value();
    for (Poly_ListOfTriangulation::Iterator anIter(BRep_Tool::Triangulations(aFace, aDummyLoc));
         anIter.More();
         anIter.Next())
    {
      Handle(RWGltf_GltfLatePrimitiveArray) aData =
        Handle(RWGltf_GltfLatePrimitiveArray)::DownCast(anIter.Value());
      if (!aData.IsNull())
      {
        aData->SetReader(theReader);
      }
    }
  }
}

//=================================================================================================

void RWGltf_CafReader::fillDocument()
{
  if (!myToFillDoc || myXdeDoc.IsNull() || myRootShapes.IsEmpty())
  {
    return;
  }
  // set units
  Standard_Real aLengthUnit = 1.;
  if (!XCAFDoc_DocumentTool::GetLengthUnit(myXdeDoc, aLengthUnit))
  {
    XCAFDoc_DocumentTool::SetLengthUnit(myXdeDoc, SystemLengthUnit());
  }
  else if (aLengthUnit != SystemLengthUnit())
  {
    Message::SendWarning("Warning: Length unit of document not equal to the system length unit");
  }

  const Standard_Boolean wasAutoNaming = XCAFDoc_ShapeTool::AutoNaming();
  XCAFDoc_ShapeTool::SetAutoNaming(Standard_False);
  const TCollection_AsciiString aRootName; // = generateRootName (theFile);
  CafDocumentTools              aTools;
  aTools.ShapeTool       = XCAFDoc_DocumentTool::ShapeTool(myXdeDoc->Main());
  aTools.ColorTool       = XCAFDoc_DocumentTool::ColorTool(myXdeDoc->Main());
  aTools.VisMaterialTool = XCAFDoc_DocumentTool::VisMaterialTool(myXdeDoc->Main());
  for (TopTools_SequenceOfShape::Iterator aRootIter(myRootShapes); aRootIter.More();
       aRootIter.Next())
  {
    addShapeIntoDoc(aTools, aRootIter.Value(), TDF_Label(), aRootName);
  }
  XCAFDoc_DocumentTool::ShapeTool(myXdeDoc->Main())->UpdateAssemblies();
  XCAFDoc_ShapeTool::SetAutoNaming(wasAutoNaming);
}

//=================================================================================================

Standard_Boolean RWGltf_CafReader::addShapeIntoDoc(CafDocumentTools&              theTools,
                                                   const TopoDS_Shape&            theShape,
                                                   const TDF_Label&               theLabel,
                                                   const TCollection_AsciiString& theParentName,
                                                   const Standard_Boolean         theHasScale,
                                                   const gp_XYZ&                  theScale)
{
  if (theShape.IsNull() || myXdeDoc.IsNull())
  {
    return Standard_False;
  }

  const TopAbs_ShapeEnum aShapeType     = theShape.ShapeType();
  TopoDS_Shape           aShapeToAdd    = theShape;
  const TopoDS_Shape     aShapeNoLoc    = theShape.Located(TopLoc_Location());
  Standard_Boolean       toMakeAssembly = Standard_False;
  bool                   isShapeScaled  = myShapeScaleMap->IsBound(theShape);
  gp_XYZ                 aCurScale;

  if (theHasScale)
  {
    // update translation part
    gp_Trsf aTrsf        = theShape.Location().Transformation();
    gp_XYZ  aTranslation = aTrsf.TranslationPart();
    aTranslation.SetX(aTranslation.X() * theScale.X());
    aTranslation.SetY(aTranslation.Y() * theScale.Y());
    aTranslation.SetZ(aTranslation.Z() * theScale.Z());
    aTrsf.SetTranslationPart(aTranslation);
    aShapeToAdd.Location(TopLoc_Location(aTrsf));
  }

  if (isShapeScaled && aShapeType == TopAbs_FACE)
  {
    // Scale triangulation
    aCurScale = myShapeScaleMap->Find(theShape);
    TopLoc_Location aLoc;
    TopoDS_Face     aFace = TopoDS::Face(aShapeToAdd);
    myShapeScaleMap->UnBind(theShape);
    const Handle(Poly_Triangulation)& aPolyTri = BRep_Tool::Triangulation(aFace, aLoc);
    if (!aPolyTri.IsNull())
    {
      for (int aNodeIdx = 1; aNodeIdx <= aPolyTri->NbNodes(); ++aNodeIdx)
      {
        gp_Pnt aNode = aPolyTri->Node(aNodeIdx);
        aNode.SetX(aNode.X() * aCurScale.X());
        aNode.SetY(aNode.Y() * aCurScale.Y());
        aNode.SetZ(aNode.Z() * aCurScale.Z());
        aPolyTri->SetNode(aNodeIdx, aNode);
      }
    }
  }

  if (theShape.ShapeType() == TopAbs_COMPOUND)
  {
    RWMesh_NodeAttributes aSubFaceAttribs;
    for (TopoDS_Iterator aSubShapeIter(theShape, Standard_True, Standard_False);
         !toMakeAssembly && aSubShapeIter.More();
         aSubShapeIter.Next())
    {
      if (aSubShapeIter.Value().ShapeType() != TopAbs_FACE)
      {
        toMakeAssembly = Standard_True;
        break;
      }

      const TopoDS_Face& aFace = TopoDS::Face(aSubShapeIter.Value());
      toMakeAssembly =
        toMakeAssembly
        || (myAttribMap.Find(aFace, aSubFaceAttribs) && !aSubFaceAttribs.Name.IsEmpty());
    }

    if (toMakeAssembly)
    {
      // create an empty Compound to add as assembly, so that we can add children one-by-one via
      // AddComponent()
      TopoDS_Compound aCompound;
      BRep_Builder    aBuilder;
      aBuilder.MakeCompound(aCompound);
      aCompound.Location(theShape.Location(), Standard_False);
      aShapeToAdd = aCompound;
    }
  }

  TDF_Label aNewLabel, anOldLabel;
  if (theLabel.IsNull())
  {
    // add new shape
    aNewLabel = theTools.ShapeTool->AddShape(aShapeToAdd, toMakeAssembly);
  }
  else if (theTools.ShapeTool->IsAssembly(theLabel))
  {
    // add shape as component
    if (theTools.ComponentMap.Find(aShapeNoLoc, anOldLabel))
    {
      aNewLabel = theTools.ShapeTool->AddComponent(theLabel, anOldLabel, theShape.Location());
    }
    else
    {
      aNewLabel = theTools.ShapeTool->AddComponent(theLabel, aShapeToAdd, toMakeAssembly);

      TDF_Label aRefLabel = aNewLabel;
      theTools.ShapeTool->GetReferredShape(aNewLabel, aRefLabel);
      if (!aRefLabel.IsNull())
      {
        theTools.ComponentMap.Bind(aShapeNoLoc, aRefLabel);
      }
    }
  }
  else
  {
    // add shape as sub-shape
    aNewLabel = theTools.ShapeTool->AddSubShape(theLabel, theShape);
    if (!aNewLabel.IsNull())
    {
      Handle(XCAFDoc_ShapeMapTool) aShapeMapTool = XCAFDoc_ShapeMapTool::Set(aNewLabel);
      aShapeMapTool->SetShape(theShape);
    }
  }
  if (aNewLabel.IsNull())
  {
    return Standard_False;
  }

  if (toMakeAssembly)
  {
    TDF_Label aRefLabel;
    theTools.ShapeTool->GetReferredShape(aNewLabel, aRefLabel);
    if (!aRefLabel.IsNull())
    {
      theTools.OriginalShapeMap.Bind(theShape, aRefLabel);
    }
  }

  // if new label is a reference get referred shape
  TDF_Label aNewRefLabel = aNewLabel;
  theTools.ShapeTool->GetReferredShape(aNewLabel, aNewRefLabel);

  RWMesh_NodeAttributes aRefShapeAttribs;
  myAttribMap.Find(aShapeNoLoc, aRefShapeAttribs);

  bool hasProductName = false;
  if (aNewLabel != aNewRefLabel)
  {
    // put attributes to the Instance (overrides Product attributes)
    RWMesh_NodeAttributes aShapeAttribs;
    if (!theShape.Location().IsIdentity() && myAttribMap.Find(theShape, aShapeAttribs))
    {
      if (!aShapeAttribs.Style.IsEqual(aRefShapeAttribs.Style))
      {
        setShapeStyle(theTools, aNewLabel, aShapeAttribs.Style);
      }
      if (aShapeAttribs.NamedData != aRefShapeAttribs.NamedData)
      {
        setShapeNamedData(theTools, aNewLabel, aShapeAttribs.NamedData);
      }
      setShapeName(aNewLabel, aShapeType, aShapeAttribs.Name, theLabel, theParentName);
      if (aRefShapeAttribs.Name.IsEmpty() && !aShapeAttribs.Name.IsEmpty())
      {
        // it is not nice having unnamed Product, so copy name from first Instance (probably the
        // only one)
        hasProductName = true;
        setShapeName(aNewRefLabel, aShapeType, aShapeAttribs.Name, theLabel, theParentName);
      }
      else if (aShapeAttribs.Name.IsEmpty() && !aRefShapeAttribs.Name.IsEmpty())
      {
        // copy name from Product
        setShapeName(aNewLabel, aShapeType, aRefShapeAttribs.Name, theLabel, theParentName);
      }
    }
    else
    {
      // copy name from Product
      setShapeName(aNewLabel, aShapeType, aRefShapeAttribs.Name, theLabel, theParentName);
    }
  }

  if (!anOldLabel.IsNull())
  {
    // already defined in the document
    return Standard_True;
  }

  // put attributes to the Product (shared across Instances)
  if (!hasProductName)
  {
    setShapeName(aNewRefLabel, aShapeType, aRefShapeAttribs.Name, theLabel, theParentName);
  }
  setShapeStyle(theTools, aNewRefLabel, aRefShapeAttribs.Style);
  setShapeNamedData(theTools, aNewRefLabel, aRefShapeAttribs.NamedData);

  if (theTools.ShapeTool->IsAssembly(aNewRefLabel))
  {
    bool   aHasScale = theHasScale | isShapeScaled;
    gp_XYZ aScale    = theScale;
    if (isShapeScaled)
    {
      aScale.SetX(aCurScale.X() * theScale.X());
      aScale.SetY(aCurScale.Y() * theScale.Y());
      aScale.SetZ(aCurScale.Z() * theScale.Z());
    }
    // store sub-shapes (iterator is set to not inherit Location of parent object)
    TCollection_AsciiString aDummyName;
    for (TopoDS_Iterator aSubShapeIter(theShape, Standard_True, Standard_False);
         aSubShapeIter.More();
         aSubShapeIter.Next())
    {
      addShapeIntoDoc(theTools, aSubShapeIter.Value(), aNewRefLabel, aDummyName, aHasScale, aScale);
    }
  }
  else
  {
    // store a plain list of sub-shapes in case if they have custom attributes (usually per-face
    // color)
    for (TopoDS_Iterator aSubShapeIter(theShape, Standard_True, Standard_False);
         aSubShapeIter.More();
         aSubShapeIter.Next())
    {
      addSubShapeIntoDoc(theTools, aSubShapeIter.Value(), aNewRefLabel);
    }
  }
  return Standard_True;
}
