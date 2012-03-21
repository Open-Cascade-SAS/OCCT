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

#include <IGESSolid_EdgeList.ixx>


IGESSolid_EdgeList::IGESSolid_EdgeList ()    {  }


    void  IGESSolid_EdgeList::Init
  (const Handle(IGESData_HArray1OfIGESEntity)& Curves,
   const Handle(IGESSolid_HArray1OfVertexList)& startVertexList,
   const Handle(TColStd_HArray1OfInteger)& startVertexIndex,
   const Handle(IGESSolid_HArray1OfVertexList)& endVertexList,
   const Handle(TColStd_HArray1OfInteger)& endVertexIndex)
{
  Standard_Integer nb = (Curves.IsNull() ? 0 : Curves->Length());

  if (nb == 0 || Curves->Lower() != 1 || 
      startVertexList->Lower()  != 1 || startVertexList->Length()  != nb ||
      startVertexIndex->Lower() != 1 || startVertexIndex->Length() != nb ||
      endVertexList->Lower()    != 1 || endVertexList->Length()    != nb ||
      endVertexIndex->Lower()   != 1 || endVertexIndex->Length()   != nb  )
    Standard_DimensionError::Raise("IGESSolid_EdgeList : Init");

  theCurves = Curves;
  theStartVertexList  = startVertexList;
  theStartVertexIndex = startVertexIndex;
  theEndVertexList    = endVertexList;
  theEndVertexIndex   = endVertexIndex;

  InitTypeAndForm(504,1);
}

    Standard_Integer  IGESSolid_EdgeList::NbEdges () const
{
  return (theCurves.IsNull() ? 0 : theCurves->Length());
}

    Handle(IGESData_IGESEntity)  IGESSolid_EdgeList::Curve
  (const Standard_Integer num) const
{
  return theCurves->Value(num);
}

    Handle(IGESSolid_VertexList)  IGESSolid_EdgeList::StartVertexList
  (const Standard_Integer num) const
{
  return theStartVertexList->Value(num);
}

    Standard_Integer  IGESSolid_EdgeList::StartVertexIndex
  (const Standard_Integer num) const
{
  return theStartVertexIndex->Value(num);
}

    Handle(IGESSolid_VertexList)  IGESSolid_EdgeList::EndVertexList
  (const Standard_Integer num) const
{
  return theEndVertexList->Value(num);
}

    Standard_Integer  IGESSolid_EdgeList::EndVertexIndex
  (const Standard_Integer num) const
{
  return theEndVertexIndex->Value(num);
}
