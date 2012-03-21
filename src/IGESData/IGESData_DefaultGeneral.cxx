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

#include <IGESData_DefaultGeneral.ixx>
#include <IGESData_UndefinedEntity.hxx>
#include <IGESData_FreeFormatEntity.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <Interface_UndefinedContent.hxx>
#include <Interface_GeneralLib.hxx>
#include <IGESData.hxx>
#include <IGESData_Protocol.hxx>
#include <Interface_Macros.hxx>



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
