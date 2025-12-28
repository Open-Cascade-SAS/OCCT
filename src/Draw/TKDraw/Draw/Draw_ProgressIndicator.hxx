// Created on: 2008-06-25
// Created by: data exchange team
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#ifndef _Draw_ProgressIndicator_HeaderFile
#define _Draw_ProgressIndicator_HeaderFile

#include <Standard.hxx>

#include <Message_ProgressIndicator.hxx>
#include <Standard_ThreadId.hxx>
#include <Draw_Interpretor.hxx>

//! Implements ProgressIndicator (interface provided by Message)
//! for DRAW, with possibility to output to TCL window
//! and/or trace file
class Draw_ProgressIndicator : public Message_ProgressIndicator
{

public:
  //! Creates a progress indicator and remembers pointer to Draw_Interpretor
  //!
  //! @param theUpdateThreshold defines minimal progress (in percents) between
  //! updates of the indicator (non-forced updates of the progress bar will be
  //! disabled until that progress is reached since last update).
  Standard_EXPORT Draw_ProgressIndicator(const Draw_Interpretor& di,
                                         double                  theUpdateThreshold = 1.);

  //! Destructor; calls Reset()
  Standard_EXPORT ~Draw_ProgressIndicator() override;

  //! Sets tcl output mode (on/off).
  Standard_EXPORT void SetTclMode(const bool theTclMode);

  //! Gets tcl output mode (on/off).
  Standard_EXPORT bool GetTclMode() const;

  //! Sets console output mode (on/off).
  //! If it is on then progress is shown in the standard output.
  Standard_EXPORT void SetConsoleMode(const bool theMode);

  //! Gets console output mode (on/off)
  Standard_EXPORT bool GetConsoleMode() const;

  //! Sets graphical output mode (on/off)
  Standard_EXPORT void SetGraphMode(const bool theGraphMode);

  //! Gets graphical output mode (on/off)
  Standard_EXPORT bool GetGraphMode() const;

  //! Clears/erases opened TCL windows if any
  //! and sets myBreak to False
  Standard_EXPORT void Reset() override;

  //! Defines method Show of Progress Indicator
  Standard_EXPORT void Show(const Message_ProgressScope& theScope,
                                    const bool                   force = true) override;

  //! Redefines method UserBreak of Progress Indicator
  Standard_EXPORT bool UserBreak() override;

  //! Get/Set default value for tcl mode
  Standard_EXPORT static bool& DefaultTclMode();

  //! Get/Set default value for console mode
  Standard_EXPORT static bool& DefaultConsoleMode();

  //! Get/Set default value for graph mode
  Standard_EXPORT static bool& DefaultGraphMode();

  //! Internal method for implementation of UserBreak mechanism;
  //! note that it uses static variable and thus not thread-safe!
  Standard_EXPORT static void*& StopIndicator();

  DEFINE_STANDARD_RTTIEXT(Draw_ProgressIndicator, Message_ProgressIndicator)

private:
  bool              myTclMode;
  bool              myConsoleMode;
  bool              myGraphMode;
  Draw_Interpretor* myDraw;
  bool              myShown;
  bool              myBreak;
  double            myUpdateThreshold;
  double            myLastPosition;
  size_t            myStartTime;
  Standard_ThreadId myGuiThreadId;
};

#endif // _Draw_ProgressIndicator_HeaderFile
