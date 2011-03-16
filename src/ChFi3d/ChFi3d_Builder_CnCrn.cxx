// File:	ChFi3d_Builder_CnCrn.cxx
// Created:	03-01-97 
// Author:      MPS  
// Modified by  MPS (14-04-97)  traitement des cas  ou il n'y a pas
//                              d'intersection entre les stripes 
//  Modified by  MPS (16-06-97) : on tient compte du fait que GeomPlate
//                                rend les courbes 2d dans meme ordre que les
//                                courbes frontieres passees en entree   
//  Modified by JLR (20-08-97) mise en place des nouveaux constructeurs de GeomPlate
//
//  Modified by MPS (03-11-97) on ne cree pas un  batten lorsque le rapport
//  entre les deux resolutions sur la surface est trop grand (PRO10649)  
//
//  Modified by MPS (05-12-97) on ne tient pas compte des aretes degenerees
//                             lors du calcul du nombre d'aretes.
//
//  Modified by JCT (08-12-97) traitement des aretes vives consecutives ou non
//                             (grille EDC412 sauf D2, L1, L2, L3)
//
//  Modified by JCT (11-12-97) pb osf avec indpoint + orientation de plate
//                             ( --> D2, L1, L2, L3 valides mais laids)
//  
//  Modified by MPS (24-02-98)  traitement des aretes de regularite 
//
//  Modified by MPS (01-06-98)  traitement des aretes de couture 
//  Modified by MPS (01-12-98)  traitement des bords libres 
//  Modified by MPS (01-02-99)  traitement des aretes de regularite 
//                              consecutives   
// Traitement des coins  		 

#include <Adaptor3d_HCurveOnSurface.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepAlgo_NormalProjection.hxx>
#include <BRepLib_MakeEdge.hxx>  
#include <ChFi3d_Builder.jxx>
#include <ChFi3d_Builder_0.hxx>
#include <ChFiDS_FaceInterference.hxx>
#include <ChFiDS_ListIteratorOfListOfStripe.hxx>
#include <ChFiDS_SurfData.hxx>
#include <ChFiDS_SequenceOfSurfData.hxx>
#include <ChFiDS_Stripe.hxx>
#include <ChFiDS_HData.hxx>
#include <ChFiDS_CommonPoint.hxx>
#include <ChFiDS_Regul.hxx>
#include <ChFiDS_StripeArray1.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_ExtCC.hxx>
#include <Extrema_POnCurv.hxx>
#include <GeomLib.hxx>
#include <Extrema_ExtPC.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Line.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomInt_IntSS.hxx>
#include <GeomLib.hxx>
#include <GeomAdaptor.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_Surface.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <GeomPlate_PlateG0Criterion.hxx>
#include <GeomPlate_HArray1OfHCurveOnSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom2dLProp_CLProps2d.hxx>
#include <GeomPlate_CurveConstraint.hxx>
#include <FairCurve_Batten.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>
#include <math_Matrix.hxx>
#include <PLib.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColgp_SequenceOfXY.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <TColgp_Array1OfXYZ.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_Array2OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColGeom2d_Array1OfCurve.hxx>
#include <TColGeom2d_SequenceOfCurve.hxx>
#include <TColGeom_Array1OfCurve.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <TColGeom2d_HArray1OfCurve.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>    
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_Surface.hxx> 
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <TopOpeBRepDS_SolidSurfaceInterference.hxx>
#include <TopOpeBRepDS_Kind.hxx>
#include <TopOpeBRepDS_Transition.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>
#include <TopOpeBRepDS_SurfaceCurveInterference.hxx>
#include <TopTools_Array2OfShape.hxx>
#include <BRepLib_MakeFace.hxx> 
#include <Precision.hxx>
// performances 
#ifdef DEB
#include <OSD_Chronometer.hxx>
extern Standard_Real  t_plate ,t_approxplate,t_batten; 
extern void ChFi3d_InitChron(OSD_Chronometer& ch);
extern void ChFi3d_ResultChron(OSD_Chronometer & ch,Standard_Real& time);
#endif

//  Modified by Sergey KHROMOV - Fri Dec 21 17:08:19 2001 Begin
Standard_Boolean isTangentFaces(const TopoDS_Edge &theEdge,
				const TopoDS_Face &theFace1,
				const TopoDS_Face &theFace2);
//  Modified by Sergey KHROMOV - Fri Dec 21 17:08:19 2001 End

//=======================================================================
//function : Indices
//purpose  : 
//=======================================================================

static void Indices (     const  Standard_Integer n,
                   const Standard_Integer ic,
                   Standard_Integer & icplus,
                   Standard_Integer & icmoins)
{
  if (ic== (n-1)) icplus=0;
    else icplus=ic+1;  
    if (ic==0) icmoins=n-1;
    else icmoins=ic-1;
}

//=======================================================================
//function : Calcul_Param
//purpose  : 
//=======================================================================

static void Calcul_Param (const Handle(ChFiDS_Stripe)& stripe,
			  const Standard_Integer jfposit,
			  const Standard_Integer indice,
			  const Standard_Boolean isfirst,
			  Standard_Real & param)
{
  if (jfposit==2 ) 
    param=stripe->SetOfSurfData()->Value(indice)->InterferenceOnS2().Parameter(isfirst);   
  else 
    param=stripe->SetOfSurfData()->Value(indice)->InterferenceOnS1().Parameter(isfirst);
}

//=======================================================================
//function : Calcul_P2dOnSurf
//purpose  : 
//=======================================================================

static void Calcul_P2dOnSurf(const Handle(ChFiDS_Stripe)& stripe,
			     const Standard_Integer jfposit,
			     const Standard_Integer indice,
			     const Standard_Real param,
			     gp_Pnt2d & p2)

{
  if (jfposit==1) 
    stripe->SetOfSurfData()->Value(indice)
      ->InterferenceOnS1().PCurveOnSurf()->D0(param,p2);   
  else
    stripe->SetOfSurfData()->Value(indice)
      ->InterferenceOnS2().PCurveOnSurf()->D0(param,p2);        
}

//=======================================================================
//function : Calcul_C2dOnFace
//purpose  : 
//=======================================================================

static void Calcul_C2dOnFace (const Handle(ChFiDS_Stripe)& stripe,
			      const Standard_Integer jfposit,
			      const Standard_Integer indice,
			      Handle(Geom2d_Curve) & c2d)

{
  if (jfposit==1) 
    c2d = stripe->SetOfSurfData()->Value(indice)
      ->InterferenceOnS1().PCurveOnFace();   
  else
    c2d = stripe->SetOfSurfData()->Value(indice)
      ->InterferenceOnS2().PCurveOnFace();        
}

//=======================================================================
//function : Calcul_Orientation
//purpose  : 
//=======================================================================

static void Calcul_Orientation(const Handle(ChFiDS_Stripe)& stripe,
                               const Standard_Integer jfposit,
                               const Standard_Integer indice,
                               TopAbs_Orientation & orient)
{
  if (jfposit==1)
    orient = stripe->SetOfSurfData()->Value(indice)
      ->InterferenceOnS1().Transition();       
  else
    orient = stripe->SetOfSurfData()->Value(indice)
      ->InterferenceOnS2().Transition();
}

//=======================================================================
//function : RemoveSD
//purpose  : 
//=======================================================================

static void RemoveSD(Handle(ChFiDS_Stripe)& Stripe,
                     const Standard_Integer num1,
                     const Standard_Integer num2     )
{
  ChFiDS_SequenceOfSurfData& Seq = 
    Stripe->ChangeSetOfSurfData()->ChangeSequence();
  if(Seq.IsEmpty()) return;
  if (num1==num2) 
    Seq.Remove(num1);
  else
    Seq.Remove(num1,num2);
}

//=======================================================================
//function : cherche_edge1
//purpose  : cherche l'edge commune entre les faces F1 et F2 
//=======================================================================

static void cherche_edge1 (const TopoDS_Face & F1,
                           const TopoDS_Face & F2,
                           TopoDS_Edge & Edge)
{ Standard_Integer i,j; 
  TopoDS_Edge Ecur1,Ecur2;
  Standard_Boolean trouve=Standard_False;
  TopTools_IndexedMapOfShape  MapE1,MapE2;
  TopExp::MapShapes( F1,TopAbs_EDGE,MapE1);
  TopExp::MapShapes( F2,TopAbs_EDGE,MapE2);
  for ( i=1; i<= MapE1.Extent()&&!trouve; i++)
  { 
    TopoDS_Shape aLocalShape = TopoDS_Shape (MapE1(i));
    Ecur1=TopoDS::Edge(aLocalShape);
    //  Ecur1=TopoDS::Edge(TopoDS_Shape (MapE1(i)));
    for ( j=1; j<= MapE2.Extent()&&!trouve; j++)
    { 
      aLocalShape = TopoDS_Shape (MapE2(j));
      Ecur2=TopoDS::Edge(aLocalShape);
      //              Ecur2=TopoDS::Edge(TopoDS_Shape (MapE2(j)));
      if (Ecur2.IsSame(Ecur1))
      {Edge=Ecur1;trouve=Standard_True;}
    }
  }
}

//=======================================================================
//function : CurveHermite
//purpose  : calcule une courbe 3d  au moyen des polynomes d'Hermite.
//           l'arete ic est une arete de regularite . On construit une courbe 3d entre 
//           les aretes icmoins et icplus. 
//=======================================================================

static void CurveHermite (const TopOpeBRepDS_DataStructure& DStr,
                          const Handle(ChFiDS_Stripe)&   CDicmoins,
                          const Standard_Integer jficmoins,
                          const Standard_Integer icmoins,
                          const Standard_Real    picmoins,
                          const Standard_Integer sensicmoins,
                          const Standard_Boolean sharpicmoins,
                          const TopoDS_Edge & Eviveicmoins,
                          const Handle(ChFiDS_Stripe)&   CDicplus,
                          const Standard_Integer jficplus,
                          const Standard_Integer icplus,
                          const Standard_Real    picplus,
                          const Standard_Integer sensicplus,
                          const Standard_Boolean sharpicplus,
                          const TopoDS_Edge & Eviveicplus,
                          const Standard_Integer nbface,
                          TopTools_SequenceOfShape & Ecom,
                          const TopTools_SequenceOfShape & Face,
                          TColGeom2d_SequenceOfCurve & proj2d,
                          TColGeom_SequenceOfCurve & cproj,  
                          TopTools_SequenceOfShape & Eproj,     
                          TColStd_SequenceOfReal & param,
                          Standard_Real & error)
{  
  gp_Pnt p01,p02;
  gp_Vec d11,d12;
  Standard_Integer ii,jj;
  Standard_Real up1,up2;
  Standard_Integer ilin,jfp;
  Handle (Geom_Curve) c1,c2;
  if (sharpicmoins) {
    c1=BRep_Tool::Curve(Eviveicmoins,up1,up2);
  }
  else {
    if (jficmoins==1) 
      ilin= CDicmoins->SetOfSurfData()->Value( icmoins)->InterferenceOnS1().LineIndex();
    else ilin= CDicmoins->SetOfSurfData()->Value(icmoins)->InterferenceOnS2().LineIndex();
    c1=DStr.Curve(ilin).Curve();
  }
  if (sharpicplus){
    c2=BRep_Tool::Curve(Eviveicplus,up1,up2);
  }
  else {
    jfp=3-jficplus;   
    if (jfp==1) 
      ilin= CDicplus->SetOfSurfData()->Value( icplus)->InterferenceOnS1().LineIndex();
    else  ilin=CDicplus->SetOfSurfData()->Value(icplus)->InterferenceOnS2().LineIndex();
    c2=DStr.Curve(ilin ).Curve();
  }
  c1->D1(picmoins,p01,d11);
  c2->D1(picplus,p02,d12);
  Standard_Integer size = 4;
  math_Matrix  MatCoefs(1,size, 1,size);
  TColgp_Array1OfXYZ Cont(1,size);
  PLib::HermiteCoefficients(0, 1,1,1,MatCoefs);
  Standard_Real L1=p01.Distance(p02);
  Standard_Real lambda= ((Standard_Real)1) / Max (d11.Magnitude() / L1, 1.e-6);
  Cont(1) = p01.XYZ();
  if (sensicmoins==1) Cont(2) = d11.XYZ()*(-lambda) ;
  else Cont(2) = d11.XYZ()*(lambda) ;
  lambda= ((Standard_Real)1) / Max (d12.Magnitude() / L1, 1.e-6);
  Cont(3) = p02.XYZ();
  if (sensicplus==1) Cont(4) =  d12.XYZ()*(lambda);
  else Cont(4) =  d12.XYZ()*(-lambda);
  TColgp_Array1OfPnt ExtrapPoles(1, size);
  TColgp_Array1OfPnt ExtraCoeffs(1, size);
  gp_Pnt p0(0,0,0);
  ExtraCoeffs.Init(p0);
  for (ii=1; ii<=size; ii++) {
    for (jj=1; jj<=size; jj++) {
      ExtraCoeffs(jj).ChangeCoord() += MatCoefs(ii,jj)*Cont(ii);
    }
  }
  PLib::CoefficientsPoles(ExtraCoeffs,  PLib::NoWeights(),
                          ExtrapPoles,  PLib::NoWeights());
  Handle(Geom_BezierCurve) Bezier = new (Geom_BezierCurve) (ExtrapPoles);
  BRepLib_MakeEdge Bedge (Bezier);
  TopoDS_Edge edg =Bedge. Edge();
  TopoDS_Face F;
  error=1.e-30;
  Standard_Integer nb;
  for(nb=1;nb<=nbface;nb++){
    F=TopoDS::Face(Face.Value(nb));
    TopTools_IndexedMapOfShape MapE1;
    TopoDS_Edge E1;
    Handle(Geom2d_Curve) proj1;
    Handle(Geom_Curve) proj1c,proj2c;
    BRepAlgo_NormalProjection OrtProj;
    OrtProj.Init(F);
    OrtProj.Add(edg);
    OrtProj.SetParams(1.e-4, 1.e-4, GeomAbs_C1, 14, 16);
    OrtProj.Build();
    if ( OrtProj.IsDone()){
      TopExp::MapShapes(OrtProj.Projection() , TopAbs_EDGE, MapE1);
        if (MapE1.Extent()!=0){
          if (MapE1.Extent()!=1) {
            BRepLib_MakeFace Bface (BRep_Tool::Surface(F));
            F=Bface.Face();
            OrtProj.Init(F);
            OrtProj.Build();
            MapE1.Clear();
            if ( OrtProj.IsDone()) 
              TopExp::MapShapes(OrtProj.Projection() ,TopAbs_EDGE, MapE1);
          }
          if (MapE1.Extent()!=0) { 
            Standard_Boolean trouve=Standard_False;
            for (Standard_Integer ind=1;ind<=MapE1.Extent()&&!trouve;ind++){
              TopoDS_Shape aLocalShape = TopoDS_Shape( MapE1(ind));  
              E1=TopoDS::Edge( aLocalShape );
              //           E1=TopoDS::Edge( TopoDS_Shape (MapE1(ind)));
              if (!BRep_Tool::Degenerated(E1)) trouve=Standard_True;
            }
            Eproj.Append(E1);
            proj1=BRep_Tool::CurveOnSurface(E1,F,up1,up2);
            proj2d.Append(new Geom2d_TrimmedCurve(proj1,up1,up2));
            proj1c=BRep_Tool::Curve(E1,up1,up2);
            cproj.Append(new Geom_TrimmedCurve(proj1c,up1,up2));
            if (error>BRep_Tool::Tolerance(E1)) error=BRep_Tool::Tolerance(E1);
          }
          else {
            Eproj.Append(E1);
            proj2d.Append(proj1);
            cproj.Append(proj1c);
          } 
        }
        else {
          Eproj.Append(E1);
          proj2d.Append(proj1);
          cproj.Append(proj1c);
        }
      }
  }
  for (nb=1;nb<=nbface-1;nb++) {
    BRepAdaptor_Curve C(TopoDS::Edge(Ecom.Value(nb)));
    C.D0(param.Value(nb),p02);
    GeomAdaptor_Curve L (Bezier);
    Extrema_ExtCC ext (C,L);
    if (ext.IsDone()){ 
      if (ext.NbExt()!=0){ 
        Extrema_POnCurv POnC, POnL;
        ext.Points(1, POnC, POnL);
        param.ChangeValue(nb) =POnC.Parameter();
      }
    }
    if (!ext.IsDone()||ext.NbExt()==0) {
      if (!cproj.Value(nb).IsNull()) {
        cproj.Value(nb)->D0(cproj.Value(nb)->LastParameter(),p01); 
      }
      else if (!cproj.Value(nb+1).IsNull()) {
        cproj.Value(nb+1)->D0(cproj.Value(nb+1)->FirstParameter(),p01);  
      } 
      if (p01.Distance(p02)>1.e-4 ){
        Extrema_ExtPC ext1 (p01,C);
        if (ext1.IsDone()){ 
          if (ext1.NbExt()!=0){  
            Extrema_POnCurv POnC(ext1.Point(1));
            param.ChangeValue(nb) =POnC.Parameter();
          }
        }
      }
    }       
  }
}

//=======================================================================
//function : CalculDroite
//purpose  : calcule une droite 2d  passant par le point  p2d1 et de direction xdir ydir 
//=======================================================================

static void CalculDroite(const gp_Pnt2d & p2d1,
                         const Standard_Real  xdir,
                         const Standard_Real  ydir,
                         Handle (Geom2d_Curve) & pcurve) 
{ gp_Dir2d dir1 (xdir, ydir);
  Handle(Geom2d_Line) l= new Geom2d_Line (p2d1,dir1);
  Standard_Real l0 = sqrt(xdir*xdir+ ydir*ydir );
  pcurve = new Geom2d_TrimmedCurve(l,0,l0);
}

//=======================================================================
//function : CalculBatten
//purpose  : calcule un batten entre les courbes 2d  curv2d1 et curv2d2  aux points p2d1 et p2d2  
//=======================================================================

