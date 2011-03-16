// File:	BooleanOperations_AncestorsSeqAndSuccessorsSeq.cxx
// Created:	Thu Aug 17 10:04:35 2000
// Author:	Vincent DELOS
//		<vds@bulox.paris1.matra-dtv.fr>


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
