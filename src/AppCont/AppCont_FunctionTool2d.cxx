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

#include <AppCont_FunctionTool2d.ixx>

#include <AppCont_Function2d.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>


Standard_Real AppCont_FunctionTool2d::FirstParameter
  (const AppCont_Function2d& F) 
{
  return F.FirstParameter();
}

Standard_Real AppCont_FunctionTool2d::LastParameter
  (const AppCont_Function2d& F) 
{
  return F.LastParameter();
}

Standard_Integer AppCont_FunctionTool2d::NbP2d
  (const AppCont_Function2d&)
{
  return (1);
}


Standard_Integer AppCont_FunctionTool2d::NbP3d
  (const AppCont_Function2d&)
{
  return (0);
}

void AppCont_FunctionTool2d::Value(const AppCont_Function2d& F,
				   const Standard_Real U, 
				   TColgp_Array1OfPnt2d& tabPt)
{
  tabPt(tabPt.Lower()) = F.Value(U);
}




Standard_Boolean AppCont_FunctionTool2d::D1
  (const AppCont_Function2d& F,
   const Standard_Real U,
   TColgp_Array1OfVec2d& tabV)
{
  gp_Pnt2d P;
  gp_Vec2d V;
  Standard_Boolean Ok = F.D1(U, P, V);
  tabV(tabV.Lower()) = V;
  return Ok;
}






void AppCont_FunctionTool2d::Value(const AppCont_Function2d&,
				 const Standard_Real, 
				 TColgp_Array1OfPnt&)
{
}


void AppCont_FunctionTool2d::Value(const AppCont_Function2d&,
				   const Standard_Real, 
				   TColgp_Array1OfPnt&,
				   TColgp_Array1OfPnt2d&)
{
}



Standard_Boolean AppCont_FunctionTool2d::D1
  (const AppCont_Function2d&,
   const Standard_Real,
   TColgp_Array1OfVec&)
{
  return (Standard_False);
}


Standard_Boolean AppCont_FunctionTool2d::D1
  (const AppCont_Function2d&,
   const Standard_Real,
   TColgp_Array1OfVec&,
   TColgp_Array1OfVec2d&)
{
  return (Standard_False);
}

