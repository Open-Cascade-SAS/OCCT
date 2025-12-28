// Created on: 1997-04-14
// Created by: Olga KOULECHOVA
// Copyright (c) 1997-1999 Matra Datavision
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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepBuilderAPI.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepExtrema_ExtCF.hxx>
#include <BRepExtrema_ExtPC.hxx>
#include <BRepExtrema_ExtPF.hxx>
#include <BRepFeat.hxx>
#include <BRepFeat_MakeLinearForm.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepTools.hxx>
#include <BRepTools_Modifier.hxx>
#include <BRepTools_TrsfModification.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <LocOpe_Gluer.hxx>
#include <LocOpe_LinearForm.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>

#ifdef OCCT_DEBUG
extern bool BRepFeat_GettraceFEAT();
extern bool BRepFeat_GettraceFEATRIB();
#endif

static void MajMap(const TopoDS_Shape&, // base
                   const LocOpe_LinearForm&,
                   NCollection_DataMap<TopoDS_Shape,
                                       NCollection_List<TopoDS_Shape>,
                                       TopTools_ShapeMapHasher>&, // myMap
                   TopoDS_Shape&,                                 // myFShape
                   TopoDS_Shape&);                                // myLShape

static void SetGluedFaces(
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
    theSlmap,
  LocOpe_LinearForm&,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>&);

//=================================================================================================

void BRepFeat_MakeLinearForm::Init(const TopoDS_Shape&            Sbase,
                                   const TopoDS_Wire&             W,
                                   const occ::handle<Geom_Plane>& Plane,
                                   const gp_Vec&                  Direc,
                                   const gp_Vec&                  Direc1,
                                   const int                      Mode,
                                   const bool                     Modify)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakeLinearForm::Init" << std::endl;
#endif
  bool RevolRib = false;
  Done();
  myGenerated.Clear();

  // modify = 0 if there is no intention to make sliding
  //        = 1 if one tries to make sliding
  bool Sliding = Modify;
  myLFMap.Clear();

  myShape.Nullify();
  myMap.Clear();
  myFShape.Nullify();
  myLShape.Nullify();
  mySbase = Sbase;
  mySkface.Nullify();
  myPbase.Nullify();

  myGShape.Nullify();
  mySUntil.Nullify();
  myListOfEdges.Clear();
  mySlface.Clear();

  TopoDS_Shape aLocalShapeW = W.Oriented(TopAbs_FORWARD);
  myWire                    = TopoDS::Wire(aLocalShapeW);
  //  myWire = TopoDS::Wire(W.Oriented(TopAbs_FORWARD));
  myDir  = Direc;
  myDir1 = Direc1;
  myPln  = Plane;

  if (Mode == 0)
    myFuse = false;
  else // if(Mode == 1)
    myFuse = true;
#ifdef OCCT_DEBUG
  if (trc)
  {
    if (myFuse)
      std::cout << " Fuse" << std::endl;
    if (!myFuse)
      std::cout << " Cut" << std::endl;
  }
