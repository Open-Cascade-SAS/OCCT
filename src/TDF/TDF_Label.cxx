// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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


#include <Standard_DomainError.hxx>
#include <Standard_GUID.hxx>
#include <Standard_ImmutableObject.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Data.hxx>
#include <TDF_IDFilter.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelMapHasher.hxx>
#include <TDF_LabelNode.hxx>
#include <TDF_LabelNodePtr.hxx>
#include <TDF_Tool.hxx>

// Attribute methods ++++++++++++++++++++++++++++++++++++++++++++++++++++
//=======================================================================
//function : Imported
//purpose  : Sets imported and all its descendants.
//=======================================================================
void TDF_Label::Imported(const Standard_Boolean aStatus) const
{
  if (IsNull()) throw Standard_NullObject("A null Label has no status.");
  if (myLabelNode->IsImported() != aStatus) {
    myLabelNode->Imported(aStatus);
    for (TDF_ChildIterator itr(*this, Standard_True);
         itr.More(); itr.Next())
      itr.Value().myLabelNode->Imported(aStatus);
  }
}


//=======================================================================
//function : FindAttribute
//purpose  : Finds an attributes according to an ID.
//=======================================================================

Standard_Boolean TDF_Label::FindAttribute
(const Standard_GUID& anID,
 Handle(TDF_Attribute)& anAttribute) const
{
  if (IsNull()) throw Standard_NullObject("A null Label has no attribute.");
  TDF_AttributeIterator itr (myLabelNode); // Without removed attributes.
  for ( ; itr.More(); itr.Next()) {
    if (itr.Value()->ID() == anID) {
      anAttribute = itr.Value();
      return Standard_True;
    }
  }
  return Standard_False;
}


//=======================================================================
//function : FindAttribute
//purpose  : Finds an attributes according to an ID and a Transaction.
//=======================================================================

Standard_Boolean TDF_Label::FindAttribute
(const Standard_GUID& anID,
 const Standard_Integer aTransaction,
 Handle(TDF_Attribute)& anAttribute) const
{
  Handle(TDF_Attribute) locAtt;
  if (FindAttribute(anID, locAtt)) {
    while ((!locAtt.IsNull()) && (locAtt->myTransaction > aTransaction))
      locAtt = locAtt->myBackup;
    if (!locAtt.IsNull()) {
      anAttribute = locAtt;
      return Standard_True;
    }
  }
  return Standard_False;
}

// Label comfort methods ++++++++++++++++++++++++++++++++++++++++++++++++

//=======================================================================
//function : Depth
//purpose  : Returns the depth of the label in the tree.
//           Root has depth 0. So the depth is the number of fathers.
//=======================================================================

Standard_Integer TDF_Label::Depth() const
{
  if (IsNull()) throw Standard_NullObject("A null Label has no depth.");
  return myLabelNode->Depth();
}


//=======================================================================
//function : IsDescendant
//purpose  : Returns True if <me> is a descendant of <aLabel>.
//=======================================================================

Standard_Boolean TDF_Label::IsDescendant(const TDF_Label& aLabel) const
{
  // Cet algorithme remonte jusqu'a la racine. On peut s'arreter
  // si la profondeur atteinte est inferieure a celle de <aLabel>.

  const TDF_LabelNode* lp1 = aLabel.myLabelNode;
  TDF_LabelNode*   lp2 = myLabelNode;
#ifdef OCCT_DEBUG
  if ((lp1 == NULL) || (lp2 == NULL))
    throw Standard_NullObject("A null label has no ancestor nor descendant.");
#endif
  if ((lp1 != NULL) && (lp2 != NULL)) {
    const Standard_Integer d1 = lp1->Depth();
    Standard_Integer d2  = lp2->Depth();
    // Tester (d1 > d2) optimise la recherche ET dispense du test (lp2 != NULL)
    while ((d2 > d1) && (lp2 != lp1)) {
      lp2 = lp2->Father();
      d2  = lp2->Depth();
    }
    return (lp1 == lp2);
  }
  return Standard_False;
}


//=======================================================================
//function : Root
//purpose  : 
//=======================================================================

