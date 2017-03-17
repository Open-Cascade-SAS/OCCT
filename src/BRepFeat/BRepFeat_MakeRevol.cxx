// Created on: 1996-02-13
// Created by: Jacques GOUSSARD
// Copyright (c) 1996-1999 Matra Datavision
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
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI.hxx>
#include <BRepFeat.hxx>
#include <BRepFeat_MakeRevol.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepSweep_Revol.hxx>
#include <BRepTools_Modifier.hxx>
#include <BRepTools_TrsfModification.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <LocOpe.hxx>
#include <LocOpe_BuildShape.hxx>
#include <LocOpe_CSIntersector.hxx>
#include <LocOpe_FindEdges.hxx>
#include <LocOpe_Gluer.hxx>
#include <LocOpe_PntFace.hxx>
#include <LocOpe_Revol.hxx>
#include <LocOpe_SequenceOfCirc.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean BRepFeat_GettraceFEAT();
#endif

static void MajMap(const TopoDS_Shape&, // base
  const LocOpe_Revol&,
  TopTools_DataMapOfShapeListOfShape&, // myMap
  TopoDS_Shape&,  // myFShape
  TopoDS_Shape&); // myLShape


static void VerifGluedFaces(const TopoDS_Face& theSkface,
  const TopoDS_Shape& thePbase,
  Handle(Geom_Curve)& theBCurve,
  TColGeom_SequenceOfCurve& theCurves,
  LocOpe_Revol& theRevol,
  TopTools_DataMapOfShapeShape& theMap);


static Standard_Boolean ToFuse(const TopoDS_Face& ,
  const TopoDS_Face&);




//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void BRepFeat_MakeRevol::Init(const TopoDS_Shape& Sbase,
  const TopoDS_Shape& Pbase,
  const TopoDS_Face& Skface,
  const gp_Ax1& Axis,
  const Standard_Integer Mode,
  const Standard_Boolean Modify)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakeRevol::Init" << endl;
#endif
  myAxis   = Axis;
  myPbase  = Pbase;
  mySbase  = Sbase;
  BasisShapeValid();
  mySkface = Skface;
  SketchFaceValid();
  myPbase  = Pbase;
  mySlface.Clear();
  if(Mode == 0) {
    myFuse   = Standard_False;
    myJustFeat = Standard_False;
  }
  else if(Mode == 1) {
    myFuse   = Standard_True;
    myJustFeat = Standard_False;
  }
  else if(Mode == 2) {
    myFuse   = Standard_True;
    myJustFeat = Standard_True;
  }
  else {    
  }
  myModify = Modify;
  myJustGluer = Standard_False;

  //-------------- ifv
  //  mySkface.Nullify();
  //-------------- ifv


  myShape.Nullify();
  myMap.Clear();
  myFShape.Nullify();
  myLShape.Nullify();
  TopExp_Explorer exp;
  for (exp.Init(mySbase,TopAbs_FACE);exp.More();exp.Next()) {
    TopTools_ListOfShape thelist;
    myMap.Bind(exp.Current(), thelist);
    myMap(exp.Current()).Append(exp.Current());
  }
#ifdef OCCT_DEBUG
  if (trc) {
    if (myJustFeat)  cout << " Just Feature" << endl;
    if (myFuse)  cout << " Fuse" << endl;
    if (!myFuse)  cout << " Cut" << endl;
    if (!myModify) cout << " Modify = 0" << endl;
  }
#endif 
}


//=======================================================================
//function : Add
//purpose  : add faces add edges of sliding
//=======================================================================

void BRepFeat_MakeRevol::Add(const TopoDS_Edge& E,
  const TopoDS_Face& F)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakeRevol::Add(Edge,face)" << endl;
