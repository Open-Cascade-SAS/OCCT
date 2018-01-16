// Created on: 1998-07-22
// Created by: Philippe MANGIN
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_HCompCurve.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepFill.hxx>
#include <BRepFill_ACRLaw.hxx>
#include <BRepFill_CompatibleWires.hxx>
#include <BRepFill_DataMapOfShapeHArray2OfShape.hxx>
#include <BRepFill_Edge3DLaw.hxx>
#include <BRepFill_EdgeOnSurfLaw.hxx>
#include <BRepFill_LocationLaw.hxx>
#include <BRepFill_NSections.hxx>
#include <BRepFill_PipeShell.hxx>
#include <BRepFill_Section.hxx>
#include <BRepFill_SectionLaw.hxx>
#include <BRepFill_SectionPlacement.hxx>
#include <BRepFill_ShapeLaw.hxx>
#include <BRepFill_Sweep.hxx>
#include <BRepGProp.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeFace.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomFill_ConstantBiNormal.hxx>
#include <GeomFill_CorrectedFrenet.hxx>
#include <GeomFill_CurveAndTrihedron.hxx>
#include <GeomFill_DiscreteTrihedron.hxx>
#include <GeomFill_Fixed.hxx>
#include <GeomFill_Frenet.hxx>
#include <GeomFill_GuideTrihedronAC.hxx>
#include <GeomFill_GuideTrihedronPlan.hxx>
#include <GeomFill_LocationGuide.hxx>
#include <GeomFill_SectionLaw.hxx>
#include <GeomFill_TrihedronLaw.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <GProp_GProps.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <Law_Function.hxx>
#include <Law_Interpol.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TopExp.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_SequenceOfShape.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(BRepFill_PipeShell,Standard_Transient)

//Specification Guide
#ifdef DRAW
#include <Draw.hxx>
#include <DrawTrSurf.hxx>
#include <DBRep.hxx>
static Standard_Boolean Affich = 0;
#endif

#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfIntegerShape.hxx>
#include <TopoDS_Compound.hxx>


static Standard_Boolean BuildBoundaries(const BRepFill_Sweep&               theSweep,
					const Handle(BRepFill_SectionLaw)&  theSection,
					TopoDS_Shape&                       theBottom,
					TopoDS_Shape&                       theTop);

//=======================================================================
//function :  ComputeSection
//purpose  : Construct an intermediary section
//=======================================================================

static Standard_Boolean ComputeSection(const TopoDS_Wire& W1,
                                       const TopoDS_Wire& W2,
                                       const Standard_Real p1,
                                       const Standard_Real p2,
                                       TopoDS_Wire& Wres)
{
  TColStd_SequenceOfReal SR;
  TopTools_SequenceOfShape SSh;
  SR.Clear();
  SR.Append(0.);
  SR.Append(1.);
  SSh.Clear();
  SSh.Append(W1);
  SSh.Append(W2);
  BRepFill_CompatibleWires CW(SSh);
  CW.SetPercent(0.1);
  CW.Perform();
  if (!CW.IsDone()) throw StdFail_NotDone("Uncompatible wires");
  GeomFill_SequenceOfTrsf EmptyTrsfs;
  Handle(BRepFill_NSections) SL = new (BRepFill_NSections) (CW.Shape(),EmptyTrsfs,SR,0.,1.);
  Standard_Real US = p1/(p1+p2);
  SL->D0(US, Wres);
  return Standard_True;
}

				      

//=======================================================================
//function : PerformTransition
//purpose  : Modify a law of location depending on Transition
//=======================================================================

static void PerformTransition(const BRepFill_TransitionStyle Mode,
			      Handle(BRepFill_LocationLaw)& Loc,
			      const Standard_Real angmin)
{
 if (!Loc.IsNull()) {
   Loc->DeleteTransform();
   if (Mode == BRepFill_Modified) Loc->TransformInG0Law();
   else  Loc->TransformInCompatibleLaw(angmin);
 } 
}
//=======================================================================
//function :  PerformPlan
//purpose  : Construct a plane of filling if exists
//=======================================================================

static Standard_Boolean PerformPlan(TopoDS_Shape& S)
{
  Standard_Boolean isDegen = Standard_True;
  TopExp_Explorer explo(S, TopAbs_EDGE);
  for (; explo.More(); explo.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(explo.Current());
      if (!BRep_Tool::Degenerated(anEdge))
	isDegen = Standard_False;
    }
  if (isDegen)
    {
      S.Nullify();
      return Standard_True;
    }

  TopoDS_Wire W = TopoDS::Wire(S);
  Standard_Boolean Ok = Standard_False;
  if (!W.IsNull()) {
    BRepLib_MakeFace mkplan(W, Standard_True);
    if (mkplan.IsDone()) {
      S = mkplan.Face();
      Ok = Standard_True;
    }
  }
 return Ok;
}

//=============================================================================
//function :  IsSameOriented
//purpose  : Checks whether aFace is oriented to the same side as aShell or not
//=============================================================================

static Standard_Boolean IsSameOriented(const TopoDS_Shape& aFace,
				       const TopoDS_Shape& aShell)
{
  TopExp_Explorer Explo(aFace, TopAbs_EDGE);
  TopoDS_Shape anEdge = Explo.Current();
  TopAbs_Orientation Or1 = anEdge.Orientation();

  TopTools_IndexedDataMapOfShapeListOfShape EFmap;
  TopExp::MapShapesAndAncestors( aShell, TopAbs_EDGE, TopAbs_FACE, EFmap );

  const TopoDS_Shape& AdjacentFace = EFmap.FindFromKey(anEdge).First();
  TopoDS_Shape theEdge;
  for (Explo.Init(AdjacentFace, TopAbs_EDGE); Explo.More(); Explo.Next())
    {
      theEdge = Explo.Current();
      if (theEdge.IsSame(anEdge))
	break;
    }

  TopAbs_Orientation Or2 = theEdge.Orientation();
  if (Or1 == Or2)
    return Standard_False;
  return Standard_True;
}
//=======================================================================
//function : BRepFill_PipeShell
//purpose  : 
//=======================================================================
BRepFill_PipeShell::BRepFill_PipeShell(const TopoDS_Wire& Spine)
                      :  mySpine(Spine), 
                         myForceApproxC1(Standard_False),
                         myIsAutomaticLaw(Standard_False),
                         myTrihedron(GeomFill_IsCorrectedFrenet),
                         myTransition(BRepFill_Modified),
                         myStatus(GeomFill_PipeOk)
{
  myLocation.Nullify();
  mySection.Nullify();
  myLaw.Nullify();
  SetTolerance();

  myMaxDegree = 11;
  myMaxSegments = 100;

  // Attention to closed non-declared wire !
  if (!mySpine.Closed()) {
    TopoDS_Vertex Vf, Vl;
    TopExp::Vertices(mySpine, Vf, Vl);
    if (Vf.IsSame(Vl)) mySpine.Closed(Standard_True);
  }  
}

