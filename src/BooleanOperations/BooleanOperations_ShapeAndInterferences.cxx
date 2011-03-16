// File:	BooleanOperations_ShapeAndInterferences.cxx
// Created:	Tue Jul 25 11:27:07 2000
// Author:	Vincent DELOS
//		<vds@bulox.paris1.matra-dtv.fr>


#include <BooleanOperations_ShapeAndInterferences.ixx>

//===========================================================================
//function : ShapeAndInterferences
//purpose  : 
//===========================================================================
  BooleanOperations_ShapeAndInterferences::BooleanOperations_ShapeAndInterferences():
    myState(BooleanOperations_UNKNOWN)
{
}
//modified by NIZNHY-PKV Wed Feb  2 12:55:46 2005f
/*
//===========================================================================
//function : SetInterference
//purpose  : 
//===========================================================================
  void BooleanOperations_ShapeAndInterferences::SetInterference
    (const BooleanOperations_InterferenceResult& Interf) 
{
  myInterferencesList.SetInterference(Interf);
}

//===========================================================================
//function : Dump
//purpose  : 
//===========================================================================
  void BooleanOperations_ShapeAndInterferences::Dump() const
{
  cout<<endl<<"myBoundingBox :"<<endl;
  myBoundingBox.Dump();
  myAncestorsAndSuccessors.Dump();

  myInterferencesList.Dump();
  cout<<endl<<"myState = "<<myState<<endl;
}
*/
//modified by NIZNHY-PKV Wed Feb  2 12:55:56 2005
