// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <Draw_View.hxx>
#include <Draw_Viewer.hxx>

#include <gp_Trsf.hxx>

//=================================================================================================

Draw_View::Draw_View(Standard_Integer theId,
                     Draw_Viewer*     theViewer,
                     Standard_Integer theX,
                     Standard_Integer theY,
                     Standard_Integer theWidth,
                     Standard_Integer theHeight,
                     Aspect_Drawable  theWindow)
    : Draw_Window("Win",
                  NCollection_Vec2<int>(theX, theY),
                  NCollection_Vec2<int>(theWidth, theHeight),
                  0,
                  theWindow),
      myId(theId),
      myViewer(theViewer),
      myIsPers(Standard_False),
      myIs2D(Standard_False),
      myFocalDistance(0.0),
      myZoom(0.0),
      myDx(0),
      myDy(0),
      myFrameX0(0),
      myFrameY0(0),
      myFrameX1(0),
      myFrameY1(0)
{
  memset(myType, 0, sizeof(myType));
}

//! Find window by it's XID - applicable only to X11.
static Aspect_Drawable findWindow(const char* theWindow)
{
  Aspect_Drawable aWindow = 0;
#ifdef HAVE_XLIB
  sscanf(theWindow, "%lx", &aWindow);
#else
  (void)theWindow;
#endif
  return aWindow;
}

//=================================================================================================

Draw_View::Draw_View(Standard_Integer theId, Draw_Viewer* theViewer, const char* theTitle)
    : Draw_Window(theTitle,
                  NCollection_Vec2<int>(0),
                  NCollection_Vec2<int>(50),
                  0,
                  findWindow(theTitle)),
      myId(theId),
      myViewer(theViewer),
      myIsPers(Standard_False),
      myIs2D(Standard_False),
      myFocalDistance(0.0),
      myZoom(0.0),
      myDx(0),
      myDy(0),
      myFrameX0(0),
      myFrameY0(0),
      myFrameX1(0),
      myFrameY1(0)
{
  memset(myType, 0, sizeof(myType));
}

//=================================================================================================

Draw_View::~Draw_View()
{
  Draw_Window::Destroy();
}

//=================================================================================================

