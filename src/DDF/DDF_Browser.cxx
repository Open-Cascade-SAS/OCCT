// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

//      	---------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Oct  3 1997	Creation


#include <DDF_Browser.ixx>

#include <DDF_AttributeBrowser.hxx>

#include <TDF_Tool.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_Attribute.hxx>
#include <TDF.hxx>

#include <TDataStd_Name.hxx>

#include <TCollection_ExtendedString.hxx>

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Communication convention with tcl:
// tcl waits for a string of characters, being an information list.
// In this list, each item is separated from another by a separator: '\'.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define TDF_BrowserSeparator1 '\\'
#define TDF_BrowserSeparator2 ' '
#define TDF_BrowserSeparator3 '#'
#define TDF_BrowserSeparator4 ','


//=======================================================================
//function : DDF_Browser
//purpose  : 
//=======================================================================

DDF_Browser::DDF_Browser(const Handle(TDF_Data)& aDF)
: myDF(aDF)
{}


//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DDF_Browser::DrawOn(Draw_Display& /*dis*/) const
{ 
  //cout<<"DDF_Browser"<<endl; 
}


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D) DDF_Browser::Copy() const
{ return new DDF_Browser(myDF); }


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void DDF_Browser::Dump(Standard_OStream& S) const
{
  S<<"DDF_Browser on a DF:"<<endl;
  S<<myDF;
}


//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void DDF_Browser::Whatis(Draw_Interpretor& I) const
{ I<<"Data Framework Browser"; }


//=======================================================================
//function : Data
//purpose  : 
//=======================================================================

void DDF_Browser::Data(const Handle(TDF_Data)& aDF) 
{ myDF = aDF; }


//=======================================================================
//function : Data
//purpose  : 
//=======================================================================

Handle(TDF_Data) DDF_Browser::Data() const
{ return myDF; }


//=======================================================================
//function : OpenRoot
//purpose  : 
//=======================================================================

TCollection_AsciiString DDF_Browser::OpenRoot() const
{
  TCollection_AsciiString list;
  const TDF_Label& root = myDF->Root();
  TDF_Tool::Entry(root,list);
  Handle(TDataStd_Name) name;
  list.AssignCat(TDF_BrowserSeparator2);
  list.AssignCat("\"");
  if (root.FindAttribute(TDataStd_Name::GetID(),name))
  {
    TCollection_AsciiString tmpStr(name->Get(),'?');
    tmpStr.ChangeAll(' ','_');
    list.AssignCat(tmpStr);
  }
  list.AssignCat("\"");
  list.AssignCat(TDF_BrowserSeparator2);
  if (!root.MayBeModified()) list.AssignCat("Not");
  list.AssignCat("Modified");
  list.AssignCat(TDF_BrowserSeparator2);
  list.AssignCat((root.HasAttribute() || root.HasChild())? "1" : "0");
  return list;
}


//=======================================================================
//function : OpenLabel
//purpose  : 
// an item is composed as follows:
// "Entry "Name" Modified|NotModified 0|1"
// the end bit shows if the label has attributes or children.
// The 1st can be
// "AttributeList Modified|NotModified"
// The items are separated by "\\".
//=======================================================================

