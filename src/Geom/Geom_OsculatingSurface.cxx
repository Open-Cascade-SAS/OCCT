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


#include <BSplSLib.hxx>
#include <Convert_GridPolynomialToPoles.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_OsculatingSurface.hxx>
#include <Geom_Surface.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_Array2OfVec.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_OsculatingSurface,Standard_Transient)

//=======================================================================
//function : Geom_OffsetOsculatingSurface
//purpose  : 
//=======================================================================
Geom_OsculatingSurface::Geom_OsculatingSurface()
  : myAlong(1,4)
{
  myAlong.Init(Standard_False);
}
//=======================================================================
//function : Geom_OffsetOsculatingSurface
//purpose  : 
//=======================================================================

Geom_OsculatingSurface::Geom_OsculatingSurface(const Handle(Geom_Surface)& BS, 
  const Standard_Real Tol)
  : myAlong(1,4)
{
  Init(BS,Tol);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Geom_OsculatingSurface::Init(const Handle(Geom_Surface)& BS,
  const Standard_Real Tol)
{
  ClearOsculFlags();
  myTol=Tol; 
  Standard_Real TolMin=0.;//consider all singularities below Tol, not just above 1.e-12 (id23943)
  Standard_Boolean OsculSurf = Standard_True;
  myBasisSurf = Handle(Geom_Surface)::DownCast(BS->Copy());
  myOsculSurf1 = new Geom_HSequenceOfBSplineSurface();
  myOsculSurf2 = new Geom_HSequenceOfBSplineSurface();
  if ((BS->IsKind(STANDARD_TYPE(Geom_BSplineSurface))) || 
    (BS->IsKind(STANDARD_TYPE(Geom_BezierSurface)))) 
  {
    Standard_Real U1=0,U2=0,V1=0,V2=0;

    Standard_Integer i = 1;
    BS->Bounds(U1,U2,V1,V2);
    myAlong.SetValue(1,IsQPunctual(BS,V1,GeomAbs_IsoV,TolMin,Tol));
    myAlong.SetValue(2,IsQPunctual(BS,V2,GeomAbs_IsoV,TolMin,Tol));
    myAlong.SetValue(3,IsQPunctual(BS,U1,GeomAbs_IsoU,TolMin,Tol));
    myAlong.SetValue(4,IsQPunctual(BS,U2,GeomAbs_IsoU,TolMin,Tol));
#ifdef OCCT_DEBUG
    cout<<myAlong(1)<<endl<<myAlong(2)<<endl<<myAlong(3)<<endl<<myAlong(4)<<endl;
#endif
    if (myAlong(1) || myAlong(2) || myAlong(3) || myAlong(4)) 
    {
      Handle(Geom_BSplineSurface) InitSurf, L,S;
      if (BS->IsKind(STANDARD_TYPE(Geom_BezierSurface))) 
      {
        Handle(Geom_BezierSurface) BzS = Handle(Geom_BezierSurface)::DownCast(BS);
        TColgp_Array2OfPnt P(1,BzS->NbUPoles(),1,BzS->NbVPoles());
        TColStd_Array1OfReal UKnots(1,2);
        TColStd_Array1OfReal VKnots(1,2);
        TColStd_Array1OfInteger UMults(1,2);
        TColStd_Array1OfInteger VMults(1,2);
        for (i=1;i<=2;i++)
        {
          UKnots.SetValue(i,(i-1));
          VKnots.SetValue(i,(i-1));
          UMults.SetValue(i,BzS->UDegree()+1);
          VMults.SetValue(i,BzS->VDegree()+1);
        }
        BzS->Poles(P);
        InitSurf = new Geom_BSplineSurface(P,UKnots,VKnots,
          UMults,VMults,
          BzS->UDegree(),
          BzS->VDegree(),
          BzS->IsUPeriodic(),
          BzS->IsVPeriodic());
      }
      else 
      {
        InitSurf = Handle(Geom_BSplineSurface)::DownCast(myBasisSurf);
      }
#ifdef OCCT_DEBUG
      cout<<"UDEG: "<<InitSurf->UDegree()<<endl;
      cout<<"VDEG: "<<InitSurf->VDegree()<<endl;
#endif

      if(IsAlongU() && IsAlongV()) ClearOsculFlags();
      //      Standard_ConstructionError_Raise_if((IsAlongU() && IsAlongV()),"Geom_OsculatingSurface");
      if ((IsAlongU() && InitSurf->VDegree()>1) ||
        (IsAlongV() && InitSurf->UDegree()>1)) 
      {
        myKdeg = new TColStd_HSequenceOfInteger();
        Standard_Integer k=0;
        Standard_Boolean IsQPunc;
        Standard_Integer UKnot,VKnot;
        if (myAlong(1) || myAlong(2)) 
        {
          for (i=1;i<InitSurf->NbUKnots();i++) 
          {
            if (myAlong(1)) 
            {
              S = InitSurf; k=0; IsQPunc=Standard_True;
              UKnot=i;
              VKnot=1;
              while(IsQPunc) 
              {
                OsculSurf  = BuildOsculatingSurface(V1,UKnot,VKnot,S,L);
                if(!OsculSurf) break;
                k++;
#ifdef OCCT_DEBUG
                cout<<"1.k = "<<k<<endl;
#endif
                IsQPunc=IsQPunctual(L,V1,GeomAbs_IsoV,0.,Tol);
                UKnot=1;
                VKnot=1;
                S=L;

              }
              if (OsculSurf)
                myOsculSurf1->Append(L);
              else
                ClearOsculFlags(); //myAlong.SetValue(1,Standard_False);
              if (myAlong(2) && OsculSurf) 
              {
                S = InitSurf; k=0; IsQPunc=Standard_True;
                UKnot=i;
                VKnot=InitSurf->NbVKnots()-1;

                while(IsQPunc) 
                {
                  OsculSurf = BuildOsculatingSurface(V2,UKnot,VKnot,S,L);
                  if(!OsculSurf) break;
                  k++;
#ifdef OCCT_DEBUG
                  cout<<"2.k = "<<k<<endl;
#endif
                  IsQPunc=IsQPunctual(L,V2,GeomAbs_IsoV,0.,Tol);
                  UKnot=1;
                  VKnot=1;
                  S=L;
                }
                if(OsculSurf)
                {
                  myOsculSurf2->Append(L);
                  myKdeg->Append(k);
                }
              } 
            }
            else 
              //if (myAlong(2)) 
            {
              S = InitSurf; k=0; IsQPunc=Standard_True;
              UKnot=i;
              VKnot=InitSurf->NbVKnots()-1;
              while(IsQPunc) 
              {
                OsculSurf = BuildOsculatingSurface(V2,UKnot,VKnot,S,L);
                if(!OsculSurf) break;
                k++;
#ifdef OCCT_DEBUG
                cout<<"2.k = "<<k<<endl;
#endif
                IsQPunc=IsQPunctual(L,V2,GeomAbs_IsoV,0.,Tol);
                UKnot=1;
                VKnot=1;
                S=L;

              }
              if(OsculSurf)
              {
                myOsculSurf2->Append(L);
                myKdeg->Append(k);
              }
              else
                ClearOsculFlags(); //myAlong.SetValue(2,Standard_False);
            }
          }
        }
        if (myAlong(3) || myAlong(4)) 
        {
          for (i=1;i<InitSurf->NbVKnots();i++) 
          {
            if (myAlong(3)) 
            {
              S = InitSurf; k=0; IsQPunc=Standard_True;
              UKnot=1;
              VKnot=i;
              while(IsQPunc) 
              {
                OsculSurf = BuildOsculatingSurface(U1,UKnot,VKnot,S,L);
                if(!OsculSurf) break;
                k++;
#ifdef OCCT_DEBUG
                cout<<"1.k = "<<k<<endl;
#endif
                IsQPunc=IsQPunctual(L,U1,GeomAbs_IsoU,0.,Tol);
                UKnot=1;
                VKnot=1;
                S=L;
              }
              if(OsculSurf)
                myOsculSurf1->Append(L);
              else
                ClearOsculFlags(); //myAlong.SetValue(3,Standard_False);
              if (myAlong(4) && OsculSurf )
              {
                S = InitSurf; k=0; IsQPunc=Standard_True;
                UKnot=InitSurf->NbUKnots()-1;
                VKnot=i;
                while(IsQPunc) 
                {
                  OsculSurf  = BuildOsculatingSurface(U2,UKnot,VKnot,S,L);
                  if(!OsculSurf) break;
                  k++;
#ifdef OCCT_DEBUG
                  cout<<"2.k = "<<k<<endl;
#endif
                  IsQPunc=IsQPunctual(L,U2,GeomAbs_IsoU,0.,Tol);
                  UKnot=1;
                  VKnot=1;
                  S=L;
                }
                if(OsculSurf)
                {
                  myOsculSurf2->Append(L);
                  myKdeg->Append(k);
                }
              }
            }
            else 
            {
              S = InitSurf; k=0; IsQPunc=Standard_True;
              UKnot=InitSurf->NbUKnots()-1;
              VKnot=i;
              while(IsQPunc) 
              {
                OsculSurf  = BuildOsculatingSurface(U2,UKnot,VKnot,S,L);
                if(!OsculSurf) break;
                k++;
#ifdef OCCT_DEBUG
                cout<<"2.k = "<<k<<endl;
#endif
                IsQPunc=IsQPunctual(L,U2,GeomAbs_IsoU,0.,Tol);
                UKnot=1;
                VKnot=1;
                S=L;
              }
              if(OsculSurf)
              {
                myOsculSurf2->Append(L);
                myKdeg->Append(k);
              }
              else
                ClearOsculFlags(); //myAlong.SetValue(4,Standard_False);
            }
          }
        }
      }
      else
      {
        ClearOsculFlags();
      } 
    }
  }
  else
    ClearOsculFlags();
}

//=======================================================================
//function : BasisSurface
//purpose  : 
//=======================================================================

Handle(Geom_Surface) Geom_OsculatingSurface::BasisSurface() const
{
  return myBasisSurf;
}

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real Geom_OsculatingSurface::Tolerance() const
{
  return myTol;
}

//=======================================================================
//function : UOscSurf
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OsculatingSurface::UOscSurf
  (const Standard_Real U,
  const Standard_Real V,
  Standard_Boolean& t,
  Handle(Geom_BSplineSurface)& L) const
{
  Standard_Boolean along = Standard_False;
  if (myAlong(1) || myAlong(2)) 
  {
    Standard_Integer NU = 1, NV = 1;
    Standard_Real u1,u2,v1,v2;
    t = Standard_False;
    myBasisSurf->Bounds(u1,u2,v1,v2);
    Standard_Integer NbUK,NbVK;
    Standard_Boolean isToSkipSecond = Standard_False;
    if (myBasisSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface))) 
    {
      Handle(Geom_BSplineSurface) BSur = 
        Handle(Geom_BSplineSurface)::DownCast (myBasisSurf);
      NbUK = BSur->NbUKnots();
      NbVK = BSur->NbVKnots();
      TColStd_Array1OfReal UKnots(1,NbUK);
      TColStd_Array1OfReal VKnots(1,NbVK);
      BSur->UKnots(UKnots);
      BSur->VKnots(VKnots);
      BSplCLib::Hunt(UKnots,U,NU);
      BSplCLib::Hunt(VKnots,V,NV);
      if (NU < 1) NU=1;
      if (NU >= NbUK) NU=NbUK-1;
      if (NbVK==2 && NV==1)
        // Need to find the closest end
        if (VKnots(NbVK)-V > V-VKnots(1))
          isToSkipSecond = Standard_True;
    }
    else {NU = 1; NV = 1 ; NbVK = 2 ;}

    if (myAlong(1) && NV == 1) 
    {
      L = *((Handle(Geom_BSplineSurface)*)& myOsculSurf1->Value(NU));
      along = Standard_True;
    }
    if (myAlong(2) && (NV == NbVK-1) && !isToSkipSecond)
    {
      // t means that derivative vector of osculating surface is opposite
      // to the original. This happens when (v-t)^k is negative, i.e.
      // difference between degrees (k) is odd and t is the last parameter
      if (myKdeg->Value(NU)%2) t = Standard_True;
      L = *((Handle(Geom_BSplineSurface)*)& myOsculSurf2->Value(NU));
      along = Standard_True;
    }
  }
  return along;
}

