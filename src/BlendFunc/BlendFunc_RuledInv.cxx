// Created on: 1993-12-02
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
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


#include <BlendFunc_RuledInv.ixx>

#include <Precision.hxx>

BlendFunc_RuledInv::BlendFunc_RuledInv(const Handle(Adaptor3d_HSurface)& S1,
                                       const Handle(Adaptor3d_HSurface)& S2,
                                       const Handle(Adaptor3d_HCurve)& C) :
       surf1(S1),surf2(S2),curv(C)
{}

void BlendFunc_RuledInv::Set(const Standard_Boolean OnFirst,
                             const Handle(Adaptor2d_HCurve2d)& C)
{
  first = OnFirst;
  csurf = C;
}

Standard_Integer BlendFunc_RuledInv::NbEquations () const
{
  return 4;
}

void BlendFunc_RuledInv::GetTolerance(math_Vector& Tolerance,
                                      const Standard_Real Tol) const
{
  Tolerance(1) = csurf->Resolution(Tol);
  Tolerance(2) = curv->Resolution(Tol);
  if (first) {
    Tolerance(3) = surf2->UResolution(Tol);
    Tolerance(4) = surf2->VResolution(Tol);
  }
  else {
    Tolerance(3) = surf1->UResolution(Tol);
    Tolerance(4) = surf1->VResolution(Tol);
  }
}

void BlendFunc_RuledInv::GetBounds(math_Vector& InfBound,
                                   math_Vector& SupBound) const
{
  InfBound(1) = csurf->FirstParameter();
  InfBound(2) = curv->FirstParameter();
  SupBound(1) = csurf->LastParameter();
  SupBound(2) = curv->LastParameter();

  if (first) {
    InfBound(3) = surf2->FirstUParameter();
    InfBound(4) = surf2->FirstVParameter();
    SupBound(3) = surf2->LastUParameter();
    SupBound(4) = surf2->LastVParameter();
    if(!Precision::IsInfinite(InfBound(3)) &&
       !Precision::IsInfinite(SupBound(3))) {
      const Standard_Real range = (SupBound(3) - InfBound(3));
      InfBound(3) -= range;
      SupBound(3) += range;
    }
    if(!Precision::IsInfinite(InfBound(4)) &&
       !Precision::IsInfinite(SupBound(4))) {
      const Standard_Real range = (SupBound(4) - InfBound(4));
      InfBound(4) -= range;
      SupBound(4) += range;
    }
  }
  else {
    InfBound(3) = surf1->FirstUParameter();
    InfBound(4) = surf1->FirstVParameter();
    SupBound(3) = surf1->LastUParameter();
    SupBound(4) = surf1->LastVParameter();
    if(!Precision::IsInfinite(InfBound(3)) &&
       !Precision::IsInfinite(SupBound(3))) {
      const Standard_Real range = (SupBound(3) - InfBound(3));
      InfBound(3) -= range;
      SupBound(3) += range;
    }
    if(!Precision::IsInfinite(InfBound(4)) &&
       !Precision::IsInfinite(SupBound(4))) {
      const Standard_Real range = (SupBound(4) - InfBound(4));
      InfBound(4) -= range;
      SupBound(4) += range;
    }
  }    
}

Standard_Boolean BlendFunc_RuledInv::IsSolution(const math_Vector& Sol,
                                                const Standard_Real Tol)
{
  math_Vector valsol(1,4);
  Value(Sol,valsol);
  if (Abs(valsol(1)) <= Tol &&
      Abs(valsol(2)) <= Tol &&
      Abs(valsol(3)) <= Tol &&
      Abs(valsol(4)) <= Tol)
    return Standard_True;

  return Standard_False;
}


