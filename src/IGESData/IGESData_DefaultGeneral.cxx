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


#include <IGESData.hxx>
#include <IGESData_DefaultGeneral.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_FreeFormatEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_Protocol.hxx>
#include <IGESData_UndefinedEntity.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Interface_UndefinedContent.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HSequenceOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESData_DefaultGeneral,IGESData_GeneralModule)

IGESData_DefaultGeneral::IGESData_DefaultGeneral ()
{  Interface_GeneralLib::SetGlobal(this, IGESData::Protocol());  }

    void  IGESData_DefaultGeneral::OwnSharedCase
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent,
   Interface_EntityIterator& iter) const
{
  if (CN == 0) return;
  DeclareAndCast(IGESData_UndefinedEntity,anent,ent);
  if (anent.IsNull()) return;
  Handle(Interface_UndefinedContent) cont = anent->UndefinedContent();
  Standard_Integer nb = cont->NbParams();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (cont->IsParamEntity(i)) iter.GetOneItem (cont->ParamEntity(i));
  }
}


    IGESData_DirChecker  IGESData_DefaultGeneral::DirChecker
  (const Standard_Integer , const Handle(IGESData_IGESEntity)& ) const 
{  IGESData_DirChecker dc; return dc;  }  // aucun critere specifique


    void  IGESData_DefaultGeneral::OwnCheckCase
  (const Standard_Integer , const Handle(IGESData_IGESEntity)& ,
   const Interface_ShareTool& , Handle(Interface_Check)& ) const 
{  }  // aucun critere specifique


    Standard_Boolean  IGESData_DefaultGeneral::NewVoid
  (const Standard_Integer CN, Handle(Standard_Transient)& entto) const
{
  entto.Nullify();
  if (CN == 0) return Standard_False;
  if (CN == 1) entto = new IGESData_UndefinedEntity;
  if (CN == 2) entto = new IGESData_FreeFormatEntity;
  return (!entto.IsNull());
}

    void  IGESData_DefaultGeneral::OwnCopyCase
  (const Standard_Integer CN,
   const Handle(IGESData_IGESEntity)& entfrom,
   const Handle(IGESData_IGESEntity)& entto,
   Interface_CopyTool& TC) const 
{
  if (CN == 0) return;
  DeclareAndCast(IGESData_UndefinedEntity,enfr,entfrom);
  DeclareAndCast(IGESData_UndefinedEntity,ento,entto);
//  ShallowCopy aura passe DirStatus
//  transmettre les contenus des UndefinedContents
  Handle(Interface_UndefinedContent) cont = new Interface_UndefinedContent;
  cont->GetFromAnother(enfr->UndefinedContent(),TC);
  ento->SetNewContent (cont);
//  FreeFormat, encore des choses
  if (enfr->IsKind(STANDARD_TYPE(IGESData_FreeFormatEntity))) {
    DeclareAndCast(IGESData_FreeFormatEntity,enf,entfrom);
    DeclareAndCast(IGESData_FreeFormatEntity,ent,entto);
    ent->ClearNegativePointers();
    ent->AddNegativePointers(enf->NegativePointers());
  }
}
