// Created on: 2003-08-15
// Created by: Sergey ZARITCHNY
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <DE_ConfigurationContext.hxx>
#include <DE_Wrapper.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Message.hxx>
#include <IFSelect_SessionPilot.hxx>
#include <IGESCAFControl_Reader.hxx>
#include <IGESCAFControl_Writer.hxx>
#include <IGESControl_Controller.hxx>
#include <Interface_Macros.hxx>
#include <OSD_OpenFile.hxx>
#include <OSD_Path.hxx>
#include <STEPCAFControl_ExternFile.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <STEPControl_Controller.hxx>
#include <TDF_Data.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <XDEDRAW_Common.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_AlgoContainer.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSDRAW.hxx>
#include <XSDRAW_Vars.hxx>
#include <VrmlAPI_CafReader.hxx>
#include <VrmlAPI_Writer.hxx>
#include <DDF.hxx>

#include <DBRep.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_Editor.hxx>
#include <TDF_Tool.hxx>
#include <TopoDS_Shape.hxx>
#include <Interface_Static.hxx>
#include <UnitsAPI.hxx>
#include <UnitsMethods.hxx>

#include <stdio.h>

//============================================================
// Support for several models in DRAW
//============================================================
static NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)> thedictws;

//=======================================================================
//function : parseCoordinateSystem
//purpose  : Parse RWMesh_CoordinateSystem enumeration.
//=======================================================================
static bool parseCoordinateSystem(const char* theArg,
                                  RWMesh_CoordinateSystem& theSystem)
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

static Standard_Boolean ClearDicWS()
{
  thedictws.Clear();
  return Standard_True;
}

static void AddWS(TCollection_AsciiString filename,
                  const Handle(XSControl_WorkSession)& WS)
{
  WS->SetVars(new XSDRAW_Vars); // support of DRAW variables
  thedictws.Bind(filename, WS);
}


static Standard_Boolean FillDicWS(NCollection_DataMap<TCollection_AsciiString, Handle(STEPCAFControl_ExternFile)>& dicFile)
{
  ClearDicWS();
  if (dicFile.IsEmpty())
  {
    return Standard_False;
  }
  Handle(STEPCAFControl_ExternFile) EF;
  NCollection_DataMap<TCollection_AsciiString, Handle(STEPCAFControl_ExternFile)>::Iterator DicEFIt(dicFile);
  for (; DicEFIt.More(); DicEFIt.Next())
  {
    TCollection_AsciiString filename = DicEFIt.Key();
    EF = DicEFIt.Value();
    AddWS(filename, EF->GetWS());
  }
  return Standard_True;
}

static Standard_Boolean SetCurrentWS(TCollection_AsciiString filename)
{
  if (!thedictws.IsBound(filename)) return Standard_False;
  Handle(XSControl_WorkSession) CurrentWS =
    Handle(XSControl_WorkSession)::DownCast(thedictws.ChangeFind(filename));
  XSDRAW::Pilot()->SetSession(CurrentWS);

  return Standard_True;
}


//=======================================================================
//function : SetCurWS
//purpose  : Set current file if many files are read
//=======================================================================

static Standard_Integer SetCurWS(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2)
  {
    di << "Use: " << argv[0] << " filename \n";
    return 1;
  }
  TCollection_AsciiString filename(argv[1]);
  SetCurrentWS(filename);
  return 0;
}


//=======================================================================
//function : GetDicWSList
//purpose  : List all files recorded after translation
//=======================================================================

static Standard_Integer GetDicWSList(Draw_Interpretor& di, Standard_Integer /*argc*/, const char** /*argv*/)
{
  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)> DictWS = thedictws;
  if (DictWS.IsEmpty()) return 1;
  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>::Iterator DicIt(DictWS);
  di << " The list of last translated files:\n";
  Standard_Integer num = 0;
  for (; DicIt.More(); DicIt.Next(), num++)
  {
    TCollection_AsciiString strng(DicIt.Key());
    if (num) di << "\n";
    di << "\"" << strng.ToCString() << "\"";
  }
  return 0;
}

//=======================================================================
//function : GetCurWS
//purpose  : Return name of file which is current
//=======================================================================

static Standard_Integer GetCurWS(Draw_Interpretor& di, Standard_Integer /*argc*/, const char** /*argv*/)
{
  Handle(XSControl_WorkSession) WS = XSDRAW::Session();
  di << "\"" << WS->LoadedFile() << "\"";
  return 0;
}

//=======================================================================
//function : FromShape
//purpose  : Apply fromshape command to all the loaded WSs
//=======================================================================

static Standard_Integer FromShape(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2)
  {
    di << argv[0] << " shape: search for shape origin among all last tranalated files\n";
    return 0;
  }

  char command[256];
  Sprintf(command, "fromshape %.200s -1", argv[1]);
  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)> DictWS = thedictws;
  if (DictWS.IsEmpty()) return di.Eval(command);

  Handle(XSControl_WorkSession) WS = XSDRAW::Session();

  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>::Iterator DicIt(DictWS);
  //  di << "Searching for shape among all the loaded files:\n";
  Standard_Integer num = 0;
  for (; DicIt.More(); DicIt.Next(), num++)
  {
    Handle(XSControl_WorkSession) CurrentWS =
      Handle(XSControl_WorkSession)::DownCast(DicIt.Value());
    XSDRAW::Pilot()->SetSession(CurrentWS);
    di.Eval(command);
  }

  XSDRAW::Pilot()->SetSession(WS);
  return 0;
}

