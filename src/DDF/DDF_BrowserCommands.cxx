// File:	DDF_BrowserCommands.cxx
//      	-----------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Oct  3 1997	Creation

#include <stdio.h>

#include <DDF.hxx>
#include <DDF_Browser.hxx>
#include <DDF_Data.hxx>

#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Drawable3D.hxx>
#include <Draw_Interpretor.hxx>

#include <TDF_Label.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_Tool.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#ifdef WNT
#include <stdio.h>
#endif

//=======================================================================
//function : DFBrowse
//purpose  : 
//  arg 1  : DF name
// [arg 2] : Browser name
//=======================================================================

static Standard_Integer DFBrowse (Draw_Interpretor& di, 
				  Standard_Integer  n, 
				  const char**            a)
{
  if (n<2) return 1;
  
  Handle(TDF_Data) DF;
  if (!DDF::GetDF (a[1], DF)) return 1;

  Handle(DDF_Browser) NewDDFBrowser = new DDF_Browser(DF);
  char *name = new char[50];
  if (n == 3) sprintf(name,"browser_%s",a[2]);
  else        sprintf(name,"browser_%s",a[1]);

  Draw::Set (name, NewDDFBrowser);
  TCollection_AsciiString inst1("dftree ");
  inst1.AssignCat(name);
  di.Eval(inst1.ToCString());
  return 0;
}


//=======================================================================
//function : DFOpenLabel
//purpose  : 
//  arg 1  : Browser name
// [arg 2] : Label name
//=======================================================================

static Standard_Integer DFOpenLabel (Draw_Interpretor& di, 
				   Standard_Integer  n, 
				   const char**            a)
{
  if (n < 2) return 1;
  
  Handle(DDF_Browser) browser =
    Handle(DDF_Browser)::DownCast (Draw::Get(a[1], Standard_True)); 

  TDF_Label lab;
  if (n == 3) TDF_Tool::Label(browser->Data(),a[2],lab);

  if (lab.IsNull()) {
    TCollection_AsciiString list = browser->OpenRoot();
    di<<list.ToCString();
  }
  else {
    TCollection_AsciiString list = browser->OpenLabel(lab);
    di<<list.ToCString();
  }
  return 0;
}


//=======================================================================
//function : DFOpenAttributeList
//purpose  : 
//  arg 1  : Browser name
//  arg 2  : Label name
//=======================================================================

static Standard_Integer DFOpenAttributeList(Draw_Interpretor& di,
					    Standard_Integer n,
					    const char** a)
{
  if (n < 3) return 1;
  
  Handle(DDF_Browser) browser =
    Handle(DDF_Browser)::DownCast (Draw::Get(a[1], Standard_True)); 

  TDF_Label lab;
  TDF_Tool::Label(browser->Data(),a[2],lab);

  if (lab.IsNull()) {
    return 1;
  }

  TCollection_AsciiString list = browser->OpenAttributeList(lab);
  di << list.ToCString();
  return 0;
}



//=======================================================================
//function : DFOpenAttribute
//purpose  : 
//  arg 1  : Browser name
//  arg 2  : Attribute index
//=======================================================================

static Standard_Integer DFOpenAttribute (Draw_Interpretor& di, 
					 Standard_Integer  n, 
					 const char**      a)
{
  if (n < 3) return 1;
  
  Handle(DDF_Browser) browser =
    Handle(DDF_Browser)::DownCast (Draw::Get(a[1], Standard_True)); 

  Standard_Integer index = atoi(a[2]);

  TCollection_AsciiString list = browser->OpenAttribute(index);

  di<<list.ToCString();
  return 0;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++




//=======================================================================
//function : BrowserCommands
//purpose  : 
//=======================================================================

void DDF::BrowserCommands (Draw_Interpretor& theCommands) 
{
/*
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  const char* g = "DF browser commands";

  theCommands.Add
    ("DFBrowse",
     "Creates a browser on a df: DFBrowse dfname [browsername]",
     __FILE__, DFBrowse, g);

  theCommands.Add
    ("DFOpenLabel",
     "DON'T USE THIS COMMAND RESERVED TO THE BROWSER!\nReturns the list of sub-label entries: DFOpenLabel browsername [label]",
     __FILE__, DFOpenLabel, g);

  theCommands.Add
    ("DFOpenAttributeList",
     "DON'T USE THIS COMMAND RESERVED TO THE BROWSER!\nReturns the attribute list of a label: DFOpenLabel browsername label",
     __FILE__, DFOpenAttributeList, g);

  theCommands.Add
    ("DFOpenAttribute",
     "DON'T USE THIS COMMAND RESERVED TO THE BROWSER!\nReturns the reference list of an attribute: DFOpenLabel browsername attributeindex",
     __FILE__, DFOpenAttribute, g);
#if 0
  theCommands.Add
    ("DFDisplayInfo",
     "DON'T USE THIS COMMAND RESERVED TO THE BROWSER!\nReturns information about an attribute, a df or a label: DFDisplayInfo {#} | {browsername [label]}",
     __FILE__, DFDisplayInfo, g);
#endif
*/
}
