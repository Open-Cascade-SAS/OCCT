// Created on: 2012-05-28 
// 
// Copyright (c) 2011-2014 OPEN CASCADE SAS 
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

#ifdef _WIN32
#include <windows.h>
#include <vtkWin32RenderWindowInteractor.h>
#include <vtkWin32OpenGLRenderWindow.h>
#endif

#include <IVtkTools_ShapePicker.hxx>
#include <IVtkTools_SubPolyDataFilter.hxx>
#include <IVtkTools_DisplayModeFilter.hxx>
#include <IVtkTools_ShapeObject.hxx>
#include <IVtkTools_ShapeDataSource.hxx>
#include <IVtkDraw_Interactor.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>

#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

#ifndef _WIN32
#include <X11/X.h>
#include <X11/Shell.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <vtkXRenderWindowInteractor.h>
#include <vtkXOpenGLRenderWindow.h>
#include <X11/Xutil.h>
#include <tk.h>
#endif

//===========================================================
// Function : ClearHighlightAndSelection
// Purpose  :
//===========================================================
static void ClearHighlightAndSelection (const Handle(ShapePipelineMap)& theMap,
                                         const Standard_Boolean doHighlighting,
                                         const Standard_Boolean doSelection)
{
  if (!doHighlighting && !doSelection)
  {
    return;
  }

  for (ShapePipelineMap::Iterator anIt (*theMap); anIt.More(); anIt.Next())
  {
    const Handle(IVtkDraw_HighlightAndSelectionPipeline)& aPL = anIt.Value();

    if (doHighlighting)
    {
      aPL->ClearHighlightFilters();
    }

    if (doSelection)
    {
      aPL->ClearSelectionFilters();
    }
  }
}

vtkStandardNewMacro(IVtkDraw_Interactor)

//===========================================================
// Function : Constructor
// Purpose  :
//===========================================================
IVtkDraw_Interactor::IVtkDraw_Interactor()
:
#ifdef _WIN32
  myWindowId (NULL),
  myMouseInWindow (0)
#else
  myIsLeftButtonPressed (Standard_False)
#endif
{ }

//===========================================================
// Function : Destructor
// Purpose  :
//===========================================================
IVtkDraw_Interactor::~IVtkDraw_Interactor()
{
}

//===========================================================
// Function : SetShapePicker
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::SetShapePicker (const PSelector& theSelector)
{
  mySelector = theSelector;
}

//===========================================================
// Function : SetPipelines
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::SetPipelines (const Handle(ShapePipelineMap)& thePipelines)
{
  myPipelines = thePipelines;
}

//===========================================================
// Function : SetOCCWindow
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::SetOCCWindow (const Handle(Aspect_Window)& theWindow)
{
  myWindow = theWindow;
}

//===========================================================
// Function : GetOCCWindow
// Purpose  :
//===========================================================
const Handle(Aspect_Window)& IVtkDraw_Interactor::GetOCCWindow() const
{
  return myWindow;
}

//===========================================================
// Function : IsEnabled
// Purpose  :
//===========================================================
Standard_Boolean IVtkDraw_Interactor::IsEnabled() const
{
  return (Enabled != 0);
}

//===========================================================
// Function : Initialize
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::Initialize()
{
  // Make sure we have a RenderWindow and camera
  if (!this->RenderWindow)
  {
    vtkErrorMacro(<<"No renderer defined!");
    return;
  }

  if (this->Initialized)
  {
    return;
  }

  this->Initialized = 1;
  
  // Get the info we need from the RenderingWindow
  Standard_Integer *aSize;
#ifdef _WIN32
  vtkWin32OpenGLRenderWindow *aRenWin;
  aRenWin = (vtkWin32OpenGLRenderWindow *)(this->RenderWindow);
  aRenWin->Start();
  aSize = aRenWin->GetSize();
  aRenWin->GetPosition();
  this->myWindowId = aRenWin->GetWindowId();
#else
  vtkXOpenGLRenderWindow *aRenWin;
  aRenWin = static_cast<vtkXOpenGLRenderWindow *>(this->RenderWindow);
  this->myDisplayId = aRenWin->GetDisplayId();
  this->myWindowId = aRenWin->GetWindowId();
  aSize = aRenWin->GetSize();
  aRenWin->Start();
#endif

  this->Enable();
  this->Size[0] = aSize[0];
  this->Size[1] = aSize[1];
}

