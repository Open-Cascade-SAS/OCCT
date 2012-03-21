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

#include <IFSelect_DispPerSignature.ixx>
#include <IFSelect_Selection.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Interface_EntityIterator.hxx>
#include <stdio.h>


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
