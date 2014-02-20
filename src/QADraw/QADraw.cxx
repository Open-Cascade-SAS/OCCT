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
#include <Draw_Interpretor.hxx>
#include <Image_AlienPixMap.hxx>
#include <V3d_View.hxx>
#include <ViewerTest.hxx>
#include <ViewerTest_EventManager.hxx>
#include <TColStd_StackOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Draw.hxx>
#include <Draw_Window.hxx>
#include <Draw_Viewer.hxx>
#include <Aspect_WindowDriver.hxx>
#include <stdio.h>
#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d.hxx>
#include <Aspect_Window.hxx>

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

void QADraw::CommonCommands(Draw_Interpretor& theCommands)
{
  const char* group = "QA_Commands";

  theCommands.Add("QAGetPixelColor", "QAGetPixelColor coordinate_X coordinate_Y [color_R color_G color_B]", __FILE__,QAAISGetPixelColor, group);
  theCommands.Add("vtri_orig",
		  "vtri_orig         : vtri_orig trihedron_name  -  draws axis origin lines",
		  __FILE__,VTrihedronOrigins,group);

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
