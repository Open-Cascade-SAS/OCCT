// Created on: 1998-11-25
// Created by: Prestataire Xuan PHAM PHU
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


#include <BndLib_Add2dCurve.hxx>
#include <BRep_Builder.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_ListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepTool.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_C2DF.hxx>
#include <TopOpeBRepTool_CORRISO.hxx>
#include <TopOpeBRepTool_define.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_PURGE.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <TopTools_Array1OfShape.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepTool_GettraceCORRISO();
Standard_EXPORT TopTools_IndexedMapOfShape STATIC_PURGE_mapv;
Standard_EXPORT TopTools_IndexedMapOfOrientedShape STATIC_PURGE_mapeds;
extern void FUN_tool_trace(const Standard_Integer Index);
extern void FUN_tool_trace(const gp_Pnt2d p2d);
#endif

static void FUN_RaiseError()
{
#ifdef OCCT_DEBUG
//  Standard_Boolean trc = TopOpeBRepTool_GettraceCORRISO();
  FUN_REINIT(); 
//  if (trc) cout <<"*********failure in CORRISO***********\n";
#endif
}
static void FUN_Raise()
{
#ifdef OCCT_DEBUG
//  cout <<"*********failure in CORRISO***********\n";
#endif
}

#ifdef DRAW
#include <TopOpeBRepTool_DRAW.hxx>
#endif

#define M_FORWARD(sta)  (sta == TopAbs_FORWARD)
#define M_REVERSED(sta) (sta == TopAbs_REVERSED)
#define M_INTERNAL(sta) (sta == TopAbs_INTERNAL)
#define M_EXTERNAL(sta) (sta == TopAbs_EXTERNAL)

//=======================================================================
//function : TopOpeBRepTool_CORRISO
//purpose  : 
//=======================================================================

TopOpeBRepTool_CORRISO::TopOpeBRepTool_CORRISO()
{
}

//=======================================================================
//function : TopOpeBRepTool_CORRISO
//purpose  : 
//=======================================================================

TopOpeBRepTool_CORRISO::TopOpeBRepTool_CORRISO(const TopoDS_Face& Fref)
{
  myFref = Fref;

  FUN_tool_closedS(myFref,myUclosed,myUper,myVclosed,myVper);

  const Handle(Geom_Surface)& SU = BRep_Tool::Surface(myFref);
  myGAS = GeomAdaptor_Surface(SU);
}

//=======================================================================
//function : Fref
//purpose  : 
//=======================================================================

const TopoDS_Face& TopOpeBRepTool_CORRISO::Fref() const
{
  return myFref;
}

//=======================================================================
//function : GASref
//purpose  : 
//=======================================================================

const GeomAdaptor_Surface& TopOpeBRepTool_CORRISO::GASref() const 
{
  return myGAS;
}


//=======================================================================
//function : Refclosed
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_CORRISO::Refclosed(const Standard_Integer x, Standard_Real& xperiod) const
{
  if (x==1) {xperiod = myUper; return myUclosed;}
  if (x==2) {xperiod = myVper; return myVclosed;}
  return Standard_False;
}



//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_CORRISO::Init(const TopoDS_Shape& S)
{
#ifdef DRAW  
  Standard_Integer ie = 0;
  Standard_Boolean trc = TopOpeBRepTool_GettraceCORRISO();
  Standard_Boolean INIT = Standard_True;
  if (INIT) FUN_REINIT();
#endif

  myERep2d.Clear();
  myEds.Clear();
  myVEds.Clear();

  if (S.IsNull()) return Standard_False;
  myS = S;

  TopExp_Explorer ex(S, TopAbs_EDGE);
  for (; ex.More(); ex.Next()){
    const TopoDS_Edge& E = TopoDS::Edge(ex.Current());
#ifdef OCCT_DEBUG
    Standard_Integer iE = STATIC_PURGE_mapeds.Add(E);
    (void)iE; // avoid warning
    #ifdef DRAW
        if (trc) {TCollection_AsciiString aa = TCollection_AsciiString("e"); FUN_tool_draw(aa,E,iE);}
    #endif
#endif

    // myEds :
    myEds.Append(E);

    // myERep2d :
//    Standard_Real f,l,tol; Handle(Geom2d_Curve) PC = FC2D_CurveOnSurface(E,myFref,f,l,tol);
    Handle(Geom2d_Curve) PC; Standard_Real f,l,tol;
    Standard_Boolean hasold = FC2D_HasOldCurveOnSurface(E,myFref,PC);
    PC = FC2D_EditableCurveOnSurface(E,myFref,f,l,tol);
    if (!hasold) FC2D_AddNewCurveOnSurface(PC,E,myFref,f,l,tol);
    if (PC.IsNull()) return Standard_False;
    TopOpeBRepTool_C2DF C2DF(PC,f,l,tol,myFref);
    myERep2d.Bind(E,C2DF);
        
    // myVEds :
    TopExp_Explorer exv(E, TopAbs_VERTEX);
    for (; exv.More(); exv.Next()){
      const TopoDS_Vertex& v = TopoDS::Vertex(exv.Current());
#ifdef OCCT_DEBUG
      Standard_Integer aniE = STATIC_PURGE_mapeds.Add(E);
      (void)aniE; // avoid warning
      #ifdef DRAW
        if (trc) {TCollection_AsciiString bb = TCollection_AsciiString("v"); FUN_tool_draw(bb,v,iv);}
      #endif
#endif
      Standard_Boolean isb = myVEds.IsBound(v);
      if (isb) myVEds.ChangeFind(v).Append(E);
      else     {TopTools_ListOfShape loe; loe.Append(E); myVEds.Bind(v,loe);}
    }//exv
  }
  return Standard_True;
}

//=======================================================================
//function : S
//purpose  : 
//=======================================================================

const TopoDS_Shape& TopOpeBRepTool_CORRISO::S() const 
{
  return myS;
}

