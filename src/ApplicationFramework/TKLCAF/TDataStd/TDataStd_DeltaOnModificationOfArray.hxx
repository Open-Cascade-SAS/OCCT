// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef TDataStd_DeltaOnModificationOfArray_HeaderFile
#define TDataStd_DeltaOnModificationOfArray_HeaderFile

#include <NCollection_HArray1.hxx>
#include <NCollection_List.hxx>
#include <TDF_Label.hxx>

//! Template helper providing the shared algorithm for array delta-on-modification classes.
//! Used by TDataStd_DeltaOnModificationOf{Real,Int,Byte,ExtString}Array.
class TDataStd_DeltaOnModificationOfArray
{
public:
  //! Computes the delta between two arrays. Populates theIndxes and theValues
  //! with the changed indices and their old values from theOldArr.
  //! @param[in] theOldArr old (backed-up) array
  //! @param[in] theCurArr current array
  //! @param[out] theIndxes indices that differ
  //! @param[out] theValues old values at those indices
  //! @param[out] theUp1 upper bound of the old array
  //! @param[out] theUp2 upper bound of the current array
  template <typename ValueType>
  static void ComputeDelta(const occ::handle<NCollection_HArray1<ValueType>>& theOldArr,
                           const occ::handle<NCollection_HArray1<ValueType>>& theCurArr,
                           occ::handle<NCollection_HArray1<int>>&             theIndxes,
                           occ::handle<NCollection_HArray1<ValueType>>&       theValues,
                           int&                                               theUp1,
                           int&                                               theUp2)
  {
    if (theOldArr.IsNull() || theCurArr.IsNull())
      return;
    if (theOldArr == theCurArr)
      return;

    theUp1 = theOldArr->Upper();
    theUp2 = theCurArr->Upper();

    int aN    = 0;
    int aCase = 0;
    if (theUp1 == theUp2)
    {
      aCase = 1;
      aN    = theUp1;
    }
    else if (theUp1 < theUp2)
    {
      aCase = 2;
      aN    = theUp1;
    }
    else
    {
      aCase = 3;
      aN    = theUp2;
    }

    NCollection_List<int> aList;
    for (int i = theOldArr->Lower(); i <= aN; i++)
    {
      if (theOldArr->Value(i) != theCurArr->Value(i))
        aList.Append(i);
    }
    if (aCase == 3)
    {
      for (int i = aN + 1; i <= theUp1; i++)
        aList.Append(i);
    }

    if (aList.Extent())
    {
      theIndxes = new NCollection_HArray1<int>(1, aList.Extent());
      theValues = new NCollection_HArray1<ValueType>(1, aList.Extent());
      NCollection_List<int>::Iterator anIt(aList);
      for (int i = 1; anIt.More(); anIt.Next(), i++)
      {
        theIndxes->SetValue(i, anIt.Value());
        theValues->SetValue(i, theOldArr->Value(anIt.Value()));
      }
    }
  }

  //! Applies the stored delta to the current array.
  //! @param[in,out] theCurArr current array to modify in-place
  //! @param[in] theIndxes stored changed indices
  //! @param[in] theValues stored old values
  //! @param[in] theUp1 upper bound of the old array
  //! @param[in] theUp2 upper bound of the current array
  //! @return new array handle if the array was resized (cases 2,3), null otherwise
  template <typename ValueType>
  static occ::handle<NCollection_HArray1<ValueType>> ApplyDelta(
    const occ::handle<NCollection_HArray1<ValueType>>& theCurArr,
    const occ::handle<NCollection_HArray1<int>>&       theIndxes,
    const occ::handle<NCollection_HArray1<ValueType>>& theValues,
    const int                                          theUp1,
    const int                                          theUp2)
  {
    if (theCurArr.IsNull())
      return occ::handle<NCollection_HArray1<ValueType>>();

    int aCase;
    if (theUp1 == theUp2)
      aCase = 1;
    else if (theUp1 < theUp2)
      aCase = 2;
    else
      aCase = 3;

    if (aCase == 1 && (theIndxes.IsNull() || theValues.IsNull()))
      return occ::handle<NCollection_HArray1<ValueType>>();

    if (aCase == 1)
    {
      for (int i = 1; i <= theIndxes->Upper(); i++)
        theCurArr->ChangeArray1().SetValue(theIndxes->Value(i), theValues->Value(i));
      return occ::handle<NCollection_HArray1<ValueType>>();
    }

    // Cases 2 and 3: array is resized
    occ::handle<NCollection_HArray1<ValueType>> aNewArr =
      new NCollection_HArray1<ValueType>(theCurArr->Lower(), theUp1);
    const int aCopyUp = (aCase == 2) ? theUp1 : theUp2;
    for (int i = theCurArr->Lower(); i <= aCopyUp && i <= theCurArr->Upper(); i++)
      aNewArr->SetValue(i, theCurArr->Value(i));
    if (!theIndxes.IsNull() && !theValues.IsNull())
    {
      for (int i = 1; i <= theIndxes->Upper(); i++)
        aNewArr->ChangeArray1().SetValue(theIndxes->Value(i), theValues->Value(i));
    }
    return aNewArr;
  }
};

#endif // TDataStd_DeltaOnModificationOfArray_HeaderFile
