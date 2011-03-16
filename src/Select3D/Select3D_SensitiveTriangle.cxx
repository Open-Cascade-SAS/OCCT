// File:    Select3D_SensitiveTriangle.cxx
// Created: Wed May 14 16:56:06 1997
// Author:  Robert COUBLANC
//      <rob@robox.paris1.matra-dtv.fr>


#include <Select3D_SensitiveTriangle.ixx>


#include <SelectBasics_BasicTool.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir2d.hxx>
#include <Precision.hxx>
#include <Bnd_Box.hxx>
#include <ElCLib.hxx>
#include <TopLoc_Location.hxx>

#include <CSLib_Class2d.hxx>

#define COORD(a,b) ((Select3D_Pnt*)mypolyg3d)[(a)].b
#define COORD2d(a,b) ((Select3D_Pnt2d*)mypolyg2d)[(a)].b

static Standard_Boolean S3D_Str_NearSegment (const gp_XY& p0, const gp_XY& p1, const gp_XY& TheP,
                                             const Standard_Real aTol, Standard_Real& aDMin)
{
  gp_XY V01(p1);
  V01 -= p0;

  gp_XY Vec(TheP);
  Vec -= p0;
  
  Standard_Real u = Vec*V01.Normalized();
  if(u<-aTol) return Standard_False;
  Standard_Real u1 = u-aTol;
  Standard_Real modmod = V01.SquareModulus();
  if(u1*u1> modmod) return Standard_False;

  gp_XY N01 (-V01.Y(),V01.X());
  N01.Normalize();
  aDMin = Abs (Vec * N01);
  return aDMin <= aTol;
}

//==================================================
// Function: 
// Purpose :
//==================================================

Select3D_SensitiveTriangle::
Select3D_SensitiveTriangle(const Handle(SelectBasics_EntityOwner)& OwnerId,
			   const gp_Pnt& P0,
			   const gp_Pnt& P1,
			   const gp_Pnt& P2,
			   const Select3D_TypeOfSensitivity aType):
Select3D_SensitivePoly(OwnerId,3),
mytype (aType)
{ 
  ((Select3D_Pnt*)mypolyg3d)[0] = P0;
  ((Select3D_Pnt*)mypolyg3d)[1] = P1;
  ((Select3D_Pnt*)mypolyg3d)[2] = P2;
}

//==================================================
// Function: 
// Purpose :
//==================================================

Standard_Boolean Select3D_SensitiveTriangle::
Matches(const Standard_Real X,
	const Standard_Real Y,
	const Standard_Real aTol,
	Standard_Real& DMin)
{
  Select3D_SensitiveEntity::Matches(X,Y,aTol,DMin);
  if(Bnd_Box2d(mybox2d).IsOut(gp_Pnt2d(X,Y))) return Standard_False;

  Standard_Integer Res;
  switch (mytype){
  case Select3D_TOS_BOUNDARY:
    Res = Status(X,Y,aTol,DMin);
    return Res== 1;
    break;
  case Select3D_TOS_INTERIOR:
    Res = Status(X,Y,aTol,DMin);
    return (Res==0 || Res == 1);
#ifndef DEB
  default:
    break;
#endif    
  }
  return Standard_True;
}

Standard_Boolean Select3D_SensitiveTriangle::
Matches (const Standard_Real XMin,
	 const Standard_Real YMin,
	 const Standard_Real XMax,
	 const Standard_Real YMax,
	 const Standard_Real aTol)
{
  Bnd_Box2d B;
  B.Update(Min(XMin,XMax)-aTol,
       Min(YMin,YMax)-aTol,
       Max(XMin,XMax)+aTol,
       Max(YMin,YMax)+aTol);
  for(Standard_Integer i=0;i<=2;i++){
    if(B.IsOut(((Select3D_Pnt2d*)mypolyg2d)[i])) 
      return Standard_False;}
  return Standard_True;
}


//=======================================================================
//function : Matches
//purpose  : 
//=======================================================================

Standard_Boolean Select3D_SensitiveTriangle::
Matches (const TColgp_Array1OfPnt2d& aPoly,
	 const Bnd_Box2d& aBox,
	 const Standard_Real aTol)
{ 
  Standard_Real Umin,Vmin,Umax,Vmax;
  aBox.Get(Umin,Vmin,Umax,Vmax);
  Standard_Real Tolu,Tolv;
  Tolu = 1e-7;
  Tolv = 1e-7;
  CSLib_Class2d aClassifier2d(aPoly,aTol,aTol,Umin,Vmin,Umax,Vmax);

  for(Standard_Integer i=0;i<=2;i++){
    Standard_Integer RES = aClassifier2d.SiDans(((Select3D_Pnt2d*)mypolyg2d)[i]);
    if(RES!=1) return Standard_False;
  }
  return Standard_True;
}



 void Select3D_SensitiveTriangle::Points3D(gp_Pnt& P0,gp_Pnt& P1,gp_Pnt& P2) const
{
  P0 = ((Select3D_Pnt*)mypolyg3d)[0];  P1 = ((Select3D_Pnt*)mypolyg3d)[1];  P2 = ((Select3D_Pnt*)mypolyg3d)[2];  
}

