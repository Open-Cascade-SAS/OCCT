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

#include <DESTL_Provider.hxx>

#include <BRep_Builder.hxx>
#include <DE_ValidationUtils.hxx>
#include <DESTL_ConfigurationNode.hxx>
#include <Message.hxx>
#include <NCollection_Vector.hxx>
#include <Poly_Triangle.hxx>
#include <RWStl.hxx>
#include <RWStl_Reader.hxx>
#include <StlAPI.hxx>
#include <StlAPI_Reader.hxx>
#include <StlAPI_Writer.hxx>
#include <Standard_ReadLineBuffer.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <cstring>

IMPLEMENT_STANDARD_RTTIEXT(DESTL_Provider, DE_Provider)

//=================================================================================================

DESTL_Provider::DESTL_Provider() {}

//=================================================================================================

DESTL_Provider::DESTL_Provider(const Handle(DE_ConfigurationNode)& theNode)
    : DE_Provider(theNode)
{
}

//=================================================================================================

bool DESTL_Provider::Read(const TCollection_AsciiString&  thePath,
                          const Handle(TDocStd_Document)& theDocument,
                          Handle(XSControl_WorkSession)&  theWS,
                          const Message_ProgressRange&    theProgress)
{
  (void)theWS;
  return Read(thePath, theDocument, theProgress);
}

//=================================================================================================

bool DESTL_Provider::Write(const TCollection_AsciiString&  thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           Handle(XSControl_WorkSession)&  theWS,
                           const Message_ProgressRange&    theProgress)
{
  (void)theWS;
  return Write(thePath, theDocument, theProgress);
}

//=================================================================================================

bool DESTL_Provider::Read(const TCollection_AsciiString&  thePath,
                          const Handle(TDocStd_Document)& theDocument,
                          const Message_ProgressRange&    theProgress)
{
  TCollection_AsciiString aContext = TCollection_AsciiString("reading the file ") + thePath;
  if (!DE_ValidationUtils::ValidateDocument(theDocument, aContext))
  {
    return false;
  }
  TopoDS_Shape aShape;
  if (!Read(thePath, aShape, theProgress))
  {
    return false;
  }
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  aShapeTool->AddShape(aShape);
  return true;
}

//=================================================================================================

