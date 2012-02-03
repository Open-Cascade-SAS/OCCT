// File:      OpenGl_GraphicDriver_1.cxx
// Created:   20 October 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_GraphicDriver.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

#include <OpenGl_CView.hxx>
#include <OpenGl_Display.hxx>

//=======================================================================
//function : Begin
//purpose  : 
//=======================================================================

Standard_Boolean OpenGl_GraphicDriver::Begin (const Standard_CString ADisplay)
{
  try
  {
    openglDisplay = new OpenGl_Display(ADisplay);
    return Standard_True;
  }
  catch (Standard_Failure)
  {
  }
  return Standard_False;
}

//=======================================================================
//function : Begin
//purpose  : 
//=======================================================================

Standard_Boolean OpenGl_GraphicDriver::Begin (const Aspect_Display ADisplay)
{
  try
  {
    openglDisplay = new OpenGl_Display(ADisplay);
    return Standard_True;
  }
  catch (Standard_Failure)
  {
  }
  return Standard_False;
}

//=======================================================================
//function : End
//purpose  : 
//=======================================================================

void OpenGl_GraphicDriver::End ()
{
  openglDisplay.Nullify();
}

//=======================================================================
//function : BeginAnimation
//purpose  : 
//=======================================================================

void OpenGl_GraphicDriver::BeginAnimation (const Graphic3d_CView& ACView)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
  {
    const Standard_Boolean UpdateAM = (ACView.IsDegenerates && !ACView.IsDegeneratesPrev) || (!ACView.IsDegenerates && ACView.IsDegeneratesPrev);
    aCView->WS->BeginAnimation(ACView.IsDegenerates != 0,UpdateAM);
    ((Graphic3d_CView*)(&ACView))->IsDegeneratesPrev = ACView.IsDegenerates; //szvgl: temporary
  }
}

//=======================================================================
//function : EndAnimation
//purpose  : 
//=======================================================================

void OpenGl_GraphicDriver::EndAnimation (const Graphic3d_CView& ACView)
{
  const OpenGl_CView *aCView = (const OpenGl_CView *)ACView.ptrView;
  if (aCView)
    aCView->WS->EndAnimation();
}
