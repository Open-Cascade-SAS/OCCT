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

#include <DEBREP_Provider.hxx>

#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BinTools.hxx>
#include <DE_ValidationUtils.hxx>
#include <DEBREP_ConfigurationNode.hxx>
#include <Message.hxx>
#include <OSD_FileSystem.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

#include <cstring>
#include <istream>

IMPLEMENT_STANDARD_RTTIEXT(DEBREP_Provider, DE_Provider)

namespace
{
static occ::handle<DEBREP_ConfigurationNode> ValidateConfigurationNode(
  const occ::handle<DE_ConfigurationNode>& theNode,
  const TCollection_AsciiString&           theContext)
{
  if (!DE_ValidationUtils::ValidateConfigurationNode(theNode,
                                                     STANDARD_TYPE(DEBREP_ConfigurationNode),
                                                     theContext))
  {
    return occ::handle<DEBREP_ConfigurationNode>();
  }
  return occ::down_cast<DEBREP_ConfigurationNode>(theNode);
}

static bool IsAsciiBRepStream(Standard_IStream& theStream)
{
  // A stream passed through DE_Wrapper has already been probed for content.
  // Start the actual format probe from the beginning as the provider consumes
  // the complete stream during the following read.
  theStream.clear();
  theStream.seekg(0);
  if (theStream.fail())
  {
    return false;
  }

  char aHeader[19] = {};
  theStream.read(aHeader, sizeof(aHeader));
  const bool isAscii = theStream.gcount() == sizeof(aHeader)
                       && ((aHeader[0] == '\n' && aHeader[1] == 'C')
                           || ::strncmp(aHeader, "DBRep_DrawableShape", sizeof(aHeader)) == 0);
  theStream.clear();
  theStream.seekg(0);
  return isAscii;
}

static TopoDS_Shape ShapeFromDocument(const occ::handle<TDocStd_Document>& theDocument)
{
  if (theDocument.IsNull())
  {
    return TopoDS_Shape();
  }

  occ::handle<XCAFDoc_ShapeTool>  aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  NCollection_Sequence<TDF_Label> aLabels;
  aShapeTool->GetFreeShapes(aLabels);
  if (aLabels.IsEmpty())
  {
    return TopoDS_Shape();
  }

  if (aLabels.Length() == 1)
  {
    return aShapeTool->GetShape(aLabels.First());
  }

  TopoDS_Compound aCompound;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aCompound);
  for (int anIndex = 1; anIndex <= aLabels.Length(); ++anIndex)
  {
    aBuilder.Add(aCompound, aShapeTool->GetShape(aLabels.Value(anIndex)));
  }
  return aCompound;
}
} // namespace

//=================================================================================================

DEBREP_Provider::DEBREP_Provider() = default;

//=================================================================================================

DEBREP_Provider::DEBREP_Provider(const occ::handle<DE_ConfigurationNode>& theNode)
    : DE_Provider(theNode)
{
}

//=================================================================================================

bool DEBREP_Provider::Read(const TCollection_AsciiString&       thePath,
                           const occ::handle<TDocStd_Document>& theDocument,
                           occ::handle<XSControl_WorkSession>&  theWS,
                           const Message_ProgressRange&         theProgress)
{
  (void)theWS;
  return Read(thePath, theDocument, theProgress);
}

//=================================================================================================

bool DEBREP_Provider::Write(const TCollection_AsciiString&       thePath,
                            const occ::handle<TDocStd_Document>& theDocument,
                            occ::handle<XSControl_WorkSession>&  theWS,
                            const Message_ProgressRange&         theProgress)
{
  (void)theWS;
  return Write(thePath, theDocument, theProgress);
}

//=================================================================================================

bool DEBREP_Provider::Read(const TCollection_AsciiString&       thePath,
                           const occ::handle<TDocStd_Document>& theDocument,
                           const Message_ProgressRange&         theProgress)
{
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the DEBREP_Provider during reading the file " << thePath
                        << "\t: theDocument shouldn't be null";
    return false;
  }
  TopoDS_Shape aShape;
  if (!Read(thePath, aShape, theProgress))
  {
    return false;
  }
  occ::handle<XCAFDoc_ShapeTool> aShTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  aShTool->AddShape(aShape);
  return true;
}

//=================================================================================================

