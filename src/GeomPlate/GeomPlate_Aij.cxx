// File:	GeomPlate_Aij.cxx
// Created:	Wed Nov 25 11:47:25 1998
// Author:	Julia GERASIMOVA
//		<jgv@redfox.nnov.matra-dtv.fr>


#include <GeomPlate_Aij.ixx>

GeomPlate_Aij::GeomPlate_Aij()
{
}

GeomPlate_Aij::GeomPlate_Aij( const Standard_Integer anInd1,
			      const Standard_Integer anInd2,
			      const gp_Vec& aVec )
{
  Ind1 = anInd1;
  Ind2 = anInd2;
  Vec  = aVec;
}