//=======================================================================
//function : Set
//purpose  : Define a law of Frenet (Correct)
//=======================================================================
 void BRepFill_PipeShell::Set(const Standard_Boolean IsFrenet) 
{
  Handle(GeomFill_TrihedronLaw) TLaw;
  if (IsFrenet) {
    myTrihedron = GeomFill_IsFrenet;
    TLaw = new (GeomFill_Frenet) ();
  }
  else {
    myTrihedron = GeomFill_IsFrenet;
    TLaw = new (GeomFill_CorrectedFrenet) ();
  }
  Handle(GeomFill_CurveAndTrihedron) Loc = 
    new (GeomFill_CurveAndTrihedron) (TLaw);
  myLocation = new (BRepFill_Edge3DLaw) (mySpine, Loc);
  mySection.Nullify(); //It is required to relocalize sections.
}

//=======================================================================
//function : SetDiscrete
//purpose  : Define a law of Discrete Trihedron
//=======================================================================
 void BRepFill_PipeShell::SetDiscrete() 
{
  Handle(GeomFill_TrihedronLaw) TLaw;

  myTrihedron = GeomFill_IsDiscreteTrihedron;
  TLaw = new (GeomFill_DiscreteTrihedron) ();

  Handle(GeomFill_CurveAndTrihedron) Loc = 
    new (GeomFill_CurveAndTrihedron) (TLaw);
  myLocation = new (BRepFill_Edge3DLaw) (mySpine, Loc);
  mySection.Nullify(); //It is required to relocalize sections.
}

//=======================================================================
//function : Set
//purpose  : Define a law Constant
//=======================================================================
 void BRepFill_PipeShell::Set(const gp_Ax2& Axe) 
{
  myTrihedron = GeomFill_IsFixed;
  gp_Vec V1, V2;
  V1.SetXYZ(Axe.Direction().XYZ());
  V2.SetXYZ(Axe.XDirection().XYZ());
  Handle(GeomFill_Fixed) TLaw = new (GeomFill_Fixed) (V1, V2);
  Handle(GeomFill_CurveAndTrihedron) Loc = 
    new (GeomFill_CurveAndTrihedron) (TLaw);
  myLocation = new (BRepFill_Edge3DLaw) (mySpine, Loc); 
  mySection.Nullify(); //It is required to relocalize sections.
}

//=======================================================================
//function : Set
//purpose  : Construct a law of location of binormal fixed type
//=======================================================================
 void BRepFill_PipeShell::Set(const gp_Dir& BiNormal) 
{
  myTrihedron = GeomFill_IsConstantNormal;

  Handle(GeomFill_ConstantBiNormal) TLaw = 
    new (GeomFill_ConstantBiNormal) (BiNormal);
  Handle(GeomFill_CurveAndTrihedron) Loc = 
    new (GeomFill_CurveAndTrihedron) (TLaw);
  myLocation = new (BRepFill_Edge3DLaw) (mySpine, Loc); 
  mySection.Nullify(); //Il faut relocaliser les sections.
}

//=======================================================================
//function : Set
//purpose  : Construct a law of location of Darboux type
//=======================================================================
 Standard_Boolean BRepFill_PipeShell::Set(const TopoDS_Shape& SpineSupport) 
{
 Standard_Boolean B;

  // A special law of location is required
 Handle(BRepFill_EdgeOnSurfLaw) loc = 
   new (BRepFill_EdgeOnSurfLaw) (mySpine, SpineSupport);
 B = loc->HasResult();
 if (B) {
   myLocation = loc; 
   myTrihedron = GeomFill_IsDarboux;
   mySection.Nullify(); //It is required to relocalize the sections.
 }
 return B;
}

//=======================================================================
//function : Set
//purpose  : Defines a lawv with help of a guided contour
//=======================================================================
 void BRepFill_PipeShell::Set(const TopoDS_Wire& AuxiliarySpine,
			      const Standard_Boolean CurvilinearEquivalence,
			      const BRepFill_TypeOfContact KeepContact) 
{  
  // Reorganization of the guide (pb of orientation and origin)
  TopoDS_Wire TheGuide;
  TheGuide =  AuxiliarySpine;
  Standard_Boolean SpClose = mySpine.Closed(), 
                   GuideClose = AuxiliarySpine.Closed();

  if (KeepContact == BRepFill_ContactOnBorder)
    myIsAutomaticLaw = Standard_True;
  
  if (!SpClose && !GuideClose) {
    // Case open reorientation of the guide
    TopoDS_Wire sp = mySpine;
    TopTools_SequenceOfShape Seq;
    Seq.Append(sp);
    Seq.Append(TheGuide);
    BRepFill_CompatibleWires CW(Seq);
    CW.SetPercent(0.1);
    CW.Perform();
    if (!CW.IsDone()) throw StdFail_NotDone("Uncompatible wires");
    TheGuide = TopoDS::Wire(CW.Shape().Value(2));
  }
  else if (GuideClose) {
    // Case guide closed : Determination of the origin 
    // & reorientation of the guide
    gp_Vec Dir;
    gp_Pnt SpOr;
    if (!SpClose) {
      TopoDS_Vertex Vf, Vl;
      gp_Pnt P;
      TopExp::Vertices(mySpine, Vf, Vl);
      SpOr = BRep_Tool::Pnt(Vf);
      P = BRep_Tool::Pnt(Vl);
      gp_Vec V(P, SpOr);
      SpOr.BaryCenter(0.5, P, 0.5);
      Dir = V;
    }
    else {
      BRepAdaptor_CompCurve BC(mySpine);
      BC.D1(0,SpOr,Dir); 
    } 
    BRepFill::SearchOrigin(TheGuide, SpOr, Dir, 100*myTol3d);
  }

#ifdef DRAW
  if (Affich)
    DBRep::Set("theguide", TheGuide);
#endif
  // transform the guide in a single curve
  Handle(BRepAdaptor_HCompCurve) Guide = new (BRepAdaptor_HCompCurve) (TheGuide);

  if (CurvilinearEquivalence) { // trihedron by curvilinear reduced abscissa
    if (KeepContact == BRepFill_Contact ||
        KeepContact == BRepFill_ContactOnBorder) 
      myTrihedron = GeomFill_IsGuideACWithContact; // with rotation 
    else
      myTrihedron = GeomFill_IsGuideAC; // without rotation 
      
    Handle(GeomFill_GuideTrihedronAC) TLaw
      = new (GeomFill_GuideTrihedronAC) (Guide);
    Handle(GeomFill_LocationGuide) Loc = 
      new (GeomFill_LocationGuide) (TLaw);      
    myLocation = new (BRepFill_ACRLaw) (mySpine, Loc); 	
  }
  else {// trihedron by plane
    if (KeepContact == BRepFill_Contact ||
        KeepContact == BRepFill_ContactOnBorder) 
      myTrihedron = GeomFill_IsGuidePlanWithContact; // with rotation 
    else 
      myTrihedron = GeomFill_IsGuidePlan; // without rotation

    Handle(GeomFill_GuideTrihedronPlan) TLaw = 
      new (GeomFill_GuideTrihedronPlan) (Guide);    
    Handle(GeomFill_LocationGuide) Loc = 
      new (GeomFill_LocationGuide) (TLaw);
    myLocation = new (BRepFill_Edge3DLaw) (mySpine, Loc);  
  }    
  mySection.Nullify(); //It is required to relocalize the sections.
}


