// Created on: 2007-12-04
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

#include <TDataStd_DeltaOnModificationOfExtStringArray.hxx>

#include <Standard_Type.hxx>
#include <TDataStd_DeltaOnModificationOfArray.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_Label.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_DeltaOnModificationOfExtStringArray, TDF_DeltaOnModification)

//=================================================================================================

TDataStd_DeltaOnModificationOfExtStringArray::TDataStd_DeltaOnModificationOfExtStringArray(
  const occ::handle<TDataStd_ExtStringArray>& OldAtt)
    : TDF_DeltaOnModification(OldAtt),
      myUp1(0),
      myUp2(0)
{
  occ::handle<TDataStd_ExtStringArray> CurrAtt;
  if (Label().FindAttribute(OldAtt->ID(), CurrAtt))
  {
    TDataStd_DeltaOnModificationOfArray::ComputeDelta(OldAtt->Array(),
                                                      CurrAtt->Array(),
                                                      myIndxes,
                                                      myValues,
                                                      myUp1,
                                                      myUp2);
    OldAtt->RemoveArray();
  }
}

//=================================================================================================

void TDataStd_DeltaOnModificationOfExtStringArray::Apply()
{
  occ::handle<TDF_Attribute>           TDFAttribute = Attribute();
  occ::handle<TDataStd_ExtStringArray> BackAtt =
    occ::down_cast<TDataStd_ExtStringArray>(TDFAttribute);
  if (BackAtt.IsNull())
    return;

  occ::handle<TDataStd_ExtStringArray> aCurAtt;
  if (!Label().FindAttribute(BackAtt->ID(), aCurAtt))
    Label().AddAttribute(BackAtt);

  if (aCurAtt.IsNull())
    return;

  aCurAtt->Backup();

  occ::handle<NCollection_HArray1<TCollection_ExtendedString>> aNewArr =
    TDataStd_DeltaOnModificationOfArray::ApplyDelta(aCurAtt->Array(),
                                                    myIndxes,
                                                    myValues,
                                                    myUp1,
                                                    myUp2);
  if (!aNewArr.IsNull())
    aCurAtt->myValue = aNewArr;
}
