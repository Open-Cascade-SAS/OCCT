// File:	TopOpeBRepTool_tol.cxx
// Created:	Tue Apr  1 11:22:56 1997
// Author:	Jean Yves LEBEY
//		<jyl@bistrox.paris1.matra-dtv.fr>

#include <TopOpeBRepTool_tol.hxx>

#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <Precision.hxx>
#include <BRep_Tool.hxx>
#include <TopOpeBRepTool_HBoxTool.hxx>
#include <Bnd_Box.hxx>
#include <TopOpeBRepTool_box.hxx>
#include <TopOpeBRepTool_define.hxx>

Standard_EXPORT void FTOL_FaceTolerances
(const Bnd_Box& B1, const Bnd_Box& B2,
 const TopoDS_Face& myFace1, const TopoDS_Face& myFace2,
 const BRepAdaptor_Surface& mySurface1, const BRepAdaptor_Surface& mySurface2,
 Standard_Real& myTol1, Standard_Real& myTol2,
 Standard_Real& Deflection, Standard_Real& MaxUV)
{
  // ============================================================
  // ==   t o l e r a n c e s   d   I n t e r s e c t i o n    ==
  // ============================================================
  Standard_Real x0,y0,z0,x1,y1,z1,dx,dy,dz;
  Standard_Boolean Box1OK,Box2OK;

  if(   !B1.IsOpenXmin() && !B1.IsOpenXmax() 
     && !B1.IsOpenYmin() && !B1.IsOpenYmax() 
     && !B1.IsOpenZmin() && !B1.IsOpenZmax()
     && !B1.IsVoid()) { 
    Box1OK=Standard_True;
  }
  else { 
    Box1OK=Standard_False;
  }

  if(   !B2.IsOpenXmin() && !B2.IsOpenXmax() 
     && !B2.IsOpenYmin() && !B2.IsOpenYmax() 
     && !B2.IsOpenZmin() && !B2.IsOpenZmax() 
     && !B2.IsVoid()) { 
    Box2OK=Standard_True;
  }
  else { 
    Box2OK=Standard_False;
  }

  if(Box1OK) { 
    B1.Get(x0,y0,z0,x1,y1,z1);
    dx=x1-x0;
    dy=y1-y0;
    dz=z1-z0;
    
    if(Box2OK) { 
      B2.Get(x0,y0,z0,x1,y1,z1);
      Standard_Real _dx=x1-x0;
      Standard_Real _dy=y1-y0;
      Standard_Real _dz=z1-z0;
      if(_dx>dx) dx=_dx;
      if(_dy>dy) dy=_dy;
      if(_dz>dz) dz=_dz;
    }
  }
  else { 
    if(Box2OK) { 
      B2.Get(x0,y0,z0,x1,y1,z1);
      dx=x1-x0;
      dy=y1-y0;
      dz=z1-z0;
    }
    else { 
      dx=dy=dz=1.0;
    }
  }
  if(dx<dy) dx=dy;
  if(dx<dz) dx=dz;
  if(dx>1000000.0) dx=1000000.0; //-- if(dx>10000.0) dx=10000.0;

  TopExp_Explorer ex;
  Standard_Real tolef1 = Precision::Confusion();
  for (ex.Init(myFace1,TopAbs_EDGE);ex.More();ex.Next()) {
    Standard_Real tole=BRep_Tool::Tolerance(TopoDS::Edge(ex.Current()));
    if (tole>tolef1) tolef1=tole;
  }
  Standard_Real tolef2 = Precision::Confusion();
  for (ex.Init(myFace2,TopAbs_EDGE);ex.More();ex.Next()) {
    Standard_Real tole=BRep_Tool::Tolerance(TopoDS::Edge(ex.Current()));
    if (tole>tolef2) tolef2=tole;
  }
  Standard_Real tolef= tolef1;
  if (tolef2>tolef) tolef=tolef2;
 //jmb le 30 juillet 99. on ne multiplie pas la tolerance par la dimension de la piece
 // Standard_Real tolbox = tolef*dx;
  Standard_Real tolbox = tolef;
  myTol1 = tolbox;
  myTol2 = tolbox;

  Standard_Real tolmin = Precision::Confusion();
  if (tolmin>myTol1) myTol1 = tolmin;
  if (tolmin>myTol2) myTol2 = tolmin;

  Deflection=0.01;
  MaxUV=0.01;
  Deflection*=dx;

  Standard_Real MTOL1 = Max(myTol1,1e-8);
  Standard_Real MTOL2 = Max(myTol2,1e-8);
  Standard_Real MDEFLECTION = Deflection;
  Standard_Real MMAXUV=0.01;

  Standard_Real MU0,MU1,MV0,MV1,DP1,DP2,DP;
  MU0=mySurface1.FirstUParameter();
  MU1=mySurface1.LastUParameter();
  MV0=mySurface1.FirstVParameter();
  MV1=mySurface1.LastVParameter();
  DP1 = MU1-MU0;
  DP2 = MV1-MV0;
  if(DP2<DP1) DP=DP2; else DP=DP1;  //-- DP + petit

  MU0=mySurface2.FirstUParameter();
  MU1=mySurface2.LastUParameter();
  MV0=mySurface2.FirstVParameter();
  MV1=mySurface2.LastVParameter();
  DP1 = MU1-MU0;
  DP2 = MV1-MV0;
  if(DP2>DP1) DP1=DP2;    //-- DP1 + petit
  if(DP1<DP) DP=DP1;    //-- DP + petit

  DP*=0.01;
//jmb le 30 juillet 99
// non ! laisser MMAXUV a 0.01
//  MMAXUV=DP;

#ifdef DEB
//  printf("\n FaceTolerances3d : TOL1 = %5.5eg TOL2=%5.5eg DEFL=%5.5eg MAXUV=%5.5eg\n",MTOL1,MTOL2,MDEFLECTION,0.01);
#endif

  if(MTOL1>1e-4*dx) MTOL1=1e-4*dx;
  if(MTOL2>1e-4*dx) MTOL2=1e-4*dx;
  if(MTOL1<1e-8) MTOL1=1e-8;
  if(MTOL2<1e-8) MTOL2=1e-8;
  if(MMAXUV<1e-3) MMAXUV=1e-3;
  if(MDEFLECTION<1e-3) MDEFLECTION = 1e-3;

  if(MTOL1>0.5) MTOL1=0.5;
  if(MTOL2>0.5) MTOL2=0.5;
  if(MMAXUV>0.01) MMAXUV=0.01;
  if(MDEFLECTION>0.1) MDEFLECTION = 0.1;

#ifdef DEB
//  printf("TOL1 = %5.5eg TOL2=%5.5eg DEFL=%5.5eg MAXUV=%5.5eg\n",MTOL1,MTOL2,MDEFLECTION,MMAXUV);
#endif

  myTol1 = MTOL1;
  myTol2 = MTOL2;
  Deflection = MDEFLECTION;
  MaxUV = MMAXUV;

} // FTOL_FaceTolerances
 
 Standard_EXPORT void FTOL_FaceTolerances3d
