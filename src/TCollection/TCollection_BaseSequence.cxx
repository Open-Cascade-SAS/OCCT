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


#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TCollection_BaseSequence.hxx>
#include <TCollection_SeqNode.hxx>

typedef void (*DelNode) (TCollection_SeqNode*);

TCollection_BaseSequence::TCollection_BaseSequence() :
       FirstItem(NULL),
       LastItem(NULL),
       CurrentItem(NULL),
       CurrentIndex(0),
       Size(0)
{
}



// ----------------------------------
// Clear : Clear the Current Sequence
// ----------------------------------
void TCollection_BaseSequence::Clear(const Standard_Address delnode)
{
  Size = 0;
  TCollection_SeqNode* p = (TCollection_SeqNode*) FirstItem;
  TCollection_SeqNode* q;
  while (p) {
    q = p;
    p = p->Next();
    ((DelNode)delnode) (q);
  }
  LastItem = FirstItem = CurrentItem  = NULL;
  CurrentIndex = 0;
}

void TCollection_BaseSequence::PAppend(const Standard_Address newnode)
{
  if (Size == 0) {
    FirstItem = LastItem = CurrentItem = newnode;   
    CurrentIndex = Size = 1;
  }
  else {
    ((TCollection_SeqNode*)LastItem)->Next() = (TCollection_SeqNode*)newnode;
    LastItem = newnode;   
    Size++;               
  }
}

// ---------------------------------------------------
// Append : Push a sequence at the end of the sequence
// ---------------------------------------------------
void TCollection_BaseSequence::PAppend(TCollection_BaseSequence& Other)
{
  if (Other.Size == 0)
    return;
  if (Size == 0) {
    Size         = Other.Size;
    FirstItem    = Other.FirstItem;
    LastItem     = Other.LastItem;
    CurrentItem  = FirstItem;
    CurrentIndex = 1;
  }
  else {
    Size += Other.Size;
    ((TCollection_SeqNode*)LastItem)->Next() = (TCollection_SeqNode*)Other.FirstItem;
    if (Other.FirstItem) {
      ((TCollection_SeqNode*)Other.FirstItem)->Previous() = (TCollection_SeqNode*)LastItem;
      LastItem = Other.LastItem;
    }
  }
  Other.Nullify();
}



void TCollection_BaseSequence::PPrepend(const Standard_Address newnode)
{
  if (Size == 0) {
    FirstItem = LastItem = CurrentItem = newnode;
    CurrentIndex = Size = 1;
  }
  else {
    ((TCollection_SeqNode*)FirstItem)->Previous() = (TCollection_SeqNode*) newnode;
    ((TCollection_SeqNode*)newnode)->Next() = (TCollection_SeqNode*)FirstItem; 
    FirstItem = newnode;
    Size++;
    CurrentIndex++;
  }
}

void TCollection_BaseSequence::PPrepend(TCollection_BaseSequence& Other)
{
  if (Other.Size == 0)
    return;
  if (Size == 0) {
    Size         = Other.Size;
    FirstItem    = Other.FirstItem;
    LastItem     = Other.LastItem;
    CurrentIndex = 1;
    CurrentItem  =  FirstItem;
  }
  else {
    Size += Other.Size;
    if (Other.LastItem) ((TCollection_SeqNode*)Other.LastItem)->Next() = (TCollection_SeqNode*)FirstItem;
    ((TCollection_SeqNode*)FirstItem)->Previous() = (TCollection_SeqNode*)Other.LastItem;
    FirstItem = Other.FirstItem;
    CurrentIndex += Other.Size;
  }
  Other.Nullify();
}

// ---------------------------------------------------------
// Reverse : Reverse the order of a given sequence
// ---------------------------------------------------------
void TCollection_BaseSequence::Reverse()
{
  TCollection_SeqNode* p = (TCollection_SeqNode*) FirstItem;
  TCollection_SeqNode* tmp;
  while (p) {
    tmp = p->Next();
    p->Next() = p->Previous();
    p->Previous() = tmp;
    p = tmp;
  }
  tmp = (TCollection_SeqNode*)FirstItem;
  FirstItem = LastItem;
  LastItem = tmp;
  if (Size != 0) CurrentIndex = Size + 1 - CurrentIndex;
}