//===========================================================
// Function : Enable
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::Enable()
{
  if (this->Enabled)
  {
    return;
  }

  // Add event handlers
#ifdef _WIN32
  SetWindowLongPtr(this->myWindowId, GWLP_USERDATA, (LONG_PTR)this);
  SetWindowLongPtr(this->myWindowId, GWLP_WNDPROC, (LONG_PTR)WndProc);
#else
  #if TCL_MAJOR_VERSION  < 8
    Tk_CreateFileHandler((void*)ConnectionNumber(this->myDisplayId),
      TK_READABLE, ProcessEvents, (ClientData) this);
  #else
    Tk_CreateFileHandler(ConnectionNumber(this->myDisplayId),
      TK_READABLE, ProcessEvents, (ClientData) this);
  #endif
#endif

  this->Enabled = 1;
  this->Modified();
}

//===========================================================
// Function : MoveTo
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::MoveTo (Standard_Integer theX, Standard_Integer theY)
{
  // Processing highlighting
  mySelector->Pick (theX, theY, 0.0);
  vtkSmartPointer<vtkActorCollection> anActorCollection = mySelector->GetPickedActors();

  if (anActorCollection)
  {
    // Highlight picked subshapes
    ClearHighlightAndSelection (myPipelines, Standard_True, Standard_False);
    anActorCollection->InitTraversal();
    while (vtkActor* anActor = anActorCollection->GetNextActor())
    {
      IVtkTools_ShapeDataSource* aDataSource = IVtkTools_ShapeObject::GetShapeSource (anActor);
      if (!aDataSource)
      {
        continue;
      }

      IVtkOCC_Shape::Handle anOccShape = aDataSource->GetShape();
      if (anOccShape.IsNull())
      {
        continue;
      }

      IVtk_IdType aShapeID = anOccShape->GetId();
      Handle(Message_Messenger) anOutput = Message::DefaultMessenger();
      if (!myPipelines->IsBound(aShapeID))
      {
        anOutput << "Warning: there is no VTK pipeline registered for highlighted shape" << endl;
        continue;
      }

      const Handle(IVtkDraw_HighlightAndSelectionPipeline)& aPL = myPipelines->Find (aShapeID);

      // Add a subpolydata filter to the highlight pipeline for the shape data source.
      IVtkTools_SubPolyDataFilter* aFilter = aPL->GetHighlightFilter();

      // Set the selected sub-shapes ids to subpolydata filter.
      IVtk_ShapeIdList aSubShapeIds = mySelector->GetPickedSubShapesIds(aShapeID);

      // Get ids of cells for picked subshapes.
      IVtk_ShapeIdList aSubIds;
      IVtk_ShapeIdList::Iterator aMetaIds (aSubShapeIds);
      for (; aMetaIds.More(); aMetaIds.Next())
      {
        IVtk_ShapeIdList aSubSubIds = anOccShape->GetSubIds (aMetaIds.Value());
        aSubIds.Append (aSubSubIds);
      }

      aFilter->SetDoFiltering (!aSubIds.IsEmpty());
      aFilter->SetData (aSubIds);
      if (!aFilter->GetInput())
      {
        aFilter->SetInputConnection (aDataSource->GetOutputPort());
      }
      aFilter->Modified();
    }
  }
  this->Render();
}