//=======================================================================
//function : VOscSurf
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OsculatingSurface::VOscSurf
  (const Standard_Real U,
  const Standard_Real V,
  Standard_Boolean& t,
  Handle(Geom_BSplineSurface)& L) const
{
  Standard_Boolean along = Standard_False;
  if (myAlong(3) || myAlong(4)) 
  {
    Standard_Integer NU = 1, NV = 1;
    Standard_Real u1,u2,v1,v2;
    t = Standard_False;
    myBasisSurf->Bounds(u1,u2,v1,v2);
    Standard_Integer NbUK,NbVK;
    Standard_Boolean isToSkipSecond = Standard_False;
    if (myBasisSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface))) 
    {
      Handle(Geom_BSplineSurface) BSur = 
        Handle(Geom_BSplineSurface)::DownCast (myBasisSurf);
      NbUK = BSur->NbUKnots();
      NbVK = BSur->NbVKnots();
      TColStd_Array1OfReal UKnots(1,NbUK);
      TColStd_Array1OfReal VKnots(1,NbVK);
      BSur->UKnots(UKnots);
      BSur->VKnots(VKnots);
      BSplCLib::Hunt(UKnots,U,NU);
      BSplCLib::Hunt(VKnots,V,NV);
      if (NV < 1) NV=1;
      if (NV >= NbVK) NV=NbVK-1;
      if (NbUK==2 && NU==1)
        // Need to find the closest end
        if (UKnots(NbUK)-U > U-UKnots(1))
          isToSkipSecond = Standard_True;
    }
    else {NU = 1; NV = 1 ; NbUK = 2;}

    if (myAlong(3) && NU == 1)  
    {
      L = *((Handle(Geom_BSplineSurface)*)& myOsculSurf1->Value(NV));
      along = Standard_True;
    }
    if (myAlong(4) && (NU == NbUK-1) && !isToSkipSecond)
    {
      if (myKdeg->Value(NV)%2) t = Standard_True;
      L = *((Handle(Geom_BSplineSurface)*)& myOsculSurf2->Value(NV));
      along = Standard_True;
    }
  }
  return along;
}

