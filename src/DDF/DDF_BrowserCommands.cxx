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
#include <OSD_File.hxx>

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
                                  const char**      a)
{
  if (n<2)
  {
    cout << "Use: " << a[0] << " document [brower_name]" << endl;
    return 1;
  }
  
  Handle(TDF_Data) DF;
  if (!DDF::GetDF (a[1], DF)) 
  {
    cout << "Error: document " << a[1] << " is not found" << endl;
    return 1;
  }

  Handle(DDF_Browser) NewDDFBrowser = new DDF_Browser(DF);
  TCollection_AsciiString name("browser_");
  name += ((n == 3)? a[2] : a[1]);
  Draw::Set (name.ToCString(), NewDDFBrowser);

  // Load Tcl Script
  TCollection_AsciiString aTclScript (getenv ("CSF_DrawPluginDefaults"));
  aTclScript.AssignCat ( "/dftree.tcl" );
  OSD_File aTclScriptFile (aTclScript);
  if (aTclScriptFile.Exists()) {
#ifdef DEB
    cout << "Load " << aTclScript << endl;
#endif
    di.EvalFile( aTclScript.ToCString() );
  }
  else
  {
    cout << "Error: Could not load script " << aTclScript << endl;
    cout << "Check environment variable CSF_DrawPluginDefaults" << endl;
  }

  // Call command dftree defined in dftree.tcl
  TCollection_AsciiString aCommand = "dftree ";
  aCommand.AssignCat(name);
  di.Eval(aCommand.ToCString());
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
                                     const char**      a)
{
  if (n < 2) return 1;
  
  Handle(DDF_Browser) browser =
    Handle(DDF_Browser)::DownCast (Draw::Get(a[1], Standard_True)); 

  TDF_Label lab;
  if (n == 3) TDF_Tool::Label(browser->Data(),a[2],lab);

  TCollection_AsciiString list(lab.IsNull()? browser->OpenRoot() : browser->OpenLabel(lab));
  di<<list.ToCString();
  return 0;
}


//=======================================================================
//function : DFOpenAttributeList
//purpose  : 
//  arg 1  : Browser name
//  arg 2  : Label name
//=======================================================================

static Standard_Integer DFOpenAttributeList(Draw_Interpretor& di,
                                            Standard_Integer  n,
                                            const char**      a)
{
  if (n < 3) return 1;
  
  Handle(DDF_Browser) browser =
    Handle(DDF_Browser)::DownCast (Draw::Get(a[1], Standard_True)); 

  TDF_Label lab;
  TDF_Tool::Label(browser->Data(),a[2],lab);

  if (lab.IsNull())
    return 1;

  TCollection_AsciiString list(browser->OpenAttributeList(lab));
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

  const Standard_Integer index = Draw::Atoi(a[2]);
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
}
