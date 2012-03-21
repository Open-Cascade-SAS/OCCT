// Created on: 1993-05-06
// Created by: Jacques GOUSSARD
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
