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

#include <RWGltf_Provider.hxx>

#include <BinXCAFDrivers.hxx>
#include <Message.hxx>
#include <RWGltf_CafWriter.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>

namespace 
{
  //=======================================================================
  // function : SetReaderParameters
  // purpose  :
  //=======================================================================
  static void SetReaderParameters(RWGltf_CafReader& theReader, const Handle(RWGltf_ConfigurationNode)& theNode)
  {
    theReader.SetDoublePrecision(!theNode->InternalParameters.ReadSinglePrecision);
    theReader.SetSystemLengthUnit(theNode->GlobalParameters.LengthUnit / 1000);
    theReader.SetSystemCoordinateSystem(theNode->InternalParameters.SystemCS);
    theReader.SetFileLengthUnit(theNode->InternalParameters.FileLengthUnit);
    theReader.SetFileCoordinateSystem(theNode->InternalParameters.FileCS);
    theReader.SetRootPrefix(theNode->InternalParameters.ReadRootPrefix);
    theReader.SetMemoryLimitMiB(theNode->InternalParameters.ReadMemoryLimitMiB);

    theReader.SetParallel(theNode->InternalParameters.ReadParallel);
    theReader.SetSkipEmptyNodes(theNode->InternalParameters.ReadSkipEmptyNodes);
    theReader.SetLoadAllScenes(theNode->InternalParameters.ReadLoadAllScenes);
    theReader.SetMeshNameAsFallback(theNode->InternalParameters.ReadUseMeshNameAsFallback);
    theReader.SetToSkipLateDataLoading(theNode->InternalParameters.ReadSkipLateDataLoading);
    theReader.SetToKeepLateData(theNode->InternalParameters.ReadKeepLateData);
    theReader.SetToPrintDebugMessages(theNode->InternalParameters.ReadPrintDebugMessages);
  }
}

IMPLEMENT_STANDARD_RTTIEXT(RWGltf_Provider, DE_Provider)

//=======================================================================
// function : RWGltf_Provider
// purpose  :
//=======================================================================
RWGltf_Provider::RWGltf_Provider()
{}

