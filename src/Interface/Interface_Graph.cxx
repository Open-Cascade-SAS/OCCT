#include <Interface_Graph.ixx> 
#include <Interface_GeneralModule.hxx>
#include <Interface_ReportEntity.hxx>
#include <Standard_DomainError.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Interface_ShareTool.hxx>



// Flags : 0 = Presence, 1 = Sharing Error
#define Graph_Present 0
#define Graph_ShareError 1


//  ###########################################################################

//  ....                           CONSTRUCTEURS                           ....

//  ....       Construction a partir de la connaissance des Entites        ....


    Interface_Graph::Interface_Graph
  (const Handle(Interface_InterfaceModel)& amodel,
   const Interface_GeneralLib& lib)
   : themodel   (amodel), thepresents ("") , thestats (0,amodel->NbEntities()) , 
     theflags   (amodel->NbEntities(),2) ,  
     theshareds (amodel->NbEntities()) , /*thesharnews(amodel->NbEntities()) , */
     thesharings (amodel->NbEntities())
{
  Handle(Interface_GTool) gtool;  // null
  theflags.AddFlag ("ShareError");  // -> flag n0 1
  Evaluate(lib,gtool);
}

    Interface_Graph::Interface_Graph
  (const Handle(Interface_InterfaceModel)& amodel,
   const Handle(Interface_Protocol)& protocol)
   : themodel   (amodel) , thepresents ("") ,
     thestats (0,amodel->NbEntities()) ,      theflags   (amodel->NbEntities(),2) ,  
     theshareds (amodel->NbEntities()) ,      /*thesharnews(amodel->NbEntities()) ,*/
     thesharings (amodel->NbEntities())
{
  Handle(Interface_GTool) gtool;  // null
  theflags.AddFlag ("ShareError");  // -> flag n0 1
  Evaluate(Interface_GeneralLib(protocol),gtool);
}

    Interface_Graph::Interface_Graph
  (const Handle(Interface_InterfaceModel)& amodel,
   const Handle(Interface_GTool)& gtool)
   : themodel   (amodel) , thepresents ("") , 
     thestats (0,amodel->NbEntities()) ,  theflags   (amodel->NbEntities(),2) ,  
     theshareds (amodel->NbEntities()) , /*thesharnews(amodel->NbEntities()) , */
     thesharings (amodel->NbEntities()) 
{
  theflags.AddFlag ("ShareError");  // -> flag n0 1
  Evaluate(gtool->Lib(),gtool);
}

    Interface_Graph::Interface_Graph
  (const Handle(Interface_InterfaceModel)& amodel)
   : themodel   (amodel) , thepresents ("") , 
     thestats (0,amodel->NbEntities()) , theflags   (amodel->NbEntities(),2) ,  
     theshareds (amodel->NbEntities()) , 
     /*thesharnews(amodel->NbEntities()) ,*/ thesharings (amodel->NbEntities())
{
  theflags.AddFlag ("ShareError");  // -> flag n0 1
  Handle(Interface_GTool) gtool = amodel->GTool();
  if (gtool.IsNull()) return;
  gtool->Reservate(amodel->NbEntities());
  Evaluate (gtool->Lib(),gtool);
}

    void Interface_Graph::Evaluate
  (const Interface_GeneralLib& lib, const Handle(Interface_GTool)& gtool)
{
//  Evaluation d un Graphe de dependances : sur chaque Entite, on prend sa
//  liste "Shared". On en deduit les "Sharing"  directement

  Standard_Boolean patool = gtool.IsNull();
  Standard_Integer n = Size(), total = 0;
  theshareds.Clear();
  thesharings.Clear();
  TColStd_Array1OfInteger counts (0,n); counts.Init(0);
  TColStd_Array1OfInteger lasts  (0,n); lasts.Init(0);
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= n; i ++) {
    theshareds.SetNumber (i);

//    ATTENTION : Si Entite non chargee donc illisible, basculer sur son
//    "Contenu" equivalent
    Handle(Standard_Transient) ent = themodel->Value(i);
    if (themodel->IsRedefinedContent(i)) ent = themodel->ReportEntity(i)->Content();

//    Resultat obtenu via GeneralLib
    Interface_EntityIterator iter;
    Handle(Interface_GeneralModule) module;
    Standard_Integer CN;

    if (patool) {
      if (lib.Select(ent,module,CN))  module->FillShared(themodel,CN,ent,iter);
    } else {
      if (gtool->Select(ent,module,CN))  module->FillShared(themodel,CN,ent,iter);
    }

    theshareds.Reservate (iter.NbEntities());
//    Mise en forme : liste d entiers
    for (iter.Start(); iter.More(); iter.Next()) {
//    num = 0 -> on sort du Model de depart, le noter "Error" et passer
      Standard_Integer num = EntityNumber(iter.Value());
      if (num == 0)  theflags.SetTrue (i,Graph_ShareError);
      else {
//  controle d unicite de couple (pere-fils)
	if (lasts(num) == i) continue;
	total ++;
	lasts.ChangeValue(num) = i;
	theshareds.Add (num);
	counts.ChangeValue(num) ++;
//  References inverses : plus tard
      }
    }
  }

