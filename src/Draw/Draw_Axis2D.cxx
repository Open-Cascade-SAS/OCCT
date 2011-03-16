// File:	Draw_Axis2D.cxx
// Created:	Wed Apr 29 15:24:18 1992
// Author:	Remi LEQUETTE
//		<rle@sdsun1>

#include <Draw_Axis2D.ixx>
#include <gp_Vec2d.hxx>
#include <gp.hxx>

extern Standard_Boolean Draw_Bounds;

//=======================================================================
//function : Draw_Axis2D
//purpose  : 
//=======================================================================

Draw_Axis2D::Draw_Axis2D (const Draw_Color& col, 
			  const Standard_Integer Size) :
       myAxes(gp_Pnt2d(0,0),gp_Dir2d(1,0)),myColor(col), mySize(Size)
{
}

//=======================================================================
//function : Draw_Axis2D
//purpose  : 
//=======================================================================

Draw_Axis2D::Draw_Axis2D (const gp_Pnt2d& p, 
			  const Draw_Color& col, 
			  const Standard_Integer Size) :
       myAxes(p,gp_Dir2d(1,0)), myColor(col), mySize(Size)
{
}

//=======================================================================
//function : Draw_Axis2D
//purpose  : 
//=======================================================================

Draw_Axis2D::Draw_Axis2D (const gp_Ax22d& a, 
			  const Draw_Color& col, 
			  const Standard_Integer Size) :
       myAxes(a), myColor(col), mySize(Size)
{
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void Draw_Axis2D::DrawOn (Draw_Display& dis) const
{
  Draw_Bounds = Standard_False;
  dis.SetColor(myColor);
  Standard_Real z = dis.Zoom();
  z = (Standard_Real)mySize / z;
  gp_Pnt2d P,P0 = myAxes.Location();
  P = P0.Translated(gp_Vec2d(myAxes.XDirection()) * z);
  dis.Draw(P0,P);
  dis.DrawString(P,"X");
  P = P0.Translated(gp_Vec2d(myAxes.YDirection()) * z);
  dis.Draw(P0,P);
  dis.DrawString(P,"Y");
  Draw_Bounds = Standard_True;
}

