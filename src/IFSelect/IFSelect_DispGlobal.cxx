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

#include <IFSelect_DispGlobal.ixx>
#include <IFSelect_Selection.hxx>
#include <Interface_EntityIterator.hxx>

// Genere un seul paquet avec la sortie finale


IFSelect_DispGlobal::IFSelect_DispGlobal ()    {  }

    TCollection_AsciiString  IFSelect_DispGlobal::Label () const
{  return TCollection_AsciiString ("One File for All Input");  }

    Standard_Boolean  IFSelect_DispGlobal::LimitedMax
  (const Standard_Integer /* nbent */, Standard_Integer& pcount) const 
      {  pcount = 1;  return Standard_True;  }

    Standard_Boolean  IFSelect_DispGlobal::PacketsCount
  (const Interface_Graph& /* G */, Standard_Integer& pcount) const 
      {  pcount = 1;  return Standard_True;  }

// 1 packet ( a partir de UniqueResult)
      void IFSelect_DispGlobal::Packets
  (const Interface_Graph& G, IFGraph_SubPartsIterator& packs) const 
{
  packs.AddPart();
  packs.GetFromIter(FinalSelection()->UniqueResult(G));
}
