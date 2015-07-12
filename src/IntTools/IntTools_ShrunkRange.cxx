// Created by: Peter KURNEV
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
#include <BndLib_Add3dCurve.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <ElCLib.hxx>
#include <Geom_Curve.hxx>
#include <gp.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_ShrunkRange.hxx>
#include <Precision.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  IntTools_ShrunkRange::IntTools_ShrunkRange ()
{
  myT1=-99;
  myT2=myT1;
  myTS1=myT1;
  myTS2=myT1;
  myErrorStatus=1;
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
IntTools_ShrunkRange::~IntTools_ShrunkRange () 
{
}
//=======================================================================
//function : SetData
//purpose  : 
//=======================================================================
void IntTools_ShrunkRange::SetData(const TopoDS_Edge& aE,
                                   const Standard_Real aT1,
                                   const Standard_Real aT2,
                                   const TopoDS_Vertex& aV1,
                                   const TopoDS_Vertex& aV2)
{
  myEdge=aE;
  myV1=aV1;
  myV2=aV2;
  myT1=aT1;
  myT2=aT2;
  myErrorStatus=1;
}
//=======================================================================
//function : SetContext
//purpose  : 
//=======================================================================
void IntTools_ShrunkRange::SetContext(const Handle(IntTools_Context)& aCtx)
{
  myCtx=aCtx;
}
//=======================================================================
//function : Context
//purpose  : 
//=======================================================================
const Handle(IntTools_Context)& IntTools_ShrunkRange::Context()const
{
  return myCtx;
}
//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================
const TopoDS_Edge& IntTools_ShrunkRange::Edge() const
{
  return myEdge;
}
//=======================================================================
//function : ShrunkRange
//purpose  : 
//=======================================================================
void IntTools_ShrunkRange::ShrunkRange(Standard_Real& aT1,
                                       Standard_Real& aT2) const
{
  aT1=myTS1;
  aT2=myTS2;
}
//=======================================================================
//function : BndBox
//purpose  : 
//=======================================================================
const Bnd_Box& IntTools_ShrunkRange::BndBox() const
{
  return myBndBox;
}
//=======================================================================
//function : ErrorStatus
//purpose  : 
//=======================================================================
Standard_Integer IntTools_ShrunkRange::ErrorStatus() const
{
  return myErrorStatus;
}

//=======================================================================
//function : SetShrunkRange
//purpose  : 
//=======================================================================
void IntTools_ShrunkRange::SetShrunkRange(const Standard_Real aT1,
                                          const Standard_Real aT2) 
{
  myTS1=aT1;
  myTS2=aT2;
  //
  BRepAdaptor_Curve aBAC(myEdge);
  BndLib_Add3dCurve::Add (aBAC, aT1, aT2, 0., myBndBox);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void IntTools_ShrunkRange::Perform()
{
  Standard_Real aCF, aCL, aTolE, aTolV1;
  Standard_Real aTolV2, t1, t11, t1C, t2, t12, t2C;
  Standard_Real aCoeff1, aCoeff2, aTol1, aTol2, dt1, dt2, aR, anEps;
  Standard_Integer pri;
  Standard_Boolean bInf1, bInf2, bAppr;
  GeomAbs_CurveType aCurveType;
  Handle(Geom_Curve) aC;
  //
  myErrorStatus=0;
  myTS1=-99;
  myTS2=myTS1;
  anEps = 1.e-8;
  //
  aTolE =BRep_Tool::Tolerance(myEdge);
  aTolV1=BRep_Tool::Tolerance(myV1);
  aTolV2=BRep_Tool::Tolerance(myV2);
  //for edges with the tolerance value 
  //more than the tolerance value of vertices
  if (aTolV1 < aTolE) {
    aTolV1 = aTolE;
  }
  //
  if (aTolV2 < aTolE) {
    aTolV2 = aTolE;
  }
  //
  t1=myT1;
  t2=myT2;
  //
  BRepAdaptor_Curve aBAC(myEdge);
  aCurveType=aBAC.GetType();
  //
  aC=BRep_Tool::Curve(myEdge, aCF, aCL);
  BRep_Tool::Range(myEdge, aCF, aCL);
  //
  if (t1 < aCF || t2 > aCL) {
    myErrorStatus=2;
    return;
  }
  //
  bAppr = !(fabs(t2 - t1) > 100);
  if (fabs(t2 - t1) < anEps) {
    myErrorStatus=7;
    return;
  }
  //
  if (t1 > t2) {
    myErrorStatus=3;
    return;
  }
  //
  aTol1 = aTolV1+aTolE;
  aTol2 = aTolV2+aTolE;
  //
  aCoeff1 = (aTolE>0.05) ? 1. : 2.;
  aCoeff2 = aCoeff1;
  if (aCoeff1 == 2.) {
    aCoeff1=(aTol1>0.05) ? 1.5 : 2.;
    aCoeff2=(aTol2>0.05) ? 1.5 : 2.;
  }
  // xf
  if (aCurveType==GeomAbs_Line && (aCoeff1 != 1 || aCoeff2 != 1)) {
    Standard_Real aTV1, aTV2, aEps;
    gp_Pnt aPV1, aPV2, aPC1, aPC2;
    gp_Lin aL;
    //
    aEps=Precision::Confusion();
    aEps=aEps*aEps;//1.e-14;
    aL=aBAC.Line();
    //
    aPV1=BRep_Tool::Pnt(myV1);
    aTV1=ElCLib::Parameter(aL, aPV1);
    //
    aPV2=BRep_Tool::Pnt(myV2);
    aTV2=ElCLib::Parameter(aL, aPV2);
    //
    if (fabs(aTV1-aCF)<aEps) {
      aCoeff1=1.;
    }
    if (fabs(aTV2-aCL)<aEps) {
      aCoeff2=1.;
    }
  }
  //
  dt1=aCoeff1*aTol1;
  dt2=aCoeff2*aTol2;
  // xt
  //
  if (aCurveType==GeomAbs_Line) {
    Standard_Real dt1x, dt2x;

    dt1x = aBAC.Resolution(dt1);
    t11=t1+dt1x;
    
    dt2x = aBAC.Resolution(dt2);
    t12=t2-dt2x;

    if (t11>t2 || t12<t1) {
      t1C=t1;
      t2C=t2;
      myTS1=t1C;
      myTS2=t2C;
      //
      // BndBox
      Standard_Real ddx=aTolE;//1.e-12;
      BndLib_Add3dCurve::Add (aBAC, t1C, t2C, ddx, myBndBox);
      
      myErrorStatus=6;//0
      return;
    }
  }
  //
  if (aCurveType==GeomAbs_Circle) {
    gp_Circ aCrc=aBAC.Circle();
    aR=aCrc.Radius();
    t1C=t1+dt1/aR;
    t2C=t2-dt2/aR;
  }
  else {
    //
    // Vertex1 => t1C
    gp_Pnt aP1,aP11;
    aC->D0 (t1, aP1);
    //
    bInf1=Precision::IsNegativeInfinite(t1);
    if (bInf1) {
      t1C=t1;
    }
    //
    else {
      Standard_Real d1 = aCoeff1*aTol1;
      //       dt1 = aBAC.Resolution(d1);
      //
      gp_Vec aD1vec1;
      gp_Pnt aPoint;
      aBAC.D1(t1, aPoint, aD1vec1);
      Standard_Real ad1length1 = aD1vec1.Magnitude();
      Standard_Boolean bTryOtherPoints = Standard_False;
      dt1 = (t2 - t1) * 0.5;

      if(ad1length1 > 1.e-12) {
        dt1 = d1 / ad1length1;
        
        if(dt1  > (t2 - t1)) {
          // bad parametrization, big tolerance or too small range
          bTryOtherPoints = Standard_True;
        }
      }
      else {
        bTryOtherPoints = Standard_True;
      }
      
      if(bTryOtherPoints) {
        Standard_Integer nbsamples = 5;
        Standard_Integer ii = 0;
        Standard_Real adelta = (t2 - t1) / (nbsamples + 1);
        Standard_Boolean bFound = Standard_False;
        
        for(ii = 1; ii <= nbsamples; ii++) {
          Standard_Real aparameter = t1 + (adelta * ii);
          gp_Pnt aPoint2;
          aBAC.D1(aparameter, aPoint2, aD1vec1);
          
          if(aPoint.Distance(aPoint2) < d1)
            dt1 = adelta * ii;
          ad1length1 = aD1vec1.Magnitude();
          
          if(ad1length1 > 1.e-12) {
            dt1 = d1 / ad1length1;
            
            if(dt1 < (t2 - t1)) {
              bFound = Standard_True;
              break;
            }
          }
        }
        
        if(!bFound) {
          if(dt1 > (t2 - t1)) {
            dt1 = aBAC.Resolution(d1);
            if (dt1 > (t2 - t1)) {
              myErrorStatus = 7;
              return;
            }
          }
        }
      }
      //
      if (!bAppr) {
        dt1 *= 10;
      }
      t11=t1+dt1;
      aC->D0 (t11, aP11);
      
      gp_Vec aV11(aP1, aP11);
      // avoid exception if aP1 == aP11
      if (aV11.SquareMagnitude() < gp::Resolution())
        t1C = t1;
      else {
        gp_Dir aD11(aV11);

        gp_Pnt aP1L;
        //
        aP1L.SetCoord (aP1.X()+d1*aD11.X(),
                       aP1.Y()+d1*aD11.Y(),
                       aP1.Z()+d1*aD11.Z());

        BRepBuilderAPI_MakeVertex aMV1(aP1L);
        const TopoDS_Vertex& aV1L=aMV1.Vertex();
        //
        pri=myCtx->ComputeVE (aV1L, myEdge, t1C);
        //
        if (pri==-3) {
          myErrorStatus=4;
          return;
        }
      }
    }
    //
    // Vertex2 => t2C
    gp_Pnt aP2, aP12;
    aC->D0 (t2, aP2);
    //
    bInf2=Precision::IsPositiveInfinite(t2);
    if (bInf2) {
      t2C=t2;
    }
    //
    else {
      Standard_Real d2 = aCoeff2*aTol2;
      //       dt2 = aBAC.Resolution(d2);

      //
      gp_Vec aD1vec2;
      gp_Pnt aPoint;
      aBAC.D1(t2, aPoint, aD1vec2);
      Standard_Real ad1length2 = aD1vec2.Magnitude();
      Standard_Boolean bTryOtherPoints = Standard_False;
      dt2 = (t2 - t1) * 0.5;

      if(ad1length2 > 1.e-12) {
        dt2 = d2 / ad1length2;
        
        if(dt2 > (t2 - t1)) {
          bTryOtherPoints = Standard_True;
        }
      }
      else {
        bTryOtherPoints = Standard_True;
      }

      if(bTryOtherPoints) {
        Standard_Integer nbsamples = 5;
        Standard_Integer ii = 0;
        Standard_Real adelta = (t2 - t1) / (nbsamples + 1);
        Standard_Boolean bFound = Standard_False;
        
        for(ii = 1; ii <= nbsamples; ii++) {
          Standard_Real aparameter = t2 - (adelta * ii);
          gp_Pnt aPoint2;
          aBAC.D1(aparameter, aPoint2, aD1vec2);
          
          if(aPoint.Distance(aPoint2) < d2)
            dt2 = adelta * ii;
          ad1length2 = aD1vec2.Magnitude();
          
          if(ad1length2 > 1.e-12) {
            dt2 = d2 / ad1length2;
            
            if(dt2 < (t2 - t1)) {
              bFound = Standard_True;
              break;
            }
          }
        }
        
        if(!bFound) {
          if(dt2 > (t2 - t1)) {
            dt2 = aBAC.Resolution(d2);
            if(dt2 > (t2 - t1)) {
              myErrorStatus = 7;
              return;
            }
          }
        }
      }
      //
      if (!bAppr) {
        dt2 *= 10;
      }

      t12=t2-dt2;
      aC->D0 (t12, aP12);
      
      gp_Vec aV12(aP2, aP12);
      // avoid exception if aP1 == aP11
      if (aV12.SquareMagnitude() < gp::Resolution())
        t2C = t2;
      else {
        gp_Dir aD12(aV12);

        gp_Pnt aP2L;
        //
        aP2L.SetCoord (aP2.X()+d2*aD12.X(),
                       aP2.Y()+d2*aD12.Y(),
                       aP2.Z()+d2*aD12.Z());

        BRepBuilderAPI_MakeVertex aMV2(aP2L);
        const TopoDS_Vertex& aV2L=aMV2.Vertex();
        //
        pri=myCtx->ComputeVE (aV2L, myEdge, t2C);
        //
        if (pri==-3) {
          myErrorStatus=5;
          return;
        }
      }
    }
  } // else {
  //
  if (t1C>t2){
    t1C=0.5*(t2+t1);
    t2C=t1C+0.1*(t2-t1C);
  }
  
  if (t1C>t2C) {
    t2C=t1C+0.1*(t2-t1C);
  }
  //
  if (t2C-t1C < anEps) {
    myErrorStatus = 7;
    return;
  }
  //
  myTS1=t1C;
  myTS2=t2C;
  //
  // BndBox
  Standard_Real ddx=aTolE;//1.e-12;
  BndLib_Add3dCurve::Add (aBAC, t1C, t2C, ddx, myBndBox);
}
/////////////////////////////////////////////////////////////////////////
//
//            myErrorStatus :
//
// 1- Nothing has been done
// 2- The source range is out of the edge's range
// 3- t1 < t2 for source range
// 4- Can not project V1L  to the Edge;
// 5- Can not project V2L  to the Edge;
// 6- for obtained shrunk range [t11, t12] ->  t11>t2 || t12<t1;
// 7- too small range.
