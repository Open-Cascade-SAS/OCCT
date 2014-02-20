// Created on: 1994-12-13
// Created by: Jacques GOUSSARD
// Copyright (c) 1994-1999 Matra Datavision
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

#include <Standard_Stream.hxx>
#include <Standard_Macro.hxx>

#include <BRepTest.hxx>

#include <DBRep.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Box.hxx>

#include <BRepBuilderAPI.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_GTransform.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <gp_Ax2.hxx>
#include <gp_Mat.hxx>
#include <gp_GTrsf.hxx>
#include <BRepOffsetAPI_NormalProjection.hxx>
#include <BRepLib.hxx>
#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <BRepTools_WireExplorer.hxx>

#include <GCPnts_QuasiUniformAbscissa.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <ProjLib_ComputeApproxOnPolarSurface.hxx>
#include <DrawTrSurf.hxx>


#include <Draw_Segment3D.hxx>
#include <Draw_Marker3D.hxx>
#include <Draw_MarkerShape.hxx>

#include <stdio.h>

Standard_IMPORT Draw_Viewer dout;



//=======================================================================
// addpcurve
//=======================================================================

static Standard_Integer addpcurve(Draw_Interpretor& , Standard_Integer n, const char** a)
{
  if(n < 4) return 1;
  TopoDS_Shape E = DBRep::Get(a[1]);
  if (E.IsNull()) return 1;
  Handle(Geom2d_Curve) PC = DrawTrSurf::GetCurve2d(a[2]);
  TopoDS_Shape F = DBRep::Get(a[3]);
  Standard_Real tol = 1.e-7;
  if (n > 4) {
    tol = Draw::Atof(a[4]);
  }
  BRep_Builder BB;
  BB.UpdateEdge(TopoDS::Edge(E), PC, TopoDS::Face(F),tol); 
  DBRep::Set(a[1], E);
  return 0;
}


//=======================================================================
// transform
//=======================================================================

static Standard_Integer transform(Draw_Interpretor& di,Standard_Integer n,const char** a)
{
  if (n <= 1) return 1;

  gp_Trsf T;
  Standard_Integer last = n;
  const char* aName = a[0];

  Standard_Boolean isBasic = Standard_False;

  if (!strcmp(aName,"reset")) {
  }
  else {
    isBasic = (aName[0] == 'b');
    aName++;

    if (!strcmp(aName,"move")) {
      if (n < 3) return 1;
      TopoDS_Shape SL = DBRep::Get(a[n-1]);
      if (SL.IsNull()) return 0;
      T = SL.Location().Transformation();
      last = n-1;
    }
    else if (!strcmp(aName,"translate")) {
      if (n < 5) return 1;
      T.SetTranslation(gp_Vec(Draw::Atof(a[n-3]),Draw::Atof(a[n-2]),Draw::Atof(a[n-1])));
      last = n-3;
    }
    else if (!strcmp(aName,"rotate")) {
      if (n < 9) return 1;
      T.SetRotation(gp_Ax1(gp_Pnt(Draw::Atof(a[n-7]),Draw::Atof(a[n-6]),Draw::Atof(a[n-5])),
                    gp_Vec(Draw::Atof(a[n-4]),Draw::Atof(a[n-3]),Draw::Atof(a[n-2]))),
                    Draw::Atof(a[n-1])* (M_PI / 180.0));
      last = n-7;
    }
    else if (!strcmp(aName,"mirror")) {
      if (n < 8) return 1;
      T.SetMirror(gp_Ax2(gp_Pnt(Draw::Atof(a[n-6]),Draw::Atof(a[n-5]),Draw::Atof(a[n-4])),
                  gp_Vec(Draw::Atof(a[n-3]),Draw::Atof(a[n-2]),Draw::Atof(a[n-1]))));
      last = n-6;
    }
    else if (!strcmp(aName,"scale")) {
      if (n < 6) return 1;
      T.SetScale(gp_Pnt(Draw::Atof(a[n-4]),Draw::Atof(a[n-3]),Draw::Atof(a[n-2])),Draw::Atof(a[n-1]));
      last = n-4;
    }
  }

  if (T.Form() == gp_Identity || isBasic) {
    TopLoc_Location L(T);
    for (Standard_Integer i = 1; i < last; i++) {
      TopoDS_Shape S = DBRep::Get(a[i]);
      if (S.IsNull())
        di << a[i] << " is not a valid shape\n";
      else
        DBRep::Set(a[i],S.Located(L));
    }
  }
  else {
    BRepBuilderAPI_Transform trf(T);
    for (Standard_Integer i = 1; i < last; i++) {
      TopoDS_Shape S = DBRep::Get(a[i]);
      if (S.IsNull()) {
        di << a[i] << " is not a valid shape\n";
      }
      else {
        trf.Perform(S);
        if (!trf.IsDone())
          return 1;
        DBRep::Set(a[i],trf.Shape());
      }
    }
  }
  return 0;
}

