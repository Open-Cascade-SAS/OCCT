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

#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_EdgeBuilder.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>


#include <GeomAdaptor_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <TopOpeBRepBuild_GIter.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_Dumper.hxx>
#include <gp_Pnt.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <BRepAdaptor_Surface.hxx>

#ifdef OCCT_DEBUG
static TCollection_AsciiString PRODINS("dins ");
#endif

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpLS(const NCollection_List<TopoDS_Shape>& L) const
{
  NCollection_List<TopoDS_Shape>::Iterator it(L);
  for (; it.More(); it.Next())
  {
    const TopoDS_Shape& SL = it.Value();
    GdumpSHA(SL);
  }
}
#else
void TopOpeBRepBuild_Builder::GdumpLS(const NCollection_List<TopoDS_Shape>&) const {}
#endif

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::PrintGeo(const TopoDS_Shape& S)
{
  if (S.ShapeType() == TopAbs_VERTEX)
    PrintPnt(TopoDS::Vertex(S));
  else if (S.ShapeType() == TopAbs_EDGE)
    PrintCur(TopoDS::Edge(S));
  else if (S.ShapeType() == TopAbs_FACE)
    PrintSur(TopoDS::Face(S));
}
#else
void TopOpeBRepBuild_Builder::PrintGeo(const TopoDS_Shape&) {}
#endif

//=======================================================================
// function : PrintSur
// purpose  : print the name of a surface
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::PrintSur(const TopoDS_Face& F)
{
  BRepAdaptor_Surface STA_Surface(F);
  GeomAbs_SurfaceType t = STA_Surface.GetType();
  switch (t)
  {
    case GeomAbs_Plane:
      std::cout << "PLANE";
      break;
    case GeomAbs_Cylinder:
      std::cout << "CYLINDER";
      break;
    case GeomAbs_Cone:
      std::cout << "CONE";
      break;
    case GeomAbs_Sphere:
      std::cout << "SPHERE";
      break;
    case GeomAbs_Torus:
      std::cout << "TORUS";
      break;
    case GeomAbs_BezierSurface:
      std::cout << "BEZIERSURFACE";
      break;
    case GeomAbs_BSplineSurface:
      std::cout << "BSPLINESURFACE";
      break;
    case GeomAbs_SurfaceOfRevolution:
      std::cout << "SURFACEOFREVOLUTION";
      break;
    case GeomAbs_SurfaceOfExtrusion:
      std::cout << "SURFACEOFEXTRUSION";
      break;
    case GeomAbs_OtherSurface:
    default:
      std::cout << "OTHERSURFACE";
      break;
  }
  std::cout.flush();
}
#else
void TopOpeBRepBuild_Builder::PrintSur(const TopoDS_Face&) {}
#endif

//=======================================================================
// function : PrintCur
// purpose  : print the name of a curve
//=======================================================================
#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::PrintCur(const TopoDS_Edge& E)
{
  TopLoc_Location         L;
  double                  f, l;
  occ::handle<Geom_Curve> C = BRep_Tool::Curve(E, L, f, l);
  if (C.IsNull())
    return;
  GeomAdaptor_Curve GC(C);
  GeomAbs_CurveType t = GC.GetType();

  switch (t)
  {
    case GeomAbs_Line:
      std::cout << "LINE";
      break;
    case GeomAbs_Circle:
      std::cout << "CIRCLE";
      break;
    case GeomAbs_Ellipse:
      std::cout << "ELLIPSE";
      break;
    case GeomAbs_Hyperbola:
      std::cout << "HYPERBOLA";
      break;
    case GeomAbs_Parabola:
      std::cout << "PARABOLA";
      break;
    case GeomAbs_BezierCurve:
      std::cout << "BEZIERCURVE";
      break;
    case GeomAbs_BSplineCurve:
      std::cout << "BSPLINECURVE " << GC.BSpline()->Degree();
      break;
    case GeomAbs_OffsetCurve:
      std::cout << "OFFSETCURVE";
      break;
    case GeomAbs_OtherCurve:
      std::cout << "OTHERCURVE";
      break;
  }
  std::cout.flush();
}
#else
void TopOpeBRepBuild_Builder::PrintCur(const TopoDS_Edge&) {}
#endif

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::PrintPnt(const TopoDS_Vertex& V)
{
  GdumpPNT(BRep_Tool::Pnt(V));
}
#else
void TopOpeBRepBuild_Builder::PrintPnt(const TopoDS_Vertex&) {}
#endif

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::PrintOri(const TopoDS_Shape& S)
{
  TopAbs::Print(S.Orientation(), std::cout);
  std::cout.flush();
}
#else
void TopOpeBRepBuild_Builder::PrintOri(const TopoDS_Shape& /*S*/) {}
#endif