//=======================================================================
//function : SetMaxDegree
//purpose  : 
//=======================================================================
void BRepFill_PipeShell::SetMaxDegree(const Standard_Integer NewMaxDegree)
{
  myMaxDegree = NewMaxDegree;
}

//=======================================================================
//function : SetMaxSegments
//purpose  : 
//=======================================================================
void BRepFill_PipeShell::SetMaxSegments(const Standard_Integer NewMaxSegments)
{
  myMaxSegments = NewMaxSegments;
}

//=======================================================================
//function : SetForceApproxC1
//purpose  : Set the flag that indicates attempt to approximate
//           a C1-continuous surface if a swept surface proved
//           to be C0.
//=======================================================================
void BRepFill_PipeShell::SetForceApproxC1(const Standard_Boolean ForceApproxC1)
{
  myForceApproxC1 = ForceApproxC1;
}

//=======================================================================
//function : Add
//purpose  : Add a Section
//=======================================================================
 void BRepFill_PipeShell::Add(const TopoDS_Shape& Profile,
			      const Standard_Boolean WithContact,
			      const Standard_Boolean WithCorrection) 
{ 
  TopoDS_Vertex V;
  V.Nullify();
  Add(Profile, V, WithContact, WithCorrection);
  ResetLoc();
}

//=======================================================================
//function : Add
//purpose  : Add a Section
//=======================================================================
 void BRepFill_PipeShell::Add(const TopoDS_Shape& Profile,
			      const TopoDS_Vertex& Location,
			      const Standard_Boolean WithContact,
			      const Standard_Boolean WithCorrection) 
{
 DeleteProfile(Profile); // No duplication
 if (myIsAutomaticLaw)
 {
   mySeq.Clear();
   BRepFill_Section S (Profile, Location, WithContact, WithCorrection);
   S.Set(Standard_True);
   mySeq.Append(S);
   mySection.Nullify();
   ResetLoc();

   Handle(GeomFill_LocationGuide) Loc = Handle(GeomFill_LocationGuide)::DownCast(myLocation->Law(1));
   Handle(TColgp_HArray1OfPnt2d) ParAndRad;
   Loc->ComputeAutomaticLaw(ParAndRad);
   
   //Compuite initial width of section (this will be 1.)
   GProp_GProps GlobalProps;
   BRepGProp::LinearProperties(Profile, GlobalProps);
   gp_Pnt BaryCenter = GlobalProps.CentreOfMass();

   TopoDS_Face ProfileFace = BRepLib_MakeFace(TopoDS::Wire(Profile), Standard_True); //only plane
   Handle(Geom_Surface) thePlane = BRep_Tool::Surface(ProfileFace);
   Handle(GeomAdaptor_HSurface) GAHplane = new GeomAdaptor_HSurface(thePlane);
   IntCurveSurface_HInter Intersector;
   Handle(Adaptor3d_HCurve) aHCurve [2];
   aHCurve[0] = Loc->GetCurve();
   aHCurve[1] = Loc->Guide();
   gp_Pnt PointsOnSpines [2];
   Standard_Integer i, j;

   for (i = 0; i < 2; i++)
   {
     Intersector.Perform(aHCurve[i], GAHplane);
     Standard_Real MinDist = RealLast();
     for (j = 1; j <= Intersector.NbPoints(); j++)
     {
       gp_Pnt aPint = Intersector.Point(j).Pnt();
       Standard_Real aDist = BaryCenter.Distance(aPint);
       if (aDist < MinDist)
       {
         MinDist = aDist;
         PointsOnSpines[i] = aPint;
       }
     }
   }

   //Correct <ParAndRad> according to <InitialWidth>
   Standard_Real InitialWidth = PointsOnSpines[0].Distance(PointsOnSpines[1]);
   Standard_Integer NbParRad = ParAndRad->Upper();
   for (i = 1; i <= NbParRad; i++)
   {
     gp_Pnt2d aParRad = ParAndRad->Value(i);
     aParRad.SetY( aParRad.Y() / InitialWidth );
     ParAndRad->SetValue(i, aParRad);
   }
  
   myLaw = new Law_Interpol();
   
   Standard_Boolean IsPeriodic =
     (Abs(ParAndRad->Value(1).Y() - ParAndRad->Value(NbParRad).Y()) < Precision::Confusion());

   (Handle(Law_Interpol)::DownCast(myLaw))->Set(ParAndRad->Array1(), IsPeriodic);
 }
 else
 {
   BRepFill_Section S (Profile, Location, WithContact, WithCorrection);
   mySeq.Append(S);
   mySection.Nullify();
   ResetLoc();
 }
}

//=======================================================================
//function : SetLaw
//purpose  : Section + law of homothety
//=======================================================================
 void BRepFill_PipeShell::SetLaw(const TopoDS_Shape& Profile,
				 const Handle(Law_Function)& L,
				 const Standard_Boolean WithContact,
				 const Standard_Boolean WithCorrection) 
{
 TopoDS_Vertex V;
 V.Nullify();
 SetLaw( Profile, L, V, WithContact, WithCorrection);
 ResetLoc();
}

//=======================================================================
//function : SetLaw
//purpose  :  Section + Law of homothety
//=======================================================================
 void BRepFill_PipeShell::SetLaw(const TopoDS_Shape& Profile,
				 const Handle(Law_Function)& L,
				 const TopoDS_Vertex& Location,
				 const Standard_Boolean WithContact,
				 const Standard_Boolean WithCorrection) 
{
  mySeq.Clear();
  BRepFill_Section S (Profile, Location, WithContact, WithCorrection);
  S.Set(Standard_True);
  mySeq.Append(S);
  myLaw = L;
  mySection.Nullify();
  ResetLoc();
}