//=======================================================================
// function : RWGltf_Provider
// purpose  :
//=======================================================================
RWGltf_Provider::RWGltf_Provider(const Handle(DE_ConfigurationNode)& theNode)
  :DE_Provider(theNode)
{}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool RWGltf_Provider::Read(const TCollection_AsciiString& thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           Handle(XSControl_WorkSession)& theWS,
                           const Message_ProgressRange& theProgress)
{
  (void)theWS;
  return Read(thePath, theDocument, theProgress);
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool RWGltf_Provider::Write(const TCollection_AsciiString& thePath,
                            const Handle(TDocStd_Document)& theDocument,
                            Handle(XSControl_WorkSession)& theWS,
                            const Message_ProgressRange& theProgress)
{
  (void)theWS;
  return Write(thePath, theDocument, theProgress);
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool RWGltf_Provider::Read(const TCollection_AsciiString& thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           const Message_ProgressRange& theProgress)
{
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the RWGltf_Provider during reading the file " <<
      thePath << "\t: theDocument shouldn't be null";
    return false;
  }
  if (GetNode().IsNull() || (!GetNode().IsNull() && !GetNode()->IsKind(STANDARD_TYPE(RWGltf_ConfigurationNode))))
  {
    Message::SendFail() << "Error in the RWGltf_Provider during reading the file " <<
      thePath << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(RWGltf_ConfigurationNode) aNode = Handle(RWGltf_ConfigurationNode)::DownCast(GetNode());
  RWGltf_CafReader aReader;
  aReader.SetDocument(theDocument);
  SetReaderParameters(aReader, aNode);
  XCAFDoc_DocumentTool::SetLengthUnit(theDocument, aNode->GlobalParameters.LengthUnit, UnitsMethods_LengthUnit_Millimeter);
  if (!aReader.Perform(thePath, theProgress))
  {
    Message::SendFail() << "Error in the RWGltf_Provider during reading the file " << thePath;
    return false;
  }
  
  return true;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool RWGltf_Provider::Write(const TCollection_AsciiString& thePath,
                            const Handle(TDocStd_Document)& theDocument,
                            const Message_ProgressRange& theProgress)
{
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(RWGltf_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the RWGltf_Provider during writing the file " <<
      thePath << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(RWGltf_ConfigurationNode) aNode = Handle(RWGltf_ConfigurationNode)::DownCast(GetNode());

  RWMesh_CoordinateSystemConverter aConverter;
  aConverter.SetInputLengthUnit(aNode->GlobalParameters.LengthUnit / 1000);
  aConverter.SetInputCoordinateSystem(aNode->InternalParameters.SystemCS);
  aConverter.SetOutputLengthUnit(aNode->InternalParameters.FileLengthUnit);
  aConverter.SetOutputCoordinateSystem(aNode->InternalParameters.FileCS);

  TColStd_IndexedDataMapOfStringString aFileInfo;
  if (!aNode->InternalParameters.WriteAuthor.IsEmpty())
  {
    aFileInfo.Add("Author", aNode->InternalParameters.WriteAuthor);
  }
  if (!aNode->InternalParameters.WriteComment.IsEmpty())
  {
    aFileInfo.Add("Comments", aNode->InternalParameters.WriteComment);
  }

  TCollection_AsciiString anExt = thePath;
  anExt.LowerCase();
  RWGltf_CafWriter aWriter(thePath, anExt.EndsWith(".glb"));
  aWriter.SetCoordinateSystemConverter(aConverter);
  aWriter.SetTransformationFormat(aNode->InternalParameters.WriteTrsfFormat);
  aWriter.SetNodeNameFormat(aNode->InternalParameters.WriteNodeNameFormat);
  aWriter.SetMeshNameFormat(aNode->InternalParameters.WriteMeshNameFormat);
  aWriter.SetForcedUVExport(aNode->InternalParameters.WriteForcedUVExport);
  aWriter.SetToEmbedTexturesInGlb(aNode->InternalParameters.WriteEmbedTexturesInGlb);
  aWriter.SetMergeFaces(aNode->InternalParameters.WriteMergeFaces);
  aWriter.SetSplitIndices16(aNode->InternalParameters.WriteSplitIndices16);
  if (!aWriter.Perform(theDocument, aFileInfo, theProgress))
  {
    Message::SendFail() << "Error in the RWGltf_Provider during writing the file " << thePath;
    return false;
  }
  return true;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool RWGltf_Provider::Read(const TCollection_AsciiString& thePath,
                           TopoDS_Shape& theShape,
                           Handle(XSControl_WorkSession)& theWS,
                           const Message_ProgressRange& theProgress)
{
  (void)theWS;
  return Read(thePath, theShape, theProgress);
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool RWGltf_Provider::Write(const TCollection_AsciiString& thePath,
                            const TopoDS_Shape& theShape,
                            Handle(XSControl_WorkSession)& theWS,
                            const Message_ProgressRange& theProgress)
{
  (void)theWS;
  return Write(thePath, theShape, theProgress);
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool RWGltf_Provider::Read(const TCollection_AsciiString& thePath,
                           TopoDS_Shape& theShape,
                           const Message_ProgressRange& theProgress)
{
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(RWGltf_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the RWGltf_Provider during reading the file " <<
      thePath << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(RWGltf_ConfigurationNode) aNode = Handle(RWGltf_ConfigurationNode)::DownCast(GetNode());
  RWGltf_CafReader aReader;
  SetReaderParameters(aReader, aNode);
  if (!aReader.Perform(thePath, theProgress))
  {
    Message::SendFail() << "Error in the RWGltf_Provider during reading the file " << thePath;
    return false;
  }
  theShape = aReader.SingleShape();
  return true;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool RWGltf_Provider::Write(const TCollection_AsciiString& thePath,
                            const TopoDS_Shape& theShape,
                            const Message_ProgressRange& theProgress)
{
  Handle(TDocStd_Document) aDoc = new TDocStd_Document("BinXCAF");
  Handle(XCAFDoc_ShapeTool) aShTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());
  aShTool->AddShape(theShape);
  return Write(thePath, aDoc, theProgress);
}

//=======================================================================
// function : GetFormat
// purpose  :
//=======================================================================
TCollection_AsciiString RWGltf_Provider::GetFormat() const
{
  return TCollection_AsciiString("GLTF");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString RWGltf_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}
