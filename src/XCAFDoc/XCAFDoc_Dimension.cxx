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

#include <gp_Pln.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_ChildIterator.hxx>
#include <XCAFDoc.hxx>
#include <TDataStd_TreeNode.hxx>
#include <Precision.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataXtd_Axis.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDataXtd_Point.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopoDS.hxx>
#include <XCAFDimTolObjects_DimensionObject.hxx>
#include <TNaming_Tool.hxx>
#include <TDataStd_Name.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_Dimension,TDF_Attribute)

enum ChildLab
{
  ChildLab_Type = 1,
  ChildLab_Value,
  ChildLab_Qualifier,
  ChildLab_Class,
  ChildLab_Dec,
  ChildLab_Modifiers,
  ChildLab_Path,
  ChildLab_Dir,
  ChildLab_Pnt1,
  ChildLab_Pnt2,
  ChildLab_Plane,
  ChildLab_PntText,
  ChildLab_Presentation,
  ChildLab_Descriptions,
  ChildLab_DescriptionNames
};

//=======================================================================
//function : XCAFDoc_Dimension
//purpose  : 
//=======================================================================

XCAFDoc_Dimension::XCAFDoc_Dimension()
{
}


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_Dimension::GetID() 
{
  static Standard_GUID DGTID ("58ed092c-44de-11d8-8776-001083004c77");
  //static Standard_GUID ID("efd212e9-6dfd-11d4-b9c8-0060b0ee281b");
  return DGTID; 
  //return ID;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(XCAFDoc_Dimension) XCAFDoc_Dimension::Set(const TDF_Label& theLabel) 
{
  Handle(XCAFDoc_Dimension) A;
  if (!theLabel.FindAttribute(XCAFDoc_Dimension::GetID(), A)) {
    A = new XCAFDoc_Dimension();
    theLabel.AddAttribute(A);
  }
  return A;
}

//=======================================================================
//function : SetObject
//purpose  : 
//=======================================================================
void XCAFDoc_Dimension::SetObject (const Handle(XCAFDimTolObjects_DimensionObject)& theObject)
{
  Backup();

  //Label().ForForgetAllAttributes();
  TDF_ChildIterator anIter(Label());
  for(;anIter.More(); anIter.Next())
  {
    anIter.Value().ForgetAllAttributes();
  }
  Handle(TDataStd_Integer) aType = TDataStd_Integer::Set(Label().FindChild(ChildLab_Type), theObject->GetType());

  if(!theObject->GetValues().IsNull())
  {
    Handle(TDataStd_RealArray) aVal = TDataStd_RealArray::Set(Label().FindChild(ChildLab_Value), theObject->GetValues()->Lower(), 
                                      theObject->GetValues()->Lower() + theObject->GetValues()->Length() - 1);
    if(!aVal.IsNull())
      aVal->ChangeArray(theObject->GetValues());
  }

  Handle(TDataStd_Integer) aQualifier = TDataStd_Integer::Set(Label().FindChild(ChildLab_Qualifier), theObject->GetQualifier());
 
  Standard_Boolean aH;
  XCAFDimTolObjects_DimensionFormVariance aF;
  XCAFDimTolObjects_DimensionGrade aG;
  theObject->GetClassOfTolerance(aH,aF,aG);
  Handle(TColStd_HArray1OfInteger) anArrI;
  if(aF != XCAFDimTolObjects_DimensionFormVariance_None)
  {
    anArrI = new TColStd_HArray1OfInteger(1,3);
    anArrI->SetValue(1,aH);
    anArrI->SetValue(2,aF);
    anArrI->SetValue(3,aG);
    Handle(TDataStd_IntegerArray) aClass = TDataStd_IntegerArray::Set(Label().FindChild(ChildLab_Class), 1, 3);
    if(!aClass.IsNull())
      aClass->ChangeArray(anArrI);
  }

  Standard_Integer aL, aR;
  theObject->GetNbOfDecimalPlaces(aL, aR);
  if (aL > 0 || aR > 0)
  {
    anArrI = new TColStd_HArray1OfInteger(1,2);
    anArrI->SetValue(1,aL);
    anArrI->SetValue(2,aR);
    Handle(TDataStd_IntegerArray) aDec = TDataStd_IntegerArray::Set(Label().FindChild(ChildLab_Dec), 1, 2);
    if(!aDec.IsNull())
      aDec->ChangeArray(anArrI);
  }

  if(theObject->GetModifiers().Length() > 0)
  {
    anArrI = new TColStd_HArray1OfInteger(1,theObject->GetModifiers().Length());
    for(Standard_Integer i = 1; i <= theObject->GetModifiers().Length(); i++)
      anArrI->SetValue(i,theObject->GetModifiers().Value(i));
    Handle(TDataStd_IntegerArray) aModifiers = TDataStd_IntegerArray::Set(Label().FindChild(ChildLab_Modifiers), 
                                               1, theObject->GetModifiers().Length());
    if(!aModifiers.IsNull())
      aModifiers->ChangeArray(anArrI);
  }

  if(!theObject->GetPath().IsNull())
  {
    TNaming_Builder tnBuild(Label().FindChild(ChildLab_Path));
    tnBuild.Generated(theObject->GetPath());
  }

  if(theObject->GetType() == XCAFDimTolObjects_DimensionType_Location_Oriented)
  {
    gp_Dir aD;
    theObject->GetDirection(aD);
    TDataXtd_Axis::Set(Label().FindChild(ChildLab_Dir), gp_Lin(gp_Pnt(), aD));
  }

  if (theObject->HasPoint())
  {
    TDataXtd_Point::Set(Label().FindChild(ChildLab_Pnt1), theObject->GetPoint());
  }

  if (theObject->HasPoint2())
  {
    TDataXtd_Point::Set(Label().FindChild(ChildLab_Pnt2), theObject->GetPoint2());
  }

  if (theObject->HasPlane())
  {
    TDataXtd_Plane::Set(Label().FindChild(ChildLab_Plane), gp_Pln(gp_Ax3(theObject->GetPlane())));
  }

  if (theObject->HasTextPoint())
  {
    TDataXtd_Point::Set(Label().FindChild(ChildLab_PntText), theObject->GetPointTextAttach());
  }

  TopoDS_Shape aPresentation = theObject->GetPresentation();
  if( !aPresentation.IsNull())
  {
    TDF_Label aLPres = Label().FindChild( ChildLab_Presentation);
    TNaming_Builder tnBuild(aLPres);
    tnBuild.Generated(aPresentation);
    Handle(TCollection_HAsciiString) aName =  theObject->GetPresentationName();
    if( !aName.IsNull() )
    {
      TCollection_ExtendedString str ( aName->String() );
      TDataStd_Name::Set ( aLPres, str );
    }
  }

  if (theObject->HasDescriptions())
  {
    Handle(TColStd_HArray1OfExtendedString) aDescrArr = new TColStd_HArray1OfExtendedString(1, theObject->NbDescriptions());
    Handle(TColStd_HArray1OfExtendedString) aDescrNameArr = new TColStd_HArray1OfExtendedString(1, theObject->NbDescriptions());
    for (Standard_Integer i = 0; i < theObject->NbDescriptions(); i++) {
      TCollection_ExtendedString aDescr(theObject->GetDescription(i)->String());
      aDescrArr->SetValue(i + 1, aDescr);
      TCollection_ExtendedString aDescrName(theObject->GetDescriptionName(i)->String());
      aDescrNameArr->SetValue(i + 1, aDescrName);
    }
    Handle(TDataStd_ExtStringArray) aDescriptions = TDataStd_ExtStringArray::Set(Label().FindChild(ChildLab_Descriptions),
                                                    1, theObject->NbDescriptions());
    Handle(TDataStd_ExtStringArray) aDescriptionNames = TDataStd_ExtStringArray::Set(Label().FindChild(ChildLab_DescriptionNames),
                                                        1, theObject->NbDescriptions());
    if(!aDescriptions.IsNull())
      aDescriptions->ChangeArray(aDescrArr);
    if(!aDescriptionNames.IsNull())
      aDescriptionNames->ChangeArray(aDescrNameArr);
  }
}

//=======================================================================
//function : GetObject
//purpose  : 
//=======================================================================
Handle(XCAFDimTolObjects_DimensionObject) XCAFDoc_Dimension::GetObject()  const
{
  Handle(XCAFDimTolObjects_DimensionObject) anObj = new XCAFDimTolObjects_DimensionObject();

  Handle(TDataStd_Integer) aType;
  if(Label().FindChild(ChildLab_Type).FindAttribute(TDataStd_Integer::GetID(), aType))
  {
    anObj->SetType((XCAFDimTolObjects_DimensionType)aType->Get());
  }

  Handle(TDataStd_RealArray) aVal;
  if(Label().FindChild(ChildLab_Value).FindAttribute(TDataStd_RealArray::GetID(), aVal) 
     && !aVal->Array().IsNull())
  {
    anObj->SetValues(aVal->Array());
  }

  Handle(TDataStd_Integer) aQualifier;
  if(Label().FindChild(ChildLab_Qualifier).FindAttribute(TDataStd_Integer::GetID(), aQualifier))
  {
    anObj->SetQualifier((XCAFDimTolObjects_DimensionQualifier)aQualifier->Get());
  }
 
  Handle(TDataStd_IntegerArray) aClass;
  if(Label().FindChild(ChildLab_Class).FindAttribute(TDataStd_IntegerArray::GetID(), aClass) 
     && !aClass->Array().IsNull() && aClass->Array()->Length() > 0)
  {
    anObj->SetClassOfTolerance(aClass->Array()->Value(1) != 0, (XCAFDimTolObjects_DimensionFormVariance)aClass->Array()->Value(2), (XCAFDimTolObjects_DimensionGrade)aClass->Array()->Value(3));
  }

  Handle(TDataStd_IntegerArray) aDec;
  if(Label().FindChild(ChildLab_Dec).FindAttribute(TDataStd_IntegerArray::GetID(), aDec)
     && !aDec->Array().IsNull() && aDec->Array()->Length() > 0)
  {
      anObj->SetNbOfDecimalPlaces(aDec->Array()->Value(1), aDec->Array()->Value(2));
  }
 
  Handle(TDataStd_IntegerArray) aModifiers;
  if(Label().FindChild(ChildLab_Modifiers).FindAttribute(TDataStd_IntegerArray::GetID(), aModifiers) 
     && !aModifiers->Array().IsNull())
  {
    XCAFDimTolObjects_DimensionModifiersSequence aM;
    for(Standard_Integer i = 1; i <= aModifiers->Array()->Length(); i++)
      aM.Append((XCAFDimTolObjects_DimensionModif)aModifiers->Array()->Value(i));
    anObj->SetModifiers(aM);
  }

  Handle(TNaming_NamedShape) aShape;
  if(Label().FindChild(ChildLab_Path).FindAttribute(TNaming_NamedShape::GetID(), aShape)
    && !aShape.IsNull())
  {
    anObj->SetPath(TopoDS::Edge(aShape->Get()));
  }

  Handle(TDataXtd_Axis) aDirAttr;
  if (Label().FindChild(ChildLab_Dir).FindAttribute(TDataXtd_Axis::GetID(), aDirAttr))
  {
    gp_Ax1 aDir;
    TDataXtd_Geometry::Axis(aDirAttr->Label(), aDir);
    anObj->SetDirection(aDir.Direction());
  }

  Handle(TDataXtd_Point) aPnt1Attr;
  if (Label().FindChild(ChildLab_Pnt1).FindAttribute(TDataXtd_Point::GetID(), aPnt1Attr))
  {
    gp_Pnt aPoint;
    TDataXtd_Geometry::Point(aPnt1Attr->Label(), aPoint);
    anObj->SetPoint(aPoint);
  }

  Handle(TDataXtd_Point) aPnt2Attr;
  if (Label().FindChild(ChildLab_Pnt2).FindAttribute(TDataXtd_Point::GetID(), aPnt2Attr))
  {
    gp_Pnt aPoint;
    TDataXtd_Geometry::Point(aPnt2Attr->Label(), aPoint);
    anObj->SetPoint2(aPoint);
  }


  Handle(TDataXtd_Plane) aPlaneAttr;
  if (Label().FindChild(ChildLab_Plane).FindAttribute(TDataXtd_Plane::GetID(), aPlaneAttr))
  {
    gp_Pln aPlane;
    TDataXtd_Geometry::Plane(aPlaneAttr->Label(), aPlane);
    gp_Ax2 anAx(aPlane.Location(), aPlane.Axis().Direction(), aPlane.XAxis().Direction());
    anObj->SetPlane(anAx);
  }

  Handle(TDataXtd_Point) aPntTextAttr;
  if (Label().FindChild(ChildLab_PntText).FindAttribute(TDataXtd_Point::GetID(), aPntTextAttr))
  {
    gp_Pnt aPoint;
    TDataXtd_Geometry::Point(aPntTextAttr->Label(), aPoint);
    anObj->SetPointTextAttach(aPoint);
  }

  Handle(TNaming_NamedShape) aNS;
  TDF_Label aLPres = Label().FindChild( ChildLab_Presentation);
  if ( aLPres.FindAttribute(TNaming_NamedShape::GetID(), aNS) ) 
  {

    TopoDS_Shape aPresentation = TNaming_Tool::GetShape(aNS);
    if( !aPresentation.IsNull())
    {
     
      Handle(TDataStd_Name) aNameAtrr;
      Handle(TCollection_HAsciiString) aPresentName;
      if (aLPres.FindAttribute(TDataStd_Name::GetID(),aNameAtrr))
      {
        const TCollection_ExtendedString& aName = aNameAtrr->Get();

        if( !aName.IsEmpty())
          aPresentName = new TCollection_HAsciiString(aName);
      }

      anObj->SetPresentation(aPresentation, aPresentName);
    }
  }

  Handle(TDataStd_ExtStringArray) aDescriptions, aDescriptionNames;
  if (Label().FindChild(ChildLab_Descriptions).FindAttribute(TDataStd_ExtStringArray::GetID(), aDescriptions) &&
    Label().FindChild(ChildLab_DescriptionNames).FindAttribute(TDataStd_ExtStringArray::GetID(), aDescriptionNames)) {
    for (Standard_Integer i = 1; i <= aDescriptions->Length(); i++) {
      Handle(TCollection_HAsciiString) aDescription, aDescriptionName;
      aDescription = new TCollection_HAsciiString(TCollection_AsciiString(aDescriptions->Value(i)));
      aDescriptionName = new TCollection_HAsciiString(TCollection_AsciiString(aDescriptionNames->Value(i)));
      anObj->AddDescription(aDescription, aDescriptionName);
    }
  }
  return anObj;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_Dimension::ID() const
{
  return GetID();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void XCAFDoc_Dimension::Restore(const Handle(TDF_Attribute)& /*With*/) 
{
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) XCAFDoc_Dimension::NewEmpty() const
{
  return new XCAFDoc_Dimension();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void XCAFDoc_Dimension::Paste(const Handle(TDF_Attribute)& /*Into*/,
                           const Handle(TDF_RelocationTable)& /*RT*/) const
{
}
