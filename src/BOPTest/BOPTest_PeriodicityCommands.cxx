// Created on: 03/19/2018
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

#include <BOPTest.hxx>

#include <BOPAlgo_MakePeriodic.hxx>

#include <BOPTest_DrawableShape.hxx>
#include <BOPTest_Objects.hxx>

#include <BRep_Builder.hxx>

#include <BRepTest_Objects.hxx>

#include <DBRep.hxx>
#include <Draw.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

static Standard_Integer MakePeriodic(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer GetTwins(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer RepeatShape(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer ClearRepetitions(Draw_Interpretor&, Standard_Integer, const char**);

namespace
{
  static BOPAlgo_MakePeriodic ThePeriodicityMaker;
}

//=======================================================================
//function : PeriodicityCommands
//purpose  : 
//=======================================================================
void BOPTest::PeriodicityCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  // Chapter's name
  const char* group = "BOPTest commands";
  // Commands
  theCommands.Add("makeperiodic", "makeperiodic result shape [-x/y/z period [-trim first]]\n"
                  "\t\tMake the shape periodic in the required directions.\n"
                  "\t\tresult        - resulting periodic shape;\n"
                  "\t\t-x/y/z period - option to make the shape periodic in X, Y or Z\n "
                  "\t\t                direction with the given period;\n"
                  "\t\t-trim first   - option to trim the shape to fit the required period,\n"
                  "\t\t                starting the period in first.",
                  __FILE__, MakePeriodic, group);

  theCommands.Add("periodictwins", "periodictwins twins shape\n"
                  "\t\tReturns the twins for the shape located on the opposite side of the periodic shape.",
                  __FILE__, GetTwins, group);

  // Repetition commands
  theCommands.Add("repeatshape", "repeatshape result -x/y/z times\n"
                  "\t\tRepeats the periodic shape in periodic directions required number of times.\n"
                  "\t\tresult       - resulting shape;\n"
                  "\t\t-x/y/z times - direction for repetition and number of repetitions.",
                  __FILE__, RepeatShape, group);

  theCommands.Add("clearrepetitions", "clearrepetitions [result]\n"
                  "\t\tClears all previous repetitions of the periodic shape.",
                  __FILE__, ClearRepetitions, group);
}

//=======================================================================
//function : MakePeriodic
//purpose  : 
//=======================================================================
Standard_Integer MakePeriodic(Draw_Interpretor& theDI,
                              Standard_Integer  theArgc,
                              const char ** theArgv)
{
  if (theArgc < 5)
  {
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  // Get the shape to make periodic
  TopoDS_Shape aShape = DBRep::Get(theArgv[2]);
  if (aShape.IsNull())
  {
    theDI << "Error: " << theArgv[2] << " is a null shape.\n";
    return 1;
  }

  ThePeriodicityMaker.Clear();
  ThePeriodicityMaker.SetShape(aShape);

  for (Standard_Integer i = 3; i < theArgc;)
  {
    // Get periodicity
    Standard_Integer iDir = i;

    Standard_Integer aDirID = -1;
    if (!strcasecmp(theArgv[i], "-x"))
      aDirID = 0;
    else if (!strcasecmp(theArgv[i], "-y"))
      aDirID = 1;
    else if (!strcasecmp(theArgv[i], "-z"))
      aDirID = 2;
    else
    {
      theDI << theArgv[i] << " - Invalid key\n";
      return 1;
    }

    char cDirName[2];
    sprintf(cDirName, "%c", theArgv[iDir][1]);

    if (theArgc == (i + 1))
    {
      theDI << "Period for " << cDirName << " direction is not set\n";
      return 1;
    }

    Standard_Real aPeriod = Draw::Atof(theArgv[++i]);

    ThePeriodicityMaker.MakePeriodic(aDirID, Standard_True, aPeriod);

    ++i;
    if (theArgc > i + 1)
    {
      // Check if trimming is necessary
      if (!strcmp(theArgv[i], "-trim"))
      {
        if (theArgc == (i + 1))
        {
          theDI << "Trim bounds for " << cDirName << " direction are not set\n";
          return 1;
        }
        Standard_Real aFirst = Draw::Atof(theArgv[++i]);

        ThePeriodicityMaker.SetTrimmed(aDirID, Standard_False, aFirst);
        ++i;
      }
    }
  }

  ThePeriodicityMaker.SetRunParallel(BOPTest_Objects::RunParallel());

  // Perform operation
  ThePeriodicityMaker.Perform();

  // Print Error/Warning messages
  BOPTest::ReportAlerts(ThePeriodicityMaker.GetReport());

  // Set the history of the operation in session
  BRepTest_Objects::SetHistory(ThePeriodicityMaker.History());

  if (ThePeriodicityMaker.HasErrors())
    return 0;

  // Draw the result shape
  const TopoDS_Shape& aResult = ThePeriodicityMaker.Shape();
  DBRep::Set(theArgv[1], aResult);

  return 0;
}

//=======================================================================
//function : GetTwin
//purpose  : 
//=======================================================================
Standard_Integer GetTwins(Draw_Interpretor& theDI,
                          Standard_Integer  theArgc,
                          const char ** theArgv)
{
  if (theArgc != 3)
  {
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  // Get the shape to find twins
  TopoDS_Shape aShape = DBRep::Get(theArgv[2]);
  if (aShape.IsNull())
  {
    theDI << "Error: " << theArgv[2] << " is a null shape.\n";
    return 1;
  }

  const TopTools_ListOfShape& aTwins = ThePeriodicityMaker.GetTwins(aShape);

  TopoDS_Shape aCTwins;
  if (aTwins.IsEmpty())
    theDI << "No twins for the shape.\n";
  else if (aTwins.Extent() == 1)
    aCTwins = aTwins.First();
  else
  {
    BRep_Builder().MakeCompound(TopoDS::Compound(aCTwins));
    for (TopTools_ListIteratorOfListOfShape it(aTwins); it.More(); it.Next())
      BRep_Builder().Add(aCTwins, it.Value());
  }

  DBRep::Set(theArgv[1], aCTwins);

  return 0;
}

//=======================================================================
//function : RepeatShape
//purpose  : 
//=======================================================================
Standard_Integer RepeatShape(Draw_Interpretor& theDI,
                             Standard_Integer  theArgc,
                             const char ** theArgv)
{
  if (theArgc < 4)
  {
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  for (Standard_Integer i = 2; i < theArgc; ++i)
  {
    Standard_Integer aDirID = -1;
    if (!strcasecmp(theArgv[i], "-x"))
      aDirID = 0;
    else if (!strcasecmp(theArgv[i], "-y"))
      aDirID = 1;
    else if (!strcasecmp(theArgv[i], "-z"))
      aDirID = 2;
    else
    {
      theDI << theArgv[i] << " - Invalid key\n";
      return 1;
    }

    char cDirName[2];
    sprintf(cDirName, "%c", theArgv[i][1]);

    Standard_Integer aTimes = 0;
    if (theArgc > i + 1)
      aTimes = Draw::Atoi(theArgv[++i]);

    if (aTimes == 0)
    {
      theDI << "Number of repetitions for " << cDirName << " direction is not set\n";
      return 1;
    }

    ThePeriodicityMaker.RepeatShape(aDirID, aTimes);
  }

  // Print Error/Warning messages
  BOPTest::ReportAlerts(ThePeriodicityMaker.GetReport());

  // Set the history of the operation in session
  BRepTest_Objects::SetHistory(ThePeriodicityMaker.History());

  if (ThePeriodicityMaker.HasErrors())
    return 0;

  // Draw the result shape
  const TopoDS_Shape& aResult = ThePeriodicityMaker.RepeatedShape();
  DBRep::Set(theArgv[1], aResult);

  return 0;
}

//=======================================================================
//function : ClearRepetitions
//purpose  : 
//=======================================================================
Standard_Integer ClearRepetitions(Draw_Interpretor&,
                                  Standard_Integer theArgc,
                                  const char **theArgv)
{
  // Clear all previous repetitions
  ThePeriodicityMaker.ClearRepetitions();

  // Set the history of the operation in session
  BRepTest_Objects::SetHistory(ThePeriodicityMaker.History());

  if (theArgc > 1)
    DBRep::Set(theArgv[1], ThePeriodicityMaker.Shape());

  return 0;
}
