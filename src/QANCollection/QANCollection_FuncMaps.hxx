// Created on: 2002-04-30
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef QANCollection_FuncMaps_HeaderFile
#define QANCollection_FuncMaps_HeaderFile

// ===================== Test methods of Map type =============================
////////////////////////////////void TestMap  (QANCollection_Map& theM)
void TestMap  (QANCollection_MapFunc& theM)
{
  // Extent
  Standard_Integer iExt=theM.Extent();
  Standard_Integer i;

  printf ("Info: testing Map(l=%d)\n", iExt);
  theM.Statistics(cout);
  // Resize
  theM.ReSize(8);
  theM.Statistics(cout);
  cout.flush();
  // Constructor
  ////////////////////////////////QANCollection_Map aM;
  QANCollection_MapFunc aM;
  // Add
  Key1Type aKey;
  for (i=0; i<8; i++)
  {
    Random (aKey);
    aM.Add (aKey);
  }
  // Contains, Remove
  if (!aM.Contains(aKey))
  {
    printf("Error   : map says that it does not contain its key ");
    PrintItem(aKey);
  }
  else
  {
    aM.Remove(aKey);
    printf("      successfully removed item, l=%d\n", aM.Size());
  }
  // Copy constructor (including operator=)
  ////////////////////////////////QANCollection_Map aM2 = QANCollection_Map(aM);
  QANCollection_MapFunc aM2 = QANCollection_MapFunc(aM);
  // Assign
  AssignCollection (aM2,theM);

  // Clear
  aM.Clear();
}

// ===================== Test methods of DataMap type =========================
////////////////////////////////void TestDataMap  (QANCollection_DataMap& theM)
void TestDataMap  (QANCollection_DataMapFunc& theM)
{
  // Extent
  Standard_Integer iExt=theM.Extent();
  Standard_Integer i;

  printf ("Info: testing DataMap(l=%d)\n", iExt);
  theM.Statistics(cout);
  // Resize
  theM.ReSize(8);
  theM.Statistics(cout);
  cout.flush();
  // Constructor
  ////////////////////////////////QANCollection_DataMap aM;
  QANCollection_DataMapFunc aM;
  // Bind, Find, ChangeFind, ()
  Key1Type aKey;
  ItemType anItem;
  for (i=0; i<8; i++)
  {
    Random (aKey);
    Random (anItem);
    aM.Bind (aKey, anItem);
    PrintItem(aM.Find(aKey));
    Random(aM(aKey));
  }
  // IsBound, UnBind
  if (!aM.IsBound(aKey))
  {
    printf("Error   : map says that it does not contain its key ");
    PrintItem(aKey);
  }
  else
  {
    aM.UnBind(aKey);
    printf("      successfully unbound the key, l=%d\n", aM.Size());
  }
  // Copy constructor (including operator=)
  ////////////////////////////////theM = QANCollection_DataMap(aM);
  theM = QANCollection_DataMapFunc(aM);
  // Assign - prohibited
  // AssignCollection (aM2,theM);
  printCollection (theM, "DataMap:");

  // Clear
  aM.Clear();
}


// ===================== Test methods of DoubleMap type =======================
////////////////////////////////void TestDoubleMap  (QANCollection_DoubleMap& theM)
void TestDoubleMap  (QANCollection_DoubleMapFunc& theM)
{
  // Extent
  Standard_Integer iExt=theM.Extent();
  Standard_Integer i;

  printf ("Info: testing DoubleMap(l=%d)\n", iExt);
  theM.Statistics(cout);
  // Resize
  theM.ReSize(8);
  theM.Statistics(cout);
  cout.flush();
  // Constructor
  ////////////////////////////////QANCollection_DoubleMap aM;
  QANCollection_DoubleMapFunc aM;
  // Bind, Find?, 
  Key1Type aKey1;
  Key2Type aKey2;
  for (i=0; i<8; i++)
  {
    Random (aKey1);
    Random (aKey2);
    aM.Bind (aKey1, aKey2);
    PrintItem(aM.Find1(aKey1));
    if (!aM.IsBound1(aKey1))
    {
      printf("Error   : map says that it does not contain its key ");
      PrintItem(aKey1);
    }
    PrintItem(aM.Find2(aKey2));
    if (!aM.IsBound2(aKey2))
    {
      printf("Error   : map says that it does not contain its key ");
      PrintItem(aKey2);
    }
  }
  // AreBound, UnBind
  if (!aM.AreBound(aKey1,aKey2))
  {
    printf("Error   : map says that it does not contain its keys ");
    PrintItem(aKey1);
    PrintItem(aKey2);
  }
  else
  {
    if (aM.UnBind2(aKey2))
      printf("      successfully unbound the key, l=%d\n", aM.Size());
    if (aM.UnBind1(aKey1))
      printf("Error   : unbound both keys?!\n");
  }
  // Copy constructor (including operator=)
  ////////////////////////////////theM = QANCollection_DoubleMap(aM);
  theM = QANCollection_DoubleMapFunc(aM);
  // Assign - prohibited
  // AssignCollection (aM2,theM);
  printCollection (theM, "DoubleMap:");

  // Clear
  aM.Clear();
}

