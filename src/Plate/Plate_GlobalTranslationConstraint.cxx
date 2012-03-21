// Created on: 1998-03-30
// Created by: # Andre LIEUTIER
// Copyright (c) 1998-1999 Matra Datavision
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



#include <Plate_GlobalTranslationConstraint.ixx>

Plate_GlobalTranslationConstraint::Plate_GlobalTranslationConstraint(const TColgp_SequenceOfXY& SOfXY)
:myLXYZC(SOfXY.Length()-1,SOfXY.Length() )
{
  Standard_Integer i ;
  for( i=1;i<=SOfXY.Length();i++)
    {
      myLXYZC.SetPPC(i,Plate_PinpointConstraint (SOfXY(i),gp_XYZ(0.,0.,0.),0,0));
    }
  for(i=1;i<=SOfXY.Length()-1;i++)
	{
	  myLXYZC.SetCoeff(i,1,-1.);
	  for(Standard_Integer j=2;j<=SOfXY.Length();j++)
	    {
	      if(j==(i+1)) myLXYZC.SetCoeff(i,j,1.);
	      else  myLXYZC.SetCoeff(i,j,0.);
	    }  
	}
}
