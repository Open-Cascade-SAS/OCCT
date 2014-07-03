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
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Window.hxx>
#include <Draw_Viewer.hxx>
#include <Image_AlienPixMap.hxx>
#include <V3d_View.hxx>
#include <ViewerTest.hxx>
#include <ViewerTest_EventManager.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <TopoDS_Shape.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Aspect_Window.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <DBRep.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <OSD_Timer.hxx>
#include <stdio.h>

#if defined(_WIN32) || defined(__WIN32__)
#  include <windows.h>
#  include <io.h>
#else
#  include <unistd.h>
#endif

#include <Draw_PluginMacro.hxx>

Handle(TColStd_HSequenceOfReal) GetColorOfPixel (const Image_PixMap&    theImage,
                                                 const Standard_Integer theCoordinateX,
                                                 const Standard_Integer theCoordinateY,
                                                 const Standard_Integer theRadius)
{
  Handle(TColStd_HSequenceOfReal) aSeq = new TColStd_HSequenceOfReal();
  if (theImage.IsEmpty()) {
    std::cerr << "The image is null.\n";
    return aSeq;
  }
  Standard_Integer aWidth   = (Standard_Integer )theImage.SizeX();
  Standard_Integer anHeight = (Standard_Integer )theImage.SizeY();

  Quantity_Color aColorTmp;
  for (Standard_Integer anXIter = theCoordinateX - theRadius;
       anXIter <= theCoordinateX + theRadius; ++anXIter)
  {
    if (anXIter < 0 || anXIter >= aWidth)
    {
      continue;
    }
    for (Standard_Integer anYIter = theCoordinateY - theRadius;
         anYIter <= theCoordinateY + theRadius; ++anYIter)
    {
      if (anYIter < 0 || anYIter >= anHeight)
      {
        continue;
      }
      // Image_PixMap stores image upside-down in memory!
      aColorTmp = theImage.PixelColor (anXIter, anYIter);
      aSeq->Append (aColorTmp.Red());
      aSeq->Append (aColorTmp.Green());
      aSeq->Append (aColorTmp.Blue());
    }
  }
  return aSeq;
}

static Standard_Integer QAAISGetPixelColor (Draw_Interpretor& theDi,
                                            Standard_Integer  theArgsNb,
                                            const char**      theArgs)
{
  if (theArgsNb != 3 && theArgsNb != 6)
  {
    theDi << "Usage : " << theArgs[0] << " coordinate_X coordinate_Y [color_R color_G color_B]" << "\n";
    return 1; // TCL_ERROR
  }

  Handle(V3d_View) aView3d = ViewerTest::CurrentView();
  if (aView3d.IsNull())
  {
    theDi << "You must initialize AISViewer before this command.\n";
    return 1; // TCL_ERROR
  }

  const Handle(Aspect_Window) anAISWindow = aView3d->Window();
  Standard_Integer aWindowSizeX = 0;
  Standard_Integer aWindowSizeY = 0;
  anAISWindow->Size (aWindowSizeX, aWindowSizeY);

  Standard_Integer anArgIter = 1;
  const Standard_Integer aPickCoordX = Draw::Atoi (theArgs[anArgIter++]);
  const Standard_Integer aPickCoordY = Draw::Atoi (theArgs[anArgIter++]);
  const Standard_Integer aRadius = (theArgsNb == 3) ? 0 : 1;

  Image_ColorRGBF aColorInput = {{ 0.0f, 0.0f, 0.0f }};
  if (theArgsNb == 6)
  {
    aColorInput.r() = (Standard_ShortReal )Draw::Atof (theArgs[anArgIter++]);
    aColorInput.g() = (Standard_ShortReal )Draw::Atof (theArgs[anArgIter++]);
    aColorInput.b() = (Standard_ShortReal )Draw::Atof (theArgs[anArgIter++]);
  }

  Image_PixMap anImage;
  aView3d->ToPixMap (anImage, aWindowSizeX, aWindowSizeY);
  const Handle(TColStd_HSequenceOfReal) aSeq = GetColorOfPixel (anImage, aPickCoordX, aPickCoordY, aRadius);
  cout << "Length = " << aSeq->Length() << endl;

  Image_ColorRGBF aColorPicked = {{ 0.0f, 0.0f, 0.0f }};
  Standard_Boolean isNotEqual = Standard_True;
  for (Standard_Integer i = 1; i <= aSeq->Length(); i += 3)
  {
    aColorPicked.r() = (Standard_ShortReal )aSeq->Value (i + 0);
    aColorPicked.g() = (Standard_ShortReal )aSeq->Value (i + 1);
    aColorPicked.b() = (Standard_ShortReal )aSeq->Value (i + 2);

    if (theArgsNb == 3 ||
        ((Abs (aColorPicked.r() - aColorInput.r()) <= Precision::Confusion())
      && (Abs (aColorPicked.g() - aColorInput.g()) <= Precision::Confusion())
      && (Abs (aColorPicked.b() - aColorInput.b()) <= Precision::Confusion())))
    {
      isNotEqual = Standard_False;
      break;
    }
  }

  theDi << "RED :   " << aColorPicked.r() << " "
        << "GREEN : " << aColorPicked.g() << " "
        << "BLUE :  " << aColorPicked.b() << "\n";

  if (theArgsNb == 6)
  {
    theDi << "User color: \n"
          << "RED :   " << aColorInput.r() << " "
          << "GREEN : " << aColorInput.g() << " "
          << "BLUE :  " << aColorInput.b() << "\n";
  }

  if (isNotEqual)
  {
    theDi << "Faulty : colors are not equal.\n";
    return 1; // TCL_ERROR
  }
  return 0;
}

