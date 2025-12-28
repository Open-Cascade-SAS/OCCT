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
#include <Standard_Type.hxx>
#include <StepData_FileProtocol.hxx>
#include <StepData_Protocol.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_FileProtocol, StepData_Protocol)

// static TCollection_AsciiString  thename("");
static const char* thename = ""; // Empty schema name for file protocols

//  Protocol factory created on demand with other Protocols

StepData_FileProtocol::StepData_FileProtocol() {}

void StepData_FileProtocol::Add(const occ::handle<StepData_Protocol>& protocol)
{
  // Add a protocol to the collection, avoiding duplicates of the same type
  if (protocol.IsNull())
    return;
  occ::handle<Standard_Type> ptype = protocol->DynamicType();
  int                        nb    = thecomps.Length();
  // Check if a protocol of the same type is already present
  for (int i = 1; i <= nb; i++)
  {
    if (thecomps.Value(i)->IsInstance(ptype))
      return; // Protocol of this type already exists
  }
  thecomps.Append(protocol);
}

int StepData_FileProtocol::NbResources() const
{
  // Return the number of component protocols in this file protocol
  return thecomps.Length();
}

occ::handle<Interface_Protocol> StepData_FileProtocol::Resource(const int num) const
{
  return occ::down_cast<Interface_Protocol>(thecomps.Value(num));
}

int StepData_FileProtocol::TypeNumber(const occ::handle<Standard_Type>& /*atype*/) const
{
  // FileProtocol doesn't recognize specific types directly (delegates to component protocols)
  return 0;
}

bool StepData_FileProtocol::GlobalCheck(const Interface_Graph&        G,
                                        occ::handle<Interface_Check>& ach) const
{
  // Perform global validation check across all component protocols
  bool res = false;
  int  i, nb = NbResources();
  for (i = 1; i <= nb; i++)
    res |= Resource(i)->GlobalCheck(G, ach); // Aggregate results from all protocols
  return res;
}

const char* StepData_FileProtocol::SchemaName(const occ::handle<Interface_InterfaceModel>&) const
{
  return thename;
}
