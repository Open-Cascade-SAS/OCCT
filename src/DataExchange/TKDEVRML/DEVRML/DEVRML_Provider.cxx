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

#include <DEVRML_Provider.hxx>

#include <DE_ValidationUtils.hxx>
#include <DEVRML_ConfigurationNode.hxx>
#include <Message.hxx>
#include <OSD_Path.hxx>
#include <TDocStd_Document.hxx>
#include <VrmlAPI_CafReader.hxx>
#include <VrmlAPI_Writer.hxx>
#include <VrmlData_Scene.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

#include <stdexcept>

IMPLEMENT_STANDARD_RTTIEXT(DEVRML_Provider, DE_Provider)

namespace
{
// Helper function to validate configuration node and downcast
static Handle(DEVRML_ConfigurationNode) ValidateConfigurationNode(
  const Handle(DE_ConfigurationNode)& theNode,
  const TCollection_AsciiString&      theContext)
{
  if (!DE_ValidationUtils::ValidateConfigurationNode(theNode,
                                                     STANDARD_TYPE(DEVRML_ConfigurationNode),
                                                     theContext))
  {
    return Handle(DEVRML_ConfigurationNode)();
  }
  return Handle(DEVRML_ConfigurationNode)::DownCast(theNode);
}

// Static function to handle VrmlData_Scene status errors
static Standard_Boolean HandleVrmlSceneStatus(const VrmlData_Scene&          theScene,
                                              const TCollection_AsciiString& theContext)
{
  const char* aStr = nullptr;
  switch (theScene.Status())
  {
    case VrmlData_StatusOK:
      return Standard_True;
    case VrmlData_EmptyData:
      aStr = "EmptyData";
      break;
    case VrmlData_UnrecoverableError:
      aStr = "UnrecoverableError";
      break;
    case VrmlData_GeneralError:
      aStr = "GeneralError";
      break;
    case VrmlData_EndOfFile:
      aStr = "EndOfFile";
      break;
    case VrmlData_NotVrmlFile:
      aStr = "NotVrmlFile";
      break;
    case VrmlData_CannotOpenFile:
      aStr = "CannotOpenFile";
      break;
    case VrmlData_VrmlFormatError:
      aStr = "VrmlFormatError";
      break;
    case VrmlData_NumericInputError:
      aStr = "NumericInputError";
      break;
    case VrmlData_IrrelevantNumber:
      aStr = "IrrelevantNumber";
      break;
    case VrmlData_BooleanInputError:
      aStr = "BooleanInputError";
      break;
    case VrmlData_StringInputError:
      aStr = "StringInputError";
      break;
    case VrmlData_NodeNameUnknown:
      aStr = "NodeNameUnknown";
      break;
    case VrmlData_NonPositiveSize:
      aStr = "NonPositiveSize";
      break;
    case VrmlData_ReadUnknownNode:
      aStr = "ReadUnknownNode";
      break;
    case VrmlData_NonSupportedFeature:
      aStr = "NonSupportedFeature";
      break;
    case VrmlData_OutputStreamUndefined:
      aStr = "OutputStreamUndefined";
      break;
    case VrmlData_NotImplemented:
      aStr = "NotImplemented";
      break;
    default:
      break;
  }

  if (aStr)
  {
    Message::SendFail() << "Error in the DEVRML_Provider during " << theContext
                        << ": ++ VRML Error: " << aStr << " in line " << theScene.GetLineError();
    return Standard_False;
  }
  return Standard_True;
}

// Static function to calculate scaling factor
static Standard_Real CalculateScalingFactor(const Handle(TDocStd_Document)&         theDocument,
                                            const Handle(DEVRML_ConfigurationNode)& theNode,
                                            const TCollection_AsciiString&          theContext)
{
  Standard_Real aScaling       = 1.;
  Standard_Real aScaleFactorMM = 1.;
  if (XCAFDoc_DocumentTool::GetLengthUnit(theDocument,
                                          aScaleFactorMM,
                                          UnitsMethods_LengthUnit_Millimeter))
  {
    aScaling = aScaleFactorMM / theNode->GlobalParameters.LengthUnit;
  }
  else
  {
    aScaling = theNode->GlobalParameters.SystemUnit / theNode->GlobalParameters.LengthUnit;
    Message::SendWarning()
      << "Warning in the DEVRML_Provider during " << theContext
      << ": The document has no information on Units. Using global parameter as initial Unit.";
  }
  return aScaling;
}

// Static function to extract VRML directory path from file path
static TCollection_AsciiString ExtractVrmlDirectory(const TCollection_AsciiString& thePath)
{
  OSD_Path                aPath(thePath.ToCString());
  TCollection_AsciiString aVrmlDir(".");
  TCollection_AsciiString aDisk = aPath.Disk();
  TCollection_AsciiString aTrek = aPath.Trek();
  if (!aTrek.IsEmpty())
  {
    if (!aDisk.IsEmpty())
    {
      aVrmlDir = aDisk;
    }
    else
    {
      aVrmlDir.Clear();
    }
    aTrek.ChangeAll('|', '/');
    aVrmlDir += aTrek;
  }
  return aVrmlDir;
}

// Static function to process VRML scene from stream and extract shape
static Standard_Boolean ProcessVrmlScene(Standard_IStream&                       theStream,
                                         const Handle(DEVRML_ConfigurationNode)& theNode,
                                         const TCollection_AsciiString&          theVrmlDir,
                                         TopoDS_Shape&                           theShape,
                                         const TCollection_AsciiString&          theContext)
{
  VrmlData_Scene aScene;
  aScene.SetLinearScale(theNode->GlobalParameters.LengthUnit);
  aScene.SetVrmlDir(theVrmlDir);

  aScene << theStream;

  if (!HandleVrmlSceneStatus(aScene, theContext))
  {
    return Standard_False;
  }

  if (aScene.Status() == VrmlData_StatusOK)
  {
    VrmlData_DataMapOfShapeAppearance aShapeAppMap;
    TopoDS_Shape                      aShape = aScene.GetShape(aShapeAppMap);
    theShape                                 = aShape;

    // Verify that a valid shape was extracted
    if (theShape.IsNull())
    {
      Message::SendFail() << "Error in the DEVRML_Provider during " << theContext
                          << ": VRML scene processed successfully but no geometry was extracted";
      return Standard_False;
    }
  }
  else
  {
    // Scene status was not OK but HandleVrmlSceneStatus didn't catch it
    Message::SendFail() << "Error in the DEVRML_Provider during " << theContext
                        << ": VRML scene status is not OK but no specific error was reported";
    return Standard_False;
  }

  return Standard_True;
}
} // namespace

