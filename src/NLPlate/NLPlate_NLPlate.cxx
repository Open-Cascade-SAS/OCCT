// Created on: 1998-04-09
// Created by: Andre LIEUTIER
// Copyright (c) 1998-1999 Matra Datavision
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


#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <NLPlate_HGPPConstraint.hxx>
#include <NLPlate_ListIteratorOfStackOfPlate.hxx>
#include <NLPlate_NLPlate.hxx>
#include <Plate_D1.hxx>
#include <Plate_D2.hxx>
#include <Plate_D3.hxx>
#include <Plate_FreeGtoCConstraint.hxx>
#include <Plate_PinpointConstraint.hxx>
#include <Plate_Plate.hxx>

NLPlate_NLPlate::NLPlate_NLPlate(const Handle(Geom_Surface)& InitialSurface) :
 myInitialSurface(InitialSurface),OK(Standard_False) {}
//=======================================================================

 void NLPlate_NLPlate::Load(const Handle(NLPlate_HGPPConstraint)& GConst) 
{
  if(!GConst.IsNull()) myHGPPConstraints.Append(GConst);
  OK = Standard_False;
}

//=======================================================================
//function : Solve
//purpose  : 
//=======================================================================
 void NLPlate_NLPlate::Solve(const Standard_Integer ord, const Standard_Integer InitialConsraintOrder) 
{
  Standard_Integer maxOrder = MaxActiveConstraintOrder();
  Standard_Integer ordre = ord;
  if(ordre<maxOrder+2) ordre = maxOrder+2;

  for(Standard_Integer iterOrder=InitialConsraintOrder;iterOrder<=maxOrder;iterOrder++)
    {
     if(!Iterate(iterOrder,ordre+iterOrder-maxOrder))
       {
	OK = Standard_False;
	break;
       }
    }
  OK = Standard_True;
}
//=======================================================================
//function : Solve2
//purpose  : 
//=======================================================================
 void NLPlate_NLPlate::Solve2(const Standard_Integer ord, const Standard_Integer InitialConsraintOrder) 
{
  Standard_Integer maxOrder = MaxActiveConstraintOrder();
  Standard_Integer ordre = ord;
  if(ordre<maxOrder+2) ordre = maxOrder+2;
  if(Iterate(0,ord)) 
    {
      mySOP.First().SetPolynomialPartOnly(Standard_True);
      ConstraintsSliding();
    }
      

  for(Standard_Integer iterOrder=InitialConsraintOrder;iterOrder<=maxOrder;iterOrder++)
    {
     if(!Iterate(iterOrder,ordre+iterOrder-maxOrder))
       {
	OK = Standard_False;
	break;
       }
    }
  OK = Standard_True;
}

//=======================================================================
//function : IncrementalSolve
//purpose  : 
//=======================================================================
 void NLPlate_NLPlate::IncrementalSolve(const Standard_Integer ord, const Standard_Integer /*InitialConsraintOrder*/,
			     const Standard_Integer NbIncrements, const Standard_Boolean UVSliding) 
{
  Standard_Integer maxOrder = MaxActiveConstraintOrder();
  Standard_Integer ordre = ord;
  if(ordre<maxOrder+2) ordre = maxOrder+2;
  Standard_Real IncrementalLoad = 1.;

  for(Standard_Integer increment=0;increment < NbIncrements;increment++)
    {
      IncrementalLoad = 1./Standard_Real(NbIncrements-increment);
//      for(Standard_Integer iterOrder=InitialConsraintOrder;iterOrder<=maxOrder;iterOrder++)
      Standard_Integer iterOrder=maxOrder;
	{
	  if(!Iterate(iterOrder,ordre+iterOrder-maxOrder,IncrementalLoad))
	    {
	      OK = Standard_False;
	      return;
	    }
	}
      if(UVSliding) ConstraintsSliding();
    }
  OK = Standard_True;
}