static void CalculBatten (const Handle (GeomAdaptor_HSurface) ASurf, 
                          const TopoDS_Face Face ,
                          const Standard_Real xdir,
                          const Standard_Real  ydir,
                          const gp_Pnt2d & p2d1,
                          const gp_Pnt2d & p2d2,
                          const Standard_Boolean contraint1,
                          const Standard_Boolean contraint2,
                          Handle (Geom2d_Curve) & curv2d1,
                          Handle (Geom2d_Curve) & curv2d2,
                          const Standard_Real  picicplus,
                          const Standard_Real   picplusic,
                          const Standard_Boolean inverseic,
                          const Standard_Boolean inverseicplus,
                          Handle (Geom2d_Curve)& pcurve)
{  
  Standard_Boolean isplane;
#ifndef DEB
  Standard_Boolean anglebig = Standard_False;
#else
  Standard_Boolean anglebig;
#endif
  isplane=ASurf->GetType()==GeomAbs_Plane;
  gp_Dir2d dir1 (xdir, ydir);
  Geom2dLProp_CLProps2d CL1(curv2d1, picicplus, 1, 1.e-4);
  Geom2dLProp_CLProps2d CL2( curv2d2, picplusic, 1, 1.e-4);
  gp_Dir2d dir3,dir4 ;
  CL1.Tangent(dir3);
  CL2.Tangent(dir4);
  if (inverseic)  dir3.Reverse();
  if (inverseicplus)  dir4.Reverse();
  Standard_Real h =  p2d2.Distance(p2d1)/20;
  FairCurve_Batten Bat(p2d1,p2d2,h);
  Bat.SetFreeSliding (Standard_True);
  Standard_Real ang1,ang2;
  ang1=dir1.Angle(dir3);
  if (dir1.Angle(dir4) >0 ) ang2=PI-dir1.Angle(dir4);
  else ang2=-PI-dir1.Angle(dir4);
  if (contraint1&&contraint2) 
  anglebig=(Abs(ang1)>1.2)|| (Abs(ang2)>1.2 );
  else if (contraint1) 
  anglebig=Abs(ang1)>1.2;
  else if (contraint2)
  anglebig=Abs(ang2)>1.2; 
  if (isplane && (Abs(ang1)>PI/2 || Abs(ang2)>PI/2))
  isplane=Standard_False;
  if (anglebig && !isplane) {
    CalculDroite(p2d1,xdir,ydir,pcurve);
  }
  else {
    if (contraint1) Bat.SetAngle1(ang1);
    else  Bat.SetConstraintOrder1(0);
    if (contraint2) Bat.SetAngle2(ang2);
    else Bat.SetConstraintOrder2(0);
    FairCurve_AnalysisCode Iana; 
    Standard_Boolean Ok;
    Ok = Bat.Compute(Iana,25,1.e-2);
#if DEB
    if (!Ok) { 
      cout<<"pas de batten :";
      Bat.Dump(cout);
    }    
#endif  
    if (Ok) {
       pcurve = Bat.Curve();
       Standard_Real umin,vmin,umax,vmax;
       BRepTools::UVBounds(Face,umin,umax,vmin,vmax);
       Bnd_Box2d bf,bc;
       Geom2dAdaptor_Curve acur(pcurve);
       BndLib_Add2dCurve::Add(acur,0,bc);
       bf.Update(umin,vmin,umax,vmax);
       Standard_Real uminc,vminc,umaxc,vmaxc;
       bc.Get(uminc,vminc,umaxc,vmaxc);
       if (uminc<umin-1.e-7) Ok=Standard_False;
       if (umaxc>umax+1.e-7) Ok=Standard_False;
       if (vminc<vmin-1.e-7) Ok=Standard_False;
       if (vmaxc>vmax+1.e-7) Ok=Standard_False;       
    } 
    if (!Ok) CalculDroite(p2d1, xdir,ydir, pcurve);    
  }
}

//=======================================================================
//function : OrientationIcNonVive
//purpose  : calcule l'orientation de la courbe de raccord  entre ic et icplus sachant que ic 
//           n'est pas une arete vive
//=======================================================================

static void OrientationIcNonVive (const Handle(ChFiDS_Stripe) & CDic,
                                  const Standard_Integer jfic,
                                  const Standard_Integer icicplus,
                                  const Standard_Integer  sensic,
                                  TopAbs_Orientation  & orien )
{
  TopAbs_Orientation orinterf; 
  Calcul_Orientation(CDic,jfic,icicplus,orinterf);
  if (sensic!=1){
    if (orinterf==TopAbs_FORWARD) orien=TopAbs_FORWARD;
    else orien=TopAbs_REVERSED; 		
  }
  else {
    if (orinterf==TopAbs_FORWARD) orien=TopAbs_REVERSED;
    else orien=TopAbs_FORWARD;
  }		
}

//=======================================================================
//function : OrientationIcplusNonVive
//purpose  : calcule l'orientation de la courbe de raccord  entre ic et icplus sachant que icplus
//           n'est pas une arete vive;
//=======================================================================

static void OrientationIcplusNonVive (const Handle(ChFiDS_Stripe) & CDicplus,
                                      const Standard_Integer jficplus,
                                      const Standard_Integer icplusic,
                                      const Standard_Integer sensicplus,
                                      TopAbs_Orientation & orien )
{
  TopAbs_Orientation orinterf; 
  Standard_Integer  jfp = 3 -jficplus;
  Calcul_Orientation(CDicplus,jfp,icplusic,orinterf);
  if (sensicplus==1){
    if (orinterf==TopAbs_FORWARD) orien=TopAbs_FORWARD;
    else orien=TopAbs_REVERSED; 		
  }
  else {
    if (orinterf==TopAbs_FORWARD) orien=TopAbs_REVERSED;
    else orien=TopAbs_FORWARD;
  }		
}

//=======================================================================
//function : OrientationAreteViveConsecutive
//purpose  : calcule l'orientation  de la courbe de raccord  entre les aretes ic et icplus 
//           ou ic et icplus sont vives consecutives
//=======================================================================

static void OrientationAreteViveConsecutive (const TopoDS_Shape & Fviveicicplus,
                                             const TopoDS_Shape & Eviveic,
                                             const TopoDS_Vertex & V1,
                                             TopAbs_Orientation & orien)

{ // orinterf est l'orientation de l'edge ic  par rapport a la face Fviveicicplus prise FORWARD
#ifndef DEB
  TopAbs_Orientation orinterf = TopAbs_FORWARD;
#else
  TopAbs_Orientation orinterf;
#endif
  TopoDS_Face F=TopoDS::Face( Fviveicicplus);
  TopoDS_Edge E=TopoDS::Edge( Eviveic);  
  TopExp_Explorer ex;
  for(ex.Init(F.Oriented(TopAbs_FORWARD),TopAbs_EDGE);ex.More(); ex.Next()){
    if(E.IsSame(ex.Current())) {
      orinterf = ex.Current().Orientation();
      break;
    }
  }
  // si V1 est le vertex REVERSED  de l'edge ic alors la courbe de 
  // raccord a la meme orientation que ic 
  TopoDS_Vertex vl;
  vl=TopExp::LastVertex(E);
  if (vl.IsSame(V1)){ 
    if (orinterf==TopAbs_FORWARD) orien=TopAbs_FORWARD;
    else orien=TopAbs_REVERSED;
  }
  else {
    if (orinterf==TopAbs_FORWARD) orien=TopAbs_REVERSED;
    else orien=TopAbs_FORWARD;
  }
}

//=======================================================================
//function : PerformTwoCornerSameExt
//purpose  : calcule l'intersection entre les deux stripes stripe1 et stripe2 
//=======================================================================

static void PerformTwoCornerSameExt(TopOpeBRepDS_DataStructure& DStr,
                                    const Handle(ChFiDS_Stripe)& stripe1,
                                    const Standard_Integer index1,
                                    const Standard_Integer sens1,
                                    const Handle(ChFiDS_Stripe) &stripe2,
                                    const Standard_Integer index2,
                                    const Standard_Integer sens2,
                                    Standard_Boolean & trouve)
                     
{ Handle (TopOpeBRepDS_CurvePointInterference) Interfp1, Interfp2;
  Handle (TopOpeBRepDS_SurfaceCurveInterference) Interfc;
  TopAbs_Orientation orpcurve,trafil1,orsurf1,orsurf2;
  Standard_Boolean isfirst;
  Standard_Integer indic1,indic2, indpoint1,indpoint2,ind,indcurve;
  Standard_Real tol;
  gp_Pnt P1,P2,P3,P4;
  gp_Pnt2d p2d;
  Handle(Geom_Curve)cint;
  Handle(Geom2d_Curve) C2dint1,C2dint2;
  isfirst=sens1==1;
  ChFiDS_CommonPoint& Com11= stripe1->SetOfSurfData()->Value(index1)->ChangeVertex (isfirst,1);
  ChFiDS_CommonPoint& Com12= stripe1->SetOfSurfData()->Value(index1)->ChangeVertex (isfirst,2);
  isfirst=sens2==1;
  ChFiDS_CommonPoint& Com21= stripe2->SetOfSurfData()->Value(index2)->ChangeVertex (isfirst,1);
#ifdef DEB
//  ChFiDS_CommonPoint& Com22= 
//    stripe2->SetOfSurfData()->Value(index2)->ChangeVertex (isfirst,2);
#endif
  indic1=stripe1->SetOfSurfData()->Value(index1)->Surf();
  indic2=stripe2->SetOfSurfData()->Value(index2)->Surf();
  const Handle(ChFiDS_SurfData) Fd1=stripe1->SetOfSurfData()->Value(index1);
  const Handle(ChFiDS_SurfData) Fd2=stripe2->SetOfSurfData()->Value(index2);

  TColStd_Array1OfReal Pardeb(1,4),Parfin(1,4);
  const ChFiDS_FaceInterference& Fi11 = Fd1->InterferenceOnS1();
  const ChFiDS_FaceInterference& Fi12 = Fd1->InterferenceOnS2();
  const ChFiDS_FaceInterference& Fi21 = Fd2->InterferenceOnS1();
  const ChFiDS_FaceInterference& Fi22 = Fd2->InterferenceOnS2();
  gp_Pnt2d pfi11,pfi12,pfi21,pfi22;
  isfirst=sens1==1;
  pfi11 = Fi11.PCurveOnSurf()->Value(Fi11.Parameter(isfirst));
  pfi12 = Fi12.PCurveOnSurf()->Value(Fi12.Parameter(isfirst));
  isfirst=sens2==1;
  if (Com11.Point().Distance(Com21.Point()) <1.e-4) { 
    pfi21 = Fi21.PCurveOnSurf()->Value(Fi21.Parameter(isfirst)); 
    pfi22 = Fi22.PCurveOnSurf()->Value(Fi22.Parameter(isfirst));
  }
  else {
    pfi22 = Fi21.PCurveOnSurf()->Value(Fi21.Parameter(isfirst));  	
    pfi21 = Fi22.PCurveOnSurf()->Value(Fi22.Parameter(isfirst));
  }
  
  Pardeb(1)= pfi11.X();Pardeb(2) = pfi11.Y();
  Pardeb(3)= pfi21.X();Pardeb(4) = pfi21.Y();
  Parfin(1)= pfi12.X();Parfin(2) = pfi12.Y();
  Parfin(3)= pfi22.X();Parfin(4) = pfi22.Y();

  Handle(GeomAdaptor_HSurface) HS1= ChFi3d_BoundSurf(DStr,Fd1,1,2);
  Handle(GeomAdaptor_HSurface) HS2= ChFi3d_BoundSurf(DStr,Fd2,1,2);
  trouve=Standard_False;
  if (ChFi3d_ComputeCurves(HS1,HS2,Pardeb,Parfin,cint,
			   C2dint1,C2dint2,1.e-4,1.e-5,tol)){
    cint->D0(cint->FirstParameter(),P1);
    cint->D0(cint->LastParameter(),P2);
    trouve=((Com11.Point().Distance(P1) <1.e-4 || Com11.Point().Distance(P2)<1.e-4)&&
	    (Com12.Point().Distance(P1) <1.e-4 || Com12.Point().Distance(P2)<1.e-4));
  }

  if (trouve) {
    isfirst=sens1==1;
    stripe1->InDS(isfirst);
    indpoint1=ChFi3d_IndexPointInDS(Com11,DStr);
    indpoint2=ChFi3d_IndexPointInDS(Com12,DStr);
    stripe1->SetIndexPoint(indpoint1,isfirst,1);
    stripe1->SetIndexPoint(indpoint2,isfirst,2);
    isfirst=sens2==1;
    stripe2->InDS(isfirst); 
    if (Com11.Point().Distance(Com21.Point()) <1.e-4) {
      stripe2->SetIndexPoint(indpoint1,isfirst,1);
      stripe2->SetIndexPoint(indpoint2,isfirst,2);
    }
    else {
      stripe2->SetIndexPoint(indpoint2,isfirst,1);
      stripe2->SetIndexPoint(indpoint1,isfirst,2);
    }
    
    orsurf1=Fd1->Orientation();
    trafil1 = DStr.Shape(Fd1->IndexOfS1()).Orientation();
    trafil1 = TopAbs::Compose(trafil1,Fd1->Orientation());
    trafil1 = TopAbs::Compose(TopAbs::Reverse(Fi11.Transition()),trafil1);
    orsurf2=Fd2->Orientation();
    TopOpeBRepDS_Curve tcurv3d(cint,tol);
    indcurve= DStr.AddCurve(tcurv3d);
    cint->D0(cint->FirstParameter(),P1);
    cint->D0(cint->LastParameter(),P2);
    Fi11.PCurveOnFace()->D0(Fi11.LastParameter(),p2d);
    const Handle(Geom_Surface) Stemp = 
      BRep_Tool::Surface(TopoDS::Face(DStr.Shape(Fd1->IndexOfS1())));
    Stemp ->D0(p2d.X(),p2d.Y(),P4);
    Fi11.PCurveOnFace()->D0(Fi11.FirstParameter(),p2d);
    Stemp ->D0(p2d.X(),p2d.Y(),P3);
    if (P1.Distance(P4)<1.e-4  || P2.Distance(P3)<1.e-4)
      orpcurve=trafil1;
    else  orpcurve=TopAbs::Reverse(trafil1);
    if (Com11.Point().Distance(P1) >1.e-4) {
      ind=indpoint1;
      indpoint1=indpoint2;
      indpoint2=ind;
    }    
    Interfp1=ChFi3d_FilPointInDS(TopAbs_FORWARD,indcurve, indpoint1, cint->FirstParameter());
    Interfp2=ChFi3d_FilPointInDS(TopAbs_REVERSED,indcurve,indpoint2, cint->LastParameter());
    DStr.ChangeCurveInterferences(indcurve).Append(Interfp1);
    DStr.ChangeCurveInterferences(indcurve).Append(Interfp2);
    Interfc=ChFi3d_FilCurveInDS(indcurve,indic1,C2dint1,orpcurve);
    DStr.ChangeSurfaceInterferences(indic1).Append(Interfc);
    if (orsurf1==orsurf2) orpcurve=TopAbs::Reverse(orpcurve);
    Interfc=ChFi3d_FilCurveInDS(indcurve,indic2,C2dint2,orpcurve);
    DStr.ChangeSurfaceInterferences(indic2).Append(Interfc);
  }   
}

//=======================================================================
//function : CpOnEdge
//purpose  : determine si la surfdata num  a un common point sur Eadj1 ou Eadj2
//=======================================================================

static void  CpOnEdge (const  Handle(ChFiDS_Stripe) & stripe,
                       Standard_Integer num,
                       Standard_Boolean isfirst,
                       const TopoDS_Edge & Eadj1,
                       const TopoDS_Edge & Eadj2,
                       Standard_Boolean & compoint)

{ ChFiDS_CommonPoint cp1,cp2;
  compoint=Standard_False;  
  cp1 = stripe->SetOfSurfData()->Value(num)->ChangeVertex (isfirst,1);
  cp2 = stripe->SetOfSurfData()->Value(num)->ChangeVertex (isfirst,2);
  if(cp1.IsOnArc()){
    if (cp1.Arc().IsSame(Eadj1)||cp1.Arc().IsSame(Eadj2))
      compoint=Standard_True;
  } 
  if(cp2.IsOnArc()){
    if (cp2.Arc().IsSame(Eadj1)||cp2.Arc().IsSame(Eadj2))
      compoint=Standard_True; 
  } 
}

//=======================================================================
//function : RemoveSurfData
//purpose  : pour chaque stripe suppression des surfdatas initiles 
//=======================================================================

static void RemoveSurfData (const ChFiDS_StripeMap & myVDataMap,
                            const ChFiDS_Map & myEFMap,
                            const TopoDS_Edge &edgecouture,
                            const TopoDS_Face & facecouture,
                            const TopoDS_Vertex &V1)
{ ChFiDS_ListIteratorOfListOfStripe It;
  Standard_Boolean isfirst;
  TopoDS_Edge Ecur,Eadj1,Eadj2;
  TopoDS_Face Fg,Fd,F1,F2;
  TopoDS_Vertex Vbid;
  Standard_Integer nbsurf,nbedge,sense,num;
  for (It.Initialize(myVDataMap(V1));It.More();It.Next()) {
    nbsurf= It.Value()->SetOfSurfData()->Length();
    nbedge = It.Value()->Spine()->NbEdges();
    if  (nbsurf!=1){
      num=ChFi3d_IndexOfSurfData(V1,It.Value(),sense);
      if (sense==1) 
	Ecur = It.Value()->Spine()->Edges(1);
      else  
	Ecur = It.Value()->Spine()->Edges(nbedge);
      ChFi3d_edge_common_faces(myEFMap(Ecur),F1,F2);
      if (F1.IsSame(facecouture)) Eadj1=edgecouture; 
      else ChFi3d_cherche_element(V1,Ecur,F1,Eadj1,Vbid);
      ChFi3d_edge_common_faces(myEFMap(Eadj1),Fg,Fd);
      if (F2.IsSame(facecouture)) Eadj2=edgecouture;
      else ChFi3d_cherche_element(V1,Ecur,F2,Eadj2,Vbid);
      ChFi3d_edge_common_faces(myEFMap(Eadj2),Fg,Fd); 
      Standard_Boolean compoint=Standard_False;
      isfirst=(sense==1); 
      Standard_Integer ind;
      if (sense==1) {
        ind=0;
	// parmi les surfdatas on cherche le plus grand indice ind tel que 
	// la surfdata ait un de ses commonpoint sur Eadj1 et Eadj2
	// on supprime les surfdata de 1 a ind-1   
	for (Standard_Integer i=1;i<=nbsurf;i++) {
	  CpOnEdge (It.Value(),i,isfirst,Eadj1,Eadj2,compoint);
	  if (compoint) ind=i;
	}
	if (ind>=2) RemoveSD(It.Value(),1,ind-1);
      }
      else {
        ind=num;
	// parmi les surfdatas on cherche le plus petit indice ind tel que 
	// la surfdata ait un de ses commonpoint sur Eadj1 et Eadj2
	// on supprime les surfdata de ind+1 a num   
	for (Standard_Integer i=num;i>=1;i--) {
	  CpOnEdge (It.Value(),i,isfirst,Eadj1,Eadj2,compoint);
	  if (compoint) ind=i;
	}
	if (ind<num) RemoveSD(It.Value(),ind+1,num);
      }
    }
  }
}

