// File:	Plate_GlobalTranslationConstraint.cxx
// Created:	Mon Mar 30 12:49:46 1998
// Author:	# Andre LIEUTIER
//		<alr@sgi63>


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
