// Created on: 2014-10-20
// Created by: Denis BOGOLEPOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <BRepExtrema_ShapeProximity.hxx>

#include <Precision.hxx>
#include <TopExp_Explorer.hxx>

// Assign a map of sub-shapes (edges/faces) of a given shape
static Standard_Boolean initSubShapes(const TopoDS_Shape&              theShape,
                                      BRepExtrema_ShapeList&           theSubshapesList,
                                      Handle(BRepExtrema_TriangleSet)& theTriangleSet)
{
  theSubshapesList.Clear();

  for (TopExp_Explorer anIter(theShape, TopAbs_FACE); anIter.More(); anIter.Next())
  {
    theSubshapesList.Append(anIter.Current());
  }

  for (TopExp_Explorer anIter(theShape, TopAbs_EDGE); anIter.More(); anIter.Next())
  {
    theSubshapesList.Append(anIter.Current());
  }

  if (theTriangleSet.IsNull())
    theTriangleSet = new BRepExtrema_TriangleSet;
  return theTriangleSet->Init(theSubshapesList);
}

//=======================================================================
//function : BRepExtrema_ShapeProximity
//purpose  : Creates uninitialized proximity tool
//=======================================================================
BRepExtrema_ShapeProximity::BRepExtrema_ShapeProximity (const Standard_Real theTolerance)
: myTolerance   (theTolerance),
  myElementSet1 (new BRepExtrema_TriangleSet),
  myElementSet2 (new BRepExtrema_TriangleSet),
  myNbSamples1  (0),
  myNbSamples2  (0)
{
  // Should be initialized later
  myIsInitS1 = myIsInitS2 = Standard_False;
}

//=======================================================================
//function : BRepExtrema_ShapeProximity
//purpose  : Creates proximity tool for the given two shapes
//=======================================================================
BRepExtrema_ShapeProximity::BRepExtrema_ShapeProximity (const TopoDS_Shape& theShape1,
                                                        const TopoDS_Shape& theShape2,
                                                        const Standard_Real theTolerance)
: myTolerance   (theTolerance),
  myElementSet1 (new BRepExtrema_TriangleSet),
  myElementSet2 (new BRepExtrema_TriangleSet),
  myNbSamples1  (0),
  myNbSamples2  (0)
{
  LoadShape1 (theShape1);
  LoadShape2 (theShape2);
}

//=======================================================================
//function : LoadShape1
//purpose  : Loads 1st shape into proximity tool
//=======================================================================
Standard_Boolean BRepExtrema_ShapeProximity::LoadShape1 (const TopoDS_Shape& theShape1)
{
  myIsInitS1 = initSubShapes(theShape1, myShapeList1, myElementSet1);

  if (myTolerance == Precision::Infinite())
  {
    myProxValTool.MarkDirty();
  }
  else
  {
    myOverlapTool.MarkDirty();
  }

  return myIsInitS1;
}

//=======================================================================
//function : LoadShape2
//purpose  : Loads 2nd shape into proximity tool
//=======================================================================
Standard_Boolean BRepExtrema_ShapeProximity::LoadShape2 (const TopoDS_Shape& theShape2)
{
  myIsInitS2 = initSubShapes(theShape2, myShapeList2, myElementSet2);

  if (myTolerance == Precision::Infinite())
  {
    myProxValTool.MarkDirty();
  }
  else
  {
    myOverlapTool.MarkDirty();
  }

  return myIsInitS2;
}

//=======================================================================
//function : Perform
//purpose  : Performs search of overlapped faces
//=======================================================================
void BRepExtrema_ShapeProximity::Perform()
{
  if (myTolerance == Precision::Infinite())
  {
    if (!myIsInitS1 || !myIsInitS2 || myProxValTool.IsDone())
    {
      return;
    }

    myProxValTool.LoadTriangleSets (myElementSet1,
                                    myElementSet2);
    myProxValTool.LoadShapeLists (myShapeList1,
                                  myShapeList2);
    myProxValTool.SetNbSamplePoints (myNbSamples1, myNbSamples2);

    myProxValTool.Perform (myTolerance);
    myProxValTool.ProximityPoints(myProxPoint1, myProxPoint2);
    myProxValTool.ProximityPointsStatus (myProxPntStatus1, myProxPntStatus2);
  }
  else
  {
    if (!myIsInitS1 || !myIsInitS2 || myOverlapTool.IsDone())
    {
      return;
    }

    myOverlapTool.LoadTriangleSets (myElementSet1,
                                    myElementSet2);

    myOverlapTool.Perform (myTolerance);
  }
}