//===========================================================
// Function : OnSelection
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::OnSelection()
{
  // Processing selection
  vtkSmartPointer<vtkActorCollection> anActorCollection = mySelector->GetPickedActors();

  if (anActorCollection)
  {
    // Highlight picked subshapes.
    ClearHighlightAndSelection (myPipelines, Standard_False, Standard_True);
    anActorCollection->InitTraversal();
    while (vtkActor* anActor = anActorCollection->GetNextActor())
    {
      IVtkTools_ShapeDataSource* aDataSource = IVtkTools_ShapeObject::GetShapeSource (anActor);
      if (!aDataSource)
      {
        continue;
      }

      IVtkOCC_Shape::Handle anOccShape = aDataSource->GetShape();
      if (anOccShape.IsNull())
      {
        continue;
      }

      IVtk_IdType aShapeID = anOccShape->GetId();
      Handle(Message_Messenger) anOutput = Message::DefaultMessenger();
      if (!myPipelines->IsBound (aShapeID))
      {
        anOutput << "Warning: there is no VTK pipeline registered for picked shape" << endl;
        continue;
      }

      const Handle(IVtkDraw_HighlightAndSelectionPipeline)& aPL = myPipelines->Find (aShapeID);

      // Add a subpolydata filter to the selection pipeline for the shape data source.
      IVtkTools_SubPolyDataFilter* aFilter = aPL->GetSelectionFilter();

      // Set the selected sub-shapes ids to subpolydata filter.
      IVtk_ShapeIdList aSubShapeIds = mySelector->GetPickedSubShapesIds(aShapeID);

      // Get ids of cells for picked subshapes.
      IVtk_ShapeIdList aSubIds;
      IVtk_ShapeIdList::Iterator aMetaIds (aSubShapeIds);
      for (; aMetaIds.More(); aMetaIds.Next())
      {
        IVtk_ShapeIdList aSubSubIds = anOccShape->GetSubIds (aMetaIds.Value());
        aSubIds.Append (aSubSubIds);
      }

      aFilter->SetDoFiltering (!aSubIds.IsEmpty());
      aFilter->SetData (aSubIds);
      if (!aFilter->GetInput())
      {
        aFilter->SetInputConnection (aDataSource->GetOutputPort());
      }
      aFilter->Modified();
    }
  }
  this->Render();
}

#ifdef _WIN32

//===========================================================
// Function : OnMouseMove
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::OnMouseMove (HWND theHWnd, UINT theNFlags,
                                       Standard_Integer theX,
                                       Standard_Integer theY)
{
  if (!this->Enabled)
  {
    return;
  }

  this->SetEventInformationFlipY (theX,
                                  theY,
                                  theNFlags & MK_CONTROL,
                                  theNFlags & MK_SHIFT);
  this->SetAltKey(GetKeyState(VK_MENU) & (~1));
  if (!this->myMouseInWindow && 
      (theX >= 0 && theX < this->Size[0] && theY >= 0 && theY < this->Size[1]))
  {
    this->InvokeEvent (vtkCommand::EnterEvent, NULL);
    this->myMouseInWindow = 1;
    // request WM_MOUSELEAVE generation
    TRACKMOUSEEVENT aTme;
    aTme.cbSize = sizeof (TRACKMOUSEEVENT);
    aTme.dwFlags = TME_LEAVE;
    aTme.hwndTrack = theHWnd;
    TrackMouseEvent (&aTme);
  }

  if (!(theNFlags & MK_LBUTTON))
    this->MoveTo (theX, this->Size[1] - theY - 1);

  this->InvokeEvent (vtkCommand::MouseMoveEvent, NULL);
}

//===========================================================
// Function : OnMouseWheelForward
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::OnMouseWheelForward (HWND, UINT theNFlags,
                                               Standard_Integer theX,
                                               Standard_Integer theY)
{
  if (!this->Enabled)
  {
    return;
  }

  this->SetEventInformationFlipY (theX,
                                  theY,
                                  theNFlags & MK_CONTROL,
                                  theNFlags & MK_SHIFT);

  this->SetAltKey (GetKeyState(VK_MENU) & (~1));
  this->InvokeEvent (vtkCommand::MouseWheelForwardEvent, NULL);
}

