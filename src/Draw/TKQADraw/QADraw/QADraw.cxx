// Created on: 2002-02-04
// Created by: QA Admin
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#include <QADraw.hxx>

#include <QABugs.hxx>
#include <QADNaming.hxx>

#include <AIS_InteractiveContext.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>
#include <OSD_Timer.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_XYZ.hxx>

#include <stdio.h>

//=================================================================================================

static int QATestExtremaSS(Draw_Interpretor& theInterpretor, int theArgNb, const char** theArgs)
{
  if (theArgNb < 3 || theArgNb > 4)
  {
    std::cerr << "Usage: type help " << theArgs[0] << std::endl;
    return 1;
  }

  // Get target shape
  TopoDS_Shape aShape = DBRep::Get(theArgs[1]);
  if (aShape.IsNull())
  {
    std::cerr << "Error: " << theArgs[1] << " shape is null\n";
    return 1;
  }

  // Get step value
  const double aStep = Draw::Atof(theArgs[2]);
  if (aStep <= 1e-5)
  {
    std::cerr << "Error: Step " << aStep << " is too small\n";
    return 1;
  }

  Extrema_ExtFlag aFlag = Extrema_ExtFlag_MIN;
  if (theArgNb > 3)
  {
    int aVal = Draw::Atoi(theArgs[3]);
    if (aVal > 0)
    {
      aFlag = aVal == 1 ? Extrema_ExtFlag_MAX : Extrema_ExtFlag_MINMAX;
    }
  }

  // Get bounding box of the shape
  Bnd_Box aBounds;
  BRepBndLib::Add(aShape, aBounds);

  double aXmin, aYmin, aZmin;
  double aXmax, aYmax, aZmax;
  aBounds.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  const double aScaleFactor = 1.5;
  aXmin *= aScaleFactor;
  aYmin *= aScaleFactor;
  aZmin *= aScaleFactor;
  aXmax *= aScaleFactor;
  aYmax *= aScaleFactor;
  aZmax *= aScaleFactor;

  NCollection_Sequence<TopoDS_Shape> aList;
  NCollection_Sequence<gp_XYZ>       aPoints;
  for (double aX = aXmin + 0.5 * aStep; aX < aXmax; aX += aStep)
  {
    for (double aY = aYmin + 0.5 * aStep; aY < aYmax; aY += aStep)
    {
      aList.Append(BRepBuilderAPI_MakeVertex(gp_Pnt(aX, aY, aZmin)));
      aList.Append(BRepBuilderAPI_MakeVertex(gp_Pnt(aX, aY, aZmax)));

      aPoints.Append(gp_XYZ(aX, aY, aZmin));
      aPoints.Append(gp_XYZ(aX, aY, aZmax));
    }

    for (double aZ = aZmin + 0.5 * aStep; aZ < aZmax; aZ += aStep)
    {
      aList.Append(BRepBuilderAPI_MakeVertex(gp_Pnt(aX, aYmin, aZ)));
      aList.Append(BRepBuilderAPI_MakeVertex(gp_Pnt(aX, aYmax, aZ)));

      aPoints.Append(gp_XYZ(aX, aYmin, aZ));
      aPoints.Append(gp_XYZ(aX, aYmax, aZ));
    }
  }

  for (double aY = aYmin + 0.5 * aStep; aY < aYmax; aY += aStep)
  {
    for (double aZ = aZmin + 0.5 * aStep; aZ < aZmax; aZ += aStep)
    {
      aList.Append(BRepBuilderAPI_MakeVertex(gp_Pnt(aXmin, aY, aZ)));
      aList.Append(BRepBuilderAPI_MakeVertex(gp_Pnt(aXmax, aY, aZ)));

      aPoints.Append(gp_XYZ(aXmin, aY, aZ));
      aPoints.Append(gp_XYZ(aXmax, aY, aZ));
    }
  }

  const int aNbPoints = aList.Length();
  theInterpretor << "Number of sampled points: " << aNbPoints << "\n";

  // Perform projection of points
  OSD_Timer aTimer;
  aTimer.Start();

  // Perform projection using standard method
  BRepExtrema_DistShapeShape aTool;
  aTool.SetFlag(aFlag);
  aTool.LoadS1(aShape);
  for (int anIdx = 1; anIdx <= aNbPoints; ++anIdx)
  {
    aTool.LoadS2(aList(anIdx));
    aTool.Perform();
  }

  aTimer.Stop();
  theInterpretor << "Test for gradient descent complete in: " << aTimer.ElapsedTime() << "\n";
  return 0;
}

//=================================================================================================

void QADraw::CommonCommands(Draw_Interpretor& theCommands)
{
  const char* group = "QA_Commands";

  theCommands.Add("QATestExtremaSS",
                  "QATestExtremaSS Shape Step [Flag { MIN = 0 | MAX = 1 | MINMAX = 2 }]",
                  __FILE__,
                  QATestExtremaSS,
                  group);

  // adding commands "rename" leads to the fact that QA commands doesn't work properly OCC23410, use
  // function "renamevar" theCommands.Add("rename","rename name1 toname1 name2 toname2
  // ...",__FILE__,QArename,group);
}

//==============================================================================
// QADraw::Factory
//==============================================================================
void QADraw::Factory(Draw_Interpretor& theCommands)
{
  // definition of QA Command
  QADraw::CommonCommands(theCommands);
  QADraw::TutorialCommands(theCommands);

  QABugs::Commands(theCommands);
  QADNaming::AllCommands(theCommands);
}

// Declare entry point PLUGINFACTORY
DPLUGIN(QADraw)
