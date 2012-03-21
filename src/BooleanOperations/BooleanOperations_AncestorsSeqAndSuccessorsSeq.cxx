// Created on: 2000-08-17
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



#include <BooleanOperations_AncestorsSeqAndSuccessorsSeq.ixx>

//===========================================================================
//function : BooleanOperations_AncestorsSeqAndSuccessorsSeq
//purpose  : creator
//===========================================================================
BooleanOperations_AncestorsSeqAndSuccessorsSeq::BooleanOperations_AncestorsSeqAndSuccessorsSeq() :
  myAncestors(),
  mySuccessors()
{
}


//===========================================================================
//function : Dump
//purpose  : 
//===========================================================================
  void BooleanOperations_AncestorsSeqAndSuccessorsSeq::Dump() const
{
  Standard_Integer i;

  cout<<endl<<"AncestorsSeqAndSuccessorsSeq :";
  cout<<endl<<"myAncestorsSize = "<<myAncestors.Length()<<endl;
  for (i=1;i<=NumberOfAncestors();i++)
    cout<<GetAncestor(i)<<" ";
  cout<<endl<<"mySuccessorsSize = "<<mySuccessors.Length()<<endl;
  for (i=1;i<=NumberOfSuccessors();i++)
    cout<<GetSuccessor(i)<<" ";
  cout<<endl;
}
