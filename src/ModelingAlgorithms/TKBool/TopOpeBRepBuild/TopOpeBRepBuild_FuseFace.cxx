// Created on: 1998-07-28
// Created by: LECLERE Florence
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

#include <TopOpeBRepBuild_FuseFace.hxx>

#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>

#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>

#include <TopExp_Explorer.hxx>

#include <TopoDS.hxx>

#include <BRepLib_MakeWire.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLib_MakeEdge.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepCheck_Analyzer.hxx>

#include <Geom_Surface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <ElCLib.hxx>
#include <Precision.hxx>

#ifdef OCCT_DEBUG
extern bool TopOpeBRepBuild_GettraceFUFA();
#endif

static void GroupShape(
  NCollection_List<TopoDS_Shape>&,
  bool,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&);

static void GroupEdge(
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&);

static void MakeEdge(
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&);

static bool SameSupport(const TopoDS_Edge&, const TopoDS_Edge&);

//=================================================================================================

void TopOpeBRepBuild_FuseFace::Init(const NCollection_List<TopoDS_Shape>& LIF,
                                    const NCollection_List<TopoDS_Shape>& LRF,
                                    const int                             CXM)
{
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepBuild_GettraceFUFA();
  if (trc)
    std::cout << "TopOpeBRepBuild_FuseFace::Init" << std::endl;
#endif
  myLIF = LIF;
  myLRF = LRF;
  if (CXM == 1)
  {
    myInternal = false;
  }
  else if (CXM == 2)
  {
    myInternal = true;
  } // CXM
#ifdef OCCT_DEBUG
  if (trc)
  {
    if (myInternal)
    {
      std::cout << " TopOpeBRepBuild_FuseFace::Init : Keep internal connections" << std::endl;
    }
    else
    {
      std::cout << " TopOpeBRepBuild_FuseFace::Init : Suppress internal connections" << std::endl;
    }
  }
#endif

  myLFF.Clear();

  myLIE.Clear();
  myLEE.Clear();
  myLME.Clear();

  myLIV.Clear();
  myLEV.Clear();
  myLMV.Clear();

  myModified = false;
  myDone     = false;
}

//=======================================================================
// function : PerformFace
// purpose  : fusion des faces cosurfaciques, connexes par une ou
// plusieurs aretes
//=======================================================================

