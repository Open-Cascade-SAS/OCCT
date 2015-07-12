// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <IFSelect_SessionPilot.hxx>
#include <XSControl.hxx>
#include <XSControl_Vars.hxx>
#include <XSControl_WorkSession.hxx>

Handle(XSControl_WorkSession)  XSControl::Session
  (const Handle(IFSelect_SessionPilot)& pilot)
      {  return Handle(XSControl_WorkSession)::DownCast(pilot->Session());  }


    Handle(XSControl_Vars)  XSControl::Vars
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(XSControl_Vars) avars;
  Handle(XSControl_WorkSession) WS = XSControl::Session(pilot);
  if (!WS.IsNull()) avars = WS->Vars();
  return avars;
}