//=======================================================================
//function : Eds
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& TopOpeBRepTool_CORRISO::Eds() const 
{
  return myEds;
}

//=======================================================================
//function : UVRep
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_CORRISO::UVRep(const TopoDS_Edge& E, TopOpeBRepTool_C2DF& C2DF) const
{
  Standard_Boolean isb = myERep2d.IsBound(E);
  if (!isb) return Standard_False;
  
  C2DF = myERep2d.Find(E);
  return Standard_True;
}

//=======================================================================
//function : SetUVRep
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_CORRISO::SetUVRep(const TopoDS_Edge& E, const TopOpeBRepTool_C2DF& C2DF)
{
  Standard_Boolean isb = myERep2d.IsBound(E);
  if (!isb) return Standard_False;
  
  myERep2d.ChangeFind(E) = C2DF;
  return Standard_True;  
}

//=======================================================================
//function : Connexity
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_CORRISO::Connexity(const TopoDS_Vertex& V, TopTools_ListOfShape& Eds) const
{
  Standard_Boolean isb = myVEds.IsBound(V);
  if (!isb) return Standard_False;
  
  Eds = myVEds.Find(V);
  return Standard_True;
}

//=======================================================================
//function : SetConnexity
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_CORRISO::SetConnexity(const TopoDS_Vertex& V, const TopTools_ListOfShape& Eds) 
{
  Standard_Boolean isb = myVEds.IsBound(V);
  if (!isb) return Standard_False;
  
  myVEds.ChangeFind(V) = Eds;
  return Standard_True;
}

//=======================================================================
//function : UVClosed
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_CORRISO::UVClosed() const
{  
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceCORRISO();
  if (trc) cout<<"** UVClosed"<<endl;
#endif
  TopTools_DataMapOfOrientedShapeInteger lfyE; Standard_Integer nfybounds=3; Standard_Boolean stopatfirst = Standard_True;
  Standard_Boolean foundfaulty = EdgesWithFaultyUV(myEds,nfybounds,lfyE,stopatfirst);
  return !foundfaulty;
}

//=======================================================================
//function : Tol
//purpose  : 
//=======================================================================

Standard_Real TopOpeBRepTool_CORRISO::Tol(const Standard_Integer I, const Standard_Real tol3d) const
{
  Standard_Real tol = (I==1) ? myGAS.UResolution(tol3d) : myGAS.VResolution(tol3d);
  return tol;
}

//static Standard_Real FUN_getx(const TopoDS_Edge& E,
static Standard_Real FUN_getx(const TopoDS_Edge& ,
                              const TopOpeBRepTool_C2DF& c2df,
		              const Standard_Boolean uiso,
                              const Standard_Real par)
{ // prequesitory : E is uviso
  gp_Pnt2d uv = TopOpeBRepTool_TOOL::UVF(par,c2df);
  Standard_Real x = (uiso) ? uv.Y() : uv.X();
  return x;
}

