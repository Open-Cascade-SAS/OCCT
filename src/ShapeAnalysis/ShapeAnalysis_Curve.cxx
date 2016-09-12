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

// pdn 04.12.98 Add method using Adaptor_Curve
//:j8 abv 10.12.98 TR10 r0501_db.stp #9423
//pdn 25.12.98 private method ProjectAct
//szv#4 S4163
//:s5 abv 22.04.99 Adding debug printouts in catch {} blocks
//    abv 14.05.99 S4174: Adding method for exact computing of the boundary box 
//    gka 21.06.99 S4208: adding method NextProject(Adaptor_Curve)
//    msv 30.05.00 correct IsPlanar for a conic curve

#include <Adaptor3d_Curve.hxx>
#include <Bnd_Box2d.hxx>
#include <ElCLib.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BoundedCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_Geom2dCurveTool.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BoundedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeExtend_ComplexCurve.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TColgp_SequenceOfPnt.hxx>

//=======================================================================
//function : ProjectOnSegments
//purpose  : 
//=======================================================================
static void ProjectOnSegments (const Adaptor3d_Curve& AC, const gp_Pnt& P3D,
			       const Standard_Integer nbseg,
			       Standard_Real& uMin, Standard_Real& uMax,
			       Standard_Real& distmin, gp_Pnt& proj, Standard_Real& param)
{
  //  On considere <nbseg> points sur [uMin,uMax]
  //  Quel est le plus proche. Et quel est le nouvel intervalle
  //  (il ne peut pas deborder l ancien)
  Standard_Real u, dist2, delta = (nbseg == 0)? 0 : (uMax-uMin)/nbseg; //szv#4:S4163:12Mar99 anti-exception
  Standard_Real  distmin2 = distmin * distmin;
  Standard_Boolean aHasChanged = Standard_False;
  for (Standard_Integer i = 0; i <= nbseg; i ++) {
    u = uMin + (delta * i);
    gp_Pnt PU = AC.Value (u);
    dist2 = PU.SquareDistance (P3D);
    if (dist2 < distmin2)  {  distmin2 = dist2;  proj = PU;  param = u; aHasChanged = Standard_True;  }
  }
  if (aHasChanged)
    distmin = Sqrt (distmin2);
#ifdef OCCT_DEBUG
  cout<<"ShapeAnalysis_Geom:Project, param="<<param<<" -> distmin="<<distmin<<endl;
#endif

  uMax = Min (uMax, param+delta);
  uMin = Max (uMin, param-delta);
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

Standard_Real ShapeAnalysis_Curve::Project(const Handle(Geom_Curve)& C3D,
					   const gp_Pnt& P3D,
					   const Standard_Real preci,
					   gp_Pnt& proj,
					   Standard_Real& param,
					   const Standard_Boolean AdjustToEnds) const
{
  Standard_Real uMin = C3D->FirstParameter();
  Standard_Real uMax = C3D->LastParameter();
  if (uMin < uMax)  return Project (C3D,P3D,preci,proj,param,uMin,uMax,AdjustToEnds);
  else              return Project (C3D,P3D,preci,proj,param,uMax,uMin,AdjustToEnds);
}

//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

Standard_Real ShapeAnalysis_Curve::Project(const Handle(Geom_Curve)& C3D,
					   const gp_Pnt& P3D,
					   const Standard_Real preci,
					   gp_Pnt& proj,
					   Standard_Real& param,
					   const Standard_Real cf,
					   const Standard_Real cl,
					   const Standard_Boolean AdjustToEnds) const
{
  Standard_Real distmin;
  Standard_Real uMin = (cf < cl ? cf : cl);
  Standard_Real uMax = (cf < cl ? cl : cf);
  
  GeomAdaptor_Curve GAC(C3D, uMin, uMax);
  if (C3D->IsKind(STANDARD_TYPE(Geom_BoundedCurve))) {
    Standard_Real prec = ( AdjustToEnds ? preci : Precision::Confusion() ); //:j8 abv 10 Dec 98: tr10_r0501_db.stp #9423: protection against densing of points near one end
    gp_Pnt LowBound = GAC.Value(uMin);
    gp_Pnt HigBound = GAC.Value(uMax);
    distmin = LowBound.Distance(P3D);
    if (distmin <= prec) {
      param = uMin;
      proj  = LowBound;
      return distmin;
    } 
    distmin = HigBound.Distance(P3D);
    if (distmin <= prec) {
      param = uMax;
      proj  = HigBound;
      return distmin;
    }
  }

  if (!C3D->IsClosed()) {
    //modified by rln on 16/12/97 after CSR# PRO11641 entity 20767
    //the VIso was not closed (according to C3D->IsClosed()) while it "almost"
    //was (the distance between ends of the curve was a little bit more than
    //Precision::Confusion())
    //in that case value 0.1 was too much and this method returned not correct parameter
    //uMin = uMin - 0.1;
    //uMax = uMax + 0.1;
    // modified by pdn on 01.07.98 after BUC60195 entity 1952 (Min() added)
    Standard_Real delta = Min (GAC.Resolution (preci), (uMax - uMin) * 0.1);
    uMin -= delta;
    uMax += delta;
    GAC.Load(C3D, uMin, uMax);
  }

  return ProjectAct(GAC, P3D, preci, proj, param);
}

//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

Standard_Real ShapeAnalysis_Curve::Project(const Adaptor3d_Curve& C3D,
					   const gp_Pnt& P3D,
					   const Standard_Real preci,
					   gp_Pnt& proj,
					   Standard_Real& param,
					   const Standard_Boolean AdjustToEnds) const

{

  Standard_Real uMin = C3D.FirstParameter();
  Standard_Real uMax = C3D.LastParameter();
  
  if (Precision::IsInfinite(uMin) && Precision::IsInfinite(uMax))
    return ProjectAct(C3D, P3D, preci, proj, param);

  Standard_Real distmin_L = Precision::Infinite(), distmin_H = Precision::Infinite();
  Standard_Real prec = ( AdjustToEnds ? preci : Precision::Confusion() ); //:j8 abv 10 Dec 98: tr10_r0501_db.stp #9423: protection against densing of points near one end
  gp_Pnt LowBound = C3D.Value(uMin);
  gp_Pnt HigBound = C3D.Value(uMax);
  distmin_L = LowBound.Distance(P3D);
  distmin_H = HigBound.Distance(P3D);

  if (distmin_L <= prec) {
    param = uMin;
    proj  = LowBound;
    return distmin_L;
  }

  if (distmin_H <= prec) {
    param = uMax;
    proj  = HigBound;
    return distmin_H;
  } 

  Standard_Real distProj = ProjectAct(C3D, P3D, preci, proj, param);
  if(  distProj < distmin_L +  Precision::Confusion() && distProj < distmin_H +  Precision::Confusion())
    return distProj;

  if( distmin_L < distmin_H)
  {
    param = uMin;
    proj  = LowBound;
    return distmin_L;
  }
  param = uMax;
  proj  = HigBound;
  return distmin_H;

}

//=======================================================================
//function : ProjectAct
//purpose  : 
//=======================================================================

Standard_Real ShapeAnalysis_Curve::ProjectAct(const Adaptor3d_Curve& C3D,
					      const gp_Pnt& P3D,
					      const Standard_Real preci,
					      gp_Pnt& proj,
					      Standard_Real& param) const
       
{
  Standard_Boolean OK = Standard_False;
  param = 0.;
  try {
    OCC_CATCH_SIGNALS
    Extrema_ExtPC myExtPC(P3D,C3D);
    Standard_Real dist2Min = RealLast() , dist2;
    Standard_Integer index = 0;
    if ( myExtPC.IsDone() && ( myExtPC.NbExt() > 0) )
    {
      for ( Standard_Integer i = 1; i <= myExtPC.NbExt(); i++)
      {
        if (!myExtPC.IsMin(i))
          continue;

        dist2 = myExtPC.SquareDistance(i);
        if ( dist2 < dist2Min)
        {
          dist2Min = dist2; index = i;
        }
      }

      if (index != 0)
      {
        param = (myExtPC.Point(index)).Parameter();
        proj  = (myExtPC.Point(index)).Value();
        OK = Standard_True;
      }
    }
  }
  catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
 //:s5
    cout << "\nWarning: ShapeAnalysis_Curve::ProjectAct(): Exception in Extrema_ExtPC: "; 
    anException.Print(cout); cout << endl;
#endif
    (void)anException;
    OK = Standard_False;
  }
  
  //szv#4:S4163:12Mar99 moved
  Standard_Real uMin = C3D.FirstParameter(), uMax = C3D.LastParameter();
  Standard_Boolean closed = Standard_False;  // si on franchit les bornes ...
  Standard_Real distmin = Precision::Infinite(), valclosed = 0.;
  Standard_Real aModParam = param;
  Standard_Real aModMin = distmin;
  
  // PTV 29.05.2002 remember the old solution, cause it could be better
  Standard_Real anOldParam =0.;
  Standard_Boolean IsHaveOldSol = Standard_False;
  gp_Pnt anOldProj;
  if (OK) {
    IsHaveOldSol = Standard_True;
    anOldProj = proj;
    anOldParam = param;
    distmin = proj.Distance (P3D);
    aModMin = distmin;
    if (distmin > preci) OK = Standard_False;
    // Cas TrimmedCurve a cheval. Voir la courbe de base.
    // Si fermee, passer une periode
    if (C3D.IsClosed()) {
      closed = Standard_True;
      valclosed = uMax - uMin; //szv#4:S4163:12Mar99 optimized
    }
  }

  if (!OK) {
    // BUG NICOLAS - Si le point est sur la courbe 0 Solutions
    // Cela fonctionne avec ElCLib

    // D une facon generale, on essaie de TOUJOURS retourner un resultat
    //  MEME PAS BIEN BON. L appelant pourra decider alors quoi faire
    param = 0.;

    switch(C3D.GetType()) {
    case GeomAbs_Circle:
      {
        const gp_Circ& aCirc = C3D.Circle();
        proj = aCirc.Position().Location();
        if(aCirc.Radius() <= gp::Resolution() ||
           P3D.SquareDistance(proj) <= gp::Resolution() ) {
          param = C3D.FirstParameter();
          proj = proj.XYZ() + aCirc.XAxis().Direction().XYZ() * aCirc.Radius();
        }
        else {
          param = ElCLib::Parameter(aCirc, P3D);
          proj  = ElCLib::Value(param, aCirc);
        }
	closed = Standard_True;
	valclosed = 2.*M_PI;
      }
      break;
    case GeomAbs_Hyperbola:
      {
	param = ElCLib::Parameter(C3D.Hyperbola(), P3D);
	proj  = ElCLib::Value(param, C3D.Hyperbola());
      }
      break;
    case GeomAbs_Parabola:
      {
	param = ElCLib::Parameter(C3D.Parabola(), P3D);
	proj  = ElCLib::Value(param, C3D.Parabola());
      }
      break;
    case GeomAbs_Line:
      {
	param = ElCLib::Parameter(C3D.Line(), P3D);
	proj  = ElCLib::Value(param, C3D.Line());
      }
      break;
    case GeomAbs_Ellipse:
      {
	param = ElCLib::Parameter(C3D.Ellipse(), P3D);
	proj  = ElCLib::Value(param, C3D.Ellipse());
	closed = Standard_True;
	valclosed = 2.*M_PI;

      }
      break;
    default:
      {
        //  on ne va quand meme pas se laisser abattre ... ???
        //  on tente ceci : 21 points sur la courbe, quel est le plus proche
	distmin = Precision::Infinite();
	ProjectOnSegments (C3D,P3D,25,uMin,uMax,distmin,proj,param);
	if (distmin <= preci) 
          return distmin;
        Extrema_LocateExtPC aProjector (P3D, C3D, param/*U0*/, uMin, uMax, preci/*TolU*/);
        if (aProjector.IsDone())
        {
          param = aProjector.Point().Parameter();
          proj = aProjector.Point().Value();
          Standard_Real aDistNewton = P3D.Distance(proj);
          if (aDistNewton < aModMin)
            return aDistNewton;
        }
        // cout <<"newton failed"<<endl;    
	ProjectOnSegments (C3D,P3D,40,uMin,uMax,distmin,proj,param);
	if (distmin <= preci) return distmin;
	ProjectOnSegments (C3D,P3D,20,uMin,uMax,distmin,proj,param);
	if (distmin <= preci) return distmin;
	ProjectOnSegments (C3D,P3D,25,uMin,uMax,distmin,proj,param);
	if (distmin <= preci) return distmin;
	ProjectOnSegments (C3D,P3D,40,uMin,uMax,distmin,proj,param);
	if (distmin <= preci) return distmin;
        //  soyons raisonnable ...
        if(distmin > aModMin) {
          distmin = aModMin;
          param = aModParam;
        }

	return distmin;
      }
    }
  }
  
  //  p = PPOC.LowerDistanceParameter();  cf try
  if ( closed && ( param < uMin || param > uMax ) ) 
    param += ShapeAnalysis::AdjustByPeriod ( param, 0.5 * ( uMin + uMax ), valclosed );
  
  if (IsHaveOldSol) {
    // PTV 29.05.2002 Compare old solution and new;
    Standard_Real adist1, adist2;
    adist1 = anOldProj.SquareDistance(P3D);
    adist2 = proj.SquareDistance (P3D);
    if (adist1 < adist2) {
      proj = anOldProj;
      param = anOldParam;
    }
  }
  return proj.Distance (P3D);
}