//===========================================================
// Function : OnMouseWheelBackward
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::OnMouseWheelBackward (HWND, UINT theNFlags,
                                                Standard_Integer theX,
                                                Standard_Integer theY)
{
  if (!this->Enabled)
  {
    return;
  }

  this->SetEventInformationFlipY (theX,
                                  theY,
                                  theNFlags & MK_CONTROL,
                                  theNFlags & MK_SHIFT);

  this->SetAltKey (GetKeyState(VK_MENU) & (~1));
  this->InvokeEvent (vtkCommand::MouseWheelBackwardEvent, NULL);
}

//===========================================================
// Function : OnLButtonDown
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::OnLButtonDown (HWND theHWnd, UINT theNFlags,
                                         Standard_Integer theX,
                                         Standard_Integer theY,
                                         Standard_Integer theRepeat)
{
  if (!this->Enabled)
  {
    return;
  }
  SetFocus (theHWnd);
  SetCapture (theHWnd);
  this->SetEventInformationFlipY (theX,
                                  theY,
                                  theNFlags & MK_CONTROL,
                                  theNFlags & MK_SHIFT,
                                  0, theRepeat);
  this->SetAltKey (GetKeyState(VK_MENU) & (~1));

  OnSelection ();

  this->InvokeEvent (vtkCommand::LeftButtonPressEvent, NULL);
}

//===========================================================
// Function : OnLButtonUp
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::OnLButtonUp (HWND, UINT theNFlags,
                                       Standard_Integer theX,
                                       Standard_Integer theY)
{
  if (!this->Enabled)
  {
    return;
  }

  this->SetEventInformationFlipY (theX,
                                  theY,
                                  theNFlags & MK_CONTROL,
                                  theNFlags & MK_SHIFT);

  this->SetAltKey (GetKeyState(VK_MENU) & (~1));
  this->InvokeEvent (vtkCommand::LeftButtonReleaseEvent, NULL);
  ReleaseCapture();
}

//===========================================================
// Function : OnMButtonDown
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::OnMButtonDown (HWND theHWnd, UINT theNFlags,
                                         Standard_Integer theX,
                                         Standard_Integer theY,
                                         Standard_Integer theRepeat)
{
  if (!this->Enabled)
  {
    return;
  }

  SetFocus (theHWnd);
  SetCapture (theHWnd);
  this->SetEventInformationFlipY (theX,
                                  theY,
                                  theNFlags & MK_CONTROL,
                                  theNFlags & MK_SHIFT,
                                  0, theRepeat);
  this->SetAltKey (GetKeyState(VK_MENU) & (~1));
  this->InvokeEvent (vtkCommand::MiddleButtonPressEvent, NULL);
}

//===========================================================
// Function : OnMButtonUp
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::OnMButtonUp (HWND, UINT theNFlags,
                                       Standard_Integer theX,
                                       Standard_Integer theY)
{
  if (!this->Enabled)
  {
    return;
  }
  this->SetEventInformationFlipY (theX,
                                  theY,
                                  theNFlags & MK_CONTROL,
                                  theNFlags & MK_SHIFT);

  this->SetAltKey (GetKeyState(VK_MENU) & (~1));
  this->InvokeEvent (vtkCommand::MiddleButtonReleaseEvent, NULL);
  ReleaseCapture();
}

//===========================================================
// Function : OnRButtonDown
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::OnRButtonDown (HWND theHWnd, UINT theNFlags,
                                         Standard_Integer theX,
                                         Standard_Integer theY,
                                         Standard_Integer theRepeat)
{
  if (!this->Enabled)
  {
    return;
  }

  SetFocus(theHWnd);
  SetCapture(theHWnd);
  this->SetEventInformationFlipY (theX,
                                  theY,
                                  theNFlags & MK_CONTROL,
                                  theNFlags & MK_SHIFT,
                                  0, theRepeat);

  this->SetAltKey (GetKeyState(VK_MENU) & (~1));
  this->InvokeEvent (vtkCommand::RightButtonPressEvent, NULL);
}

