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
#include <DEBREP_ConfigurationNode.hxx>
#include <Message.hxx>
#include <OSD_FileSystem.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DEBREP_Provider, DE_Provider)

namespace
{
  Standard_Boolean ValidateStreamMap(const DEBREP_Provider::ReadStreamMap& theStreams,
                                      TCollection_AsciiString& theFirstKey)
  {
    if (theStreams.IsEmpty())
    {
      Message::SendFail() << "Error: DEBREP_Provider stream map is empty";
      return Standard_False;
    }
    if (theStreams.Size() > 1)
    {
      Message::SendWarning() << "Warning: DEBREP_Provider received " << theStreams.Size()
                             << " streams, using only the first one";
    }
    theFirstKey = theStreams.FindKey(1);
    return Standard_True;
  }

  Standard_Boolean ValidateStreamMap(const DEBREP_Provider::WriteStreamMap& theStreams,
                                      TCollection_AsciiString& theFirstKey)
  {
    if (theStreams.IsEmpty())
    {
      Message::SendFail() << "Error: DEBREP_Provider stream map is empty";
      return Standard_False;
    }
    if (theStreams.Size() > 1)
    {
      Message::SendWarning() << "Warning: DEBREP_Provider received " << theStreams.Size()
                             << " streams, using only the first one";
    }
    theFirstKey = theStreams.FindKey(1);
    return Standard_True;
  }

  Standard_Boolean ValidateConfigurationNode(const Handle(DE_ConfigurationNode)& theNode,
                                              const TCollection_AsciiString& theContext,
                                              Handle(DEBREP_ConfigurationNode)& theDowncastNode)
  {
    if (theNode.IsNull() || !theNode->IsKind(STANDARD_TYPE(DEBREP_ConfigurationNode)))
    {
      Message::SendFail() << "Error: DEBREP_Provider configuring failed in " << theContext;
      return Standard_False;
    }
    theDowncastNode = Handle(DEBREP_ConfigurationNode)::DownCast(theNode);
    return Standard_True;
  }

  Standard_Boolean ValidateBinaryFormatVersion(const Handle(DEBREP_ConfigurationNode)& theNode,
                                                const TCollection_AsciiString& theContext)
  {
    if (theNode->InternalParameters.WriteVersionBin
          > static_cast<BinTools_FormatVersion>(BinTools_FormatVersion_UPPER)
        || theNode->InternalParameters.WriteVersionBin
             < static_cast<BinTools_FormatVersion>(BinTools_FormatVersion_LOWER))
    {
      Message::SendFail() << "Error in the DEBREP_Provider during " << theContext
                          << ": Unknown format version";
      return Standard_False;
    }
    if (theNode->InternalParameters.WriteNormals
        && theNode->InternalParameters.WriteVersionBin < BinTools_FormatVersion_VERSION_4)
    {
      Message::SendFail() << "Error in the DEBREP_Provider during " << theContext
                          << ": Vertex normals require binary format version 4 or later";
      return Standard_False;
    }
    return Standard_True;
  }

  Standard_Boolean ValidateAsciiFormatVersion(const Handle(DEBREP_ConfigurationNode)& theNode,
                                               const TCollection_AsciiString& theContext)
  {
    if (theNode->InternalParameters.WriteVersionAscii
          > static_cast<TopTools_FormatVersion>(TopTools_FormatVersion_UPPER)
        || theNode->InternalParameters.WriteVersionAscii
             < static_cast<TopTools_FormatVersion>(TopTools_FormatVersion_LOWER))
    {
      Message::SendFail() << "Error in the DEBREP_Provider during " << theContext
                          << ": Unknown format version";
      return Standard_False;
    }
    if (theNode->InternalParameters.WriteNormals
        && theNode->InternalParameters.WriteVersionAscii < TopTools_FormatVersion_VERSION_3)
    {
      Message::SendFail() << "Error in the DEBREP_Provider during " << theContext
                          << ": Error: vertex normals require ascii format version 3 or later";
      return Standard_False;
    }
    return Standard_True;
  }