const TDF_Label TDF_Label::Root() const
{
  if (IsNull()) throw Standard_NullObject("A null Label has no root.");
  return myLabelNode->RootNode();
}


//=======================================================================
//function : NbChildren
//purpose  : Returns the number of children.
//=======================================================================

Standard_Integer TDF_Label::NbChildren() const
{
  if (IsNull()) throw Standard_NullObject("A null Label has no children.");
  Standard_Integer n = 0;
  if (myLabelNode->FirstChild() != NULL)
    for (TDF_ChildIterator itr(*this); itr.More(); itr.Next()) ++n;
  return n;
}


//=======================================================================
//function : FindChild
//purpose  : 
//=======================================================================

TDF_Label TDF_Label::FindChild
(const Standard_Integer aTag,
 const Standard_Boolean create) const
{
  if (IsNull())
    throw Standard_NullObject("A null Label has no child.");
  if (create && ((Depth()+1) & TDF_LabelNodeFlagsMsk))
      throw Standard_OutOfRange("Depth value out of range");

  return FindOrAddChild(aTag,create);
}

// Attribute comfort methods ++++++++++++++++++++++++++++++++++++++++++++

//=======================================================================
//function : IsA
//purpose  : Returns true if owns an attribute with <anID> as ID.
//=======================================================================

// Standard_Boolean TDF_Label::IsA(const Standard_GUID& anID) const
// {
//   Handle(TDF_Attribute) att;
//   return FindAttribute(anID,att);
// }

//=======================================================================
//function : IsAttribute
//purpose  : Returns true if owns an attribute with <anID> as ID.
//=======================================================================

Standard_Boolean TDF_Label::IsAttribute(const Standard_GUID& anID) const
{
  Handle(TDF_Attribute) att;
  return FindAttribute(anID,att);
}


//=======================================================================
//function : HasAttribute
//purpose  : Returns true if the label has at least one unremoved attribute.
//=======================================================================

Standard_Boolean TDF_Label::HasAttribute() const
{
  if (IsNull()) throw Standard_NullObject("A null Label has no attribute.");

  if (!myLabelNode->FirstAttribute().IsNull()) {
    TDF_AttributeIterator itr (myLabelNode); // Without removed attributes.
    return itr.More();
  }
  return Standard_False;
}


//=======================================================================
//function : NbAttributes
//purpose  : Returns the number of attributes.
//=======================================================================

Standard_Integer TDF_Label::NbAttributes() const
{
  if (IsNull()) throw Standard_NullObject("A null Label has no attribute.");
  Standard_Integer n = 0;
  if (!myLabelNode->FirstAttribute().IsNull())
    for (TDF_AttributeIterator itr (myLabelNode); itr.More(); itr.Next()) ++n;
  return n;
}


// Miscelleaneous +++++++++++++++++++++++++++++++++++++++++++++++++++++++


//=======================================================================
//function : Transaction
//purpose  : 
//=======================================================================

Standard_Integer TDF_Label::Transaction() const
{
  if (IsNull()) throw Standard_NullObject("A null Label has no transaction.");
  return myLabelNode->Data()->Transaction();
}


//=======================================================================
//function : Dump
//purpose  : This method is equivalent to operator <<
//=======================================================================

Standard_OStream& TDF_Label::Dump
(Standard_OStream& anOS) const
{
  TDF_IDFilter f; TDF_AttributeIndexedMap m;
  TDF_Label::InternalDump(anOS,f,m,Standard_False);
  return anOS;
}


//=======================================================================
//function : ExtendedDump
//purpose  : 
//=======================================================================

void TDF_Label::ExtendedDump
(Standard_OStream& anOS,
 const TDF_IDFilter& aFilter,
 TDF_AttributeIndexedMap& aMap) const
{ TDF_Label::InternalDump(anOS,aFilter,aMap,Standard_True); }


//=======================================================================
//function : EntryDump
//purpose  : 
//=======================================================================

void TDF_Label::EntryDump(Standard_OStream& anOS) const
{
  if (IsNull ()) {
    anOS<<"This label is null.";
  }
  else {
    TCollection_AsciiString entry;
    TDF_Tool::Entry(*this,entry);
    anOS<<entry;
  }
}

