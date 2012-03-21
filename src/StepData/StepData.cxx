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

//    abv 09.04.99 S4136: eliminate parameter step.readaccept.void
//    svv #2    23.02.00: porting on SIL
#include <StepData.ixx>
# include <StepData_Protocol.hxx>
# include <StepData_DefaultGeneral.hxx>
# include <StepData_FileProtocol.hxx>
#include <Interface_Static.hxx>

#include <Interface_Statics.hxx>
#include <Interface_Macros.hxx>



StaticHandle(StepData_Protocol,proto);
//svv #2: StaticHandle(StepData_DefaultGeneral,stmod);

StaticHandleA(StepData_Protocol,theheader);


    void StepData::Init ()
{
//  InitHandleVoid(StepData_Protocol,proto);
//  InitHandleVoid(StepData_DefaultGeneral,stmod);
//:S4136  Interface_Static::Init("step","step.readaccept.void",'i',"1");
//  if (proto.IsNull()) proto = new StepData_Protocol;
//  if (stmod.IsNull()) stmod = new StepData_DefaultGeneral;
}

    Handle(StepData_Protocol) StepData::Protocol ()
{
  InitHandleVoid(StepData_Protocol,proto);// svv #2
//  UseHandle(StepData_Protocol,proto);
  return proto;
}


    void  StepData::AddHeaderProtocol (const Handle(StepData_Protocol)& header)
{
  InitHandle(StepData_Protocol,theheader);
  if (theheader.IsNull()) theheader = header;
  else {
    DeclareAndCast(StepData_FileProtocol,headmult,theheader);
    if (headmult.IsNull()) {
      headmult = new StepData_FileProtocol;
      headmult->Add(theheader);
    }
    headmult->Add(header);
    theheader = headmult;
  }
}

    Handle(StepData_Protocol) StepData::HeaderProtocol ()
{
  UseHandle(StepData_Protocol,theheader);
  return theheader;
}
