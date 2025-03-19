// Copyright (c) 2023 OPEN CASCADE SAS
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

#include <XSDRAWVRML.hxx>

#include <DBRep.hxx>
#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <OSD_Path.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Application.hxx>
#include <TopoDS_Shape.hxx>
#include <UnitsAPI.hxx>
#include <UnitsMethods.hxx>
#include <VrmlAPI_CafReader.hxx>
#include <VrmlAPI_Writer.hxx>
#include <VrmlData_DataMapOfShapeAppearance.hxx>
#include <VrmlData_Scene.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_ShapeProcessor.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSDRAW.hxx>

//=============================================================================
// function : parseCoordinateSystem
// purpose  : Parse RWMesh_CoordinateSystem enumeration
//=============================================================================
static bool parseCoordinateSystem(const char* theArg, RWMesh_CoordinateSystem& theSystem)
{
  TCollection_AsciiString aCSStr(theArg);
  aCSStr.LowerCase();
  if (aCSStr == "zup")
  {
    theSystem = RWMesh_CoordinateSystem_Zup;
  }
  else if (aCSStr == "yup")
  {
    theSystem = RWMesh_CoordinateSystem_Yup;
  }
  else
  {
    return Standard_False;
  }
  return Standard_True;
}

//=================================================================================================

static Standard_Integer ReadVrml(Draw_Interpretor& theDI,
                                 Standard_Integer  theArgc,
                                 const char**      theArgv)
{
  if (theArgc < 3)
  {
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  Standard_Real            aFileUnitFactor = 1.0;
  RWMesh_CoordinateSystem  aFileCoordSys   = RWMesh_CoordinateSystem_Yup,
                          aSystemCoordSys  = RWMesh_CoordinateSystem_Zup;
  Standard_Boolean        toUseExistingDoc = Standard_False;
  Standard_Boolean        toFillIncomplete = Standard_True;
  Standard_CString        aDocName         = NULL;
  TCollection_AsciiString aFilePath;

  for (Standard_Integer anArgIt = 1; anArgIt < theArgc; anArgIt++)
  {
    TCollection_AsciiString anArg(theArgv[anArgIt]);
    anArg.LowerCase();
    if (anArgIt + 1 < theArgc && anArg == "-fileunit")
    {
      const TCollection_AsciiString aUnitStr(theArgv[++anArgIt]);
      aFileUnitFactor = UnitsAPI::AnyToSI(1.0, aUnitStr.ToCString());
      if (aFileUnitFactor <= 0.0)
      {
        Message::SendFail() << "Error: wrong length unit '" << aUnitStr << "'";
        return 1;
      }
    }
    else if (anArgIt + 1 < theArgc && anArg == "-filecoordsys")
    {
      if (!parseCoordinateSystem(theArgv[++anArgIt], aFileCoordSys))
      {
        Message::SendFail() << "Error: unknown coordinate system '" << theArgv[anArgIt] << "'";
        return 1;
      }
    }
    else if (anArgIt + 1 < theArgc && anArg == "-systemcoordsys")
    {
      if (!parseCoordinateSystem(theArgv[++anArgIt], aSystemCoordSys))
      {
        Message::SendFail() << "Error: unknown coordinate system '" << theArgv[anArgIt] << "'";
        return 1;
      }
    }
    else if (anArg == "-fillincomplete")
    {
      toFillIncomplete = true;
      if (anArgIt + 1 < theArgc && Draw::ParseOnOff(theArgv[anArgIt + 1], toFillIncomplete))
      {
        ++anArgIt;
      }
    }
    else if (anArg == "-nocreatedoc")
    {
      toUseExistingDoc = true;
    }
    else if (aDocName == nullptr)
    {
      aDocName = theArgv[anArgIt];
      DDocStd::GetDocument(aDocName, aDoc, Standard_False);
    }
    else if (aFilePath.IsEmpty())
    {
      aFilePath = theArgv[anArgIt];
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << theArgv[anArgIt] << "'";
      return 1;
    }
  }

  if (aFilePath.IsEmpty() || aDocName == nullptr)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  if (aDoc.IsNull())
  {
    if (toUseExistingDoc)
    {
      Message::SendFail() << "Error: document with name " << aDocName << " does not exist";
      return 1;
    }
    Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
    anApp->NewDocument("BinXCAF", aDoc);
  }
  else if (!toUseExistingDoc)
  {
    Message::SendFail() << "Error: document with name " << aDocName << " already exists\n";
    return 1;
  }

  Standard_Real aScaleFactor = 1.;
  if (!XCAFDoc_DocumentTool::GetLengthUnit(aDoc, aScaleFactor))
  {
    XSAlgo_ShapeProcessor::PrepareForTransfer();
    aScaleFactor = UnitsMethods::GetCasCadeLengthUnit();
  }

  VrmlAPI_CafReader aVrmlReader;
  aVrmlReader.SetDocument(aDoc);
  aVrmlReader.SetFileLengthUnit(aFileUnitFactor);
  aVrmlReader.SetSystemLengthUnit(aScaleFactor);
  aVrmlReader.SetFileCoordinateSystem(aFileCoordSys);
  aVrmlReader.SetSystemCoordinateSystem(aSystemCoordSys);
  aVrmlReader.SetFillIncompleteDocument(toFillIncomplete);

  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(theDI, 1);
  if (!aVrmlReader.Perform(aFilePath, aProgress->Start()))
  {
    if (aVrmlReader.ExtraStatus() != RWMesh_CafReaderStatusEx_Partial)
    {
      Message::SendFail() << "Error: file reading failed '" << aFilePath << "'";
      return 1;
    }
    Message::SendWarning() << "Warning: file has been read paratially (due to unexpected EOF, "
                              "syntax error, memory limit) "
                           << aFilePath;
  }

  TDataStd_Name::Set(aDoc->GetData()->Root(), aDocName);
  Handle(DDocStd_DrawDocument) aDD = new DDocStd_DrawDocument(aDoc);
  Draw::Set(aDocName, aDD);

  return 0;
}

//=======================================================================
// function : WriteVrml
// purpose  : Write DECAF document to Vrml
//=======================================================================
static Standard_Integer WriteVrml(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Use: " << argv[0] << " Doc filename: write document to Vrml file\n";
    return 0;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    di << argv[1] << " is not a document\n";
    return 1;
  }

  if (argc < 3 || argc > 5)
  {
    di << "wrong number of parameters\n";
    return 0;
  }

  VrmlAPI_Writer writer;
  writer.SetRepresentation(VrmlAPI_ShadedRepresentation);
  Standard_Real aScaleFactorM = 1.;
  if (!XCAFDoc_DocumentTool::GetLengthUnit(aDoc, aScaleFactorM))
  {
    XSAlgo_ShapeProcessor::PrepareForTransfer(); // update unit info
    aScaleFactorM = UnitsMethods::GetCasCadeLengthUnit(UnitsMethods_LengthUnit_Meter);
  }
  if (!writer.WriteDoc(aDoc, argv[2], aScaleFactorM))
  {
    di << "Error: File " << argv[2] << " was not written\n";
  }

  return 0;
}