bool DEBREP_Provider::Write(const TCollection_AsciiString&       thePath,
                            const occ::handle<TDocStd_Document>& theDocument,
                            const Message_ProgressRange&         theProgress)
{
  TopoDS_Shape                    aShape;
  NCollection_Sequence<TDF_Label> aLabels;
  occ::handle<XCAFDoc_ShapeTool>  aSTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  aSTool->GetFreeShapes(aLabels);
  if (aLabels.Length() <= 0)
  {
    Message::SendFail() << "Error in the DEBREP_Provider during writing the file " << thePath
                        << "\t: Document contain no shapes";
    return false;
  }

  occ::handle<DEBREP_ConfigurationNode> aNode = occ::down_cast<DEBREP_ConfigurationNode>(GetNode());
  if (aNode->GlobalParameters.LengthUnit != 1.0)
  {
    Message::SendWarning()
      << "Warning in the DEBREP_Provider during writing the file " << thePath
      << "\t: Target Units for writing were changed, but current format doesn't support scaling";
  }

  if (aLabels.Length() == 1)
  {
    aShape = aSTool->GetShape(aLabels.Value(1));
  }
  else
  {
    TopoDS_Compound aComp;
    BRep_Builder    aBuilder;
    aBuilder.MakeCompound(aComp);
    for (int anIndex = 1; anIndex <= aLabels.Length(); anIndex++)
    {
      TopoDS_Shape aS = aSTool->GetShape(aLabels.Value(anIndex));
      aBuilder.Add(aComp, aS);
    }
    aShape = aComp;
  }
  return Write(thePath, aShape, theProgress);
}

//=================================================================================================

bool DEBREP_Provider::Read(const TCollection_AsciiString&      thePath,
                           TopoDS_Shape&                       theShape,
                           occ::handle<XSControl_WorkSession>& theWS,
                           const Message_ProgressRange&        theProgress)
{
  (void)theWS;
  return Read(thePath, theShape, theProgress);
}

//=================================================================================================

bool DEBREP_Provider::Write(const TCollection_AsciiString&      thePath,
                            const TopoDS_Shape&                 theShape,
                            occ::handle<XSControl_WorkSession>& theWS,
                            const Message_ProgressRange&        theProgress)
{
  (void)theWS;
  return Write(thePath, theShape, theProgress);
}

//=================================================================================================

bool DEBREP_Provider::Read(const TCollection_AsciiString& thePath,
                           TopoDS_Shape&                  theShape,
                           const Message_ProgressRange&   theProgress)
{
  bool isBinaryFormat = true;
  {
    // probe file header to recognize format
    const occ::handle<OSD_FileSystem>& aFileSystem = OSD_FileSystem::DefaultFileSystem();
    std::shared_ptr<std::istream>      aFile =
      aFileSystem->OpenIStream(thePath, std::ios::in | std::ios::binary);
    if (aFile.get() == nullptr)
    {
      Message::SendFail() << "Error in the DEBREP_Provider during reading the file " << thePath
                          << "\t: Cannot read the file";
      return false;
    }

    char aStringBuf[255] = {};
    aFile->read(aStringBuf, 255);
    if (aFile->fail())
    {
      Message::SendFail() << "Error in the DEBREP_Provider during reading the file " << thePath
                          << "\t: Cannot read the file";
      return false;
    }
    isBinaryFormat = !(::strncmp(aStringBuf, "DBRep_DrawableShape", 19) == 0);
  }

  if (isBinaryFormat)
  {
    if (!BinTools::Read(theShape, thePath.ToCString(), theProgress))
    {
      Message::SendFail() << "Error in the DEBREP_Provider during reading the file " << thePath
                          << "\t: Cannot read from the file";
      return false;
    }
  }
  else
  {
    if (!BRepTools::Read(theShape, thePath.ToCString(), BRep_Builder(), theProgress))
    {
      Message::SendFail() << "Error in the DEBREP_Provider during reading the file " << thePath
                          << "\t: Cannot read from the file";
      return false;
    }
  }

  return true;
}

//=================================================================================================