Standard_Boolean BlendFunc_RuledInv::Value(const math_Vector& X,
                                           math_Vector& F)
{
  gp_Pnt ptcur;
  gp_Vec d1cur;
  curv->D1(X(2),ptcur,d1cur);

  const gp_XYZ nplan = d1cur.Normalized().XYZ();
  const Standard_Real theD = -(nplan.Dot(ptcur.XYZ()));
  const gp_Pnt2d pt2d(csurf->Value(X(1)));

  gp_Pnt pts1,pts2;
  gp_Vec d1u1,d1v1,d1u2,d1v2;
  if (first)
  {
    surf1->D1(pt2d.X(),pt2d.Y(),pts1,d1u1,d1v1);
    surf2->D1(X(3),X(4),pts2,d1u2,d1v2);
  }
  else
  {
    surf1->D1(X(3),X(4),pts1,d1u1,d1v1);
    surf2->D1(pt2d.X(),pt2d.Y(),pts2,d1u2,d1v2);
  }

  const gp_XYZ temp(pts2.XYZ()-pts1.XYZ());

  gp_XYZ ns1 = d1u1.Crossed(d1v1).XYZ();
  gp_XYZ ns2 = d1u2.Crossed(d1v2).XYZ();
  const Standard_Real norm1 = nplan.Crossed(ns1).Modulus();
  const Standard_Real norm2 = nplan.Crossed(ns2).Modulus();
  ns1.SetLinearForm(nplan.Dot(ns1)/norm1,nplan, -1./norm1,ns1);
  ns2.SetLinearForm(nplan.Dot(ns2)/norm2,nplan, -1./norm2,ns2);

  F(1) = (nplan.Dot(pts1.XYZ())) + theD;
  F(2) = (nplan.Dot(pts2.XYZ())) + theD;
  
  F(3) = temp.Dot(ns1);
  F(4) = temp.Dot(ns2);

  return Standard_True;
}

