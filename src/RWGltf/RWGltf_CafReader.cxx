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

#include "RWGltf_GltfJsonParser.pxx"
#include <RWGltf_TriangulationReader.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressScope.hxx>
#include <OSD_OpenFile.hxx>
#include <OSD_ThreadPool.hxx>

#include <fstream>

IMPLEMENT_STANDARD_RTTIEXT(RWGltf_CafReader, RWMesh_CafReader)

//! Functor for parallel execution.
class RWGltf_CafReader::CafReader_GltfReaderFunctor
{
public:

  struct GltfReaderTLS
  {
    Handle(RWGltf_PrimitiveArrayReader) Reader;
  };

  //! Main constructor.
  CafReader_GltfReaderFunctor (RWGltf_CafReader* myCafReader,
                               NCollection_Vector<TopoDS_Face>& theFaceList,
                               const Message_ProgressRange& theProgress,
                               const OSD_ThreadPool::Launcher& theThreadPool,
                               const TCollection_AsciiString& theErrPrefix)
  : myCafReader (myCafReader),
    myFaceList  (&theFaceList),
    myErrPrefix (theErrPrefix),
    myProgress  (theProgress, "Loading glTF triangulation", Max (1, theFaceList.Size())),
    myThreadPool(theThreadPool),
    myTlsData   (theThreadPool.LowerThreadIndex(), theThreadPool.UpperThreadIndex())
  {
    //
  }

  //! Execute task for a face with specified index.
  void operator() (int theThreadIndex,
                   int theFaceIndex) const
  {
    GltfReaderTLS& aTlsData = myTlsData.ChangeValue (theThreadIndex);
    if (aTlsData.Reader.IsNull())
    {
      aTlsData.Reader = myCafReader->createMeshReaderContext();
      aTlsData.Reader->SetErrorPrefix (myErrPrefix);
      aTlsData.Reader->SetCoordinateSystemConverter (myCafReader->myCoordSysConverter);
    }

    TopLoc_Location aDummyLoc;
    TopoDS_Face& aFace = myFaceList->ChangeValue (theFaceIndex);
    Handle(RWGltf_GltfLatePrimitiveArray) aLateData = Handle(RWGltf_GltfLatePrimitiveArray)::DownCast (BRep_Tool::Triangulation (aFace, aDummyLoc));
    Handle(Poly_Triangulation) aPolyData = aTlsData.Reader->Load (aLateData);
    BRep_Builder aBuilder;
    aBuilder.UpdateFace (aFace, aPolyData);

    if (myThreadPool.HasThreads())
    {
      Standard_Mutex::Sentry aLock (&myMutex);
      myProgress.Next();
    }
    else
    {
      myProgress.Next();
    }
  }

private:

  RWGltf_CafReader* myCafReader;
  NCollection_Vector<TopoDS_Face>* myFaceList;
  TCollection_AsciiString   myErrPrefix;
  mutable Standard_Mutex    myMutex;
  mutable Message_ProgressScope myProgress;
  const OSD_ThreadPool::Launcher& myThreadPool;
  mutable NCollection_Array1<GltfReaderTLS> myTlsData;
};

//================================================================
// Function : Constructor
// Purpose  :
//================================================================
RWGltf_CafReader::RWGltf_CafReader()
: myToParallel (false),
  myToSkipEmptyNodes (true),
  myUseMeshNameAsFallback (true)
{
  myCoordSysConverter.SetInputLengthUnit (1.0); // glTF defines model in meters
  myCoordSysConverter.SetInputCoordinateSystem (RWMesh_CoordinateSystem_glTF);
}

