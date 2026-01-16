// Created on: 1996-01-09
// Created by: Jacques GOUSSARD
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

#include <LocOpe_Generator.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgo_Loop.hxx>
#include <BRepTools.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomProjLib.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <LocOpe_BuildShape.hxx>
#include <LocOpe_GeneratedShape.hxx>
#include <Precision.hxx>
#include <Standard_NullObject.hxx>
#include <StdFail_NotDone.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_Map.hxx>

static bool ToFuse(const TopoDS_Face&, const TopoDS_Face&);

static bool ToFuse(const TopoDS_Edge&, const TopoDS_Edge&);

static bool ToFuse(const TopoDS_Edge&,
                   const TopoDS_Face&,
                   const TopoDS_Vertex&,
                   const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&);

static double NewParameter(const TopoDS_Edge&,
                           const TopoDS_Vertex&,
                           const TopoDS_Edge&,
                           const TopoDS_Vertex&);

//=================================================================================================

void LocOpe_Generator::Perform(const occ::handle<LocOpe_GeneratedShape>& G)
{
  if (myShape.IsNull())
  {
    throw Standard_NullObject();
  }
  myDone = false;
  myRes.Nullify();
  //  myDescFaces.Clear();
  myModShapes.Clear();
  //  myFFromE.Clear();

  const NCollection_List<TopoDS_Shape>& ledges = G->GeneratingEdges();

  // On genere une liste des faces a gauche du wire. Equivalent du LeftOf.
  // Attention : il faudra bien propager pour ne pas oublier des faces
  // a l`interieur

  TopExp_Explorer                                        exp, exp2, exp3;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> theLeft; // Faces a gauche

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> GEdg, GVtx; // Edges et vertex generateurs

  NCollection_List<TopoDS_Shape>::Iterator itl, itl2;

  for (itl.Initialize(ledges); itl.More(); itl.Next())
  {
    const TopoDS_Edge& edg = TopoDS::Edge(itl.Value());

    GEdg.Add(edg);
    for (exp2.Init(edg, TopAbs_VERTEX); exp2.More(); exp2.Next())
    {
      const TopoDS_Vertex& vtx = TopoDS::Vertex(exp2.Current());
      GVtx.Add(vtx);
    }
    for (exp2.Init(myShape, TopAbs_FACE); exp2.More(); exp2.Next())
    {
      const TopoDS_Face& fac = TopoDS::Face(exp2.Current());
      for (exp3.Init(fac, TopAbs_EDGE); exp3.More(); exp3.Next())
      {
        if (exp3.Current().IsSame(edg) && exp3.Current().Orientation() == edg.Orientation())
        {
          theLeft.Add(fac);
          myModShapes.Bound(fac, NCollection_List<TopoDS_Shape>());
          break;
        }
      }
      if (exp3.More())
      {
        break;
      }
    }
  }

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    theEFMap;
  TopExp::MapShapesAndAncestors(myShape, TopAbs_EDGE, TopAbs_FACE, theEFMap);

  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    theEEMap;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                                                   theFFMap;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>           toRemove;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itm;

  // search for face fusions
  for (itm.Initialize(GEdg); itm.More(); itm.Next())
  {
    const TopoDS_Edge& edg = TopoDS::Edge(itm.Key());
    if (!theEFMap.Contains(edg))
    {
      continue;
    }
    for (itl2.Initialize(theEFMap.FindFromKey(edg)); itl2.More(); itl2.Next())
    {
      if (!theLeft.Contains(itl2.Value()))
      {
        break;
      }
    }
    if (!itl2.More())
    { // edge "interne" au shell, ou bord libre
    }
    else
    {
      const TopoDS_Face& fac    = TopoDS::Face(itl2.Value());
      TopoDS_Face        facbis = G->Generated(edg);
      if (ToFuse(fac, facbis))
      {
        // On recherche si une face a deja fusionne avec facbis
        bool facbisfound = false;
        for (NCollection_DataMap<TopoDS_Shape,
                                 NCollection_List<TopoDS_Shape>,
                                 TopTools_ShapeMapHasher>::Iterator itf(theFFMap);
             itf.More();
             itf.Next())
        {
          if (itf.Key().IsSame(fac))
          {
            continue;
          }
          for (itl.Initialize(itf.Value()); itl.More(); itl.Next())
          {
            if (itl.Value().IsSame(facbis))
            {
              facbisfound = true;
              break;
            }
          }
          if (facbisfound)
          {
            theFFMap(itf.Key()).Append(fac);
            toRemove.Add(fac);
            toRemove.Add(edg);
            break;
          }
        }

        if (!facbisfound)
        {
          if (!theFFMap.IsBound(fac))
          {
            NCollection_List<TopoDS_Shape> thelist;
            theFFMap.Bind(fac, thelist);
          }
          for (itl.Initialize(theFFMap(fac)); itl.More(); itl.Next())
          {
            if (itl.Value().IsSame(facbis))
            {
              break;
            }
          }
          if (!itl.More())
          {
            theFFMap(fac).Append(facbis);
          }
          toRemove.Add(edg);
          toRemove.Add(facbis);
        }
      }
      else
      { // face generee par edg : on la marque.
        //	myFFromE.Bind(edg,facbis);
      }
    }
  }

  // Il faut ici ajouter dans toRemove les edges de connexites entre faces
  // a fusionner avec une meme face de base

  //  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>,
  //  TopTools_ShapeMapHasher>::Iterator itf(theFFMap);
  for (NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
         Iterator itf(theFFMap);
       itf.More();
       itf.Next())
  {
    for (itl.Initialize(itf.Value()); itl.More(); itl.Next())
    {
      for (exp.Init(itl.Value(), TopAbs_EDGE); exp.More(); exp.Next())
      {
        const TopoDS_Edge& ed = TopoDS::Edge(exp.Current());
        if (toRemove.Contains(ed))
        {
          continue;
        }
        for (itl2.Initialize(itf.Value()); itl2.More(); itl2.Next())
        {
          if (!itl2.Value().IsSame(itl.Value()))
          {
            for (exp2.Init(itl2.Value(), TopAbs_EDGE); exp2.More(); exp2.Next())
            {
              if (ed.IsSame(exp2.Current()))
              {
                toRemove.Add(ed);
                break;
              }
            }
            if (exp2.More())
            {
              break;
            }
          }
        }
      }
    }
  }

  NCollection_List<TopoDS_Shape>                                           RebuildFace;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>                   mapTreated;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> DontFuse;
  TopAbs_Orientation                                                       orient, orface;

  for (NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
         Iterator itf(theFFMap);
       itf.More();
       itf.Next())
  {
    const TopoDS_Face& fac = TopoDS::Face(itf.Key());
    for (exp.Init(fac, TopAbs_EDGE); exp.More(); exp.Next())
    {
      const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
      if (mapTreated.Contains(edg))
      {
        continue; // on saute l`edge
      }

      mapTreated.Add(edg);
      for (exp2.Init(edg, TopAbs_VERTEX); exp2.More(); exp2.Next())
      {
        const TopoDS_Vertex& vtx = TopoDS::Vertex(exp2.Current());
        if (GVtx.Contains(vtx))
        {
          TopoDS_Edge edgbis = G->Generated(vtx);

          if ((edgbis.IsNull() || BRep_Tool::Degenerated(edgbis)) ||
              //	      toRemove.Contains(edgbis) ||
              !ToFuse(edg, edgbis))
          {
            continue;
          }
          // a voir
          if (BRepTools::IsReallyClosed(edg, fac))
          {
            if (!theEEMap.IsBound(edg))
            {
              NCollection_List<TopoDS_Shape> thelist1;
              theEEMap.Bind(edg, thelist1);
              theEEMap(edg).Append(edgbis);
              toRemove.Add(edgbis); // toujours vrai pour edge double
              bool          FuseEdge = true;
              TopoDS_Vertex Vf, Vl;
              TopExp::Vertices(edg, Vf, Vl);
              bool ConnectLast = (Vl.IsSame(vtx));
              for (exp3.Init(fac.Oriented(TopAbs_FORWARD), TopAbs_EDGE); exp3.More(); exp3.Next())
              {
                const TopoDS_Edge& eee = TopoDS::Edge(exp3.Current());
                orient                 = eee.Orientation();
                if (!eee.IsSame(edg))
                {
                  TopExp::Vertices(eee, Vf, Vl);
                  if ((Vf.IsSame(vtx) || Vl.IsSame(vtx)) && !toRemove.Contains(eee))
                  {
                    FuseEdge = false;
                    // On recherche celui qu`il ne faut pas fusionner

                    if ((Vf.IsSame(vtx) && orient == TopAbs_FORWARD)
                        || (Vl.IsSame(vtx) && orient == TopAbs_REVERSED))
                    {
                      if (ConnectLast)
                      {
                        DontFuse.Bind(edg, fac.Oriented(TopAbs_FORWARD));
                      }
                      else
                      {
                        DontFuse.Bind(edg, fac.Oriented(TopAbs_REVERSED));
                      }
                    }
                    else
                    {
                      if (ConnectLast)
                      {
                        DontFuse.Bind(edg, fac.Oriented(TopAbs_REVERSED));
                      }
                      else
                      {
                        DontFuse.Bind(edg, fac.Oriented(TopAbs_FORWARD));
                      }
                    }
                    break;
                  }
                }
              }
              if (FuseEdge)
              {
                toRemove.Add(vtx);
              }
            }
          }

          else
          {
            /*  A VOIR
                  if (!BRep_Tool::IsClosed(edg,fac)) {
            */
            if (!theEEMap.IsBound(edg))
            {
              NCollection_List<TopoDS_Shape> thelist2;
              theEEMap.Bind(edg, thelist2);
            }
            theEEMap(edg).Append(edgbis);
            const NCollection_List<TopoDS_Shape>&    L = theEEMap(edg);
            NCollection_List<TopoDS_Shape>::Iterator Lit(L);
            bool                                     OK = true;
            for (; Lit.More(); Lit.Next())
            {
              if (Lit.Value().IsSame(edgbis))
              {
                OK = false;
                break;
              }
            }
            if (OK)
              theEEMap(edg).Append(edgbis);

            itl.Initialize(theEFMap.FindFromKey(edg));
            bool FuseEdge = ToFuse(edg, fac, vtx, toRemove);
            if (!FuseEdge)
            {
              DontFuse.Bind(edg, fac);
            }
            else
            {
              for (; itl.More(); itl.Next())
              {
                if (!itl.Value().IsSame(fac))
                {
                  if (theFFMap.IsBound(itl.Value()))
                  {
                    FuseEdge = ToFuse(edg, TopoDS::Face(itl.Value()), vtx, toRemove);
                    // edge a fusionner
                    if (FuseEdge)
                    {
                      toRemove.Add(vtx);
                    }
                    else
                    {
                      if (toRemove.Contains(vtx))
                      {
                        toRemove.Remove(vtx);
                      }
                      DontFuse.Bind(edg, itl.Value());
                    }
                  }
                  else
                  { // on marque comme face a reconstruire
                    RebuildFace.Append(itl.Value());
                    if (toRemove.Contains(vtx))
                    {
                      toRemove.Remove(vtx);
                    }
                    DontFuse.Bind(edg, itl.Value());
                  }

                  break;
                }
              }
            }
          }
        }
      }
    }
  }

  for (itl.Initialize(RebuildFace); itl.More(); itl.Next())
  {
    NCollection_List<TopoDS_Shape> thelist3;
    theFFMap.Bind(itl.Value(), thelist3);
  }

  BRep_Builder    B;
  TopoDS_Face     newface;
  TopoDS_Wire     outw, newwire;
  TopoDS_Edge     newedg;
  TopoDS_Vertex   newvtx;
  TopLoc_Location loc;
  double          tol, prm, f, l, Uminc = 0., Umaxc = 0.;
  gp_Pnt2d        pf, pl;

  occ::handle<Geom_Surface> S;
  occ::handle<Geom_Plane>   P;
  occ::handle<Geom_Curve>   C;

  // Fusion des edges
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator ite(theEEMap);
  for (; ite.More(); ite.Next())
  {
    bool               KeepNewEdge = false;
    const TopoDS_Edge& edg         = TopoDS::Edge(ite.Key());
    BRep_Tool::Range(edg, f, l);
    TopoDS_Shape aLocalEdge = edg.EmptyCopied();
    newedg                  = TopoDS::Edge(aLocalEdge);
    //    newedg = TopoDS::Edge(edg.EmptyCopied());
    newedg.Orientation(TopAbs_FORWARD);
    for (exp.Init(edg.Oriented(TopAbs_FORWARD), TopAbs_VERTEX); exp.More(); exp.Next())
    {
      const TopoDS_Vertex& vtx = TopoDS::Vertex(exp.Current());
      prm                      = BRep_Tool::Parameter(vtx, edg);

      newvtx.Nullify();
      for (itl.Initialize(theEEMap(edg)); itl.More(); itl.Next())
      {
        const TopoDS_Edge& edgbis = TopoDS::Edge(itl.Value());
        for (exp2.Init(edgbis, TopAbs_VERTEX); exp2.More(); exp2.Next())
        {
          if (exp2.Current().IsSame(vtx))
          {
            break;
          }
        }
        if (exp2.More())
        {
          for (exp2.ReInit(); exp2.More(); exp2.Next())
          {
            if (!exp2.Current().IsSame(vtx))
            {
              newvtx = TopoDS::Vertex(exp2.Current());
              prm    = NewParameter(edg, vtx, newedg, newvtx);
              break;
            }
          }
          break;
        }
      }

      if (toRemove.Contains(vtx) || (prm < l && prm > f))
      {
        B.Add(newedg, newvtx.Oriented(vtx.Orientation()));
        tol = BRep_Tool::Tolerance(newvtx);
        B.UpdateVertex(newvtx, prm, newedg, tol);
        toRemove.Add(itl.Value()); // i-e edgbis
        KeepNewEdge = true;
      }
      else
      {
        B.Add(newedg, vtx.Oriented(vtx.Orientation()));
        tol = BRep_Tool::Tolerance(vtx);
        B.UpdateVertex(vtx, prm, newedg, tol);
      }
    }
    if (KeepNewEdge)
    {
      myModShapes.Bound(edg, NCollection_List<TopoDS_Shape>())->Append(newedg);
      toRemove.Add(edg);
    }
  }

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> EdgAdded;

  // Fusion des faces, ou reconstruction
  for (NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
         Iterator itf(theFFMap);
       itf.More();
       itf.Next())
  {
    const TopoDS_Face&             fac     = TopoDS::Face(itf.Key());
    bool                           ModFace = false;
    NCollection_List<TopoDS_Shape> listofedg;

    EdgAdded.Clear();

    for (exp.Init(myShape, TopAbs_FACE); exp.More(); exp.Next())
    {
      if (exp.Current().IsSame(fac))
      {
        break;
      }
    }
    orface                             = exp.Current().Orientation();
    TopoDS_Shape aLocalFaceEmptyCopied = fac.EmptyCopied();
    newface                            = TopoDS::Face(aLocalFaceEmptyCopied);
    //    newface = TopoDS::Face(fac.EmptyCopied());
    newface.Orientation(TopAbs_FORWARD);
    S = BRep_Tool::Surface(fac);
    if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
    {
      S = occ::down_cast<Geom_RectangularTrimmedSurface>(S)->BasisSurface();
    }
    P = occ::down_cast<Geom_Plane>(S);
    TopoDS_Wire wir;
    for (exp.Init(fac.Oriented(TopAbs_FORWARD), TopAbs_WIRE); exp.More(); exp.Next())
    {
      wir = TopoDS::Wire(exp.Current());
      for (exp2.Init(wir, TopAbs_EDGE); exp2.More(); exp2.Next())
      {
        const TopoDS_Edge& edg = TopoDS::Edge(exp2.Current());
        if (toRemove.Contains(edg) || myModShapes.IsBound(edg))
        {
          break;
        }
      }
      if (!exp2.More())
      { // wire non modifie
        //	B.Add(newface,wir.Oriented(wir.Orientation()));
        for (exp2.Init(wir, TopAbs_EDGE); exp2.More(); exp2.Next())
        {
          listofedg.Append(exp2.Current());
        }
      }
      else
      {
        if (!ModFace)
        {

          // Petit truc crad pour les p-curves sur les cylindres...
          if (P.IsNull())
          {
            double Vminc, Vmaxc;
            BRepTools::UVBounds(fac, Uminc, Umaxc, Vminc, Vmaxc);
          }

          // premier passage : on met les wires non touches des faces
          // en vis a vis

          for (itl.Initialize(itf.Value()); itl.More(); itl.Next())
          {
            TopoDS_Face facbis = TopoDS::Face(itl.Value());
            for (itl2.Initialize(G->OrientedFaces()); itl2.More(); itl2.Next())
            {
              if (itl2.Value().IsSame(facbis))
              {
                break;
              }
            }
            if (itl2.More())
            {
              orient = itl2.Value().Orientation();
              facbis.Orientation(orient);
            }
            else
            { // on fusionne avec une autre face du shape...
              for (exp2.Init(myShape, TopAbs_FACE); exp2.More(); exp2.Next())
              {
                if (exp2.Current().IsSame(facbis))
                {
                  facbis.Orientation(exp2.Current().Orientation());
                  break;
                }
              }
            }

            for (exp3.Init(facbis, TopAbs_WIRE); exp3.More(); exp3.Next())
            {
              for (exp2.Init(exp3.Current(), TopAbs_EDGE); exp2.More(); exp2.Next())
              {
                if (toRemove.Contains(exp2.Current()))
                {
                  break;
                }
              }
              if (!exp2.More())
              {
                TopoDS_Wire theNew;
                B.MakeWire(theNew); // FORWARD
                for (exp2.ReInit(); exp2.More(); exp2.Next())
                {
                  const TopoDS_Edge& edg = TopoDS::Edge(exp2.Current());

                  orient = TopAbs::Compose(orface, edg.Orientation());
                  //		  B.Add(theNew,edg.Oriented(or));
                  listofedg.Append(edg.Oriented(orient));
                  EdgAdded.Add(edg);
                  if (P.IsNull())
                  {
                    // on met les courbes 2d si on n`est pas sur un plan
                    // on ne devrait pas avoir de pb d`edge de couture.
                    tol = BRep_Tool::Tolerance(edg);
                    C   = BRep_Tool::Curve(edg, loc, f, l);
                    if (!loc.IsIdentity())
                    {
                      occ::handle<Geom_Geometry> GG = C->Transformed(loc.Transformation());
                      C                             = occ::down_cast<Geom_Curve>(GG);
                    }
                    if (C->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve))
                    {
                      C = occ::down_cast<Geom_TrimmedCurve>(C)->BasisCurve();
                    }

                    occ::handle<Geom2d_Curve> C2d = GeomProjLib::Curve2d(C, f, l, S, tol);

                    // Tentative de recalage dans la facette
                    pf                     = C2d->Value(f);
                    pl                     = C2d->Value(l);
                    constexpr double tttol = Precision::Angular();
                    while (std::min(pf.X(), pl.X()) >= Umaxc - tttol)
                    {
                      C2d->Translate(gp_Vec2d(-2. * M_PI, 0));
                      pf = C2d->Value(f);
                      pl = C2d->Value(l);
                    }

                    while (std::max(pf.X(), pl.X()) <= Uminc + tttol)
                    {
                      C2d->Translate(gp_Vec2d(2. * M_PI, 0));
                      pf = C2d->Value(f);
                      pl = C2d->Value(l);
                    }

                    if (!BRepTools::IsReallyClosed(edg, facbis))
                    {
                      B.UpdateEdge(edg, C2d, newface, tol);
                    }
                    //		else {
                    //		  std::cout << "Edge double bizarre... " << std::endl;
                    //		}
                  }
                }
                //		B.Add(newface,theNew);
              }
            }
          }
          ModFace = true;
        }

        // reconstruction du wire
        // B.MakeWire(newwire);

        occ::handle<Geom2d_Curve> C2d, C2d1;

        //	for (exp2.Init(wir.Oriented(TopAbs_FORWARD),TopAbs_EDGE);
        for (exp2.Init(wir, TopAbs_EDGE); exp2.More(); exp2.Next())
        {
          const TopoDS_Edge& edg = TopoDS::Edge(exp2.Current());
          orient                 = edg.Orientation();
          if (!toRemove.Contains(edg) && !theEEMap.IsBound(edg))
          {
            //	    B.Add(newwire,edg.Oriented(or));
            //            listofedg.Append(edg.Oriented(or));
            listofedg.Append(edg);
          }
          else if (myModShapes.IsBound(edg) || theEEMap.IsBound(edg))
          {
            if (myModShapes.IsBound(edg))
            {
              newedg = TopoDS::Edge(myModShapes(edg).First());
            }
            else
            {
              newedg = edg;
            }
            //	    B.Add(newwire,newedg.Oriented(or));
            listofedg.Append(newedg.Oriented(orient));
            C = BRep_Tool::Curve(newedg, loc, f, l);
            if (!loc.IsIdentity())
            {
              occ::handle<Geom_Geometry> GG = C->Transformed(loc.Transformation());
              C                             = occ::down_cast<Geom_Curve>(GG);
            }
            if (C->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve))
            {
              C = occ::down_cast<Geom_TrimmedCurve>(C)->BasisCurve();
            }
            if (P.IsNull())
            { // on met les courbes 2d si on n`est pas
              // sur un plan
              //	      TopAbs_Orientation oredonfafw = TopAbs::Compose(or,orsav);
              TopAbs_Orientation oredonfafw = orient;
              tol                           = BRep_Tool::Tolerance(newedg);
              TopoDS_Shape aLocalEdge       = edg.Oriented(oredonfafw);
              TopoDS_Shape aLocalFace       = fac.Oriented(TopAbs_FORWARD);
              C2d =
                BRep_Tool::CurveOnSurface(TopoDS::Edge(aLocalEdge), TopoDS::Face(aLocalFace), f, l);
              //	      C2d = BRep_Tool::CurveOnSurface
              //		(TopoDS::Edge(edg.Oriented(oredonfafw)),
              //		 TopoDS::Face(fac.Oriented(TopAbs_FORWARD)),
              //		 f,l);

              if (!BRepTools::IsReallyClosed(edg, fac))
              {
                B.UpdateEdge(newedg, C2d, newface, tol);
              }
              else if (C2d1.IsNull())
              {
                C2d1 = C2d;
              }
              else
              {
                //		if (TopAbs::Compose(orsav,or) == TopAbs_FORWARD) {
                if (orient == TopAbs_FORWARD)
                {
                  B.UpdateEdge(newedg, C2d, C2d1, newface, tol);
                }
                else
                {
                  B.UpdateEdge(newedg, C2d1, C2d, newface, tol);
                }
              }
            }
            bool AddPart = false;
            if (DontFuse.IsBound(edg))
            {
              if (!BRepTools::IsReallyClosed(edg, fac))
              {
                if (DontFuse(edg).IsSame(fac))
                {
                  if (myModShapes.IsBound(edg))
                  {
                    AddPart = true;
                  }
                }
                else if (!toRemove.Contains(edg))
                {
                  AddPart = true;
                }
              }
              else
              {
                if (myModShapes.IsBound(edg))
                { // edg raccourci
                  //		  if (TopAbs::Compose(orsav,or)==DontFuse(edg).Orientation()){
                  if (orient == DontFuse(edg).Orientation())
                  {
                    AddPart = true;
                  }
                }
                else
                {
                  //		  if (TopAbs::Compose(orsav,or) ==
                  if (orient == TopAbs::Reverse(DontFuse(edg).Orientation()))
                  {
                    AddPart = true;
                  }
                }
              }
            }
            if (AddPart)
            {
              itl2.Initialize(theEEMap(edg));
              gp_Vec dir1, dir2;
              for (; itl2.More(); itl2.Next())
              {
                if (EdgAdded.Contains(itl2.Value()))
                {
                  continue;
                }
                const TopoDS_Edge& edgbis = TopoDS::Edge(itl2.Value());
                TopoDS_Iterator    it1(newedg), it2;
                for (; it1.More(); it1.Next())
                {
                  for (it2.Initialize(edgbis); it2.More(); it2.Next())
                  {
                    if (it1.Value().IsSame(it2.Value()))
                    {
                      break;
                    }
                  }
                  if (it2.More())
                  {
                    break;
                  }
                }

                if (it1.More())
                {
                  gp_Pnt ptbid;
                  double prmvt = BRep_Tool::Parameter(TopoDS::Vertex(it1.Value()), newedg);
                  C->D1(prmvt, ptbid, dir1);

                  C = BRep_Tool::Curve(edgbis, loc, f, l);
                  if (!loc.IsIdentity())
                  {
                    occ::handle<Geom_Geometry> GG = C->Transformed(loc.Transformation());
                    C                             = occ::down_cast<Geom_Curve>(GG);
                  }
                  if (C->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve))
                  {
                    C = occ::down_cast<Geom_TrimmedCurve>(C)->BasisCurve();
                  }
                  prmvt = BRep_Tool::Parameter(TopoDS::Vertex(it1.Value()), edgbis);
                  C->D1(prmvt, ptbid, dir2);
                }
                else
                {
                  dir1 = dir2 = gp_Vec(1, 0, 0);
                }
                EdgAdded.Add(edgbis);
                if (dir1.Dot(dir2) < 0.)
                {
                  //		  B.Add(newwire,edgbis.Oriented(TopAbs::Reverse(or)));
                  listofedg.Append(edgbis.Oriented(TopAbs::Reverse(orient)));
                }
                else
                {
                  //		  B.Add(newwire,edgbis.Oriented(or));
                  listofedg.Append(edgbis.Oriented(orient));
                }

                if (P.IsNull())
                {
                  // on met les courbes 2d si on n`est pas sur un plan
                  // C est la courbe de edgbis, f et l s`y rapportent
                  occ::handle<Geom2d_Curve> PTC = GeomProjLib::Curve2d(C, f, l, S, tol);
                  if (S->IsUPeriodic())
                  {
                    double Uref;
                    if (DontFuse.IsBound(edg))
                    {
                      TopAbs_Orientation oredge = DontFuse(edg).Orientation();
                      if (myModShapes.IsBound(edg))
                      { // edge raccourci...
                        TopoDS_Shape aLocalShape = edg.Oriented(oredge);
                        TopoDS_Shape aLocalFace  = fac.Oriented(TopAbs_FORWARD);
                        BRep_Tool::UVPoints(TopoDS::Edge(aLocalShape),
                                            TopoDS::Face(aLocalFace),
                                            pf,
                                            pl);
                        //			BRep_Tool::
                        //			UVPoints(TopoDS::Edge(edg.Oriented(oredge)),
                        //				 TopoDS::Face(fac.Oriented(TopAbs_FORWARD)),
                        //				 pf,pl);
                      }
                      else
                      {
                        TopoDS_Shape aLocalShape = edg.Oriented(TopAbs::Reverse(oredge));
                        TopoDS_Shape aLocalFace  = fac.Oriented(TopAbs_FORWARD);
                        BRep_Tool::UVPoints(TopoDS::Edge(aLocalShape),
                                            TopoDS::Face(aLocalFace),
                                            pf,
                                            pl);
                        //			BRep_Tool::
                        //			UVPoints(TopoDS::Edge(edg.Oriented(TopAbs::Reverse(oredge))),
                        //				 TopoDS::Face(fac.Oriented(TopAbs_FORWARD)),
                        //				 pf,pl);
                      }
                      Uref = pf.X();
                    }
                    else
                    {
                      BRep_Tool::UVPoints(edg, fac, pf, pl);
                      Uref = pf.X();
                    }

                    double NewU = (PTC->Value(f)).X();

                    //		    if(abs(NewU - Uref) > Epsilon(S->UPeriod()))   {
                    if (fabs(NewU - Uref) > Epsilon(S->UPeriod()))
                    {
                      PTC->Translate(gp_Vec2d((Uref - NewU), 0.));
                    }
                  }

                  B.UpdateEdge(edgbis, PTC, newface, tol);
                }
              }
            }
          }
        }

        // Recuperation des edges sur les faces a fusionner.

        // ICICICICI

        //	orface = TopAbs::Compose(orsav,orface);

        bool includeinw = false;
        for (itl.Initialize(itf.Value()); itl.More(); itl.Next())
        {
          TopoDS_Face facbis  = TopoDS::Face(itl.Value());
          bool        genface = true;
          for (itl2.Initialize(G->OrientedFaces()); itl2.More(); itl2.Next())
          {
            if (itl2.Value().IsSame(facbis))
            {
              break;
            }
          }
          if (itl2.More())
          {
            orient = itl2.Value().Orientation();
            facbis.Orientation(orient);
          }
          else
          { // on fusionne avec une autre face du shape...
            genface = false;
            for (exp2.Init(myShape, TopAbs_FACE); exp2.More(); exp2.Next())
            {
              if (exp2.Current().IsSame(facbis))
              {
                facbis.Orientation(exp2.Current().Orientation());
                break;
              }
            }
          }

          for (exp3.Init(facbis, TopAbs_WIRE); exp3.More(); exp3.Next())
          {
            for (exp2.Init(exp3.Current(), TopAbs_EDGE); exp2.More(); exp2.Next())
            {
              if (toRemove.Contains(exp2.Current()))
              {
                break;
              }
            }
            if (genface)
            {
              includeinw = true;
            }
            else
            {
              bool isIncludedInW = false;
              if (exp2.More())
              {
                for (exp2.ReInit(); exp2.More(); exp2.Next())
                {
                  if (!toRemove.Contains(exp2.Current()))
                  {
                    continue;
                  }
                  TopoDS_Vertex VF, VL;
                  TopExp::Vertices(TopoDS::Edge(exp2.Current()), VF, VL);
                  TopExp_Explorer exp4;
                  for (exp4.Init(wir, TopAbs_VERTEX);
                       //		  for (TopExp_Explorer exp4(wir,TopAbs_VERTEX);
                       exp4.More();
                       exp4.Next())
                  {
                    if (exp4.Current().IsSame(VF) || exp4.Current().IsSame(VL))
                    {
                      isIncludedInW = true;
                      break;
                    }
                  }
                  if (isIncludedInW)
                  {
                    break;
                  }
                }
              }
            }

            if (!includeinw)
            {
              continue;
            }

            for (exp2.ReInit(); exp2.More(); exp2.Next())
            {
              const TopoDS_Edge& edg = TopoDS::Edge(exp2.Current());
              if (!toRemove.Contains(edg) && !EdgAdded.Contains(edg))
              {

                orient = TopAbs::Compose(orface, edg.Orientation());
                //		B.Add(newwire,edg.Oriented(or));
                listofedg.Append(edg.Oriented(orient));
                EdgAdded.Add(edg);
                if (P.IsNull())
                {
                  // on met les courbes 2d si on n`est pas sur un plan
                  // on ne devrait pas avoir de pb d`edge de couture.
                  tol = BRep_Tool::Tolerance(edg);
                  C   = BRep_Tool::Curve(edg, loc, f, l);
                  if (!loc.IsIdentity())
                  {
                    occ::handle<Geom_Geometry> GG = C->Transformed(loc.Transformation());
                    C                             = occ::down_cast<Geom_Curve>(GG);
                  }
                  if (C->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve))
                  {
                    C = occ::down_cast<Geom_TrimmedCurve>(C)->BasisCurve();
                  }

                  C2d = GeomProjLib::Curve2d(C, f, l, S, tol);

                  // Tentative de recalage dans la facette
                  pf                     = C2d->Value(f);
                  pl                     = C2d->Value(l);
                  constexpr double tttol = Precision::Angular();
                  while (std::min(pf.X(), pl.X()) >= Umaxc - tttol)
                  {
                    C2d->Translate(gp_Vec2d(-2. * M_PI, 0));
                    pf = C2d->Value(f);
                    pl = C2d->Value(l);
                  }

                  while (std::max(pf.X(), pl.X()) <= Uminc + tttol)
                  {
                    C2d->Translate(gp_Vec2d(2. * M_PI, 0));
                    pf = C2d->Value(f);
                    pl = C2d->Value(l);
                  }

                  if (!BRepTools::IsReallyClosed(edg, facbis))
                  {
                    B.UpdateEdge(edg, C2d, newface, tol);
                  }
                  //		else {
                  //		  std::cout << "Edge double bizarre... " << std::endl;
                  //		}
                }
              }
            }
          }
        }
      }
    }
    if (!listofedg.IsEmpty())
    {
      BRepAlgo_Loop L;
      L.Init(newface);
      L.AddConstEdges(listofedg);
      L.Perform();
      L.WiresToFaces();
      const NCollection_List<TopoDS_Shape>& listoffaces = L.NewFaces();
      toRemove.Add(fac);
      //      if (!HasWire) {
      //	newface.Nullify();
      //      }
      myModShapes.Bind(fac, listoffaces);
      for (itl.Initialize(itf.Value()); itl.More(); itl.Next())
      {
        myModShapes.Bind(itl.Value(), listoffaces);
      }
    }
  }

  /* JAG 16.09.96 : on utilise LocOpe_BuildShape
    TopoDS_Shell theShell;
    B.MakeShell(theShell);
    for (exp.Init(myShape,TopAbs_FACE); exp.More(); exp.Next()) {
      const TopoDS_Face& fac = TopoDS::Face(exp.Current());
      if (!theLeft.Contains(fac)) {
        if (!toRemove.Contains(exp.Current())) {
      B.Add(theShell,fac);
      myModShapes.Bind(fac,fac);
        }
        else if (!myModShapes(fac).IsNull())  {
      B.Add(theShell, myModShapes(fac).Oriented(fac.Orientation()));
        }
      }
    }

    TopAbs_Orientation orsolid = myShape.Orientation();
    for (itl.Initialize(G->OrientedFaces()); itl.More(); itl.Next()) {
      const TopoDS_Face& fac = TopoDS::Face(itl.Value());
      if (toRemove.Contains(fac)) {
        continue;
      }

      if (orsolid == TopAbs_FORWARD) {
        B.Add(theShell,fac);
      }
      else {
        B.Add(theShell,fac.Reversed());
      }
      myModShapes.Bind(fac,fac);

    }

    B.MakeSolid(TopoDS::Solid(myRes));
    B.Add(myRes,theShell);
    myRes.Orientation(orsolid);

  */

  // 06.11.96
  // Debug temporaire. Il faudra prevoir un syntaxe de BuildShape
  // qui impose une ori de certaines faces.

  TopoDS_Face FaceRefOri;

  NCollection_List<TopoDS_Shape> lfres;
  for (exp.Init(myShape, TopAbs_FACE); exp.More(); exp.Next())
  {
    const TopoDS_Face& fac = TopoDS::Face(exp.Current());
    if (!theLeft.Contains(fac))
    {
      if (!toRemove.Contains(exp.Current()))
      {
        lfres.Append(fac);
        if (!myModShapes.IsBound(fac))
        {
          NCollection_List<TopoDS_Shape> emptylist;
          myModShapes.Bind(fac, emptylist);
        }
        myModShapes(fac).Append(fac);
        if (FaceRefOri.IsNull())
        {
          FaceRefOri = fac;
        }
      }
      else if (myModShapes.IsBound(fac))
      {
        lfres.Append(myModShapes(fac).First().Oriented(fac.Orientation()));
      }
    }
  }

  TopAbs_Orientation orsolid = myShape.Orientation();
  for (itl.Initialize(G->OrientedFaces()); itl.More(); itl.Next())
  {
    const TopoDS_Face& fac = TopoDS::Face(itl.Value());
    if (toRemove.Contains(fac))
    {
      continue;
    }

    if (orsolid == TopAbs_FORWARD)
    {
      lfres.Append(fac);
    }
    else
    {
      lfres.Append(fac.Reversed());
    }
    if (!myModShapes.IsBound(fac))
    {
      NCollection_List<TopoDS_Shape> emptylist;
      myModShapes.Bind(fac, emptylist);
    }
    myModShapes(fac).Append(fac);
  }

  LocOpe_BuildShape BS(lfres);
  myRes = BS.Shape();
  // Suite debug du 06.11.96
  if (myRes.ShapeType() == TopAbs_SOLID)
  {
    for (exp.Init(myRes, TopAbs_FACE); exp.More(); exp.Next())
    {
      if (exp.Current().IsSame(FaceRefOri))
      {
        break;
      }
    }
    if (exp.More() && exp.Current().Orientation() != FaceRefOri.Orientation())
    {
      // 	---C++: return const&	---C++: return const&	---C++: return const&Si un seul Shell ,
      // on change son orientation
      TopoDS_Solid NewSol;
      B.MakeSolid(NewSol);
      exp.Init(myRes, TopAbs_SHELL);
      B.Add(NewSol, exp.Current().Reversed());
      myRes.Nullify();
      myRes = NewSol;
    }
  }

  // recodage des regularites qui existaient sur le shape colle

  myDone = true;
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& LocOpe_Generator::DescendantFace(const TopoDS_Face& F)
{
  // NCollection_List<TopoDS_Shape> list;

  if (!myDone)
  {
    throw StdFail_NotDone();
  }
  return myModShapes(F);
}