  Standard_Boolean ExtractShapeFromDocument(const Handle(TDocStd_Document)& theDocument,
                                             const Handle(DEBREP_ConfigurationNode)& theNode,
                                             const TCollection_AsciiString& theContext,
                                             TopoDS_Shape& theShape)
  {
    TDF_LabelSequence         aLabels;
    Handle(XCAFDoc_ShapeTool) aSTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
    aSTool->GetFreeShapes(aLabels);
    
    if (aLabels.Length() <= 0)
    {
      Message::SendFail() << "Error in the DEBREP_Provider during " << theContext
                          << ": Document contain no shapes";
      return Standard_False;
    }

    if (theNode->GlobalParameters.LengthUnit != 1.0)
    {
      Message::SendWarning()
        << "Warning in the DEBREP_Provider during " << theContext
        << ": Target Units for writing were changed, but current format doesn't support scaling";
    }

    if (aLabels.Length() == 1)
    {
      theShape = aSTool->GetShape(aLabels.Value(1));
    }
    else
    {
      TopoDS_Compound aComp;
      BRep_Builder    aBuilder;
      aBuilder.MakeCompound(aComp);
      for (Standard_Integer anIndex = 1; anIndex <= aLabels.Length(); anIndex++)
      {
        TopoDS_Shape aS = aSTool->GetShape(aLabels.Value(anIndex));
        aBuilder.Add(aComp, aS);
      }
      theShape = aComp;
    }
    return Standard_True;
  }

  Standard_Boolean ReadShapeFromStream(Standard_IStream& theStream,
                                        const TCollection_AsciiString& theContext,
                                        TopoDS_Shape& theShape,
                                        const Message_ProgressRange& theProgress)
  {
    bool isBinaryFormat = true;
    std::streampos aOrigPos = theStream.tellg();
    
    char aStringBuf[255] = {};
    theStream.read(aStringBuf, 255);
    if (theStream.fail())
    {
      Message::SendFail() << "Error in the DEBREP_Provider during " << theContext
                          << ": Cannot read from the stream";
      return Standard_False;
    }
    isBinaryFormat = !(::strncmp(aStringBuf, "DBRep_DrawableShape", 19) == 0);
    
    theStream.seekg(aOrigPos);
    
    try 
    {
      if (isBinaryFormat)
      {
        BinTools::Read(theShape, theStream, theProgress);
      }
      else
      {
        BRepTools::Read(theShape, theStream, BRep_Builder(), theProgress);
      }
      
      if (theStream.fail() || theShape.IsNull())
      {
        Message::SendFail() << "Error in the DEBREP_Provider during " << theContext
                            << ": Cannot read from the stream";
        return Standard_False;
      }
    }
    catch (const Standard_Failure& anException)
    {
      Message::SendFail() << "Error in the DEBREP_Provider during " << theContext
                          << ": " << anException.GetMessageString();
      return Standard_False;
    }
    return Standard_True;
  }

  Standard_Boolean WriteShapeToStream(const Handle(DEBREP_ConfigurationNode)& theNode,
                                       const TopoDS_Shape& theShape,
                                       Standard_OStream& theStream,
                                       const TCollection_AsciiString& theContext,
                                       const Message_ProgressRange& theProgress)
  {
    try 
    {
      if (theNode->InternalParameters.WriteBinary)
      {
        if (theNode->InternalParameters.WriteNormals
            && theNode->InternalParameters.WriteVersionBin < BinTools_FormatVersion_VERSION_4)
        {
          Message::SendFail() << "Error in the DEBREP_Provider during " << theContext
                              << ": Vertex normals require binary format version 4 or later";
          return Standard_False;
        }

        BinTools::Write(theShape,
                        theStream,
                        theNode->InternalParameters.WriteTriangles,
                        theNode->InternalParameters.WriteNormals,
                        theNode->InternalParameters.WriteVersionBin,
                        theProgress);
      }
      else
      {
        BRepTools::Write(theShape,
                         theStream,
                         theNode->InternalParameters.WriteTriangles,
                         theNode->InternalParameters.WriteNormals,
                         theNode->InternalParameters.WriteVersionAscii,
                         theProgress);
      }
      
      if (theStream.fail())
      {
        Message::SendFail() << "Error in the DEBREP_Provider during " << theContext
                            << ": Cannot write to the stream";
        return Standard_False;
      }
    }
    catch (const Standard_Failure& anException)
    {
      Message::SendFail() << "Error in the DEBREP_Provider during " << theContext
                          << ": " << anException.GetMessageString();
      return Standard_False;
    }
    return Standard_True;
  }
}

//=================================================================================================

DEBREP_Provider::DEBREP_Provider() {}

//=================================================================================================

DEBREP_Provider::DEBREP_Provider(const Handle(DE_ConfigurationNode)& theNode)
    : DE_Provider(theNode)
{
}

//=================================================================================================

