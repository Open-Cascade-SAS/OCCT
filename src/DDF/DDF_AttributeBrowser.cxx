// File:	DDF_AttributeBrowser.cxx
//      	------------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
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
