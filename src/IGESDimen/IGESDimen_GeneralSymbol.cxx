//--------------------------------------------------------------------
//
//  File Name : IGESDimen_GeneralSymbol.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDimen_GeneralSymbol.ixx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>


    IGESDimen_GeneralSymbol::IGESDimen_GeneralSymbol ()    {  }

    void  IGESDimen_GeneralSymbol::Init 
  (const Handle(IGESDimen_GeneralNote)& aNote,
   const Handle(IGESData_HArray1OfIGESEntity)& allGeoms,
   const Handle(IGESDimen_HArray1OfLeaderArrow)& allLeaders)
{
  if (!allGeoms.IsNull() &&  allGeoms->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESDimen_GeneralSymbol : Init");
  if (!allLeaders.IsNull())
    if (allLeaders->Lower() != 1) Standard_DimensionMismatch::Raise("$");
  theNote    = aNote;
  theGeoms   = allGeoms;
  theLeaders = allLeaders;
  InitTypeAndForm(228,FormNumber());
//  FormNumber precises the Nature of the Symbol, cf G.14 (0-3 or > 5000)
}

    void  IGESDimen_GeneralSymbol::SetFormNumber (const Standard_Integer form)
{
  if ((form < 0 || form > 3) && form < 5000) Standard_OutOfRange::Raise
    ("IGESDimen_GeneralSymbol : SetFormNumber");
  InitTypeAndForm(228,form);
}


    Standard_Boolean  IGESDimen_GeneralSymbol::HasNote () const
{
  return (!theNote.IsNull());
}

    Handle(IGESDimen_GeneralNote)  IGESDimen_GeneralSymbol::Note () const
{
  return theNote;
}

    Standard_Integer  IGESDimen_GeneralSymbol::NbGeomEntities () const
{
  return theGeoms->Length();
}

    Handle(IGESData_IGESEntity)  IGESDimen_GeneralSymbol::GeomEntity
  (const Standard_Integer Index) const
{
  return theGeoms->Value(Index);
}

    Standard_Integer  IGESDimen_GeneralSymbol::NbLeaders () const
{
  return (theLeaders.IsNull() ? 0 : theLeaders->Length());
}

    Handle(IGESDimen_LeaderArrow)  IGESDimen_GeneralSymbol::LeaderArrow
  (const Standard_Integer Index) const
{
  return theLeaders->Value(Index);
}
