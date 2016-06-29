// Created on: 1996-03-07
// Created by: Jean Yves LEBEY
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


#include <gp_Pnt.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_EdgeBuilder.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_SurfaceIterator.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>

#include <stdio.h>
#ifdef DRAW
#include <DBRep.hxx>
#endif

#include <TopOpeBRepBuild_GIter.hxx>
#include <TopOpeBRepBuild_GTool.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_Config.hxx>
#include <TopOpeBRepDS_Dumper.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>

#ifdef OCCT_DEBUG
static TCollection_AsciiString PRODINS("dins ");
#endif

//=======================================================================
//function : GdumpLS
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpLS(const TopTools_ListOfShape& L) const 
{
  TopTools_ListIteratorOfListOfShape it(L);
  for (; it.More(); it.Next() ) {
    const TopoDS_Shape& SL = it.Value();
    GdumpSHA(SL);
  }
}
#else
void TopOpeBRepBuild_Builder::GdumpLS(const TopTools_ListOfShape&) const 
{
}
#endif

//=======================================================================
//function : PrintGeo
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::PrintGeo(const TopoDS_Shape& S)
{
  if      (S.ShapeType() == TopAbs_VERTEX) PrintPnt(TopoDS::Vertex(S));
  else if (S.ShapeType() == TopAbs_EDGE)   PrintCur(TopoDS::Edge(S));
  else if (S.ShapeType() == TopAbs_FACE)   PrintSur(TopoDS::Face(S));
}
#else
void TopOpeBRepBuild_Builder::PrintGeo(const TopoDS_Shape&)
{
}
#endif

//=======================================================================
//function : PrintSur
//purpose  : print the name of a surface
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::PrintSur(const TopoDS_Face& F)
{
  BRepAdaptor_Surface STA_Surface(F);
  GeomAbs_SurfaceType t =  STA_Surface.GetType();
  switch(t) {
  case GeomAbs_Plane               : cout<<"PLANE";               break;
  case GeomAbs_Cylinder            : cout<<"CYLINDER";            break;
  case GeomAbs_Cone                : cout<<"CONE";                break;
  case GeomAbs_Sphere              : cout<<"SPHERE";              break;
  case GeomAbs_Torus               : cout<<"TORUS";               break;
  case GeomAbs_BezierSurface       : cout<<"BEZIERSURFACE";       break;
  case GeomAbs_BSplineSurface      : cout<<"BSPLINESURFACE";      break;
  case GeomAbs_SurfaceOfRevolution : cout<<"SURFACEOFREVOLUTION"; break;
  case GeomAbs_SurfaceOfExtrusion  : cout<<"SURFACEOFEXTRUSION";  break;
  case GeomAbs_OtherSurface : default : cout<<"OTHERSURFACE";     break;
  }
  cout.flush();
}
#else
void TopOpeBRepBuild_Builder::PrintSur(const TopoDS_Face& )
{
}
#endif

//=======================================================================
//function : PrintCur
//purpose  : print the name of a curve
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::PrintCur(const TopoDS_Edge& E)
{
  TopLoc_Location L; Standard_Real f,l;
  Handle(Geom_Curve) C = BRep_Tool::Curve(E,L,f,l);
  if ( C.IsNull() ) return;
  GeomAdaptor_Curve GC(C);
  GeomAbs_CurveType t = GC.GetType();

  switch(t) {
  case GeomAbs_Line                : cout<<"LINE";              break;
  case GeomAbs_Circle              : cout<<"CIRCLE";            break;
  case GeomAbs_Ellipse             : cout<<"ELLIPSE";           break;
  case GeomAbs_Hyperbola           : cout<<"HYPERBOLA";         break;
  case GeomAbs_Parabola            : cout<<"PARABOLA";          break;
  case GeomAbs_BezierCurve         : cout<<"BEZIERCURVE";       break;
  case GeomAbs_BSplineCurve        : cout<<"BSPLINECURVE "<<GC.BSpline()->Degree(); break;
  case GeomAbs_OffsetCurve         : cout<<"OFFSETCURVE";       break;
  case GeomAbs_OtherCurve          : cout<<"OTHERCURVE";        break;
  }
  cout.flush();
}
#else
void TopOpeBRepBuild_Builder::PrintCur(const TopoDS_Edge&)
{
}
#endif

