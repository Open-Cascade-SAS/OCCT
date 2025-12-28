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
#include <TDF_Data.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DDocStd_DrawDocument, DDF_Data)

//=================================================================================================

occ::handle<DDocStd_DrawDocument> DDocStd_DrawDocument::Find(const occ::handle<TDocStd_Document>& /*Doc*/)
{
  occ::handle<DDocStd_DrawDocument> adoc;
  return adoc;
}

//=================================================================================================

DDocStd_DrawDocument::DDocStd_DrawDocument(const occ::handle<TDocStd_Document>& Doc)
    : DDF_Data(new TDF_Data), // Doc->GetData())
      myDocument(Doc)
{
  DataFramework(Doc->GetData());
}

//=================================================================================================

occ::handle<TDocStd_Document> DDocStd_DrawDocument::GetDocument() const
{
  return myDocument;
}

//=================================================================================================

void DDocStd_DrawDocument::DrawOn(Draw_Display& /*dis*/) const {}

//=================================================================================================

occ::handle<Draw_Drawable3D> DDocStd_DrawDocument::Copy() const
{
  occ::handle<DDocStd_DrawDocument> D = new DDocStd_DrawDocument(myDocument);
  return D;
}

//=================================================================================================

void DDocStd_DrawDocument::Dump(Standard_OStream& S) const
{
  occ::handle<TDocStd_Document> STDDOC = myDocument;
  if (!STDDOC.IsNull())
  {
    S << "TDocStd_Document\n";
    DDF_Data::Dump(S);
  }
  else
  {
    S << myDocument->DynamicType()->Name() << " is not a CAF document" << std::endl;
  }
}

//=================================================================================================

void DDocStd_DrawDocument::Whatis(Draw_Interpretor& I) const
{
  I << myDocument->DynamicType()->Name();
}
