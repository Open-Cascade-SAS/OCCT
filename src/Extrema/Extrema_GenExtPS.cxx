// Created on: 1995-07-18
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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


//  Modified by skv - Thu Sep 30 15:21:07 2004 OCC593


#include <Extrema_GenExtPS.ixx>
#include <StdFail_NotDone.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_Array2OfInteger.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_Vector.hxx>
#include <math_NewtonFunctionSetRoot.hxx>
#include <GeomAbs_IsoType.hxx>
#include <Bnd_Sphere.hxx>
#include <Extrema_HUBTreeOfSphere.hxx>
#include <Extrema_ExtFlag.hxx>
#include <Bnd_Array1OfSphere.hxx>
#include <Bnd_HArray1OfSphere.hxx>
#include <Precision.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
//IMPLEMENT_HARRAY1(Extrema_HArray1OfSphere)


class Bnd_SphereUBTreeSelector : public Extrema_UBTreeOfSphere::Selector
{
 public:

  Bnd_SphereUBTreeSelector (const Handle(Bnd_HArray1OfSphere)& theSphereArray,
Bnd_Sphere& theSol)
    : myXYZ(0,0,0),
      mySphereArray(theSphereArray),
      mySol(theSol)
  {
    //myXYZ = gp_Pnt(0, 0, 0);    
  }

  void DefineCheckPoint( const gp_Pnt& theXYZ )
  { myXYZ = theXYZ; }

  Bnd_Sphere& Sphere() const
  { return mySol; }

  virtual Standard_Boolean Reject( const Bnd_Sphere &theBnd ) const = 0;
  
  virtual Standard_Boolean Accept(const Standard_Integer& theObj) = 0;

 protected:
  gp_Pnt                      myXYZ;
  const Handle(Bnd_HArray1OfSphere)& mySphereArray;
  Bnd_Sphere&						 mySol;

};

class Bnd_SphereUBTreeSelectorMin : public Bnd_SphereUBTreeSelector
{
public:
  Bnd_SphereUBTreeSelectorMin (const Handle(Bnd_HArray1OfSphere)& theSphereArray,
		Bnd_Sphere& theSol)
		: Bnd_SphereUBTreeSelector(theSphereArray, theSol),
		  myMinDist(RealLast())
  {}
  
  void SetMinDist( const Standard_Real theMinDist )
  { myMinDist = theMinDist; }

  Standard_Real MinDist() const
  { return myMinDist; }

  Standard_Boolean Reject( const Bnd_Sphere &theBnd ) const
  { 
    Bnd_SphereUBTreeSelectorMin* me =
      const_cast<Bnd_SphereUBTreeSelectorMin*>(this);
    // myMinDist is decreased each time a nearer object is found
    return theBnd.IsOut( myXYZ.XYZ(), me->myMinDist );
  }

  Standard_Boolean Accept(const Standard_Integer&);

private:
	Standard_Real	myMinDist;
};

Standard_Boolean Bnd_SphereUBTreeSelectorMin::Accept(const Standard_Integer& theInd)
{
  const Bnd_Sphere& aSph = mySphereArray->Value(theInd);
  Standard_Real aCurDist;

    if ( (aCurDist = aSph.SquareDistance(myXYZ.XYZ())) < mySol.SquareDistance(myXYZ.XYZ()) )
    {
      mySol = aSph;
      if ( aCurDist < myMinDist ) 
        myMinDist = aCurDist;

      return Standard_True;
    }

  return Standard_False;
}

class Bnd_SphereUBTreeSelectorMax : public Bnd_SphereUBTreeSelector
{
public:
  Bnd_SphereUBTreeSelectorMax (const Handle(Bnd_HArray1OfSphere)& theSphereArray,
		Bnd_Sphere& theSol)
		: Bnd_SphereUBTreeSelector(theSphereArray, theSol),
		  myMaxDist(0)
  {}

  void SetMaxDist( const Standard_Real theMaxDist )
  { myMaxDist = theMaxDist; }

