// File:	PFunction_Function.cxx
// Created:	Thu Jun 17 11:03:48 1999
// Author:	Vladislav ROMASHKO
//		<vro@flox.nnov.matra-dtv.fr>


#include <PFunction_Function.ixx>
#include <Standard_GUID.hxx>

PFunction_Function::PFunction_Function()
{}

void PFunction_Function::SetDriverGUID(const Standard_GUID& driverGUID)
{
  myDriverGUID = driverGUID;
}

Standard_GUID PFunction_Function::GetDriverGUID() const
{
  return myDriverGUID;
}

Standard_Integer PFunction_Function::GetFailure() const
{
  return myFailure;
}

void PFunction_Function::SetFailure(const Standard_Integer mode)
{
  myFailure = mode;
}