//=======================================================================
//function : ReadIges
//purpose  : Read IGES to DECAF document
//=======================================================================

static Standard_Integer ReadIges(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Use: " << argv[0] << " Doc filename [mode]: read IGES file to a document\n";
    return 0;
  }

  DeclareAndCast(IGESControl_Controller, ctl, XSDRAW::Controller());
  if (ctl.IsNull()) XSDRAW::SetNorm("IGES");

  TCollection_AsciiString fnom, rnom;
  Standard_Boolean modfic = XSDRAW::FileAndVar(argv[2], argv[1], "IGES", fnom, rnom);
  if (modfic) di << " File IGES to read : " << fnom.ToCString() << "\n";
  else        di << " Model taken from the session : " << fnom.ToCString() << "\n";
  //  di<<" -- Names of variables BREP-DRAW prefixed by : "<<rnom<<"\n";

  IGESCAFControl_Reader reader(XSDRAW::Session(), modfic);
  Standard_Integer onlyvisible = Interface_Static::IVal("read.iges.onlyvisible");
  reader.SetReadVisible(onlyvisible == 1);

  if (argc == 4)
  {
    Standard_Boolean mode = Standard_True;
    for (Standard_Integer i = 0; argv[3][i]; i++)
      switch (argv[3][i])
      {
        case '-': mode = Standard_False; break;
        case '+': mode = Standard_True; break;
        case 'c': reader.SetColorMode(mode); break;
        case 'n': reader.SetNameMode(mode); break;
        case 'l': reader.SetLayerMode(mode); break;
      }
  }

  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(di);
  Message_ProgressScope aRootScope(aProgress->Start(), "IGES import", modfic ? 2 : 1);

  IFSelect_ReturnStatus readstat = IFSelect_RetVoid;
  if (modfic)
  {
    Message_ProgressScope aReadScope(aRootScope.Next(), "File reading", 1);
    aReadScope.Show();
    readstat = reader.ReadFile(fnom.ToCString());
  }
  else if (XSDRAW::Session()->NbStartingEntities() > 0)
  {
    readstat = IFSelect_RetDone;
  }
  if (readstat != IFSelect_RetDone)
  {
    if (modfic)
    {
      di << "Could not read file " << fnom.ToCString() << " , abandon\n";
    }
    else
    {
      di << "No model loaded\n";
    }
    return 1;
  }

  Handle(TDocStd_Document) doc;
  if (!DDocStd::GetDocument(argv[1], doc, Standard_False))
  {
    Handle(TDocStd_Application) A = DDocStd::GetApplication();
    A->NewDocument("BinXCAF", doc);
    TDataStd_Name::Set(doc->GetData()->Root(), argv[1]);
    Handle(DDocStd_DrawDocument) DD = new DDocStd_DrawDocument(doc);
    Draw::Set(argv[1], DD);
    //     di << "Document saved with name " << argv[1];
  }
  if (!reader.Transfer(doc, aRootScope.Next()))
  {
    di << "Cannot read any relevant data from the IGES file\n";
    return 1;
  }

  //  Handle(DDocStd_DrawDocument) DD = new DDocStd_DrawDocument(doc);  
  //  Draw::Set(argv[1],DD);       
  di << "Document saved with name " << argv[1];

  return 0;
}

//=======================================================================
//function : WriteIges
//purpose  : Write DECAF document to IGES
//=======================================================================

static Standard_Integer WriteIges(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Use: " << argv[0] << " Doc filename [mode]: write document to IGES file\n";
    return 0;
  }

  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if (Doc.IsNull())
  {
    di << argv[1] << " is not a document\n";
    return 1;
  }

  XSDRAW::SetNorm("IGES");

  TCollection_AsciiString fnom, rnom;
  const Standard_Boolean modfic = XSDRAW::FileAndVar(argv[2], argv[1], "IGES", fnom, rnom);

  //  IGESControl_Writer ICW (Interface_Static::CVal("write.iges.unit"),
  //			  Interface_Static::IVal("write.iges.brep.mode"));

  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(di);
  Message_ProgressScope aRootScope(aProgress->Start(), "IGES export", modfic ? 2 : 1);

  IGESCAFControl_Writer writer(XSDRAW::Session(), Standard_True);
  if (argc == 4)
  {
    Standard_Boolean mode = Standard_True;
    for (Standard_Integer i = 0; argv[3][i]; i++)
      switch (argv[3][i])
      {
        case '-': mode = Standard_False; break;
        case '+': mode = Standard_True; break;
        case 'c': writer.SetColorMode(mode); break;
        case 'n': writer.SetNameMode(mode); break;
        case 'l': writer.SetLayerMode(mode); break;
      }
  }
  writer.Transfer(Doc, aRootScope.Next());

  if (modfic)
  {
    Message_ProgressScope aWriteScope(aRootScope.Next(), "File writing", 1);
    aWriteScope.Show();
    di << "Writing IGES model to file " << argv[2] << "\n";
    if (writer.Write(argv[2]))
    {
      di << " Write OK\n";
    }
    else
    {
      di << " Write failed\n";
    }
  }
  else
  {
    di << "Document has been translated into the session";
  }
  return 0;
}