  Standard_Real MaxDist() const
  { return myMaxDist; }

  Standard_Boolean Reject( const Bnd_Sphere &theBnd ) const
  { 
    Bnd_SphereUBTreeSelectorMax* me =
      const_cast<Bnd_SphereUBTreeSelectorMax*>(this);
    // myMaxDist is decreased each time a nearer object is found
    return theBnd.IsOut( myXYZ.XYZ(), me->myMaxDist );
  }

  Standard_Boolean Accept(const Standard_Integer&);

private:
	Standard_Real	myMaxDist;
};

Standard_Boolean Bnd_SphereUBTreeSelectorMax::Accept(const Standard_Integer& theInd)
{
  const Bnd_Sphere& aSph = mySphereArray->Value(theInd);
  Standard_Real aCurDist;

    if ( (aCurDist = aSph.SquareDistance(myXYZ.XYZ())) > mySol.SquareDistance(myXYZ.XYZ()) )
    {
      mySol = aSph;
      if ( aCurDist > myMaxDist ) 
        myMaxDist = aCurDist;

      return Standard_True;
    }

  return Standard_False;
}



//=============================================================================

/*-----------------------------------------------------------------------------
Function:
   Find all extremum distances between point P and surface
  S using sampling (NbU,NbV).

Method:
   The algorithm bases on the hypothesis that sampling is precise enough, 
  if there exist N extreme distances between the point and the surface,
  so there also exist N extrema between the point and the grid.
  So, the algorithm consists in starting from extrema of the grid to find the 
  extrema of the surface.
  The extrema are calculated by the algorithm math_FunctionSetRoot with the
  following arguments:
  - F: Extrema_FuncExtPS created from P and S,
  - UV: math_Vector the components which of are parameters of the extremum on the 
    grid,
  - Tol: Min(TolU,TolV), (Prov.:math_FunctionSetRoot does not autorize a vector)
  - UVinf: math_Vector the components which of are lower limits of u and v,
  - UVsup: math_Vector the components which of are upper limits of u and v.

Processing:
  a- Creation of the table of distances (TbDist(0,NbU+1,0,NbV+1)):
     The table is expanded at will; lines 0 and NbU+1 and
     columns 0 and NbV+1 are initialized at RealFirst() or RealLast()
     to simplify the tests carried out at stage b
     (there is no need to test if the point is on border of the grid).
  b- Calculation of extrema:
     First the minimums and then the maximums are found. These 2 procedured 
     pass in a similar way:
  b.a- Initialization:
      - 'borders' of table  TbDist (RealLast() in case of minimums
        and  RealLast() in case of maximums),
      - table TbSel(0,NbU+1,0,NbV+1) of selection of points for 
        calculation of local extremum (0). When a point will selected,
	it will not be selectable, as well as the ajacent points 
	(8 at least). The corresponding addresses will be set to 1.
  b.b- Calculation of minimums (or maximums):
       All distances from table TbDist are parsed in a loop:
      - search minimum (or maximum) in the grid,
      - calculate extremum on the surface,
      - update table TbSel.
-----------------------------------------------------------------------------*/

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
      Step = (U2 - U1)/10;
      D1NormMax=0.;
      for (T=U1;T<=U2;T=T+Step) 
      {
        S.D1(T,Param,P,D1U,D1V);
        D1NormMax=Max(D1NormMax,D1U.Magnitude());
      }

      if (D1NormMax >TolMax || D1NormMax < TolMin ) 
           Along = Standard_False;
    }
    else 
    {
      Step = (V2 - V1)/10;
      D1NormMax=0.;
      for (T=V1;T<=V2;T=T+Step) 
      {
	S.D1(Param,T,P,D1U,D1V);
        D1NormMax=Max(D1NormMax,D1V.Magnitude());
      }

      if (D1NormMax >TolMax || D1NormMax < TolMin ) 
           Along = Standard_False;


    }
    return Along;
}
//----------------------------------------------------------
Extrema_GenExtPS::Extrema_GenExtPS() 
{
  myDone = Standard_False;
  myInit = Standard_False;
  myFlag = Extrema_ExtFlag_MINMAX;
  myAlgo = Extrema_ExtAlgo_Grad;
}