//=======================================================================
//function : ParametrePlate
//purpose  : 
//=======================================================================

static void ParametrePlate(const Standard_Integer n3d,
                           const GeomPlate_BuildPlateSurface & PSurf,
                           const Handle(Geom_Surface) & Surf,
                           const gp_Pnt & point,
                           const Standard_Real apperror,
                           gp_Pnt2d & uv)
{  Standard_Integer ip;
   gp_Pnt P1;
   Standard_Real par;
   Standard_Boolean trouve=Standard_False;
   for (ip=1;ip<=n3d && !trouve;ip++){
     par=PSurf.Curves2d()->Value(ip)->FirstParameter();
     PSurf.Curves2d()->Value(ip)->D0(par,uv);
     Surf->D0(uv.X(),uv.Y(),P1);       
     trouve=P1.IsEqual(point,apperror);
     if (!trouve) {
        par=PSurf.Curves2d()->Value(ip)->LastParameter();
        PSurf.Curves2d()->Value(ip)->D0(par,uv);
        Surf->D0(uv.X(),uv.Y(),P1);       
        trouve=P1.IsEqual(point,apperror); 
     } 
  }
}

//=======================================================================
//function : SummarizeNormal
//purpose  : 
//=======================================================================

static void SummarizeNormal(const TopoDS_Vertex& V1,
			    const TopoDS_Face& Fcur,
			    const TopoDS_Edge& Ecur,
			    gp_Vec& SumFaceNormalAtV1)
{
  gp_Pnt2d uv1, uv2;
  BRep_Tool::UVPoints(Ecur,Fcur,uv1,uv2);
  if ( ! V1.IsSame(TopExp::FirstVertex(Ecur))) uv1 = uv2;
  
  gp_Pnt P;
  gp_Vec d1U, d1V;
  BRep_Tool::Surface(Fcur)->D1( uv1.X(), uv1.Y(), P, d1U, d1V);
  gp_Vec N = d1U.Crossed(d1V);
  if (Fcur.Orientation() == TopAbs_REVERSED) N.Reverse();
  
  if (N.SquareMagnitude() <= Precision::PConfusion()) return;
  
  SumFaceNormalAtV1 += N.Normalized();
  SumFaceNormalAtV1.Normalize();
}

enum ChFi3d_SurfType { ChFiSURFACE, FACE1, FACE2 }; // for call SurfIndex(...)

//=======================================================================
//function : SurfIndex
//purpose  : 
//=======================================================================

static Standard_Integer SurfIndex(const ChFiDS_StripeArray1& StripeArray1,
				  const Standard_Integer     StripeIndex,
				  const Standard_Integer     SurfDataIndex,
				  const ChFi3d_SurfType      SurfType)
{
  const Handle(ChFiDS_SurfData)& aSurfData =
    StripeArray1(StripeIndex)->SetOfSurfData()->Value(SurfDataIndex);
  switch (SurfType) {
  case ChFiSURFACE: return aSurfData->Surf();
  case FACE1:       return aSurfData->IndexOfS1();
  case FACE2:       return aSurfData->IndexOfS2();
  default:          return -1;
  }
  return -1;
}

//=======================================================================
//function : PlateOrientation
//purpose  : Define Plate orientation compared to <theRefDir> previewing
//           that Plate surface can have a sharp angle with adjacent 
//           filet (bug occ266: 2 chamfs, OnSame and OnDiff) and
//           can be even twisted (grid tests cfi900 B1)
//=======================================================================

static TopAbs_Orientation PlateOrientation(const Handle(Geom_Surface)& thePlateSurf,
					   const Handle(TColGeom2d_HArray1OfCurve)& thePCArr,
					   const gp_Vec& theRefDir)
{
  gp_Vec du,dv;
  gp_Pnt pp1,pp2,pp3;
  gp_Pnt2d uv;
  Standard_Real fpar, lpar;
  Standard_Real SumScal1 = 0, SumScal2 = 0;
  
  Standard_Integer i, nb = thePCArr->Upper();
  Handle(Geom2d_Curve) aPC = thePCArr->Value(nb);
  fpar = aPC->FirstParameter();
  lpar = aPC->LastParameter();
  aPC->D0( (fpar + lpar) / 2., uv);
  thePlateSurf-> D0(uv.X(),uv.Y(),pp1);
  aPC->D0( lpar ,uv);
  thePlateSurf-> D0(uv.X(),uv.Y(),pp2);
  
  for (i=1; i<=nb; i++) {
    aPC = thePCArr->Value(i);
    fpar = aPC->FirstParameter();
    lpar = aPC->LastParameter();
    aPC->D0( fpar ,uv);
    thePlateSurf -> D1(uv.X(),uv.Y(),pp2,du,dv);
    gp_Vec n1 = du^dv;
    n1.Normalize();
    
    aPC->D0( (fpar + lpar) / 2., uv);
    thePlateSurf-> D0(uv.X(),uv.Y(),pp3);
    
    gp_Vec vv1(pp2,pp1), vv2(pp2,pp3);
    gp_Vec n2 = vv2^vv1;
    n2.Normalize();
    
    SumScal1 += n1*n2;
    SumScal2 += n2*theRefDir;

    pp1 = pp3;
  }
  if (SumScal2*SumScal1>0) return TopAbs_FORWARD;
  else                     return TopAbs_REVERSED;
}
                                          
//=======================================================================
//function : PerformMoreThreeCorner
//purpose  : Traite le cas d'un sommet a n aretes.      
//=======================================================================

