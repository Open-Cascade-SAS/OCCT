// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#include "DocumentCommon.h"

#include "ApplicationCommon.h"
#include "Transparency.h"

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QColor>
#include <QColorDialog>
#include <QStatusBar>
#include <Standard_WarningsRestore.hxx>

#include <AIS_InteractiveObject.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OSD_Environment.hxx>

#include <TCollection_AsciiString.hxx>

// =======================================================================
// function : Viewer
// purpose  :
// =======================================================================
Handle(V3d_Viewer) DocumentCommon::Viewer(const Standard_ExtString,
                                          const Standard_CString,
                                          const Standard_Real theViewSize,
                                          const V3d_TypeOfOrientation theViewProj,
                                          const Standard_Boolean theComputedMode,
                                          const Standard_Boolean theDefaultComputedMode)
{
  static Handle(OpenGl_GraphicDriver) aGraphicDriver;
  if (aGraphicDriver.IsNull())
  {
    Handle(Aspect_DisplayConnection) aDisplayConnection;
#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
    aDisplayConnection = new Aspect_DisplayConnection(OSD_Environment("DISPLAY").Value());
#endif
    aGraphicDriver = new OpenGl_GraphicDriver(aDisplayConnection);
  }

  Handle(V3d_Viewer) aViewer = new V3d_Viewer(aGraphicDriver);
  aViewer->SetDefaultViewSize(theViewSize);
  aViewer->SetDefaultViewProj(theViewProj);
  aViewer->SetComputedMode(theComputedMode);
  aViewer->SetDefaultComputedMode(theDefaultComputedMode);
  return aViewer;
}

DocumentCommon::DocumentCommon(ApplicationCommonWindow* theApp)
: QObject (theApp),
  myContextIsEmpty(true)
{
  TCollection_ExtendedString a3DName("Visu3D");

  myViewer = Viewer(a3DName.ToExtString(), "", 1000.0, V3d_XposYnegZpos, Standard_True, Standard_True);

  myViewer->SetDefaultLights();
  myViewer->SetLightOn();

  myContext = new AIS_InteractiveContext(myViewer);
}

void DocumentCommon::SetObjects (const NCollection_Vector<Handle(AIS_InteractiveObject)>& theObjects,
                                 Standard_Boolean theDisplayShaded)
{
  myContext->RemoveAll(Standard_False);
  myContextIsEmpty = theObjects.IsEmpty();

  for(NCollection_Vector<Handle(AIS_InteractiveObject)>::Iterator anIter(theObjects); 
      anIter.More(); anIter.Next())
  {
    const Handle(AIS_InteractiveObject)& anObject = anIter.Value();
    if (!theDisplayShaded)
    {
      myContext->Display(anObject, Standard_False);
    }
    else
    {
      myContext->Display(anObject, AIS_Shaded, 0, Standard_False);
    }
  }
  myViewer->Redraw();
}

void DocumentCommon::Clear()
{
  myContext->RemoveAll(Standard_True);
  myContextIsEmpty = true;
}