Standard_Boolean Draw_View::Init(const char* theType)
{
  { // default fields
    myFocalDistance = 500.;
    myIsPers        = Standard_False;
    myIs2D          = strcmp("-2D-", theType) ? Standard_False : Standard_True;
    myZoom          = 1;

    ResetFrame();
  }

  gp_Trsf aRotation;
  gp_Pnt  Pvise(0.0, 0.0, 0.0);

  if (!strcmp("+X+Y", theType) || myIs2D)
  {
    myMatrix = aRotation;
  }
  else if (!strcmp("-Y+X", theType))
  {
    const gp_Dir aD(0., 0., 1.);
    myMatrix.SetRotation(gp_Ax1(Pvise, aD), 0.5 * M_PI);
  }
  else if (!strcmp("-X-Y", theType))
  {
    const gp_Dir aD(0., 0., 1.);
    myMatrix.SetRotation(gp_Ax1(Pvise, aD), M_PI);
  }
  else if (!strcmp("+Y-X", theType))
  {
    const gp_Dir aD(0., 0., 1.);
    myMatrix.SetRotation(gp_Ax1(Pvise, aD), -0.5 * M_PI);
  }
  else if (!strcmp("+Y+X", theType))
  {
    const gp_Dir aD1(0., 0., 1.);
    const gp_Dir aD2(0., 1., 0.);

    myMatrix.SetRotation(gp_Ax1(Pvise, aD1), -0.5 * M_PI);
    aRotation.SetRotation(gp_Ax1(Pvise, aD2), M_PI);
    myMatrix.Multiply(aRotation);
  }
  else if (!strcmp("-X+Y", theType))
  {
    const gp_Dir aD(0., 1., 0.);
    myMatrix.SetRotation(gp_Ax1(Pvise, aD), M_PI);
  }
  else if (!strcmp("-Y-X", theType))
  {
    const gp_Dir aD1(0., 0., 1.);
    const gp_Dir aD2(1., 0., 0.);

    myMatrix.SetRotation(gp_Ax1(Pvise, aD1), -0.5 * M_PI);
    aRotation.SetRotation(gp_Ax1(Pvise, aD2), M_PI);
    myMatrix.Multiply(aRotation);
  }
  else if (!strcmp("+X-Y", theType))
  {
    const gp_Dir aD(1., 0., 0.);
    myMatrix.SetRotation(gp_Ax1(Pvise, aD), M_PI);
  }
  else if (!strcmp("+X+Z", theType))
  {
    const gp_Dir aD(1., 0., 0.);
    myMatrix.SetRotation(gp_Ax1(Pvise, aD), -0.5 * M_PI);
  }
  else if (!strcmp("-Z+X", theType))
  {
    const gp_Dir aD1(1., 0., 0.);
    const gp_Dir aD2(0., 1., 0.);

    myMatrix.SetRotation(gp_Ax1(Pvise, aD1), -0.5 * M_PI);
    aRotation.SetRotation(gp_Ax1(Pvise, aD2), -0.5 * M_PI);
    myMatrix.Multiply(aRotation);
  }
  else if (!strcmp("-X-Z", theType))
  {
    const gp_Dir aD1(1., 0., 0.);
    const gp_Dir aD2(0., 1., 0.);

    myMatrix.SetRotation(gp_Ax1(Pvise, aD1), -0.5 * M_PI);
    aRotation.SetRotation(gp_Ax1(Pvise, aD2), -M_PI);
    myMatrix.Multiply(aRotation);
  }
  else if (!strcmp("+Z-X", theType))
  {
    const gp_Dir aD1(1., 0., 0.);
    const gp_Dir aD2(0., 1., 0.);

    myMatrix.SetRotation(gp_Ax1(Pvise, aD1), -0.5 * M_PI);
    aRotation.SetRotation(gp_Ax1(Pvise, aD2), 0.5 * M_PI);
    myMatrix.Multiply(aRotation);
  }
  else if (!strcmp("+Z+X", theType))
  {
    const gp_Dir aD1(1., 0., 0.);
    const gp_Dir aD2(0., 1., 0.);

    myMatrix.SetRotation(gp_Ax1(Pvise, aD1), 0.5 * M_PI);
    aRotation.SetRotation(gp_Ax1(Pvise, aD2), 0.5 * M_PI);
    myMatrix.Multiply(aRotation);
  }
  else if (!strcmp("-X+Z", theType))
  {
    const gp_Dir aD1(1., 0., 0.);
    const gp_Dir aD2(0., 1., 0.);

    myMatrix.SetRotation(gp_Ax1(Pvise, aD1), 0.5 * M_PI);
    aRotation.SetRotation(gp_Ax1(Pvise, aD2), M_PI);
    myMatrix.Multiply(aRotation);
  }
  else if (!strcmp("-Z-X", theType))
  {
    const gp_Dir aD1(1., 0., 0.);
    const gp_Dir aD2(0., 1., 0.);

    myMatrix.SetRotation(gp_Ax1(Pvise, aD1), 0.5 * M_PI);
    aRotation.SetRotation(gp_Ax1(Pvise, aD2), -0.5 * M_PI);
    myMatrix.Multiply(aRotation);
  }
  else if (!strcmp("+X-Z", theType))
  {
    const gp_Dir aD(1., 0., 0.);
    myMatrix.SetRotation(gp_Ax1(Pvise, aD), 0.5 * M_PI);
  }
  else if (!strcmp("+Y+Z", theType))
  {
    const gp_Dir aD1(0., 1., 0.);
    const gp_Dir aD2(1., 0., 0.);

    myMatrix.SetRotation(gp_Ax1(Pvise, aD1), -0.5 * M_PI);
    aRotation.SetRotation(gp_Ax1(Pvise, aD2), -0.5 * M_PI);
    myMatrix.Multiply(aRotation);
  }
  else if (!strcmp("-Z+Y", theType))
  {
    const gp_Dir aD(0., 1., 0.);
    myMatrix.SetRotation(gp_Ax1(Pvise, aD), -0.5 * M_PI);
  }
  else if (!strcmp("-Y-Z", theType))
  {
    const gp_Dir aD1(0., 1., 0.);
    const gp_Dir aD2(1., 0., 0.);

    myMatrix.SetRotation(gp_Ax1(Pvise, aD1), -0.5 * M_PI);
    aRotation.SetRotation(gp_Ax1(Pvise, aD2), 0.5 * M_PI);
    myMatrix.Multiply(aRotation);
  }
  else if (!strcmp("+Z-Y", theType))
  {
    const gp_Dir aD1(0., 1., 0.);
    const gp_Dir aD2(1., 0., 0.);

    myMatrix.SetRotation(gp_Ax1(Pvise, aD1), -0.5 * M_PI);
    aRotation.SetRotation(gp_Ax1(Pvise, aD2), M_PI);
    myMatrix.Multiply(aRotation);
  }
  else if (!strcmp("+Z+Y", theType))
  {
    const gp_Dir aD(0., 1., 0.);
    myMatrix.SetRotation(gp_Ax1(Pvise, aD), 0.5 * M_PI);
  }
  else if (!strcmp("-Y+Z", theType))
  {
    const gp_Dir aD1(0., 1., 0.);
    const gp_Dir aD2(1., 0., 0.);

    myMatrix.SetRotation(gp_Ax1(Pvise, aD1), 0.5 * M_PI);
    aRotation.SetRotation(gp_Ax1(Pvise, aD2), -0.5 * M_PI);
    myMatrix.Multiply(aRotation);
  }
  else if (!strcmp("-Z-Y", theType))
  {
    const gp_Dir aD1(0., 1., 0.);
    const gp_Dir aD2(1., 0., 0.);

    myMatrix.SetRotation(gp_Ax1(Pvise, aD1), 0.5 * M_PI);
    aRotation.SetRotation(gp_Ax1(Pvise, aD2), M_PI);
    myMatrix.Multiply(aRotation);
  }
  else if (!strcmp("+Y-Z", theType))
  {
    const gp_Dir aD1(0., 1., 0.);
    const gp_Dir aD2(1., 0., 0.);

    myMatrix.SetRotation(gp_Ax1(Pvise, aD1), 0.5 * M_PI);
    aRotation.SetRotation(gp_Ax1(Pvise, aD2), 0.5 * M_PI);
    myMatrix.Multiply(aRotation);
  }
  else if (!strcmp("AXON", theType))
  {
    const gp_Dir aD1(1., 0., 0.);
    const gp_Dir aD2(0., 0., 1.);

    myMatrix.SetRotation(gp_Ax1(Pvise, aD1), -0.25 * M_PI);
    aRotation.SetRotation(gp_Ax1(Pvise, aD2), -0.25 * M_PI);
    myMatrix.Multiply(aRotation);
  }
  else if (!strcmp("PERS", theType))
  {
    const gp_Dir aD1(1., 0., 0.);
    const gp_Dir aD2(0., 0., 1.);

    myMatrix.SetRotation(gp_Ax1(Pvise, aD1), -0.25 * M_PI);
    aRotation.SetRotation(gp_Ax1(Pvise, aD2), -0.25 * M_PI);
    myMatrix.Multiply(aRotation);

    myIsPers = Standard_True;
  }
  else
  {
    return Standard_False;
  }

  strcpy(myType, theType);
  return Standard_True;
}

//=================================================================================================

void Draw_View::Transform(const gp_Trsf& theTransformation)
{
  myMatrix.Multiply(theTransformation);
}

//=================================================================================================

void Draw_View::ResetFrame()
{
  myFrameX0 = 0;
  myFrameY0 = 0;
  myFrameX1 = 0;
  myFrameY1 = 0;
}

//=================================================================================================

void Draw_View::GetFrame(Standard_Integer& theX0,
                         Standard_Integer& theY0,
                         Standard_Integer& theX1,
                         Standard_Integer& theY1)
{
  if (myFrameX0 == myFrameX1)
  {
    myViewer->GetFrame(myId, theX0, theY0, theX1, theY1);
    myFrameX0 = theX0;
    myFrameX1 = theX1;
    myFrameY0 = theY0;
    myFrameY1 = theY1;
  }
  else
  {
    theX0 = myFrameX0;
    theX1 = myFrameX1;
    theY0 = myFrameY0;
    theY1 = myFrameY1;
  }
}

//=================================================================================================

void Draw_View::WExpose()
{
  ResetFrame();
  myViewer->RepaintView(myId);
}
