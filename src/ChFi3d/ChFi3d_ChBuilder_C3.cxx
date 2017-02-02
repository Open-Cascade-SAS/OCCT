// Created on: 1995-07-04
// Created by: Stagiaire Flore Lantheaume
// Copyright (c) 1995-1999 Matra Datavision
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


#include <Adaptor3d_TopolTool.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <ChFi3d_Builder_0.hxx>
#include <ChFi3d_ChBuilder.hxx>
#include <ChFiDS_ChamfSpine.hxx>
#include <ChFiDS_HData.hxx>
#include <ChFiDS_HElSpine.hxx>
#include <ChFiDS_ListIteratorOfListOfStripe.hxx>
#include <ChFiDS_Regul.hxx>
#include <ChFiDS_Spine.hxx>
#include <ChFiDS_Stripe.hxx>
#include <ChFiDS_SurfData.hxx>
#include <ChFiKPart_ComputeData_Fcts.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomInt_IntSS.hxx>
#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <Precision.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>

//=======================================================================
//function : CoPlanar
//purpose  : Sert a savoir si 4 points sont coplanaires, pour cela on calcul
//           la distance de PntD par rapport au plan passant par les trois 
//           points PntA, PntB, PntC
//=======================================================================
static Standard_Boolean CoPlanar(const gp_Pnt  PntA,
	            const gp_Pnt  PntB,
		    const gp_Pnt  PntC,
		    const gp_Pnt  PntD)
{
  gp_Vec vecAB(PntA, PntB);
  gp_Vec vecAC(PntA, PntC);
  gp_Vec vecAD(PntA, PntD);

  Standard_Real nor2AB  = vecAB.SquareMagnitude();
  Standard_Real nor2AC  = vecAC.SquareMagnitude();
  Standard_Real ProABAC = vecAB.Dot(vecAC);


  Standard_Real Alpha  = nor2AB * nor2AC - ProABAC * ProABAC;

  if (Alpha < Precision::Confusion()) {
    return Standard_True;
  }

  Standard_Real ProABAD = vecAB.Dot(vecAD);
  Standard_Real ProACAD = vecAC.Dot(vecAD);
  Standard_Real Alpha1 = ProABAD * nor2AC - ProABAC * ProACAD;
  Standard_Real Alpha2 = ProACAD * nor2AB - ProABAC * ProABAD;
  gp_Vec vecDABC = Alpha1 * vecAB + Alpha2 * vecAC - Alpha * vecAD;

  return (vecDABC.Magnitude() / Alpha) < Precision::Confusion();
}




//=======================================================================
//function : BoundSurf
//purpose  : computes a GeomAdaptor_Surface from the surface and trims
//           it to allow the intersection computation
//=======================================================================

static Handle(GeomAdaptor_HSurface) BoundSurf(const Handle(Geom_Surface)& S,
					      const gp_Pnt2d& Pdeb,
					      const gp_Pnt2d& Pfin)
{
  Handle(GeomAdaptor_HSurface) HS = new GeomAdaptor_HSurface();
  GeomAdaptor_Surface& GAS = HS->ChangeSurface();
  GAS.Load(S);

  Standard_Real uu1,uu2,vv1,vv2;
  Standard_Real uuu1,uuu2,vvv1,vvv2;
  S->Bounds(uuu1,uuu2,vvv1,vvv2);
  ChFi3d_Boite(Pdeb,Pfin,uu1,uu2,vv1,vv2);
  Standard_Real Step = Max((uu2-uu1),(vv2-vv1));
  Step *= 0.2;
  uuu1 = Max((uu1-Step),uuu1);  uuu2 = Min((uu2+Step),uuu2);
  vvv1 = Max((vv1-Step),vvv1);  vvv2 = Min((vv2+Step),vvv2);
  GAS.Load(S,uuu1,uuu2,vvv1,vvv2);
  return HS;
}

//=======================================================================
//function : ComputeIntersection
//purpose  : compute the 3d curve <gc> and the pcurves <pc1> and <pc2>
//           of the intersection between one of the 3 SurfData <SD> and 
//           the SurfData of the corner <SDCoin>. Here we know the 
//           extremities of the intersection <pdeb> and <pfin>, and
//           their parameters <p2dfin>, <p2ddeb> on <SD>.
//           <ptcoindeb> cointains the intersection 2d point on the corner
//            which corresponds to the point <pdeb>
//           <derudeb> and <dervdeb> are the derivative vectors on the 
//            SurfData <SD> at the point <ptdeb>
//=======================================================================