#endif

  // ---Determine Tolerance : max tolerance on parameters
  myTol = Precision::Confusion();

  TopExp_Explorer exx;
  exx.Init(myWire, TopAbs_VERTEX);
  for (; exx.More(); exx.Next())
  {
    const double& tol = BRep_Tool::Tolerance(TopoDS::Vertex(exx.Current()));
    if (tol > myTol)
      myTol = tol;
  }

  exx.Init(Sbase, TopAbs_VERTEX);
  for (; exx.More(); exx.Next())
  {
    const double& tol = BRep_Tool::Tolerance(TopoDS::Vertex(exx.Current()));
    if (tol > myTol)
      myTol = tol;
  }

  // ---Control of directions
  //    the wire should be in the rib
  gp_Vec nulldir(0, 0, 0);
  if (!myDir1.IsEqual(nulldir, myTol, myTol))
  {
    double ang = myDir1.Angle(myDir);
    if (ang != M_PI)
    {
#ifdef OCCT_DEBUG
      if (trc)
        std::cout << " Directions must be opposite" << std::endl;
#endif
      myStatusError = BRepFeat_BadDirect;
      NotDone();
      return;
    }
  }
  else
  {

// Rib is centre in the middle of translation
#ifdef OCCT_DEBUG
    if (trc)
      std::cout << " Rib is centre" << std::endl;
#endif
    const gp_Vec& DirTranslation = (Direc + Direc1) * 0.5;
    gp_Trsf       T;
    T.SetTranslation(DirTranslation);
    BRepBuilderAPI_Transform trf(T);
    trf.Perform(myWire);
    myWire = TopoDS::Wire(trf.Shape());
    myDir  = Direc - DirTranslation;
    myDir1 = Direc1 - DirTranslation;
    myPln->Transform(T);
  }

  // ---Calculate bounding box
  BRep_Builder BB;

  NCollection_List<TopoDS_Shape> theList;

  TopoDS_Shape U;
  U.Nullify();
  gp_Pnt FirstCorner, LastCorner;
  double bnd = HeightMax(mySbase, U, FirstCorner, LastCorner);
  myBnd      = bnd;

  BRepPrimAPI_MakeBox Bndbox(FirstCorner, LastCorner);
  TopoDS_Solid        BndBox = Bndbox.Solid();

  // ---Construction of the face workplane (section bounding box)
  BRepLib_MakeFace PlaneF(myPln->Pln(), -6. * myBnd, 6. * myBnd, -6. * myBnd, 6. * myBnd);
  TopoDS_Face      PlaneFace = TopoDS::Face(PlaneF.Shape());

  BRepAlgoAPI_Common PlaneS(BndBox, PlaneFace);
  TopExp_Explorer    EXP;
  TopoDS_Shape       PlaneSect = PlaneS.Shape();
  EXP.Init(PlaneSect, TopAbs_WIRE);
  TopoDS_Wire      www = TopoDS::Wire(EXP.Current());
  BRepLib_MakeFace Bndface(myPln->Pln(), www, true);
  TopoDS_Face      BndFace = TopoDS::Face(Bndface.Shape());

  // ---Find support faces of the rib
  TopoDS_Edge   FirstEdge, LastEdge;
  TopoDS_Face   FirstFace, LastFace;
  TopoDS_Vertex FirstVertex, LastVertex;

  bool        OnFirstFace   = false;
  bool        OnLastFace    = false;
  bool        PtOnFirstEdge = false;
  bool        PtOnLastEdge  = false;
  TopoDS_Edge OnFirstEdge, OnLastEdge;
  OnFirstEdge.Nullify();
  OnLastEdge.Nullify();

  bool Data = ExtremeFaces(RevolRib,
                           myBnd,
                           myPln,
                           FirstEdge,
                           LastEdge,
                           FirstFace,
                           LastFace,
                           FirstVertex,
                           LastVertex,
                           OnFirstFace,
                           OnLastFace,
                           PtOnFirstEdge,
                           PtOnLastEdge,
                           OnFirstEdge,
                           OnLastEdge);

  if (!Data)
  {
#ifdef OCCT_DEBUG
    if (trc)
      std::cout << " No Extreme faces" << std::endl;
#endif
    myStatusError = BRepFeat_NoExtFace;
    NotDone();
    return;
  }

  // ---Proofing Point for the side of the wire to be filled - side material
  gp_Pnt CheckPnt = CheckPoint(FirstEdge, bnd / 10., myPln);

  //  double f, l;

  // ---Control sliding valuable
  // Many cases when the sliding is abandoned
  int Concavite = 3; // a priori the profile is not concave

  myFirstPnt = BRep_Tool::Pnt(FirstVertex);
  myLastPnt  = BRep_Tool::Pnt(LastVertex);

  // SliList : list of faces concerned by the rib
  NCollection_List<TopoDS_Shape> SliList;
  SliList.Append(FirstFace);

  if (Sliding)
  { // sliding
#ifdef OCCT_DEBUG
    if (trc)
      std::cout << " Sliding" << std::endl;
#endif
    Sliding                     = false;
    occ::handle<Geom_Surface> s = BRep_Tool::Surface(FirstFace);
    if (s->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
    {
      s = occ::down_cast<Geom_RectangularTrimmedSurface>(s)->BasisSurface();
    }
    if (s->DynamicType() == STANDARD_TYPE(Geom_Plane)
        || s->DynamicType() == STANDARD_TYPE(Geom_CylindricalSurface))
    {
      // if plane or cylinder : sliding is possible
      Sliding = true;
    }
  }

  // Control only start and end points
  // -> no control at the middle - improve
  // Controle between Surface and segment between 2 limit points
  // is too expensive - improve
  if (Sliding)
  {
    gp_Pnt p1(myFirstPnt.X() + myDir.X(), myFirstPnt.Y() + myDir.Y(), myFirstPnt.Z() + myDir.Z());
    BRepLib_MakeEdge  ee1(myFirstPnt, p1);
    BRepExtrema_ExtCF ext1(ee1, FirstFace);
    if (ext1.NbExt() == 1
        && ext1.SquareDistance(1)
             <= BRep_Tool::Tolerance(FirstFace) * BRep_Tool::Tolerance(FirstFace))
    {
      gp_Pnt p2(myLastPnt.X() + myDir.X(), myLastPnt.Y() + myDir.Y(), myLastPnt.Z() + myDir.Z());
      BRepLib_MakeEdge  ee2(myLastPnt, p2);
      BRepExtrema_ExtCF ext2(ee2, LastFace); // ExtCF : curves and surfaces
      if (ext2.NbExt() == 1
          && ext2.SquareDistance(1)
               <= BRep_Tool::Tolerance(LastFace) * BRep_Tool::Tolerance(LastFace))
      {
        Sliding = true;
      }
      else
      {
        Sliding = false;
      }
    }
    else
    {
      Sliding = false;
    }
  }

  if (!myDir1.IsEqual(nulldir, Precision::Confusion(), Precision::Confusion()))
  {
    if (Sliding)
    {
      gp_Pnt            p1(myFirstPnt.X() + myDir1.X(),
                myFirstPnt.Y() + myDir1.Y(),
                myFirstPnt.Z() + myDir1.Z());
      BRepLib_MakeEdge  ee1(myFirstPnt, p1);
      BRepExtrema_ExtCF ext1(ee1, FirstFace);
      if (ext1.NbExt() == 1
          && ext1.SquareDistance(1)
               <= BRep_Tool::Tolerance(FirstFace) * BRep_Tool::Tolerance(FirstFace))
      {
        gp_Pnt            p2(myLastPnt.X() + myDir1.X(),
                  myLastPnt.Y() + myDir1.Y(),
                  myLastPnt.Z() + myDir1.Z());
        BRepLib_MakeEdge  ee2(myLastPnt, p2);
        BRepExtrema_ExtCF ext2(ee2, LastFace);
        if (ext2.NbExt() == 1
            && ext2.SquareDistance(1)
                 <= BRep_Tool::Tolerance(LastFace) * BRep_Tool::Tolerance(LastFace))
        {
          Sliding = true;
        }
        else
        {
          Sliding = false;
        }
      }
      else
      {
        Sliding = false;
      }
    }
  }

  // Construct a great profile that goes till the bounding box
  // -> by tangency with the first and the last edge of the Wire
  // -> by normals to the support faces : statistically better
  // Intersect everything to find the final profile

  // ---case of sliding : construction of the profile face
  if (Sliding)
  {
#ifdef OCCT_DEBUG
    if (trc)
      std::cout << " still Sliding" << std::endl;
#endif
    TopoDS_Face Prof;
    bool        ProfileOK;
    ProfileOK = SlidingProfile(Prof,
                               RevolRib,
                               myTol,
                               Concavite,
                               myPln,
                               BndFace,
                               CheckPnt,
                               FirstFace,
                               LastFace,
                               FirstVertex,
                               LastVertex,
                               FirstEdge,
                               LastEdge);

    if (!ProfileOK)
    {
#ifdef OCCT_DEBUG
      if (trc)
      {
        std::cout << "Not computable" << std::endl;
        std::cout << "Face profile not computable" << std::endl;
      }
#endif
      myStatusError = BRepFeat_NoFaceProf;
      NotDone();
      return;
    }

    // ---Propagation on faces of the initial shape
    // to find the faces concerned by the rib
    bool falseside = true;
    Sliding        = Propagate(SliList, Prof, myFirstPnt, myLastPnt, falseside);
    // Control if there is everything required to have the material at the proper side
    if (falseside == false)
    {
#ifdef OCCT_DEBUG
      std::cout << "Verify plane and wire orientation" << std::endl;
#endif
      myStatusError = BRepFeat_FalseSide;
      NotDone();
      return;
    }
  }

  // ---Generation of the base of the rib profile

  TopoDS_Wire w;
  BB.MakeWire(w);
  TopoDS_Edge   thePreviousEdge;
  TopoDS_Vertex theFV;
  thePreviousEdge.Nullify();

  // calculate the number of edges to fill the map
  int counter = 1;

  // ---case of sliding
  if (Sliding && !myListOfEdges.IsEmpty())
  {
    BRepTools_WireExplorer EX1(myWire);
    for (; EX1.More(); EX1.Next())
    {
      const TopoDS_Edge& E = EX1.Current();
      if (!myLFMap.IsBound(E))
      {
        NCollection_List<TopoDS_Shape> theTmpList;
        myLFMap.Bind(E, theTmpList);
      }
      if (E.IsSame(FirstEdge))
      {
        double                  f, l;
        occ::handle<Geom_Curve> cc = BRep_Tool::Curve(E, f, l);
        cc                         = new Geom_TrimmedCurve(cc, f, l);
        gp_Pnt pt;
        if (!FirstEdge.IsSame(LastEdge))
        {
          pt = BRep_Tool::Pnt(TopExp::LastVertex(E, true));
        }
        else
        {
          pt          = myLastPnt;
          double fpar = IntPar(cc, myFirstPnt);
          double lpar = IntPar(cc, pt);
          if (fpar > lpar)
          {
            cc = cc->Reversed();
          }
        }
        TopoDS_Edge ee1;
        if (thePreviousEdge.IsNull())
        {
          BRepLib_MakeVertex v1(myFirstPnt);
          BRepLib_MakeVertex v2(pt);
          BRepLib_MakeEdge   e(cc, v1, v2);
          ee1 = TopoDS::Edge(e.Shape());
        }
        else
        {
          const TopoDS_Vertex& v1 = TopExp::LastVertex(thePreviousEdge, true);
          BRepLib_MakeVertex   v2(pt);

          BRepLib_MakeEdge e(cc, v1, v2);
          ee1 = TopoDS::Edge(e.Shape());
        }
        TopoDS_Shape aLocalShape = ee1.Oriented(E.Orientation());
        ee1                      = TopoDS::Edge(aLocalShape);
        //	ee1 = TopoDS::Edge(ee1.Oriented(E.Orientation()));
        if (counter == 1)
          theFV = TopExp::FirstVertex(ee1, true);
        myLFMap(E).Append(ee1);
        BB.Add(w, ee1);
        thePreviousEdge = ee1;
        counter++;
        EX1.Next();
        break;
      }
    }

    // Case of several edges
    if (!FirstEdge.IsSame(LastEdge))
    {
      for (; EX1.More(); EX1.Next())
      {
        const TopoDS_Edge& E = EX1.Current();
        if (!myLFMap.IsBound(E))
        {
          NCollection_List<TopoDS_Shape> thelist1;
          myLFMap.Bind(E, thelist1);
        }
        theList.Append(E);
        double f, l;
        if (!E.IsSame(LastEdge))
        {
          occ::handle<Geom_Curve> ccc = BRep_Tool::Curve(E, f, l);
          TopoDS_Vertex           v1, v2;
          if (!thePreviousEdge.IsNull())
          {
            v1 = TopExp::LastVertex(thePreviousEdge, true);
            v2 = TopExp::LastVertex(E, true);
          }
          else
          {
            //	    v1 = TopExp::LastVertex(E,true);
            v1 = TopExp::FirstVertex(E, true);
            v2 = TopExp::LastVertex(E, true);
          }
          BRepLib_MakeEdge E1(ccc, v1, v2);
          TopoDS_Edge      E11         = TopoDS::Edge(E1.Shape());
          TopoDS_Shape     aLocalShape = E11.Oriented(E.Orientation());
          E11                          = TopoDS::Edge(aLocalShape);
          //	  E11 = TopoDS::Edge(E11.Oriented(E.Orientation()));
          thePreviousEdge = E11;
          myLFMap(E).Append(E11);
          BB.Add(w, E11);
          if (counter == 1)
            theFV = TopExp::FirstVertex(E11, true);
          counter++;
        }
        else
        {
          occ::handle<Geom_Curve> cc = BRep_Tool::Curve(E, f, l);
          gp_Pnt                  pf = BRep_Tool::Pnt(TopExp::FirstVertex(E, true));
          gp_Pnt                  pl = myLastPnt;
          TopoDS_Edge             ee;
          if (thePreviousEdge.IsNull())
          {
            BRepLib_MakeEdge e(cc, pf, pl);
            ee = TopoDS::Edge(e.Shape());
          }
          else
          {
            const TopoDS_Vertex& v1 = TopExp::LastVertex(thePreviousEdge, true);
            BRepLib_MakeVertex   v2(pl);
            BRepLib_MakeEdge     e(cc, v1, v2);
            ee = TopoDS::Edge(e.Shape());
          }
          TopoDS_Shape aLocalShape = ee.Oriented(E.Orientation());
          ee                       = TopoDS::Edge(aLocalShape);
          //	  ee = TopoDS::Edge(ee.Oriented(E.Orientation()));
          BB.Add(w, ee);
          myLFMap(E).Append(ee);
          if (counter == 1)
            theFV = TopExp::FirstVertex(ee, true);
          thePreviousEdge = ee;
          counter++;
          break;
        }
      }
    }

    NCollection_List<TopoDS_Shape>::Iterator it(myListOfEdges);
    bool                                     FirstOK = false;
    bool                                     LastOK  = false;

    gp_Pnt                         theLastPnt = myLastPnt;
    int                            sens       = 0;
    TopoDS_Edge                    theEdge, theLEdge, theFEdge;
    int                            counter1 = counter;
    NCollection_List<TopoDS_Shape> NewListOfEdges;
    NewListOfEdges.Clear();
    while (!FirstOK)
    {
      const TopoDS_Edge&             edg = TopoDS::Edge(it.Value());
      gp_Pnt                         fp, lp;
      double                         f, l;
      occ::handle<Geom_Curve>        ccc = BRep_Tool::Curve(edg, f, l);
      occ::handle<Geom_TrimmedCurve> cc  = new Geom_TrimmedCurve(ccc, f, l);
      if (edg.Orientation() == TopAbs_REVERSED)
        cc->Reverse();

      fp          = cc->Value(cc->FirstParameter());
      lp          = cc->Value(cc->LastParameter());
      double dist = fp.Distance(theLastPnt);
      if (dist <= myTol)
      {
        sens   = 1;
        LastOK = true;
      }
      else
      {
        dist = lp.Distance(theLastPnt);
        if (dist <= myTol)
        {
          sens   = 2;
          LastOK = true;
          cc->Reverse();
        }
      }
      int FirstFlag = 0;
      if (sens == 1 && lp.Distance(myFirstPnt) <= myTol)
      {
        FirstOK   = true;
        FirstFlag = 1;
      }
      else if (sens == 2 && fp.Distance(myFirstPnt) <= myTol)
      {
        FirstOK   = true;
        FirstFlag = 2;
      }

      if (LastOK)
      {
        TopoDS_Edge eeee;
        double      fpar = cc->FirstParameter();
        double      lpar = cc->LastParameter();
        if (!FirstOK)
        {
          if (thePreviousEdge.IsNull())
          {
            BRepLib_MakeEdge e(cc, fpar, lpar);
            eeee = TopoDS::Edge(e.Shape());
          }
          else
          {
            const TopoDS_Vertex& v1 = TopExp::LastVertex(thePreviousEdge, true);
            BB.UpdateVertex(v1, dist);
            BRepLib_MakeVertex v2(cc->Value(lpar));
            TopoDS_Vertex      nv = v2.Vertex();
            BRepLib_MakeEdge   e(cc, v1, nv);
            eeee = TopoDS::Edge(e.Shape());
          }
        }
        else
        {
          if (thePreviousEdge.IsNull())
          {
            BRepLib_MakeVertex v1(cc->Value(fpar));
            BRepLib_MakeEdge   e(cc, v1, theFV);
            eeee = TopoDS::Edge(e.Shape());
          }
          else
          {
            const TopoDS_Vertex& v1 = TopExp::LastVertex(thePreviousEdge, true);
            BRepLib_MakeEdge     e(cc, v1, theFV);
            eeee = TopoDS::Edge(e.Shape());
          }
        }

        thePreviousEdge = eeee;
        BB.Add(w, eeee);
        if (counter == 1)
          theFV = TopExp::FirstVertex(eeee, true);
        counter1++;
        NewListOfEdges.Append(edg);
        theEdge = eeee;

        if (dist <= myTol)
          theFEdge = edg;
        theLastPnt = BRep_Tool::Pnt(TopExp::LastVertex(theEdge, true));
      }

      if (FirstFlag == 1)
      {
        theLEdge = edg;
      }
      else if (FirstFlag == 2)
      {
        theLEdge = theEdge;
      }

      if (LastOK)
      {
        myListOfEdges.Remove(it);
        it.Initialize(myListOfEdges);
        LastOK = false;
      }
      else if (it.More())
        it.Next();
      else
      {
        Sliding = false;
        break;
      }
      sens = 0;
    }

    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
      SlidMap;
    SlidMap.Clear();

    if (Sliding && counter1 > counter)
    {
      NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
        Iterator      itm;
      TopExp_Explorer EX2(w, TopAbs_EDGE);
      int             ii = 0;
      for (; EX2.More(); EX2.Next())
      {
        const TopoDS_Edge& E = TopoDS::Edge(EX2.Current());
        ii++;
        if (ii >= counter && ii <= counter1)
        {
          it.Initialize(NewListOfEdges);
          int jj = 0;
          for (; it.More(); it.Next())
          {
            const TopoDS_Edge& e2 = TopoDS::Edge(it.Value());
            jj++;
            if (jj == (ii - counter + 1))
            {
              itm.Initialize(mySlface);
              for (; itm.More(); itm.Next())
              {
                const TopoDS_Face&                       fac  = TopoDS::Face(itm.Key());
                const NCollection_List<TopoDS_Shape>&    ledg = itm.Value();
                NCollection_List<TopoDS_Shape>::Iterator itedg(ledg);
                // int iiii = 0;
                for (; itedg.More(); itedg.Next())
                {
                  const TopoDS_Edge& e1 = TopoDS::Edge(itedg.Value());
                  if (e1.IsSame(e2))
                  {
                    if (!SlidMap.IsBound(fac))
                    {
                      NCollection_List<TopoDS_Shape> thelist2;
                      SlidMap.Bind(fac, thelist2);
                    }
                    SlidMap(fac).Append(E);
                  }
                }
              }
            }
          }
        }
      }
    }

    mySlface.Clear();
    mySlface = SlidMap;
  }

  // ---Arguments of LocOpe_LinearForm : arguments of the prism sliding
  if (Sliding)
  {
    TopoDS_Face F;
    BB.MakeFace(F, myPln, myTol);
    w.Closed(BRep_Tool::IsClosed(w));
    BB.Add(F, w);
    //    BRepLib_MakeFace F(myPln->Pln(),w, true);
    mySkface = F;
    myPbase  = mySkface;
    mySUntil.Nullify();
  }

  // ---Case without sliding : construction of the profile face
  if (!Sliding)
  {
#ifdef OCCT_DEBUG
    if (trc)
    {
      if (Modify)
        std::cout << " Sliding failure" << std::endl;
      std::cout << " no Sliding" << std::endl;
    }
#endif
    TopoDS_Face Prof;
    bool        ProfileOK;
    ProfileOK = NoSlidingProfile(Prof,
                                 RevolRib,
                                 myTol,
                                 Concavite,
                                 myPln,
                                 bnd,
                                 BndFace,
                                 CheckPnt,
                                 FirstFace,
                                 LastFace,
                                 FirstVertex,
                                 LastVertex,
                                 FirstEdge,
                                 LastEdge,
                                 OnFirstFace,
                                 OnLastFace);

    if (!ProfileOK)
    {
#ifdef OCCT_DEBUG
      if (trc)
      {
        std::cout << "Not computable" << std::endl;
        std::cout << " Face profile not computable" << std::endl;
      }
#endif
      myStatusError = BRepFeat_NoFaceProf;
      NotDone();
      return;
    }

    // ---Propagation on faces of the initial shape
    // to find the faces concerned by the rib
    bool falseside = true;
    Propagate(SliList, Prof, myFirstPnt, myLastPnt, falseside);
    // Control if there is everything required to have the material at the proper side
    if (falseside == false)
    {
#ifdef OCCT_DEBUG
      std::cout << "Verify plane and wire orientation" << std::endl;
#endif
      myStatusError = BRepFeat_FalseSide;
      NotDone();
      return;
    }

    mySlface.Clear();

    NCollection_List<TopoDS_Shape>::Iterator it;
    it.Initialize(SliList);

    TopoDS_Shape comp;

    BB.MakeShell(TopoDS::Shell(comp));

    for (; it.More(); it.Next())
    {
      BB.Add(comp, it.Value());
    }
    comp.Closed(BRep_Tool::IsClosed(comp));

    mySUntil = comp;

    mySkface = Prof;
    myPbase  = Prof;
  }

  mySliding = Sliding;

  TopExp_Explorer exp;
  for (exp.Init(mySbase, TopAbs_FACE); exp.More(); exp.Next())
  {
    NCollection_List<TopoDS_Shape> thelist3;
    myMap.Bind(exp.Current(), thelist3);
    myMap(exp.Current()).Append(exp.Current());
  }
}