//=======================================================================
//function : ReadStep
//purpose  : Read STEP file to DECAF document 
//=======================================================================
static Standard_Integer ReadStep(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  DeclareAndCast(STEPControl_Controller, ctl, XSDRAW::Controller());
  if (ctl.IsNull())
  {
    XSDRAW::SetNorm ("STEP");
  }

  Standard_CString aDocName = NULL;
  TCollection_AsciiString aFilePath, aModeStr;
  bool toTestStream = false;
  for (Standard_Integer anArgIter = 1; anArgIter < argc; ++anArgIter)
  {
    TCollection_AsciiString anArgCase(argv[anArgIter]);
    anArgCase.LowerCase();
    if (anArgCase == "-stream")
    {
      toTestStream = true;
    }
    else if (aDocName == NULL)
    {
      aDocName = argv[anArgIter];
    }
    else if (aFilePath.IsEmpty())
    {
      aFilePath = argv[anArgIter];
    }
    else if (aModeStr.IsEmpty())
    {
      aModeStr = argv[anArgIter];
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << argv[anArgIter] << "'";
      return 1;
    }
  }

  TCollection_AsciiString aFileName, anOldVarName;
  Standard_Boolean isFileMode = XSDRAW::FileAndVar (aFilePath.ToCString(), aDocName, "STEP", aFileName, anOldVarName);
  if (isFileMode) di << " File STEP to read : " << aFileName << "\n";
  else            di << " Model taken from the session : " << aFileName << "\n";
  //  di<<" -- Names of variables BREP-DRAW prefixed by : "<<rnom<<"\n";

  STEPCAFControl_Reader aReader (XSDRAW::Session(), isFileMode);
  if (!aModeStr.IsEmpty())
  {
    Standard_Boolean aMode = Standard_True;
    for (Standard_Integer i = 1; aModeStr.Value (i); ++i)
    {
      switch (aModeStr.Value (i))
      {
        case '-' : aMode = Standard_False; break;
        case '+' : aMode = Standard_True;  break;
        case 'c' : aReader.SetColorMode (aMode); break;
        case 'n' : aReader.SetNameMode  (aMode); break;
        case 'l' : aReader.SetLayerMode (aMode); break;
        case 'v' : aReader.SetPropsMode (aMode); break;
        default:
        {
          Message::SendFail() << "Syntax error at '" << aModeStr << "'\n";
          return 1;
        }
      }
    }
  }
  
  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator (di);
  Message_ProgressScope aRootScope (aProgress->Start(), "STEP import", isFileMode ? 2 : 1);

  IFSelect_ReturnStatus aReadStat = IFSelect_RetVoid;
  if (isFileMode)
  {
    Message_ProgressScope aReadScope (aRootScope.Next(), "File reading", 1);
    aReadScope.Show();
    if (toTestStream)
    {
      std::ifstream aStream;
      OSD_OpenStream (aStream, aFileName.ToCString(), std::ios::in | std::ios::binary);
      TCollection_AsciiString aFolder, aFileNameShort;
      OSD_Path::FolderAndFileFromPath (aFileName, aFolder, aFileNameShort);
      aReadStat = aReader.ReadStream (aFileNameShort.ToCString(), aStream);
    }
    else
    {
      aReadStat = aReader.ReadFile (aFileName.ToCString());
    }
  }
  else if (XSDRAW::Session()->NbStartingEntities() > 0)
  {
    aReadStat = IFSelect_RetDone;
  }
  if (aReadStat != IFSelect_RetDone)
  {
    if (isFileMode)
    {
      di << "Could not read file " << aFileName << " , abandon\n";
    }
    else
    {
      di << "No model loaded\n";
    }
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  if (!DDocStd::GetDocument (aDocName, aDoc, Standard_False))
  {
    Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
    anApp->NewDocument("BinXCAF", aDoc);
    TDataStd_Name::Set (aDoc->GetData()->Root(), aDocName);
    Handle(DDocStd_DrawDocument) aDrawDoc = new DDocStd_DrawDocument (aDoc);
    Draw::Set (aDocName, aDrawDoc);
    //     di << "Document saved with name " << aDocName;
  }
  if (!aReader.Transfer (aDoc, aRootScope.Next()))
  {
    di << "Cannot read any relevant data from the STEP file\n";
    return 1;
  }

  Handle(DDocStd_DrawDocument) aDrawDoc = new DDocStd_DrawDocument (aDoc);
  Draw::Set (aDocName, aDrawDoc);
  di << "Document saved with name " << aDocName;

  NCollection_DataMap<TCollection_AsciiString, Handle(STEPCAFControl_ExternFile)> aDicFile = aReader.ExternFiles();
  FillDicWS (aDicFile);
  AddWS (aFileName, XSDRAW::Session());
  return 0;
}

//=======================================================================
//function : WriteStep
//purpose  : Write DECAF document to STEP
//=======================================================================
static Standard_Integer WriteStep(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  DeclareAndCast(STEPControl_Controller,ctl,XSDRAW::Controller());
  if (ctl.IsNull())
  {
    XSDRAW::SetNorm ("STEP");
  }
  STEPCAFControl_Writer aWriter (XSDRAW::Session(), Standard_True);

  Handle(TDocStd_Document) aDoc;
  TCollection_AsciiString aDocName, aFilePath;
  STEPControl_StepModelType aMode = STEPControl_AsIs;
  bool hasModeArg = false, toTestStream = false;
  TCollection_AsciiString aMultiFilePrefix, aLabelName;
  TDF_Label aLabel;
  for (Standard_Integer anArgIter = 1; anArgIter < argc; ++anArgIter)
  {
    TCollection_AsciiString anArgCase (argv[anArgIter]);
    anArgCase.LowerCase();
    if (anArgCase == "-stream")
    {
      toTestStream = true;
    }
    else if (aDocName.IsEmpty())
    {
      Standard_CString aDocNameStr = argv[anArgIter];
      DDocStd::GetDocument (aDocNameStr, aDoc);
      if (aDoc.IsNull())
      {
        di << "Syntax error: '" << argv[anArgIter] << "' is not a document";
        return 1;
      }
      aDocName = aDocNameStr;
    }
    else if (aFilePath.IsEmpty())
    {
      aFilePath = argv[anArgIter];
    }
    else if (!hasModeArg)
    {
      hasModeArg = true;
      switch (anArgCase.Value (1))
      {
        case 'a':
        case '0': aMode = STEPControl_AsIs;                    break;
        case 'f':
        case '1': aMode = STEPControl_FacetedBrep;             break;
        case 's':
        case '2': aMode = STEPControl_ShellBasedSurfaceModel;  break;
        case 'm':
        case '3': aMode = STEPControl_ManifoldSolidBrep;       break;
        case 'w':
        case '4': aMode = STEPControl_GeometricCurveSet;       break;
        default:
        {
          di << "Syntax error: mode '" << argv[anArgIter] << "' is incorrect [give fsmw]";
          return 1;
        }
      }
      Standard_Boolean wrmode = Standard_True;
      for (Standard_Integer i = 1; i <= anArgCase.Length(); ++i)
      {
        switch (anArgCase.Value (i))
        {
          case '-' : wrmode = Standard_False; break;
          case '+' : wrmode = Standard_True;  break;
          case 'c' : aWriter.SetColorMode (wrmode); break;
          case 'n' : aWriter.SetNameMode  (wrmode); break;
          case 'l' : aWriter.SetLayerMode (wrmode); break;
          case 'v' : aWriter.SetPropsMode (wrmode); break;
        }
      }
    }
    else if (aMultiFilePrefix.IsEmpty()
          && anArgCase.Search (":") == -1)
    {
      aMultiFilePrefix = argv[anArgIter];
    }
    else if (aLabel.IsNull())
    {
      if (!DDF::FindLabel (aDoc->Main().Data(), argv[anArgIter], aLabel)
       || aLabel.IsNull())
      {
        di << "Syntax error: No label for entry '" << argv[anArgIter] << "'";
        return 1;
      }
      aLabelName = argv[anArgIter];
    }
    else
    {
      di << "Syntax error: unknown argument '" << argv[anArgIter] << "'";
      return 1;
    }
  }
  if (aFilePath.IsEmpty())
  {
    di << "Syntax error: wrong number of arguments";
    return 1;
  }

  TCollection_AsciiString aFileName, anOldVarName;
  const Standard_Boolean isFileMode = XSDRAW::FileAndVar (aFilePath.ToCString(), aDocName.ToCString(), "STEP", aFileName, anOldVarName);

  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator (di);
  Message_ProgressScope aRootScope (aProgress->Start(), "STEP export", isFileMode ? 2 : 1);
  if (!aLabel.IsNull())
  {  
    di << "Translating label " << aLabelName << " of document " << aDocName << " to STEP\n";
    if (!aWriter.Transfer (aLabel, aMode,
                           !aMultiFilePrefix.IsEmpty() ? aMultiFilePrefix.ToCString() : NULL,
                           aRootScope.Next()))
    {
      di << "Error: the label of document cannot be translated or gives no result";
      return 1;
    }
  }
  else
  {
    di << "Translating document " << aDocName << " to STEP\n";
    if (!aWriter.Transfer (aDoc, aMode,
                           !aMultiFilePrefix.IsEmpty() ? aMultiFilePrefix.ToCString() : NULL,
                           aRootScope.Next()))
    {
      di << "Error: The document cannot be translated or gives no result\n";
    }
  }

  if (!isFileMode)
  {
    di << "Document has been translated into the session";
    return 0;
  }

  Message_ProgressScope aWriteScope (aRootScope.Next(), "File writing", 1);
  aWriteScope.Show();
  di << "Writing STEP file " << aFilePath << "\n";

  IFSelect_ReturnStatus aStat = IFSelect_RetVoid;
  if (toTestStream)
  {
    std::ofstream aStream;
    OSD_OpenStream (aStream, aFilePath, std::ios::out | std::ios::binary);
    aStat = aWriter.WriteStream (aStream);
    aStream.close();
    if (!aStream.good()
      && aStat == IFSelect_RetDone)
    {
      aStat = IFSelect_RetFail;
    }
  }
  else
  {
    aStat = aWriter.Write (aFilePath.ToCString());
  }

  switch (aStat)
  {
    case IFSelect_RetVoid:
    {
      di << "Error: no file written";
      break;
    }
    case IFSelect_RetDone:
    {
      di << "File " << aFilePath << " written\n";

      NCollection_DataMap<TCollection_AsciiString, Handle(STEPCAFControl_ExternFile)> aDicFile = aWriter.ExternFiles();
      FillDicWS (aDicFile);
      AddWS (aFilePath, XSDRAW::Session());
      break;
    }
    default:
    {
      di << "Error on writing file";
      break;
    }
  }
  return 0;
}

//=======================================================================
//function : Expand
//purpose  :
//=======================================================================
static Standard_Integer Expand(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Use: " << argv[0] << " Doc recurs(0/1) or Doc recurs(0/1) label1 label2 ... or Doc recurs(0/1 shape1 shape2 ...\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if (Doc.IsNull())
  {
    di << argv[1] << " is not a document\n"; return 1;
  }

  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  Standard_Boolean recurs = Standard_False;
  if (atoi(argv[2]) != 0)
    recurs = Standard_True;

  if (argc == 3)
  {
    if (!XCAFDoc_Editor::Expand(Doc->Main(), recurs))
    {
      di << "No suitable labels to expand\n";
      return 1;
    }
  }
  else
  {
    for (Standard_Integer i = 3; i < argc; i++)
    {
      TDF_Label aLabel;
      TDF_Tool::Label(Doc->GetData(), argv[i], aLabel);
      if (aLabel.IsNull())
      {
        TopoDS_Shape aShape;
        aShape = DBRep::Get(argv[i]);
        aLabel = aShapeTool->FindShape(aShape);
      }

      if (!aLabel.IsNull())
      {
        if (!XCAFDoc_Editor::Expand(Doc->Main(), aLabel, recurs))
        {
          di << "The shape is assembly or not compound\n";
          return 1;
        }
      }
      else
      {
        di << argv[i] << " is not a shape\n"; return 1;
      }
    }
  }
  return 0;
}

//=======================================================================
//function : Extract
//purpose  :
//=======================================================================
static Standard_Integer Extract(Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char** argv)
{
  if (argc < 4)
  {
    di << "Use: " << argv[0] << "dstDoc [dstAssmblSh] srcDoc srcLabel1 srcLabel2 ...\n";
    return 1;
  }

  Handle(TDocStd_Document) aSrcDoc, aDstDoc;
  DDocStd::GetDocument(argv[1], aDstDoc);
  if (aDstDoc.IsNull())
  {
    di << "Error " << argv[1] << " is not a document\n";
    return 1;
  }
  TDF_Label aDstLabel;
  Standard_Integer anArgInd = 3;
  TDF_Tool::Label(aDstDoc->GetData(), argv[2], aDstLabel);
  Handle(XCAFDoc_ShapeTool) aDstShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDstDoc->Main());
  if (aDstLabel.IsNull())
  {
    aDstLabel = aDstShapeTool->Label();
    anArgInd = 2; // to get Src Doc
  }
  DDocStd::GetDocument(argv[anArgInd++], aSrcDoc);
  if (aSrcDoc.IsNull())
  {
    di << "Error " << argv[anArgInd] << " is not a document\n";
    return 1;
  }

  TDF_LabelSequence aSrcShapes;
  for (; anArgInd < argc; anArgInd++)
  {
    TDF_Label aSrcLabel;
    TDF_Tool::Label(aSrcDoc->GetData(), argv[anArgInd], aSrcLabel);
    if (aSrcLabel.IsNull())
    {
      di << "[" << argv[anArgInd] << "] is not valid Src label\n";
      return 1;
    }
    aSrcShapes.Append(aSrcLabel);
  }
  if (aSrcShapes.IsEmpty())
  {
    di << "Error: No Shapes to extract\n";
    return 1;
  }

  if (!XCAFDoc_Editor::Extract(aSrcShapes, aDstLabel))
  {
    di << "Error: Cannot extract labels\n";
    return 1;
  }
  return 0;
}

//=======================================================================
//function : ReadVrml
//purpose  :
//=======================================================================
static Standard_Integer ReadVrml(Draw_Interpretor& theDI,
                                 Standard_Integer  theArgc,
                                 const char**      theArgv)
{
  if(theArgc < 3)
  {
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  Standard_Real aFileUnitFactor = 1.0;
  RWMesh_CoordinateSystem aFileCoordSys = RWMesh_CoordinateSystem_Yup, aSystemCoordSys = RWMesh_CoordinateSystem_Zup;
  Standard_Boolean toUseExistingDoc = Standard_False;
  Standard_Boolean toFillIncomplete = Standard_True;
  Standard_CString aDocName = NULL;
  TCollection_AsciiString aFilePath;

  for(Standard_Integer anArgIt = 1; anArgIt < theArgc; anArgIt++)
  {
    TCollection_AsciiString anArg(theArgv[anArgIt]);
    anArg.LowerCase();
    if(anArgIt + 1 < theArgc && anArg == "-fileunit")
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
    else if(aFilePath.IsEmpty())
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
    if(toUseExistingDoc)
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
    XSAlgo::AlgoContainer()->PrepareForTransfer();
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
    Message::SendWarning() <<
      "Warning: file has been read paratially (due to unexpected EOF, syntax error, memory limit) " << aFilePath;
  }

  TDataStd_Name::Set(aDoc->GetData()->Root(), aDocName);
  Handle(DDocStd_DrawDocument) aDD = new DDocStd_DrawDocument(aDoc);
  Draw::Set(aDocName, aDD);

  return 0;
}

//=======================================================================
//function : WriteVrml
//purpose  : Write DECAF document to Vrml
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
    XSAlgo::AlgoContainer()->PrepareForTransfer(); // update unit info
    aScaleFactorM = UnitsMethods::GetCasCadeLengthUnit(UnitsMethods_LengthUnit_Meter);
  }
  if (!writer.WriteDoc(aDoc, argv[2], aScaleFactorM))
  {
    di << "Error: File " << argv[2] << " was not written\n";
  }

  return 0;
}

