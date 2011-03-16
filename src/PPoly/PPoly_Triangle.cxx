#include <PPoly_Triangle.ixx>

//=======================================================================
//function : PPoly_Triangle::PPoly_Triangle
//purpose  : 
//=======================================================================

PPoly_Triangle::PPoly_Triangle(const Standard_Integer N1, 
			       const Standard_Integer N2, 
			       const Standard_Integer N3)
{
  myNodes[0] = N1;
  myNodes[1] = N2;
  myNodes[2] = N3;  
}

//=======================================================================
//function : PPoly_Triangle::Set
//purpose  : 
//=======================================================================

void PPoly_Triangle::Set(const Standard_Integer N1, 
			 const Standard_Integer N2, 
			 const Standard_Integer N3)
{
  myNodes[0] = N1;
  myNodes[1] = N2;
  myNodes[2] = N3;  
}

//=======================================================================
//function : PPoly_Triangle::Get
//purpose  : 
//=======================================================================

void PPoly_Triangle::Get(Standard_Integer& N1, 
			 Standard_Integer& N2, 
			 Standard_Integer& N3) const 
{
  N1 = myNodes[0];
  N2 = myNodes[1];
  N3 = myNodes[2];  
}

