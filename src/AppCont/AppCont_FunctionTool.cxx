// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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