///=======================================================================
// gtransform
//=======================================================================

static Standard_Integer deform(Draw_Interpretor& di,Standard_Integer n,const char** a)
{
  if (n <= 1) return 1;
  
  Standard_Integer last = n;
  
  gp_Trsf T;
  gp_GTrsf GT(T);
  
//  gp_Mat rot(Draw::Atof(a[last-3]),0,0,0,Draw::Atof(a[last-2]),0,0,0,Draw::Atof(a[last-1]));
  gp_Mat rot(Draw::Atof(a[3]),0,0,0,Draw::Atof(a[4]),0,0,0,Draw::Atof(a[5]));
  GT.SetVectorialPart(rot);
  last -= 3;
  BRepBuilderAPI_GTransform gtrf(GT);
  BRepBuilderAPI_NurbsConvert nbscv;
  //  for (Standard_Integer i = 1; i < last; i++) {
  //    TopoDS_Shape S = DBRep::Get(a[i]);
  TopoDS_Shape S = DBRep::Get(a[2]);    
  if (S.IsNull()) {
    //cout << a[2] << " is not a valid shape" << endl;
    di << a[2] << " is not a valid shape" << "\n";
  }
  else {
    gtrf.Perform(S);
    if (gtrf.IsDone()){
      DBRep::Set(a[1],gtrf.Shape());
    }
    else {
      return 1;
    }
  }
  
  return 0;
}

//=======================================================================
// tcopy
//=======================================================================

static Standard_Integer tcopy(Draw_Interpretor& di,Standard_Integer n,const char** a)
{
  Standard_Boolean copyGeom = Standard_True;
  Standard_Integer iFirst = 1; // index of first shape argument

  if (n > 1 && a[1][0] == '-' && a[1][1] == 'n' )
  {
    copyGeom = Standard_False;
    iFirst = 2;
  }

  if (n < 3 || (n - iFirst) % 2) {
    cout << "Use: " << a[0] << " [-n(ogeom)] shape1 copy1 [shape2 copy2 [...]]" << endl;
    cout << "Option -n forbids copying of geometry (it will be shared)" << endl; 
    return 1;
  }

  BRepBuilderAPI_Copy cop;
  Standard_Integer nbPairs = (n - iFirst) / 2;
  for (Standard_Integer i=0; i < nbPairs; i++) {
    cop.Perform(DBRep::Get(a[i+iFirst]), copyGeom);
    DBRep::Set(a[i+iFirst+1],cop.Shape());
    di << a[i+iFirst+1] << " ";
  }
  return 0;
}


//=======================================================================
// NurbsConvert
//=======================================================================

static Standard_Integer nurbsconvert(Draw_Interpretor& di,Standard_Integer n,const char** a)
{
  if (n < 3) return 1;
  if ((n-1)%2 != 0) return 1;
  BRepBuilderAPI_NurbsConvert nbscv;
  for (Standard_Integer i=0; i<(n-1)/2; i++) {
    TopoDS_Shape S = DBRep::Get(a[2*i+2]);
    if (S.IsNull()) {
      //cout << a[2*i+2] << " is not a valid shape" << endl;
      di << a[2*i+2] << " is not a valid shape" << "\n";
    }
    else {
      nbscv.Perform(S);
      if (nbscv.IsDone()){
	DBRep::Set(a[2*i+1],nbscv.Shape());
      }
      else {
	return 1;
      }
    }
  }
  
  return 0;
  
}

//=======================================================================
// make a 3D edge curve
//=======================================================================

static Standard_Integer mkedgecurve (Draw_Interpretor& ,Standard_Integer n,const char** a)
{

  if (n < 3) return 1;
  Standard_Real Tolerance = Draw::Atof(a[2]) ;

  TopoDS_Shape S = DBRep::Get(a[1]);
  
  if (S.IsNull()) return 1;
  
   BRepLib::BuildCurves3d(S,
			  Tolerance) ;
   return 0 ;
}

