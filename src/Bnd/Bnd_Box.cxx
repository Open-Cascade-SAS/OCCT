// Created on: 1991-03-08
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
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

#include <Bnd_Box.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <Standard_ConstructionError.hxx>

// set the flag to one
#define ClearVoidFlag() ( Flags &= ~VoidMask )

#include <Standard_Stream.hxx>
#include <gp.hxx>
// #include <Precision.hxx>
#define Bnd_Precision_Infinite 1e+100

//=======================================================================
//function : Bnd_Box
//purpose  : 
//=======================================================================

Bnd_Box::Bnd_Box()
     : Xmin(0.), Xmax(0.), Ymin(0.), Ymax(0.),  Zmin(0.), Zmax(0.), Gap(0.)
{
  SetVoid();
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void Bnd_Box::Set(const gp_Pnt& P)
{
  SetVoid();
  Add(P);
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void Bnd_Box::Set(const gp_Pnt& P, const gp_Dir& D)
{
  SetVoid();
  Add(P,D);
}


//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void Bnd_Box::Update (const Standard_Real x,
		      const Standard_Real y, 
		      const Standard_Real z, 
		      const Standard_Real X,
		      const Standard_Real Y, 
		      const Standard_Real Z)
{
  if (IsVoid())
  {
    Xmin = x;
    Ymin = y;
    Zmin = z;
    Xmax = X;
    Ymax = Y;
    Zmax = Z;
    ClearVoidFlag();
  }
  else {
    if (!IsOpenXmin() && (x < Xmin)) Xmin = x;
    if (!IsOpenXmax() && (X > Xmax)) Xmax = X;
    if (!IsOpenYmin() && (y < Ymin)) Ymin = y;
    if (!IsOpenYmax() && (Y > Ymax)) Ymax = Y;
    if (!IsOpenZmin() && (z < Zmin)) Zmin = z;
    if (!IsOpenZmax() && (Z > Zmax)) Zmax = Z;
  }
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void Bnd_Box::Update (const Standard_Real X,
		      const Standard_Real Y,
		      const Standard_Real Z)
{
  if (IsVoid())
  {
    Xmin = X;
    Ymin = Y;
    Zmin = Z;
    Xmax = X;
    Ymax = Y;
    Zmax = Z;
    ClearVoidFlag();
  }
  else {
    if      (!IsOpenXmin() && (X < Xmin)) Xmin = X;
    else if (!IsOpenXmax() && (X > Xmax)) Xmax = X;
    if      (!IsOpenYmin() && (Y < Ymin)) Ymin = Y;
    else if (!IsOpenYmax() && (Y > Ymax)) Ymax = Y;
    if      (!IsOpenZmin() && (Z < Zmin)) Zmin = Z;
    else if (!IsOpenZmax() && (Z > Zmax)) Zmax = Z;
  }
}

//=======================================================================
//function : GetGap
//purpose  : 
//=======================================================================

Standard_Real Bnd_Box::GetGap () const
{
  return Gap;
}

//=======================================================================
//function : SetGap
//purpose  : 
//=======================================================================

void Bnd_Box::SetGap (const Standard_Real Tol)
{
  Gap = Tol;
}

//=======================================================================
//function : Enlarge
//purpose  : 
//=======================================================================

void Bnd_Box::Enlarge (const Standard_Real Tol)
{
  Gap=Max(Gap, Abs(Tol));
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

void Bnd_Box::Get (Standard_Real& theXmin,
                   Standard_Real& theYmin,
                   Standard_Real& theZmin,
                   Standard_Real& theXmax,
                   Standard_Real& theYmax,
                   Standard_Real& theZmax) const
{
  if (IsVoid())
  {
    throw Standard_ConstructionError("Bnd_Box is void");
  }

  if (IsOpenXmin()) theXmin = -Bnd_Precision_Infinite;
  else              theXmin = Xmin - Gap;
  if (IsOpenXmax()) theXmax = Bnd_Precision_Infinite;
  else              theXmax = Xmax + Gap;
  if (IsOpenYmin()) theYmin = -Bnd_Precision_Infinite;
  else              theYmin = Ymin - Gap;
  if (IsOpenYmax()) theYmax = Bnd_Precision_Infinite;
  else              theYmax = Ymax + Gap;
  if (IsOpenZmin()) theZmin = -Bnd_Precision_Infinite;
  else              theZmin = Zmin - Gap;
  if (IsOpenZmax()) theZmax = Bnd_Precision_Infinite;
  else              theZmax = Zmax + Gap;
}

//=======================================================================
//function : CornerMin
//purpose  :
//=======================================================================

gp_Pnt Bnd_Box::CornerMin() const
{
  gp_Pnt aCornerMin;
  if (IsVoid())
  {
    throw Standard_ConstructionError("Bnd_Box is void");
  }
  if (IsOpenXmin()) aCornerMin.SetX (-Bnd_Precision_Infinite);
  else              aCornerMin.SetX (Xmin - Gap);
  if (IsOpenYmin()) aCornerMin.SetY (-Bnd_Precision_Infinite);
  else              aCornerMin.SetY (Ymin - Gap);
  if (IsOpenZmin()) aCornerMin.SetZ (-Bnd_Precision_Infinite);
  else              aCornerMin.SetZ (Zmin - Gap);
  return aCornerMin;
}

//=======================================================================
//function : CornerMax
//purpose  :
//=======================================================================

gp_Pnt Bnd_Box::CornerMax() const
{
  gp_Pnt aCornerMax;
  if (IsVoid())
  {
    throw Standard_ConstructionError("Bnd_Box is void");
  }
  if (IsOpenXmax()) aCornerMax.SetX (Bnd_Precision_Infinite);
  else              aCornerMax.SetX (Xmax + Gap);
  if (IsOpenYmin()) aCornerMax.SetY (Bnd_Precision_Infinite);
  else              aCornerMax.SetY (Ymax + Gap);
  if (IsOpenZmin()) aCornerMax.SetZ (Bnd_Precision_Infinite);
  else              aCornerMax.SetZ (Zmax + Gap);
  return aCornerMax;
}

//=======================================================================
//function : IsXThin
//purpose  : 
//=======================================================================

Standard_Boolean Bnd_Box::IsXThin (const Standard_Real tol) const
{
  if (IsWhole())       return Standard_False;
  if (IsVoid())        return Standard_True;
  if (IsOpenXmin())    return Standard_False;
  if (IsOpenXmax())    return Standard_False;
  if (Xmax-Xmin < tol) return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : IsYThin
//purpose  : 
//=======================================================================

Standard_Boolean Bnd_Box::IsYThin (const Standard_Real tol) const
{
  if (IsWhole())       return Standard_False;
  if (IsVoid())        return Standard_True;
  if (IsOpenYmin())    return Standard_False;
  if (IsOpenYmax())    return Standard_False;
  if (Ymax-Ymin < tol) return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : IsZThin
//purpose  : 
//=======================================================================

Standard_Boolean Bnd_Box::IsZThin (const Standard_Real tol) const
{
  if (IsWhole())       return Standard_False;
  if (IsVoid())        return Standard_True;
  if (IsOpenZmin())    return Standard_False;
  if (IsOpenZmax())    return Standard_False;
  if (Zmax-Zmin < tol) return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : IsThin
//purpose  : 
//=======================================================================

Standard_Boolean Bnd_Box::IsThin (const Standard_Real tol) const
{
  if (!IsXThin(tol)) return Standard_False;
  if (!IsYThin(tol)) return Standard_False;
  if (!IsZThin(tol)) return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : Transformed
//purpose  : 
//=======================================================================

Bnd_Box Bnd_Box::Transformed (const gp_Trsf& T) const
{
  gp_TrsfForm F = T.Form();
  Bnd_Box newb(*this);
  if ( IsVoid() ) return newb;

  if      (F == gp_Identity) {}
  else if (F == gp_Translation) {
    Standard_Real DX,DY,DZ;
    (T.TranslationPart()).Coord(DX,DY,DZ);
    if (!IsOpenXmin()) newb.Xmin += DX;
    if (!IsOpenXmax()) newb.Xmax += DX;
    if (!IsOpenYmin()) newb.Ymin += DY;
    if (!IsOpenYmax()) newb.Ymax += DY;
    if (!IsOpenZmin()) newb.Zmin += DZ;
    if (!IsOpenZmax()) newb.Zmax += DZ;
  }
  else {
    gp_Pnt P[8];
    Standard_Boolean Vertex[8];
    Standard_Integer i;
    for (i=0;i<8;i++) Vertex[i] = Standard_True;
    gp_Dir D[6];
//    Standard_Integer vertices = 0;
    Standard_Integer directions = 0;

    if (IsOpenXmin())
    {
      directions++;
      D[directions-1].SetCoord(-1., 0., 0.);
      Vertex[0] = Vertex[2] = Vertex[4] = Vertex[6] = Standard_False;
    }
    if (IsOpenXmax())
    {
      directions++;
      D[directions-1].SetCoord( 1., 0., 0.);
      Vertex[1] = Vertex[3] = Vertex[5] = Vertex[7] = Standard_False;
    }
    if (IsOpenYmin())
    {
      directions++;
      D[directions-1].SetCoord( 0.,-1., 0.);
      Vertex[0] = Vertex[1] = Vertex[4] = Vertex[5] = Standard_False;
    }
    if (IsOpenYmax())
    {
      directions++;
      D[directions-1].SetCoord( 0., 1., 0.);
      Vertex[2] = Vertex[3] = Vertex[6] = Vertex[7] = Standard_False;
    }
    if (IsOpenZmin())
    {
      directions++;
      D[directions-1].SetCoord( 0., 0.,-1.);
      Vertex[0] = Vertex[1] = Vertex[2] = Vertex[3] = Standard_False;
    }
    if (IsOpenZmax())
    {
      directions++;
      D[directions-1].SetCoord( 0., 0., 1.);
      Vertex[4] = Vertex[5] = Vertex[6] = Vertex[7] = Standard_False;
    }

    newb.SetVoid();
    for (i=0;i<directions;i++) {
      D[i].Transform(T);
      newb.Add(D[i]);
    }
    P[0].SetCoord(Xmin,Ymin,Zmin);
    P[1].SetCoord(Xmax,Ymin,Zmin);
    P[2].SetCoord(Xmin,Ymax,Zmin);
    P[3].SetCoord(Xmax,Ymax,Zmin);
    P[4].SetCoord(Xmin,Ymin,Zmax);
    P[5].SetCoord(Xmax,Ymin,Zmax);
    P[6].SetCoord(Xmin,Ymax,Zmax);
    P[7].SetCoord(Xmax,Ymax,Zmax);
    for (i=0;i<8;i++) {
      if (Vertex[i]) {
	P[i].Transform(T);
	newb.Add(P[i]);
      }
    }
    newb.Gap=Gap;
  }
  return newb;
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void Bnd_Box::Add (const Bnd_Box& Other)
{
  if (IsWhole()) return;
  else if (Other.IsVoid()) return; 
  else if (Other.IsWhole()) SetWhole();
  else if (IsVoid()) (*this) = Other;
  else
  {
    if ( ! IsOpenXmin() )
    {
      if (Other.IsOpenXmin()) OpenXmin();
      else if (Xmin > Other.Xmin) Xmin = Other.Xmin;
    }
    if ( ! IsOpenXmax() )
    {
      if (Other.IsOpenXmax()) OpenXmax();
      else if (Xmax < Other.Xmax) Xmax = Other.Xmax;
    }
    if ( ! IsOpenYmin() )
    {
      if (Other.IsOpenYmin()) OpenYmin();
      else if (Ymin > Other.Ymin) Ymin = Other.Ymin;
    }
    if ( ! IsOpenYmax() )
    {
      if (Other.IsOpenYmax()) OpenYmax();
      else if (Ymax < Other.Ymax) Ymax = Other.Ymax;
    }
    if ( ! IsOpenZmin() )
    {
      if (Other.IsOpenZmin()) OpenZmin();
      else if (Zmin > Other.Zmin) Zmin = Other.Zmin;
    }
    if ( ! IsOpenZmax() )
    {
      if (Other.IsOpenZmax()) OpenZmax();
      else if (Zmax < Other.Zmax) Zmax = Other.Zmax;
    }
    Gap = Max (Gap, Other.Gap);
  }
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void Bnd_Box::Add (const gp_Pnt& P)
{
  Standard_Real X,Y,Z;
  P.Coord(X,Y,Z);
  Update(X,Y,Z);
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void Bnd_Box::Add (const gp_Pnt& P, const gp_Dir& D)
{
  Add(P);
  Add(D);
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void Bnd_Box::Add (const gp_Dir& D)
{
  Standard_Real DX,DY,DZ;
  D.Coord(DX,DY,DZ);

  if (DX < -RealEpsilon()) 
    OpenXmin();
  else if (DX > RealEpsilon()) 
    OpenXmax();

  if (DY < -RealEpsilon())
    OpenYmin();
  else if (DY > RealEpsilon())
    OpenYmax();

  if (DZ < -RealEpsilon())
    OpenZmin();
  else if (DZ > RealEpsilon())
    OpenZmax();
}

//=======================================================================
//function : IsOut
//purpose  : 
//=======================================================================

Standard_Boolean Bnd_Box::IsOut (const gp_Pnt& P) const
{
  if        (IsWhole())  return Standard_False;
  else if   (IsVoid())   return Standard_True;
  else {
    Standard_Real X,Y,Z;
    P.Coord(X,Y,Z);
    if      (!IsOpenXmin() && (X < (Xmin-Gap))) return Standard_True;
    else if (!IsOpenXmax() && (X > (Xmax+Gap))) return Standard_True;
    else if (!IsOpenYmin() && (Y < (Ymin-Gap))) return Standard_True;
    else if (!IsOpenYmax() && (Y > (Ymax+Gap))) return Standard_True;
    else if (!IsOpenZmin() && (Z < (Zmin-Gap))) return Standard_True;
    else if (!IsOpenZmax() && (Z > (Zmax+Gap))) return Standard_True;
    else return Standard_False;
  }
}


//=======================================================================
//function : IsOut
//purpose  : 
//=======================================================================

Standard_Boolean Bnd_Box::IsOut (const gp_Pln& P) const
{
  if        (IsWhole())  return Standard_False;
  else if   (IsVoid())   return Standard_True;
  else {
    Standard_Real A,B,C,D;
    P.Coefficients (A, B ,C ,D);
    Standard_Real d = A * (Xmin-Gap) + B * (Ymin-Gap) + C * (Zmin-Gap) + D;
//    Standard_Boolean plus = d > 0;
    Standard_Integer plus = d > 0;
    if (plus != ((A*(Xmin-Gap) + B*(Ymin-Gap) + C*(Zmax+Gap) + D) > 0))
      return Standard_False;
    if (plus != ((A*(Xmin-Gap) + B*(Ymax+Gap) + C*(Zmin-Gap) + D) > 0))
      return Standard_False;
    if (plus != ((A*(Xmin-Gap) + B*(Ymax+Gap) + C*(Zmax+Gap) + D) > 0))
      return Standard_False;
    if (plus != ((A*(Xmax+Gap) + B*(Ymin-Gap) + C*(Zmin-Gap) + D) > 0))
      return Standard_False;
    if (plus != ((A*(Xmax+Gap) + B*(Ymin-Gap) + C*(Zmax+Gap) + D) > 0))
      return Standard_False;
    if (plus != ((A*(Xmax+Gap) + B*(Ymax+Gap) + C*(Zmin-Gap) + D) > 0))
      return Standard_False;
    if (plus != ((A*(Xmax+Gap) + B*(Ymax+Gap) + C*(Zmax+Gap) + D) > 0))
      return Standard_False;
    else return Standard_True;
  }
}

//=======================================================================
//function : IsOut
//purpose  : 
//=======================================================================

Standard_Boolean Bnd_Box::IsOut (const gp_Lin& L) const
{
  if        (IsWhole())  return Standard_False;
  else if   (IsVoid())   return Standard_True;
  else {
    Standard_Real xmin = 0, xmax = 0, ymin = 0, ymax = 0, zmin, zmax;
    Standard_Real parmin, parmax, par1, par2;
    Standard_Boolean xToSet, yToSet;
    Standard_Real myXmin, myYmin, myZmin, myXmax, myYmax, myZmax;
    Get (myXmin, myYmin, myZmin, myXmax, myYmax, myZmax);

    if (Abs(L.Direction().XYZ().X())>0.) {
      par1=(myXmin-L.Location().XYZ().X())/L.Direction().XYZ().X();
      par2=(myXmax-L.Location().XYZ().X())/L.Direction().XYZ().X();
      parmin=Min(par1, par2);
      parmax=Max(par1, par2);
      xToSet=Standard_True;
    }
    else {
      if (L.Location().XYZ().X()<myXmin || myXmax<L.Location().XYZ().X()) {
	return Standard_True;
      }
      xmin=L.Location().XYZ().X();
      xmax=L.Location().XYZ().X();
      parmin=-Bnd_Precision_Infinite;
      parmax=Bnd_Precision_Infinite;
      xToSet=Standard_False;
    }

    if (Abs(L.Direction().XYZ().Y())>0.) {
      par1=(myYmin-L.Location().XYZ().Y())/L.Direction().XYZ().Y();
      par2=(myYmax-L.Location().XYZ().Y())/L.Direction().XYZ().Y();
      //=================DET change 06/03/01====================
      if(parmax < Min(par1,par2) || parmin > Max(par1,par2))
	return Standard_True;
      //========================================================
      parmin=Max(parmin, Min(par1,par2));
      parmax=Min(parmax, Max(par1,par2));
      yToSet=Standard_True;
    }
    else {
      if (L.Location().XYZ().Y()<myYmin || myYmax<L.Location().XYZ().Y()) {
	return Standard_True;
      }
      ymin=L.Location().XYZ().Y();
      ymax=L.Location().XYZ().Y();
      yToSet=Standard_False;
    }

    if (Abs(L.Direction().XYZ().Z())>0.) {
      par1=(myZmin-L.Location().XYZ().Z())/L.Direction().XYZ().Z();
      par2=(myZmax-L.Location().XYZ().Z())/L.Direction().XYZ().Z();
      //=================DET change 06/03/01====================
      if(parmax < Min(par1,par2) || parmin > Max(par1,par2))
	return Standard_True;
      //========================================================
      parmin=Max(parmin, Min(par1,par2));
      parmax=Min(parmax, Max(par1,par2));
      par1=L.Location().XYZ().Z()+parmin*L.Direction().XYZ().Z();
      par2=L.Location().XYZ().Z()+parmax*L.Direction().XYZ().Z();
      zmin=Min(par1, par2);
      zmax=Max(par1, par2);
    }
    else {
      if (L.Location().XYZ().Z()<myZmin || myZmax<L.Location().XYZ().Z())
	return Standard_True;
      zmin=L.Location().XYZ().Z();
      zmax=L.Location().XYZ().Z();
    }
    if (zmax<myZmin || myZmax<zmin) return Standard_True;

    if (xToSet) {
      par1=L.Location().XYZ().X()+parmin*L.Direction().XYZ().X();
      par2=L.Location().XYZ().X()+parmax*L.Direction().XYZ().X();
      xmin=Min(par1, par2);
      xmax=Max(par1, par2);
    }
    if (xmax<myXmin || myXmax<xmin) return Standard_True;

    if (yToSet) {
      par1=L.Location().XYZ().Y()+parmin*L.Direction().XYZ().Y();
      par2=L.Location().XYZ().Y()+parmax*L.Direction().XYZ().Y();
      ymin=Min(par1, par2);
      ymax=Max(par1, par2);
    }
    if (ymax<myYmin || myYmax<ymin) return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : IsOut
//purpose  : 
//=======================================================================

Standard_Boolean Bnd_Box::IsOut (const Bnd_Box& Other) const
{ 
  //modified by NIZNHY-PKV Fri Jul 08 11:03:43 2011f
  if (!Flags && !Other.Flags) {
    Standard_Boolean bRet;
    Standard_Real delta;
    //
    delta = Other.Gap + Gap;
    bRet=((Xmin - Other.Xmax > delta) ||
	  (Other.Xmin - Xmax > delta) ||
	  (Ymin - Other.Ymax > delta) ||
	  (Other.Ymin - Ymax > delta) ||
	  (Zmin - Other.Zmax > delta) ||
	  (Other.Zmin - Zmax > delta));
    return bRet;
  }
  //modified by NIZNHY-PKV Fri Jul 08 11:03:46 2011t
  if   (IsVoid())         return Standard_True;
  if   (Other.IsVoid())   return Standard_True;
  if   (IsWhole())        return Standard_False;
  if   (Other.IsWhole())  return Standard_False;

  Standard_Real delta = Other.Gap + Gap;

  if (!IsOpenXmin() && !Other.IsOpenXmax())
    if (Xmin - Other.Xmax > delta) return Standard_True;
  if (!IsOpenXmax() && !Other.IsOpenXmin())
    if (Other.Xmin - Xmax > delta) return Standard_True;
  
  if (!IsOpenYmin() && !Other.IsOpenYmax())
    if (Ymin - Other.Ymax > delta) return Standard_True;
  if (!IsOpenYmax() && !Other.IsOpenYmin())
    if (Other.Ymin - Ymax > delta) return Standard_True;
  
  if (!IsOpenZmin() && !Other.IsOpenZmax())
    if (Zmin - Other.Zmax > delta) return Standard_True;
  if (!IsOpenZmax() && !Other.IsOpenZmin())
    if (Other.Zmin - Zmax > delta) return Standard_True;

  return Standard_False;
}

//=======================================================================
//function : IsOut
//purpose  : 
//=======================================================================

Standard_Boolean Bnd_Box::IsOut (const Bnd_Box& Other, 
				 const gp_Trsf& T) const
{
  return IsOut(Other.Transformed(T));
}

//=======================================================================
//function : IsOut
//purpose  : 
//=======================================================================

Standard_Boolean Bnd_Box::IsOut (const gp_Trsf& T1,
				 const Bnd_Box& Other, 
				 const gp_Trsf& T2) const
{
  return Transformed(T1).IsOut(Other.Transformed(T2));
}


//=======================================================================
//function : IsSegmentOut
//purpose  : 
//=======================================================================

static Standard_Boolean IsSegmentOut(Standard_Real x1,Standard_Real y1,
                                     Standard_Real x2,Standard_Real y2,
                                     Standard_Real xs1,Standard_Real ys1,
                                     Standard_Real xs2,Standard_Real ys2)
{
  Standard_Real eps = RealSmall();
  Standard_Real xsmin = Min (xs1, xs2);
  Standard_Real xsmax = Max (xs1, xs2);
  Standard_Real ysmin = Min (ys1, ys2);
  Standard_Real ysmax = Max (ys1, ys2);

  if (ysmax-ysmin < eps && (y1-ys1 < eps && ys1-y2 < eps) &&
      ((xsmin-x1 < eps && x1-xsmax < eps) || 
       (xsmin-x2 < eps && x2-xsmax < eps) || 
       (x1-xs1 < eps && xs1-x2 < eps)))
    return Standard_False;
  if (xsmax-xsmin < eps && (x1-xs1 < eps && xs1-x2 < eps) &&
      ((ysmin-y1 < eps && y1-ysmax < eps) ||
       (ysmin-y2 < eps && y2-ysmax < eps) ||
       (y1-ys1 < eps && ys1-y2 < eps)))
    return Standard_False;

  if ((xs1 < x1 && xs2 < x1) || (xs1 > x2 && xs2 > x2) ||
      (ys1 < y1 && ys2 < y1) || (ys1 > y2 && ys2 > y2) )
    return Standard_True;

  if (Abs(xs2-xs1) > eps)
  {
    Standard_Real ya = ( Min(x1, x2) - xs1 ) * ( ys2 - ys1 ) / ( xs2 - xs1 ) + ys1;
    Standard_Real yb = ( Max(x1, x2) - xs1 ) * ( ys2 - ys1 ) / ( xs2 - xs1 ) + ys1;
    if ( (ya < y1 && yb < y1) || (ya > y2 && yb > y2) ) return Standard_True;
  }
  else if (Abs(ys2-ys1) > eps)
  {
    Standard_Real xa = ( Min(y1, y2) - ys1 ) * ( xs2 - xs1 ) / ( ys2 - ys1 ) + xs1;
    Standard_Real xb = ( Max(y1, y2) - ys1 ) * ( xs2 - xs1 ) / ( ys2 - ys1 ) + xs1;
    if ( (xa < x1 && xb < x1) || (xa > x2 && xb > x2) ) return Standard_True;
  }
  else 
    return Standard_True;

  return Standard_False;
}

Standard_Boolean Bnd_Box::IsOut(const gp_Pnt& P1, const gp_Pnt& P2, const gp_Dir& D) const
{

  if        (IsWhole())  return Standard_False;
  else if   (IsVoid())   return Standard_True;

  Standard_Real eps = RealSmall();
  Standard_Real myXmin, myYmin, myZmin, myXmax, myYmax, myZmax;
  Get (myXmin, myYmin, myZmin, myXmax, myYmax, myZmax);

  if(Abs(D.X()) < eps && Abs(D.Y()) < eps)
    return IsSegmentOut(myXmin, myYmin, myXmax, myYmax, P1.X(), P1.Y(), P2.X(), P2.Y());

  if(Abs(D.X()) < eps && Abs(D.Z()) < eps)
    return IsSegmentOut(myXmin, myZmin, myXmax, myZmax, P1.X(), P1.Z(), P2.X(), P2.Z());

  if(Abs(D.Y()) < eps && Abs(D.Z()) < eps)
    return IsSegmentOut(myYmin, myZmin, myYmax, myZmax, P1.Y(), P1.Z(), P2.Y(), P2.Z());

  if(Abs(D.X()) < eps)
  {
    if(!IsSegmentOut(myXmin, myZmin, myXmax, myZmax,
                     P1.X(),(myYmin-P1.Y())*D.Z()/D.Y()+P1.Z(), 
                     P2.X(),(myYmin-P2.Y())*D.Z()/D.Y()+P2.Z()))
      return Standard_False;

    if(!IsSegmentOut(myXmin, myZmin, myXmax, myZmax,
                     P1.X(),(myYmax-P1.Y())*D.Z()/D.Y()+P1.Z(), 
                     P2.X(),(myYmax-P2.Y())*D.Z()/D.Y()+P2.Z()))
      return Standard_False;

    if(!IsSegmentOut(myXmin, myYmin, myXmax, myYmax,
                     P1.X(),(myZmin-P1.Z())*D.Y()/D.Z()+P1.Y(), 
                     P2.X(),(myZmin-P2.Z())*D.Y()/D.Z()+P2.Y()))
      return Standard_False;

    if(!IsSegmentOut(myXmin, myYmin, myXmax, myYmax,
                     P1.X(),(myZmax-P1.Z())*D.Y()/D.Z()+P1.Y(), 
                     P2.X(),(myZmax-P2.Z())*D.Y()/D.Z()+P2.Y()))
      return Standard_False;

    return Standard_True;
  }//if(D.X() == 0) 

  if(Abs(D.Y()) < eps)
  {
    if(!IsSegmentOut(myYmin, myZmin, myYmax, myZmax,
                     P1.Y(),(myXmin-P1.X())*D.Z()/D.X()+P1.Z(), 
                     P2.Y(),(myXmin-P2.X())*D.Z()/D.X()+P2.Z()))
      return Standard_False;

    if(!IsSegmentOut(myYmin, myZmin, myYmax, myZmax,
                     P1.Y(),(myXmax-P1.X())*D.Z()/D.X()+P1.Z(), 
                     P2.Y(),(myXmax-P2.X())*D.Z()/D.X()+P2.Z()))
      return Standard_False;

    if(!IsSegmentOut(myYmin, myXmin, myYmax, myXmax,
                     P1.Y(),(myZmin-P1.Z())*D.X()/D.Z()+P1.X(), 
                     P2.Y(),(myZmin-P2.Z())*D.X()/D.Z()+P2.X()))
      return Standard_False;

    if(!IsSegmentOut(myYmin, myXmin, myYmax, myXmax,
                     P1.Y(),(myZmax-P1.Z())*D.X()/D.Z()+P1.X(), 
                     P2.Y(),(myZmax-P2.Z())*D.X()/D.Z()+P2.X()))
      return Standard_False;

    return Standard_True;
  }//if(D.Y() == 0) 

  if(Abs(D.Z()) < eps)
  {
    if(!IsSegmentOut(myZmin, myXmin, myZmax, myXmax,
                     P1.Z(),(myYmax-P1.Y())*D.X()/D.Y()+P1.X(), 
                     P2.Z(),(myYmax-P2.Y())*D.X()/D.Y()+P2.X()))
      return Standard_False;

    if(!IsSegmentOut(myZmin, myXmin, myZmax, myXmax,
                     P1.Z(),(myYmin-P1.Y())*D.X()/D.Y()+P1.X(), 
                     P2.Z(),(myYmin-P2.Y())*D.X()/D.Y()+P2.X()))
      return Standard_False;

    if(!IsSegmentOut(myZmin, myYmin, myZmax, myYmax,
                     P1.Z(),(myXmax-P1.X())*D.Y()/D.X()+P1.Y(), 
                     P2.Z(),(myXmax-P2.X())*D.Y()/D.X()+P2.Y()))
      return Standard_False;

    if(!IsSegmentOut(myZmin, myYmin, myZmax, myYmax,
                     P1.Z(),(myXmin-P1.X())*D.Y()/D.X()+P1.Y(), 
                     P2.Z(),(myXmin-P2.X())*D.Y()/D.X()+P2.Y()))
      return Standard_False;

    return Standard_True;
  }//if(D.Z() == 0) 

  if(!IsSegmentOut(myXmin,myZmin,myXmax,myZmax,
                   (myYmin - P1.Y())/D.Y()*D.X() + P1.X(),
                   (myYmin - P1.Y())/D.Y()*D.Z() + P1.Z(),
                   (myYmin - P2.Y())/D.Y()*D.X() + P2.X(),
                   (myYmin - P2.Y())/D.Y()*D.Z() + P2.Z()))
    return Standard_False;

  if(!IsSegmentOut(myXmin,myZmin,myXmax,myZmax,
                   (myYmax - P1.Y())/D.Y()*D.X() + P1.X(),
                   (myYmax - P1.Y())/D.Y()*D.Z() + P1.Z(),
                   (myYmax - P2.Y())/D.Y()*D.X() + P2.X(),
                   (myYmax - P2.Y())/D.Y()*D.Z() + P2.Z()))
    return Standard_False;

  if(!IsSegmentOut(myXmin,myYmin,myXmax,myYmax,
                   (myZmin - P1.Z())/D.Z()*D.X() + P1.X(),
                   (myZmin - P1.Z())/D.Z()*D.Y() + P1.Y(),
                   (myZmin - P2.Z())/D.Z()*D.X() + P2.X(),
                   (myZmin - P2.Z())/D.Z()*D.Y() + P2.Y()))
    return Standard_False;

  if(!IsSegmentOut(myXmin,myYmin,myXmax,myYmax,
                   (myZmax - P1.Z())/D.Z()*D.X() + P1.X(),
                   (myZmax - P1.Z())/D.Z()*D.Y() + P1.Y(),
                   (myZmax - P2.Z())/D.Z()*D.X() + P2.X(),
                   (myZmax - P2.Z())/D.Z()*D.Y() + P2.Y()))
    return Standard_False;

  if(!IsSegmentOut(myZmin,myYmin,myZmax,myYmax,
                   (myXmin - P1.X())/D.X()*D.Z() + P1.Z(),
                   (myXmin - P1.X())/D.X()*D.Y() + P1.Y(),
                   (myXmin - P2.X())/D.X()*D.Z() + P2.Z(),
                   (myXmin - P2.X())/D.X()*D.Y() + P2.Y()))
    return Standard_False;

  if(!IsSegmentOut(myZmin,myYmin,myZmax,myYmax,
                   (myXmax - P1.X())/D.X()*D.Z() + P1.Z(),
                   (myXmax - P1.X())/D.X()*D.Y() + P1.Y(),
                   (myXmax - P2.X())/D.X()*D.Z() + P2.Z(),
                   (myXmax - P2.X())/D.X()*D.Y() + P2.Y()))
    return Standard_False;

  return Standard_True; 

}

//=======================================================================
//function : Distance
//purpose  : computes the minimum distance between two boxes 
//=======================================================================

static Standard_Real DistMini2Box( const Standard_Real r1min, const  Standard_Real r1max,  const Standard_Real r2min, const  Standard_Real r2max)
{  Standard_Real   r1, r2;

   r1 = Square(r1min - r2max);
   r2 = Square(r1max - r2min);
   return (Min( r1, r2 ));   
}



Standard_Real Bnd_Box::Distance(const Bnd_Box& Other) const 
{  Standard_Real   xminB1, yminB1, zminB1, xmaxB1, ymaxB1, zmaxB1;
   Standard_Real   xminB2, yminB2, zminB2, xmaxB2, ymaxB2, zmaxB2;
   Standard_Real   dist_x, dist_y, dist_z, dist_t;

   Get( xminB1, yminB1, zminB1, xmaxB1, ymaxB1, zmaxB1);
   Other.Get( xminB2, yminB2, zminB2, xmaxB2, ymaxB2, zmaxB2);

   if ( ((xminB1<= xminB2)&&( xminB2 <= xmaxB1)) || ((xminB2<= xminB1)&&( xminB1 <= xmaxB2)) )
      { dist_x=0; }
      else { dist_x= DistMini2Box(xminB1, xmaxB1, xminB2, xmaxB2);}
   if ( ((yminB1<= yminB2)&&( yminB2 <= ymaxB1)) || ((yminB2<= yminB1)&&( yminB1 <= ymaxB2)) )
      { dist_y=0; }
      else { dist_y= DistMini2Box(yminB1, ymaxB1, yminB2, ymaxB2);}  
   if ( ((zminB1<= zminB2)&&( zminB2 <= zmaxB1)) || ((zminB2<= zminB1)&&( zminB1 <= zmaxB2)) )
      { dist_z=0; }
      else { dist_z= DistMini2Box(zminB1, zmaxB1, zminB2, zmaxB2);}
   dist_t = dist_x+ dist_y+ dist_z;
   return( Sqrt ( dist_t));
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Bnd_Box::Dump () const
{
  cout << "Box3D : ";
  if      (IsVoid())  cout << "Void";
  else if (IsWhole()) cout << "Whole";
  else {
    cout << "\n Xmin : ";
    if (IsOpenXmin()) cout << "Infinite";
    else              cout << Xmin;
    cout << "\n Xmax : ";
    if (IsOpenXmax()) cout << "Infinite";
    else              cout << Xmax;
    cout << "\n Ymin : ";
    if (IsOpenYmin()) cout << "Infinite";
    else              cout << Ymin;
    cout << "\n Ymax : ";
    if (IsOpenYmax()) cout << "Infinite";
    else              cout << Ymax;
    cout << "\n Zmin : ";
    if (IsOpenZmin()) cout << "Infinite";
    else              cout << Zmin;
    cout << "\n Zmax : ";
    if (IsOpenZmax()) cout << "Infinite";
    else              cout << Zmax;
  }
  cout << "\n Gap : " << Gap;
  cout << "\n";
}
