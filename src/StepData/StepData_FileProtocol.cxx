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


#include <Interface_Check.hxx>
#include <Interface_Graph.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Type.hxx>
#include <StepData_FileProtocol.hxx>
#include <StepData_Protocol.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_FileProtocol,StepData_Protocol)

//static TCollection_AsciiString  thename("");
static Standard_CString  thename = "";

//  Protocol fabrique a la demande avec d autres Protocoles


    StepData_FileProtocol::StepData_FileProtocol ()    {  }

    void StepData_FileProtocol::Add (const Handle(StepData_Protocol)& protocol)
{
  if (protocol.IsNull()) return;
  Handle(Standard_Type) ptype = protocol->DynamicType();
  Standard_Integer nb = thecomps.Length();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (thecomps.Value(i)->IsInstance(ptype)) return;
  }
  thecomps.Append(protocol);
}


    Standard_Integer  StepData_FileProtocol::NbResources () const
      {  return thecomps.Length();  }

    Handle(Interface_Protocol) StepData_FileProtocol::Resource
  (const Standard_Integer num) const
      {  return Handle(Interface_Protocol)::DownCast(thecomps.Value(num));  }


    Standard_Integer  StepData_FileProtocol::TypeNumber
  (const Handle(Standard_Type)& /*atype*/) const
      {  return 0;  }


Standard_Boolean StepData_FileProtocol::GlobalCheck(const Interface_Graph& G,
                                                    Handle(Interface_Check)& ach) const
{
  Standard_Boolean res = Standard_False;
  Standard_Integer i,nb = NbResources();
  for (i = 1; i <= nb; i ++) res |= Resource(i)->GlobalCheck (G,ach);
  return res;
}


    Standard_CString StepData_FileProtocol::SchemaName () const
      {  return thename;  }
