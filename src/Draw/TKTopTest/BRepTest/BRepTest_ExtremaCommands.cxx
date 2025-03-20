// Created on: 1995-09-08
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <DBRep.hxx>
#include <BRepTest.hxx>
#include <BRepExtrema_Poly.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepExtrema_ShapeProximity.hxx>
#include <BRepExtrema_SelfIntersection.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <Draw.hxx>
#include <Message.hxx>
#include <OSD_Timer.hxx>
#include <TCollection_AsciiString.hxx>
#include <Precision.hxx>

#include <stdio.h>

//=================================================================================================

static Standard_Integer distance(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3)
    return 1;

  const char* name1 = a[1];
  const char* name2 = a[2];

  TopoDS_Shape S1 = DBRep::Get(name1);
  TopoDS_Shape S2 = DBRep::Get(name2);
  if (S1.IsNull() || S2.IsNull())
    return 1;
  gp_Pnt        P1, P2;
  Standard_Real D;
  if (!BRepExtrema_Poly::Distance(S1, S2, P1, P2, D))
    return 1;
  // std::cout << " distance : " << D << std::endl;
  di << " distance : " << D << "\n";
  TopoDS_Edge E = BRepLib_MakeEdge(P1, P2);
  DBRep::Set("distance", E);
  return 0;
}

static Standard_Integer distmini(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 4 || n > 6)
  {
    return 1;
  }

  const char * ns1 = (a[2]), *ns2 = (a[3]), *ns0 = (a[1]);
  TopoDS_Shape S1(DBRep::Get(ns1)), S2(DBRep::Get(ns2));

  Standard_Real    aDeflection = Precision::Confusion();
  Standard_Integer anIndex     = 4;
  if (n >= 5 && a[4][0] != '-')
  {
    aDeflection = Draw::Atof(a[4]);
    anIndex++;
  }

  Standard_Boolean isMultiThread = Standard_False;
  for (Standard_Integer anI = anIndex; anI < n; anI++)
  {
    TCollection_AsciiString anArg(a[anI]);
    anArg.LowerCase();
    if (anArg == "-parallel")
    {
      isMultiThread = Standard_True;
    }
    else
    {
      di << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }

  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(di, 1);
  BRepExtrema_DistShapeShape     dst;
  dst.LoadS1(S1);
  dst.LoadS2(S2);
  dst.SetDeflection(aDeflection);
  dst.SetMultiThread(isMultiThread);
  dst.Perform(aProgress->Start());

  if (dst.IsDone())
  {
#ifdef OCCT_DEBUG
    // dst.Dump(std::cout);
    di << "*** Dump of \"BRepExtrema_DistShapeShape\" in DEBUG mode (begin) *****\n";
    Standard_SStream aSStream;
    dst.Dump(aSStream);
    di << aSStream;
    di << "*** Dump of \"BRepExtrema_DistShapeShape\" in DEBUG mode (end)   *****\n";
#endif

    di << "\"distmini\" command returns:\n";

    char named[100];
    Sprintf(named, "%s%s", ns0, "_val");
    char* tempd = named;
    Draw::Set(tempd, dst.Value());
    di << named << " ";

    for (Standard_Integer i1 = 1; i1 <= dst.NbSolution(); i1++)
    {
      gp_Pnt P1, P2;
      P1 = (dst.PointOnShape1(i1));
      P2 = (dst.PointOnShape2(i1));
      if (dst.Value() <= 1.e-9)
      {
        TopoDS_Vertex V = BRepLib_MakeVertex(P1);
        char          namev[100];
        if (i1 == 1)
          Sprintf(namev, "%s", ns0);
        else
          Sprintf(namev, "%s%d", ns0, i1);
        char* tempv = namev;
        DBRep::Set(tempv, V);
        di << namev << " ";
      }
      else
      {
        char        name[100];
        TopoDS_Edge E = BRepLib_MakeEdge(P1, P2);
        if (i1 == 1)
        {
          Sprintf(name, "%s", ns0);
        }
        else
        {
          Sprintf(name, "%s%d", ns0, i1);
        }

        char* temp = name;
        DBRep::Set(temp, E);
        di << name << " ";
      }
    }

    di << "\nOutput is complete.\n";
  }

  else
    di << "problem: no distance is found\n";
  return 0;
}