#endif
  TopExp_Explorer exp;
  for (exp.Init(mySbase,TopAbs_FACE);exp.More();exp.Next()) {
    if (exp.Current().IsSame(F)) {
      break;
    }
  }
  if (!exp.More()) {
    throw Standard_ConstructionError();
  }

  for (exp.Init(myPbase,TopAbs_EDGE);exp.More();exp.Next()) {
    if (exp.Current().IsSame(E)) {
      break;
    }
  }
  if (!exp.More()) {
    throw Standard_ConstructionError();
  }

  if (!mySlface.IsBound(F)) {
    TopTools_ListOfShape thelist;
    mySlface.Bind(F, thelist);
  }
  TopTools_ListIteratorOfListOfShape itl(mySlface(F));
  for (; itl.More();itl.Next()) {
    if (itl.Value().IsSame(E)) {
      break;
    }
  }
  if (!itl.More()) {
    mySlface(F).Append(E);
  }
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepFeat_MakeRevol::Perform(const Standard_Real Angle)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakeRevol::Perform(Angle)" << endl;
#endif
  mySFrom.Nullify();
  ShapeFromValid();
  mySUntil.Nullify();
  ShapeUntilValid();
  myGluedF.Clear();
  myPerfSelection = BRepFeat_NoSelection;
  PerfSelectionValid();
  Standard_Boolean RevolComp = (2*M_PI-Abs(Angle) <= Precision::Angular());
  LocOpe_Revol theRevol;
  Standard_Real angledec = 0.;
  TopExp_Explorer exp;
  if(RevolComp) {
    /*
    if (!mySkface.IsNull() || !mySlface.IsEmpty()) {
    for (exp.Init(mySbase,TopAbs_FACE); exp.More(); exp.Next()) {
    if (exp.Current().IsSame(mySkface)) {
    angledec = M_PI/5; // pourquoi pas
    if (myFuse) angledec = -angledec;
    break;
    }
    }
    }
    */
    mySkface.Nullify();
  }
  if(angledec == 0.) theRevol.Perform(myPbase, myAxis, Angle);
  else theRevol.Perform(myPbase, myAxis, Angle, angledec);

  TopoDS_Shape VraiRevol = theRevol.Shape();

  MajMap(myPbase,theRevol,myMap,myFShape,myLShape);

  myGShape = VraiRevol;
  GeneratedShapeValid();
  TopoDS_Shape Base = theRevol.FirstShape();
  exp.Init(Base, TopAbs_FACE);
  TopoDS_Face theBase = TopoDS::Face(exp.Current());
  exp.Next();
  if(exp.More()) {
    NotDone();
    myStatusError = BRepFeat_InvFirstShape;
    return;
  }

  TopoDS_Face FFace;

  Standard_Boolean found = Standard_False;

  if(!mySkface.IsNull() || !mySlface.IsEmpty()) {
    if(myLShape.ShapeType() == TopAbs_WIRE) {
      TopExp_Explorer ex1(VraiRevol, TopAbs_FACE);
      for(; ex1.More(); ex1.Next()) {
        TopExp_Explorer ex2(ex1.Current(), TopAbs_WIRE);
        for(; ex2.More(); ex2.Next()) {
          if(ex2.Current().IsSame(myLShape)) {
            FFace = TopoDS::Face(ex1.Current());
            found = Standard_True;
            break;
          }
        }
        if(found) break;
      }
    }

    TopExp_Explorer anExp(mySbase, TopAbs_FACE);
    for(; anExp.More(); anExp.Next()) {
      const TopoDS_Face& ff = TopoDS::Face(anExp.Current());
      if(ToFuse(ff, FFace)) {
        TopTools_DataMapOfShapeListOfShape sl;
        if(!FFace.IsSame(myPbase) && BRepFeat::IsInside(ff, FFace)) 
          break;
      }
    }
  }
  GluedFacesValid();
  if (!mySkface.IsNull()) {
    VerifGluedFaces(mySkface, theBase, myBCurve, myCurves, theRevol, myGluedF);
  }

  if(myGluedF.IsEmpty()) {
    if(myFuse == 1) {
      BRepAlgoAPI_Fuse f(mySbase, myGShape);
      myShape = f.Shape();
      UpdateDescendants(f, myShape, Standard_False);
      Done();
    }
    else if(myFuse == 0) {
      BRepAlgoAPI_Cut c(mySbase, myGShape);
      myShape = c.Shape();
      UpdateDescendants(c, myShape, Standard_False);
      Done();
    }
    else {
      myShape = myGShape;
      Done();
    }
  }
  else {
    theRevol.Curves(myCurves);
    myBCurve = theRevol.BarycCurve();
    GlobalPerform();
  }
}