bool DEBREP_Provider::Write(const TCollection_AsciiString& thePath,
                            const TopoDS_Shape&            theShape,
                            const Message_ProgressRange&   theProgress)
{
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(DEBREP_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEBREP_Provider during writing the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  occ::handle<DEBREP_ConfigurationNode> aNode = occ::down_cast<DEBREP_ConfigurationNode>(GetNode());
  if (aNode->GlobalParameters.LengthUnit != 1.0)
  {
    Message::SendWarning()
      << "Warning in the DEBREP_Provider during writing the file " << thePath
      << "\t: Target Units for writing were changed, but current format doesn't support scaling";
  }
  if (aNode->InternalParameters.WriteBinary)
  {
    if (aNode->InternalParameters.WriteVersionBin
          > static_cast<BinTools_FormatVersion>(BinTools_FormatVersion_UPPER)
        || aNode->InternalParameters.WriteVersionBin
             < static_cast<BinTools_FormatVersion>(BinTools_FormatVersion_LOWER))
    {
      Message::SendFail() << "Error in the DEBREP_Provider during writing the file " << thePath
                          << "\t: Unknown format version";
      return false;
    }
    if (aNode->InternalParameters.WriteNormals
        && aNode->InternalParameters.WriteVersionBin < BinTools_FormatVersion_VERSION_4)
    {
      Message::SendFail() << "Error in the DEBREP_Provider during writing the file " << thePath
                          << "\t: Vertex normals require binary format version 4 or later";
      return false;
    }

    if (!BinTools::Write(theShape,
                         thePath.ToCString(),
                         aNode->InternalParameters.WriteTriangles,
                         aNode->InternalParameters.WriteNormals,
                         aNode->InternalParameters.WriteVersionBin,
                         theProgress))
    {
      Message::SendFail() << "Error in the DEBREP_Provider during writing the file " << thePath
                          << "\t: Cannot write the file";
      return false;
    }
  }
  else
  {
    if (aNode->InternalParameters.WriteVersionAscii
          > static_cast<TopTools_FormatVersion>(TopTools_FormatVersion_UPPER)
        || aNode->InternalParameters.WriteVersionAscii
             < static_cast<TopTools_FormatVersion>(TopTools_FormatVersion_LOWER))
    {
      Message::SendFail() << "Error in the DEBREP_Provider during writing the file " << thePath
                          << "\t: Unknown format version";
      return false;
    }
    if (aNode->InternalParameters.WriteNormals
        && aNode->InternalParameters.WriteVersionAscii < TopTools_FormatVersion_VERSION_3)
    {
      Message::SendFail() << "Error in the DEBREP_Provider during writing the file " << thePath
                          << "\t: Error: vertex normals require ascii format version 3 or later";
      return false;
    }
    if (!BRepTools::Write(theShape,
                          thePath.ToCString(),
                          aNode->InternalParameters.WriteTriangles,
                          aNode->InternalParameters.WriteNormals,
                          aNode->InternalParameters.WriteVersionAscii,
                          theProgress))
    {
      Message::SendFail() << "Error in the DEBREP_Provider during writing the file " << thePath
                          << "\t: Cannot write the file";
      return false;
    }
  }

  return true;
}

//=================================================================================================

bool DEBREP_Provider::Read(ReadStreamList&              theStreams,
                           TopoDS_Shape&                theShape,
                           const Message_ProgressRange& theProgress)
{
  const TCollection_AsciiString aContext = "reading stream";
  if (!DE_ValidationUtils::ValidateReadStreamList(theStreams, aContext))
  {
    return false;
  }

  const TCollection_AsciiString& aFirstKey    = theStreams.First().Path;
  const TCollection_AsciiString  aFullContext = aContext + " " + aFirstKey;
  if (ValidateConfigurationNode(GetNode(), aFullContext).IsNull())
  {
    return false;
  }

  Standard_IStream& aStream = theStreams.First().Stream;
  const bool        isAscii = IsAsciiBRepStream(aStream);
  if (aStream.fail())
  {
    Message::SendFail() << "Error in the DEBREP_Provider during " << aFullContext
                        << ": Cannot seek input stream";
    return false;
  }

  if (isAscii)
  {
    BRepTools::Read(theShape, aStream, BRep_Builder(), theProgress);
  }
  else
  {
    BinTools::Read(theShape, aStream, theProgress);
  }
  const bool isDone = !theShape.IsNull() && !aStream.fail();
  if (!isDone)
  {
    Message::SendFail() << "Error in the DEBREP_Provider during " << aFullContext
                        << ": Cannot read from the stream";
  }
  return isDone;
}

//=================================================================================================

bool DEBREP_Provider::Write(WriteStreamList&             theStreams,
                            const TopoDS_Shape&          theShape,
                            const Message_ProgressRange& theProgress)
{
  const TCollection_AsciiString aContext = "writing stream";
  if (!DE_ValidationUtils::ValidateWriteStreamList(theStreams, aContext))
  {
    return false;
  }

  const TCollection_AsciiString&        aFirstKey    = theStreams.First().Path;
  const TCollection_AsciiString         aFullContext = aContext + " " + aFirstKey;
  occ::handle<DEBREP_ConfigurationNode> aNode = ValidateConfigurationNode(GetNode(), aFullContext);
  if (aNode.IsNull())
  {
    return false;
  }

  Standard_OStream& aStream = theStreams.First().Stream;
  bool              isDone  = false;
  if (aNode->InternalParameters.WriteBinary)
  {
    if (aNode->InternalParameters.WriteVersionBin < BinTools_FormatVersion_LOWER
        || aNode->InternalParameters.WriteVersionBin > BinTools_FormatVersion_UPPER)
    {
      Message::SendFail() << "Error in the DEBREP_Provider during " << aFullContext
                          << ": Unknown binary format version";
      return false;
    }
    if (aNode->InternalParameters.WriteNormals
        && aNode->InternalParameters.WriteVersionBin < BinTools_FormatVersion_VERSION_4)
    {
      Message::SendFail() << "Error in the DEBREP_Provider during " << aFullContext
                          << ": Vertex normals require binary format version 4 or later";
      return false;
    }
    BinTools::Write(theShape,
                    aStream,
                    aNode->InternalParameters.WriteTriangles,
                    aNode->InternalParameters.WriteNormals,
                    aNode->InternalParameters.WriteVersionBin,
                    theProgress);
    isDone = !aStream.fail();
  }
  else
  {
    if (aNode->InternalParameters.WriteVersionAscii < TopTools_FormatVersion_LOWER
        || aNode->InternalParameters.WriteVersionAscii > TopTools_FormatVersion_UPPER)
    {
      Message::SendFail() << "Error in the DEBREP_Provider during " << aFullContext
                          << ": Unknown ASCII format version";
      return false;
    }
    if (aNode->InternalParameters.WriteNormals
        && aNode->InternalParameters.WriteVersionAscii < TopTools_FormatVersion_VERSION_3)
    {
      Message::SendFail() << "Error in the DEBREP_Provider during " << aFullContext
                          << ": Vertex normals require ascii format version 3 or later";
      return false;
    }
    BRepTools::Write(theShape,
                     aStream,
                     aNode->InternalParameters.WriteTriangles,
                     aNode->InternalParameters.WriteNormals,
                     aNode->InternalParameters.WriteVersionAscii,
                     theProgress);
    isDone = !aStream.fail();
  }
  if (!isDone)
  {
    Message::SendFail() << "Error in the DEBREP_Provider during " << aFullContext
                        << ": Cannot write to the stream";
  }
  return isDone;
}

//=================================================================================================

bool DEBREP_Provider::Read(ReadStreamList&                      theStreams,
                           const occ::handle<TDocStd_Document>& theDocument,
                           const Message_ProgressRange&         theProgress)
{
  const TCollection_AsciiString aContext = "reading stream";
  if (!DE_ValidationUtils::ValidateDocument(theDocument, aContext))
  {
    return false;
  }

  TopoDS_Shape aShape;
  if (!Read(theStreams, aShape, theProgress))
  {
    return false;
  }

  XCAFDoc_DocumentTool::ShapeTool(theDocument->Main())->AddShape(aShape);
  return true;
}

//=================================================================================================

bool DEBREP_Provider::Write(WriteStreamList&                     theStreams,
                            const occ::handle<TDocStd_Document>& theDocument,
                            const Message_ProgressRange&         theProgress)
{
  if (theDocument.IsNull())
  {
    return false;
  }

  const TopoDS_Shape aShape = ShapeFromDocument(theDocument);
  if (aShape.IsNull())
  {
    Message::SendFail() << "Error in the DEBREP_Provider during writing stream"
                        << ": Document contains no shapes";
    return false;
  }
  return Write(theStreams, aShape, theProgress);
}

//=================================================================================================

bool DEBREP_Provider::Read(ReadStreamList&                      theStreams,
                           const occ::handle<TDocStd_Document>& theDocument,
                           occ::handle<XSControl_WorkSession>&,
                           const Message_ProgressRange& theProgress)
{
  return Read(theStreams, theDocument, theProgress);
}

//=================================================================================================

bool DEBREP_Provider::Write(WriteStreamList&                     theStreams,
                            const occ::handle<TDocStd_Document>& theDocument,
                            occ::handle<XSControl_WorkSession>&,
                            const Message_ProgressRange& theProgress)
{
  return Write(theStreams, theDocument, theProgress);
}

//=================================================================================================

bool DEBREP_Provider::Read(ReadStreamList& theStreams,
                           TopoDS_Shape&   theShape,
                           occ::handle<XSControl_WorkSession>&,
                           const Message_ProgressRange& theProgress)
{
  return Read(theStreams, theShape, theProgress);
}

//=================================================================================================

bool DEBREP_Provider::Write(WriteStreamList&    theStreams,
                            const TopoDS_Shape& theShape,
                            occ::handle<XSControl_WorkSession>&,
                            const Message_ProgressRange& theProgress)
{
  return Write(theStreams, theShape, theProgress);
}

//=================================================================================================

TCollection_AsciiString DEBREP_Provider::GetFormat() const
{
  return TCollection_AsciiString("BREP");
}

//=================================================================================================

TCollection_AsciiString DEBREP_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}
