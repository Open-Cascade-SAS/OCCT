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


#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <XSControl_SelectForTransfer.hxx>
#include <XSControl_TransferReader.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XSControl_SelectForTransfer,IFSelect_SelectExtract)

XSControl_SelectForTransfer::XSControl_SelectForTransfer ()    {  }

    XSControl_SelectForTransfer::XSControl_SelectForTransfer
  (const Handle(XSControl_TransferReader)& TR)
      {  theTR = TR;  }


    void  XSControl_SelectForTransfer::SetReader
  (const Handle(XSControl_TransferReader)& TR)
      {  theTR = TR;  }

    void  XSControl_SelectForTransfer::SetActor
  (const Handle(Transfer_ActorOfTransientProcess)& act)
      {  theAC = act;  }

    Handle(Transfer_ActorOfTransientProcess)  XSControl_SelectForTransfer::Actor () const
      {  return theAC;  }

    Handle(XSControl_TransferReader)  XSControl_SelectForTransfer::Reader () const
      {  return theTR;  }

    Standard_Boolean  XSControl_SelectForTransfer::Sort
  (const Standard_Integer /*rank*/, const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& /*model*/) const
{
  Handle(Transfer_ActorOfTransientProcess) act = theAC;
  if (act.IsNull() && !theTR.IsNull()) act = theTR->Actor();
  if (!act.IsNull()) return act->Recognize(ent);//,theTR->TransientProcess());//act->Recognize(ent);
  return Standard_False;
}

 
    TCollection_AsciiString  XSControl_SelectForTransfer::ExtractLabel () const
{
  if (!theTR.IsNull()) return TCollection_AsciiString
    ("Recognized for Transfer (current actor)");
  return TCollection_AsciiString("Recognized for Transfer");
}