void  ChFi3d_Builder::PerformMoreThreeCorner(const Standard_Integer Jndex,
                                             const Standard_Integer nconges)
{ 
//    ========================================
//             Initialisations
//     ========================================
#ifdef DEB
  OSD_Chronometer ch;
#endif 
  TopOpeBRepDS_DataStructure& DStr=myDS->ChangeDS();  
  const TopoDS_Vertex& V1 = myVDataMap.FindKey(Jndex);
  Standard_Integer nedge;
  Standard_Boolean bordlibre;
  TopoDS_Edge edgelibre1,edgelibre2;
//  TopTools_ListIteratorOfListOfShape ItE;
  nedge=ChFi3d_NbNotDegeneratedEdges(V1,myVEMap);
  ChFi3d_ChercheBordsLibres(myVEMap,V1,bordlibre,edgelibre1,edgelibre2);
  Standard_Boolean droit=Standard_False;
  if (bordlibre) {nedge=(nedge-2)/2 +2;
     Standard_Real angedg=Abs(ChFi3d_AngleEdge(V1,edgelibre1,edgelibre2));
     droit=Abs(angedg-PI)<0.01;   
  }
  else  nedge=nedge/2;
  Standard_Integer size=nedge*2;
  ChFiDS_StripeArray1 CD(0,size);
  TColStd_Array1OfInteger jf(0,size);
  TColStd_Array1OfInteger Index(0,size);
  TColStd_Array1OfInteger Indice(0,size);
  TColStd_Array1OfInteger sens(0,size);
  TColStd_Array1OfInteger indcurve3d(0,size);
  TColStd_Array2OfInteger numfa( 0,size,0,size);
  TColStd_Array1OfInteger Order( 0,size);
  TColStd_Array2OfInteger i(0,size,0,size);
  TColStd_Array2OfInteger indpoint(0,size,0,1);
  TColStd_Array1OfBoolean  oksea(0,size);
  TColStd_Array1OfBoolean  sharp(0,size);
  TColStd_Array1OfBoolean regul(0,size);
  TColStd_Array2OfReal p (0,size,0,size);
  TColStd_Array1OfReal errapp (0,size);
  TopTools_Array1OfShape Evive(0,size);
  TopTools_Array2OfShape Fvive(0,size,0,size);
  TColStd_Array1OfBoolean ponctuel (0,size);
  TColStd_Array1OfBoolean samedge (0,size);
  TColStd_Array1OfBoolean moresurf (0,size);
  TColStd_Array1OfBoolean libre(0,size);
  TColStd_Array1OfBoolean tangentregul (0,size);
  TColStd_Array1OfBoolean isG1(0,size);
//  for(Standard_Integer ind=0;ind<=size;ind++){
  Standard_Integer ind;
  for(ind=0;ind<=size;ind++){
   indpoint.SetValue(ind,0,0);
    indpoint.SetValue(ind,1,0);
    Indice.SetValue(ind,0);
    oksea.SetValue(ind,Standard_False);
    sharp.SetValue(ind,Standard_False);
    regul.SetValue(ind,Standard_False);
    ponctuel.SetValue(ind,Standard_False);
    samedge.SetValue(ind,Standard_False);
    moresurf.SetValue(ind,Standard_False);
    libre.SetValue(ind,Standard_False);
    tangentregul.SetValue(ind,Standard_False);
    isG1.SetValue(ind,Standard_False);
  }
  ChFiDS_ListIteratorOfListOfStripe It;
  Handle(ChFiDS_Stripe) cd2,cdbid,cnext;
  TopoDS_Face face;
  Standard_Integer jfp,ii;
  Standard_Integer ic,icplus,icmoins,icplus2,
                   sense,index,indice,isurf1,isurf2;
  Standard_Integer cbplus=0, n3d=0,IVtx,nb;
  Standard_Boolean sameside,trouve,isfirst;
  Standard_Real pardeb ,parfin,xdir,ydir;
  Standard_Real tolapp=1.e-4,maxapp,maxapp1,avedev;
  Handle (TopOpeBRepDS_CurvePointInterference) Interfp1, Interfp2;
  Handle (TopOpeBRepDS_SurfaceCurveInterference) Interfc;
  Handle(Geom_Curve) Curv3d;
  ChFiDS_Regul regular;
  TopTools_SequenceOfShape Fproj;
  Standard_Integer num;
  TopoDS_Edge Ecur; 
  TopTools_ListIteratorOfListOfShape ItF;
#ifdef DEB
//  Standard_Integer nface=ChFi3d_nbface(myVFMap(V1));
#endif
  TopoDS_Face F1,F2;
  gp_Vec SumFaceNormalAtV1(0,0,0); // is used to define Plate orientation 

  // on determine s'il y a une arete de couture 
  // la face qui a une arete de couture et l'arete de couture
  Standard_Boolean couture=Standard_False;
  TopoDS_Face facecouture;
  TopoDS_Edge edgecouture;
  for(ItF.Initialize(myVFMap(V1));ItF.More()&&!couture;ItF.Next()) {
    TopoDS_Face fcur = TopoDS::Face(ItF.Value());
    ChFi3d_CoutureOnVertex(fcur,V1,couture,edgecouture);
    if (couture)
      facecouture=fcur;
  }

// On enleve les surfdata inutiles 
  RemoveSurfData (myVDataMap, myEFMap,edgecouture,facecouture,V1);

 // tri des aretes et des faces
  trouve=Standard_False;
  TopoDS_Edge Enext;
  TopoDS_Vertex VV;
  TopoDS_Face Fcur,Fnext;
  for (It.Initialize(myVDataMap(Jndex));It.More()&&!trouve;It.Next()) {
    cnext=It.Value();
    CD.SetValue(0,cnext);
    Index.SetValue(0,ChFi3d_IndexOfSurfData(V1,cnext,sense));
    sens.SetValue(0,sense);
    numfa.SetValue(0 ,1,SurfIndex(CD, 0, Index.Value(0), FACE2));
    numfa.SetValue(1 ,0, numfa.Value(0 ,1));
    Fcur=TopoDS::Face(DStr.Shape(numfa.Value(0,1)));
    Fvive.SetValue(0,1,Fcur);
    Fvive.SetValue(1,0,Fcur);
    jf.SetValue(0,2);
    if (sens.Value(0)==1) 
      Ecur = CD.Value(0)->Spine()->Edges(1);
    else  
      Ecur = CD.Value(0)->Spine()->Edges(CD.Value(0)->Spine()->NbEdges());
    Evive.SetValue(0,Ecur);
    ChFi3d_cherche_edge(V1,Evive,Fcur,Enext,VV);
    trouve= !Enext.IsNull();
  }
  // find sum of all face normales at V1
  SummarizeNormal(V1, Fcur, Ecur, SumFaceNormalAtV1);
  
  Standard_Integer nbcouture=0;
  for ( ii=1; ii<nedge; ii++) {
    if (Fcur.IsSame(facecouture)&& nbcouture==0) {
      Enext=edgecouture;
      nbcouture++;
    }
    else ChFi3d_cherche_edge(V1,Evive,Fcur,Enext,VV);
    if (Enext.IsNull())Standard_Failure::Raise
    ("PerformMoreThreeCorner: pb dans le tri des aretes et des faces"); 
    if (Enext.IsSame(edgelibre1)|| Enext.IsSame(edgelibre2)) {
      CD.SetValue(ii, cdbid);
      Index.SetValue(ii, 0);
      sens.SetValue(ii, -1);
      TopoDS_Vertex Vref;
      Vref = TopExp::FirstVertex(Enext);
      if (Vref.IsSame(V1)) sens.SetValue(ii, 1);   
      sharp.SetValue(ii, Standard_True);
      Evive.SetValue(ii, Enext);
      jf.SetValue(ii, 0);
      Indices(nedge,ii,icplus,icmoins);
      Fvive.SetValue(ii,icplus, Fcur);
      Fvive.SetValue(icplus,ii, Fcur);
      numfa.SetValue(ii,icplus, DStr.AddShape(Fcur));
      numfa.SetValue(icplus,ii,numfa.Value(ii,icplus));
      ii++;
      if (Enext.IsSame (edgelibre1)) Ecur=edgelibre2;
      else Ecur=edgelibre1;
      ChFi3d_edge_common_faces(myEFMap(Ecur),Fcur,Fcur);
      Indices(nedge,ii,icplus,icmoins);
      Fvive.SetValue(ii,icplus, Fcur);
      Fvive.SetValue(icplus,ii, Fcur);
      numfa.SetValue(ii,icplus, DStr.AddShape(Fcur));
      numfa.SetValue(icplus,ii,numfa.Value(ii,icplus));
      CD.SetValue(ii, cdbid);
      Index.SetValue(ii, 0);
      sens.SetValue(ii, -1);
      Vref = TopExp::FirstVertex(Ecur);
      if (Vref.IsSame(V1)) sens.SetValue(ii, 1);   
      sharp.SetValue(ii, Standard_True);
      Evive.SetValue(ii, Ecur);
      jf.SetValue(ii, 0); 
    }
    else {
// on cherche si Enext est dans la map des stripes 
      TopoDS_Edge EE;
      /*Standard_Boolean */trouve = Standard_False;
      for (It.Initialize(myVDataMap(Jndex));It.More()&&!trouve;It.Next()) {
	index = ChFi3d_IndexOfSurfData(V1,It.Value(),sense);
	if (sense==1) 
	  EE = It.Value()->Spine()->Edges(1);
	else  
	  EE = It.Value()->Spine()->Edges(It.Value()->Spine()->NbEdges());
	if (Enext.IsSame(EE)) {
	  cnext=It.Value();
	  trouve=Standard_True;
	}
      }
      if (trouve) {
	CD.SetValue(ii, cnext);
	Index.SetValue(ii, index);
	sens.SetValue(ii, sense);
	sharp.SetValue(ii, Standard_False);
	Evive.SetValue(ii, Enext);
      }
      else {
	//      l'arete ii est vive
	CD.SetValue(ii, cdbid);
	Index.SetValue(ii, 0);
	sens.SetValue(ii, -1);
	TopoDS_Vertex Vref;
	Vref = TopExp::FirstVertex(Enext);
	if (Vref.IsSame(V1)) sens.SetValue(ii, 1);   
	sharp.SetValue(ii, Standard_True);
	Evive.SetValue(ii, Enext);
	jf.SetValue(ii, 0);
      }
      // On cherche la face Fnext!=Fcur qui contient Enext 
      Fnext=Fcur;
      ChFi3d_cherche_face1(myEFMap(Enext),Fcur,Fnext);
      Indices(nedge,ii,icplus,icmoins);
      Fvive.SetValue(ii,icplus, Fnext);
      Fvive.SetValue(icplus,ii, Fnext);
      numfa.SetValue(ii,icplus, DStr.AddShape(Fnext));
      numfa.SetValue(icplus,ii,numfa.Value(ii,icplus));
      Standard_Integer numface1,numface2;
      if (trouve) {
	// on regarde si numfa correspond a IndexOfS1 ou IndexOfS2 
	// on met a jour en consequence jf 
        // si ce n'est pas le cas on recherche parmi les faces precedentes 
        // celle qui correspond a IndexOfs1 IndexOfS2  et on remet a jour 
        // numfa et Fvive (cts16288)  
	numface2 = SurfIndex(CD, ii, Index.Value(ii), FACE2);
	if (numface2==numfa.Value(ii,icplus))
	  jf.SetValue(ii, 2);
	else {
	  numface1 = SurfIndex(CD, ii, Index.Value(ii), FACE1);
	  if (numface1==numfa.Value(ii,icplus))
	    jf.SetValue(ii, 1);
	  else {
	    if (numface1==numfa.Value(icmoins,ii))  {
	      jf.SetValue(ii, 2);
	      Fvive.SetValue(ii,icplus,TopoDS::Face(DStr.Shape(numface2)));
	      Fvive.SetValue(icplus,ii,TopoDS::Face(DStr.Shape(numface2)));
	      numfa.SetValue(ii,icplus, DStr.AddShape(TopoDS::Face(DStr.Shape(numface2))));
	      numfa.SetValue(icplus,ii, numfa.Value (ii,icplus));
	    }
	    if (numface2==numfa.Value(icmoins,ii)) {
	      jf.SetValue(ii, 1);
	      Fvive.SetValue(ii,icplus,TopoDS::Face(DStr.Shape(numface1)));
	      Fvive.SetValue(icplus,ii,TopoDS::Face(DStr.Shape(numface1)));
	      numfa.SetValue(ii,icplus, DStr.AddShape(TopoDS::Face(DStr.Shape(numface1))));
	      numfa.SetValue(icplus,ii, numfa.Value (ii,icplus));
	    }
	  }
	}
      }
      Ecur = Enext;
      Fcur = Fnext;
      // find sum of all face normales at V1
      SummarizeNormal(V1, Fcur, Ecur, SumFaceNormalAtV1);
    }
  }
  // mise a jour du tableau regul 
  for (ic=0;ic<nedge;ic++) {
    if (sharp.Value(ic)) {
      Ecur=TopoDS::Edge(Evive.Value(ic));
      if (!Ecur.IsSame(edgecouture)) {
	ChFi3d_edge_common_faces(myEFMap(Ecur),F1,F2);   
//  Modified by Sergey KHROMOV - Fri Dec 21 18:11:02 2001 Begin
// 	regul.SetValue(ic,BRep_Tool::Continuity(TopoDS::Edge(Evive.Value(ic)),F1,F2)
// 		     !=GeomAbs_C0); 
	regul.SetValue(ic,isTangentFaces(TopoDS::Edge(Evive.Value(ic)),F1,F2)); 
//  Modified by Sergey KHROMOV - Fri Dec 21 18:11:07 2001 End
      }
    }
  }
  // on verifie qu'une arete de regularite n'est pas tangente a une autre arete
  // dans ce cas on ne la considere pas comme reguliere (cts60072)
  for (ic=0;ic<nedge;ic++) {
    if (regul.Value(ic) ) {
      trouve=Standard_False;
      TopoDS_Edge ereg=TopoDS::Edge(Evive.Value(ic));
      for( ind=0;ind<nedge &&!trouve;ind++) {
	if (ind!=ic) {
	  TopoDS_Edge ecur=TopoDS::Edge(Evive.Value(ind));
	  Standard_Real ang=Abs(ChFi3d_AngleEdge(V1,ecur,ereg)); 
	  if (ang<0.01 || Abs(ang-PI) <0.01) {
	    regul.SetValue(ic,Standard_False);
	    tangentregul.SetValue(ic,Standard_True);
	    trouve=Standard_True;
	  }  
	}
      }   
    }
  }

  // la variable deuxconges permet de detecter les cas ou on a un sommet a
  // n aretes et deux conges sur deux aretes tangentes qui ne sont pas
  // des bords libres 
  // les courbes de raccord partent du conge jusqu'au sommet 
  
  Standard_Boolean deuxconges,deuxcgnontg;
  deuxconges=Standard_False;
  trouve=Standard_False;
  if (nconges==2) {
    TopoDS_Edge E1,E2; 
    for (ic=0;ic<nedge&&!trouve;ic++) {
      Indices(nedge,ic,icplus,icmoins);
      if (!sharp.Value(ic) && !sharp.Value(icplus)){
	E1=TopoDS::Edge(Evive.Value(ic));
	E2=TopoDS::Edge(Evive.Value(icplus));                                   
	deuxconges=(Abs(ChFi3d_AngleEdge(V1 ,E1,E2)   )<0.01) ;
        trouve=deuxconges;
      }
    }
  }

  // on utilise la variable deuxconges dans le cas particulier
  // ou on a deux conges et si les deux autres aretes vives 
  // sont tangentes (cts60072) 
  if (nconges==2 && nedge==4) {
    TopoDS_Edge E1,E2; 
     for (ic=0;ic<nedge&&!deuxconges;ic++) {
       Indices(nedge,ic,icplus,icmoins);
       if (sharp.Value(ic) && sharp.Value(icplus)){
	 E1=TopoDS::Edge(Evive.Value(ic));
	 E2=TopoDS::Edge(Evive.Value(icplus));
         if ( !E1.IsSame(edgelibre1) && !E1.IsSame(edgelibre2) &&
              !E2.IsSame(edgelibre1) && !E2.IsSame(edgelibre2)){ 
	   Standard_Real ang=Abs(ChFi3d_AngleEdge(V1 ,E1,E2));
	   deuxconges=(ang<0.01 || Abs(ang-PI)<0.01);
         }
       }
     }
  }
  
  deuxcgnontg=nconges==2&& nedge==3 && !deuxconges; // pro12305 

  if (deuxconges )
  for  (ic=0;ic<nedge;ic++){
   regul.SetValue(ic,Standard_False);
  }

  // Detect case of 3 edges & 2 conges: OnSame + OnDiff
  // (eap, Arp 9 2002, occ266)
  Standard_Boolean isOnSameDiff = Standard_False;
  if (deuxcgnontg) {
    Standard_Boolean isOnSame = Standard_False, isOnDiff = Standard_False;
    for (ic=0; ic<nedge; ic++) {
      if (sharp.Value(ic)) continue;
      ChFiDS_State stat;
      if ( sens(ic) == 1 )
	stat = CD.Value(ic)->Spine()->FirstStatus();
      else
	stat = CD.Value(ic)->Spine()->LastStatus();
      
      if      (stat == ChFiDS_OnSame) isOnSame = Standard_True;
      else if (stat == ChFiDS_OnDiff) isOnDiff = Standard_True;
    }
    isOnSameDiff = isOnSame && isOnDiff;
  }
  if ( isOnSameDiff ) {
#ifdef DEB
    cout << "OnSame + OnDiff, PerformMoreThreeCorner() calls PerformOneCorner()" << endl;
#endif
    PerformOneCorner (Jndex, Standard_True);
  }

// si le commonpoint est sur une arete qui n'a pas comme
// extremite le vertex , on  determine a nouveau Evive     
// On determine a nouveau Fvive si elle ne correspondant 
// pas a une des deux faces adjacentes a Evive (cts16288)

  if (!deuxconges && !isOnSameDiff) 
    for (ic=0;ic<nedge;ic++) { 
      if (sharp.Value(ic)) {
	Indices(nedge,ic,icplus,icmoins);
        TopoDS_Edge Arc=TopoDS::Edge(Evive.Value(ic));
	ChFiDS_CommonPoint cp1, cp2;
        Standard_Real angedg=PI;
        TopoDS_Vertex Vcom;
	if (!sharp.Value(icplus)) {
	  isfirst=(sens.Value(icplus)==1);
	  jfp = 3 - jf.Value(icplus);
	  cp1 = CD.Value(icplus)->SetOfSurfData()->Value(Index.Value(icplus))->
	    ChangeVertex (isfirst,jfp);
	  if (cp1.IsOnArc()){
            ChFi3d_cherche_vertex(Arc,cp1.Arc(),Vcom,trouve);
            if (trouve) angedg=Abs(ChFi3d_AngleEdge(Vcom,Arc,cp1.Arc()));
	    if (!cp1.Arc().IsSame(Arc) && Abs(angedg-PI)<0.01){
	      Evive.SetValue(ic,cp1.Arc());
	      ChFi3d_edge_common_faces(myEFMap(cp1.Arc()),F1,F2);
	      if (!Fvive.Value(ic,icplus).IsSame(F1) && !Fvive.Value(ic,icplus).IsSame(F2)) {
	        if (Fvive.Value(ic,icmoins).IsSame(F2))  {
		  Fvive.SetValue(ic,icplus,F1);
                  Fvive.SetValue(icplus,ic,F1);
		  numfa.SetValue(ic,icplus,DStr.AddShape(F1)); 
                  numfa.SetValue(icplus,ic,DStr.AddShape(F1)); 
                }
		else  {
		  Fvive.SetValue(ic,icplus,F2);
                  Fvive.SetValue(icplus,ic,F2);
		  numfa.SetValue(ic,icplus,DStr.AddShape(F2)); 
                  numfa.SetValue(icplus,ic,DStr.AddShape(F2)); 
		}
	      }
	      samedge.SetValue(ic,Standard_True);
	      p.SetValue(ic,icplus,cp1.ParameterOnArc());
	      p.SetValue(ic,icmoins,cp1.ParameterOnArc());  
	      i.SetValue(ic,icplus,1);	      
	    }
	  }
	}
	if (!sharp.Value(icmoins)) {
	  isfirst=(sens.Value(icmoins)==1);
	  cp2 = CD.Value(icmoins)->SetOfSurfData()->Value(Index.Value(icmoins))->
	    ChangeVertex (isfirst,jf.Value(icmoins));
	  if (cp2.IsOnArc()) {
            angedg=PI;
            ChFi3d_cherche_vertex(Arc,cp2.Arc(),Vcom,trouve);
            if (trouve) angedg=Abs(ChFi3d_AngleEdge(Vcom,Arc,cp2.Arc()));
	    if (!cp2.Arc().IsSame(Arc)&&Abs(angedg-PI)<0.01) {
	      Evive.SetValue(ic,cp2.Arc());
	      ChFi3d_edge_common_faces(myEFMap(cp2.Arc()),F1,F2);
	      if (!Fvive.Value(ic,icmoins).IsSame(F1) && !Fvive.Value(ic,icmoins).IsSame(F2)) {
	        if (Fvive.Value(ic,icplus).IsSame(F2))  {
		  Fvive.SetValue(ic,icmoins,F1);
		  numfa.SetValue(ic,icmoins,DStr.AddShape(F1));
                  Fvive.SetValue(icmoins,ic,F1);
		  numfa.SetValue(icmoins,ic,DStr.AddShape(F1)); 
                }
		else  {
		  Fvive.SetValue(ic,icmoins,F2);
		  numfa.SetValue(ic,icmoins,DStr.AddShape(F2));
                  Fvive.SetValue(icmoins,ic,F2);
		  numfa.SetValue(icmoins,ic,DStr.AddShape(F2)); 
		}
	      }
	      samedge.SetValue(ic,Standard_True);
	      p.SetValue(ic,icmoins,cp2.ParameterOnArc());
	      p.SetValue(ic,icplus,cp2.ParameterOnArc());
	      i.SetValue(ic,icmoins,1);   
	    }
	  }
	}     
      }
    } 

// on recupere la premiere arete libre si elle existe
  trouve=Standard_False;
  for (ic=0; ic<nedge&&!trouve;ic++) {  
    TopoDS_Edge ecom;
    ecom=TopoDS::Edge(Evive.Value(ic));
    if (ecom.IsSame(edgelibre1)||ecom.IsSame(edgelibre2)){ 
      libre.SetValue(ic,Standard_True);
      trouve=Standard_True;
    }
  }  

// determination de la distance de recul distmin a ne pas depasser
  Standard_Boolean distmini=Standard_False;
  gp_Pnt som=BRep_Tool::Pnt(V1),pic;  
  gp_Pnt2d p2;
  TopoDS_Edge edgemin;
  TopoDS_Vertex V,V2;
  Standard_Real dst,distmin;
  distmin=1.e30;
  for (ic=0;ic<nedge;ic++) {
    if (sharp.Value(ic))
      edgemin=TopoDS::Edge(Evive.Value(ic));
    else {
      if (sens.Value(ic)==1) 
	edgemin= CD.Value(ic)->Spine()->Edges(1);
      else  
	edgemin = CD.Value(ic)->Spine()->Edges(CD.Value(ic)->Spine()->NbEdges());
    }
    V=TopExp::FirstVertex(edgemin);
    V2=TopExp::LastVertex(edgemin);
    dst=(BRep_Tool::Pnt(V)).Distance(BRep_Tool::Pnt(V2))/1.5;
    if (dst<distmin) distmin=dst;
  }

//  calcul des intersections entre les stripes et determination des parametres
// sur chaque pcurve  
  Standard_Boolean inters=Standard_True;
  for (ic=0;ic<nedge;ic++) {
    Indices(nedge,ic,icplus,icmoins);
    if (sharp.Value(ic)||sharp.Value(icplus)) {
      oksea.SetValue(ic, Standard_False);
    }
    else {
      Standard_Integer jf1;
      Standard_Integer i1,i2;
      Standard_Real pa1,pa2;
      Standard_Boolean ok;
      Handle(ChFiDS_Stripe) strip;
      Standard_Real angedg;
      Standard_Integer iface;
      // si les deux aretes sont tangentes on ne tente pas
      // l'intersection (cts60046)
      angedg=Abs(ChFi3d_AngleEdge(V1,TopoDS::Edge(Evive.Value(ic)),TopoDS::Edge(Evive.Value(icplus))));
      if (Abs(angedg-PI)>0.01)
	ok = ChFi3d_SearchFD(DStr,CD.Value(ic),CD.Value(icplus),sens.Value(ic),sens.Value(icplus),
				  i1,i2,pa1,pa2,
				  Index.Value(ic),Index.Value(icplus),
				  face,sameside,jf1,jfp);
      else ok=Standard_False;
      // s'il y a une intersection on regarde si la surfdata ou il y a l'intersection
     // correspond a la premiere ou  a la derniere 
     // si ce n'est pas le cas on enleve de la SD les surfdata  
      
      if (ok) {
	if (i1!=Index.Value(ic) ){
          Standard_Integer ideb,ifin;
          strip=CD.Value(ic);
          if (sens.Value(ic)==1) {
	    ideb=Index.Value(ic);
	    ifin=i1-1;
          }
          else {
	    ifin=Index.Value(ic);
	    ideb=i1+1;
          }
          if (i1<Index.Value(ic)) {
	    for (nb=Index.Value(ic);nb>=i1;nb--) {
	      if ((3-jf1)==1) 
		iface=SurfIndex(CD, ic, nb , FACE1);
	      else    iface=SurfIndex(CD, ic, nb , FACE2);
	      Fproj.Append(TopoDS::Face(myDS->Shape(iface)));  
	    }
          }
          if (i1>Index.Value(ic)) {
	    for (nb=Index.Value(ic);nb<=i1;nb++) {
	      if ((3-jf1)==1) 
		iface=SurfIndex(CD, ic, nb , FACE1);
	      else    iface=SurfIndex(CD, ic, nb , FACE2);
               Fproj.Append(TopoDS::Face(myDS->Shape(iface)));  
	    }
          }    
          strip=CD.Value(ic);
	  RemoveSD(strip,ideb,ifin);
	  num=ChFi3d_IndexOfSurfData(V1,CD.Value(ic),sense);
	  Index.SetValue(ic,num);
	  i1=num; 
	}
	if (i2!=Index.Value(icplus) ){
          Standard_Integer ideb,ifin;
          strip=CD.Value(icplus);
          if (sens.Value(icplus)==1) {
	    ideb=Index.Value(icplus);
	    ifin=i2-1;
          }
          else {
	    ifin=Index.Value(icplus);
	    ideb=i2+1;
          }
	  
          if (i2<Index.Value(icplus)) {
	    for (nb=i2;nb<=Index.Value(icplus);nb++) {
	      if ((3-jfp)==1) 
		iface=SurfIndex(CD, icplus, nb , FACE1);
	      else    iface=SurfIndex(CD, icplus, nb , FACE2);
	      Fproj.Append(TopoDS::Face(myDS->Shape(iface)));  
	    }
          }
          if (i2>Index.Value(icplus)) {
	    for (nb=i2;nb>=Index.Value(icplus);nb--) {
	      if ((3-jfp)==1) 
		iface=SurfIndex(CD, icplus, nb , FACE1);
	      else    iface=SurfIndex(CD, icplus, nb , FACE2);
	      Fproj.Append(TopoDS::Face(myDS->Shape(iface)));  
	    }
          }    
	  RemoveSD(strip,ideb,ifin);
	  num=ChFi3d_IndexOfSurfData(V1,CD.Value(icplus),sense);
	  Index.SetValue(icplus,num);
	  i2=num; 
      }
	Calcul_P2dOnSurf(CD.Value(ic),jf1,i1,pa1,p2);
	indice=SurfIndex(CD, ic, i1, ChFiSURFACE);
	DStr.Surface(indice).Surface()->D0(p2.X(),p2.Y(),pic);
	if (pic.Distance(som)>distmin) distmini =Standard_True;
	jf.SetValue(ic,jf1);
	i.SetValue(ic,icplus,i1);
	i.SetValue(icplus,ic,i2);
	p.SetValue(ic,icplus,pa1);
	p.SetValue(icplus,ic,pa2);
      }
      oksea.SetValue(ic, ok);
    }
    if (!oksea.Value(ic) ) inters=Standard_False; 
  }

  // cas ou il n'y a pas que des intersections 
  // les parametres sur les Pcurves sont les extremites  de la stripe
  Standard_Real para;
  if (!inters) {
    for (ic=0;ic<nedge;ic++) {
      Indices(nedge,ic,icplus,icmoins);
      Indices(nedge,icplus,icplus2,ic);
      if (!oksea.Value(ic)) {
	cbplus++;
	if (sharp.Value(ic)) {
          if (!samedge.Value(ic)){
            para=BRep_Tool::Parameter(V1,TopoDS::Edge(Evive.Value(ic)));
	    p.SetValue(ic,icplus,para);
	    i.SetValue(ic,icplus,1);
          }
	}
	else {
	  isfirst= (sens.Value(ic)==1);
	  i.SetValue(ic,icplus,ChFi3d_IndexOfSurfData(V1,CD.Value(ic),sense));
	  if (oksea.Value(icmoins)) {
           para=p.Value(ic,icmoins);
	   p.SetValue(ic,icplus,para);
          }
	  else {
	    Calcul_Param(CD.Value(ic),jf.Value(ic),i.Value(ic,icplus),isfirst,para);
            p.SetValue(ic,icplus,para);
          }
   	}
	if (sharp.Value(icplus)) {
          if (!samedge.Value(icplus)) {
            para=BRep_Tool::Parameter(V1,TopoDS::Edge(Evive.Value(icplus)));
	    p.SetValue(icplus,ic, para);
	    i.SetValue(icplus,ic,1);
          }
	}
	else {
	  isfirst= (sens.Value(icplus)==1);
	  i.SetValue(icplus,ic,ChFi3d_IndexOfSurfData(V1,CD.Value(icplus),sense));
	  if (oksea.Value(icplus)){
          para=p.Value(icplus,icplus2);
	  p.SetValue(icplus,ic,para);
          }
	  else {
	    jfp = 3 - jf.Value(icplus);
	    Calcul_Param(CD.Value(icplus),jfp,i.Value(icplus,ic),isfirst,para);
            p.SetValue(icplus,ic,para);
	  }
   	}
      }
    }
      
//  calcul  de la distance max du sommet a chaque point  
    TColStd_Array1OfReal dist1(0,size);
    TColStd_Array1OfReal dist2(0,size);
    Standard_Real distance=0.;
    gp_Pnt sommet=BRep_Tool::Pnt(V1);
    if (!deuxconges)
    for (ic=0;ic<nedge;ic++) {
      Indices(nedge,ic,icplus,icmoins);
      if (sharp.Value(ic)) {
	dist1.SetValue(ic, 0);
	dist2.SetValue(ic, 0);
      }
      else {        
	jfp = 3 - jf.Value(ic);       
	Calcul_P2dOnSurf(CD.Value(ic),jfp,i.Value(ic,icmoins),p.Value(ic,icmoins),p2);
	indice=SurfIndex(CD, ic, i.Value(ic,icmoins), ChFiSURFACE);
	DStr.Surface(indice).Surface()->D0(p2.X(),p2.Y(),pic);
	dist1.SetValue(ic, sommet.Distance(pic));
	if (dist1.Value(ic) > distance ) distance= dist1.Value(ic);

	Calcul_P2dOnSurf(CD.Value(ic),jf.Value(ic),i.Value(ic,icplus),p.Value(ic,icplus),p2);
	indice=SurfIndex(CD, ic, i.Value(ic,icplus), ChFiSURFACE);
	DStr.Surface(indice).Surface()->D0(p2.X(),p2.Y(),pic);
	dist2.SetValue(ic, sommet.Distance(pic));
	if( dist2.Value(ic) > distance  )  
	  distance= dist2.Value(ic);
      }
    }

//  decalage des parametres  et elimination des points d'intersections 
//  trop proches du sommet 

    Standard_Real ec, dist; 
    if (!deuxconges && !deuxcgnontg)
    for (ic=0;ic<nedge;ic++) {
      Indices(nedge,ic,icplus,icmoins);
      if (sharp.Value(ic) ) {
        BRepAdaptor_Curve C(TopoDS::Edge(Evive.Value(ic)));
        // pour passer d'une distance 3d a une distance parametrique
	if (!tangentregul(ic))
	  ec = distance*100*C.Resolution(0.01);
        else ec=0.0;
	if (TopExp::FirstVertex(TopoDS::Edge(Evive.Value(ic))).IsSame(V1)) {
          para=p.Value(ic,icmoins) + ec;
	  p.SetValue(ic,icmoins, para);
        }
	else {
          para=p.Value(ic,icmoins) - ec;
	  p.SetValue(ic,icmoins,para);
        }
// il faudra etre sur de rester sur l'edge
	p.SetValue(ic,icplus, p.Value(ic,icmoins));
      }
      else if (!distmini) {
	dist = dist1.Value(ic);
	if ((!oksea.Value(icmoins))||(oksea.Value(icmoins)&&(distance>1.3*dist))) {
	  ec= distance-dist;
	  if (oksea.Value(icmoins)) {
	    oksea.SetValue(icmoins,Standard_False);
	    inters=Standard_False;
	    cbplus++;
	  }
	  if (sens.Value(ic)==1) {
            para=p.Value(ic,icmoins) + ec;
            p.SetValue(ic,icmoins, para);
          }
	  else{
             para=p.Value(ic,icmoins) - ec;
             p.SetValue(ic,icmoins,para);
          }
	}
	dist = dist2.Value(ic);
	if ((!oksea.Value(ic))||(oksea.Value(ic)&&(distance>1.3*dist))) {
	  if(oksea.Value(ic)) { 
	    oksea.SetValue(ic,Standard_False);
	    inters=Standard_False;
	    cbplus++;
	  }
	  if (nconges!=1) {
	    Standard_Real parold,parnew;
	    parold=p.Value(ic,icplus);
	    parnew=p.Value(ic,icmoins);
	    if (sens.Value(ic)==1) {
	      if (parnew> parold) p.SetValue(ic,icplus, p.Value(ic,icmoins));
	    }
	    else {
	      if (parnew<parold) p.SetValue(ic,icplus, p.Value(ic,icmoins));
	    }
	  }  
	}
      }
    }
  }

// on tente de limiter l'arete vive par un des commonpoint
//
 
  Standard_Real tolcp=0;
  gp_Pnt PE, sommet=BRep_Tool::Pnt(V1);
  if (!deuxconges)  
  for (ic=0;ic<nedge;ic++) {
    if (sharp.Value(ic)) {
      Indices(nedge,ic,icplus,icmoins);
      BRepAdaptor_Curve C(TopoDS::Edge(Evive.Value(ic)));
      PE = C.Value(p.Value(ic,icplus));
      Standard_Real d1=0., d2=0., dS = PE.Distance(sommet);
      ChFiDS_CommonPoint cp1, cp2;
      if (!sharp.Value(icplus)) {
	isfirst=(sens.Value(icplus)==1);
	jfp = 3 - jf.Value(icplus);
	cp1 = CD.Value(icplus)->SetOfSurfData()->Value(i.Value(icplus,ic))->
	  ChangeVertex (isfirst,jfp);
	d1 = cp1.Point().Distance(sommet);
      }
      if (!sharp.Value(icmoins)) {
	isfirst=(sens.Value(icmoins)==1);
	cp2 = CD.Value(icmoins)->SetOfSurfData()->Value(i.Value(icmoins,ic))->
	  ChangeVertex (isfirst,jf.Value(icmoins));
	d2 = cp2.Point().Distance(sommet);
      }
      Standard_Boolean samecompoint=Standard_False;
      if (!sharp.Value(icmoins) &&  !sharp.Value(icplus))
	samecompoint=cp1.Point().Distance(cp2.Point())<tolapp;
      if ((dS<d1 || dS<d2)&& !samecompoint) {
// on recule jusqu'aux Common Points
// sans sortir de l'Edge ??
	if (d2<d1 &&cp1.IsOnArc() ) {
// on choisit cp1
	  p.SetValue(ic,icmoins, cp1.ParameterOnArc());
	  p.SetValue(ic,icplus, p.Value(ic,icmoins));
	  isfirst=(sens.Value(icplus)==1);
	  jfp = 3 - jf.Value(icplus);
	  Calcul_Param(CD.Value(icplus),jfp,i.Value(icplus,ic),isfirst,para);
          p.SetValue(icplus,ic,para);
          if (cp1.Tolerance()>tolcp &&cp1.Tolerance()<1 ) tolcp=cp1.Tolerance();
	}
	else if( cp2.IsOnArc()){
// on choisit cp2
	  p.SetValue(ic,icmoins, cp2.ParameterOnArc());
	  p.SetValue(ic,icplus, p.Value(ic,icmoins));
	  isfirst=(sens.Value(icmoins)==1);
	  Calcul_Param(CD.Value(icmoins),jf.Value(icmoins),i.Value(icmoins,ic),isfirst, para);
          p.SetValue(icmoins,ic,para);
           if (cp2.Tolerance()>tolcp&&cp2.Tolerance()<1) tolcp=cp2.Tolerance();
	}
      }
      else {
// on se recale sur un Common Point uniquement si on en est tres pres
	if (!sharp.Value(icplus)) {
	  if ((cp1.Point().Distance(PE)<cp1.Tolerance() || 
               samecompoint || nconges==1) && cp1.IsOnArc()) {
// on est tres proche de cp1
	    p.SetValue(ic,icmoins, cp1.ParameterOnArc());
            ponctuel.SetValue(ic,Standard_True);
	    p.SetValue(ic,icplus, p.Value(ic,icmoins));
	    isfirst=(sens.Value(icplus)==1);
	    jfp = 3 - jf.Value(icplus);
	    Calcul_Param(CD.Value(icplus),jfp,i.Value(icplus,ic),isfirst,para);
            p.SetValue(icplus,ic,para);
            if (cp1.Tolerance()>tolcp &&cp1.Tolerance()<1) tolcp=cp1.Tolerance();
	  }
	}
	 if (!sharp.Value(icmoins)){
	  if ((cp2.Point().Distance(PE)<cp2.Tolerance() || 
	       samecompoint || nconges==1) && cp2.IsOnArc()) {
// on est tres proche de cp2
            ponctuel.SetValue(icmoins,Standard_True);
	    p.SetValue(ic,icmoins, cp2.ParameterOnArc());
	    p.SetValue(ic,icplus,p.Value(ic,icmoins));
	    isfirst=(sens.Value(icmoins)==1);
	    Calcul_Param(CD.Value(icmoins),jf.Value(icmoins),i.Value(icmoins,ic),isfirst,para);
            p.SetValue(icmoins,ic,para);
            if (cp2.Tolerance()>tolcp&&cp2.Tolerance()<1 ) tolcp=cp2.Tolerance();
	  }
	}
      }
    }
  }

// dans le cas d'un bord libre  on prend le parametre correspondant 
// au common point sur l'arete libre. 

  for (ic=0;ic<nedge;ic++) {
    if (TopoDS::Edge(Evive.Value(ic)).IsSame(edgelibre1) || 
	TopoDS::Edge(Evive.Value(ic)).IsSame(edgelibre2)) {
      Standard_Integer indic;
      ChFiDS_CommonPoint CP1;
      Indices(nedge,ic,icplus,icmoins);
      if (libre.Value(ic))indic=icmoins;
      else indic=icplus;
      if (!sharp(indic)) {
	isfirst=sens.Value(indic)==1;         
	CP1 = CD.Value(indic)->SetOfSurfData()->Value(Index.Value(indic))->ChangeVertex(isfirst,1);
	/*Standard_Boolean*/ trouve=Standard_False;
	if (CP1.IsOnArc()) {
	  if(CP1.Arc().IsSame(TopoDS::Edge(Evive.Value(ic)))) {
	    p.SetValue(ic,icmoins,CP1.ParameterOnArc());
            p.SetValue(ic,icplus,CP1.ParameterOnArc());
            trouve=Standard_True;
	  }  
	}
	if (!trouve) {
	  CP1 = CD.Value(indic)->SetOfSurfData()->Value(Index.Value(indic))->ChangeVertex(isfirst,2);
	  if (CP1.IsOnArc()) {
	    if(CP1.Arc().IsSame(TopoDS::Edge(Evive.Value(ic)))) {
	      p.SetValue(ic,icmoins,CP1.ParameterOnArc());
              p.SetValue(ic,icplus,CP1.ParameterOnArc());
	    }  
	  }
	}
      }
    }   
  }

// si ic est une arete de regularite, on cherche l'arete indfin qui ne 
// soit pas une arete de regularite, on construit une courbe 3d 
// entre les aretes (ou stripes ) icmoins et indfin. 
// On projette ensuite cette courbe3d sur toutes les faces (nbface) qui
// separent icmoins et indfin
#ifndef DEB
  Standard_Integer nbface = 0;
#else
  Standard_Integer nbface;
#endif
  Standard_Real  error;
  TColGeom2d_Array1OfCurve proj2d1(0,size);
  TColGeom2d_Array1OfCurve proj2d2(0,size);
  TColGeom_Array1OfCurve cproj1(0,size);
  TColGeom_Array1OfCurve cproj2(0,size);
  if (!deuxconges) 
  for (ic=0;ic<nedge;ic++) {
    Standard_Integer ilin;
    TColGeom_SequenceOfCurve cr;
    TColGeom2d_SequenceOfCurve pr;
    TopTools_SequenceOfShape Lface;
    TopTools_SequenceOfShape Ledge;
    Lface.Clear();
    if (regul.Value(ic)){
      Indices(nedge,ic,icplus,icmoins);
      Indices(nedge,icplus,icplus2,ic);
      Standard_Integer indfin,indfinmoins,indfinplus;
      indfin=icplus;
      trouve=Standard_False;
      ii=icplus;
      while (!trouve) { 
        if (!regul.Value(ii)) { 
	  indfin=ii;
	  trouve=Standard_True;
        }
        if (ii==nedge-1) ii=0;
        else ii++;
      } 
      Indices(nedge,indfin,indfinplus,indfinmoins);
      if (!sharp.Value(icmoins)){
	if( jf.Value(icmoins)==1)
          ilin= SurfIndex(CD, icmoins, i.Value(icmoins,ic), FACE1);
	else 
          ilin= SurfIndex(CD, icmoins, i.Value(icmoins,ic), FACE2);
	Lface.Append(TopoDS::Face(DStr.Shape(ilin)));
      }
      else Lface.Append( Fvive(ic,icmoins));
      if (indfin>icmoins) 
	nbface=indfin-icmoins;
      else nbface =nedge-(icmoins-indfin);
      TopTools_SequenceOfShape Epj;
      TColStd_SequenceOfReal  seqpr;
      ii=ic;
      for (Standard_Integer nf=1;nf<=nbface-1;nf++) { 
        Standard_Integer iimoins,iiplus;
        Indices(nedge,ii,iiplus,iimoins); 
        Ledge.Append(TopoDS::Edge(Evive.Value(ii)));
        seqpr.Append(p.Value(ii,iiplus));
        if (nf!=nbface-1) Lface.Append( Fvive(ii,iiplus));
        if (ii==nedge-1) ii=0;
        else ii++;
      }
      if (!sharp.Value(indfin) ){
	jfp=3-jf.Value(indfin);   
	if (jfp==1) 
	  ilin= SurfIndex(CD, indfin, i.Value(indfin,indfinmoins), FACE1);
	else  ilin=SurfIndex(CD, indfin, i.Value(indfin,indfinmoins), FACE2);
	Lface.Append(TopoDS::Face(DStr.Shape(ilin)));
      }
      else  Lface.Append(Fvive(indfin,indfinmoins));
      CurveHermite(DStr,CD.Value(icmoins),jf.Value(icmoins),i.Value(icmoins,ic),
		   p.Value(icmoins,ic),sens.Value(icmoins),sharp.Value(icmoins),
		   TopoDS::Edge(Evive.Value(icmoins)),CD.Value(indfin),jf.Value(indfin),
		   i.Value(indfin,indfinmoins),p.Value(indfin,indfinmoins),sens.Value(indfin),
		   sharp.Value(indfin),TopoDS::Edge(Evive.Value(indfin)),nbface,Ledge,
		   Lface,pr,cr,Epj,seqpr,error);
      ii=ic;
      for (ind=1;ind<=nbface-1;ind++) {
        Standard_Integer iimoins,iiplus;
        Indices(nedge,ii,iiplus,iimoins);
	p.SetValue(ii,iiplus,seqpr.Value(ind));
	p.SetValue(ii,iimoins,seqpr.Value(ind));
	proj2d1.SetValue(ii,pr.Value(ind));
	proj2d2.SetValue(ii,pr.Value(ind+1));
	cproj1.SetValue(ii,cr.Value(ind));
	cproj2.SetValue(ii,cr.Value(ind+1));
        if (ii==nedge-1) ii=0;
        else ii++;
      }
      if (!sharp.Value(icmoins)&&!sharp.Value(indfin)) {   
        ii=icmoins;
        while (ii!=indfin) {
         isG1.SetValue(ii,Standard_True);
         if (ii==nedge-1) ii=0;
         else ii++;
        }
      }
      ic=ic+nbface-1;
    }       
  }

 // cas ou  la courbe de raccord entre ic et icplus traverse plusieurs faces
  
  TopTools_SequenceOfShape Ecom;
  TopTools_SequenceOfShape Eproj;
  TColStd_SequenceOfReal parcom; 
  if (!deuxconges) 
  for (ic=0;ic<nedge;ic++) {
    Standard_Integer iface1,iface2;
    TopoDS_Face face1,face2;
    TopoDS_Edge edge;
    TColGeom_SequenceOfCurve cr;
    TColGeom2d_SequenceOfCurve pr;
    Indices(nedge,ic,icplus,icmoins);
    if (!oksea.Value(ic)){
      iface1=numfa.Value(ic,icplus);
      iface2=numfa.Value(icplus,ic);
       if (!sharp.Value(ic)) {
        if (jf.Value(ic)==1)
	  iface1 =SurfIndex(CD, ic, i.Value(ic,icplus), FACE1);
        else   iface1=SurfIndex(CD, ic, i.Value(ic,icplus), FACE2);
      }
      face1=TopoDS::Face(myDS->Shape(iface1));
      
      if (!sharp.Value(icplus)) {
	if (jf.Value(icplus)==1)
	  iface2 =SurfIndex(CD, icplus, i.Value(icplus,ic), FACE2);
	else   iface2=SurfIndex(CD, icplus, i.Value(icplus,ic), FACE1);
      }
      face2=TopoDS::Face(myDS->Shape(iface2));
      if (!face1.IsSame(face2)) {
	if (Fproj.Length()==0) {
	  Fproj.Append(face1);
	  Fproj.Append(face2); 
	}
	moresurf.SetValue(ic,Standard_True);	
	nbface=Fproj.Length();
	if (!TopoDS::Face(Fproj.Value(nbface)).IsSame(face2)) {
	  Fproj.Remove(nbface); 
	  Fproj.Append(face2);   
	}
	if (!TopoDS::Face(Fproj.Value(1)).IsSame(face1)) {
	  Fproj.Remove(1); 
	  Fproj.Prepend(face1);   
	}
	for (nb=1;nb<=nbface-1; nb++) {
          cherche_edge1 ( TopoDS::Face(Fproj.Value(nb)), TopoDS::Face(Fproj.Value(nb+1)),edge);
          Ecom.Append(edge); 
	  para=BRep_Tool::Parameter(TopExp::FirstVertex(edge),edge);
	  parcom.Append(para);   
	}      
	CurveHermite (DStr,CD.Value(ic),jf.Value(ic),i.Value(ic,icplus),
		    p.Value(ic,icplus),sens.Value(ic),sharp.Value(ic),
                    TopoDS::Edge(Evive.Value(ic)),
	            CD.Value(icplus),jf.Value(icplus),i.Value(icplus,ic),
                    p.Value(icplus,ic),sens.Value(icplus),sharp.Value(icplus),
                    TopoDS::Edge(Evive.Value(icplus)),nbface,Ecom,Fproj, pr,cr,Eproj,parcom,error);
	Ecom.Append(Ecom.Value(nbface-1)); 
	parcom.Append(parcom.Value(nbface-1));
      }
    }
  } 

// cas ou deux conges ont les memes commonpoints 
// on procede alors par intersection 
// on verifie cependant que les extremites de l'intersection coincident avec les commonpoints

  Standard_Boolean intersection=Standard_False, introuve;
  if (nconges==2 && !deuxconges) { 
    gp_Pnt P1,P2,P3,P4;
#ifndef DEB
    Standard_Integer ic1 = 0,ic2 = 0;
#else
    Standard_Integer ic1,ic2;
#endif
    trouve=Standard_False;
    for (ic=0;ic<nedge&&!trouve;ic++) {
      if (!sharp.Value(ic)){ 
	ic1=ic;
	trouve=Standard_True;
      }
    }
    for (ic=0;ic<nedge;ic++) {
      if (!sharp.Value(ic)&& ic!=ic1) ic2=ic;   
    }
    jfp = 3 - jf.Value(ic1);
    Indices(nedge,ic1,icplus,icmoins);   
    Calcul_P2dOnSurf(CD.Value(ic1),jfp,i.Value(ic1,icmoins),p.Value(ic1,icmoins),p2);
    indice=SurfIndex(CD, ic1, i.Value(ic1,icmoins), ChFiSURFACE);
    DStr.Surface(indice).Surface()->D0(p2.X(),p2.Y(),P1);
    
    Calcul_P2dOnSurf(CD.Value(ic1),jf.Value(ic1),i.Value(ic1,icplus),p.Value(ic1,icplus),p2);
    indice=SurfIndex(CD, ic1, i.Value(ic1,icplus), ChFiSURFACE);
    DStr.Surface(indice).Surface()->D0(p2.X(),p2.Y(),P2);
    
    jfp = 3 - jf.Value(ic2);    
    Indices(nedge,ic2,icplus,icmoins);   
    Calcul_P2dOnSurf(CD.Value(ic2),jfp,i.Value(ic2,icmoins),p.Value(ic2,icmoins),p2);
    indice=SurfIndex(CD, ic2, i.Value(ic2,icmoins), ChFiSURFACE);
    DStr.Surface(indice).Surface()->D0(p2.X(),p2.Y(),P3);
    
    Calcul_P2dOnSurf(CD.Value(ic2),jf.Value(ic2),i.Value(ic2,icplus),p.Value(ic2,icplus),p2);
    indice=SurfIndex(CD, ic2, i.Value(ic2,icplus), ChFiSURFACE);
    DStr.Surface(indice).Surface()->D0(p2.X(),p2.Y(),P4);
    intersection=(P1.Distance(P4)<=1.e-7 ||  P1.Distance(P3)<=1.e-7) &&
      (P2.Distance(P4)<=1.e-7 ||  P2.Distance(P3)<=1.e-7);
    if (intersection) { 
      PerformTwoCornerSameExt(DStr,CD.Value(ic1),Index.Value(ic1),sens.Value(ic1),
			      CD.Value(ic2),Index.Value(ic2),sens.Value(ic2),introuve);      
      if (introuve) return;
    }
  } 

// declaration pour plate 
  GeomPlate_BuildPlateSurface PSurf(3,10,3,tol2d,tolesp,angular);

// calcul des courbes sur surface  pour chaque   stripe 
  for (ic=0;ic<nedge;ic++) {
    gp_Pnt2d p2d1, p2d2;
    if (!sharp.Value(ic)) {
      n3d++;
      Indices(nedge,ic,icplus,icmoins);
      jfp = 3 - jf.Value(ic);
      Calcul_P2dOnSurf(CD.Value(ic),jfp,i.Value(ic,icmoins),p.Value(ic,icmoins),p2d1);
      Calcul_P2dOnSurf(CD.Value(ic),jf.Value(ic),i.Value(ic,icplus),p.Value(ic,icplus),p2d2);
//      if (i[ic][icplus]!=  i[ic][icmoins]) cout<<"probleme surface"<<endl;
      indice= SurfIndex(CD, ic, i.Value(ic,icplus), ChFiSURFACE);
      Handle (GeomAdaptor_HSurface) Asurf =
	new GeomAdaptor_HSurface(DStr.Surface(indice).Surface());
      // calcul de la courbe 2d  
      xdir= p2d2.X()-p2d1.X();  
      ydir= p2d2.Y()-p2d1.Y();
      Standard_Real l0 = sqrt(xdir*xdir+ ydir*ydir );
      gp_Dir2d dir (xdir, ydir);
      Handle(Geom2d_Line) l= new Geom2d_Line (p2d1 ,dir);
      Handle (Geom2d_Curve) pcurve = new  Geom2d_TrimmedCurve(l,0,l0); 
      Handle (Geom2dAdaptor_HCurve) Acurv = new Geom2dAdaptor_HCurve(pcurve);
      Adaptor3d_CurveOnSurface  CurvOnS (Acurv,Asurf);
      Handle(Adaptor3d_HCurveOnSurface) HCons =
	new Adaptor3d_HCurveOnSurface(CurvOnS);
      Order.SetValue(ic,1);
      Handle(GeomPlate_CurveConstraint) Cont = 
	new GeomPlate_CurveConstraint(HCons,Order.Value(ic),10,tolesp,angular,0.1);
      PSurf.Add(Cont);
      
      // calcul des indices de points  et de  la courbe pour la DS         
      isfirst=(sens.Value(ic)==1);
      GeomLib::BuildCurve3d(tolapp,CurvOnS,CurvOnS.FirstParameter(),
			    CurvOnS.LastParameter(),Curv3d,maxapp,avedev);
      TopOpeBRepDS_Curve tcurv3d( Curv3d,maxapp);
      indcurve3d.SetValue(n3d,DStr.AddCurve(tcurv3d));
      gp_Pnt point1,point2; 
      point1=  CurvOnS.Value(CurvOnS.FirstParameter());
      point2 =CurvOnS.Value(CurvOnS.LastParameter());
      
      TopOpeBRepDS_Point tpoint1 (point1,maxapp);
      TopOpeBRepDS_Point tpoint2 (point2,maxapp);
      errapp.SetValue(ic,maxapp);
      if (ic==0) {
// il faut creer les deux points
	indpoint.SetValue(ic,0,DStr.AddPoint(tpoint1));
	indpoint.SetValue(ic,1,DStr.AddPoint(tpoint2));
      }
      else {
// les points existent peut-etre deja sur un conge 
// (intersection precedente,...)
	trouve = Standard_False;
	for (ii=0;ii<ic&&(!trouve);ii++) {
	  if (!sharp.Value(ii)) {
	    TopOpeBRepDS_Point & tpt= DStr.ChangePoint(indpoint.Value(ii,1));
	    if (point1.Distance(tpt.Point())<1.e-4)
	      trouve = Standard_True;
	  }
	}
	if (trouve)
	  indpoint.SetValue(ic,0,indpoint.Value(ii-1,1));
	else
	  indpoint.SetValue(ic,0,DStr.AddPoint(tpoint1));

	trouve = Standard_False;
	for (ii=0;ii<ic&&(!trouve);ii++) {
	  if (!sharp.Value(ii)) {
	    TopOpeBRepDS_Point & tpt= DStr.ChangePoint(indpoint.Value(ii,0));
	    if (point2.Distance(tpt.Point())<1.e-4)
	      trouve = Standard_True;
	  }
	}
	if (trouve)
	  indpoint.SetValue(ic,1,indpoint.Value(ii-1,0));
	else
	  indpoint.SetValue(ic,1,DStr.AddPoint(tpoint2));
      }
      
      //   mise a jour de la stripe 
      isurf1=3-jf.Value(ic); isurf2=jf.Value(ic);
      if (isurf1==2)  CD.Value(ic)->SetOrientation(TopAbs_REVERSED,isfirst);
      CD.Value(ic)->SetCurve(indcurve3d.Value(n3d),isfirst);
      CD.Value(ic)->SetIndexPoint(indpoint.Value(ic,0),isfirst,isurf1); 
      CD.Value(ic)->SetIndexPoint(indpoint.Value(ic,1),isfirst,isurf2);
      CD.Value(ic)->SetParameters(isfirst,pcurve->FirstParameter(),pcurve->LastParameter());     
      ChFiDS_CommonPoint cp1;
      ChFiDS_CommonPoint cp2;
      cp1.SetPoint (point1);
      cp2.SetPoint( point2);
      CD.Value(ic)->SetOfSurfData()->Value(i.Value(ic,icmoins))->
	ChangeVertex (isfirst,isurf1)=cp1;
      CD.Value(ic)->SetOfSurfData()->Value(i.Value(ic,icmoins))->
	ChangeVertex (isfirst,isurf2)=cp2; 
      CD.Value(ic)->SetOfSurfData()->Value(i.Value(ic,icmoins))->
	ChangeInterference(isurf1).SetParameter(p.Value(ic,icmoins),isfirst);
      CD.Value(ic)->SetOfSurfData()->Value(i.Value(ic,icmoins))->
	ChangeInterference(isurf2).SetParameter(p.Value(ic,icplus),isfirst);
      CD.Value(ic)-> ChangePCurve(isfirst)= pcurve;
    }
  }
      
// calcul des indices de points pour les aretes vives   
  for (ic=0;ic<nedge;ic++) {
    if (sharp.Value(ic)) {
      Indices(nedge,ic,icplus,icmoins);
      BRepAdaptor_Curve C(TopoDS::Edge(Evive.Value(ic)));
      /*gp_Pnt*/ PE = C.Value(p.Value(ic,icplus));
      TopOpeBRepDS_Point TPE(PE,BRep_Tool::Tolerance(TopoDS::Edge(Evive.Value(ic))));
      ChFiDS_CommonPoint cp;
      if (deuxconges ) {
        IVtx = DStr.AddShape(V1);
        indpoint.SetValue(ic,0, IVtx );
        indpoint.SetValue(ic,1, IVtx );
      }
      if (!sharp.Value(icplus)) {
	isfirst=(sens.Value(icplus)==1);
	jfp = 3 - jf.Value(icplus);
	cp = CD.Value(icplus)->SetOfSurfData()->Value(i.Value(icplus,ic))->
	  ChangeVertex (isfirst,jfp);
	if ( cp.Point().Distance(PE) <= Max(1.e-4,tolcp)) {
// l'arete a ete limitee par le 1er CommonPoint de CD[icplus]
	  indpoint.SetValue(ic,0,indpoint.Value(icplus,0));
	  indpoint.SetValue(ic,1,indpoint.Value(icplus,0));
	}
      }
      if (!sharp.Value(icmoins)) {
	isfirst=(sens.Value(icmoins)==1);
	cp = CD.Value(icmoins)->SetOfSurfData()->Value(i.Value(icmoins,ic))->
	  ChangeVertex (isfirst,jf.Value(icmoins));
	if ( cp.Point().Distance(PE) <= Max(1.e-4,tolcp)) {
// l'arete a ete limitee par le 2eme CommonPoint de CD[icmoins]
	 if (indpoint.Value(ic,0)==0) { 
          indpoint.SetValue(ic,0, indpoint.Value(icmoins,1));
	  indpoint.SetValue(ic,1, indpoint.Value(icmoins,1));
         }
	}
      }
      if (indpoint.Value(ic,0)==0) { 
         indpoint.SetValue(ic,0,DStr.AddPoint(TPE));
         indpoint.SetValue(ic,1, indpoint.Value(ic,0));
      }
    }
  }

 // calcul des courbes intermediaires reliant  deux stripes dans le cas ou il n'y a pas 
// d'intersection. La courbe peut etre une droite une projection ou un batten 
  
  Standard_Boolean raccordbatten;
  if (!inters) {

    for (ic=0;ic<nedge;ic++) {
       
      if (!oksea.Value(ic)&& !moresurf.Value(ic) && !libre.Value(ic) ) {
	Indices(nedge,ic,icplus,icmoins);
        raccordbatten=Standard_False;
        if (!regul.Value(ic)) {
          raccordbatten=Standard_True;
          if (regul.Value(icplus))
          raccordbatten=Standard_False;
        }
	n3d++;
	gp_Pnt2d p2d1, p2d2;
	Handle(Geom2d_Curve) curv2d1,curv2d2;
	Handle (Geom2d_Curve) pcurve;
        Handle (Geom_Curve) curveint;
	Handle (GeomAdaptor_HSurface) Asurf;
	Standard_Real u1bid,u2bid;
	
	// recuperation de la premiere courbe 2d 
	// et du premier point de raccordement
	if (sharp.Value(ic))
	  curv2d1 = BRep_Tool::CurveOnSurface(TopoDS::Edge(Evive.Value(ic)),TopoDS::Face(Fvive.Value(ic,icplus)),
                                              u1bid,u2bid);
	else
	  Calcul_C2dOnFace(CD.Value(ic),jf.Value(ic),i.Value(ic,icplus),curv2d1);
	p2d1 = curv2d1 ->Value(p.Value(ic,icplus));
	
	// recuperation de la deuxieme courbe 2d
	// et du deuxieme point de raccordement
	if (sharp.Value(icplus))
	  curv2d2 = BRep_Tool::CurveOnSurface(TopoDS::Edge(Evive.Value(icplus)),
                                              TopoDS::Face(Fvive.Value(ic,icplus)),u1bid,u2bid);
	else {
	  jfp = 3 - jf.Value(icplus);
	  Calcul_C2dOnFace(CD.Value(icplus),jfp,i.Value(icplus,ic),curv2d2);
	}
	p2d2 = curv2d2 ->Value(p.Value(icplus,ic));

	Asurf = new GeomAdaptor_HSurface(BRep_Tool::Surface(TopoDS::Face(Fvive.Value(ic,icplus))));
	Standard_Real tolu,tolv,ratio; 
	tolu=Asurf->Surface().UResolution(1.e-3);
	tolv=Asurf->Surface().VResolution(1.e-3);
	if (tolu>tolv) ratio=tolu/tolv;
	else ratio=tolv/tolu;
        
        // dans le cas d'une arete de couture on recadre les parametres 
        if (couture) {
	  Standard_Boolean PI1=Standard_False, PI2=Standard_False;
	  Standard_Real xx;
	  PI1=0<=p2d1.X() && p2d1.X() <=PI;
	  PI2=0<=p2d2.X() && p2d2.X() <=PI;
	  
	  if (Evive.Value(ic).IsSame(edgecouture)){
	    xx=p2d1.X();
	    if (PI2&&!PI1) xx=xx-2*PI;
	    if (!PI2&&PI1) xx=xx+2*PI;
	    p2d1.SetX(xx);
              
	  }
	  if (Evive.Value(icplus).IsSame(edgecouture)){
	    xx=p2d2.X();
	    if (PI2&&!PI1) xx=xx+2*PI;
	    if (!PI2&&PI1) xx=xx-2*PI;
	    p2d2.SetX(xx); 
	  }
        }
        xdir= p2d2.X()-p2d1.X();
	ydir= p2d2.Y()-p2d1.Y();

	Standard_Real l0 = sqrt(xdir*xdir+ ydir*ydir );
	if (l0<1.e-7|| ponctuel.Value(ic)) {
// raccord inutile
	  n3d--;
          ponctuel.SetValue(ic,Standard_True);
          if (!deuxconges) {
	    if ( sharp.Value(icplus) && indpoint.Value(icplus,0) == 0 ) {
	      indpoint.SetValue(icplus,0, indpoint.Value(ic,1));
	      indpoint.SetValue(icplus,1, indpoint.Value(ic,1));
	    }
	    if ( sharp.Value(ic) && indpoint.Value(ic,0) == 0 ) {
	      indpoint.SetValue(ic,0,indpoint.Value(icmoins,1));
	      indpoint.SetValue(ic,1,indpoint.Value(icmoins,1));
	   }
          }
	}
	else {  // le raccord est soit une droite soit une projection soit un batten 
         if (ratio>10 && nconges==1) raccordbatten=Standard_True; 
         if (ratio>10 && raccordbatten) {
	    CalculDroite(p2d1,xdir,ydir,pcurve);
            raccordbatten=Standard_False;
          }
          else  if (!raccordbatten){  // on recupere les courbes projetees 
	    if (regul.Value(ic)) {
               if (cproj2.Value(ic).IsNull()){ 
                   raccordbatten=Standard_True;
               }
               else {
                pcurve=proj2d2.Value(ic);
                curveint=cproj2.Value(ic);
                maxapp1=1.e-6;
               }
               
            }
	    else  {
              if (cproj1.Value(ic+1).IsNull()) {
                  raccordbatten=Standard_True; 
              }
              else {
                pcurve=proj2d1.Value(ic+1);
                curveint=cproj1.Value(ic+1); 
                maxapp1=1.e-6;
              }
            }
          }
          Standard_Boolean contraint1=Standard_True,
                            contraint2=Standard_True;
          if (raccordbatten) {
#ifdef DEB
	    ChFi3d_InitChron(ch);// init performances pour les batten  
#endif  
            Standard_Boolean inverseic,inverseicplus;
            if (sharp.Value(ic)) {
                 inverseic=TopExp::FirstVertex(TopoDS::Edge(Evive.Value(ic))).
                         IsSame(V1);   
            }
            else {
              inverseic=sens.Value(ic)==1;
            }
            if (sharp.Value(icplus)){
              inverseicplus=TopExp::FirstVertex(TopoDS::Edge(Evive.Value(icplus))).
                         IsSame(V1);
            }
            else {
              inverseicplus=sens.Value(icplus)==1;
            } 
            if (TopoDS::Edge(Evive.Value(ic)).IsSame(edgelibre1) ||
                TopoDS::Edge(Evive.Value(ic)).IsSame(edgelibre2)) 
                contraint1=Standard_False;
            if (TopoDS::Edge(Evive.Value(icplus)).IsSame(edgelibre1) ||
                TopoDS::Edge(Evive.Value(icplus)).IsSame(edgelibre2))
            contraint2=Standard_False;
	    CalculBatten(Asurf,TopoDS::Face(Fvive(ic,icplus)),xdir,ydir,p2d1,p2d2,contraint1,contraint2,curv2d1,curv2d2,p.Value(ic,icplus),
			 p.Value(icplus,ic),inverseic,inverseicplus,pcurve);
#ifdef DEB
	    ChFi3d_ResultChron( ch,t_batten);  // result performances pour les batten 
#endif 
          }

         // construction des frontieres pour Plate 
         Handle (Geom2dAdaptor_HCurve)  Acurv=new Geom2dAdaptor_HCurve(pcurve);
         Adaptor3d_CurveOnSurface  CurvOnS (Acurv,Asurf);
         Handle(Adaptor3d_HCurveOnSurface) HCons =
           new Adaptor3d_HCurveOnSurface(CurvOnS);

         // on met des contraintes G1  si les aretes ic et icplus ne sont pas toutes les deux vives 


         Order.SetValue(n3d,0);
         if (!sharp.Value(ic)&& !sharp.Value(icplus))
           Order.SetValue(n3d,1);
         if (!contraint1 && !sharp.Value(icplus))
           Order.SetValue(n3d,1);
         if (!contraint2 && !sharp.Value(ic))
           Order.SetValue(n3d,1);
         if (tangentregul(ic) || tangentregul(icplus) )
           Order.SetValue(n3d,1);
         if (isG1.Value(ic)) 
           Order.SetValue(n3d,1);
         Handle(GeomPlate_CurveConstraint) Cont =
           new GeomPlate_CurveConstraint(HCons,Order.Value(n3d),10,tolesp,angular,0.1);
         PSurf.Add(Cont);

         //calcul de la courbe 3d si ce n'est pas une projection 
         if (curveint.IsNull()) {
           GeomLib::BuildCurve3d(tolapp,CurvOnS,CurvOnS.FirstParameter(),
                                 CurvOnS.LastParameter(),Curv3d,maxapp1,avedev);
           pardeb=CurvOnS.FirstParameter();
           parfin= CurvOnS.LastParameter();
           curveint= new Geom_TrimmedCurve(Curv3d,pardeb,parfin);
         }

         //stockage dans la DS  
         TopOpeBRepDS_Curve tcurv3d( curveint,maxapp1);
         indcurve3d.SetValue(n3d, DStr.AddCurve(tcurv3d));
         pardeb=curveint->FirstParameter();
         parfin=curveint->LastParameter();
         if ( sharp.Value(icplus) && indpoint.Value(icplus,0) == 0) {
           // il faut initialiser indpoint[icplus][0] et indpoint[icplus][1]
           gp_Pnt point2; 
           point2 =curveint->Value(parfin);
           TopOpeBRepDS_Point tpoint2 (point2,maxapp); 
           indpoint.SetValue(icplus,0,DStr.AddPoint(tpoint2));
           indpoint.SetValue(icplus,1,indpoint.Value(icplus,0));
         }
         Standard_Boolean IsVt1=Standard_False;
         Standard_Boolean IsVt2=Standard_False;
         if(deuxconges) {
           IsVt1=sharp.Value(ic);
           IsVt2=sharp.Value(icplus);
          }
	  Interfp1=ChFi3d_FilPointInDS(TopAbs_FORWARD,indcurve3d.Value(n3d),
				       indpoint.Value(ic,1),pardeb,IsVt1);
	  Interfp2=ChFi3d_FilPointInDS(TopAbs_REVERSED,indcurve3d.Value(n3d),
				       indpoint(icplus,0),parfin,IsVt2);
	  DStr.ChangeCurveInterferences(indcurve3d.Value(n3d)).Append(Interfp1);
	  DStr.ChangeCurveInterferences(indcurve3d.Value(n3d)).Append(Interfp2);
	  if (!IsVt1) {
	    TopOpeBRepDS_Point & tpt1= DStr.ChangePoint(indpoint(ic,1));
            tpt1.Tolerance (tpt1.Tolerance()+maxapp1);
          }	  
          if (!IsVt2) {
            TopOpeBRepDS_Point &tpt2= DStr.ChangePoint(indpoint(icplus,0));
	    tpt2.Tolerance (tpt2.Tolerance()+maxapp1);     
	  }

	// calcul de l'orientation de la courbe  
	  TopAbs_Orientation orinterf; 
	  if (!sharp.Value(ic)) {
              OrientationIcNonVive(CD.Value(ic),jf.Value(ic),i.Value(ic,icplus),sens.Value(ic),orinterf);
	  }
	  else if (!sharp.Value(icplus)) {
	      OrientationIcplusNonVive(CD.Value(icplus),jf.Value(icplus),i.Value(icplus,ic),sens.Value(icplus),orinterf);
	    }
	  else {
                OrientationAreteViveConsecutive (Fvive.Value(ic,icplus) ,Evive.Value(ic),V1,orinterf);                                      
	  } 
	  Interfc=ChFi3d_FilCurveInDS(indcurve3d.Value(n3d),numfa.Value(ic,icplus),pcurve,orinterf);	  
	  DStr.ChangeShapeInterferences(numfa.Value(ic,icplus)).Append(Interfc);
	}
       } // fin du traitement par arete
    } // fin de la boucle sur les aretes
  }  // fin du traitement pour les courbes intermediaires 
        
//  stockage dans la DS des courbes projetees sur plusieurs faces 
  for (ic=0;ic<nedge;ic++) {
    if (moresurf.Value(ic) ){
      TopoDS_Vertex Vf,Vl;
      gp_Pnt Pf,Pl,P1,P2,Pcom;
      ind = 0; //must be initialized because of possible use, see L2249
      Standard_Real up1,up2;
      TopAbs_Orientation orvt;
      TopAbs_Orientation oredge = TopAbs_FORWARD;
      Standard_Integer indpoint1,indpoint2;
      Indices(nedge,ic,icplus,icmoins);
      Handle(Geom2d_Curve) proj,proj2d;
      Handle(Geom_Curve) projc,cproj;
      TopOpeBRepDS_Point& tpt1= DStr.ChangePoint(indpoint(ic,1));
      TopOpeBRepDS_Point& tpt2= DStr.ChangePoint(indpoint(icplus,0));
      tpt1.Tolerance (tpt1.Tolerance()+error);
      tpt2.Tolerance (tpt1.Tolerance()+error);
      for(nb=1;nb<=nbface;nb++) {
	orvt=TopAbs_REVERSED;
	Vf=TopExp::FirstVertex(TopoDS::Edge(Ecom.Value(nb)));
	Vl=TopExp::LastVertex (TopoDS::Edge(Ecom.Value(nb)));
	Pf=BRep_Tool::Pnt(Vf);
	Pl=BRep_Tool::Pnt(Vl);
	para=parcom.Value(nb);
	Pcom=BRep_Tool::Curve(TopoDS::Edge(Ecom.Value(nb)),up1,up2)->Value(para);
	if (Pf.Distance(BRep_Tool::Pnt(V1))< Pl.Distance(BRep_Tool::Pnt(V1)))
	  orvt=TopAbs_FORWARD;
	if (!Eproj.Value(nb).IsNull())  {
	  n3d++;
	  proj=BRep_Tool::CurveOnSurface(TopoDS::Edge(Eproj.Value(nb)),
					 TopoDS::Face(Fproj.Value(nb)),up1,up2);
	  proj2d=new Geom2d_TrimmedCurve(proj,up1,up2);
	  projc=BRep_Tool::Curve(TopoDS::Edge(Eproj.Value(nb)),up1,up2);
	  cproj=new Geom_TrimmedCurve(projc,up1,up2);
	  pardeb=cproj->FirstParameter();
	  parfin=cproj->LastParameter();
	  P1=cproj->Value(pardeb);
	  P2=cproj->Value(parfin);
	  if (P1.Distance(tpt1.Point())<1.e-3) 
	    indpoint1=indpoint(ic,1); 
	  else  indpoint1=ind;
	  if (P2.Distance(tpt2.Point())<1.e-3) 
	    indpoint2=indpoint(icplus,0); 
	  else  {
	    TopOpeBRepDS_Point tpoint2 (P2,error);
	    indpoint2= DStr.AddPoint(tpoint2);
	    ind=indpoint2;    
	  }
	  Handle (GeomAdaptor_HSurface) Asurf;
	  Asurf = new GeomAdaptor_HSurface(BRep_Tool::Surface
					   (TopoDS::Face(Fproj.Value(nb))));
	  Handle (Geom2dAdaptor_HCurve)  Acurv=new Geom2dAdaptor_HCurve(proj2d);
	  Adaptor3d_CurveOnSurface  CurvOnS (Acurv,Asurf);
	  Handle(Adaptor3d_HCurveOnSurface) HCons =new Adaptor3d_HCurveOnSurface(CurvOnS);
	  Order.SetValue(n3d,1);
	  Handle(GeomPlate_CurveConstraint) Cont =
	    new GeomPlate_CurveConstraint(HCons,Order.Value(n3d),10,tolesp,angular,0.1);
	  PSurf.Add(Cont);
	  TopOpeBRepDS_Curve tcurv3d( cproj,error);
	  indcurve3d.SetValue(n3d, DStr.AddCurve(tcurv3d));
	  Interfp1=ChFi3d_FilPointInDS(TopAbs_FORWARD,indcurve3d.Value(n3d),
				       indpoint1,pardeb);
	  Interfp2=ChFi3d_FilPointInDS(TopAbs_REVERSED,indcurve3d.Value(n3d),
				       indpoint2,parfin);
	  DStr.ChangeCurveInterferences(indcurve3d.Value(n3d)).Append(Interfp1);
	  DStr.ChangeCurveInterferences(indcurve3d.Value(n3d)).Append(Interfp2);
	  num=DStr.AddShape(Fproj.Value(nb));
	  TopExp_Explorer ex;
	  for(ex.Init(Fproj.Value(nb).Oriented(TopAbs_FORWARD),TopAbs_EDGE); 
	      ex.More(); ex.Next()){
	    if(Ecom.Value(nb).IsSame(ex.Current())) {
	      oredge = ex.Current().Orientation();
	      break;
	    }
	  }

	  //calcul de l'orientation 	  
	  TopAbs_Orientation orinterf;
	  if (P1.Distance(Pcom)>1.e-4) {
	    if (orvt==TopAbs_FORWARD) {
	      orinterf=oredge;
	    }
	    else {
	      orinterf=TopAbs::Reverse(oredge);
	    }
	  }
	  else {
	    if (orvt==TopAbs_FORWARD) {
	      orinterf=TopAbs::Reverse(oredge); 
	    }
	    else {
	      orinterf=oredge;
	    }
	  }
	  Interfc=ChFi3d_FilCurveInDS(indcurve3d.Value(n3d),num,proj2d,orinterf);	  
	  DStr.ChangeShapeInterferences(num).Append(Interfc);
	}
	indice=ind;
	if (nb!=nbface) {    
	  if (Eproj.Value(nb).IsNull())  indice=indpoint(ic,1);
	  if (Eproj.Value(nb+1).IsNull()) indice=indpoint(icplus,0); 
	  Indice.SetValue(n3d,indice);
	  Standard_Integer Iarc1=DStr.AddShape(TopoDS::Edge(Ecom.Value(nb)));
	  Interfp1=ChFi3d_FilPointInDS(orvt,Iarc1,indice,parcom.Value(nb));
	  DStr.ChangeShapeInterferences(Iarc1).Append(Interfp1);
	}
      }
    }                     
  }
            
// cas ou les deux bords libres sont tangents 
  if (droit)      
    for (ic=0;ic<nedge;ic++) {
      Handle(Geom_Curve) curve,ctrim,rcurve;
      Handle(Geom2d_Curve) curve2d,ctrim2d,rcurve2d;
      Standard_Real ufirst,ulast;  
      Indices(nedge,ic,icplus,icmoins);
      Standard_Integer indpoint1,indpoint2;
      Standard_Boolean isvt1=Standard_False,isvt2=Standard_False;
      TopoDS_Edge ecur =TopoDS::Edge(Evive.Value(ic));
      if (ecur.IsSame(edgelibre1)|| ecur.IsSame(edgelibre2)) {
	n3d++;
	curve2d=BRep_Tool::CurveOnSurface(TopoDS::Edge(Evive.Value(ic)),
					  TopoDS::Face(Fvive.Value(ic,icplus)),ufirst,ulast);
	curve=BRep_Tool::Curve(TopoDS::Edge(Evive.Value(ic)),ufirst,ulast);
	if (TopExp::FirstVertex((TopoDS::Edge(Evive.Value(ic)))).IsSame (V1)) { 
	  ctrim=new Geom_TrimmedCurve(curve,ufirst,p.Value(ic,icmoins));
	  ctrim2d=new Geom2d_TrimmedCurve(curve2d,ufirst,p.Value(ic,icmoins)); 
	  indpoint1=DStr.AddShape(V1);
	  isvt1=1;
	  indpoint2=indpoint(ic,1);
	}
	else { 
	  ctrim=new Geom_TrimmedCurve(curve, p.Value(ic,icmoins),ulast);
	  ctrim2d=new Geom2d_TrimmedCurve(curve2d,p.Value(ic,icmoins),ulast);
	  indpoint2=DStr.AddShape(V1);
	  isvt2=1;
	  indpoint1=indpoint(ic,1);
	}
	if (libre.Value(ic)){
	  if (TopExp::FirstVertex((TopoDS::Edge(Evive.Value(ic)))).IsSame(V1)) { 
	    ctrim->Reverse();
	    ctrim2d->Reverse();
	    indpoint2=DStr.AddShape(V1);
	    isvt2=1;
	    isvt1=0;
	    indpoint1=indpoint(ic,1);
	  }
	}
	else {
	  if (TopExp::LastVertex((TopoDS::Edge(Evive.Value(ic)))).IsSame(V1)) {
	    ctrim->Reverse();
	    ctrim2d->Reverse();
	    indpoint1=DStr.AddShape(V1);
	    isvt1=1;
	    isvt2=0;
	    indpoint2=indpoint(ic,1);  
	  } 
	}       
	ufirst=ctrim->FirstParameter();
	ulast=ctrim->LastParameter();
	Handle (GeomAdaptor_HSurface) Asurf;
	Asurf = new GeomAdaptor_HSurface(BRep_Tool::Surface
					 (TopoDS::Face(Fvive.Value(ic,icplus))));
	Handle (Geom2dAdaptor_HCurve)  Acurv=new Geom2dAdaptor_HCurve(ctrim2d);
	Adaptor3d_CurveOnSurface  CurvOnS (Acurv,Asurf);
	Handle(Adaptor3d_HCurveOnSurface) HCons =new Adaptor3d_HCurveOnSurface(CurvOnS);
	Order.SetValue(n3d,0);
	Handle(GeomPlate_CurveConstraint) Cont =
	  new GeomPlate_CurveConstraint(HCons,Order.Value(n3d),10,tolesp,angular,0.1);
	PSurf.Add(Cont);
	TopOpeBRepDS_Curve tcurv3d( ctrim,1.e-4);
	indcurve3d.SetValue(n3d, DStr.AddCurve(tcurv3d));
	Interfp1=ChFi3d_FilPointInDS(TopAbs_FORWARD,indcurve3d.Value(n3d),
				     indpoint1,ufirst,isvt1);
	Interfp2=ChFi3d_FilPointInDS(TopAbs_REVERSED,indcurve3d.Value(n3d),
				     indpoint2,ulast,isvt2);
	DStr.ChangeCurveInterferences(indcurve3d.Value(n3d)).Append(Interfp1);
	DStr.ChangeCurveInterferences(indcurve3d.Value(n3d)).Append(Interfp2);
      }
  } 
 
#ifdef DEB
  ChFi3d_InitChron(ch); // init performances pour plate 
#endif

  PSurf.Perform();

#ifdef DEB  
  ChFi3d_ResultChron(ch, t_plate); //result performances pour plate 
#endif 

  // appel a l'approx  
 
#ifdef DEB
  ChFi3d_InitChron(ch);  // init performances pour approxplate
#endif
  if (PSurf.IsDone()) {
    Standard_Integer nbcarreau=9;
    Standard_Integer degmax=8;
    Standard_Real seuil;
    Handle(GeomPlate_Surface) gpPlate = PSurf.Surface();

    TColgp_SequenceOfXY S2d;
    TColgp_SequenceOfXYZ S3d;
    S2d.Clear();
    S3d.Clear();
    PSurf.Disc2dContour(4,S2d);
    PSurf.Disc3dContour(4,0,S3d);
    seuil = Max(tolapp,10*PSurf.G0Error());
    GeomPlate_PlateG0Criterion critere (S2d,S3d,seuil);
    GeomPlate_MakeApprox Mapp(gpPlate,critere,tolapp,nbcarreau,degmax);
    Handle (Geom_Surface) Surf (Mapp.Surface());
    Standard_Real coef = 1.1 ,apperror;
    apperror=Mapp.CriterionError()*coef;

#ifdef DEB  
  ChFi3d_ResultChron(ch, t_approxplate); // result performances pour approxplate
#endif
  
//  Stockage de la surface plate  et des courbes  correspondantes dans la DS 

    TopAbs_Orientation orplate,orsurfdata,orpcurve,orien;
#ifdef DEB
//    Standard_Real ang1=PSurf.G1Error();
#endif
//     gp_Vec n1,n2,du,dv,du1,dv1;
//     gp_Pnt pp,pp1;
//     Standard_Real tpar;
//     gp_Pnt2d uv; 
//     Standard_Real scal;
    
    TopOpeBRepDS_Surface Tsurf(Surf,Mapp.ApproxError());
    Standard_Integer Isurf=DStr.AddSurface(Tsurf);
    //lbo : historique QDF.
    if(!myEVIMap.IsBound(V1)){
      TColStd_ListOfInteger li;
      myEVIMap.Bind(V1,li);
    }
    myEVIMap.ChangeFind(V1).Append(Isurf);
  
    Standard_Integer SolInd = CD.Value(0)->SolidIndex();
    TopOpeBRepDS_ListOfInterference& SolidInterfs = 
      DStr.ChangeShapeInterferences(SolInd);
 
// dans le cas ou l'on relie au sommet , il faut que les 
// aretes  vives qui arrivent au sommet  soient enlevees de la DS. 
// Pour cela on les stocke dans la DS avec leur orientation inverse 
    Standard_Integer nbedge;
    TopExp_Explorer ex;
    if (deuxconges)
      for (ic=0;ic<nedge;ic++) {
	if (!sharp.Value(ic)){
	  nbedge = CD.Value(ic)->Spine()->NbEdges();
	  TopoDS_Edge Arcspine; 
	  if (sens.Value(ic) ==1) 
	    Arcspine=CD.Value(ic) ->Spine()->Edges(1);
	  else  
	    Arcspine= CD.Value(ic)->Spine()->Edges(nbedge);
	  Standard_Integer IArcspine = DStr.AddShape(Arcspine);
#ifndef DEB
	  TopAbs_Orientation OVtx = TopAbs_FORWARD;
#else
	  TopAbs_Orientation OVtx;
#endif
	  for(ex.Init(Arcspine.Oriented(TopAbs_FORWARD),TopAbs_VERTEX); 
	      ex.More(); ex.Next()){
	    if(V1.IsSame(ex.Current())) {
	      OVtx = ex.Current().Orientation();
	      break;
	    }
	  }
	  OVtx = TopAbs::Reverse(OVtx);
	  Standard_Real parVtx = BRep_Tool::Parameter(V1,Arcspine);
	  Handle(TopOpeBRepDS_CurvePointInterference) 
	    interfv = ChFi3d_FilVertexInDS(OVtx,IArcspine,IVtx,parVtx);
	  DStr.ChangeShapeInterferences(IArcspine).Append(interfv);
	}
      }
    
    // calcul de l'orientation de Plate  orplate  par rapport aux surfdata 
    // on fait le calcul par rapport a la premiere la stripe 
    Indices(nedge,0,icplus,icmoins);
    isfirst=(sens.Value(0)==1);
    const Handle(ChFiDS_SurfData)& Fd = 
      CD.Value(0)->SetOfSurfData()->Value(i.Value(0,icmoins));
    indice= Fd->Surf();
//    Handle (Geom_Surface) surfdata  = DStr.Surface(indice).Surface();
//     tpar= (CD.Value(0)->PCurve(isfirst)->FirstParameter()+
// 	 CD.Value(0)->PCurve(isfirst)->LastParameter())/2 ;
//     CD.Value(0)->PCurve(isfirst)->D0(tpar,uv);
//     surfdata->D1(uv.X(),uv.Y(),pp,du,dv);
//     tpar=(PSurf.Curves2d()->Value(1)->FirstParameter()+
// 	  PSurf.Curves2d()->Value(1)->LastParameter())/2;
//     (PSurf.Curves2d())->Value(1)->D0(tpar,uv);
//     Surf-> D1(uv.X(),uv.Y(),pp1,du1,dv1);
//     n1=du.Crossed(dv);
//     n2=du1.Crossed(dv1);  
//     scal= n1.Dot(n2);
    orsurfdata=Fd->Orientation();
//     if (scal>0) orplate=orsurfdata;
//     else  orplate=TopAbs::Reverse(orsurfdata);  
    orplate = PlateOrientation(Surf,PSurf.Curves2d(),SumFaceNormalAtV1);
    
    //  creation de la solidinterderence pour Plate 
    Handle(TopOpeBRepDS_SolidSurfaceInterference) SSI = 
      new TopOpeBRepDS_SolidSurfaceInterference(TopOpeBRepDS_Transition(orplate),
					      TopOpeBRepDS_SOLID,
					      SolInd,
					      TopOpeBRepDS_SURFACE,
					      Isurf);
    SolidInterfs.Append(SSI);
  
  // calcul de l'orientation orien des pcurves de Plate
  // les courbes allant de ic a icplus les pcurves de Plate 
  // ont toutes la meme orientation  
    Standard_Integer Ishape1,Ishape2; 
#ifndef DEB
    TopAbs_Orientation trafil1 = TopAbs_FORWARD, trafil2 = TopAbs_FORWARD;
#else
    TopAbs_Orientation trafil1,trafil2;
#endif
    Ishape1 = Fd->IndexOfS1();
    Ishape2 = Fd->IndexOfS2();
    const ChFiDS_FaceInterference& Fi1 = Fd->InterferenceOnS1();
    const ChFiDS_FaceInterference& Fi2 = Fd->InterferenceOnS2();     
    if (Ishape1 != 0) {
      if (Ishape1 > 0) {
	trafil1 = DStr.Shape(Ishape1).Orientation();
      }
      trafil1 = TopAbs::Compose(trafil1,Fd->Orientation());
      trafil1 = TopAbs::Compose(TopAbs::Reverse(Fi1.Transition()),trafil1);
      trafil2 = TopAbs::Reverse(trafil1);
    }
    else {
      if (Ishape2 > 0) {
	trafil2 = DStr.Shape(Ishape2).Orientation();
      }
      trafil2 = TopAbs::Compose(trafil2,Fd->Orientation());
      trafil2 = TopAbs::Compose(TopAbs::Reverse(Fi2.Transition()),trafil2);
      trafil1 = TopAbs::Reverse(trafil2);
    }
    if (isfirst) {
      orpcurve=TopAbs::Reverse(trafil1);
      orpcurve= TopAbs::Compose(orpcurve,CD.Value(0)->FirstPCurveOrientation ()); }
    else {
      orpcurve=trafil1;
      orpcurve= TopAbs::Compose(orpcurve,CD.Value(0)->LastPCurveOrientation ());
    }
    if (orsurfdata==orplate) 
      orien =TopAbs::Reverse(orpcurve);
    else  orien=orpcurve; 
    
    
    if (!droit)
      for (ic=0;ic<=nedge;ic++) {
	if (libre.Value(ic)) {
	  Standard_Integer icplus21;
	  Indices(nedge,ic,icplus,icmoins);
	  Indices(nedge,icplus,icplus21,ic);
	  gp_Pnt2d UV1,UV2;
	  Handle (Geom_Curve) C3d;
	  Handle (Geom2d_Curve) C2d,curv2d;
	  gp_Pnt ptic,pticplus;
	  BRepAdaptor_Curve BCurv1(TopoDS::Edge(Evive.Value(ic)));
	  BRepAdaptor_Curve BCurv2(TopoDS::Edge(Evive.Value(icplus)));
	  Standard_Real par1=p.Value(ic,icplus);
	  Standard_Real par2=p.Value(icplus,ic);
	  BCurv1.D0(par1,ptic);
	  BCurv2.D0(par2,pticplus);
	  ParametrePlate(n3d,PSurf,Surf,ptic,apperror,UV1);
	  ParametrePlate(n3d,PSurf,Surf,pticplus,apperror,UV2);
	  Standard_Real to3d=1.e-3,to2d=1.e-6,tolreached;
	  ChFiDS_CommonPoint CP1,CP2;
	  CP1.SetArc(1.e-3, TopoDS::Edge(Evive.Value(ic)),par1,TopAbs_FORWARD);
	  CP1.SetPoint(ptic);
	  CP2.SetArc(1.e-3, TopoDS::Edge(Evive.Value(icplus)),par2,TopAbs_FORWARD);
	  CP2.SetPoint(pticplus);
	  Standard_Real param1,param2;
	  ChFi3d_ComputeArete( CP1,UV1,CP2,UV2,Surf,C3d,C2d,param1,param2, 
			      to3d,to2d,tolreached,0);
	  TopOpeBRepDS_Curve tcurv3d( C3d,tolreached);
	  Standard_Integer ind1,ind2;
	  ind1=indpoint(ic,0);
	  ind2=indpoint(icplus,0);
	  Standard_Integer indcurv=DStr.AddCurve(tcurv3d);
	  Interfp1=ChFi3d_FilPointInDS(TopAbs_FORWARD,indcurv,ind1,param1);
	  Interfp2=ChFi3d_FilPointInDS(TopAbs_REVERSED,indcurv,ind2,param2);
	  DStr.ChangeCurveInterferences(indcurv).Append(Interfp1);
	  DStr.ChangeCurveInterferences(indcurv).Append(Interfp2);
	  Interfc=ChFi3d_FilCurveInDS(indcurv,Isurf,C2d,orien);
	  DStr.ChangeSurfaceInterferences(Isurf).Append(Interfc);
	}
  }
    
    //  stockage des courbes relatives aux stripes  
    n3d = 0;
    for (ic=0; ic<nedge ;ic++) {
      if (!sharp.Value(ic)) {
	n3d++;
	Indices(nedge,ic,icplus,icmoins);
	
	isfirst=(sens.Value(ic)==1);
      //   calcul des curves interference relatives aux stripes
      
	apperror=Mapp.CriterionError()*coef;
	pardeb=CD.Value(ic)->PCurve(isfirst)->FirstParameter();
	parfin=CD.Value(ic)->PCurve(isfirst)->LastParameter();
      
	Interfp1=ChFi3d_FilPointInDS(TopAbs_FORWARD,indcurve3d.Value(n3d),
				     indpoint.Value(ic,0),pardeb);
	Interfp2=ChFi3d_FilPointInDS(TopAbs_REVERSED,indcurve3d.Value(n3d),
				     indpoint.Value(ic,1),parfin);
	DStr.ChangeCurveInterferences(indcurve3d.Value(n3d)).Append( Interfp1);
	DStr.ChangeCurveInterferences(indcurve3d.Value(n3d)).Append( Interfp2);
	TopOpeBRepDS_Curve& tcourb = DStr.ChangeCurve(indcurve3d.Value(n3d));
	
	tcourb.Tolerance(errapp.Value(ic)+apperror);
	TopOpeBRepDS_Point& tpt1= DStr.ChangePoint(indpoint(ic,0));
	TopOpeBRepDS_Point& tpt2= DStr.ChangePoint(indpoint(ic,1));
	tpt1.Tolerance (tpt1.Tolerance()+apperror);
	tpt2.Tolerance (tpt2.Tolerance()+apperror ); 
	
      // calcul de la surfaceinterference
	Interfc=ChFi3d_FilCurveInDS(indcurve3d.Value(n3d),Isurf,
				  PSurf.Curves2d()->Value(n3d),orien);     
	DStr.ChangeSurfaceInterferences(Isurf).Append(Interfc);
	regular.SetCurve(indcurve3d.Value(n3d));
	regular.SetS1(Isurf,Standard_False);
	indice=CD.Value(ic)->SetOfSurfData()->Value( i.Value(ic,icmoins))->Surf();
	regular.SetS2(indice,Standard_False);
	myRegul.Append(regular);
      }
    }

  // stockages des courbes de raccords 

    for (ic=0; ic<nedge;ic++) {
      Indices(nedge,ic,icplus,icmoins);
      if (!oksea.Value(ic)) {
	if (sharp.Value(ic) &&!deuxconges) {
	  // limitation de l'arete vive
	  TopAbs_Orientation ori;
	  gp_Pnt Pf,Pl,sommet1;
	  TopoDS_Vertex Vd = TopExp::FirstVertex(TopoDS::Edge(Evive.Value(ic)));
	  TopoDS_Vertex Vf = TopExp::LastVertex(TopoDS::Edge(Evive.Value(ic)));
	  Pf=BRep_Tool::Pnt(Vd);
	  Pl=BRep_Tool::Pnt(Vf);
	  sommet1=BRep_Tool::Pnt(V1);
	  if (Pf.Distance(sommet1)<Pl.Distance(sommet1))
	    ori = TopAbs_FORWARD;
	  else
	    ori = TopAbs_REVERSED;
	  Standard_Integer Iarc1=DStr.AddShape(TopoDS::Edge(Evive.Value(ic)));
	  Interfp1=ChFi3d_FilPointInDS(ori,Iarc1,indpoint(ic,1),p.Value(ic,icplus));
	  DStr.ChangeShapeInterferences(TopoDS::Edge(Evive.Value(ic))).Append(Interfp1);
	}
	
	if (!ponctuel.Value(ic) && !libre.Value(ic)) {
	  // raccord effectif
	  if (!moresurf.Value(ic)){
	    n3d++;
	    TopOpeBRepDS_Curve& tcourb1 = DStr.ChangeCurve(indcurve3d.Value(n3d));
	    tcourb1.Tolerance(tcourb1.Tolerance()+apperror);
	    if (!deuxconges) {
	      TopOpeBRepDS_Point& tpt11= DStr.ChangePoint(indpoint(ic,1));
	      TopOpeBRepDS_Point& tpt21= DStr.ChangePoint(indpoint(icplus,0));
	      tpt11.Tolerance (tpt11.Tolerance()+apperror);
	      tpt21.Tolerance (tpt21.Tolerance()+apperror ); 
	    }
	    Interfc=ChFi3d_FilCurveInDS(indcurve3d.Value(n3d),Isurf,
					PSurf.Curves2d()->Value(n3d),orien);
	    DStr.ChangeSurfaceInterferences(Isurf).Append(Interfc);
	    if( Order.Value(n3d)==1) {
	      regular.SetCurve(indcurve3d.Value(n3d));
	      regular.SetS1(Isurf,Standard_False);
	      regular.SetS2(numfa.Value(ic,icplus));
	      myRegul.Append(regular);
	    }
	  }
	}
      }
    }

  //stockage des courbes projetees sur plusieurs faces 
    for (ic=0; ic<nedge;ic++) {
      Indices(nedge,ic,icplus,icmoins);
      if (moresurf(ic))
	for (nb=1;nb<=nbface;nb++){ 
	  if (!Eproj.Value(nb).IsNull()) {
	    n3d++;
	    TopOpeBRepDS_Curve& tcourb1 = DStr.ChangeCurve(indcurve3d.Value(n3d));
	    tcourb1.Tolerance(tcourb1.Tolerance()+apperror);
	    if(Indice.Value(n3d)!=0) {
	      TopOpeBRepDS_Point& tpt11= DStr.ChangePoint(Indice.Value(n3d));
	      tpt11.Tolerance (tpt11.Tolerance()+apperror);
	    }
	    Interfc=ChFi3d_FilCurveInDS(indcurve3d.Value(n3d),Isurf,
					PSurf.Curves2d()->Value(n3d),orien);
	    DStr.ChangeSurfaceInterferences(Isurf).Append(Interfc);
	    if( Order.Value(n3d)==1) {
	      regular.SetCurve(indcurve3d.Value(n3d));
	      regular.SetS1(Isurf,Standard_False);
	      regular.SetS2(DStr.AddShape(TopoDS::Face(Fproj.Value(nb))));
	      myRegul.Append(regular);
	    }  
	  }
	} 
    }

  // stockage des courbes dans le cas de bords libres tangents 
    if (droit)
      for (ic=0; ic<nedge;ic++) {
	Indices(nedge,ic,icplus,icmoins);  
	TopoDS_Edge ecom;
	ecom=TopoDS::Edge(Evive.Value(ic));
	if (ecom.IsSame(edgelibre1)||ecom.IsSame(edgelibre2)) {
	  n3d++;
	  TopOpeBRepDS_Curve& tcourb1 = DStr.ChangeCurve(indcurve3d.Value(n3d));
	  tcourb1.Tolerance(tcourb1.Tolerance()+apperror);
	  Interfc=ChFi3d_FilCurveInDS(indcurve3d.Value(n3d),Isurf,
				    PSurf.Curves2d()->Value(n3d),orien);
	  DStr.ChangeSurfaceInterferences(Isurf).Append(Interfc);
	}	  
      } 
  }
  else { // il n'y a qu'un resultat partiel 
    done=Standard_False;
    hasresult=Standard_True;
    for (ic=0; ic<nedge;ic++) {
      Indices(nedge,ic,icplus,icmoins);
      if (!oksea.Value(ic)) {
	if (sharp.Value(ic) &&!deuxconges) {
	  // limitation de l'arete vive
	  TopAbs_Orientation ori;
	  gp_Pnt Pf,Pl,sommet1;
	  TopoDS_Vertex Vd = TopExp::FirstVertex(TopoDS::Edge(Evive.Value(ic)));
	  TopoDS_Vertex Vf = TopExp::LastVertex(TopoDS::Edge(Evive.Value(ic)));
	  Pf=BRep_Tool::Pnt(Vd);
	  Pl=BRep_Tool::Pnt(Vf);
	  sommet1=BRep_Tool::Pnt(V1);
	  if (Pf.Distance(sommet1)<Pl.Distance(sommet1))
	    ori = TopAbs_FORWARD;
	  else
	    ori = TopAbs_REVERSED;
	  Standard_Integer Iarc1=DStr.AddShape(TopoDS::Edge(Evive.Value(ic)));
	  Interfp1=ChFi3d_FilPointInDS(ori,Iarc1,indpoint(ic,1),p.Value(ic,icplus));
	  DStr.ChangeShapeInterferences(TopoDS::Edge(Evive.Value(ic))).Append(Interfp1);
	}
      }
    }
  }
}