//=======================================================================
//function : PurgeFyClosingE
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_CORRISO::PurgeFyClosingE(const TopTools_ListOfShape& ClEds, TopTools_ListOfShape& fyClEds) const
{
  fyClEds.Clear();
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceCORRISO();
  if (trc) cout<<"* PurgeFyClosingE"<<endl;
#endif
//  Standard_Real xperiod = myUclosed ? myUper : myVper; 
  Standard_Real tttol = 1.e-8;
  Standard_Real tttolS = BRep_Tool::Tolerance(myFref);
  Standard_Real tolu = Tol(1,tttolS), tolv = Tol(2,tttolS);
  Standard_Real tttuvF = Max(tolu,tolv);

  TopTools_IndexedMapOfOrientedShape mapcl;
  TopTools_ListIteratorOfListOfShape itce(ClEds);
  for (; itce.More(); itce.Next()) mapcl.Add(itce.Value());

  //* one closing edge should be removed     
  itce.Initialize(ClEds);
  TopTools_DataMapOfOrientedShapeInteger fyceds; Standard_Boolean found = EdgesWithFaultyUV(ClEds,3,fyceds);
  if (!found) return Standard_False;  

  if      (fyceds.Extent() == 1) {// ivf == 3 : cto016G*
    TopTools_DataMapOfOrientedShapeInteger fyeds;

    EdgesWithFaultyUV(myEds,3,fyeds);
    Standard_Integer nfy = fyeds.Extent();

    TopTools_DataMapIteratorOfDataMapOfOrientedShapeInteger itm(fyceds);
    const TopoDS_Edge& cE = TopoDS::Edge(itm.Key());

    TopAbs_Orientation OocE = TopAbs::Complement(cE.Orientation());
    Standard_Boolean isoncE = mapcl.Contains(cE.Oriented(OocE));
    if (isoncE) {
      TopTools_Array1OfShape vcE(1,2); TopOpeBRepTool_TOOL::Vertices(cE,vcE); 
      TopAbs_Orientation ocE = cE.Orientation();       
      Standard_Real tttolcE = BRep_Tool::Tolerance(cE);
      Standard_Real tttuvcE = Max(Tol(1,tttolcE),Tol(2,tttolcE));
      TopOpeBRepTool_C2DF cE2d; Standard_Boolean isb = UVRep(cE,cE2d);
      if (!isb) return Standard_False; // NYIRAISE
  
      // isonOcE2d :
      // OcE (closing edge with complemented orientation):
      TopAbs_Orientation oOcE = TopAbs::Complement(ocE);
      TopoDS_Shape alocalShape = cE.Oriented(oOcE);     
      TopoDS_Edge OcE = TopoDS::Edge(alocalShape);
//      TopoDS_Edge OcE = TopoDS::Edge(cE.Oriented(oOcE));
      TopTools_Array1OfShape vOcE(1,2); TopOpeBRepTool_TOOL::Vertices(OcE,vOcE); 
      Standard_Real tttolOcE = BRep_Tool::Tolerance(OcE);
      Standard_Real tttuvOcE = Max(Tol(1,tttolOcE),Tol(2,tttolOcE));
      TopOpeBRepTool_C2DF OcE2d; Standard_Boolean isOb = UVRep(OcE,OcE2d);
      if (!isOb) return Standard_False; // NYIRAISE
      
      Standard_Real parvce1 = TopOpeBRepTool_TOOL::ParE(1,cE);   gp_Pnt2d UVvce1 = TopOpeBRepTool_TOOL::UVF(parvce1,cE2d);

      Standard_Real parvOcE2 = TopOpeBRepTool_TOOL::ParE(2,OcE); gp_Pnt2d UVvOcE2 = TopOpeBRepTool_TOOL::UVF(parvOcE2,OcE2d);
      Standard_Real tol = Max(tttuvcE,tttuvOcE);
      isoncE = (UVvce1.Distance(UVvOcE2) < tol);
      if (isoncE && (nfy != 1)) {// cto009L2
	    return Standard_False; 
      }
    }

    Standard_Integer ivf = itm.Value();
    if (ivf == 3) {
      fyClEds.Append(cE); 
      return Standard_True;
    }
  }
  else if (fyceds.Extent() > 1) {// ivf == 1,2 : cto016E*
    // if {edges of fyceds} describe a closing edge with its first and last 
    // uvbounds non connexed -> we do remove these edges
    Standard_Boolean hasinit=Standard_False; Standard_Boolean isou=Standard_False,isov=Standard_False;
    gp_Pnt2d o2d; gp_Dir2d d2d; 
    Standard_Real xinf=1.e7,  xsup=-1.e7; // faulty inf and sup bounds
    Standard_Boolean infdef=Standard_False, supdef=Standard_False;
    TopTools_DataMapIteratorOfDataMapOfOrientedShapeInteger itm(fyceds);
    for (; itm.More(); itm.Next()){
      const TopoDS_Edge& cE = TopoDS::Edge(itm.Key());
      TopOpeBRepTool_C2DF c2df; Standard_Boolean isb = UVRep(cE,c2df);
      if (!isb) return Standard_False; // NYIRAISE

      Standard_Integer ivf = itm.Value();
      Standard_Boolean isoux,isovx; gp_Pnt2d o2dx; gp_Dir2d d2dx;
      Standard_Boolean uvisox = TopOpeBRepTool_TOOL::UVISO(c2df,isoux,isovx, d2dx, o2dx);
      if (!uvisox) return Standard_False;
      
      if (hasinit) {
	Standard_Boolean onsamline = (isou && isoux) || (isov && isovx);
	if (!onsamline) return Standard_False;
      }
      if (!hasinit) {
	isou=isoux; isov=isovx; 
	o2d=o2dx; d2d=d2dx;
	hasinit = Standard_True;
      }
      else {
	Standard_Boolean onsamline = Standard_False;
	if (isou && isoux) {
	  Standard_Real du = o2d.X()-o2dx.X();
	  onsamline = (Abs(du) < tolu);
	}
	if (isov && isovx) {
	  Standard_Real dv = o2d.Y()-o2dx.Y();
	  onsamline = (Abs(dv) < tolv);
	}
	if (!onsamline) return Standard_False;
      }        
      for (Standard_Integer i = 1; i <=2; i++) {
	Standard_Real pari = TopOpeBRepTool_TOOL::ParE(i,cE);
	Standard_Real xi = FUN_getx(cE,c2df,isou,pari);
	Standard_Boolean vifaulty = (ivf == i || ivf == 3);	
	Standard_Boolean inff = (xi < xinf);
	Standard_Boolean supl = (xi > xsup);
//	if (inff) xinf = (ivf == i || ivf == 3) ? xi : 1.e7;
//	if (supl) xsup = (ivf == i || ivf == 3) ? xi : -1.e7;
	if (inff) {xinf = xi; infdef = vifaulty;}
	if (supl) {xsup = xi; supdef = vifaulty;}
      }      
      fyClEds.Append(cE);
    }//itm
    Standard_Boolean toremove = infdef && supdef; // ie infx,supx are not "uv-connexed"
    if (!toremove) fyClEds.Clear();
  }
  if (!fyClEds.IsEmpty()) return Standard_True; // keeping only one closing edge

  //* the 2 closing edges have they 2drep "confunded"
  itce.Initialize(ClEds);
  for (; itce.More(); itce.Next()){
    // cE : 
    const TopoDS_Edge& cE = TopoDS::Edge(itce.Value());
    TopTools_Array1OfShape vcE(1,2); TopOpeBRepTool_TOOL::Vertices(cE,vcE); 
    TopAbs_Orientation ocE = cE.Orientation(); 

    Standard_Real tttolcE = BRep_Tool::Tolerance(cE);
    Standard_Real tttuvcE = Max(Tol(1,tttolcE),Tol(2,tttolcE));
    TopOpeBRepTool_C2DF cE2d; Standard_Boolean isb = UVRep(cE,cE2d);
    if (!isb) return Standard_False; // NYIRAISE
#ifdef OCCT_DEBUG
    Standard_Integer icE = STATIC_PURGE_mapeds.Add(cE);
    if (trc) cout<<"? e"<<icE<<" :"<<endl;
#endif

    // isonOcE2d :
    Standard_Boolean isonOcE2d = Standard_False;
    {
      // OcE (closing edge with complemented orientation):
      TopAbs_Orientation oOcE = TopAbs::Complement(ocE);
      TopoDS_Shape aLocalShape = cE.Oriented(oOcE);
      TopoDS_Edge OcE = TopoDS::Edge(aLocalShape);
//      TopoDS_Edge OcE = TopoDS::Edge(cE.Oriented(oOcE));
      TopTools_Array1OfShape vOcE(1,2); TopOpeBRepTool_TOOL::Vertices(OcE,vOcE); 
      Standard_Boolean hasOcE = mapcl.Contains(OcE);
      if (!hasOcE) continue; // closing edge appears twice
      Standard_Real tttolOcE = BRep_Tool::Tolerance(OcE);
      Standard_Real tttuvOcE = Max(Tol(1,tttolOcE),Tol(2,tttolOcE));
      TopOpeBRepTool_C2DF OcE2d; Standard_Boolean isOb = UVRep(OcE,OcE2d);
      if (!isOb) return Standard_False; // NYIRAISE

      Standard_Real parvce1 = TopOpeBRepTool_TOOL::ParE(1,cE);   gp_Pnt2d UVvce1 = TopOpeBRepTool_TOOL::UVF(parvce1,cE2d);

      Standard_Real parvOcE2 = TopOpeBRepTool_TOOL::ParE(2,OcE); gp_Pnt2d UVvOcE2 = TopOpeBRepTool_TOOL::UVF(parvOcE2,OcE2d);
      Standard_Real tol = Max(tttuvcE,tttuvOcE);
      isonOcE2d = (UVvce1.Distance(UVvOcE2) < tol);
    }
    if (!isonOcE2d) {
#ifdef OCCT_DEBUG
      if (trc) cout<<"-> valid edge"<<endl;
#endif
      continue;
    }

    Standard_Integer nvcEok = 0;   
    for (Standard_Integer ivce = 1; ivce <=2; ivce++) {
      // <vce> (boundary of <cE>):   
      const TopoDS_Vertex& vce = TopoDS::Vertex(vcE(ivce)); 
      TopTools_ListOfShape loe; isb = Connexity(vce,loe);

      if (!isb) return Standard_False; // NYIRAISE

      Standard_Real parvce = TopOpeBRepTool_TOOL::ParE(ivce,cE); gp_Pnt2d UVvce = TopOpeBRepTool_TOOL::UVF(parvce,cE2d);
#ifdef OCCT_DEBUG
      // recall in one wire, there are 2 vertices for one non-degenerated closing edge
      Standard_Integer ivmapv = STATIC_PURGE_mapv.Add(vce);
      if (trc) {cout<<" connexity for v("<<ivce<<")=v"<<ivmapv;FUN_tool_trace(UVvce);}
#ifdef DRAW	
      if (trc) {TCollection_AsciiString bb("uv_");bb += TCollection_AsciiString(ivmapv);FUN_tool_draw(bb,UVvce);}
#endif
#endif	      
      Standard_Real tttolvce = BRep_Tool::Tolerance(vce); 
      Standard_Real tttuvvce = Max(Tol(1,tttolvce),Tol(2,tttolvce));

      Standard_Boolean vceok = Standard_False;
      for (TopTools_ListIteratorOfListOfShape ite(loe); ite.More(); ite.Next()) {
	const TopoDS_Edge& E = TopoDS::Edge(ite.Value());

#ifdef OCCT_DEBUG
	Standard_Integer iE = STATIC_PURGE_mapeds.Add(E);
	if (trc) {cout<<"    : on e"<<iE<<endl;}
#endif
//	if (E.IsSame(cE)) continue;
	if (mapcl.Contains(E)) continue; // do NOT check connexity on closing edges 
	                                  // xpu090399 cto016E1

	TopOpeBRepTool_C2DF E2d; Standard_Boolean isB2 = UVRep(E,E2d);
	if (!isB2) return Standard_False; // NYIRAISE
	
	Standard_Real tttolE = BRep_Tool::Tolerance(E);
	Standard_Real tttuvE = Max(Tol(1,tttolE),Tol(2,tttolE));
	
	TopTools_Array1OfShape vE(1,2); TopOpeBRepTool_TOOL::Vertices(E,vE);
	for (Standard_Integer ive = 1; ive <=2; ive++) {	 

	  const TopoDS_Vertex& ve = TopoDS::Vertex(vE(ive));		  
	  Standard_Boolean samev = ve.IsSame(vce);
	  if (!samev) continue; 
	  Standard_Real parve = TopOpeBRepTool_TOOL::ParE(ive,E); gp_Pnt2d UVve = TopOpeBRepTool_TOOL::UVF(parve,E2d);
#ifdef OCCT_DEBUG
	  if (trc) {cout<<"    ve("<<ive<<")";FUN_tool_trace(UVve);}
#endif 
	  if (ive == ivce) continue; // vertex FORWARD connexed to REVERSED one
	  Standard_Real tttolve = BRep_Tool::Tolerance(ve);
	  Standard_Real tttuvve = Max(Tol(1,tttolve),Tol(2,tttolve));
	  
	  tttol = Max(tttol,Max(tttuvF,Max(tttuvE,Max(tttuvcE,Max(tttuvve,tttuvvce)))));
//	  Standard_Real dd = myUclosed ? (UVve.X()-UVvce.X()) : (UVve.Y()-UVvce.Y());
//	  Standard_Boolean xok = (Abs(dd)<tttol) || (Abs(Abs(dd)-xperiod)<tttol);
//	  if (xok) {
	  Standard_Real dd = UVve.Distance(UVvce);
	  Standard_Boolean sameuv = (dd < tttol);
	  if (myUclosed) {	    
	    Standard_Real xperiod = myUper;
	    dd = (UVve.X()-UVvce.X());
	    sameuv = sameuv || (Abs(Abs(dd)-xperiod)<tttol);
	  }
	  if (myVclosed) {	    
	    Standard_Real xperiod = myVper;
	    dd = (UVve.Y()-UVvce.Y());
	    sameuv = sameuv || (Abs(Abs(dd)-xperiod)<tttol);
	  }
	  if (sameuv) {
	    vceok = Standard_True;
#ifdef OCCT_DEBUG
	    if (trc){cout<<" connexity->ok"<<endl;}
#endif	
	  }
	  break;
	} // ive=1..2
	if (vceok) break;	
      } //ite(loe)     
 
#ifdef OCCT_DEBUG
      if (trc && !vceok) {cout<<" connexity->KO"<<endl;}	
#endif     
      if (vceok) nvcEok++;
    }// ivce=1..2

    Standard_Boolean isfycE = (nvcEok == 0); // each bound is not connexed to any non-closed edge

#ifdef OCCT_DEBUG
    if (trc) 
      {if (isfycE) cout<<"-> faulty edge"<<endl; 
       else        cout<<"-> valid edge"<<endl;}
#endif 
    if (isfycE) fyClEds.Append(cE);
  }//itce
  return (!fyClEds.IsEmpty());
}