void TopOpeBRepBuild_FuseFace::PerformFace()
{
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepBuild_GettraceFUFA();
  if (trc)
    std::cout << "TopOpeBRepBuild_FuseFace::PerformFace()" << std::endl;
#endif

  myModified = false;
  myLFF.Clear();
  if (myLRF.IsEmpty())
  {
#ifdef OCCT_DEBUG
    if (trc)
      std::cout << " TopOpeBRepBuild_FuseFace::PerformFace : Empty list of reconstructed faces"
                << std::endl;
#endif
    myModified = false;
    myDone     = true;
    myLFF      = myLRF;
    return;
  }

  int number = myLRF.Extent();
  if (number == 1)
  {
#ifdef OCCT_DEBUG
    if (trc)
      std::cout << " TopOpeBRepBuild_FuseFace::PerformFace : only 1 reconstructed face"
                << std::endl;
#endif
    myModified = false;
    myDone     = true;
    myLFF      = myLRF;
    return;
  }

  NCollection_List<TopoDS_Shape>::Iterator it2, it3, it4;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator itt1,
    itt2, itt3;
  TopAbs_Orientation ori1;

  bool                           Ori3dReversed = false;
  bool                           Ori3dForward  = false;
  NCollection_List<TopoDS_Shape> mylist;
  for (it2.Initialize(myLRF); it2.More(); it2.Next())
  {
    TopoDS_Shape fac = it2.Value();
    ori1             = fac.Orientation();
    if (ori1 == TopAbs_FORWARD)
    {
      Ori3dForward = true;
    }
    if (ori1 == TopAbs_REVERSED)
    {
      Ori3dReversed = true;
    }
    BRepCheck_Analyzer ana(fac);
    if (!ana.IsValid(fac))
    {
//    if (!BRepCheck_Analyzer::IsValid(fac)) {
#ifdef OCCT_DEBUG
      if (trc)
        std::cout << " TopOpeBRepBuild_FuseFace::PerformFace : Invalid reconstructed face"
                  << std::endl;
#endif
      myModified = false;
      myDone     = true;
      myLFF      = myLRF;
      return;
    }
    fac.Orientation(TopAbs_FORWARD);
    mylist.Append(fac);
  }

  // Orientation 3d de l'espace limite par la face
  if (Ori3dForward && Ori3dReversed)
  {
#ifdef OCCT_DEBUG
    if (trc)
      std::cout << " TopOpeBRepBuild_FuseFace::PerformFace : Bad faces 3d orientation" << std::endl;
#endif
    myModified = false;
    myDone     = true;
    myLFF      = myLRF;
    return;
  }

  // listes de faces avec edges communes.
  bool Keep_Edge;
  Keep_Edge = false;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    mapFacLFac;
  GroupShape(mylist, Keep_Edge, mapFacLFac);
  if (mapFacLFac.IsEmpty())
  {
#ifdef OCCT_DEBUG
    if (trc)
      std::cout << " TopOpeBRepBuild_FuseFace::PerformFace : Empty list of faces" << std::endl;
#endif
    myModified = false;
    myDone     = true;
    myLFF      = myLRF;
    return;
  }
  int n1 = myLRF.Extent();
  int n2 = mapFacLFac.Extent();
  if (n1 == n2)
  {
#ifdef OCCT_DEBUG
    if (trc)
      std::cout << " TopOpeBRepBuild_FuseFace::PerformFace : No connection" << std::endl;
#endif
    myModified = false;
    myDone     = true;
    myLFF      = myLRF;
    return;
  }

  // loop over the face lists for each face in LRF

  for (itt1.Initialize(mapFacLFac); itt1.More(); itt1.Next())
  {
    const TopoDS_Shape&                   fac    = itt1.Key();
    TopoDS_Face                           facref = TopoDS::Face(fac);
    const NCollection_List<TopoDS_Shape>& LFac   = mapFacLFac.Find(fac);

    int n11 = LFac.Extent();
    if (n11 != 1)
    {
      NCollection_List<TopoDS_Shape> LWir;
      for (it2.Initialize(LFac); it2.More(); it2.Next())
      {
        const TopoDS_Shape& fac1 = it2.Value();

        TopExp_Explorer exp;
        for (exp.Init(fac1, TopAbs_WIRE); exp.More(); exp.Next())
        {
          const TopoDS_Shape& wir = exp.Current();
          LWir.Append(wir);
        }
      } // LFac
      //  listes des wires avec edges communes.
      Keep_Edge = false;
      NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
        mapWirLWir;
      GroupShape(LWir, Keep_Edge, mapWirLWir);
      if (mapWirLWir.IsEmpty())
      {
#ifdef OCCT_DEBUG
        if (trc)
          std::cout << " TopOpeBRepBuild_FuseFace::PerformFace : Empty list of wires" << std::endl;
#endif
        myModified = false;
        myDone     = true;
        myLFF      = myLRF;
        return;
      }

      //  boucle sur les listes des wires de 1 face de LRF
      NCollection_List<TopoDS_Shape> myFaceLIE, myFaceLEE, myFaceLME, myFaceLW;
      for (itt2.Initialize(mapWirLWir); itt2.More(); itt2.Next())
      {
        const TopoDS_Shape&                   wir   = itt2.Key();
        const NCollection_List<TopoDS_Shape>& LWir1 = mapWirLWir.Find(wir);

        int n22 = LWir1.Extent();
        if (n22 != 1)
        {
          //    boucle sur 1 liste des wires avec edges communes.
          NCollection_List<TopoDS_Shape> LEdg;
          for (it3.Initialize(LWir1); it3.More(); it3.Next())
          {
            const TopoDS_Shape& wir1 = it3.Value();

            TopExp_Explorer exp;
            for (exp.Init(wir1, TopAbs_EDGE); exp.More(); exp.Next())
            {
              const TopoDS_Shape& edg = exp.Current();
              LEdg.Append(edg);
            }
          } // LWir1
          //    listes des edges avec edges communes.
          Keep_Edge = true;
          NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
            mapEdgLEdg;
          GroupShape(LEdg, Keep_Edge, mapEdgLEdg);
          if (mapEdgLEdg.IsEmpty())
          {
#ifdef OCCT_DEBUG
            if (trc)
              std::cout << " TopOpeBRepBuild_FuseFace::PerformFace : Empty list of edges"
                        << std::endl;
#endif
            myModified = false;
            myDone     = true;
            myLFF      = myLRF;
            return;
          }

          //    Elimination selon logique pure
          //    boucle sur les listes des egdes de 1 wire de 1 face de LRF
          NCollection_List<TopoDS_Shape> myWireLE;
          for (itt3.Initialize(mapEdgLEdg); itt3.More(); itt3.Next())
          {
            const TopoDS_Shape&                   edg         = itt3.Key();
            const NCollection_List<TopoDS_Shape>& LEdg1       = mapEdgLEdg.Find(edg);
            bool                                  OriReversed = false;
            bool                                  OriForward  = false;
            bool                                  OriInternal = false;
            bool                                  OriExternal = false;
            for (it4.Initialize(LEdg1); it4.More(); it4.Next())
            {
              const TopoDS_Shape& edg1 = it4.Value();
              ori1                     = edg1.Orientation();
              if (ori1 == TopAbs_REVERSED)
              {
                if (OriReversed)
                {
#ifdef OCCT_DEBUG
                  if (trc)
                    std::cout << " TopOpeBRepBuild_FuseFace::PerformFace : Bad faces orientation"
                              << std::endl;
#endif
                  myModified = false;
                  myDone     = true;
                  myLFF      = myLRF;
                  return;
                }
                OriReversed = true;
              }
              else if (ori1 == TopAbs_FORWARD)
              {
                if (OriForward)
                {
#ifdef OCCT_DEBUG
                  if (trc)
                    std::cout << " TopOpeBRepBuild_FuseFace::PerformFace : Bad faces orientation"
                              << std::endl;
#endif
                  myModified = false;
                  myDone     = true;
                  myLFF      = myLRF;
                  return;
                }
                OriForward = true;
              }
              else if (ori1 == TopAbs_INTERNAL)
              {
                OriInternal = true;
              }
              else if (ori1 == TopAbs_EXTERNAL)
              {
                OriExternal = true;
              }
            } // LEdg1

            //      - Traitement edge selon orientation
            //      On privilegie orientation selon 1) reversed ou forward
            //                                      2) internal
            //                                      3) external
            //      pour traiter cas ou l'on a au moins 2 orientations differentes parmi
            //           forward et reversed - interne - externe

            if (OriReversed || OriForward)
            {
              if (OriReversed && OriForward)
              {
                //		TopoDS_Shape& edg1 = edg.Oriented(TopAbs_INTERNAL);
                const TopoDS_Shape& edg1 = edg.Oriented(TopAbs_INTERNAL);
                myLME.Append(edg1);
                myFaceLME.Append(edg1);
              }
              else if (OriReversed)
              {
                //		TopoDS_Shape& edg1 = edg.Oriented(TopAbs_REVERSED);
                const TopoDS_Shape& edg1 = edg.Oriented(TopAbs_REVERSED);
                myWireLE.Append(edg1);
              }
              else
              {
                //		TopoDS_Shape& edg1 = edg.Oriented(TopAbs_FORWARD);
                const TopoDS_Shape& edg1 = edg.Oriented(TopAbs_FORWARD);
                myWireLE.Append(edg1);
              }
            }
            else if (OriInternal)
            {
              //	      TopoDS_Shape& edg1 = edg.Oriented(TopAbs_INTERNAL);
              const TopoDS_Shape& edg1 = edg.Oriented(TopAbs_INTERNAL);
              myLIE.Append(edg1);
              myFaceLIE.Append(edg1);
            }
            else if (OriExternal)
            {
              //	      TopoDS_Shape& edg1 = edg.Oriented(TopAbs_EXTERNAL);
              const TopoDS_Shape& edg1 = edg.Oriented(TopAbs_EXTERNAL);
              myLEE.Append(edg1);
              myFaceLEE.Append(edg1);
            } // Ori
          } // mapEdgLEdg

          //    Reconstrution de 1 wire de 1 face de LRF
          //    Attention cas ou une liste de wire connectes conduit a plusieurs Wires
          int number1 = myWireLE.Extent();
          while (number1 > 0)
          {
            BRepLib_MakeWire MW;
            MW.Add(myWireLE);
            if (!MW.IsDone())
            {
#ifdef OCCT_DEBUG
              if (trc)
                std::cout << " TopOpeBRepBuild_FuseFace::PerformFace : Failure in making wire"
                          << std::endl;
#endif
              myModified = false;
              myDone     = true;
              myLFF      = myLRF;
              return;
            }

            //    Astuce pour contourner Wire Not Closed
            TopoDS_Wire      W = MW.Wire();
            BRepLib_MakeWire MW1(W);
            W = MW1.Wire();

            myFaceLW.Append(W);

            TopExp_Explorer                                        exp;
            NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> M;
            int                                                    nb = 0;
            for (exp.Init(W, TopAbs_EDGE); exp.More(); exp.Next())
            {
              const TopoDS_Shape& edg3 = exp.Current();
              M.Add(edg3);
              nb++;
            }

            if (nb == number1)
            {
              number1 = 0;
            }
            else
            {
              NCollection_List<TopoDS_Shape> ListEdge;
              for (it3.Initialize(myWireLE); it3.More(); it3.Next())
              {
                const TopoDS_Shape& edg2 = it3.Value();
                if (M.Add(edg2))
                {
                  ListEdge.Append(edg2);
                }
              }
              myWireLE.Assign(ListEdge);
              number1 = myWireLE.Extent();
            } // nb
          } // number
        }
        else
        {
          myFaceLW.Append(wir);
        } // n2 =1
      } // mapWirLWir

      //  Reconstrution de 1 face de LRF
      occ::handle<Geom_Surface> S = BRep_Tool::Surface(facref);
      if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
      {
        S = occ::down_cast<Geom_RectangularTrimmedSurface>(S)->BasisSurface();
      }
      BRepLib_MakeFace MF(S, Precision::Confusion());

      for (it2.Initialize(myFaceLW); it2.More(); it2.Next())
      {
        const TopoDS_Wire& wir1 = TopoDS::Wire(it2.Value());
        MF.Add(wir1);
      }

      // Ajout des Edges Internes
      //                 Externes
      //                 Modifiees
      for (it2.Initialize(myFaceLIE); it2.More(); it2.Next())
      {
        const TopoDS_Edge& edg1 = TopoDS::Edge(it2.Value());
        BRepLib_MakeWire   MW(edg1);
        //      MW.Add(edg1);
        const TopoDS_Wire& W = MW.Wire();
        MF.Add(W);
      }
      for (it2.Initialize(myFaceLEE); it2.More(); it2.Next())
      {
        const TopoDS_Edge& edg1 = TopoDS::Edge(it2.Value());
        BRepLib_MakeWire   MW(edg1);
        //      MW.Add(edg1);
        const TopoDS_Wire& W = MW.Wire();
        MF.Add(W);
      }
      if (myInternal)
      {
        for (it2.Initialize(myFaceLME); it2.More(); it2.Next())
        {
          const TopoDS_Edge& edg1 = TopoDS::Edge(it2.Value());
          BRepLib_MakeWire   MW(edg1);
          //	  MW.Add(edg1);
          const TopoDS_Wire& W = MW.Wire();
          MF.Add(W);
        }
      }

      if (!MF.IsDone())
      {
#ifdef OCCT_DEBUG
        if (trc)
          std::cout << " TopOpeBRepBuild_FuseFace::PerformFace : Failure in making face"
                    << std::endl;
#endif
        myModified = false;
        myDone     = true;
        myLFF      = myLRF;
        return;
      }
      TopoDS_Face F = MF.Face();
      if (Ori3dReversed)
      {
        F.Reverse();
      }
      myLFF.Append(F);
    }
    else
    {
      myLFF.Append(facref);
    } // n1 = 1
  } // mapFacLFac

  if (myLFF.IsEmpty())
  {
#ifdef OCCT_DEBUG
    if (trc)
      std::cout << " TopOpeBRepBuild_FuseFace::PerformFace : Empty list of fusionned faces"
                << std::endl;
#endif
    myModified = false;
    myDone     = true;
    myLFF      = myLRF;
    return;
  }

  myModified = true;
  myDone     = true;

#ifdef OCCT_DEBUG
  if (trc)
    std::cout << " TopOpeBRepBuild_FuseFace::PerformFace() : Done" << std::endl;
#endif
}