//=======================================================================
// function : Add
// purpose  : add des element de collage
//=======================================================================

void BRepFeat_MakeLinearForm::Add(const TopoDS_Edge& E, const TopoDS_Face& F)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakeLinearForm::Add" << std::endl;
#endif
  if (mySlface.IsEmpty())
  {
    TopExp_Explorer exp;
    for (exp.Init(mySbase, TopAbs_FACE); exp.More(); exp.Next())
    {
      if (exp.Current().IsSame(F))
      {
        break;
      }
    }
    if (!exp.More())
    {
      throw Standard_ConstructionError();
    }

    if (!mySlface.IsBound(F))
    {
      NCollection_List<TopoDS_Shape> thelist;
      mySlface.Bind(F, thelist);
    }
    NCollection_List<TopoDS_Shape>::Iterator itl(mySlface(F));
    for (; itl.More(); itl.Next())
    {
      if (itl.Value().IsSame(E))
      {
        break;
      }
    }
    if (!itl.More())
    {
      mySlface(F).Append(E);
    }
  }
}

//=======================================================================
// function : Perform
// purpose  : construction of rib from a profile and the initial shape
//=======================================================================

void BRepFeat_MakeLinearForm::Perform()
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEAT();
  if (trc)
    std::cout << "BRepFeat_MakeLinearForm::Perform()" << std::endl;
