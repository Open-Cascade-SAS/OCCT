// Created by: Peter KURNEV
// Copyright (c) 2014 OPEN CASCADE SAS
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


#include <BRep_Tool.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_ListOfStatus.hxx>
#include <BRepCheck_Solid.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ShapeMapHasher.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepCheck_Solid,BRepCheck_Result)

//
class BRepCheck_HSC;
DEFINE_STANDARD_HANDLE(BRepCheck_HSC, Standard_Transient)
//=======================================================================
//class    : BRepCheck_HSC
//purpose  :
//=======================================================================
class BRepCheck_HSC : public Standard_Transient {
 public: 
  //
  Standard_EXPORT
    BRepCheck_HSC(){
    };
  //
  Standard_EXPORT
    virtual ~BRepCheck_HSC(){
  };
  //
  Standard_EXPORT
    BRepClass3d_SolidClassifier& SolidClassifier(){
      return mySC;
    };
  //
  DEFINE_STANDARD_RTTI_INLINE(BRepCheck_HSC,Standard_Transient);

 protected:
  BRepClass3d_SolidClassifier mySC;
};

//
//=======================================================================
//class    : BRepCheck_ToolSolid
//purpose  : 
//=======================================================================
class BRepCheck_ToolSolid  {

 public:
  DEFINE_STANDARD_ALLOC

  BRepCheck_ToolSolid() {
    myIsHole=Standard_False;
    myPntTol=Precision::Confusion();
    myPnt.SetCoord(-1.,-1.,-1.);
  };
   
  virtual ~BRepCheck_ToolSolid() {
  };
  // 
  void SetSolid(const TopoDS_Solid& aZ) {
    mySolid=aZ;
  };
  //
  const TopoDS_Solid& Solid()const {
    return mySolid;
  };
  //
  Standard_Boolean IsHole() const {
    return myIsHole;
  };
  //
  const gp_Pnt& InnerPoint() {
    return myPnt;
  }
  //
  Standard_Real CheckTol() const {
    return myPntTol;
  };
  //
  // IsOut
  Standard_Boolean IsOut(BRepCheck_ToolSolid& aOther)  {
    Standard_Boolean bFlag;
    TopAbs_State aState;
    //
    BRepClass3d_SolidClassifier& aSC=myHSC->SolidClassifier();
    //
    aSC.Perform(aOther.InnerPoint(), aOther.CheckTol());
    aState=aSC.State();
    bFlag=(aState==TopAbs_OUT);
    //
    return bFlag;
  };
  //
  // Init
  void Init() {
    Standard_Real aT, aT1, aT2, aPAR_T;
    TopExp_Explorer aExp;
    //
    // 0.myHSC
    myHSC=new BRepCheck_HSC();
    //
    BRepClass3d_SolidClassifier& aSC=myHSC->SolidClassifier();
    // 1. Load
    aSC.Load(mySolid);
    //
    // 2. myIsHole
    aSC.PerformInfinitePoint(::RealSmall());
    myIsHole=(aSC.State()==TopAbs_IN);
    // 
    // 3. myPnt
    aPAR_T=0.43213918; // 10*e^(-PI)
    aExp.Init(mySolid, TopAbs_EDGE);
    for (; aExp.More();  aExp.Next()) {
      const TopoDS_Edge& aE=*((TopoDS_Edge*)&aExp.Current());
      if (!BRep_Tool::Degenerated(aE)) {
        Handle(Geom_Curve) aC3D=BRep_Tool::Curve(aE, aT1, aT2);
        aT=(1.-aPAR_T)*aT1 + aPAR_T*aT2;
        myPnt=aC3D->Value(aT);
        myPntTol = BRep_Tool::Tolerance(aE);
        break;
      }
    }
  };
  //
 protected:
  Standard_Boolean myIsHole;
  gp_Pnt myPnt; 
  Standard_Real myPntTol;
  TopoDS_Solid mySolid;
  Handle(BRepCheck_HSC) myHSC;
};
//
typedef NCollection_Vector<BRepCheck_ToolSolid>
  BRepCheck_VectorOfToolSolid;
//