//=======================================================================
//function : Perform
//purpose  : feature till shape Until
//=======================================================================

void BRepFeat_MakeRevol::Perform(const TopoDS_Shape& Until)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakeRevol::Perform(Until)" << endl;
#endif
  Standard_Real Angle = 0.;
  Standard_Boolean TourComplet = Standard_False;

  if (Until.IsNull()) {
    throw Standard_ConstructionError();
  }
  TopExp_Explorer exp(Until, TopAbs_FACE);
  if (!exp.More()) {
    throw Standard_ConstructionError();
  }
  if (!mySkface.IsNull() && Until.IsSame(mySkface)) {
    Angle = 2*M_PI;
    TourComplet = Standard_True;
  }
  myGluedF.Clear();
  myPerfSelection = BRepFeat_SelectionU;
  PerfSelectionValid();
  mySFrom.Nullify();
  ShapeFromValid();
  mySUntil = Until;
  Standard_Boolean Trf = TransformShapeFU(1);
  ShapeUntilValid();

  // Do systematically almost complete revolution
  // BRepSweep_Revol theRevol(myPbase,myAxis,2.*M_PI-10.*Precision::Angular());
  LocOpe_Revol theRevol;
  if(!TourComplet) {
    Angle = 2.*M_PI- 3*M_PI/180.;
#ifdef OCCT_DEBUG
    if (trc) cout << " No complete Revolution" << endl;
#endif
  }
  theRevol.Perform(myPbase, myAxis, Angle);
  TopoDS_Shape VraiRevol = theRevol.Shape();
  MajMap(myPbase,theRevol,myMap,myFShape,myLShape);


  if(!Trf) {

    myGShape = VraiRevol;
    GeneratedShapeValid();

    TopoDS_Shape Base = theRevol.FirstShape();
    exp.Init(Base, TopAbs_FACE);
    TopoDS_Face theBase = TopoDS::Face(exp.Current());
    exp.Next();
    if(exp.More()) {
      NotDone();
      myStatusError = BRepFeat_InvFirstShape;
      return;
    }
    GluedFacesValid();
    //VerifGluedFaces(mySkface, theBase, myBCurve, myCurves, theRevol, myGluedF);

    theRevol.Curves(myCurves);
    myBCurve = theRevol.BarycCurve();
    GlobalPerform();
  }
  else {
    TColGeom_SequenceOfCurve scur;
    theRevol.Curves(myCurves);
    myBCurve = theRevol.BarycCurve();    
    scur.Clear();    
    scur.Append(myBCurve);
    LocOpe_CSIntersector ASI(mySUntil);
    ASI.Perform(scur);
    if (ASI.IsDone() && ASI.NbPoints(1) >=1) {
      TopAbs_Orientation Or = ASI.Point(1,1).Orientation();
      TopoDS_Face FUntil = ASI.Point(1,1).Face();
      TopoDS_Shape Comp;
      BRep_Builder B;
      B.MakeCompound(TopoDS::Compound(Comp));
      TopoDS_Solid S = BRepFeat::Tool(mySUntil, FUntil, Or);
      if (!S.IsNull()) B.Add(Comp,S);
      BRepAlgoAPI_Cut trP(VraiRevol,Comp);
      TopoDS_Shape Cutsh = trP.Shape();
      TopExp_Explorer ex(Cutsh, TopAbs_SOLID);
      for(; ex.More(); ex.Next()) {
        TopExp_Explorer ex1(ex.Current(), TopAbs_FACE);
        for(; ex1.More(); ex1.Next()) {
          const TopoDS_Face& fac = TopoDS::Face(ex1.Current());
          if(fac.IsSame(myPbase)) {
            VraiRevol = ex.Current();
            break;
          }
        }
      }
      if(myFuse == 1) {
        BRepAlgoAPI_Fuse f(mySbase, VraiRevol);
        myShape = f.Shape();
        UpdateDescendants(f, myShape, Standard_False);
        Done();
      }
      else if(myFuse == 0) {
        BRepAlgoAPI_Cut c(mySbase, VraiRevol);
        myShape = c.Shape();
        UpdateDescendants(c, myShape, Standard_False);
        Done();
      }
      else {
        myShape = VraiRevol;
        Done();	
      }
    }         
  }
}


