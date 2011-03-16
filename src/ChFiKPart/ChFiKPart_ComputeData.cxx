// File:	ChFiKPart_ComputeData.cxx
// Created:	Thu Dec 23 16:42:44 1993
// Author:	Isabelle GRIGNON
//		<isg@sdsun1>


#include <ChFiKPart_ComputeData.ixx>

#include <Standard_NotImplemented.hxx>
#include <Standard_ConstructionError.hxx>
#include <Precision.hxx>
#include <gp.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <gp_Lin.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Pln.hxx>
#include <gp_Cylinder.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
//#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <IntSurf_Transition.hxx>
#include <IntSurf_TypeTrans.hxx>
#include <IntRes2d_Transition.hxx>
#include <IntRes2d_TypeTrans.hxx>
#include <IntRes2d_Position.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntAna_QuadQuadGeo.hxx>
#include <Geom2dInt_GInter.hxx>
#include <TopExp.hxx>
//#include <BRepAdaptor_HCurve2d.hxx>
#include <ChFiDS_FilSpine.hxx>
#include <ChFiDS_ChamfSpine.hxx>

#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_Surface.hxx>

#include <ChFiKPart_ComputeData_Fcts.hxx>
#include <ChFiKPart_ComputeData_ChPlnPln.hxx>
#include <ChFiKPart_ComputeData_ChPlnCyl.hxx>
#include <ChFiKPart_ComputeData_ChPlnCon.hxx>
#include <ChFiKPart_ComputeData_ChAsymPlnPln.hxx>
#include <ChFiKPart_ComputeData_ChAsymPlnCyl.hxx>
#include <ChFiKPart_ComputeData_ChAsymPlnCon.hxx> 
#include <ChFiKPart_ComputeData_FilPlnCon.hxx>
#include <ChFiKPart_ComputeData_FilPlnCyl.hxx>
#include <ChFiKPart_ComputeData_FilPlnPln.hxx>


#include <ChFiKPart_ComputeData_CS.hxx>
#include <ChFiKPart_ComputeData_Rotule.hxx>
#include <ChFiKPart_ComputeData_Sphere.hxx>

//=======================================================================
//function : ComputeAbscissa
//purpose  : 
//=======================================================================