//=======================================================================
//function : Delete
//purpose  : Delete a section
//=======================================================================
 void BRepFill_PipeShell::DeleteProfile(const TopoDS_Shape&  Profile)
{
  Standard_Boolean Trouve=Standard_False;
  Standard_Integer ii;
  for (ii=1; ii<=mySeq.Length() && !Trouve; ii++) {
    const TopoDS_Shape& aSection = mySeq.Value(ii).OriginalShape();
    if (Profile.IsSame(aSection))
    {
      Trouve = Standard_True;
      mySeq.Remove(ii);
    }
  }

  if (Trouve) mySection.Nullify();
  ResetLoc();
}


//=======================================================================
//function : IsReady
//purpose  : 
//=======================================================================
 Standard_Boolean BRepFill_PipeShell::IsReady() const
{
 return (mySeq.Length() != 0);
}
//=======================================================================
//function : GetStatus
//purpose  : 
//=======================================================================
 GeomFill_PipeError BRepFill_PipeShell::GetStatus() const
{
 return myStatus;
}


//=======================================================================
//function : SetTolerance
//purpose  :
//=======================================================================
 void BRepFill_PipeShell::SetTolerance(const Standard_Real Tol3d  ,
				       const Standard_Real BoundTol, 
				       const Standard_Real TolAngular)
{
 myTol3d = Tol3d;
 myBoundTol =  BoundTol;
 myTolAngular = TolAngular;
}

//=======================================================================
//function : SetTransition
//purpose  : Defines the mode of processing of corners
//=======================================================================
 void BRepFill_PipeShell::SetTransition(const BRepFill_TransitionStyle Mode,
					const Standard_Real Angmin,
					const Standard_Real Angmax)
{
 if (myTransition != Mode) 
   mySection.Nullify(); //It is required to relocalize the sections.
 myTransition = Mode;
 angmin =  Angmin;
 angmax = Angmax;
}  

//=======================================================================
//function : Simulate
//purpose  : Calculate N Sections
//=======================================================================
 void BRepFill_PipeShell::Simulate(const Standard_Integer N, 
				   TopTools_ListOfShape& List)
{
  // Preparation
  Prepare();
  List.Clear();

  Standard_Real First, Last, Length, Delta, U, 
                US, DeltaS,FirstS;
  Standard_Integer ii, NbL = myLocation->NbLaw();
  Standard_Boolean Finis=Standard_False;
  TopoDS_Shape W;
  
  // Calculate the parameters of digitalization
  mySection->Law(1)->GetDomain(FirstS, Last);
  DeltaS = Last - FirstS; 
  myLocation->CurvilinearBounds(NbL,First, Length);
  Delta = Length;
  if (N>1) Delta /= (N-1);

  myLocation->CurvilinearBounds(1,First, Last); // Initiation of Last
  for (U=0.0, ii=1; !Finis ; U+=Delta) {
    if (U >= Length) {
      U = Length;
      Finis = Standard_True;
    }
    else {
      if (ii <  NbL) myLocation->CurvilinearBounds(NbL,First, Last);
      if (U > Last) U = (Last+First)/2; // The edge is not skipped
      if (U> First) ii++;
    }
    US = FirstS + (U/Length)*DeltaS;
    // Calcul d'une section
    mySection->D0(US, W);
    myLocation->D0(U, W);
    List.Append(W);
  } 
}

//=======================================================================
//function : Build
//purpose  : Construct the Shell and the history
//=======================================================================
 Standard_Boolean BRepFill_PipeShell::Build() 
{
  Standard_Boolean Ok;
  Standard_Real FirstS, LastS;
  // 1) Preparation
  Prepare();

  if (myStatus != GeomFill_PipeOk) {
    BRep_Builder B;
    TopoDS_Shell Sh;
    B.MakeShell(Sh); 
    myShape = Sh; // Nullify
    return Standard_False; 
  }

  // 2) Calculate myFirst and myLast
  mySection->Law(1)->GetDomain(FirstS, LastS);
  mySection->D0(FirstS, myFirst);
  myLocation->D0(0, myFirst);
  if (mySection->IsVClosed() && myLocation->IsClosed()) {
      if (myLocation->IsG1(0)>=0) 
	myLast = myFirst;
      else {
	myFirst.Nullify();
	myLast.Nullify();
      }
  }
  else {
    Standard_Real Length;
    myLocation->CurvilinearBounds(myLocation->NbLaw(), 
				  FirstS, Length);
    mySection->D0(LastS,   myLast);
    myLocation->D0(Length, myLast);
    // eap 5 Jun 2002 occ332, myLast and myFirst must not share one TShape,
    // tolerances of shapes built on them may be quite different
    if (myFirst.IsPartner( myLast )) {
      BRepBuilderAPI_Copy copy(myLast);
      if (copy.IsDone()) 
        myLast = copy.Shape();
    }
    // eap 5 Jun 2002 occ332, end modif
  }
#if DRAW
  if (Affich) {
    DBRep::Set("PipeFirst", myFirst);
    DBRep::Set("PipeLast",  myLast);
  }
#endif

  // 3) Construction
  BRepFill_Sweep MkSw(mySection, myLocation, Standard_True);
  MkSw.SetTolerance(myTol3d, myBoundTol, 1.e-5, myTolAngular);
  MkSw.SetAngularControl(angmin, angmax);
  MkSw.SetForceApproxC1(myForceApproxC1);
  MkSw.SetBounds(TopoDS::Wire(myFirst), 
		 TopoDS::Wire(myLast));
  GeomAbs_Shape theContinuity = GeomAbs_C2;
  if (myTrihedron == GeomFill_IsDiscreteTrihedron)
    theContinuity = GeomAbs_C0;
  TopTools_MapOfShape Dummy;
  BRepFill_DataMapOfShapeHArray2OfShape Dummy2;
  BRepFill_DataMapOfShapeHArray2OfShape Dummy3;
  MkSw.Build(Dummy, Dummy2, Dummy3, myTransition, theContinuity,
             GeomFill_Location, myMaxDegree, myMaxSegments);

  myStatus = myLocation->GetStatus();
  Ok =  (MkSw.IsDone() && (myStatus == GeomFill_PipeOk));

  if (Ok) {
    myShape = MkSw.Shape();
    myErrorOnSurf = MkSw.ErrorOnSurface();

    TopoDS_Shape aBottomWire = myFirst;
    TopoDS_Shape aTopWire    = myLast;

    if(BuildBoundaries(MkSw, mySection, aBottomWire, aTopWire)) {
      myFirst = aBottomWire;
      myLast = aTopWire;
    }
    
    if (mySection->IsUClosed())
      {
	TopExp_Explorer explo;
	Standard_Boolean DegenFirst = Standard_True, DegenLast = Standard_True;

	for (explo.Init(myFirst, TopAbs_EDGE); explo.More(); explo.Next())
	  {
	    const TopoDS_Edge& anEdge = TopoDS::Edge(explo.Current());
	    DegenFirst = DegenFirst && BRep_Tool::Degenerated(anEdge);
	  }

	for (explo.Init(myLast, TopAbs_EDGE); explo.More(); explo.Next())
	  {
	    const TopoDS_Edge& anEdge = TopoDS::Edge(explo.Current());
	    DegenLast = DegenLast && BRep_Tool::Degenerated(anEdge);
	  }

	if (DegenFirst && DegenLast)
	  myShape.Closed(Standard_True);
      }

    BuildHistory(MkSw);
  }
  else {
    BRep_Builder B;
    TopoDS_Shell Sh;
    B.MakeShell(Sh); 
    myShape = Sh; // Nullify
    if (myStatus == GeomFill_PipeOk) myStatus = GeomFill_PipeNotOk;
  }
  return Ok;
}