//=======================================================================
//function : BuildOsculatingSurface
//purpose  : 
//=======================================================================

Standard_Boolean  Geom_OsculatingSurface::BuildOsculatingSurface
  (const Standard_Real Param,
  const Standard_Integer SUKnot,
  const Standard_Integer SVKnot,
  const Handle(Geom_BSplineSurface)& BS,
  Handle(Geom_BSplineSurface)& BSpl) const
{
  Standard_Integer i, j;
  Standard_Boolean OsculSurf=Standard_True;
#ifdef OCCT_DEBUG
  cout<<"t = "<<Param<<endl;
  cout<<"======================================"<<endl<<endl;
#endif

  // for cache
  Standard_Integer MinDegree,
    MaxDegree ;
  Standard_Real udeg, vdeg;
  udeg = BS->UDegree();
  vdeg = BS->VDegree();
  if( (IsAlongU() && vdeg <=1) || (IsAlongV() && udeg <=1))
  {
#ifdef OCCT_DEBUG
    cout<<" surface osculatrice nulle "<<endl;
#endif
    //throw Standard_ConstructionError("Geom_OsculatingSurface");
    OsculSurf=Standard_False;
  }
  else
  {
    MinDegree = (Standard_Integer ) Min(udeg,vdeg) ;
    MaxDegree = (Standard_Integer ) Max(udeg,vdeg) ;

    TColgp_Array2OfPnt cachepoles(1, MaxDegree + 1, 1, MinDegree + 1);
    // end for cache

    // for polynomial grid 
    Standard_Integer MaxUDegree, MaxVDegree;
    Standard_Integer UContinuity, VContinuity;

    Handle(TColStd_HArray2OfInteger) NumCoeffPerSurface = 
      new TColStd_HArray2OfInteger(1, 1, 1, 2);
    Handle(TColStd_HArray1OfReal) PolynomialUIntervals = 
      new TColStd_HArray1OfReal(1, 2);
    Handle(TColStd_HArray1OfReal) PolynomialVIntervals = 
      new TColStd_HArray1OfReal(1, 2);
    Handle(TColStd_HArray1OfReal) TrueUIntervals = 
      new TColStd_HArray1OfReal(1, 2);
    Handle(TColStd_HArray1OfReal) TrueVIntervals = 
      new TColStd_HArray1OfReal(1, 2);
    MaxUDegree = (Standard_Integer ) udeg;
    MaxVDegree = (Standard_Integer ) vdeg;

    for (i=1;i<=2;i++) 
    {
      PolynomialUIntervals->ChangeValue(i) = i-1;
      PolynomialVIntervals->ChangeValue(i) = i-1;
      TrueUIntervals->ChangeValue(i) = BS->UKnot(SUKnot+i-1);
      TrueVIntervals->ChangeValue(i) = BS->VKnot(SVKnot+i-1);
    }


    Standard_Integer OscUNumCoeff=0, OscVNumCoeff=0;
    if (IsAlongU()) 
    {
#ifdef OCCT_DEBUG
      cout<<">>>>>>>>>>> AlongU"<<endl;
#endif
      OscUNumCoeff = (Standard_Integer ) udeg + 1;  
      OscVNumCoeff = (Standard_Integer ) vdeg;  
    }
    if (IsAlongV()) 
    {
#ifdef OCCT_DEBUG
      cout<<">>>>>>>>>>> AlongV"<<endl;
#endif
      OscUNumCoeff = (Standard_Integer ) udeg;  
      OscVNumCoeff = (Standard_Integer ) vdeg + 1;  
    }
    NumCoeffPerSurface->ChangeValue(1,1) = OscUNumCoeff;  
    NumCoeffPerSurface->ChangeValue(1,2) = OscVNumCoeff;  
    Standard_Integer nbc = NumCoeffPerSurface->Value(1,1)*NumCoeffPerSurface->Value(1,2)*3;
    //
    if(nbc == 0)
    {
      return Standard_False;
    }
    //
    Handle(TColStd_HArray1OfReal) Coefficients = new TColStd_HArray1OfReal(1, nbc);
    //    end for polynomial grid

    //    building the cache
    Standard_Integer ULocalIndex, VLocalIndex;
    Standard_Real ucacheparameter, vcacheparameter,uspanlength, vspanlength;
    TColgp_Array2OfPnt NewPoles(1, BS->NbUPoles(), 1, BS->NbVPoles());

    Standard_Integer aUfKnotsLength = BS->NbUPoles() + BS->UDegree() + 1;
    Standard_Integer aVfKnotsLength = BS->NbVPoles() + BS->VDegree() + 1;

    if(BS->IsUPeriodic())
    {
      TColStd_Array1OfInteger aMults(1, BS->NbUKnots());
      BS->UMultiplicities(aMults);
      aUfKnotsLength = BSplCLib::KnotSequenceLength(aMults, BS->UDegree(), Standard_True);
    }

    if(BS->IsVPeriodic())
    {
      TColStd_Array1OfInteger aMults(1, BS->NbVKnots());
      BS->VMultiplicities(aMults);
      aVfKnotsLength = BSplCLib::KnotSequenceLength(aMults, BS->VDegree(), Standard_True);
    }

    TColStd_Array1OfReal UFlatKnots(1, aUfKnotsLength);
    TColStd_Array1OfReal VFlatKnots(1, aVfKnotsLength);
    BS->Poles(NewPoles);
    BS->UKnotSequence(UFlatKnots);
    BS->VKnotSequence(VFlatKnots);

    VLocalIndex = 0;
    ULocalIndex = 0;
    for(j = 1; j <= SVKnot; j++) VLocalIndex += BS->VMultiplicity(j);
    for(i = 1; i <= SUKnot; i++) ULocalIndex += BS->UMultiplicity(i);
    ucacheparameter = BS->UKnot(SUKnot);
    vcacheparameter = BS->VKnot(SVKnot);
    vspanlength = BS->VKnot(SVKnot + 1) - BS->VKnot(SVKnot);
    uspanlength = BS->UKnot(SUKnot + 1) - BS->UKnot(SUKnot);

    // On se ramene toujours a un parametrage tel que localement ce soit l'iso 
    // u=0 ou v=0 qui soit degeneree

    Standard_Boolean IsVNegative = Param > vcacheparameter + vspanlength/2;
    Standard_Boolean IsUNegative = Param > ucacheparameter + uspanlength/2;

    if (IsAlongU() && (Param > vcacheparameter + vspanlength/2))
      vcacheparameter = vcacheparameter +  vspanlength;
    if (IsAlongV() && (Param > ucacheparameter + uspanlength/2))
      ucacheparameter = ucacheparameter +  uspanlength;

    BSplSLib::BuildCache(ucacheparameter,   
      vcacheparameter,   
      uspanlength,         
      vspanlength,         
      BS->IsUPeriodic(),
      BS->IsVPeriodic(),
      BS->UDegree(),       
      BS->VDegree(),       
      ULocalIndex,         
      VLocalIndex,         
      UFlatKnots,
      VFlatKnots,
      NewPoles,
      BSplSLib::NoWeights(),
      cachepoles,
      BSplSLib::NoWeights());
    Standard_Integer m, n, index;
    TColgp_Array2OfPnt OscCoeff(1,OscUNumCoeff , 1, OscVNumCoeff);

    if (IsAlongU()) 
    {
      if (udeg > vdeg) 
      {
        for(n = 1; n <= udeg + 1; n++) 
          for(m = 1; m <= vdeg; m++)
            OscCoeff(n,m) = cachepoles(n,m+1) ;
      }
      else
      {
        for(n = 1; n <= udeg + 1; n++) 
          for(m = 1; m <= vdeg; m++)
            OscCoeff(n,m) = cachepoles(m+1,n) ;
      }
      if (IsVNegative) PLib::VTrimming(-1,0,OscCoeff,PLib::NoWeights2());

      index=1;
      for(n = 1; n <= udeg + 1; n++) 
        for(m = 1; m <= vdeg; m++)
        {
          Coefficients->ChangeValue(index++) = OscCoeff(n,m).X();
          Coefficients->ChangeValue(index++) = OscCoeff(n,m).Y();
          Coefficients->ChangeValue(index++) = OscCoeff(n,m).Z();
        }
    }

    if (IsAlongV()) 
    {
      if (udeg > vdeg) 
      {
        for(n = 1; n <= udeg; n++) 
          for(m = 1; m <= vdeg + 1; m++)
            OscCoeff(n,m) = cachepoles(n+1,m);
      }
      else
      {
        for(n = 1; n <= udeg; n++) 
          for(m = 1; m <= vdeg + 1; m++)
            OscCoeff(n,m) = cachepoles(m,n+1);
      }
      if (IsUNegative) PLib::UTrimming(-1,0,OscCoeff,PLib::NoWeights2());
      index=1;
      for(n = 1; n <= udeg; n++) 
        for(m = 1; m <= vdeg + 1; m++)
        {
          Coefficients->ChangeValue(index++) = OscCoeff(n,m).X();
          Coefficients->ChangeValue(index++) = OscCoeff(n,m).Y();
          Coefficients->ChangeValue(index++) = OscCoeff(n,m).Z();
        }
    }

    if (IsAlongU()) MaxVDegree--;
    if (IsAlongV()) MaxUDegree--;
    UContinuity = - 1;
    VContinuity = - 1;

    Convert_GridPolynomialToPoles Data(1,1,
      UContinuity,
      VContinuity,
      MaxUDegree,
      MaxVDegree,
      NumCoeffPerSurface,
      Coefficients,
      PolynomialUIntervals,
      PolynomialVIntervals,
      TrueUIntervals,
      TrueVIntervals);

    //      Handle(Geom_BSplineSurface) BSpl = 
    BSpl =new Geom_BSplineSurface(Data.Poles()->Array2(),
      Data.UKnots()->Array1(),
      Data.VKnots()->Array1(),
      Data.UMultiplicities()->Array1(),
      Data.VMultiplicities()->Array1(),
      Data.UDegree(),
      Data.VDegree(),
      0, 0);
#ifdef OCCT_DEBUG
    cout<<"^====================================^"<<endl<<endl;
#endif

    //      L=BSpl;
  }
  return OsculSurf;
}