//=================================================================================================

bool ToFuse(const TopoDS_Face& F1, const TopoDS_Face& F2)
{
  if (F1.IsNull() || F2.IsNull())
  {
    return false;
  }

  occ::handle<Geom_Surface>  S1, S2;
  TopLoc_Location            loc1, loc2;
  occ::handle<Standard_Type> typS1, typS2;
  constexpr double           tollin = Precision::Confusion();
  constexpr double           tolang = Precision::Angular();

  S1 = BRep_Tool::Surface(F1, loc1);
  S2 = BRep_Tool::Surface(F2, loc2);

  typS1 = S1->DynamicType();
  typS2 = S2->DynamicType();

  if (typS1 == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    S1    = occ::down_cast<Geom_RectangularTrimmedSurface>(S1)->BasisSurface();
    typS1 = S1->DynamicType();
  }

  if (typS2 == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    S2    = occ::down_cast<Geom_RectangularTrimmedSurface>(S2)->BasisSurface();
    typS2 = S2->DynamicType();
  }

  if (typS1 != typS2)
  {
    return false;
  }

  bool ValRet = false;
  if (typS1 == STANDARD_TYPE(Geom_Plane))
  {

    gp_Pln pl1(occ::down_cast<Geom_Plane>(S1)->Pln());
    gp_Pln pl2(occ::down_cast<Geom_Plane>(S2)->Pln());

    pl1.Transform(loc1.Transformation());
    pl2.Transform(loc2.Transformation());

    if (pl1.Position().IsCoplanar(pl2.Position(), tollin, tolang))
    {
      ValRet = true;
    }
  }

  return ValRet;
}

