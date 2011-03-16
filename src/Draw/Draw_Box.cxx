// File:	Draw_Box.cxx
// Created:	Fri Mar 10 14:43:04 1995
// Author:	Remi LEQUETTE
//		<rle@phobox>


#include <Draw_Box.ixx>

//=======================================================================
//function : Draw_Box
//purpose  : 
//=======================================================================

Draw_Box::Draw_Box(const gp_Pnt& p1, const gp_Pnt& p2, const Draw_Color& col) :
   myFirst(p1), myLast(p2),myColor(col)
{
  Standard_Real t;
  if (myLast.X() < myFirst.X()) {
    t = myFirst.X();
    myFirst.SetX(myLast.X());
    myLast.SetX(t);
  }
  if (myLast.Y() < myFirst.Y()) {
    t = myFirst.Y();
    myFirst.SetY(myLast.Y());
    myLast.SetY(t);
  }
  if (myLast.Z() < myFirst.Z()) {
    t = myFirst.Z();
    myFirst.SetZ(myLast.Z());
    myLast.SetZ(t);
  }
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void Draw_Box::DrawOn(Draw_Display& dis) const 
{
  dis.SetColor(myColor);
  gp_Pnt P = myFirst;

  dis.MoveTo(P);
  P.SetX(myLast.X());
  dis.DrawTo(P);
  P.SetY(myLast.Y());
  dis.DrawTo(P);
  P.SetZ(myLast.Z());
  dis.DrawTo(P);
  P.SetX(myFirst.X());
  dis.DrawTo(P);
  P.SetY(myFirst.Y());
  dis.DrawTo(P);
  P.SetZ(myFirst.Z());
  dis.DrawTo(P);

  P.SetX(myLast.X());
  dis.MoveTo(P);
  P.SetZ(myLast.Z());
  dis.DrawTo(P);
  P.SetX(myFirst.X());
  dis.DrawTo(P);
  
  P.SetX(myLast.X());
  dis.MoveTo(P);
  P.SetY(myLast.Y());
  dis.DrawTo(P);
  
  P.SetX(myFirst.X());
  dis.MoveTo(P);
  P.SetZ(myFirst.Z());
  dis.DrawTo(P);
  P.SetY(myFirst.Y());
  dis.DrawTo(P);
  
  P.SetY(myLast.Y());
  dis.MoveTo(P);
  P.SetX(myLast.X());
  dis.DrawTo(P);
}

//=======================================================================
//function : First
//purpose  : 
//=======================================================================

const gp_Pnt& Draw_Box::First() const 
{
  return myFirst;
}

//=======================================================================
//function : First
//purpose  : 
//=======================================================================

void Draw_Box::First(const gp_Pnt& P)
{
  myFirst = P;
}

//=======================================================================
//function : Last
//purpose  : 
//=======================================================================

const gp_Pnt& Draw_Box::Last() const 
{
  return myLast;
}

//=======================================================================
//function : Last
//purpose  : 
//=======================================================================

void Draw_Box::Last(const gp_Pnt& P)
{
  myLast = P;
}

