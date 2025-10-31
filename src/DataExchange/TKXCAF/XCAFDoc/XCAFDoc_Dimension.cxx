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

#include <XCAFDoc_Dimension.hxx>

#include <TDF_RelocationTable.hxx>
#include <TDF_ChildIterator.hxx>
#include <XCAFDoc.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_RealArray.hxx>
#include <TNaming_Builder.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopoDS.hxx>
#include <XCAFDimTolObjects_DimensionObject.hxx>
#include <TNaming_Tool.hxx>
#include <TDataStd_Name.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE(XCAFDoc_Dimension, TDataStd_GenericEmpty)

enum ChildLab
{
  ChildLab_Begin = 1,
  ChildLab_Type  = ChildLab_Begin,
  ChildLab_Value,
  ChildLab_Qualifier,
  ChildLab_Class,
  ChildLab_Dec,
  ChildLab_Modifiers,
  ChildLab_Path,
  ChildLab_Dir,
  ChildLab_Pnt1,
  ChildLab_Pnt2,
  ChildLab_PlaneLoc,
  ChildLab_PlaneN,
  ChildLab_PlaneRef,
  ChildLab_PntText,
  ChildLab_Presentation,
  ChildLab_Descriptions,
  ChildLab_DescriptionNames,
  ChildLab_AngularQualifier,
  ChildLab_ConnectionAxis1,
  ChildLab_ConnectionAxis2,
  ChildLab_End
};

//=================================================================================================

XCAFDoc_Dimension::XCAFDoc_Dimension() {}

//=================================================================================================

const Standard_GUID& XCAFDoc_Dimension::GetID()
{
  static Standard_GUID DGTID("58ed092c-44de-11d8-8776-001083004c77");
  // static Standard_GUID ID("efd212e9-6dfd-11d4-b9c8-0060b0ee281b");
  return DGTID;
  // return ID;
}

//=================================================================================================