#define SPLITEDGE (0)
#define INCREASE  (1)
#define DECREASE (-1)

static Standard_Integer FUN_tool_recadre(const Standard_Real minx,const Standard_Real maxx,
			    const Standard_Real xfirst,const Standard_Real xlast,const Standard_Real tolx,
			    Standard_Boolean& maxsup)
{
  Standard_Integer recadre = 10; // INIT  
  Standard_Boolean maxinf = (maxx < xfirst+tolx);    // permissive
  Standard_Boolean mininf   = (minx   < xfirst-tolx);// 
  maxsup = (maxx > xlast+tolx);     // 
  Standard_Boolean minsup   = (minx   > xlast-tolx); // permissive
  Standard_Boolean maxok  = (xfirst-tolx < maxx) && (maxx < xlast+tolx);// permissive
  Standard_Boolean minok    = (xfirst-tolx < minx) && (minx < xlast+tolx);    // permissive
  
  if      (maxinf)          recadre = INCREASE;
  else if (minsup)          recadre = DECREASE;
  else if (mininf && maxok) recadre = SPLITEDGE;
  else if (minok && maxsup) recadre = SPLITEDGE;
  return recadre;
}

//=======================================================================
//function : EdgeOUTofBoundsUV
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepTool_CORRISO::EdgeOUTofBoundsUV(const TopoDS_Edge& E, const Standard_Boolean onU, const Standard_Real tolx, 
					      Standard_Real& parspE) const 
{
  Standard_Integer recadre = 10; parspE = -1.e7; // INIT
  Standard_Integer isb = myERep2d.IsBound(E);
  if (!isb) return Standard_False;
  
  const TopOpeBRepTool_C2DF& C2DF = myERep2d.Find(E);
  Standard_Real f,l,tol; const Handle(Geom2d_Curve)& PC = C2DF.PC(f,l,tol);
 
  Standard_Real xfirst  = onU ? myGAS.FirstUParameter() : myGAS.FirstVParameter();
  Standard_Real xlast   = onU ? myGAS.LastUParameter() : myGAS.LastVParameter(); // xlast=xfirst+xperiod
  Standard_Real xperiod = onU ? myUper : myVper;

  Standard_Boolean isou,isov; gp_Pnt2d o2d; gp_Dir2d d2d; 
  Standard_Boolean iso = TopOpeBRepTool_TOOL::UVISO(PC,isou,isov,d2d,o2d);

  if (iso) { // 2drep(E,myFref) is an ISO
             // -------------------------
    Standard_Boolean inX = (onU && isou) || ((!onU) && isov);
    if (inX) {
      // faulty u-iso : upar out of ubound
      Standard_Real xpar = onU ? o2d.X() : o2d.Y(); 
      Standard_Boolean toosmall = (xpar < xfirst-tolx); 
      Standard_Boolean tobig = (xpar > xfirst+xperiod+tolx);
      
      if (toosmall) recadre = INCREASE;
      if (tobig)    recadre = DECREASE;
      return recadre;
    } // inX
    Standard_Boolean inY = (onU && isov) || ((!onU) && isou); // inY = !inX
    if (inY) {
      // faulty u-iso : vpar describes (minv,maxv) out of vbounds
      // PC describes [minx,maxx] in x-space 
      // recadre = INCREASE : maxx < 0.
      //           DECREASE : minx > 2PI
      //           SPLITEDGE : minx<0.<maxx || minx<2PI<maxx
      Standard_Real d2ddir = onU? d2d.Y(): d2d.X();
      Standard_Boolean reverse = (d2ddir < 0.); Standard_Real xfactor = reverse? -1.: 1.;
      Standard_Real max = reverse? f: l;
      Standard_Real min   = reverse? l: f;
      gp_Pnt2d maxuv = PC->Value(max); 
      gp_Pnt2d minuv = PC->Value(min); 

      Standard_Real maxx = onU? maxuv.X(): maxuv.Y();
      Standard_Real minx = onU? minuv.X(): minuv.Y();

      Standard_Boolean maxsup;
      recadre = FUN_tool_recadre(minx,maxx,xfirst,xlast,tolx,
				 maxsup);      
      if (recadre == SPLITEDGE) {
	Standard_Real xbound = maxsup? xperiod: 0.;
	parspE = max - xfactor*(maxx-xbound);
      }      
      return recadre;
    } // inY
  } // iso
  else { // 2drep(E, myFref) is NOT an iso
         // ------------------------------
    Bnd_Box2d Bn2d;
    Geom2dAdaptor_Curve GC2d(PC,f,l);
    Standard_Real tolE = BRep_Tool::Tolerance(E);
    Standard_Real toladd = Max(tolE,tol);
    BndLib_Add2dCurve::Add(GC2d,toladd,Bn2d);
    Standard_Real umin,vmin,umax,vmax; Bn2d.Get(umin,vmin,umax,vmax);
    Standard_Real xmin = onU ? umin : vmin;
    Standard_Real xmax = onU ? umax : vmax;
    Standard_Boolean maxsup;
    recadre = FUN_tool_recadre(xmin,xmax,xfirst,xlast,tolx,
               		       maxsup);  
    if (recadre == SPLITEDGE) {
      // ================================================================
      //NYIxpu271198 : intersection PC avec xiso (x= maxsup? xperiod: 0.)
      // ================================================================
      return 10;
    }  
    return recadre;
  }  
  return recadre;
}