//=================================================================================================

static Standard_Integer loadvrml(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3)
    di << "wrong number of parameters" << "\n";
  else
  {
    TopoDS_Shape                      aShape;
    VrmlData_DataMapOfShapeAppearance aShapeAppMap;

    //-----------------------------------------------------------
    std::filebuf aFic;
    std::istream aStream(&aFic);

    if (aFic.open(argv[2], std::ios::in))
    {

      // Get path of the VRML file.
      OSD_Path                aPath(argv[2]);
      TCollection_AsciiString aVrmlDir(".");
      TCollection_AsciiString aDisk = aPath.Disk();
      TCollection_AsciiString aTrek = aPath.Trek();
      if (!aTrek.IsEmpty())
      {
        if (!aDisk.IsEmpty())
          aVrmlDir = aDisk;
        else
          aVrmlDir.Clear();
        aTrek.ChangeAll('|', '/');
        aVrmlDir += aTrek;
      }

      VrmlData_Scene      aScene;
      const Standard_Real anOCCUnitMM = XSDRAW::GetLengthUnit();
      aScene.SetLinearScale(1000. / anOCCUnitMM);

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
        di << " ++ VRML Error: " << aStr << " in line " << aScene.GetLineError() << "\n";
      }
      else
      {
        DBRep::Set(argv[1], aShape);
      }
    }
    else
    {
      di << "cannot open file\n";
    }

    //-----------------------------------------------------------
  }
  return 0;
}