//=======================================================================
//function : IsQPunctual
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OsculatingSurface::IsQPunctual
  (const Handle(Geom_Surface)& S,
  const Standard_Real         Param,
  const GeomAbs_IsoType       IT,
  const Standard_Real         TolMin,
  const Standard_Real         TolMax) const
{
  Standard_Real U1=0,U2=0,V1=0,V2=0,T;
  Standard_Boolean Along = Standard_True;
  S->Bounds(U1,U2,V1,V2);
  gp_Vec D1U,D1V;
  gp_Pnt P;
  Standard_Real Step,D1NormMax;
  if (IT == GeomAbs_IsoV) 
  {
    Step = (U2 - U1)/10;
    D1NormMax=0.;
    for (T=U1;T<=U2;T=T+Step) 
    {
      S->D1(T,Param,P,D1U,D1V);
      D1NormMax=Max(D1NormMax,D1U.Magnitude());
    }

#ifdef OCCT_DEBUG
    cout << " D1NormMax = " << D1NormMax << endl;
#endif
    if (D1NormMax >TolMax || D1NormMax < TolMin ) 
      Along = Standard_False;
  }
  else 
  {
    Step = (V2 - V1)/10;
    D1NormMax=0.;
    for (T=V1;T<=V2;T=T+Step) 
    {
      S->D1(Param,T,P,D1U,D1V);
      D1NormMax=Max(D1NormMax,D1V.Magnitude());
    }
#ifdef OCCT_DEBUG
    cout << " D1NormMax = " << D1NormMax << endl;
#endif
    if (D1NormMax >TolMax || D1NormMax < TolMin ) 
      Along = Standard_False;


  }
  return Along;
}

