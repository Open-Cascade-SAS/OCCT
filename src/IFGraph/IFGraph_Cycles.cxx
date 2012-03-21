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

#include <IFGraph_Cycles.ixx>
#include <Interface_GraphContent.hxx>
#include <IFGraph_StrongComponants.hxx>



//  Cycles utilise les services de StrongComponants :
//  Il retient les Strong Componants qui ne sont pas Single


IFGraph_Cycles::IFGraph_Cycles
  (const Interface_Graph& agraph, const Standard_Boolean whole)
      :  IFGraph_SubPartsIterator (agraph,whole)    {  }

    IFGraph_Cycles::IFGraph_Cycles (IFGraph_StrongComponants& subparts)
      :  IFGraph_SubPartsIterator (subparts)    {  }


    void  IFGraph_Cycles::Evaluate ()
{
  IFGraph_StrongComponants complist(Model(),Standard_False);
  complist.GetFromIter(Loaded());
  for (complist.Start(); complist.More(); complist.Next()) {
    if (complist.IsSingle()) continue;
    AddPart();
    GetFromIter(complist.Entities());
  }
}
