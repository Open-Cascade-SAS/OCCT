// Created on: 2012-11-29
// Created by: Peter KURNEV
// Copyright (c) 2012 OPEN CASCADE SAS
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


#include <IntTools_EdgeEdge.ixx>

#include <gp_Elips.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_XYZ.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
//
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_OffsetCurve.hxx>
//
#include <GeomAPI_ProjectPointOnCurve.hxx>
//
#include <BRep_Tool.hxx>


//=======================================================================
//class    : IntTools_ComparatorCurve
//purpose  : 
//=======================================================================
class IntTools_ComparatorCurve {
 public: 
  IntTools_ComparatorCurve() {
    myT11=0.;
    myT12=0.;
    myT21=0.;
    myT22=0.;
    myIsSame=Standard_False;
  };
  //
  virtual ~IntTools_ComparatorCurve(){
  }; 
  //
  void SetCurve1(const BRepAdaptor_Curve& aBC3D) {
    myBC1=aBC3D;
  };
  //
  const BRepAdaptor_Curve& Curve1()const {
    return myBC1;
  };
  //
  void SetRange1(const Standard_Real aT1,
		 const Standard_Real aT2) {
    myT11=aT1;
    myT12=aT2;
  };
  //
  void Range1(Standard_Real& aT1, Standard_Real& aT2)const {
    aT1=myT11;
    aT2=myT12;
  };
  //
  void SetCurve2(const BRepAdaptor_Curve& aBC3D){
    myBC2=aBC3D;
  };
  //
  const BRepAdaptor_Curve& Curve2()const{
    return myBC2;
  };
  //
  void SetRange2(const Standard_Real aT1,
		 const Standard_Real aT2){
    myT21=aT1;
    myT22=aT2;
  };
  //
  void Range2(Standard_Real& aT1, 
	      Standard_Real& aT2)const {
    aT1=myT21;
    aT2=myT22;
  };
  //
  Standard_Boolean IsSame()const {
    return myIsSame;
  }; 
  //
  void Perform();
  //
  //--------------------------------------
 protected:
  //
  void IsSameElipse();
  //
  void IsSameBSplineCurve();
  //
  static 
    Standard_Boolean 
      IsSameReal(const Standard_Real aR1, 
		 const Standard_Real aR2);
  //
  static
    Standard_Boolean 
      IsSameAx2(const gp_Ax2& aAx21, 
		const gp_Ax2& aAx22);
  //
  static
    Standard_Boolean
      IsSameAx1(const gp_Ax1& aAx1, 
		const gp_Ax1& aAx2);
  //
  static
    Standard_Boolean 
      IsSamePnt(const gp_Pnt& aP1, 
		const gp_Pnt& aP2);
  static
    Standard_Boolean 
      IsSameDir(const gp_Dir& aDir1, 
		const gp_Dir& aDir2);
  //
  static
    Standard_Boolean 
      IsSameXYZ(const gp_XYZ& aXYZ1, 
		const gp_XYZ& aXYZ2);
  //
  static
    void GetCurveBase(const Handle(Geom_Curve)& aC3D,
		      GeomAbs_CurveType& aTypeBase,
		      Handle(Geom_Curve)& aCurveBase);
  //
  static
    Standard_Boolean 
      IsTypeBase(const Handle(Geom_Curve)& aC,
		 GeomAbs_CurveType& aTypeB);
  //
 protected:
  BRepAdaptor_Curve myBC1;
  Standard_Real myT11;
  Standard_Real myT12;
  //
  BRepAdaptor_Curve myBC2;
  Standard_Real myT21;
  Standard_Real myT22;
  //
  Standard_Boolean myIsSame;
};
//
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void IntTools_ComparatorCurve::Perform()
{
  GeomAbs_CurveType aCurveType1, aCurveType2;
  //
  myIsSame=Standard_False;
  //
  aCurveType1=myBC1.GetType();
  aCurveType2=myBC2.GetType();
  //
  myIsSame=(aCurveType1==aCurveType2);
  if (!myIsSame) {
    return; 
  }
  // 
  myIsSame=IsSameReal(myT11, myT21);
  if (!myIsSame) {
    return;
  }
  //
  myIsSame=IsSameReal(myT12, myT22);
  if (!myIsSame) {
    return;
  }
  //
  if (aCurveType1==GeomAbs_Ellipse) {
    IsSameElipse();
    return;
  } 
  else if (aCurveType1==GeomAbs_BSplineCurve) {
    IsSameBSplineCurve();
    return ;
  }
  else {
    myIsSame=Standard_False;
    return;
  }
}
//=======================================================================
//function : IsSameBSplineCurve
//purpose  : 
//=======================================================================
void IntTools_ComparatorCurve::IsSameBSplineCurve()
{
  Standard_Boolean bIsRational, bIsPreiodic;
  Standard_Integer iNbPoles, iNbKnots, iDegree;
  //
  bIsRational=myBC1.IsRational();
  myIsSame=(bIsRational==myBC2.IsRational());
  if (!myIsSame) {
    return;
  }
  //
  iNbPoles=myBC1.NbPoles();
  myIsSame=(iNbPoles==myBC2.NbPoles());
  if (!myIsSame) {
    return;
  }
  //
  iNbKnots=myBC1.NbKnots();
  myIsSame=(iNbKnots==myBC2.NbKnots());
  if (!myIsSame) {
    return;
  }
  //
  iDegree=myBC1.Degree();
  myIsSame=(iDegree==myBC2.Degree());
  if (!myIsSame) {
    return;
  } 
  //
  bIsPreiodic=myBC1.IsPeriodic();
  myIsSame=(bIsPreiodic==myBC2.IsPeriodic());
  if (!myIsSame) {
    return;
  }
 
  //-------------------------------------------
  Standard_Integer i, j, aM[2];
  Standard_Real aT1, aT2, aX0[4], aX1[4];
  GeomAbs_CurveType aTypeBase;
  gp_Pnt aP;
  Handle(Geom_Curve) aC;
  Handle(Geom_BSplineCurve) aBSp[2];
  TopoDS_Edge aE1, aE2;
  //
  aE1=myBC1.Edge();
  aE2=myBC2.Edge();
  //
  aC=BRep_Tool::Curve (aE1, aT1, aT2);
  GetCurveBase(aC, aTypeBase, aBSp[0]);
  //
  aC=BRep_Tool::Curve (aE2, aT1, aT2);
  GetCurveBase(aC, aTypeBase, aBSp[1]);
  //
  // Poles / Weights
  for(i=1; i<=iNbPoles; ++i) {
    aP=aBSp[0]->Pole(i);
    aP.Coord(aX0[0], aX0[1], aX0[2]);
    aX0[3]=aBSp[0]->Weight(i);
    //
    aP=aBSp[1]->Pole(i);
    aP.Coord(aX1[0], aX1[1], aX1[2]);
    aX1[3]=aBSp[1]->Weight(i);
    //
    for (j=0; j<4; ++j) {
      myIsSame=IsSameReal(aX0[j], aX1[j]);
      if(!myIsSame) {
	return;
      }
    }
  }//for(i=1; i<iNbPoles; ++i) {
  //
  // Knots / Multiplicities
  for(i=1; i<=iNbKnots; ++i) {
    aX0[0]=aBSp[0]->Knot(i);
    aX0[1]=aBSp[1]->Knot(i);
    myIsSame=IsSameReal(aX0[0], aX0[1]);
    if(!myIsSame) {
      return;
    }
    //
    aM[0]=aBSp[0]->Multiplicity(i);
    aM[1]=aBSp[1]->Multiplicity(i);
    myIsSame=(aM[0]==aM[1]);
    if(!myIsSame) {
      return;
    }
  }
}
//=======================================================================
//function : GetCurveBase
//purpose  : 
//=======================================================================
void IntTools_ComparatorCurve::GetCurveBase(const Handle(Geom_Curve)& aC3D,
					    GeomAbs_CurveType& aTypeBase,
					    Handle(Geom_Curve)& aCurveBase)
{
  Standard_Boolean bIsTypeBase;
  Standard_Integer  iTrimmed, iOffset;
  Standard_Real aOffsetBase;
  GeomAbs_CurveType aTypeB;
  Handle(Geom_Curve) aC3DB;
  Handle(Geom_TrimmedCurve) aCT3D;
  Handle(Geom_OffsetCurve) aCF3D;
  //
  aTypeBase=GeomAbs_OtherCurve;
  aOffsetBase=0.;
  //
  aC3DB=aC3D;
  bIsTypeBase=IsTypeBase(aC3DB, aTypeB);
  if (bIsTypeBase) {
    aTypeBase=aTypeB;
    aCurveBase=aC3D;
    return;
  }
  //
  while(1) {
    iTrimmed=0;
    iOffset=0;
    aCT3D=Handle(Geom_TrimmedCurve)::DownCast(aC3DB);
    if (!aCT3D.IsNull()) {
      aC3DB=aCT3D->BasisCurve();
      ++iTrimmed;
    }
    //
    aCF3D=Handle(Geom_OffsetCurve)::DownCast(aC3DB);
    if (!aCF3D.IsNull()) {
      Standard_Real aOffset;
      //
      aOffset=aCF3D->Offset();
      aOffsetBase=aOffsetBase+aOffset;
      //
      aC3DB=aCF3D->BasisCurve();
      ++iOffset;
    }
    //
    if (!(iTrimmed || iOffset)) {
      break;
    }
    //
    bIsTypeBase=IsTypeBase(aC3DB, aTypeB);
    if (bIsTypeBase) {
      aTypeBase=aTypeB;
      aCurveBase=aC3DB;
      return;
    }
  }
}
//=======================================================================
//function : IsTypeBase
//purpose  : 
//=======================================================================
Standard_Boolean 
  IntTools_ComparatorCurve::IsTypeBase(const Handle(Geom_Curve)& aC,
				       GeomAbs_CurveType& aTypeB)
{
  Standard_Boolean bRet; 
  Handle(Standard_Type) aType;
  //
  bRet=Standard_True;
  //
  aType=aC->DynamicType();
  if (aType==STANDARD_TYPE(Geom_Line)) {
    aTypeB=GeomAbs_Line;
  }
  else if (aType==STANDARD_TYPE(Geom_Circle)) {
    aTypeB=GeomAbs_Circle;
  }
  else if (aType==STANDARD_TYPE(Geom_Ellipse)) {
    aTypeB=GeomAbs_Ellipse;
  }
  else if (aType==STANDARD_TYPE(Geom_Parabola)) {
    aTypeB=GeomAbs_Parabola;
  }
  else if (aType==STANDARD_TYPE(Geom_Hyperbola)) {
    aTypeB=GeomAbs_Hyperbola;
  }
  else if (aType==STANDARD_TYPE(Geom_BezierCurve)) {
    aTypeB=GeomAbs_BezierCurve;
  }
  else if (aType==STANDARD_TYPE(Geom_BSplineCurve)) {
    aTypeB=GeomAbs_BSplineCurve;
  }
  else {
    aTypeB=GeomAbs_OtherCurve;
    bRet=!bRet;
  }
  return bRet;
}
//=======================================================================
//function : IsSameElipse
//purpose  : 
//=======================================================================
void IntTools_ComparatorCurve::IsSameElipse()
{
  Standard_Real aR1, aR2;
  gp_Elips aElips1, aElips2;
  // 
  myIsSame=Standard_False;
  //
  aElips1=myBC1.Ellipse();
  aElips2=myBC2.Ellipse();
  //
  aR1=aElips1.MajorRadius();
  aR2=aElips2.MajorRadius();
  myIsSame=IsSameReal(aR1, aR2);
  if (!myIsSame) {
    return;
  }
  //
  aR1=aElips1.MinorRadius();
  aR2=aElips2.MinorRadius();
  myIsSame=IsSameReal(aR1, aR2);
  if (!myIsSame) {
    return;
  }
  //
  const gp_Ax2& aAx21=aElips1.Position();
  const gp_Ax2& aAx22=aElips2.Position();
  myIsSame=IsSameAx2(aAx21, aAx22);
}
//=======================================================================
//function : IsSameAx2
//purpose  : 
//=======================================================================
Standard_Boolean 
  IntTools_ComparatorCurve::IsSameAx2(const gp_Ax2& aAx21, 
				      const gp_Ax2& aAx22)
{
  Standard_Boolean bRet;
  //
  const gp_Ax1& aAx1=aAx21.Axis();
  const gp_Ax1& aAx2=aAx22.Axis();
  //
  bRet=IsSameAx1(aAx1, aAx2);
  if (!bRet) {
    return bRet;
  }
  //
  const gp_Dir& aDirX1=aAx21.XDirection();
  const gp_Dir& aDirX2=aAx22.XDirection();
  //
  bRet=IsSameDir(aDirX1, aDirX2);
  if (!bRet) {
    return bRet;
  }
  //
  //
  const gp_Dir& aDirY1=aAx21.YDirection();
  const gp_Dir& aDirY2=aAx22.YDirection();
  //
  bRet=IsSameDir(aDirY1, aDirY2);
  //
  return bRet;
}
//=======================================================================
//function : IsSamePnt
//purpose  : 
//=======================================================================
Standard_Boolean 
  IntTools_ComparatorCurve::IsSamePnt(const gp_Pnt& aP1, 
				      const gp_Pnt& aP2)
{
  const gp_XYZ& aXYZ1=aP1.XYZ();
  const gp_XYZ& aXYZ2=aP2.XYZ();
  return IsSameXYZ(aXYZ1, aXYZ2);
}
//=======================================================================
//function : IsSameAx1
//purpose  : 
//=======================================================================
Standard_Boolean 
  IntTools_ComparatorCurve::IsSameAx1(const gp_Ax1& aAx1, 
				      const gp_Ax1& aAx2)
{
  Standard_Boolean bRet;
  //
  const gp_Pnt& aP1=aAx1.Location();
  const gp_Pnt& aP2=aAx2.Location();
  //
  bRet=IsSamePnt(aP1, aP2);
  if (!bRet) {
    return bRet;
  }
  //
  const gp_Dir& aDir1=aAx1.Direction();
  const gp_Dir& aDir2=aAx2.Direction();
  //
  bRet=IsSameDir(aDir1, aDir2);
  return bRet;
}
//=======================================================================
//function : IsSameDir
//purpose  : 
//=======================================================================
Standard_Boolean 
  IntTools_ComparatorCurve::IsSameDir(const gp_Dir& aDir1, 
				      const gp_Dir& aDir2)
{
  const gp_XYZ& aXYZ1=aDir1.XYZ();
  const gp_XYZ& aXYZ2=aDir2.XYZ();
  return IsSameXYZ(aXYZ1, aXYZ2);
}
//=======================================================================
//function : IsSameXYZ
//purpose  : 
//=======================================================================
Standard_Boolean 
  IntTools_ComparatorCurve::IsSameXYZ(const gp_XYZ& aXYZ1, 
				      const gp_XYZ& aXYZ2)
{
  Standard_Boolean bRet;
  Standard_Integer i;
  Standard_Real aX1[3], aX2[3];
  
  aXYZ1.Coord(aX1[0], aX1[1], aX1[2]);
  aXYZ2.Coord(aX2[0], aX2[1], aX2[2]);
  //
  for (i=0; i<3; ++i) {
    bRet=IsSameReal(aX1[i], aX2[i]);
    if(!bRet) {
      break;
    }
  }
  return bRet;
}
//=======================================================================
//function : IsSameReal
//purpose  : 
//=======================================================================
Standard_Boolean 
  IntTools_ComparatorCurve::IsSameReal(const Standard_Real aR1, 
				       const Standard_Real aR2)
{
  Standard_Boolean bRet;
  Standard_Real aEpsilon;
  //
  aEpsilon=Epsilon(aR1);
  bRet=(fabs(aR1-aR2)<aEpsilon);
  return bRet;
}
//
//modified by NIZNHY-PKV Tue Jan 15 07:44:33 2013f
//=======================================================================
// class: IntTools_DistCC
// purpose  : 
//=======================================================================
class IntTools_DistCC {
 public:
  IntTools_DistCC() {
    myT11=0.;
    myT12=0.;
    myT21=0.;
    myT21=0.;
    myErrorStatus=1;
    myThreshold=1.e-7;
    myDx=0.;
    myTx=0.;
    myNbP=10;
    myIndent=0.3;
    myEps=0.001;
  };
  //
  ~IntTools_DistCC() {
  };
  //-------- 1
  void SetCurve1(const Handle(Geom_Curve)& aC1) {
    myC1=aC1;
  };
  //
  const Handle(Geom_Curve)& Curve1()const {
    return myC1;
  };
  //
  void SetRange1(const Standard_Real aT11,
		 const Standard_Real aT12) {
    myT11=aT11;
    myT12=aT12;
  };
  //
  void Range1(Standard_Real& aT11,
	      Standard_Real& aT12)const {
    aT11=myT11;
    aT12=myT12;
  };
  //-------- 2
  void SetCurve2(const Handle(Geom_Curve)& aC2) {
    myC2=aC2;
  };
  //
  const Handle(Geom_Curve)& Curve2()const {
    return myC2;
  };
  //
  void SetRange2(const Standard_Real aT21,
		 const Standard_Real aT22) {
    myT21=aT21;
    myT22=aT22;
  };
  //
  void Range2(Standard_Real& aT21,
	      Standard_Real& aT22)const {
    aT21=myT21;
    aT22=myT22;
  };
  //
  void SetThreshold(const Standard_Real aD) {
    myThreshold=aD;
  }
  //
  Standard_Real Threshold() const {
    return myThreshold;
  }
  //
  void Perform();
  //
  Standard_Integer ErrorStatus()const {
    return myErrorStatus;
  };
  //
  Standard_Real MaxDeviation()const {
    return myDx;
  }
  //
  Standard_Real MaxParameter()const {
    return myTx;
  }
 //-----------------------------------------------------
 protected :  
  //
  void Init();
  //
  void CheckData();
  //
  Standard_Real Distance(const Standard_Real aT); 
  //
  void FindMaxDeviation();
  //
  void FindMaxLocal (const Standard_Real aT11,
		     const Standard_Real aT12,
		     const Standard_Real aEps,
		     Standard_Real& aDx,
		     Standard_Real& aTx);
  
