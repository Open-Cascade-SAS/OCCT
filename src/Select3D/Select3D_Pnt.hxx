#ifndef _Select3D_Pnt_HeaderFile
#define _Select3D_Pnt_HeaderFile

#include<gp_Pnt.hxx>
#include<Standard_ShortReal.hxx>
#include<Select3D_Macro.hxx>

struct Select3D_Pnt{  

 Standard_ShortReal x, y, z;
 
 inline operator gp_Pnt() const
 {
   return gp_Pnt(x, y, z);
 }

 inline operator gp_XYZ() const
 {
  return gp_XYZ(x, y, z);
 }

 inline gp_Pnt operator = (const gp_Pnt& thePnt)
 {
   x = DToF(thePnt.X());
   y = DToF(thePnt.Y());
   z = DToF(thePnt.Z());
   return *this;
 }

};

#endif