// Unused :
#ifdef DEB
static Standard_Real ComputeAbscissa(const BRepAdaptor_Curve& C,
				     const Standard_Real UFirst) 
{
  Standard_Real fp = fp = C.FirstParameter();
  switch (C.GetType()) {
  case GeomAbs_Line:
    return UFirst - fp;
  case GeomAbs_Circle:
    return C.Circle().Radius()*(UFirst-fp);
#ifndef DEB
  default:
    break;
#endif
  }    
  Standard_NotImplemented::Raise("calcul abscisse non traite ");
  return 0.;
}
#endif

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
 Standard_Boolean ChFiKPart_ComputeData::Compute
 (TopOpeBRepDS_DataStructure&    DStr, 
  Handle(ChFiDS_SurfData)&       Data, 
  const Handle_Adaptor3d_HSurface& S1, 
  const Handle_Adaptor3d_HSurface& S2, 
  const TopAbs_Orientation       Or1, 
  const TopAbs_Orientation       Or2, 
  const Handle(ChFiDS_Spine)&    Sp, 
  const Standard_Integer         Iedge)
{
  Standard_Real Wref = 0.;

  Handle(ChFiDS_FilSpine)   Spine  = Handle(ChFiDS_FilSpine)::DownCast(Sp);
  Handle(ChFiDS_ChamfSpine) CSpine = Handle(ChFiDS_ChamfSpine)::DownCast(Sp);
#ifndef DEB
  Standard_Boolean surfok = Standard_False;
#else
  Standard_Boolean surfok;
#endif
  GeomAbs_SurfaceType typ1 = S1->GetType();
  GeomAbs_SurfaceType typ2 = S2->GetType();
  GeomAbs_CurveType   ctyp;

  if (!Spine.IsNull())
    ctyp = Spine->CurrentElementarySpine(Iedge).GetType();
  else 
    ctyp = CSpine->CurrentElementarySpine(Iedge).GetType();

  // Recuperation des orientations.
  TopAbs_Orientation OrFace1 = TopAbs_FORWARD, OrFace2 = TopAbs_FORWARD;
  Handle(BRepAdaptor_HSurface) HS = Handle(BRepAdaptor_HSurface)::DownCast(S1);
  if (!HS.IsNull()) OrFace1 = HS->ChangeSurface().Face().Orientation();
  HS = Handle(BRepAdaptor_HSurface)::DownCast(S2);
  if (!HS.IsNull()) OrFace2 = HS->ChangeSurface().Face().Orientation();
  
  if(!Spine.IsNull()){
    Standard_Real Radius = Spine->Radius(Iedge);
    if ( typ1 == GeomAbs_Plane && typ2 == GeomAbs_Plane ){
      surfok = ChFiKPart_MakeFillet(DStr,Data,S1->Plane(),S2->Plane(), 
				    Or1,Or2,Radius,Spine->Line(),
				    Wref,OrFace1);
    }
    else if ( typ1 == GeomAbs_Plane && typ2 == GeomAbs_Cylinder ){
      if (ctyp == GeomAbs_Line) 
	surfok = ChFiKPart_MakeFillet(DStr,Data,S1->Plane(),S2->Cylinder(),
				      S2->FirstUParameter(),S2->LastUParameter(),
				      Or1,Or2,Radius,Spine->Line(),
				      Wref,OrFace1,Standard_True);
	else
	  surfok = ChFiKPart_MakeFillet(DStr,Data,S1->Plane(),S2->Cylinder(),
					S2->FirstUParameter(),S2->LastUParameter(),
					Or1,Or2,Radius,Spine->Circle(),
					Wref,OrFace1,Standard_True);
    }
    else if ( typ1 == GeomAbs_Cylinder && typ2 == GeomAbs_Plane ){
      if (ctyp == GeomAbs_Line) 
	surfok = ChFiKPart_MakeFillet(DStr,Data,S2->Plane(),S1->Cylinder(), 
				      S1->FirstUParameter(),S1->LastUParameter(),
				      Or2,Or1,Radius,Spine->Line(),
				      Wref,OrFace2,Standard_False);
      else 
	surfok = ChFiKPart_MakeFillet(DStr,Data,S2->Plane(),S1->Cylinder(), 
				      S1->FirstUParameter(),S1->LastUParameter(),
				      Or2,Or1,Radius,Spine->Circle(),
				      Wref,OrFace2,Standard_False);
    }
    else if ( typ1 == GeomAbs_Plane && typ2 == GeomAbs_Cone ){
      surfok =  ChFiKPart_MakeFillet(DStr,Data,S1->Plane(),S2->Cone(), 
				     S2->FirstUParameter(),S2->LastUParameter(),
				     Or1,Or2,Radius,Spine->Circle(),
				     Wref,OrFace1,Standard_True);
    }
    else if ( typ1 == GeomAbs_Cone && typ2 == GeomAbs_Plane ){
      surfok = ChFiKPart_MakeFillet(DStr,Data,S2->Plane(),S1->Cone(), 
				    S1->FirstUParameter(),S1->LastUParameter(),
				    Or2,Or1,Radius,Spine->Circle(),
				    Wref,OrFace2,Standard_False);
    }
    else{
      Standard_NotImplemented::Raise("cas particulier non ecrit");
    }
  }
  else if(!CSpine.IsNull()){
    if (CSpine->IsChamfer() == ChFiDS_Sym) {
      Standard_Real dis;
      CSpine->GetDist(dis);
      if ( typ1 == GeomAbs_Plane && typ2 == GeomAbs_Plane ){
	surfok = ChFiKPart_MakeChamfer(DStr,Data,S1->Plane(),S2->Plane(), 
				      Or1,Or2,dis, dis,CSpine->Line(),
				      Wref,OrFace1);
      }
      else if ( typ1 == GeomAbs_Plane && typ2 == GeomAbs_Cylinder ){
	if (ctyp == GeomAbs_Circle)
	  surfok = ChFiKPart_MakeChamfer(DStr,Data,S1->Plane(),S2->Cylinder(),
					 S2->FirstUParameter(),S2->LastUParameter(),
					 Or1,Or2,dis,dis ,CSpine->Circle(),
					 Wref,OrFace1,Standard_True);
	else
	  surfok = ChFiKPart_MakeChamfer(DStr,Data,S1->Plane(),S2->Cylinder(),
					 S2->FirstUParameter(),S2->LastUParameter(),
					 Or1,Or2,dis,dis,CSpine->Line(),
					 Wref,OrFace1,Standard_True);
      }
      else if ( typ1 == GeomAbs_Cylinder && typ2 == GeomAbs_Plane ){
	if (ctyp == GeomAbs_Circle)
	  surfok = ChFiKPart_MakeChamfer(DStr,Data,S2->Plane(),S1->Cylinder(), 
					 S1->FirstUParameter(),S1->LastUParameter(),
					 Or2,Or1,dis,dis,CSpine->Circle(),
					 Wref,OrFace2,Standard_False);
	else
	  surfok = ChFiKPart_MakeChamfer(DStr,Data,S2->Plane(),S1->Cylinder(), 
					 S1->FirstUParameter(),S1->LastUParameter(),
					 Or2,Or1,dis,dis,CSpine->Line(),
					 Wref,OrFace2,Standard_False);
      }
      else if ( typ1 == GeomAbs_Plane && typ2 == GeomAbs_Cone ){
	surfok = ChFiKPart_MakeChamfer(DStr,Data,S1->Plane(),S2->Cone(), 
				       S2->FirstUParameter(),S2->LastUParameter(),
				       Or1,Or2,dis,dis,CSpine->Circle(),
				       Wref,OrFace1,Standard_True);
      }
      else if ( typ1 == GeomAbs_Cone && typ2 == GeomAbs_Plane ){
	surfok = ChFiKPart_MakeChamfer(DStr,Data,S2->Plane(),S1->Cone(), 
				       S1->FirstUParameter(),S1->LastUParameter(),
				       Or2,Or1,dis,dis,CSpine->Circle(),
				       Wref,OrFace2,Standard_False); 
      }
      else{
	Standard_NotImplemented::Raise("cas particulier non ecrit");
      }
    }
    else if (CSpine->IsChamfer() == ChFiDS_TwoDist) {    
      Standard_Real dis1,dis2;
      CSpine->Dists(dis1,dis2);
      if ( typ1 == GeomAbs_Plane && typ2 == GeomAbs_Plane ){
	surfok = ChFiKPart_MakeChamfer(DStr,Data,S1->Plane(),S2->Plane(), 
				      Or1,Or2,dis1,dis2,CSpine->Line(),
				      Wref,OrFace1);
      }
      else if ( typ1 == GeomAbs_Plane && typ2 == GeomAbs_Cylinder ){
	if (ctyp == GeomAbs_Circle)
	  surfok = ChFiKPart_MakeChamfer(DStr,Data,S1->Plane(),S2->Cylinder(),
					 S2->FirstUParameter(),S2->LastUParameter(),
					 Or1,Or2,dis1,dis2,CSpine->Circle(),
					 Wref,OrFace1,Standard_True);
	else
	  surfok = ChFiKPart_MakeChamfer(DStr,Data,S1->Plane(),S2->Cylinder(),
					 S2->FirstUParameter(),S2->LastUParameter(),
					 Or1,Or2,dis1,dis2,CSpine->Line(),
					 Wref,OrFace1,Standard_True);
      }
      else if ( typ1 == GeomAbs_Cylinder && typ2 == GeomAbs_Plane ){
	if (ctyp == GeomAbs_Circle)
	  surfok = ChFiKPart_MakeChamfer(DStr,Data,S2->Plane(),S1->Cylinder(), 
					 S1->FirstUParameter(),S1->LastUParameter(),
					 Or2,Or1,dis2,dis1,CSpine->Circle(),
					 Wref,OrFace2,Standard_False);
	else
	  surfok = ChFiKPart_MakeChamfer(DStr,Data,S2->Plane(),S1->Cylinder(), 
					 S1->FirstUParameter(),S1->LastUParameter(),
					 Or2,Or1,dis2,dis1,CSpine->Line(),
					 Wref,OrFace2,Standard_False);
      }
      else if ( typ1 == GeomAbs_Plane && typ2 == GeomAbs_Cone ){
	surfok = ChFiKPart_MakeChamfer(DStr,Data,S1->Plane(),S2->Cone(), 
				       S2->FirstUParameter(),S2->LastUParameter(),
				       Or1,Or2,dis1,dis2,CSpine->Circle(),
				       Wref,OrFace1,Standard_True);
      }
      else if ( typ1 == GeomAbs_Cone && typ2 == GeomAbs_Plane ){
	surfok = ChFiKPart_MakeChamfer(DStr,Data,S2->Plane(),S1->Cone(), 
				       S1->FirstUParameter(),S1->LastUParameter(),
				       Or2,Or1,dis2,dis1,CSpine->Circle(),
				       Wref,OrFace2,Standard_False); 
      }
      else{
	Standard_NotImplemented::Raise("cas particulier non ecrit");
      }
    }
    else {
      Standard_Real dis, Angle;
      Standard_Boolean DisOnP;
      CSpine->GetDistAngle(dis, Angle, DisOnP);
      if ( typ1 == GeomAbs_Plane && typ2 == GeomAbs_Plane ){
	surfok = ChFiKPart_MakeChAsym(DStr,Data,S1->Plane(),S2->Plane(), 
				      Or1,Or2, dis, Angle, CSpine->Line(),
				      Wref,OrFace1, DisOnP);
      }
      else if ( typ1 == GeomAbs_Plane && typ2 == GeomAbs_Cylinder ){
	if (ctyp == GeomAbs_Circle)
	  surfok = ChFiKPart_MakeChAsym(DStr,Data,S1->Plane(),S2->Cylinder(),
					S2->FirstUParameter(),S2->LastUParameter(),
					Or1,Or2,dis, Angle, CSpine->Circle(),
					Wref,OrFace1,Standard_True, DisOnP);
	else
	  surfok = ChFiKPart_MakeChAsym(DStr,Data,S1->Plane(),S2->Cylinder(),
					S2->FirstUParameter(),S2->LastUParameter(),
					Or1,Or2, dis, Angle, CSpine->Line(),
					Wref,OrFace1,Standard_True, DisOnP);
      }
      else if ( typ1 == GeomAbs_Cylinder && typ2 == GeomAbs_Plane ){
	if (ctyp == GeomAbs_Circle)
	  surfok = ChFiKPart_MakeChAsym(DStr,Data,S2->Plane(),S1->Cylinder(), 
					S1->FirstUParameter(),S1->LastUParameter(),
					Or2,Or1,dis, Angle, CSpine->Circle(),
					Wref,OrFace2,Standard_False, DisOnP);
	else
	  surfok = ChFiKPart_MakeChAsym(DStr,Data,S2->Plane(),S1->Cylinder(), 
					S1->FirstUParameter(),S1->LastUParameter(),
					Or2,Or1,dis, Angle, CSpine->Line(),
					Wref,OrFace2,Standard_False, DisOnP);
      }
      else if ( typ1 == GeomAbs_Plane && typ2 == GeomAbs_Cone ){
	surfok = ChFiKPart_MakeChAsym(DStr,Data,S1->Plane(),S2->Cone(), 
				      S2->FirstUParameter(),S2->LastUParameter(),
				      Or1,Or2, dis, Angle, CSpine->Circle(),
				      Wref,OrFace1,Standard_True, DisOnP);
      }
      else if ( typ1 == GeomAbs_Cone && typ2 == GeomAbs_Plane ){
	surfok = ChFiKPart_MakeChAsym(DStr,Data,S2->Plane(),S1->Cone(), 
				      S1->FirstUParameter(),S1->LastUParameter(),
				      Or2,Or1, dis, Angle, CSpine->Circle(),
				      Wref,OrFace2,Standard_False, DisOnP); 
      }
      else{
	Standard_NotImplemented::Raise("cas particulier non ecrit");
      }
    }
  }
  return surfok; 
}