//=======================================================================
//function : PrintPnt
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::PrintPnt(const TopoDS_Vertex& V)
{
  GdumpPNT(BRep_Tool::Pnt(V));
}
#else
void TopOpeBRepBuild_Builder::PrintPnt(const TopoDS_Vertex&)
{
}
#endif

//=======================================================================
//function : PrintOri
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::PrintOri(const TopoDS_Shape& S) 
{ 
  TopAbs::Print(S.Orientation(),cout);
  cout.flush(); 
}
#else
void TopOpeBRepBuild_Builder::PrintOri(const TopoDS_Shape& /*S*/) 
{ 
}
#endif

//=======================================================================
//function : StringState
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
TCollection_AsciiString TopOpeBRepBuild_Builder::StringState(const TopAbs_State st) 
#else
TCollection_AsciiString TopOpeBRepBuild_Builder::StringState(const TopAbs_State) 
#endif
{ 
  TCollection_AsciiString s;
#ifdef OCCT_DEBUG
  switch(st) {
  case TopAbs_ON : s.AssignCat("ON"); break;
  case TopAbs_IN : s.AssignCat("IN"); break;
  case TopAbs_OUT : s.AssignCat("OUT"); break;
  case TopAbs_UNKNOWN : s.AssignCat("UNKNOWN"); break;
  }
#endif
  return s;
}

