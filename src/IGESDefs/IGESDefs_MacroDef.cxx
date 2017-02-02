// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESDefs_MacroDef.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDefs_MacroDef,IGESData_IGESEntity)

IGESDefs_MacroDef::IGESDefs_MacroDef ()    {  }

    void  IGESDefs_MacroDef::Init
  (const Handle(TCollection_HAsciiString)& macro,
   const Standard_Integer entityTypeID,
   const Handle(Interface_HArray1OfHAsciiString)& langStatements,
   const Handle(TCollection_HAsciiString)& endMacro)
{
  if (langStatements->Lower() != 1)
    throw Standard_DimensionMismatch("IGESDefs_MacroDef : Init");
  theMACRO          = macro;
  theEntityTypeID   = entityTypeID;
  theLangStatements = langStatements;
  theENDMACRO       = endMacro;
  InitTypeAndForm(306,0);
}

    Standard_Integer  IGESDefs_MacroDef::NbStatements () const
{
  return theLangStatements->Length();
}

    Handle(TCollection_HAsciiString)  IGESDefs_MacroDef::MACRO () const 
{
  return theMACRO;
}

    Standard_Integer  IGESDefs_MacroDef::EntityTypeID () const 
{
  return theEntityTypeID;
}

    Handle(TCollection_HAsciiString)  IGESDefs_MacroDef::LanguageStatement
  (const Standard_Integer StatNum) const 
{
  return theLangStatements->Value(StatNum);
}

    Handle(TCollection_HAsciiString)  IGESDefs_MacroDef::ENDMACRO () const 
{
  return theENDMACRO;
}
