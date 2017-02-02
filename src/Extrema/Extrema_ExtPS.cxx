// Copyright (c) 1995-1999 Matra Datavision
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

//-----------------------------------------------------------------

#include <GeomAdaptor_HSurfaceOfLinearExtrusion.hxx>
#include <GeomAdaptor_HSurfaceOfRevolution.hxx>
#include <Adaptor3d_Surface.hxx>
#include <ElCLib.hxx>
#include <Extrema_ExtPExtS.hxx>
#include <Extrema_ExtPRevS.hxx>
#include <Extrema_ExtPS.hxx>
#include <Extrema_GenExtPS.hxx>
#include <Extrema_POnSurf.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_IsoType.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_TypeMismatch.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function : IsoIsDeg
//purpose  : 
//=======================================================================
static Standard_Boolean IsoIsDeg  (const Adaptor3d_Surface& S,
				   const Standard_Real      Param,
				   const GeomAbs_IsoType    IT,
				   const Standard_Real      TolMin,
				   const Standard_Real      TolMax) 
{
    Standard_Real U1=0.,U2=0.,V1=0.,V2=0.,T;
    Standard_Boolean Along = Standard_True;
    U1 = S.FirstUParameter();
    U2 = S.LastUParameter();
    V1 = S.FirstVParameter();
    V2 = S.LastVParameter();
    gp_Vec D1U,D1V;
    gp_Pnt P;
    Standard_Real Step,D1NormMax;
    if (IT == GeomAbs_IsoV) 
    {
      if( !Precision::IsInfinite(U1) &&  !Precision::IsInfinite(U2) )
      {
        Step = (U2 - U1)/10;
        if(Step < Precision::PConfusion()) {
          return Standard_False;
        }
        D1NormMax=0.;

        for (T=U1;T<=U2;T=T+Step) 
        {
          S.D1(T,Param,P,D1U,D1V);
          D1NormMax=Max(D1NormMax,D1U.Magnitude());
        }

        if (D1NormMax >TolMax || D1NormMax < TolMin ) 
          Along = Standard_False;
      }
    }
    else 
    {
      if( !Precision::IsInfinite(V1) &&  !Precision::IsInfinite(V2) )
      {
        Step = (V2 - V1)/10;
        if(Step < Precision::PConfusion()) {
          return Standard_False;
        }
        D1NormMax=0.;
        for (T=V1;T<=V2;T=T+Step) 
        {
          S.D1(Param,T,P,D1U,D1V);
          D1NormMax=Max(D1NormMax,D1V.Magnitude());
        }

        if (D1NormMax >TolMax || D1NormMax < TolMin ) 
          Along = Standard_False;
      }



    }
    return Along;
}

//=======================================================================
//function : TreatSolution
//purpose  : 
//=======================================================================

void Extrema_ExtPS::TreatSolution (const Extrema_POnSurf& PS,
				   const Standard_Real Val)
{
  Standard_Real U, V;
  PS.Parameter(U, V);
  if (myS->IsUPeriodic()) {
    U = ElCLib::InPeriod(U, myuinf, myuinf + myS->UPeriod());
    
    // Handle trimmed surfaces.
    if (U > myusup + mytolu)
      U -= myS->UPeriod();
    if (U < myuinf - mytolu)
      U += myS->UPeriod();
  }
  if (myS->IsVPeriodic()) {
    V = ElCLib::InPeriod(V, myvinf, myvinf + myS->VPeriod());

    // Handle trimmed surfaces.
    if (V > myvsup + mytolv)
      V -= myS->VPeriod();
    if (V < myvinf - mytolv)
      V += myS->VPeriod();
  }
  if ((myuinf-U) <= mytolu && (U-myusup) <= mytolu &&
      (myvinf-V) <= mytolv && (V-myvsup) <= mytolv) {
    myPoints.Append(Extrema_POnSurf (U, V, PS.Value()));
    mySqDist.Append(Val);
  }
}


//=======================================================================
//function : Extrema_ExtPS
//purpose  : 
//=======================================================================

Extrema_ExtPS::Extrema_ExtPS()
{
  myDone = Standard_False;
}


//=======================================================================
//function : Extrema_ExtPS
//purpose  : 
//=======================================================================

