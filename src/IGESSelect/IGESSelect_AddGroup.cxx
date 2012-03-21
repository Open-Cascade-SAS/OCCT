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

#include <IGESSelect_AddGroup.ixx>
#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESBasic_Group.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Interface_Macros.hxx>



IGESSelect_AddGroup::IGESSelect_AddGroup ()
    : IGESSelect_ModelModifier (Standard_True)    {  }

    void  IGESSelect_AddGroup::Performing
  (IFSelect_ContextModif& ctx,
   const Handle(IGESData_IGESModel)& target,
   Interface_CopyTool& TC) const
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
