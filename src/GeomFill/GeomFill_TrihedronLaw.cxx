// File:	GeomFill_TrihedronLaw.cxx
// Created:	Fri Dec  5 18:03:11 1997
// Author:	Philippe MANGIN
//		<pmn@sgi29>


#include <GeomFill_TrihedronLaw.ixx>

#include <Standard_NotImplemented.hxx>

 void GeomFill_TrihedronLaw::SetCurve(const Handle(Adaptor3d_HCurve)& C) 
{
  myCurve = C;
  myTrimmed = myCurve;
}

//==================================================================
//Function : ErrorStatus
//Purpose :
//==================================================================
 GeomFill_PipeError GeomFill_TrihedronLaw::ErrorStatus() const
{
  return GeomFill_PipeOk;
}

 Standard_Boolean GeomFill_TrihedronLaw::D1(const Standard_Real,
					    gp_Vec& ,gp_Vec&,gp_Vec&,
					    gp_Vec&,gp_Vec&,gp_Vec& ) 
{
  Standard_NotImplemented::Raise(" GeomFill_TrihedronLaw::D2");
  return Standard_False; 
}

 Standard_Boolean GeomFill_TrihedronLaw::D2(const Standard_Real,
					    gp_Vec& ,gp_Vec&,gp_Vec&,
					    gp_Vec& ,gp_Vec&,gp_Vec&,
					    gp_Vec&,gp_Vec& ,gp_Vec&) 
{
  Standard_NotImplemented::Raise(" GeomFill_TrihedronLaw::D2");
  return Standard_False;
}

void GeomFill_TrihedronLaw::SetInterval(const Standard_Real First,
					 const Standard_Real Last) 
{
 myTrimmed = myCurve->Trim(First, Last, 0);  
}

 void GeomFill_TrihedronLaw::GetInterval(Standard_Real& First,
					 Standard_Real& Last) 
{
  First =  myTrimmed->FirstParameter();
  Last  =  myTrimmed->LastParameter();
}

 Standard_Boolean GeomFill_TrihedronLaw::IsConstant() const
{
  return Standard_False;
}

 Standard_Boolean GeomFill_TrihedronLaw::IsOnlyBy3dCurve() const
{
  return Standard_False;
}