// ===================== Test methods of IndexedMap type ======================
////////////////////////////////void TestIndexedMap  (QANCollection_IndexedMap& theM)
void TestIndexedMap  (QANCollection_IndexedMapFunc& theM)
{
  // Extent
  Standard_Integer iExt=theM.Extent();
  Standard_Integer i;

  printf ("Info: testing IndexedMap(l=%d)\n", iExt);
  theM.Statistics(cout);
  // Resize
  theM.ReSize(8);
  theM.Statistics(cout);
  cout.flush();
  // Constructor
  ////////////////////////////////QANCollection_IndexedMap aM;
  QANCollection_IndexedMapFunc aM;
  // Add, FindKey, FindIndex
  Key1Type aKey;
  for (i=0; i<8; i++)
  {
    Random (aKey);
    aM.Add (aKey);
    Standard_Integer iIndex=aM.FindIndex(aKey);
    printf ("     added a key, i=%d, k=",iIndex);
    PrintItem(aM(iIndex));
  }
  // Contains, Remove
  if (!aM.Contains(aM.FindKey(aM.FindIndex(aKey))))
  {
    printf("Error   : map says that it does not contain its key ");
    PrintItem(aKey);
  }
  else
  {
    aM.RemoveLast();
    printf("      successfully removed item, l=%d\n", aM.Size());
  }
  // Substitute
  Random(aKey);
  aM.Substitute(1,aKey);
  // Copy constructor (including operator=)
  ////////////////////////////////QANCollection_IndexedMap aM2 = QANCollection_IndexedMap(aM);
  QANCollection_IndexedMapFunc aM2 = QANCollection_IndexedMapFunc(aM);
  // Assign
  AssignCollection (aM2,theM);

  // Clear
  aM.Clear();
}

// ===================== Test methods of IndexedDataMap type ==================
////////////////////////////////void TestIndexedDataMap  (QANCollection_IDMap& theM)
void TestIndexedDataMap  (QANCollection_IDMapFunc& theM)
{
  // Extent
  Standard_Integer iExt=theM.Extent();
  Standard_Integer i;

  printf ("Info: testing IndexedDataMap(l=%d)\n", iExt);
  theM.Statistics(cout);
  // Resize
  theM.ReSize(8);
  theM.Statistics(cout);
  cout.flush();
  // Constructor
  ////////////////////////////////QANCollection_IDMap aM;
  QANCollection_IDMapFunc aM;
  // Add, FindKey, FindIndex, FindFromIndex, Change..., ()
  Key1Type aKey;
  ItemType anItem;
  for (i=0; i<8; i++)
  {
    Random (aKey);
    Random (anItem);
    aM.Add (aKey, anItem);
    Standard_Integer iIndex=aM.FindIndex(aKey);
    printf ("     added a key, i=%d, k=",iIndex);
    PrintItem(aM.FindKey(iIndex));
    PrintItem(aM(iIndex));
    Random(aM.ChangeFromIndex(iIndex));
  }
  // Contains, Remove, FindFromKey
  if (!aM.Contains(aM.FindKey(aM.FindIndex(aKey))))
  {
    printf("Error   : map says that it does not contain its key ");
    PrintItem(aKey);
  }
  else
  {
    anItem = aM.FindFromKey(aKey);
    aM.RemoveLast();
    printf("      successfully removed item, l=%d\n", aM.Size());
  }
  // Substitute
  Random(aKey);
  aM.Substitute (1, aKey, anItem);
  // Copy constructor (including operator=)
  ////////////////////////////////theM = QANCollection_IDMap(aM);
  theM = QANCollection_IDMapFunc(aM);
  // Assign - prohibited
  // AssignCollection (aM2,theM);
  printCollection (theM, "DoubleMap:");

  // Clear
  aM.Clear();
}


#endif
