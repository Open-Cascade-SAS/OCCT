// Created on: 2012-05-28 
// 
// Copyright (c) 2012-2014 OPEN CASCADE SAS 
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

#ifndef _IVtkDraw_Interactor_HeaderFile
#define _IVtkDraw_Interactor_HeaderFile

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Transient.hxx>

#ifdef _WIN32
#include <windows.h>
#else
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Shell.h>
#include <X11/Xutil.h>
#include <tk.h>
#endif

// prevent disabling some MSVC warning messages by VTK headers 
#include <Standard_WarningsDisable.hxx>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <Standard_WarningsRestore.hxx>

#include <IVtkTools_ShapePicker.hxx>
#include <IVtkDraw_HighlightAndSelectionPipeline.hxx>
#include <Aspect_Window.hxx>

class vtkWin32RenderWindowInteractor;
typedef vtkSmartPointer<IVtkTools_ShapePicker> PSelector;

class IVtkDraw_Interactor : public vtkRenderWindowInteractor
{
public:
  static IVtkDraw_Interactor *New();

  vtkTypeMacro (IVtkDraw_Interactor, vtkRenderWindowInteractor)

  virtual void Initialize();
  virtual void Enable();
  virtual void Start() { }

  const PSelector& Selector() const { return mySelector; }
  void SetShapePicker (const PSelector& theSelector);
  void SetPipelines (const Handle(ShapePipelineMap)& thePipelines);
  void SetOCCWindow (const Handle(Aspect_Window)& theWindow);
  const Handle(Aspect_Window)& GetOCCWindow() const;

  //! Process highlighting
  void MoveTo (Standard_Integer theX, Standard_Integer theY);

  //! Process selection
  void OnSelection();

  Standard_Boolean IsEnabled() const;

#ifndef _WIN32
  Display* GetDisplayId() const;
  Standard_Integer ViewerMainLoop (Standard_Integer theArgNum, const char** theArgs);
#endif

protected:
  IVtkDraw_Interactor();
  ~IVtkDraw_Interactor();

#ifdef _WIN32
  friend LRESULT CALLBACK WndProc (HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);
  friend LRESULT CALLBACK ViewerWindowProc (HWND hwnd,
                                            UINT Msg,
                                            WPARAM wParam,
                                            LPARAM lParam,
                                            IVtkDraw_Interactor *theInteractor);

  void OnMouseMove (HWND wnd, UINT nFlags, Standard_Integer X, Standard_Integer Y);
  void OnRButtonDown (HWND wnd, UINT nFlags, Standard_Integer X, Standard_Integer Y, Standard_Integer repeat=0);
  void OnRButtonUp (HWND wnd, UINT nFlags, Standard_Integer X, Standard_Integer Y);
  void OnMButtonDown (HWND wnd, UINT nFlags, Standard_Integer X, Standard_Integer Y, Standard_Integer repeat=0);
  void OnMButtonUp (HWND wnd, UINT nFlags, Standard_Integer X, Standard_Integer Y);
  void OnLButtonDown (HWND wnd, UINT nFlags, Standard_Integer X, Standard_Integer Y, Standard_Integer repeat=0);
  void OnLButtonUp (HWND wnd, UINT nFlags, Standard_Integer X, Standard_Integer Y);
  void OnSize (HWND wnd, UINT nType,  Standard_Integer X, Standard_Integer Y);
  void OnTimer (HWND wnd, UINT nIDEvent);
  void OnMouseWheelForward (HWND wnd, UINT nFlags, Standard_Integer X, Standard_Integer Y);
  void OnMouseWheelBackward (HWND wnd, UINT nFlags, Standard_Integer X, Standard_Integer Y);
#else
  static void ProcessEvents (ClientData theData, int);
  void GetMousePosition (Standard_Integer *theX, Standard_Integer *theY);
#endif

private:
  // copying is prohibited
  IVtkDraw_Interactor (const IVtkDraw_Interactor&);
  void operator = (const IVtkDraw_Interactor&);

private:

#ifdef _WIN32
  HWND                     myWindowId;
  Standard_Integer         myMouseInWindow;
#else
  Window                   myWindowId;
  Display                 *myDisplayId;
  Standard_Boolean         myIsLeftButtonPressed;
#endif

  PSelector                mySelector;
  Handle(ShapePipelineMap) myPipelines;
  Handle(Aspect_Window)    myWindow;

};

#endif