//=======================================================================
//function : MakeSolid
//purpose  : 
//=======================================================================
 Standard_Boolean BRepFill_PipeShell::MakeSolid() 
{ 
  if (myShape.IsNull()) 
    throw StdFail_NotDone("PipeShell is not built");
  Standard_Boolean B = myShape.Closed();
  BRep_Builder BS;

  if (!B)
    {
      if(!myFirst.IsNull() && !myLast.IsNull()) {
	B = (myFirst.Closed() && myLast.Closed());
      }
      if (B) {
	// It is necessary to block the extremities 
	B =  PerformPlan(myFirst);
	if (B) {
	  B =  PerformPlan(myLast);
	  if (B) {
	    if (!myFirst.IsNull() && !IsSameOriented( myFirst, myShape ))
	      myFirst.Reverse();
	    if (!myLast.IsNull() && !IsSameOriented( myLast, myShape ))
	      myLast.Reverse();

	    if (!myFirst.IsNull())
	      BS.Add(myShape, TopoDS::Face(myFirst));
	    if (!myLast.IsNull())
	      BS.Add(myShape, TopoDS::Face(myLast));

	    myShape.Closed(Standard_True);
	  }
	}
      }
    }

  if (B) {
   TopoDS_Solid solid;
   BS.MakeSolid(solid);
   BS.Add(solid,TopoDS::Shell(myShape));
   BRepClass3d_SolidClassifier SC(solid);
   SC.PerformInfinitePoint(Precision::Confusion());
   if ( SC.State() == TopAbs_IN) {
     BS.MakeSolid(solid);
     myShape.Reverse();
     BS.Add(solid,TopoDS::Shell(myShape));
   }
   myShape = solid;   
   myShape.Closed(Standard_True);
 }
  return B;
}

//=======================================================================
//function : Shape
//purpose  : Return the result
//=======================================================================
const TopoDS_Shape& BRepFill_PipeShell::Shape() const
{
  return myShape;
}

//=======================================================================
//function : ErrorOnSurface
//purpose  : 
//=======================================================================

Standard_Real BRepFill_PipeShell::ErrorOnSurface() const 
{
  return myErrorOnSurf;
}

//=======================================================================
//function : FirstShape
//purpose  : Return the start section 
//=======================================================================
const TopoDS_Shape& BRepFill_PipeShell::FirstShape() const
{
  return myFirst;
}

//=======================================================================
//function : LastShape
//purpose  : Return the end section 
//=======================================================================
const TopoDS_Shape& BRepFill_PipeShell::LastShape() const
{
 return myLast;
}

//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================
void BRepFill_PipeShell::Generated(const TopoDS_Shape&   theShape,
				   TopTools_ListOfShape& theList) 
{
  theList.Clear();

  if(myGenMap.IsBound(theShape)) {
    theList = myGenMap.Find(theShape);
  }
}

