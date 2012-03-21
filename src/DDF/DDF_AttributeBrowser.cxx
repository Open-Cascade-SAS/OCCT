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

//      	------------------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Oct  6 1997	Creation



#include <DDF_AttributeBrowser.hxx>

static DDF_AttributeBrowser* DDF_FirstBrowser = NULL;

//=======================================================================
//function : DDF_AttributeBrowser
//purpose  : 
//=======================================================================

DDF_AttributeBrowser::DDF_AttributeBrowser
(Standard_Boolean (*test)(const Handle(TDF_Attribute)&),
 TCollection_AsciiString (*open)(const Handle(TDF_Attribute)&),
 TCollection_AsciiString (*text)(const Handle(TDF_Attribute)&))
: myTest(test),
  myOpen(open), 
  myText(text),
  myNext(DDF_FirstBrowser)
{
  DDF_FirstBrowser = this;
}


//=======================================================================
//function : Test
//purpose  : 
//=======================================================================

Standard_Boolean DDF_AttributeBrowser::Test
(const Handle(TDF_Attribute)&anAtt) const
{return (*myTest) (anAtt);}


//=======================================================================
//function : Open
//purpose  : 
//=======================================================================

TCollection_AsciiString DDF_AttributeBrowser::Open
(const Handle(TDF_Attribute)& anAtt) const
{ return (*myOpen) (anAtt);}


//=======================================================================
//function : Text
//purpose  : 
//=======================================================================

TCollection_AsciiString DDF_AttributeBrowser::Text
(const Handle(TDF_Attribute)& anAtt) const
{return (*myText) (anAtt);}


//=======================================================================
//function : FindBrowser
//purpose  : 
//=======================================================================

DDF_AttributeBrowser* DDF_AttributeBrowser::FindBrowser
(const Handle(TDF_Attribute)&anAtt)
{
  DDF_AttributeBrowser* browser = DDF_FirstBrowser;
  while (browser) {
    if (browser->Test(anAtt)) break;
    browser = browser->Next();
  }
  return browser;
}
