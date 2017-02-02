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


#include <Interface_CopyControl.hxx>
#include <Interface_CopyMap.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_GeneralModule.hxx>
#include <Interface_InterfaceError.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ReportEntity.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_HAsciiString.hxx>

// Se souvenir qu une version plus riche de CopyTool existe : c est
// TransferDispatch (package Transfer). Cette classe offre beaucoup plus de
// possibilite (parametrage des actions, gestion du Mapping ...)
// Mais le principe (transfert en 2 passes) reste le meme, a savoir :
// Passe 1 normale : les entites a transferer sont designees, elles entrainent
// leurs sous-references vraies
// Passe 2 : une fois tous les transferts faits, les relations "Imply" sont
// mises, pour les entites designees ET QUI ONT ETE AUSSI TRANSFEREES, la
// relation est reconduite (pas de nouveau Share)
//  #####################################################################
//  ....                        CONSTRUCTEURS                        ....
Interface_CopyTool::Interface_CopyTool
  (const Handle(Interface_InterfaceModel)& amodel,
   const Interface_GeneralLib& lib)
    : thelib (lib) , thelst (amodel->NbEntities())
{
  thelst.Init(Standard_False);
  themod = amodel;
  themap = new Interface_CopyMap (amodel);
  therep = new Interface_CopyMap (amodel);
  thelev = 0;  theimp = Standard_False;
}

    Interface_CopyTool::Interface_CopyTool
  (const Handle(Interface_InterfaceModel)& amodel,
   const Handle(Interface_Protocol)& protocol)
    : thelib (protocol) , thelst (amodel->NbEntities())
{
  thelst.Init(Standard_False);
  themod = amodel;
  themap = new Interface_CopyMap (amodel);
  therep = new Interface_CopyMap (amodel);
  thelev = 0;  theimp = Standard_False;
}


    Interface_CopyTool::Interface_CopyTool
  (const Handle(Interface_InterfaceModel)& amodel)
    : thelib (Interface_Protocol::Active()) , thelst (amodel->NbEntities())
{
  if (Interface_Protocol::Active().IsNull()) throw Interface_InterfaceError("Interface CopyTool : Create with Active Protocol undefined");

  thelst.Init(Standard_False);
  themod = amodel;
  themap = new Interface_CopyMap (amodel);
  therep = new Interface_CopyMap (amodel);
  thelev = 0;  theimp = Standard_False;
}

    Handle(Interface_InterfaceModel)  Interface_CopyTool::Model () const
      {  return themod;  }

    void Interface_CopyTool::SetControl
  (const Handle(Interface_CopyControl)& othermap)
      {  themap = othermap;  }

    Handle(Interface_CopyControl)  Interface_CopyTool::Control () const
      {  return themap;  }