//=======================================================================
//function : BRepCheck_Solid
//purpose  : 
//=======================================================================
BRepCheck_Solid::BRepCheck_Solid (const TopoDS_Solid& S)
{
  Init(S);
}
//=======================================================================
//function : Blind
//purpose  : 
//=======================================================================
void BRepCheck_Solid::Blind()
{
  if (!myBlind) {
    // nothing more than in the minimum
    myBlind = Standard_True;
  }
}
//=======================================================================
//function : InContext
//purpose  : 
//=======================================================================
void BRepCheck_Solid::InContext(const TopoDS_Shape& )
{
}
//=======================================================================
//function : Minimum
//purpose  : 
//=======================================================================
void BRepCheck_Solid::Minimum()
{
  if (myMin) {
    return;
  }
  myMin = Standard_True;
  //
  Standard_Boolean bFound, bIsHole, bFlag;
  Standard_Integer i, j, aNbVTS, aNbVTS1, iCntSh, iCntShInt;
  TopoDS_Solid aZ;
  TopoDS_Iterator aIt, aItF;
  TopoDS_Builder aBB;
  TopExp_Explorer aExp;
  TopTools_MapOfShape aMSS;
  TopAbs_Orientation aOr; 
  BRepCheck_VectorOfToolSolid aVTS;
  BRepCheck_ListOfStatus thelist;
  //
  myMap.Bind(myShape, thelist);
  BRepCheck_ListOfStatus& aLST = myMap(myShape);
  aLST.Append(BRepCheck_NoError);
  //
  //-------------------------------------------------
  // 1. InvalidImbricationOfShells
  bFound=Standard_False;
  aExp.Init(myShape, TopAbs_FACE);
  for (; !bFound && aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aF=aExp.Current();
    if (!aMSS.Add(aF)) {
      BRepCheck::Add(myMap(myShape), 
                     BRepCheck_InvalidImbricationOfShells);
      bFound=!bFound;
    }
  } 
  //
  //-------------------------------------------------
  // 2. 
  //    - Too many growths,
  //    - There is smt of the solid that is out of solid
  iCntSh=0;
  iCntShInt=0;
  aIt.Initialize(myShape);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSx=aIt.Value();
    //
    if (aSx.ShapeType()!=TopAbs_SHELL) {
      aOr=aSx.Orientation();
      if (aOr!=TopAbs_INTERNAL) {
        BRepCheck::Add(myMap(myShape), 
                       BRepCheck_BadOrientationOfSubshape);
      } 
      continue;
    }
    //
    const TopoDS_Shell& aSh=*((TopoDS_Shell*)&aSx);
    //
    // Skip internal shells
    bFound=Standard_False;
    aItF.Initialize(aSh);
    for (; !bFound && aItF.More(); aItF.Next()) {
      const TopoDS_Shape& aF=aItF.Value();
      aOr=aF.Orientation();
      if (aOr==TopAbs_INTERNAL) {
        bFound=!bFound;
      }
    }
    if (bFound) {
      ++iCntShInt;
      continue;
    }
    //
    ++iCntSh;
    //
    // Skip not closed shells
    if (!BRep_Tool::IsClosed(aSh)) {
      continue;
    }
    //
    aBB.MakeSolid(aZ);
    aBB.Add(aZ, aSh);
    //
    BRepCheck_ToolSolid aTS;
    //
    aTS.SetSolid(aZ);
    aVTS.Append(aTS);
  }//for (; aIt.More(); aIt.Next()) {
  //
  if (!iCntSh && iCntShInt) {
    // all shells in the solid are internal
    BRepCheck::Add(myMap(myShape), 
                   BRepCheck_BadOrientationOfSubshape);
  }
  //
  aNbVTS=aVTS.Size();
  if (aNbVTS<2) {
    return;
  }
  //
  aNbVTS1=0;
  for (i=0; i<aNbVTS; ++i) {
    BRepCheck_ToolSolid& aTS=aVTS(i);
    //
    aTS.Init();
    bIsHole=aTS.IsHole();
    if (!bIsHole) {
      ++aNbVTS1;
      if (aNbVTS1>1) {
        // Too many growths
        BRepCheck::Add(myMap(myShape), 
                       BRepCheck_EnclosedRegion);
        break;
      }
    }
  }
  //
  bFound=Standard_False;
  aNbVTS1=aNbVTS-1;
  for (i=0; !bFound && i<aNbVTS1; ++i) {
    BRepCheck_ToolSolid& aTSi=aVTS(i);
    //
    for (j=i+1; !bFound && j<aNbVTS; ++j) {
      BRepCheck_ToolSolid& aTSj=aVTS(j);
      //
      bFlag=aTSi.IsOut(aTSj);
      if (bFlag) {
        // smt of solid is out of solid
        BRepCheck::Add(myMap(myShape), 
                       BRepCheck_SubshapeNotInShape);
        bFound=!bFound;
      }
    }
  }
  //
  //myMin = Standard_True;
}
