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
  TopoDS_Shape              aShape;
  TDF_LabelSequence         aLabels;
  Handle(XCAFDoc_ShapeTool) aSTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  aSTool->GetFreeShapes(aLabels);
  if (aLabels.Length() <= 0)
  {
    Message::SendFail() << "Error in the DEBREP_Provider during writing the file " << thePath
                        << "\t: Document contain no shapes";
    return false;
  }

  Handle(DEBREP_ConfigurationNode) aNode = Handle(DEBREP_ConfigurationNode)::DownCast(GetNode());
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
    for (Standard_Integer anIndex = 1; anIndex <= aLabels.Length(); anIndex++)
    {
      TopoDS_Shape aS = aSTool->GetShape(aLabels.Value(anIndex));
      aBuilder.Add(aComp, aS);
    }
    aShape = aComp;
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
