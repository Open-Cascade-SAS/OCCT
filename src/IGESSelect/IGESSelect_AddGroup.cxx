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


#include <IFSelect_ContextModif.hxx>
#include <IGESBasic_Group.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESSelect_AddGroup.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_AddGroup,IGESSelect_ModelModifier)

IGESSelect_AddGroup::IGESSelect_AddGroup ()
    : IGESSelect_ModelModifier (Standard_True)    {  }

void  IGESSelect_AddGroup::Performing (IFSelect_ContextModif& ctx,
                                       const Handle(IGESData_IGESModel)& target,
                                       Interface_CopyTool& /*TC*/) const
{
  if (ctx.IsForAll()) {
    ctx.CCheck(0)->AddFail ("Add Group : Selection required not defined");
    return;
  }
  Interface_EntityIterator list = ctx.SelectedResult();
  Standard_Integer i = 0 , nb = list.NbEntities();
  if (nb == 0) {
    ctx.CCheck(0)->AddWarning ("Add Group : No entity selected");
    return;
  }
  if (nb == 1) {
    ctx.CCheck(0)->AddWarning ("Add Group : ONE entity selected");
    return;
  }
  Handle(IGESData_HArray1OfIGESEntity) arr =
    new IGESData_HArray1OfIGESEntity(1,nb);
  for (ctx.Start(); ctx.More(); ctx.Next()) {
    DeclareAndCast(IGESData_IGESEntity,ent,ctx.ValueResult());
    i ++;
    arr->SetValue(i,ent);
  }
  Handle(IGESBasic_Group) gr = new IGESBasic_Group;
  gr->Init (arr);
  target->AddEntity(gr);
}


    TCollection_AsciiString  IGESSelect_AddGroup::Label () const
      {  return TCollection_AsciiString ("Add Group");  }
