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
