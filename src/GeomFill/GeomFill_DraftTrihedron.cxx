// File:	GeomFill_DraftTrihedron.cxx
// Created:	Wed Apr 15 14:59:31 1998
// Author:	 Stephanie HUMEAU
//		<shu@sun17>


#include <GeomFill_DraftTrihedron.ixx>

#include <Precision.hxx>
#include <GeomAbs_CurveType.hxx>
#include <Adaptor3d_HCurve.hxx>

//=======================================================================
//function : DDeriv
//purpose  : computes (F/|F|)''
//=======================================================================
static gp_Vec DDeriv(const gp_Vec& F, const gp_Vec& DF, const gp_Vec& D2F)
{
  Standard_Real Norma = F.Magnitude();
#ifdef DEB
  Standard_Real dot = F.Dot(DF);
#else
  F.Dot(DF);
#endif
  gp_Vec Result = (D2F - 2*DF*(F*DF)/(Norma*Norma))/Norma - 
     F*((DF.SquareMagnitude() + F*D2F 
        - 3*(F*DF)*(F*DF)/(Norma*Norma))/(Norma*Norma*Norma));
  return Result;
}

//=======================================================================
//function : DraftTrihedron
//purpose  : Constructor
//=======================================================================
 GeomFill_DraftTrihedron::GeomFill_DraftTrihedron(const gp_Vec& BiNormal,
						  const Standard_Real Angle)
{
  B = BiNormal;
  B.Normalize();
  SetAngle(Angle);
}


//=======================================================================
//function : Setangle
//purpose  : 
//=======================================================================
 void GeomFill_DraftTrihedron::SetAngle(const Standard_Real Angle)
{
  myAngle = PI/2 + Angle;
  myCos = Cos(myAngle);
}


//=======================================================================
//function : D0
//purpose  : calculation of trihedron
//=======================================================================
 Standard_Boolean GeomFill_DraftTrihedron::D0(const Standard_Real Param,
					      gp_Vec& Tangent,
					      gp_Vec& Normal,
					      gp_Vec& BiNormal) 
{  
  gp_Pnt P;
  gp_Vec T;
  myTrimmed->D1(Param,P,T);
  T.Normalize();

  gp_Vec b = T.Crossed(B);
  Standard_Real normb = b.Magnitude();

  b /=  normb;
  if (normb < 1.e-12) 
    return Standard_False;

  gp_Vec v = b.Crossed(T);

  Standard_Real mu = myCos ;
  mu = myCos;

  //La Normal est portee par la regle
  Normal.SetLinearForm(Sqrt(1-mu*mu), b,  mu, v);
 
  // Le reste suit....
  // La tangente est perpendiculaire a la normale et a la direction de depouille
  Tangent =  Normal.Crossed(B);
  Tangent.Normalize();

  BiNormal = Tangent;
  BiNormal.Cross(Normal);

  return Standard_True; 
}

//=======================================================================
//function : D1
//purpose  :  calculation of trihedron and first derivative 
//=======================================================================
 Standard_Boolean GeomFill_DraftTrihedron::D1(const Standard_Real Param, 
					      gp_Vec& Tangent,
					      gp_Vec& DTangent,
					      gp_Vec& Normal,
					      gp_Vec& DNormal,
					      gp_Vec& BiNormal,
					      gp_Vec& DBiNormal) 
{    
  gp_Pnt P;
  gp_Vec T, DT, aux;

  myTrimmed->D2(Param, P, T, aux);

  Standard_Real normT, normb;
  normT = T.Magnitude();
  T /=  normT;
  DT.SetLinearForm(-(T.Dot(aux)), T, aux);
  DT /=  normT;

  gp_Vec db, b = T.Crossed(B);
  normb = b.Magnitude();
  if (normb < 1.e-12) 
    return Standard_False;
  b /=  normb;
  aux = DT.Crossed(B);
  db.SetLinearForm( -(b.Dot(aux)), b, aux);
  db /=  normb;

  gp_Vec v = b.Crossed(T);
  gp_Vec dv = db.Crossed(T) + b.Crossed(DT);

  Standard_Real mu = myCos;

  Normal.SetLinearForm(Sqrt(1-mu*mu), b,  mu, v); 
  DNormal.SetLinearForm(Sqrt(1-mu*mu), db,  mu, dv); 

  Tangent =  Normal.Crossed(B);
  normT = Tangent.Magnitude();
  Tangent/= normT;
  aux = DNormal.Crossed(B);
  DTangent.SetLinearForm( -(Tangent.Dot(aux)), Tangent, aux);
  DTangent /= normT;

  BiNormal = Tangent;
  BiNormal.Cross(Normal);
  DBiNormal.SetLinearForm(DTangent.Crossed(Normal),Tangent.Crossed(DNormal)); 
   
  return Standard_True;
}

