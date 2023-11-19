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

#include <XDEDRAW_Common.hxx>

#include <DBRep.hxx>
#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Message.hxx>
#include <OSD_OpenFile.hxx>
#include <OSD_Path.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS_Shape.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_Editor.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XSDRAW.hxx>

//=======================================================================
//function : SetCurWS
//purpose  : Set current file if many files are read
//=======================================================================
static Standard_Integer SetCurWS(Draw_Interpretor& theDI,
                                 Standard_Integer theNbArgs,
                                 const char** theArgVec)
{
  if (theNbArgs < 2)
  {
    theDI << "Use: " << theArgVec[0] << " filename \n";
    return 1;
  }
  const TCollection_AsciiString aSessionName(theArgVec[1]);
  Handle(XSControl_WorkSession) aSession;
  const XSControl_WorkSessionMap& aWSList = XSDRAW::WorkSessionList();
  if (!aWSList.Find(aSessionName, aSession))
  {
    TCollection_AsciiString aWSs;
    for (XSControl_WorkSessionMap::Iterator anIter(aWSList);
         anIter.More(); anIter.Next())
    {
      aWSs += "\"";
      aWSs += anIter.Key();
      aWSs += "\"\n";
    }
    theDI << "Error: Can't find active session. Active sessions list:\n" << aWSs;
    return 1;
  }
  XSDRAW::SetSession(aSession);
  return 0;
}

//=======================================================================
//function : GetDicWSList
//purpose  : List all files recorded after translation
//=======================================================================
static Standard_Integer GetDicWSList(Draw_Interpretor& theDI,
                                     Standard_Integer theNbArgs,
                                     const char** theArgVec)
{
  (void)theNbArgs;
  (void)theArgVec;
  Message::SendInfo() << "Active sessions list:";
  TCollection_AsciiString aWSs;
  for (XSControl_WorkSessionMap::Iterator anIter(XSDRAW::WorkSessionList());
       anIter.More(); anIter.Next())
  {
    theDI << "\"" << anIter.Key() << "\"\n";
  }
  return 0;
}

//=======================================================================
//function : GetCurWS
//purpose  : Return name of file which is current
//=======================================================================
static Standard_Integer GetCurWS(Draw_Interpretor& theDI,
                                 Standard_Integer theNbArgs,
                                 const char** theArgVec)
{
  (void)theNbArgs;
  (void)theArgVec;
  Handle(XSControl_WorkSession) WS = XSDRAW::Session();
  theDI << "\"" << WS->LoadedFile() << "\"";
  return 0;
}

//=======================================================================
//function : FromShape
//purpose  : Apply fromshape command to all the loaded WSs
//=======================================================================
static Standard_Integer FromShape(Draw_Interpretor& theDI,
                                  Standard_Integer theNbArgs,
                                  const char** theArgVec)
{
  if (theNbArgs < 2)
  {
    theDI << theArgVec[0] << " shape: search for shape origin among all last tranalated files\n";
    return 0;
  }

  char command[256];
  Sprintf(command, "fromshape %.200s -1", theArgVec[1]);
  const XSControl_WorkSessionMap& DictWS = XSDRAW::WorkSessionList();
  if (DictWS.IsEmpty())
    return theDI.Eval(command);

  Handle(XSControl_WorkSession) aWS = XSDRAW::Session();
  for (XSControl_WorkSessionMap::Iterator DicIt(DictWS);
       DicIt.More(); DicIt.Next())
  {
    Handle(XSControl_WorkSession) CurrentWS = Handle(XSControl_WorkSession)::DownCast(DicIt.Value());
    if (!CurrentWS.IsNull())
    {
      XSDRAW::SetSession(CurrentWS);
      theDI.Eval(command);
    }
  }
  XSDRAW::SetSession(aWS);
  return 0;
}