//=======================================================================
// sameparameter
//=======================================================================

static Standard_Integer sameparameter(Draw_Interpretor& ,Standard_Integer n,const char** a)
{
  if (n < 2) return 1;
  Standard_Real tol = 1.e-7;
  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull()) return 1;
  Standard_Boolean force  = !strcmp(a[0],"fsameparameter");
  if (n == 3) tol = Draw::Atof(a[2]);

  BRepLib::SameParameter(S,tol,force);

  DBRep::Set(a[1],S);
  return 0;
}
//=======================================================================
//function : updatetol
//purpose  : 
//=======================================================================
static Standard_Integer updatetol(Draw_Interpretor& ,Standard_Integer n,const char** a)
{
  if (n < 2) return 1;

  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull()) return 1;

  if (n==2) BRepLib::UpdateTolerances(S);
  else BRepLib::UpdateTolerances(S,Standard_True);
  DBRep::Set(a[1],S);
  return 0;

}

//=======================================================================
//function : OrienSolid
//purpose  : 
//=======================================================================
static Standard_Integer orientsolid(Draw_Interpretor& ,Standard_Integer n,const char** a)
{
  if (n < 2) return 1;

  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull()) return 1;
  if (S.ShapeType()!=TopAbs_SOLID) return 1;

  BRepLib::OrientClosedSolid(TopoDS::Solid(S));

  DBRep::Set(a[1],S);
  return 0;

}

//=======================================================================
//function : boundingstr
//purpose  : 
//=======================================================================
static Standard_Integer boundingstr(Draw_Interpretor& di,Standard_Integer n,const char** a)
{
  if (n < 2) return 1;
  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull()) return 1;
  Bnd_Box B;
  BRepBndLib::Add(S,B);
  Standard_Real axmin,aymin,azmin,axmax,aymax,azmax;
  B.Get(axmin,aymin,azmin,axmax,aymax,azmax);
  di << axmin<<" "<< aymin<<" "<< azmin<<" "<< axmax<<" "<< aymax<<" "<< azmax;
  if (n >= 8) {
    Draw::Set(a[2],axmin) ;
    Draw::Set(a[3],aymin) ;
    Draw::Set(a[4],azmin) ;
    Draw::Set(a[5],axmax) ;
    Draw::Set(a[6],aymax) ;
    Draw::Set(a[7],azmax) ;
  }
  return 0;
}

//=======================================================================
//function : getcoords
//purpose  : 
//=======================================================================
static Standard_Integer getcoords(Draw_Interpretor& di,Standard_Integer n,const char** a)
{
  if(n < 2) 
    return 1;

  for (Standard_Integer i = 1; i < n; i++) 
  {
    const TopoDS_Shape aShape = DBRep::Get (a[i]);

    if (aShape.IsNull())
      continue;

    if (aShape.ShapeType() == TopAbs_VERTEX)
    {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(aShape);
      gp_Pnt aPnt = BRep_Tool::Pnt(aVertex);

      di << a[i] << " (x,y,z) : " << aPnt.X() << " " << aPnt.Y() << " " << aPnt.Z() << "\n";
    }
  }

  return 0;
}

//=======================================================================
//function : bounding
//purpose  : 
//=======================================================================
static Standard_Integer bounding(Draw_Interpretor& di,Standard_Integer n,const char** a)
{
  if (n < 2) return 1;
  Standard_Real axmin,aymin,azmin,axmax,aymax,azmax;
  Bnd_Box B; Handle(Draw_Box) DB;
  
  if (n == 2) { 
    TopoDS_Shape S = DBRep::Get(a[1]);
    if (S.IsNull()) return 1;
    BRepBndLib::Add(S,B);
    B.Get(axmin,aymin,azmin,axmax,aymax,azmax);
    DB = new Draw_Box(gp_Pnt(axmin,aymin,azmin),gp_Pnt(axmax,aymax,azmax),Draw_orange);
    dout<<DB;
    di << axmin<<" "<< aymin<<" "<< azmin<<" "<< axmax<<" "<< aymax<<" "<< azmax;
  }
  else if (n == 7) {
    axmin=Draw::Atof(a[1]);
    aymin=Draw::Atof(a[2]);
    azmin=Draw::Atof(a[3]);
    axmax=Draw::Atof(a[4]);
    aymax=Draw::Atof(a[5]);
    azmax=Draw::Atof(a[6]);
    DB = new Draw_Box(gp_Pnt(axmin,aymin,azmin),gp_Pnt(axmax,aymax,azmax),Draw_orange);
    dout<<DB;
  }
  return 0;
}
//=======================================================================
//function : findplane
//purpose  : 
//=======================================================================
static Standard_Integer findplane(Draw_Interpretor& di,Standard_Integer n,const char** a)
{
  if (n < 3) return 1;
  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull()) return 1;
  Standard_Real tolerance = 1.0e-5 ;
  BRepBuilderAPI_FindPlane a_plane_finder(S,
				   tolerance) ;
  if (a_plane_finder.Found()) {
    //cout << " a plane is found "   ;
    di << " a plane is found \n";
    DrawTrSurf::Set(a[2],a_plane_finder.Plane()) ;
  }
  return 0 ;
}
//=======================================================================
//function : precision
//purpose  : 
//=======================================================================