//=======================================================================
//function : DumpConfiguration
//purpose  : 
//=======================================================================
static Standard_Integer DumpConfiguration(Draw_Interpretor& theDI,
                                          Standard_Integer theNbArgs,
                                          const char** theArgVec)
{
  Handle(DE_Wrapper) aConf = DE_Wrapper::GlobalWrapper();
  TCollection_AsciiString aPath;
  Standard_Boolean aIsRecursive = Standard_True;
  Standard_Boolean isHandleFormat = Standard_False;
  Standard_Boolean isHandleVendors = Standard_False;
  TColStd_ListOfAsciiString aFormats;
  TColStd_ListOfAsciiString aVendors;
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if ((anArg == "-path") &&
        (anArgIter + 1 < theNbArgs))
    {
      ++anArgIter;
      aPath = theArgVec[anArgIter];
    }
    else if ((anArg == "-recursive") &&
             (anArgIter + 1 < theNbArgs) &&
             Draw::ParseOnOff(theArgVec[anArgIter + 1], aIsRecursive))
    {
      ++anArgIter;
    }
    else if (anArg == "-format")
    {
      isHandleFormat = Standard_True;
      isHandleVendors = Standard_False;
    }
    else if (anArg == "-vendor")
    {
      isHandleFormat = Standard_False;
      isHandleVendors = Standard_True;
    }
    else if (isHandleFormat)
    {
      aFormats.Append(theArgVec[anArgIter]);
    }
    else if (isHandleVendors)
    {
      aVendors.Append(theArgVec[anArgIter]);
    }
    else if (!isHandleFormat && !isHandleVendors)
    {
      Message::SendFail() << "Syntax error at argument '" << theArgVec[anArgIter] << "'\n";
      return 1;
    }
  }
  Standard_Boolean aStat = Standard_True;
  if (!aPath.IsEmpty())
  {
    aStat = aConf->Save(aPath, aIsRecursive, aFormats ,aVendors);
  }
  else
  {
    theDI << aConf->Save(aIsRecursive, aFormats, aVendors) << "\n";
  }
  if (!aStat)
  {
    return 1;
  }
  return 0;
}

