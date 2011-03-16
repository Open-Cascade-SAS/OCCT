//static const char* sccsid = "@(#)math_IntegerRandom.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
#include <math_IntegerRandom.ixx>

#include <math_Recipes.hxx>
  
 math_IntegerRandom::math_IntegerRandom(const Standard_Integer Lower, 
                                        const Standard_Integer Upper) {

   Low = Lower;
   Up = Upper;
   Dummy=-1;
   Random2(Dummy);
 }

void math_IntegerRandom::Reset() {
 
  Dummy=-1;
  Random2(Dummy);
}

Standard_Integer math_IntegerRandom::Next() {

  Standard_Real value=Random2(Dummy);
  Standard_Integer Result=(Standard_Integer)(Standard_Real((Up-Low))*value + Low);
  return (Result) ;
}

