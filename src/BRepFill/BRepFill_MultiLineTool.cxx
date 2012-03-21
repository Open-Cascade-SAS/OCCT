// Created on: 1994-11-14
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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