//================================================================
// Function : performMesh
// Purpose  :
//================================================================
Standard_Boolean RWGltf_CafReader::performMesh (const TCollection_AsciiString& theFile,
                                                const Message_ProgressRange& theProgress,
                                                const Standard_Boolean theToProbe)
{
  Message_ProgressScope aPSentry (theProgress, "Reading glTF", 2);
  aPSentry.Show();

  std::ifstream aFile;
  OSD_OpenStream (aFile, theFile.ToCString(), std::ios::in | std::ios::binary);
  if (!aFile.is_open()
   || !aFile.good())
  {
    Message::SendFail (TCollection_AsciiString ("File '") + theFile + "' is not found");
    return false;
  }

  bool isBinaryFile = false;
  char aGlbHeader[12] = {};
  aFile.read (aGlbHeader, sizeof(aGlbHeader));
  int64_t aBinBodyOffset  = 0;
  int64_t aBinBodyLen     = 0;
  int64_t aJsonBodyOffset = 0;
  int64_t aJsonBodyLen    = 0;
  if (::strncmp (aGlbHeader, "glTF", 4) == 0)
  {
    isBinaryFile = true;
    const uint32_t* aVer = (const uint32_t* )(aGlbHeader + 4);
    const uint32_t* aLen = (const uint32_t* )(aGlbHeader + 8);
    if (*aVer == 1)
    {
      if (*aLen < 20)
      {
        Message::SendFail (TCollection_AsciiString ("File '") + theFile + "' has broken glTF format");
        return false;
      }

      char aHeader1[8] = {};
      aFile.read (aHeader1, sizeof(aHeader1));

      const uint32_t* aSceneLen    = (const uint32_t* )(aHeader1 + 0);
      const uint32_t* aSceneFormat = (const uint32_t* )(aHeader1 + 4);
      aJsonBodyOffset = 20;
      aJsonBodyLen    = int64_t(*aSceneLen);

      aBinBodyOffset = aJsonBodyOffset + aJsonBodyLen;
      aBinBodyLen    = int64_t(*aLen) - aBinBodyOffset;
      if (*aSceneFormat != 0)
      {
        Message::SendFail (TCollection_AsciiString ("File '") + theFile + "' is written using unsupported Scene format");
        return false;
      }
    }
    else //if (*aVer == 2)
    {
      if (*aVer != 2)
      {
        Message::SendWarning (TCollection_AsciiString ("File '") + theFile + "' is written using unknown version " + int(*aVer));
      }

      for (int aChunkIter = 0; !aFile.eof() && aChunkIter < 2; ++aChunkIter)
      {
        char aChunkHeader2[8] = {};
        if (int64_t(aFile.tellg()) + int64_t(sizeof(aChunkHeader2)) > int64_t(*aLen))
        {
          break;
        }

        aFile.read (aChunkHeader2, sizeof(aChunkHeader2));
        if (!aFile.good())
        {
          Message::SendFail (TCollection_AsciiString ("File '") + theFile + "' is written using unsupported format");
          return false;
        }

        const uint32_t* aChunkLen  = (const uint32_t* )(aChunkHeader2 + 0);
        const uint32_t* aChunkType = (const uint32_t* )(aChunkHeader2 + 4);
        if (*aChunkType == 0x4E4F534A)
        {
          aJsonBodyOffset = int64_t(aFile.tellg());
          aJsonBodyLen    = int64_t(*aChunkLen);
        }
        else if (*aChunkType == 0x004E4942)
        {
          aBinBodyOffset = int64_t(aFile.tellg());
          aBinBodyLen    = int64_t(*aChunkLen);
        }
        if (*aChunkLen != 0)
        {
          aFile.seekg (*aChunkLen, std::ios_base::cur);
        }
      }

      aFile.seekg ((std::streamoff )aJsonBodyOffset, std::ios_base::beg);
    }
  }
  else
  {
    aFile.seekg (0, std::ios_base::beg);
  }

  TCollection_AsciiString anErrPrefix = TCollection_AsciiString ("File '") + theFile + "' defines invalid glTF!\n";
  RWGltf_GltfJsonParser aDoc (myRootShapes);
  aDoc.SetFilePath (theFile);
  aDoc.SetProbeHeader (theToProbe);
  aDoc.SetExternalFiles (myExternalFiles);
  aDoc.SetMetadata (myMetadata);
  aDoc.SetErrorPrefix (anErrPrefix);
  aDoc.SetCoordinateSystemConverter (myCoordSysConverter);
  aDoc.SetSkipEmptyNodes (myToSkipEmptyNodes);
  aDoc.SetMeshNameAsFallback (myUseMeshNameAsFallback);
  if (!theToProbe)
  {
    aDoc.SetAttributeMap (myAttribMap);
  }
  if (isBinaryFile)
  {
    aDoc.SetBinaryFormat (aBinBodyOffset, aBinBodyLen);
  }

#ifdef HAVE_RAPIDJSON
  rapidjson::ParseResult aRes;
  rapidjson::IStreamWrapper aFileStream (aFile);
  if (isBinaryFile)
  {
    aRes = aDoc.ParseStream<rapidjson::kParseStopWhenDoneFlag, rapidjson::UTF8<>, rapidjson::IStreamWrapper> (aFileStream);
  }
  else
  {
    aRes = aDoc.ParseStream (aFileStream);
  }
  if (aRes.IsError())
  {
    if (aRes.Code() == rapidjson::kParseErrorDocumentEmpty)
    {
      Message::SendFail (TCollection_AsciiString ("File '") + theFile + "' is empty");
      return false;
    }
    TCollection_AsciiString anErrDesc (RWGltf_GltfJsonParser::FormatParseError (aRes.Code()));
    Message::SendFail (TCollection_AsciiString ("File '") + theFile + "' defines invalid JSON document!\n"
                     + anErrDesc + " [at offset " + (int )aRes.Offset() + "].");
    return false;
  }
#endif

  if (!aDoc.Parse (aPSentry.Next()))
  {
    return false;
  }

  if (!theToProbe
   && !readLateData (aDoc.FaceList(), theFile, aPSentry.Next()))
  {
    return false;
  }

  return true;
}

//================================================================
// Function : createMeshReaderContext
// Purpose  :
//================================================================
Handle(RWGltf_PrimitiveArrayReader) RWGltf_CafReader::createMeshReaderContext()
{
  Handle(RWGltf_TriangulationReader) aReader = new RWGltf_TriangulationReader();
  return aReader;
}

//================================================================
// Function : readLateData
// Purpose  :
//================================================================
Standard_Boolean RWGltf_CafReader::readLateData (NCollection_Vector<TopoDS_Face>& theFaces,
                                                 const TCollection_AsciiString& theFile,
                                                 const Message_ProgressRange& theProgress)
{
  const Handle(OSD_ThreadPool)& aThreadPool = OSD_ThreadPool::DefaultPool();
  const int aNbThreads = myToParallel ? Min (theFaces.Size(), aThreadPool->NbDefaultThreadsToLaunch()) : 1;
  OSD_ThreadPool::Launcher aLauncher (*aThreadPool, aNbThreads);

  CafReader_GltfReaderFunctor aFunctor (this, theFaces, theProgress, aLauncher,
                                        TCollection_AsciiString ("File '") + theFile + "' defines invalid glTF!\n");
  aLauncher.Perform (theFaces.Lower(), theFaces.Upper() + 1, aFunctor);
  return Standard_True;
}
