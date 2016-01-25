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


#include <DDocStd_DrawDocument.hxx>
#include <Draw_Display.hxx>
#include <Draw_Drawable3D.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Data.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DDocStd_DrawDocument,DDF_Data)

//=======================================================================
//function : Find
//purpose  : 
//=======================================================================
Handle(DDocStd_DrawDocument) DDocStd_DrawDocument::Find (const Handle(TDocStd_Document)& /*Doc*/) 
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

void DDocStd_DrawDocument::DrawOn(Draw_Display& /*dis*/) const
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
  Handle(TDocStd_Document) STDDOC = myDocument;
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


