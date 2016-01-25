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


#include <IFSelect_Functions.hxx>
#include <IFSelect_SessionPilot.hxx>
#include <IFSelect_ShareOut.hxx>
#include <IFSelect_WorkSession.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESSelect.hxx>
#include <IGESSelect_Activator.hxx>
#include <IGESSelect_WorkLibrary.hxx>
#include <Interface_Graph.hxx>
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
  Handle(IGESData_IGESEntity) igesent = ent;
  if (igesent.IsNull()) return Standard_False;
//  Standard_Integer igt = igesent->TypeNumber();
  DeclareAndCast(IGESData_IGESModel,model,G.Model());
  if (igesent.IsNull() || model.IsNull()) return 0;

//  Plane : de View ? de SingleParent ?  sinon cf TrimmedSurface & cie

    

  return 0;
}
