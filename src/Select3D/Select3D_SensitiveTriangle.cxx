// Created on: 1997-05-14
// Created by: Robert COUBLANC
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
// Function: Creation
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
  mypolyg.SetPnt(0, P0);
  mypolyg.SetPnt(1, P1);
  mypolyg.SetPnt(2, P2);
}

//==================================================
// Function: Matches
// Purpose :
//==================================================

Standard_Boolean Select3D_SensitiveTriangle::Matches (const SelectBasics_PickArgs& thePickArgs,
                                                      Standard_Real& theMatchDMin,
                                                      Standard_Real& theMatchDepth)
{
  Standard_Real aDepth = ComputeDepth (thePickArgs.PickLine());
  if (thePickArgs.IsClipped (aDepth))
  {
    return Standard_False;
  }

  theMatchDepth = aDepth;

  if (Bnd_Box2d (mybox2d).IsOut (gp_Pnt2d (thePickArgs.X(), thePickArgs.Y())))
  {
    return Standard_False;
  }

  Standard_Integer Res;
  switch (mytype)
  {
  case Select3D_TOS_BOUNDARY:
    Res = Status (thePickArgs.X(), thePickArgs.Y(), thePickArgs.Tolerance(), theMatchDMin);
    return Res== 1;
    break;
  case Select3D_TOS_INTERIOR:
    Res = Status (thePickArgs.X(), thePickArgs.Y(), thePickArgs.Tolerance(), theMatchDMin);
    return (Res==0 || Res == 1);
  default:
    break;
  }

  return Standard_True;
}

//==================================================
// Function: Matches
// Purpose :
//==================================================

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
  for(Standard_Integer anIndex=0;anIndex<=2;++anIndex)
  {
    if(B.IsOut(mypolyg.Pnt2d(anIndex)))
      return Standard_False;
  }
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
  CSLib_Class2d aClassifier2d(aPoly,aTol,aTol,Umin,Vmin,Umax,Vmax);

  for(Standard_Integer anIndex=0;anIndex<=2;++anIndex)
  {
    Standard_Integer RES = aClassifier2d.SiDans(mypolyg.Pnt2d(anIndex));
    if(RES!=1)
      return Standard_False;
  }
  return Standard_True;
}

//==================================================
// Function: Points3D
// Purpose :
//==================================================

void Select3D_SensitiveTriangle::Points3D(gp_Pnt& P0,gp_Pnt& P1,gp_Pnt& P2) const
{
  P0 = mypolyg.Pnt(0);
  P1 = mypolyg.Pnt(1);
  P2 = mypolyg.Pnt(2);
}

//==================================================
// Function: Center3D
// Purpose :
//==================================================

gp_Pnt Select3D_SensitiveTriangle::Center3D() const
{
  gp_XYZ aPnt1, aPnt2, aPnt3;
  aPnt1 = mypolyg.Pnt(0);
  aPnt2 = mypolyg.Pnt(1);
  aPnt3 = mypolyg.Pnt(2);
  return gp_Pnt((aPnt1+aPnt2+aPnt3)/3.);
}

//==================================================
// Function: Center2D
// Purpose :
//==================================================