static Standard_Integer precision(Draw_Interpretor& di,Standard_Integer n,const char** a)
{
  n--;

  if ( n == 0) {
    //cout << " Current Precision = " << BRepBuilderAPI::Precision() << endl;
    di << " Current Precision = " << BRepBuilderAPI::Precision() << "\n";
  }
  else {
    BRepBuilderAPI::Precision(Draw::Atof(a[1]));
  }
  return 0;
}


//=======================================================================
//function : reperage shape (Int lin Shape) + pointe double click   + maxtol
//purpose  : 
//=======================================================================
#include <IntCurvesFace_ShapeIntersector.hxx>
#include <gp_Lin.hxx>

static Standard_Integer reperageshape(Draw_Interpretor& di, Standard_Integer narg , const char** a) 
{
  Standard_Integer details=0;
  if(narg<2) return 1;
  if(narg==3) details=1;
  const char *id1 = a[1];
  TopoDS_Shape TheShape1 = DBRep::Get(id1);
  
  //cout << "Pick positions with button "<<endl;
  di << "Pick positions with button "<< "\n";
  Standard_Integer id,X,Y,b;
  gp_Trsf T;
  gp_Pnt P1,P2;
  dout.Select(id,X,Y,b);
  
  dout.GetTrsf(id,T);
  T.Invert();
  Standard_Real z = dout.Zoom(id);
  P2.SetCoord((Standard_Real)X /z,(Standard_Real)Y /z, 0.0);
  P2.Transform(T);
  P1.SetCoord((Standard_Real)X /z,(Standard_Real)Y /z,-1.0);
  P1.Transform(T);
  
  
  gp_Ax1 Axe(P1,gp_Vec(P1,P2));
  IntCurvesFace_ShapeIntersector Inter;
  Inter.Load(TheShape1,1e-7);
  
  Inter.Perform(Axe,-RealLast(),RealLast());
  
  //cout<<"\n --> ";
  di <<"\n --> ";
  if(Inter.NbPnt()) { 
    for(Standard_Integer i=1; i<=Inter.NbPnt(); i++) { 
      Standard_Integer numface=1;
      TopExp_Explorer ExF;
      for(ExF.Init(TheShape1,TopAbs_FACE);
	  ExF.More();
	  ExF.Next(),numface++) { 
	TopoDS_Face Face=TopoDS::Face(ExF.Current());
	if(Face.IsEqual(Inter.Face(i))) { 
	  //cout<<" "<<a[1]<<"_"<<numface;
	  di<<" "<<a[1]<<"_"<<numface;
	  continue;	  
	}
      }
      const gp_Pnt& P = Inter.Pnt(i);
      Standard_Real PMin = Inter.WParameter(i);
      if(details) { 
	//cout<<" w:"<<PMin<<endl;
	di<<" w:"<<PMin<< "\n";
      }
      if(Inter.Transition(i) == IntCurveSurface_In) { 
	if(Inter.State(i) == TopAbs_IN) { 
	  Handle(Draw_Marker3D) p = new Draw_Marker3D(P, Draw_Square, Draw_rouge,2); 
	  dout << p;   dout.Flush();
	}
	else if(Inter.State(i) == TopAbs_ON) { 
	  Handle(Draw_Marker3D) p = new Draw_Marker3D(P, Draw_Square, Draw_vert,2); 
	  dout << p;   dout.Flush();
	}
      }
      else { 
	if(Inter.Transition(i) == IntCurveSurface_Out) { 
	  if(Inter.State(i) == TopAbs_IN) { 
	    Handle(Draw_Marker3D) p = new Draw_Marker3D(P, Draw_X, Draw_rouge,2); 
	    dout << p;   dout.Flush();
	  }
	  else if(Inter.State(i) == TopAbs_ON) { 
	    Handle(Draw_Marker3D) p = new Draw_Marker3D(P, Draw_X, Draw_vert,2); 
	    dout << p;   dout.Flush();
	  }
	} 
      }
    }
  }
  //cout<<endl;
  di << "\n";
  return(0);
}