//=======================================================================
//function : CompareConfiguration
//purpose  : 
//=======================================================================
static Standard_Integer CompareConfiguration(Draw_Interpretor& theDI,
                                             Standard_Integer theNbArgs,
                                             const char** theArgVec)
{
  if (theNbArgs > 5)
  {
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }
  Handle(DE_ConfigurationContext) aResourceFirst = new DE_ConfigurationContext();
  if (!aResourceFirst->Load(theArgVec[1]))
  {
    Message::SendFail() << "Error: Can't load first configuration";
    return 1;
  }
  Handle(DE_ConfigurationContext) aResourceSecond = new DE_ConfigurationContext();
  if (!aResourceSecond->Load(theArgVec[2]))
  {
    Message::SendFail() << "Error: Can't load second configuration";
    return 1;
  }
  const DE_ResourceMap& aResourceMapFirst = aResourceFirst->GetInternalMap();
  const DE_ResourceMap& aResourceMapSecond = aResourceSecond->GetInternalMap();
  Standard_Integer anDiffers = 0;
  for (DE_ResourceMap::Iterator anOrigIt(aResourceMapFirst);
       anOrigIt.More(); anOrigIt.Next())
  {
    const TCollection_AsciiString& anOrigValue = anOrigIt.Value();
    const TCollection_AsciiString& anOrigKey = anOrigIt.Key();
    TCollection_AsciiString aCompValue;
    if (!aResourceMapSecond.Find(anOrigKey, aCompValue))
    {
      Message::SendWarning() << "Second configuration don't have the next scope : " << anOrigKey;
      anDiffers++;
    }
    if (!aCompValue.IsEqual(anOrigValue))
    {
      Message::SendWarning() << "Configurations have differs value with the next scope :" << anOrigKey
        << " First value : " << anOrigValue << " Second value : " << aCompValue;
      anDiffers++;
    }
  }
  TCollection_AsciiString aMessage;
  if (aResourceMapFirst.Extent() != aResourceMapSecond.Extent() || anDiffers > 0)
  {
    Message::SendFail() << "Error: Configurations are not same : " << " Differs count : " << anDiffers << " Count of first's scopes : " << aResourceMapFirst.Extent()
      << " Count of second's scopes : " << aResourceMapSecond.Extent();
    return 1;
  }
  return 0;
}