//=======================================================================
//function : Prepare
//purpose  : - Check that everything is ready
//           - Construct the law of section
//           - Construct the law of location if required
//           - Calculate First & Last
//=======================================================================
 void BRepFill_PipeShell::Prepare() 
{
  WSeq.Clear();
  myEdgeNewEdges.Clear();
  
  TopoDS_Wire theSect;
  if (!IsReady()) throw StdFail_NotDone("PipeShell");
  if (!myLocation.IsNull() && !mySection.IsNull()) return; // It is ready
 
  //Check set of section for right configuration of punctual sections
  Standard_Integer i;
  TopoDS_Iterator iter;
  for (i = 2; i <= mySeq.Length()-1; i++)
    {
      Standard_Boolean wdeg = Standard_True;
      for (iter.Initialize(mySeq(i).Wire()); iter.More(); iter.Next())
	{
	  const TopoDS_Edge& anEdge = TopoDS::Edge(iter.Value());
	  wdeg = wdeg && (BRep_Tool::Degenerated(anEdge));
	}
      if (wdeg)
	throw Standard_Failure("Wrong usage of punctual sections");
    }
  if (mySeq.Length() <= 2)
    {
      Standard_Boolean wdeg = Standard_True;
      for (i = 1; i <= mySeq.Length(); i++)
	for (iter.Initialize(mySeq(i).Wire()); iter.More(); iter.Next())
	  {
	    const TopoDS_Edge& anEdge = TopoDS::Edge(iter.Value());
	    wdeg = wdeg && (BRep_Tool::Degenerated(anEdge));
	  }
      if (wdeg)
	throw Standard_Failure("Wrong usage of punctual sections");
    }

  // Construction of the law of location  
  if(myLocation.IsNull()) 
    {
      switch(myTrihedron)
	{
	case GeomFill_IsCorrectedFrenet :
	  {
	    Handle(GeomFill_TrihedronLaw) TLaw = 
	      new (GeomFill_CorrectedFrenet) ();
	    Handle(GeomFill_CurveAndTrihedron) Loc = 
	      new (GeomFill_CurveAndTrihedron) (TLaw);
	    myLocation = new (BRepFill_Edge3DLaw) (mySpine, Loc);
	    break;
	  } 
	  default :
	    { // Not planned!
	      throw Standard_ConstructionError("PipeShell");
	    }
	}
    }  
  
  //Transformation of the law (Transition Management)
  PerformTransition(myTransition, myLocation, angmin);

  
 // Construction of the section law
  if (mySeq.Length() == 1) {
    Standard_Real p1;
    gp_Trsf aTrsf;
    Place(mySeq(1), theSect, aTrsf, p1);
    TopoDS_Wire aLocalShape = theSect;
    if (mySeq(1).IsLaw())
      mySection = new BRepFill_ShapeLaw(aLocalShape, myLaw);
//      mySection = new (BRepFill_ShapeLaw) (TopoDS_Wire(theSect), myLaw);
    else 
      mySection = new BRepFill_ShapeLaw(aLocalShape);
// mySection = new (BRepFill_ShapeLaw) (TopoDS::Wire(theSect));
    
    WSeq.Append(theSect);
    //Simple case of single section
    myIndOfSec.Append(1);
    TopoDS_Iterator itw(theSect);
    for (; itw.More(); itw.Next())
    {
      const TopoDS_Shape& anEdge = itw.Value();
      TopTools_ListOfShape Elist;
      Elist.Append(anEdge);
      myEdgeNewEdges.Bind(anEdge, Elist);
    }
    ///////////////////////////////
  }   
  else 
  {
    TColStd_SequenceOfReal Param;
    TColStd_SequenceOfInteger IndSec;
    GeomFill_SequenceOfTrsf Transformations;
    Standard_Integer NbL = myLocation->NbLaw();
    gp_Trsf aTrsf;
    Standard_Real V1, V2, param;
    myLocation->CurvilinearBounds(NbL, V1, V2);
    V1 = 0.;
    Standard_Integer ideb = 0, ifin = 0;
    Standard_Integer iseq;
    for (iseq = 1; iseq <= mySeq.Length(); iseq++) {
      IndSec.Append(iseq);
      Place(mySeq(iseq), theSect, aTrsf, param);
      Param.Append(param);
      WSeq.Append(theSect);
      Transformations.Append(aTrsf);
      if (param==V1) ideb = iseq;
      if (param==V2) ifin = iseq;
    }
    
    
    // looping sections ?
    if (myLocation->IsClosed()) {
      if (ideb>0) {
        // place the initial section at the final position 
        Param.Append(V2);
        WSeq.Append(WSeq(ideb));
      }
      else if (ifin>0) {
        // place the final section at the initial position
        Param.Append(V1);
        WSeq.Append(WSeq(ifin));
      }
      else {
        // it is necessary to find a medium section to impose by V1 and by V2
        Standard_Real pmin = RealLast(), pmax = RealFirst();
        TopoDS_Wire Wmin, Wmax;
        for (iseq = 1; iseq <= WSeq.Length(); iseq++) {
          if (Param.Value(iseq)<pmin) {
            pmin = Param.Value(iseq);
            Wmin = TopoDS::Wire(WSeq.Value(iseq));
          }
          if (Param.Value(iseq)>pmax) {
            pmax = Param.Value(iseq);
            Wmax = TopoDS::Wire(WSeq.Value(iseq));
          }
        }
        // medium section between Wmin and Wmax
        TopoDS_Wire Wres;
        Standard_Real dmin = Abs(pmin-V1);
        Standard_Real dmax = Abs(pmax-V2);
        if (ComputeSection(Wmin,Wmax,dmin,dmax,Wres)) {
          // impose section Wres at the beginning and the end
          Param.Append(V1);
          WSeq.Append(Wres);
          IndSec.Append(WSeq.Length());
          Param.Append(V2);
          WSeq.Append(Wres);
          IndSec.Append(WSeq.Length());
        }
      }
    }
    
    // parse sections by increasing parameter
    Standard_Boolean play_again = Standard_True;
    while (play_again) {
      play_again = Standard_False;
      for (iseq=1;iseq<=WSeq.Length();iseq++) {
        for (Standard_Integer jseq=iseq+1;jseq<=WSeq.Length();jseq++) {
          if (Param.Value(iseq) > Param.Value(jseq)) {
            Param.Exchange(iseq,jseq);
            WSeq.Exchange(iseq,jseq);
            IndSec.Exchange(iseq,jseq);
            play_again = Standard_True;
          }
        }
      }
    }
    //Fill the array of real indices of sections
    for (Standard_Integer ii = 1; ii <= mySeq.Length(); ii++)
      for (Standard_Integer jj = 1; jj <= IndSec.Length(); jj++)
        if (IndSec(jj) == ii)
        {
          myIndOfSec.Append(jj);
          break;
        }
    
#ifdef DRAW
    if ( Affich) {
      char*  name = new char[100];
      Standard_Integer NBSECT = 0;
      for (Standard_Integer i=1;i<=WSeq.Length();i++) {
        NBSECT++;
        sprintf(name,"WSeq_%d",NBSECT);
        DBRep::Set(name,TopoDS::Wire(WSeq.Value(i)));
      }
    }
#endif
    
    
    
    //  Calculate work sections 
    TopTools_SequenceOfShape WorkingSections;
    WorkingSections.Clear();
    TopTools_DataMapOfShapeListOfShape WorkingMap;
    BRepFill_CompatibleWires Georges(WSeq);
    Georges.SetPercent(0.1);
    Georges.Perform(Standard_False);
    if (Georges.IsDone()) {
      WorkingSections = Georges.Shape();
      WorkingMap = Georges.Generated();
      //For each sub-edge of each section
      //we save its splits
      for (Standard_Integer ii = 1; ii <= WSeq.Length(); ii++)
      {
        TopExp_Explorer Explo(WSeq(ii), TopAbs_EDGE);
        for (; Explo.More(); Explo.Next())
        {
          const TopoDS_Edge& anEdge = TopoDS::Edge(Explo.Current());
          TopTools_ListOfShape aNewEdges = Georges.GeneratedShapes(anEdge);
          myEdgeNewEdges.Bind(anEdge, aNewEdges);
        }
      }
    }
    else {
      throw Standard_ConstructionError("PipeShell : uncompatible wires");
    }
    mySection = new (BRepFill_NSections) (WorkingSections,Transformations,Param,V1,V2);
    
  }// else

  //  modify the law of location if contact
  if ( (myTrihedron == GeomFill_IsGuidePlanWithContact)
      || (myTrihedron == GeomFill_IsGuideACWithContact) )  {
    Standard_Real fs, f, l, Delta, Length;
    Handle(GeomFill_LocationGuide) Loc;
    Handle(GeomFill_SectionLaw) Sec = mySection->ConcatenedLaw();
    myLocation->CurvilinearBounds(myLocation->NbLaw(), f, Length);
    Sec->GetDomain(fs,l);
    Delta = (l-fs)/Length;

    Standard_Real angle, old_angle = 0;
    for (Standard_Integer ipath=1; ipath<=myLocation->NbLaw(); ipath++) {
      myLocation->CurvilinearBounds(ipath, f, l);
      Loc = Handle(GeomFill_LocationGuide)::DownCast(myLocation->Law(ipath));
      Loc->Set(Sec, Standard_True, fs + f*Delta, fs + l*Delta,
	       old_angle, angle); // force the rotation	
      old_angle = angle;
    }      
  }

  myStatus = myLocation->GetStatus();
  if (!mySection->IsDone())
    myStatus = GeomFill_PipeNotOk;
}