static Standard_Integer maxtolerance(Draw_Interpretor& theCommands, 
				     Standard_Integer n, const char** a) { 
  if(n<2) return(1);
  TopoDS_Shape TheShape = DBRep::Get(a[1]);
  if(TheShape.IsNull()) return(1);

  Standard_Real T,TMF,TME,TMV,TmF,TmE,TmV;
  Standard_Integer nbF,nbE,nbV;
  TMF=TME=TMV=-RealLast();
  TmF=TmE=TmV=RealLast();
  
  TopTools_MapOfShape mapS;
  mapS.Clear();

  for(TopExp_Explorer ex(TheShape,TopAbs_FACE);
      ex.More();
      ex.Next()) { 
    TopoDS_Face Face=TopoDS::Face(ex.Current());
    T=BRep_Tool::Tolerance(Face);
    if(T>TMF) TMF=T;
    if(T<TmF) TmF=T;
    mapS.Add(Face);
  }
  
  nbF = mapS.Extent();
  mapS.Clear();
  
  for(TopExp_Explorer ex(TheShape,TopAbs_EDGE);
      ex.More();
      ex.Next()) { 
    TopoDS_Edge Edge=TopoDS::Edge(ex.Current());
    T=BRep_Tool::Tolerance(Edge);
    if(T>TME) TME=T;
    if(T<TmE) TmE=T;
    mapS.Add(Edge);
  }

  nbE = mapS.Extent();
  mapS.Clear();

  for(TopExp_Explorer ex(TheShape,TopAbs_VERTEX);
      ex.More();
      ex.Next()) { 
    TopoDS_Vertex Vertex=TopoDS::Vertex(ex.Current());
    T=BRep_Tool::Tolerance(Vertex);
    if(T>TMV) TMV=T;
    if(T<TmV) TmV=T;
    mapS.Add(Vertex);
  }

  nbV = mapS.Extent();

//#ifndef WNT
  Standard_SStream sss;
  sss << "\n## Tolerances on the shape " << a[1] << "  (nbFaces:" << nbF
      << "  nbEdges:" << nbE << " nbVtx:" << nbV << ")\n" ;
  sss.precision(5);
  sss.setf(ios::scientific);
  if(TmF<=TMF) sss << "\n    Face   : Min " << setw(8) << TmF <<"    Max  " << setw(8) << TMF << " \n ";
  if(TmE<=TME) sss << "\n    Edge   : Min " << setw(8) << TmE <<"    Max  " << setw(8) << TME << " \n ";
  if(TmV<=TMV) sss << "\n    Vertex : Min " << setw(8) << TmV <<"    Max  " << setw(8) << TMV << " \n ";
  theCommands << sss;
  //#endif*/
  return 0;
}