void TCollection_BaseSequence::PInsertAfter(const Standard_Integer Index, const Standard_Address N)
{
  if (Index == 0) 
    PPrepend(N);
  else {
    TCollection_SeqNode* p = (TCollection_SeqNode*) Find(Index);
    TCollection_SeqNode* newnode = (TCollection_SeqNode*) N;
    newnode->Next() = p->Next();
    newnode->Previous() = p;
    if (Index == Size) LastItem = newnode;
    else p->Next()->Previous() = newnode;
    p->Next() = newnode;
    Size++;
    if (Index < CurrentIndex) CurrentIndex++;
  }
}

// -------------------------------------------------------------------
// InsertAfter : Insert a sequence after a given index in the sequence
// -------------------------------------------------------------------

void TCollection_BaseSequence::PInsertAfter(const Standard_Integer Index, TCollection_BaseSequence& Other)
{
  if (Index < 0 || Index > Size) throw Standard_OutOfRange();
  if (Other.Size == 0) return;
  if (Index == 0) 
    PPrepend( Other );
  else {
    TCollection_SeqNode* p = (TCollection_SeqNode*) Find(Index);
    ((TCollection_SeqNode*)Other.FirstItem)->Previous() = p;
    ((TCollection_SeqNode*)Other.LastItem)->Next() = p->Next();
    if (Index == Size) LastItem = Other.LastItem;
    else p->Next()->Previous() = (TCollection_SeqNode*)Other.LastItem;
    p->Next() = (TCollection_SeqNode*)Other.FirstItem;
    Size += Other.Size;
    if (Index < CurrentIndex) CurrentIndex += Other.Size;
    Other.Nullify();
  }
}

// ----------------------------------------
// Exchange : Exchange two elements in the sequence
// ----------------------------------------
void TCollection_BaseSequence::Exchange(const Standard_Integer I, const Standard_Integer J)
{
   Standard_OutOfRange_Raise_if ( I <= 0 || J <= 0 || I > Size || J > Size,"" ) ;

   // Assume I < J
   if (I == J) return;
   if (J < I) {
     Exchange(J,I);
     return;
   }

   TCollection_SeqNode* pi = (TCollection_SeqNode*) Find(I);
   TCollection_SeqNode* pj = (TCollection_SeqNode*) Find(J);

   // update the node before I
   if (pi->Previous())
     pi->Previous()->Next() = pj;
   else 
     FirstItem = pj;

   // update the node after J
   if (pj->Next())
     pj->Next()->Previous() = pi;
   else
     LastItem = pi;

   if (pi->Next() == pj) {          // I and J are consecutives, update them
     pj->Previous() = pi->Previous();
     pi->Previous() = pj;
     pi->Next() = pj->Next();
     pj->Next() = pi;
   }
   else {                        // I and J are not consecutive
     pi->Next()->Previous() = pj;    // update the node after I
     pj->Previous()->Next() = pi;    // update the node before J
     TCollection_SeqNode* tmp = pi->Next();       // update nodes I and J
     pi->Next() = pj->Next();
     pj->Next() = tmp;
     tmp = pi->Previous();
     pi->Previous() = pj->Previous();
     pj->Previous() = tmp;
   }

   if      (CurrentIndex == I) CurrentItem = pj;
   else if (CurrentIndex == J) CurrentItem = pi;
}