Extrema_GenExtPS::Extrema_GenExtPS (const gp_Pnt&          P,
			      const Adaptor3d_Surface& S,
			      const Standard_Integer NbU, 
			      const Standard_Integer NbV,
			      const Standard_Real    TolU, 
                              const Standard_Real    TolV,
                              const Extrema_ExtFlag F,
                              const Extrema_ExtAlgo A) 
  : myF (P,S), myFlag(F), myAlgo(A)
{
  Initialize(S, NbU, NbV, TolU, TolV);
  Perform(P);
}

Extrema_GenExtPS::Extrema_GenExtPS (const gp_Pnt&          P,
			      const Adaptor3d_Surface& S,
			      const Standard_Integer NbU, 
			      const Standard_Integer NbV,
			      const Standard_Real    Umin,
			      const Standard_Real    Usup,
			      const Standard_Real    Vmin,
			      const Standard_Real    Vsup,
			      const Standard_Real    TolU, 
                              const Standard_Real    TolV,
                              const Extrema_ExtFlag F,
                              const Extrema_ExtAlgo A) 
  : myF (P,S), myFlag(F), myAlgo(A)
{
  Initialize(S, NbU, NbV, Umin, Usup, Vmin, Vsup, TolU, TolV);
  Perform(P);
}


void Extrema_GenExtPS::Initialize(const Adaptor3d_Surface& S,
				  const Standard_Integer NbU, 
				  const Standard_Integer NbV,
				  const Standard_Real    TolU, 
				  const Standard_Real    TolV)
{
  myumin = S.FirstUParameter();
  myusup = S.LastUParameter();
  myvmin = S.FirstVParameter();
  myvsup = S.LastVParameter();
  Initialize(S,NbU,NbV,myumin,myusup,myvmin,myvsup,TolU,TolV);
}


void Extrema_GenExtPS::Initialize(const Adaptor3d_Surface& S,
				  const Standard_Integer NbU, 
				  const Standard_Integer NbV,
				  const Standard_Real    Umin,
				  const Standard_Real    Usup,
				  const Standard_Real    Vmin,
				  const Standard_Real    Vsup,
				  const Standard_Real    TolU, 
				  const Standard_Real    TolV)
{
  myS = (Adaptor3d_SurfacePtr)&S;
  myusample = NbU;
  myvsample = NbV;
  mytolu = TolU;
  mytolv = TolV;
  myumin = Umin;
  myusup = Usup;
  myvmin = Vmin;
  myvsup = Vsup;

  if ((myusample < 2) ||
      (myvsample < 2)) { Standard_OutOfRange::Raise(); }

  myF.Initialize(S);

  mySphereUBTree.Nullify();
  myUParams.Nullify();
  myVParams.Nullify();
  myInit = Standard_False;
}

inline static void fillParams(const TColStd_Array1OfReal& theKnots,
                              Standard_Integer theDegree,
                              Standard_Real theParMin,
                              Standard_Real theParMax,
                              Handle_TColStd_HArray1OfReal& theParams,
                              Standard_Integer theSample)
{
  NCollection_Vector<Standard_Real> aParams;
  Standard_Integer i = 1;
  Standard_Real aPrevPar = theParMin;
  aParams.Append(aPrevPar);
  //calculation the array of parametric points depending on the knots array variation and degree of given surface
  for ( ; i <  theKnots.Length() && theKnots(i) < (theParMax - Precision::PConfusion()); i++ )
  {
    if (  theKnots(i+1) < theParMin + Precision::PConfusion())
      continue;

    Standard_Real aStep = (theKnots(i+1) - theKnots(i))/Max(theDegree,2);
    Standard_Integer k =1;
    for( ; k <= theDegree ; k++)
    {
      Standard_Real aPar = theKnots(i) + k * aStep;
      if(aPar > theParMax - Precision::PConfusion())
        break;
      if(aPar > aPrevPar + Precision::PConfusion() )
      {
        aParams.Append(aPar);
        aPrevPar = aPar;
      }
    }

  }
  aParams.Append(theParMax);
  Standard_Integer nbPar = aParams.Length();
  //in case of an insufficient number of points the grid will be built later 
  if (nbPar < theSample)
    return;
  theParams = new TColStd_HArray1OfReal(1, nbPar );
  for( i = 0; i < nbPar; i++)
    theParams->SetValue(i+1,aParams(i));
}