//=======================================================================
//function : Expand
//purpose  :
//=======================================================================
static Standard_Integer Expand(Draw_Interpretor& theDI,
                               Standard_Integer theNbArgs,
                               const char** theArgVec)
{
  if (theNbArgs < 3)
  {
    theDI << "Use: " << theArgVec[0]
      << " Doc recurs(0/1) or Doc recurs(0/1) label1 label2 ... or Doc recurs(0/1 shape1 shape2 ...\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(theArgVec[1], Doc);
  if (Doc.IsNull())
  {
    theDI << theArgVec[1] << " is not a document\n"; return 1;
  }

  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  Standard_Boolean recurs = Standard_False;
  if (atoi(theArgVec[2]) != 0)
    recurs = Standard_True;

  if (theNbArgs == 3)
  {
    if (!XCAFDoc_Editor::Expand(Doc->Main(), recurs))
    {
      theDI << "No suitable labels to expand\n";
      return 1;
    }
  }
  else
  {
    for (Standard_Integer i = 3; i < theNbArgs; i++)
    {
      TDF_Label aLabel;
      TDF_Tool::Label(Doc->GetData(), theArgVec[i], aLabel);
      if (aLabel.IsNull())
      {
        TopoDS_Shape aShape;
        aShape = DBRep::Get(theArgVec[i]);
        aLabel = aShapeTool->FindShape(aShape);
      }

      if (!aLabel.IsNull())
      {
        if (!XCAFDoc_Editor::Expand(Doc->Main(), aLabel, recurs))
        {
          theDI << "The shape is assembly or not compound\n";
          return 1;
        }
      }
      else
      {
        theDI << theArgVec[i] << " is not a shape\n"; return 1;
      }
    }
  }
  return 0;
}

//=======================================================================
//function : Extract
//purpose  :
//=======================================================================
static Standard_Integer Extract(Draw_Interpretor& theDI,
                                Standard_Integer theNbArgs,
                                const char** theArgVec)
{
  if (theNbArgs < 4)
  {
    theDI << "Use: " << theArgVec[0] << "dstDoc [dstAssmblSh] srcDoc srcLabel1 srcLabel2 ...\n";
    return 1;
  }

  Handle(TDocStd_Document) aSrcDoc, aDstDoc;
  DDocStd::GetDocument(theArgVec[1], aDstDoc);
  if (aDstDoc.IsNull())
  {
    theDI << "Error " << theArgVec[1] << " is not a document\n";
    return 1;
  }
  TDF_Label aDstLabel;
  Standard_Integer anArgInd = 3;
  TDF_Tool::Label(aDstDoc->GetData(), theArgVec[2], aDstLabel);
  Handle(XCAFDoc_ShapeTool) aDstShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDstDoc->Main());
  if (aDstLabel.IsNull())
  {
    aDstLabel = aDstShapeTool->Label();
    anArgInd = 2; // to get Src Doc
  }
  DDocStd::GetDocument(theArgVec[anArgInd++], aSrcDoc);
  if (aSrcDoc.IsNull())
  {
    theDI << "Error " << theArgVec[anArgInd] << " is not a document\n";
    return 1;
  }

  TDF_LabelSequence aSrcShapes;
  for (; anArgInd < theNbArgs; anArgInd++)
  {
    TDF_Label aSrcLabel;
    TDF_Tool::Label(aSrcDoc->GetData(), theArgVec[anArgInd], aSrcLabel);
    if (aSrcLabel.IsNull())
    {
      theDI << "[" << theArgVec[anArgInd] << "] is not valid Src label\n";
      return 1;
    }
    aSrcShapes.Append(aSrcLabel);
  }
  if (aSrcShapes.IsEmpty())
  {
    theDI << "Error: No Shapes to extract\n";
    return 1;
  }

  if (!XCAFDoc_Editor::Extract(aSrcShapes, aDstLabel))
  {
    theDI << "Error: Cannot extract labels\n";
    return 1;
  }
  return 0;
}

//=======================================================================
//function : InitCommands
//purpose  :
//=======================================================================
void XDEDRAW_Common::InitCommands(Draw_Interpretor& theDI)
{
  static Standard_Boolean aIsActivated = Standard_False;
  if (aIsActivated)
  {
    return;
  }
  aIsActivated = Standard_True;

  Standard_CString aGroup = "XDE translation commands";

  theDI.Add("XFileList", "Print list of files that was transferred by the last transfer", __FILE__, GetDicWSList, aGroup);
  theDI.Add("XFileCur", ": returns name of file which is set as current", __FILE__, GetCurWS, aGroup);
  theDI.Add("XFileSet", "filename: Set the specified file to be the current one", __FILE__, SetCurWS, aGroup);
  theDI.Add("XFromShape", "shape: do fromshape command for all the files", __FILE__, FromShape, aGroup);

  theDI.Add("XExpand", "XExpand Doc recursively(0/1) or XExpand Doc recursively(0/1) label1 label2 ..."
            "or XExpand Doc recursively(0/1) shape1 shape2 ...", __FILE__, Expand, aGroup);
  theDI.Add("XExtract", "XExtract dstDoc [dstAssmblSh] srcDoc srcLabel1 srcLabel2 ...\t"
            "Extracts given srcLabel1 srcLabel2 ... from srcDoc into given Doc or assembly shape",
            __FILE__, Extract, aGroup);

  // Load XSDRAW session for pilot activation
  XSDRAW::LoadDraw(theDI);
}