//=======================================================================
//function : LoadConfiguration
//purpose  : 
//=======================================================================
static Standard_Integer LoadConfiguration(Draw_Interpretor& theDI,
                                          Standard_Integer theNbArgs,
                                          const char** theArgVec)
{
  if (theNbArgs > 4)
  {
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }
  Handle(DE_Wrapper) aConf = DE_Wrapper::GlobalWrapper();
  TCollection_AsciiString aString = theArgVec[1];
  Standard_Boolean aIsRecursive = Standard_True;
  if (theNbArgs == 4)
  {
    TCollection_AsciiString anArg = theArgVec[2];
    anArg.LowerCase();
    if (!(anArg == "-recursive") ||
        !Draw::ParseOnOff(theArgVec[3], aIsRecursive))
    {
      Message::SendFail() << "Syntax error at argument '" << theArgVec[3] << "'";
      return 1;
    }
  }
  if (!aConf->Load(aString, aIsRecursive))
  {
    Message::SendFail() << "Error: configuration is incorrect";
    return 1;
  }
  return 0;
}

//=======================================================================
//function : ReadFile
//purpose  : 
//=======================================================================
static Standard_Integer ReadFile(Draw_Interpretor& theDI,
                                 Standard_Integer theNbArgs,
                                 const char** theArgVec)
{
  if (theNbArgs > 6)
  {
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }
  TCollection_AsciiString aDocShapeName;
  TCollection_AsciiString aFilePath;
  Handle(TDocStd_Document) aDoc;
  Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
  TCollection_AsciiString aConfString;
  Standard_Boolean isNoDoc = (TCollection_AsciiString(theArgVec[0]) == "readfile");
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if ((anArg == "-conf") &&
        (anArgIter + 1 < theNbArgs))
    {
      ++anArgIter;
      aConfString = theArgVec[anArgIter];
    }
    else if (aDocShapeName.IsEmpty())
    {
      aDocShapeName = theArgVec[anArgIter];
      Standard_CString aNameVar = aDocShapeName.ToCString();
      if(!isNoDoc)
      {
        DDocStd::GetDocument(aNameVar, aDoc, Standard_False);
      }
    }
    else if (aFilePath.IsEmpty())
    {
      aFilePath = theArgVec[anArgIter];
    }
    else
    {
      Message::SendFail() << "Syntax error at argument '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }
  if (aDocShapeName.IsEmpty() || aFilePath.IsEmpty())
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }
  if (aDoc.IsNull() && !isNoDoc)
  {
    anApp->NewDocument(TCollection_ExtendedString("BinXCAF"), aDoc);
    Handle(DDocStd_DrawDocument) aDrawDoc = new DDocStd_DrawDocument(aDoc);
    TDataStd_Name::Set(aDoc->GetData()->Root(), theArgVec[1]);
    Draw::Set(theArgVec[1], aDrawDoc);
  }

  Handle(DE_Wrapper) aConf = DE_Wrapper::GlobalWrapper()->Copy();
  Standard_Boolean aStat = Standard_True;
  if (!aConfString.IsEmpty())
  {
    aStat = aConf->Load(aConfString);
  }
  if (aStat)
  {
    TopoDS_Shape aShape;
    aStat = isNoDoc ? aConf->Read(aFilePath, aShape) : aConf->Read(aFilePath, aDoc);
    if(isNoDoc && aStat)
    {
      DBRep::Set(aDocShapeName.ToCString(), aShape);
    }
  }
  if (!aStat)
  {
    return 1;
  }
  return 0;
}