#endif
  if (mySbase.IsNull() || mySkface.IsNull() || myPbase.IsNull())
  {
#ifdef OCCT_DEBUG
    if (trc)
      std::cout << " Fields not initialized" << std::endl;
#endif
    myStatusError = BRepFeat_NotInitialized;
    NotDone();
    return;
  }

  gp_Vec nulldir(0, 0, 0);

  double Length = myDir.Magnitude() + myDir1.Magnitude();

  myGluedF.Clear();

  if (!mySUntil.IsNull())
    myPerfSelection = BRepFeat_SelectionU;
  else
    myPerfSelection = BRepFeat_NoSelection;

  gp_Dir dir(myDir);
  gp_Vec V = Length * dir;

  LocOpe_LinearForm theForm;

  if (myDir1.IsEqual(nulldir, Precision::Confusion(), Precision::Confusion()))
    theForm.Perform(myPbase, V, myFirstPnt, myLastPnt);
  else
    theForm.Perform(myPbase, V, myDir1, myFirstPnt, myLastPnt);

  TopoDS_Shape VraiForm = theForm.Shape(); // primitive of the rib

  myFacesForDraft.Append(theForm.FirstShape());
  myFacesForDraft.Append(theForm.LastShape());
  MajMap(myPbase, theForm, myMap, myFShape, myLShape); // management of descendants

  TopExp_Explorer exx(myPbase, TopAbs_EDGE);
  for (; exx.More(); exx.Next())
  {
    const TopoDS_Edge& e = TopoDS::Edge(exx.Current());
    if (!myMap.IsBound(e))
    {
#ifdef OCCT_DEBUG
      if (trc)
        std::cout << " Sliding face not in Base shape" << std::endl;
#endif
      myStatusError = BRepFeat_IncSlidFace;
      NotDone();
      return;
    }
  }

  myGShape = VraiForm;
  SetGluedFaces(mySlface, theForm, myGluedF); // management of sliding faces

  if (!myGluedF.IsEmpty() && !mySUntil.IsNull())
  {
#ifdef OCCT_DEBUG
    if (trc)
    {
      std::cout << "The case is not computable" << std::endl;
      std::cout << " Glued faces not empty and Until shape not null" << std::endl;
    }
#endif
    myStatusError = BRepFeat_InvShape;
    NotDone();
    return;
  }

  LFPerform();

  /*

    TopExp_Explorer expr(mySbase, TopAbs_FACE);
    char nom1[20], nom2[20];
    int ii = 0;
    for(; expr.More(); expr.Next()) {
      ii++;
      Sprintf(nom1, "faceinitial_%d", ii);
      DBRep::Set(nom1, expr.Current());
      int jj = 0;
      const NCollection_List<TopoDS_Shape>& list = Modified(expr.Current());
      NCollection_List<TopoDS_Shape>::Iterator ite(list);
      for(; ite.More(); ite.Next()) {
        jj++;
        Sprintf(nom2, "facemodifie_%d_%d", ii, jj);
        DBRep::Set(nom2, ite.Value());
      }
    }

    expr.Init(myWire, TopAbs_EDGE);
    ii=0;
    for(; expr.More(); expr.Next()) {
      ii++;
      Sprintf(nom1, "edgeinitial_%d", ii);
      DBRep::Set(nom1, expr.Current());
      int jj = 0;
      const NCollection_List<TopoDS_Shape>& genf = Generated(expr.Current());
      NCollection_List<TopoDS_Shape>::Iterator ite(genf);
      for(; ite.More(); ite.Next()) {
        jj++;
        Sprintf(nom2, "egdegeneree_%d_%d", ii, jj);
        DBRep::Set(nom2, ite.Value());
      }
    }
  */
}

