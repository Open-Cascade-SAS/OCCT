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

//:r5 abv 06.04.99: ec_turbine-A.stp, #4313: protect against null curve
//    abv 09.04.99  S4136: add parameter preci (to eliminate BRepAPI::Precision)

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Tool.hxx>
#include <ElCLib.hxx>
#include <Extrema_ExtPC.hxx>
#include <Geom2d_BoundedCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeFix_EdgeProjAux.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <TopExp.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <BSplCLib.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeFix_EdgeProjAux,Standard_Transient)

//=======================================================================
//function : ShapeFix_EdgeProjAux
//purpose  : 
//=======================================================================
ShapeFix_EdgeProjAux::ShapeFix_EdgeProjAux ()
{
  myFirstDone = myLastDone = Standard_False;
}

//=======================================================================
//function : ShapeFix_EdgeProjAux
//purpose  : 
//=======================================================================

ShapeFix_EdgeProjAux::ShapeFix_EdgeProjAux (const TopoDS_Face& F,
                                            const TopoDS_Edge& E)
{
  Init (F, E);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ShapeFix_EdgeProjAux::Init (const TopoDS_Face& F,
                                 const TopoDS_Edge& E)
{
  myFace = F;
  myEdge = E;
  myFirstParam = myLastParam = 0.;
  myFirstDone = myLastDone = Standard_False;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void ShapeFix_EdgeProjAux::Compute (const Standard_Real preci) 
{
  myFirstDone = myLastDone = Standard_False;
    
  // Project Point3d on Surface
  // TEMPORARY Call ShapeFix_EdgeProjAux
  myFirstParam = 0.;
  myLastParam = 0.;
  Init2d(preci);
  if (IsFirstDone() && IsLastDone()) {
    Standard_Real U1 = FirstParam();
    Standard_Real U2 = LastParam();
    if (U1>=U2) {
#ifdef OCCT_DEBUG
      cout << "Parametres inverses ... " << endl;
#endif
      Standard_Real tmp = U1;
      U1 = U2; U2 = tmp;
    }
    myFirstParam = U1;
    myFirstDone  = Standard_True;
    myLastParam  = U2;
    myLastDone   = Standard_True;
  }
}

//=======================================================================
//function : IsFirstDone
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix_EdgeProjAux::IsFirstDone() const
{
  return myFirstDone;
}

//=======================================================================
//function : IsLastDone
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix_EdgeProjAux::IsLastDone() const
{
  return myLastDone;
}

//=======================================================================
//function : FirstParam
//purpose  : 
//=======================================================================

Standard_Real ShapeFix_EdgeProjAux::FirstParam() const
{
  return myFirstParam;
}

//=======================================================================
//function : LastParam
//purpose  : 
//=======================================================================

Standard_Real ShapeFix_EdgeProjAux::LastParam() const
{
  return myLastParam;
}

//=======================================================================
//function : IsIso
//purpose  : 
//=======================================================================

Standard_Boolean ShapeFix_EdgeProjAux::IsIso (const Handle(Geom2d_Curve)& /*theCurve2d*/)
{
  // Until an ISO is recognized by Adaptor3d_Curve
/*  
  if (theCurve2d->IsKind(STANDARD_TYPE(Geom2d_Line))) {
    Handle(Geom2d_Line) theLine2d = Handle(Geom2d_Line)::DownCast(theCurve2d);
    gp_Dir2d theDir2d = theLine2d->Direction();
    
    gp_Dir2d dir1(0.,1.);
    gp_Dir2d dir2(0.,-1.);
    
    return (theDir2d.IsEqual(dir1,Precision::Angular()) ||
	    theDir2d.IsEqual(dir2,Precision::Angular()) ||
	    theDir2d.IsNormal(dir1,Precision::Angular()) ||
	    theDir2d.IsNormal(dir2,Precision::Angular()) );
  }
*/
  return Standard_False;
}

// ----------------------------------------------------------------------------
// static  : FindParameterWithExt
// Purpose : Computes the trimming parameter of Pt1 on COnS
// ----------------------------------------------------------------------------

static Standard_Boolean FindParameterWithExt (const gp_Pnt& Pt1, 
					      const Adaptor3d_CurveOnSurface& COnS,
					      const Standard_Real Uinf,
					      const Standard_Real Usup, 
					      const Standard_Real preci,
					      Standard_Real& w1)
{
  try {  // et allez donc !
    OCC_CATCH_SIGNALS
    Extrema_ExtPC myExtPC (Pt1, COnS, Uinf, Usup, preci);

    //ShapeFix_ExtPCOnS myExtPCOnS1 = 
    //ShapeFix_ExtPCOnS(Pt1, COnS, Uinf, Usup, preci);
  
    if (myExtPC.IsDone()) {
      Standard_Integer NbExt1 = myExtPC.NbExt();
      for (Standard_Integer i=1; i<=NbExt1; i++) {
	if (myExtPC.IsMin(i)) {
	  //Standard_Real dist = myExtPC.Value(i); //szv#4:S4163:12Mar99 debug mode only
	  w1   = myExtPC.Point(i).Parameter();
	}
      }
      return Standard_True;
    }
    else return Standard_False;
  }  // end try
  catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
//:s5
    cout << "Warning: ShapeFix_EdgeProjAux, FindParameterWithExt(): Exception: ";
    anException.Print(cout); cout << endl;
#endif
    (void)anException;
    return Standard_False;
  }
}

//=======================================================================
//function : Init2d
//purpose  : 
//=======================================================================

void ShapeFix_EdgeProjAux::Init2d (const Standard_Real preci) 
{
  Standard_Real cl = 0., cf = 0.;
    // Extract Geometries
  myFirstDone = myLastDone = Standard_False;
  Handle(Geom_Surface) theSurface = BRep_Tool::Surface(myFace);
  Handle(Geom2d_Curve) theCurve2d = BRep_Tool::CurveOnSurface(myEdge, myFace, cf, cl);
  if ( theCurve2d.IsNull() ) return; //:r5 abv 6 Apr 99:  ec_turbine-A.stp, #4313
  myFirstParam = 0.;
  myLastParam  = 0.;
  TopoDS_Vertex V1,V2;
  TopExp::Vertices(myEdge, V1, V2);
  gp_Pnt Pt1,Pt2;
  // pdn 28.12.98: r_39-db.stp #605: use ends of 3d curve instead of vertices 
  ShapeAnalysis_Edge sae;
  Standard_Real a,b;
  Handle(Geom_Curve) C3d;
  if(sae.Curve3d(myEdge,C3d,a,b,Standard_False)) {
    Pt1 = C3d->Value(a);
    Pt2 = C3d->Value(b);
  } 
  else {
    Pt1 = BRep_Tool::Pnt(V1);
    Pt2 = BRep_Tool::Pnt(V2);
  }
  //:S4136  Standard_Real preci = BRepAPI::Precision();
  //pdn to manage degenerated case
  if (V1.IsSame(V2)) {
    Handle(ShapeAnalysis_Surface) stsu = new ShapeAnalysis_Surface (theSurface);
    gp_Pnt2d aPt1,aPt2;
    Standard_Real firstpar,lastpar;
    if (stsu->DegeneratedValues(Pt1,preci,aPt1,aPt2,firstpar,lastpar)){

      if(theCurve2d->IsKind(STANDARD_TYPE(Geom2d_Line))) {
        if (aPt1.IsEqual(theCurve2d->Value(firstpar),preci) && 
          aPt2.IsEqual(theCurve2d->Value(lastpar),preci)){
            myFirstParam = firstpar;
            myLastParam  = lastpar;
            myFirstDone = myLastDone = Standard_True;
            return;
        }
      } 
#ifdef OCCT_DEBUG
      else cout <<"Other type of deg curve"<<endl;
#endif

    }
  }

  Standard_Boolean parU = Standard_False, parV = Standard_False;
  GeomAdaptor_Surface          SA     = GeomAdaptor_Surface(theSurface);
  Handle(GeomAdaptor_HSurface) myHSur = new GeomAdaptor_HSurface(SA);

  cf = theCurve2d->FirstParameter();
  cl = theCurve2d->LastParameter();
  //pdn cutting pcurve by suface bounds
  if (Precision::IsInfinite(cf)||Precision::IsInfinite(cl)) {
    if(theCurve2d->IsKind(STANDARD_TYPE(Geom2d_Line))) {
      Standard_Real uf,ul,vf,vl;
      theSurface->Bounds(uf,ul,vf,vl);
      //Correct surface limits for extrusion/revolution surfaces based on hyperbola
      //23 is ln(1.0e+10)
      if(SA.GetType() == GeomAbs_SurfaceOfExtrusion)
      {
        if(SA.BasisCurve()->GetType() == GeomAbs_Hyperbola)
        {
          uf = Max(uf, -23.);
          ul = Min(ul,  23.);
        }
      }
      if(SA.GetType() == GeomAbs_SurfaceOfRevolution)
      {
        if(SA.BasisCurve()->GetType() == GeomAbs_Hyperbola)
        {
          vf = Max(vf, -23.);
          vl = Min(vl,  23.);
        }
      }
      if(!Precision::IsInfinite(uf)&&!Precision::IsInfinite(ul)&&
         !Precision::IsInfinite(vf)&&!Precision::IsInfinite(vl)) {
          Standard_Real cfi,cli;
          Handle(Geom2d_Line) lin = Handle(Geom2d_Line)::DownCast(theCurve2d);
          gp_Pnt2d pnt = lin->Location();
          gp_Dir2d dir = lin->Direction();
          if (dir.Y()==0) {
            parU = Standard_True;
            cfi = (uf-pnt.X())/dir.X();
            cli = (ul-pnt.X())/dir.X();
          } 
          else if (dir.X()==0) {
            parV = Standard_True;
            cfi = (vf-pnt.Y())/dir.Y();
            cli = (vl-pnt.Y())/dir.Y();
          } 
          else {//common case
            Standard_Real xfi, xli, yfi, yli;
            xfi = (uf-pnt.X())/dir.X();
            xli = (ul-pnt.X())/dir.X();
            yfi = (vf-pnt.Y())/dir.Y();
            yli = (vl-pnt.Y())/dir.Y();
            if (dir.X()*dir.Y() > 0) {
              cfi = (Abs(xli-xfi) < Abs(xli-yfi)? xfi : yfi);
              cli = (Abs(xfi-xli) < Abs(xfi-yli)? xli : yli);
            } else {
              cfi = (Abs(xli-xfi) < Abs(xli-yli)? xfi : yli);
              cli = (Abs(yli-xli) < Abs(yli-yfi)? xli : yfi);
            }
          }
          if (cfi < cli) { cf = cfi; cl = cli; }
          else { cf = cli; cl = cfi; }
      } 
      else if(!Precision::IsInfinite(uf)&&!Precision::IsInfinite(ul)){
        Handle(Geom2d_Line) lin = Handle(Geom2d_Line)::DownCast(theCurve2d);
        gp_Dir2d dir = lin->Direction();
        if (dir.X()!=0) {
          if (dir.Y()==0) parU = Standard_True;
          gp_Pnt2d pnt = lin->Location(); //szv#4:S4163:12Mar99 moved
          Standard_Real cfi = (uf-pnt.X())/dir.X();
          Standard_Real cli = (ul-pnt.X())/dir.X();
          if (cfi < cli) { cf = cfi; cl = cli; }
          else { cf = cli; cl = cfi; }
        }
        else {
          cf=-10000;
          cl= 10000;
        }
      }
      else {
        cf=-10000;
        cl= 10000;
        //pdn not cutted by bounds
#ifdef OCCT_DEBUG
        cout<<"Infinite Surface"<<endl;
#endif	
      }
    }
    else {
      //pdn not linear case not managed
      cf=-10000;
      cl= 10000;
      if(theCurve2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
      {
        //Try to reparametrize (CASE dxf read bug25899)
        Handle(Geom2d_BSplineCurve) aBspl = Handle(Geom2d_BSplineCurve)::DownCast(theCurve2d->Copy());
        TColStd_Array1OfReal aNewKnots(1, aBspl->NbKnots());  
        aBspl->Knots(aNewKnots);
        BSplCLib::Reparametrize(cf, cl, aNewKnots);
        aBspl->SetKnots(aNewKnots);
        theCurve2d = aBspl;
      }

#ifdef OCCT_DEBUG
      cout<<"Some infinite curve"<<endl;
#endif 
    }
  }

  Geom2dAdaptor_Curve          CA     = Geom2dAdaptor_Curve(theCurve2d,cf,cl);
  Handle(Geom2dAdaptor_HCurve) myHCur = new Geom2dAdaptor_HCurve(CA);

  Adaptor3d_CurveOnSurface COnS = Adaptor3d_CurveOnSurface(myHCur, myHSur);

  // ----------------------------------------------
  // --- topological limit == geometric limit ? ---
  // ----------------------------------------------
  Standard_Real Uinf = COnS.FirstParameter();
  Standard_Real Usup = COnS.LastParameter();
 
  Standard_Real w1 = 0., w2 = 0.;
  ShapeAnalysis_Curve sac;
  gp_Pnt pnt;
  Standard_Real dist = sac.Project(COnS,Pt1,preci,pnt,w1,Standard_False);
  //if distance is infinite then projection is not performed
  if( Precision::IsInfinite(dist))
    return;
  
  myFirstDone = Standard_True;
  myFirstParam = w1;
 
  dist = sac.Project(COnS,Pt2,preci,pnt,w2,Standard_False);
  
  if( Precision::IsInfinite(dist))
    return;
  
  myLastDone = Standard_True;
  myLastParam  = w2;
    
  if(fabs(w1 - w2) < Precision::PConfusion())
  {
    if(!theSurface->IsUPeriodic() && !theSurface->IsVPeriodic())
      return;
  }
    
  if ( myFirstParam == Uinf && myLastParam == Usup ) return;
  if ( myFirstParam == Usup && myLastParam == Uinf ) {
    myFirstParam = theCurve2d->ReversedParameter(Usup);
    myLastParam  = theCurve2d->ReversedParameter(Uinf);
    theCurve2d->Reverse();
#ifdef OCCT_DEBUG
    cout << "Warning: ShapeFix_EdgeProjAux: pcurve reversed" << endl;
#endif
    return;
  }
  //:abv 29.08.01: SAT: fix for closed case
  if ( COnS.Value(Uinf).Distance ( COnS.Value(Usup) ) < Precision::Confusion() ) {
    // 18.11.2002 SKL OCC630 compare values with tolerance Precision::PConfusion() instead of "=="
    if ( Abs(myFirstParam-Uinf) < ::Precision::PConfusion() &&
      Abs(myLastParam-Uinf) < ::Precision::PConfusion() )
      myLastParam = w2 = Usup;
    // 18.11.2002 SKL OCC630 compare values with tolerance Precision::PConfusion() instead of "=="
    else if ( Abs(myFirstParam-Usup) < ::Precision::PConfusion() && 
      Abs(myLastParam-Usup) < ::Precision::PConfusion() )
      myFirstParam = w1 = Uinf;
  }

  //pdn adjust parameters in periodic case
  if(parU || parV) {
    Standard_Real uf,ul,vf,vl;
    theSurface->Bounds(uf,ul,vf,vl);
    Standard_Real period = (parU ? ul-uf : vl-vf);
    w1+=ShapeAnalysis::AdjustToPeriod(w1,0,period);
    myFirstParam = w1;
    w2+=ShapeAnalysis::AdjustToPeriod(w2,0,period);
    myLastParam = w2;
    Handle(Geom_Curve) C3d1;
    if(!sae.Curve3d (myEdge, C3d1, cf, cl, Standard_False )) {
      UpdateParam2d(theCurve2d);
      return;
    }
    gp_Pnt mid = C3d1->Value((cf+cl)/2);
    Standard_Real wmid;
    sac.Project(COnS,mid,preci,pnt,wmid,Standard_False);
    wmid+=ShapeAnalysis::AdjustToPeriod(wmid,0,period);
    if(w1>=w2) {
      if(w2 > wmid) myFirstParam -= period;
      else if (w1 > wmid)
        UpdateParam2d(theCurve2d);
      else {
        myLastParam+=period;
#ifdef OCCT_DEBUG
        cout <<" Added"<<endl;
#endif	
      }
    }
    else {
      if(w1 > wmid) {
        myLastParam -=period;
        UpdateParam2d(theCurve2d);
#ifdef OCCT_DEBUG
        cout <<" Added & Inverted"<<endl;
#endif	
      } else if (w2 < wmid) {
        myFirstParam += period;
        UpdateParam2d(theCurve2d);
      }
    }
  }
  UpdateParam2d(theCurve2d);
  return;
}

//=======================================================================
//function : Init3d
//purpose  : 
//=======================================================================

void ShapeFix_EdgeProjAux::Init3d (const Standard_Real preci) 
{
  Standard_Real cl, cf;

  // Extract Geometries
  Handle(Geom_Surface) theSurface = BRep_Tool::Surface(myFace);
  Handle(Geom2d_Curve) theCurve2d = BRep_Tool::CurveOnSurface(myEdge, myFace, cf, cl);
  if ( theCurve2d.IsNull() ) return; //:r5 abv 6 Apr 99:  ec_turbine-A.stp, #4313
  TopoDS_Vertex V1,V2;

  V1 = TopExp::FirstVertex(myEdge);
  V2 = TopExp::LastVertex(myEdge);
  gp_Pnt Pt1 = BRep_Tool::Pnt(V1);
  gp_Pnt Pt2 = BRep_Tool::Pnt(V2);


  GeomAdaptor_Surface          SA     = GeomAdaptor_Surface(theSurface);  
  Handle(GeomAdaptor_HSurface) myHSur = new GeomAdaptor_HSurface(SA);

  Geom2dAdaptor_Curve          CA     = Geom2dAdaptor_Curve(theCurve2d);
  Handle(Geom2dAdaptor_HCurve) myHCur = new Geom2dAdaptor_HCurve(CA);

  Adaptor3d_CurveOnSurface COnS = Adaptor3d_CurveOnSurface(myHCur, myHSur);
  
//:S4136  Standard_Real preci = BRepAPI::Precision();
  Standard_Real Uinf = theCurve2d->FirstParameter();
  Standard_Real Usup = theCurve2d->LastParameter();

  // ----------------------------------------------
  // --- topological limit == geometric limit ? ---
  // ----------------------------------------------
  
  if (theCurve2d->IsKind(STANDARD_TYPE(Geom2d_BoundedCurve))) {
    
    gp_Pnt Pdeb = COnS.Value(Uinf);
    gp_Pnt Pfin = COnS.Value(Usup);
    
    //szv#4:S4163:12Mar99 optimized
    if ( Pdeb.IsEqual(Pt1, preci) && Pfin.IsEqual(Pt2, preci) ) {
      myFirstParam = Uinf;
      myLastParam  = Usup;
      myFirstDone = myLastDone = Standard_True;
      return;
    }
  }

  // ------------------------------------------
  // --- The CurveOnSurface is not infinite ---
  // ---          Try with Extrema          ---
  // ------------------------------------------

  Standard_Real w1 = COnS.FirstParameter();
  Standard_Real w2 = COnS.LastParameter();

  if ((!Precision::IsInfinite(w1) &&
       !Precision::IsInfinite(w2) &&
       theCurve2d->Continuity() != GeomAbs_C0) ||
      IsIso(theCurve2d))  {

    //szv#4:S4163:12Mar99 optimized
    if ( FindParameterWithExt(Pt1, COnS, Uinf, Usup, preci, w1) &&
	 FindParameterWithExt(Pt2, COnS, Uinf, Usup, preci, w2) ) {
      myFirstParam = w1;
      myLastParam  = w2;
      UpdateParam2d(theCurve2d);
      myFirstDone = myLastDone = Standard_True;
      return;
    }
  }
  myFirstDone = myLastDone = Standard_True;
}

//=======================================================================
//function : UpdateParam2d
//purpose  : 
//=======================================================================

void ShapeFix_EdgeProjAux::UpdateParam2d (const Handle(Geom2d_Curve)& theCurve2d)
{
  if (myFirstParam < myLastParam)  return;

  Standard_Real cf = theCurve2d->FirstParameter();
  Standard_Real cl = theCurve2d->LastParameter();
//:S4136  Standard_Real preci = BRepAPI::Precision();
  Standard_Real preci2d = Precision::PConfusion(); //:S4136: Parametric(preci, 0.01);

  // 15.11.2002 PTV OCC966
  if (ShapeAnalysis_Curve::IsPeriodic(theCurve2d)) {
    ElCLib::AdjustPeriodic(cf,cl,preci2d,myFirstParam,myLastParam);
  }
  else if (theCurve2d->IsClosed()) {
    //szv#4:S4163:12Mar99 optimized
    if      ( Abs ( myFirstParam - cl ) <= preci2d ) myFirstParam = cf;
    else if ( Abs ( myLastParam  - cf ) <= preci2d ) myLastParam  = cl;
    else {
#ifdef OCCT_DEBUG
      cout << "Error : curve 2d range crossing non periodic curve origin";
      cout <<  endl;
#endif
      // add fail result;
      return;
    }
  }
  // the curve is closed within the 'file' 2D tolerance 
  else if (theCurve2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))) {
    Handle(Geom2d_BSplineCurve) aBSpline2d =
      Handle(Geom2d_BSplineCurve)::DownCast(theCurve2d);
    if (aBSpline2d->StartPoint().Distance(aBSpline2d->EndPoint()) <= preci2d) {
      if      ( Abs ( myFirstParam - cl ) <= preci2d ) myFirstParam = cf;
      else if ( Abs ( myLastParam  - cf ) <= preci2d ) myLastParam  = cl;
    }
  }
  else {
#ifdef OCCT_DEBUG
    cout << "Warning : non increasing parameters for 2d curve." << endl;
    cout << "          update parameter 2d uncertain." << endl;
#endif
    Standard_Real tmp1 = myFirstParam, tmp2 = myLastParam;
    myFirstParam = theCurve2d->ReversedParameter(tmp1);
    myLastParam = theCurve2d->ReversedParameter(tmp2);
    theCurve2d->Reverse();
    //cout<<"Reversed case 2"<<endl;
  }
}