//=================================================================================================

bool ToFuse(const TopoDS_Edge& E1, const TopoDS_Edge& E2)
{

  if (E1.IsNull() || E2.IsNull())
  {
    return false;
  }

  occ::handle<Geom_Curve>    C1, C2;
  TopLoc_Location            loc1, loc2;
  occ::handle<Standard_Type> typC1, typC2;
  constexpr double           tollin = Precision::Confusion();
  constexpr double           tolang = Precision::Angular();
  double                     f, l;

  C1 = BRep_Tool::Curve(E1, loc1, f, l);
  if (!loc1.IsIdentity())
  {
    occ::handle<Geom_Geometry> CC1 = C1->Transformed(loc1.Transformation());
    C1                             = occ::down_cast<Geom_Curve>(CC1);
  }

  C2 = BRep_Tool::Curve(E2, loc2, f, l);
  if (!loc2.IsIdentity())
  {
    occ::handle<Geom_Geometry> CC2 = C2->Transformed(loc2.Transformation());
    C2                             = occ::down_cast<Geom_Curve>(CC2);
  }

  typC1 = C1->DynamicType();
  typC2 = C2->DynamicType();

  if (typC1 == STANDARD_TYPE(Geom_TrimmedCurve))
  {
    C1    = occ::down_cast<Geom_TrimmedCurve>(C1)->BasisCurve();
    typC1 = C1->DynamicType();
  }
  if (typC2 == STANDARD_TYPE(Geom_TrimmedCurve))
  {
    C2    = occ::down_cast<Geom_TrimmedCurve>(C2)->BasisCurve();
    typC2 = C2->DynamicType();
  }

  if (typC1 != typC2)
  {
    return false;
  }

  bool ValRet = false;
  if (typC1 == STANDARD_TYPE(Geom_Line))
  {
    gp_Lin li1(occ::down_cast<Geom_Line>(C1)->Lin());
    gp_Lin li2(occ::down_cast<Geom_Line>(C2)->Lin());

    if (li1.Position().IsCoaxial(li2.Position(), tolang, tollin))
    {
      ValRet = true;
    }
  }

  return ValRet;
}