//=======================================================================
// function : PerformEdge
// purpose  : fusion des edges cosurfaciques, connexes par une ou
// plusieurs aretes
//=======================================================================

void TopOpeBRepBuild_FuseFace::PerformEdge()
{
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepBuild_GettraceFUFA();
  if (trc)
    std::cout << "TopOpeBRepBuild_FuseFace::PerformEdge()" << std::endl;
#endif
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    mapVerLEdg, mapTampon;

  NCollection_List<TopoDS_Shape>::Iterator it1;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator itt1;
  //  TopAbs_Orientation ori,ori1;

  // Niveau 1
  // loop over the face lists for each face in LRF

  for (it1.Initialize(myLFF); it1.More(); it1.Next())
  {
    const TopoDS_Shape& fac = it1.Value();

    TopExp_Explorer expw;
    for (expw.Init(fac, TopAbs_WIRE); expw.More(); expw.Next())
    {
      const TopoDS_Shape& wir = expw.Current();

      TopExp_Explorer expe;
      for (expe.Init(wir, TopAbs_EDGE); expe.More(); expe.Next())
      {
        const TopoDS_Shape& edg = expe.Current();

        TopExp_Explorer expv;
        for (expv.Init(edg, TopAbs_VERTEX); expv.More(); expv.Next())
        {
          const TopoDS_Shape& ver = expv.Current();
          if (!mapVerLEdg.IsBound(ver))
          {
            NCollection_List<TopoDS_Shape> LmapEdg;
            LmapEdg.Append(edg);
            mapVerLEdg.Bind(ver, LmapEdg);
          }
          else
          {
            NCollection_List<TopoDS_Shape>& LmapEdg = mapVerLEdg.ChangeFind(ver);
            LmapEdg.Append(edg);
          }
        }
      }
    }
  }

  // nettoyage du tableau mapVerLSh : shap1 : shap1 shap2 shap3
  // On ne garde que les vertex qui appartiennent a - exactement 2 edges
  //                                                - de meme support geometrique
  mapTampon = mapVerLEdg;
  mapVerLEdg.Clear();

  for (itt1.Initialize(mapTampon); itt1.More(); itt1.Next())
  {
    const TopoDS_Shape&                   ver     = itt1.Key();
    const NCollection_List<TopoDS_Shape>& LmapEdg = mapTampon.Find(ver);
    int                                   number  = LmapEdg.Extent();
    if (number == 2)
    {
      it1.Initialize(LmapEdg);
      const TopoDS_Edge& edg1 = TopoDS::Edge(it1.Value());
      it1.Next();
      const TopoDS_Edge& edg2 = TopoDS::Edge(it1.Value());
      if (SameSupport(edg1, edg2))
      {
        mapVerLEdg.Bind(ver, LmapEdg);
      }
    }
  }

  // On regroupe ensemble tous les edges consecutifs et SameSupport
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    mapEdgLEdg;
  GroupEdge(mapVerLEdg, mapEdgLEdg);

  // On construit les edges somme des edges consecutifs et SameSupport
  MakeEdge(mapEdgLEdg);

  myModified = true;
  myDone     = true;

#ifdef OCCT_DEBUG
  if (trc)
    std::cout << " TopOpeBRepBuild_FuseFace::PerformEdge() : Done" << std::endl;
#endif
}

