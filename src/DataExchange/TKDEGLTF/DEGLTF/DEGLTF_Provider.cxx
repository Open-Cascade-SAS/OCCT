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

#include <DEGLTF_Provider.hxx>

#include <Message.hxx>
#include <RWGltf_CafWriter.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

namespace
{
//=================================================================================================

static void SetReaderParameters(RWGltf_CafReader&                       theReader,
                                const Handle(DEGLTF_ConfigurationNode)& theNode)
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
} // namespace

IMPLEMENT_STANDARD_RTTIEXT(DEGLTF_Provider, DE_Provider)

//=================================================================================================

DEGLTF_Provider::DEGLTF_Provider() {}

//=================================================================================================

DEGLTF_Provider::DEGLTF_Provider(const Handle(DE_ConfigurationNode)& theNode)
    : DE_Provider(theNode)
{
}

//=================================================================================================

bool DEGLTF_Provider::Read(const TCollection_AsciiString&  thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           Handle(XSControl_WorkSession)&  theWS,
                           const Message_ProgressRange&    theProgress)
{
  (void)theWS;
  return Read(thePath, theDocument, theProgress);
}

//=================================================================================================

bool DEGLTF_Provider::Write(const TCollection_AsciiString&  thePath,
                            const Handle(TDocStd_Document)& theDocument,
                            Handle(XSControl_WorkSession)&  theWS,
                            const Message_ProgressRange&    theProgress)
{
  (void)theWS;
  return Write(thePath, theDocument, theProgress);
}

//=================================================================================================

bool DEGLTF_Provider::Read(const TCollection_AsciiString&  thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           const Message_ProgressRange&    theProgress)
{
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the DEGLTF_Provider during reading the file " << thePath
                        << "\t: theDocument shouldn't be null";
    return false;
  }
  if (GetNode().IsNull()
      || (!GetNode().IsNull() && !GetNode()->IsKind(STANDARD_TYPE(DEGLTF_ConfigurationNode))))
  {
    Message::SendFail() << "Error in the DEGLTF_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DEGLTF_ConfigurationNode) aNode = Handle(DEGLTF_ConfigurationNode)::DownCast(GetNode());
  RWGltf_CafReader                 aReader;
  aReader.SetDocument(theDocument);
  SetReaderParameters(aReader, aNode);
  XCAFDoc_DocumentTool::SetLengthUnit(theDocument,
                                      aNode->GlobalParameters.LengthUnit,
                                      UnitsMethods_LengthUnit_Millimeter);
  if (!aReader.Perform(thePath, theProgress))
  {
    Message::SendFail() << "Error in the DEGLTF_Provider during reading the file " << thePath;
    return false;
  }

  return true;
}

//=================================================================================================

bool DEGLTF_Provider::Write(const TCollection_AsciiString&  thePath,
                            const Handle(TDocStd_Document)& theDocument,
                            const Message_ProgressRange&    theProgress)
{
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(DEGLTF_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEGLTF_Provider during writing the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DEGLTF_ConfigurationNode) aNode = Handle(DEGLTF_ConfigurationNode)::DownCast(GetNode());

  RWMesh_CoordinateSystemConverter aConverter;
  Standard_Real                    aScaleFactorM = 1.;
  if (!XCAFDoc_DocumentTool::GetLengthUnit(theDocument, aScaleFactorM))
  {
    aConverter.SetInputLengthUnit(aNode->GlobalParameters.SystemUnit / 1000.);
    Message::SendWarning()
      << "Warning in the DEGLTF_Provider during writing the file " << thePath
      << "\t: The document has no information on Units. Using global parameter as initial Unit.";
  }
  aConverter.SetInputCoordinateSystem(aNode->InternalParameters.SystemCS);
  if (aNode->GlobalParameters.LengthUnit != 1000.)
  {
    Message::SendWarning()
      << "Warning in the DEGLTF_Provider during writing the file " << thePath
      << "\t: Target format doesn't support custom units. Model will be scaled to Meters";
  }
  aConverter.SetOutputLengthUnit(1.); // gltf units always Meters
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
    Message::SendFail() << "Error in the DEGLTF_Provider during writing the file " << thePath;
    return false;
  }
  return true;
}

//=================================================================================================

bool DEGLTF_Provider::Read(const TCollection_AsciiString& thePath,
                           TopoDS_Shape&                  theShape,
                           Handle(XSControl_WorkSession)& theWS,
                           const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  return Read(thePath, theShape, theProgress);
}

//=================================================================================================

bool DEGLTF_Provider::Write(const TCollection_AsciiString& thePath,
                            const TopoDS_Shape&            theShape,
                            Handle(XSControl_WorkSession)& theWS,
                            const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  return Write(thePath, theShape, theProgress);
}

//=================================================================================================

bool DEGLTF_Provider::Read(const TCollection_AsciiString& thePath,
                           TopoDS_Shape&                  theShape,
                           const Message_ProgressRange&   theProgress)
{
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(DEGLTF_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEGLTF_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DEGLTF_ConfigurationNode) aNode = Handle(DEGLTF_ConfigurationNode)::DownCast(GetNode());
  RWGltf_CafReader                 aReader;
  SetReaderParameters(aReader, aNode);
  if (!aReader.Perform(thePath, theProgress))
  {
    Message::SendFail() << "Error in the DEGLTF_Provider during reading the file " << thePath;
    return false;
  }
  theShape = aReader.SingleShape();
  return true;
}

//=================================================================================================

bool DEGLTF_Provider::Write(const TCollection_AsciiString& thePath,
                            const TopoDS_Shape&            theShape,
                            const Message_ProgressRange&   theProgress)
{
  Handle(TDocStd_Document)  aDoc    = new TDocStd_Document("BinXCAF");
  Handle(XCAFDoc_ShapeTool) aShTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());
  aShTool->AddShape(theShape);
  return Write(thePath, aDoc, theProgress);
}

//=================================================================================================

TCollection_AsciiString DEGLTF_Provider::GetFormat() const
{
  return TCollection_AsciiString("GLTF");
}

//=================================================================================================

TCollection_AsciiString DEGLTF_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}