void Extrema_GenExtPS::GetGridPoints( const Adaptor3d_Surface& theSurf)
{
  //creation parametric points for BSpline and Bezier surfaces
  //with taking into account of Degree and NbKnots of BSpline or Bezier geometry
  if(theSurf.GetType() == GeomAbs_OffsetSurface)
    GetGridPoints(theSurf.BasisSurface()->Surface());
  //parametric points for BSpline surfaces
  else if( theSurf.GetType() == GeomAbs_BSplineSurface) 
  {
    Handle(Geom_BSplineSurface) aBspl = theSurf.BSpline();
    if(!aBspl.IsNull())
    {
      TColStd_Array1OfReal aUKnots(1, aBspl->NbUKnots());
      aBspl->UKnots( aUKnots);
      TColStd_Array1OfReal aVKnots(1, aBspl->NbVKnots());
      aBspl->VKnots( aVKnots);
      fillParams(aUKnots,aBspl->UDegree(),myumin, myusup, myUParams, myusample);
      fillParams(aVKnots,aBspl->VDegree(),myvmin, myvsup, myVParams, myvsample);
    }
  }
  //calculation parametric points for Bezier surfaces
  else if(theSurf.GetType() == GeomAbs_BezierSurface)
  {
    Handle(Geom_BezierSurface) aBezier = theSurf.Bezier();
    if(aBezier.IsNull())
      return;

    TColStd_Array1OfReal aUKnots(1,2);
    TColStd_Array1OfReal aVKnots(1,2);
    aBezier->Bounds(aUKnots(1), aUKnots(2), aVKnots(1), aVKnots(2));
    fillParams(aUKnots, aBezier->UDegree(), myumin, myusup, myUParams, myusample);
    fillParams(aVKnots, aBezier->VDegree(), myvmin, myvsup, myVParams, myvsample);

  }
  //creation points for surfaces based on BSpline or Bezier curves
  else if(theSurf.GetType() == GeomAbs_SurfaceOfRevolution || 
    theSurf.GetType() == GeomAbs_SurfaceOfExtrusion)
  {
    Handle(TColStd_HArray1OfReal) anArrKnots;
    Standard_Integer aDegree = 0;
    GeomAbs_CurveType aType = theSurf.BasisCurve()->Curve().GetType();
    if(theSurf.BasisCurve()->Curve().GetType() == GeomAbs_BSplineCurve)
    {
      Handle(Geom_BSplineCurve) aBspl = theSurf.BasisCurve()->Curve().BSpline();
      if(!aBspl.IsNull())
      {
        anArrKnots = new TColStd_HArray1OfReal(1,aBspl->NbKnots());
        aBspl->Knots( anArrKnots->ChangeArray1() );
        aDegree = aBspl->Degree();
        
      }

    }
    if(theSurf.BasisCurve()->Curve().GetType() == GeomAbs_BezierCurve)
    {
      Handle(Geom_BezierCurve) aBez = theSurf.BasisCurve()->Curve().Bezier();
      if(!aBez.IsNull())
      {
        anArrKnots = new TColStd_HArray1OfReal(1,2);
        anArrKnots->SetValue(1, aBez->FirstParameter());
        anArrKnots->SetValue(2, aBez->LastParameter());
        aDegree = aBez->Degree();
        
      }
    }
    if(anArrKnots.IsNull())
      return;
    if( theSurf.GetType() == GeomAbs_SurfaceOfRevolution )
      fillParams( anArrKnots->Array1(), aDegree, myvmin, myvsup, myVParams, myvsample);
    else
      fillParams( anArrKnots->Array1(), aDegree, myumin, myusup, myUParams, myusample);
  }
  //update the number of points in sample
  if(!myUParams.IsNull())
    myusample = myUParams->Length();
  if( !myVParams.IsNull())
    myvsample = myVParams->Length();
  
}