//=======================================================================
// function : ClearEdge
// purpose  : Nettoyage des Faces : Suppression des edges internes et externes
//=======================================================================

void TopOpeBRepBuild_FuseFace::ClearEdge()
{
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepBuild_GettraceFUFA();
  if (trc)
    std::cout << "TopOpeBRepBuild_FuseFace::ClearEdge()" << std::endl;
#endif

  NCollection_List<TopoDS_Shape>::Iterator it1, it2;
  TopAbs_Orientation                       ori;
  NCollection_List<TopoDS_Shape>           myLFFnew;

  // Niveau 1
  // loop over the face lists for each face in LRF

  for (it1.Initialize(myLFF); it1.More(); it1.Next())
  {
    const TopoDS_Shape& fac = it1.Value();

    NCollection_List<TopoDS_Shape> myFaceLW;
    TopExp_Explorer                expw;
    for (expw.Init(fac, TopAbs_WIRE); expw.More(); expw.Next())
    {
      const TopoDS_Shape& wir = expw.Current();

      NCollection_List<TopoDS_Shape> myWireLE;
      TopExp_Explorer                expe;
      for (expe.Init(wir, TopAbs_EDGE); expe.More(); expe.Next())
      {
        const TopoDS_Shape& edg = expe.Current();

        //    Elimination selon des edges interne et externe

        ori = edg.Orientation();
        if (ori == TopAbs_INTERNAL)
        {
          myLIE.Append(edg);
        }
        else if (ori == TopAbs_EXTERNAL)
        {
          myLEE.Append(edg);
        }
        else
        {
          myWireLE.Append(edg);
        }
      }
      //    Fin Niveau 3
      //    Reconstrution de 1 wire de 1 face de LRF
      if (!myWireLE.IsEmpty())
      {
        BRepLib_MakeWire MW;
        MW.Add(myWireLE);
        if (!MW.IsDone())
        {
#ifdef OCCT_DEBUG
          if (trc)
            std::cout << " TopOpeBRepBuild_FuseFace::ClearEdge : Failure in making wire"
                      << std::endl;
#endif
          myModified = false;
          myDone     = true;
          myLFF      = myLRF;
          return;
        }

        //    Astuce pour contourner Wire Not Closed
        TopoDS_Wire      W = MW.Wire();
        BRepLib_MakeWire MW1(W);
        W = MW1.Wire();

        myFaceLW.Append(W);
      }
    }
    //  Fin Niveau 2
    //  Reconstrution de 1 face de LRF
    if (myFaceLW.IsEmpty())
    {
#ifdef OCCT_DEBUG
      if (trc)
        std::cout << " TopOpeBRepBuild_FuseFace::ClearEdge : Empty list of wires" << std::endl;
#endif
      myModified = false;
      myDone     = true;
      myLFF      = myLRF;
      return;
    }
    it2.Initialize(myFaceLW);
    const TopoDS_Wire& wir       = TopoDS::Wire(it2.Value());
    const bool         OnlyPlane = false;
    BRepLib_MakeFace   MF(wir, OnlyPlane);

    it2.Next();
    for (; it2.More(); it2.Next())
    {
      const TopoDS_Wire& wir1 = TopoDS::Wire(it2.Value());
      MF.Add(wir1);
    }
    if (!MF.IsDone())
    {
#ifdef OCCT_DEBUG
      if (trc)
        std::cout << " TopOpeBRepBuild_FuseFace::ClearEdge : Failure in making face" << std::endl;
#endif
      myModified = false;
      myDone     = true;
      myLFF      = myLRF;
      return;
    }
    const TopoDS_Face& F = MF.Face();
    myLFFnew.Append(F);
  }
  // Fin Niveau 1
  if (myLFFnew.IsEmpty())
  {
#ifdef OCCT_DEBUG
    if (trc)
      std::cout << " TopOpeBRepBuild_FuseFace::ClearEdge : Empty list of fusionned faces"
                << std::endl;
#endif
    myModified = false;
    myDone     = true;
    myLFF      = myLRF;
    return;
  }
  myLFF = myLFFnew;

  myModified = true;
  myDone     = true;

#ifdef OCCT_DEBUG
  if (trc)
    std::cout << " TopOpeBRepBuild_FuseFace::ClearEdge() : Done" << std::endl;
#endif
}

