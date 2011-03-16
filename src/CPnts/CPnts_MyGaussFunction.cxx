
#include <CPnts_MyGaussFunction.ixx>

void CPnts_MyGaussFunction::Init(const CPnts_RealFunction& F,
				 const Standard_Address D) 
{
  myFunction = F;
  myData = D;
}

Standard_Boolean CPnts_MyGaussFunction::Value(const Standard_Real X,
					      Standard_Real& F)
{
  F = myFunction(X,myData);
  return Standard_True;
}