//=======================================================================
//function : Place
//purpose  : Implement a Section in the local refernce frame
//           and return its parameter on the trajectory
//=======================================================================
void BRepFill_PipeShell::Place(const BRepFill_Section& Sec,
			       TopoDS_Wire& W,
                               gp_Trsf& aTrsf,
			       Standard_Real& param)
{
  BRepFill_SectionPlacement Place(myLocation, 
				  Sec.Wire(),
				  Sec.Vertex(),
				  Sec.WithContact(),
				  Sec.WithCorrection());
  TopoDS_Wire TmpWire =  Sec.Wire();
  aTrsf = Place.Transformation();
  //Transform the copy
  W = TopoDS::Wire(BRepBuilderAPI_Transform(TmpWire, aTrsf, Standard_True));
  ////////////////////////////////////
  param = Place.AbscissaOnPath();
}


//=======================================================================
//function : ResetLoc
//purpose  : Remove references to the sections in the laws of location
//=======================================================================
 void BRepFill_PipeShell::ResetLoc() 
{
  if ( (myTrihedron == GeomFill_IsGuidePlanWithContact)
      || (myTrihedron == GeomFill_IsGuideACWithContact) ) {
    Handle(GeomFill_LocationGuide) Loc;
    for (Standard_Integer isec=1; isec<=myLocation->NbLaw(); isec++) { 
      Loc = Handle(GeomFill_LocationGuide)::DownCast(myLocation->Law(isec));
      Loc->EraseRotation();// remove the rotation	
    }    
  }
}

//=======================================================================
//function : BuildHistory
//purpose  : Builds history for edges and vertices 
//           of sections
//=======================================================================
void BRepFill_PipeShell::BuildHistory(const BRepFill_Sweep& theSweep) 
{
  //Filling of <myGenMap>
  const Handle(TopTools_HArray2OfShape)& anUEdges = theSweep.InterFaces();
  BRep_Builder BB;
  
  TopTools_DataMapOfIntegerShape IndWireMap;
  
  Standard_Integer indw, inde;
  TopoDS_Iterator itw;
  for (indw = 1; indw <= mySeq.Length(); indw++)
  {
    const TopoDS_Shape& Section = mySeq(indw).OriginalShape();
    TopoDS_Wire aSection;
    Standard_Boolean IsPunctual = mySeq(indw).IsPunctual();
    if (IsPunctual)
    {
      //for punctual sections (first or last)
      //we take all the wires generated along the path
      
      TopTools_ListOfShape Elist;
      for (Standard_Integer i = 1; i <= anUEdges->UpperRow(); i++)
        for (Standard_Integer j = 1; j <= anUEdges->UpperCol(); j++)
          Elist.Append(anUEdges->Value(i,j));
      myGenMap.Bind(Section, Elist);
      continue;
    }
    else
      aSection = TopoDS::Wire(Section);
    //Take the real index of section on the path
    Standard_Integer IndOfW = myIndOfSec(indw);
    const TopoDS_Wire& theWire = TopoDS::Wire(WSeq(IndOfW));
    BRepTools_WireExplorer wexp_sec(aSection);
    for (inde = 1; wexp_sec.More(); wexp_sec.Next())
    {
      const TopoDS_Edge& anOriginalEdge = TopoDS::Edge(wexp_sec.Current());
      TopoDS_Edge anEdge = TopoDS::Edge(mySeq(indw).ModifiedShape(anOriginalEdge));
      if (BRep_Tool::Degenerated(anEdge))
        continue;

      TopoDS_Shell aShell;
      BB.MakeShell(aShell);
      TopoDS_Vertex aVertex [2];
      TopExp::Vertices(anOriginalEdge, aVertex[0], aVertex[1]);
      Standard_Integer SignOfAnEdge =
        (anOriginalEdge.Orientation() == TopAbs_FORWARD)? 1 : -1;
      
      //For each non-degenerated inde-th edge of <aSection>
      //we find inde-th edge in <theWire>
      TopoDS_Edge theEdge;
      BRepTools_WireExplorer wexp(theWire);
      for (Standard_Integer i = 1; wexp.More(); wexp.Next())
      {
        theEdge = TopoDS::Edge(wexp.Current());
        if (BRep_Tool::Degenerated(anEdge))
          continue;
        if (i == inde)
          break;
        i++;
      }

      //Take the list of splits for <theEdge>
      const TopTools_ListOfShape& NewEdges = myEdgeNewEdges(theEdge);
      Standard_Integer SignOfANewEdge = 0, SignOfIndex = 0;
      TopTools_ListIteratorOfListOfShape iter(NewEdges);
      for (; iter.More(); iter.Next())
      {
        const TopoDS_Edge& aNewEdge = TopoDS::Edge(iter.Value());
        SignOfANewEdge = (aNewEdge.Orientation() == TopAbs_FORWARD)? 1 : -1;
        Standard_Integer anIndE = mySection->IndexOfEdge(aNewEdge);
        SignOfIndex = (anIndE > 0)? 1 : -1;
        anIndE = Abs(anIndE);
        //For an edge generated shape is a "tape" -
        //a shell usually containing this edge and
        //passing from beginning of path to its end
        TopoDS_Shape aTape = theSweep.Tape(anIndE);
        TopoDS_Iterator itsh(aTape);
        for (; itsh.More(); itsh.Next())
          BB.Add(aShell, itsh.Value());
      }

      //Processing of vertices of <anEdge>
      //We should choose right index in <anUEdges>
      //for each vertex of edge 
      Standard_Integer ToReverse = SignOfAnEdge * SignOfANewEdge * SignOfIndex;
      Standard_Integer UIndex [2];
      UIndex[0] = Abs(mySection->IndexOfEdge(NewEdges.First()));
      UIndex[1] = Abs(mySection->IndexOfEdge(NewEdges.Last())) + ToReverse;
      if (ToReverse == -1)
      {
        UIndex[0]++;
        UIndex[1]++;
      }
      if (mySection->IsUClosed())
      {
        if (UIndex[0] > mySection->NbLaw())
          UIndex[0] = 1;
        if (UIndex[1] > mySection->NbLaw())
          UIndex[1] = 1;
      }
      //if (SignOfAnEdge * SignOfANewEdge == -1)
      if (SignOfAnEdge   == -1 ||
          SignOfANewEdge == -1)
      { Standard_Integer Tmp = UIndex[0]; UIndex[0] = UIndex[1]; UIndex[1] = Tmp; }

      TopTools_IndexedDataMapOfShapeListOfShape VEmap;
      TopExp::MapShapesAndAncestors(aShell, TopAbs_VERTEX, TopAbs_EDGE, VEmap);
      for (Standard_Integer kk = 0; kk < 2; kk++)
      {
        if (myGenMap.IsBound(aVertex[kk]))
          continue;
        if (IndWireMap.IsBound(UIndex[kk]))
        {
          TopTools_ListOfShape Wlist;
          Wlist.Append(IndWireMap(UIndex[kk]));
          myGenMap.Bind(aVertex[kk], Wlist);
          continue;
        }
        
        //Collect u-edges
        TopTools_SequenceOfShape SeqEdges;
        Standard_Integer jj;
        for (jj = 1; jj <= anUEdges->UpperCol(); jj++)
          SeqEdges.Append(anUEdges->Value(UIndex[kk], jj));

        //Assemble the wire ("rail" along the path)
        //checking for possible holes
        //(they appear with option "Round Corner")
        //and filling them
        //Missed edges are taken from <aShell>
        TopoDS_Wire aWire;
        BB.MakeWire(aWire);
        const TopoDS_Edge& FirstEdge = TopoDS::Edge(SeqEdges(1));
        if (FirstEdge.IsNull())
          continue;
        BB.Add(aWire, FirstEdge);
        TopoDS_Vertex FirstVertex, CurVertex;
        TopExp::Vertices(FirstEdge, FirstVertex, CurVertex);
        TopoDS_Edge CurEdge;
        for (jj = 2; jj <= SeqEdges.Length(); jj++)
        {
          CurEdge = TopoDS::Edge(SeqEdges(jj));
          TopoDS_Vertex Vfirst, Vlast;
          TopExp::Vertices(CurEdge, Vfirst, Vlast);
          if (CurVertex.IsSame(Vfirst))
            CurVertex = Vlast;
          else //a hole
          {
            const TopTools_ListOfShape& Elist = VEmap.FindFromKey(Vfirst);
            TopTools_ListIteratorOfListOfShape itl(Elist);
            for (; itl.More(); itl.Next())
            {
              const TopoDS_Edge& Candidate = TopoDS::Edge(itl.Value());
              if (Candidate.IsSame(CurEdge))
                continue;
              TopoDS_Vertex V1, V2;
              TopExp::Vertices(Candidate, V1, V2);
              if (V1.IsSame(CurVertex) || V2.IsSame(CurVertex))
              {
                BB.Add(aWire, Candidate);
                break;
              }
            }
          }
          CurVertex = Vlast;
          BB.Add(aWire, CurEdge);
        } //for (jj = 2; jj <= SeqEdges.Length(); jj++)
        //case of closed wire
        if (mySection->IsVClosed() &&
            !CurVertex.IsSame(FirstVertex))
        {
          const TopTools_ListOfShape& Elist = VEmap.FindFromKey(CurVertex);
          TopTools_ListIteratorOfListOfShape itl(Elist);
          for (; itl.More(); itl.Next())
          {
            const TopoDS_Edge& Candidate = TopoDS::Edge(itl.Value());
            if (Candidate.IsSame(CurEdge))
              continue;
            TopoDS_Vertex V1, V2;
            TopExp::Vertices(Candidate, V1, V2);
            if (V1.IsSame(FirstVertex) || V2.IsSame(FirstVertex))
            {
              BB.Add(aWire, Candidate);
              break;
            }
          }
        }
        TopTools_ListOfShape Wlist;
        Wlist.Append(aWire);
        myGenMap.Bind(aVertex[kk], Wlist);
        //Save already built wire with its index
        IndWireMap.Bind(UIndex[kk], aWire);
      } //for (Standard_Integer kk = 0; kk < 2; kk++)
      ////////////////////////////////////
      
      TopTools_ListOfShape ListShell;
      ListShell.Append(aShell);
      myGenMap.Bind(anOriginalEdge, ListShell);
      ////////////////////////

      inde++;
    }
  }
}