//=======================================================================
// function : Propagate
// purpose  : propagation on faces of the initial shape, find
// faces concerned by the rib
//=======================================================================
bool BRepFeat_MakeLinearForm::Propagate(NCollection_List<TopoDS_Shape>& SliList,
                                        const TopoDS_Face&              fac,
                                        const gp_Pnt&                   Firstpnt,
                                        const gp_Pnt&                   Lastpnt,
                                        bool&                           falseside)
{
#ifdef OCCT_DEBUG
  bool trc = BRepFeat_GettraceFEATRIB();
  if (trc)
    std::cout << "BRepFeat_MakeLinearForm::Propagate" << std::endl;
#endif
  gp_Pnt Firstpoint = Firstpnt;
  gp_Pnt Lastpoint  = Lastpnt;

  bool        result = true;
  TopoDS_Face CurrentFace, saveFace;
  CurrentFace = TopoDS::Face(SliList.First());
  saveFace    = CurrentFace;

  bool          LastOK = false, FirstOK = false;
  bool          v1OK = false, v2OK = false;
  TopoDS_Vertex v1, v2, v3, v4, ve1, ve2;

  BRepAlgoAPI_Section sect(fac, CurrentFace, false);

  sect.Approximation(true);
  sect.Build();

  TopExp_Explorer Ex;
  TopoDS_Edge     eb, ec;
  gp_Pnt          p1, p2;
  double          t1 = 0., t2 = 0.;
  bool            c1f, c2f, c1l, c2l;

  for (Ex.Init(sect.Shape(), TopAbs_EDGE); Ex.More(); Ex.Next())
  {
    ec  = TopoDS::Edge(Ex.Current());
    v1  = TopExp::FirstVertex(ec, true);
    v2  = TopExp::LastVertex(ec, true);
    p1  = BRep_Tool::Pnt(v1);
    p2  = BRep_Tool::Pnt(v2);
    t1  = BRep_Tool::Tolerance(v1);
    t2  = BRep_Tool::Tolerance(v2);
    c1f = p1.Distance(Firstpoint) <= t1;
    c2f = p2.Distance(Firstpoint) <= t2;
    c1l = p1.Distance(Lastpoint) <= t1;
    c2l = p2.Distance(Lastpoint) <= t2;
    if (c1f || c2f || c1l || c2l)
    {
      eb = ec;
      if (c1f || c1l)
        v1OK = true;
      if (c2f || c2l)
        v2OK = true;
      if (c1f || c2f)
        FirstOK = true;
      if (c1l || c2l)
        LastOK = true;
      break;
    }
  }

  if (eb.IsNull())
  {
    falseside = false;
    return false;
  }
  NCollection_List<TopoDS_Shape> thelist;
  mySlface.Bind(CurrentFace, thelist);
  mySlface(CurrentFace).Append(eb);

  myListOfEdges.Clear();
  myListOfEdges.Append(eb);

  // two points are on the same face.
  if (LastOK && FirstOK)
  {
    return result;
  }

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    mapedges;
  TopExp::MapShapesAndAncestors(mySbase, TopAbs_EDGE, TopAbs_FACE, mapedges);
  TopExp_Explorer ex;
  TopoDS_Edge     FirstEdge;
  BRep_Builder    BB;

  TopoDS_Vertex Vprevious;
  gp_Pnt        ptprev;
  double        dp;

  while (!(LastOK && FirstOK))
  {
    if (v1OK)
    {
      Vprevious = v2;
      ptprev    = p2;
    }
    else
    {
      Vprevious = v1;
      ptprev    = p1;
    }

    // find edge connected to v1 or v2:
    for (ex.Init(CurrentFace, TopAbs_EDGE); ex.More(); ex.Next())
    {
      const TopoDS_Edge& rfe = TopoDS::Edge(ex.Current());

      BRepExtrema_ExtPC projF(Vprevious, rfe);

      if (projF.IsDone() && projF.NbExt() >= 1)
      {
        double dist2min = RealLast();
        int    index    = 0;
        for (int sol = 1; sol <= projF.NbExt(); sol++)
        {
          if (projF.SquareDistance(sol) <= dist2min)
          {
            index    = sol;
            dist2min = projF.SquareDistance(sol);
          }
        }
        if (index != 0)
        {
          if (dist2min <= BRep_Tool::Tolerance(rfe) * BRep_Tool::Tolerance(rfe))
          {
            FirstEdge = rfe;
            // If the edge is not perpendicular to the plane of the rib
            // it is required to set Sliding(result) to false.
            if (result)
            {
              result = false;
              ve1    = TopExp::FirstVertex(rfe, true);
              ve2    = TopExp::LastVertex(rfe, true);
              BRepExtrema_ExtPF perp(ve1, fac);
              if (perp.IsDone())
              {
                gp_Pnt pe1 = perp.Point(1);
                perp.Perform(ve2, fac);
                if (perp.IsDone())
                {
                  gp_Pnt pe2 = perp.Point(1);
                  if (pe1.Distance(pe2) <= BRep_Tool::Tolerance(rfe))
                    result = true;
                }
              }
            }
            break;
          }
        }
      }
    }

    const NCollection_List<TopoDS_Shape>&    L = mapedges.FindFromKey(FirstEdge);
    NCollection_List<TopoDS_Shape>::Iterator It(L);

    for (; It.More(); It.Next())
    {
      const TopoDS_Face& FF = TopoDS::Face(It.Value());
      if (!FF.IsSame(CurrentFace))
      {
        CurrentFace = FF;
        break;
      }
    }

    BRepAlgoAPI_Section sectf(fac, CurrentFace, false);
    sectf.Approximation(true);
    sectf.Build();

    TopoDS_Edge edg1;
    for (Ex.Init(sectf.Shape(), TopAbs_EDGE); Ex.More(); Ex.Next())
    {
      edg1 = TopoDS::Edge(Ex.Current());
      v1   = TopExp::FirstVertex(edg1, true);
      v2   = TopExp::LastVertex(edg1, true);
      t1   = BRep_Tool::Tolerance(v1);
      t2   = BRep_Tool::Tolerance(v2);
      p1   = BRep_Tool::Pnt(v1);
      p2   = BRep_Tool::Pnt(v2);
      v1OK = p1.Distance(ptprev) <= t1;
      v2OK = p2.Distance(ptprev) <= t2;
      if (v1OK || v2OK)
        break;
    }

    if (v1OK)
    {
      if (!FirstOK)
      {
        dp = p2.Distance(Firstpoint);
        if (dp <= 2 * t2)
        {
          FirstOK = true;
          BB.UpdateVertex(v2, dp);
        }
      }
      if (!LastOK)
      {
        dp = p2.Distance(Lastpoint);
        if (dp <= 2 * t2)
        {
          LastOK = true;
          BB.UpdateVertex(v2, dp);
        }
      }
    }
    else if (v2OK)
    {
      if (!FirstOK)
      {
        dp = p1.Distance(Firstpoint);
        if (dp <= 2 * t1)
        {
          FirstOK = true;
          BB.UpdateVertex(v1, dp);
        }
      }
      if (!LastOK)
      {
        dp = p1.Distance(Lastpoint);
        if (dp <= 2 * t1)
        {
          LastOK = true;
          BB.UpdateVertex(v1, dp);
        }
      }
    }
    else
    {
      // end by chaining the section
      return false;
    }
    NCollection_List<TopoDS_Shape> thelist1;
    mySlface.Bind(CurrentFace, thelist1);
    mySlface(CurrentFace).Append(edg1);
    myListOfEdges.Append(edg1);
  }

  return result;
}