//=======================================================================
//function : NextProject
//purpose  : Newton algo for projecting point on curve (S4030)
//=======================================================================

Standard_Real ShapeAnalysis_Curve::NextProject(const Standard_Real paramPrev,
					       const Handle(Geom_Curve)& C3D,
					       const gp_Pnt& P3D,
					       const Standard_Real preci,
					       gp_Pnt& proj,
					       Standard_Real& param,
					       const Standard_Real cf,
					       const Standard_Real cl,
					       const Standard_Boolean AdjustToEnds) const
{
  Standard_Real uMin = (cf < cl ? cf : cl);
  Standard_Real uMax = (cf < cl ? cl : cf);
  Standard_Real distmin = Precision::Infinite();
  GeomAdaptor_Curve GAC(C3D, uMin, uMax);
  if (C3D->IsKind(STANDARD_TYPE(Geom_BoundedCurve))) {
    Standard_Real prec = ( AdjustToEnds ? preci : Precision::Confusion() ); //:j8 abv 10 Dec 98: tr10_r0501_db.stp #9423: protection against densing of points near one end
    gp_Pnt LowBound = GAC.Value(uMin);
    gp_Pnt HigBound = GAC.Value(uMax);
    distmin = LowBound.Distance(P3D);
    if (distmin <= prec) {
      param = uMin;
      proj  = LowBound;
      return distmin;
    } 
    distmin = HigBound.Distance(P3D);
    if (distmin <= prec) {
      param = uMax;
      proj  = HigBound;
      return distmin;
    }
  }

  if (!C3D->IsClosed()) {
    //modified by rln on 16/12/97 after CSR# PRO11641 entity 20767
    //the VIso was not closed (according to C3D->IsClosed()) while it "almost"
    //was (the distance between ends of the curve was a little bit more than
    //Precision::Confusion())
    //in that case value 0.1 was too much and this method returned not correct parameter
    //uMin = uMin - 0.1;
    //uMax = uMax + 0.1;
    // modified by pdn on 01.07.98 after BUC60195 entity 1952 (Min() added)
    Standard_Real delta = Min (GAC.Resolution (preci), (uMax - uMin) * 0.1);
    uMin -= delta;
    uMax += delta;
    GAC.Load(C3D, uMin, uMax);
  }
  return NextProject ( paramPrev, GAC, P3D, preci, proj, param );
}

