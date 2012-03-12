// File:      IntPatch_Polygo.cxx
// Created:   Thu May  6 17:49:16 1993
// Author:    Jacques GOUSSARD
// Copyright: Matra Datavision 1993

#include <IntPatch_Polygo.ixx>

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

IntPatch_Polygo::IntPatch_Polygo (const Standard_Real theError)
: myError(theError)
{}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void IntPatch_Polygo::Dump () const
{
  static int num=0;
  num++; 
  cout<<"\n#------------- D u m p     B o x 2 d   ("<<num<<")"<<endl;
  Bounding().Dump();
  cout<<"\n#-----------------------------------------------"<<endl;
  
  const Standard_Integer nbs = NbSegments();
  cout<<"\npol2d "<<num<<" "<<nbs<<" ";
  cout<<DeflectionOverEstimation()<<endl;
  
  gp_Pnt2d P, PF;
  for(Standard_Integer i=1;i<=nbs;i++) {
    Segment(i,P,PF);
    cout<<"pnt2d "<<num<<"  "<< P.X()<<" "<<P.Y()<<endl;
  }
  cout<<"pnt2d "<<num<<"  "<< PF.X()<<" "<<PF.Y()<<endl;
}