//=======================================================================
//function : Perform
//purpose  : feature limited by two shapes
//=======================================================================

void BRepFeat_MakeRevol::Perform(const TopoDS_Shape& From,
  const TopoDS_Shape& Until)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakeRevol::Perform(From,Until)" << endl;
#endif
  if (From.IsNull() || Until.IsNull()) {
    throw Standard_ConstructionError();
  }
  if (!mySkface.IsNull()) {
    if (From.IsSame(mySkface)) {
      myJustGluer = Standard_True;
      Perform(Until);
      if (myJustGluer) return;
    }
    else if (Until.IsSame(mySkface)) {
      myJustGluer = Standard_True;
      myAxis.Reverse();
      Perform(From);
      if (myJustGluer) return;
    }
  }

  myGluedF.Clear();
  myPerfSelection = BRepFeat_SelectionFU;
  PerfSelectionValid();

  TopExp_Explorer exp(From, TopAbs_FACE);
  if (!exp.More()) {
    throw Standard_ConstructionError();
  }
  exp.Init(Until, TopAbs_FACE);
  if (!exp.More()) {
    throw Standard_ConstructionError();
  }

  mySFrom = From;
  Standard_Boolean Trff = TransformShapeFU(0);
  ShapeFromValid();
  mySUntil = Until;
  Standard_Boolean Trfu = TransformShapeFU(1);
  ShapeUntilValid();  

  if(Trfu != Trff) {
    NotDone();
    myStatusError = BRepFeat_IncTypes;
    return;
  }

  LocOpe_Revol theRevol;
  theRevol.Perform(myPbase, myAxis, 2*M_PI);
  TopoDS_Shape VraiRevol = theRevol.Shape();

  MajMap(myPbase,theRevol,myMap,myFShape,myLShape);

  if(!Trff) {    
    myGShape = VraiRevol;
    GeneratedShapeValid();
    GluedFacesValid();
    //    VerifGluedFaces(mySkface, theBase, myBCurve, myCurves, theRevol, myGluedF);

    theRevol.Curves(myCurves);
    myBCurve = theRevol.BarycCurve();
    GlobalPerform();
  }
  else {
    theRevol.Curves(myCurves);
    myBCurve = theRevol.BarycCurve();    
    TColGeom_SequenceOfCurve scur;
    scur.Clear();    
    scur.Append(myBCurve);
    LocOpe_CSIntersector ASI1(mySUntil);
    LocOpe_CSIntersector ASI2(mySFrom);
    ASI1.Perform(scur);
    ASI2.Perform(scur);
    TopAbs_Orientation OrU, OrF;
    TopoDS_Face FFrom, FUntil;
    Standard_Real PrF, PrU;
    if (ASI1.IsDone() && ASI1.NbPoints(1) >=1) {
      OrU = ASI1.Point(1,1).Orientation();
      FUntil = ASI1.Point(1,1).Face();
      PrU = ASI1.Point(1,1).Parameter();
    }
    else {
      NotDone();
      myStatusError = BRepFeat_NoIntersectU;
      return;
    }
    if (ASI2.IsDone() && ASI2.NbPoints(1) >=1) {
      Standard_Real pr1 = ASI2.Point(1,1).Parameter();
      pr1 = ElCLib::InPeriod(pr1,PrU-2*M_PI,PrU);
      Standard_Real pr2 = ASI2.Point(1,ASI2.NbPoints(1)).Parameter();
      pr2 = ElCLib::InPeriod(pr2,PrU-2*M_PI,PrU);
      //OrF = OrU;
      OrF = TopAbs::Reverse(OrU);
      FFrom = ASI2.Point(1,1).Face();
      PrF = Max(pr1, pr2);
    }
    else {
      NotDone();
      myStatusError = BRepFeat_NoIntersectF;
      return;
    }
    if(!(PrU > PrF)) {
      NotDone();
      myStatusError = BRepFeat_IncParameter;
      return;
    }
    TopoDS_Shape Comp;
    BRep_Builder B;
    B.MakeCompound(TopoDS::Compound(Comp));
    TopoDS_Solid SF = BRepFeat::Tool(mySFrom, FFrom, OrF);
    if (!SF.IsNull()) B.Add(Comp,SF);
    TopoDS_Solid SU = BRepFeat::Tool(mySUntil, FUntil, OrU);
    if (!SU.IsNull()) B.Add(Comp,SU);
    BRepAlgoAPI_Cut trP(VraiRevol,Comp);
    TopoDS_Shape Cutsh = trP.Shape();
    TopExp_Explorer ex(Cutsh, TopAbs_SOLID);
    VraiRevol = ex.Current();
    for(; ex.More(); ex.Next()) {
      Standard_Real PrCur = BRepFeat::
        ParametricBarycenter(ex.Current(), myBCurve);
      if(PrF <= PrCur && PrU >= PrCur) {
        VraiRevol = ex.Current();
        break;
      }
    }
    if(myFuse == 1 && !myJustFeat) {
      BRepAlgoAPI_Fuse f(mySbase, VraiRevol);
      myShape = f.Shape();
      UpdateDescendants(f, myShape, Standard_False);
      Done();
    }
    else if(myFuse == 0 && !myJustFeat) {
      BRepAlgoAPI_Cut c(mySbase, VraiRevol);
      myShape = c.Shape();
      UpdateDescendants(c, myShape, Standard_False);
      Done();
    }
    else {
      myShape = VraiRevol;
      Done();	
    }
  }
}


