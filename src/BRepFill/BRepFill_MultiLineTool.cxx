// Created on: 1994-11-14
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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

#include <BRepFill_MultiLineTool.ixx>


#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real BRepFill_MultiLineTool::FirstParameter
(const BRepFill_MultiLine& ML)
{
  return ML.FirstParameter();
}


//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real BRepFill_MultiLineTool::LastParameter
(const BRepFill_MultiLine& ML)
{
  return ML.LastParameter();
}


//=======================================================================
//function : NbP2d
//purpose  : 
//=======================================================================

Standard_Integer BRepFill_MultiLineTool::NbP2d
(const BRepFill_MultiLine&)
{
  return 2;
}


//=======================================================================
//function : NbP3d
//purpose  : 
//=======================================================================

Standard_Integer BRepFill_MultiLineTool::NbP3d(const BRepFill_MultiLine&)
{
  return 1;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void BRepFill_MultiLineTool::Value(const BRepFill_MultiLine& , 
				   const Standard_Real, 
				   TColgp_Array1OfPnt&)
{
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void BRepFill_MultiLineTool::Value(const BRepFill_MultiLine&, 
				   const Standard_Real, 
				   TColgp_Array1OfPnt2d&)
{
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

void BRepFill_MultiLineTool::Value(const BRepFill_MultiLine& ML, 
				   const Standard_Real U,
				   TColgp_Array1OfPnt& tabPt,
				   TColgp_Array1OfPnt2d& tabPt2d)
{
  tabPt(1)   = ML.Value(U);
  tabPt2d(1) = ML.ValueOnF1(U);
  tabPt2d(2) = ML.ValueOnF2(U);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

Standard_Boolean BRepFill_MultiLineTool::D1(const BRepFill_MultiLine&,
					    const Standard_Real,
					    TColgp_Array1OfVec&)
{
  return Standard_False;
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

Standard_Boolean BRepFill_MultiLineTool::D1(const BRepFill_MultiLine&,
					    const Standard_Real,
					    TColgp_Array1OfVec2d&)
{
  return Standard_False;
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

Standard_Boolean BRepFill_MultiLineTool::D1(const BRepFill_MultiLine&,
					    const Standard_Real,
					    TColgp_Array1OfVec&,
					    TColgp_Array1OfVec2d&)
{
  return Standard_False;
}
