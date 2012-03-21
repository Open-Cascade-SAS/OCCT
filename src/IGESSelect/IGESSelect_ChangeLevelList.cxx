// Created on: 1994-08-25
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#include <IGESSelect_ChangeLevelList.ixx>
#include <IGESData_LevelListEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Interface_Macros.hxx>
#include <stdio.h>
#include <Interface_Check.hxx>


IGESSelect_ChangeLevelList::IGESSelect_ChangeLevelList ()
    :  IGESSelect_ModelModifier (Standard_True)    {  }

    Standard_Boolean  IGESSelect_ChangeLevelList::HasOldNumber () const
      {  return (!theold.IsNull());  }

    Handle(IFSelect_IntParam)  IGESSelect_ChangeLevelList::OldNumber () const
      {  return theold;  }

    void  IGESSelect_ChangeLevelList::SetOldNumber
  (const Handle(IFSelect_IntParam)& param)
      {  theold = param;  }

    Standard_Boolean  IGESSelect_ChangeLevelList::HasNewNumber () const
      {  return (!thenew.IsNull());  }

    Handle(IFSelect_IntParam)  IGESSelect_ChangeLevelList::NewNumber () const
      {  return thenew;  }

    void  IGESSelect_ChangeLevelList::SetNewNumber
  (const Handle(IFSelect_IntParam)& param)
      {  thenew = param;  }


    void  IGESSelect_ChangeLevelList::Performing
  (IFSelect_ContextModif& ctx,
   const Handle(IGESData_IGESModel)& ,
   Interface_CopyTool& ) const
{
  Standard_Integer oldl = 0;
  Standard_Boolean yaold = HasOldNumber();
  if (yaold) oldl = theold->Value();
  Standard_Boolean yanew = HasOldNumber();
  Standard_Integer newl = 0;
  if (yanew) newl = thenew->Value();
  if (oldl < 0) ctx.CCheck()->AddFail("ChangeLevelList : OldNumber negative");
  if (newl < 0) ctx.CCheck()->AddFail("ChangeLevelList : NewNumber negative");
  if (oldl < 0 || newl < 0) return;

  Handle(IGESData_LevelListEntity) nulist;
  for (ctx.Start(); ctx.More(); ctx.Next()) {
    DeclareAndCast(IGESData_IGESEntity,ent,ctx.ValueResult());
    if (ent.IsNull()) continue;
    if (ent->DefLevel() != IGESData_DefSeveral) continue;
    if (yaold && ent->Level() != oldl) continue;
    if (!yanew) {
      Handle(IGESData_LevelListEntity) list = ent->LevelList();
      if (list.IsNull()) continue;
      newl = (list->NbLevelNumbers() > 0 ? list->LevelNumber(1) : 0);
      if (newl < 0) newl = 0;
    }
    ent->InitLevel(nulist,newl);
    ctx.Trace();
  }
}

    TCollection_AsciiString  IGESSelect_ChangeLevelList::Label () const
{
  char labl[100];
  Standard_Integer oldl = 0;
  Standard_Boolean yaold = HasOldNumber();
  if (yaold) oldl = theold->Value();
  Standard_Boolean yanew = HasOldNumber();
  Standard_Integer newl = 0;
  if (yanew) newl = thenew->Value();

  if (yaold) sprintf(labl,"Changes Level Lists containing %d", oldl);
  else       sprintf(labl,"Changes all Level Lists in D.E. %d", oldl);
  TCollection_AsciiString label(labl);
  if (yanew) sprintf(labl," to Number %d",newl);
  else       sprintf(labl," to Number = first value in List");
  label.AssignCat(labl);
  return label;
}
