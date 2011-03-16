// File:	DBRep_Face.cxx
// Created:	Thu Jul 15 11:32:53 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <DBRep_Face.ixx>

//=======================================================================
//function : DBRep_Face
//purpose  : 
//=======================================================================

DBRep_Face::DBRep_Face(const TopoDS_Face& F, 
		       const Standard_Integer N,
		       const Draw_Color& C) :
       myFace(F),
       myColor(C),
       myTypes(N ? 1 : 0,N),
       myParams(N ? 1 : 0,3*N)
{
}