static Standard_Boolean ComputeIntersection(TopOpeBRepDS_DataStructure&    DStr,
					    const Handle(ChFiDS_SurfData)& SD,
					    const Handle(ChFiDS_SurfData)& SDCoin,
					    const gp_Pnt&                  pdeb,
					    const gp_Pnt2d&                p2ddeb,
					    const gp_Pnt&                  pfin,
					    const gp_Pnt2d&                p2dfin,
					    Handle(Geom_Curve)&            gc,
					    Handle(Geom2d_Curve)&          pc1,
					    Handle(Geom2d_Curve)&          pc2,
					    gp_Vec&                        derudeb,
					    gp_Vec&                        dervdeb,
					    gp_Pnt2d&                      ptcoindeb,
					    const Standard_Real            tol3d,
					    const Standard_Real            tol2d,
					    Standard_Real&                 tolreached)
{
//  gp_Pnt2d UVf1,UVf2,UVl1,UVl2;

    // take the surface of the pivot SurfData and trim it to allow
    // the intersection computation if it's an analytic surface
  Handle(GeomAdaptor_HSurface) HS1;
  HS1 = ChFi3d_BoundSurf(DStr,SD,1,2);

  const Handle(Geom_Surface)& gpl = DStr.Surface(SDCoin->Surf()).Surface();
  const Handle(Geom_Surface)& gSD = DStr.Surface(SD->Surf()).Surface();

    // compute pardeb
  TColStd_Array1OfReal Pardeb(1,4),Parfin(1,4);
  Standard_Real u,v;
  gp_Pnt Pbidon;
  u = p2ddeb.X();
  v = p2ddeb.Y();
  gSD->D1(u,v,Pbidon,derudeb,dervdeb);
  Pardeb(1) = u;
  Pardeb(2) = v;
//  gp_Pnt2d pd2(u,v);

  ChFi3d_Parameters(gpl,pdeb,u,v);
  Pardeb(3) = u;
  Pardeb(4) = v;
  ptcoindeb.SetCoord(u,v);

    // compute parfin
  u = p2dfin.X();
  v = p2dfin.Y();
  Parfin(1) = u;
  Parfin(2) = v;
//  gp_Pnt2d pf2(u,v);

  ChFi3d_Parameters(gpl,pfin,u,v);
  Parfin(3) = u; 
  Parfin(4) = v;
  gp_Pnt2d cpf2(u,v);

  // Trims the chamfer surface to allow the intersection computation
  // and computes a GeomAdaptor_Surface for using the ComputeCurves
  // function
  Handle(GeomAdaptor_HSurface) HS2;
  HS2 = BoundSurf(gpl,ptcoindeb,cpf2);
  
  // compute the intersection curves and pcurves
  return ChFi3d_ComputeCurves(HS1,HS2,Pardeb,Parfin,gc,
			      pc1,pc2,tol3d,tol2d,tolreached);
}

//======================================================================
// function : PerformThreeCorner
// purpose  : compute the intersection of three chamfers on a same 
//            vertex of index <Jndex> in myVDataMap
//======================================================================

