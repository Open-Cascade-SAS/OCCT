//--------------------------------------------------------------------
//
//  File Name : IGESDimen_GeneralLabel.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDimen_GeneralLabel.ixx>
#include <Standard_DimensionMismatch.hxx>


    IGESDimen_GeneralLabel::IGESDimen_GeneralLabel ()    {  }

    void  IGESDimen_GeneralLabel::Init
  (const Handle(IGESDimen_GeneralNote)& aNote,
   const Handle(IGESDimen_HArray1OfLeaderArrow)& someLeaders)
{
  if (someLeaders->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESDimen_GeneralLabel : Init");
  theNote    = aNote;
  theLeaders = someLeaders;
  InitTypeAndForm(210,0);
}

    Handle(IGESDimen_GeneralNote)  IGESDimen_GeneralLabel::Note () const 
{
  return theNote;
}

    Standard_Integer  IGESDimen_GeneralLabel::NbLeaders () const 
{
  return theLeaders->Length();
}

    Handle(IGESDimen_LeaderArrow)  IGESDimen_GeneralLabel::Leader
  (const Standard_Integer Index) const 
{
  return theLeaders->Value(Index);
}