void Extrema_GenExtPS::BuildGrid()
{
  //if grid was already built do nothing
  if(myInit)
    return;
  
  //build parametric grid in case of a complex surface geometry (BSpline and Bezier surfaces)
 GetGridPoints(*myS);
  
  //build grid in other cases 
  if( myUParams.IsNull() )
  {
    Standard_Real PasU = myusup - myumin;
    Standard_Real U0 = PasU / myusample / 100.;
    PasU = (PasU - U0) / (myusample - 1);
    U0 = U0/2. + myumin;
    myUParams = new TColStd_HArray1OfReal(1,myusample );
    Standard_Integer NoU;
    Standard_Real U = U0;
    for ( NoU = 1 ; NoU <= myusample; NoU++, U += PasU) 
      myUParams->SetValue(NoU, U);

  }

  if( myVParams.IsNull())
  {
    Standard_Real PasV = myvsup - myvmin;
    Standard_Real V0 = PasV / myvsample / 100.;
    PasV = (PasV - V0) / (myvsample - 1);
    V0 = V0/2. + myvmin;
    
    myVParams = new TColStd_HArray1OfReal(1,myvsample );
    Standard_Integer  NoV;
    Standard_Real V = V0;
   
    for ( NoV = 1, V = V0; NoV <= myvsample; NoV++, V += PasV)
      myVParams->SetValue(NoV, V);
  }
  //If flag was changed and extrema not reinitialized Extrema would fail
  mypoints = new TColgp_HArray2OfPnt(0,myusample+1,0,myvsample+1);
  // Calculation of distances

  Standard_Integer NoU, NoV;
 
  for ( NoU = 1 ; NoU <= myusample; NoU++ ) {
    for ( NoV = 1 ; NoV <= myvsample; NoV++) {
      gp_Pnt P1 = myS->Value(myUParams->Value(NoU), myVParams->Value(NoV));
      mypoints->SetValue(NoU,NoV,P1);
    }
  }
  
  myInit = Standard_True;
}




  
/*
a- Constitution of the table of distances (TbDist(0,myusample+1,0,myvsample+1)):
   ---------------------------------------------------------------
*/

// Parameterisation of the sample

void Extrema_GenExtPS::BuildTree()
{
  // if tree already exists, assume it is already correctly filled
  if ( ! mySphereUBTree.IsNull() )
    return;

  Standard_Real PasU = myusup - myumin;
  Standard_Real PasV = myvsup - myvmin;
  Standard_Real U0 = PasU / myusample / 100.;
  Standard_Real V0 = PasV / myvsample / 100.;
  gp_Pnt P1;
  PasU = (PasU - U0) / (myusample - 1);
  PasV = (PasV - V0) / (myvsample - 1);
  U0 = U0/2. + myumin;
  V0 = V0/2. + myvmin;

  //build grid of parametric points 
  myUParams = new TColStd_HArray1OfReal(1,myusample );
  myVParams = new TColStd_HArray1OfReal(1,myvsample );
  Standard_Integer NoU, NoV;
  Standard_Real U = U0, V = V0;
  for ( NoU = 1 ; NoU <= myusample; NoU++, U += PasU) 
    myUParams->SetValue(NoU, U);
  for ( NoV = 1, V = V0; NoV <= myvsample; NoV++, V += PasV)
    myVParams->SetValue(NoV, V);

  // Calculation of distances
  mySphereUBTree = new Extrema_UBTreeOfSphere;
  Extrema_UBTreeFillerOfSphere aFiller(*mySphereUBTree);
  Standard_Integer i = 0;
  
  mySphereArray = new Bnd_HArray1OfSphere(0, myusample * myvsample);
 
  for ( NoU = 1; NoU <= myusample; NoU++ ) {
    for ( NoV = 1; NoV <= myvsample; NoV++) {
      P1 = myS->Value(myUParams->Value(NoU), myVParams->Value(NoV));
      Bnd_Sphere aSph(P1.XYZ(), 0/*mytolu < mytolv ? mytolu : mytolv*/, NoU, NoV);
      aFiller.Add(i, aSph);
      mySphereArray->SetValue( i, aSph );
      i++;
    }
  }
  aFiller.Fill();
}