//=======================================================================
//function : PerformThruAll
//purpose  : feature throughout the initial shape
//=======================================================================

void BRepFeat_MakeRevol::PerformThruAll()
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakeRevol::PerformThruAll()" << endl;
#endif
  Perform(2.*M_PI);
}

//=======================================================================
//function : PerformUntilAngle
//purpose  : feature till shape Until defined with the angle
//=======================================================================

void BRepFeat_MakeRevol::PerformUntilAngle(const TopoDS_Shape& Until,
  const Standard_Real Angle)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = BRepFeat_GettraceFEAT();
  if (trc) cout << "BRepFeat_MakeRevol::PerformUntilAngle(Until,Angle)" << endl;
#endif
  if (Until.IsNull()) {
    Perform(Angle);
  }
  if(Angle == 0) {
    Perform(Until);
  }
  TopExp_Explorer exp(Until, TopAbs_FACE);
  if (!exp.More()) {
    throw Standard_ConstructionError();
  }
  if (!mySkface.IsNull() && Until.IsSame(mySkface)) {
    Perform(Angle);
    return;
  }
  myGluedF.Clear();
  myPerfSelection = BRepFeat_NoSelection;
  PerfSelectionValid();
  mySFrom.Nullify();
  ShapeFromValid();
  mySUntil = Until;
  Standard_Boolean Trf = TransformShapeFU(1);
  ShapeUntilValid();

  // Produce systematicallt an almost complete revolution
  //  BRepSweep_Revol theRevol(myPbase,myAxis,2.*M_PI-10.*Precision::Angular());
  LocOpe_Revol theRevol;
  theRevol.Perform(myPbase, myAxis, Angle);
  TopoDS_Shape VraiRevol = theRevol.Shape();

  MajMap(myPbase,theRevol,myMap,myFShape,myLShape);

  if(Trf) {
    myGShape = VraiRevol;
    GeneratedShapeValid();

    TopoDS_Shape Base = theRevol.FirstShape();
    exp.Init(Base, TopAbs_FACE);
    TopoDS_Face theBase = TopoDS::Face(exp.Current());
    exp.Next();
    if(exp.More()) {
      NotDone();
      myStatusError = BRepFeat_InvFirstShape;
      return;
    }
    GluedFacesValid();
    //VerifGluedFaces(mySkface, theBase, myBCurve, myCurves, theRevol, myGluedF);


    theRevol.Curves(myCurves);
    myBCurve = theRevol.BarycCurve();
    GlobalPerform();
  }
  else {
    TColGeom_SequenceOfCurve scur;
    theRevol.Curves(myCurves);
    myBCurve = theRevol.BarycCurve();    
    scur.Clear();    
    scur.Append(myBCurve);
    LocOpe_CSIntersector ASI(mySUntil);
    ASI.Perform(scur);
    if (ASI.IsDone() && ASI.NbPoints(1) >=1) {
      TopAbs_Orientation Or = ASI.Point(1,1).Orientation();
      TopoDS_Face FUntil = ASI.Point(1,1).Face();
      TopoDS_Shape Comp;
      BRep_Builder B;
      B.MakeCompound(TopoDS::Compound(Comp));
      TopoDS_Solid S = BRepFeat::Tool(mySUntil, FUntil, Or);
      if (!S.IsNull()) B.Add(Comp,S);
      BRepAlgoAPI_Cut trP(VraiRevol,Comp);
      TopoDS_Shape Cutsh = trP.Shape();
      TopExp_Explorer ex(Cutsh, TopAbs_SOLID);
      for(; ex.More(); ex.Next()) {
        TopExp_Explorer ex1(ex.Current(), TopAbs_FACE);
        for(; ex1.More(); ex1.Next()) {
          const TopoDS_Face& fac = TopoDS::Face(ex1.Current());
          if(fac.IsSame(myPbase)) {
            VraiRevol = ex.Current();
            break;
          }
        }
      }
      if(myFuse == 1) {
        BRepAlgoAPI_Fuse f(mySbase, VraiRevol);
        myShape = f.Shape();
        UpdateDescendants(f, myShape, Standard_False);
        Done();
      }
      else if(myFuse == 0) {
        BRepAlgoAPI_Cut c(mySbase, VraiRevol);
        myShape = c.Shape();
        UpdateDescendants(c, myShape, Standard_False);
        Done();
      }
      else {
        myShape = VraiRevol;
        Done();	
      }
    }         
  }
}
//=======================================================================
//function : Curves
//purpose  : circles parallel to the generating edge of revolution
//=======================================================================