// ---------------------------------------------------------------------------------
// static function: BuildBoundaries
// purpose:
// ---------------------------------------------------------------------------------
Standard_Boolean BuildBoundaries(const BRepFill_Sweep&               theSweep,
				 const Handle(BRepFill_SectionLaw)&  theSection,
				 TopoDS_Shape&                       theBottom,
				 TopoDS_Shape&                       theTop) {
  
  TopoDS_Wire aBottomWire;
  TopoDS_Wire aTopWire;
  BRep_Builder aB;
  aB.MakeWire(aBottomWire);
  aB.MakeWire(aTopWire);
  Standard_Boolean bfoundbottom = Standard_False;
  Standard_Boolean bfoundtop = Standard_False;
  Handle(TopTools_HArray2OfShape) aVEdges = theSweep.Sections();
  Standard_Integer i = 0;
  Standard_Boolean bAllSame = Standard_True;

  for(i = 1; i <= theSection->NbLaw(); i++) {
    const TopoDS_Shape& aBottomEdge = aVEdges->Value(i, aVEdges->LowerCol());

    if(!aBottomEdge.IsNull() && (aBottomEdge.ShapeType() == TopAbs_EDGE)) {
      aB.Add(aBottomWire, aBottomEdge);
      bfoundbottom = Standard_True;
    }
    const TopoDS_Shape& aTopEdge = aVEdges->Value(i, aVEdges->UpperCol());

    if(!aTopEdge.IsNull() && (aTopEdge.ShapeType() == TopAbs_EDGE)) {
      aB.Add(aTopWire, aTopEdge);
      bfoundtop = Standard_True;
    }

    if(!aBottomEdge.IsNull() && !aTopEdge.IsNull() && !aBottomEdge.IsSame(aTopEdge))
      bAllSame = Standard_False;
  }

  if(theSection->IsUClosed()) {
    aBottomWire.Closed(Standard_True);
    aTopWire.Closed(Standard_True);
  }

  if(bfoundbottom) {
    theBottom = aBottomWire;
  }

  if(bfoundtop) {
    theTop  = aTopWire;
  }

  if(bAllSame && bfoundbottom && bfoundtop)
    theTop = theBottom;

  return bfoundbottom || bfoundtop;
}
