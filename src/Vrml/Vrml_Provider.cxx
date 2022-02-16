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

#include <Vrml_Provider.hxx>

#include <BinXCAFDrivers.hxx>
#include <Message.hxx>
#include <OSD_Path.hxx>
#include <TDocStd_Document.hxx>
#include <VrmlAPI_Writer.hxx>
#include <VrmlData_Scene.hxx>
#include <Vrml_ConfigurationNode.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Vrml_Provider, DE_Provider)

//=======================================================================
// function : Vrml_Provider
// purpose  :
//=======================================================================
Vrml_Provider::Vrml_Provider()
{}

//=======================================================================
// function : Vrml_Provider
// purpose  :
//=======================================================================
Vrml_Provider::Vrml_Provider(const Handle(DE_ConfigurationNode)& theNode)
  :DE_Provider(theNode)
{}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool Vrml_Provider::Read(const TCollection_AsciiString& thePath,
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
bool Vrml_Provider::Write(const TCollection_AsciiString& thePath,
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
bool Vrml_Provider::Read(const TCollection_AsciiString& thePath,
                         const Handle(TDocStd_Document)& theDocument,
                         const Message_ProgressRange& theProgress)
{
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the Vrml_Provider during reading the file " <<
      thePath << "\t: theDocument shouldn't be null";
    return false;
  }
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(Vrml_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the Vrml_Provider during reading the file " <<
      thePath << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(Vrml_ConfigurationNode) aNode = Handle(Vrml_ConfigurationNode)::DownCast(GetNode());
  TopoDS_Shape aShape;
  if(!Read(thePath, aShape, theProgress))
  {
    return false;
  }
  Handle(XCAFDoc_ShapeTool) aShTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  aShTool->AddShape(aShape);
  XCAFDoc_DocumentTool::SetLengthUnit(theDocument, aNode->GlobalParameters.LengthUnit, UnitsMethods_LengthUnit_Millimeter);
  return true;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool Vrml_Provider::Write(const TCollection_AsciiString& thePath,
                          const Handle(TDocStd_Document)& theDocument,
                          const Message_ProgressRange& theProgress)
{
  (void)theProgress;
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(Vrml_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the Vrml_Provider during writing the file " <<
      thePath << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(Vrml_ConfigurationNode) aNode = Handle(Vrml_ConfigurationNode)::DownCast(GetNode());

  VrmlAPI_Writer aWriter;
  aWriter.SetRepresentation(static_cast<VrmlAPI_RepresentationOfShape>(aNode->InternalParameters.WriteRepresentationType));
  Standard_Real aScaleFactorM = aNode->GlobalParameters.LengthUnit / 1000;
  if (!aWriter.WriteDoc(theDocument, thePath.ToCString(), aScaleFactorM))
  {
    Message::SendFail() << "Error in the Vrml_Provider during wtiting the file " <<
      thePath << "\t: File was not written";
    return false;
  }

  return true;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool Vrml_Provider::Read(const TCollection_AsciiString& thePath,
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
bool Vrml_Provider::Write(const TCollection_AsciiString& thePath,
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
bool Vrml_Provider::Read(const TCollection_AsciiString& thePath,
                         TopoDS_Shape& theShape,
                         const Message_ProgressRange& theProgress)
{
  (void)theProgress;
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(Vrml_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the Vrml_Provider during reading the file " <<
      thePath << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(Vrml_ConfigurationNode) aNode = Handle(Vrml_ConfigurationNode)::DownCast(GetNode());

  TopoDS_Shape aShape;
  VrmlData_DataMapOfShapeAppearance aShapeAppMap;

  std::filebuf aFic;
  std::istream aStream(&aFic);

  if (aFic.open(thePath.ToCString(), std::ios::in))
  {
    // Get path of the VRML file.
    OSD_Path aPath(thePath.ToCString());
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
    Standard_Real anOCCUnitMM = aNode->GlobalParameters.LengthUnit;
    aScene.SetLinearScale(1000. / anOCCUnitMM);

    aScene.SetVrmlDir(aVrmlDir);
    aScene << aStream;
    const char* aStr = 0L;
    switch (aScene.Status())
    {
      case VrmlData_StatusOK:
      {
        aShape = aScene.GetShape(aShapeAppMap);
        break;
      }
      case VrmlData_EmptyData:            aStr = "EmptyData"; break;
      case VrmlData_UnrecoverableError:   aStr = "UnrecoverableError"; break;
      case VrmlData_GeneralError:         aStr = "GeneralError"; break;
      case VrmlData_EndOfFile:            aStr = "EndOfFile"; break;
      case VrmlData_NotVrmlFile:          aStr = "NotVrmlFile"; break;
      case VrmlData_CannotOpenFile:       aStr = "CannotOpenFile"; break;
      case VrmlData_VrmlFormatError:      aStr = "VrmlFormatError"; break;
      case VrmlData_NumericInputError:    aStr = "NumericInputError"; break;
      case VrmlData_IrrelevantNumber:     aStr = "IrrelevantNumber"; break;
      case VrmlData_BooleanInputError:    aStr = "BooleanInputError"; break;
      case VrmlData_StringInputError:     aStr = "StringInputError"; break;
      case VrmlData_NodeNameUnknown:      aStr = "NodeNameUnknown"; break;
      case VrmlData_NonPositiveSize:      aStr = "NonPositiveSize"; break;
      case VrmlData_ReadUnknownNode:      aStr = "ReadUnknownNode"; break;
      case VrmlData_NonSupportedFeature:  aStr = "NonSupportedFeature"; break;
      case VrmlData_OutputStreamUndefined:aStr = "OutputStreamUndefined"; break;
      case VrmlData_NotImplemented:       aStr = "NotImplemented"; break;
      default:
        break;
    }
    if (aStr)
    {
      Message::SendFail() << "Error in the Vrml_Provider during reading the file " <<
        thePath << "\t: ++ VRML Error: " << aStr << " in line " << aScene.GetLineError();
      return false;
    }
    else
    {
      theShape = aShape;
    }
  }
  else
  {
    Message::SendFail() << "Error in the Vrml_Provider during reading the file " <<
      thePath << "\t: cannot open file";
    return false;
  }
  return true;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool Vrml_Provider::Write(const TCollection_AsciiString& thePath,
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
TCollection_AsciiString Vrml_Provider::GetFormat() const
{
  return TCollection_AsciiString("VRML");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString Vrml_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}
