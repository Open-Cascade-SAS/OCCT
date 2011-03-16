// File:	Draw_Axis3D.cxx
// Created:	Wed Apr 29 15:24:18 1992
// Author:	Remi LEQUETTE
//		<rle@sdsun1>

#include <Draw_Axis3D.ixx>
#include <gp_Vec.hxx>
#include <gp.hxx>

extern Standard_Boolean Draw_Bounds;


//=======================================================================
//function : Draw_Axis3D
//purpose  : 
//=======================================================================

Draw_Axis3D::Draw_Axis3D (const Draw_Color& col, 
			  const Standard_Integer Size) :
       myAxes(gp::XOY()),myColor(col), mySize(Size)
{
}

//=======================================================================
//function : Draw_Axis3D
//purpose  : 
//=======================================================================

Draw_Axis3D::Draw_Axis3D (const gp_Pnt& p, 
			  const Draw_Color& col, 
			  const Standard_Integer Size) :
       myAxes(p,gp::DZ(),gp::DX()), myColor(col), mySize(Size)
{
}

//=======================================================================
//function : Draw_Axis3D
//purpose  : 
//=======================================================================

Draw_Axis3D::Draw_Axis3D (const gp_Ax3& a, 
			  const Draw_Color& col, 
			  const Standard_Integer Size) :
       myAxes(a), myColor(col), mySize(Size)
{
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void Draw_Axis3D::DrawOn (Draw_Display& dis) const
{
  Draw_Bounds = Standard_False;
  dis.SetColor(myColor);
  Standard_Real z = dis.Zoom();
  z = (Standard_Real)mySize / z;
  gp_Pnt P,P0 = myAxes.Location();
  P = P0.Translated(gp_Vec(myAxes.XDirection()) * z);
  dis.Draw(P0,P);
  dis.DrawString(P,"X");
  P = P0.Translated(gp_Vec(myAxes.YDirection()) * z);
  dis.Draw(P0,P);
  dis.DrawString(P,"Y");
  P = P0.Translated(gp_Vec(myAxes.Direction()) * z);
  dis.Draw(P0,P);
  dis.DrawString(P,"Z");
  Draw_Bounds = Standard_True;
}