//=======================================================================
//function : FindOrAddChild
//purpose  : Finds or adds a label child having <aTag> as tag.
//=======================================================================

TDF_LabelNode* TDF_Label::FindOrAddChild
(const Standard_Integer aTag,
 const Standard_Boolean create) const
{
  TDF_LabelNode* currentLnp      = myLabelNode->FirstChild();
  TDF_LabelNode* lastLnp         = NULL;
  TDF_LabelNode* lastFoundLnp    = myLabelNode->myLastFoundChild; //jfa 10.01.2003
  TDF_LabelNode* childLabelNode  = NULL;

  // Finds the right place.

  //jfa 10.01.2003
  // 1. Check, if we access to a child, which is after last touched upon
  if (lastFoundLnp != NULL) {
    if (lastFoundLnp->Tag() == aTag) {
      return lastFoundLnp;
    }
    else if (lastFoundLnp->Tag() < aTag) {
      lastLnp = lastFoundLnp;
      currentLnp = lastFoundLnp->Brother();
    }
  }
  //jfa 10.01.2003 end

  // To facilitate many tools, label brethren are stored in increasing order.
  while ((currentLnp != NULL) && (currentLnp->Tag() < aTag) ) {
    lastLnp    = currentLnp;
    currentLnp = currentLnp->Brother();
  }

  if ( (currentLnp != NULL) && (currentLnp->Tag() == aTag) ) {
    // The label exists.
    childLabelNode = currentLnp;
  }
  else if (create) {
    // Creates the label to be inserted always before currentLnp.
    const TDF_HAllocator& anAllocator = myLabelNode->Data()->LabelNodeAllocator();
    childLabelNode =  new (anAllocator) TDF_LabelNode (aTag, myLabelNode);
    childLabelNode->myBrother = currentLnp; // May be NULL.
    childLabelNode->Imported(IsImported());
    //Inserts the label:
    if (lastLnp == NULL) // ... at beginning.
      myLabelNode->myFirstChild = childLabelNode;
    else                 // ... somewhere.
      lastLnp->myBrother = childLabelNode;
  }

  if (lastLnp)                               //agv 14.07.2010
    myLabelNode->myLastFoundChild = lastLnp; //jfa 10.01.2003

  return childLabelNode;
}

//=======================================================================
//function : InternalDump
//purpose  : Private method.
//=======================================================================

void TDF_Label::InternalDump
(Standard_OStream& anOS,
 const TDF_IDFilter& aFilter,
 TDF_AttributeIndexedMap& aMap,
 const Standard_Boolean extended) const
{
  if (IsNull ()) {
    anOS<<"This label is null.";
  }
  else {
    TCollection_AsciiString entry; TDF_Tool::Entry(*this,entry);
    anOS<<entry<<"\t";
    if (IsImported()) anOS<<"IS "; else anOS<<"NOT"; anOS<<" imported; ";
    if (MayBeModified()) anOS<<"MAYBE"; else anOS<<"NOT"; anOS<<" modified; ";
    if (AttributesModified()) anOS<<"HAS attributes"; else anOS<<"NO attribute"; anOS<<" modified; ";
    if (HasAttribute()) {
      Standard_Integer nba = NbAttributes();
      anOS<<"has "<<nba<<" attribute"; if (nba > 1) anOS<<"s"; anOS<<"."<<endl;
      for (TDF_AttributeIterator itr(myLabelNode); itr.More(); itr.Next()) {
        // CLE
        // const Handle(TDF_Attribute)& att = itr.Value();
        Handle(TDF_Attribute) att = itr.Value();
        // ENDCLE
        if (extended && aFilter.IsKept(att)) anOS<<"\t# "<<aMap.Add(att);
        att->TDF_Attribute::Dump(anOS);
      }
    }
    else {
      anOS<<" has no attribute"<<endl;
    }
  }
}

Standard_Boolean TDF_Label::HasLowerNode(const TDF_Label& aLabel) const
{ return (myLabelNode < aLabel.myLabelNode); }

Standard_Boolean TDF_Label::HasGreaterNode(const TDF_Label& aLabel) const
{ return (myLabelNode > aLabel.myLabelNode); }

// from insertor ////////////////////////////////////////////////////////

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