//=======================================================================
//function : ComputeCorner
//purpose  : 
//=======================================================================

Standard_Boolean ChFiKPart_ComputeData::ComputeCorner
  (TopOpeBRepDS_DataStructure& DStr,
   const Handle(ChFiDS_SurfData)& Data, 
   const Handle(Adaptor3d_HSurface)& S1, 
   const Handle(Adaptor3d_HSurface)& S2,
   const TopAbs_Orientation OrFace1,
   const TopAbs_Orientation,
   const TopAbs_Orientation Or1,
   const TopAbs_Orientation Or2,
   const Standard_Real minRad, 
   const Standard_Real majRad, 
   const gp_Pnt2d& P1S1,
   const gp_Pnt2d& P2S1,
   const gp_Pnt2d& P1S2,
   const gp_Pnt2d& P2S2)
{
  Standard_Boolean surfok;
  GeomAbs_SurfaceType typ1 = S1->GetType();
  GeomAbs_SurfaceType typ2 = S2->GetType();
  if ( typ1 != GeomAbs_Plane ){
    Standard_ConstructionError::Raise
      ("la face du conge torique doit etre plane");
  }
  // On calcule comme ligne guide le cercle correspondant 
  // a la section de S2, et divers elements de construction.

  gp_Cylinder cyl;
  gp_Circ circ;
  Standard_Real First,Last,fu,lu;
  ChFiKPart_CornerSpine(S1,S2,P1S1,P2S1,P1S2,P2S2,majRad,cyl,circ,First,Last);
  if ( typ2 == GeomAbs_Cylinder ){
    cyl = S2->Cylinder();
    fu = P1S2.X();
    lu = P2S2.X();
  }
  else{
    fu = First;
    lu = Last;
  }
  surfok = ChFiKPart_MakeFillet(DStr,Data,S1->Plane(),cyl, 
			       fu,lu,Or1,Or2,minRad,circ,
			       First,OrFace1,Standard_True);
  if(surfok){
    if ( typ2 != GeomAbs_Cylinder ){
      Data->ChangeInterferenceOnS2().ChangePCurveOnFace() = 
	ChFiKPart_PCurve(P1S2,P2S2,First,Last);
    }
    Data->ChangeVertexFirstOnS1().SetPoint(S1->Value(P1S1.X(),P1S1.Y()));
    Data->ChangeVertexLastOnS1().SetPoint(S1->Value(P2S1.X(),P2S1.Y()));
    Data->ChangeVertexFirstOnS2().SetPoint(S2->Value(P1S2.X(),P1S2.Y()));
    Data->ChangeVertexLastOnS2().SetPoint(S2->Value(P2S2.X(),P2S2.Y()));
    Data->ChangeInterferenceOnS1().SetFirstParameter(First);
    Data->ChangeInterferenceOnS1().SetLastParameter(Last);
    Data->ChangeInterferenceOnS2().SetFirstParameter(First);
    Data->ChangeInterferenceOnS2().SetLastParameter(Last);
    return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : ComputeCorner
//purpose  : 
//=======================================================================

Standard_Boolean ChFiKPart_ComputeData::ComputeCorner
  (TopOpeBRepDS_DataStructure& DStr,
   const Handle(ChFiDS_SurfData)& Data, 
   const Handle(Adaptor3d_HSurface)& S1, 
   const Handle(Adaptor3d_HSurface)& S2,
   const TopAbs_Orientation OrFace1,
   const TopAbs_Orientation OrFace2,
   const TopAbs_Orientation Or1,
   const TopAbs_Orientation Or2,
   const Standard_Real Rad, 
   const gp_Pnt2d& PS1,
   const gp_Pnt2d& P1S2,
   const gp_Pnt2d& P2S2)
{
  return ChFiKPart_Sphere(DStr,Data,S1,S2,OrFace1,OrFace2,Or1,Or2,Rad,PS1,P1S2,P2S2);
}

//=======================================================================
//function : ComputeCorner
//purpose  : 
//=======================================================================

Standard_Boolean ChFiKPart_ComputeData::ComputeCorner
  (TopOpeBRepDS_DataStructure& DStr,
   const Handle(ChFiDS_SurfData)& Data, 
   const Handle(Adaptor3d_HSurface)& S, 
   const Handle(Adaptor3d_HSurface)& S1, 
   const Handle(Adaptor3d_HSurface)& S2,
   const TopAbs_Orientation OfS,
   const TopAbs_Orientation OS,
   const TopAbs_Orientation OS1,
   const TopAbs_Orientation OS2,
   const Standard_Real Radius)
{
  GeomAbs_SurfaceType typ = S->GetType();
  GeomAbs_SurfaceType typ1 = S1->GetType();
  GeomAbs_SurfaceType typ2 = S2->GetType();
  if (typ != GeomAbs_Plane ||
      typ1 != GeomAbs_Plane ||
      typ2 != GeomAbs_Plane){
    Standard_ConstructionError::Raise
      ("rotule torique seulement entre des plans");
  }
  return ChFiKPart_MakeRotule(DStr,Data,S->Plane(),S1->Plane(),
			      S2->Plane(),OS,OS1,OS2,Radius,OfS);
}