Standard_Boolean BlendFunc_RuledInv::Derivatives(const math_Vector& X,
						 math_Matrix& D)
{
  gp_Pnt ptcur;
  gp_Vec d1cur,d2cur;
  curv->D2(X(2),ptcur,d1cur,d2cur);

  const Standard_Real normtgcur = d1cur.Magnitude();
  const gp_Vec nplan = d1cur.Normalized();

  gp_Vec dnplan;
  dnplan.SetLinearForm(-nplan.Dot(d2cur),nplan,d2cur);
  dnplan /= normtgcur;

  gp_Pnt2d p2d;
  gp_Vec2d v2d;
  csurf->D1(X(1),p2d,v2d);

  gp_Pnt pts1,pts2;
  gp_Vec d1u1,d1v1,d1u2,d1v2;
  gp_Vec d2u1,d2v1,d2u2,d2v2,d2uv1,d2uv2;
  gp_Vec dpdt, p1p2;
  if (first)
  {
    surf1->D2(p2d.X(),p2d.Y(),pts1,d1u1,d1v1,d2u1,d2v1,d2uv1);
    surf2->D2(X(3),X(4),pts2,d1u2,d1v2,d2u2,d2v2,d2uv2);
    dpdt.SetLinearForm(v2d.X(),d1u1,v2d.Y(),d1v1);
    p1p2 = gp_Vec(pts1,pts2);
    D(1,1) = dpdt.Dot(nplan);
    D(1,2) = dnplan.XYZ().Dot(pts1.XYZ()-ptcur.XYZ()) - normtgcur;
    D(1,3) = 0.;
    D(1,4) = 0.;

    D(2,1) = 0.;
    D(2,2) = dnplan.XYZ().Dot(pts2.XYZ()-ptcur.XYZ()) - normtgcur;
    D(2,3) = d1u2.Dot(nplan);
    D(2,4) = d1v2.Dot(nplan);
  }
  else
  {
    surf1->D2(X(3),X(4),pts1,d1u1,d1v1,d2u1,d2v1,d2uv1);
    surf2->D2(p2d.X(),p2d.Y(),pts2,d1u2,d1v2,d2u2,d2v2,d2uv2);
    dpdt.SetLinearForm(v2d.X(),d1u2,v2d.Y(),d1v2);
    p1p2 = gp_Vec(pts1,pts2);
    D(1,1) = 0.;
    D(1,2) = dnplan.XYZ().Dot(pts1.XYZ()-ptcur.XYZ()) - normtgcur;
    D(1,3) = d1u1.Dot(nplan);
    D(1,4) = d1v1.Dot(nplan);

    D(2,1) = dpdt.Dot(nplan);
    D(2,2) = dnplan.XYZ().Dot(pts2.XYZ()-ptcur.XYZ()) - normtgcur;
    D(2,3) = 0.;
    D(2,4) = 0.;
  }

  const gp_Vec ns1 = d1u1.Crossed(d1v1);
  const gp_Vec ns2 = d1u2.Crossed(d1v2);
  const gp_Vec ncrossns1 = nplan.Crossed(ns1);
  const gp_Vec ncrossns2 = nplan.Crossed(ns2);
  const Standard_Real norm1 = ncrossns1.Magnitude();
  const Standard_Real norm2 = ncrossns2.Magnitude();

  const Standard_Real ndotns1 = nplan.Dot(ns1);
  const Standard_Real ndotns2 = nplan.Dot(ns2);

  gp_Vec nor1,nor2;
  nor1.SetLinearForm(ndotns1/norm1,nplan,-1./norm1,ns1);
  nor2.SetLinearForm(ndotns2/norm2,nplan,-1./norm2,ns2);

  if (first) {
    D(3,3) = d1u2.Dot(nor1);
    D(3,4) = d1v2.Dot(nor1);

    D(4,1) = -(dpdt.Dot(nor2));
  }
  else {
    D(3,1) = dpdt.Dot(nor1);

    D(4,3) = -(d1u1.Dot(nor2));
    D(4,4) = -(d1v1.Dot(nor2));
  }

  gp_Vec resul1,resul2,temp;
  Standard_Real grosterme;

  // Derivee de nor1 par rapport a u1

  temp = d2u1.Crossed(d1v1).Added(d1u1.Crossed(d2uv1));
  grosterme = ncrossns1.Dot(nplan.Crossed(temp))/norm1/norm1;
  resul1.SetLinearForm(-(grosterme*ndotns1-nplan.Dot(temp))/norm1,nplan,
		       grosterme/norm1,ns1,
		       -1./norm1,temp);

  // Derivee par rapport a v1

  temp = d2uv1.Crossed(d1v1).Added(d1u1.Crossed(d2v1));
  grosterme = ncrossns1.Dot(nplan.Crossed(temp))/norm1/norm1;
  resul2.SetLinearForm(-(grosterme*ndotns1-nplan.Dot(temp))/norm1,nplan,
		       grosterme/norm1,ns1,
		       -1./norm1,temp);

  if (first) {
    resul1.SetLinearForm(v2d.X(),resul1,v2d.Y(),resul2);
    D(3,1) = p1p2.Dot(resul1) - (dpdt.Dot(nor1));
  }
  else {
    D(3,3) = -(d1u1.Dot(nor1)) + p1p2.Dot(resul1);
    D(3,4) = -(d1v1.Dot(nor1)) + p1p2.Dot(resul2);
  }

  // Derivee de nor2 par rapport a u2
  temp = d2u2.Crossed(d1v2).Added(d1u2.Crossed(d2uv2));
  grosterme = ncrossns2.Dot(nplan.Crossed(temp))/norm2/norm2;
  resul1.SetLinearForm(-(grosterme*ndotns2-nplan.Dot(temp))/norm2,nplan,
		       grosterme/norm2,ns2,
		       -1./norm2,temp);

  // Derivee par rapport a v2
  temp = d2uv2.Crossed(d1v2).Added(d1u2.Crossed(d2v2));
  grosterme = ncrossns2.Dot(nplan.Crossed(temp))/norm2/norm2;
  resul2.SetLinearForm(-(grosterme*ndotns2-nplan.Dot(temp))/norm2,nplan,
		       grosterme/norm2,ns2,
		       -1./norm2,temp);

  if (first) {
    D(4,3) = d1u2.Dot(nor2) + p1p2.Dot(resul1);
    D(4,4) = d1v2.Dot(nor2) + p1p2.Dot(resul2);
  }
  else {
    resul1.SetLinearForm(v2d.X(),resul1,v2d.Y(),resul2);
    D(4,1) = p1p2.Dot(resul1) + dpdt.Dot(nor2) ;
  }


  // derivee par rapport a w (parametre sur ligne guide)

  grosterme = ncrossns1.Dot(dnplan.Crossed(ns1))/norm1/norm1;
  resul1.SetLinearForm(-(grosterme*ndotns1-dnplan.Dot(ns1))/norm1,nplan,
		       ndotns1/norm1,dnplan,
		       grosterme/norm1,ns1);


  grosterme = ncrossns2.Dot(dnplan.Crossed(ns2))/norm2/norm2;
  resul2.SetLinearForm(-(grosterme*ndotns2-dnplan.Dot(ns2))/norm2,nplan,
		       ndotns2/norm2,dnplan,
		       grosterme/norm2,ns2);

  D(3,2) = p1p2.Dot(resul1);
  D(4,2) = p1p2.Dot(resul2);

  return Standard_True;
}


