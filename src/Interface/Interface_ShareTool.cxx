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

#include <Interface_ShareTool.ixx>
#include <Interface_InterfaceError.hxx>
#include <Interface_IntList.hxx>
#include <Interface_GeneralModule.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Message_Messenger.hxx>

Interface_ShareTool::Interface_ShareTool (const Handle(Interface_InterfaceModel)& amodel,
                                          const Interface_GeneralLib& lib)
{
  theHGraph = new Interface_HGraph(amodel,lib);
}

Interface_ShareTool::Interface_ShareTool (const Handle(Interface_InterfaceModel)& amodel,
                                          const Handle(Interface_GTool)& gtool)
{
  theHGraph = new Interface_HGraph(amodel,gtool);
}

Interface_ShareTool::Interface_ShareTool (const Handle(Interface_InterfaceModel)& amodel,
                                          const Handle(Interface_Protocol)& protocol)
{
  theHGraph = new Interface_HGraph(amodel,protocol);
}

Interface_ShareTool::Interface_ShareTool (const Handle(Interface_InterfaceModel)& amodel)
{
  theHGraph = new Interface_HGraph(amodel);
}

Interface_ShareTool::Interface_ShareTool (const Interface_Graph& agraph)
{
  theHGraph = new Interface_HGraph(agraph.Model());
}

Interface_ShareTool::Interface_ShareTool (const Handle(Interface_HGraph)& ahgraph)
{
  theHGraph = ahgraph;
}

//    Ajout des "Implied" sur toutes les Entites du Graphe
/*void Interface_ShareTool::AddImplied (const Handle(Interface_GTool)& gtool)
{
  Interface_Graph& thegraph = theHGraph->CGraph();
  Standard_Integer nb = thegraph.Size();
  Standard_Boolean yena = Standard_False;
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(Standard_Transient) ent = thegraph.Entity(i);
    if (ent.IsNull()) continue;
    Handle(Interface_GeneralModule) module;  Standard_Integer CN;
    if (gtool->Select(ent,module,CN)) {
      Interface_EntityIterator iter;
      module->ListImpliedCase(CN,ent,iter);
      if (iter.NbEntities() == 0) continue;
      yena = Standard_True;
      thegraph.SetShare(ent);
      for (iter.Start(); iter.More(); iter.Next())
	thegraph.AddShared(ent,iter.Value());
    }
  }
  if (yena) thegraph.EvalSharings();
}*/


    Handle(Interface_InterfaceModel) Interface_ShareTool::Model () const
      {  return theHGraph->Graph().Model();  }

    const Interface_Graph& Interface_ShareTool::Graph () const
      {  return theHGraph->Graph();  }

    Interface_EntityIterator  Interface_ShareTool::RootEntities () const
      {  return theHGraph->Graph().RootEntities();  }

    Standard_Boolean  Interface_ShareTool::IsShared
  (const Handle(Standard_Transient)& ent) const
{
  const Interface_Graph& thegraph = theHGraph->Graph();
  Handle(TColStd_HSequenceOfTransient) list =
    thegraph.GetShareds (ent);
  return (!list.IsNull() && list->Length() > 0);
}

    Interface_EntityIterator  Interface_ShareTool::Shareds
  (const Handle(Standard_Transient)& ent) const
      {  return theHGraph->Graph().Shareds(ent);  }

    Interface_EntityIterator  Interface_ShareTool::Sharings
  (const Handle(Standard_Transient)& ent) const
      {  return theHGraph->Graph().Sharings(ent);  }


    Standard_Integer Interface_ShareTool::NbTypedSharings
  (const Handle(Standard_Transient)& ent,
   const Handle(Standard_Type)& atype) const
{
  Interface_Graph& thegraph = theHGraph->CGraph();
  Handle(TColStd_HSequenceOfTransient) list = thegraph.GetSharings (ent);
  if(list.IsNull())
    return 0;

  Standard_Integer result = 0;
  Standard_Integer n = list->Length();
  for (Standard_Integer i = 1; i <= n; i ++) {
    Handle(Standard_Transient) entsh = list->Value(i);
    if (entsh.IsNull()) continue;
    if (entsh->IsKind(atype)) result ++;
  }
  return result;
}

    Handle(Standard_Transient) Interface_ShareTool::TypedSharing
  (const Handle(Standard_Transient)& ent,
   const Handle(Standard_Type)& atype) const
{
  Interface_Graph& thegraph = theHGraph->CGraph();
  Handle(TColStd_HSequenceOfTransient) list = thegraph.GetSharings(ent);
  if(list.IsNull())
    return 0;
  Handle(Standard_Transient) entresult;
  Standard_Integer result = 0;
  Standard_Integer n = list->Length();
  for (Standard_Integer i = 1; i <= n; i ++) {
    Handle(Standard_Transient) entsh = list->Value(i);
    if (entsh.IsNull()) continue;
    if (entsh->IsKind(atype)) {
      entresult = entsh;
      result ++;
      if (result > 1)  Interface_InterfaceError::Raise
	("Interface ShareTool : TypedSharing, more than one found");
    }
  }
  if (result == 0) Interface_InterfaceError::Raise
    ("Interface ShareTool : TypedSharing, not found");
  return entresult;
}

    Interface_EntityIterator  Interface_ShareTool::All