bool DESTL_Provider::Write(const TCollection_AsciiString&  thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           const Message_ProgressRange&    theProgress)
{
  TCollection_AsciiString aContext = TCollection_AsciiString("writing the file ") + thePath;
  if (!DE_ValidationUtils::ValidateDocument(theDocument, aContext))
  {
    return false;
  }

  // Extract shape from document
  TDF_LabelSequence         aLabels;
  Handle(XCAFDoc_ShapeTool) aSTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  aSTool->GetFreeShapes(aLabels);

  if (aLabels.Length() <= 0)
  {
    Message::SendFail() << "Error in the DESTL_Provider during writing the file " << thePath
                        << ": Document contain no shapes";
    return false;
  }

  if (!DE_ValidationUtils::ValidateConfigurationNode(GetNode(),
                                                     STANDARD_TYPE(DESTL_ConfigurationNode),
                                                     aContext))
  {
    return false;
  }

  Handle(DESTL_ConfigurationNode) aNode = Handle(DESTL_ConfigurationNode)::DownCast(GetNode());
  DE_ValidationUtils::WarnLengthUnitNotSupported(aNode->GlobalParameters.LengthUnit, aContext);

  TopoDS_Shape aShape;
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

bool DESTL_Provider::Read(const TCollection_AsciiString& thePath,
                          TopoDS_Shape&                  theShape,
                          Handle(XSControl_WorkSession)& theWS,
                          const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  return Read(thePath, theShape, theProgress);
}

//=================================================================================================

bool DESTL_Provider::Write(const TCollection_AsciiString& thePath,
                           const TopoDS_Shape&            theShape,
                           Handle(XSControl_WorkSession)& theWS,
                           const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  return Write(thePath, theShape, theProgress);
}

//=================================================================================================

bool DESTL_Provider::Read(const TCollection_AsciiString& thePath,
                          TopoDS_Shape&                  theShape,
                          const Message_ProgressRange&   theProgress)
{
  Message::SendWarning()
    << "OCCT Stl reader does not support model scaling according to custom length unit";

  TCollection_AsciiString aContext = TCollection_AsciiString("reading the file ") + thePath;
  if (!DE_ValidationUtils::ValidateConfigurationNode(GetNode(),
                                                     STANDARD_TYPE(DESTL_ConfigurationNode),
                                                     aContext))
  {
    return false;
  }

  Handle(DESTL_ConfigurationNode) aNode = Handle(DESTL_ConfigurationNode)::DownCast(GetNode());
  double aMergeAngle                    = aNode->InternalParameters.ReadMergeAngle * M_PI / 180.0;

  if (aMergeAngle != M_PI_2)
  {
    if (aMergeAngle < 0.0 || aMergeAngle > M_PI_2)
    {
      Message::SendFail() << "Error in the DESTL_Provider during reading the file " << thePath
                          << "\t: The merge angle is out of the valid range";
      return false;
    }
  }

  if (!aNode->InternalParameters.ReadBRep)
  {
    Handle(Poly_Triangulation) aTriangulation =
      RWStl::ReadFile(thePath.ToCString(), aMergeAngle, theProgress);

    TopoDS_Face  aFace;
    BRep_Builder aB;
    aB.MakeFace(aFace);
    aB.UpdateFace(aFace, aTriangulation);
    theShape = aFace;
  }
  else
  {
    Standard_DISABLE_DEPRECATION_WARNINGS

      if (!StlAPI::Read(theShape, thePath.ToCString()))
    {
      Message::SendFail() << "Error in the DESTL_Provider during reading the file " << thePath;
      return false;
    }
    Standard_ENABLE_DEPRECATION_WARNINGS
  }
  return true;
}

//=================================================================================================

bool DESTL_Provider::Write(const TCollection_AsciiString& thePath,
                           const TopoDS_Shape&            theShape,
                           const Message_ProgressRange&   theProgress)
{
  Message::SendWarning()
    << "OCCT Stl writer does not support model scaling according to custom length unit";

  TCollection_AsciiString aContext = TCollection_AsciiString("writing the file ") + thePath;
  if (!DE_ValidationUtils::ValidateConfigurationNode(GetNode(),
                                                     STANDARD_TYPE(DESTL_ConfigurationNode),
                                                     aContext))
  {
    return false;
  }

  Handle(DESTL_ConfigurationNode) aNode = Handle(DESTL_ConfigurationNode)::DownCast(GetNode());
  DE_ValidationUtils::WarnLengthUnitNotSupported(aNode->GlobalParameters.LengthUnit, aContext);

  StlAPI_Writer aWriter;
  aWriter.ASCIIMode() = aNode->InternalParameters.WriteAscii;
  if (!aWriter.Write(theShape, thePath.ToCString(), theProgress))
  {
    Message::SendFail() << "Error in the DESTL_Provider during writing the file " << thePath
                        << "\t: Mesh writing has been failed";
    return false;
  }
  return true;
}

//=================================================================================================

Standard_Boolean DESTL_Provider::Read(ReadStreamList&                 theStreams,
                                      const Handle(TDocStd_Document)& theDocument,
                                      Handle(XSControl_WorkSession)&  theWS,
                                      const Message_ProgressRange&    theProgress)
{
  (void)theWS;
  return Read(theStreams, theDocument, theProgress);
}

//=================================================================================================

Standard_Boolean DESTL_Provider::Write(WriteStreamList&                theStreams,
                                       const Handle(TDocStd_Document)& theDocument,
                                       Handle(XSControl_WorkSession)&  theWS,
                                       const Message_ProgressRange&    theProgress)
{
  (void)theWS;
  return Write(theStreams, theDocument, theProgress);
}

//=================================================================================================

Standard_Boolean DESTL_Provider::Read(ReadStreamList&                theStreams,
                                      TopoDS_Shape&                  theShape,
                                      Handle(XSControl_WorkSession)& theWS,
                                      const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  return Read(theStreams, theShape, theProgress);
}

//=================================================================================================

Standard_Boolean DESTL_Provider::Write(WriteStreamList&               theStreams,
                                       const TopoDS_Shape&            theShape,
                                       Handle(XSControl_WorkSession)& theWS,
                                       const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  return Write(theStreams, theShape, theProgress);
}

//=================================================================================================

Standard_Boolean DESTL_Provider::Read(ReadStreamList&                 theStreams,
                                      const Handle(TDocStd_Document)& theDocument,
                                      const Message_ProgressRange&    theProgress)
{
  TCollection_AsciiString aContext = "reading stream";
  if (!DE_ValidationUtils::ValidateReadStreamList(theStreams, aContext))
  {
    return Standard_False;
  }

  const TCollection_AsciiString& aFirstKey    = theStreams.First().Path;
  TCollection_AsciiString        aFullContext = aContext + " " + aFirstKey;
  if (!DE_ValidationUtils::ValidateDocument(theDocument, aFullContext))
  {
    return Standard_False;
  }

  TopoDS_Shape aShape;
  if (!Read(theStreams, aShape, theProgress))
  {
    return Standard_False;
  }

  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  aShapeTool->AddShape(aShape);
  return Standard_True;
}

//=================================================================================================

Standard_Boolean DESTL_Provider::Write(WriteStreamList&                theStreams,
                                       const Handle(TDocStd_Document)& theDocument,
                                       const Message_ProgressRange&    theProgress)
{
  TCollection_AsciiString aContext = "writing stream";
  if (!DE_ValidationUtils::ValidateWriteStreamList(theStreams, aContext))
  {
    return Standard_False;
  }

  const TCollection_AsciiString& aFirstKey    = theStreams.First().Path;
  TCollection_AsciiString        aFullContext = aContext + " " + aFirstKey;
  if (!DE_ValidationUtils::ValidateDocument(theDocument, aFullContext))
  {
    return Standard_False;
  }

  // Extract shape from document
  TDF_LabelSequence         aLabels;
  Handle(XCAFDoc_ShapeTool) aSTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  aSTool->GetFreeShapes(aLabels);

  if (aLabels.Length() <= 0)
  {
    Message::SendFail() << "Error in the DESTL_Provider during writing stream " << aFirstKey
                        << ": Document contain no shapes";
    return Standard_False;
  }

  if (!DE_ValidationUtils::ValidateConfigurationNode(GetNode(),
                                                     STANDARD_TYPE(DESTL_ConfigurationNode),
                                                     aFullContext))
  {
    return Standard_False;
  }

  Handle(DESTL_ConfigurationNode) aNode  = Handle(DESTL_ConfigurationNode)::DownCast(GetNode());
  TCollection_AsciiString aLengthContext = TCollection_AsciiString("writing stream ") + aFirstKey;
  DE_ValidationUtils::WarnLengthUnitNotSupported(aNode->GlobalParameters.LengthUnit,
                                                 aLengthContext);

  TopoDS_Shape aShape;
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

  return Write(theStreams, aShape, theProgress);
}

//=================================================================================================

Standard_Boolean DESTL_Provider::Read(ReadStreamList&              theStreams,
                                      TopoDS_Shape&                theShape,
                                      const Message_ProgressRange& theProgress)
{
  // Validate stream map
  if (theStreams.IsEmpty())
  {
    Message::SendFail() << "Error: DESTL_Provider stream map is empty";
    return Standard_False;
  }
  if (theStreams.Size() > 1)
  {
    Message::SendWarning() << "Warning: DESTL_Provider received " << theStreams.Size()
                           << " streams for reading, using only the first one";
  }

  const TCollection_AsciiString& aFirstKey = theStreams.First().Path;
  Standard_IStream&              aStream   = theStreams.First().Stream;

  Message::SendWarning()
    << "OCCT Stl reader does not support model scaling according to custom length unit";

  TCollection_AsciiString aNodeContext = TCollection_AsciiString("reading stream ") + aFirstKey;
  if (!DE_ValidationUtils::ValidateConfigurationNode(GetNode(),
                                                     STANDARD_TYPE(DESTL_ConfigurationNode),
                                                     aNodeContext))
  {
    return Standard_False;
  }

  Handle(DESTL_ConfigurationNode) aNode = Handle(DESTL_ConfigurationNode)::DownCast(GetNode());
  double aMergeAngle                    = aNode->InternalParameters.ReadMergeAngle * M_PI / 180.0;

  if (aMergeAngle != M_PI_2)
  {
    if (aMergeAngle < 0.0 || aMergeAngle > M_PI_2)
    {
      Message::SendFail() << "Error in the DESTL_Provider during reading stream " << aFirstKey
                          << ": The merge angle is out of the valid range";
      return Standard_False;
    }
  }

  if (!aNode->InternalParameters.ReadBRep)
  {
    Handle(Poly_Triangulation) aTriangulation =
      RWStl::ReadStream(aStream, aMergeAngle, theProgress);
    if (aTriangulation.IsNull())
    {
      Message::SendFail() << "Error in the DESTL_Provider during reading stream " << aFirstKey
                          << ": Failed to create triangulation";
      return Standard_False;
    }

    TopoDS_Face  aFace;
    BRep_Builder aB;
    aB.MakeFace(aFace);
    aB.UpdateFace(aFace, aTriangulation);
    theShape = aFace;
  }
  else
  {
    Standard_DISABLE_DEPRECATION_WARNINGS

      StlAPI_Reader aReader;
    if (!aReader.Read(theShape, aStream))
    {
      Message::SendFail() << "Error in the DESTL_Provider during reading stream " << aFirstKey;
      return Standard_False;
    }
    Standard_ENABLE_DEPRECATION_WARNINGS
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean DESTL_Provider::Write(WriteStreamList&             theStreams,
                                       const TopoDS_Shape&          theShape,
                                       const Message_ProgressRange& theProgress)
{
  // Validate stream map
  if (theStreams.IsEmpty())
  {
    Message::SendFail() << "Error: DESTL_Provider stream map is empty";
    return Standard_False;
  }
  if (theStreams.Size() > 1)
  {
    Message::SendWarning() << "Warning: DESTL_Provider received " << theStreams.Size()
                           << " streams for writing, using only the first one";
  }

  const TCollection_AsciiString& aFirstKey = theStreams.First().Path;
  Standard_OStream&              aStream   = theStreams.First().Stream;

  Message::SendWarning()
    << "OCCT Stl writer does not support model scaling according to custom length unit";

  TCollection_AsciiString aNodeContext = TCollection_AsciiString("writing stream ") + aFirstKey;
  if (!DE_ValidationUtils::ValidateConfigurationNode(GetNode(),
                                                     STANDARD_TYPE(DESTL_ConfigurationNode),
                                                     aNodeContext))
  {
    return Standard_False;
  }

  Handle(DESTL_ConfigurationNode) aNode  = Handle(DESTL_ConfigurationNode)::DownCast(GetNode());
  TCollection_AsciiString aLengthContext = TCollection_AsciiString("writing stream ") + aFirstKey;
  DE_ValidationUtils::WarnLengthUnitNotSupported(aNode->GlobalParameters.LengthUnit,
                                                 aLengthContext);

  StlAPI_Writer aWriter;
  aWriter.ASCIIMode() = aNode->InternalParameters.WriteAscii;
  if (!aWriter.Write(theShape, aStream, theProgress))
  {
    Message::SendFail() << "Error in the DESTL_Provider during writing stream " << aFirstKey
                        << ": Mesh writing has been failed";
    return Standard_False;
  }

  return Standard_True;
}

//=================================================================================================

TCollection_AsciiString DESTL_Provider::GetFormat() const
{
  return TCollection_AsciiString("STL");
}

//=================================================================================================

TCollection_AsciiString DESTL_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}