void BRepFeat_MakeRevol::Curves(TColGeom_SequenceOfCurve& scur)
{
  scur = myCurves;
}

//=======================================================================
//function : BarycCurve
//purpose  : pass through the center of mass of the primitive
//=======================================================================

Handle(Geom_Curve) BRepFeat_MakeRevol::BarycCurve()
{
  return myBCurve;
}

//=======================================================================
//function : VerifGluedFaces
//purpose  : Check intersection Tool/theSkface = thePbase
//           if yes -> OK otherwise -> case without gluing
//=======================================================================

static void VerifGluedFaces(const TopoDS_Face& theSkface,
  const TopoDS_Shape& thePbase,
  Handle(Geom_Curve)& theBCurve,
  TColGeom_SequenceOfCurve& theCurves,
  LocOpe_Revol& theRevol,
  TopTools_DataMapOfShapeShape& theMap)
{
  Standard_Boolean GluedFaces = Standard_True;
  TopoDS_Shape VraiRevol = theRevol.Shape();

  TColGeom_SequenceOfCurve scur;
  theRevol.Curves(theCurves);
  theBCurve = theRevol.BarycCurve();    
  scur.Clear();    
  scur.Append(theBCurve);
  LocOpe_CSIntersector ASI(theSkface);
  ASI.Perform(scur);
  if (ASI.IsDone() && ASI.NbPoints(1) >=1) {
    TopAbs_Orientation Or = ASI.Point(1,1).Orientation();
    TopoDS_Face FSk = ASI.Point(1,1).Face();
    TopoDS_Shape Comp;
    BRep_Builder B;
    B.MakeCompound(TopoDS::Compound(Comp));
    TopoDS_Solid S = BRepFeat::Tool(theSkface, FSk, Or);
    if (!S.IsNull()) B.Add(Comp,S);
    BRepAlgoAPI_Cut trP(VraiRevol,Comp);
    TopoDS_Shape Cutsh = trP.Shape();
    TopExp_Explorer ex(Cutsh, TopAbs_SOLID);
    for(; ex.More(); ex.Next()) {
      TopExp_Explorer ex1(ex.Current(), TopAbs_FACE);
      for(; ex1.More(); ex1.Next()) {
        const TopoDS_Face& fac1 = TopoDS::Face(ex1.Current());
        TopExp_Explorer ex2(thePbase, TopAbs_FACE);
        for(; ex2.More(); ex2.Next()) {
          const TopoDS_Face& fac2 = TopoDS::Face(ex2.Current());
          if(fac1.IsSame(fac2)) break;
        }
        if (ex2.More()) break;
      }
      if (ex1.More()) continue;
      GluedFaces = Standard_False;
      break;
    }
    if (!GluedFaces) {
#ifdef OCCT_DEBUG
      Standard_Boolean trc = BRepFeat_GettraceFEAT();
      if (trc) cout << " Intersection Revol/skface : no gluing" << endl;
#endif
      theMap.Clear();
    }
  }
}