void Extrema_GenExtPS::FindSolution(const gp_Pnt& P, 
                                    const math_Vector& UV, 
                                    const Standard_Integer theNoU, 
                                    const Standard_Integer theNoV, const Extrema_ExtFlag f)
{
  math_Vector Tol(1,2);
  Tol(1) = mytolu;
  Tol(2) = mytolv;

  math_Vector UVinf(1,2), UVsup(1,2);
  UVinf(1) = myumin;
  UVinf(2) = myvmin;
  UVsup(1) = myusup;
  UVsup(2) = myvsup;

  math_Vector errors(1,2);
  math_Vector root(1, 2);
  Standard_Real eps = 1.e-9;
  Standard_Integer nbsubsample = 11;

  Standard_Integer aNbMaxIter = 100;

  gp_Pnt PStart = myS->Value(UV(1), UV(2));
  Standard_Real DistStart = P.SquareDistance(PStart);
  Standard_Real DistSol = DistStart;
  
  math_FunctionSetRoot S (myF,UV,Tol,UVinf,UVsup, aNbMaxIter);
  Standard_Boolean ToResolveOnSubgrid = Standard_False;
  Standard_Boolean NewSolution = Standard_False;
  if (f == Extrema_ExtFlag_MIN)
  {
    if(S.IsDone())
    {
      root = S.Root();
      myF.Value(root, errors);
      gp_Pnt PSol = myS->Value(root(1), root(2));
      DistSol = P.SquareDistance(PSol);
      if(Abs(errors(1)) > eps || Abs(errors(2)) > eps || DistStart < DistSol)
      {
        //try to improve solution on subgrid of sample points
        ToResolveOnSubgrid = Standard_True;
      }
    }
    else
    {
      //keep found roots and try to find solution
      Standard_Integer nbExt = myF.NbExt();
      Standard_Integer k = 1;
      for( ; k <= nbExt; k++)
      {
        Standard_Real aD = myF.SquareDistance(k);
        if(aD < DistSol)
        {
          DistSol = aD;
          myF.Point(k).Parameter(UV(1),UV(2));
          NewSolution = Standard_True;
        }
      }
      ToResolveOnSubgrid = Standard_True;
    }
    
    if (ToResolveOnSubgrid)
    {
      //extension of interval to find new solution
      Standard_Real u1 = theNoU == 1 ? myumin : myUParams->Value(theNoU-1), 
        u2 = theNoU == myusample ? myusup : myUParams->Value(theNoU + 1);
      Standard_Real v1 = theNoV == 1 ? myvmin : myVParams->Value(theNoV-1),
        v2 = theNoV == myvsample ? myvsup : myVParams->Value(theNoV + 1);
            
      Standard_Real du = (u2 - u1)/(nbsubsample-1);
      Standard_Real dv = (v2 - v1)/(nbsubsample-1);
      Standard_Real u, v;
      Standard_Real dist;
      
      //try to find solution on subgrid
      Standard_Integer Nu, Nv;
      Standard_Integer minU = 0;
      Standard_Integer minV = 0;
      for (Nu = 1, u = u1; Nu < nbsubsample; Nu++, u += du) {
        for (Nv = 1, v = v1; Nv < nbsubsample; Nv++, v += dv) {
          gp_Pnt Puv = myS->Value(u, v);
          dist = P.SquareDistance(Puv);
          
          if(dist < DistSol) {
            UV(1) = u;
            UV(2) = v;
            NewSolution = Standard_True;
            DistSol = dist;
            minU = Nu;
            minV = Nv;
          }
        }
      }
      
      if(NewSolution) {
        //try to precise
        UVinf(1) = u1 + (minU == 1 ? 0 : minU - 2) * du;
        UVinf(2) = v1 + (minV == 1 ? 0 : minV - 2) * dv;
        UVsup(1) = u1 + (minU == nbsubsample ? nbsubsample : minU +1) * du;;
        UVsup(2) = v1 + (minV == nbsubsample ? nbsubsample : minV +1) * dv;
        math_FunctionSetRoot S (myF,UV,Tol,UVinf,UVsup, aNbMaxIter);

      }
    } //end of if (ToResolveOnSubgrid)
  } //end of if (f == Extrema_ExtFlag_MIN)
  
  myDone = Standard_True;
}