//===========================================================
// Function : OnRButtonUp
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::OnRButtonUp (HWND, UINT theNFlags,
                                       Standard_Integer theX,
                                       Standard_Integer theY)
{
  if (!this->Enabled)
  {
    return;
  }
  this->SetEventInformationFlipY (theX,
                                  theY,
                                  theNFlags & MK_CONTROL,
                                  theNFlags & MK_SHIFT);

  this->SetAltKey (GetKeyState(VK_MENU) & (~1));
  this->InvokeEvent (vtkCommand::RightButtonReleaseEvent, NULL);
  ReleaseCapture();
}

//===========================================================
// Function : OnSize
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::OnSize (HWND, UINT,
                                  Standard_Integer theX,
                                  Standard_Integer theY)
{
  this->UpdateSize (theX, theY);
  if (this->Enabled)
  {
    this->InvokeEvent (vtkCommand::ConfigureEvent, NULL);
  }
}

//===========================================================
// Function : OnTimer
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::OnTimer (HWND, UINT theTimerId)
{
  if (!this->Enabled)
  {
    return;
  }

  Standard_Integer aTid = static_cast<Standard_Integer>(theTimerId);
  this->InvokeEvent (vtkCommand::TimerEvent, (void*)&aTid);

  // Here we deal with one-shot versus repeating timers
  if (this->IsOneShotTimer(aTid))
  {
    KillTimer (this->myWindowId, aTid); //'cause windows timers are always repeating
  }
}

//===========================================================
// Function : WndProc
// Purpose  :
//===========================================================
LRESULT CALLBACK WndProc (HWND theHWnd,UINT theUMsg,
                          WPARAM theWParam,
                          LPARAM theLParam)
{
  LRESULT aRes = 0;
  IVtkDraw_Interactor *anInteractor = 0;

  anInteractor = (IVtkDraw_Interactor *)GetWindowLongPtr (theHWnd, GWLP_USERDATA);

  if (anInteractor && anInteractor->GetReferenceCount() > 0)
  {
    anInteractor->Register (anInteractor);
    aRes = ViewerWindowProc (theHWnd, theUMsg, theWParam, theLParam, anInteractor);
    anInteractor->UnRegister (anInteractor);
  }

  return aRes;
}

