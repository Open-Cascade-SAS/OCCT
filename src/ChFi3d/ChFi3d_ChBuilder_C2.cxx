// File:	ChFi3d_ChBuilder_C2.cxx
// Created:	Fri Jul 26 17:27:36 1996
// Author:	Stagiaire Xuan Trang PHAMPHU
//		<xpu@pomalox.paris1.matra-dtv.fr>


#include <ChFi3d_ChBuilder.jxx>

//=======================================================================
//function : PerformTwoCorner
//purpose  :

//=======================================================================

void ChFi3d_ChBuilder::PerformTwoCorner(const Standard_Integer Index)
{
   PerformTwoCornerbyInter(Index);
}