void ChFi3d_ChBuilder::PerformThreeCorner(const Standard_Integer Jndex) 
{

  //modifier pour le passer en option dans le cdl!!!!!!!!!!!!
  Standard_Boolean issmooth = Standard_False;

  TopOpeBRepDS_DataStructure& DStr = myDS->ChangeDS();
  const TopoDS_Vertex& Vtx = myVDataMap.FindKey(Jndex);
  ChFiDS_ListIteratorOfListOfStripe It;
//  Standard_Integer Index[3],pivot,deb,fin,ii,jj,kk;
  Standard_Integer Index[3],pivot=0,deb=0,fin=0,ii;
  Handle(ChFiDS_Stripe) CD[3];
  TopoDS_Face face[3];
  Standard_Integer jf[3][3];
  Standard_Boolean sameside[3], oksea[3];
  for(Standard_Integer g = 0; g <= 2; g++){oksea[g] = Standard_False;}
  Standard_Integer i[3][3];
  Standard_Integer sens[3];
  Standard_Real p[3][3];

  Standard_Boolean c1triangle = Standard_False;
  
  for (It.Initialize(myVDataMap(Jndex)),ii=0;It.More() && ii<3;It.Next(),ii++){
    Index[ii] = ChFi3d_IndexOfSurfData(Vtx,It.Value(),sens[ii]);
    CD[ii] = It.Value();
  }
  // On verifie que l une des CD ne figure pas deux fois, au quel cas 
  // il faut modifier le retour de IndexOfSurfData qui prend la 
  // premiere des solutions.
  if(CD[0] == CD[1]){ 
    Index[1] = CD[1]->SetOfSurfData()->Length();
    sens[1] = -1;
  }
  else if(CD[1] == CD[2]){ 
    Index[2] = CD[2]->SetOfSurfData()->Length();
    sens[2] = -1;
  }
  else if(CD[0] == CD[2]){ 
    Index[2] = CD[2]->SetOfSurfData()->Length();
    sens[2] = -1;
  }
  oksea[2] = ChFi3d_SearchFD(DStr,CD[0],CD[1],sens[0],sens[1],i[0][1],i[1][0],
		      p[0][1],p[1][0],Index[0],Index[1],face[2],sameside[2],
		      jf[0][1],jf[1][0]);
  oksea[1] = ChFi3d_SearchFD(DStr,CD[0],CD[2],sens[0],sens[2],i[0][2],i[2][0],
		      p[0][2],p[2][0],Index[0],Index[2],face[1],sameside[1],
		      jf[0][2],jf[2][0]);
  oksea[0] = ChFi3d_SearchFD(DStr,CD[1],CD[2],sens[1],sens[2],i[1][2],i[2][1],
		      p[1][2],p[2][1],Index[1],Index[2],face[0],sameside[0],
		      jf[1][2],jf[2][1]);
  //
  // Analyse des concavites des 3 chanfreins :
  //        - 2 concavites identiques et 1 inverse.
  //        - 3 concavites identiques
  //
  Standard_Boolean CornerAllSame = Standard_False;
  Standard_Boolean okinter = Standard_True;
  Standard_Boolean visavis;

  if(oksea[2] && oksea[1] && !sameside[2] && !sameside[1]) {
     pivot = 0; deb = 1; fin = 2;
     //on calcule l'intersection des pcurves sans les restreindre a leur common point
     if (!oksea[0])
       okinter =  ChFi3d_IsInFront(DStr,CD[1],CD[2],i[1][2],i[2][1],sens[1],sens[2],
				   p[1][2],p[2][1],face[0],sameside[0],
				   jf[1][2],jf[2][1],visavis,Vtx,Standard_False,1);
   }
  else if(oksea[2] && oksea[0] && !sameside[2] && !sameside[0]) {
    pivot = 1; deb = 2; fin = 0;
    if (!oksea[1])
      okinter =  ChFi3d_IsInFront(DStr,CD[0],CD[2],i[0][2],i[2][0],sens[0],sens[2],
				  p[0][2],p[2][0],face[1],sameside[1],
				  jf[0][2],jf[2][0],visavis,Vtx,Standard_False,1);
  }
  else if(oksea[1] && oksea[0] && !sameside[1] && !sameside[0]) {
    pivot = 2; deb = 0; fin = 1;
    if (!oksea[2])
      okinter =  ChFi3d_IsInFront(DStr,CD[0],CD[1],i[0][1],i[1][0],sens[0],sens[1],
				  p[0][1],p[1][0],face[2],sameside[2],
				  jf[0][1],jf[1][0],visavis,Vtx,Standard_False,1);
  }
  else if(oksea[0] && oksea[1] && oksea[2]){ 
    // 3 concavites identiques.
    pivot = ChFi3d_SearchPivot(sens,p,tol2d);
    if(pivot < 0)
      // on prend un pivot au hasard!!!!!!!!!!!!!!!
      pivot = 0;
    deb = (pivot+1)%3 ; fin = (pivot+2)%3;
    CornerAllSame = Standard_True;
  } 
  else throw Standard_Failure("FD en vis a vis non trouvees");
  if (!okinter)
     throw Standard_Failure("Echec intersection PCurves OnCommonFace");

  // on a le pivot, le CD deb et le CD fin (enfin on espere !?!) :
  // -------------------------------------------------------------
  
  /* Remarque Importante : dans le cas ou les indices des Surf data
     du pivot sur lesquelles ont ete trouvees les intersections de pcurves
     ne sont pas egaux, il va y avoir changement de Surf data lors du 
     cheminement et creations de Surf data mutantes a 3 ou 5 cotes!!!
     NON TRAITE !!!!!! (pour l instant)*/
  if(i[pivot][deb] != i[pivot][fin]){
    throw Standard_NotImplemented("coin mutant non programme");
  }
  /* Autre Remarque : dans le cas ou les indices des Surf data
     du deb (de la fin) sur lesquelles ont ete trouvees les intersections
     de pcurves ne sont pas egaux, il va y avoir changement de face lors du
     cheminement NON GERE !!!!!! (pour l instant). Prevoir un
     PerformSetOfSurf adapte.*/
  if(oksea[pivot] && 
     (i[deb][pivot] != i[deb][fin] || i[fin][pivot] != i[fin][deb])){
    throw Standard_NotImplemented("coin sur plusieurs faces non programme");
  }
  
  Handle(ChFiDS_SurfData)& 
    fddeb = CD[deb]->ChangeSetOfSurfData()->ChangeValue(i[deb][pivot]);
  Handle(ChFiDS_SurfData)& 
    fdfin = CD[fin]->ChangeSetOfSurfData()->ChangeValue(i[fin][pivot]);
  Handle(ChFiDS_SurfData)& 
    fdpiv = CD[pivot]->ChangeSetOfSurfData()->ChangeValue(i[pivot][deb]);
  
  
  // On construit les HSurfaces et autres outils qui vont bien.
  // ----------------------------------------------------------

  Handle(BRepAdaptor_HSurface) Fac = new BRepAdaptor_HSurface(face[pivot]);
  Handle(GeomAdaptor_HSurface) 
    bidsurf = new GeomAdaptor_HSurface(Fac->ChangeSurface().Surface());
  Handle(Adaptor3d_TopolTool)  IFac = new Adaptor3d_TopolTool(bidsurf);

  Handle(GeomAdaptor_HSurface) Surf = ChFi3d_BoundSurf (DStr,fdpiv,jf[pivot][deb],jf[pivot][fin]);
  Handle(Adaptor3d_TopolTool) ISurf = new Adaptor3d_TopolTool(Surf);
 
  // Creation of a new Stripe for the corner
  Handle(ChFiDS_Stripe) corner = new ChFiDS_Stripe();
  Handle(ChFiDS_HData)& cornerset = corner->ChangeSetOfSurfData();
  cornerset = new ChFiDS_HData();
  Handle(ChFiDS_SurfData) coin = new ChFiDS_SurfData();
  cornerset->Append(coin);

  // Pour plus de surete, on verifie les intersections des pcurves des chanfreins sur leur
  // face commune
  Handle(GeomAdaptor_HSurface) HSdeb
    = new GeomAdaptor_HSurface( GeomAdaptor_Surface(DStr.Surface(fddeb->Surf()).Surface()) );
  Handle(GeomAdaptor_HSurface) HSfin
    = new GeomAdaptor_HSurface( GeomAdaptor_Surface(DStr.Surface(fdfin->Surf()).Surface()) );
  Handle(GeomAdaptor_HSurface) HSpiv
    = new GeomAdaptor_HSurface( GeomAdaptor_Surface(DStr.Surface(fdpiv->Surf()).Surface()) );

  gp_Pnt2d p2d[4];
  gp_Pnt p3d[4], PSom;

  ChFi3d_ComputesIntPC (fdpiv->Interference(jf[pivot][deb]),fddeb->Interference(jf[deb][pivot]),
			HSpiv,HSdeb,p[pivot][deb],p[deb][pivot], p3d[fin]);
  ChFi3d_ComputesIntPC (fdpiv->Interference(jf[pivot][fin]),fdfin->Interference(jf[fin][pivot]),
			HSpiv,HSfin,p[pivot][fin],p[fin][pivot], p3d[deb]);
  ChFi3d_ComputesIntPC (fddeb->Interference(jf[deb][fin]),fdfin->Interference(jf[fin][deb]),
			HSdeb,HSfin,p[deb][fin],p[fin][deb], PSom);



  // On determine les extremites du coin
  //------------------------------------
  // c1triangle : on n'a besoin que des 3 points intersection des 3 chanfreins
  // sinon : on a les 2 points intersection de fdpiv avec fddeb et fdfin, et on
  //         cree 2 autres points sur la face commune a l'aide des deux premiers

    // p2d[deb] et p2d[fin] sur la surface du chanfrein fdpiv.
    // p2d[piv], p2d[3] (confondus si c1triangle) sur la face en bout du chanfrein de fdpiv
    // p2d[piv](resp.vp2d[3]) est sur la Uiso de fddeb(resp. fdfin) passant par p2d[deb]
    // (resp. p2d[fin]) 

//  if (CornerAllSame) 
//    c1triangle = (Abs(p[deb][pivot]-p[deb][fin])<tolesp &&
//		  Abs(p[fin][pivot]-p[fin][deb])<tolesp);

  gp_Vec2d Tg3,Tgpiv;
  
//  if (c1triangle)
//    p2d[pivot] = fddeb->Interference(jf[deb][fin]).PCurveOnFace()->Value(p[deb][pivot]);
//  else {
    if (issmooth) {
      fddeb->Interference(jf[deb][fin]).PCurveOnFace()->D1(p[deb][pivot],p2d[pivot],Tgpiv);
      fdfin->Interference(jf[fin][deb]).PCurveOnFace()->D1(p[fin][pivot],p2d[3],Tg3);
    }
    else {
      p2d[pivot] = fddeb->Interference(jf[deb][fin]).PCurveOnFace()->Value(p[deb][pivot]);
      p2d[3] = fdfin->Interference(jf[fin][deb]).PCurveOnFace()->Value(p[fin][pivot]);
    }
//  }
  p2d[fin] = fdpiv->Interference(jf[pivot][deb]).PCurveOnSurf()->Value(p[pivot][deb]);
  p2d[deb] = fdpiv->Interference(jf[pivot][fin]).PCurveOnSurf()->Value(p[pivot][fin]);

//  gp_Pnt pnt;
  gp_Vec deru,derv;

//  p3d[fin] = HSpiv->Value(p2d[fin].X(),p2d[fin].Y());
//  p3d[deb] = HSpiv->Value(p2d[deb].X(),p2d[deb].Y());
  Fac->D1(p2d[pivot].X(),p2d[pivot].Y(),p3d[pivot],deru,derv);
  gp_Vec norpl = deru.Crossed(derv);
//  if (!c1triangle)
    p3d[3] = Fac->Value(p2d[3].X(),p2d[3].Y());

  Standard_Real DistMin    = (p3d[3]).Distance(p3d[fin]);
  Standard_Real DistTmp    = (p3d[pivot]).Distance(p3d[deb]);
  Standard_Real DistDebFin = (p3d[pivot]).Distance(p3d[3]);

  if (DistTmp > DistMin) DistMin = DistTmp;

  // on elargi la notion de triangle pour eviter de creer
  // des surfaces ecraser avec deux coins proches
  // attention ceci entraine un effet de seuil
  if (CornerAllSame) 
    c1triangle = (DistDebFin  < 0.3 * DistMin);

  if (c1triangle)
    p3d[pivot] = PSom;


  // on calcule la surface portant le coin
  //--------------------------------------
  // Si c1triangle ou les 4 points p3d sont coplanaires, alors 
  // le chanfrein est porte par le plan passant par les 3  premiers p3d.
  // Sinon, on construit le chanfrein par la methode GeomFill_ConstrainedFilling
  Standard_Boolean c1plan = c1triangle;
  gp_Vec v1(p3d[pivot],p3d[deb]);
  gp_Vec v2(p3d[pivot],p3d[fin]);
  gp_Vec nor = v1.Crossed(v2);

  done = Standard_False;

  Standard_Integer Icf=0,Icl=0;
  Handle(Geom2d_Curve) debpc1,finpc1;

  if (!c1triangle) {
    c1plan = CoPlanar(p3d[0], p3d[1], p3d[2], p3d[3]);
  }

  if (c1plan) {    
    // c1plan
    //-------

    // on construit le plan
    gp_Dir ndir(nor);
//    gp_Dir xdir(gp_Vec(p3d[fin],p3d[deb]));
    gp_Dir xdir = gp_Dir(gp_Vec(p3d[fin],p3d[deb]));
    gp_Ax3 planAx3(p3d[pivot],ndir,xdir);
    if (planAx3.YDirection().Dot(v1)<=0.)
      planAx3.YReverse();
    Handle(Geom_Plane) gpl= new Geom_Plane(planAx3);
    coin->ChangeSurf(ChFiKPart_IndexSurfaceInDS(gpl,DStr));
    
    // on oriente coin
    gp_Vec norface = norpl;
    if (face[pivot].Orientation() == TopAbs_REVERSED )
      norface.Reverse();
    gp_Vec norcoin =  gpl->Pln().Position().XDirection().
      Crossed (gpl->Pln().Position().YDirection());
    if  ( norcoin.Dot(norface) <= 0. )
      coin->ChangeOrientation() = TopAbs_REVERSED;
    else
      coin->ChangeOrientation() = TopAbs_FORWARD; 

    // on calcule les intersections 
    Handle(Geom_Curve) gcpiv,gcdeb,gcfin;
    Handle(Geom_TrimmedCurve) gcface;
    Handle(Geom2d_Curve) pivpc1,pivpc2,debpc2,finpc2,facepc1,facepc2;
    gp_Pnt2d ptbid; 
               
      //intersection coin-pivot
    Standard_Real tolrcoinpiv;
    if (!ComputeIntersection(DStr,fdpiv,coin,
			     p3d[fin],p2d[fin],p3d[deb],p2d[deb],
			     gcpiv,pivpc1,pivpc2,deru,derv,ptbid,
			     tolesp,tol2d,tolrcoinpiv))
      throw StdFail_NotDone("echec calcul intersection coin-pivot");
    gp_Vec norpiv = deru.Crossed(derv);
    
    //intersection coin-deb
    Standard_Real tolrcoindeb;
    gp_Pnt2d p2d1,p2d2;
    if(c1triangle) 
      p2d1 = fddeb->Interference(jf[deb][fin]).PCurveOnSurf()->Value(p[deb][fin]);
    else
      p2d1 = fddeb->Interference(jf[deb][fin]).PCurveOnSurf()->Value(p[deb][pivot]);

    p2d2 = fddeb->Interference(jf[deb][pivot]).PCurveOnSurf()->Value(p[deb][pivot]);
   
    if (!ComputeIntersection(DStr,fddeb,coin,
			     p3d[pivot],p2d1,p3d[fin],p2d2,
			     gcdeb,debpc1,debpc2,deru,derv,ptbid,
			     tolesp,tol2d,tolrcoindeb))
      throw StdFail_NotDone("echec calcul intersection coin-deb");
    Icf = DStr.AddCurve(TopOpeBRepDS_Curve(gcdeb,tolrcoindeb));    

    //intersection coin-fin
    Standard_Real tolrcoinfin;
    gp_Pnt p3dface;
    if (c1triangle){
      p3dface = p3d[pivot];
      p2d1 = fdfin->Interference(jf[fin][deb]).PCurveOnSurf()->Value(p[fin][deb]);
    }
    else { 
      p3dface = p3d[3];
      p2d1 = fdfin->Interference(jf[fin][deb]).PCurveOnSurf()->Value(p[fin][pivot]);
    }
    p2d2 = fdfin->Interference(jf[fin][pivot]).PCurveOnSurf()->Value(p[fin][pivot]);
    if (!ComputeIntersection(DStr,fdfin,coin,
			     p3dface,p2d1,p3d[deb],p2d2,
			     gcfin,finpc1,finpc2,deru,derv,ptbid,
			     tolesp,tol2d,tolrcoinfin)) 
      throw StdFail_NotDone("echec calcul intersection coin-face");
    Icl = DStr.AddCurve(TopOpeBRepDS_Curve(gcfin,tolrcoinfin));  
    
    //!c1triangle: intersection coin-face[pivot]
    if (!c1triangle) {
      GeomInt_IntSS inter;
      BRepAdaptor_Surface facebid(face[pivot]);
      Handle(Geom_Surface) 
      surfbid = Handle(Geom_Surface)::DownCast(facebid.Surface().Surface()->Transformed(facebid.Trsf()));
      inter.Perform(gpl,surfbid,Precision::Intersection());
      if (inter.IsDone()) {
	Standard_Integer nbl = inter.NbLines();
	if (nbl > 1) {
#ifdef OCCT_DEBUG
	  cout<<"trop d'intersection entre les surfaces"<<endl;
#endif
	}
	else if (nbl == 1) {
	  ChFi3d_TrimCurve(inter.Line(1),p3d[pivot],p3dface,gcface);
	  
	  Handle(GeomAdaptor_HCurve) gac = new GeomAdaptor_HCurve();
	  gac->ChangeCurve().Load(gcface);
	  Handle(GeomAdaptor_HSurface) gas = new GeomAdaptor_HSurface;
	  gas->ChangeSurface().Load(gpl);
	  Handle(BRepAdaptor_HSurface) gaf = new BRepAdaptor_HSurface;
	  gaf->ChangeSurface().Initialize(face[pivot]);
	  
	  Standard_Real tolr;
	  ChFi3d_ProjectPCurv(gac,gaf,facepc1,tolesp,tolr);
	  ChFi3d_ProjectPCurv(gac,gas,facepc2,tolesp,tolr);
	}
      }
    }
    
    // on remplit les donnees du coin oriente face-pivot 
    TopAbs_Orientation trans;
    
    //avec les CommonPoints
    coin->ChangeVertexFirstOnS1().SetPoint(p3d[pivot]);
    coin->ChangeVertexFirstOnS2().SetPoint(p3d[fin]);
    if (c1triangle)
      coin->ChangeVertexLastOnS1().SetPoint(p3d[pivot]);
    else
      coin->ChangeVertexLastOnS1().SetPoint(p3d[3]);
    coin->ChangeVertexLastOnS2().SetPoint(p3d[deb]);
    
    //avec les FaceInterference
//    Standard_Integer Igcpiv,Igcdeb,Igcfin,Igcface;
    Standard_Integer Igcpiv,Igcface;
    ChFiDS_FaceInterference& fi1 = coin->ChangeInterferenceOnS1();    
    ChFiDS_FaceInterference& fi2 = coin->ChangeInterferenceOnS2();
    
    //sur face[pivot]
    if (norcoin.Dot(norpl) <= 0.) 
      trans =  TopAbs_FORWARD;
    else
      trans = TopAbs_REVERSED;
    Handle(Geom2d_Curve) bidpc;
    if (c1triangle)
      fi1.SetInterference(0,trans,bidpc,bidpc);
    else {
      Igcface = ChFiKPart_IndexCurveInDS(gcface,DStr);
      fi1.SetInterference(Igcface,trans,facepc1,facepc2);
      fi1.SetFirstParameter(gcface->FirstParameter());
      fi1.SetLastParameter(gcface->LastParameter());
    }
    //sur le pivot
    if (norcoin.Dot(norpiv) <= 0.) 
      trans =  TopAbs_REVERSED;
    else
      trans = TopAbs_FORWARD;
    Igcpiv = ChFiKPart_IndexCurveInDS(gcpiv,DStr);
    fi2.SetInterference(Igcpiv,trans,pivpc1,pivpc2);
    fi2.SetFirstParameter(gcpiv->FirstParameter());
    fi2.SetLastParameter(gcpiv->LastParameter());
    
    done = Standard_True;
    
  }
  else {
    // !c1plan
    //--------
    
    Handle(Geom_Surface) Surfcoin;
    Handle(Geom2d_Curve) PCurveOnFace,PCurveOnPiv;
    
    // le contour a remplir est constitue de courbes isos sur deb et fin
    // de deux pcurves calculees sur piv et la face opposee.
    Handle(GeomFill_Boundary) Bdeb,Bfin,Bpiv,Bfac;
    Standard_Integer ind1 = fddeb->Interference(jf[deb][pivot]).LineIndex();
    Standard_Integer ind2 = fdfin->Interference(jf[fin][pivot]).LineIndex();
    gp_Pnt Pfin,Pdeb;
    gp_Vec vpfin,vpdeb;
    
    DStr.Curve(ind1).Curve()->D1(p[deb][pivot],Pfin,vpfin);
    DStr.Curve(ind2).Curve()->D1(p[fin][pivot],Pdeb,vpdeb);
    
    if (issmooth) {
      // les bords de coin sont des lignes courbes qui suivent les 
      // tangentes donnees
      Bfac = ChFi3d_mkbound(Fac,PCurveOnFace,sens[deb],p2d[pivot],Tgpiv,
			    sens[fin],p2d[3],Tg3,tolesp,2.e-4);
      Bpiv = ChFi3d_mkbound(Surf,PCurveOnPiv,sens[deb],p2d[fin],vpfin,
			    sens[fin],p2d[deb],vpdeb,tolesp,2.e-4);
    }
    else {
      // les bords de coin sont des segments
      //      Bfac = ChFi3d_mkbound(Fac,PCurveOnFace,p2d[pivot],
      //			    p2d[3],tolesp,2.e-4);
      Bfac = ChFi3d_mkbound(Fac,PCurveOnFace,p2d[pivot],
			    p2d[3],tolesp,2.e-4);
      Bpiv = ChFi3d_mkbound(Surf,PCurveOnPiv,p2d[fin],
			    p2d[deb],tolesp,2.e-4);
    }
    
    gp_Pnt2d pdeb1 = fddeb->Interference(jf[deb][pivot]).PCurveOnSurf()->Value(p[deb][pivot]);
    gp_Pnt2d pdeb2 = fddeb->Interference(jf[deb][fin]).PCurveOnSurf()->Value(p[deb][pivot]);
    gp_Pnt2d pfin1 = fdfin->Interference(jf[fin][pivot]).PCurveOnSurf()->Value(p[fin][pivot]);
    gp_Pnt2d pfin2 = fdfin->Interference(jf[fin][deb]).PCurveOnSurf()->Value(p[fin][pivot]);
    
    if (issmooth) {
      // il faut homogeneiser, mettre les bords "BoundWithSurf"
      Bdeb = ChFi3d_mkbound(DStr.Surface(fddeb->Surf()).Surface(),pdeb1,pdeb2,tolesp,2.e-4);
      Bfin = ChFi3d_mkbound(DStr.Surface(fdfin->Surf()).Surface(),pfin1,pfin2,tolesp,2.e-4);
    }
    else {
      // ou les 4 bords de type "FreeBoundary"
      Bdeb = ChFi3d_mkbound(DStr.Surface(fddeb->Surf()).Surface(),pdeb1,pdeb2,
			    tolesp,2.e-4,Standard_True);
      Bfin = ChFi3d_mkbound(DStr.Surface(fdfin->Surf()).Surface(),pfin1,pfin2,
			    tolesp,2.e-4,Standard_True);
    }
    GeomFill_ConstrainedFilling fil(8,20);
    fil.Init(Bpiv,Bfin,Bfac,Bdeb);
    
    Surfcoin = fil.Surface();
    // on se ramene au sens face surf: S1 = face, S2 = surf
    Surfcoin->VReverse();
    
    done = CompleteData(coin,Surfcoin,
			Fac,PCurveOnFace,
			Surf,PCurveOnPiv,fdpiv->Orientation(),0,
			0,0,0,0);
  }
  Standard_Real P1deb,P2deb,P1fin,P2fin;
  
  if (done){
    Standard_Integer If1,If2,Il1,Il2;
    
    // Mise a jour des 4 Stripes et de la DS
    // -------------------------------------
    
    const ChFiDS_CommonPoint& Pf1 = coin->VertexFirstOnS1();
    const ChFiDS_CommonPoint& Pf2 = coin->VertexFirstOnS2();
    ChFiDS_CommonPoint& Pl1 = coin->ChangeVertexLastOnS1();
    if(c1triangle) 
      Pl1 = coin->ChangeVertexFirstOnS1();
    const ChFiDS_CommonPoint& Pl2 = coin->VertexLastOnS2();
    
    // le coin pour commencer,
    // -----------------------
    ChFiDS_Regul regdeb, regfin;
    If1 = ChFi3d_IndexPointInDS(Pf1,DStr);
    If2 = ChFi3d_IndexPointInDS(Pf2,DStr);
    if (c1triangle)
      Il1 = If1;
    else
      Il1 = ChFi3d_IndexPointInDS(Pl1,DStr);
    Il2 = ChFi3d_IndexPointInDS(Pl2,DStr);

    coin->ChangeIndexOfS1(DStr.AddShape(face[pivot]));
    coin->ChangeIndexOfS2(-fdpiv->Surf());

    // first points
    gp_Pnt2d pp1,pp2;
    if (c1plan) {
      P1deb = DStr.Curve(Icf).Curve()->FirstParameter();
      P2deb = DStr.Curve(Icf).Curve()->LastParameter();
    }
    else {
      pp1 = coin->InterferenceOnS1().PCurveOnSurf()->
	Value(coin->InterferenceOnS1().FirstParameter());
      pp2 = coin->InterferenceOnS2().PCurveOnSurf()->
	Value( coin->InterferenceOnS2().FirstParameter());
      Handle(Geom_Curve) C3d;
      Standard_Real tolreached;
      ChFi3d_ComputeArete(Pf1,pp1,Pf2,pp2,
			  DStr.Surface(coin->Surf()).Surface(),C3d,
			  corner->ChangeFirstPCurve(),P1deb,P2deb,
			  tolesp,tol2d,tolreached,0);
      TopOpeBRepDS_Curve Tcurv(C3d,tolreached);
      Icf = DStr.AddCurve(Tcurv);
    }
 
    regdeb.SetCurve(Icf);
    regdeb.SetS1(coin->Surf(),0);
    regdeb.SetS2(fddeb->Surf(),0);
    myRegul.Append(regdeb);
    corner->ChangeFirstCurve(Icf);
    corner->ChangeFirstParameters(P1deb,P2deb);
    corner->ChangeIndexFirstPointOnS1(If1);
    corner->ChangeIndexFirstPointOnS2(If2);

    // last points
    if (c1plan) {
      P1fin = DStr.Curve(Icl).Curve()->FirstParameter();
      P2fin = DStr.Curve(Icl).Curve()->LastParameter();
    } 
    else {
      pp1 = coin->InterferenceOnS1().PCurveOnSurf()->
	Value(coin->InterferenceOnS1().LastParameter());
      pp2 = coin->InterferenceOnS2().PCurveOnSurf()->
	Value(coin->InterferenceOnS2().LastParameter());
      Handle(Geom_Curve) C3d;
      Standard_Real tolreached;
      ChFi3d_ComputeArete(Pl1,pp1,Pl2,pp2,
			  DStr.Surface(coin->Surf()).Surface(),C3d,
			  corner->ChangeLastPCurve(),P1fin,P2fin,
			  tolesp,tol2d,tolreached,0);
      TopOpeBRepDS_Curve Tcurv(C3d,tolreached);
      Icl = DStr.AddCurve(Tcurv);
    }
    regfin.SetCurve(Icl);
    regfin.SetS1(coin->Surf(),0);
    regfin.SetS2(fdfin->Surf(),0);
    myRegul.Append(regfin);
    corner->ChangeLastCurve(Icl);
    corner->ChangeLastParameters(P1fin,P2fin);
    corner->ChangeIndexLastPointOnS1(Il1);
    corner->ChangeIndexLastPointOnS2(Il2);
    
    // puis la CornerData du debut,
    // ----------------------------
    Standard_Boolean isfirst = (sens[deb] == 1), rev = (jf[deb][fin] == 2);
    Standard_Integer isurf1 = 1, isurf2 = 2;
    Standard_Real par = p[deb][pivot], par2 = p[deb][pivot];
    if(c1triangle) par2 = p[deb][fin];
    if (rev) { 
      isurf1 = 2; isurf2 = 1; 
      CD[deb]->SetOrientation(TopAbs_REVERSED,isfirst);
    }
    CD[deb]->SetCurve(Icf,isfirst);
    CD[deb]->SetIndexPoint(If1,isfirst,isurf1);
    CD[deb]->SetIndexPoint(If2,isfirst,isurf2);
    CD[deb]->SetParameters(isfirst,P1deb,P2deb);
    fddeb->ChangeVertex(isfirst,isurf1) = Pf1;
    fddeb->ChangeVertex(isfirst,isurf2) = Pf2;
    fddeb->ChangeInterference(isurf1).SetParameter(par2,isfirst);
    fddeb->ChangeInterference(isurf2).SetParameter(par,isfirst);
    if (c1plan) 
      CD[deb]->ChangePCurve(isfirst) = debpc1;
    else {
      pp1 = fddeb->InterferenceOnS1().PCurveOnSurf()->Value(par);
      pp2 = fddeb->InterferenceOnS2().PCurveOnSurf()->Value(par);
      ChFi3d_ComputePCurv(pp1,pp2,CD[deb]->ChangePCurve(isfirst),P1deb,P2deb,rev);
    }

    // puis la CornerData de la fin,
    // -----------------------------
    isfirst = (sens[fin] == 1); rev = (jf[fin][deb] == 2);
    isurf1 = 1; isurf2 = 2;
    par = p[fin][pivot]; par2 = p[fin][pivot];
    if(c1triangle) par2 = p[fin][deb];
    if (rev) { 
      isurf1 = 2; isurf2 = 1; 
      CD[fin]->SetOrientation(TopAbs_REVERSED,isfirst);
    }
    CD[fin]->SetCurve(Icl,isfirst);
    CD[fin]->SetIndexPoint(Il1,isfirst,isurf1);
    CD[fin]->SetIndexPoint(Il2,isfirst,isurf2);
    CD[fin]->SetParameters(isfirst,P1fin,P2fin);
    fdfin->ChangeVertex(isfirst,isurf1) = Pl1;
    fdfin->ChangeVertex(isfirst,isurf2) = Pl2;
    fdfin->ChangeInterference(isurf1).SetParameter(par2,isfirst);
    fdfin->ChangeInterference(isurf2).SetParameter(par,isfirst);
    if (c1plan) 
      CD[fin]->ChangePCurve(isfirst) = finpc1;
    else {
      pp1 = fdfin->InterferenceOnS1().PCurveOnSurf()->Value(par);
      pp2 = fdfin->InterferenceOnS2().PCurveOnSurf()->Value(par);
      ChFi3d_ComputePCurv(pp1,pp2,CD[fin]->ChangePCurve(isfirst),P1fin,P2fin,rev);
    }

    // et enfin le pivot.
    // ------------------
    ChFiDS_FaceInterference& fi = coin->ChangeInterferenceOnS2();
    isfirst = (sens[pivot] == 1); rev = (jf[pivot][deb] == 2);
    isurf1 = 1; isurf2 = 2;
    if (rev) { 
      isurf1 = 2; isurf2 = 1; 
      CD[pivot]->SetOrientation(TopAbs_REVERSED,isfirst);
    }
    CD[pivot]->SetCurve(fi.LineIndex(),isfirst);
    CD[pivot]->ChangePCurve(isfirst) = fi.PCurveOnFace();
    CD[pivot]->SetIndexPoint(If2,isfirst,isurf1);
    CD[pivot]->SetIndexPoint(Il2,isfirst,isurf2);
    CD[pivot]->SetParameters(isfirst,fi.FirstParameter(),fi.LastParameter());
    fdpiv->ChangeVertex(isfirst,isurf1) = Pf2;
    fdpiv->ChangeVertex(isfirst,isurf2) = Pl2;
    fdpiv->ChangeInterference(isurf1).SetParameter(p[pivot][deb],isfirst);
    fdpiv->ChangeInterference(isurf2).SetParameter(p[pivot][fin],isfirst);
    CD[pivot]->InDS(isfirst); // filDS fait deja le boulot depuis le coin.
  }
  
  //On tronque les corners data et met a jour les index.
  //----------------------------------------------------
  
  if(i[deb][pivot] < Index[deb]){
    CD[deb]->ChangeSetOfSurfData()->Remove(i[deb][pivot]+1,Index[deb]);
    Index[deb] = i[deb][pivot];
  }
  else if(i[deb][pivot] > Index[deb]) {
    CD[deb]->ChangeSetOfSurfData()->Remove(Index[deb],i[deb][pivot]-1);
    i[deb][pivot] = Index[deb]; 
  }
  if(i[fin][pivot] < Index[fin]) {
    CD[fin]->ChangeSetOfSurfData()->Remove(i[fin][pivot]+1,Index[fin]);
    Index[fin] = i[fin][pivot];
  }
  else if(i[fin][pivot] > Index[fin]) {
    CD[fin]->ChangeSetOfSurfData()->Remove(Index[fin],i[fin][pivot]-1);
    i[fin][pivot] = Index[fin]; 
  }
  // il faudra ici tenir compte des coins mutants.
  if(i[pivot][deb] < Index[pivot]) {
    CD[pivot]->ChangeSetOfSurfData()->Remove(i[pivot][deb]+1,Index[pivot]);
    Index[pivot] = i[pivot][deb];
  }
  else if(i[pivot][deb] > Index[pivot]) {
    CD[pivot]->ChangeSetOfSurfData()->Remove(Index[pivot],i[pivot][deb]-1);
    i[pivot][deb] = Index[pivot]; 
  }
  if(!myEVIMap.IsBound(Vtx)){
    TColStd_ListOfInteger li;
    myEVIMap.Bind(Vtx,li);
  }
  myEVIMap.ChangeFind(Vtx).Append(coin->Surf());
  corner->SetSolidIndex(CD[pivot]->SolidIndex());
  myListStripe.Append(corner);
}
