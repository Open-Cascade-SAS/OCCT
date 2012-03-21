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