//=======================================================================
//function : EdgesOUTofBoundsUV
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_CORRISO::EdgesOUTofBoundsUV(const TopTools_ListOfShape& EdsToCheck, const Standard_Boolean onU, const Standard_Real tolx, 
					       TopTools_DataMapOfOrientedShapeInteger & FyEds) const 
{
  FyEds.Clear();
  TopTools_ListIteratorOfListOfShape it(EdsToCheck);
  for (; it.More(); it.Next()){
    const TopoDS_Edge& E = TopoDS::Edge(it.Value());
    Standard_Real sspar = -1.e7;
    Standard_Integer recadre = EdgeOUTofBoundsUV(E,onU,tolx,sspar);
    if (recadre == SPLITEDGE) FUN_Raise();
    if (recadre == INCREASE)  FyEds.Bind(E,1);
    if (recadre == DECREASE)  FyEds.Bind(E,-1);
  }
  return (!FyEds.IsEmpty());
}


//=======================================================================
//function : EdgeWithFaultyUV
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_CORRISO::EdgeWithFaultyUV(const TopoDS_Edge& E, Standard_Integer& Ivfaulty) const 
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceCORRISO();
  Standard_Integer iE = STATIC_PURGE_mapeds.Add(E);
  if (trc) cout<<"? e"<<iE<<" :"<<endl;
