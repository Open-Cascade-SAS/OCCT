// Copyright (c) 2019-2020 OPEN CASCADE SAS
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

#ifndef _ViewerTest_ContinuousRedrawer_HeaderFile
#define _ViewerTest_ContinuousRedrawer_HeaderFile

#include <OSD_Thread.hxx>
#include <Standard_Condition.hxx>
#include <Standard_Mutex.hxx>
#include <Standard_Type.hxx>

class V3d_View;

//! Auxiliary tool performing continuous redraws of specified window.
//! Tool creates an extra working thread pushing content invalidation messages to specific window using Aspect_Window::InvalidateContent() method.
//! Normally, GUI application should done continuous rendering in simple fashion - just by drawing next frame without waiting for new events from windowing system;
//! however, implementation of this approach is problematic in context of ViewerTest due to message loop binding mechanism implied by Tcl/Tk.
class ViewerTest_ContinuousRedrawer
{
public:
  //! Return global instance.
  Standard_EXPORT static ViewerTest_ContinuousRedrawer& Instance();
public:

  //! Destructor.
  Standard_EXPORT ~ViewerTest_ContinuousRedrawer();

  //! Return TRUE if redrawer thread is started.
  bool IsStarted() const { return myThread.GetId() != 0; }

  //! Start thread.
  Standard_EXPORT void Start (const Handle(V3d_View)& theView,
                              Standard_Real theTargetFps);

  //! Stop thread.
  Standard_EXPORT void Stop (const Handle(V3d_View)& theView = NULL);

  //! Return TRUE if redrawer thread is in paused state.
  bool IsPaused() const { return myToPause; }

  //! Pause working thread, but does not terminate it.
  Standard_EXPORT void Pause();

private:

  //! Thread loop.
  void doThreadLoop();

  //! Thread creation callback.
  static Standard_Address doThreadWrapper (Standard_Address theData)
  {
    ViewerTest_ContinuousRedrawer* aThis = (ViewerTest_ContinuousRedrawer* )theData;
    aThis->doThreadLoop();
    return 0;
  }

  //! Empty constructor.
  ViewerTest_ContinuousRedrawer();

private:
  Handle(V3d_View)   myView;      //!< view to invalidate
  OSD_Thread         myThread;    //!< working thread
  Standard_Mutex     myMutex;     //!< mutex for accessing common variables
  Standard_Condition myWakeEvent; //!< event to wake up working thread
  Standard_Real      myTargetFps; //!< desired update framerate
  volatile bool      myToStop;    //!< flag to stop working thread
  volatile bool      myToPause;   //!< flag to put  working thread asleep without stopping
};

#endif // _ViewerTest_ContinuousRedrawer_HeaderFile