//=======================================================================
// function : ClearVertex
// purpose  : Nettoyage des Faces : Suppression des vertex internes et externes
//=======================================================================

void TopOpeBRepBuild_FuseFace::ClearVertex()
{
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepBuild_GettraceFUFA();
  if (trc)
    std::cout << "TopOpeBRepBuild_FuseFace::ClearVertex()" << std::endl;
#endif

#ifdef OCCT_DEBUG
  if (trc)
    std::cout << " TopOpeBRepBuild_FuseFace::ClearVertex() : Done" << std::endl;
#endif
}

//=================================================================================================

static void GroupShape(
  NCollection_List<TopoDS_Shape>& mylist,
  bool                            Keep_Edge,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    mymapShLSh)
{
  NCollection_List<TopoDS_Shape>::Iterator it, it1, it2;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                 mapEdgLSh, mapShLSh;
  NCollection_List<TopoDS_Shape> LmapSh4;
  TopAbs_Orientation             ori;

  // construction du tableau C=locmapEdgLSh : egde1 - shap1 shap2 shap3
  // construction du tableau   locmapShLSh  : shap1 - shap1 shap2 shap3
  LmapSh4.Clear();
  for (it.Initialize(mylist); it.More(); it.Next())
  {
    const TopoDS_Shape&            shap1 = it.Value();
    NCollection_List<TopoDS_Shape> LmapSh;
    LmapSh.Append(shap1);

    mapShLSh.Bind(shap1, LmapSh);

    TopExp_Explorer expe;
    for (expe.Init(shap1, TopAbs_EDGE); expe.More(); expe.Next())
    {
      const TopoDS_Shape& edg1 = expe.Current();
      //    verification si Edge a prendre en compte
      ori          = edg1.Orientation();
      bool Edge_OK = true;
      if (ori == TopAbs_INTERNAL || ori == TopAbs_EXTERNAL)
      {
        Edge_OK = false;
      }
      if (Edge_OK || Keep_Edge)
      {
        if (!mapEdgLSh.IsBound(edg1))
        {
          NCollection_List<TopoDS_Shape> LmapEdg;
          LmapEdg.Append(shap1);
          mapEdgLSh.Bind(edg1, LmapEdg);
        }
        else
        {
          NCollection_List<TopoDS_Shape>& LmapEdg = mapEdgLSh.ChangeFind(edg1);
          LmapEdg.Append(shap1);

          if (!Keep_Edge)
          {

            //          Recuperation premier shape de liste liee a edg1
            it1.Initialize(LmapEdg);
            const TopoDS_Shape& shap2 = it1.Value();

            //          Controle si premier shape et shape courant sont deja lies
            NCollection_List<TopoDS_Shape> LmapSh1;
            LmapSh1 = mapShLSh.Find(shap2);
            for (it1.Initialize(LmapSh1); it1.More(); it1.Next())
            {
              const TopoDS_Shape& shap = it1.Value();
              if (shap.IsSame(shap1))
              {
                break;
              }
            }
            //          Premier shape et Shape courant ne sont pas deja lies
            if (!it1.More())
            {
              const NCollection_List<TopoDS_Shape>& LmapSh11 = mapShLSh.Find(shap1);
              const NCollection_List<TopoDS_Shape>& LmapSh2  = mapShLSh.Find(shap2);
              NCollection_List<TopoDS_Shape>        Lmap1;
              NCollection_List<TopoDS_Shape>        Lmap2;
              Lmap1.Assign(LmapSh11);
              Lmap2.Assign(LmapSh2);

              for (it2.Initialize(Lmap1); it2.More(); it2.Next())
              {
                const TopoDS_Shape&             shap = it2.Value();
                NCollection_List<TopoDS_Shape>& Lmap = mapShLSh.ChangeFind(shap);
                NCollection_List<TopoDS_Shape>  Lmap3;
                Lmap3.Assign(Lmap2);
                Lmap.Append(Lmap3);
              }
              for (it2.Initialize(Lmap2); it2.More(); it2.Next())
              {
                const TopoDS_Shape&             shap = it2.Value();
                NCollection_List<TopoDS_Shape>& Lmap = mapShLSh.ChangeFind(shap);
                NCollection_List<TopoDS_Shape>  Lmap3;
                Lmap3.Assign(Lmap1);
                Lmap.Append(Lmap3);
              }
            }
          }
        }
      }
    }
  }

  // nettoyage du tableau mapShLSh : shap1 : shap1 shap2 shap3
  mymapShLSh.Clear();

  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator itt;
  if (!Keep_Edge)
  {
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> M;
    for (itt.Initialize(mapShLSh); itt.More(); itt.Next())
    {
      const TopoDS_Shape& shap1 = itt.Key();
      if (M.Add(shap1))
      {
        const NCollection_List<TopoDS_Shape>& LmapSh = mapShLSh.Find(shap1);
        mymapShLSh.Bind(shap1, LmapSh);

        for (it1.Initialize(LmapSh); it1.More(); it1.Next())
        {
          const TopoDS_Shape& shap2 = it1.Value();
          M.Add(shap2);
        }
      }
    }
  }
  else
  {
    mymapShLSh = mapEdgLSh;
  }
}

