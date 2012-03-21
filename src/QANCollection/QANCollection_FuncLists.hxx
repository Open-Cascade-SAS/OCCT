// Created on: 2002-04-30
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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



#ifndef QANCollection_FuncLists_HeaderFile
#define QANCollection_FuncLists_HeaderFile

// ===================== Test methods of List type ==========================
////////////////////////////////void TestList (QANCollection_List&     theL)
void TestList (QANCollection_ListFunc&     theL)
{
  // Extent
  Standard_Integer iExt=theL.Extent();
  Standard_Integer i;

  printf ("Info: testing List(%d)\n", iExt);
  // Append(2), Prepend(2), InsertBefore(2), InsertAfter(2), 
  // Remove, RemoveFirst, First, Last
  ItemType anItem;
  ////////////////////////////////QANCollection_List aL, aL1;
  QANCollection_ListFunc aL, aL1;
  for (i=0; i<4; i++)
  {
    Random (anItem);
    aL.Append (anItem); // #1
    aL.Append (aL1);    // #2
    Random (anItem);
    aL1.Prepend (anItem); // #3
    aL1.Prepend (aL);     // #4
    ////////////////////////////////QANCollection_List::Iterator anI(theL);
    QANCollection_ListFunc::Iterator anI(theL);
    if (anI.More())
    {
      Random (anItem);
      theL.InsertBefore (anItem, anI); // #5
      theL.InsertBefore (aL1, anI);    // #6
      Random (anItem);
      theL.InsertAfter (anItem, anI); // #7
      theL.InsertAfter (aL, anI);     // #8
      theL.Remove (anI);  // #9
      if (theL.Extent() > 0)
        theL.RemoveFirst(); // #10
    }
    else
    {
      theL.Prepend (anItem);
      PrintItem(theL.First());
      PrintItem(theL.Last());
    }
  }
  // Copy constructor + operator=
  ////////////////////////////////aL = QANCollection_List(theL);
  aL = QANCollection_ListFunc(theL);

  // Assign
  AssignCollection (theL, aL);

  // Clear
  aL.Clear();
}

// ===================== Test methods of Queue type ===========================
////////////////////////////////void TestQueue (QANCollection_Queue& theQ)
void TestQueue (QANCollection_QueueFunc& theQ)
{
  // Length
  Standard_Integer iLen=theQ.Length();
  Standard_Integer i;

  printf ("Info: testing Queue(%d)\n", iLen);
  // Push, Pop, Front, ChangeFront
  ItemType anItem;
  ////////////////////////////////QANCollection_Queue aQ;
  QANCollection_QueueFunc aQ;
  for (i=0; i<4; i++)
  {
    Random (anItem);
    aQ.Push (anItem);
    Random(aQ.ChangeFront());
    Random (anItem);
    aQ.Push (anItem);
    PrintItem(aQ.Front());
    aQ.Pop();
  }
  // Copy constructor + operator=
  ////////////////////////////////theQ = QANCollection_Queue(aQ);
  theQ = QANCollection_QueueFunc(aQ);

  // Assign
  AssignCollection (theQ, aQ);

  // Clear
  aQ.Clear();
}

// ===================== Test methods of Stack type ===========================
////////////////////////////////void TestStack (QANCollection_Stack& theS)
void TestStack (QANCollection_StackFunc& theS)
{
  // Depth
  Standard_Integer iDep=theS.Depth();
  Standard_Integer i;

  printf ("Info: testing Stack(%d)\n", iDep);
  // Push, Pop, Top, ChangeTop
  ItemType anItem;
  ////////////////////////////////QANCollection_Stack aS;
  QANCollection_StackFunc aS;
  for (i=0; i<4; i++)
  {
    Random (anItem);
    aS.Push (anItem);
    Random(aS.ChangeTop());
    Random (anItem);
    aS.Push (anItem);
    PrintItem(aS.Top());
    aS.Pop();
  }
  // Copy constructor + operator=
  ////////////////////////////////theS = QANCollection_Stack(aS);
  theS = QANCollection_StackFunc(aS);

  // Assign
  AssignCollection (theS, aS);

  // Clear
  aS.Clear();
}

