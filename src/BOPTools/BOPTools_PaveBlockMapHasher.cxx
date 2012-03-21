// Created on: 2003-12-11
// Created by: Peter KURNEV
// Copyright (c) 2003-2012 OPEN CASCADE SAS
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



#include <BOPTools_PaveBlockMapHasher.ixx>

#include <BOPTools_Pave.hxx>
#include <BOPTools_PaveBlock.hxx>

//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================
Standard_Integer BOPTools_PaveBlockMapHasher::HashCode(const BOPTools_PaveBlock& aPB,
							 const Standard_Integer aUpper)
{
  Standard_Integer aS, nV1, nV2, nE;
  //
  nV1=aPB.Pave1().Index();
  nV2=aPB.Pave2().Index();
  nE=aPB.OriginalEdge();
  aS=nV1+nV2+nE;
  return aS%aUpper;
}

//=======================================================================
//function :IsEqual
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_PaveBlockMapHasher::IsEqual(const BOPTools_PaveBlock& aPB1,
							const BOPTools_PaveBlock& aPB2)
{
  return aPB1.IsEqual(aPB2);
}
