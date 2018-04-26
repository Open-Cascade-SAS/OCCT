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

#include <BOPTest_DrawableShape.hxx>
#include <BOPTest_Objects.hxx>

#include <BRep_Builder.hxx>

#include <BRepAlgoAPI_Defeaturing.hxx>

#include <BRepTest_Objects.hxx>

#include <DBRep.hxx>
#include <Draw.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

static Standard_Integer RemoveFeatures (Draw_Interpretor&, Standard_Integer, const char**);

//=======================================================================
//function : RemoveFeaturesCommands
//purpose  : 
//=======================================================================
void BOPTest::RemoveFeaturesCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  // Chapter's name
  const char* group = "BOPTest commands";
  // Commands
  theCommands.Add("removefeatures", "removefeatures result shape f1 f2 ... [-parallel]\n"
                  "\t\tRemoves user-defined features (faces) from the shape.\n"
                  "\t\tresult   - result of the operation;\n"
                  "\t\tshape    - the shape to remove the features from;\n"
                  "\t\tf1, f2   - features to remove from the shape;\n"
                  "\t\tparallel - enables the parallel processing mode.",
                  __FILE__, RemoveFeatures, group);
}

//=======================================================================
//function : RemoveFeatures
//purpose  : 
//=======================================================================
Standard_Integer RemoveFeatures(Draw_Interpretor& theDI,
                                Standard_Integer  theArgc,
                                const char ** theArgv)
{
  if (theArgc < 4)
  {
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  // Get the shape to remove the features from
  TopoDS_Shape aShape = DBRep::Get(theArgv[2]);
  if (aShape.IsNull())
  {
    theDI << "Error: " << theArgv[2] << " is a null shape.\n";
    return 1;
  }

  BRepAlgoAPI_Defeaturing aRF;
  aRF.SetShape(aShape);

  // Add faces to remove
  for (Standard_Integer i = 3; i < theArgc; ++i)
  {
    TopoDS_Shape aF = DBRep::Get(theArgv[i]);
    if (aF.IsNull())
    {
      if (!strcmp(theArgv[i], "-parallel"))
      {
        // enable the parallel processing mode
        aRF.SetRunParallel(Standard_True);
      }
      else
        theDI << "Warning: " << theArgv[i] << " is a null shape. Skip it.\n";

      continue;
    }

    aRF.AddFaceToRemove(aF);
  }

  aRF.SetToFillHistory(BRepTest_Objects::IsHistoryNeeded());

  // Perform the removal
  aRF.Build();

  // Check for the errors/warnings
  BOPTest::ReportAlerts(aRF.GetReport());

  if (BRepTest_Objects::IsHistoryNeeded())
    BRepTest_Objects::SetHistory(aRF.History());

  if (aRF.HasErrors())
    return 0;

  const TopoDS_Shape& aResult = aRF.Shape();
  DBRep::Set(theArgv[1], aResult);

  return 0;
}