//=======================================================================
//function : WriteFile
//purpose  : 
//=======================================================================
static Standard_Integer WriteFile(Draw_Interpretor& theDI,
                                  Standard_Integer theNbArgs,
                                  const char** theArgVec)
{
  if (theNbArgs > 6)
  {
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }
  TCollection_AsciiString aDocShapeName;
  TCollection_AsciiString aFilePath;
  Handle(TDocStd_Document) aDoc;
  Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
  TCollection_AsciiString aConfString;
  Standard_Boolean isNoDoc = (TCollection_AsciiString(theArgVec[0]) == "writefile");
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if ((anArg == "-conf") &&
        (anArgIter + 1 < theNbArgs))
    {
      ++anArgIter;
      aConfString = theArgVec[anArgIter];
    }
    else if (aDocShapeName.IsEmpty())
    {
      aDocShapeName = theArgVec[anArgIter];
      Standard_CString aNameVar = aDocShapeName.ToCString();
      if (!isNoDoc)
      {
        DDocStd::GetDocument(aNameVar, aDoc, Standard_False);
      }
    }
    else if (aFilePath.IsEmpty())
    {
      aFilePath = theArgVec[anArgIter];
    }
    else
    {
      Message::SendFail() << "Syntax error at argument '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }
  if (aDocShapeName.IsEmpty() || aFilePath.IsEmpty())
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }
  if (aDoc.IsNull() && !isNoDoc)
  {
    Message::SendFail() << "Error: incorrect document";
  }
  Handle(DE_Wrapper) aConf = DE_Wrapper::GlobalWrapper()->Copy();
  Standard_Boolean aStat = Standard_True;
  if (!aConfString.IsEmpty())
  {
    aStat = aConf->Load(aConfString);
  }
  if (aStat)
  {
    if(isNoDoc)
    {
      TopoDS_Shape aShape = DBRep::Get(aDocShapeName);
      if(aShape.IsNull())
      {
        Message::SendFail() << "Error: incorrect shape";
        return 1;
      }
      aStat = aConf->Write(aFilePath, aShape);
    }
    else
    {
      aStat = aConf->Write(aFilePath, aDoc);
    }
  }
  if (!aStat)
  {
    return 1;
  }
  return 0;
}