//  Les references inverses : preallocation (pour compte > 1 seulement)

  thesharings.SetNumber(0);  thesharings.Reservate (total);
  for (i = 1; i <= n; i ++) {
    if (counts(i) < 2) continue;
    thesharings.SetNumber(i);
    thesharings.Reservate (-counts(i));
  }
//  Enregistrement par inversion
  for (i = 1; i <= n; i ++) {
    theshareds.SetNumber (i);
    Standard_Integer j,num, nb = theshareds.Length();
    for (j = 1; j <= nb; j ++) {
      num = theshareds.Value(j);
      thesharings.SetNumber(-num);
      thesharings.Add(-i);
    }
  }

}

    void Interface_Graph::EvalSharings ()
{
  thesharings.Clear();
  Standard_Integer n = thesharings.NbEntities();

// Difference avec Evaluate : SharedNums prend les Redefinis si necessaire

  for (Standard_Integer i = 1; i <= n; i ++) {
    Standard_Integer nb, num;
    if (thesharnews.IsRedefined(i)) {
      thesharnews.SetNumber(i);
      nb = thesharnews.Length();
      for (Standard_Integer j = 1; j <= nb; j ++) {
	num = thesharnews.Value (j);
	thesharings.SetNumber (num);
	thesharings.Reservate (thesharings.Length()+1);
	thesharings.Add (i);
      }
    } else {
      theshareds.SetNumber(i);
      nb = theshareds.Length();
      for (Standard_Integer j = 1; j <= nb; j ++) {
	num = theshareds.Value (j);
	thesharings.SetNumber (num);
	thesharings.Reservate (thesharings.Length()+1);
	thesharings.Add (i);
      }
    }
  }
}


//  ....                Construction depuis un autre Graph                ....

    Interface_Graph::Interface_Graph
  (const Interface_Graph& agraph, const Standard_Boolean /*copied*/)
    : themodel   (agraph.Model()), thepresents ("") , 
      thestats (0,agraph.Size()) , 
      theflags   (agraph.BitMap(),Standard_True) , 
      theshareds  (agraph.BasicSharedTable(),Standard_True) ,
      thesharnews (agraph.RedefinedSharedTable(),Standard_True) ,
      thesharings (agraph.SharingTable(),Standard_True)
{
  Standard_Integer nb = Size();
  for (Standard_Integer i = 1; i <= nb; i ++)
    thestats.SetValue (i,agraph.Status(i));
}

    Interface_IntList  Interface_Graph::BasicSharedTable () const
      {  return theshareds;  }

    Interface_IntList  Interface_Graph::RedefinedSharedTable () const
      {  return thesharnews;  }

    Interface_IntList  Interface_Graph::SharingTable () const
      {  return thesharings;  }


//  ###########################################################################