//=======================================================================
//function : GdumpPNT
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpPNT(const gp_Pnt& P)
{ 
  cout<<P.X()<<" "<<P.Y()<<" "<<P.Z();  cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpPNT(const gp_Pnt&)
{ 
}
#endif

//=======================================================================
//function : GdumpORIPARPNT
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpORIPARPNT(const TopAbs_Orientation o,
                                             const Standard_Real p,
                                             const gp_Pnt& Pnt)
{ 
  TopAbs::Print(o,cout); cout<<" "<<p<<" pnt "; GdumpPNT(Pnt);  cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpORIPARPNT(const TopAbs_Orientation, 
                                             const Standard_Real,const gp_Pnt&)
{
}
#endif

//=======================================================================
//function : GdumpEDGVER
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpEDGVER(const TopoDS_Shape& E,
                                          const TopoDS_Shape& V,
                                          const Standard_Address s) const 
{ 
  char* c = (char*)s; if (c) cout<<c;
  const TopoDS_Edge& EE = TopoDS::Edge(E);
  const TopoDS_Vertex& VV = TopoDS::Vertex(V);
  Standard_Real par = BRep_Tool::Parameter(VV,EE);
  gp_Pnt P = BRep_Tool::Pnt(VV);
  GdumpORIPARPNT(VV.Orientation(),par,P);
  cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpEDGVER(const TopoDS_Shape&,
                                          const TopoDS_Shape&,
                                          const Standard_Address) const 
{
}
#endif

//=======================================================================
//function : GdumpEDG
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpEDG(const TopoDS_Shape& E,
                                       const Standard_Address s) const 
{
  char* c = (char*)s; if (c) cout<<c;
  const TopoDS_Edge& EE = TopoDS::Edge(E);
  Standard_Integer n = 0;
  GdumpSHAORI(E, (char *) "vertices of ");cout<<endl;
  TopOpeBRepTool_ShapeExplorer ex(E,TopAbs_VERTEX);
  char strpar[256]; 
  Sprintf(strpar," #");
  for (; ex.More(); ex.Next()) {
    const TopoDS_Vertex& VV = TopoDS::Vertex(ex.Current());
    TopAbs_Orientation o = VV.Orientation();
    cout<<"vertex v";
    if      (o == TopAbs_FORWARD)  cout<<"F";
    else if (o == TopAbs_REVERSED) cout<<"R";
    else if (o == TopAbs_INTERNAL) cout<<"I";
    else if (o == TopAbs_EXTERNAL) cout<<"E";
    cout<<++n<<" "; TopOpeBRepBuild_Builder::PrintPnt(VV); cout<<";";
    Standard_Real par = BRep_Tool::Parameter(VV,EE);
    char spar[255];
    Sprintf(spar," par%d %f",n,par); 
    strcat(strpar,spar);
  }
  if(n) cout<<strpar<<endl;
  cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpEDG(const TopoDS_Shape&,
                                       const Standard_Address) const 
{
}
#endif

//=======================================================================
//function : GdumpSAMDOM
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GdumpSAMDOM(const TopTools_ListOfShape& L,
                                          const Standard_Address astr) const 
{
  TopOpeBRepDS_Dumper Dumper(myDataStructure);
  cout<<Dumper.SPrintShapeRefOri(L,(char*)astr)<<endl;
  cout.flush();
}

//=======================================================================
//function : GdumpSHA
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpSHA(const TopoDS_Shape& S,
                                       const Standard_Address str) const 
{
  char* c = (char*)str; if (c) cout<<c;
  if (S.IsNull()) return;
  TopAbs_ShapeEnum tS = S.ShapeType(); Standard_Integer iS = 0;
  if ( ! myDataStructure.IsNull() ) iS = myDataStructure->Shape(S);
  TopOpeBRepDS::Print(tS,iS,cout);
  cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpSHA(const TopoDS_Shape&,
                                       const Standard_Address) const 
{
}
#endif

//=======================================================================
//function : GdumpSHAORI
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpSHAORI(const TopoDS_Shape& S,
                                          const Standard_Address str) const
{
  char* c = (char*)str; if (c) cout<<c;
  GdumpSHA(S,NULL); cout<<","; PrintOri(S);
  cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpSHAORI(const TopoDS_Shape& ,
                                          const Standard_Address ) const
{
}
#endif
//=======================================================================
//function : GdumpSHAORIGEO
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpSHAORIGEO(const TopoDS_Shape& S,
                                             const Standard_Address str) const
{
  char* c = (char*)str; if (c) cout<<c;
  GdumpSHAORI(S,NULL); cout<<","; PrintGeo(S);
  cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpSHAORIGEO(const TopoDS_Shape& ,
                                             const Standard_Address ) const
{
}
#endif
//=======================================================================
//function : GdumpSHASTA
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpSHASTA(const TopoDS_Shape& S,
                                          const TopAbs_State T,
                                          const TCollection_AsciiString& a,
                                          const TCollection_AsciiString& b) const 
{
  cout<<a;
  GdumpSHAORIGEO(S,NULL); cout<<","<<StringState(T).ToCString();
  cout<<b;
  cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpSHASTA(const TopoDS_Shape& ,
                                          const TopAbs_State ,
                                          const TCollection_AsciiString& ,
                                          const TCollection_AsciiString& ) const 
{
}
#endif

//=======================================================================
//function : GdumpSHASTA
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpSHASTA(const Standard_Integer iS,
                                          const TopAbs_State T,
                                          const TCollection_AsciiString& a,
                                          const TCollection_AsciiString& b) const 
{
  const TopoDS_Shape& S = myDataStructure->Shape(iS);
  GdumpSHASTA(S,T,a,b);
  cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpSHASTA(const Standard_Integer ,
                                          const TopAbs_State ,
                                          const TCollection_AsciiString& ,
                                          const TCollection_AsciiString& ) const 
{
}
#endif

//=======================================================================
//function : GdumpSHASTA
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpSHASTA(const Standard_Integer iS,
                                          const TopAbs_State T,
                                          const TopOpeBRepBuild_ShapeSet& SS,
                                          const TCollection_AsciiString& a,
                                          const TCollection_AsciiString& b,
                                          const TCollection_AsciiString& c)const
{
  const TopoDS_Shape& S = myDataStructure->Shape(iS);
  TCollection_AsciiString aib = a + " " + SS.DEBNumber() + " " + b;
  GdumpSHASTA(S,T,aib,c);
  cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpSHASTA(const Standard_Integer ,
                                          const TopAbs_State ,
                                          const TopOpeBRepBuild_ShapeSet& ,
                                          const TCollection_AsciiString& ,
                                          const TCollection_AsciiString& ,
                                          const TCollection_AsciiString& )const
{
}
#endif

//=======================================================================
//function : GdumpSHASETreset
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GdumpSHASETreset()
{ 
#ifdef OCCT_DEBUG
  mySHASETindex = 0;
#endif
}

//=======================================================================
//function : GdumpSHASETindex
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepBuild_Builder::GdumpSHASETindex()
{
  Standard_Integer n = 0;
#ifdef OCCT_DEBUG
  n = ++mySHASETindex;
#endif
  return n;
}

//=======================================================================
//function : GdumpEXP
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpEXP(const TopOpeBRepTool_ShapeExplorer& Ex) const 
{
  if ( ! Ex.More() ) return;
  TopAbs_ShapeEnum t = Ex.Current().ShapeType();
  if      (t == TopAbs_SOLID) cout<<"";
  else if (t == TopAbs_FACE)  cout<<"  ";
  else if (t == TopAbs_EDGE)  cout<<"     ";
  else cout<<"??";
  Ex.DumpCurrent(cout);
  Standard_Integer I = myDataStructure->Shape(Ex.Current());
  if ( I != 0 ) cout<<" :  shape "<<I;
  cout<<endl;
  cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpEXP(const TopOpeBRepTool_ShapeExplorer& ) const 
{
}
#endif

//=======================================================================
//function : GdumpSOBU
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GdumpSOBU(TopOpeBRepBuild_SolidBuilder& /*ME*/) const 
{
#ifdef OCCT_DEBUG
#endif
} // GdumpSOBU

#ifdef OCCT_DEBUG
void* GFABUMAKEFACEPWES_DEB = NULL;
#endif

//=======================================================================
//function : GdumpFABU
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpFABU(TopOpeBRepBuild_FaceBuilder& ME) const 
{
  const TopoDS_Shape& face = ME.Face();
  Standard_Integer iF; 
//  Standard_Boolean tSPS = 
  GtraceSPS(face,iF);
  TopOpeBRepBuild_WireEdgeSet* PWES = (TopOpeBRepBuild_WireEdgeSet*)GFABUMAKEFACEPWES_DEB;

  Standard_Integer nf,nw,ne;
  ME.InitFace();
  if ( ME.MoreFace() ) cout<<"clear;"<<endl;
  for (nf=0;ME.MoreFace();ME.NextFace()) { 
    nf++;
    cout<<"# face "<<nf<<endl;
    for (nw=0,ME.InitWire();ME.MoreWire();ME.NextWire()) { 
      nw++;
      Standard_Boolean ow = ME.IsOldWire();
      cout<<"#  wire "<<nw;if(ow)cout<<" (old)";else cout<<" (new)";cout<<endl;
      if (!ow) {
	TCollection_AsciiString whatis("whatis");
	for(ne=0,ME.InitEdge();ME.MoreEdge();ME.NextEdge()) { 
	  ne++;
	  const TopoDS_Edge& EE = TopoDS::Edge(ME.Edge());
	  TCollection_AsciiString Enam("E");
	  TCollection_AsciiString VFnam("VF");
	  TCollection_AsciiString VRnam("VR");
	  Enam  = Enam  + ne + "NF" + nf + "F" + iF;
	  if (PWES) Enam = Enam + PWES->DEBName() + PWES->DEBNumber();
	  VFnam = VFnam + ne + "NF" + nf + "F" + iF;
	  VRnam = VRnam + ne + "NF" + nf + "F" + iF;
//	  cout<<"    puts \"edge "<<ne<<" : "<<Enam<<"\"";cout<<"; ";
	  TopoDS_Vertex VF,VR; TopExp::Vertices(EE,VF,VR);
	  if ( ! VF.IsNull() && !VR.IsNull() && !EE.IsNull()) {
#ifdef DRAW
	    DBRep::Set(Enam.ToCString(),EE); 
	    DBRep::Set(VFnam.ToCString(),VF);
	    DBRep::Set(VRnam.ToCString(),VR);
#endif
	    cout<<PRODINS<<"-O -p 0.5 "<<Enam; cout<<"; ";
//	    cout<<PRODINS<<VFnam; cout<<"; ";
//	    cout<<PRODINS<<VRnam; cout<<"; ";
//	    gp_Pnt PF = BRep_Tool::Pnt(VF); 
//	    gp_Pnt PR = BRep_Tool::Pnt(VR);
//	    cout<<endl;
//	    cout<<"# ";
//	    cout<<"dinp "<<VFnam<<" ";TopOpeBRepBuild_Builder::PrintPnt(VF);cout<<"; ";
//	    cout<<"dinp "<<VRnam<<" ";TopOpeBRepBuild_Builder::PrintPnt(VR);cout<<"; ";
	    cout<<endl;
	    whatis += " "; whatis += Enam;
	  }
	}
	if (ne) cout<<"    "<<whatis<<endl<<endl;
      }
    }
  }
  cout.flush();
} // GdumpFABU
#else
void TopOpeBRepBuild_Builder::GdumpFABU(TopOpeBRepBuild_FaceBuilder& ) const 
{
}
#endif

//=======================================================================
//function : GdumpEDBU
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GdumpEDBU(TopOpeBRepBuild_EdgeBuilder& /*ME*/) const 
{
#ifdef OCCT_DEBUG
#endif
} // GdumpEDBU

//=======================================================================
//function : GtraceSPS
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
Standard_Boolean TopOpeBRepBuild_Builder::GtraceSPS(const Standard_Integer iS) const 
#else
Standard_Boolean TopOpeBRepBuild_Builder::GtraceSPS(const Standard_Integer ) const        
#endif       
{
  Standard_Boolean b = Standard_False;
#ifdef OCCT_DEBUG
  Standard_Integer ibid;
  b = GtraceSPS(myDataStructure->Shape(iS),ibid);
#endif
  return b;
}

//=======================================================================
//function : GtraceSPS
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::GtraceSPS(const Standard_Integer ,
                                                    const Standard_Integer ) const 
{
  return Standard_False;
}

//=======================================================================
//function : GtraceSPS
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
Standard_Boolean TopOpeBRepBuild_Builder::GtraceSPS(const TopoDS_Shape& S) const 
#else
Standard_Boolean TopOpeBRepBuild_Builder::GtraceSPS(const TopoDS_Shape& ) const        
#endif       
{
  Standard_Boolean b = Standard_False;
#ifdef OCCT_DEBUG
  Standard_Integer iS;
  b = GtraceSPS(S,iS);
#endif
  return b;
}

//=======================================================================
//function : GtraceSPS
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_Builder::GtraceSPS(const TopoDS_Shape&,
                                                    Standard_Integer& IS) const 
{
  IS = 0;
  return Standard_False;
}


//=======================================================================
//function : GcheckNBOUNDS
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
Standard_Boolean TopOpeBRepBuild_Builder::GcheckNBOUNDS(const TopoDS_Shape& E) 
#else
Standard_Boolean TopOpeBRepBuild_Builder::GcheckNBOUNDS(const TopoDS_Shape& )
#endif     
{
  Standard_Boolean res = Standard_False;
#ifdef OCCT_DEBUG
  Standard_Integer nf = 0, nr = 0;
  TopOpeBRepTool_ShapeExplorer ex(E,TopAbs_VERTEX);
  for(; ex.More(); ex.Next()) {
    TopAbs_Orientation o = ex.Current().Orientation();
    if (o == TopAbs_FORWARD) nf++;
    if (o == TopAbs_REVERSED) nr++;
  }
  if ( nf == 1 && nr == 1 ) res = Standard_True;
#endif
  return res;
}
