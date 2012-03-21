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


#include <DDocStd_DrawDocument.ixx>

#include <TDocStd_Document.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Data.hxx>
#include <TCollection_AsciiString.hxx>


//=======================================================================
//function : Find
//purpose  : 
//=======================================================================

Handle(DDocStd_DrawDocument) DDocStd_DrawDocument::Find (const Handle(TDocStd_Document)& Doc) 
{
Handle(DDocStd_DrawDocument)  adoc;
  return adoc;
}


//=======================================================================
//function : DDocStd_DrawDocument
//purpose  : 
//=======================================================================

DDocStd_DrawDocument::DDocStd_DrawDocument (const Handle(TDocStd_Document)& Doc) 
: DDF_Data(new TDF_Data), // Doc->GetData())
  myDocument(Doc)
{
  DataFramework(Doc->GetData());
}

//=======================================================================
//function : GetDocument
//purpose  : 
//=======================================================================

Handle(TDocStd_Document) DDocStd_DrawDocument::GetDocument() const
{
  return myDocument;
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DDocStd_DrawDocument::DrawOn(Draw_Display& dis) const
{
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Draw_Drawable3D) DDocStd_DrawDocument::Copy() const
{
  Handle(DDocStd_DrawDocument) D = new DDocStd_DrawDocument (myDocument);
  return D;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void DDocStd_DrawDocument::Dump (Standard_OStream& S) const
{
  Handle(TDocStd_Document) STDDOC =  Handle(TDocStd_Document)::DownCast(myDocument);
  if (!STDDOC.IsNull()) {
    S << "TDocStd_Document\n";
    DDF_Data::Dump(S);
  }
  else {  
    S << myDocument->DynamicType()->Name() << " is not a CAF document" << endl;
  }
}

//=======================================================================
//function : Whatis
//purpose  : 
//=======================================================================

void DDocStd_DrawDocument::Whatis(Draw_Interpretor& I) const
{ 
  I << myDocument->DynamicType()->Name();
}