//=================================================================================================

static int ShapeProximity(Draw_Interpretor& theDI, Standard_Integer theNbArgs, const char** theArgs)
{
  if (theNbArgs < 3 || theNbArgs > 6)
  {
    Message::SendFail() << "Usage: " << theArgs[0]
                        << " Shape1 Shape2 [-tol <value> | -value] [-profile]";
    return 1;
  }

  TopoDS_Shape aShape1 = DBRep::Get(theArgs[1]);
  TopoDS_Shape aShape2 = DBRep::Get(theArgs[2]);

  if (aShape1.IsNull() || aShape2.IsNull())
  {
    Message::SendFail() << "Error: Failed to find specified shapes";
    return 1;
  }

  BRepExtrema_ShapeProximity aTool(0.0);

  Standard_Boolean aProfile    = Standard_False;
  Standard_Boolean isTolerance = Standard_False;
  Standard_Boolean isValue     = Standard_False;

  for (Standard_Integer anArgIdx = 3; anArgIdx < theNbArgs; ++anArgIdx)
  {
    TCollection_AsciiString aFlag(theArgs[anArgIdx]);
    aFlag.LowerCase();

    if (aFlag == "-tol")
    {
      isTolerance = Standard_True;
      if (++anArgIdx >= theNbArgs)
      {
        Message::SendFail() << "Error: wrong syntax at argument '" << aFlag;
        return 1;
      }

      const Standard_Real aTolerance = Draw::Atof(theArgs[anArgIdx]);
      if (aTolerance < 0.0)
      {
        Message::SendFail() << "Error: Tolerance value should be non-negative";
        return 1;
      }
      else
      {
        aTool.SetTolerance(aTolerance);
      }
    }
    else if (aFlag == "-value")
    {
      isValue = Standard_True;
      aTool.SetTolerance(Precision::Infinite());
    }
    else if (aFlag == "-profile")
    {
      aProfile = Standard_True;
    }
  }

  if (isTolerance && isValue)
  {
    Message::SendFail() << "Error: Proximity value could not be computed if the tolerance is set";
    return 1;
  }

  Standard_Real aInitTime = 0.0;
  Standard_Real aWorkTime = 0.0;

  OSD_Timer aTimer;

  if (aProfile)
  {
    aTimer.Start();
  }

  aTool.LoadShape1(aShape1);
  aTool.LoadShape2(aShape2);

  if (aProfile)
  {
    aInitTime = aTimer.ElapsedTime();
    aTimer.Reset();
    aTimer.Start();
  }

  // Perform shape proximity test
  aTool.Perform();

  if (aProfile)
  {
    aWorkTime = aTimer.ElapsedTime();
    aTimer.Stop();
  }

  if (!aTool.IsDone())
  {
    Message::SendFail() << "Error: Failed to perform proximity test";
    return 1;
  }

  if (aProfile)
  {
    theDI << "Number of primitives in shape 1: " << aTool.ElementSet1()->Size() << "\n";
    theDI << "Number of primitives in shape 2: " << aTool.ElementSet2()->Size() << "\n";
    theDI << "Building data structures: " << aInitTime << "\n";
    theDI << "Executing proximity test: " << aWorkTime << "\n";
  }

  if (isValue)
  {
    theDI << "Proximity value: " << aTool.Proximity() << "\n";

    // proximity points
    TopoDS_Vertex aProxVtx1 = BRepLib_MakeVertex(aTool.ProximityPoint1());
    TopoDS_Vertex aProxVtx2 = BRepLib_MakeVertex(aTool.ProximityPoint2());

    DBRep::Set("ProxPnt1", aProxVtx1);
    DBRep::Set("ProxPnt2", aProxVtx2);

    // proximity points' status
    TCollection_AsciiString ProxPntStatus1;
    TCollection_AsciiString ProxPntStatus2;

    switch (aTool.ProxPntStatus1())
    {
      case 0:
        ProxPntStatus1 = "Border";
        break;
      case 1:
        ProxPntStatus1 = "Middle";
        break;
      default:
        ProxPntStatus1 = "Unknown";
    }

    switch (aTool.ProxPntStatus2())
    {
      case 0:
        ProxPntStatus2 = "Border";
        break;
      case 1:
        ProxPntStatus2 = "Middle";
        break;
      default:
        ProxPntStatus2 = "Unknown";
    }

    theDI << " Status of ProxPnt1 on " << theArgs[1] << " : " << ProxPntStatus1 << "\n";
    theDI << " Status of ProxPnt2 on " << theArgs[2] << " : " << ProxPntStatus2 << "\n";
  }
  else
  {
    TopoDS_Builder aCompBuilder;

    TopoDS_Compound aFaceCompound1;
    aCompBuilder.MakeCompound(aFaceCompound1);

    for (BRepExtrema_MapOfIntegerPackedMapOfInteger::Iterator anIt1(aTool.OverlapSubShapes1());
         anIt1.More();
         anIt1.Next())
    {
      TCollection_AsciiString aStr = TCollection_AsciiString(theArgs[1]) + "_" + (anIt1.Key() + 1);

      const TopoDS_Shape& aShape = aTool.GetSubShape1(anIt1.Key());
      aCompBuilder.Add(aFaceCompound1, aShape);
      DBRep::Set(aStr.ToCString(), aShape);

      theDI << aStr << " \n";
    }

    TopoDS_Compound aFaceCompound2;
    aCompBuilder.MakeCompound(aFaceCompound2);

    for (BRepExtrema_MapOfIntegerPackedMapOfInteger::Iterator anIt2(aTool.OverlapSubShapes2());
         anIt2.More();
         anIt2.Next())
    {
      TCollection_AsciiString aStr = TCollection_AsciiString(theArgs[2]) + "_" + (anIt2.Key() + 1);

      const TopoDS_Shape& aShape = aTool.GetSubShape2(anIt2.Key());
      aCompBuilder.Add(aFaceCompound2, aShape);
      DBRep::Set(aStr.ToCString(), aShape);

      theDI << aStr << " \n";
    }

    DBRep::Set((TCollection_AsciiString(theArgs[1]) + "_" + "overlapped").ToCString(),
               aFaceCompound1);
    DBRep::Set((TCollection_AsciiString(theArgs[2]) + "_" + "overlapped").ToCString(),
               aFaceCompound2);
  }

  return 0;
}