bool DEBREP_Provider::Read(const TCollection_AsciiString&  thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           Handle(XSControl_WorkSession)&  theWS,
                           const Message_ProgressRange&    theProgress)
{
  (void)theWS;
  return Read(thePath, theDocument, theProgress);
}

//=================================================================================================

bool DEBREP_Provider::Write(const TCollection_AsciiString&  thePath,
                            const Handle(TDocStd_Document)& theDocument,
                            Handle(XSControl_WorkSession)&  theWS,
                            const Message_ProgressRange&    theProgress)
{
  (void)theWS;
  return Write(thePath, theDocument, theProgress);
}

//=================================================================================================

bool DEBREP_Provider::Read(const TCollection_AsciiString&  thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           const Message_ProgressRange&    theProgress)
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
  Handle(XCAFDoc_ShapeTool) aShTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  aShTool->AddShape(aShape);
  return true;
}

//=================================================================================================

bool DEBREP_Provider::Write(const TCollection_AsciiString&  thePath,
                            const Handle(TDocStd_Document)& theDocument,
                            const Message_ProgressRange&    theProgress)
{
  Handle(DEBREP_ConfigurationNode) aNode = Handle(DEBREP_ConfigurationNode)::DownCast(GetNode());
  TopoDS_Shape aShape;
  if (!ExtractShapeFromDocument(theDocument, aNode, TCollection_AsciiString("writing the file ") + thePath, aShape))
  {
    return false;
  }
  return Write(thePath, aShape, theProgress);
}

//=================================================================================================

bool DEBREP_Provider::Read(const TCollection_AsciiString& thePath,
                           TopoDS_Shape&                  theShape,
                           Handle(XSControl_WorkSession)& theWS,
                           const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  return Read(thePath, theShape, theProgress);
}

//=================================================================================================

bool DEBREP_Provider::Write(const TCollection_AsciiString& thePath,
                            const TopoDS_Shape&            theShape,
                            Handle(XSControl_WorkSession)& theWS,
                            const Message_ProgressRange&   theProgress)
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
    const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
    std::shared_ptr<std::istream> aFile =
      aFileSystem->OpenIStream(thePath, std::ios::in | std::ios::binary);
    if (aFile.get() == NULL)
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
  Handle(DEBREP_ConfigurationNode) aNode = Handle(DEBREP_ConfigurationNode)::DownCast(GetNode());
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

TCollection_AsciiString DEBREP_Provider::GetFormat() const
{
  return TCollection_AsciiString("BREP");
}

//=================================================================================================

TCollection_AsciiString DEBREP_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=================================================================================================

Standard_Boolean DEBREP_Provider::Read(ReadStreamMap&            theStreams,
                                        const Handle(TDocStd_Document)& theDocument,
                                        Handle(XSControl_WorkSession)&  theWS,
                                        const Message_ProgressRange&    theProgress)
{
  (void)theDocument;
  (void)theWS;
  Message::SendFail() << "Error: DEBREP_Provider doesn't support document read operations with streams";
  return Standard_False;
}

//=================================================================================================

Standard_Boolean DEBREP_Provider::Write(WriteStreamMap&                 theStreams,
                                         const Handle(TDocStd_Document)& theDocument,
                                         Handle(XSControl_WorkSession)&  theWS,
                                         const Message_ProgressRange&    theProgress)
{
  (void)theStreams;
  (void)theDocument;
  (void)theWS;
  (void)theProgress;
  Message::SendFail() << "Error: DEBREP_Provider doesn't support document write operations with streams";
  return Standard_False;
}

//=================================================================================================

Standard_Boolean DEBREP_Provider::Read(ReadStreamMap&           theStreams,
                                        TopoDS_Shape&                  theShape,
                                        Handle(XSControl_WorkSession)& theWS,
                                        const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  TCollection_AsciiString aFirstKey;
  if (!ValidateStreamMap(theStreams, aFirstKey))
  {
    return Standard_False;
  }
  
  Handle(DEBREP_ConfigurationNode) aNode;
  if (!ValidateConfigurationNode(GetNode(), TCollection_AsciiString("reading stream ") + aFirstKey, aNode))
  {
    return Standard_False;
  }
  
  Standard_IStream& aStream = theStreams.ChangeFromIndex(1);
  return ReadShapeFromStream(aStream, TCollection_AsciiString("reading stream ") + aFirstKey, theShape, theProgress);
}

//=================================================================================================

