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

#include <XSDRAWOBJ.hxx>

#include <BRep_Builder.hxx>
#include <DBRep.hxx>
#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <DEOBJ_ConfigurationNode.hxx>
#include <DE_PluginHolder.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <RWObj_CafReader.hxx>
#include <RWObj_CafWriter.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Application.hxx>
#include <TopoDS_Shape.hxx>
#include <UnitsAPI.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
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
    return false;
  }
  return true;
}

//=================================================================================================

static int ReadObj(Draw_Interpretor& theDI, int theNbArgs, const char** theArgVec)
{
  TCollection_AsciiString aDestName, aFilePath;
  bool                    toUseExistingDoc = false;
  double                  aFileUnitFactor  = -1.0;
  RWMesh_CoordinateSystem aResultCoordSys  = RWMesh_CoordinateSystem_Zup,
                          aFileCoordSys    = RWMesh_CoordinateSystem_Yup;
  bool toListExternalFiles = false, isSingleFace = false, isSinglePrecision = false;
  bool isNoDoc = (TCollection_AsciiString(theArgVec[0]) == "readobj");
  for (int anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArgCase(theArgVec[anArgIter]);
    anArgCase.LowerCase();
    if (anArgIter + 1 < theNbArgs
        && (anArgCase == "-unit" || anArgCase == "-units" || anArgCase == "-fileunit"
            || anArgCase == "-fileunits"))
    {
      const TCollection_AsciiString aUnitStr(theArgVec[++anArgIter]);
      aFileUnitFactor = UnitsAPI::AnyToSI(1.0, aUnitStr.ToCString());
      if (aFileUnitFactor <= 0.0)
      {
        Message::SendFail() << "Syntax error: wrong length unit '" << aUnitStr << "'";
        return 1;
      }
    }
    else if (anArgIter + 1 < theNbArgs
             && (anArgCase == "-filecoordinatesystem" || anArgCase == "-filecoordsystem"
                 || anArgCase == "-filecoordsys"))
    {
      if (!parseCoordinateSystem(theArgVec[++anArgIter], aFileCoordSys))
      {
        Message::SendFail() << "Syntax error: unknown coordinate system '" << theArgVec[anArgIter]
                            << "'";
        return 1;
      }
    }
    else if (anArgIter + 1 < theNbArgs
             && (anArgCase == "-resultcoordinatesystem" || anArgCase == "-resultcoordsystem"
                 || anArgCase == "-resultcoordsys" || anArgCase == "-rescoordsys"))
    {
      if (!parseCoordinateSystem(theArgVec[++anArgIter], aResultCoordSys))
      {
        Message::SendFail() << "Syntax error: unknown coordinate system '" << theArgVec[anArgIter]
                            << "'";
        return 1;
      }
    }
    else if (anArgCase == "-singleprecision" || anArgCase == "-singleprec")
    {
      isSinglePrecision = true;
      if (anArgIter + 1 < theNbArgs
          && Draw::ParseOnOff(theArgVec[anArgIter + 1], isSinglePrecision))
      {
        ++anArgIter;
      }
    }
    else if (isNoDoc && (anArgCase == "-singleface" || anArgCase == "-singletriangulation"))
    {
      isSingleFace = true;
    }
    else if (!isNoDoc && (anArgCase == "-nocreate" || anArgCase == "-nocreatedoc"))
    {
      toUseExistingDoc = true;
      if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], toUseExistingDoc))
      {
        ++anArgIter;
      }
    }
    else if (anArgCase == "-listexternalfiles" || anArgCase == "-listexternals"
             || anArgCase == "-listexternal" || anArgCase == "-external"
             || anArgCase == "-externalfiles")
    {
      toListExternalFiles = true;
    }
    else if (aDestName.IsEmpty())
    {
      aDestName = theArgVec[anArgIter];
    }
    else if (aFilePath.IsEmpty())
    {
      aFilePath = theArgVec[anArgIter];
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }
  if (aFilePath.IsEmpty())
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  occ::handle<Draw_ProgressIndicator> aProgress = new Draw_ProgressIndicator(theDI, 1);
  occ::handle<TDocStd_Document>       aDoc;
  if (!isNoDoc && !toListExternalFiles)
  {
    occ::handle<TDocStd_Application> anApp    = DDocStd::GetApplication();
    const char*                      aNameVar = aDestName.ToCString();
    DDocStd::GetDocument(aNameVar, aDoc, false);
    if (aDoc.IsNull())
    {
      if (toUseExistingDoc)
      {
        Message::SendFail() << "Error: document with name " << aDestName << " does not exist";
        return 1;
      }
      anApp->NewDocument(TCollection_ExtendedString("BinXCAF"), aDoc);
    }
    else if (!toUseExistingDoc)
    {
      Message::SendFail() << "Error: document with name " << aDestName << " already exists";
      return 1;
    }
  }
  const double aScaleFactorM = XSDRAW::GetLengthUnit() / 1000;

  RWObj_CafReader aReader;
  aReader.SetSinglePrecision(isSinglePrecision);
  aReader.SetSystemLengthUnit(aScaleFactorM);
  aReader.SetSystemCoordinateSystem(aResultCoordSys);
  aReader.SetFileLengthUnit(aFileUnitFactor);
  aReader.SetFileCoordinateSystem(aFileCoordSys);
  aReader.SetDocument(aDoc);
  if (isSingleFace)
  {
    RWObj_TriangulationReader aSimpleReader;
    aSimpleReader.SetSinglePrecision(isSinglePrecision);
    aSimpleReader.SetCreateShapes(false);
    aSimpleReader.SetTransformation(aReader.CoordinateSystemConverter());
    aSimpleReader.Read(aFilePath.ToCString(), aProgress->Start());

    occ::handle<Poly_Triangulation> aTriangulation = aSimpleReader.GetTriangulation();
    TopoDS_Face                     aFace;
    BRep_Builder                    aBuiler;
    aBuiler.MakeFace(aFace);
    aBuiler.UpdateFace(aFace, aTriangulation);
    DBRep::Set(aDestName.ToCString(), aFace);
    return 0;
  }

  if (toListExternalFiles)
  {
    aReader.ProbeHeader(aFilePath);
    for (NCollection_IndexedMap<TCollection_AsciiString>::Iterator aFileIter(
           aReader.ExternalFiles());
         aFileIter.More();
         aFileIter.Next())
    {
      theDI << "\"" << aFileIter.Value() << "\" ";
    }
  }
  else
  {
    aReader.Perform(aFilePath, aProgress->Start());
    if (isNoDoc)
    {
      DBRep::Set(aDestName.ToCString(), aReader.SingleShape());
    }
    else
    {
      occ::handle<DDocStd_DrawDocument> aDrawDoc = new DDocStd_DrawDocument(aDoc);
      TDataStd_Name::Set(aDoc->GetData()->Root(), aDestName);
      Draw::Set(aDestName.ToCString(), aDrawDoc);
    }
  }
  return 0;
}