gp_XY Select3D_SensitiveTriangle::Center2D() const
{
  gp_XY aPnt1, aPnt2, aPnt3;
  aPnt1 = mypolyg.Pnt2d(0);
  aPnt2 = mypolyg.Pnt2d(1);
  aPnt3 = mypolyg.Pnt2d(2);
  return (aPnt1+aPnt2+aPnt3)/3.;
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
  return Status(mypolyg.Pnt2d(0), mypolyg.Pnt2d(1), mypolyg.Pnt2d(2),
                gp_XY(X,Y), aTol, DMin);
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

  // the point is classified corresponding to demi-spaces limited
  // by each side of the triangle (with tolerance)
  gp_XY V01(p1);V01-=p0;
  gp_XY V02(p2);V02-=p0;
  gp_XY V12(p2);V12-=p1;

  // check these particular cases...
  // if one of vectors is almost null (2 points are mixed),
  // leave at once (it is already in the bounding box, which is good...)

  DMin = aTol;

  if ( V01.SquareModulus() <= gp::Resolution() )
  {
    Standard_Real LV = V02.SquareModulus();
    if ( LV <= gp::Resolution())
      return 0; // 3 points are mixed, and TheP is in the bounding box B...

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

  // oriented normal to p0p1...
  gp_Dir2d N (-V01.Y(), V01.X());
  Standard_Boolean Neg = (N * V02 < 0.);
  if ( Neg )
    N.Reverse();

  gp_XY Vec(TheP);
  Vec -= p0;

  Standard_Real aD1 = Vec * N.XY();
  if ( aD1 < -aTol )
    return 2;

  // oriented normal to p1p2...
  if(Neg)
    N.SetCoord(p2.Y()-p1.Y(),p1.X()-p2.X());
  else
    N.SetCoord(p1.Y()-p2.Y(),p2.X()-p1.X());

  Vec.SetCoord(TheP.X()-p1.X(),TheP.Y()-p1.Y());
  Standard_Real aD2 = Vec * N.XY();
  if ( aD2 < -aTol )
    return 2;   // outside

  // oriented normal to p2p0...
  // attention v20 (x0-x2)    =>   N y2-y0   => -N  y0-y2
  //               (y0-y2)           x0-x2          x2-x0
  if(Neg)
    N.SetCoord(p0.Y()-p2.Y(),p2.X()-p0.X());
  else
    N.SetCoord(p2.Y()-p0.Y(),p0.X()-p2.X());

  Vec.SetCoord(TheP.X()-p2.X(),TheP.Y()-p2.Y());
  Standard_Real aD3 = Vec * N.XY();
  if ( aD3 < -aTol )
    return 2;  // outside

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
  // general information....

  S<<"\tSensitiveTriangle 3D :\n";
  if(HasLocation())
    S<<"\t\tExisting Location"<<endl;

  gp_Pnt aPnt1, aPnt2, aPnt3;
  aPnt1 = mypolyg.Pnt(0);
  aPnt2 = mypolyg.Pnt(1);
  aPnt3 = mypolyg.Pnt(2);
  S<<"\t\t P0 [ "<<aPnt1.X()<<" , "<<aPnt1.Y()<<" , "<<aPnt1.Z()<<" ]"<<endl;
  S<<"\t\t P1 [ "<<aPnt2.X()<<" , "<<aPnt2.Y()<<" , "<<aPnt2.Z()<<" ]"<<endl;
  S<<"\t\t P2 [ "<<aPnt3.X()<<" , "<<aPnt3.Y()<<" , "<<aPnt3.Z()<<" ]"<<endl;

  if(FullDump)
  {
    S<<"\t\tProjected Points"<<endl;

    gp_Pnt2d aPnt1, aPnt2, aPnt3;
    aPnt1 = mypolyg.Pnt2d(0);
    aPnt2 = mypolyg.Pnt2d(1);
    aPnt3 = mypolyg.Pnt2d(2);
    S<<"\t\t  0.[ "<<aPnt1.X()<<" , "<<aPnt1.Y()<<" ]"<<endl;
    S<<"\t\t  1.[ "<<aPnt2.X()<<" , "<<aPnt2.Y()<<" ]"<<endl;
    S<<"\t\t  2.[ "<<aPnt3.X()<<" , "<<aPnt3.Y()<<" ]"<<endl;
    Select3D_SensitiveEntity::DumpBox(S,mybox2d);
  }
}

//=======================================================================
//function : ComputeDepth
//purpose  :
//=======================================================================

Standard_Real Select3D_SensitiveTriangle::ComputeDepth(const gp_Lin& EyeLine) const
{
  Standard_Real prof(Precision::Infinite());

  gp_Pnt P1, P2, P3;
  P1 = mypolyg.Pnt(0);
  P2 = mypolyg.Pnt(1);
  P3 = mypolyg.Pnt(2);

  gp_Trsf TheTrsf ;
  if(HasLocation())
    TheTrsf = Location().Transformation();

  if(TheTrsf.Form()!=gp_Identity)
  {
    P1.Transform(TheTrsf);
    P2.Transform(TheTrsf);
    P3.Transform(TheTrsf);
  }

  // formula calculation of the point parameters on intersection
  // t = (P1P2 ^P1P3)* OP1  / ((P1P2^P1P3)*Dir)

  gp_Pnt Oye  = EyeLine.Location(); // origin of the target line eye/point...
  gp_Dir Dir  = EyeLine.Direction();

  gp_Vec P1P2 (P1,P2), P1P3(P1,P3);
  P1P2.Normalize();
  P1P3.Normalize();

  gp_Vec oP1(Oye,P1);
  Standard_Real val1 = oP1.DotCross(P1P2,P1P3);
  Standard_Real val2 = Dir.DotCross(P1P2,P1P3);

  if(Abs(val2)>Precision::Confusion())
    prof =val1/val2;

  if (prof==Precision::Infinite())
  {
    prof= ElCLib::Parameter(EyeLine,P1);
    prof = Min (prof, ElCLib::Parameter(EyeLine,P2));
    prof = Min (prof, ElCLib::Parameter(EyeLine,P3));
  }
  return prof;
}

//==================================================
// Function: GetConnected
// Purpose :
//==================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveTriangle::
GetConnected(const TopLoc_Location &theLocation)
{
  // Create a copy of this
  Handle(Select3D_SensitiveEntity) aNewEntity =
    new Select3D_SensitiveTriangle(myOwnerId, mypolyg.Pnt(0), mypolyg.Pnt(1), mypolyg.Pnt(2), mytype);

  if (HasLocation())
    aNewEntity->SetLocation(Location());

  aNewEntity->UpdateLocation(theLocation);

  return aNewEntity;
}
