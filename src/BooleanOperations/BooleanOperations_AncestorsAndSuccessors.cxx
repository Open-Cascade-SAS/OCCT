// Created on: 2000-07-10
// Created by: Vincent DELOS
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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



#include <BooleanOperations_AncestorsAndSuccessors.ixx>
#include <BooleanOperations_AncestorsSeqAndSuccessorsSeq.hxx>

//===========================================================================
//function : BooleanOperations_AncestorsAndSuccessors
//purpose  : default creator
//===========================================================================
BooleanOperations_AncestorsAndSuccessors::BooleanOperations_AncestorsAndSuccessors():
 myAncestors(0L),
 mySuccessors(0L),
 myOrientations(0L),
 myAncestorsSize(0),
 mySuccessorsSize(0)
{
}


//===========================================================================
//function : BooleanOperations_AncestorsAndSuccessors
//purpose  : 
//===========================================================================
  BooleanOperations_AncestorsAndSuccessors::BooleanOperations_AncestorsAndSuccessors
                          (const BooleanOperations_AncestorsSeqAndSuccessorsSeq& AncSuc, 
			   const Standard_Integer shift) :
  
  myAncestors(0L),
  mySuccessors(0L),
  myOrientations(0L),
  myAncestorsSize(0),
  mySuccessorsSize(0)
{
  Standard_Integer i, j;
  //
  // Ancestors
  //
  myAncestorsSize  = AncSuc.NumberOfAncestors();
  if (myAncestorsSize) {
    myAncestors =      (Standard_Integer*)Standard::Allocate(myAncestorsSize*sizeof(Standard_Integer));

    for (i=1; i<=myAncestorsSize; i++) {
      ((Standard_Integer*)myAncestors)[i-1] = AncSuc.GetAncestor(i) + shift;
    }
  }
  //
  // Successors
  //
  mySuccessorsSize = AncSuc.NumberOfSuccessors();
  if (mySuccessorsSize) {
    mySuccessors   = (Standard_Integer*)  Standard::Allocate(mySuccessorsSize*sizeof(Standard_Integer));
    myOrientations = (TopAbs_Orientation*)Standard::Allocate(mySuccessorsSize*sizeof(TopAbs_Orientation));
    
    for (i=1; i<=mySuccessorsSize; i++)    {
      j=i-1;
      ((Standard_Integer*)mySuccessors    )[j] = AncSuc.GetSuccessor(i) + shift;
      ((TopAbs_Orientation*)myOrientations)[j] = AncSuc.GetOrientation(i);
    }
  }
}


//===========================================================================
//function : Destroy
//purpose  : destructor
//===========================================================================
  void BooleanOperations_AncestorsAndSuccessors::Destroy()
{
  if (myAncestors) {
    Standard::Free((Standard_Address&)myAncestors);
  }
  if (mySuccessors) {
    Standard::Free((Standard_Address&)mySuccessors);
  }
  if (myOrientations) {
    Standard::Free((Standard_Address&)myOrientations);
  }
}


//===========================================================================
//function : Dump
//purpose  : 
//===========================================================================
  void BooleanOperations_AncestorsAndSuccessors::Dump() const
{
  Standard_Integer i;

  cout<<endl<<"AncestorsAndSuccessors :";
  cout<<endl<<"myAncestorsSize = "<<myAncestorsSize<<endl;
  for (i=1;i<=NumberOfAncestors();i++)
    cout<<GetAncestor(i)<<" ";
  cout<<endl<<"mySuccessorsSize = "<<mySuccessorsSize<<endl;
  for (i=1;i<=NumberOfSuccessors();i++)
    cout<<GetSuccessor(i)<<" ";
  cout<<endl;
  for (i=1;i<=NumberOfSuccessors();i++)
    cout<<GetOrientation(i)<<" ";
  cout<<endl;
}
