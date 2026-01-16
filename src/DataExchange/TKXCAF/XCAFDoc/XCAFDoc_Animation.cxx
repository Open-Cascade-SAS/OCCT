// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <XCAFDoc_Animation.hxx>

#include <Standard_GUID.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <XCAFAnimObjects_Object.hxx>
#include <XCAFAnimObjects_CustomOperation.hxx>
#include <XCAFAnimObjects_Orient.hxx>
#include <XCAFAnimObjects_Rotate.hxx>
#include <XCAFAnimObjects_Scale.hxx>
#include <XCAFAnimObjects_Skew.hxx>
#include <XCAFAnimObjects_Transform.hxx>
#include <XCAFAnimObjects_Translate.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE(XCAFDoc_Animation, TDataStd_GenericEmpty)

namespace
{

//=================================================================================================

const Standard_GUID& AnimRotateRefGUID()
{
  static const Standard_GUID ID("09135874-3B7E-4379-8BDB-E781422B8DD7");
  return ID;
}

//=================================================================================================

const Standard_GUID& AnimCustomRefGUID()
{
  static const Standard_GUID ID("1D0BC396-328D-45CC-B968-FD58DB7109A0");
  return ID;
}

//=================================================================================================

const Standard_GUID& AnimOrientRefGUID()
{
  static const Standard_GUID ID("F601BE38-D3F8-4594-90C8-04B790ACD08A");
  return ID;
}

//=================================================================================================

const Standard_GUID& AnimScaleRefGUID()
{
  static const Standard_GUID ID("40602308-A430-4912-A480-66DF8788338B");
  return ID;
}

//=================================================================================================

const Standard_GUID& AnimSkewRefGUID()
{
  static const Standard_GUID ID("9A260C95-B2D6-472D-AEB4-D802C7528FEE");
  return ID;
}

//=================================================================================================

const Standard_GUID& AnimTransformRefGUID()
{
  static const Standard_GUID ID("F26898A6-C7A8-4FC8-B328-4B442F935E7A");
  return ID;
}

//=================================================================================================

const Standard_GUID& AnimTranslateRefGUID()
{
  static const Standard_GUID ID("A8777A51-B13E-417A-82A0-6176246DD441");
  return ID;
}

//=================================================================================================

const Standard_GUID& AnimInterpolationRefGUID()
{
  static const Standard_GUID ID("4C565EBB-70C2-4934-B451-0B45C3460412");
  return ID;
}

//=================================================================================================

const Standard_GUID& AnimInvertRefGUID()
{
  static const Standard_GUID ID("7898D79A-6CCE-434C-A494-A37FC1931CC2");
  return ID;
}

//=================================================================================================

const Standard_GUID& AnimValuesDimensionGUID()
{
  static const Standard_GUID ID("95CBDC47-5A79-4229-9851-B6F04EAEE482");
  return ID;
}

//=================================================================================================

const Standard_GUID& AnimValuesGUID()
{
  static const Standard_GUID ID("C3CDFA73-1C9B-4674-BCAA-D1B7038AFE86");
  return ID;
}

//=================================================================================================

const Standard_GUID& AnimTimeStampsValuesGUID()
{
  static const Standard_GUID ID("AAF6F1A2-F764-4A4B-8984-BF7CF09B7646");
  return ID;
}

//=================================================================================================

const Standard_GUID& AnimRotateSubTypeGUID()
{
  static const Standard_GUID ID("A1E22D67-CD3E-4F8D-BD75-1DF04EF45266");
  return ID;
}
} // namespace

//=================================================================================================

XCAFDoc_Animation::XCAFDoc_Animation() {}

//=================================================================================================

