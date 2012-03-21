// Created on: 1991-01-25
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
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


#include <Standard_Stream.hxx>
#include <TopLoc_Datum3D.ixx>

//=======================================================================
//function : TopLoc_Datum3D
//purpose  : Identity
//=======================================================================

TopLoc_Datum3D::TopLoc_Datum3D () 
{
}

//=======================================================================
//function : TopLoc_Datum3D
//purpose  : 
//=======================================================================

TopLoc_Datum3D::TopLoc_Datum3D (const gp_Trsf& T) :
 myTrsf(T)
{
}

//=======================================================================
//function : ShallowDump
//purpose  : 
//=======================================================================

void  TopLoc_Datum3D::ShallowDump(Standard_OStream& S) const 
{
  S << " TopLoc_Datum3D " << (void*)this << endl;
  Standard_Integer i;
  gp_Trsf T = myTrsf;
  for (i = 1; i<=3; i++) {
    S<<"  ( "<<setw(10)<<T.Value(i,1);
    S<<","<<setw(10)<<T.Value(i,2);
    S<<","<<setw(10)<<T.Value(i,3);
    S<<","<<setw(10)<<T.Value(i,4);
    S<<")\n";
  }
  S << endl;
}
    
