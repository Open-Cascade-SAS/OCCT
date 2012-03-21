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

#include <IFGraph_AllShared.ixx>
#include <Interface_InterfaceModel.hxx>


IFGraph_AllShared::IFGraph_AllShared (const Interface_Graph& agraph)
      : thegraph (agraph)    {  }


    IFGraph_AllShared::IFGraph_AllShared
  (const Interface_Graph& agraph, const Handle(Standard_Transient)& ent)
      : thegraph (agraph)
{
  if (!agraph.Model()->Contains(ent)) return;
  GetFromEntity(ent);
}

    void  IFGraph_AllShared::GetFromEntity
  (const Handle(Standard_Transient)& ent)
      {  thegraph.GetFromEntity(ent,Standard_True);  }  // le fait pour nous

     void IFGraph_AllShared::GetFromIter (const Interface_EntityIterator& iter)
{
  for (iter.Start(); iter.More(); iter.Next())
    thegraph.GetFromEntity(iter.Value(),Standard_True);
}

     void IFGraph_AllShared::ResetData ()
      {  Reset();  thegraph.Reset();  }

     void IFGraph_AllShared::Evaluate()
      {  Reset();  GetFromGraph(thegraph);  }