Extrema_ExtPS::Extrema_ExtPS (const gp_Pnt&            theP,
                              const Adaptor3d_Surface& theS,
                              const Standard_Real      theTolU,
                              const Standard_Real      theTolV,
                              const Extrema_ExtFlag    theF,
                              const Extrema_ExtAlgo    theA)
{
  myExtPS.SetFlag (theF);
  myExtPS.SetAlgo (theA);

  Initialize (theS,
              theS.FirstUParameter(),
              theS.LastUParameter(),
              theS.FirstVParameter(),
              theS.LastVParameter(),
              theTolU,
              theTolV);

  Perform (theP);
}

//=======================================================================
//function : Extrema_ExtPS
//purpose  : 
//=======================================================================

Extrema_ExtPS::Extrema_ExtPS (const gp_Pnt&            theP,
                              const Adaptor3d_Surface& theS,
                              const Standard_Real      theUinf,
                              const Standard_Real      theUsup,
                              const Standard_Real      theVinf,
                              const Standard_Real      theVsup,
                              const Standard_Real      theTolU,
                              const Standard_Real      theTolV,
                              const Extrema_ExtFlag    theF,
                              const Extrema_ExtAlgo    theA)
{
  myExtPS.SetFlag (theF);
  myExtPS.SetAlgo (theA);

  Initialize (theS,
              theUinf,
              theUsup,
              theVinf,
              theVsup,
              theTolU,
              theTolV);

  Perform (theP);
}