//=================================================================================================

static void GroupEdge(
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    mymapVerLEdg,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    mymapEdgLEdg)
{
  NCollection_List<TopoDS_Shape>::Iterator it1, it2;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator itt;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    mapEdgLEdg;

  // construction du tableau C=locmapEdgLSh : egde1 - shap1 shap2 shap3
  // construction du tableau   locmapShLSh  : shap1 - shap1 shap2 shap3
  for (itt.Initialize(mymapVerLEdg); itt.More(); itt.Next())
  {
    const TopoDS_Shape&            ver1 = itt.Key();
    NCollection_List<TopoDS_Shape> LmapEdg;
    LmapEdg = mymapVerLEdg.Find(ver1);

    it1.Initialize(LmapEdg);
    const TopoDS_Edge& edg1 = TopoDS::Edge(it1.Value());
    it1.Next();
    const TopoDS_Edge& edg2 = TopoDS::Edge(it1.Value());

    bool        Edge1Add, Edge2Add;
    TopoDS_Edge edgold, edgnew;
    if (mapEdgLEdg.IsBound(edg1))
    {
      Edge1Add = false;
      edgold   = edg1;
    }
    else
    {
      Edge1Add = true;
      edgnew   = edg1;
    }
    if (mapEdgLEdg.IsBound(edg2))
    {
      Edge2Add = false;
      edgold   = edg2;
    }
    else
    {
      Edge2Add = true;
      edgnew   = edg2;
    }

    if (!(Edge1Add || Edge2Add))
    {
      continue;
    }
    else if (Edge1Add && Edge2Add)
    {
      mapEdgLEdg.Bind(edg1, LmapEdg);
      mapEdgLEdg.Bind(edg2, LmapEdg);
    }
    else
    {

      //    Recuperation premier shape de liste liee a edg1 et mise a jour

      NCollection_List<TopoDS_Shape> LmapEdg11;
      LmapEdg11.Append(edgnew);
      mapEdgLEdg.Bind(edgnew, LmapEdg11);

      NCollection_List<TopoDS_Shape> LmapEdg1;
      LmapEdg1 = mapEdgLEdg.Find(edgold);

      for (it2.Initialize(LmapEdg1); it2.More(); it2.Next())
      {
        const TopoDS_Shape&             edg22    = it2.Value();
        NCollection_List<TopoDS_Shape>& LmapEdg2 = mapEdgLEdg.ChangeFind(edgnew);
        LmapEdg2.Append(edg22);
        NCollection_List<TopoDS_Shape>& LmapEdg3 = mapEdgLEdg.ChangeFind(edg22);
        LmapEdg3.Append(edgnew);
      }
    }
  }

  // nettoyage du tableau mapEdgLedg : edg1 : edg1 edg2 edg3

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> M;

  for (itt.Initialize(mapEdgLEdg); itt.More(); itt.Next())
  {
    const TopoDS_Shape& edg1 = itt.Key();
    if (M.Add(edg1))
    {
      const NCollection_List<TopoDS_Shape>& LmapEdg = mapEdgLEdg.Find(edg1);
      mymapEdgLEdg.Bind(edg1, LmapEdg);

      for (it1.Initialize(LmapEdg); it1.More(); it1.Next())
      {
        const TopoDS_Shape& edg2 = it1.Value();
        M.Add(edg2);
      }
    }
  }
}

