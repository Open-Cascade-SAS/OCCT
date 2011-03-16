#include <AppCont_FunctionTool.ixx>

#include <AppCont_Function.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

    
Standard_Real AppCont_FunctionTool::FirstParameter
  (const AppCont_Function& F) 
{
  return F.FirstParameter();
}

Standard_Real AppCont_FunctionTool::LastParameter
  (const AppCont_Function& F) 
{
  return F.LastParameter();
}

Standard_Integer AppCont_FunctionTool::NbP2d
  (const AppCont_Function&)
{
  return (0);
}


Standard_Integer AppCont_FunctionTool::NbP3d
  (const AppCont_Function&)
{
  return (1);
}

void AppCont_FunctionTool::Value(const AppCont_Function& F,
			      const Standard_Real U, 
			      TColgp_Array1OfPnt& tabPt)
{
  tabPt(tabPt.Lower()) = F.Value(U);
}




Standard_Boolean AppCont_FunctionTool::D1
  (const AppCont_Function& F,
   const Standard_Real U,
   TColgp_Array1OfVec& tabV)
{
  gp_Pnt P;
  gp_Vec V;
  Standard_Boolean Ok = F.D1(U, P, V);
  tabV(tabV.Lower()) = V;
  return Ok;
}






void AppCont_FunctionTool::Value(const AppCont_Function&,
			      const Standard_Real, 
			      TColgp_Array1OfPnt2d&)
{
}


void AppCont_FunctionTool::Value(const AppCont_Function&,
			      const Standard_Real, 
			      TColgp_Array1OfPnt&,
			      TColgp_Array1OfPnt2d&)
{
}



Standard_Boolean AppCont_FunctionTool::D1
  (const AppCont_Function&,
   const Standard_Real,
   TColgp_Array1OfVec2d&)
{
  return (Standard_True);
}


Standard_Boolean AppCont_FunctionTool::D1
  (const AppCont_Function&,
   const Standard_Real,
   TColgp_Array1OfVec&,
   TColgp_Array1OfVec2d&)
{
  return (Standard_True);
}