//=================================================================================================

static Standard_Integer writevrml(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3 || argc > 5)
  {
    di << "wrong number of parameters\n";
    return 0;
  }

  TopoDS_Shape aShape = DBRep::Get(argv[1]);

  // Get the optional parameters
  Standard_Integer aVersion = 2;
  Standard_Integer aType    = 1;
  if (argc >= 4)
  {
    aVersion = Draw::Atoi(argv[3]);
    if (argc == 5)
      aType = Draw::Atoi(argv[4]);
  }

  // Bound parameters
  aVersion = Max(1, aVersion);
  aVersion = Min(2, aVersion);
  aType    = Max(0, aType);
  aType    = Min(2, aType);

  VrmlAPI_Writer writer;

  switch (aType)
  {
    case 0:
      writer.SetRepresentation(VrmlAPI_ShadedRepresentation);
      break;
    case 1:
      writer.SetRepresentation(VrmlAPI_WireFrameRepresentation);
      break;
    case 2:
      writer.SetRepresentation(VrmlAPI_BothRepresentation);
      break;
  }

  if (!writer.Write(aShape, argv[2], aVersion))
  {
    di << "Error: File " << argv[2] << " was not written\n";
  }

  return 0;
}

//=================================================================================================

void XSDRAWVRML::Factory(Draw_Interpretor& theDI)
{
  static Standard_Boolean anInitActor = Standard_False;
  if (anInitActor)
  {
    return;
  }
  anInitActor = Standard_True;

  Standard_CString aGroup = "XDE translation commands";
  theDI.Add(
    "ReadVrml",
    "ReadVrml docName filePath [-fileCoordSys {Zup|Yup}] [-fileUnit Unit]"
    "\n\t\t:                   [-systemCoordSys {Zup|Yup}] [-noCreateDoc] [-fillIncomplete "
    "{ON|OFF}]"
    "\n\t\t: Read Vrml file into XDE document."
    "\n\t\t:   -fileCoordSys   coordinate system defined by Vrml file; Yup when not specified."
    "\n\t\t:   -fileUnit       length unit of Vrml file content."
    "\n\t\t:   -systemCoordSys result coordinate system; Zup when not specified."
    "\n\t\t:   -noCreateDoc    read into existing XDE document."
    "\n\t\t:   -fillIncomplete fill the document with partially retrieved data even if reader has "
    "failed with "
    "error; true when not specified",
    __FILE__,
    ReadVrml,
    aGroup);
  theDI.Add("WriteVrml",
            "WriteVrml Doc filename [version VRML#1.0/VRML#2.0 (1/2): 2 by default] "
            "[representation shaded/wireframe/both (0/1/2): 0 by default]",
            __FILE__,
            WriteVrml,
            aGroup);
  theDI.Add("loadvrml", "shape file", __FILE__, loadvrml, aGroup);
  theDI.Add("writevrml",
            "shape file [version VRML#1.0/VRML#2.0 (1/2): 2 by default] [representation "
            "shaded/wireframe/both (0/1/2): 1 by default]",
            __FILE__,
            writevrml,
            aGroup);

  // Load XSDRAW session for pilot activation
  XSDRAW::LoadDraw(theDI);
}

// Declare entry point PLUGINFACTORY
DPLUGIN(XSDRAWVRML)
