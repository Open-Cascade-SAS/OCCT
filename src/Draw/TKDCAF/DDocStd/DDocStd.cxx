// Created on: 2000-03-01
// Created by: Denis PASCAL
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <DDocStd.hxx>

#include <DDocStd_DrawDocument.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Standard_GUID.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>

#include <StdLDrivers.hxx>
#include <BinLDrivers.hxx>
#include <XmlLDrivers.hxx>
#include <StdDrivers.hxx>
#include <BinDrivers.hxx>
#include <XmlDrivers.hxx>

//=================================================================================================

const occ::handle<TDocStd_Application>& DDocStd::GetApplication()
{
  static occ::handle<TDocStd_Application> anApp;
  if (anApp.IsNull())
  {
    anApp = new TDocStd_Application;

    // Initialize standard document formats at creation - they should
    // be available even if this DRAW plugin is not loaded by pload command
    StdLDrivers::DefineFormat(anApp);
    BinLDrivers::DefineFormat(anApp);
    XmlLDrivers::DefineFormat(anApp);
    StdDrivers::DefineFormat(anApp);
    BinDrivers::DefineFormat(anApp);
    XmlDrivers::DefineFormat(anApp);
  }
  return anApp;
}

//=================================================================================================

bool DDocStd::GetDocument(const char*&                   Name,
                          occ::handle<TDocStd_Document>& DOC,
                          const bool                     Complain)
{
  occ::handle<DDocStd_DrawDocument> DD =
    occ::down_cast<DDocStd_DrawDocument>(Draw::GetExisting(Name));
  if (DD.IsNull())
  {
    if (Complain)
      std::cout << Name << " is not a Document" << std::endl;
    return false;
  }
  occ::handle<TDocStd_Document> STDDOC = DD->GetDocument();
  if (!STDDOC.IsNull())
  {
    DOC = STDDOC;
    return true;
  }
  if (Complain)
    std::cout << Name << " is not a CAF Document" << std::endl;
  return false;
}

//=======================================================================
// function : Label
// purpose  : try to retrieve a label
//=======================================================================

bool DDocStd::Find(const occ::handle<TDocStd_Document>& D,
                   const char* const                    Entry,
                   TDF_Label&                           Label,
                   const bool                           Complain)
{
  Label.Nullify();
  TDF_Tool::Label(D->GetData(), Entry, Label, false);
  if (Label.IsNull() && Complain)
    std::cout << "No label for entry " << Entry << std::endl;
  return !Label.IsNull();
}

//=======================================================================
// function : Find
// purpose  : Try to retrieve an attribute.
//=======================================================================

bool DDocStd::Find(const occ::handle<TDocStd_Document>& D,
                   const char* const                    Entry,
                   const Standard_GUID&                 ID,
                   occ::handle<TDF_Attribute>&          A,
                   const bool                           Complain)
{
  TDF_Label L;
  if (Find(D, Entry, L, Complain))
  {
    if (L.FindAttribute(ID, A))
      return true;
    if (Complain)
      std::cout << "attribute not found for entry : " << Entry << std::endl;
  }
  return false;
}

//=================================================================================================

Draw_Interpretor& DDocStd::ReturnLabel(Draw_Interpretor& di, const TDF_Label& L)
{
  TCollection_AsciiString S;
  TDF_Tool::Entry(L, S);
  di << S.ToCString();
  return di;
}

//=================================================================================================

void DDocStd::AllCommands(Draw_Interpretor& theCommands)
{
  static bool done = false;
  if (done)
    return;
  done = true;

  // define commands
  DDocStd::ApplicationCommands(theCommands);
  DDocStd::DocumentCommands(theCommands);
  DDocStd::ToolsCommands(theCommands);
  DDocStd::MTMCommands(theCommands);
  DDocStd::ShapeSchemaCommands(theCommands);
}