//=======================================================================
#if ! defined(WNT)
extern ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();
extern Handle(AIS_InteractiveContext)& TheAISContext();
#else
Standard_EXPORT ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();
Standard_EXPORT Handle(AIS_InteractiveContext)& TheAISContext();
#endif
#include <ViewerTest_DoubleMapOfInteractiveAndName.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_Axis.hxx>
#include <Geom_Line.hxx>
#include <AIS_Line.hxx>

//==============================================================================
// function : VTrihedronOrigins
// author   : ota
// purpose  : draws triheron axis origin lines.
// Draw arg : vtri_orig trihedron_name
//==============================================================================
static int VTrihedronOrigins(Draw_Interpretor& di,
			      Standard_Integer argc,
			      const char ** argv)
{
  if(argc != 2){
    di <<"Usage : vtri_orig tri_name"<<"\n";
    return 1;
  }

  if(TheAISContext().IsNull()){
    di<<"Make 'vinit' before this method call"<<"\n";
    return 1;
  }

  //get trihedron from AIS map.
  TCollection_AsciiString aName(argv[1]);
  if(!GetMapOfAIS().IsBound2(aName)){
    di<<"No object named '"<<argv[1]<<"'"<<"\n";
    return 1;
  }

  Handle(AIS_Trihedron) aTrih =
    Handle(AIS_Trihedron)::DownCast(GetMapOfAIS().Find2(aName));
  if(aTrih.IsNull()){
    di<<"Trihedron is not found, try another name"<<"\n";
    return 1;
  }

  //get axis
  Handle(AIS_Axis) XAxis = aTrih->XAxis();
  Handle(AIS_Axis) YAxis = aTrih->YAxis();
  Handle(AIS_Axis) ZAxis = aTrih->Axis();

  //get geometrical lines
  Handle(Geom_Line) XGLine = XAxis->Component();
  Handle(Geom_Line) YGLine = YAxis->Component();
  Handle(Geom_Line) ZGLine = ZAxis->Component();

  //make AIS_Lines
  Handle(AIS_Line) XLine = new AIS_Line(XGLine);
  Handle(AIS_Line) YLine = new AIS_Line(YGLine);
  Handle(AIS_Line) ZLine = new AIS_Line(ZGLine);

  //put them into AIS map:
  GetMapOfAIS().Bind(XLine,aName+"_X");
  GetMapOfAIS().Bind(YLine,aName+"_Y");
  GetMapOfAIS().Bind(ZLine,aName+"_Z");
  //print names of created objects:
  di<<argv[1]<<"_X  "<<argv[1]<<"_Y  "<<argv[1]<<"_Z"<<"\n";

  //try to draw them:
  TheAISContext()->Display(XLine);
  TheAISContext()->Display(YLine);
  TheAISContext()->Display(ZLine);

  return 0;
}

