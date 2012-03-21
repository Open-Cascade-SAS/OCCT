// Created on: 1994-09-02
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



#include <IFSelect_PacketList.ixx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <Interface_InterfaceError.hxx>



IFSelect_PacketList::IFSelect_PacketList
  (const Handle(Interface_InterfaceModel)& model)
    : thedupls (0,model->NbEntities()) , 
      thepacks (100) ,
      theflags (0,model->NbEntities()) ,
      thename  ("Packets")
{
  themodel = model;  thelast = 0;  thebegin = Standard_False;  // begin-begin
  thedupls.Init(0);  theflags.Init(0);
}

    void  IFSelect_PacketList::SetName (const Standard_CString name)
      {  thename.Clear();  thename.AssignCat (name);  }

    Standard_CString  IFSelect_PacketList::Name () const
      {  return thename.ToCString();  }

    Handle(Interface_InterfaceModel)  IFSelect_PacketList::Model () const
      {  return themodel;  }

    void  IFSelect_PacketList::AddPacket ()
{
  Standard_Integer nbl = thepacks.NbEntities();
  Standard_Integer nbe = theflags.Upper();
  for (Standard_Integer i = 1; i <= nbe; i ++) theflags.SetValue(i,0);

  if (thelast >= nbl) thepacks.SetNbEntities (nbl*2);

  if (!thebegin) thelast ++;
  thepacks.SetNumber (thelast);
  thebegin = Standard_False;
}


    void  IFSelect_PacketList::Add
  (const Handle(Standard_Transient)& ent)
{
  Standard_Integer num = themodel->Number(ent);
  if (num == 0) Interface_InterfaceError::Raise
    ("PacketList:Add, Entity not in Model");
  if (thelast == 0) Interface_InterfaceError::Raise
    ("PacketList:Add, no Packet yet added");
  if (theflags(num) != 0) return;
  theflags(num) = 1;
  thedupls(num) ++;
  thepacks.Add(num);
  thebegin = Standard_False;
}

    void  IFSelect_PacketList::AddList
  (const Handle(TColStd_HSequenceOfTransient)& list)
{
  if (list.IsNull()) return;
  Standard_Integer i , nb = list->Length();
  thepacks.Reservate (nb+1);
  for (i = 1; i <= nb; i ++) Add (list->Value(i));
}


    Standard_Integer  IFSelect_PacketList::NbPackets () const
      {  return (thebegin ? thelast-1 : thelast);  }

    Standard_Integer  IFSelect_PacketList::NbEntities
  (const Standard_Integer numpack) const
{
  if (numpack <= 0 || numpack > NbPackets()) return 0;
  Interface_IntList lisi(thepacks,Standard_False);  lisi.SetNumber (numpack);
  return lisi.Length();
}

    Interface_EntityIterator  IFSelect_PacketList::Entities
  (const Standard_Integer numpack) const
{
  Interface_EntityIterator list;
  if (numpack <= 0 || numpack > NbPackets()) return list;
  Interface_IntList lisi(thepacks,Standard_False);  lisi.SetNumber (numpack);
  Standard_Integer i , nb = lisi.Length();
  for (i = 1; i <= nb; i ++)
    list.AddItem(themodel->Value(lisi.Value(i)));
  return list;
}

    Standard_Integer  IFSelect_PacketList::HighestDuplicationCount () const
{
  Standard_Integer i , nb = themodel->NbEntities();
  Standard_Integer high = 0;
  for (i = 1; i <= nb; i ++) {
    Standard_Integer j = thedupls.Value(i);
    if (j > high) high = j;
  }
  return high;
}

    Standard_Integer  IFSelect_PacketList::NbDuplicated
  (const Standard_Integer newcount, const Standard_Boolean andmore) const
{
  Standard_Integer i, nb = themodel->NbEntities();
  Standard_Integer nbdu = 0;

  for (i = 1; i <= nb; i ++) {
    Standard_Integer j = thedupls.Value(i);
    if (j == newcount || (j > newcount && andmore)) nbdu ++;
  }
  return nbdu;
}

    Interface_EntityIterator  IFSelect_PacketList::Duplicated
  (const Standard_Integer newcount, const Standard_Boolean andmore) const
{
  Standard_Integer nb = themodel->NbEntities();
  Interface_EntityIterator list;

  Standard_Integer i;
  for (i = 1; i <= nb; i ++) {
    Standard_Integer j = thedupls.Value(i);
    if (j == newcount || (j > newcount && andmore)) list.AddItem(themodel->Value(i));
  }
  return list;
}