//===========================================================
// Function : ViewerWindowProc
// Purpose  :
//===========================================================
LRESULT CALLBACK ViewerWindowProc (HWND theHWnd,
                                   UINT theMsg,
                                   WPARAM theWParam,
                                   LPARAM theLParam,
                                   IVtkDraw_Interactor *theInteractor)
{
  switch (theMsg)
  {
  case WM_CLOSE:
    theInteractor->GetOCCWindow ()->Unmap ();
    return 0;
  case WM_PAINT:
    theInteractor->Render();
    break;
  case WM_SIZE:
    theInteractor->OnSize (theHWnd, (UINT)theWParam, LOWORD(theLParam), HIWORD(theLParam));
    break;
  case WM_LBUTTONDBLCLK:
    theInteractor->OnLButtonDown (theHWnd, (UINT)theWParam, MAKEPOINTS(theLParam).x, MAKEPOINTS(theLParam).y, 1);
    break;
  case WM_LBUTTONDOWN:
    theInteractor->OnLButtonDown (theHWnd, (UINT)theWParam, MAKEPOINTS(theLParam).x, MAKEPOINTS(theLParam).y, 0);
    break;
  case WM_LBUTTONUP:
    theInteractor->OnLButtonUp (theHWnd, (UINT)theWParam, MAKEPOINTS(theLParam).x, MAKEPOINTS(theLParam).y);
    break;
  case WM_MBUTTONDBLCLK:
    theInteractor->OnMButtonDown (theHWnd, (UINT)theWParam, MAKEPOINTS(theLParam).x, MAKEPOINTS(theLParam).y, 1);
    break;
  case WM_MBUTTONDOWN:
    theInteractor->OnMButtonDown (theHWnd, (UINT)theWParam, MAKEPOINTS(theLParam).x, MAKEPOINTS(theLParam).y, 0);
    break;
  case WM_MBUTTONUP:
    theInteractor->OnMButtonUp (theHWnd, (UINT)theWParam, MAKEPOINTS(theLParam).x, MAKEPOINTS(theLParam).y);
    break;
  case WM_RBUTTONDBLCLK:
    theInteractor->OnRButtonDown (theHWnd, (UINT)theWParam, MAKEPOINTS(theLParam).x, MAKEPOINTS(theLParam).y, 1);
    break;
  case WM_RBUTTONDOWN:
    theInteractor->OnRButtonDown (theHWnd, (UINT)theWParam, MAKEPOINTS(theLParam).x, MAKEPOINTS(theLParam).y, 0);
    break;
  case WM_RBUTTONUP:
    theInteractor->OnRButtonUp (theHWnd, (UINT)theWParam, MAKEPOINTS(theLParam).x, MAKEPOINTS(theLParam).y);
    break;
  case WM_MOUSELEAVE:
    {
      theInteractor->InvokeEvent (vtkCommand::LeaveEvent, NULL);
      theInteractor->myMouseInWindow = 0;
    }
    break;
  case WM_MOUSEMOVE:
    theInteractor->OnMouseMove (theHWnd, (UINT)theWParam, MAKEPOINTS(theLParam).x, MAKEPOINTS(theLParam).y);
    break;
  case WM_MOUSEWHEEL:
    {
      POINT pt;
      pt.x = MAKEPOINTS(theLParam).x;
      pt.y = MAKEPOINTS(theLParam).y;
      ::ScreenToClient(theHWnd, &pt);
      if( GET_WHEEL_DELTA_WPARAM(theWParam) > 0)
      {
        theInteractor->OnMouseWheelForward (theHWnd, (UINT)theWParam, pt.x, pt.y);
      }
      else
      {
        theInteractor->OnMouseWheelBackward (theHWnd, (UINT)theWParam, pt.x, pt.y);
      }
    }
    break;
  case WM_TIMER:
    theInteractor->OnTimer (theHWnd, (UINT)theWParam);
    break;
  }
  return DefWindowProc(theHWnd, theMsg, theWParam, theLParam);
}

#else

//===========================================================
// Function : GetDisplayId
// Purpose  :
//===========================================================
Display* IVtkDraw_Interactor::GetDisplayId() const
{
  return myDisplayId;
}

//===========================================================
// Function : GetMousePosition
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::GetMousePosition (Standard_Integer *theX,
                                            Standard_Integer *theY)
{
  Window aRoot, aChild;
  Standard_Integer aRoot_x, aRoot_y;
  unsigned int aKeys;

  XQueryPointer (this->myDisplayId, this->myWindowId,
                 &aRoot, &aChild, &aRoot_x, &aRoot_y, theX, theY, &aKeys);

}