void XDEDRAW_Common::InitCommands(Draw_Interpretor& di)
{
  static Standard_Boolean initactor = Standard_False;
  if (initactor)
  {
    return;
  }
  initactor = Standard_True;

  Standard_CString g = "XDE translation commands";

  di.Add("ReadIges", "Doc filename: Read IGES file to DECAF document", __FILE__, ReadIges, g);
  di.Add("WriteIges", "Doc filename: Write DECAF document to IGES file", __FILE__, WriteIges, g);
  di.Add("ReadStep",
         "Doc filename [mode] [-stream]"
         "\n\t\t: Read STEP file to a document."
         "\n\t\t:  -stream read using istream reading interface (testing)",
         __FILE__, ReadStep, g);
  di.Add("WriteStep" ,
         "Doc filename [mode=a [multifile_prefix] [label]] [-stream]"
         "\n\t\t: Write DECAF document to STEP file"
         "\n\t\t:   mode can be: a or 0 : AsIs (default)"
         "\n\t\t:                f or 1 : FacettedBRep        s or 2 : ShellBasedSurfaceModel"
         "\n\t\t:                m or 3 : ManifoldSolidBrep   w or 4 : GeometricCurveSet/WireFrame"
         "\n\t\t:   multifile_prefix: triggers writing assembly components as separate files,"
         "\n\t\t:                     and defines common prefix for their names"
         "\n\t\t:   label  tag of the sub-assembly label to save only that sub-assembly"
         "\n\t\t:  -stream read using ostream writing interface (testing)",
         __FILE__, WriteStep, g);

  di.Add("XFileList", "Print list of files that was transferred by the last transfer", __FILE__, GetDicWSList, g);
  di.Add("XFileCur", ": returns name of file which is set as current", __FILE__, GetCurWS, g);
  di.Add("XFileSet", "filename: Set the specified file to be the current one", __FILE__, SetCurWS, g);
  di.Add("XFromShape", "shape: do fromshape command for all the files", __FILE__, FromShape, g);

  di.Add("XExpand", "XExpand Doc recursively(0/1) or XExpand Doc recursively(0/1) label1 label2 ..."
         "or XExpand Doc recursively(0/1) shape1 shape2 ...", __FILE__, Expand, g);
  di.Add("XExtract", "XExtract dstDoc [dstAssmblSh] srcDoc srcLabel1 srcLabel2 ...\t"
         "Extracts given srcLabel1 srcLabel2 ... from srcDoc into given Doc or assembly shape",
         __FILE__, Extract, g);

  di.Add("ReadVrml",
         "ReadVrml docName filePath [-fileCoordSys {Zup|Yup}] [-fileUnit Unit]"
         "\n\t\t:                   [-systemCoordSys {Zup|Yup}] [-noCreateDoc] [-fillIncomplete {ON|OFF}]"
         "\n\t\t: Read Vrml file into XDE document."
         "\n\t\t:   -fileCoordSys   coordinate system defined by Vrml file; Yup when not specified."
         "\n\t\t:   -fileUnit       length unit of Vrml file content."
         "\n\t\t:   -systemCoordSys result coordinate system; Zup when not specified."
         "\n\t\t:   -noCreateDoc    read into existing XDE document."
         "\n\t\t:   -fillIncomplete fill the document with partially retrieved data even if reader has failed with "
         "error; true when not specified", 
         __FILE__, ReadVrml, g);
  di.Add("WriteVrml",
         "WriteVrml Doc filename [version VRML#1.0/VRML#2.0 (1/2): 2 by default] [representation shaded/wireframe/both (0/1/2): 0 by default]",
         __FILE__, WriteVrml, g);

  di.Add("DumpConfiguration",
         "DumpConfiguration [-path <path>] [-recursive {on|off}] [-format fmt1 fmt2 ...] [-vendor vend1 vend2 ...]\n"
         "\n\t\t: Dump special resource generated from global configuration."
         "\n\t\t:   '-path' - save resource configuration to the file"
         "\n\t\t:   '-recursive' - flag to generate a resource from providers. Default is On. Off disables other options"
         "\n\t\t:   '-format' - flag to generate a resource for choosen formats. If list is empty, generate it for all"
         "\n\t\t:   '-vendor' - flag to generate a resource for choosen vendors. If list is empty, generate it for all",
         __FILE__, DumpConfiguration, g);
  di.Add("LoadConfiguration",
         "LoadConfiguration conf [-recursive {on|off}]\n"
         "\n\t\t:   'conf' - path to the resouce file or string value in the special format"
         "\n\t\t:   '-recursive' - flag to generate a resource for all providers. Default is true"
         "\n\t\t: Configure global configuration according special resource",
         __FILE__, LoadConfiguration, g);
  di.Add("CompareConfiguration",
         "CompareConfiguration conf1 conf2\n"
         "\n\t\t:   'conf1' - path to the first resouce file or string value in the special format"
         "\n\t\t:   'conf2' - path to the second resouce file or string value in the special format"
         "\n\t\t: Compare two configurations",
         __FILE__, CompareConfiguration, g);
  di.Add("ReadFile",
         "ReadFile docName filePath [-conf <value|path>]\n"
         "\n\t\t: Read CAD file to document with registered format's providers. Use global configuration by default.",
         __FILE__, ReadFile, g);
  di.Add("readfile",
         "readfile shapeName filePath [-conf <value|path>]\n"
         "\n\t\t: Read CAD file to shape with registered format's providers. Use global configuration by default.",
         __FILE__, ReadFile, g);
  di.Add("WriteFile",
         "WriteFile docName filePath [-conf <value|path>]\n"
         "\n\t\t: Write CAD file to document with registered format's providers. Use global configuration by default.",
         __FILE__, WriteFile, g);
  di.Add("writefile",
         "writefile shapeName filePath [-conf <value|path>]\n"
         "\n\t\t: Write CAD file to shape with registered format's providers. Use global configuration by default.",
         __FILE__, WriteFile, g);
}