//=======================================================================
//function : MajMap
//purpose  : management of descendants
//=======================================================================

static void MajMap(const TopoDS_Shape& theB,
  const LocOpe_Revol& theP,
  TopTools_DataMapOfShapeListOfShape& theMap, // myMap
  TopoDS_Shape& theFShape,  // myFShape
  TopoDS_Shape& theLShape) // myLShape
{
  TopExp_Explorer exp(theP.FirstShape(),TopAbs_WIRE);
  if (exp.More()) {
    theFShape = exp.Current();
    TopTools_ListOfShape thelist;
    theMap.Bind(theFShape, thelist);
    for (exp.Init(theP.FirstShape(),TopAbs_FACE);exp.More();exp.Next()) {
      theMap(theFShape).Append(exp.Current());
    }
  }

  exp.Init(theP.LastShape(),TopAbs_WIRE);
  if (exp.More()) {
    theLShape = exp.Current();
    TopTools_ListOfShape thelist1;
    theMap.Bind(theLShape, thelist1);
    for (exp.Init(theP.LastShape(),TopAbs_FACE);exp.More();exp.Next()) {
      theMap(theLShape).Append(exp.Current());
    }
  }

  for (exp.Init(theB,TopAbs_EDGE); exp.More(); exp.Next()) {
    if (!theMap.IsBound(exp.Current())) {
      TopTools_ListOfShape thelist2;
      theMap.Bind(exp.Current(), thelist2);
      theMap(exp.Current()) = theP.Shapes(exp.Current());
    }
  }
}



//=======================================================================
//function : ToFuse
//purpose  : two faces samedomaine or not
//=======================================================================

Standard_Boolean ToFuse(const TopoDS_Face& F1,
  const TopoDS_Face& F2)
{
  if (F1.IsNull() || F2.IsNull()) {
    return Standard_False;
  }

  Handle(Geom_Surface) S1,S2;
  TopLoc_Location loc1, loc2;
  Handle(Standard_Type) typS1,typS2;
  const Standard_Real tollin = Precision::Confusion();
  const Standard_Real tolang = Precision::Angular();

  S1 = BRep_Tool::Surface(F1,loc1);
  S2 = BRep_Tool::Surface(F2,loc2);

  typS1 = S1->DynamicType();
  typS2 = S2->DynamicType();

  if (typS1 == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    S1 =  Handle(Geom_RectangularTrimmedSurface)::DownCast (S1)->BasisSurface();
    typS1 = S1->DynamicType();
  }

  if (typS2 == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    S2 =  Handle(Geom_RectangularTrimmedSurface)::DownCast (S2)->BasisSurface();
    typS2 = S2->DynamicType();
  }

  if (typS1 != typS2) {
    return Standard_False;
  }


  Standard_Boolean ValRet = Standard_False;
  if (typS1 == STANDARD_TYPE(Geom_Plane)) {
    S1 = BRep_Tool::Surface(F1);  // to apply the location.
    S2 = BRep_Tool::Surface(F2);
    gp_Pln pl1( Handle(Geom_Plane)::DownCast (S1)->Pln());
    gp_Pln pl2( Handle(Geom_Plane)::DownCast (S2)->Pln());

    if (pl1.Position().IsCoplanar(pl2.Position(),tollin,tolang)) {
      ValRet = Standard_True;
    }
  }

  return ValRet;
}









