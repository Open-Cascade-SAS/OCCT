// Created on: 04/02/2018
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

#include <BOPAlgo_MakeConnected.hxx>

#include <BOPTest_DrawableShape.hxx>
#include <BOPTest_Objects.hxx>

#include <BRep_Builder.hxx>

#include <BRepTest_Objects.hxx>

#include <DBRep.hxx>
#include <Draw.hxx>

#include <Precision.hxx>

#include <TopoDS.hxx>

static Standard_Integer MakeConnected(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer MakePeriodic(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer MaterialsOn(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer RepeatShape(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer GetTwins(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer ClearRepetitions(Draw_Interpretor&, Standard_Integer, const char**);

namespace
{
static BOPAlgo_MakeConnected& getMakeConnectedTool()
{
  static BOPAlgo_MakeConnected TheMakeConnectedTool;
  return TheMakeConnectedTool;
}
} // namespace

//=================================================================================================

void BOPTest::MkConnectedCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done)
    return;
  done = Standard_True;
  // Chapter's name
  const char* group = "BOPTest commands";
  // Commands
  theCommands.Add("makeconnected",
                  "makeconnected result shape1 shape2 ...\n"
                  "\t\tMake the given shapes connected (glued).",
                  __FILE__,
                  MakeConnected,
                  group);

  theCommands.Add("cmakeperiodic",
                  "cmakeperiodic result [-x/y/z period [-trim first]]\n"
                  "\t\tMake the connected shape periodic in the required directions.\n"
                  "\t\tresult        - resulting periodic shape;\n"
                  "\t\t-x/y/z period - option to make the shape periodic in X, Y or Z\n "
                  "\t\t                direction with the given period;\n"
                  "\t\t-trim first   - option to trim the shape to fit the required period,\n"
                  "\t\t                starting the period in first.",
                  __FILE__,
                  MakePeriodic,
                  group);

  theCommands.Add(
    "cmaterialson",
    "cmaterialson result +/- shape\n"
    "\t\tReturns the original shapes located on the required side of a shape:\n"
    "\t\t'+' - on a positive side of a shape (containing the shape with orientation FORWARD)\n"
    "\t\t'-' - on a negative side of a shape (containing the shape with orientation REVERSED).",
    __FILE__,
    MaterialsOn,
    group);

  theCommands.Add(
    "crepeatshape",
    "crepeatshape result -x/y/z times\n"
    "\t\tRepeats the periodic connected shape in periodic directions required number of times.\n"
    "\t\tresult       - resulting shape;\n"
    "\t\t-x/y/z times - direction for repetition and number of repetitions.",
    __FILE__,
    RepeatShape,
    group);

  theCommands.Add(
    "cperiodictwins",
    "cperiodictwins twins shape\n"
    "\t\tReturns the twins for the shape located on the opposite side of the periodic shape.",
    __FILE__,
    GetTwins,
    group);

  theCommands.Add("cclearrepetitions",
                  "cclearrepetitions [result]\n"
                  "\t\tClears all previous repetitions of the periodic shape.",
                  __FILE__,
                  ClearRepetitions,
                  group);
}

//=================================================================================================

Standard_Integer MakeConnected(Draw_Interpretor& theDI,
                               Standard_Integer  theArgc,
                               const char**      theArgv)
{
  if (theArgc < 3)
  {
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  getMakeConnectedTool().Clear();

  for (Standard_Integer i = 2; i < theArgc; ++i)
  {
    TopoDS_Shape aS = DBRep::Get(theArgv[i]);
    if (aS.IsNull())
    {
      theDI << "Error: " << theArgv[i] << " is a null shape. Skip it.\n";
      continue;
    }
    getMakeConnectedTool().AddArgument(aS);
  }

  getMakeConnectedTool().SetRunParallel(BOPTest_Objects::RunParallel());

  getMakeConnectedTool().Perform();

  // Print Error/Warning messages
  BOPTest::ReportAlerts(getMakeConnectedTool().GetReport());

  // Set the history of the operation in session
  BRepTest_Objects::SetHistory(getMakeConnectedTool().History());

  if (getMakeConnectedTool().HasErrors())
    return 0;

  // Draw the result shape
  const TopoDS_Shape& aResult = getMakeConnectedTool().Shape();
  DBRep::Set(theArgv[1], aResult);

  return 0;
}

//=================================================================================================

Standard_Integer MakePeriodic(Draw_Interpretor& theDI,
                              Standard_Integer  theArgc,
                              const char**      theArgv)
{
  if (theArgc < 4)
  {
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  if (getMakeConnectedTool().Shape().IsNull() || getMakeConnectedTool().HasErrors())
  {
    theDI << "Make the shapes connected first.\n";
    return 1;
  }

  BOPAlgo_MakePeriodic::PeriodicityParams aParams;

  for (Standard_Integer i = 2; i < theArgc;)
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

    Standard_Real aPeriod = 0;
    if (theArgc > i + 1)
      aPeriod = Draw::Atof(theArgv[++i]);

    if (aPeriod <= Precision::Confusion())
    {
      theDI << "Period for " << cDirName << " direction is not set\n";
      return 1;
    }

    aParams.myPeriodic[aDirID] = Standard_True;
    aParams.myPeriod[aDirID]   = aPeriod;

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

        aParams.myIsTrimmed[aDirID]   = Standard_False;
        aParams.myPeriodFirst[aDirID] = aFirst;
        ++i;
      }
    }
  }

  getMakeConnectedTool().MakePeriodic(aParams);

  // Print Error/Warning messages
  BOPTest::ReportAlerts(getMakeConnectedTool().GetReport());

  // Set the history of the operation in session
  BRepTest_Objects::SetHistory(getMakeConnectedTool().History());

  if (getMakeConnectedTool().HasErrors())
    return 0;

  // Draw the result shape
  const TopoDS_Shape& aResult = getMakeConnectedTool().PeriodicShape();
  DBRep::Set(theArgv[1], aResult);

  return 0;
}

//=================================================================================================

Standard_Integer RepeatShape(Draw_Interpretor& theDI,
                             Standard_Integer  theArgc,
                             const char**      theArgv)
{
  if (theArgc < 4)
  {
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  if (getMakeConnectedTool().PeriodicityTool().HasErrors())
  {
    theDI << "The shapes have not been made periodic yet.\n";
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

    getMakeConnectedTool().RepeatShape(aDirID, aTimes);
  }

  // Print Error/Warning messages
  BOPTest::ReportAlerts(getMakeConnectedTool().GetReport());

  // Set the history of the operation in session
  BRepTest_Objects::SetHistory(getMakeConnectedTool().History());

  if (getMakeConnectedTool().HasErrors())
    return 0;

  // Draw the result shape
  const TopoDS_Shape& aResult = getMakeConnectedTool().PeriodicShape();
  DBRep::Set(theArgv[1], aResult);

  return 0;
}

//=================================================================================================

Standard_Integer MaterialsOn(Draw_Interpretor& theDI,
                             Standard_Integer  theArgc,
                             const char**      theArgv)
{
  if (theArgc != 4)
  {
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  // Get the shape to get materials
  TopoDS_Shape aShape = DBRep::Get(theArgv[3]);
  if (aShape.IsNull())
  {
    theDI << "Error: " << theArgv[3] << " is a null shape.\n";
    return 1;
  }

  // Get the sign of a shape
  Standard_Boolean bPositive;

  if (!strcmp("+", theArgv[2]))
    bPositive = Standard_True;
  else if (!strcmp("-", theArgv[2]))
    bPositive = Standard_False;
  else
  {
    theDI << theArgv[2] << " - invalid key.\n";
    return 1;
  }

  const TopTools_ListOfShape& aLS = bPositive
                                      ? getMakeConnectedTool().MaterialsOnPositiveSide(aShape)
                                      : getMakeConnectedTool().MaterialsOnNegativeSide(aShape);

  TopoDS_Shape aResult;
  if (aLS.IsEmpty())
    theDI << "No materials on this side.\n";
  else if (aLS.Extent() == 1)
    aResult = aLS.First();
  else
  {
    BRep_Builder().MakeCompound(TopoDS::Compound(aResult));
    for (TopTools_ListIteratorOfListOfShape it(aLS); it.More(); it.Next())
      BRep_Builder().Add(aResult, it.Value());
  }

  DBRep::Set(theArgv[1], aResult);

  return 0;
}

//=================================================================================================

Standard_Integer GetTwins(Draw_Interpretor& theDI, Standard_Integer theArgc, const char** theArgv)
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

  const TopTools_ListOfShape& aTwins = getMakeConnectedTool().PeriodicityTool().GetTwins(aShape);

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

//=================================================================================================

Standard_Integer ClearRepetitions(Draw_Interpretor&, Standard_Integer theArgc, const char** theArgv)
{
  // Clear all previous repetitions
  getMakeConnectedTool().ClearRepetitions();

  // Set the history of the operation in session
  BRepTest_Objects::SetHistory(getMakeConnectedTool().History());

  if (theArgc > 1)
  {
    DBRep::Set(theArgv[1], getMakeConnectedTool().PeriodicShape());
  }

  return 0;
}