#endif
  Ivfaulty = 0;
  Standard_Real tttol = 1.e-8;
  Standard_Real tttolF = BRep_Tool::Tolerance(TopoDS::Face(myFref));
  Standard_Real tttuvF = Max(Tol(1,tttolF),Tol(2,tttolF));
  Standard_Real tttolE = BRep_Tool::Tolerance(E);
  Standard_Real tttuvE = Max(Tol(1,tttolE),Tol(2,tttolE));

  TopAbs_Orientation oE = E.Orientation();
  if (M_INTERNAL(oE) || M_EXTERNAL(oE)) return Standard_False;
    
  TopTools_Array1OfShape vEs(1,2); TopOpeBRepTool_TOOL::Vertices(E, vEs);
  Standard_Boolean closed = vEs(1).IsSame(vEs(2));
  if (closed) {
#ifdef OCCT_DEBUG
    if (trc) {cout<<"closed -> valid edge"<<endl;}
#endif     
    return Standard_False; // closed edge is assumed valid
  }
  
  Standard_Integer nfyv = 0;
  for (Standard_Integer ivE = 1; ivE <=2; ivE++) {
    
    // <vE> (boundary of <E>):      
    const TopoDS_Vertex& vE = TopoDS::Vertex(vEs(ivE)); 
    Standard_Real parvE = TopOpeBRepTool_TOOL::ParE(ivE,E);
    TopOpeBRepTool_C2DF C2DF; Standard_Boolean isb = UVRep(E,C2DF);
    if (!isb) return Standard_False; //NYIRAISE
    gp_Pnt2d UVvE = TopOpeBRepTool_TOOL::UVF(parvE,C2DF);
#ifdef OCCT_DEBUG
      // recall in one wire, there are 2 vertices for one non-degenerated closing edge
    Standard_Integer ivmapv = STATIC_PURGE_mapv.Add(vE);
    if (trc) {cout<<" connexity for v("<<ivE<<")=v"<<ivmapv;FUN_tool_trace(UVvE);}
#ifdef DRAW	
    if (trc) {TCollection_AsciiString bb("uv_");bb += TCollection_AsciiString(ivmapv);FUN_tool_draw(bb,UVvE);}
#endif
#endif	
    
    Standard_Real tttolvE = BRep_Tool::Tolerance(vE);
    Standard_Real tttuvvE = Max(Tol(1,tttolvE),Tol(2,tttolvE));
    
    Standard_Boolean isbound = myVEds.IsBound(vE);
    if (!isbound) {FUN_RaiseError(); return Standard_False;}
    
    // <vEok> :
    Standard_Boolean vEok = Standard_False;
    const TopTools_ListOfShape& loe = myVEds.Find(vE);

    for (TopTools_ListIteratorOfListOfShape ite(loe); ite.More(); ite.Next()) {
      const TopoDS_Edge& e = TopoDS::Edge(ite.Value());
      TopAbs_Orientation oe = e.Orientation();

#ifdef OCCT_DEBUG
    Standard_Integer ie = STATIC_PURGE_mapeds.Add(e);
    if (trc) {cout<<"    : on e"<<ie<<endl;}
#endif

      if (e.IsSame(E)) continue;      
      if (M_INTERNAL(oe) || M_EXTERNAL(oe)) continue;
      
      Standard_Boolean isBound = myERep2d.IsBound(e);
      if (!isBound) {FUN_RaiseError(); return Standard_False;}
      const TopOpeBRepTool_C2DF& aC2DF = myERep2d.Find(e);
      
      TopTools_Array1OfShape ves(1,2); TopOpeBRepTool_TOOL::Vertices(e,ves);	
      for (Standard_Integer ive = 1; ive <=2; ive++) {	  
	const TopoDS_Vertex& ve = TopoDS::Vertex(ves(ive));	  
	Standard_Boolean samev = ve.IsSame(vE);
	if (!samev) continue;
	  
	Standard_Real pare = TopOpeBRepTool_TOOL::ParE(ive,e); gp_Pnt2d UVve = TopOpeBRepTool_TOOL::UVF(pare,aC2DF);
#ifdef OCCT_DEBUG
	if (trc) {cout<<"    ve("<<ive<<")";FUN_tool_trace(UVve);}
#endif 
	if (ive == ivE) continue;	
	
	Standard_Real tttolve = BRep_Tool::Tolerance(ve);
	Standard_Real tttuvve = Max(Tol(1,tttolve),Tol(2, tttolve));
	
	tttol = Max(tttol,Max(tttuvF,Max(tttuvE,Max(tttuvE,Max(tttuvve,tttuvvE)))));
	Standard_Boolean isequal = UVvE.IsEqual(UVve,tttol);
	if (isequal) {
	  vEok = Standard_True;
#ifdef OCCT_DEBUG
	  if (trc){cout<<" connexity->ok"<<endl;}
#endif	
	  break;
	}
      } // ive	
      if (vEok) break;
    } // ite(loe)
      
    if (!vEok) {nfyv++; Ivfaulty = ivE;}      
#ifdef OCCT_DEBUG
    if (trc && !vEok) {cout<<" connexity->KO"<<endl;}	
#endif      

  } // ivE = 1..2          
  if (nfyv == 2) Ivfaulty = 3;