const Standard_GUID& XCAFDoc_Animation::GetID()
{
  static Standard_GUID ANIMID("D755686B-872E-421E-8871-E98BE8051644");
  return ANIMID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc_Animation::AnimRefShapeGUID()
{
  static const Standard_GUID ID("0BE692B6-2E38-4FDC-A349-27615CF8784F");
  return ID;
}

//=================================================================================================

Handle(XCAFDoc_Animation) XCAFDoc_Animation::Set(const TDF_Label& theLabel)
{
  Handle(XCAFDoc_Animation) anAnimAttr;
  if (!theLabel.FindAttribute(XCAFDoc_Animation::GetID(), anAnimAttr))
  {
    anAnimAttr = new XCAFDoc_Animation();
    theLabel.AddAttribute(anAnimAttr);
  }
  return anAnimAttr;
}

//=================================================================================================

void XCAFDoc_Animation::SetObject(const Handle(XCAFAnimObjects_Object)& theObject)
{
  Backup();

  // Setting name
  const static TCollection_ExtendedString anObjName("Animation");
  TDataStd_Name::Set(Label(), anObjName);

  // Setting Interpolation type
  TDataStd_Integer::Set(Label(), AnimInterpolationRefGUID(), theObject->GetInterpolationType());

  Standard_Integer anOperInd = 1;
  // Setting ordered operations
  for (NCollection_List<Handle(XCAFAnimObjects_Operation)>::Iterator anIter(
         theObject->GetOrderedOperations());
       anIter.More();
       anIter.Next(), anOperInd++)
  {
    const TDF_Label aChild = Label().FindChild(anOperInd, true);
    aChild.ForgetAllAttributes(); // Clear old values

    const Handle(XCAFAnimObjects_Operation)& anOperation = anIter.Value();
    // Setting inverse flag
    if (anOperation->IsInverse())
    {
      TDataStd_UAttribute::Set(aChild, AnimInvertRefGUID());
    }
    // Setting operation type and name
    TDataStd_Name::Set(aChild, anOperation->GetTypeName());
    switch (anOperation->GetType())
    {
      case XCAFAnimObjects_OperationType_Custom:
        TDataStd_UAttribute::Set(aChild, AnimCustomRefGUID());
        break;
      case XCAFAnimObjects_OperationType_Orient:
        TDataStd_UAttribute::Set(aChild, AnimOrientRefGUID());
        break;
      case XCAFAnimObjects_OperationType_Rotate: {
        TDataStd_UAttribute::Set(aChild, AnimRotateRefGUID());
        const Handle(XCAFAnimObjects_Rotate) aRotate =
          Handle(XCAFAnimObjects_Rotate)::DownCast(anOperation);
        TDataStd_Integer::Set(aChild, AnimRotateSubTypeGUID(), aRotate->GetRotateType());
        break;
      }
      case XCAFAnimObjects_OperationType_Scale:
        TDataStd_UAttribute::Set(aChild, AnimScaleRefGUID());
        break;
      case XCAFAnimObjects_OperationType_Skew:
        TDataStd_UAttribute::Set(aChild, AnimSkewRefGUID());
        break;
      case XCAFAnimObjects_OperationType_Transform:
        TDataStd_UAttribute::Set(aChild, AnimTransformRefGUID());
        break;
      case XCAFAnimObjects_OperationType_Translate:
        TDataStd_UAttribute::Set(aChild, AnimTranslateRefGUID());
        break;
    }
    // Setting operation values
    const NCollection_Array2<double> anOperPresentation = anOperation->GeneralPresentation();
    Handle(TDataStd_IntegerArray)    aDimArr =
      TDataStd_IntegerArray::Set(aChild, AnimValuesDimensionGUID(), 1, 2);
    aDimArr->SetValue(1, anOperPresentation.RowLength());
    aDimArr->SetValue(2, anOperPresentation.ColLength());
    const int                  aNbValues = anOperPresentation.Length();
    Handle(TDataStd_RealArray) aValuesArr =
      TDataStd_RealArray::Set(aChild, AnimValuesGUID(), 1, aNbValues);
    int anOperValueInd = 1;
    for (NCollection_Array2<double>::Iterator aOperValIter(anOperPresentation); aOperValIter.More();
         aOperValIter.Next())
    {
      aValuesArr->SetValue(anOperValueInd++, aOperValIter.Value());
    }
    if (anOperation->HasTimeStamps())
    {
      const NCollection_Array1<double>& aTimeStamps = anOperation->TimeStamps();
      Handle(TDataStd_RealArray)        aTimeStampsAttr =
        TDataStd_RealArray::Set(aChild,
                                AnimTimeStampsValuesGUID(),
                                aTimeStamps.Lower(),
                                aTimeStamps.Upper());
      for (int aTimeStampInd = 1; aTimeStampInd <= aTimeStamps.Length(); aTimeStampInd++)
      {
        aTimeStampsAttr->SetValue(aTimeStampInd, aTimeStamps.Value(aTimeStampInd));
      }
    }
  }
}

//=================================================================================================

Handle(XCAFAnimObjects_Object) XCAFDoc_Animation::GetObject() const
{
  Handle(XCAFAnimObjects_Object) anObj = new XCAFAnimObjects_Object();
  Handle(TDataStd_Integer)       anIntType;
  if (Label().FindAttribute(AnimInterpolationRefGUID(), anIntType))
  {
    const XCAFAnimObjects_InterpolationType aType =
      static_cast<XCAFAnimObjects_InterpolationType>(anIntType->Get());
    anObj->SetInterpolationType(aType);
  }
  NCollection_List<Handle(XCAFAnimObjects_Operation)> anOrderedOperations;
  for (TDF_ChildIterator aChildIterator(Label()); aChildIterator.More(); aChildIterator.Next())
  {
    const TDF_Label&              anOperL    = aChildIterator.Value();
    XCAFAnimObjects_OperationType anOperType = XCAFAnimObjects_OperationType_Custom;
    Handle(TDataStd_UAttribute)   anOperTypeAttr;
    if (anOperL.FindAttribute(AnimCustomRefGUID(), anOperTypeAttr))
    {
      anOperType = XCAFAnimObjects_OperationType_Custom;
    }
    else if (anOperL.FindAttribute(AnimOrientRefGUID(), anOperTypeAttr))
    {
      anOperType = XCAFAnimObjects_OperationType_Orient;
    }
    else if (anOperL.FindAttribute(AnimRotateRefGUID(), anOperTypeAttr))
    {
      anOperType = XCAFAnimObjects_OperationType_Rotate;
    }
    else if (anOperL.FindAttribute(AnimScaleRefGUID(), anOperTypeAttr))
    {
      anOperType = XCAFAnimObjects_OperationType_Scale;
    }
    else if (anOperL.FindAttribute(AnimSkewRefGUID(), anOperTypeAttr))
    {
      anOperType = XCAFAnimObjects_OperationType_Skew;
    }
    else if (anOperL.FindAttribute(AnimTransformRefGUID(), anOperTypeAttr))
    {
      anOperType = XCAFAnimObjects_OperationType_Transform;
    }
    else if (anOperL.FindAttribute(AnimTranslateRefGUID(), anOperTypeAttr))
    {
      anOperType = XCAFAnimObjects_OperationType_Translate;
    }
    if (anOperTypeAttr.IsNull())
    {
      continue;
    }
    Handle(TDataStd_IntegerArray) aDimAtrArr;
    if (!anOperL.FindAttribute(AnimValuesDimensionGUID(), aDimAtrArr) || aDimAtrArr->Length() != 2)
    {
      continue;
    }
    Handle(TDataStd_RealArray) aValuesAtrArr;
    if (!anOperL.FindAttribute(AnimValuesGUID(), aValuesAtrArr)
        || (aDimAtrArr->Value(1) * aDimAtrArr->Value(2)) != aValuesAtrArr->Length())
    {
      continue;
    }
    NCollection_Array2<double> aValuesArr(1, aDimAtrArr->Value(2), 1, aDimAtrArr->Value(1));
    int                        aValuesInd = 1;
    for (NCollection_Array2<double>::Iterator aOperValIter(aValuesArr); aOperValIter.More();
         aOperValIter.Next(), aValuesInd++)
    {
      aOperValIter.ChangeValue() = aValuesAtrArr->Value(aValuesInd);
    }
    Handle(TDataStd_RealArray) aTimeStampsAttr;
    NCollection_Array1<double> aTimeStampsArr;
    if (anOperL.FindAttribute(AnimTimeStampsValuesGUID(), aTimeStampsAttr)
        && aTimeStampsAttr->Length() == aValuesArr.NbRows())
    {
      aTimeStampsArr.Resize(1, aTimeStampsAttr->Length(), false);
      for (int anTimeStampInd = 1; anTimeStampInd <= aTimeStampsAttr->Length(); anTimeStampInd++)
      {
        aTimeStampsArr.SetValue(anTimeStampInd, aTimeStampsAttr->Value(anTimeStampInd));
      }
    }
    Handle(TDataStd_UAttribute) anInvertAttr;
    const bool anIsInvert = anOperL.FindAttribute(AnimInvertRefGUID(), anInvertAttr);
    Handle(XCAFAnimObjects_Operation) aNewOperObj;
    switch (anOperType)
    {
      case XCAFAnimObjects_OperationType_Custom: {
        Handle(TDataStd_Name) aOperNameAttr;
        if (!anOperL.FindAttribute(TDataStd_Name::GetID(), aOperNameAttr))
        {
          continue;
        }
        const TCollection_AsciiString aOperName = aOperNameAttr->Get();
        aNewOperObj = new XCAFAnimObjects_CustomOperation(aValuesArr, aTimeStampsArr, aOperName);
        break;
      }
      case XCAFAnimObjects_OperationType_Orient: {
        aNewOperObj = new XCAFAnimObjects_Orient(aValuesArr, aTimeStampsArr);
        break;
      }
      case XCAFAnimObjects_OperationType_Rotate: {
        Handle(TDataStd_Integer) aRotateTypeAttr;
        if (!anOperL.FindAttribute(AnimRotateSubTypeGUID(), aRotateTypeAttr))
        {
          continue;
        }
        const XCAFAnimObjects_Rotate::XCAFAnimObjects_Rotate_Type aRotateType =
          static_cast<XCAFAnimObjects_Rotate::XCAFAnimObjects_Rotate_Type>(aRotateTypeAttr->Get());
        aNewOperObj = new XCAFAnimObjects_Rotate(aValuesArr, aTimeStampsArr, aRotateType);
        break;
      }
      case XCAFAnimObjects_OperationType_Scale: {
        aNewOperObj = new XCAFAnimObjects_Scale(aValuesArr, aTimeStampsArr);
        break;
      }
      case XCAFAnimObjects_OperationType_Skew: {
        aNewOperObj = new XCAFAnimObjects_Skew(aValuesArr, aTimeStampsArr);
        break;
      }
      case XCAFAnimObjects_OperationType_Transform: {
        aNewOperObj = new XCAFAnimObjects_Transform(aValuesArr, aTimeStampsArr);
        break;
      }
      case XCAFAnimObjects_OperationType_Translate: {
        aNewOperObj = new XCAFAnimObjects_Translate(aValuesArr, aTimeStampsArr);
        break;
      }
    }
    if (aNewOperObj.IsNull())
    {
      continue;
    }
    aNewOperObj->SetInverse(anIsInvert);
    anOrderedOperations.Append(aNewOperObj);
  }
  anObj->ChangeOrderedOperations() = anOrderedOperations;
  return anObj;
}

//=================================================================================================

const Standard_GUID& XCAFDoc_Animation::ID() const
{
  return GetID();
}

//=================================================================================================

void XCAFDoc_Animation::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  (void)theOStream;
  (void)theDepth;
}
