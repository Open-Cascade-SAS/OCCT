// Created on: 1998-12-09
// Created by: Xuan PHAM PHU
// Copyright (c) 1998-1999 Matra Datavision
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


#include <TopoDS_Shape.hxx>
#include <TopOpeBRepTool_connexity.hxx>
#include <TopOpeBRepTool_define.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#define M_FORWARD(sta)  (sta == TopAbs_FORWARD)
#define M_REVERSED(sta) (sta == TopAbs_REVERSED)
#define M_INTERNAL(sta) (sta == TopAbs_INTERNAL)
#define M_EXTERNAL(sta) (sta == TopAbs_EXTERNAL)

#define FORWARD  (1)
#define REVERSED (2)
#define INTERNAL (3)
#define EXTERNAL (4)
#define CLOSING  (5)

//=======================================================================
//function : TopOpeBRepTool_connexity
//purpose  : 
//=======================================================================

TopOpeBRepTool_connexity::TopOpeBRepTool_connexity()
: theItems(1,5)
{
}
//=======================================================================
//function : TopOpeBRepTool_connexity
//purpose  : 
//=======================================================================

TopOpeBRepTool_connexity::TopOpeBRepTool_connexity(const TopoDS_Shape& Key)
: theKey(Key), theItems(1,5)
{
}

//=======================================================================
//function : SetKey
//purpose  : 
//=======================================================================

void TopOpeBRepTool_connexity::SetKey(const TopoDS_Shape& Key)
{
  theKey = Key;
}



//=======================================================================
//function : Key
//purpose  : 
//=======================================================================

const TopoDS_Shape& TopOpeBRepTool_connexity::Key() const
{
  return theKey;
}

/*static Standard_Integer FUN_toI(const TopAbs_Orientation& O)
{
  Standard_Integer Index = 0;
  if      (O == TopAbs_FORWARD)  Index = 1;
  else if (O == TopAbs_REVERSED) Index = 2;
  else if (O == TopAbs_INTERNAL) Index = 3;
  else if (O == TopAbs_EXTERNAL) Index = 0;
  return Index;
}*/

//=======================================================================
//function : Item
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepTool_connexity::Item(const Standard_Integer OriKey, TopTools_ListOfShape& Item) const
{
  Item.Clear();
  Item = theItems(OriKey);
  return (Item.Extent());
}

//=======================================================================
//function : AllItems
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepTool_connexity::AllItems(TopTools_ListOfShape& Item) const
{
  Item.Clear();
  for (Standard_Integer i = 1; i <=4; i++) {
    TopTools_ListOfShape copy; copy.Assign(theItems.Value(i));
    Item.Append(copy);
  }
  return Item.Extent();
}

//=======================================================================
//function : AddItem
//purpose  : 
//=======================================================================

void TopOpeBRepTool_connexity::AddItem(const Standard_Integer OriKey, const TopTools_ListOfShape& Item)
{
  TopTools_ListOfShape copy; copy.Assign(Item);
  theItems(OriKey).Append(copy);
}

void TopOpeBRepTool_connexity::AddItem(const Standard_Integer OriKey, const TopoDS_Shape& Item)
{
  TopTools_ListOfShape copy; copy.Append(Item);
  theItems(OriKey).Append(copy);
}

//=======================================================================
//function : RemoveItem
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_connexity::RemoveItem(const Standard_Integer OriKey, const TopoDS_Shape& Item)
{
  TopTools_ListOfShape& item = theItems.ChangeValue(OriKey);
  TopTools_ListIteratorOfListOfShape it(item);
  while (it.More()) {
    if (it.Value().IsEqual(Item)) {item.Remove(it); return Standard_True;}
    else it.Next();
  }
  return Standard_False;
}

//=======================================================================
//function : RemoveItem
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_connexity::RemoveItem(const TopoDS_Shape& Item)
{
  Standard_Boolean removed = Standard_False;
  for (Standard_Integer i = 1; i<= 5; i++) {
    Standard_Boolean found = RemoveItem(i,Item);
    if (found) removed = Standard_True;
  }
  return removed;
}



//=======================================================================
//function : ChangeItem
//purpose  : 
//=======================================================================

TopTools_ListOfShape& TopOpeBRepTool_connexity::ChangeItem(const Standard_Integer OriKey)
{  
  return theItems.ChangeValue(OriKey);
}

//=======================================================================
//function : IsMultiple
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_connexity::IsMultiple() const
{
  TopTools_ListOfShape lfound; 
  Standard_Integer nkeyitem = Item(FORWARD,lfound);
//  nkeyRitem += Item(INTERNAL,lfound); NOT VALID
  // if key is vertex : key appears F in closing E, only one time
  nkeyitem += Item(CLOSING,lfound); 
  Standard_Boolean multiple = (nkeyitem > 1);
  return multiple;
}

//=======================================================================
//function : IsFaulty
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_connexity::IsFaulty() const
{
  TopTools_ListOfShape lfound; 
  Standard_Integer nkeyRintem = Item(FORWARD,lfound);
  Standard_Integer nkeyFitem  = Item(REVERSED,lfound);
  Standard_Boolean faulty = (nkeyRintem != nkeyFitem);
  return faulty;
}

//=======================================================================
//function : IsInternal
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepTool_connexity::IsInternal(TopTools_ListOfShape& Item) const
{
  Item.Clear();

  // all subshapes of INTERNAL(EXTERNAL) are oriented INTERNAL(EXTERNAL)
  TopTools_ListOfShape lINT; lINT.Assign(theItems.Value(INTERNAL));
  TopTools_ListIteratorOfListOfShape it1(lINT);
  while (it1.More()) {
    const TopoDS_Shape& item1 = it1.Value();
    TopAbs_Orientation o1 = item1.Orientation();
    if (!M_INTERNAL(o1)) {it1.Next(); continue;}
    Standard_Integer oKey1 = TopOpeBRepTool_TOOL::OriinSor(theKey,item1.Oriented(TopAbs_FORWARD));
    if (oKey1 != INTERNAL) lINT.Remove(it1);
    else it1.Next();
  }

  TopTools_ListOfShape lEXT; lEXT.Assign(theItems.Value(EXTERNAL));
  TopTools_ListIteratorOfListOfShape it2(lEXT);
  while (it2.More()) {
    const TopoDS_Shape& item2 = it2.Value();
    TopAbs_Orientation o2 = item2.Orientation();
    if (!M_EXTERNAL(o2)) {it2.Next(); continue;}
    Standard_Integer oKey2 = TopOpeBRepTool_TOOL::OriinSor(theKey,item2.Oriented(TopAbs_FORWARD));
    if (oKey2 == INTERNAL) lINT.Append(item2);
    it2.Next();
  }

  Item.Append(lINT);  
  return Item.Extent();
}



