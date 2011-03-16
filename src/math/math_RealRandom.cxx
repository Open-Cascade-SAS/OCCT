//static const char* sccsid = "@(#)math_RealRandom.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
#include <math_RealRandom.ixx>
#include <math_Recipes.hxx>

math_RealRandom::math_RealRandom(const Standard_Real Lower,
				 const Standard_Real Upper) {

  Low = Lower;
  Up = Upper;
  Dummy = -1;
  Random2(Dummy);
}

void math_RealRandom::Reset() {

  Dummy = -1;
  Random2(Dummy);
}

Standard_Real math_RealRandom::Next() {
  
  Standard_Real value = Random2(Dummy);
  Standard_Real Result = (Up - Low)*value + Low;
  return Result;  

}