//  ....                ACCES UNITAIRES AUX DONNEES DE BASE                ....

    void  Interface_Graph::Reset ()
{
  thestats.Init(0);
  theflags.Init (Standard_False, Graph_Present);
  thesharnews.Clear();
  EvalSharings();
}

    void  Interface_Graph::ResetStatus ()
      {    thestats.Init(0);  theflags.Init (Standard_False, Graph_Present);  }


    Standard_Integer  Interface_Graph::Size () const 
      {  return thestats.Upper();  }

    Standard_Integer  Interface_Graph::EntityNumber
  (const Handle(Standard_Transient)& ent) const 
     {  return themodel->Number(ent);  }

    Standard_Boolean  Interface_Graph::IsPresent
  (const Standard_Integer num) const 
{
  if (num <= 0 || num > Size()) return Standard_False;
  return theflags.Value (num,Graph_Present);
}

    Standard_Boolean  Interface_Graph::IsPresent
  (const Handle(Standard_Transient)& ent) const 
      {  return IsPresent(EntityNumber(ent));  }

    const Handle(Standard_Transient)&  Interface_Graph::Entity
  (const Standard_Integer num) const 
      {  return themodel->Value(num);  }


    Standard_Integer  Interface_Graph::Status (const Standard_Integer num) const
      {  return thestats.Value(num);  }

    Standard_Integer&  Interface_Graph::CStatus (const Standard_Integer num)
      {  return thestats.ChangeValue(num);  }

    void  Interface_Graph::SetStatus
  (const Standard_Integer num, const Standard_Integer stat)
      {  thestats.SetValue(num,stat);  }

    void  Interface_Graph::RemoveItem(const Standard_Integer num)
{
  thestats.SetValue(num,0);  theflags.SetFalse (num,Graph_Present);
}

    void  Interface_Graph::ChangeStatus
  (const Standard_Integer oldstat, const Standard_Integer newstat)
{
  Standard_Integer nb = thestats.Upper();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (thestats.Value(i) == oldstat) thestats.SetValue(i,newstat);
  }
}

    void  Interface_Graph::RemoveStatus(const Standard_Integer stat)
{
  Standard_Integer nb = thestats.Upper();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (thestats.Value(i) == stat) RemoveItem(i);
  }
}

    const Interface_BitMap&  Interface_Graph::BitMap () const
      {  return theflags;  }

    Interface_BitMap&  Interface_Graph::CBitMap ()
      {  return theflags;  }


//  ###########################################################################

//  ....      Chargements Elementaires avec Propagation de "Share"      .... //

    const Handle(Interface_InterfaceModel)&  Interface_Graph::Model() const 
      {  return themodel;  }

    void  Interface_Graph::GetFromModel ()
{
  if (themodel.IsNull()) return;    // no model ... (-> on n ira pas loin)
  theflags.Init (Standard_True,Graph_Present);
  thestats.Init (0);
}

    void  Interface_Graph::GetFromEntity
  (const Handle(Standard_Transient)& ent, const Standard_Boolean shared,
   const Standard_Integer newstat)
{
  Standard_Integer num = EntityNumber(ent);
  if (num == 0) return;
  if (theflags.CTrue(num,Graph_Present)) return;  // deja pris : on passe
  thestats.SetValue(num,newstat);
  if (!shared) return;
//  Attention a la redefinition !
  Interface_IntList list = thesharnews;
  if (!list.IsRedefined(num)) list = theshareds;
  list.SetNumber (num);

  Standard_Integer nb = list.Length();
  for (Standard_Integer i = 1; i <= nb; i ++)    GetFromEntity
    (themodel->Value (list.Value(i)),Standard_True,newstat);
}

    void  Interface_Graph::GetFromEntity
  (const Handle(Standard_Transient)& ent, const Standard_Boolean shared,
   const Standard_Integer newstat, const Standard_Integer overlapstat,
   const Standard_Boolean cumul)
{
  Standard_Integer num   = EntityNumber(ent);
  if (num == 0) return;
  Standard_Boolean pasla = !theflags.CTrue (num,Graph_Present);
  Standard_Integer stat  = thestats.Value(num); 

  if (pasla) {
///    theflags.SetTrue (num, Graph_Present);   // nouveau : noter avec newstat
    thestats.SetValue(num,newstat);
  } else {
    Standard_Integer overstat = stat;
    if (stat != newstat) {                   // deja pris, meme statut : passer
      if (cumul) overstat += overlapstat;    // nouveau statut : avec cumul ...
      else       overstat  = overlapstat;    // ... ou sans (statut force)
      if (stat != overstat)                  // si repasse deja faite, passer
	thestats.SetValue(num,overstat);
    }
  }
  if (!shared) return;
//  Attention a la redefinition !
  Interface_IntList list = thesharnews;
  if (!list.IsRedefined(num)) list = theshareds;
  list.SetNumber (num);
  Standard_Integer nb = list.Length();
  for (Standard_Integer i = 1; i <= nb; i ++)    GetFromEntity
    (themodel->Value (list.Value(i)),Standard_True,newstat);
}



    void  Interface_Graph::GetFromIter
  (const Interface_EntityIterator& iter, const Standard_Integer newstat)
{
  for (iter.Start(); iter.More(); iter.Next()) {
    Handle(Standard_Transient) ent = iter.Value();
    Standard_Integer num = EntityNumber(ent);
    if (num == 0) continue;
    if (theflags.CTrue(num,Graph_Present)) continue;
    thestats.SetValue(num,newstat);
  }
}


    void  Interface_Graph::GetFromIter
  (const Interface_EntityIterator& iter,
   const Standard_Integer newstat, const Standard_Integer overlapstat,
   const Standard_Boolean cumul)
{
  for (iter.Start(); iter.More(); iter.Next()) {
    Handle(Standard_Transient) ent = iter.Value();
    Standard_Integer num   = EntityNumber(ent);
    if (num == 0) continue;
    /*Standard_Boolean pasla = !*/theflags.Value(num,Graph_Present);
    /*Standard_Integer stat  = */thestats.Value(num); 
    GetFromEntity (ent,Standard_False,newstat,overlapstat,cumul);
  }
}


    void Interface_Graph::GetFromGraph (const Interface_Graph& agraph)
{
  if (Model() != agraph.Model()) Standard_DomainError::Raise
    ("Graph from Interface : GetFromGraph");
  Standard_Integer nb = Size();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (agraph.IsPresent(i))
      GetFromEntity (agraph.Entity(i),Standard_False,agraph.Status(i));
  }
}

    void Interface_Graph::GetFromGraph
  (const Interface_Graph& agraph, const Standard_Integer stat)
{
  if (Model() != agraph.Model()) Standard_DomainError::Raise
    ("Graph from Interface : GetFromGraph");
  Standard_Integer nb = Size();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (agraph.IsPresent(i) && agraph.Status(i) == stat)
      GetFromEntity (agraph.Entity(i),Standard_False,stat);
  }
}