///=================================================================================================

static int WriteObj(Draw_Interpretor& theDI, int theNbArgs, const char** theArgVec)
{
  TCollection_AsciiString          anObjFilePath;
  occ::handle<TDocStd_Document>    aDoc;
  occ::handle<TDocStd_Application> anApp = DDocStd::GetApplication();
  NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString> aFileInfo;
  double                  aFileUnitFactor = -1.0;
  RWMesh_CoordinateSystem aSystemCoordSys = RWMesh_CoordinateSystem_Zup,
                          aFileCoordSys   = RWMesh_CoordinateSystem_Yup;
  for (int anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArgCase(theArgVec[anArgIter]);
    anArgCase.LowerCase();
    if (anArgIter + 1 < theNbArgs
        && (anArgCase == "-unit" || anArgCase == "-units" || anArgCase == "-fileunit"
            || anArgCase == "-fileunits"))
    {
      const TCollection_AsciiString aUnitStr(theArgVec[++anArgIter]);
      aFileUnitFactor = UnitsAPI::AnyToSI(1.0, aUnitStr.ToCString());
      if (aFileUnitFactor <= 0.0)
      {
        Message::SendFail() << "Syntax error: wrong length unit '" << aUnitStr << "'";
        return 1;
      }
    }
    else if (anArgIter + 1 < theNbArgs
             && (anArgCase == "-filecoordinatesystem" || anArgCase == "-filecoordsystem"
                 || anArgCase == "-filecoordsys"))
    {
      if (!parseCoordinateSystem(theArgVec[++anArgIter], aFileCoordSys))
      {
        Message::SendFail() << "Syntax error: unknown coordinate system '" << theArgVec[anArgIter]
                            << "'";
        return 1;
      }
    }
    else if (anArgIter + 1 < theNbArgs
             && (anArgCase == "-systemcoordinatesystem" || anArgCase == "-systemcoordsystem"
                 || anArgCase == "-systemcoordsys" || anArgCase == "-syscoordsys"))
    {
      if (!parseCoordinateSystem(theArgVec[++anArgIter], aSystemCoordSys))
      {
        Message::SendFail() << "Syntax error: unknown coordinate system '" << theArgVec[anArgIter]
                            << "'";
        return 1;
      }
    }
    else if (anArgCase == "-comments" && anArgIter + 1 < theNbArgs)
    {
      aFileInfo.Add("Comments", theArgVec[++anArgIter]);
    }
    else if (anArgCase == "-author" && anArgIter + 1 < theNbArgs)
    {
      aFileInfo.Add("Author", theArgVec[++anArgIter]);
    }
    else if (aDoc.IsNull())
    {
      const char* aNameVar = theArgVec[anArgIter];
      DDocStd::GetDocument(aNameVar, aDoc, false);
      if (aDoc.IsNull())
      {
        TopoDS_Shape aShape = DBRep::Get(aNameVar);
        if (aShape.IsNull())
        {
          Message::SendFail() << "Syntax error: '" << aNameVar << "' is not a shape nor document";
          return 1;
        }

        anApp->NewDocument(TCollection_ExtendedString("BinXCAF"), aDoc);
        occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());
        aShapeTool->AddShape(aShape);
      }
    }
    else if (anObjFilePath.IsEmpty())
    {
      anObjFilePath = theArgVec[anArgIter];
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }
  if (anObjFilePath.IsEmpty())
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  occ::handle<Draw_ProgressIndicator> aProgress = new Draw_ProgressIndicator(theDI, 1);

  const double    aSystemUnitFactor = XSDRAW::GetLengthUnit() * 0.001;
  RWObj_CafWriter aWriter(anObjFilePath);
  aWriter.ChangeCoordinateSystemConverter().SetInputLengthUnit(aSystemUnitFactor);
  aWriter.ChangeCoordinateSystemConverter().SetInputCoordinateSystem(aSystemCoordSys);
  aWriter.ChangeCoordinateSystemConverter().SetOutputLengthUnit(aFileUnitFactor);
  aWriter.ChangeCoordinateSystemConverter().SetOutputCoordinateSystem(aFileCoordSys);
  aWriter.Perform(aDoc, aFileInfo, aProgress->Start());
  return 0;
}