Handle(XCAFDoc_Dimension) XCAFDoc_Dimension::Set(const TDF_Label& theLabel)
{
  Handle(XCAFDoc_Dimension) A;
  if (!theLabel.FindAttribute(XCAFDoc_Dimension::GetID(), A))
  {
    A = new XCAFDoc_Dimension();
    theLabel.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

static void setString(const TDF_Label& theLabel, const Handle(TCollection_HAsciiString)& theStr)
{
  if (theStr.IsNull())
    return;

  TCollection_ExtendedString aStr(theStr->String());
  TDataStd_Name::Set(theLabel, aStr);
}

//=================================================================================================

static void setRealArrayXYZ(const TDF_Label& theLabel, const gp_XYZ& theArr)
{
  Handle(TColStd_HArray1OfReal) anArr = new TColStd_HArray1OfReal(1, 3);
  anArr->SetValue(1, theArr.X());
  anArr->SetValue(2, theArr.Y());
  anArr->SetValue(3, theArr.Z());
  Handle(TDataStd_RealArray) anArrayAttr = TDataStd_RealArray::Set(theLabel, 1, 3);
  if (!anArrayAttr.IsNull())
    anArrayAttr->ChangeArray(anArr);
}

//=================================================================================================

static Handle(TCollection_HAsciiString) getString(const TDF_Label& theLabel)
{
  Handle(TDataStd_Name)            aStrAttr;
  Handle(TCollection_HAsciiString) aStr;
  if (theLabel.FindAttribute(TDataStd_Name::GetID(), aStrAttr))
  {
    const TCollection_ExtendedString& aName = aStrAttr->Get();
    if (!aName.IsEmpty())
      aStr = new TCollection_HAsciiString(aName);
  }
  return aStr;
}

//=================================================================================================

static bool getRealArrayXYZ(const TDF_Label& theLabel, gp_XYZ& theXYZ)
{
  Handle(TDataStd_RealArray) anArrayAttr;
  if (theLabel.FindAttribute(TDataStd_RealArray::GetID(), anArrayAttr) && !anArrayAttr.IsNull()
      && anArrayAttr->Length() == 3)
  {
    theXYZ.SetX(anArrayAttr->Value(1));
    theXYZ.SetY(anArrayAttr->Value(2));
    theXYZ.SetZ(anArrayAttr->Value(3));
    return true;
  }
  return false;
}

//=================================================================================================

void XCAFDoc_Dimension::SetObject(const Handle(XCAFDimTolObjects_DimensionObject)& theObject)
{
  Backup();

  setString(Label(), theObject->GetSemanticName());

  for (int aChild = ChildLab_Begin; aChild < ChildLab_End; aChild++)
  {
    Label().FindChild(aChild).ForgetAllAttributes();
  }
  Handle(TDataStd_Integer) aType =
    TDataStd_Integer::Set(Label().FindChild(ChildLab_Type), theObject->GetType());

  if (!theObject->GetValues().IsNull())
  {
    Handle(TDataStd_RealArray) aVal = TDataStd_RealArray::Set(
      Label().FindChild(ChildLab_Value),
      theObject->GetValues()->Lower(),
      theObject->GetValues()->Lower() + theObject->GetValues()->Length() - 1);
    if (!aVal.IsNull())
      aVal->ChangeArray(theObject->GetValues());
  }

  Handle(TDataStd_Integer) aQualifier =
    TDataStd_Integer::Set(Label().FindChild(ChildLab_Qualifier), theObject->GetQualifier());

  Handle(TDataStd_Integer) anAngularQualifier =
    TDataStd_Integer::Set(Label().FindChild(ChildLab_AngularQualifier),
                          theObject->GetAngularQualifier());

  Standard_Boolean                        aH;
  XCAFDimTolObjects_DimensionFormVariance aF;
  XCAFDimTolObjects_DimensionGrade        aG;
  theObject->GetClassOfTolerance(aH, aF, aG);
  Handle(TColStd_HArray1OfInteger) anArrI;
  if (aF != XCAFDimTolObjects_DimensionFormVariance_None)
  {
    anArrI = new TColStd_HArray1OfInteger(1, 3);
    anArrI->SetValue(1, aH);
    anArrI->SetValue(2, aF);
    anArrI->SetValue(3, aG);
    Handle(TDataStd_IntegerArray) aClass =
      TDataStd_IntegerArray::Set(Label().FindChild(ChildLab_Class), 1, 3);
    if (!aClass.IsNull())
      aClass->ChangeArray(anArrI);
  }

  Standard_Integer aL, aR;
  theObject->GetNbOfDecimalPlaces(aL, aR);
  if (aL > 0 || aR > 0)
  {
    anArrI = new TColStd_HArray1OfInteger(1, 2);
    anArrI->SetValue(1, aL);
    anArrI->SetValue(2, aR);
    Handle(TDataStd_IntegerArray) aDec =
      TDataStd_IntegerArray::Set(Label().FindChild(ChildLab_Dec), 1, 2);
    if (!aDec.IsNull())
      aDec->ChangeArray(anArrI);
  }

  if (theObject->GetModifiers().Length() > 0)
  {
    anArrI = new TColStd_HArray1OfInteger(1, theObject->GetModifiers().Length());
    for (Standard_Integer i = 1; i <= theObject->GetModifiers().Length(); i++)
      anArrI->SetValue(i, theObject->GetModifiers().Value(i));
    Handle(TDataStd_IntegerArray) aModifiers =
      TDataStd_IntegerArray::Set(Label().FindChild(ChildLab_Modifiers),
                                 1,
                                 theObject->GetModifiers().Length());
    if (!aModifiers.IsNull())
      aModifiers->ChangeArray(anArrI);
  }

  if (!theObject->GetPath().IsNull())
  {
    TNaming_Builder tnBuild(Label().FindChild(ChildLab_Path));
    tnBuild.Generated(theObject->GetPath());
  }

  if (theObject->GetType() == XCAFDimTolObjects_DimensionType_Location_Oriented)
  {
    gp_Dir aD;
    theObject->GetDirection(aD);
    setRealArrayXYZ(Label().FindChild(ChildLab_Dir), aD.XYZ());
  }

  if (theObject->HasPoint())
  {
    // put point
    setRealArrayXYZ(Label().FindChild(ChildLab_Pnt1), theObject->GetPoint().XYZ());
    setString(Label().FindChild(ChildLab_Pnt1), theObject->GetConnectionName());

    if (!theObject->IsPointConnection())
    {
      // put additional info about axis
      gp_Ax2 anAxis1 = theObject->GetConnectionAxis();

      Handle(TColStd_HArray1OfReal) aAxisArr = new TColStd_HArray1OfReal(1, 6);
      for (Standard_Integer i = 1; i <= 3; i++)
      {
        aAxisArr->SetValue(i, anAxis1.Direction().Coord(i));
        aAxisArr->SetValue(i + 3, anAxis1.XDirection().Coord(i));
      }
      Handle(TDataStd_RealArray) aAxis =
        TDataStd_RealArray::Set(Label().FindChild(ChildLab_ConnectionAxis1), 1, 6);
      if (!aAxis.IsNull())
        aAxis->ChangeArray(aAxisArr);
    }
  }

  if (theObject->HasPoint2())
  {
    setRealArrayXYZ(Label().FindChild(ChildLab_Pnt2), theObject->GetPoint2().XYZ());
    setString(Label().FindChild(ChildLab_Pnt2), theObject->GetConnectionName2());

    if (!theObject->IsPointConnection2())
    {
      // put additional info about axis
      gp_Ax2 anAxis2 = theObject->GetConnectionAxis2();

      Handle(TColStd_HArray1OfReal) aAxisArr = new TColStd_HArray1OfReal(1, 6);
      for (Standard_Integer i = 1; i <= 3; i++)
      {
        aAxisArr->SetValue(i, anAxis2.Direction().Coord(i));
        aAxisArr->SetValue(i + 3, anAxis2.XDirection().Coord(i));
      }
      Handle(TDataStd_RealArray) aAxis =
        TDataStd_RealArray::Set(Label().FindChild(ChildLab_ConnectionAxis2), 1, 6);
      if (!aAxis.IsNull())
        aAxis->ChangeArray(aAxisArr);
    }
  }

  if (theObject->HasPlane())
  {
    gp_Ax2 anAx = theObject->GetPlane();
    setRealArrayXYZ(Label().FindChild(ChildLab_PlaneLoc), anAx.Location().XYZ());
    setRealArrayXYZ(Label().FindChild(ChildLab_PlaneN), anAx.Direction().XYZ());
    setRealArrayXYZ(Label().FindChild(ChildLab_PlaneRef), anAx.XDirection().XYZ());
  }

  if (theObject->HasTextPoint())
  {
    gp_Pnt aPntText = theObject->GetPointTextAttach();
    setRealArrayXYZ(Label().FindChild(ChildLab_PntText), aPntText.XYZ());
  }

  TopoDS_Shape aPresentation = theObject->GetPresentation();
  if (!aPresentation.IsNull())
  {
    TDF_Label       aLPres = Label().FindChild(ChildLab_Presentation);
    TNaming_Builder tnBuild(aLPres);
    tnBuild.Generated(aPresentation);
    setString(aLPres, theObject->GetPresentationName());
  }

  if (theObject->HasDescriptions())
  {
    Handle(TColStd_HArray1OfExtendedString) aDescrArr =
      new TColStd_HArray1OfExtendedString(1, theObject->NbDescriptions());
    Handle(TColStd_HArray1OfExtendedString) aDescrNameArr =
      new TColStd_HArray1OfExtendedString(1, theObject->NbDescriptions());
    for (Standard_Integer i = 0; i < theObject->NbDescriptions(); i++)
    {
      TCollection_ExtendedString aDescr(theObject->GetDescription(i)->String());
      aDescrArr->SetValue(i + 1, aDescr);
      TCollection_ExtendedString aDescrName(theObject->GetDescriptionName(i)->String());
      aDescrNameArr->SetValue(i + 1, aDescrName);
    }
    Handle(TDataStd_ExtStringArray) aDescriptions =
      TDataStd_ExtStringArray::Set(Label().FindChild(ChildLab_Descriptions),
                                   1,
                                   theObject->NbDescriptions());
    Handle(TDataStd_ExtStringArray) aDescriptionNames =
      TDataStd_ExtStringArray::Set(Label().FindChild(ChildLab_DescriptionNames),
                                   1,
                                   theObject->NbDescriptions());
    if (!aDescriptions.IsNull())
      aDescriptions->ChangeArray(aDescrArr);
    if (!aDescriptionNames.IsNull())
      aDescriptionNames->ChangeArray(aDescrNameArr);
  }
}

//=================================================================================================

Handle(XCAFDimTolObjects_DimensionObject) XCAFDoc_Dimension::GetObject() const
{
  Handle(XCAFDimTolObjects_DimensionObject) anObj = new XCAFDimTolObjects_DimensionObject();
  gp_XYZ                                    aXYZValue;

  anObj->SetSemanticName(getString(Label()));

  Handle(TDataStd_Integer) aType;
  if (Label().FindChild(ChildLab_Type).FindAttribute(TDataStd_Integer::GetID(), aType))
  {
    anObj->SetType((XCAFDimTolObjects_DimensionType)aType->Get());
  }

  Handle(TDataStd_RealArray) aVal;
  if (Label().FindChild(ChildLab_Value).FindAttribute(TDataStd_RealArray::GetID(), aVal)
      && !aVal->Array().IsNull())
  {
    anObj->SetValues(aVal->Array());
  }

  Handle(TDataStd_Integer) aQualifier;
  if (Label().FindChild(ChildLab_Qualifier).FindAttribute(TDataStd_Integer::GetID(), aQualifier))
  {
    anObj->SetQualifier((XCAFDimTolObjects_DimensionQualifier)aQualifier->Get());
  }

  Handle(TDataStd_Integer) anAngularQualifier;
  if (Label()
        .FindChild(ChildLab_AngularQualifier)
        .FindAttribute(TDataStd_Integer::GetID(), anAngularQualifier))
  {
    anObj->SetAngularQualifier((XCAFDimTolObjects_AngularQualifier)anAngularQualifier->Get());
  }

  Handle(TDataStd_IntegerArray) aClass;
  if (Label().FindChild(ChildLab_Class).FindAttribute(TDataStd_IntegerArray::GetID(), aClass)
      && !aClass->Array().IsNull() && aClass->Array()->Length() > 0)
  {
    anObj->SetClassOfTolerance(aClass->Array()->Value(1) != 0,
                               (XCAFDimTolObjects_DimensionFormVariance)aClass->Array()->Value(2),
                               (XCAFDimTolObjects_DimensionGrade)aClass->Array()->Value(3));
  }

  Handle(TDataStd_IntegerArray) aDec;
  if (Label().FindChild(ChildLab_Dec).FindAttribute(TDataStd_IntegerArray::GetID(), aDec)
      && !aDec->Array().IsNull() && aDec->Array()->Length() > 0)
  {
    anObj->SetNbOfDecimalPlaces(aDec->Array()->Value(1), aDec->Array()->Value(2));
  }

  Handle(TDataStd_IntegerArray) aModifiers;
  if (Label()
        .FindChild(ChildLab_Modifiers)
        .FindAttribute(TDataStd_IntegerArray::GetID(), aModifiers)
      && !aModifiers->Array().IsNull())
  {
    XCAFDimTolObjects_DimensionModifiersSequence aM;
    for (Standard_Integer i = 1; i <= aModifiers->Array()->Length(); i++)
      aM.Append((XCAFDimTolObjects_DimensionModif)aModifiers->Array()->Value(i));
    anObj->SetModifiers(aM);
  }

  Handle(TNaming_NamedShape) aShape;
  if (Label().FindChild(ChildLab_Path).FindAttribute(TNaming_NamedShape::GetID(), aShape)
      && !aShape.IsNull())
  {
    anObj->SetPath(TopoDS::Edge(aShape->Get()));
  }

  if (getRealArrayXYZ(Label().FindChild(ChildLab_Dir), aXYZValue))
  {
    anObj->SetDirection(aXYZValue);
  }

  Handle(TDataStd_RealArray) anAxis1;
  if (getRealArrayXYZ(Label().FindChild(ChildLab_Pnt1), aXYZValue))
  {
    gp_Pnt aP(aXYZValue);
    anObj->SetConnectionName(getString(Label().FindChild(ChildLab_Pnt1)));

    if (Label()
          .FindChild(ChildLab_ConnectionAxis1)
          .FindAttribute(TDataStd_RealArray::GetID(), anAxis1)
        && anAxis1->Length() == 6)
    {
      gp_Ax2 anAx1(aP,
                   gp_Dir(anAxis1->Value(1), anAxis1->Value(2), anAxis1->Value(3)),
                   gp_Dir(anAxis1->Value(4), anAxis1->Value(5), anAxis1->Value(6)));
      anObj->SetConnectionAxis(anAx1);
    }
    else
    {
      anObj->SetPoint(aP);
    }
  }

  Handle(TDataStd_RealArray) anAxis2;
  if (getRealArrayXYZ(Label().FindChild(ChildLab_Pnt2), aXYZValue))
  {
    gp_Pnt aP(aXYZValue);
    anObj->SetConnectionName2(getString(Label().FindChild(ChildLab_Pnt2)));

    if (Label()
          .FindChild(ChildLab_ConnectionAxis2)
          .FindAttribute(TDataStd_RealArray::GetID(), anAxis2)
        && anAxis2->Length() == 6)
    {
      gp_Ax2 anAx2(aP,
                   gp_Dir(anAxis2->Value(1), anAxis2->Value(2), anAxis2->Value(3)),
                   gp_Dir(anAxis2->Value(4), anAxis2->Value(5), anAxis2->Value(6)));
      anObj->SetConnectionAxis2(anAx2);
    }
    else
    {
      anObj->SetPoint2(aP);
    }
  }

  gp_XYZ aLoc, aN, aR;
  if (getRealArrayXYZ(Label().FindChild(ChildLab_PlaneLoc), aLoc)
      && getRealArrayXYZ(Label().FindChild(ChildLab_PlaneN), aN)
      && getRealArrayXYZ(Label().FindChild(ChildLab_PlaneRef), aR))
  {
    gp_Ax2 anAx(aLoc, aN, aR);
    anObj->SetPlane(anAx);
  }

  if (getRealArrayXYZ(Label().FindChild(ChildLab_PntText), aXYZValue))
  {
    anObj->SetPointTextAttach(aXYZValue);
  }

  Handle(TNaming_NamedShape) aNS;
  TDF_Label                  aLPres = Label().FindChild(ChildLab_Presentation);
  if (aLPres.FindAttribute(TNaming_NamedShape::GetID(), aNS))
  {
    TopoDS_Shape aPresentation = TNaming_Tool::GetShape(aNS);
    if (!aPresentation.IsNull())
    {
      anObj->SetPresentation(aPresentation, getString(aLPres));
    }
  }

  Handle(TDataStd_ExtStringArray) aDescriptions, aDescriptionNames;
  if (Label()
        .FindChild(ChildLab_Descriptions)
        .FindAttribute(TDataStd_ExtStringArray::GetID(), aDescriptions)
      && Label()
           .FindChild(ChildLab_DescriptionNames)
           .FindAttribute(TDataStd_ExtStringArray::GetID(), aDescriptionNames))
  {
    for (Standard_Integer i = 1; i <= aDescriptions->Length(); i++)
    {
      Handle(TCollection_HAsciiString) aDescription, aDescriptionName;
      aDescription = new TCollection_HAsciiString(TCollection_AsciiString(aDescriptions->Value(i)));
      aDescriptionName =
        new TCollection_HAsciiString(TCollection_AsciiString(aDescriptionNames->Value(i)));
      anObj->AddDescription(aDescription, aDescriptionName);
    }
  }
  return anObj;
}

//=================================================================================================

const Standard_GUID& XCAFDoc_Dimension::ID() const
{
  return GetID();
}

//=================================================================================================

void XCAFDoc_Dimension::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDataStd_GenericEmpty)

  Handle(XCAFDimTolObjects_DimensionObject) anObject = GetObject();
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, anObject.get())
}