//=======================================================================
//function : HasOscSurf
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OsculatingSurface::HasOscSurf() const
{
  return (myAlong(1) || myAlong(2) || myAlong(3) || myAlong(4));
}

//=======================================================================
//function : IsAlongU
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OsculatingSurface::IsAlongU() const
{
  return (myAlong(1) || myAlong(2));
}
//=======================================================================
//function : IsAlongV
//purpose  : 
//=======================================================================

Standard_Boolean Geom_OsculatingSurface::IsAlongV() const
{
  return (myAlong(3) || myAlong(4));
}


//=======================================================================
//function : IsGetSeqOfL1
//purpose  : 
//=======================================================================

const Geom_SequenceOfBSplineSurface& Geom_OsculatingSurface::GetSeqOfL1() const
{
  return myOsculSurf1->Sequence();
}
//=======================================================================
//function : IsGetSeqOfL2
//purpose  : 
//=======================================================================

const Geom_SequenceOfBSplineSurface& Geom_OsculatingSurface::GetSeqOfL2() const
{
  return myOsculSurf2->Sequence();
}
//=======================================================================
//function : ClearOsculFlags
//purpose  : 
//=======================================================================

void Geom_OsculatingSurface::ClearOsculFlags()
{
  myAlong.SetValue(1,Standard_False);
  myAlong.SetValue(2,Standard_False);
  myAlong.SetValue(3,Standard_False);
  myAlong.SetValue(4,Standard_False);

}



