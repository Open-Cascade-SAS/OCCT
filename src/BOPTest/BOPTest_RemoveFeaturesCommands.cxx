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

#include <DBRep.hxx>
#include <Draw.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

static Standard_Integer RemoveFeatures (Draw_Interpretor&, Standard_Integer, const char**);
// History commands
static Standard_Integer rfModified  (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer rfGenerated (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer rfIsDeleted (Draw_Interpretor&, Standard_Integer, const char**);

namespace
{
  static BRepAlgoAPI_Defeaturing TheDefeaturingTool;
}

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
  theCommands.Add("removefeatures", "removefeatures result shape f1 f2 ... [-nohist] [-parallel]\n"
                  "\t\tRemoves user-defined features (faces) from the shape.\n"
                  "\t\tresult   - result of the operation;\n"
                  "\t\tshape    - the shape to remove the features from;\n"
                  "\t\tf1, f2   - features to remove from the shape;\n"
                  "\t\tnohist   - disables the history collection;\n"
                  "\t\tparallel - enables the parallel processing mode.",
                  __FILE__, RemoveFeatures, group);

  theCommands.Add("rfmodified", "rfmodified c_modified shape\n"
                  "\t\tShows the shapes <c_modified> modified from the shape <shape> during Defeaturing.",
                  __FILE__, rfModified, group);
  theCommands.Add("rfgenerated", "rfgenerated c_generated shape\n"
                  "\t\tShows the shapes <c_generated> generated from the shape <shape> during Defeaturing.",
                  __FILE__, rfGenerated, group);
  theCommands.Add("rfisdeleted", "rfisdeleted shape\n"
                  "\t\tChecks if the shape has been deleted during Defeaturing.",
                  __FILE__, rfIsDeleted, group);
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
      // Check for the options
      if (!strcmp(theArgv[i], "-nohist"))
      {
        // disable the history collection
        aRF.TrackHistory(Standard_False);
      }
      else if (!strcmp(theArgv[i], "-parallel"))
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

  // Perform the removal
  aRF.Build();

  // Check for the errors/warnings
  BOPTest::ReportAlerts(aRF.GetReport());

  if (aRF.HasErrors())
    return 0;

  const TopoDS_Shape& aResult = aRF.Shape();
  DBRep::Set(theArgv[1], aResult);

  TheDefeaturingTool = aRF;

  return 0;
}

//=======================================================================
//function : CheckHistory
//purpose  : Checks if the history available for the shape
//=======================================================================
Standard_Boolean IsHistoryAvailable(const TopoDS_Shape& theS,
                                    Draw_Interpretor& theDI)
{
  if (theS.IsNull())
  {
    theDI << "Null shape.\n";
    return Standard_False;
  }

  if (!BRepTools_History::IsSupportedType(theS))
  {
    theDI << "The history is not supported for this kind of shape.\n";
    return Standard_False;
  }

  if (!TheDefeaturingTool.HasHistory())
  {
    theDI << "The history has not been prepared.\n";
    return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : rfModified
//purpose  : 
//=======================================================================
Standard_Integer rfModified(Draw_Interpretor& theDI,
                            Standard_Integer  theArgc,
                            const char ** theArgv)
{
  if (theArgc != 3)
  {
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  const TopoDS_Shape& aS = DBRep::Get(theArgv[2]);
  if (!IsHistoryAvailable(aS, theDI))
    return 0;

  const TopTools_ListOfShape& aLSIm = TheDefeaturingTool.Modified(aS);
  if (aLSIm.IsEmpty())
  {
    theDI << "The shape has not been modified.\n";
    return 0;
  }

  TopoDS_Shape aCModified;
  if (aLSIm.Extent() == 1)
    aCModified = aLSIm.First();
  else
  {
    BRep_Builder().MakeCompound(TopoDS::Compound(aCModified));
    TopTools_ListIteratorOfListOfShape itLS(aLSIm);
    for (; itLS.More(); itLS.Next())
      BRep_Builder().Add(aCModified, itLS.Value());
  }

  DBRep::Set(theArgv[1], aCModified);

  return 0;
}
//=======================================================================
//function : rfGenerated
//purpose  : 
//=======================================================================
Standard_Integer rfGenerated(Draw_Interpretor& theDI,
                             Standard_Integer  theArgc,
                             const char ** theArgv)
{
  if (theArgc != 3)
  {
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  const TopoDS_Shape& aS = DBRep::Get(theArgv[2]);
  if (!IsHistoryAvailable(aS, theDI))
    return 0;

  const TopTools_ListOfShape& aLSGen = TheDefeaturingTool.Generated(aS);
  if (aLSGen.IsEmpty())
  {
    theDI << "No shapes were generated from the shape.\n";
    return 0;
  }

  TopoDS_Shape aCGenerated;
  if (aLSGen.Extent() == 1)
    aCGenerated = aLSGen.First();
  else
  {
    BRep_Builder().MakeCompound(TopoDS::Compound(aCGenerated));
    TopTools_ListIteratorOfListOfShape itLS(aLSGen);
    for (; itLS.More(); itLS.Next())
      BRep_Builder().Add(aCGenerated, itLS.Value());
  }

  DBRep::Set(theArgv[1], aCGenerated);

  return 0;
}
//=======================================================================
//function : rfIsDeleted
//purpose  : 
//=======================================================================
Standard_Integer rfIsDeleted(Draw_Interpretor& theDI,
                             Standard_Integer  theArgc,
                             const char ** theArgv)
{
  if (theArgc != 2)
  {
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  const TopoDS_Shape& aS = DBRep::Get(theArgv[1]);
  if (!IsHistoryAvailable(aS, theDI))
    return 0;

  theDI << (TheDefeaturingTool.IsDeleted(aS) ? "Deleted" : "Not deleted") << "\n";

  return 0;
}
