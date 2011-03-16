//--------------------------------------------------------------------
//
//  File Name : IGESDefs_MacroDef.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
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