// ===================== Test methods of Set type =============================
////////////////////////////////void TestSet (QANCollection_Set& theS)
void TestSet (QANCollection_SetFunc& theS)
{
  // Extent
  Standard_Integer iExt=theS.Extent();
  Standard_Integer i;

  printf ("Info: testing Set(%d)\n", iExt);
  Key2Type anItem;
  // Constructor, Add
  ////////////////////////////////QANCollection_Set aSet1, aSet2, aSet;
  QANCollection_SetFunc aSet1, aSet2, aSet;
  for (i=1; i<=8; i++)
  {
    Random(anItem);
    aSet1.Add(anItem);
    if (i>4)
      aSet2.Add(anItem);
  }
  for (i=1; i<=4; i++)
  {
    Random(anItem);
    aSet2.Add(anItem);
  }
  if (!aSet2.Contains(anItem))
    printf ("Error   : set sais it does not contain its item\n");
  // operator=, Union, Difference, Intersection
  aSet = aSet1;
  printCollection(aSet2,"Set2");
  aSet1.Union(aSet2);
  printCollection(aSet1,"Union");
  if (!aSet1.IsAProperSubset(aSet2))
    printf ("Error   : not a proper subset?\n");
  if (!aSet1.IsAProperSubset(aSet2))
    printf ("Error   : not a subset?!\n");
  aSet1.Intersection(aSet);
  printCollection(aSet,"Intersection");
  aSet1.Difference(aSet2);
  printCollection(aSet1,"Difference");

  // operator=
  ////////////////////////////////Handle(QANCollection_HSet) aHS = new QANCollection_HSet(aSet);
  Handle(QANCollection_HSetFunc) aHS = new QANCollection_HSetFunc(aSet);

  // Assign
  AssignCollection (aHS->ChangeSet(), theS);

  // Clear
  aSet.Clear();
}

// ===================== Test methods of SList type ===========================
////////////////////////////////void TestSList (QANCollection_SList&     theSL)
void TestSList (QANCollection_SListFunc&     theSL)
{
  printf ("Info: testing SList\n");
  ItemType anItem;
  ////////////////////////////////QANCollection_SList aSL, aSL1;
  QANCollection_SListFunc aSL, aSL1;

  // Construct, Constructed, operator=, IsEmpty
  Random(anItem);
  aSL.Construct(anItem);
  Random(anItem);
  aSL1 = aSL.Constructed(anItem);
  if (aSL.IsEmpty())
    printf ("Error   : SList must not be empty\n");
  printCollection(aSL1,"aSL1");
  
  Random(anItem);
  aSL.Construct(anItem);
  // SetTail
  aSL.SetTail(aSL1);
  printCollection(aSL,"aSL");

  // ChangeValue
  Random(aSL1.Tail().ChangeValue());
  // ChangeTail, ToTail
  Random(anItem);
  theSL.Construct(anItem);
  printCollection(theSL,"theSL");
  theSL.ChangeTail() = aSL;
  printCollection(theSL,"theSL");

  // Assign
  AssignCollection (aSL, theSL);

  // Clear
  aSL.Clear();
}

// ===================== Test methods of Sequence type ========================
////////////////////////////////void TestSequence (QANCollection_Sequence& theS)
void TestSequence (QANCollection_SequenceFunc& theS)
{
  Standard_Integer i;

  printf ("Info: testing Sequence\n");
  // Append(2)
  ItemType anItem;
  ////////////////////////////////QANCollection_Sequence aS, aS1;
  QANCollection_SequenceFunc aS, aS1;
  // Append(2), Prepend(2), InsertBefore(2), InsertAfter(2), 
  // Remove, RemoveFirst, First, Last
  for (i=0; i<4; i++)
  {
    Random (anItem);
    aS.Append (anItem); // #1
    aS.Append (aS1);    // #2
    Random (anItem);
    aS1.Prepend (anItem); // #3
    aS1.Prepend (aS);     // #4
    if (theS.Length() > 0)
    {
      Random (anItem);
      theS.InsertBefore (1, anItem); // #5
      theS.InsertBefore (2, aS1);    // #6
      Random (anItem);
      theS.InsertAfter (1, anItem); // #7
      theS.InsertAfter (2, aS);     // #8
      theS.Remove (1);  // #9
      if (theS.Length() > 0)
        theS.Remove(1); // #10
    }
    else
    {
      theS.Prepend (anItem);
      PrintItem(theS.First());
      PrintItem(theS.Last());
    }
  }

  // ()
  PrintItem(theS(1));

  // Handle, Split
  ////////////////////////////////Handle(QANCollection_HSequence) aHS = new QANCollection_HSequence(aS1);
  Handle(QANCollection_HSequenceFunc) aHS = new QANCollection_HSequenceFunc(aS1);
  theS.Split (3, aHS->ChangeSequence());

  // Assign
  AssignCollection (theS, aS);

  // Clear
  aS.Clear();
}

#endif