//=======================================================================
// function : MajMap
// purpose  : management of descendants
//=======================================================================

static void MajMap(
  const TopoDS_Shape&      theB,
  const LocOpe_LinearForm& theP,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                theMap,    // myMap
  TopoDS_Shape& theFShape, // myFShape
  TopoDS_Shape& theLShape) // myLShape
{
  TopExp_Explorer exp(theP.FirstShape(), TopAbs_WIRE);
  if (exp.More())
  {
    theFShape = exp.Current();
    NCollection_List<TopoDS_Shape> thelist;
    theMap.Bind(theFShape, thelist);
    for (exp.Init(theP.FirstShape(), TopAbs_FACE); exp.More(); exp.Next())
    {
      theMap(theFShape).Append(exp.Current());
    }
  }

  exp.Init(theP.LastShape(), TopAbs_WIRE);
  if (exp.More())
  {
    theLShape = exp.Current();
    NCollection_List<TopoDS_Shape> thelist1;
    theMap.Bind(theLShape, thelist1);
    for (exp.Init(theP.LastShape(), TopAbs_FACE); exp.More(); exp.Next())
    {
      theMap(theLShape).Append(exp.Current());
    }
  }

  for (exp.Init(theB, TopAbs_EDGE); exp.More(); exp.Next())
  {
    if (!theMap.IsBound(exp.Current()))
    {
      NCollection_List<TopoDS_Shape> thelist2;
      theMap.Bind(exp.Current(), thelist2);
      theMap(exp.Current()) = theP.Shapes(exp.Current());
    }
  }
}

//=======================================================================
// function : SetGluedFaces
// purpose  : management of faces of gluing
//=======================================================================

static void SetGluedFaces(
  const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                                            theSlmap,
  LocOpe_LinearForm&                                                        thePrism,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theMap)
{
  // Slidings
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator itm(theSlmap);
  if (!theSlmap.IsEmpty())
  {
    for (; itm.More(); itm.Next())
    {
      const TopoDS_Face&                       fac  = TopoDS::Face(itm.Key());
      const NCollection_List<TopoDS_Shape>&    ledg = itm.Value();
      NCollection_List<TopoDS_Shape>::Iterator it;
      for (it.Initialize(ledg); it.More(); it.Next())
      {
        const NCollection_List<TopoDS_Shape>& gfac = thePrism.Shapes(it.Value());
        if (gfac.Extent() != 1)
        {
#ifdef OCCT_DEBUG
          std::cout << "Pb SetGluedFace" << std::endl;
#endif
        }
        theMap.Bind(gfac.First(), fac);
      }
    }
  }
}