static Standard_Integer vecdc(Draw_Interpretor& di,Standard_Integer ,const char** ) {
  //cout << "Pick positions with button "<<endl;
  di << "Pick positions with button "<< "\n";

  Standard_Integer id,X,Y,b;
  gp_Trsf T;
  gp_Pnt P1,P2,PP1,PP2;
  
  //-----------------------------------------------------------
  dout.Select(id,X,Y,b);    dout.GetTrsf(id,T);
  T.Invert();
  Standard_Real z = dout.Zoom(id);
  P1.SetCoord((Standard_Real)X /z,(Standard_Real)Y /z,0.0);
  P1.Transform(T);
  
  dout.Select(id,X,Y,b);  dout.GetTrsf(id,T);
  T.Invert();  z = dout.Zoom(id);
  
  P2.SetCoord((Standard_Real)X /z,(Standard_Real)Y /z,0.0);
  P2.Transform(T);
  Standard_Real xa,ya,za;
  if(Abs(P1.X())>Abs(P2.X())) xa = P1.X(); else xa = P2.X();
  if(Abs(P1.Y())>Abs(P2.Y())) ya = P1.Y(); else ya = P2.Y();
  if(Abs(P1.Z())>Abs(P2.Z())) za = P1.Z(); else za = P2.Z();
  P1.SetCoord(xa,ya,za);
  Handle(Draw_Marker3D) D0 = new Draw_Marker3D(gp_Pnt(P1.X(),
						      P1.Y(),
						      P1.Z()),
					       Draw_Square,Draw_blanc,1);
  
  dout << D0;
  dout.Flush();
  //-----------------------------------------------------------
  dout.Select(id,X,Y,b);  
  dout.GetTrsf(id,T);
  T.Invert();
  z = dout.Zoom(id);
  PP1.SetCoord((Standard_Real)X /z,(Standard_Real)Y /z,0.0);
  PP1.Transform(T);
  dout.Select(id,X,Y,b);
  dout.GetTrsf(id,T);
  T.Invert();
  z = dout.Zoom(id);
  PP2.SetCoord((Standard_Real)X /z,(Standard_Real)Y /z,0.0);
  PP2.Transform(T);
  if(Abs(PP1.X())>Abs(PP2.X())) xa = PP1.X(); else xa = PP2.X();
  if(Abs(PP1.Y())>Abs(PP2.Y())) ya = PP1.Y(); else ya = PP2.Y();
  if(Abs(PP1.Z())>Abs(PP2.Z())) za = PP1.Z(); else za = PP2.Z();
  PP1.SetCoord(xa,ya,za);
  Handle(Draw_Segment3D) d = new Draw_Segment3D(P1,PP1,Draw_blanc);
  dout << d;
  dout.Flush();
  //cout<<"\nttran   "<<PP1.X()-P1.X()<<" "<<PP1.Y()-P1.Y()<<" "<<PP1.Z()-P1.Z()<<endl;
  di <<"\nttran   "<<PP1.X()-P1.X()<<" "<<PP1.Y()-P1.Y()<<" "<<PP1.Z()-P1.Z()<< "\n";

  static Standard_Integer nboxvecdp=0;
  //cout<<"\nbox  b"<<++nboxvecdp<<" "<<Min(P1.X(),PP1.X())<<" "<<Min(P1.Y(),PP1.Y())<<" "<<Min(PP1.Z(),P1.Z());
  //cout<<"  "<<Abs(PP1.X()-P1.X())<<" "<<Abs(PP1.Y()-P1.Y())<<" "<<Abs(PP1.Z()-P1.Z())<<endl;

  //cout<<"\nDistance :"<<sqrt( (PP1.X()-P1.X())*(PP1.X()-P1.X())
	//		     +(PP1.Y()-P1.Y())*(PP1.Y()-P1.Y())
	//		     +(PP1.Z()-P1.Z())*(PP1.Z()-P1.Z()))<<endl;

  di <<"\nbox  b"<<++nboxvecdp<<" "<<Min(P1.X(),PP1.X())<<" "<<Min(P1.Y(),PP1.Y())<<" "<<Min(PP1.Z(),P1.Z());
  di <<"  "<<Abs(PP1.X()-P1.X())<<" "<<Abs(PP1.Y()-P1.Y())<<" "<<Abs(PP1.Z()-P1.Z())<< "\n";

  di <<"\nDistance :"<<sqrt( (PP1.X()-P1.X())*(PP1.X()-P1.X())
			     +(PP1.Y()-P1.Y())*(PP1.Y()-P1.Y())
			     +(PP1.Z()-P1.Z())*(PP1.Z()-P1.Z()))<< "\n";
  return(0);
}
//=======================================================================
// nproject
//=======================================================================