//=======================================================================
 Standard_Boolean NLPlate_NLPlate::IsDone() const
{
  return OK;
}
//=======================================================================

 void NLPlate_NLPlate::destroy() 
{
  Init();
}
//=======================================================================

 void NLPlate_NLPlate::Init() 
{
  mySOP.Clear();
  myHGPPConstraints.Clear();
}
//=======================================================================

 gp_XYZ NLPlate_NLPlate::Evaluate(const gp_XY& point2d) const
{
  return EvaluateDerivative(point2d,0,0);
}
//=======================================================================

 gp_XYZ NLPlate_NLPlate::EvaluateDerivative(const gp_XY& point2d,const Standard_Integer iu,const Standard_Integer iv) const
{
  gp_XYZ Value(0.,0.,0.);
  if((iu==0)&&(iv==0))
    Value = myInitialSurface->Value(point2d.X(),point2d.Y()).XYZ();
  else
    Value = myInitialSurface->DN(point2d.X(),point2d.Y(),iu,iv).XYZ();

  for(NLPlate_ListIteratorOfStackOfPlate SI(mySOP);SI.More();SI.Next())
    {
      if(SI.Value().IsDone())
	Value += SI.Value().EvaluateDerivative(point2d,iu,iv);
    }
  return Value;
}
//=======================================================================

 Standard_Integer NLPlate_NLPlate::Continuity() const
{
  Standard_Integer cont ;
  for( cont=-1; cont<10;cont++)
    {
      if(!(myInitialSurface->IsCNu(cont+1)&&myInitialSurface->IsCNv(cont+1)))break;
    }
  for(NLPlate_ListIteratorOfStackOfPlate SI(mySOP);SI.More();SI.Next())
    {
      if((SI.Value().IsDone())&&(cont > SI.Value().Continuity())) cont = SI.Value().Continuity();
    }
  return cont;
}
//=======================================================================