//=================================================================================================

DEVRML_Provider::DEVRML_Provider() {}

//=================================================================================================

DEVRML_Provider::DEVRML_Provider(const Handle(DE_ConfigurationNode)& theNode)
    : DE_Provider(theNode)
{
}

//=================================================================================================

bool DEVRML_Provider::Read(const TCollection_AsciiString&  thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           Handle(XSControl_WorkSession)&  theWS,
                           const Message_ProgressRange&    theProgress)
{
  (void)theWS;
  return Read(thePath, theDocument, theProgress);
}

//=================================================================================================

bool DEVRML_Provider::Write(const TCollection_AsciiString&  thePath,
                            const Handle(TDocStd_Document)& theDocument,
                            Handle(XSControl_WorkSession)&  theWS,
                            const Message_ProgressRange&    theProgress)
{
  (void)theWS;
  return Write(thePath, theDocument, theProgress);
}

//=================================================================================================

bool DEVRML_Provider::Read(const TCollection_AsciiString&  thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           const Message_ProgressRange&    theProgress)
{
  TCollection_AsciiString aContext = TCollection_AsciiString("reading the file ") + thePath;
  if (!DE_ValidationUtils::ValidateDocument(theDocument, aContext))
  {
    return false;
  }
  Handle(DEVRML_ConfigurationNode) aNode = ValidateConfigurationNode(GetNode(), aContext);
  if (aNode.IsNull())
  {
    return false;
  }

  VrmlAPI_CafReader aVrmlReader;
  aVrmlReader.SetDocument(theDocument);
  aVrmlReader.SetFileLengthUnit(aNode->InternalParameters.ReadFileUnit);
  aVrmlReader.SetSystemLengthUnit(aNode->GlobalParameters.LengthUnit);
  aVrmlReader.SetFileCoordinateSystem(aNode->InternalParameters.ReadFileCoordinateSys);
  aVrmlReader.SetSystemCoordinateSystem(aNode->InternalParameters.ReadSystemCoordinateSys);
  aVrmlReader.SetFillIncompleteDocument(aNode->InternalParameters.ReadFillIncomplete);

  XCAFDoc_DocumentTool::SetLengthUnit(theDocument, aNode->InternalParameters.ReadFileUnit);

  if (!aVrmlReader.Perform(thePath, theProgress))
  {
    if (aVrmlReader.ExtraStatus() != RWMesh_CafReaderStatusEx_Partial)
    {
      Message::SendFail() << "Error in the DEVRML_Provider during reading the file '" << thePath
                          << "'";
      return false;
    }
    Message::SendWarning()
      << "Warning in the DEVRML_Provider during reading the file: file has been read paratially "
      << "(due to unexpected EOF, syntax error, memory limit) '" << thePath << "'";
  }
  return true;
}

