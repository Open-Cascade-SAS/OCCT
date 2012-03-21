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

#include <StepData_DescrGeneral.ixx>
#include <StepData_EDescr.hxx>
#include <StepData_Described.hxx>


StepData_DescrGeneral::StepData_DescrGeneral
  (const Handle(StepData_Protocol)& proto)
    :  theproto (proto)    {  }


    void  StepData_DescrGeneral::FillSharedCase
  (const Standard_Integer CN, const Handle(Standard_Transient)& ent,
   Interface_EntityIterator& iter) const
{
  Handle(StepData_Described) ds = Handle(StepData_Described)::DownCast(ent);
  if (!ds.IsNull()) ds->Shared (iter);
}


void StepData_DescrGeneral::CheckCase(const Standard_Integer CN,
                                      const Handle(Standard_Transient)& ent,
                                      const Interface_ShareTool& shares,
                                      Handle(Interface_Check)& ach) const
{
}    // pour l instant


    void  StepData_DescrGeneral::CopyCase
  (const Standard_Integer CN, const Handle(Standard_Transient)& entfrom,
   const Handle(Standard_Transient)& entto, Interface_CopyTool& TC) const
{  }    // pour l instant

    Standard_Boolean  StepData_DescrGeneral::NewVoid
  (const Standard_Integer CN, Handle(Standard_Transient)& ent) const
{
  ent = theproto->Descr(CN)->NewEntity();
  return (!ent.IsNull());
}