namespace
{
// Singleton to ensure DEOBJ plugin is registered only once
void DEOBJSingleton()
{
  static DE_PluginHolder<DEOBJ_ConfigurationNode> aHolder;
  (void)aHolder;
}
} // namespace

//=================================================================================================

void XSDRAWOBJ::Factory(Draw_Interpretor& theDI)
{
  static bool aIsActivated = false;
  if (aIsActivated)
  {
    return;
  }
  aIsActivated = true;

  //! Ensure DEOBJ plugin is registered
  DEOBJSingleton();

  const char* aGroup = "XSTEP-STL/VRML"; // Step transfer file commands
  theDI.Add(
    "ReadObj",
    "ReadObj Doc file [-fileCoordSys {Zup|Yup}] [-fileUnit Unit]"
    "\n\t\t:                  [-resultCoordSys {Zup|Yup}] [-singlePrecision]"
    "\n\t\t:                  [-listExternalFiles] [-noCreateDoc]"
    "\n\t\t: Read OBJ file into XDE document."
    "\n\t\t:   -fileUnit       length unit of OBJ file content;"
    "\n\t\t:   -fileCoordSys   coordinate system defined by OBJ file; Yup when not specified."
    "\n\t\t:   -resultCoordSys result coordinate system; Zup when not specified."
    "\n\t\t:   -singlePrecision truncate vertex data to single precision during read; FALSE by "
    "default."
    "\n\t\t:   -listExternalFiles do not read mesh and only list external files."
    "\n\t\t:   -noCreateDoc    read into existing XDE document.",
    __FILE__,
    ReadObj,
    aGroup);
  theDI.Add("readobj",
            "readobj shape file [-fileCoordSys {Zup|Yup}] [-fileUnit Unit]"
            "\n\t\t:                    [-resultCoordSys {Zup|Yup}] [-singlePrecision]"
            "\n\t\t:                    [-singleFace]"
            "\n\t\t: Same as ReadObj but reads OBJ file into a shape instead of a document."
            "\n\t\t:   -singleFace merge OBJ content into a single triangulation Face.",
            __FILE__,
            ReadObj,
            aGroup);
  theDI.Add(
    "WriteObj",
    "WriteObj Doc file [-fileCoordSys {Zup|Yup}] [-fileUnit Unit]"
    "\n\t\t:                   [-systemCoordSys {Zup|Yup}]"
    "\n\t\t:                   [-comments Text] [-author Name]"
    "\n\t\t: Write XDE document into OBJ file."
    "\n\t\t:   -fileUnit       length unit of OBJ file content;"
    "\n\t\t:   -fileCoordSys   coordinate system defined by OBJ file; Yup when not specified."
    "\n\t\t:   -systemCoordSys system coordinate system; Zup when not specified.",
    __FILE__,
    WriteObj,
    aGroup);
  theDI.Add("writeobj", "writeobj shape file", __FILE__, WriteObj, aGroup);

  // Load XSDRAW session for pilot activation
  XSDRAW::LoadDraw(theDI);
}

// Declare entry point PLUGINFACTORY
DPLUGIN(XSDRAWOBJ)
