// File:	BOPTools_PaveBlockMapHasher.cxx
// Created:	Thu Dec 11 14:10:44 2003
// Author:	Peter KURNEV
//		<pkv@irinox>


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
