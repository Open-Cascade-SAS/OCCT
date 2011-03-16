// File:	GeomFill_PlanFunc.cxx
// Created:	Thu Oct 29 12:00:47 1998
// Author:	Philippe MANGIN
//		<pmn@sgi29>


#include <GeomFill_PlanFunc.ixx>


 GeomFill_PlanFunc::GeomFill_PlanFunc(const gp_Pnt& P,
				      const gp_Vec& V,
				      const Handle(Adaptor3d_HCurve)& C) :
				      myCurve(C)
{
  myPnt = P.XYZ();
  myVec = V.XYZ();
}


 Standard_Boolean GeomFill_PlanFunc::Value(const Standard_Real  X,
					         Standard_Real& F)
{
  myCurve->D0(X, G);
  V.SetLinearForm(-1, myPnt, G.XYZ());
  F = myVec.Dot(V);
  return Standard_True;
}

 Standard_Boolean GeomFill_PlanFunc::Derivative(const Standard_Real  X,
					         Standard_Real& D)
{
  gp_Vec dg;
  myCurve->D1(X, G, dg);
  D = myVec.Dot(dg.XYZ());

  return Standard_True;  
}

 Standard_Boolean GeomFill_PlanFunc::Values(const Standard_Real  X,
					         Standard_Real& F,
					         Standard_Real& D)
{
  gp_Vec dg;
  myCurve->D1(X, G, dg);
  V.SetLinearForm(-1, myPnt, G.XYZ());
  F = myVec.Dot(V);
  D = myVec.Dot(dg.XYZ());

  return Standard_True;
}

//void  GeomFill_PlanFunc::D2(const Standard_Real  X,
//			    Standard_Real& F,
//			    Standard_Real& D1,
//			    Standard_Real& D2)
void  GeomFill_PlanFunc::D2(const Standard_Real  ,
			    Standard_Real& ,
			    Standard_Real& ,
			    Standard_Real& )
{
}


void  GeomFill_PlanFunc::DEDT(const Standard_Real  X,
			      const gp_Vec& DPnt,
			      const gp_Vec& DVec,
			      Standard_Real& DFDT)
{
  myCurve->D0(X, G);
  V.SetLinearForm(-1, myPnt, G.XYZ());
  DFDT = DVec.Dot(V) - myVec.Dot(DPnt.XYZ());

}


void  GeomFill_PlanFunc::D2E(const Standard_Real  X,
			     const gp_Vec& DP,
//			     const gp_Vec& D2P,
			     const gp_Vec& ,
			     const gp_Vec& DV,
//			     const gp_Vec& D2V,
			     const gp_Vec& ,
			     Standard_Real& DFDT,
//			     Standard_Real& ,
			     Standard_Real& D2FDT2,
//			     Standard_Real& D2FDTDX)
			     Standard_Real& )
{
  gp_Vec dg;
  myCurve->D1(X, G, dg);
  gp_XYZ DVDT;
  V.SetLinearForm(-1, myPnt, G.XYZ());
  DVDT.SetLinearForm(-1, DP.XYZ(), G.XYZ());
  DFDT = DV.Dot(V) + myVec.Dot(DVDT);

}
