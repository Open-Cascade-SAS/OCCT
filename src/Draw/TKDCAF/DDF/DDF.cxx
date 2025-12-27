// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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

//      	-------
// Version:	0.0
// Version	Date		Purpose
//		0.0	Feb 10 1997	Creation

#include <DDF.hxx>
#include <DDF_Data.hxx>
#include <Draw.hxx>
#include <Standard_GUID.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>

//=================================================================================================

bool DDF::AddLabel

  (const occ::handle<TDF_Data>& DF, const char* Entry, TDF_Label& Label)
{
  TDF_Tool::Label(DF, Entry, Label, true);
  return true;
}

//=================================================================================================

bool DDF::FindLabel(const occ::handle<TDF_Data>& DF,
                                const char*  Entry,
                                TDF_Label&              Label,
                                const bool  Complain)
{
  Label.Nullify();
  TDF_Tool::Label(DF, Entry, Label, false);
  if (Label.IsNull() && Complain)
    std::cout << "No label for entry " << Entry << std::endl;
  return !Label.IsNull();
}

//=================================================================================================

bool DDF::GetDF(const char*&      Name,
                            occ::handle<TDF_Data>&      DF,
                            const bool Complain)
{
  occ::handle<Standard_Transient> t   = Draw::Get(Name);
  occ::handle<DDF_Data>           DDF = occ::down_cast<DDF_Data>(t);
  // occ::handle<DDF_Data> DDF = Handle(DDF_Data)::DownCast (Draw::Get(Name, Complain));
  if (!DDF.IsNull())
  {
    DF = DDF->DataFramework();
    return true;
  }
  if (Complain)
    std::cout << "framework " << Name << " not found " << std::endl;
  return false;
}

//=================================================================================================

bool DDF::Find(const occ::handle<TDF_Data>& DF,
                           const char*  Entry,
                           const Standard_GUID&    ID,
                           occ::handle<TDF_Attribute>&  A,
                           const bool  Complain)
{
  TDF_Label L;
  if (FindLabel(DF, Entry, L, Complain))
  {
    if (L.FindAttribute(ID, A))
      return true;
    if (Complain)
      std::cout << "attribute not found for entry : " << Entry << std::endl;
  }
  return false;
}

//=================================================================================================

Draw_Interpretor& DDF::ReturnLabel(Draw_Interpretor& di, const TDF_Label& L)
{
  TCollection_AsciiString S;
  TDF_Tool::Entry(L, S);
  di << S.ToCString();
  return di;
}

//=================================================================================================

void DDF::AllCommands(Draw_Interpretor& theCommands)
{
  static bool done = false;
  if (done)
    return;
  done = true;

  DDF::BasicCommands(theCommands);
  DDF::DataCommands(theCommands);
  DDF::TransactionCommands(theCommands);
  DDF::BrowserCommands(theCommands);
  // define the TCL variable DDF
  const char* com = "set DDF";
  theCommands.Eval(com);
}
