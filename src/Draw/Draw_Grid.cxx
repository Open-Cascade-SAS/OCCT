// Created on: 1994-02-03
// Created by: Jean Marc LACHAUME
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



#include <Draw_Grid.ixx>

#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>

#include <Draw_Appli.hxx>
#include <Draw_ColorKind.hxx>
#include <Draw_Color.hxx>

static Standard_Real MinimumStep = 1.e-3 ;
static Standard_Real Ratio       = 200.0 ;

//#ifdef WNT
extern Draw_Viewer dout;
//#endif

//=======================================================================
// Function : Draw_Grid
// Purpose  : Constructor.
//=======================================================================

Draw_Grid::Draw_Grid () :
       myStepX    (0.0) ,
       myStepY    (0.0) ,
       myStepZ    (0.0) ,
       myIsActive (Standard_False)
{
}

//=======================================================================
// Function : Steps
// Purpose  : Sets the steps along the X, Y & Z axis.
//=======================================================================

void Draw_Grid::Steps (const Standard_Real StepX,
				const Standard_Real StepY,
				const Standard_Real StepZ)
{
  myStepX = Abs (StepX) ;
  myStepY = Abs (StepY) ;
  myStepZ = Abs (StepZ) ;
  myIsActive =    myStepX > MinimumStep
               && myStepY > MinimumStep
	       && myStepZ > MinimumStep ;
}

//=======================================================================
// Function : DrawOn
// Purpose  : Displays the grid.
//=======================================================================

void Draw_Grid::DrawOn (Draw_Display& Out) const
{
  if (!myIsActive) return ;
  
  Standard_Integer xmin, xmax, ymin, ymax ;
  Standard_Integer IndexX, IndexY ;
  Standard_Real StepX, StepY ;
  Standard_Integer MinIndexX, MaxIndexX, MinIndexY, MaxIndexY ;
  Standard_Real Offset ;
  Standard_Real zoom, Xmin, Xmax, Ymin, Ymax ;
  gp_Trsf T ;
  gp_Pnt Pnt1, Pnt2 ;

  Standard_Integer IdtView ;
  char *Type ;

  IdtView = Out.ViewId () ;
  if (!dout.HasView (IdtView)) return ;
  Type = dout.GetType (IdtView) ;
  switch (*(Type+1)) {
    case 'X' : StepX = myStepX ; break ;
    case 'Y' : StepX = myStepY ; break ;
    case 'Z' : StepX = myStepZ ; break ;
    default  : StepX = 0.0 ; break ;
  }
  switch (*(Type+3)) {
    case 'X' : StepY = myStepX ; break ;
    case 'Y' : StepY = myStepY ; break ;
    case 'Z' : StepY = myStepZ ; break ;
    default  : StepY = 0.0 ; break ;
  }
  
  if (StepX > MinimumStep && StepY > MinimumStep) {

    dout.GetFrame (IdtView, xmin, ymin, xmax, ymax) ;
    dout.GetTrsf  (IdtView, T) ; T.Invert () ;
    zoom = dout.Zoom (IdtView) ;

    Xmin = ((Standard_Real) xmin) / zoom ;
    Xmax = ((Standard_Real) xmax) / zoom ;
    Ymin = ((Standard_Real) ymin) / zoom ;
    Ymax = ((Standard_Real) ymax) / zoom ;

    Offset = Min (Xmax - Xmin, Ymax - Ymin) / Ratio ;

    MinIndexX = (Standard_Integer) (Xmin / StepX) ;
    MaxIndexX = (Standard_Integer) (Xmax / StepX) ;
    MinIndexY = (Standard_Integer) (Ymin / StepY) ;
    MaxIndexY = (Standard_Integer) (Ymax / StepY) ;

    for (IndexX = MinIndexX ; IndexX <= MaxIndexX ; IndexX++) {
      for (IndexY = MinIndexY ; IndexY <= MaxIndexY ; IndexY++) {
	Standard_Real X = ((Standard_Real) IndexX) * StepX ;
	Standard_Real Y = ((Standard_Real) IndexY) * StepY ;
	
	Pnt1.SetCoord (X - Offset, Y, 0.0) ; Pnt1.Transform (T) ;
	Pnt2.SetCoord (X + Offset, Y, 0.0) ; Pnt2.Transform (T) ;
	Out.SetColor (Draw_Color (Draw_bleu)) ;
	Out.Draw (Pnt1, Pnt2) ;

	Pnt1.SetCoord (X, Y - Offset, 0.0) ; Pnt1.Transform (T) ;
	Pnt2.SetCoord (X, Y + Offset, 0.0) ; Pnt2.Transform (T) ;
	Out.SetColor (Draw_Color (Draw_bleu)) ;
	Out.Draw (Pnt1, Pnt2) ;
      }
    }

  }
}