//=================================================================================================

static void MakeEdge(
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    mymapEdgLEdg)
{
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepBuild_GettraceFUFA();
#endif

  NCollection_List<TopoDS_Shape>::Iterator it;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator                                                                itt1;
  NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>::Iterator itt2;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    mapEdgLEdg;

  // construction du tableau C=locmapEdgLSh : egde1 - shap1 shap2 shap3
  // construction du tableau   locmapShLSh  : shap1 - shap1 shap2 shap3
  for (itt1.Initialize(mymapEdgLEdg); itt1.More(); itt1.Next())
  {
    const TopoDS_Shape&            edg1 = itt1.Key();
    NCollection_List<TopoDS_Shape> LmapEdg;
    LmapEdg = mymapEdgLEdg.Find(edg1);
    NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher> mapVerInt;

    int           VertexExtrem;
    TopoDS_Vertex V1, V2;
    for (it.Initialize(LmapEdg); it.More(); it.Next())
    {
      const TopoDS_Edge& edg2 = TopoDS::Edge(it.Value());

      TopExp_Explorer expv;
      for (expv.Init(edg2, TopAbs_VERTEX); expv.More(); expv.Next())
      {
        const TopoDS_Shape& ver = expv.Current();

        VertexExtrem = 1;
        if (mapVerInt.IsBound(ver))
        {
          VertexExtrem = 0;
        }
        mapVerInt.Bind(ver, VertexExtrem);
      }
    }

    NCollection_List<TopoDS_Shape> myEdgeLV, myEdgeLMV;
    for (itt2.Initialize(mapVerInt); itt2.More(); itt2.Next())
    {
      const TopoDS_Shape& ver = itt2.Key();
      VertexExtrem            = mapVerInt.Find(ver);
      if (VertexExtrem == 1)
      {
        myEdgeLV.Append(ver);
      }
      else
      {
        //	TopoDS_Shape& ver1 = ver.Oriented(TopAbs_INTERNAL);
        const TopoDS_Shape& ver1 = ver.Oriented(TopAbs_INTERNAL);
        myEdgeLMV.Append(ver1);
      }
    }
    int number = myEdgeLV.Extent();
    if (!(number == 2))
    {
#ifdef OCCT_DEBUG
      if (trc)
        std::cout << " TopOpeBRepBuild_FuseFace::MakeEdge : Failure in reconstructing new edge"
                  << std::endl;
#endif
      return;
    }
    it.Initialize(myEdgeLV);
    const TopoDS_Vertex& ver1 = TopoDS::Vertex(it.Value());
    //    TopoDS_Shape& verf = ver1.Oriented(TopAbs_FORWARD);
    const TopoDS_Shape& verf = ver1.Oriented(TopAbs_FORWARD);
    it.Next();
    const TopoDS_Vertex& ver2 = TopoDS::Vertex(it.Value());
    //    TopoDS_Shape& verl = ver2.Oriented(TopAbs_FORWARD);
    const TopoDS_Shape& verl = ver2.Oriented(TopAbs_FORWARD);

    occ::handle<Geom_Curve> curv;
    const TopoDS_Edge&      edg = TopoDS::Edge(edg1);
    TopLoc_Location         loc;
    double                  first, last;
    curv = BRep_Tool::Curve(edg, loc, first, last);

    BRepLib_MakeEdge   ME(curv, TopoDS::Vertex(verf), TopoDS::Vertex(verl));
    const TopoDS_Edge& edgnew = ME.Edge();

    //    if (myInternal) {
    //      for (it.Initialize(myEdgeLMV); it.More(); it.Next()) {
    //	const TopoDS_Vertex& ver1 = TopoDS::Vertex(it.Value());
    //	BRep_Builder B;
    //	B.MakeEdge(edgnew);
    //	B.Add(edgnew,ver1);
    //      }
    //    }
    mapEdgLEdg.Bind(edgnew, LmapEdg);
  }
  mymapEdgLEdg = mapEdgLEdg;
}

//=======================================================================
// function : SameSupport
// purpose  : Edges SameSupport ou pas
//=======================================================================

