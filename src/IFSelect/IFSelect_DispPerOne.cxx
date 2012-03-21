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

#include <IFSelect_DispPerOne.ixx>
#include <IFSelect_Selection.hxx>
#include <IFGraph_SCRoots.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_EntityIterator.hxx>


//  Genere un paquet par racine (strong comp.) locale a la liste transmise


IFSelect_DispPerOne::IFSelect_DispPerOne ()    {  }

    TCollection_AsciiString  IFSelect_DispPerOne::Label () const
{  return TCollection_AsciiString ("One File per Input Entity");  }


    Standard_Boolean  IFSelect_DispPerOne::LimitedMax
  (const Standard_Integer nbent, Standard_Integer& pcount) const 
{  pcount = nbent;  return Standard_True;  }

    Standard_Boolean  IFSelect_DispPerOne::PacketsCount
  (const Interface_Graph& G, Standard_Integer& pcount) const 
{  pcount = G.Size();  return Standard_True;  }

    void  IFSelect_DispPerOne::Packets
  (const Interface_Graph& G, IFGraph_SubPartsIterator& packs) const 
{
  IFGraph_SCRoots packsc(G,Standard_False);  // OK pour SubPartsIterator
  packsc.SetLoad();
  packsc.GetFromIter(FinalSelection()->UniqueResult(G));
//   SCRoots a initie la resolution : decoupage en StrongComponants + selection
//   des Racines. Chaque Racine correspond a un Packet. CQFD
  packs.GetParts(packsc);
}
