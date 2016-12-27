// OCC_2dDoc.cpp: implementation of the OCC_2dDoc class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h>

#include "OCC_2dDoc.h"

#include "OCC_App.h"
#include "OCC_2dView.h"

IMPLEMENT_DYNCREATE(OCC_2dDoc, CDocument)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OCC_2dDoc::OCC_2dDoc() : OCC_BaseDoc()
{
  // Get the Graphic Driver from the application 
  Handle(Graphic3d_GraphicDriver) aGraphicDriver = 
    ((OCC_App*)AfxGetApp())->GetGraphicDriver();

  // create the Viewer
  myViewer = new V3d_Viewer (aGraphicDriver);
  myViewer->SetDefaultLights();
  myViewer->SetLightOn();
  myViewer->SetDefaultViewProj (V3d_Zpos);

  // set default values for grids
  myViewer->SetCircularGridValues (0, 0, 10, 8, 0);
  myViewer->SetRectangularGridValues (0, 0, 10, 10, 0);

  myAISContext = new AIS_InteractiveContext (myViewer);

  AfxInitRichEdit();
}

OCC_2dDoc::~OCC_2dDoc()
{
}

void OCC_2dDoc::FitAll2DViews(Standard_Boolean theUpdateViewer)
{
  if (theUpdateViewer)
  {
    myViewer->Update();
  }

  POSITION aPosition = GetFirstViewPosition();
  while (aPosition != (POSITION)NULL)
  {
    OCC_2dView* aCurrentView = (OCC_2dView*)GetNextView (aPosition);
    ASSERT_VALID (aCurrentView);
    aCurrentView->GetV2dView()->FitAll();
  }
}

void OCC_2dDoc::MoveEvent(const Standard_Integer theMouseX,
                          const Standard_Integer theMouseY,
                          const Handle(V3d_View)& theView)
{
  myAISContext->MoveTo (theMouseX, theMouseY, theView, Standard_True);
}

void OCC_2dDoc::ShiftMoveEvent(const Standard_Integer theMouseX,
                               const Standard_Integer theMouseY,
                               const Handle(V3d_View)& theView)
{
  myAISContext->MoveTo (theMouseX, theMouseY, theView, Standard_True);
}