//  #####################################################################

//  ....                Listage des Entites Partagees                ....

    Standard_Boolean Interface_Graph::HasShareErrors
  (const Handle(Standard_Transient)& ent) const
{
  Standard_Integer num   = EntityNumber(ent);
  if (num == 0) return Standard_True;
  return theflags.Value (num,Graph_ShareError);
}

//  A chaque entite, on peut attacher une Liste de numeros d entites partagees
//  Si elle est nulle, BasicShareds convient, sinon on prend RedefinedShareds

    Standard_Boolean Interface_Graph::HasRedefinedShareds
  (const Handle(Standard_Transient)& ent) const
{
  Standard_Integer num   = EntityNumber(ent);
  if (num == 0) return Standard_False;
  return thesharnews.IsRedefined (num);
}

    Interface_EntityIterator Interface_Graph::Shareds
  (const Handle(Standard_Transient)& ent) const
{
  Standard_Integer num   = EntityNumber(ent);
  if (num == 0)  Standard_DomainError::Raise ("Interface : Shareds");

  Interface_EntityIterator iter;
  Interface_IntList list = thesharnews;
  if (!list.IsRedefined(num)) list = theshareds;
  list.SetNumber (num);
  Standard_Integer nb = list.Length();
  for (Standard_Integer i = 1; i <= nb; i ++)
    iter.GetOneItem (themodel->Value (list.Value(i) ));
  return iter;
}

    Interface_IntList  Interface_Graph::SharedNums
  (const Standard_Integer num) const
{
  Interface_IntList list = thesharnews;
  if (!list.IsRedefined(num)) list = theshareds;
  list.SetNumber (num);
  return list;
}

    Interface_EntityIterator Interface_Graph::Sharings
  (const Handle(Standard_Transient)& ent) const
{
  Standard_Integer num   = EntityNumber(ent);
  if (num == 0) Standard_DomainError::Raise ("Interface : Sharings");

  Interface_EntityIterator iter;
  Interface_IntList list = thesharings;  list.SetNumber (num);
  Standard_Integer nb = list.Length();
  for (Standard_Integer i = 1; i <= nb; i ++)
    iter.GetOneItem (themodel->Value(list.Value(i)));
  return iter;
}

    Interface_IntList Interface_Graph::SharingNums
  (const Standard_Integer num) const
{
  Interface_IntList list = thesharings;
  if (num > 0) list.SetNumber (num);
  return list;
}


