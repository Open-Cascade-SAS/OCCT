#include <FairCurve_BattenLaw.ixx>

 FairCurve_BattenLaw::FairCurve_BattenLaw(const Standard_Real Heigth, 
					  const Standard_Real Slope, 
					  const Standard_Real Sliding)
                     : MiddleHeigth(Heigth), 
		       GeometricSlope(Slope),
                       LengthSliding(Sliding)
{
}


