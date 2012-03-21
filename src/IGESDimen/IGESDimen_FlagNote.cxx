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
  return (0.5 * (Height()/Tan((35./180.)*M_PI)));
}
