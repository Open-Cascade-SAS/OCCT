// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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