gp_Pnt Select3D_SensitiveTriangle::Center3D() const
{
  gp_XYZ CDG(((Select3D_Pnt*)mypolyg3d)[0]);
  CDG += ((Select3D_Pnt*)mypolyg3d)[1];
  CDG += ((Select3D_Pnt*)mypolyg3d)[2];
  CDG /=3.;
  return gp_Pnt(CDG);;
}      

gp_XY Select3D_SensitiveTriangle::Center2D() const
{
  return (gp_XY(((Select3D_Pnt2d*)mypolyg2d)[0])+gp_XY(((Select3D_Pnt2d*)mypolyg2d)[1])
          +gp_XY(((Select3D_Pnt2d*)mypolyg2d)[2]))/3.;
}

//=======================================================================
//function : Status
//purpose  : 0 = inside /1 = Boundary/ 2 = outside
//=======================================================================

Standard_Integer  Select3D_SensitiveTriangle::Status(const Standard_Real X,
						     const Standard_Real Y,
						     const Standard_Real aTol,
						     Standard_Real& DMin) const 
{
  
  return Status(((Select3D_Pnt2d*)mypolyg2d)[0],((Select3D_Pnt2d*)mypolyg2d)[1],
                ((Select3D_Pnt2d*)mypolyg2d)[2],gp_XY(X,Y),aTol,DMin);
  
}


//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