//===========================================================
// Function : ViewerMainLoop
// Purpose  :
//===========================================================
Standard_Integer IVtkDraw_Interactor::ViewerMainLoop (Standard_Integer theArgNum, const char** /*theArgs*/)
{
  Standard_Integer aXp, aYp;
  Standard_Boolean aPick = theArgNum > 0;

  static XEvent anEvent;
  XNextEvent (myDisplayId, &anEvent);

  switch (anEvent.type)
  {
  case Expose:
    {
      if (!this->Enabled)
      {
        return aPick;
      }
      XEvent aResult;
      while (XCheckTypedWindowEvent (this->myDisplayId,
                                     this->myWindowId,
                                     Expose,
                                     &aResult))
      {
        // just getting the expose configure event
        anEvent = aResult;
      }

      this->SetEventSize (anEvent.xexpose.width, anEvent.xexpose.height);


      aXp = anEvent.xexpose.x;
      aYp = this->Size[1] - anEvent.xexpose.y - 1;
      this->SetEventPosition (aXp, aYp);
      
      // only render if we are currently accepting events
      if (this->Enabled)
      {
        this->InvokeEvent(vtkCommand::ExposeEvent,NULL);
        this->Render();
      }
    }
    break;

  case MapNotify:
    {
      // only render if we are currently accepting events
      if (this->Enabled && this->GetRenderWindow()->GetNeverRendered())
      {
        this->Render();
      }
    }
    break;

  case ConfigureNotify:
    {
      XEvent aResult;
      while (XCheckTypedWindowEvent(this->myDisplayId,
                                    this->myWindowId,
                                    ConfigureNotify,
                                    &aResult))
      {
        // just getting the last configure event
        anEvent = aResult;
      }
      Standard_Integer aWidth = anEvent.xconfigure.width;
      Standard_Integer aHeight = anEvent.xconfigure.height;
      if (aWidth != this->Size[0] || aHeight != this->Size[1])
      {
        Standard_Boolean toResizeSmaller = aWidth <= this->Size[0] && aHeight <= this->Size[1];
        this->UpdateSize (aWidth, aHeight);
        aXp = anEvent.xbutton.x;
        aYp = anEvent.xbutton.y;

        SetEventPosition (aXp, this->Size[1] - aYp - 1);

        // only render if we are currently accepting events
        if (Enabled)
        {
          this->InvokeEvent(vtkCommand::ConfigureEvent,NULL);
          if (toResizeSmaller)
          {
            // Don't call Render when the window is resized to be larger:
            //
            // - if the window is resized to be larger, an Expose event will
            // be trigged by the X server which will trigger a call to
            // Render().
            // - if the window is resized to be smaller, no Expose event will
            // be trigged by the X server, as no new area become visible.
            // only in this case, we need to explicitly call Render()
            // in ConfigureNotify.
            this->Render();
          }
        }
      }
    }
    break;

  case ButtonPress:
    {
      if (!this->Enabled)
      {
        return aPick;
      }
      
      Standard_Integer aCtrl = anEvent.xbutton.state & ControlMask ? 1 : 0;
      Standard_Integer aShift = anEvent.xbutton.state & ShiftMask ? 1 : 0;
      Standard_Integer anAlt = anEvent.xbutton.state & Mod1Mask ? 1 : 0;
      aXp = anEvent.xbutton.x;
      aYp = anEvent.xbutton.y;

      // check for double click
      static Standard_Integer aMousePressTime = 0;
      Standard_Integer aRepeat = 0;
      // 400 ms threshold by default is probably good to start
      Standard_Integer anEventTime = static_cast<int>(anEvent.xbutton.time);
      if ((anEventTime - aMousePressTime) < 400)
      {
        aMousePressTime -= 2000;  // no double click next time
        aRepeat = 1;
      }
      else
      {
        aMousePressTime = anEventTime;
      }

      this->SetEventInformationFlipY (aXp, aYp, aCtrl, aShift, 0, aRepeat);
      this->SetAltKey (anAlt);

      switch (anEvent.xbutton.button)
      {
        case Button1:
          this->OnSelection ();
          this->myIsLeftButtonPressed = 1;
          this->InvokeEvent (vtkCommand::LeftButtonPressEvent,NULL);
          break;
        case Button2:
          this->InvokeEvent (vtkCommand::MiddleButtonPressEvent,NULL);
          break;
        case Button3:
          this->InvokeEvent (vtkCommand::RightButtonPressEvent,NULL);
          break;
        case Button4:
          this->InvokeEvent (vtkCommand::MouseWheelForwardEvent,NULL);
          break;
        case Button5:
          this->InvokeEvent (vtkCommand::MouseWheelBackwardEvent,NULL);
          break;
      }
      this->Render();
    }
    break;

  case ButtonRelease:
    {
      if (!this->Enabled)
      {
        return aPick;
      }
      Standard_Integer aCtrl = anEvent.xbutton.state & ControlMask ? 1 : 0;
      Standard_Integer aShift = anEvent.xbutton.state & ShiftMask ? 1 : 0;
      Standard_Integer anAlt = anEvent.xbutton.state & Mod1Mask ? 1 : 0;
      aXp = anEvent.xbutton.x;
      aYp = anEvent.xbutton.y;
      
      this->SetEventInformationFlipY (aXp, aYp, aCtrl, aShift);
      this->SetAltKey (anAlt);
      switch (anEvent.xbutton.button)
      {
        case Button1:
          this->InvokeEvent (vtkCommand::LeftButtonReleaseEvent,NULL);
          this->myIsLeftButtonPressed = False;
          break;
        case Button2:
          this->InvokeEvent (vtkCommand::MiddleButtonReleaseEvent,NULL);
          break;
        case Button3:
          this->InvokeEvent (vtkCommand::RightButtonReleaseEvent,NULL);
          break;
      }
      this->Render();
    }
    break;

  case EnterNotify:
    {
      if (this->Enabled)
      {
        XEnterWindowEvent *anEnterEvent = reinterpret_cast<XEnterWindowEvent *>(&anEvent);
        this->SetEventInformationFlipY (anEnterEvent->x,
                                        anEnterEvent->y,
                                        (anEnterEvent->state & ControlMask) != 0,
                                        (anEnterEvent->state & ShiftMask) != 0);
                                        
        this->SetAltKey (anEvent.xbutton.state & Mod1Mask ? 1 : 0);
        this->InvokeEvent (vtkCommand::EnterEvent, NULL);
      }
      this->Render();
    }
    break;

  case LeaveNotify:
    {
      if (this->Enabled)
      {
        XLeaveWindowEvent *aLeaveEvent = reinterpret_cast<XLeaveWindowEvent *>(&anEvent);
        this->SetEventInformationFlipY (aLeaveEvent->x,
                                        aLeaveEvent->y,
                                        (aLeaveEvent->state & ControlMask) != 0,
                                        (aLeaveEvent->state & ShiftMask) != 0);
                                        
        this->SetAltKey (anEvent.xbutton.state & Mod1Mask ? 1 : 0);
        this->InvokeEvent(vtkCommand::LeaveEvent, NULL);
      }
      this->Render();
    }
    break;

  case MotionNotify:
    {
      if (!this->Enabled)
      {
        return aPick;
      }
      
      Standard_Integer aCtrl = anEvent.xbutton.state & ControlMask ? 1 : 0;
      Standard_Integer aShift = anEvent.xbutton.state & ShiftMask ? 1 : 0;
      Standard_Integer anAlt = anEvent.xbutton.state & Mod1Mask ? 1 : 0;

      // Note that even though the (x,y) location of the pointer is event structure,
      // we must call XQueryPointer for the hints (motion event compression) to
      // work properly.
      this->GetMousePosition (&aXp, &aYp);
      this->SetEventInformationFlipY (aXp, aYp, aCtrl, aShift);
      this->SetAltKey (anAlt);
      if (!myIsLeftButtonPressed)
        MoveTo (aXp, this->Size[1]- aYp - 1); 
      this->InvokeEvent (vtkCommand::MouseMoveEvent, NULL);
    }
    break;
  }

  return aPick;
}

//===========================================================
// Function : ProcessEvents
// Purpose  :
//===========================================================
void IVtkDraw_Interactor::ProcessEvents (ClientData theData, int)
{
  IVtkDraw_Interactor *anInteractor = (IVtkDraw_Interactor *)theData;
  // test for X Event
  while (XPending(anInteractor->GetDisplayId()))
  {
    anInteractor->ViewerMainLoop (0, NULL);
  }
}

#endif