static void AddTypedSharings
  (const Handle(Standard_Transient)& ent, const Handle(Standard_Type)& type,
   Interface_EntityIterator& iter, const Standard_Integer n,
   const Interface_Graph& G)
{
  if (ent.IsNull()) return;
  if (ent->IsKind(type))  {  iter.AddItem (ent);  return;  }
  if (iter.NbEntities() > n) return;

  Interface_IntList list = G.SharingNums(G.EntityNumber(ent));
  Standard_Integer nb = list.Length();
  for (Standard_Integer i = 1; i <= nb; i ++)
    AddTypedSharings (G.Entity(list.Value(i)) ,type,iter,nb,G);
}

    Interface_EntityIterator Interface_Graph::TypedSharings
  (const Handle(Standard_Transient)& ent, const Handle(Standard_Type)& type) const
{
  Interface_EntityIterator iter;
  Standard_Integer n = Size();
  AddTypedSharings (ent,type,iter,n,*this);
  return iter;
}
  

    Interface_EntityIterator Interface_Graph::RootEntities () const
{
  Interface_EntityIterator iter;
  Standard_Integer nb = Size();
  Interface_IntList list (thesharings);
  for (Standard_Integer i = 1; i <= nb; i ++) {
    list.SetNumber (i);
    if (list.Length() == 0) iter.GetOneItem (themodel->Value(i));
  }
  return iter;
}


//  ######################################################################