bool SameSupport(const TopoDS_Edge& E1, const TopoDS_Edge& E2)
{
#ifdef OCCT_DEBUG
  bool trc = TopOpeBRepBuild_GettraceFUFA();
#endif

  if (E1.IsNull() || E2.IsNull())
  {
    return false;
  }

  occ::handle<Geom_Curve>    C1, C2;
  TopLoc_Location            loc;
  double                     f1, l1, f2, l2;
  occ::handle<Standard_Type> typC1, typC2;

  C1 = BRep_Tool::Curve(E1, loc, f1, l1);
  if (!loc.IsIdentity())
  {
    occ::handle<Geom_Geometry> GG1 = C1->Transformed(loc.Transformation());
    C1                             = occ::down_cast<Geom_Curve>(GG1);
  }
  C2 = BRep_Tool::Curve(E2, loc, f2, l2);
  if (!loc.IsIdentity())
  {
    occ::handle<Geom_Geometry> GG2 = C2->Transformed(loc.Transformation());
    C2                             = occ::down_cast<Geom_Curve>(GG2);
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

  if (typC1 != STANDARD_TYPE(Geom_Line) && typC1 != STANDARD_TYPE(Geom_Circle)
      && typC1 != STANDARD_TYPE(Geom_Ellipse) && typC1 != STANDARD_TYPE(Geom_BSplineCurve)
      && typC1 != STANDARD_TYPE(Geom_BezierCurve))
  {
#ifdef OCCT_DEBUG
    if (trc)
      std::cout << " TopOpeBRepBuild_FuseFace : Type de Support non traite" << std::endl;
#endif
    return false;
  }

  // On a presomption de confusion
  const double tollin = Precision::Confusion();
  const double tolang = Precision::Angular();
  if (typC1 == STANDARD_TYPE(Geom_Line))
  {
    gp_Lin li1(occ::down_cast<Geom_Line>(C1)->Lin());
    gp_Lin li2(occ::down_cast<Geom_Line>(C2)->Lin());

    return std::abs(li1.Angle(li2)) <= tolang
           && li1.Location().SquareDistance(li2.Location()) <= tollin * tollin;
  }
  else if (typC1 == STANDARD_TYPE(Geom_Circle))
  {
    gp_Circ ci1 = occ::down_cast<Geom_Circle>(C1)->Circ();
    gp_Circ ci2 = occ::down_cast<Geom_Circle>(C2)->Circ();
    return std::abs(ci1.Radius() - ci2.Radius()) <= tollin
           && ci1.Location().SquareDistance(ci2.Location()) <= tollin * tollin;
  }
  else if (typC1 == STANDARD_TYPE(Geom_Ellipse))
  {
    gp_Elips ci1 = occ::down_cast<Geom_Ellipse>(C1)->Elips();
    gp_Elips ci2 = occ::down_cast<Geom_Ellipse>(C2)->Elips();

    return std::abs(ci1.MajorRadius() - ci2.MajorRadius()) <= tollin
           && std::abs(ci1.MinorRadius() - ci2.MinorRadius()) <= tollin
           && ci1.Location().SquareDistance(ci2.Location()) <= tollin * tollin;
  }
  else if (typC1 == STANDARD_TYPE(Geom_BSplineCurve))
  {
    occ::handle<Geom_BSplineCurve> B1 = occ::down_cast<Geom_BSplineCurve>(C1);
    occ::handle<Geom_BSplineCurve> B2 = occ::down_cast<Geom_BSplineCurve>(C2);

    int nbpoles = B1->NbPoles();
    if (nbpoles != B2->NbPoles())
    {
      return false;
    }

    int nbknots = B1->NbKnots();
    if (nbknots != B2->NbKnots())
    {
      return false;
    }

    const NCollection_Array1<gp_Pnt>& P1 = B1->Poles();
    const NCollection_Array1<gp_Pnt>& P2 = B2->Poles();

    double tol3d = BRep_Tool::Tolerance(E1);
    for (int p = 1; p <= nbpoles; p++)
    {
      if ((P1(p)).Distance(P2(p)) > tol3d)
      {
        return false;
      }
    }

    const NCollection_Array1<double>& K1 = B1->Knots();
    const NCollection_Array1<double>& K2 = B2->Knots();

    const NCollection_Array1<int>& M1 = B1->Multiplicities();
    const NCollection_Array1<int>& M2 = B2->Multiplicities();

    for (int k = 1; k <= nbknots; k++)
    {
      if ((K1(k) - K2(k)) > tollin)
      {
        return false;
      }
      if (std::abs(M1(k) - M2(k)) > tollin)
      {
        return false;
      }
    }

    if (!B1->IsRational())
    {
      if (B2->IsRational())
      {
        return false;
      }
    }
    else
    {
      if (!B2->IsRational())
      {
        return false;
      }
    }

    if (B1->IsRational())
    {
      const NCollection_Array1<double>& W1 = B1->WeightsArray();
      const NCollection_Array1<double>& W2 = B2->WeightsArray();

      for (int w = 1; w <= nbpoles; w++)
      {
        if (std::abs(W1(w) - W2(w)) > tollin)
        {
          return false;
        }
      }
    }
    return true;
  }
  else if (typC1 == STANDARD_TYPE(Geom_BezierCurve))
  {
    occ::handle<Geom_BezierCurve> B1 = occ::down_cast<Geom_BezierCurve>(C1);
    occ::handle<Geom_BezierCurve> B2 = occ::down_cast<Geom_BezierCurve>(C2);

    int nbpoles = B1->NbPoles();
    if (nbpoles != B2->NbPoles())
    {
      return false;
    }

    const NCollection_Array1<gp_Pnt>& P1 = B1->Poles();
    const NCollection_Array1<gp_Pnt>& P2 = B2->Poles();

    for (int p = 1; p <= nbpoles; p++)
    {
      if ((P1(p)).Distance(P2(p)) > tollin)
      {
        return false;
      }
    }

    if (!B1->IsRational())
    {
      if (B2->IsRational())
      {
        return false;
      }
    }
    else
    {
      if (!B2->IsRational())
      {
        return false;
      }
    }

    if (B1->IsRational())
    {
      const NCollection_Array1<double>& W1 = B1->WeightsArray();
      const NCollection_Array1<double>& W2 = B2->WeightsArray();

      for (int w = 1; w <= nbpoles; w++)
      {
        if (std::abs(W1(w) - W2(w)) > tollin)
        {
          return false;
        }
      }
    }
    return true;
  }
  return false;
}
