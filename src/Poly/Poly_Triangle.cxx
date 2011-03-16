// File:	Poly_Triangle.cxx
// Created:	Mon Mar  6 10:47:39 1995
// Author:	Laurent PAINNOT
//		<lpa@metrox>


#include <Poly_Triangle.ixx>

//=======================================================================
//function : Poly_Triangle
//purpose  : 
//=======================================================================

Poly_Triangle::Poly_Triangle()
{
  myNodes[0] =  myNodes[1] = myNodes[2] = 0;
}

//=======================================================================
//function : Poly_Triangle
//purpose  : 
//=======================================================================

Poly_Triangle::Poly_Triangle(const Standard_Integer N1,
			     const Standard_Integer N2, 
			     const Standard_Integer N3)
{
  myNodes[0] = N1;
  myNodes[1] = N2;
  myNodes[2] = N3;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void Poly_Triangle::Set(const Standard_Integer N1, 
			const Standard_Integer N2, 
			const Standard_Integer N3)
{
  myNodes[0] = N1;
  myNodes[1] = N2;
  myNodes[2] = N3;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

void Poly_Triangle::Get(Standard_Integer& N1, 
			Standard_Integer& N2, 
			Standard_Integer& N3) const 
{
  N1 = myNodes[0];
  N2 = myNodes[1];
  N3 = myNodes[2];
}

