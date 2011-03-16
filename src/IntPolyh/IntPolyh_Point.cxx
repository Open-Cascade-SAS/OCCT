// File:	IntPolyh_Point.cxx
// Created:	Mon Mar  8 09:32:00 1999
// Author:	Fabrice SERVANT
//		<fst@cleox.paris1.matra-dtv.fr>


#include <IntPolyh_Point.ixx>

#include <stdio.h>


IntPolyh_Point::IntPolyh_Point() : x(0),y(0),z(0),u(0),v(0),POC(1) {
}

IntPolyh_Point::IntPolyh_Point(const Standard_Real _x,
			       const Standard_Real _y,
			       const Standard_Real _z,
			       const Standard_Real _u,
			       const Standard_Real _v):POC(1) { 
  x=_x; y=_y; z=_z; u=_u; v=_v;
}

Standard_Real IntPolyh_Point::X() const { return(x); } 
Standard_Real IntPolyh_Point::Y() const { return(y); }  
Standard_Real IntPolyh_Point::Z() const { return(z); } 
Standard_Real IntPolyh_Point::U() const { return(u); } 
Standard_Real IntPolyh_Point::V() const { return(v); }
Standard_Integer IntPolyh_Point::PartOfCommon() const {return(POC);}
  

void IntPolyh_Point::Set(const Standard_Real _x,const Standard_Real _y, const Standard_Real _z,
			 const Standard_Real _u, const Standard_Real _v, const Standard_Integer II) { 
  x=_x; y=_y; z=_z; u=_u; v=_v; POC=II;}

void IntPolyh_Point::Equal(const IntPolyh_Point &Pt) {
  x = Pt.x;
  y = Pt.y;
  z = Pt.z;
  u = Pt.u;
  v = Pt.v;
}

void IntPolyh_Point::SetX(const Standard_Real _x) { x=_x; } 
void IntPolyh_Point::SetY(const Standard_Real _y) { y=_y; } 
void IntPolyh_Point::SetZ(const Standard_Real _z) { z=_z; } 
void IntPolyh_Point::SetU(const Standard_Real _u) { u=_u; } 
void IntPolyh_Point::SetV(const Standard_Real _v) { v=_v; } 
void IntPolyh_Point::SetPartOfCommon(const Standard_Integer ii) { POC=ii; }

void IntPolyh_Point::Middle(const Handle(Adaptor3d_HSurface)& MySurface,
				      const IntPolyh_Point & Point1, 
				      const IntPolyh_Point & Point2){
  u = (Point1.U()+Point2.U())*0.5;
  v = (Point1.V()+Point2.V())*0.5;
  
  gp_Pnt PtXYZ = (MySurface)->Value(u, v);

  x=PtXYZ.X();
  y=PtXYZ.Y(); 
  z=PtXYZ.Z();
}


IntPolyh_Point IntPolyh_Point::Add(const IntPolyh_Point &P1)const{
  IntPolyh_Point res;
  res.SetX(x+P1.X());
  res.SetY(y+P1.Y());
  res.SetZ(z+P1.Z());
  res.SetU(u+P1.U());
  res.SetV(v+P1.V());
  return(res);
}  

IntPolyh_Point IntPolyh_Point::Sub(const IntPolyh_Point &P1)const{ 
  IntPolyh_Point res;
  res.SetX(x-P1.X());
  res.SetY(y-P1.Y());
  res.SetZ(z-P1.Z());
  res.SetU(u-P1.U());
  res.SetV(v-P1.V());
  return(res);
} 

IntPolyh_Point IntPolyh_Point::Divide(const Standard_Real RR)const{ 
  IntPolyh_Point res;
  if (Abs(RR)>10.0e-20) {
    res.SetX(x/RR);
    res.SetY(y/RR);
    res.SetZ(z/RR);
    res.SetU(u/RR);
    res.SetV(v/RR);
  }
  else { 
   printf("Division par zero RR=%f\n",RR);
   //Dump();
  }
  return(res);
}

IntPolyh_Point IntPolyh_Point::Multiplication(const Standard_Real RR)const{ 
  IntPolyh_Point res;
  res.SetX(x*RR);
  res.SetY(y*RR);
  res.SetZ(z*RR);
  res.SetU(u*RR);
  res.SetV(v*RR);
  return(res);
}

Standard_Real IntPolyh_Point::SquareModulus()const{
  Standard_Real res=x*x+y*y+z*z;
  return(res);
}

Standard_Real IntPolyh_Point::SquareDistance(const IntPolyh_Point &P2)const{
  Standard_Real res=(x-P2.x)*(x-P2.x)+(y-P2.y)*(y-P2.y)+(z-P2.z)*(z-P2.z);
  return(res);
}

//inline 
Standard_Real IntPolyh_Point::Dot(const IntPolyh_Point &b ) const{ 
  Standard_Real t=x*b.x+y*b.y+z*b.z;
  return(t);
}

//inline 
void IntPolyh_Point::Cross(const IntPolyh_Point &a,const IntPolyh_Point &b){ 
  x=a.y*b.z-a.z*b.y;
  y=a.z*b.x-a.x*b.z;
  z=a.x*b.y-a.y*b.x;
//u=?
//v=?
}

void IntPolyh_Point::Dump() const{ 
  printf("\nPoint : x=%+8.3eg y=%+8.3eg z=%+8.3eg u=%+8.3eg v=%+8.3eg\n",x,y,z,u,v);
}

void IntPolyh_Point::Dump(const Standard_Integer i) const{ 
  printf("\nPoint(%3d) : x=%+8.3eg y=%+8.3eg z=%+8.3eg u=%+8.3eg v=%+8.3eg poc=%3d\n",i,x,y,z,u,v,POC);
}







