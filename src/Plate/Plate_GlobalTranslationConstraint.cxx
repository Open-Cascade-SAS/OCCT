// Created on: 1998-03-30
// Created by: # Andre LIEUTIER
// Copyright (c) 1998-1999 Matra Datavision
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


#include <Plate_GlobalTranslationConstraint.hxx>
#include <Plate_LinearXYZConstraint.hxx>

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