//  #####################################################################
//  ....                    Actions Individuelles                    ....

    void Interface_CopyTool::Clear ()
{
  themap->Clear();
  therep->Clear();
  thelev = 0;  theimp = Standard_False;
  therts.Clear();
  ClearLastFlags();
}

    Standard_Boolean Interface_CopyTool::NewVoid
  (const Handle(Standard_Transient)& entfrom,
   Handle(Standard_Transient)& entto)
{
  if (entfrom == theent) {
    if (themdu.IsNull()) return Standard_False;
    return themdu->NewVoid(theCN,entto);
  }
  theent = entfrom;
  Standard_Boolean res = thelib.Select (entfrom,themdu,theCN);
  if (res)   res   = themdu->NewVoid (theCN,entto);
  if (!res)  res   = themdu->NewCopiedCase (theCN,entfrom,entto,*this);
//  if (!res) entto = entfrom->ShallowCopy();   sorry, nothing more possible
  return res;
}


    Standard_Boolean Interface_CopyTool::Copy
  (const Handle(Standard_Transient)& entfrom,
   Handle(Standard_Transient)& entto,
   const Standard_Boolean mapped, const Standard_Boolean errstat)
{
  Standard_Boolean res = Standard_True;
  if (entfrom == theent) {
    if (themdu.IsNull()) res = Standard_False;
  } else {
    theent = entfrom;
    res = thelib.Select(entfrom,themdu,theCN);
  }
  if (!res) {
//  Built-in :
    if (entfrom.IsNull()) return res;
    if (entfrom->DynamicType() == STANDARD_TYPE(TCollection_HAsciiString)) {
      entto = new TCollection_HAsciiString
	( Handle(TCollection_HAsciiString)::DownCast(entfrom)->ToCString() );
      res = Standard_True;
    }
    return res;
  }
//  On cree l Entite vide (NewVoid), la Copie reste a faire
  res = NewVoid(entfrom,entto);
  if (mapped) themap->Bind (entfrom,entto);    // Mapper avant de continuer ...

//  A present, on effectue la Copie (selon cas; si ShallowCopy ne suffit pas :
//  c est <themdu> qui decide)

//    Une Entite en Erreur n est pas copiee (pas de sens et c est risque ...)
//    Cependant, elle est "Copiee a Vide (NewVoid)" donc referencable
  if (!errstat)    themdu->CopyCase(theCN,entfrom,entto,*this);
  return res;
}

    void  Interface_CopyTool::Implied
  (const Handle(Standard_Transient)& entfrom,
   const Handle(Standard_Transient)& entto)
{
  Handle(Interface_GeneralModule) module;
  Standard_Integer CN;
  if (thelib.Select(entfrom,module,CN))
    module->RenewImpliedCase(CN,entfrom,entto,*this);
}


//  ....                Alimentation de la Map                ....

    Handle(Standard_Transient) Interface_CopyTool::Transferred
  (const Handle(Standard_Transient)& ent)
{
  Handle(Standard_Transient) res;
  if (ent.IsNull()) return res;    // Copie d un Null : tres simple ...
  Standard_Integer nument = themod->Number(ent);

//  <nument> == 0 -> Peut etre une sous-partie non partagee ...
//  On accepte mais on se protege contre un bouclage
  if (nument == 0 && thelev > 100) throw Interface_InterfaceError("CopyTool : Transferred, Entity is not contained in Starting Model");
  if (!themap->Search(ent,res)) {       // deja transfere ? sinon, le faire

//  On opere la Copie (enfin, on tente)
//  En cas d echec, rien n est enregistre
    if (!Copy(ent,res, (nument != 0), themod->IsRedefinedContent(nument) ))
      return res;

    thelev ++;
    if (nument != 0) thelst.SetTrue (nument);
    Handle(Interface_ReportEntity) rep;
    if (nument != 0) rep = themod->ReportEntity (nument);
    if (!rep.IsNull()) {
//  ATTENTION ATTENTION, si ReportEntity : Copier aussi Content et refaire une
//  ReportEntity avec les termes initiaux
      if (rep->IsUnknown()) therep->Bind
	(ent, new Interface_ReportEntity(res));
      else {
	Handle(Standard_Transient) contfrom, contto;
	contfrom = rep->Content();
	Handle(Interface_ReportEntity) repto =
	  new Interface_ReportEntity (rep->Check(),res);
	if (!contfrom.IsNull()) {
	  if (contfrom == ent) contto = res;
	  else  Copy (contfrom,contto, themod->Contains(contfrom), Standard_False);
	  repto->SetContent (contto);
	}
	therep->Bind (ent,repto);
      }
    }
//    Gerer le niveau d imbrication (0 = racine du transfert)
    thelev --;
  }
  if (thelev == 0 && nument > 0) therts.Append(nument);
  return res;
}

    void Interface_CopyTool::Bind
  (const Handle(Standard_Transient)& ent,
   const Handle(Standard_Transient)& res)
{
  Standard_Integer num = themod->Number(ent);
  themap->Bind (ent,res);
  thelst.SetTrue (num);
}

    Standard_Boolean Interface_CopyTool::Search
  (const Handle(Standard_Transient)& ent,
   Handle(Standard_Transient)& res) const
      {  return themap->Search (ent,res);  }