// void TDF_Label::Add(const Handle(TDF_Attribute)& anAttribute)  const
// { AddToNode(myLabelNode,anAttribute); }

//=======================================================================
//function : AddAttribute
//purpose  : 
//=======================================================================

void TDF_Label::AddAttribute (const Handle(TDF_Attribute)& anAttribute,
                              const Standard_Boolean append/* = Standard_False*/)  const
{ AddToNode(myLabelNode,anAttribute,append); }


//=======================================================================
//function : Forget
//purpose  : 
//=======================================================================

// void TDF_Label::Forget(const Handle(TDF_Attribute)& anAttribute)  const
// { ForgetFromNode(myLabelNode,anAttribute); }


//=======================================================================
//function : ForgetAttribute
//purpose  : 
//=======================================================================

void TDF_Label::ForgetAttribute (const Handle(TDF_Attribute)& anAttribute)  const
{ ForgetFromNode(myLabelNode,anAttribute); }


//=======================================================================
//function : Forget
//purpose  : 
//=======================================================================

// Standard_Boolean  TDF_Label::Forget (const Standard_GUID& anID) const
// {  
//   Handle(TDF_Attribute) anAttribute;
//   //if (Label().FindAttribute(anID,anAttribute)) { 
//   if (FindAttribute(anID,anAttribute)) {
//     Forget(anAttribute);
//     return Standard_True;
//   }
//   return Standard_False;
// }

//=======================================================================
//function : ForgetAttribute
//purpose  : 
//=======================================================================