//=======================================================================
//function : NextProject
//purpose  : 
//=======================================================================

Standard_Real ShapeAnalysis_Curve::NextProject(const Standard_Real paramPrev,
					       const Adaptor3d_Curve& C3D,
					       const gp_Pnt& P3D,
					       const Standard_Real preci,
					       gp_Pnt& proj,
					       Standard_Real& param) const
{
  Standard_Real uMin = C3D.FirstParameter();
  Standard_Real uMax = C3D.LastParameter();
  
  Extrema_LocateExtPC aProjector (P3D, C3D, paramPrev/*U0*/, uMin, uMax, preci/*TolU*/);
  if (aProjector.IsDone()){
    param = aProjector.Point().Parameter();
    proj = aProjector.Point().Value();
    return P3D.Distance(proj);
  }
  return Project(C3D, P3D, preci, proj, param, Standard_False);
}

//=======================================================================
//function : AdjustParameters
//purpose  : Copied from StepToTopoDS_GeometricTuul::UpdateParam3d (Aug 2001)
//=======================================================================

Standard_Boolean ShapeAnalysis_Curve::ValidateRange (const Handle(Geom_Curve)& theCurve, 
                                                     Standard_Real& First, Standard_Real& Last,
                                                     const Standard_Real preci) const
{
  // First et/ou Last peuvent etre en dehors des bornes naturelles de la courbe.
  // On donnera alors la valeur en bout a First et/ou Last
  
  Standard_Real cf = theCurve->FirstParameter();
  Standard_Real cl = theCurve->LastParameter();
//  Standard_Real preci = BRepAPI::Precision();

  if (theCurve->IsKind(STANDARD_TYPE(Geom_BoundedCurve)) && !theCurve->IsClosed()) {
    if (First < cf) {
#ifdef OCCT_DEBUG
      cout << "Update Edge First Parameter to Curve First Parameter" << endl;
#endif
      First = cf;
    }
    else if (First > cl) {
#ifdef OCCT_DEBUG
      cout << "Update Edge First Parameter to Curve Last Parameter" << endl;
#endif
      First = cl;
    }
    if (Last < cf) {
#ifdef OCCT_DEBUG
      cout << "Update Edge Last Parameter to Curve First Parameter" << endl;
#endif
      Last = cf;
    }
    else if (Last > cl) {
#ifdef OCCT_DEBUG
      cout << "Update Edge Last Parameter to Curve Last Parameter" << endl;
#endif
      Last = cl;
    }
  }

  // 15.11.2002 PTV OCC966
  if (ShapeAnalysis_Curve::IsPeriodic(theCurve)) {
    ElCLib::AdjustPeriodic(cf,cl,Precision::PConfusion(),First,Last); //:a7 abv 11 Feb 98: preci -> PConfusion()
  }
  else if (First < Last) {
    // nothing to fix
  }
  else if (theCurve->IsClosed()) {
    // l'un des points projecte se trouve sur l'origine du parametrage
    // de la courbe 3D. L algo a donne cl +- preci au lieu de cf ou vice-versa
    // DANGER precision 3d applique a une espace 1d
    
    // Last = cf au lieu de Last = cl
    if      (Abs(Last - cf) < Precision::PConfusion() /*preci*/)  Last = cl ;
    // First = cl au lieu de First = cf
    else if (Abs(First - cl) < Precision::PConfusion() /*preci*/)  First = cf;

    // on se trouve dans un cas ou l origine est traversee
    // illegal sur une courbe fermee non periodique
    // on inverse quand meme les parametres !!!!!!
    else {
      //:S4136 abv 20 Apr 99: r0701_ug.stp #6230: add check in 3d
      if ( theCurve->Value(First).Distance(theCurve->Value(cf)) < preci ) First = cf;
      if ( theCurve->Value(Last).Distance(theCurve->Value(cl)) < preci ) Last = cl;
      if ( First > Last ) {
#ifdef OCCT_DEBUG
	cout << "Warning : parameter range of edge crossing non periodic curve origin" << endl;
#endif
	Standard_Real tmp = First;
	First = Last;
	Last = tmp;
      }
    }
  }
  // The curve is closed within the 3D tolerance
  else if (theCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) {
    Handle(Geom_BSplineCurve) aBSpline = 
      Handle(Geom_BSplineCurve)::DownCast(theCurve);
    if (aBSpline->StartPoint().Distance(aBSpline->EndPoint()) <= preci ) {
//:S4136	<= BRepAPI::Precision()) {
      // l'un des points projecte se trouve sur l'origine du parametrage
      // de la courbe 3D. L algo a donne cl +- preci au lieu de cf ou vice-versa
      // DANGER precision 3d applique a une espace 1d
      
      // Last = cf au lieu de Last = cl
      if      (Abs(Last - cf) < Precision::PConfusion() /*preci*/) Last = cl ;
      // First = cl au lieu de First = cf
      else if (Abs(First - cl) < Precision::PConfusion() /*preci*/) First =  cf;

      // on se trouve dans un cas ou l origine est traversee
      // illegal sur une courbe fermee non periodique
      // on inverse quand meme les parametres !!!!!!
      else {
#ifdef OCCT_DEBUG
	cout << "Warning : parameter range of edge crossing non periodic curve origin" << endl;
#endif
	Standard_Real tmp = First;
	First = Last;
	Last = tmp;
      }
    }
    //abv 15.03.00 #72 bm1_pe_t4 protection of exceptions in draw
    else if ( First > Last ) {
#ifdef OCCT_DEBUG
      cout << "Warning: parameter range is bad; curve reversed" << endl;
#endif
      First = theCurve->ReversedParameter ( First );
      Last = theCurve->ReversedParameter ( Last );
      theCurve->Reverse();
    }
//:j9 abv 11 Dec 98: PRO7747 #4875, after :j8:    else 
    if (First == Last) {  //gka 10.07.1998 file PRO7656 entity 33334
      First = cf; Last = cl;
      return Standard_False;
    }
  }
  else {
#ifdef OCCT_DEBUG
    cout << "UpdateParam3d Failed" << endl;
    cout << "  - Curve Type : " << theCurve->DynamicType() << endl;
    cout << "  - Param 1    : " << First << endl;
    cout << "  - Param 2    : " << Last << endl;
#endif
    //abv 15.03.00 #72 bm1_pe_t4 protection of exceptions in draw
    if ( First > Last ) {
#ifdef OCCT_DEBUG
      cout << "Warning: parameter range is bad; curve reversed" << endl;
#endif
      First = theCurve->ReversedParameter ( First );
      Last = theCurve->ReversedParameter ( Last );
      theCurve->Reverse();
    }
    //pdn 11.01.99 #144 bm1_pe_t4 protection of exceptions in draw
    if (First == Last) {
      First -= Precision::PConfusion();
      Last += Precision::PConfusion();
    }
    return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : FillBndBox
//purpose  : WORK-AROUND for methods brom BndLib which give not exact bounds
//=======================================================================

// search for extremum using Newton
static Standard_Integer SearchForExtremum (const Handle(Geom2d_Curve)& C2d,
					   const Standard_Real First,
					   const Standard_Real Last,
					   const gp_Vec2d &dir,
					   Standard_Real &par,
					   gp_Pnt2d &res)
{
  Standard_Real prevpar;
  Standard_Integer nbOut = 0;
  for (Standard_Integer i = 0; i <10; i++) {
    prevpar = par;
      
    gp_Vec2d D1, D2;
    C2d->D2 ( par, res, D1, D2 );
    Standard_Real Det = ( D2 * dir );
    if ( Abs ( Det ) < 1e-10 ) return Standard_True;
    
    par -= ( D1 * dir ) / Det;
    if ( Abs ( par - prevpar ) < Precision::PConfusion() ) return Standard_True;

    if (par < First)
    {
      if (nbOut++ > 2 || prevpar == First)
        return Standard_False;
      par = First;
    }
    if (par > Last)
    {
      if (nbOut++ > 2 || prevpar == Last)
        return Standard_False;
      par = Last;
    }
  }
  return Standard_True;
}

void ShapeAnalysis_Curve::FillBndBox (const Handle(Geom2d_Curve)& C2d,
				      const Standard_Real First,
				      const Standard_Real Last,
				      const Standard_Integer NPoints,
				      const Standard_Boolean Exact,
				      Bnd_Box2d &Box) const
{
  if (!Exact) {
    Standard_Integer nseg = (NPoints < 2 ? 1 : NPoints - 1);
    Standard_Real step = (Last - First) / nseg;
    for (Standard_Integer i = 0; i <= nseg; i++) {
      Standard_Real par = First + i * step;
      gp_Pnt2d pnt = C2d->Value(par);
      Box.Add(pnt);
    }
    return;
  }

  // We should solve the task on intervals of C2 continuity.
  Geom2dAdaptor_Curve anAC(C2d, First, Last);
  Standard_Integer nbInt = anAC.NbIntervals(GeomAbs_C2);
  // If we have only 1 interval then use input NPoints parameter to get samples.
  Standard_Integer nbSamples = (nbInt < 2 ? NPoints - 1 : nbInt);
  TColStd_Array1OfReal aParams(1, nbSamples + 1);
  if (nbSamples == nbInt)
    anAC.Intervals(aParams, GeomAbs_C2);
  else {
    Standard_Real step = (Last - First) / nbSamples;
    for (Standard_Integer i = 0; i <= nbSamples; i++)
      aParams(i+1) = First + i * step;
  }
  for (Standard_Integer i = 1; i <= nbSamples + 1; i++) {
    Standard_Real aPar1 = aParams(i);
    gp_Pnt2d aPnt = C2d->Value(aPar1);
    Box.Add(aPnt);
    if (i <= nbSamples) {
      Standard_Real aPar2 = aParams(i + 1);
      Standard_Real par = (aPar1 + aPar2) * 0.5;
      gp_Pnt2d pextr;
      Standard_Real parextr = par;
      if (SearchForExtremum(C2d, aPar1, aPar2, gp_Vec2d(1, 0), parextr, pextr)) {
        Box.Add(pextr);
      }
      parextr = par;
      if (SearchForExtremum(C2d, aPar1, aPar2, gp_Vec2d(0, 1), parextr, pextr)) {
        Box.Add(pextr);
      }
    }
  }
}

//=======================================================================
//function : SelectForwardSeam
//purpose  : 
//=======================================================================

Standard_Integer ShapeAnalysis_Curve::SelectForwardSeam(const Handle(Geom2d_Curve)& C1,
							const Handle(Geom2d_Curve)& C2) const
{
  //  SelectForward est destine a devenir un outil distinct
  //  Il est sans doute optimisable !

  Standard_Integer theCurveIndice = 0;

  Handle(Geom2d_Line) L1 = Handle(Geom2d_Line)::DownCast(C1);
  if (L1.IsNull()) {
    // if we have BoundedCurve, create a line from C1
    Handle(Geom2d_BoundedCurve) BC1 = Handle(Geom2d_BoundedCurve)::DownCast(C1);
    if (BC1.IsNull()) return theCurveIndice;
    gp_Pnt2d StartBC1 = BC1->StartPoint();
    gp_Pnt2d EndBC1   = BC1->EndPoint();
    gp_Vec2d VecBC1(StartBC1, EndBC1);
    if (VecBC1.SquareMagnitude() < gp::Resolution()) return theCurveIndice;
    L1 = new Geom2d_Line(StartBC1, VecBC1);
  }

  Handle(Geom2d_Line) L2 = Handle(Geom2d_Line)::DownCast(C2);
  if (L2.IsNull()) {
    // if we have BoundedCurve, creates a line from C2
    Handle(Geom2d_BoundedCurve) BC2 = Handle(Geom2d_BoundedCurve)::DownCast(C2);
    if (BC2.IsNull()) return theCurveIndice;
    gp_Pnt2d StartBC2 = BC2->StartPoint();
    gp_Pnt2d EndBC2   = BC2->EndPoint();
    gp_Vec2d VecBC2(StartBC2, EndBC2);
    if (VecBC2.SquareMagnitude() < gp::Resolution()) return theCurveIndice;
    L2 = new Geom2d_Line(StartBC2, VecBC2);
  }

  Standard_Boolean UdirPos, UdirNeg, VdirPos, VdirNeg;
  UdirPos = UdirNeg = VdirPos = VdirNeg = Standard_False;

  gp_Dir2d theDir  = L1->Direction();
  gp_Pnt2d theLoc1 = L1->Location();
  gp_Pnt2d theLoc2 = L2->Location();

  if        (theDir.X() > 0.) {
    UdirPos = Standard_True; //szv#4:S4163:12Mar99 Udir unused
  } else if (theDir.X() < 0.) {
    UdirNeg = Standard_True; //szv#4:S4163:12Mar99 Udir unused
  } else if (theDir.Y() > 0.) {
    VdirPos = Standard_True; //szv#4:S4163:12Mar99 Vdir unused
  } else if (theDir.Y() < 0.) {
    VdirNeg = Standard_True; //szv#4:S4163:12Mar99 Vdir unused
  }
  
  if        (VdirPos) {
    // max of Loc1.X() Loc2.X()
    if (theLoc1.X() > theLoc2.X())   theCurveIndice  = 1;
    else                             theCurveIndice  = 2;
  } else if (VdirNeg) {
    if (theLoc1.X() > theLoc2.X())   theCurveIndice  = 2;
    else                             theCurveIndice  = 1;
  } else if (UdirPos) {
    // min of Loc1.X() Loc2.X()
    if (theLoc1.Y() < theLoc2.Y())   theCurveIndice  = 1;
    else                             theCurveIndice  = 2;
  } else if (UdirNeg) {
    if (theLoc1.Y() < theLoc2.Y())   theCurveIndice  = 2;
    else                             theCurveIndice  = 1;
  }

  return theCurveIndice;
}

//%11 pdn 12.01.98
//=============================================================================
// Static methods for IsPlanar
// IsPlanar
//=============================================================================

static gp_XYZ GetAnyNormal ( gp_XYZ orig )
{
  gp_XYZ Norm;
  if ( Abs ( orig.Z() ) < Precision::Confusion() )
    Norm.SetCoord ( 0, 0, 1 );
  else {
    Norm.SetCoord ( orig.Z(), 0, -orig.X() );
    Standard_Real nrm = Norm.Modulus();
    if ( nrm < Precision::Confusion() ) Norm.SetCoord ( 0, 0, 1 );
    else Norm = Norm / nrm;
  }
  return Norm;
}

//=======================================================================
//function : GetSamplePoints
//purpose  : 
//=======================================================================
static void AppendControlPoles (TColgp_SequenceOfPnt& seq,
				const Handle(Geom_Curve)& curve)
{
  if ( curve->IsKind(STANDARD_TYPE(Geom_Line))) {
    seq.Append(curve->Value(0));
    seq.Append(curve->Value(1));
  } else if ( curve->IsKind(STANDARD_TYPE(Geom_Conic))) {
    seq.Append(curve->Value(0));
    seq.Append(curve->Value(M_PI/2));
    seq.Append(curve->Value(M_PI));
  } else if ( curve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
    //DeclareAndCast(Geom_TrimmedCurve, Trimmed, curve);
    Handle(Geom_TrimmedCurve) Trimmed = Handle(Geom_TrimmedCurve)::DownCast (curve);
//     AppendControlPoles(seq,Trimmed->BasisCurve());
    Handle(Geom_Curve) aBaseCrv = Trimmed->BasisCurve();
    Standard_Boolean done = Standard_False;
    if ( aBaseCrv->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) {
      try {
        OCC_CATCH_SIGNALS
        Handle(Geom_Geometry) Ctmp = aBaseCrv->Copy();
        Handle(Geom_BSplineCurve) bslp = Handle(Geom_BSplineCurve)::DownCast(Ctmp);
        bslp->Segment(curve->FirstParameter(), curve->LastParameter());
        AppendControlPoles(seq,bslp);
        done = Standard_True;
      }
      catch (Standard_Failure) {
      }
    }
    else if ( aBaseCrv->IsKind(STANDARD_TYPE(Geom_BezierCurve))) {
      try {
        OCC_CATCH_SIGNALS
        Handle(Geom_Geometry) Ctmp = aBaseCrv->Copy();
        Handle(Geom_BezierCurve) bz = Handle(Geom_BezierCurve)::DownCast(Ctmp);
        bz->Segment(curve->FirstParameter(), curve->LastParameter());
        AppendControlPoles(seq,bz);
        done = Standard_True;
      }
      catch (Standard_Failure) {
      }
    }
    if (!done) {
      seq.Append(curve->Value(curve->FirstParameter()));
      seq.Append(curve->Value((curve->FirstParameter() + curve->LastParameter())/2.));
      seq.Append(curve->Value(curve->LastParameter()));
    }
  } else if ( curve->IsKind(STANDARD_TYPE(Geom_OffsetCurve))) {
    //DeclareAndCast(Geom_OffsetCurve, OffsetC, curve);
    Handle(Geom_OffsetCurve) OffsetC = Handle(Geom_OffsetCurve)::DownCast (curve);
//     AppendControlPoles(seq,OffsetC->BasisCurve());
    seq.Append(curve->Value(curve->FirstParameter()));
    seq.Append(curve->Value((curve->FirstParameter() + curve->LastParameter())/2.));
    seq.Append(curve->Value(curve->LastParameter()));
  } else if ( curve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) {
    //DeclareAndCast(Geom_BSplineCurve, BSpline, curve);
    Handle(Geom_BSplineCurve) BSpline = Handle(Geom_BSplineCurve)::DownCast (curve);
    TColgp_Array1OfPnt Poles(1,BSpline->NbPoles());
    BSpline->Poles(Poles);
    for(Standard_Integer i = 1; i <= BSpline->NbPoles(); i++)
      seq.Append(Poles(i));
  } else if ( curve->IsKind(STANDARD_TYPE(Geom_BezierCurve)))  {
    //DeclareAndCast(Geom_BezierCurve, Bezier, curve);
    //Handle(Geom_BezierCurve) Bezier = Handle(Geom_BezierCurve)::DownCast(curve);
    Handle(Geom_BezierCurve) Bezier = Handle(Geom_BezierCurve)::DownCast (curve);
    TColgp_Array1OfPnt Poles(1,Bezier->NbPoles());
    Bezier->Poles(Poles);
    for(Standard_Integer i = 1; i <= Bezier->NbPoles(); i++)
      seq.Append(Poles(i));
  }
}

//%11 pdn 12.01.98
//szv modified
//=======================================================================
//function : IsPlanar
//purpose  : Detects if points lie in some plane and returns normal
//=======================================================================

Standard_Boolean ShapeAnalysis_Curve::IsPlanar (const TColgp_Array1OfPnt& pnts,
                                                gp_XYZ& Normal,
                                                const Standard_Real preci)
{
  Standard_Real precision = (preci > 0.0)? preci : Precision::Confusion();
  Standard_Boolean noNorm = (Normal.SquareModulus() == 0);

  if (pnts.Length() < 3) {
    gp_XYZ N1 = pnts(1).XYZ()-pnts(2).XYZ();
    if (noNorm) {
      Normal = GetAnyNormal(N1);
      return Standard_True;
    }
    return Abs(N1*Normal) < Precision::Confusion();
  }
  
  gp_XYZ aMaxDir;
  if (noNorm) {
    //define a center point
    gp_XYZ aCenter(0,0,0);
    Standard_Integer i = 1;
    for (; i <= pnts.Length(); i++) 
      aCenter += pnts(i).XYZ();
    aCenter/=pnts.Length();
    
    
    aMaxDir = pnts(1).XYZ() - aCenter;
    Normal = (pnts(pnts.Length()).XYZ() - aCenter) ^ aMaxDir;

    for ( i = 1; i < pnts.Length(); i++) {
      gp_XYZ aTmpDir = pnts(i+1).XYZ() - aCenter;
      if(aTmpDir.SquareModulus() > aMaxDir.SquareModulus())
	aMaxDir = aTmpDir;

      gp_XYZ aDelta = (pnts(i).XYZ() - aCenter) ^ (pnts(i+1).XYZ() - aCenter);
      if(Normal*aDelta < 0)
        aDelta*=-1;
      Normal += aDelta;
    }
  }

  // check if points are linear
  Standard_Real nrm = Normal.Modulus();
  if ( nrm < Precision::Confusion() ) {
    Normal = GetAnyNormal(aMaxDir);
    return Standard_True;
  }
  Normal = Normal / nrm;

  Standard_Real mind = RealLast(), maxd = -RealLast(), dev;
  for (Standard_Integer i = 1; i <= pnts.Length(); i++) {
    dev = pnts(i).XYZ() * Normal;
    if (dev < mind) mind = dev;
    if (dev > maxd) maxd = dev;
  }

  return ((maxd - mind) <= precision);
}


//=======================================================================
//function : IsPlanar
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeAnalysis_Curve::IsPlanar (const Handle(Geom_Curve)& curve,
						 gp_XYZ& Normal,
						 const Standard_Real preci)
{
  Standard_Real precision = (preci > 0.0)? preci : Precision::Confusion();
  Standard_Boolean noNorm = (Normal.SquareModulus() == 0);

  if (curve->IsKind(STANDARD_TYPE(Geom_Line))) {
    //DeclareAndCast(Geom_Line, Line, curve);
    Handle(Geom_Line) Line = Handle(Geom_Line)::DownCast (curve);
    gp_XYZ N1 = Line->Position().Direction().XYZ();
    if (noNorm) {
      Normal = GetAnyNormal(N1);
      return Standard_True;
    }
    return Abs(N1*Normal) < Precision::Confusion();
  }

  if (curve->IsKind(STANDARD_TYPE(Geom_Conic))) {
    //DeclareAndCast(Geom_Conic, Conic, curve);
    Handle(Geom_Conic) Conic = Handle(Geom_Conic)::DownCast (curve);
    gp_XYZ N1 = Conic->Axis().Direction().XYZ();
    if (noNorm) {
      Normal = N1;
      return Standard_True;
    }
    gp_XYZ aVecMul = N1^Normal;
    return aVecMul.SquareModulus() < Precision::SquareConfusion();
  }

  if (curve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
    //DeclareAndCast(Geom_TrimmedCurve, Trimmed, curve);
    Handle(Geom_TrimmedCurve) Trimmed = Handle(Geom_TrimmedCurve)::DownCast (curve);
    return IsPlanar(Trimmed->BasisCurve(),Normal,precision);
  }

  if (curve->IsKind(STANDARD_TYPE(Geom_OffsetCurve))) {
    //DeclareAndCast(Geom_OffsetCurve, OffsetC, curve);
    Handle(Geom_OffsetCurve) OffsetC = Handle(Geom_OffsetCurve)::DownCast (curve);
    return IsPlanar(OffsetC->BasisCurve(),Normal,precision);
  }

  if (curve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) {
    //DeclareAndCast(Geom_BSplineCurve, BSpline, curve);
    Handle(Geom_BSplineCurve) BSpline = Handle(Geom_BSplineCurve)::DownCast (curve);
    TColgp_Array1OfPnt Poles(1,BSpline->NbPoles());
    BSpline->Poles(Poles);
    return IsPlanar(Poles,Normal,precision);
  }

  if (curve->IsKind(STANDARD_TYPE(Geom_BezierCurve))) {
    //DeclareAndCast(Geom_BezierCurve, Bezier, curve);
    Handle(Geom_BezierCurve) Bezier = Handle(Geom_BezierCurve)::DownCast (curve);
    TColgp_Array1OfPnt Poles(1,Bezier->NbPoles());
    Bezier->Poles(Poles);
    return IsPlanar(Poles,Normal,precision);
  }

  if (curve->IsKind(STANDARD_TYPE(ShapeExtend_ComplexCurve))) {
    //DeclareAndCast(ShapeExtend_ComplexCurve, Complex, curve);
    Handle(ShapeExtend_ComplexCurve) Complex = Handle(ShapeExtend_ComplexCurve)::DownCast (curve);
    TColgp_SequenceOfPnt sequence;
    Standard_Integer i; // svv Jan11 2000 : porting on DEC
    for (i = 1; i <= Complex->NbCurves(); i++)
      AppendControlPoles(sequence,Complex->Curve(i));
    TColgp_Array1OfPnt Poles(1,sequence.Length());
    for (i=1; i <= sequence.Length(); i++) Poles(i) = sequence(i);
    return IsPlanar(Poles,Normal,precision);
  }

  return Standard_False;
}

//=======================================================================
//function : GetSamplePoints
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Curve::GetSamplePoints (const Handle(Geom_Curve)& curve,
                                                       const Standard_Real first,
                                                       const Standard_Real last,
                                                       TColgp_SequenceOfPnt& seq)
{
  Standard_Real adelta = curve->LastParameter() - curve->FirstParameter();
  if(!adelta )
    return Standard_False;
  
  Standard_Integer aK = (Standard_Integer)ceil ((last - first) / adelta);
  Standard_Integer nbp =100*aK;
  if(curve->IsKind(STANDARD_TYPE(Geom_Line)))
    nbp =2;
  else if(curve->IsKind(STANDARD_TYPE(Geom_Circle)))
    nbp =360*aK;
  
  else if (curve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) {
    Handle(Geom_BSplineCurve) aBspl = Handle(Geom_BSplineCurve)::DownCast(curve);
    
    nbp = aBspl->NbKnots() * aBspl->Degree()*aK;
    if(nbp < 2.0) nbp=2;
  }
  else if (curve->IsKind(STANDARD_TYPE(Geom_BezierCurve))) {
    Handle(Geom_BezierCurve) aB = Handle(Geom_BezierCurve)::DownCast(curve);
    nbp = 3 + aB->NbPoles();
  }
  else if(curve->IsKind(STANDARD_TYPE(Geom_OffsetCurve))) {
    Handle(Geom_OffsetCurve) aC = Handle(Geom_OffsetCurve)::DownCast(curve);
    return GetSamplePoints(aC->BasisCurve(),first,last,seq);
  }
  else if(curve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
    Handle(Geom_TrimmedCurve) aC = Handle(Geom_TrimmedCurve)::DownCast(curve);
    return GetSamplePoints(aC->BasisCurve(),first,last,seq);
  }

  GeomAdaptor_Curve GAC(curve);
  Standard_Real step = ( last - first ) / (Standard_Real)( nbp - 1 );
  for (Standard_Integer i = 0; i < nbp - 1; ++i)
    seq.Append(GAC.Value(first + step * i));
  seq.Append(GAC.Value(last));
  return Standard_True;
}
//=======================================================================
//function : GetSamplePoints
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Curve::GetSamplePoints (const Handle(Geom2d_Curve)& curve,
                                                       const Standard_Real first,
                                                       const Standard_Real last,
                                                       TColgp_SequenceOfPnt2d& seq)
{
  //:abv 05.06.02: TUBE.stp 
  // Use the same distribution of points as BRepTopAdaptor_FClass2d for consistency
  Geom2dAdaptor_Curve C ( curve, first, last );
  Standard_Integer nbs = Geom2dInt_Geom2dCurveTool::NbSamples(C);
  //-- Attention aux bsplines rationnelles de degree 3. (bouts de cercles entre autres)
  if (nbs > 2) nbs*=4;
  Standard_Real step = ( last - first ) / (Standard_Real)( nbs - 1 );
  for (Standard_Integer i = 0; i < nbs - 1; ++i)
    seq.Append(C.Value(first + step * i));
  seq.Append(C.Value(last));
  return Standard_True;
/*
  Standard_Integer i;
  Standard_Real step;
  gp_Pnt2d Ptmp;
  if ( curve->IsKind(STANDARD_TYPE(Geom2d_Line))) {
    seq.Append(curve->Value(first));
    seq.Append(curve->Value(last));
    return Standard_True;
  }
  else if(curve->IsKind(STANDARD_TYPE(Geom2d_Conic))) {
    step = Min ( M_PI, last-first ) / 19; //:abv 05.06.02 TUBE.stp #19209...: M_PI/16
//     if( step>(last-first) ) {
//       seq.Append(curve->Value(first));
//       seq.Append(curve->Value((last+first)/2));
//       seq.Append(curve->Value(last));
//       return Standard_True;
//     }
//     else {
      Standard_Real par=first;
      for(i=0; par<last; i++) {
        seq.Append(curve->Value(par));
        par += step;
      }
      seq.Append(curve->Value(last));
      return Standard_True;
//     }
  }
  else if ( curve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve))) {
    DeclareAndCast(Geom2d_TrimmedCurve, Trimmed, curve);
    return GetControlPoints(Trimmed->BasisCurve(), seq, first, last);
  }
  else if ( curve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))) {
    DeclareAndCast(Geom2d_BSplineCurve, aBs, curve);
    TColStd_SequenceOfReal aSeqParam;
    if(!aBs.IsNull()) {
      aSeqParam.Append(first);
      for(i=1; i<=aBs->NbKnots(); i++) {
        if( aBs->Knot(i)>first && aBs->Knot(i)<last ) 
          aSeqParam.Append(aBs->Knot(i));
      }
      aSeqParam.Append(last);
      Standard_Integer NbPoints=aBs->Degree();
      if( (aSeqParam.Length()-1)*NbPoints>10 ) {
        for(i=1; i<aSeqParam.Length(); i++) {
          Standard_Real FirstPar = aSeqParam.Value(i);
          Standard_Real LastPar = aSeqParam.Value(i+1);
          step = (LastPar-FirstPar)/NbPoints;
          for(Standard_Integer k=0; k<NbPoints; k++ ) {
            aBs->D0(FirstPar+step*k, Ptmp);
            seq.Append(Ptmp);
          }
        }
        aBs->D0(last, Ptmp);
        seq.Append(Ptmp);
        return Standard_True;
      }
      else {
        step = (last-first)/10;
        for(Standard_Integer k=0; k<=10; k++ ) {
          aBs->D0(first+step*k, Ptmp);
          seq.Append(Ptmp);
        }
        return Standard_True;
      }
    }
  }
  else if ( curve->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)))  {
    DeclareAndCast(Geom2d_BezierCurve, aBz, curve);
    if(!aBz.IsNull()) {
      Standard_Integer NbPoints=aBz->Degree();
      step = (last-first)/NbPoints;
      for(Standard_Integer k=0; k<NbPoints; k++ ) {
        aBz->D0(first+step*k, Ptmp);
        seq.Append(Ptmp);
      }
      aBz->D0(last, Ptmp);
      seq.Append(Ptmp);
      return Standard_True;
    }
  }
  return Standard_False;
*/
}

//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean ShapeAnalysis_Curve::IsClosed(const Handle(Geom_Curve)& theCurve,
                                               const Standard_Real preci)
{
  if (theCurve->IsClosed())
    return Standard_True;
  
  Standard_Real prec = Max (preci, Precision::Confusion());

  Standard_Real f, l;
  f = theCurve->FirstParameter();
  l = theCurve->LastParameter();
  
  if (Precision::IsInfinite (f) || Precision::IsInfinite (l))
    return Standard_False;
  
  Standard_Real aClosedVal = theCurve->Value(f).SquareDistance(theCurve->Value(l));
  Standard_Real preci2 = prec*prec;

  return (aClosedVal <= preci2);
}
//=======================================================================
//function : IsPeriodic
//purpose  : OCC996
//=======================================================================

Standard_Boolean ShapeAnalysis_Curve::IsPeriodic(const Handle(Geom_Curve)& theCurve)
{
  // 15.11.2002 PTV OCC966
  // remove regressions in DE tests (diva, divb, divc, toe3) in KAS:dev
  // ask IsPeriodic on BasisCurve
  Handle(Geom_Curve) aTmpCurve = theCurve;
  while ( (aTmpCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve))) ||
          (aTmpCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) ) {
    if (aTmpCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
      aTmpCurve = Handle(Geom_OffsetCurve)::DownCast(aTmpCurve)->BasisCurve();
    if (aTmpCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
      aTmpCurve = Handle(Geom_TrimmedCurve)::DownCast(aTmpCurve)->BasisCurve();
  }
  return aTmpCurve->IsPeriodic();
}

Standard_Boolean ShapeAnalysis_Curve::IsPeriodic(const Handle(Geom2d_Curve)& theCurve)
{
  // 15.11.2002 PTV OCC966
  // remove regressions in DE tests (diva, divb, divc, toe3) in KAS:dev
  // ask IsPeriodic on BasisCurve
  Handle(Geom2d_Curve) aTmpCurve = theCurve;
  while ( (aTmpCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve))) ||
          (aTmpCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve))) ) {
    if (aTmpCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)))
      aTmpCurve = Handle(Geom2d_OffsetCurve)::DownCast(aTmpCurve)->BasisCurve();
    if (aTmpCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
      aTmpCurve = Handle(Geom2d_TrimmedCurve)::DownCast(aTmpCurve)->BasisCurve();
  }
  return aTmpCurve->IsPeriodic();
}
