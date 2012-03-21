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

#include <IFSelect_SelectSignedSharing.ixx>


IFSelect_SelectSignedSharing::IFSelect_SelectSignedSharing
  (const Handle(IFSelect_Signature)& matcher,
   const Standard_CString signtext, const Standard_Boolean exact,
   const Standard_Integer level)
    : IFSelect_SelectExplore (level) ,
      thematcher (matcher) , thesigntext (signtext) , theexact (exact)    {  }
 
    Handle(IFSelect_Signature) IFSelect_SelectSignedSharing::Signature () const
      {  return thematcher;  }
 
    const TCollection_AsciiString&  IFSelect_SelectSignedSharing::SignatureText () const
      {  return thesigntext;  }

    Standard_Boolean  IFSelect_SelectSignedSharing::IsExact () const
      {  return theexact;  }


    Standard_Boolean  IFSelect_SelectSignedSharing::Explore
  (const Standard_Integer level, const Handle(Standard_Transient)& ent,
   const Interface_Graph& G,  Interface_EntityIterator& explored) const
{
  if (thematcher->Matches (ent,G.Model(),thesigntext,theexact)) return Standard_True;

//  sinon, on fait le tri ici
  Interface_EntityIterator list = G.Sharings (ent);
//  Si plus de Sharing, alors c est cuit
  if (list.NbEntities() == 0) return Standard_False;

//  Sinon, trier si on est au niveau
  if (level < Level()) {  explored = list;  return Standard_True;  }
  for (list.Start(); list.More(); list.Next()) {
    if (thematcher->Matches (list.Value(),G.Model(),thesigntext,theexact))
      explored.AddItem (list.Value());
  }
  return Standard_True;
}


    TCollection_AsciiString IFSelect_SelectSignedSharing::ExploreLabel () const
{
  TCollection_AsciiString lab("Sharing,");
  lab.AssignCat (thematcher->Name());
  if (theexact) lab.AssignCat(" matching ");
  else          lab.AssignCat(" containing ");
  lab.AssignCat(thesigntext);
  return lab;
}