//=======================================================================
//function : D2
//purpose  : calculation of trihedron and derivatives 1 et 2
//=======================================================================
Standard_Boolean GeomFill_DraftTrihedron::D2(const Standard_Real Param, 
						 gp_Vec& Tangent,
					         gp_Vec& DTangent,
					         gp_Vec& D2Tangent,
						 gp_Vec& Normal,
					         gp_Vec& DNormal,
					         gp_Vec& D2Normal,
						 gp_Vec& BiNormal,
					         gp_Vec& DBiNormal,
					         gp_Vec& D2BiNormal) 
{  
  gp_Pnt P;
  gp_Vec T, DT, D2T, aux, aux2;
  Standard_Real dot;

  myTrimmed->D3(Param, P, T, aux, aux2);

  Standard_Real normT, normb;

  D2T = DDeriv(T, aux, aux2);
  normT = T.Magnitude();
  T /=  normT;
  dot = T.Dot(aux);
  DT.SetLinearForm(-dot, T, aux);
  DT /=  normT;
		    
  gp_Vec db, d2b, b = T.Crossed(B);
  normb = b.Magnitude();
  if (normb < 1.e-12) 
    return Standard_False;

  aux = DT.Crossed(B);  aux2 = D2T.Crossed(B);
  d2b =  DDeriv(b, aux, aux2);
  b /=  normb;
  dot = b.Dot(aux);
  db.SetLinearForm( -dot, b, aux);
  db /=  normb;

  gp_Vec v = b.Crossed(T);
  gp_Vec dv = db.Crossed(T) + b.Crossed(DT);
  gp_Vec d2v = d2b.Crossed(T) + 2*db.Crossed(DT) + b.Crossed(D2T);
  

  Standard_Real mu = myCos, rac;
  rac = Sqrt(1-mu*mu);

  Normal  .SetLinearForm( rac, b  ,  mu, v); 
  DNormal .SetLinearForm( rac, db ,  mu, dv);
  D2Normal.SetLinearForm( rac, d2b,  mu, d2v); 

  Tangent =  Normal.Crossed(B);
  normT = Tangent.Magnitude();

  aux = DNormal.Crossed(B);  
  aux2 = D2Normal.Crossed(B);
  D2Tangent = DDeriv(Tangent, aux, aux2);
  Tangent/= normT;
  dot = Tangent.Dot(aux);
  DTangent.SetLinearForm( -dot, Tangent, aux);
  DTangent /= normT;

  BiNormal = Tangent;
  BiNormal.Cross(Normal);
  DBiNormal.SetLinearForm(DTangent.Crossed(Normal),Tangent.Crossed(DNormal)); 
  D2BiNormal.SetLinearForm(1, D2Tangent.Crossed(Normal),
			   2,  DTangent.Crossed(DNormal),
			   Tangent.Crossed(D2Normal));
   
  return Standard_True;
}


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================
 Handle(GeomFill_TrihedronLaw) GeomFill_DraftTrihedron::Copy() const
{
 Handle(GeomFill_DraftTrihedron) copy = 
   new (GeomFill_DraftTrihedron) (B,myAngle-PI/2);
 copy->SetCurve(myCurve);
 return copy;
} 

