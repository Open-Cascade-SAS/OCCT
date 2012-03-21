// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESDefs_MacroDef.ixx>
#include <Standard_DimensionMismatch.hxx>


IGESDefs_MacroDef::IGESDefs_MacroDef ()    {  }

    void  IGESDefs_MacroDef::Init
  (const Handle(TCollection_HAsciiString)& macro,
   const Standard_Integer entityTypeID,
   const Handle(Interface_HArray1OfHAsciiString)& langStatements,
   const Handle(TCollection_HAsciiString)& endMacro)
{
  if (langStatements->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESDefs_MacroDef : Init");
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
