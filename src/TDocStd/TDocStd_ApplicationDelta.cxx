// File:      TDocStd_ApplicationDelta.cxx
// Created:   19.11.02 16:17:37
// Author:    Vladimir ANIKIN
// Copyright: Open CASCADE 2002

#include <TDocStd_ApplicationDelta.ixx>
#include <TCollection_AsciiString.hxx>
#include <TDocStd_Document.hxx>

//=======================================================================
//function : TDocStd_ApplicationDelta
//purpose  : 
//=======================================================================

TDocStd_ApplicationDelta::TDocStd_ApplicationDelta() {}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void TDocStd_ApplicationDelta::Dump(Standard_OStream& anOS) const {
  anOS<<"\t";
  myName.Print(anOS);
  anOS<<" - " << myDocuments.Length() << " documents ";
  anOS<<" ( ";
  Standard_Integer i;
  for (i = 1; i <= myDocuments.Length(); i++) {
    Handle(TDocStd_Document) aDocAddr= myDocuments.Value(i);
    anOS << "\"" << ((Standard_Transient*)aDocAddr);
    anOS << "\" ";
  }
  anOS << ") ";
}