(const Handle(Standard_Transient)& ent, const Standard_Boolean rootlast) const
{
  Handle(Interface_InterfaceModel) model = Model();
  Interface_EntityIterator list;
  Standard_Integer i, n0 = 0, nb = model->NbEntities();
  Handle(TColStd_HArray1OfInteger) fl = new TColStd_HArray1OfInteger (0,nb);
  fl->Init(0);
  if (ent == model) {
//    On passe les racines en revue (l ordre de base est conserve)
    Interface_EntityIterator roots = RootEntities();
    for (roots.Start(); roots.More(); roots.Next()) {
      Interface_EntityIterator subl = All(roots.Value(),rootlast);
      for (subl.Start(); subl.More(); subl.Next()) {
	Standard_Integer nm = model->Number(subl.Value());
	if (fl->Value(nm) > 0) continue;
	n0 ++;  fl->SetValue(nm,n0);
      }
    }
//    Attention, y a t il des oublis ?      
    for (i = 1; i <= nb; i ++)
      if (fl->Value(i) == 0)  {  n0 ++;  fl->SetValue(i,n0);  }
  } else {
    Handle(TColStd_HSequenceOfTransient) sq = new TColStd_HSequenceOfTransient();
    sq->Append(ent);
//    processus de type file
    for (i = 1; i <= sq->Length(); i ++) {    // Length croit
      Handle(Standard_Transient) en = sq->Value(i);
      Standard_Integer num = model->Number(en);
      if (fl->Value(num) != 0) continue;  // deja vu
      n0 ++;  fl->SetValue (num,n0);
      Interface_EntityIterator sh = Shareds(en);
      sq->Append (sh.Content());
    }
  }
//    Reste a constituer la liste, retourner si necessaire
  Handle(TColStd_HArray1OfInteger) ord = new TColStd_HArray1OfInteger (0,nb);
  ord->Init(0);
  for (i = 1; i <= nb; i ++)  {  n0 = fl->Value(i);  ord->SetValue (n0,i);  }
  if (rootlast && ent != model)    for (i = 1; i <= nb; i ++)
      {  if (ord->Value(i) != 0) list.AddItem (model->Value(ord->Value(i)));  }
  else                             for (i = nb; i  > 0; i --)
      {  if (ord->Value(i) != 0) list.AddItem (model->Value(ord->Value(i)));  }

  return list;
}

    void  Interface_ShareTool::Print
  (const Interface_EntityIterator& iter, const Handle(Message_Messenger)& S) const
{
  S << " Nb.Entities : " << iter.NbEntities() << " : ";
  for (iter.Start(); iter.More(); iter.Next()) {
    Handle(Standard_Transient) ent = iter.Value();
    S << " n0/id:"; Model()->Print(ent,S);
  }
  S<<endl;
}