//=================================================================================================

bool ToFuse(const TopoDS_Edge&                                            E,
            const TopoDS_Face&                                            F,
            const TopoDS_Vertex&                                          V,
            const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& toRemove)
{
  TopoDS_Vertex   Vf, Vl;
  TopExp_Explorer exp;
  for (exp.Init(F, TopAbs_EDGE); exp.More(); exp.Next())
  {
    //  for (TopExp_Explorer exp(F,TopAbs_EDGE); exp.More(); exp.Next()) {
    const TopoDS_Edge& eee = TopoDS::Edge(exp.Current());
    if (!eee.IsSame(E))
    {
      TopExp::Vertices(eee, Vf, Vl);
      if ((Vf.IsSame(V) || Vl.IsSame(V)) && !toRemove.Contains(eee))
      {
        return false;
      }
    }
  }
  return true;
}

//=================================================================================================

double NewParameter(const TopoDS_Edge&   Edg,
                    const TopoDS_Vertex& Vtx,
                    const TopoDS_Edge&   NewEdg,
                    const TopoDS_Vertex& NewVtx)
{

  occ::handle<Geom_Curve>    C;
  TopLoc_Location            loc;
  occ::handle<Standard_Type> typC;
  double                     f, l;

  gp_Pnt P = BRep_Tool::Pnt(NewVtx);

  C = BRep_Tool::Curve(Edg, loc, f, l);
  if (!loc.IsIdentity())
  {
    occ::handle<Geom_Geometry> GG = C->Transformed(loc.Transformation());
    C                             = occ::down_cast<Geom_Curve>(GG);
  }
  typC = C->DynamicType();
  if (typC == STANDARD_TYPE(Geom_TrimmedCurve))
  {
    C    = occ::down_cast<Geom_TrimmedCurve>(C)->BasisCurve();
    typC = C->DynamicType();
  }

  if (typC == STANDARD_TYPE(Geom_Line))
  {
    return ElCLib::Parameter(occ::down_cast<Geom_Line>(C)->Lin(), P);
  }
  else if (typC == STANDARD_TYPE(Geom_Circle))
  {
    double prm = ElCLib::Parameter(occ::down_cast<Geom_Circle>(C)->Circ(), P);
    // Vtx vient d`une exploration de Edg orientee FORWARD

    TopAbs_Orientation orient = TopAbs::Reverse(Vtx.Orientation());
    if (orient == TopAbs_FORWARD || orient == TopAbs_REVERSED)
    {
      //      for (TopExp_Explorer exp(NewEdg.Oriented(TopAbs_FORWARD),TopAbs_VERTEX);
      TopExp_Explorer exp(NewEdg.Oriented(TopAbs_FORWARD), TopAbs_VERTEX);
      for (; exp.More(); exp.Next())
      {
        if (exp.Current().Orientation() == orient)
        {
          break;
        }
      }
      if (exp.More())
      {
        double prmmax = BRep_Tool::Parameter(TopoDS::Vertex(exp.Current()), NewEdg);
        if (std::abs(prmmax - prm) <= Epsilon(2. * M_PI))
        {
          if (orient == TopAbs_REVERSED)
          {
            prm -= 2. * M_PI;
          }
          else
          {
            prm += 2. * M_PI;
          }
        }
      }
    }
    return prm;
  }
  return 0;
}