#ifdef OCCT_DEBUG
  if (trc) {if (Ivfaulty == 0) cout<<"-> valid edge"<<endl; else cout<<"-> faulty edge"<<endl;}
#endif   
  return (Ivfaulty != 0);
}

//=======================================================================
//function : EdgesWithFaultyUV
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_CORRISO::EdgesWithFaultyUV(const TopTools_ListOfShape& EdsToCheck, const Standard_Integer nfybounds,
					      TopTools_DataMapOfOrientedShapeInteger& FyEds, const Standard_Boolean stopatfirst) const 
{
  FyEds.Clear();
#ifdef OCCT_DEBUG
  Standard_Integer ifault = 0;
  Standard_Boolean trc = TopOpeBRepTool_GettraceCORRISO(); 
  if (trc) cout<<endl<<"* EdgesWithFaultyUV"<<endl;
#endif

  // fF's checking :
  // -----------------
  // An edge is valid if the first and last vertices are valid:
  // vertex <vEchk> is valid if there is an edge with bound <ve> such that :
  //   <vEchk> and <ve> share same UV geometry
  //   <vEchk> and <ve> are of opposite orientation.   
  TopTools_ListIteratorOfListOfShape itchk(EdsToCheck);
  for (; itchk.More(); itchk.Next()) {

    const TopoDS_Edge& Echk = TopoDS::Edge(itchk.Value());    
    Standard_Integer Ivfaulty=0; Standard_Boolean faulty = EdgeWithFaultyUV(Echk,Ivfaulty);
    if (!faulty) continue;
    Standard_Integer nfyv = (Ivfaulty == 3)? 2 : 1;

#ifdef OCCT_DEBUG
    ifault++; 
    if (trc) cout<<"e"<<STATIC_PURGE_mapeds.FindIndex(Echk)<<" has ifyv="<<Ivfaulty<<endl;
#ifdef DRAW
    if (trc) {TCollection_AsciiString aa("fault"); FUN_tool_draw(aa,Echk,ifault);}
#endif
#endif

    Standard_Boolean found = Standard_False;
    if      (nfybounds == 1) found = (nfyv == nfybounds);
    else if (nfybounds == 2) found = (nfyv == nfybounds);
    else if (nfybounds == 3) found = (nfyv > 0);

    if (found) { 
      FyEds.Bind(Echk,Ivfaulty);
      if (stopatfirst) return Standard_True;
    }
  } // itchk
  Standard_Integer n = FyEds.Extent(); // DEB
  return (n != 0);   
}

//=======================================================================
//function : EdgeWithFaultyUV
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_CORRISO::EdgeWithFaultyUV(const TopTools_ListOfShape& EdsToCheck, const Standard_Integer nfybounds,
					     TopoDS_Shape& fyE, Standard_Integer& Ifaulty) const 
{
  TopTools_DataMapOfOrientedShapeInteger FyEds;
  Standard_Boolean found = EdgesWithFaultyUV(EdsToCheck,nfybounds,FyEds,Standard_True);
  if (!found) return Standard_False;

  TopTools_DataMapIteratorOfDataMapOfOrientedShapeInteger itm(FyEds);
  fyE = itm.Key();
  Ifaulty = itm.Value();
  return Standard_True;
}

//=======================================================================
//function : TrslUV
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_CORRISO::TrslUV(const Standard_Boolean onU, const TopTools_DataMapOfOrientedShapeInteger& FyEds)
{
  gp_Vec2d tt2d; 
  if (onU) {Standard_Real uper;
                     Refclosed(1,uper);
            if (!uper) return Standard_False;
	    tt2d = gp_Vec2d(uper,0.);}
  else     {Standard_Real vper;
                     Refclosed(2,vper);
            if (!vper) return Standard_False;
	    tt2d = gp_Vec2d(0.,vper);}
  TopTools_DataMapIteratorOfDataMapOfOrientedShapeInteger itm(FyEds);
  for (; itm.More(); itm.Next()){
    const TopoDS_Edge& E = TopoDS::Edge(itm.Key());
    TopOpeBRepTool_C2DF C2DF; Standard_Boolean isb = UVRep(E,C2DF);
    if (!isb) return Standard_False;

    Standard_Integer itt = itm.Value();
    if      (itt == SPLITEDGE) return Standard_False;
    else if (itt == INCREASE) TopOpeBRepTool_TOOL::TrslUV(tt2d,C2DF);
    else if (itt == DECREASE) TopOpeBRepTool_TOOL::TrslUV(tt2d.Multiplied(-1.),C2DF);
    else return Standard_False;
    SetUVRep(E,C2DF);
  }
  return Standard_True;
}