//=================================================================================================

static int ShapeSelfIntersection(Draw_Interpretor& theDI,
                                 Standard_Integer  theNbArgs,
                                 const char**      theArgs)
{
  if (theNbArgs < 2 || theNbArgs > 5)
  {
    Message::SendFail() << "Usage: " << theArgs[0] << " Shape [-tol <value>] [-profile]";
    return 1;
  }

  TopoDS_Shape aShape = DBRep::Get(theArgs[1]);
  if (aShape.IsNull())
  {
    Message::SendFail() << "Error: Failed to find specified shape";
    return 1;
  }

  Standard_Real    aTolerance = 0.0;
  Standard_Boolean aToProfile = Standard_False;

  for (Standard_Integer anArgIdx = 2; anArgIdx < theNbArgs; ++anArgIdx)
  {
    TCollection_AsciiString aFlag(theArgs[anArgIdx]);
    aFlag.LowerCase();

    if (aFlag == "-tol")
    {
      if (++anArgIdx >= theNbArgs)
      {
        Message::SendFail() << "Error: wrong syntax at argument '" << aFlag;
        return 1;
      }

      const Standard_Real aValue = Draw::Atof(theArgs[anArgIdx]);
      if (aValue < 0.0)
      {
        Message::SendFail() << "Error: Tolerance value should be non-negative";
        return 1;
      }
      else
      {
        aTolerance = aValue;
      }
    }

    if (aFlag == "-profile")
    {
      aToProfile = Standard_True;
    }
  }

  OSD_Timer aTimer;

  Standard_Real aInitTime = 0.0;
  Standard_Real aWorkTime = 0.0;

  if (aToProfile)
  {
    aTimer.Start();
  }

  BRepExtrema_SelfIntersection aTool(aShape, aTolerance);

  if (aToProfile)
  {
    aInitTime = aTimer.ElapsedTime();

    aTimer.Reset();
    aTimer.Start();
  }

  // Perform shape self-intersection test
  aTool.Perform();

  if (!aTool.IsDone())
  {
    Message::SendFail() << "Error: Failed to perform proximity test";
    return 1;
  }

  if (aToProfile)
  {
    aWorkTime = aTimer.ElapsedTime();
    aTimer.Stop();

    theDI << "Building data structure (BVH):    " << aInitTime << "\n";
    theDI << "Executing self-intersection test: " << aWorkTime << "\n";
  }

  // Extract output faces
  TopoDS_Builder  aCompBuilder;
  TopoDS_Compound aFaceCompound;

  aCompBuilder.MakeCompound(aFaceCompound);

  for (BRepExtrema_MapOfIntegerPackedMapOfInteger::Iterator anIt(aTool.OverlapElements());
       anIt.More();
       anIt.Next())
  {
    TCollection_AsciiString aStr = TCollection_AsciiString(theArgs[1]) + "_" + (anIt.Key() + 1);

    const TopoDS_Face& aFace = aTool.GetSubShape(anIt.Key());
    aCompBuilder.Add(aFaceCompound, aFace);
    DBRep::Set(aStr.ToCString(), aFace);

    theDI << aStr << " \n";
  }

  theDI << "Compound of overlapped sub-faces: " << theArgs[1] << "_overlapped\n";
  DBRep::Set((TCollection_AsciiString(theArgs[1]) + "_" + "overlapped").ToCString(), aFaceCompound);

  return 0;
}