Standard_Boolean NLPlate_NLPlate::Iterate(const Standard_Integer ConstraintOrder,
const Standard_Integer ResolutionOrder,
const Standard_Real IncrementalLoading) 
{
  Plate_Plate EmptyPlate;
  mySOP.Prepend(EmptyPlate);
  Plate_Plate &TopP = mySOP.First();
  for(Standard_Integer index =1; index <= myHGPPConstraints.Length(); index++)
    {
      const Handle(NLPlate_HGPPConstraint) &HGPP = myHGPPConstraints(index);
      Standard_Integer Order = HGPP->ActiveOrder();
      if(ConstraintOrder<Order) Order = ConstraintOrder;
      const gp_XY &UV = HGPP->UV();
      
      if((Order >=0)&& HGPP->IsG0())
	{
	  if(HGPP->IncrementalLoadAllowed())
	    TopP.Load(Plate_PinpointConstraint(UV, (HGPP->G0Target()-Evaluate(UV))*IncrementalLoading));
	  else
	    TopP.Load(Plate_PinpointConstraint(UV, HGPP->G0Target()-Evaluate(UV)));
	}
      
      if((IncrementalLoading != 1.) && HGPP->IncrementalLoadAllowed() && (Order>=1))
	{
	  switch(Order)
	    {
	    case 1:
	      {
		Plate_D1 D1S(EvaluateDerivative(UV,1,0),EvaluateDerivative(UV,0,1));
		TopP.Load(Plate_FreeGtoCConstraint(UV,D1S,HGPP->G1Target(),IncrementalLoading,HGPP->Orientation()));
	      }
	      break;
	    case 2:
	      {
		Plate_D1 D1S(EvaluateDerivative(UV,1,0),EvaluateDerivative(UV,0,1));
		Plate_D2 D2S(EvaluateDerivative(UV,2,0),EvaluateDerivative(UV,1,1),EvaluateDerivative(UV,0,2));
		TopP.Load(Plate_FreeGtoCConstraint(UV,D1S,HGPP->G1Target(),D2S,HGPP->G2Target(),
						   IncrementalLoading,HGPP->Orientation()));
	      }
	      break;
	    case 3:
	      {
		Plate_D1 D1S(EvaluateDerivative(UV,1,0),EvaluateDerivative(UV,0,1));
		Plate_D2 D2S(EvaluateDerivative(UV,2,0),EvaluateDerivative(UV,1,1),EvaluateDerivative(UV,0,2));
		Plate_D3 D3S(EvaluateDerivative(UV,3,0),EvaluateDerivative(UV,2,1),
			     EvaluateDerivative(UV,1,2),EvaluateDerivative(UV,0,3));
		TopP.Load(Plate_FreeGtoCConstraint(UV,D1S,HGPP->G1Target(),D2S,HGPP->G2Target(),D3S,HGPP->G3Target(),
						   IncrementalLoading,HGPP->Orientation()));
	      }
	      break;
	    default:
	      break;
	    }
	}
      else
	{
	  switch(Order)
	    {
	    case 1:
	      {
		Plate_D1 D1S(EvaluateDerivative(UV,1,0),EvaluateDerivative(UV,0,1));
		TopP.Load(Plate_FreeGtoCConstraint(UV,D1S,HGPP->G1Target()));
	      }
	      break;
	    case 2:
	      {
		Plate_D1 D1S(EvaluateDerivative(UV,1,0),EvaluateDerivative(UV,0,1));
		Plate_D2 D2S(EvaluateDerivative(UV,2,0),EvaluateDerivative(UV,1,1),EvaluateDerivative(UV,0,2));
		TopP.Load(Plate_FreeGtoCConstraint(UV,D1S,HGPP->G1Target(),D2S,HGPP->G2Target()));
	      }
	      break;
	    case 3:
	      {
		Plate_D1 D1S(EvaluateDerivative(UV,1,0),EvaluateDerivative(UV,0,1));
		Plate_D2 D2S(EvaluateDerivative(UV,2,0),EvaluateDerivative(UV,1,1),EvaluateDerivative(UV,0,2));
		Plate_D3 D3S(EvaluateDerivative(UV,3,0),EvaluateDerivative(UV,2,1),
			     EvaluateDerivative(UV,1,2),EvaluateDerivative(UV,0,3));
		TopP.Load(Plate_FreeGtoCConstraint(UV,D1S,HGPP->G1Target(),D2S,HGPP->G2Target(),D3S,HGPP->G3Target()));
	      }
	      break;
	    default:
	      break;
	    }
	}
    }
  
  TopP.SolveTI(ResolutionOrder);
  if(!TopP.IsDone())
    {
      mySOP.RemoveFirst();
      return Standard_False;
    }
  else
    return Standard_True;
}
//=======================================================================

 void NLPlate_NLPlate::ConstraintsSliding(const Standard_Integer NbIterations) 
{
  for(Standard_Integer index =1; index <= myHGPPConstraints.Length(); index++)
    {
      const Handle(NLPlate_HGPPConstraint) &HGPP = myHGPPConstraints(index);
      if(HGPP->UVFreeSliding() && HGPP->IsG0())
	{
	  gp_XY UV = HGPP->UV();
	  gp_XYZ P0 = Evaluate(UV);
	  const gp_XYZ P1 = HGPP->G0Target();
	  for(Standard_Integer iter=1;iter<=NbIterations;iter++)
	    {
	      // on itere au premier ordre, ce qui suffit si on est assez pres de la surface ??
	      gp_XYZ DP = P1 - P0;
	      gp_XYZ Du = EvaluateDerivative(UV,1,0);
	      gp_XYZ Dv = EvaluateDerivative(UV,0,1);
	      math_Matrix mat(0,1,0,1);
	      mat(0,0) = Du*Du;
	      mat(0,1) = Du*Dv;
	      mat(1,0) = Du*Dv;
	      mat(1,1) = Dv*Dv;
	      math_Gauss gauss(mat);
	      if(!gauss.IsDone()) break;

	      math_Vector vec(0,1);
	      vec(0) = Du*DP;
	      vec(1) = Dv*DP;
	      math_Vector sol(0,1);
	      gauss.Solve(vec,sol);
	      UV.SetX(UV.X()+sol(0));
	      UV.SetY(UV.Y()+sol(1));
	      P0 = Evaluate(UV);
	    }
	  HGPP->SetUV(UV);
	}
    }
}

//=======================================================================

 Standard_Integer NLPlate_NLPlate::MaxActiveConstraintOrder() const
{
  Standard_Integer MaxOrder = -1;
  for(Standard_Integer index =1; index <= myHGPPConstraints.Length(); index++)
    {
      Standard_Integer CAOrder = myHGPPConstraints(index)->ActiveOrder();
      if(CAOrder > MaxOrder) MaxOrder = CAOrder;
    }
  return MaxOrder;
}