void TCollection_BaseSequence::PSplit(const Standard_Integer Index, TCollection_BaseSequence& Sub)
{
   Standard_OutOfRange_Raise_if( Index <= 0 || Index > Size,"" );
   Standard_DomainError_Raise_if(this == &Sub,"No Split on myself!!");

   TCollection_SeqNode* p = (TCollection_SeqNode*) Find(Index);

   Sub.LastItem = LastItem;
   Sub.Size = Size - Index + 1;

   LastItem = p->Previous();
   if (LastItem) {
     ((TCollection_SeqNode*)LastItem)->Next() = NULL;
     Size = Index - 1;
     if (CurrentIndex >= Index) {
       CurrentIndex = 1;
       CurrentItem = (TCollection_SeqNode*) FirstItem;
     }
   }
   else {
     FirstItem = CurrentItem = NULL;
     Size = CurrentIndex = 0;
   }

   Sub.FirstItem = Sub.CurrentItem = p;
   p->Previous() = NULL;
   Sub.CurrentIndex = 1;
}

void TCollection_BaseSequence::Remove(const Standard_Integer Index, const Standard_Address delnode)
{
  Standard_OutOfRange_Raise_if(Index <= 0 || Index > Size,"" );
  
  TCollection_SeqNode* p = (TCollection_SeqNode*) Find(Index);
  if (p->Previous())
    p->Previous()->Next() = p->Next();
  else
    FirstItem = p->Next();
  if (p->Next())
    p->Next()->Previous() = p->Previous();
  else
    LastItem = p->Previous();

  Size--;
  if      (CurrentIndex > Index) CurrentIndex--;
  else if (CurrentIndex == Index) {
    if (p->Next()) 
      CurrentItem = p->Next();
    else {
      CurrentItem = (TCollection_SeqNode*) LastItem;
      CurrentIndex = Size;
    }
  }
   ((DelNode)delnode) (p);
}

// ---------------------
// Remove a set of items
// --------------------- 
void TCollection_BaseSequence::Remove(const Standard_Integer From, const Standard_Integer To, 
				      const Standard_Address delnode)
{
  Standard_OutOfRange_Raise_if (From <= 0 || From > Size || To <= 0 || To > Size || From > To,"" );

  TCollection_SeqNode* pfrom = (TCollection_SeqNode*) Find(From);
  TCollection_SeqNode* pto   = (TCollection_SeqNode*) Find(To);
  
  if (pfrom->Previous())
    pfrom->Previous()->Next() = pto->Next();
  else
    FirstItem = pto->Next();
  if (pto->Next())
    pto->Next()->Previous() = pfrom->Previous();
  else
    LastItem = pfrom->Previous();
  
  Size -= To - From + 1;
  if      (CurrentIndex > To) 
    CurrentIndex -= To - From + 1;
  else if (CurrentIndex >= From) {
    if (pto->Next()) {
      CurrentItem = pto->Next();
      CurrentIndex = From;                      // AGV fix 24.05.01
    } else {
      CurrentItem = (TCollection_SeqNode*) LastItem;
      CurrentIndex = Size;
    }
  }
  
  Standard_Integer i;
  for (i = From; i <= To; i++) {
    pto = pfrom;
    pfrom = pfrom->Next();
     ((DelNode)delnode) (pto);
  }
}

Standard_Address TCollection_BaseSequence::Find(const Standard_Integer Index) const 
{
  Standard_Integer i;
  TCollection_SeqNode* p;
  if (Index <= CurrentIndex) {
    if (Index < CurrentIndex / 2) {
      p = (TCollection_SeqNode*) FirstItem;
      for (i = 1; i < Index; i++) p = p->Next();
    }
    else {
      p = (TCollection_SeqNode*) CurrentItem;
      for (i = CurrentIndex; i > Index; i--) p = p->Previous();
    }
  }
  else {
    if (Index < (CurrentIndex + Size) / 2) {
      p = (TCollection_SeqNode*) CurrentItem;
      for (i = CurrentIndex; i < Index; i++) p = p->Next();
    }
    else {
      p = (TCollection_SeqNode*) LastItem;
      for (i = Size; i > Index; i--) p = p->Previous();
    }
  }
  return p;
}

//=======================================================================
//function : Nullify
//purpose  : 
//=======================================================================

void TCollection_BaseSequence::Nullify()
{
  FirstItem = LastItem = CurrentItem = NULL;
  CurrentIndex = Size = 0;
}