//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================
 Standard_Integer GeomFill_DraftTrihedron::NbIntervals(const GeomAbs_Shape S) const
{
  GeomAbs_Shape tmpS=GeomAbs_C0;
  switch (S) {
  case GeomAbs_C0: tmpS = GeomAbs_C2; break;
  case GeomAbs_C1: tmpS = GeomAbs_C3; break;
  case GeomAbs_C2:
  case GeomAbs_C3:
  case GeomAbs_CN: tmpS = GeomAbs_CN; break;
  default: Standard_OutOfRange::Raise();
  }
  
  return myCurve->NbIntervals(tmpS);
}

//======================================================================
//function :Intervals
//purpose  : 
//=======================================================================
 void GeomFill_DraftTrihedron::Intervals(TColStd_Array1OfReal& TT,
					    const GeomAbs_Shape S) const
{
  GeomAbs_Shape tmpS=GeomAbs_C0;
  switch (S) {
  case GeomAbs_C0: tmpS = GeomAbs_C2; break;
  case GeomAbs_C1: tmpS = GeomAbs_C3; break;
  case GeomAbs_C2:
  case GeomAbs_C3:
  case GeomAbs_CN: tmpS = GeomAbs_CN; break;
  default: Standard_OutOfRange::Raise();
  }
  
  myCurve->Intervals(TT, tmpS);
}

//=======================================================================
//function : GetAverageLaw
//purpose  : 
//=======================================================================
 void GeomFill_DraftTrihedron::GetAverageLaw(gp_Vec& ATangent,
					     gp_Vec& ANormal,
					     gp_Vec& ABiNormal) 
{
  Standard_Integer Num = 20; //order of digitalization
  gp_Vec T, N, BN;
  ATangent = gp_Vec(0, 0, 0);
  ANormal = gp_Vec(0, 0, 0);
  ABiNormal = gp_Vec(0, 0, 0);

  Standard_Real Step = (myTrimmed->LastParameter() - 
                        myTrimmed->FirstParameter()) / Num;
  Standard_Real Param;
  for (Standard_Integer i = 0; i <= Num; i++) {
    Param = myTrimmed->FirstParameter() + i*Step;
    if (Param > myTrimmed->LastParameter()) Param = myTrimmed->LastParameter();
    D0(Param, T, N, BN);
    ATangent += T;
    ANormal += N;
    ABiNormal += BN;
  }

  ANormal /= Num + 1;
  ABiNormal /= Num + 1;
  ATangent /= Num + 1;
}

//=======================================================================
//function : IsConstant
//purpose  : 
//=======================================================================
 Standard_Boolean GeomFill_DraftTrihedron::IsConstant() const
{
  return (myCurve->GetType() == GeomAbs_Line);
}

//=======================================================================
//function : IsOnlyBy3dCurve
//purpose  : 
//=======================================================================
 Standard_Boolean GeomFill_DraftTrihedron::IsOnlyBy3dCurve() const
{
    GeomAbs_CurveType TheType = myCurve->GetType();
  gp_Ax1 TheAxe;

  switch  (TheType) {
  case GeomAbs_Circle:
    {
      TheAxe =  myCurve->Circle().Axis();
      break;
    }
  case GeomAbs_Ellipse:
    {
      TheAxe =  myCurve->Ellipse().Axis();
      break;
    }
  case GeomAbs_Hyperbola:
    {
      TheAxe =  myCurve->Hyperbola().Axis();
      break;
    }
  case GeomAbs_Parabola:
    {
      TheAxe =  myCurve->Parabola().Axis();
      break;
    }
  case GeomAbs_Line:
    { //La normale du plan de la courbe est il perpendiculaire a la BiNormale ?
     gp_Vec V;
     V.SetXYZ(myCurve->Line().Direction().XYZ());
     return V.IsParallel(B, Precision::Angular());
    }
  default:
    return Standard_False; // pas de risques
  }

  // La normale du plan de la courbe est il // a la BiNormale ?
  gp_Vec V;
  V.SetXYZ(TheAxe.Direction().XYZ());
  return V.IsParallel(B, Precision::Angular());
}