TCollection_AsciiString DDF_Browser::OpenLabel(const TDF_Label& aLab) const
{
  Standard_Boolean split = Standard_False;
  TCollection_AsciiString entry, list;
  if (aLab.HasAttribute() || aLab.AttributesModified())
  {
    list.AssignCat("AttributeList");
    list.AssignCat(TDF_BrowserSeparator2);
    if (!aLab.AttributesModified()) list.AssignCat("Not");
    list.AssignCat("Modified");
    split = Standard_True;
  }
  Handle(TDataStd_Name) name;
  for (TDF_ChildIterator itr(aLab); itr.More(); itr.Next())
  {
    if (split) list.AssignCat(TDF_BrowserSeparator1);
    TDF_Tool::Entry(itr.Value(),entry);
    list.AssignCat(entry);
    list.AssignCat(TDF_BrowserSeparator2);
    list.AssignCat("\"");
    if (itr.Value().FindAttribute(TDataStd_Name::GetID(),name))
    {
      TCollection_AsciiString tmpStr(name->Get(),'?');
      tmpStr.ChangeAll(' ','_');
      list.AssignCat(tmpStr);
    }
    list.AssignCat("\"");
    list.AssignCat(TDF_BrowserSeparator2);
    if (!itr.Value().MayBeModified()) list.AssignCat("Not");
    list.AssignCat("Modified");
    list.AssignCat(TDF_BrowserSeparator2);
    // May be open.
    list.AssignCat((itr.Value().HasAttribute() || itr.Value().HasChild())? "1" : "0");
    split = Standard_True;
  }
  return list;
}


//=======================================================================
//function : OpenAttributeList
//purpose  : 
// an item is composed as follows:
// "DynamicType#MapIndex TransactionIndex Valid|Notvalid Forgotten|NotForgotten Backuped|NotBackuped"
// The items are separated by "\\".
//=======================================================================

TCollection_AsciiString DDF_Browser::OpenAttributeList
  (const TDF_Label& aLab) 
{
  TCollection_AsciiString list;
  Standard_Boolean split1 = Standard_False;
  for (TDF_AttributeIterator itr(aLab,Standard_False);itr.More();itr.Next())
  {
    if (split1) list.AssignCat(TDF_BrowserSeparator1);
    const Handle(TDF_Attribute)& att = itr.Value();
    const Standard_Integer index = myAttMap.Add(att);
    TCollection_AsciiString indexStr(index);
    list.AssignCat(att->DynamicType()->Name());
    list.AssignCat(TDF_BrowserSeparator3);
    list.AssignCat(indexStr);
    list.AssignCat(TDF_BrowserSeparator2);
    list.AssignCat(att->Transaction());
    // Valid.
    list.AssignCat(TDF_BrowserSeparator2);
    if (!att->IsValid()) list.AssignCat("Not");
    list.AssignCat("Valid");
    // Forgotten.
    list.AssignCat(TDF_BrowserSeparator2);
    if (!att->IsForgotten()) list.AssignCat("Not");
    list.AssignCat("Forgotten");
    // Backuped.
    list.AssignCat(TDF_BrowserSeparator2);
    if (!att->IsBackuped()) list.AssignCat("Not");
    list.AssignCat("Backuped");
    // May be open.
    list.AssignCat(TDF_BrowserSeparator2);
    DDF_AttributeBrowser* br = DDF_AttributeBrowser::FindBrowser(att);
    list.AssignCat(br? "1" : "0");
    split1 = Standard_True;
  }
  return list;
}


//=======================================================================
//function : OpenAttribute
//purpose  : Attribute's intrinsic information given by an attribute browser.
//=======================================================================

TCollection_AsciiString DDF_Browser::OpenAttribute
  (const Standard_Integer anIndex) 
{
  TCollection_AsciiString list;
  Handle(TDF_Attribute) att = myAttMap.FindKey(anIndex);
  DDF_AttributeBrowser* br = DDF_AttributeBrowser::FindBrowser(att);
  if (br) list = br->Open(att);
  return list;
}


//=======================================================================
//function : Information
//purpose  : Information about <myDF>.
//=======================================================================

TCollection_AsciiString DDF_Browser::Information() const
{
  TCollection_AsciiString list;
  return list;
}


//=======================================================================
//function : Information
//purpose  : Information about a label.
//=======================================================================

TCollection_AsciiString DDF_Browser::Information(const TDF_Label& /*aLab*/) const
{
  TCollection_AsciiString list;
  return list;
}


//=======================================================================
//function : Information
//purpose  : Information about an attribute.
//=======================================================================

TCollection_AsciiString DDF_Browser::Information(const Standard_Integer /*anIndex*/) const
{
  TCollection_AsciiString list;
  return list;
}