Standard_Integer  Select3D_SensitiveTriangle::Status(const gp_XY& p0,
						     const gp_XY& p1,
						     const gp_XY& p2,
						     const gp_XY& TheP,
						     const Standard_Real aTol,
						     Standard_Real& DMin)
{

  Bnd_Box2d B;
  B.Update(p0.X(),p0.Y());B.Update(p1.X(),p1.Y());B.Update(p2.X(),p2.Y());
  B.Enlarge(aTol);
  if(B.IsOut(TheP)) return 2; 
  
  // on classifie le point par rapport aux demi -espaces delimites
  // par chaque cote du triangle (a la tolerance pres)
  gp_XY V01(p1);V01-=p0;
  gp_XY V02(p2);V02-=p0;
  gp_XY V12(p2);V12-=p1;

  Standard_Real TolTol = aTol*aTol;

  // regardons les cas particuliers...
  //si l'un des vecteurs est quasi nul (2 points sont confondus), 
  // on sort tout de suite (on est deja dans la boite d'encombrement, c'est bon...)
  
  DMin = aTol;
  
  if ( V01.SquareModulus() <= gp::Resolution() )
  {
    Standard_Real LV = V02.SquareModulus();
    if ( LV <= gp::Resolution())
      return 0; // les 3 points sont confondus, et TheP est dans la boite englobante B...

    if ( S3D_Str_NearSegment (p0, p2, TheP, aTol, DMin) )
      return 0;
    return 2;    
  }
  if ( V02.SquareModulus() <= gp::Resolution() )
  {
    if ( S3D_Str_NearSegment (p0, p1, TheP, aTol, DMin) )
      return 0;
    return 2;
  }
  if ( V12.SquareModulus() <= gp::Resolution() )
  {
    if ( S3D_Str_NearSegment (p0, p1, TheP, aTol, DMin) )
      return 0;
    return 2;
  }
  if ( V01.CrossMagnitude(V02) <= gp::Resolution() )
  {
    if ( S3D_Str_NearSegment (p0, p1, TheP, aTol, DMin) )
      return 0;
    return 2;
  }

  // normale a p0p1 orientee...
  gp_Dir2d N (-V01.Y(), V01.X()); 
  Standard_Boolean Neg = (N * V02 < 0.);
  if ( Neg ) 
    N.Reverse();

  gp_XY Vec(TheP);
  Vec -= p0;
  
  Standard_Real aD1 = Vec * N.XY();
  if ( aD1 < -aTol ) 
    return 2;

  // normale a p1p2 orientee...
  if(Neg)
    N.SetCoord(p2.Y()-p1.Y(),p1.X()-p2.X());
  else
    N.SetCoord(p1.Y()-p2.Y(),p2.X()-p1.X());
  
  Vec.SetCoord(TheP.X()-p1.X(),TheP.Y()-p1.Y());
  Standard_Real aD2 = Vec * N.XY();
  if ( aD2 < -aTol ) 
    return 2;   // dehors

  // normale a p2p0 orientee...
  // attention v20 (x0-x2)    =>   N y2-y0   => -N  y0-y2
  //               (y0-y2)           x0-x2          x2-x0
  if(Neg)
    N.SetCoord(p0.Y()-p2.Y(),p2.X()-p0.X());
  else
    N.SetCoord(p2.Y()-p0.Y(),p0.X()-p2.X());
  
  Vec.SetCoord(TheP.X()-p2.X(),TheP.Y()-p2.Y());
  Standard_Real aD3 = Vec * N.XY();
  if ( aD3 < -aTol ) 
    return 2;  // dehors

  // compute 2d distance to triangle
  Standard_Real aD = Min (aD1, Min (aD2, aD3));
  DMin = ( aD < 0 ? -aD : 0. );
  return 0;
}
                             
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Select3D_SensitiveTriangle::Dump(Standard_OStream& S,const Standard_Boolean FullDump) const 
{
  // les generalites....
  
  S<<"\tSensitiveTriangle 3D :\n";
  if(HasLocation())
    S<<"\t\tExisting Location"<<endl;

  S<<"\t\t P0 [ "<<COORD(0,x)<<" , "<<COORD(0,y) <<" , "<<COORD(0,z)<<" ]"<<endl;
  S<<"\t\t P1 [ "<<COORD(1,x)<<" , "<<COORD(1,y) <<" , "<<COORD(1,z)<<" ]"<<endl;
  S<<"\t\t P2 [ "<<COORD(2,x)<<" , "<<COORD(2,y) <<" , "<<COORD(2,z)<<" ]"<<endl;

  if(FullDump){
    S<<"\t\tProjected Points"<<endl;
    
    S<<"\t\t  0.[ "<<COORD2d(0,x)<<" , "<<COORD2d(0,y)<<" ]"<<endl;
    S<<"\t\t  1.[ "<<COORD2d(1,x)<<" , "<<COORD2d(1,y)<<" ]"<<endl;
    S<<"\t\t  2.[ "<<COORD2d(2,x)<<" , "<<COORD2d(2,y)<<" ]"<<endl;
//    S<<"\t\t\tOwner:"<<myOwnerId<<endl;
    Select3D_SensitiveEntity::DumpBox(S,mybox2d);
  
  }
}
//=======================================================================
//function : ComputeDepth
//purpose  : 
//=======================================================================
Standard_Real Select3D_SensitiveTriangle::ComputeDepth(const gp_Lin& EyeLine) const
{
  gp_Pnt P1(((Select3D_Pnt*)mypolyg3d)[0].x, ((Select3D_Pnt*)mypolyg3d)[0].y, ((Select3D_Pnt*)mypolyg3d)[0].z);
  gp_Pnt P2(((Select3D_Pnt*)mypolyg3d)[1].x, ((Select3D_Pnt*)mypolyg3d)[1].y, ((Select3D_Pnt*)mypolyg3d)[1].z);
  gp_Pnt P3(((Select3D_Pnt*)mypolyg3d)[2].x, ((Select3D_Pnt*)mypolyg3d)[2].y, ((Select3D_Pnt*)mypolyg3d)[2].z);

  gp_Trsf TheTrsf ;
  if(HasLocation()) 
    TheTrsf = Location().Transformation();
  
  if(TheTrsf.Form()!=gp_Identity){
    P1.Transform(TheTrsf);
    P2.Transform(TheTrsf);
    P3.Transform(TheTrsf);
  }
  
  Standard_Real prof(Precision::Infinite());
  // formule calcul du parametre du point sur l'intersection
  // t = (P1P2 ^P1P3)* OP1  / ((P1P2^P1P3)*Dir)
  
  gp_Pnt Oye  = EyeLine.Location(); // origine de la ligne oeil/point vise...
  gp_Dir Dir  = EyeLine.Direction();
  
  gp_Vec P1P2 (P1,P2), P1P3(P1,P3);
  P1P2.Normalize();
  P1P3.Normalize();
  
  gp_Vec oP1(Oye,P1);
  Standard_Real val1 = oP1.DotCross(P1P2,P1P3);
  Standard_Real val2 = Dir.DotCross(P1P2,P1P3);
  
  if(Abs(val2)>Precision::Confusion())
    prof =val1/val2;
  
  if (prof==Precision::Infinite()){
    prof= ElCLib::Parameter(EyeLine,P1);
    prof = Min (prof, ElCLib::Parameter(EyeLine,P2));
    prof = Min (prof, ElCLib::Parameter(EyeLine,P3));
  }
  return prof;

}

