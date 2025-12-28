// Created on: 2007-09-06
// Created by: Sergey ZARITCHNY
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <TDataStd_DeltaOnModificationOfIntArray.hxx>

#include <Standard_Type.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_Label.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_DeltaOnModificationOfIntArray, TDF_DeltaOnModification)

#ifdef OCCT_DEBUG
  #define MAXUP 1000
#endif
//=================================================================================================

TDataStd_DeltaOnModificationOfIntArray::TDataStd_DeltaOnModificationOfIntArray(
  const occ::handle<TDataStd_IntegerArray>& OldAtt)
    : TDF_DeltaOnModification(OldAtt),
      myUp1(0),
      myUp2(0)
{
  occ::handle<TDataStd_IntegerArray> CurrAtt;
  if (Label().FindAttribute(OldAtt->ID(), CurrAtt))
  {
    {
      occ::handle<NCollection_HArray1<int>> Arr1, Arr2;
      Arr1 = OldAtt->Array();
      Arr2 = CurrAtt->Array();
#ifdef OCCT_DEBUG
      if (Arr1.IsNull())
        std::cout << "DeltaOnModificationOfIntArray:: Old IntArray is Null" << std::endl;
      if (Arr2.IsNull())
        std::cout << "DeltaOnModificationOfIntArray:: Current IntArray is Null" << std::endl;
#endif

      if (Arr1.IsNull() || Arr2.IsNull())
        return;
      if (Arr1 != Arr2)
      {
        myUp1 = Arr1->Upper();
        myUp2 = Arr2->Upper();
        int i, N = 0, aCase = 0;
        if (myUp1 == myUp2)
        {
          aCase = 1;
          N     = myUp1;
        }
        else if (myUp1 < myUp2)
        {
          aCase = 2;
          N     = myUp1;
        }
        else
        {
          aCase = 3;
          N     = myUp2;
        } // Up1 > Up2

        NCollection_List<int> aList;
        for (i = Arr1->Lower(); i <= N; i++)
          if (Arr1->Value(i) != Arr2->Value(i))
            aList.Append(i);
        if (aCase == 3)
        {
          for (i = N + 1; i <= myUp1; i++)
            aList.Append(i);
        }
        if (aList.Extent())
        {
          myIndxes = new NCollection_HArray1<int>(1, aList.Extent());
          myValues = new NCollection_HArray1<int>(1, aList.Extent());
          NCollection_List<int>::Iterator anIt(aList);
          for (i = 1; anIt.More(); anIt.Next(), i++)
          {
            myIndxes->SetValue(i, anIt.Value());
            myValues->SetValue(i, Arr1->Value(anIt.Value()));
          }
        }
      }
    }
    OldAtt->RemoveArray();
#ifdef OCCT_DEBUG
    if (OldAtt->Array().IsNull())
      std::cout << "BackUp Arr is Nullified" << std::endl;
#endif
  }
}

//=================================================================================================

void TDataStd_DeltaOnModificationOfIntArray::Apply()
{

  occ::handle<TDF_Attribute>         TDFAttribute = Attribute();
  occ::handle<TDataStd_IntegerArray> BackAtt = occ::down_cast<TDataStd_IntegerArray>(TDFAttribute);
  if (BackAtt.IsNull())
  {
#ifdef OCCT_DEBUG
    std::cout << "DeltaOnModificationOfIntArray::Apply: OldAtt is Null" << std::endl;
#endif
    return;
  }

  occ::handle<TDataStd_IntegerArray> aCurAtt;
  if (!Label().FindAttribute(BackAtt->ID(), aCurAtt))
  {

    Label().AddAttribute(BackAtt);
  }

  if (aCurAtt.IsNull())
  {
#ifdef OCCT_DEBUG
    std::cout << "DeltaOnModificationOfIntArray::Apply: CurAtt is Null" << std::endl;
#endif
    return;
  }
  else
    aCurAtt->Backup();

  int aCase;
  if (myUp1 == myUp2)
    aCase = 1;
  else if (myUp1 < myUp2)
    aCase = 2;
  else
    aCase = 3; // Up1 > Up2
  ////
  if (aCase == 1 && (myIndxes.IsNull() || myValues.IsNull()))
    return;

  int                 i;
  occ::handle<NCollection_HArray1<int>> IntArr = aCurAtt->Array();
  if (IntArr.IsNull())
    return;
  if (aCase == 1)
    for (i = 1; i <= myIndxes->Upper(); i++)
      IntArr->ChangeArray1().SetValue(myIndxes->Value(i), myValues->Value(i));
  else if (aCase == 2)
  {
    occ::handle<NCollection_HArray1<int>> intArr = new NCollection_HArray1<int>(IntArr->Lower(), myUp1);
    for (i = IntArr->Lower(); i <= myUp1 && i <= IntArr->Upper(); i++)
      intArr->SetValue(i, IntArr->Value(i));
    if (!myIndxes.IsNull() && !myValues.IsNull())
      for (i = 1; i <= myIndxes->Upper(); i++)
        intArr->ChangeArray1().SetValue(myIndxes->Value(i), myValues->Value(i));
    aCurAtt->myValue = intArr;
  }
  else
  { // aCase == 3
    int                 low    = IntArr->Lower();
    occ::handle<NCollection_HArray1<int>> intArr = new NCollection_HArray1<int>(low, myUp1);
    for (i = IntArr->Lower(); i <= myUp2 && i <= IntArr->Upper(); i++)
      intArr->SetValue(i, IntArr->Value(i));
    if (!myIndxes.IsNull() && !myValues.IsNull())
      for (i = 1; i <= myIndxes->Upper(); i++)
      {
#ifdef OCCT_DEBUG
        std::cout << "i = " << i << "  myIndxes->Upper = " << myIndxes->Upper() << std::endl;
        std::cout << "myIndxes->Value(i) = " << myIndxes->Value(i) << std::endl;
        std::cout << "myValues->Value(i) = " << myValues->Value(i) << std::endl;
#endif
        intArr->ChangeArray1().SetValue(myIndxes->Value(i), myValues->Value(i));
      }
    aCurAtt->myValue = intArr;
  }

#ifdef OCCT_DEBUG
  std::cout << " << Array Dump after Delta Apply >>" << std::endl;
  occ::handle<NCollection_HArray1<int>> IntArr2 = aCurAtt->Array();
  for (i = IntArr2->Lower(); i <= IntArr2->Upper() && i <= MAXUP; i++)
    std::cout << IntArr2->Value(i) << "  ";
  std::cout << std::endl;
#endif
}