#include <TopTools_SequenceOfShape.hxx>
 static Standard_Integer nproject(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if ( n < 4) return 1;
  TopoDS_Shape InpShape;
  Standard_Integer arg = 2, i;
  TopTools_SequenceOfShape Args; 

  Standard_Real Tol = 1.e-4;        
  Standard_Real Tol2d;
  Standard_Real MaxDistance = 1.e-3;
  GeomAbs_Shape Continuity = GeomAbs_C2;  
  Standard_Integer MaxDeg = 14;           
  Standard_Integer MaxSeg = 16;           

  while((n > arg) && !(InpShape = DBRep::Get(a[arg])).IsNull()){
    Args.Append(InpShape);
    arg++;
  }
  if(Args.Length() < 2) return 1;
  
  BRepOffsetAPI_NormalProjection OrtProj(Args.Last());

  for(i = 1; i < Args.Length(); i++)
    OrtProj.Add(Args(i));

  if(n > arg)
    if (!strcmp(a[arg],"-g")) {
      OrtProj.SetLimit(Standard_False);
      arg++;
    }
  
  if(n > arg)
    if (!strcmp(a[arg],"-d")) {
      arg++;
      if(n > arg)
	MaxDistance = Draw::Atof(a[arg++]);
      OrtProj.SetMaxDistance(MaxDistance);
    }
  if(n > arg) {
    Tol = Max(Draw::Atof(a[arg++]),1.e-10);
  }

  if(n > arg) {
    if (Draw::Atoi(a[arg]) == 0) Continuity = GeomAbs_C0;
    else if (Draw::Atoi(a[arg]) == 1) Continuity = GeomAbs_C1;
    arg++;
  }

 
  if(n > arg) {
    MaxDeg = Draw::Atoi(a[arg++]);
    if (MaxDeg<1 || MaxDeg>14) MaxDeg = 14;
  }

  if(n > arg) MaxSeg = Draw::Atoi(a[arg]);
    
  Tol2d = Pow(Tol, 2./3);

  OrtProj.SetParams(Tol, Tol2d, Continuity, MaxDeg, MaxSeg);
  OrtProj.Build();
  TopTools_ListOfShape Wire;
  Standard_Boolean IsWire=OrtProj.BuildWire(Wire);
  if (IsWire) {
    //cout << " BuildWire OK " << endl;
    di << " BuildWire OK " << "\n";
  }
  DBRep::Set(a[1], OrtProj.Shape());
  return 0;  
}

//==========================================================================
//function : wexplo
//           exploration of a wire 
//==========================================================================
static Standard_Integer wexplo (Draw_Interpretor&, 
				Standard_Integer argc, const char** argv)
{ 
  char name[100];
  if (argc < 2) return 1;
  
  TopoDS_Shape C1 = DBRep::Get (argv[1],TopAbs_WIRE);
  TopoDS_Shape C2 ;

  if (argc > 2)  C2 = DBRep::Get (argv[2],TopAbs_FACE);

  if (C1.IsNull()) return 1;

  BRepTools_WireExplorer we;
  if (C2.IsNull()) we.Init(TopoDS::Wire(C1));
  else             we.Init(TopoDS::Wire(C1),TopoDS::Face(C2));

  Standard_Integer k = 1;
  while (we.More()) {
    TopoDS_Edge E = we.Current();
    Sprintf(name,"WEDGE_%d",k);	
	  DBRep::Set(name,E);
    we.Next();
    k++;
  }

  return 0;
}

static Standard_Integer scalexyz(Draw_Interpretor& /*di*/, Standard_Integer n, const char** a)
{
  if (n < 6) return 1;

  TopoDS_Shape aShapeBase = DBRep::Get(a[2]);
  if (aShapeBase.IsNull()) return 1;
  
  Standard_Real aFactorX = Draw::Atof(a[3]);
  Standard_Real aFactorY = Draw::Atof(a[4]);
  Standard_Real aFactorZ = Draw::Atof(a[5]);

  gp_GTrsf aGTrsf;
  gp_Mat rot (aFactorX, 0, 0,
              0, aFactorY, 0,
              0, 0, aFactorZ);
  aGTrsf.SetVectorialPart(rot);
  BRepBuilderAPI_GTransform aBRepGTrsf (aShapeBase, aGTrsf, Standard_False);
  if (!aBRepGTrsf.IsDone())
    Standard_ConstructionError::Raise("Scaling not done");
  TopoDS_Shape Result = aBRepGTrsf.Shape();

  DBRep::Set(a[1], Result);
  return 0;  
}

