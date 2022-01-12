// Created on: 2018/03/21
// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2018 OPEN CASCADE SAS
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

#include <BRepTest.hxx>

#include <BRep_Builder.hxx>

#include <BRepTest_DrawableHistory.hxx>
#include <BRepTest_Objects.hxx>

#include <Draw.hxx>
#include <DBRep.hxx>

#include <TopoDS.hxx>

static Standard_Integer SetFillHistory(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer SaveHistory   (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer Modified      (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer Generated     (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer IsDeleted     (Draw_Interpretor&, Standard_Integer, const char**);

//=======================================================================
//function : HistoryCommands
//purpose  : 
//=======================================================================
void BRepTest::HistoryCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean isDone = Standard_False;
  if (isDone) return;
  isDone = Standard_True;
  // Chapter's name
  const char* group = "History commands";

  // Commands
  theCommands.Add("setfillhistory" , "Controls the history collection by the algorithms and its saving into the session after algorithm is done.\n"
                  "\t\tUsage: setfillhistory [flag]\n"
                  "\t\tw/o arguments prints the current state of the option;\n"
                  "\t\tflag == 0 - history will not be collected and saved;\n"
                  "\t\tflag != 0 - history will be collected and saved into the session (default).",
                  __FILE__, SetFillHistory , group);

  theCommands.Add("savehistory" , "savehistory name\n"
                  "\t\tSaves the history from the session into a drawable object with the name <name>.",
                  __FILE__, SaveHistory , group);

  theCommands.Add("modified" , "modified modified_shapes history shape\n"
                  "\t\tReturns the shapes Modified from the given shape in the given history",
                  __FILE__, Modified , group);

  theCommands.Add("generated", "generated generated_shapes history shape\n"
                  "\t\tReturns the shapes Generated from the given shape in the given history",
                  __FILE__, Generated, group);

  theCommands.Add("isdeleted", "isdeleted history shape\n"
                  "\t\tChecks if the given shape has been deleted in the given history",
                  __FILE__, IsDeleted, group);
}

//=======================================================================
//function : SetFillHistory
//purpose  : 
//=======================================================================
Standard_Integer SetFillHistory(Draw_Interpretor& theDI,
                                Standard_Integer theArgc,
                                const char** theArgv)
{
  if (theArgc > 2)
  {
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  if (theArgc == 1)
  {
    theDI << "Filling of the history is " <<
      (BRepTest_Objects::IsHistoryNeeded() ? "enabled." : "disabled.");
  }
  else
  {
    Standard_Integer iHist = Draw::Atoi(theArgv[1]);
    BRepTest_Objects::SetToFillHistory(iHist != 0);
  }
  return 0;
}

//=======================================================================
//function : SaveHistory
//purpose  : 
//=======================================================================
Standard_Integer SaveHistory(Draw_Interpretor& theDI,
                             Standard_Integer theArgc,
                             const char** theArgv)
{
  if (theArgc != 2)
  {
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  // Get the history from the session
  Handle(BRepTools_History) aHistory = BRepTest_Objects::History();
  if (aHistory.IsNull())
  {
    theDI << "No history has been prepared yet.";
    return 1;
  }

  Handle(BRepTest_DrawableHistory) aDrawHist = new BRepTest_DrawableHistory(aHistory);

  Draw::Set(theArgv[1], aDrawHist);

  return 0;
}

//=======================================================================
//function : GetHistory
//purpose  : 
//=======================================================================
static Handle(BRepTools_History) GetHistory(Draw_Interpretor& theDI,
                                            Standard_CString theName)
{
  Handle(BRepTest_DrawableHistory) aHistory =
    Handle(BRepTest_DrawableHistory)::DownCast(Draw::Get(theName));

  if (aHistory.IsNull() || aHistory->History().IsNull())
  {
    theDI << "History with the name " << theName << " does not exist.";
    return NULL;
  }

  return aHistory->History();
}

//=======================================================================
//function : GetShape
//purpose  : 
//=======================================================================
static TopoDS_Shape GetShape(Draw_Interpretor& theDI,
                             Standard_CString theName)
{
  TopoDS_Shape aS = DBRep::Get(theName);

  if (aS.IsNull())
  {
    theDI << theName << " is a null shape.";
    return TopoDS_Shape();
  }

  if (!BRepTools_History::IsSupportedType(aS))
  {
    theDI << "History is not supported for this kind of shape.";
    return TopoDS_Shape();
  }
  return aS;
}

//=======================================================================
//function : MakeCompound
//purpose  : 
//=======================================================================
static TopoDS_Shape MakeCompound(const TopTools_ListOfShape& theLS)
{
  TopoDS_Shape aC;
  if (theLS.Extent() == 1)
    aC = theLS.First();
  else
  {
    BRep_Builder().MakeCompound(TopoDS::Compound(aC));
    TopTools_ListIteratorOfListOfShape it(theLS);
    for (; it.More(); it.Next())
      BRep_Builder().Add(aC, it.Value());
  }
  return aC;
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================
Standard_Integer Modified(Draw_Interpretor& theDI,
                          Standard_Integer theArgc,
                          const char** theArgv)
{
  if (theArgc != 4)
  {
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  Handle(BRepTools_History) aHistory = GetHistory(theDI, theArgv[2]);
  if (aHistory.IsNull())
    return 1;

  TopoDS_Shape aS = GetShape(theDI, theArgv[3]);
  if (aS.IsNull())
    return 1;

  const TopTools_ListOfShape& aModified = aHistory->Modified(aS);

  if (aModified.IsEmpty())
  {
    theDI << "The shape has not been modified.";
    return 0;
  }

  DBRep::Set(theArgv[1], MakeCompound(aModified));

  return 0;
}

//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================
Standard_Integer Generated(Draw_Interpretor& theDI,
                           Standard_Integer theArgc,
                           const char** theArgv)
{
  if (theArgc != 4)
  {
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  Handle(BRepTools_History) aHistory = GetHistory(theDI, theArgv[2]);
  if (aHistory.IsNull())
    return 1;

  TopoDS_Shape aS = GetShape(theDI, theArgv[3]);
  if (aS.IsNull())
    return 1;

  const TopTools_ListOfShape& aGenerated = aHistory->Generated(aS);

  if (aGenerated.IsEmpty())
  {
    theDI << "No shapes were generated from the shape.";
    return 0;
  }

  DBRep::Set(theArgv[1], MakeCompound(aGenerated));

  return 0;
}

//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================
Standard_Integer IsDeleted(Draw_Interpretor& theDI,
                           Standard_Integer theArgc,
                           const char** theArgv)
{
  if (theArgc != 3)
  {
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  Handle(BRepTools_History) aHistory = GetHistory(theDI, theArgv[1]);
  if (aHistory.IsNull())
    return 1;

  TopoDS_Shape aS = GetShape(theDI, theArgv[2]);
  if (aS.IsNull())
    return 1;

  theDI << (aHistory->IsRemoved(aS) ? "Deleted." : "Not deleted.");

  return 0;
}
