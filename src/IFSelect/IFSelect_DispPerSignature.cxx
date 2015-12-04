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


#include <IFGraph_SubPartsIterator.hxx>
#include <IFSelect_DispPerSignature.hxx>
#include <IFSelect_Selection.hxx>
#include <IFSelect_SignCounter.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceError.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IFSelect_DispPerSignature,IFSelect_Dispatch)

IFSelect_DispPerSignature::IFSelect_DispPerSignature ()    {  }

    Handle(IFSelect_SignCounter)  IFSelect_DispPerSignature::SignCounter () const
{  return thesign;  }

    void  IFSelect_DispPerSignature::SetSignCounter
  (const Handle(IFSelect_SignCounter)& sign)
{  thesign = sign;  thesign->SetList (Standard_True);  }

    Standard_CString  IFSelect_DispPerSignature::SignName () const
{  return (Standard_CString ) (thesign.IsNull() ? "???" : thesign->Name());  }

    TCollection_AsciiString  IFSelect_DispPerSignature::Label () const
{
  char lab[50];
  sprintf (lab,"One File per Signature %s",SignName());
  return TCollection_AsciiString(lab);
}

    Standard_Boolean  IFSelect_DispPerSignature::LimitedMax
  (const Standard_Integer nbent, Standard_Integer& max) const
{
  max = nbent;
  return Standard_True;
}

    void  IFSelect_DispPerSignature::Packets
  (const Interface_Graph& G, IFGraph_SubPartsIterator& packs) const
{
  if (thesign.IsNull()) {
    packs.AddPart();
    packs.GetFromIter (FinalSelection()->RootResult(G));
    return;
  }

  thesign->Clear();
  thesign->AddList (FinalSelection()->RootResult(G).Content(),G.Model());
  Handle(TColStd_HSequenceOfHAsciiString) list = thesign->List();
  Standard_Integer i,nb,is,nbs = list->Length();
  Handle(TCollection_HAsciiString) asign;
  Handle(TColStd_HSequenceOfTransient) ents;
  for (is = 1; is <= nbs; is ++) {
    asign = list->Value(is);
    ents = thesign->Entities (asign->ToCString());
    if (ents.IsNull()) continue;
    packs.AddPart();
    nb = ents->Length();
    for (i = 1; i <= nb; i ++)
      packs.GetFromEntity (ents->Value(i),Standard_False);
  }
}
