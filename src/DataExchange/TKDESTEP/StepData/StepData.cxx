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

#include <StepData.hxx>

#include <Interface_Macros.hxx>
#include <Interface_Statics.hxx>
#include <StepData_DefaultGeneral.hxx>
#include <StepData_FileProtocol.hxx>
#include <StepData_Protocol.hxx>

StaticHandle(StepData_Protocol, proto);
// svv #2: StaticHandle(StepData_DefaultGeneral,stmod);

StaticHandleA(StepData_Protocol, theheader);

void StepData::Init() {}

Handle(StepData_Protocol) StepData::Protocol()
{
  InitHandleVoid(StepData_Protocol, proto); // svv #2
                                            //  UseHandle(StepData_Protocol,proto);
  return proto;
}

void StepData::AddHeaderProtocol(const Handle(StepData_Protocol)& header)
{
  InitHandle(StepData_Protocol, theheader);
  if (theheader.IsNull())
    theheader = header;
  else
  {
    DeclareAndCast(StepData_FileProtocol, headmult, theheader);
    if (headmult.IsNull())
    {
      headmult = new StepData_FileProtocol;
      headmult->Add(theheader);
    }
    headmult->Add(header);
    theheader = headmult;
  }
}

Handle(StepData_Protocol) StepData::HeaderProtocol()
{
  UseHandle(StepData_Protocol, theheader);
  return theheader;
}
