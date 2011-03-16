//--------------------------------------------------------------------
//
//  File Name : IGESDimen_FlagNote.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDimen_FlagNote.ixx>
#include <Standard_DimensionMismatch.hxx>
#include <gp_GTrsf.hxx>


    IGESDimen_FlagNote::IGESDimen_FlagNote ()    {  }

    void  IGESDimen_FlagNote::Init
  (const gp_XYZ& leftCorner, const Standard_Real anAngle,
   const Handle(IGESDimen_GeneralNote)& aNote,
   const Handle(IGESDimen_HArray1OfLeaderArrow)& someLeaders)
{
  if (!someLeaders.IsNull())
    if (someLeaders->Lower() != 1)
      Standard_DimensionMismatch::Raise("IGESDimen_FlagNote : Init");
  theLowerLeftcorner = leftCorner;
  theAngle           = anAngle;
  theNote            = aNote;
  theLeaders         = someLeaders;
  InitTypeAndForm(208,0);
}

    gp_Pnt  IGESDimen_FlagNote::LowerLeftCorner () const 
{
  gp_Pnt lowerleft(theLowerLeftcorner);
  return lowerleft;
}

    gp_Pnt  IGESDimen_FlagNote::TransformedLowerLeftCorner () const 
{
  gp_XYZ tempXYZ = theLowerLeftcorner;
  if (HasTransf()) Location().Transforms(tempXYZ);
  return gp_Pnt(tempXYZ);
}

    Standard_Real  IGESDimen_FlagNote::Angle () const 
{
  return theAngle;
}

    Handle(IGESDimen_GeneralNote)  IGESDimen_FlagNote::Note () const 
{
  return theNote;
}

    Standard_Integer  IGESDimen_FlagNote::NbLeaders () const 
{
  return (theLeaders.IsNull() ? 0 : theLeaders->Length() );
}

    Handle(IGESDimen_LeaderArrow)  IGESDimen_FlagNote::Leader
  (const Standard_Integer Index) const 
{
  return theLeaders->Value(Index);
}

    Standard_Real  IGESDimen_FlagNote::Height () const 
{
  return (2 * CharacterHeight());
}

    Standard_Real  IGESDimen_FlagNote::CharacterHeight () const 
{
  Standard_Real Max = theNote->BoxHeight(1);
  for(Standard_Integer i = 2; i <= theNote->NbStrings(); i++)
    {
      if (Max < theNote->BoxHeight(i))
	Max = theNote->BoxHeight(i);
    }
  return (Max);
}

    Standard_Real  IGESDimen_FlagNote::Length () const 
{
  return (TextWidth() + (0.4 * CharacterHeight()));
}

    Standard_Real  IGESDimen_FlagNote::TextWidth () const 
{
  Standard_Real width = 0;
  for(Standard_Integer i = 1; i <= theNote->NbStrings(); i++)
    width += theNote->BoxWidth(i);
  return (width);
}

    Standard_Real  IGESDimen_FlagNote::TipLength () const 
{
  return (0.5 * (Height()/Tan((35./180.)*PI)));
}