//=======================================================================
//function : QATestExtremaSS
//purpose  :
//=======================================================================
static Standard_Integer QATestExtremaSS (Draw_Interpretor& theInterpretor,
                                         Standard_Integer  theArgNb,
                                         const char**      theArgs)
{
  if (theArgNb < 3
   || theArgNb > 4)
  {
    std::cerr << "Usage: type help " << theArgs[0] << std::endl;
    return 1;
  }

  // Get target shape
  TopoDS_Shape aShape = DBRep::Get (theArgs[1]);
  if (aShape.IsNull())
  {
    std::cerr << "Error: " << theArgs[1] << " shape is null\n";
    return 1;
  }

  // Get step value
  const Standard_Real aStep = Draw::Atof (theArgs[2]);
  if (aStep <= 1e-5)
  {
    std::cerr << "Error: Step " << aStep << " is too small\n";
    return 1;
  }

  Extrema_ExtFlag aFlag = Extrema_ExtFlag_MIN;
  if (theArgNb > 3)
  {
    Standard_Integer aVal = Draw::Atoi (theArgs[3]);
    if (aVal > 0)
    {
      aFlag = aVal == 1 ? Extrema_ExtFlag_MAX : Extrema_ExtFlag_MINMAX;
    }
  }

  // Get bounding box of the shape
  Bnd_Box aBounds;
  BRepBndLib::Add (aShape, aBounds);

  Standard_Real aXmin, aYmin, aZmin;
  Standard_Real aXmax, aYmax, aZmax;
  aBounds.Get (aXmin, aYmin, aZmin,
               aXmax, aYmax, aZmax);

  const Standard_Real aScaleFactor = 1.5;
  aXmin *= aScaleFactor;
  aYmin *= aScaleFactor;
  aZmin *= aScaleFactor;
  aXmax *= aScaleFactor;
  aYmax *= aScaleFactor;
  aZmax *= aScaleFactor;

  TopTools_SequenceOfShape aList;
  TColgp_SequenceOfXYZ     aPoints;
  for (Standard_Real aX = aXmin + 0.5 * aStep; aX < aXmax; aX += aStep)
  {
    for (Standard_Real aY = aYmin + 0.5 * aStep; aY < aYmax; aY += aStep)
    {
      aList.Append (BRepBuilderAPI_MakeVertex (gp_Pnt (aX, aY, aZmin)));
      aList.Append (BRepBuilderAPI_MakeVertex (gp_Pnt (aX, aY, aZmax)));

      aPoints.Append (gp_XYZ (aX, aY, aZmin));
      aPoints.Append (gp_XYZ (aX, aY, aZmax));
    }

    for (Standard_Real aZ = aZmin + 0.5 * aStep; aZ < aZmax; aZ += aStep)
    {
      aList.Append (BRepBuilderAPI_MakeVertex (gp_Pnt (aX, aYmin, aZ)));
      aList.Append (BRepBuilderAPI_MakeVertex (gp_Pnt (aX, aYmax, aZ)));

      aPoints.Append (gp_XYZ (aX, aYmin, aZ));
      aPoints.Append (gp_XYZ (aX, aYmax, aZ));
    }
  }

  for (Standard_Real aY = aYmin + 0.5 * aStep; aY < aYmax; aY += aStep)
  {
    for (Standard_Real aZ = aZmin + 0.5 * aStep; aZ < aZmax; aZ += aStep)
    {
      aList.Append (BRepBuilderAPI_MakeVertex (gp_Pnt (aXmin, aY, aZ)));
      aList.Append (BRepBuilderAPI_MakeVertex (gp_Pnt (aXmax, aY, aZ)));

      aPoints.Append (gp_XYZ (aXmin, aY, aZ));
      aPoints.Append (gp_XYZ (aXmax, aY, aZ));
    }
  }

  const Standard_Integer aNbPoints = aList.Length();
  theInterpretor << "Number of sampled points: " << aNbPoints << "\n";

  // Perform projection of points
  OSD_Timer aTimer;
  aTimer.Start();

  // Perform projection using standard method
  BRepExtrema_DistShapeShape aTool;
  aTool.SetFlag (aFlag);
  aTool.LoadS1 (aShape);
  for (Standard_Integer anIdx = 1; anIdx <= aNbPoints; ++anIdx)
  {
    aTool.LoadS2 (aList (anIdx));
    aTool.Perform();
  }

  aTimer.Stop();
  theInterpretor << "Test for gradient descent complete in: " << aTimer.ElapsedTime() << "\n";
  return 0;
}

//=======================================================================
//function : CommonCommands
//purpose  :
//=======================================================================
void QADraw::CommonCommands (Draw_Interpretor& theCommands)
{
  const char* group = "QA_Commands";

  theCommands.Add ("QAGetPixelColor",
                   "QAGetPixelColor coordinate_X coordinate_Y [color_R color_G color_B]",
                   __FILE__,
                   QAAISGetPixelColor,
                   group);

  theCommands.Add ("vtri_orig",
                   "vtri_orig         : vtri_orig trihedron_name  -  draws axis origin lines",
                   __FILE__,
                   VTrihedronOrigins,
                   group);

  theCommands.Add ("QATestExtremaSS",
                   "QATestExtremaSS Shape Step [Flag { MIN = 0 | MAX = 1 | MINMAX = 2 }]",
                   __FILE__,
                   QATestExtremaSS,
                   group);

// adding commands "rename" leads to the fact that QA commands doesn't work properly OCC23410, use function "renamevar"
// theCommands.Add("rename","rename name1 toname1 name2 toname2 ...",__FILE__,QArename,group);
}

//==============================================================================
// QADraw::Factory
//==============================================================================
void QADraw::Factory(Draw_Interpretor& theCommands)
{
  // definition of QA Command
  QADraw::CommonCommands(theCommands);
  QADraw::AdditionalCommands(theCommands);
}

// Declare entry point PLUGINFACTORY
DPLUGIN(QADraw)