Standard_Boolean  TDF_Label::ForgetAttribute (const Standard_GUID& anID) const
{  
  Handle(TDF_Attribute) anAttribute;
  //if (Label().FindAttribute(anID,anAttribute)) { 
  if (FindAttribute(anID,anAttribute)) {
    ForgetAttribute (anAttribute);
    return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : ForgetAll
//purpose  : 
//=======================================================================

// void TDF_Label::ForgetAll (const Standard_Boolean clearChildren) const
// {
//   for (TDF_AttributeIterator itr1(myLabelNode); itr1.More(); itr1.Next())
//     ForgetFromNode(myLabelNode,itr1.Value());
//   if (clearChildren)
//     for (TDF_ChildIterator itr2(myLabelNode); itr2.More(); itr2.Next()) {
//       itr2.Value().ForgetAll(clearChildren);
//     }
// }


//=======================================================================
//function : ForgetAllAttributes
//purpose  : 
//=======================================================================

void TDF_Label::ForgetAllAttributes (const Standard_Boolean clearChildren) const
{
  TDF_AttributeIterator itr1 (myLabelNode);
// AGV-OCC5031: iterator must be incremented before removal of the attribute
  while (itr1.More()) {
    const Handle(TDF_Attribute) anAttr = itr1.Value();
    itr1.Next();
    ForgetFromNode (myLabelNode, anAttr);
  }
// while (itr1.More()) {
//   ForgetFromNode(myLabelNode,itr1.Value());
//   itr1.Next();
// }
  if (clearChildren)
    for (TDF_ChildIterator itr2(myLabelNode); itr2.More(); itr2.Next()) {
      itr2.Value().ForgetAllAttributes (clearChildren);
    }
}


//=======================================================================
//function : Resume
//purpose  : 
//=======================================================================

// void TDF_Label::Resume (const Handle(TDF_Attribute)& anAttribute)  const
// { ResumeToNode(myLabelNode,anAttribute); }

//=======================================================================
//function : ResumeAttribute
//purpose  : 
//=======================================================================

void TDF_Label::ResumeAttribute (const Handle(TDF_Attribute)& anAttribute)  const
{ ResumeToNode(myLabelNode,anAttribute); }



//=======================================================================
//function : AddToNode
//purpose  : Private method used by Add
//=======================================================================

void TDF_Label::AddToNode (const TDF_LabelNodePtr& toNode,
                           const Handle(TDF_Attribute)& anAttribute,
                           const Standard_Boolean append) const
{
  // check that modification is allowed
  if ( !toNode->Data()->IsModificationAllowed() ) {
    TCollection_AsciiString aMess;
    aMess = "Attribute \"";
    aMess += anAttribute->DynamicType()->Name();
    aMess += "\" is added to label outside transaction";
    throw Standard_ImmutableObject(aMess.ToCString());
  }
    
  if (!anAttribute->Label().IsNull())
    throw Standard_DomainError("Attribute to add is already attached to a label.");
  Handle(TDF_Attribute) dummyAtt;
  //if (Find(anAttribute->ID(),dummyAtt))  
  if (FindAttribute(anAttribute->ID(),dummyAtt))
    throw Standard_DomainError("This label has already such an attribute.");

  anAttribute->myTransaction = toNode->Data()->Transaction();  /// myData->Transaction();
  anAttribute->mySavedTransaction = 0;

  //append to the end of the attribute list
  dummyAtt.Nullify();
  if (append) {
    for (TDF_AttributeIterator itr (toNode); itr.More(); itr.Next())
      dummyAtt = itr.Value();
  }

  toNode->AddAttribute(dummyAtt,anAttribute);
  toNode->AttributesModified(anAttribute->myTransaction != 0);
  //if (myData->NotUndoMode()) anAttribute->AfterAddition();  
  if (toNode->Data()->NotUndoMode()) anAttribute->AfterAddition();
}


//=======================================================================
//function : ForgetFromNode
//purpose  : Private method used by Forget
//=======================================================================

void TDF_Label::ForgetFromNode (const TDF_LabelNodePtr& fromNode,
                                const Handle(TDF_Attribute)& anAttribute) const
{
  // check that modification is allowed
  if ( !fromNode->Data()->IsModificationAllowed() ) {
    TCollection_AsciiString aMess;
    aMess = "Attribute \"";
    aMess += anAttribute->DynamicType()->Name();
    aMess += "\" is removed from label outside transaction";
    throw Standard_ImmutableObject(aMess.ToCString());
  }
    
  if (fromNode != anAttribute->Label().myLabelNode)
    throw Standard_DomainError("Attribute to forget not attached to my label.");

  Standard_Integer curTrans = fromNode->Data()->Transaction();
  if (!anAttribute->IsForgotten()) {
    if ( (curTrans == 0) ||
        ( (anAttribute->myTransaction == curTrans) &&
         anAttribute->myBackup.IsNull())) {
      // 1- No transaction is open;
      // OR
      // 2- The attribute has been created in the current transaction;
      // ==> Complete disparition of the attribute.
      Handle(TDF_Attribute) lastAtt;
      for (TDF_AttributeIterator itr(fromNode, Standard_False);
           itr.More();
           itr.Next()) {
        if (itr.Value() == anAttribute) {
          //if (myData->NotUndoMode()) { 
          if (fromNode->Data()->NotUndoMode()) {
            anAttribute->BeforeForget();
            anAttribute->BeforeRemoval();
          }
          fromNode->RemoveAttribute(lastAtt,anAttribute);
          anAttribute->Forget(fromNode->Data()->Transaction());
          break;
        }
        lastAtt = itr.Value();
      }
    }
    else {
      // One case is here ignored:
      // The attribute has been modified in the current transaction.
      // (It has at least one backup.) We don't restore the previous
      // version before forgetting. It may generated a strange behaviour
      // in case of forgetting, commiting, aborting...
      if (fromNode->Data()->NotUndoMode()) anAttribute->BeforeForget();
      anAttribute->Forget(fromNode->Data()->Transaction());
    }
  }
}

//=======================================================================
//function : ResumeToNode
//purpose  : Private method used by Resume
//=======================================================================

void TDF_Label::ResumeToNode (const TDF_LabelNodePtr& toNode,
                              const Handle(TDF_Attribute)& anAttribute) const
{
  if (anAttribute.IsNull())
    throw Standard_NullObject("The attribute is a null handle.");
  if (!anAttribute->Label().IsNull())
    throw Standard_NullObject("Cannot resume an attribute already attached to a label.");
  if (!anAttribute->IsForgotten()) 
    throw Standard_DomainError("Cannot resume an unforgotten attribute.");

  AddToNode(toNode, anAttribute, Standard_False);
  anAttribute->Resume();
  if (toNode->Data()->NotUndoMode()) anAttribute->AfterResume();
}

//////////////////end from insertor ///////////////////////////////////////////////////