//=================================================================================================

bool DEVRML_Provider::Write(const TCollection_AsciiString&  thePath,
                            const Handle(TDocStd_Document)& theDocument,
                            const Message_ProgressRange&    theProgress)
{
  (void)theProgress;
  TCollection_AsciiString aContext = "writing the file ";
  aContext += thePath;
  if (!DE_ValidationUtils::ValidateDocument(theDocument, aContext))
  {
    return false;
  }

  Handle(DEVRML_ConfigurationNode) aNode = ValidateConfigurationNode(GetNode(), aContext);
  if (aNode.IsNull())
  {
    return false;
  }

  VrmlAPI_Writer aWriter;
  aWriter.SetRepresentation(
    static_cast<VrmlAPI_RepresentationOfShape>(aNode->InternalParameters.WriteRepresentationType));

  Standard_Real aScaling = CalculateScalingFactor(theDocument, aNode, aContext);
  if (!aWriter.WriteDoc(theDocument, thePath.ToCString(), aScaling))
  {
    Message::SendFail() << "Error in the DEVRML_Provider during wtiting the file " << thePath
                        << "\t: File was not written";
    return false;
  }

  return true;
}

//=================================================================================================

bool DEVRML_Provider::Read(const TCollection_AsciiString& thePath,
                           TopoDS_Shape&                  theShape,
                           Handle(XSControl_WorkSession)& theWS,
                           const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  return Read(thePath, theShape, theProgress);
}

//=================================================================================================

bool DEVRML_Provider::Write(const TCollection_AsciiString& thePath,
                            const TopoDS_Shape&            theShape,
                            Handle(XSControl_WorkSession)& theWS,
                            const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  return Write(thePath, theShape, theProgress);
}

//=================================================================================================

bool DEVRML_Provider::Read(const TCollection_AsciiString& thePath,
                           TopoDS_Shape&                  theShape,
                           const Message_ProgressRange&   theProgress)
{
  (void)theProgress;
  TCollection_AsciiString aContext = "reading the file ";
  aContext += thePath;
  Handle(DEVRML_ConfigurationNode) aNode = ValidateConfigurationNode(GetNode(), aContext);
  if (aNode.IsNull())
  {
    return false;
  }

  std::filebuf aFic;
  std::istream aStream(&aFic);

  if (!aFic.open(thePath.ToCString(), std::ios::in))
  {
    Message::SendFail() << "Error in the DEVRML_Provider during reading the file " << thePath
                        << "\t: cannot open file";
    return false;
  }

  TCollection_AsciiString aVrmlDir = ExtractVrmlDirectory(thePath);
  return ProcessVrmlScene(aStream, aNode, aVrmlDir, theShape, aContext);
}

//=================================================================================================

bool DEVRML_Provider::Write(const TCollection_AsciiString& thePath,
                            const TopoDS_Shape&            theShape,
                            const Message_ProgressRange&   theProgress)
{
  Handle(TDocStd_Document)  aDoc    = new TDocStd_Document("BinXCAF");
  Handle(XCAFDoc_ShapeTool) aShTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());
  aShTool->AddShape(theShape);
  return Write(thePath, aDoc, theProgress);
}

//=================================================================================================

Standard_Boolean DEVRML_Provider::Read(ReadStreamList&                 theStreams,
                                       const Handle(TDocStd_Document)& theDocument,
                                       Handle(XSControl_WorkSession)&  theWS,
                                       const Message_ProgressRange&    theProgress)
{
  (void)theWS;
  return Read(theStreams, theDocument, theProgress);
}

//=================================================================================================

Standard_Boolean DEVRML_Provider::Write(WriteStreamList&                theStreams,
                                        const Handle(TDocStd_Document)& theDocument,
                                        Handle(XSControl_WorkSession)&  theWS,
                                        const Message_ProgressRange&    theProgress)
{
  (void)theWS;
  return Write(theStreams, theDocument, theProgress);
}

