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

#include <IGESSelect_RebuildGroups.ixx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESBasic_Group.hxx>
#include <IGESBasic_GroupWithoutBackP.hxx>
#include <IGESBasic_OrderedGroup.hxx>
#include <IGESBasic_OrderedGroupWithoutBackP.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>



IGESSelect_RebuildGroups::IGESSelect_RebuildGroups ()
    : IGESSelect_ModelModifier (Standard_True)    {  }

    void  IGESSelect_RebuildGroups::Performing
  (IFSelect_ContextModif& ctx,
   const Handle(IGESData_IGESModel)& target,
   Interface_CopyTool& TC) const
{
//  On reconstruit les groupes qui peuvent l etre
//  Pour chaque groupe de l original, on regarde les composants transferes
//   (evt filtres par <ctx>)
//  Ensuite, silyena plus d une, on refait un nouveau groupe
  DeclareAndCast(IGESData_IGESModel,original,ctx.OriginalModel());
  Standard_Integer nbo = original->NbEntities();

//  Entites a prendre en compte pour la reconstruction
//  NB : Les groupes deja transferes ne sont bien sur pas reconstruits !
  TColStd_Array1OfInteger pris(0,nbo); pris.Init(0);
  for (ctx.Start(); ctx.More(); ctx.Next()) {
    pris.SetValue (original->Number(ctx.ValueOriginal()),1);
  }

  for (Standard_Integer i = 1; i <= nbo; i ++) {
    Handle(IGESData_IGESEntity) ent = original->Entity(i);
    if (ent->TypeNumber() != 402) continue;
    Standard_Integer casenum = 0;
    Handle(Standard_Transient) newent;
    Interface_EntityIterator newlist;
    if (TC.Search(ent,newent)) continue;    // deja passe
    if (ent->IsKind(STANDARD_TYPE(IGESBasic_Group))) {
      DeclareAndCast(IGESBasic_Group,g,ent);
      casenum = 1;
      Standard_Integer nbg = g->NbEntities();
      for (Standard_Integer ig = 1; ig <= nbg; ig ++) {
	if (TC.Search(g->Value(i),newent)) newlist.GetOneItem(newent);
      }
    }
    if (ent->IsKind(STANDARD_TYPE(IGESBasic_GroupWithoutBackP))) {
      DeclareAndCast(IGESBasic_GroupWithoutBackP,g,ent);
      casenum = 2;
      Standard_Integer nbg = g->NbEntities();
      for (Standard_Integer ig = 1; ig <= nbg; ig ++) {
	if (TC.Search(g->Value(i),newent)) newlist.GetOneItem(newent);
      }
    }
    if (ent->IsKind(STANDARD_TYPE(IGESBasic_OrderedGroup))) {
      DeclareAndCast(IGESBasic_OrderedGroup,g,ent);
      casenum = 3;
      Standard_Integer nbg = g->NbEntities();
      for (Standard_Integer ig = 1; ig <= nbg; ig ++) {
	if (TC.Search(g->Value(i),newent)) newlist.GetOneItem(newent);
      }
    }
    if (ent->IsKind(STANDARD_TYPE(IGESBasic_OrderedGroupWithoutBackP))) {
      DeclareAndCast(IGESBasic_OrderedGroupWithoutBackP,g,ent);
      casenum = 4;
      Standard_Integer nbg = g->NbEntities();
      for (Standard_Integer ig = 1; ig <= nbg; ig ++) {
	if (TC.Search(g->Value(i),newent)) newlist.GetOneItem(newent);
      }
    }
//  A present, reconstruire sil le faut
    if (newlist.NbEntities() <= 1) continue;   // 0 ou 1 : rien a refaire
    Handle(IGESData_HArray1OfIGESEntity) tab =
      new IGESData_HArray1OfIGESEntity(1,newlist.NbEntities());
    Standard_Integer ng = 0;
    for (newlist.Start(); newlist.More(); newlist.Next()) {
      ng ++;  tab->SetValue(ng,GetCasted(IGESData_IGESEntity,newlist.Value()));
    }
    switch (casenum) {
      case 1 : {
	Handle(IGESBasic_Group) g = new IGESBasic_Group;
	g->Init(tab);
	target->AddEntity(g);

//  Q : faut-il transferer le nom silyena un ?
      }
	break;
      case 2 : {
	Handle(IGESBasic_GroupWithoutBackP) g = new IGESBasic_GroupWithoutBackP;
	g->Init(tab);
	target->AddEntity(g);
      }
	break;
      case 3 : {
	Handle(IGESBasic_OrderedGroup) g = new IGESBasic_OrderedGroup;
	g->Init(tab);
	target->AddEntity(g);
      }
	break;
      case 4 : {
	Handle(IGESBasic_OrderedGroupWithoutBackP) g =
	  new IGESBasic_OrderedGroupWithoutBackP;
	g->Init(tab);
	target->AddEntity(g);
      }
	break;
      default : break;
    }
  }
}

    TCollection_AsciiString  IGESSelect_RebuildGroups::Label () const
{  return TCollection_AsciiString("Rebuild Groups");  }
