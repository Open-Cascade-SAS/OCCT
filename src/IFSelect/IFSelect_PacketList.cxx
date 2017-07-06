// Created on: 1994-09-02
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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


#include <IFSelect_PacketList.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_InterfaceError.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HSequenceOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_PacketList,Standard_Transient)

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
  if (num == 0) throw Interface_InterfaceError("PacketList:Add, Entity not in Model");
  if (thelast == 0) throw Interface_InterfaceError("PacketList:Add, no Packet yet added");
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
