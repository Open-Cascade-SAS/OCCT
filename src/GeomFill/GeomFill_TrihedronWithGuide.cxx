// File:	GeomFill_TrihedronWithGuide.cxx
// Created:	Wed Jul  8 11:42:29 1998
// Author:	Stephanie HUMEAU
//		<shu@sun17>


#include <GeomFill_TrihedronWithGuide.ixx>


 Handle(Adaptor3d_HCurve) GeomFill_TrihedronWithGuide::Guide()const
{
  return myGuide;
}