// modif in BRep_Builder031298
/*static void FUN_tool_correctdgE(const TopoDS_Edge& E)
{
  const Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*) &E.TShape());
  BRep_ListOfCurveRepresentation& lcr = TE->ChangeCurves();
  BRep_ListIteratorOfListOfCurveRepresentation itcr(lcr);
  while (itcr.More()) {
    Handle(BRep_GCurve) GC = Handle(BRep_GCurve)::DownCast(itcr.Value());
    if (!GC.IsNull()) {
      if (GC->IsCurve3D()) lcr.Remove(itcr);
      else itcr.Next();
    }
  }
}*/

//=======================================================================
//function : GetnewS
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_CORRISO::GetnewS(TopoDS_Face& newS) const 
{
  newS.Nullify();
  if (myS.ShapeType() != TopAbs_FACE) return Standard_False;

  newS = TopoDS::Face(myS);
  BRep_Builder BB;

  TopTools_ListIteratorOfListOfShape it(myEds);
  for (; it.More(); it.Next()){
    TopoDS_Edge E = TopoDS::Edge(it.Value());   
    TopAbs_Orientation oriE = E.Orientation();
    TopOpeBRepTool_C2DF C2DF; Standard_Boolean isb = UVRep(E,C2DF);
    if (!isb) return Standard_False;

    Standard_Real f,l,tol;const Handle(Geom2d_Curve)& PC = C2DF.PC(f,l,tol);
    Handle(Geom2d_TrimmedCurve) cu = new Geom2d_TrimmedCurve(PC,f,l);
    
    TopoDS_Shape aLocalShape = E.Oriented(TopAbs::Complement(oriE));
    TopoDS_Edge Err = TopoDS::Edge(aLocalShape);
//    TopoDS_Edge Err = TopoDS::Edge(E.Oriented(TopAbs::Complement(oriE)));
    TopOpeBRepTool_C2DF C2DFrr; Standard_Boolean isclo = UVRep(Err,C2DFrr);
    
//    Standard_Boolean isdgE = BRep_Tool::Degenerated(E);
    // !BUC60380 : degenerated edge has a 3d curve !!, remove it
//    if (isdgE) {FUN_tool_correctdgE(E);}     
		
    if (isclo) {
      Standard_Real frr,lrr,tolrr;const Handle(Geom2d_Curve)& PCrr = C2DFrr.PC(frr,lrr,tolrr);
      Handle(Geom2d_TrimmedCurve) curr = new Geom2d_TrimmedCurve(PCrr,frr,lrr);
      if (M_FORWARD(oriE)) BB.UpdateEdge(E,cu,curr,newS,tol);
    }
    else BB.UpdateEdge(E,cu,newS,tol);
  }
  return Standard_True;
}

//=======================================================================
//function : AddNewConnexity
//purpose  : 
//=======================================================================

//Standard_Boolean TopOpeBRepTool_CORRISO::AddNewConnexity(const TopoDS_Vertex& V,
Standard_Boolean TopOpeBRepTool_CORRISO::AddNewConnexity(const TopoDS_Vertex& ,
                                                         const TopoDS_Edge& E)
{
  // <myERep2d> : 
  Standard_Boolean isb = myERep2d.IsBound(E);
  if (!isb) {
    Handle(Geom2d_Curve) PC; Standard_Real f,l,tol;
    Standard_Boolean hasold = FC2D_HasOldCurveOnSurface(E,myFref,PC);
    PC = FC2D_EditableCurveOnSurface(E,myFref,f,l,tol);
    if (!hasold) FC2D_AddNewCurveOnSurface(PC,E,myFref,f,l,tol);
    if (PC.IsNull()) return Standard_False;
    TopOpeBRepTool_C2DF C2DF(PC,f,l,tol,myFref);
    myERep2d.Bind(E,C2DF);  
  }

  // <myEds> : 
  if (!isb) myEds.Append(E);

  // <myVEds> :
  TopExp_Explorer exv(E, TopAbs_VERTEX);
  for (; exv.More(); exv.Next()){
    const TopoDS_Vertex& v = TopoDS::Vertex(exv.Current()); 
    Standard_Boolean isbb = myVEds.IsBound(v);
    if (isbb) myVEds.ChangeFind(v).Append(E);
    else      {TopTools_ListOfShape loe; loe.Append(E); myVEds.Bind(v,loe);}
  }//exv
  return Standard_True;
  
}

//=======================================================================
//function : RemoveOldConnexity
//purpose  : 
//=======================================================================

//Standard_Boolean TopOpeBRepTool_CORRISO::RemoveOldConnexity(const TopoDS_Vertex& V,
Standard_Boolean TopOpeBRepTool_CORRISO::RemoveOldConnexity(const TopoDS_Vertex& ,
                                                            const TopoDS_Edge& E)
{
  // <myERep2d> :
  Standard_Boolean isb = myERep2d.IsBound(E);
  if (isb) myERep2d.UnBind(E);

  // <myEds> : 
  if (isb) {
    TopTools_ListIteratorOfListOfShape it(myEds);
    while (it.More()) {
      if (it.Value().IsEqual(E)) {myEds.Remove(it);break;}
      else                       it.Next();
    }
  }

  // <myVEds> :
  Standard_Boolean done = Standard_False;
  TopExp_Explorer exv(E, TopAbs_VERTEX);
  for (; exv.More(); exv.Next()){
    const TopoDS_Vertex& v = TopoDS::Vertex(exv.Current()); 
    Standard_Boolean isBoundV = myVEds.IsBound(v); 
    if (!isBoundV) return Standard_False;
    TopTools_ListOfShape& loe = myVEds.ChangeFind(v);
    TopTools_ListIteratorOfListOfShape ite(loe);
    while (ite.More()) {
      if (ite.Value().IsEqual(E)) {done = Standard_True; loe.Remove(ite);break;}
      else                         ite.Next();
    }
  }//exv
  return done;
}

