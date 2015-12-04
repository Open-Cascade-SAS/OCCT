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


#include <IFSelect_SelectSignedShared.hxx>
#include <IFSelect_Signature.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SelectSignedShared,IFSelect_SelectExplore)

IFSelect_SelectSignedShared::IFSelect_SelectSignedShared
  (const Handle(IFSelect_Signature)& matcher,
   const Standard_CString signtext, const Standard_Boolean exact,
   const Standard_Integer level)
    : IFSelect_SelectExplore (level) ,
      thematcher (matcher) , thesigntext (signtext) , theexact (exact)    {  }
 
    Handle(IFSelect_Signature) IFSelect_SelectSignedShared::Signature () const
      {  return thematcher;  }
 
    const TCollection_AsciiString&  IFSelect_SelectSignedShared::SignatureText () const
      {  return thesigntext;  }

    Standard_Boolean  IFSelect_SelectSignedShared::IsExact () const
      {  return theexact;  }


    Standard_Boolean  IFSelect_SelectSignedShared::Explore
  (const Standard_Integer level, const Handle(Standard_Transient)& ent,
   const Interface_Graph& G,  Interface_EntityIterator& explored) const
{
  if (thematcher->Matches (ent,G.Model(),thesigntext,theexact)) return Standard_True;

//  sinon, on fait le tri ici
  Interface_EntityIterator list = G.Shareds (ent);
//  Si plus de Shared, alors c est cuit
  if (list.NbEntities() == 0) return Standard_False;

//  Sinon, trier si on est au niveau
  if (level < Level()) {  explored = list;  return Standard_True;  }
  for (list.Start(); list.More(); list.Next()) {
    if (thematcher->Matches (list.Value(),G.Model(),thesigntext,theexact))
      explored.AddItem (list.Value());
  }
  return Standard_True;
}


    TCollection_AsciiString IFSelect_SelectSignedShared::ExploreLabel () const
{
  TCollection_AsciiString lab("Shared,");
  lab.AssignCat (thematcher->Name());
  if (theexact) lab.AssignCat(" matching ");
  else          lab.AssignCat(" containing ");
  lab.AssignCat(thesigntext);
  return lab;
}
