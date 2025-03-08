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

#include <DEVRML_ConfigurationNode.hxx>
#include <Message.hxx>
#include <OSD_Path.hxx>
#include <TDocStd_Document.hxx>
#include <VrmlAPI_CafReader.hxx>
#include <VrmlAPI_Writer.hxx>
#include <VrmlData_Scene.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DEVRML_Provider, DE_Provider)

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
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the DEVRML_Provider during reading the file " << thePath
                        << "\t: theDocument shouldn't be null";
    return false;
  }
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(DEVRML_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEVRML_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DEVRML_ConfigurationNode) aNode = Handle(DEVRML_ConfigurationNode)::DownCast(GetNode());

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
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(DEVRML_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEVRML_Provider during writing the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DEVRML_ConfigurationNode) aNode = Handle(DEVRML_ConfigurationNode)::DownCast(GetNode());

  VrmlAPI_Writer aWriter;
  aWriter.SetRepresentation(
    static_cast<VrmlAPI_RepresentationOfShape>(aNode->InternalParameters.WriteRepresentationType));
  Standard_Real aScaling       = 1.;
  Standard_Real aScaleFactorMM = 1.;
  if (XCAFDoc_DocumentTool::GetLengthUnit(theDocument,
                                          aScaleFactorMM,
                                          UnitsMethods_LengthUnit_Millimeter))
  {
    aScaling = aScaleFactorMM / aNode->GlobalParameters.LengthUnit;
  }
  else
  {
    aScaling = aNode->GlobalParameters.SystemUnit / aNode->GlobalParameters.LengthUnit;
    Message::SendWarning()
      << "Warning in the DEVRML_Provider during writing the file " << thePath
      << "\t: The document has no information on Units. Using global parameter as initial Unit.";
  }
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
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(DEVRML_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEVRML_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(DEVRML_ConfigurationNode) aNode = Handle(DEVRML_ConfigurationNode)::DownCast(GetNode());

  TopoDS_Shape                      aShape;
  VrmlData_DataMapOfShapeAppearance aShapeAppMap;

  std::filebuf aFic;
  std::istream aStream(&aFic);

  if (aFic.open(thePath.ToCString(), std::ios::in))
  {
    // Get path of the VRML file.
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

    VrmlData_Scene aScene;
    aScene.SetLinearScale(aNode->GlobalParameters.LengthUnit);

    aScene.SetVrmlDir(aVrmlDir);
    aScene << aStream;
    const char* aStr = 0L;
    switch (aScene.Status())
    {
      case VrmlData_StatusOK: {
        aShape = aScene.GetShape(aShapeAppMap);
        break;
      }
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
      Message::SendFail() << "Error in the DEVRML_Provider during reading the file " << thePath
                          << "\t: ++ VRML Error: " << aStr << " in line " << aScene.GetLineError();
      return false;
    }
    else
    {
      theShape = aShape;
    }
  }
  else
  {
    Message::SendFail() << "Error in the DEVRML_Provider during reading the file " << thePath
                        << "\t: cannot open file";
    return false;
  }
  return true;
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

TCollection_AsciiString DEVRML_Provider::GetFormat() const
{
  return TCollection_AsciiString("VRML");
}

//=================================================================================================

TCollection_AsciiString DEVRML_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}
