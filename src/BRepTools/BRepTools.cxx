// Created on: 1993-01-21
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BRepTools.ixx>
#include <BRepTools_ShapeSet.hxx>
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <BRepTools_MapOfVertexPnt2d.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColGeom2d_SequenceOfCurve.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <Precision.hxx>

#include <Poly_Triangulation.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_MapOfTransient.hxx>

#include <gp_Lin2d.hxx>
#include <ElCLib.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

#include <errno.h>

//=======================================================================
//function : UVBounds
//purpose  : 
//=======================================================================

void  BRepTools::UVBounds(const TopoDS_Face& F, 
                          Standard_Real& UMin, Standard_Real& UMax, 
                          Standard_Real& VMin, Standard_Real& VMax)
{
  Bnd_Box2d B;
  AddUVBounds(F,B);
  B.Get(UMin,VMin,UMax,VMax);
}

//=======================================================================
//function : UVBounds
//purpose  : 
//=======================================================================

void  BRepTools::UVBounds(const TopoDS_Face& F,
                          const TopoDS_Wire& W, 
                          Standard_Real& UMin, Standard_Real& UMax, 
                          Standard_Real& VMin, Standard_Real& VMax)
{
  Bnd_Box2d B;
  AddUVBounds(F,W,B);
  B.Get(UMin,VMin,UMax,VMax);
}


//=======================================================================
//function : UVBounds
//purpose  : 
//=======================================================================

void  BRepTools::UVBounds(const TopoDS_Face& F,
                          const TopoDS_Edge& E, 
                          Standard_Real& UMin, Standard_Real& UMax, 
                          Standard_Real& VMin, Standard_Real& VMax)
{
  Bnd_Box2d B;
  AddUVBounds(F,E,B);
  B.Get(UMin,VMin,UMax,VMax);
}

//=======================================================================
//function : AddUVBounds
//purpose  : 
//=======================================================================

void  BRepTools::AddUVBounds(const TopoDS_Face& FF, Bnd_Box2d& B)
{
  TopoDS_Face F = FF;
  F.Orientation(TopAbs_FORWARD);
  TopExp_Explorer ex(F,TopAbs_EDGE);

  // fill box for the given face
  Bnd_Box2d aBox;
  for (;ex.More();ex.Next()) {
    BRepTools::AddUVBounds(F,TopoDS::Edge(ex.Current()),aBox);
  }
  
  // if the box is empty (face without edges or without pcurves),
  // get natural bounds
  if (aBox.IsVoid()) {
    Standard_Real UMin,UMax,VMin,VMax;
    TopLoc_Location L;
    BRep_Tool::Surface(F,L)->Bounds(UMin,UMax,VMin,VMax);
    aBox.Update(UMin,VMin,UMax,VMax);
  }
  
  // add face box to result
  B.Add ( aBox );
}


//=======================================================================
//function : AddUVBounds
//purpose  : 
//=======================================================================

void  BRepTools::AddUVBounds(const TopoDS_Face& F, 
                             const TopoDS_Wire& W, 
                             Bnd_Box2d& B)
{
  TopExp_Explorer ex;
  for (ex.Init(W,TopAbs_EDGE);ex.More();ex.Next()) {
    BRepTools::AddUVBounds(F,TopoDS::Edge(ex.Current()),B);
    }
}


//=======================================================================
//function : AddUVBounds
//purpose  : 
//=======================================================================

