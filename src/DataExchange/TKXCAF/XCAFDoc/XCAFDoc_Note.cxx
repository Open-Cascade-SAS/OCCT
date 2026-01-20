// Copyright (c) 2017-2018 OPEN CASCADE SAS
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

#include <XCAFDoc_Note.hxx>

#include <gp_Pln.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDataXtd_Point.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Tool.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_GraphNode.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_Note, TDF_Attribute)

enum ChildLab
{
  ChildLab_PntText = 1,
  ChildLab_Plane,
  ChildLab_Pnt,
  ChildLab_Presentation
};

//=================================================================================================

bool XCAFDoc_Note::IsMine(const TDF_Label& theLabel)
{
  return !Get(theLabel).IsNull();
}

//=================================================================================================

XCAFDoc_Note::XCAFDoc_Note() {}

//=================================================================================================

occ::handle<XCAFDoc_Note> XCAFDoc_Note::Get(const TDF_Label& theLabel)
{
  occ::handle<XCAFDoc_Note> aNote;
  for (TDF_AttributeIterator anIt(theLabel); anIt.More(); anIt.Next())
  {
    aNote = occ::down_cast<XCAFDoc_Note>(anIt.Value());
    if (!aNote.IsNull())
      break;
  }
  return aNote;
}

//=================================================================================================

void XCAFDoc_Note::Set(const TCollection_ExtendedString& theUserName,
                       const TCollection_ExtendedString& theTimeStamp)
{
  Backup();

  myUserName  = theUserName;
  myTimeStamp = theTimeStamp;
}

//=================================================================================================

bool XCAFDoc_Note::IsOrphan() const
{
  occ::handle<XCAFDoc_GraphNode> aFather;
  return !Label().FindAttribute(XCAFDoc::NoteRefGUID(), aFather) || (aFather->NbChildren() == 0);
}

//=================================================================================================

occ::handle<XCAFNoteObjects_NoteObject> XCAFDoc_Note::GetObject() const
{
  occ::handle<XCAFNoteObjects_NoteObject> anObj = new XCAFNoteObjects_NoteObject();

  occ::handle<TDataXtd_Point> aPnt;
  if (Label().FindChild(ChildLab_Pnt).FindAttribute(TDataXtd_Point::GetID(), aPnt))
  {
    gp_Pnt aP;
    if (TDataXtd_Geometry::Point(aPnt->Label(), aP))
    {
      anObj->SetPoint(aP);
    }
  }

  occ::handle<TDataXtd_Plane> aPln;
  if (Label().FindChild(ChildLab_Plane).FindAttribute(TDataXtd_Plane::GetID(), aPln))
  {
    gp_Pln aP;
    if (TDataXtd_Geometry::Plane(aPln->Label(), aP))
    {
      anObj->SetPlane(aP.Position().Ax2());
    }
  }

  occ::handle<TDataXtd_Point> aPntText;
  if (Label().FindChild(ChildLab_PntText).FindAttribute(TDataXtd_Point::GetID(), aPntText))
  {
    gp_Pnt aP;
    if (TDataXtd_Geometry::Point(aPntText->Label(), aP))
    {
      anObj->SetPointText(aP);
    }
  }

  occ::handle<TNaming_NamedShape> aNS;
  TDF_Label                  aLPres = Label().FindChild(ChildLab_Presentation);
  if (aLPres.FindAttribute(TNaming_NamedShape::GetID(), aNS))
  {
    TopoDS_Shape aPresentation = TNaming_Tool::GetShape(aNS);
    if (!aPresentation.IsNull())
    {
      anObj->SetPresentation(aPresentation);
    }
  }

  return anObj;
}

//=================================================================================================

void XCAFDoc_Note::SetObject(const occ::handle<XCAFNoteObjects_NoteObject>& theObject)
{
  Backup();

  for (TDF_ChildIterator anIter(Label()); anIter.More(); anIter.Next())
  {
    anIter.Value().ForgetAllAttributes();
  }

  if (theObject->HasPoint())
  {
    gp_Pnt aPnt1 = theObject->GetPoint();
    TDataXtd_Point::Set(Label().FindChild(ChildLab_Pnt), aPnt1);
  }

  if (theObject->HasPlane())
  {
    gp_Ax2 anAx = theObject->GetPlane();

    gp_Pln aP(anAx);
    TDataXtd_Plane::Set(Label().FindChild(ChildLab_Plane), aP);
  }

  if (theObject->HasPointText())
  {
    gp_Pnt aPntText = theObject->GetPointText();
    TDataXtd_Point::Set(Label().FindChild(ChildLab_PntText), aPntText);
  }

  TopoDS_Shape aPresentation = theObject->GetPresentation();
  if (!aPresentation.IsNull())
  {
    TDF_Label       aLPres = Label().FindChild(ChildLab_Presentation);
    TNaming_Builder aBuilder(aLPres);
    aBuilder.Generated(aPresentation);
  }
}

//=================================================================================================

void XCAFDoc_Note::Restore(const occ::handle<TDF_Attribute>& theAttr)
{
  myUserName  = occ::down_cast<XCAFDoc_Note>(theAttr)->myUserName;
  myTimeStamp = occ::down_cast<XCAFDoc_Note>(theAttr)->myTimeStamp;
}

//=================================================================================================

void XCAFDoc_Note::Paste(const occ::handle<TDF_Attribute>& theAttrInto,
                         const occ::handle<TDF_RelocationTable>& /*theRT*/) const
{
  occ::down_cast<XCAFDoc_Note>(theAttrInto)->Set(myUserName, myTimeStamp);
}

//=================================================================================================

Standard_OStream& XCAFDoc_Note::Dump(Standard_OStream& theOS) const
{
  TDF_Attribute::Dump(theOS);
  theOS << "Note : " << (myUserName.IsEmpty() ? myUserName : "<anonymous>") << " on "
        << (myTimeStamp.IsEmpty() ? myTimeStamp : "<unknown>");
  return theOS;
}

//=================================================================================================

void XCAFDoc_Note::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myUserName)
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myTimeStamp)
}
