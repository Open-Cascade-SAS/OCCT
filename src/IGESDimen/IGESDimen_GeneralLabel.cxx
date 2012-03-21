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
