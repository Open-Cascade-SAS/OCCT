// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepData_FileProtocol.ixx>


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
  (const Handle(Standard_Type)& atype) const
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