//  ##    ##    ##    ##    ##    ##    ##    ##    ##    ##    ##    ##    ##
//                              LastFlag

    void Interface_CopyTool::ClearLastFlags ()
      {  thelst.Init(Standard_False);  }

    Standard_Integer  Interface_CopyTool::LastCopiedAfter
  (const Standard_Integer numfrom,
   Handle(Standard_Transient)& ent, Handle(Standard_Transient)& res) const
{
  Standard_Integer nb = thelst.Length();
  for (Standard_Integer num = numfrom + 1; num <= nb; num ++) {
    if (thelst.Value(num)) {
      ent = themod->Value(num);
      if (themap->Search(ent,res)) return num;
    }
  }
  return 0;
}


//  #########################################################################
//  ....                        Actions Generales                        ....

    void Interface_CopyTool::TransferEntity
  (const Handle(Standard_Transient)& ent)
      {  Handle(Standard_Transient) res = Transferred(ent);  }

    void Interface_CopyTool::RenewImpliedRefs ()
{
  if (theimp) return;    // deja fait
  theimp = Standard_True;

//  Transfert Passe 2 : recuperation des relations non "Share" (mais "Imply")
//  c-a-d portant sur des entites qui ont pu ou non etre transferees
//  (Et que la 1re passe n a pas copie mais laisse en Null)
//  N.B. : on devrait interdire de commander des nouveaux transferts ...

  Standard_Integer nb = themod->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(Standard_Transient) ent = themod->Value(i);
    Handle(Standard_Transient) res;
    if (!themap->Search(ent,res)) continue;        // entite pas transferee
//    Reconduction des references "Imply".  Attention, ne pas copier si non chargee
    Handle(Standard_Transient) aRep;
    if (!therep->Search(ent,aRep))
    {
      Implied (ent,res);
    }
    else 
    {
      Handle(Interface_ReportEntity) rep = Handle(Interface_ReportEntity)::DownCast (aRep);
      if (! rep.IsNull() && ! rep->HasNewContent())
        Implied (ent,res);
    }
  }
}


    void Interface_CopyTool::FillModel
  (const Handle(Interface_InterfaceModel)& bmodel)
{
//  Travaux preparatoires concernant les modeles
//  On commence : cela implique le Header
  bmodel->Clear();
  bmodel->GetFromAnother(themod);

//  Transfert Passe 1 : On prend les Entites prealablement copiees
  Interface_EntityIterator list = CompleteResult(Standard_True);
  bmodel->GetFromTransfer(list);

//  Transfert Passe 2 : recuperation des relations non "Share" (mais "Imply")
  RenewImpliedRefs();
}


    Interface_EntityIterator Interface_CopyTool::CompleteResult
  (const Standard_Boolean withreports) const
{
  Interface_EntityIterator iter;
  Standard_Integer nb = themod->NbEntities();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(Standard_Transient) ent = themod->Value(i);
    Handle(Standard_Transient) res;
    if (!themap->Search(ent,res)) continue;
    if (withreports) {
      Handle(Standard_Transient) rep;
      if (therep->Search(ent,rep)) res = rep;
    }
    iter.GetOneItem(res);
  }
  return iter;
}

    Interface_EntityIterator Interface_CopyTool::RootResult
  (const Standard_Boolean withreports) const
{
  Interface_EntityIterator iter;
  Standard_Integer nb = therts.Length();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Standard_Integer j = therts.Value(i);
    Handle(Standard_Transient) ent = themod->Value(j);
    Handle(Standard_Transient) res;
    if (!themap->Search(ent,res)) continue;
    if (withreports) {
      Handle(Standard_Transient) rep;
      if (therep->Search(ent,rep)) res = rep;
    }
    iter.GetOneItem(res);
  }
  return iter;
}

//=======================================================================
//function : ~Interface_CopyTool
//purpose  : Destructor
//=======================================================================

Interface_CopyTool::~Interface_CopyTool()
{
}
