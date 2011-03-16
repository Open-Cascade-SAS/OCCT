#include <Blend_Function.ixx>

Standard_Integer Blend_Function::NbVariables () const
{
  return 4;
}

const gp_Pnt& Blend_Function::Pnt1() const 
{
  return PointOnS1();
}

const gp_Pnt& Blend_Function::Pnt2() const 
{
  return PointOnS2(); 
}

Standard_Boolean Blend_Function::TwistOnS1() const
{
  return Standard_False;
}

Standard_Boolean Blend_Function::TwistOnS2() const
{
  return Standard_False;
}

Standard_Boolean Blend_Function::Section(const Blend_Point& P, 
					 TColgp_Array1OfPnt& Poles, 
					 TColgp_Array1OfVec& DPoles, 
					 TColgp_Array1OfVec& D2Poles, 
					 TColgp_Array1OfPnt2d& Poles2d, 
					 TColgp_Array1OfVec2d& DPoles2d, 
					 TColgp_Array1OfVec2d& D2Poles2d, 
					 TColStd_Array1OfReal& Weigths, 
					 TColStd_Array1OfReal& DWeigths,
					 TColStd_Array1OfReal& D2Weigths)
{
  return Standard_False;
}
