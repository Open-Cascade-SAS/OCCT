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

#include <IGESSelect.ixx>
#include <IFSelect_Functions.hxx>
#include <IFSelect_SessionPilot.hxx>
#include <IGESSelect_Activator.hxx>
#include <IFSelect_WorkSession.hxx>
#include <IFSelect_ShareOut.hxx>
#include <IGESSelect_WorkLibrary.hxx>

#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <Interface_Macros.hxx>


void  IGESSelect::Run ()
{
//  Handle(IFSelect_BasicActivator) Activator = new IFSelect_BasicActivator;
  IFSelect_Functions::Init();
  Handle(IFSelect_SessionPilot)   pilot = new IFSelect_SessionPilot("XSTEP-IGES>");
  Handle(IGESSelect_Activator)    igesact = new IGESSelect_Activator;
  pilot->SetSession (new IFSelect_WorkSession ( ));
  pilot->SetLibrary (new IGESSelect_WorkLibrary);

  pilot->ReadScript();
}


Standard_Integer  IGESSelect::WhatIges
  (const Handle(IGESData_IGESEntity)& ent, const Interface_Graph& G,
   Handle(IGESData_IGESEntity)& /* sup */, Standard_Integer& /* index */)
{
  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
  if (igesent.IsNull()) return Standard_False;
//  Standard_Integer igt = igesent->TypeNumber();
  DeclareAndCast(IGESData_IGESModel,model,G.Model());
  if (igesent.IsNull() || model.IsNull()) return 0;

//  Plane : de View ? de SingleParent ?  sinon cf TrimmedSurface & cie

    

  return 0;
}