(const TopoDS_Face& myFace1,
 const TopoDS_Face& myFace2,
 Standard_Real& Tol)
{
  const Handle(TopOpeBRepTool_HBoxTool)& hbt = FBOX_GetHBoxTool();
  Bnd_Box B1, B2;
  if(hbt->HasBox(myFace1)) {
    B1 = hbt->Box(myFace1);
  }
  else {
    B1.Update(0., 0., 0., 1., 1., 1.);
  }
  if(hbt->HasBox(myFace2)) {
    B2 = hbt->Box(myFace2);
  }
  else {
    B2.Update(0., 0., 0., 1., 1., 1.);
  }
  BRepAdaptor_Surface mySurface1;
  BRepAdaptor_Surface mySurface2;
  mySurface1.Initialize(myFace1);
  mySurface2.Initialize(myFace2);
  Standard_Real Deflection=0.01,MaxUV=0.01;
  Standard_Real myTol1,myTol2;
  FTOL_FaceTolerances(B1,B2,
			    myFace1,myFace2,
			    mySurface1,mySurface2,
			    myTol1,myTol2,Deflection,MaxUV);  
  myTol1 = (myTol1 > 1.e-4)? 1.e-4: myTol1;
  myTol2 = (myTol2 > 1.e-4)? 1.e-4: myTol2;
  Tol = Max(myTol1,myTol2);
}

Standard_EXPORT void FTOL_FaceTolerances3d
(const Bnd_Box& B1, const Bnd_Box& B2,
 const TopoDS_Face& myFace1, const TopoDS_Face& myFace2,
 const BRepAdaptor_Surface& mySurface1, const BRepAdaptor_Surface& mySurface2,
 Standard_Real& myTol1, Standard_Real& myTol2,
 Standard_Real& Deflection, Standard_Real& MaxUV)
{
  FTOL_FaceTolerances(B1,B2,
			    myFace1,myFace2,
			    mySurface1,mySurface2,
			    myTol1,myTol2,
			    Deflection,MaxUV);
}

Standard_EXPORT void FTOL_FaceTolerances2d
(const Bnd_Box& B1, 
 const Bnd_Box& B2,
 const TopoDS_Face& myFace1, 
 const TopoDS_Face& myFace2,
 const BRepAdaptor_Surface& mySurface1,
 const BRepAdaptor_Surface& mySurface2,
 Standard_Real& myTol1, Standard_Real& myTol2)
{
  Standard_Real BIDDeflection,BIDMaxUV;
  FTOL_FaceTolerances(B1,B2,
			    myFace1,myFace2,
			    mySurface1,mySurface2,
			    myTol1,myTol2,
			    BIDDeflection,BIDMaxUV);  
}