Standard_Boolean DEBREP_Provider::Write(WriteStreamMap&                theStreams,
                                         const TopoDS_Shape&            theShape,
                                         Handle(XSControl_WorkSession)& theWS,
                                         const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  TCollection_AsciiString aFirstKey;
  if (!ValidateStreamMap(theStreams, aFirstKey))
  {
    return Standard_False;
  }
  
  Handle(DEBREP_ConfigurationNode) aNode;
  if (!ValidateConfigurationNode(GetNode(), TCollection_AsciiString("writing stream ") + aFirstKey, aNode))
  {
    return Standard_False;
  }
  
  Standard_OStream& aStream = theStreams.ChangeFromIndex(1);
  return WriteShapeToStream(aNode, theShape, aStream, TCollection_AsciiString("writing stream ") + aFirstKey, theProgress);
}

//=================================================================================================

Standard_Boolean DEBREP_Provider::Read(ReadStreamMap&            theStreams,
                                        const Handle(TDocStd_Document)& theDocument,
                                        const Message_ProgressRange&    theProgress)
{
  TCollection_AsciiString aFirstKey;
  if (!ValidateStreamMap(theStreams, aFirstKey))
  {
    return Standard_False;
  }
  
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the DEBREP_Provider during reading stream " << aFirstKey
                        << ": theDocument shouldn't be null";
    return Standard_False;
  }
  
  TopoDS_Shape aShape;
  Standard_IStream& aStream = theStreams.ChangeFromIndex(1);
  if (!ReadShapeFromStream(aStream, TCollection_AsciiString("reading stream ") + aFirstKey, aShape, theProgress))
  {
    return Standard_False;
  }
  
  Handle(XCAFDoc_ShapeTool) aShTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  aShTool->AddShape(aShape);
  return Standard_True;
}

//=================================================================================================

Standard_Boolean DEBREP_Provider::Write(WriteStreamMap&                 theStreams,
                                         const Handle(TDocStd_Document)& theDocument,
                                         const Message_ProgressRange&    theProgress)
{
  TCollection_AsciiString aFirstKey;
  if (!ValidateStreamMap(theStreams, aFirstKey))
  {
    return Standard_False;
  }
  
  Handle(DEBREP_ConfigurationNode) aNode;
  if (!ValidateConfigurationNode(GetNode(), TCollection_AsciiString("writing stream ") + aFirstKey, aNode))
  {
    return Standard_False;
  }
  
  TopoDS_Shape aShape;
  if (!ExtractShapeFromDocument(theDocument, aNode, TCollection_AsciiString("writing stream ") + aFirstKey, aShape))
  {
    return Standard_False;
  }
  
  Standard_OStream& aStream = theStreams.ChangeFromIndex(1);
  return WriteShapeToStream(aNode, aShape, aStream, TCollection_AsciiString("writing stream ") + aFirstKey, theProgress);
}

//=================================================================================================

Standard_Boolean DEBREP_Provider::Read(ReadStreamMap&           theStreams,
                                        TopoDS_Shape&                  theShape,
                                        const Message_ProgressRange&   theProgress)
{
  TCollection_AsciiString aFirstKey;
  if (!ValidateStreamMap(theStreams, aFirstKey))
  {
    return Standard_False;
  }
  
  Handle(DEBREP_ConfigurationNode) aNode;
  if (!ValidateConfigurationNode(GetNode(), TCollection_AsciiString("reading stream ") + aFirstKey, aNode))
  {
    return Standard_False;
  }
  
  Standard_IStream& aStream = theStreams.ChangeFromIndex(1);
  return ReadShapeFromStream(aStream, TCollection_AsciiString("reading stream ") + aFirstKey, theShape, theProgress);
}

//=================================================================================================

Standard_Boolean DEBREP_Provider::Write(WriteStreamMap&                theStreams,
                                         const TopoDS_Shape&            theShape,
                                         const Message_ProgressRange&   theProgress)
{
  TCollection_AsciiString aFirstKey;
  if (!ValidateStreamMap(theStreams, aFirstKey))
  {
    return Standard_False;
  }
  
  Handle(DEBREP_ConfigurationNode) aNode;
  if (!ValidateConfigurationNode(GetNode(), TCollection_AsciiString("writing stream ") + aFirstKey, aNode))
  {
    return Standard_False;
  }
  
  Standard_OStream& aStream = theStreams.ChangeFromIndex(1);
  return WriteShapeToStream(aNode, theShape, aStream, TCollection_AsciiString("writing stream ") + aFirstKey, theProgress);
}