void Extrema_GenExtPS::SetFlag(const Extrema_ExtFlag F)
{
  myFlag = F;
}

void Extrema_GenExtPS::SetAlgo(const Extrema_ExtAlgo A)
{
  if(myAlgo != A)
     myInit = Standard_False;
  myAlgo = A;
 
}

void Extrema_GenExtPS::Perform(const gp_Pnt& P) 
{  
  myDone = Standard_False;
  myF.SetPoint(P);
  
  math_Vector UV(1,2);

  if(myAlgo == Extrema_ExtAlgo_Grad)
  {
    BuildGrid();
    Standard_Integer NoU,NoV;

    TColStd_Array2OfReal TheDist(0, myusample+1, 0, myvsample+1);
    
    for ( NoU = 1 ; NoU <= myusample; NoU++) {
      for ( NoV = 1; NoV <= myvsample; NoV++) {
        TheDist(NoU, NoV) = P.SquareDistance(mypoints->Value(NoU, NoV));
      }
    }

   

    Standard_Real Dist;

    if(myFlag == Extrema_ExtFlag_MIN || myFlag == Extrema_ExtFlag_MINMAX) 
    {
      for (NoV = 0; NoV <= myvsample+1; NoV++) {
        TheDist(0,NoV) = RealLast();
        TheDist(myusample+1,NoV) = RealLast();
      }
      for (NoU = 1; NoU <= myusample; NoU++) {
        TheDist(NoU,0) = RealLast();
        TheDist(NoU,myvsample+1) = RealLast();
      }
      for (NoU = 1; NoU <= myusample; NoU++) {
        for (NoV = 1; NoV <= myvsample; NoV++) {
            Dist = TheDist(NoU,NoV);
            if ((TheDist(NoU-1,NoV-1) >= Dist) &&
              (TheDist(NoU-1,NoV  ) >= Dist) &&
              (TheDist(NoU-1,NoV+1) >= Dist) &&
              (TheDist(NoU  ,NoV-1) >= Dist) &&
              (TheDist(NoU  ,NoV+1) >= Dist) &&
              (TheDist(NoU+1,NoV-1) >= Dist) &&
              (TheDist(NoU+1,NoV  ) >= Dist) &&
              (TheDist(NoU+1,NoV+1) >= Dist)) {
                //Create array of UV vectors to calculate min
                
                UV(1) = myUParams->Value(NoU);
                UV(2) = myVParams->Value(NoV);
                FindSolution(P, UV, NoU, NoV, Extrema_ExtFlag_MIN);
            }
         
        }
      }
    }
    
    if(myFlag == Extrema_ExtFlag_MAX || myFlag == Extrema_ExtFlag_MINMAX)
    {
      for (NoV = 0; NoV <= myvsample+1; NoV++) {
        TheDist(0,NoV) = RealFirst();
        TheDist(myusample+1,NoV) = RealFirst();
      }
      for (NoU = 1; NoU <= myusample; NoU++) {
        TheDist(NoU,0) = RealFirst();
        TheDist(NoU,myvsample+1) = RealFirst();
      }
      for (NoU = 1; NoU <= myusample; NoU++) {
        for (NoV = 1; NoV <= myvsample; NoV++) {
            Dist = TheDist(NoU,NoV);
            if ((TheDist(NoU-1,NoV-1) <= Dist) &&
              (TheDist(NoU-1,NoV  ) <= Dist) &&
              (TheDist(NoU-1,NoV+1) <= Dist) &&
              (TheDist(NoU  ,NoV-1) <= Dist) &&
              (TheDist(NoU  ,NoV+1) <= Dist) &&
              (TheDist(NoU+1,NoV-1) <= Dist) &&
              (TheDist(NoU+1,NoV  ) <= Dist) &&
              (TheDist(NoU+1,NoV+1) <= Dist)) {
                //Create array of UV vectors to calculate max
                UV(1) = myUParams->Value(NoU);
                UV(2) = myVParams->Value(NoV);
                FindSolution(P, UV, NoU, NoV, Extrema_ExtFlag_MAX);
            }
         }
      }
    }
  }
  else
  {
    BuildTree();
    if(myFlag == Extrema_ExtFlag_MIN || myFlag == Extrema_ExtFlag_MINMAX)
    {
      Bnd_Sphere aSol = mySphereArray->Value(0);
      Bnd_SphereUBTreeSelectorMin aSelector(mySphereArray, aSol);
      //aSelector.SetMaxDist( RealLast() );
      aSelector.DefineCheckPoint( P );
      Standard_Integer aNbSel = mySphereUBTree->Select( aSelector );
      //TODO: check if no solution in binary tree
      Bnd_Sphere& aSph = aSelector.Sphere();

      UV(1) = myUParams->Value(aSph.U());//U0 + (aSph.U() - 1) * PasU;
      UV(2) = myVParams->Value(aSph.V());//V0 + (aSph.V() - 1) * PasV;

      //FindSolution(P, UV, PasU, PasV, Extrema_ExtFlag_MIN);
      FindSolution(P, UV, aSph.U(), aSph.V(), Extrema_ExtFlag_MIN);
    }
    if(myFlag == Extrema_ExtFlag_MAX || myFlag == Extrema_ExtFlag_MINMAX)
    {
      Bnd_Sphere aSol = mySphereArray->Value(0);
      Bnd_SphereUBTreeSelectorMax aSelector(mySphereArray, aSol);
      //aSelector.SetMaxDist( RealLast() );
      aSelector.DefineCheckPoint( P );
      Standard_Integer aNbSel = mySphereUBTree->Select( aSelector );
      //TODO: check if no solution in binary tree
      Bnd_Sphere& aSph = aSelector.Sphere();
      UV(1) = myUParams->Value(aSph.U());
      UV(2) = myVParams->Value(aSph.V());
      //UV(1) = U0 + (aSph.U() - 1) * PasU;
      //UV(2) = V0 + (aSph.V() - 1) * PasV;

      FindSolution(P, UV, aSph.U(),aSph.V(), Extrema_ExtFlag_MAX);
    }
  }
}
//=============================================================================

Standard_Boolean Extrema_GenExtPS::IsDone () const { return myDone; }
//=============================================================================

Standard_Integer Extrema_GenExtPS::NbExt () const
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myF.NbExt();
}
//=============================================================================

Standard_Real Extrema_GenExtPS::SquareDistance (const Standard_Integer N) const
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myF.SquareDistance(N);
}
//=============================================================================

Extrema_POnSurf Extrema_GenExtPS::Point (const Standard_Integer N) const
{
  if (!IsDone()) { StdFail_NotDone::Raise(); }
  return myF.Point(N);
}
//=============================================================================
