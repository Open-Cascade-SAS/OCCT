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

#include <Interface_ShareFlags.ixx>
#include <Interface_GeneralModule.hxx>
#include <Interface_ReportEntity.hxx>
#include <Interface_IntList.hxx>
#include <Standard_DomainError.hxx>



Interface_ShareFlags::Interface_ShareFlags
  (const Handle(Interface_InterfaceModel)& amodel,
   const Interface_GeneralLib& lib)
   : theflags (amodel->NbEntities())
{
  Handle(Interface_GTool) gtool;  // null
  themodel = amodel;
  Evaluate(lib,gtool);
}

    Interface_ShareFlags::Interface_ShareFlags
  (const Handle(Interface_InterfaceModel)& amodel,
   const Handle(Interface_GTool)& gtool)
   : theflags (amodel->NbEntities())
{
  themodel = amodel;
  Evaluate(gtool->Lib(),gtool);
}

    Interface_ShareFlags::Interface_ShareFlags
  (const Handle(Interface_InterfaceModel)& amodel,
   const Handle(Interface_Protocol)& protocol)
   : theflags (amodel->NbEntities())
{
  Handle(Interface_GTool) gtool;  // null
  themodel = amodel;
  Evaluate(Interface_GeneralLib(protocol),gtool);
}

    Interface_ShareFlags::Interface_ShareFlags
  (const Handle(Interface_InterfaceModel)& amodel)
   : theflags (amodel->NbEntities())
{
  Handle(Interface_GTool) gtool = themodel->GTool();
  gtool->Reservate(amodel->NbEntities());
  themodel = amodel;
  Evaluate (gtool->Lib(),gtool);
}

    Interface_ShareFlags::Interface_ShareFlags (const Interface_Graph& agraph)
      : theflags (agraph.Model()->NbEntities())
    {
      themodel = agraph.Model();
      Standard_Integer nb = themodel->NbEntities();
      if (nb == 0) return;
      theroots = new TColStd_HSequenceOfTransient();
      for (Standard_Integer i = 1; i <= nb; i ++) {
        //    Resultat obtenu depuis le Graph
        Handle(Standard_Transient) ent = themodel->Value(i);
        Handle(TColStd_HSequenceOfTransient) list = agraph.GetSharings(ent);
       
        if (!list.IsNull() && list->Length() > 0) theflags.SetTrue(i);
        else theroots->Append (ent);
      }
    }


    void  Interface_ShareFlags::Evaluate
  (const Interface_GeneralLib& lib, const Handle(Interface_GTool)& gtool)
{
  Standard_Boolean patool = gtool.IsNull();
  Standard_Integer nb = themodel->NbEntities();
  if (nb == 0) return;
  theroots = new TColStd_HSequenceOfTransient();
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= nb; i ++) {

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

//    Entites partagees par <ent> : reste a noter chacune comme "Shared"
    for (iter.Start(); iter.More(); iter.Next()) {
      Standard_Integer num = themodel->Number(iter.Value());
      theflags.SetTrue(num);    // Et Voila
    }
  }
  for (i = 1; i <= nb; i ++) {
    if (!theflags.Value(i)) theroots->Append (themodel->Value(i));
  }
}


    Handle(Interface_InterfaceModel)  Interface_ShareFlags::Model () const 
      {  return themodel;  }

    Standard_Boolean  Interface_ShareFlags::IsShared
  (const Handle(Standard_Transient)& ent) const 
{
  Standard_Integer num = themodel->Number(ent);
  if (num == 0 || num > themodel->NbEntities()) Standard_DomainError::Raise
    ("Interface ShareFlags : IsShared");
  return theflags.Value(num);
}

    Interface_EntityIterator  Interface_ShareFlags::RootEntities () const
{
  Interface_EntityIterator iter (theroots);
  return iter;
}

    Standard_Integer  Interface_ShareFlags::NbRoots () const
      {  return (theroots.IsNull() ? 0 : theroots->Length());  }

    Handle(Standard_Transient)  Interface_ShareFlags::Root
  (const Standard_Integer num) const
      {  return theroots->Value(num);  }
