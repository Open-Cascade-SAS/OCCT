// Created on: 1993-05-06
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <gp_Pnt2d.hxx>
#include <IntPatch_Polygo.hxx>
#include <Standard_OutOfRange.hxx>

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