  //
 protected :
  Standard_Real myT11;
  Standard_Real myT12;
  Handle(Geom_Curve) myC1;
  
  Standard_Real myT21;
  Standard_Real myT22;
  Handle(Geom_Curve) myC2;
  //
  Standard_Real myThreshold;
  Standard_Integer myErrorStatus;
  //
  Standard_Real myDx;
  Standard_Real myTx;
  //
  Standard_Integer myNbP;
  Standard_Real myIndent;
  Standard_Real myEps;
  GeomAPI_ProjectPointOnCurve myPPC2; 
};

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void IntTools_DistCC::Perform()
{
  myErrorStatus=0;
  //
  CheckData();
  if (myErrorStatus){
    return;
  }
  // Init
  myPPC2.Init(myC2, myT21, myT22);
  //
  FindMaxDeviation();
  if (myErrorStatus){
    return;
  }
}
//=======================================================================
//function : CheckData
//purpose  : 
//=======================================================================
void IntTools_DistCC::CheckData()
{
  myErrorStatus=0;
  //
  if (myC1.IsNull()) {
    myErrorStatus=2;
    return;
  }
  //
  if (myC2.IsNull()) {
    myErrorStatus=3;
    return;
  }
}
//
//=======================================================================
//function : FindMaxDeviation
//purpose  : 
//=======================================================================
void IntTools_DistCC::FindMaxDeviation()
{ 
  Standard_Integer i, aNbP1, aNbP2;
  Standard_Real aTmax, aT, aT1, aT2, dT, aDmax, aEps, aD;
  //
  myErrorStatus=0;
  myDx=0.;
  myTx=0.;
  //
  aTmax=0;
  aDmax=0.;
  aEps=myEps*(myT12-myT11);
  //
  aNbP1=myNbP-1;
  aNbP2=aNbP1-1;
  dT=(myT12-myT11)/aNbP1;
  for (i=0; i<aNbP1; ++i) {
    aT1=myT11+i*dT;
    aT2=aT1+dT;
    //
    if (!i) {
      aT1=aT1+myIndent*dT;
    }
    else if (i==aNbP2) {
      aT2=aT2-myIndent*dT;
      if (aT2>myT12) {
	aT2=myT12;
      }
    }
    //
    FindMaxLocal(aT1, aT2, aEps, aD, aT);
    if (myErrorStatus) {
      return ;
    }
    //
    if (aD>aDmax) {
      aDmax=aD;
      aTmax=aT;
    }
  }
  //
  myTx=aTmax;
  myDx=aDmax;
}
//=======================================================================
//function : FindMaxLocal
//purpose  : Solver: Golden Mean
//=======================================================================
void IntTools_DistCC::FindMaxLocal(const Standard_Real aT11,
				   const Standard_Real aT12,
				   const Standard_Real aEps,
				   Standard_Real& aDx,
				   Standard_Real& aTx)
{
  Standard_Integer iErr;
  Standard_Real aA, aB, aCf, aX1, aX2, aF1, aF2, aX, aF;
  //
  myErrorStatus=0;
  iErr=0;
  aDx=0.;
  aTx=0.;
  //
  aCf=1.6180339887498948482045868343656;// =0.5*(1.+sqrt(5.));
  //
  aA=aT11;
  aB=aT12;
  //
  aX1=aB-(aB-aA)/aCf;
  aF1=Distance(aX1);
  if (myErrorStatus) {
    return ;
  }
  //
  aX2=aA+(aB-aA)/aCf;
  aF2=Distance(aX2);
  if (myErrorStatus) {
    return;
  }
  //
  while(1) {
    if (fabs(aA-aB)<aEps) {
      aX=0.5*(aA+aB);
      aF=Distance(aX);
      if (myErrorStatus) {
	return;
      }
      //
      break;
    }
    if (aF1<aF2){
      aA=aX1;
      aX1=aX2;
      aF1=aF2;
      aX2=aA+(aB-aA)/aCf;
      aF2=Distance(aX2);
      if (myErrorStatus) {
	return ;
      }
    }
    else {
      aB=aX2;
      aX2=aX1;
      aF2=aF1;
      aX1=aB-(aB-aA)/aCf;
      aF1=Distance(aX1);
      if (myErrorStatus) {
	return;
      }
    }
  }
  //
  aDx=aF;
  aTx=aX;
}
//=======================================================================
//function : Distance
//purpose  : 
//=======================================================================
Standard_Real IntTools_DistCC::Distance(const Standard_Real aT)
{
  Standard_Integer aNbP2;
  Standard_Real aD;
  gp_Pnt aP1;
  //
  aD=0.;
  if (myErrorStatus) {
    return aD;
  }
  //
  myC1->D0(aT, aP1);
  myPPC2.Perform(aP1);
  //
  aNbP2=myPPC2.NbPoints();
  if (!aNbP2) {
    myErrorStatus=4;
    return aD;
  }
  //
  aD=myPPC2.LowerDistance();
  if (aD>myThreshold) {
    myErrorStatus=10;
  }
  return aD;
}
//modified by NIZNHY-PKV Tue Jan 15 07:44:44 2013t
//
//=======================================================================
//function : IsSameCurves
//purpose  : 
//=======================================================================
Standard_Boolean IntTools_EdgeEdge::IsSameCurves()
{
  Standard_Boolean bRet, bIsBC;
  GeomAbs_CurveType aCT1, aCT2;
  IntTools_ComparatorCurve aICC;
  //
  // 1. Check letter
  aICC.SetCurve1(myCFrom);
  aICC.SetRange1(myTminFrom, myTmaxFrom);
  //
  aICC.SetCurve2(myCTo);
  aICC.SetRange2(myTminTo, myTmaxTo);
  //
  aICC.Perform();
  bRet=aICC.IsSame();
  if (bRet) {
    return bRet;
  }
  //
  // 2. Check inwards
  aCT1=myCFrom.GetType();
  aCT2=myCTo.GetType();
  bIsBC=(aCT1==GeomAbs_BSplineCurve || 
	 aCT1==GeomAbs_BezierCurve  ||
	 aCT2==GeomAbs_BSplineCurve ||
	 aCT2==GeomAbs_BezierCurve);
  //
  if (bIsBC) {
    Standard_Integer iErr;
    Standard_Real aT11, aT12, aT21, aT22;
    Handle(Geom_Curve) aC1, aC2;
    IntTools_DistCC aDistCC; 
    //
    const TopoDS_Edge& aE1=myCFrom.Edge();
    aC1=BRep_Tool::Curve(aE1, aT11, aT12);
    //
    const TopoDS_Edge& aE2=myCTo.Edge();
    aC2=BRep_Tool::Curve(aE2, aT21, aT22);
    //
    aDistCC.SetCurve1(aC1);
    aDistCC.SetRange1(myTminFrom, myTmaxFrom);
    aDistCC.SetCurve2(aC2);
    aDistCC.SetRange2(myTminTo, myTmaxTo);
    aDistCC.SetThreshold(myCriteria);
    //
    aDistCC.Perform();
    //
    iErr=aDistCC.ErrorStatus();
    //
    bRet=(!iErr); 
    //
  }
  return bRet;
}

