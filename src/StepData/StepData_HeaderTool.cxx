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


#include <Interface_Protocol.hxx>
#include <Interface_ReaderLib.hxx>
#include <StepData_FileProtocol.hxx>
#include <StepData_HeaderTool.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_StepReaderData.hxx>
#include <TCollection_AsciiString.hxx>

//  HeaderTool prend en charge le Schema de Donnees utilise pour un Fichier
//  Ce Schema peut etre compose de un ou plusieurs Protocoles, chacun etant
//  designe par une String. Les Strings correspondent au type "SCHEMA_NAME"
//  (typedef) et le Schema est une entite de Header de type "FILE_SCHEMA",
//  il a cette forme dans le fichier :
static Interface_ReaderLib lib;


    StepData_HeaderTool::StepData_HeaderTool
  (const Handle(StepData_StepReaderData)& data)
{
  lib.SetComplete();
  thedone = Standard_False;
  Standard_Integer num = 0;
  while ( (num = data->FindNextRecord(num)) != 0) {
    const TCollection_AsciiString& headertype = data->RecordType(num);
    if (headertype == "FILE_SCHEMA") {
      Standard_Integer numsub = data->SubListNumber(num,1,Standard_True);
      Standard_Integer nb = data->NbParams(numsub);
      for (Standard_Integer i = 1; i <= nb; i ++) {
	TCollection_AsciiString unom = data->ParamCValue(numsub,i);
	unom.Remove(unom.Length());
	unom.Remove(1);               // quotes debut et fin
	thenames.Append(unom);
      }
    }
  }
}


    StepData_HeaderTool::StepData_HeaderTool
  (const TColStd_SequenceOfAsciiString& names)
{
  lib.SetComplete();
  thedone = Standard_False;
  Standard_Integer nb = names.Length();
  for (Standard_Integer i = 1; i <= nb; i ++) thenames.Append(names.Value(i));
}

    Standard_Integer StepData_HeaderTool::NbSchemaNames () const
      {  return thenames.Length();  }

    const TCollection_AsciiString& StepData_HeaderTool::SchemaName
  (const Standard_Integer num) const
      {  return thenames.Value(num);  }

    Handle(StepData_Protocol) StepData_HeaderTool::NamedProtocol
  (const TCollection_AsciiString& name) const
{
  Handle(StepData_Protocol) proto;
  for (lib.Start(); lib.More(); lib.Next()) {
    proto = Handle(StepData_Protocol)::DownCast(lib.Protocol());
    if ( name.IsEqual(proto->SchemaName()) ) return proto;
  }
  return proto;
}


    void StepData_HeaderTool::Build
  (const Handle(StepData_FileProtocol)& proto)
{
  thedone = Standard_True;
  theignored.Clear();
  Standard_Integer nb = thenames.Length();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Handle(StepData_Protocol) unproto = NamedProtocol(thenames.Value(i));
    if (unproto.IsNull()) theignored.Append(thenames.Value(i));
    else proto->Add(unproto);
  }
}

    Handle(StepData_Protocol) StepData_HeaderTool::Protocol ()
{
  thedone = Standard_True;
  theignored.Clear();
  Handle(StepData_Protocol) unproto;
  if (thenames.IsEmpty()) return unproto;
  if (thenames.Length() == 1) {
    unproto = NamedProtocol (thenames.Value(1));
    if (unproto.IsNull()) theignored.Append (thenames.Value(1));
    return unproto;
  }
  Handle(StepData_FileProtocol) proto = new StepData_FileProtocol;
  Build(proto);
  return proto;
}


    Standard_Boolean StepData_HeaderTool::IsDone () const
      {  return thedone;  }


    Standard_Integer StepData_HeaderTool::NbIgnoreds () const
      {  return theignored.Length();  }

    const TCollection_AsciiString& StepData_HeaderTool::Ignored
  (const Standard_Integer num) const
      {  return theignored.Value(num);  }


    void StepData_HeaderTool::Print (Standard_OStream& S) const
{
  Standard_Integer nb = thenames.Length();
  Standard_Integer lng = 0;  Standard_Integer ln1;
  S << " ---  StepData_HeaderTool : List of Protocol Names  ---  Count : "
    << nb << endl;
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= nb; i ++) {
    ln1 = thenames.Value(i).Length() + 8;  lng += ln1;
    if (lng > 80) {  S << endl;  lng = ln1;  }
    S << "  " << i << " : " << thenames.Value(i);
  }
  if (lng == 0) S << endl;

  nb = theignored.Length();
  if (!thedone) {
    S << " ---   Evaluation of Protocol not Done   ---" << endl;
  } else if (nb == 0) {
    S << " ---   All Names correspond to a known Protocol  ---" << endl;
  } else {
    lng = ln1 = 0;
    S << " ---   Among them, " << nb << " remain unrecognized  ---" << endl;
    for (i = 1; i <= nb; i ++) {
      ln1 = theignored.Value(i).Length() + 3;  lng += ln1;
      if (lng > 80) {  S << endl;  lng = ln1;  }
      S << " : " << theignored.Value(i);
    }
    if (lng == 0) S << endl;
  }
}