void  BRepTest::BasicCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  DBRep::BasicCommands(theCommands);

  const char* g = "TOPOLOGY Basic shape commands";

  theCommands.Add("addpcurve",
		  "addpcurve edge 2dcurve face [tol (default 1.e-7)]",
		  __FILE__,
		  addpcurve,g);

  theCommands.Add("reset",
		  "reset name1 name2 ..., remove location",
		  __FILE__,
		  transform,g);

  theCommands.Add("tmove",
		  "tmove name1 name2 ... name, set location from name",
		  __FILE__,
		  transform,g);

  theCommands.Add("ttranslate",
		  "ttranslate name1 name2 ... dx dy dz",
		  __FILE__,
		  transform,g);

  theCommands.Add("trotate",
		  "trotate name1 name2 ... x y z dx dy dz angle",
		  __FILE__,
		  transform,g);

  theCommands.Add("tmirror",
		  "tmirror name x y z dx dy dz",
		  __FILE__,
		  transform,g);

  theCommands.Add("tscale",
		  "tscale name x y z scale",
		  __FILE__,
		  transform,g);

  theCommands.Add("tcopy",
		  "tcopy [-n(ogeom)] name1 result1 [name2 result2 ...]",
		  __FILE__,
		  tcopy,g);

  theCommands.Add("bmove",
		  "bmove name1 name2 ... name, set location from name",
		  __FILE__,
		  transform,g);

  theCommands.Add("btranslate",
		  "btranslate name1 name2 ... dx dy dz",
		  __FILE__,
		  transform,g);

  theCommands.Add("brotate",
		  "brotate name1 name2 ... x y z dx dy dz angle",
		  __FILE__,
		  transform,g);

  theCommands.Add("bmirror",
		  "bmirror name x y z dx dy dz",
		  __FILE__,
		  transform,g);

  theCommands.Add("bscale",
		  "bscale name x y z scale",
		  __FILE__,
		  transform,g);

  theCommands.Add("precision",
		  "precision [preci]",
		  __FILE__,
		  precision,g);

  theCommands.Add("mkedgecurve",
		  "mkedgecurve name tolerance",
		  __FILE__,
		  mkedgecurve,g);

  theCommands.Add("fsameparameter",
		  "fsameparameter shapename [tol (default 1.e-7)], \nforce sameparameter on all edges of the shape",
		  __FILE__,
		  sameparameter,g);

  theCommands.Add("sameparameter",
		  "sameparameter shapename [tol (default 1.e-7)]",
		  __FILE__,
		  sameparameter,g);

  theCommands.Add("updatetolerance",
		  "updatetolerance myShape [param] \n  if [param] is absent - not verify of face tolerance, else - perform it",
		  __FILE__,
		  updatetol,g);

  theCommands.Add("solidorientation",
		  "orientsolid myClosedSolid",
		  __FILE__,
		  orientsolid,g);

  theCommands.Add("getcoords",
    "getcoords vertex1 vertex 2... ; shows coords of input vertices",
    __FILE__,
    getcoords,g);
  
  theCommands.Add("bounding",
		  "bounding shape [ xmin ymin zmin xmax ymax zmax] ; draw bounds",
		  __FILE__,
		  bounding,g);

  theCommands.Add("boundingstr",
		  "boundingstr shape [ xmin ymin zmin xmax ymax zmax] ; print bounding box",
		  __FILE__,
		  boundingstr,g);

  theCommands.Add("nurbsconvert",
		  "nurbsconvert result name [result name]",
		  __FILE__,
		  nurbsconvert,g);

  theCommands.Add("deform",
		  "deform newname name CoeffX CoeffY CoeffZ",
		  __FILE__,
		  deform,g);
  
  theCommands.Add("findplane",
		  "findplane name planename ",
		  __FILE__,
		  findplane,g) ;
  
  theCommands.Add("maxtolerance",
		  "maxtolerance shape ",
		  __FILE__,
		  maxtolerance,g) ;

  theCommands.Add("reperageshape",
		  "reperage shape -> list of shape (result of interstion shape , line)",
		  __FILE__,
		  reperageshape,g) ;

  theCommands.Add("vecdc",
		  "vecdc + Pointe double click ",
		  __FILE__,
		  vecdc,g) ;

  theCommands.Add("nproject","nproject pj e1 e2 e3 ... surf -g -d [dmax] [Tol [continuity [maxdeg [maxseg]]]",
                  __FILE__,
                  nproject,g);

  theCommands.Add("wexplo","wexplo wire [face] create WEDGE_i",
		  __FILE__,
		  wexplo,g);

  theCommands.Add("scalexyz",
                  "scalexyz res shape factor_x factor_y factor_z",
		  __FILE__,
		  scalexyz, g);
}
