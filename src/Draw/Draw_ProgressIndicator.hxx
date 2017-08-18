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
#include <Draw_Interpretor.hxx>

class Draw_ProgressIndicator;
DEFINE_STANDARD_HANDLE(Draw_ProgressIndicator, Message_ProgressIndicator)

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
  Standard_EXPORT Draw_ProgressIndicator(const Draw_Interpretor& di, Standard_Real theUpdateThreshold = 1.);
  
  //! Destructor; calls Reset()
  Standard_EXPORT ~Draw_ProgressIndicator();
  
  //! Sets text output mode (on/off)
  Standard_EXPORT void SetTextMode (const Standard_Boolean theTextMode);
  
  //! Gets text output mode (on/off)
  Standard_EXPORT Standard_Boolean GetTextMode() const;
  
  //! Sets graphical output mode (on/off)
  Standard_EXPORT void SetGraphMode (const Standard_Boolean theGraphMode);
  
  //! Gets graphical output mode (on/off)
  Standard_EXPORT Standard_Boolean GetGraphMode() const;
  
  //! Clears/erases opened TCL windows if any
  //! and sets myBreak to False
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;
  
  //! Defines method Show of Progress Indicator
  Standard_EXPORT virtual Standard_Boolean Show (const Standard_Boolean force = Standard_True) Standard_OVERRIDE;
  
  //! Redefines method UserBreak of Progress Indicator
  Standard_EXPORT virtual Standard_Boolean UserBreak() Standard_OVERRIDE;
  
  Standard_EXPORT static Standard_Boolean& DefaultTextMode();
  
  //! Get/Set default values for output modes
  Standard_EXPORT static Standard_Boolean& DefaultGraphMode();
  
  //! Internal method for implementation of UserBreak mechanism;
  //! note that it uses static variable and thus not thread-safe! 
  Standard_EXPORT static Standard_Address& StopIndicator();

  DEFINE_STANDARD_RTTIEXT(Draw_ProgressIndicator,Message_ProgressIndicator)

private:
  Standard_Boolean myTextMode;
  Standard_Boolean myGraphMode;
  Standard_Address myDraw;
  Standard_Boolean myShown;
  Standard_Boolean myBreak;
  Standard_Real myUpdateThreshold;
  Standard_Real myLastPosition;
  Standard_Size myStartTime;
};

#endif // _Draw_ProgressIndicator_HeaderFile