//=================================================================================================

Standard_Boolean DEVRML_Provider::Read(ReadStreamList&                theStreams,
                                       TopoDS_Shape&                  theShape,
                                       Handle(XSControl_WorkSession)& theWS,
                                       const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  return Read(theStreams, theShape, theProgress);
}

//=================================================================================================

Standard_Boolean DEVRML_Provider::Write(WriteStreamList&               theStreams,
                                        const TopoDS_Shape&            theShape,
                                        Handle(XSControl_WorkSession)& theWS,
                                        const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  return Write(theStreams, theShape, theProgress);
}

//=================================================================================================

Standard_Boolean DEVRML_Provider::Read(ReadStreamList&                 theStreams,
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

Standard_Boolean DEVRML_Provider::Write(WriteStreamList&                theStreams,
                                        const Handle(TDocStd_Document)& theDocument,
                                        const Message_ProgressRange&    theProgress)
{
  (void)theProgress;
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

  Handle(DEVRML_ConfigurationNode) aNode = ValidateConfigurationNode(GetNode(), aFullContext);
  if (aNode.IsNull())
  {
    return Standard_False;
  }

  Standard_Real aScaling = CalculateScalingFactor(theDocument, aNode, aContext);

  // Use VrmlAPI_Writer with stream support
  VrmlAPI_Writer aWriter;
  aWriter.SetRepresentation(
    static_cast<VrmlAPI_RepresentationOfShape>(aNode->InternalParameters.WriteRepresentationType));

  Standard_OStream& aStream = theStreams.First().Stream;

  if (!aWriter.WriteDoc(theDocument, aStream, aScaling))
  {
    Message::SendFail() << "Error in the DEVRML_Provider during " << aContext
                        << ": WriteDoc operation failed";
    return Standard_False;
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean DEVRML_Provider::Read(ReadStreamList&              theStreams,
                                       TopoDS_Shape&                theShape,
                                       const Message_ProgressRange& theProgress)
{
  (void)theProgress;
  TCollection_AsciiString aContext = "reading stream";
  if (!DE_ValidationUtils::ValidateReadStreamList(theStreams, aContext))
  {
    return Standard_False;
  }

  const TCollection_AsciiString& aFirstKey = theStreams.First().Path;
  Standard_IStream&              aStream   = theStreams.First().Stream;

  TCollection_AsciiString          aFullContext = aContext + " " + aFirstKey;
  Handle(DEVRML_ConfigurationNode) aNode = ValidateConfigurationNode(GetNode(), aFullContext);
  if (aNode.IsNull())
  {
    return Standard_False;
  }

  return ProcessVrmlScene(aStream, aNode, ".", theShape, aContext);
}

//=================================================================================================

Standard_Boolean DEVRML_Provider::Write(WriteStreamList&             theStreams,
                                        const TopoDS_Shape&          theShape,
                                        const Message_ProgressRange& theProgress)
{
  (void)theProgress;
  TCollection_AsciiString aContext = "writing stream";
  if (!DE_ValidationUtils::ValidateWriteStreamList(theStreams, aContext))
  {
    return Standard_False;
  }

  const TCollection_AsciiString&   aFirstKey    = theStreams.First().Path;
  TCollection_AsciiString          aFullContext = aContext + " " + aFirstKey;
  Handle(DEVRML_ConfigurationNode) aNode = ValidateConfigurationNode(GetNode(), aFullContext);
  if (aNode.IsNull())
  {
    return Standard_False;
  }

  // Use VrmlAPI_Writer with stream support
  VrmlAPI_Writer aWriter;
  aWriter.SetRepresentation(
    static_cast<VrmlAPI_RepresentationOfShape>(aNode->InternalParameters.WriteRepresentationType));

  Standard_OStream& aStream = theStreams.First().Stream;

  if (!aWriter.Write(theShape, aStream, 2)) // Use version 2 by default
  {
    Message::SendFail() << "Error in the DEVRML_Provider during " << aContext
                        << ": Write operation failed";
    return Standard_False;
  }

  return Standard_True;
}

//=================================================================================================

TCollection_AsciiString DEVRML_Provider::GetFormat() const
{
  return TCollection_AsciiString("VRML");
}

//=================================================================================================

TCollection_AsciiString DEVRML_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}
