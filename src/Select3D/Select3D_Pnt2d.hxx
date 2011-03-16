#ifndef _Select3D_Pnt2d_HeaderFile
#define _Select3D_Pnt2d_HeaderFile

#include<gp_Pnt2d.hxx>
#include<Standard_ShortReal.hxx>
#include<Select3D_Macro.hxx>

struct Select3D_Pnt2d{

 Standard_ShortReal x, y;

 inline operator gp_Pnt2d() const
 {
   return gp_Pnt2d(x, y);
 }

 inline operator gp_XY() const
 {
  return gp_XY(x, y); 
 }

 inline gp_Pnt2d operator = (const gp_Pnt2d& thePnt)
 {
   x = DToF(thePnt.X());
   y = DToF(thePnt.Y());
   return *this;
 }


};

#endif