//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void Extrema_ExtPS::Initialize (const Adaptor3d_Surface& theS,
                                const Standard_Real      theUinf,
                                const Standard_Real      theUsup,
                                const Standard_Real      theVinf,
                                const Standard_Real      theVsup,
                                const Standard_Real      theTolU,
                                const Standard_Real      theTolV)
{
  myS = (Adaptor3d_SurfacePtr)&theS;
  myuinf = theUinf;
  myusup = theUsup;
  myvinf = theVinf;
  myvsup = theVsup;

  if (Precision::IsNegativeInfinite(myuinf)) myuinf = -1e10;
  if (Precision::IsPositiveInfinite(myusup)) myusup = 1e10;
  if (Precision::IsNegativeInfinite(myvinf)) myvinf = -1e10;
  if (Precision::IsPositiveInfinite(myvsup)) myvsup = 1e10;

  mytolu = theTolU;
  mytolv = theTolV;
  mytype = myS->GetType();

  Standard_Boolean isB = ( myS->GetType() == GeomAbs_BSplineSurface ||
                           myS->GetType() == GeomAbs_BezierSurface );

  Standard_Integer nbU = (isB) ? 44 : 32;
  Standard_Integer nbV = (isB) ? 44 : 32;

  Standard_Boolean bUIsoIsDeg = Standard_False, bVIsoIsDeg = Standard_False;

  if(myS->GetType() != GeomAbs_Plane) {
    bUIsoIsDeg = IsoIsDeg(theS, myuinf, GeomAbs_IsoU, 0., 1.e-9) ||
                 IsoIsDeg(theS, myusup, GeomAbs_IsoU, 0., 1.e-9);
    bVIsoIsDeg = IsoIsDeg(theS, myvinf, GeomAbs_IsoV, 0., 1.e-9) ||
                 IsoIsDeg(theS, myvsup, GeomAbs_IsoV, 0., 1.e-9);
  }

  if(bUIsoIsDeg) nbU = 300;
  if(bVIsoIsDeg) nbV = 300;

  myExtPS.Initialize(*myS, nbU, nbV, myuinf, myusup, myvinf, myvsup, mytolu, mytolv);

  myExtPExtS.Nullify();
  myExtPRevS.Nullify();
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void Extrema_ExtPS::Perform(const gp_Pnt& thePoint)
{
  myPoints.Clear();
  mySqDist.Clear();

  switch (mytype)
  {
    case GeomAbs_Cylinder:
      myExtPElS.Perform (thePoint, myS->Cylinder(), Precision::Confusion());
      break;
    case GeomAbs_Plane:
      myExtPElS.Perform (thePoint, myS->Plane(), Precision::Confusion());
      break;
    case GeomAbs_Cone:
      myExtPElS.Perform (thePoint, myS->Cone(), Precision::Confusion());
      break;
    case GeomAbs_Sphere:
      myExtPElS.Perform (thePoint, myS->Sphere(), Precision::Confusion());
      break;
    case GeomAbs_Torus:
      myExtPElS.Perform (thePoint, myS->Torus(), Precision::Confusion());
      break;

    case GeomAbs_SurfaceOfExtrusion:
    {
      if (myExtPExtS.IsNull())
      {
        Handle(GeomAdaptor_HSurfaceOfLinearExtrusion) aS (new GeomAdaptor_HSurfaceOfLinearExtrusion (
          GeomAdaptor_SurfaceOfLinearExtrusion (myS->BasisCurve(), myS->Direction())));

        myExtPExtS = new Extrema_ExtPExtS (thePoint, aS, myuinf, myusup, myvinf, myvsup, mytolu, mytolv);
      }
      else
      {
        myExtPExtS->Perform (thePoint);
      }

      myDone = myExtPExtS->IsDone();
      if (myDone)
      {
        for (Standard_Integer anIdx = 1; anIdx <= myExtPExtS->NbExt(); ++anIdx)
        {
          TreatSolution (myExtPExtS->Point (anIdx), myExtPExtS->SquareDistance (anIdx));
        }
      }

      return;
    }

    case GeomAbs_SurfaceOfRevolution:
    {
      if (myExtPRevS.IsNull())
      {
        Handle(GeomAdaptor_HSurfaceOfRevolution) aS (new GeomAdaptor_HSurfaceOfRevolution (
          GeomAdaptor_SurfaceOfRevolution (myS->BasisCurve(), myS->AxeOfRevolution())));

        myExtPRevS = new Extrema_ExtPRevS (thePoint, aS, myuinf, myusup, myvinf, myvsup, mytolu, mytolv);
      }
      else
      {
        myExtPRevS->Perform (thePoint);
      }

      myDone = myExtPRevS->IsDone();
      if (myDone)
      {
        for (Standard_Integer anIdx = 1; anIdx <= myExtPRevS->NbExt(); ++anIdx)
        {
          TreatSolution (myExtPRevS->Point (anIdx), myExtPRevS->SquareDistance (anIdx));
        }
      }

      return;
    }

    default:
    {
      myExtPS.Perform (thePoint);
      myDone = myExtPS.IsDone();
      if (myDone)
      {
        for (Standard_Integer anIdx = 1; anIdx <= myExtPS.NbExt(); ++anIdx)
        {
          TreatSolution (myExtPS.Point (anIdx), myExtPS.SquareDistance (anIdx));
        }
      }
      return;
    }
  }

  myDone = myExtPElS.IsDone();
  if (myDone)
  {
    for (Standard_Integer anIdx = 1; anIdx <= myExtPElS.NbExt(); ++anIdx)
    {
      TreatSolution (myExtPElS.Point (anIdx), myExtPElS.SquareDistance (anIdx));
    }
  }
}


Standard_Boolean Extrema_ExtPS::IsDone() const
{
  return myDone;
}


Standard_Real Extrema_ExtPS::SquareDistance(const Standard_Integer N) const
{
  if(!myDone) throw StdFail_NotDone();
  if ((N < 1) || (N > mySqDist.Length())) throw Standard_OutOfRange();
  return mySqDist.Value(N);
}


Standard_Integer Extrema_ExtPS::NbExt() const
{
  if(!myDone) throw StdFail_NotDone();
  return mySqDist.Length();
}


const Extrema_POnSurf& Extrema_ExtPS::Point(const Standard_Integer N) const
{
  if(!myDone) throw StdFail_NotDone();
  return myPoints.Value(N);
}


void Extrema_ExtPS::TrimmedSquareDistances(Standard_Real& dUfVf,
				      Standard_Real& dUfVl,
				      Standard_Real& dUlVf,
				      Standard_Real& dUlVl,
				      gp_Pnt&        PUfVf,
				      gp_Pnt&        PUfVl,
				      gp_Pnt&        PUlVf,
				      gp_Pnt&        PUlVl) const
{
  dUfVf = d11;
  dUfVl = d12;
  dUlVf = d21;
  dUlVl = d22;
  PUfVf = P11;
  PUfVl = P12;
  PUlVf = P21;
  PUlVl = P22;
}

void Extrema_ExtPS::SetFlag(const Extrema_ExtFlag F)
{
  myExtPS.SetFlag(F);
}

void Extrema_ExtPS::SetAlgo(const Extrema_ExtAlgo A)
{
  myExtPS.SetAlgo(A);
}
