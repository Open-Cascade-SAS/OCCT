// Created on: 2007-12-14
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2012 OPEN CASCADE SAS
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



#include <Poly_CoherentNode.hxx>
#include <Poly_CoherentTriangle.hxx>

#ifdef WNT
#pragma warning(disable:4996)
#endif

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void Poly_CoherentNode::Clear (const Handle_NCollection_BaseAllocator& theAlloc)
{
  Poly_CoherentTriPtr::RemoveList (myTriangles, theAlloc);
  myUV[0] = Precision::Infinite();
  myUV[1] = Precision::Infinite();
  myNormal[0] = 0.f;
  myNormal[1] = 0.f;
  myNormal[2] = 0.f;
  SetCoord(0., 0., 0.);
}

//=======================================================================
//function : SetNormal
//purpose  : Define the normal vector in the Node.
//=======================================================================

void Poly_CoherentNode::SetNormal (const gp_XYZ& theVector)
{
  myNormal[0] = static_cast<Standard_ShortReal>(theVector.X());
  myNormal[1] = static_cast<Standard_ShortReal>(theVector.Y());
  myNormal[2] = static_cast<Standard_ShortReal>(theVector.Z());
}

//=======================================================================
//function : AddTriangle
//purpose  : 
//=======================================================================

void Poly_CoherentNode::AddTriangle
                        (const Poly_CoherentTriangle&            theTri,
                         const Handle_NCollection_BaseAllocator& theAlloc)
{
  if (myTriangles == NULL)
    myTriangles = new (theAlloc) Poly_CoherentTriPtr(theTri);
  else
    myTriangles->Prepend(&theTri, theAlloc);
}

//=======================================================================
//function : RemoveTriangle
//purpose  : 
//=======================================================================

Standard_Boolean Poly_CoherentNode::RemoveTriangle
                        (const Poly_CoherentTriangle&            theTri,
                         const Handle_NCollection_BaseAllocator& theAlloc)
{
  Standard_Boolean aResult(Standard_False);
  if (&myTriangles->GetTriangle() == &theTri) {
    Poly_CoherentTriPtr * aLostPtr = myTriangles;
    if (myTriangles == &myTriangles->Next())
      myTriangles = 0L;
    else
      myTriangles = &myTriangles->Next();
    Poly_CoherentTriPtr::Remove(aLostPtr, theAlloc);
    aResult = Standard_True;
  } else {
    Poly_CoherentTriPtr::Iterator anIter(* myTriangles);
    for (anIter.Next(); anIter.More(); anIter.Next())
      if (&anIter.Value() == &theTri) {
        Poly_CoherentTriPtr::Remove
          (const_cast<Poly_CoherentTriPtr *>(&anIter.PtrValue()), theAlloc);
        aResult = Standard_True;
        break;
      }
  }
  return aResult;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Poly_CoherentNode::Dump(Standard_OStream& theStream) const
{
  char buf[256];
  sprintf (buf, "  X =%9.4f; Y =%9.4f; Z =%9.4f", X(), Y(), Z());
  theStream << buf << endl;
  Poly_CoherentTriPtr::Iterator anIter(* myTriangles);
  for (; anIter.More(); anIter.Next()) {
    const Poly_CoherentTriangle& aTri = anIter.Value();
    sprintf (buf, "      %5d %5d %5d", aTri.Node(0),aTri.Node(1),aTri.Node(2));
    theStream << buf << endl;
  }
}