void  BRepTools::AddUVBounds(const TopoDS_Face& F, 
                             const TopoDS_Edge& E,
                             Bnd_Box2d& B)
{
  Standard_Real pf,pl;
  Bnd_Box2d Baux; 
  const Handle(Geom2d_Curve) C = BRep_Tool::CurveOnSurface(E,F,pf,pl);
  if (C.IsNull()) return;
  if (pl < pf) { // Petit Blindage
    Standard_Real aux;
    aux = pf; pf = pl; pl = aux;
  }
  Geom2dAdaptor_Curve PC(C,pf,pl);
  if (Precision::IsNegativeInfinite(pf) ||
      Precision::IsPositiveInfinite(pf)) {
    Geom2dAdaptor_Curve GC(PC);
    BndLib_Add2dCurve::Add(GC,0.,B);
    }
  else {

    // just compute points to get a close box.
    TopLoc_Location L;
    Standard_Real Umin,Umax,Vmin,Vmax;
    const Handle(Geom_Surface)& Surf=BRep_Tool::Surface(F,L);
    Surf->Bounds(Umin,Umax,Vmin,Vmax);
    gp_Pnt2d Pa,Pb,Pc;


    Standard_Integer i, j, k, nbp = 20;
    if (PC.GetType() == GeomAbs_Line) nbp = 2;
    Standard_Integer NbIntC1 = PC.NbIntervals(GeomAbs_C1);
    if (NbIntC1 > 1)
      nbp = 10;
    TColStd_Array1OfReal SharpPoints(1, NbIntC1+1);
    PC.Intervals(SharpPoints, GeomAbs_C1);
    TColStd_Array1OfReal Parameters(1, nbp*NbIntC1+1);
    k = 1;
    for (i = 1; i <= NbIntC1; i++)
    {
      Standard_Real delta = (SharpPoints(i+1) - SharpPoints(i))/nbp;
      for (j = 0; j < nbp; j++)
        Parameters(k++) = SharpPoints(i) + j*delta;
    }
    Parameters(nbp*NbIntC1+1) = SharpPoints(NbIntC1+1);
    
    gp_Pnt2d P;
    PC.D0(pf,P);
    Baux.Add(P);

    Standard_Real du=0.0;
    Standard_Real dv=0.0;

    Pc=P;
    for (i = 2; i < Parameters.Upper(); i++) {
      pf = Parameters(i);
      PC.D0(pf,P);
      Baux.Add(P);
      if(i==2) { Pb=Pc; Pc=P; } 
      else { 
        //-- Calcul de la fleche 
        Pa=Pb; Pb=Pc; Pc=P;     
        gp_Vec2d PaPc(Pa,Pc);
//      gp_Lin2d L2d(Pa,PaPc);
//      Standard_Real up = ElCLib::Parameter(L2d,Pb);
//      gp_Pnt2d PProj   = ElCLib::Value(up,L2d);
        gp_Pnt2d PProj(Pa.Coord()+(PaPc.XY()/2.));
        Standard_Real ddu=Abs(Pb.X()-PProj.X());
        Standard_Real ddv=Abs(Pb.Y()-PProj.Y());
        if(ddv>dv) dv=ddv;
        if(ddu>du) du=ddu;
      }
    }
    PC.D0(pl,P);
    Baux.Add(P);

    //-- cout<<" du="<<du<<"   dv="<<dv<<endl;
    Standard_Real u0,u1,v0,v1;
    Baux.Get(u0,v0,u1,v1);
    du*=1.5;
    dv*=1.5;
    u0-=du; v0-=dv; u1+=du; v1+=dv;
    if(Surf->IsUPeriodic()) { } 
    else { 
      if(u0<=Umin) {  u0=Umin; } 
      if(u1>=Umax) {  u1=Umax; } 
    }
    if(Surf->IsVPeriodic()) { } 
    else { 
      if(v0<=Vmin) {  v0=Vmin; } 
      if(v1>=Vmax) {  v1=Vmax; }
    }
    P.SetCoord(u0,v0) ; Baux.Add(P);
    P.SetCoord(u1,v1) ; Baux.Add(P);

    Bnd_Box2d FinalBox;
    Standard_Real aXmin, aYmin, aXmax, aYmax;
    Baux.Get(aXmin, aYmin, aXmax, aYmax);
    Standard_Real Tol2d = Precision::PConfusion();
    if (Abs(aXmin - Umin) <= Tol2d)
      aXmin = Umin;
    if (Abs(aYmin - Vmin) <= Tol2d)
      aYmin = Vmin;
    if (Abs(aXmax - Umax) <= Tol2d)
      aXmax = Umax;
    if (Abs(aYmax - Vmax) <= Tol2d)
      aYmax = Vmax;
    FinalBox.Update(aXmin, aYmin, aXmax, aYmax);
    
    B.Add(FinalBox);
  }
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void BRepTools::Update(const TopoDS_Vertex&)
{
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void BRepTools::Update(const TopoDS_Edge&)
{
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void BRepTools::Update(const TopoDS_Wire&)
{
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void BRepTools::Update(const TopoDS_Face& F)
{
  if (!F.Checked()) {
    UpdateFaceUVPoints(F);
    F.TShape()->Checked(Standard_True);
  }
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void BRepTools::Update(const TopoDS_Shell& S)
{
  TopExp_Explorer ex(S,TopAbs_FACE);
  while (ex.More()) {
    Update(TopoDS::Face(ex.Current()));
    ex.Next();
  }
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void BRepTools::Update(const TopoDS_Solid& S)
{
  TopExp_Explorer ex(S,TopAbs_FACE);
  while (ex.More()) {
    Update(TopoDS::Face(ex.Current()));
    ex.Next();
  }
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void BRepTools::Update(const TopoDS_CompSolid& CS)
{
  TopExp_Explorer ex(CS,TopAbs_FACE);
  while (ex.More()) {
    Update(TopoDS::Face(ex.Current()));
    ex.Next();
  }
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void BRepTools::Update(const TopoDS_Compound& C)
{
  TopExp_Explorer ex(C,TopAbs_FACE);
  while (ex.More()) {
    Update(TopoDS::Face(ex.Current()));
    ex.Next();
  }
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void BRepTools::Update(const TopoDS_Shape& S)
{
  switch (S.ShapeType()) {

  case TopAbs_VERTEX :
    Update(TopoDS::Vertex(S));
    break;

  case TopAbs_EDGE :
    Update(TopoDS::Edge(S));
    break;

  case TopAbs_WIRE :
    Update(TopoDS::Wire(S));
    break;

  case TopAbs_FACE :
    Update(TopoDS::Face(S));
    break;

  case TopAbs_SHELL :
    Update(TopoDS::Shell(S));
    break;

  case TopAbs_SOLID :
    Update(TopoDS::Solid(S));
    break;

  case TopAbs_COMPSOLID :
    Update(TopoDS::CompSolid(S));
    break;

  case TopAbs_COMPOUND :
    Update(TopoDS::Compound(S));
    break;

  default:
    break;

  }
}


//=======================================================================
//function : UpdateFaceUVPoints
//purpose  : reset the UV points of a  Face
//=======================================================================

void  BRepTools::UpdateFaceUVPoints(const TopoDS_Face& F)
{
  // Recompute for each edge the two UV points in order to have the same
  // UV point on connected edges.

  // First edge loop, store the vertices in a Map with their 2d points

  BRepTools_MapOfVertexPnt2d theVertices;
  TopoDS_Iterator expE,expV;
  TopoDS_Iterator EdgeIt,VertIt;
  TColStd_SequenceOfReal aFSeq, aLSeq;
  TColGeom2d_SequenceOfCurve aCSeq;
  TopTools_SequenceOfShape aShSeq;
  gp_Pnt2d P;
  Standard_Integer i;
  // a 3d tolerance for UV !!
  Standard_Real tolerance = BRep_Tool::Tolerance(F);
  TColgp_SequenceOfPnt2d emptySequence;
  
  for (expE.Initialize(F); expE.More(); expE.Next()) {
    if(expE.Value().ShapeType() != TopAbs_WIRE)
      continue;
    
    EdgeIt.Initialize(expE.Value());
    for( ; EdgeIt.More(); EdgeIt.Next())
    {
      const TopoDS_Edge& E = TopoDS::Edge(EdgeIt.Value());
      Standard_Real f,l;
      Handle(Geom2d_Curve) C = BRep_Tool::CurveOnSurface(E,F,f,l);

      aFSeq.Append(f);
      aLSeq.Append(l);
      aCSeq.Append(C);
      aShSeq.Append(E);

      if (C.IsNull()) continue;

      for (expV.Initialize(E.Oriented(TopAbs_FORWARD)); 
           expV.More(); expV.Next()) {
        
        const TopoDS_Vertex& V = TopoDS::Vertex(expV.Value());
        
        TopAbs_Orientation Vori = V.Orientation();
        if ( Vori == TopAbs_INTERNAL ) {
          continue;
        }
        
        Standard_Real p = BRep_Tool::Parameter(V,E,F);
        C->D0(p,P);
        if (!theVertices.IsBound(V)) 
          theVertices.Bind(V,emptySequence);
        TColgp_SequenceOfPnt2d& S = theVertices(V);
        for (i = 1; i <= S.Length(); i++) {
          if (P.Distance(S(i)) < tolerance) break;
        }
        if (i > S.Length())
          S.Append(P);
      }
    }
  }
 
  // second edge loop, update the edges 2d points
  TopoDS_Vertex Vf,Vl;
  gp_Pnt2d Pf,Pl;

  for(Standard_Integer j = 1; j <= aShSeq.Length(); j++)
  {
    const TopoDS_Edge& E = TopoDS::Edge(aShSeq.Value(j));
    const Handle(Geom2d_Curve)& C = aCSeq.Value(j);
    if (C.IsNull()) continue;
    
    TopExp::Vertices(E,Vf,Vl);
    if (Vf.IsNull()) {
      Pf.SetCoord(RealLast(),RealLast());
    }
    else {
      if ( Vf.Orientation() == TopAbs_INTERNAL ) {
        continue;
      }
      const TColgp_SequenceOfPnt2d& seqf = theVertices(Vf);
      if (seqf.Length() == 1) 
        Pf = seqf(1);
      else {
        C->D0(aFSeq.Value(j),Pf);
        for (i = 1; i <= seqf.Length(); i++) {
          if (Pf.Distance(seqf(i)) <= tolerance) {
            Pf = seqf(i);
            break;
          }
        }
      }
    }
    if (Vl.IsNull()) {
      Pl.SetCoord(RealLast(),RealLast());
    }
    else {
      if ( Vl.Orientation() == TopAbs_INTERNAL ) {
        continue;
      }
      const TColgp_SequenceOfPnt2d& seql = theVertices(Vl);
      if (seql.Length() == 1) 
        Pl = seql(1);
      else {
        C->D0(aLSeq.Value(j),Pl);
        for (i = 1; i <= seql.Length(); i++) {
          if (Pl.Distance(seql(i)) <= tolerance) {
            Pl = seql(i);
            break;
          }
        }
      }
    }

    // set the correct points
    BRep_Tool::SetUVPoints(E,F,Pf,Pl);
  }
}



//=======================================================================
//function : Compare
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools::Compare(const TopoDS_Vertex& V1,
                                    const TopoDS_Vertex& V2)
{
  if (V1.IsSame(V2)) return Standard_True;
  gp_Pnt p1 = BRep_Tool::Pnt(V1);
  gp_Pnt p2 = BRep_Tool::Pnt(V2);
  Standard_Real l = p1.Distance(p2);
  if (l <= BRep_Tool::Tolerance(V1)) return Standard_True;
  if (l <= BRep_Tool::Tolerance(V2)) return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : Compare
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools::Compare(const TopoDS_Edge& E1,
                                    const TopoDS_Edge& E2)
{
  if (E1.IsSame(E2)) return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : OuterWire
//purpose  : 
//=======================================================================

TopoDS_Wire  BRepTools::OuterWire(const TopoDS_Face& F)
{
  TopoDS_Wire Wres;
  TopExp_Explorer expw (F,TopAbs_WIRE);

  if (expw.More()) {
    Wres = TopoDS::Wire(expw.Current());
    expw.Next();
    if (expw.More()) {
      Standard_Real UMin, UMax, VMin, VMax;
      Standard_Real umin, umax, vmin, vmax;
      BRepTools::UVBounds(F,Wres,UMin,UMax,VMin,VMax);
        while (expw.More()) {
          const TopoDS_Wire& W = TopoDS::Wire(expw.Current());
          BRepTools::UVBounds(F,W,umin, umax, vmin, vmax);
          if ((umin <= UMin) &&
              (umax >= UMax) &&
              (vmin <= VMin) &&
              (vmax >= VMax)) {
            Wres = W;
            UMin = umin;
            UMax = umax;
            VMin = vmin;
            VMax = vmax;
          }
          expw.Next();
        }
    }
  }
  return Wres;
}

//=======================================================================
//function : Map3DEdges
//purpose  : 
//=======================================================================

void  BRepTools::Map3DEdges(const TopoDS_Shape& S, 
                            TopTools_IndexedMapOfShape& M)
{
  TopExp_Explorer Ex;
  for (Ex.Init(S,TopAbs_EDGE); Ex.More(); Ex.Next()) {
    if (!BRep_Tool::Degenerated(TopoDS::Edge(Ex.Current())))
      M.Add(Ex.Current());
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void  BRepTools::Dump(const TopoDS_Shape& Sh, Standard_OStream& S)
{
  BRepTools_ShapeSet SS;
  SS.Add(Sh);
  SS.Dump(Sh,S);
  SS.Dump(S);
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

void  BRepTools::Write(const TopoDS_Shape& Sh, Standard_OStream& S,
                       const Handle(Message_ProgressIndicator)& PR)
{
  BRepTools_ShapeSet SS;
  SS.SetProgress(PR);
  SS.Add(Sh);
  SS.Write(S);
  SS.Write(Sh,S);
}


//=======================================================================
//function : Read
//purpose  : 
//=======================================================================

void  BRepTools::Read(TopoDS_Shape& Sh, 
                      istream& S, 
                      const BRep_Builder& B,
                      const Handle(Message_ProgressIndicator)& PR)
{
  BRepTools_ShapeSet SS(B);
  SS.SetProgress(PR);
  SS.Read(S);
  SS.Read(Sh,S);
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

Standard_Boolean  BRepTools::Write(const TopoDS_Shape& Sh, 
                                   const Standard_CString File,
                                   const Handle(Message_ProgressIndicator)& PR)
{
  ofstream os;
  //  if (!fic.open(File,output)) return Standard_False;
  os.open(File, ios::out);
  if (!os.rdbuf()->is_open()) return Standard_False;

  Standard_Boolean isGood = (os.good() && !os.eof());
  if(!isGood)
    return isGood;
  
  BRepTools_ShapeSet SS;
  SS.SetProgress(PR);
  SS.Add(Sh);
  
  os << "DBRep_DrawableShape\n";  // for easy Draw read
  SS.Write(os);
  isGood = os.good();
  if(isGood )
    SS.Write(Sh,os);
  os.flush();
  isGood = os.good();

  errno = 0;
  os.close();
  isGood = os.good() && isGood && !errno;

  return isGood;
}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools::Read(TopoDS_Shape& Sh, 
                                 const Standard_CString File,
                                 const BRep_Builder& B,
                                 const Handle(Message_ProgressIndicator)& PR)
{
  filebuf fic;
  istream in(&fic);
  if (!fic.open(File, ios::in)) return Standard_False;

  BRepTools_ShapeSet SS(B);
  SS.SetProgress(PR);
  SS.Read(in);
  if(!SS.NbShapes()) return Standard_False;
  SS.Read(Sh,in);
  return Standard_True;
}


//=======================================================================
//function : Clean
//purpose  : 
//=======================================================================

void BRepTools::Clean(const TopoDS_Shape& S)
{
  BRep_Builder B;
  TopExp_Explorer ex;
  Handle(Poly_Triangulation) TNULL, T;
  Handle(Poly_PolygonOnTriangulation) PolyNULL, Poly;

  if (!S.IsNull()) {
    TopLoc_Location L;
    for (ex.Init(S,TopAbs_FACE);ex.More();ex.Next()) {
      const TopoDS_Face& F = TopoDS::Face(ex.Current());
      B.UpdateFace(F, TNULL);
    }
    for (ex.Init(S, TopAbs_EDGE); ex.More(); ex.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(ex.Current());
// agv 21.09.01 : Inefficient management of Locations -> improve performance
//    do {
//      BRep_Tool::PolygonOnTriangulation(E, Poly, T, L);
//      B.UpdateEdge(E, PolyNULL, T, L);
//    } while(!Poly.IsNull());
//
      Handle(BRep_CurveRepresentation) cr;
      const Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*) &E.TShape());
      BRep_ListOfCurveRepresentation& lcr = TE -> ChangeCurves();
      BRep_ListIteratorOfListOfCurveRepresentation itcr(lcr);

      // find and remove all representations
      while (itcr.More()) {
        cr = itcr.Value();
        if (cr->IsPolygonOnTriangulation())
          lcr.Remove(itcr);
        else
          itcr.Next();
      }
      TE->Modified(Standard_True);
// agv : fin
    }
  }
}

//=======================================================================
//function : RemoveUnusedPCurves
//purpose  : 
//=======================================================================

void BRepTools::RemoveUnusedPCurves(const TopoDS_Shape& S)
{
  TColStd_MapOfTransient UsedSurfaces;
  
  TopExp_Explorer Explo(S, TopAbs_FACE);
  for (; Explo.More(); Explo.Next())
  {
    TopoDS_Face aFace = TopoDS::Face(Explo.Current());
    TopLoc_Location aLoc;
    Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace, aLoc);
    UsedSurfaces.Add(aSurf);
  }

  TopTools_IndexedMapOfShape Emap;
  TopExp::MapShapes(S, TopAbs_EDGE, Emap);

  Standard_Integer i;
  for (i = 1; i <= Emap.Extent(); i++)
  {
    const Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*) &Emap(i).TShape());
    BRep_ListOfCurveRepresentation& lcr = TE -> ChangeCurves();
    BRep_ListIteratorOfListOfCurveRepresentation itrep(lcr );
    while (itrep.More())
    {
      Standard_Boolean ToRemove = Standard_False;
      
      Handle(BRep_CurveRepresentation) CurveRep = itrep.Value();
      if (CurveRep->IsCurveOnSurface())
      {
        Handle(Geom_Surface) aSurface = CurveRep->Surface();
        if (!UsedSurfaces.Contains(aSurface))
          ToRemove = Standard_True;
      }
      else if (CurveRep->IsRegularity())
      {
        Handle(Geom_Surface) Surf1 = CurveRep->Surface();
        Handle(Geom_Surface) Surf2 = CurveRep->Surface2();
        ToRemove = (!UsedSurfaces.Contains(Surf1) || !UsedSurfaces.Contains(Surf2));
      }
      
      if (ToRemove)
        lcr.Remove(itrep);
      else
        itrep.Next();
    }
  }
}

//=======================================================================
//function : Triangulation
//purpose  : 
//=======================================================================

Standard_Boolean  BRepTools::Triangulation(const TopoDS_Shape&    S,
                                           const Standard_Real    deflec)
{
  TopExp_Explorer exf, exe;
  TopLoc_Location l;
  Handle(Poly_Triangulation) T;
  Handle(Poly_PolygonOnTriangulation) Poly;

  for (exf.Init(S, TopAbs_FACE); exf.More(); exf.Next()) {
    const TopoDS_Face& F = TopoDS::Face(exf.Current());
    T = BRep_Tool::Triangulation(F, l);
    if (T.IsNull() || (T->Deflection() > deflec))
      return Standard_False;
    for (exe.Init(F, TopAbs_EDGE); exe.More(); exe.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(exe.Current());
      Poly = BRep_Tool::PolygonOnTriangulation(E, T, l);
      if (Poly.IsNull()) return Standard_False;
    }
  }
  return Standard_True;
}


//=======================================================================
//function : IsReallyClosed
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools::IsReallyClosed(const TopoDS_Edge& E,
                                           const TopoDS_Face& F)
{
  if (!BRep_Tool::IsClosed(E,F)) {
    return Standard_False;
  }
  Standard_Integer nbocc = 0;
  TopExp_Explorer exp;
  for (exp.Init(F,TopAbs_EDGE);exp.More();exp.Next()) {
    if (exp.Current().IsSame(E)) {
      nbocc++;
    }
  }
  return nbocc == 2;
}