Standard_Boolean BlendFunc_RuledInv::Values(const math_Vector& X,
                                            math_Vector& F,
                                            math_Matrix& D)
{
  gp_Pnt ptcur;
  gp_Vec d1cur,d2cur;
  curv->D2(X(2),ptcur,d1cur,d2cur);

  const Standard_Real normtgcur = d1cur.Magnitude();
  const gp_Vec nplan = d1cur.Normalized();
  const Standard_Real theD = -(nplan.XYZ().Dot(ptcur.XYZ()));

  gp_Vec dnplan;
  dnplan.SetLinearForm(-nplan.Dot(d2cur),nplan,d2cur);
  dnplan /= normtgcur;

  gp_Pnt2d p2d;
  gp_Vec2d v2d;
  csurf->D1(X(1),p2d,v2d);

  gp_Pnt pts1,pts2;
  gp_Vec d1u1,d1v1,d1u2,d1v2;
  gp_Vec d2u1,d2v1,d2u2,d2v2,d2uv1,d2uv2;
  gp_Vec dpdt,p1p2;
  if (first)
  {
    surf1->D2(p2d.X(),p2d.Y(),pts1,d1u1,d1v1,d2u1,d2v1,d2uv1);
    surf2->D2(X(3),X(4),pts2,d1u2,d1v2,d2u2,d2v2,d2uv2);
    dpdt.SetLinearForm(v2d.X(),d1u1,v2d.Y(),d1v1);
    p1p2 = gp_Vec(pts1,pts2);
    D(1,1) = dpdt.Dot(nplan);
    D(1,2) = dnplan.XYZ().Dot(pts1.XYZ()-ptcur.XYZ()) - normtgcur;
    D(1,3) = 0.;
    D(1,4) = 0.;

    D(2,1) = 0.;
    D(2,2) = dnplan.XYZ().Dot(pts2.XYZ()-ptcur.XYZ()) - normtgcur;
    D(2,3) = d1u2.Dot(nplan);
    D(2,4) = d1v2.Dot(nplan);
  }
  else
  {
    surf1->D2(X(3),X(4),pts1,d1u1,d1v1,d2u1,d2v1,d2uv1);
    surf2->D2(p2d.X(),p2d.Y(),pts2,d1u2,d1v2,d2u2,d2v2,d2uv2);
    dpdt.SetLinearForm(v2d.X(),d1u2,v2d.Y(),d1v2);
    p1p2 = gp_Vec(pts1,pts2);
    D(1,1) = 0.;
    D(1,2) = dnplan.XYZ().Dot(pts1.XYZ()-ptcur.XYZ()) - normtgcur;
    D(1,3) = d1u1.Dot(nplan);
    D(1,4) = d1v1.Dot(nplan);

    D(2,1) = dpdt.Dot(nplan);
    D(2,2) = dnplan.XYZ().Dot(pts2.XYZ()-ptcur.XYZ()) - normtgcur;
    D(2,3) = 0.;
    D(2,4) = 0.;
  }

  const gp_Vec ns1 = d1u1.Crossed(d1v1);
  const gp_Vec ns2 = d1u2.Crossed(d1v2);
  const gp_Vec ncrossns1 = nplan.Crossed(ns1);
  const gp_Vec ncrossns2 = nplan.Crossed(ns2);
  const Standard_Real norm1 = ncrossns1.Magnitude();
  const Standard_Real norm2 = ncrossns2.Magnitude();

  const Standard_Real ndotns1 = nplan.Dot(ns1);
  const Standard_Real ndotns2 = nplan.Dot(ns2);

  gp_Vec nor1,nor2;
  nor1.SetLinearForm(ndotns1/norm1,nplan,-1./norm1,ns1);
  nor2.SetLinearForm(ndotns2/norm2,nplan,-1./norm2,ns2);

  F(1) = (nplan.Dot(pts1.XYZ())) + theD;
  F(2) = (nplan.Dot(pts2.XYZ())) + theD;
  
  F(3) = p1p2.Dot(nor1);
  F(4) = p1p2.Dot(nor2);

  if (first) {
    D(3,3) = d1u2.Dot(nor1);
    D(3,4) = d1v2.Dot(nor1);

    D(4,1) = -(dpdt.Dot(nor2));
  }
  else {
    D(3,1) = dpdt.Dot(nor1);

    D(4,3) = -(d1u1.Dot(nor2));
    D(4,4) = -(d1v1.Dot(nor2));
  }

  gp_Vec resul1,resul2,temp;
  Standard_Real grosterme;

  // Derivee de nor1 par rapport a u1

  temp = d2u1.Crossed(d1v1).Added(d1u1.Crossed(d2uv1));
  grosterme = ncrossns1.Dot(nplan.Crossed(temp))/norm1/norm1;
  resul1.SetLinearForm(-(grosterme*ndotns1-nplan.Dot(temp))/norm1,nplan,
		       grosterme/norm1,ns1,
		       -1./norm1,temp);

  // Derivee par rapport a v1

  temp = d2uv1.Crossed(d1v1).Added(d1u1.Crossed(d2v1));
  grosterme = ncrossns1.Dot(nplan.Crossed(temp))/norm1/norm1;
  resul2.SetLinearForm(-(grosterme*ndotns1-nplan.Dot(temp))/norm1,nplan,
		       grosterme/norm1,ns1,
		       -1./norm1,temp);

  if (first) {
    resul1.SetLinearForm(v2d.X(),resul1,v2d.Y(),resul2);
    D(3,1) = p1p2.Dot(resul1) - (dpdt.Dot(nor1));
  }
  else {
    D(3,3) = -(d1u1.Dot(nor1)) + p1p2.Dot(resul1);
    D(3,4) = -(d1v1.Dot(nor1)) + p1p2.Dot(resul2);
  }

  // Derivee de nor2 par rapport a u2
  temp = d2u2.Crossed(d1v2).Added(d1u2.Crossed(d2uv2));
  grosterme = ncrossns2.Dot(nplan.Crossed(temp))/norm2/norm2;
  resul1.SetLinearForm(-(grosterme*ndotns2-nplan.Dot(temp))/norm2,nplan,
		       grosterme/norm2,ns2,
		       -1./norm2,temp);

  // Derivee par rapport a v2
  temp = d2uv2.Crossed(d1v2).Added(d1u2.Crossed(d2v2));
  grosterme = ncrossns2.Dot(nplan.Crossed(temp))/norm2/norm2;
  resul2.SetLinearForm(-(grosterme*ndotns2-nplan.Dot(temp))/norm2,nplan,
		       grosterme/norm2,ns2,
		       -1./norm2,temp);

  if (first) {
    D(4,3) = d1u2.Dot(nor2) + p1p2.Dot(resul1);
    D(4,4) = d1v2.Dot(nor2) + p1p2.Dot(resul2);
  }
  else {
    resul1.SetLinearForm(v2d.X(),resul1,v2d.Y(),resul2);
    D(4,1) = p1p2.Dot(resul1) + dpdt.Dot(nor2) ;
  }


  // derivee par rapport a w (parametre sur ligne guide)

  grosterme = ncrossns1.Dot(dnplan.Crossed(ns1))/norm1/norm1;
  resul1.SetLinearForm(-(grosterme*ndotns1-dnplan.Dot(ns1))/norm1,nplan,
		       ndotns1/norm1,dnplan,
		       grosterme/norm1,ns1);


  grosterme = ncrossns2.Dot(dnplan.Crossed(ns2))/norm2/norm2;
  resul2.SetLinearForm(-(grosterme*ndotns2-dnplan.Dot(ns2))/norm2,nplan,
		       ndotns2/norm2,dnplan,
		       grosterme/norm2,ns2);

  D(3,2) = p1p2.Dot(resul1);
  D(4,2) = p1p2.Dot(resul2);

  return Standard_True;
}
