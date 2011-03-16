// File:	GeomFill_Line.cxx
// Created:	Fri Feb 18 16:48:13 1994
// Author:	Bruno DUMORTIER
//		<dub@fuegox>

#include <GeomFill_Line.ixx>



//=======================================================================
//function : GeomFill_Line
//purpose  : 
//=======================================================================

GeomFill_Line::GeomFill_Line()
{
  myNbPoints = 0;
}


//=======================================================================
//function : GeomFill_Line
//purpose  : 
//=======================================================================

GeomFill_Line::GeomFill_Line(const Standard_Integer NbPoints)
     : myNbPoints(NbPoints)
{
}