//  ....                Redefinition des listes Shared-Sharing        ....

    void Interface_Graph::SetShare
  (const Handle(Standard_Transient)& ent)
{
  Standard_Integer num = EntityNumber(ent);
  if (num == 0) return;
  SetShare (ent,SharedNums(num));
}

    void Interface_Graph::SetShare
  (const Handle(Standard_Transient)& ent,
   const Interface_EntityIterator& list)
{
  if(!thesharnews.NbEntities())
    thesharnews.Initialize(themodel->NbEntities());
  Standard_Integer num   = EntityNumber(ent);
  if (num == 0) return;
  thesharnews.SetNumber (num);
  thesharnews.SetRedefined (Standard_True);
  thesharnews.Reservate (list.NbEntities());

  for (list.Start(); list.More(); list.Next()) {
    Standard_Integer nsh = EntityNumber(list.Value());
    if (nsh != 0) thesharnews.Add (nsh);
  }
}

    void Interface_Graph::SetShare
  (const Handle(Standard_Transient)& ent, const Interface_IntList& list)
{
  Standard_Integer num   = EntityNumber(ent);
  if (num == 0) return;
  if(!thesharnews.NbEntities())
    thesharnews.Initialize(themodel->NbEntities());
  Standard_Integer i, n = list.Length();
  thesharnews.SetNumber (num);
  thesharnews.SetRedefined (Standard_True);
  thesharnews.Reservate (n);
  for (i = 1; i <= n; i ++) {
    Standard_Integer nsh = list.Value(i);
    if (nsh != 0) thesharnews.Add (nsh);
  }
}

    void Interface_Graph::SetNoShare
  (const Handle(Standard_Transient)& ent)
{
  Standard_Integer num   = EntityNumber(ent);
  if (num == 0) return;
  if(!thesharnews.NbEntities()) return;
   
  thesharnews.SetNumber (num);
  thesharnews.SetRedefined (Standard_False);
}

    void Interface_Graph::SetNoShare (const Interface_EntityIterator& list)
{
  for (list.Start(); list.More(); list.Next()) SetNoShare(list.Value());
}

    void Interface_Graph::AddShared
  (const Handle(Standard_Transient)& ent,
   const Handle(Standard_Transient)& shared)
{
  //Standard_Integer i, nb; svv Feb21 2000 : porting on SIL
  Standard_Integer num   = EntityNumber(ent);
  Standard_Integer nsh   = EntityNumber(shared);
  
  if (!thesharnews.IsRedefined(num) || num == 0 || nsh == 0) Standard_DomainError::Raise
    ("Interface Graph : AddShared, cannot be applied");
  if(!thesharnews.NbEntities()) 
    thesharnews.Initialize(themodel->NbEntities());
//   Liste Shared(ent)
  thesharnews.SetNumber (num);
  thesharnews.Reservate (thesharnews.Length()+1);
  thesharnews.Add (nsh);

//  Liste Sharing(shared)
  thesharings.SetNumber (nsh);
  thesharings.Reservate (thesharings.Length()+1);
  thesharings.Add (num);
}


    void Interface_Graph::RemoveShared
  (const Handle(Standard_Transient)& ent,
   const Handle(Standard_Transient)& shared)
{
  Standard_Integer num   = EntityNumber(ent);
  Standard_Integer nsh   = EntityNumber(shared);
  if (!thesharnews.IsRedefined(num) || num == 0 || nsh == 0) Standard_DomainError::Raise
    ("Interface Graph : RemoveShared, cannot be applied");

//  Liste Shared (ent)
  if(!thesharnews.NbEntities()) 
    thesharnews.Initialize(themodel->NbEntities());
  
  thesharnews.SetNumber (num);
  Standard_Integer i,nbsh = thesharnews.Length();
  for (i = nbsh; i > 0; i --)
    if (thesharnews.Value(i) == nsh) thesharnews.Remove(i);

//  Liste Sharing (shared)
  thesharings.SetNumber (nsh);
  nbsh = thesharings.Length();
  for (i = nbsh; i > 0; i --)
    if (thesharings.Value(i) == num) thesharings.Remove(i);
}


    void Interface_Graph::ResetShare
  (const Handle(Standard_Transient)& ent)
{
  Standard_Integer num   = EntityNumber(ent);
  if (num == 0) return;

  Interface_IntList snew(thesharnews);
  
  if (!thesharnews.NbEntities() || !snew.IsRedefined(num)) return;
  snew.SetNumber (num);
  Interface_IntList sold(theshareds);
  sold.SetNumber (num);

//  Attention aux Sharings ... Il va falloir ajouter, supprimer ... laisser !
//  Tableau // Entites : 0 hors jeu  1 reapparait  2 disparait  3 reste 

  Standard_Integer n = Size();
  TColStd_Array1OfInteger oldnew(1,n); oldnew.Init(0);
  Standard_Integer ns = sold.Length();
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= ns; i ++)
    oldnew.SetValue(sold.Value(i),1);    // pourra passer en 3 plus tard ...
  ns = snew.Length();
  for (i = 1; i <= ns; i ++) {
    Standard_Integer oldstat = oldnew.Value(snew.Value(i));
    oldnew.SetValue(snew.Value(i),oldstat+2);  // 0 -> 2, 1 -> 3
  }

//  Muni de ces donnees, on pourra modifier les listes Sharings impliquees
  for (i = 1; i <= n; i ++) {
    Standard_Integer oldstat = oldnew.Value(snew.Value(i));
    if (oldstat == 0 || oldstat == 2) continue;
    thesharings.SetNumber(i);
    if (oldstat == 1) {
      thesharings.Reservate (thesharings.Length()+1);
      thesharings.Add (num);
    } else if (oldstat == 3) {
      Standard_Integer j,nbsh = thesharings.Length();
      for (j = nbsh; j > 0; j --)
	if (thesharings.Value(j) == num) thesharings.Remove(j);
    }
  }

//  Shared : beaucoup plus simple, ANNULER Redefined
  thesharnews.SetRedefined (Standard_False);
}

    void Interface_Graph::ResetAllShare ()
{
  thesharnews.Clear();
  EvalSharings();
}


    Handle(TCollection_HAsciiString)  Interface_Graph::Name
  (const Handle(Standard_Transient)& ent) const
{
  Handle(TCollection_HAsciiString) str;
  if (themodel.IsNull()) return str;
  if (themodel->Number(ent)) return str;

  Handle(Interface_GTool) gtool = themodel->GTool();
  if (gtool.IsNull()) return str;

  Handle(Interface_GeneralModule) module;
  Standard_Integer CN;
  if (!gtool->Select(ent,module,CN)) return str;

  Interface_ShareTool sht (*this);
  return module->Name (CN,ent,sht);
}