//=================================================================================================

void BRepTest::ExtremaCommands(Draw_Interpretor& theCommands)
{
  static const char*      aGroup = "TOPOLOGY Extrema commands";
  static Standard_Boolean isDone = Standard_False;
  if (isDone)
  {
    return;
  }
  isDone = Standard_True;

  theCommands.Add("dist", "dist Shape1 Shape2", __FILE__, distance, aGroup);

  theCommands.Add("distmini",
                  "distmini name Shape1 Shape2 [deflection] [-parallel]",
                  "\n\t\t: Searches minimal distance between two shapes."
                  "\n\t\t: The option is:"
                  "\n\t\t:   -parallel : calculate distance in multithreaded mode" __FILE__,
                  distmini,
                  aGroup);

  theCommands.Add("proximity",
                  "proximity Shape1 Shape2 [-tol <value> | -value] [-profile]"
                  "\n\t\t: Searches for pairs of overlapping faces of the given shapes."
                  "\n\t\t: The options are:"
                  "\n\t\t:   -tol     : non-negative tolerance value used for overlapping"
                  "\n\t\t:              test (for zero tolerance, the strict intersection"
                  "\n\t\t:              test will be performed)"
                  "\n\t\t:   -value   : compute the proximity value (minimal value which"
                  "\n\t\t:              shows both shapes fully overlapped)"
                  "\n\t\t:   -profile : outputs execution time for main algorithm stages",
                  __FILE__,
                  ShapeProximity,
                  aGroup);

  theCommands.Add("selfintersect",
                  "selfintersect Shape [-tol <value>] [-profile]"
                  "\n\t\t: Searches for intersected/overlapped faces in the given shape."
                  "\n\t\t: The algorithm uses shape tessellation (should be computed in"
                  "\n\t\t: advance), and provides approximate results. The options are:"
                  "\n\t\t:   -tol     : non-negative tolerance value used for overlapping"
                  "\n\t\t:              test (for zero tolerance, the strict intersection"
                  "\n\t\t:              test will be performed)"
                  "\n\t\t:   -profile : outputs execution time for main algorithm stages",
                  __FILE__,
                  ShapeSelfIntersection,
                  aGroup);
}