//=================================================================================================

#ifdef OCCT_DEBUG
TCollection_AsciiString TopOpeBRepBuild_Builder::StringState(const TopAbs_State st)
#else
TCollection_AsciiString TopOpeBRepBuild_Builder::StringState(const TopAbs_State)
#endif
{
  TCollection_AsciiString s;
#ifdef OCCT_DEBUG
  switch (st)
  {
    case TopAbs_ON:
      s.AssignCat("ON");
      break;
    case TopAbs_IN:
      s.AssignCat("IN");
      break;
    case TopAbs_OUT:
      s.AssignCat("OUT");
      break;
    case TopAbs_UNKNOWN:
      s.AssignCat("UNKNOWN");
      break;
  }
#endif
  return s;
}

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpPNT(const gp_Pnt& P)
{
  std::cout << P.X() << " " << P.Y() << " " << P.Z();
  std::cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpPNT(const gp_Pnt&) {}
#endif

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpORIPARPNT(const TopAbs_Orientation o,
                                             const double             p,
                                             const gp_Pnt&            Pnt)
{
  TopAbs::Print(o, std::cout);
  std::cout << " " << p << " pnt ";
  GdumpPNT(Pnt);
  std::cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpORIPARPNT(const TopAbs_Orientation, const double, const gp_Pnt&)
{
}
#endif

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpEDGVER(const TopoDS_Shape& E,
                                          const TopoDS_Shape& V,
                                          void* const         s) const
{
  char* c = (char*)s;
  if (c)
    std::cout << c;
  const TopoDS_Edge&   EE  = TopoDS::Edge(E);
  const TopoDS_Vertex& VV  = TopoDS::Vertex(V);
  double               par = BRep_Tool::Parameter(VV, EE);
  gp_Pnt               P   = BRep_Tool::Pnt(VV);
  GdumpORIPARPNT(VV.Orientation(), par, P);
  std::cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpEDGVER(const TopoDS_Shape&,
                                          const TopoDS_Shape&,
                                          void* const) const
{
}
#endif

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpEDG(const TopoDS_Shape& E, void* const s) const
{
  char* c = (char*)s;
  if (c)
    std::cout << c;
  const TopoDS_Edge& EE = TopoDS::Edge(E);
  int                n  = 0;
  GdumpSHAORI(E, (char*)"vertices of ");
  std::cout << std::endl;
  TopOpeBRepTool_ShapeExplorer ex(E, TopAbs_VERTEX);
  char                         strpar[256];
  Sprintf(strpar, " #");
  for (; ex.More(); ex.Next())
  {
    const TopoDS_Vertex& VV = TopoDS::Vertex(ex.Current());
    TopAbs_Orientation   o  = VV.Orientation();
    std::cout << "vertex v";
    if (o == TopAbs_FORWARD)
      std::cout << "F";
    else if (o == TopAbs_REVERSED)
      std::cout << "R";
    else if (o == TopAbs_INTERNAL)
      std::cout << "I";
    else if (o == TopAbs_EXTERNAL)
      std::cout << "E";
    std::cout << ++n << " ";
    TopOpeBRepBuild_Builder::PrintPnt(VV);
    std::cout << ";";
    double par = BRep_Tool::Parameter(VV, EE);
    char   spar[255];
    Sprintf(spar, " par%d %f", n, par);
    strcat(strpar, spar);
  }
  if (n)
    std::cout << strpar << std::endl;
  std::cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpEDG(const TopoDS_Shape&, void* const) const {}
#endif

//=================================================================================================

void TopOpeBRepBuild_Builder::GdumpSAMDOM(const NCollection_List<TopoDS_Shape>& L,
                                          void* const                           astr) const
{
  TopOpeBRepDS_Dumper Dumper(myDataStructure);
  std::cout << Dumper.SPrintShapeRefOri(L, (char*)astr) << std::endl;
  std::cout.flush();
}

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpSHA(const TopoDS_Shape& S, void* const str) const
{
  char* c = (char*)str;
  if (c)
    std::cout << c;
  if (S.IsNull())
    return;
  TopAbs_ShapeEnum tS = S.ShapeType();
  int              iS = 0;
  if (!myDataStructure.IsNull())
    iS = myDataStructure->Shape(S);
  TopOpeBRepDS::Print(tS, iS, std::cout);
  std::cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpSHA(const TopoDS_Shape&, void* const) const {}
#endif

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpSHAORI(const TopoDS_Shape& S, void* const str) const
{
  char* c = (char*)str;
  if (c)
    std::cout << c;
  GdumpSHA(S, NULL);
  std::cout << ",";
  PrintOri(S);
  std::cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpSHAORI(const TopoDS_Shape&, void* const) const {}
#endif
//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpSHAORIGEO(const TopoDS_Shape& S, void* const str) const
{
  char* c = (char*)str;
  if (c)
    std::cout << c;
  GdumpSHAORI(S, NULL);
  std::cout << ",";
  PrintGeo(S);
  std::cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpSHAORIGEO(const TopoDS_Shape&, void* const) const {}
#endif
//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpSHASTA(const TopoDS_Shape&            S,
                                          const TopAbs_State             T,
                                          const TCollection_AsciiString& a,
                                          const TCollection_AsciiString& b) const
{
  std::cout << a;
  GdumpSHAORIGEO(S, NULL);
  std::cout << "," << StringState(T).ToCString();
  std::cout << b;
  std::cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpSHASTA(const TopoDS_Shape&,
                                          const TopAbs_State,
                                          const TCollection_AsciiString&,
                                          const TCollection_AsciiString&) const
{
}
#endif

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpSHASTA(const int                      iS,
                                          const TopAbs_State             T,
                                          const TCollection_AsciiString& a,
                                          const TCollection_AsciiString& b) const
{
  const TopoDS_Shape& S = myDataStructure->Shape(iS);
  GdumpSHASTA(S, T, a, b);
  std::cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpSHASTA(const int,
                                          const TopAbs_State,
                                          const TCollection_AsciiString&,
                                          const TCollection_AsciiString&) const
{
}
#endif

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpSHASTA(const int                       iS,
                                          const TopAbs_State              T,
                                          const TopOpeBRepBuild_ShapeSet& SS,
                                          const TCollection_AsciiString&  a,
                                          const TCollection_AsciiString&  b,
                                          const TCollection_AsciiString&  c) const
{
  const TopoDS_Shape&     S   = myDataStructure->Shape(iS);
  TCollection_AsciiString aib = a + " " + SS.DEBNumber() + " " + b;
  GdumpSHASTA(S, T, aib, c);
  std::cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpSHASTA(const int,
                                          const TopAbs_State,
                                          const TopOpeBRepBuild_ShapeSet&,
                                          const TCollection_AsciiString&,
                                          const TCollection_AsciiString&,
                                          const TCollection_AsciiString&) const
{
}
#endif

//=================================================================================================

void TopOpeBRepBuild_Builder::GdumpSHASETreset()
{
#ifdef OCCT_DEBUG
  mySHASETindex = 0;
#endif
}

//=================================================================================================

int TopOpeBRepBuild_Builder::GdumpSHASETindex()
{
  int n = 0;
#ifdef OCCT_DEBUG
  n = ++mySHASETindex;
#endif
  return n;
}

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpEXP(const TopOpeBRepTool_ShapeExplorer& Ex) const
{
  if (!Ex.More())
    return;
  TopAbs_ShapeEnum t = Ex.Current().ShapeType();
  if (t == TopAbs_SOLID)
    std::cout << "";
  else if (t == TopAbs_FACE)
    std::cout << "  ";
  else if (t == TopAbs_EDGE)
    std::cout << "     ";
  else
    std::cout << "??";
  Ex.DumpCurrent(std::cout);
  int I = myDataStructure->Shape(Ex.Current());
  if (I != 0)
    std::cout << " :  shape " << I;
  std::cout << std::endl;
  std::cout.flush();
}
#else
void TopOpeBRepBuild_Builder::GdumpEXP(const TopOpeBRepTool_ShapeExplorer&) const {}
#endif

//=================================================================================================

void TopOpeBRepBuild_Builder::GdumpSOBU(TopOpeBRepBuild_SolidBuilder& /*ME*/) const
{
#ifdef OCCT_DEBUG
#endif
} // GdumpSOBU

#ifdef OCCT_DEBUG
void* GFABUMAKEFACEPWES_DEB = NULL;
#endif

//=================================================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_Builder::GdumpFABU(TopOpeBRepBuild_FaceBuilder& ME) const
{
  const TopoDS_Shape& face = ME.Face();
  int                 iF;
  //  bool tSPS =
  GtraceSPS(face, iF);
  TopOpeBRepBuild_WireEdgeSet* PWES = (TopOpeBRepBuild_WireEdgeSet*)GFABUMAKEFACEPWES_DEB;

  int nf, nw, ne;
  ME.InitFace();
  if (ME.MoreFace())
    std::cout << "clear;" << std::endl;
  for (nf = 0; ME.MoreFace(); ME.NextFace())
  {
    nf++;
    std::cout << "# face " << nf << std::endl;
    for (nw = 0, ME.InitWire(); ME.MoreWire(); ME.NextWire())
    {
      nw++;
      bool ow = ME.IsOldWire();
      std::cout << "#  wire " << nw;
      if (ow)
        std::cout << " (old)";
      else
        std::cout << " (new)";
      std::cout << std::endl;
      if (!ow)
      {
        TCollection_AsciiString whatis("whatis");
        for (ne = 0, ME.InitEdge(); ME.MoreEdge(); ME.NextEdge())
        {
          ne++;
          const TopoDS_Edge&      EE = TopoDS::Edge(ME.Edge());
          TCollection_AsciiString Enam("E");
          TCollection_AsciiString VFnam("VF");
          TCollection_AsciiString VRnam("VR");
          Enam = Enam + ne + "NF" + nf + "F" + iF;
          if (PWES)
            Enam = Enam + PWES->DEBName() + PWES->DEBNumber();
          VFnam = VFnam + ne + "NF" + nf + "F" + iF;
          VRnam = VRnam + ne + "NF" + nf + "F" + iF;
          //	  std::cout<<"    puts \"edge "<<ne<<" : "<<Enam<<"\"";std::cout<<"; ";
          TopoDS_Vertex VF, VR;
          TopExp::Vertices(EE, VF, VR);
          if (!VF.IsNull() && !VR.IsNull() && !EE.IsNull())
          {
            std::cout << PRODINS << "-O -p 0.5 " << Enam;
            std::cout << "; ";
            //	    std::cout<<PRODINS<<VFnam; std::cout<<"; ";
            //	    std::cout<<PRODINS<<VRnam; std::cout<<"; ";
            //	    gp_Pnt PF = BRep_Tool::Pnt(VF);
            //	    gp_Pnt PR = BRep_Tool::Pnt(VR);
            //	    std::cout<<std::endl;
            //	    std::cout<<"# ";
            //	    std::cout<<"dinp "<<VFnam<<"
            //";TopOpeBRepBuild_Builder::PrintPnt(VF);std::cout<<"; "; 	    std::cout<<"dinp
            //"<<VRnam<<"
            //";TopOpeBRepBuild_Builder::PrintPnt(VR);std::cout<<"; ";
            std::cout << std::endl;
            whatis += " ";
            whatis += Enam;
          }
        }
        if (ne)
          std::cout << "    " << whatis << std::endl << std::endl;
      }
    }
  }
  std::cout.flush();
} // GdumpFABU
#else
void TopOpeBRepBuild_Builder::GdumpFABU(TopOpeBRepBuild_FaceBuilder&) const {}
#endif

//=================================================================================================

void TopOpeBRepBuild_Builder::GdumpEDBU(TopOpeBRepBuild_EdgeBuilder& /*ME*/) const
{
#ifdef OCCT_DEBUG
#endif
} // GdumpEDBU

//=================================================================================================

#ifdef OCCT_DEBUG
bool TopOpeBRepBuild_Builder::GtraceSPS(const int iS) const
#else
bool TopOpeBRepBuild_Builder::GtraceSPS(const int) const
#endif
{
  bool b = false;
#ifdef OCCT_DEBUG
  int ibid;
  b = GtraceSPS(myDataStructure->Shape(iS), ibid);
#endif
  return b;
}

//=================================================================================================

bool TopOpeBRepBuild_Builder::GtraceSPS(const int, const int) const
{
  return false;
}

//=================================================================================================

#ifdef OCCT_DEBUG
bool TopOpeBRepBuild_Builder::GtraceSPS(const TopoDS_Shape& S) const
#else
bool TopOpeBRepBuild_Builder::GtraceSPS(const TopoDS_Shape&) const
#endif
{
  bool b = false;
#ifdef OCCT_DEBUG
  int iS;
  b = GtraceSPS(S, iS);
#endif
  return b;
}

//=================================================================================================

bool TopOpeBRepBuild_Builder::GtraceSPS(const TopoDS_Shape&, int& IS) const
{
  IS = 0;
  return false;
}

//=================================================================================================

#ifdef OCCT_DEBUG
bool TopOpeBRepBuild_Builder::GcheckNBOUNDS(const TopoDS_Shape& E)
#else
bool TopOpeBRepBuild_Builder::GcheckNBOUNDS(const TopoDS_Shape&)
#endif
{
  bool res = false;
#ifdef OCCT_DEBUG
  int                          nf = 0, nr = 0;
  TopOpeBRepTool_ShapeExplorer ex(E, TopAbs_VERTEX);
  for (; ex.More(); ex.Next())
  {
    TopAbs_Orientation o = ex.Current().Orientation();
    if (o == TopAbs_FORWARD)
      nf++;
    if (o == TopAbs_REVERSED)
      nr++;
  }
  if (nf == 1 && nr == 1)
    res = true;
#endif
  return res;
}
