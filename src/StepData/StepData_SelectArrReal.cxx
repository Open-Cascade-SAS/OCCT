#include <StepData_SelectArrReal.ixx>

//  Definitions : cf Field
#define myKindArrReal 8


//=======================================================================
//function : StepData_SelectSeqReal
//purpose  : 
//=======================================================================

StepData_SelectArrReal::StepData_SelectArrReal ()
{
}




//=======================================================================
//function : Kind
//purpose  : 
//=======================================================================

Standard_Integer StepData_SelectArrReal::Kind () const
{
  return myKindArrReal;  
}


//=======================================================================
//function : ArrReal
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) StepData_SelectArrReal::ArrReal () const
{
 return theArr;  
}


//=======================================================================
//function : SetArrReal
//purpose  : 
//=======================================================================

void StepData_SelectArrReal::SetArrReal (const Handle(TColStd_HArray1OfReal)& arr)
{
  theArr = arr;  
}
