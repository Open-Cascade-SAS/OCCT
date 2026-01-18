// Created on: 1995-12-12
// Created by: Jacques GOUSSARD
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

#include <BRepCheck_Shell.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_Status.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Shared.hxx>
#include <Standard_Type.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_Map.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepCheck_Shell, BRepCheck_Result)

//=================================================================================================

static void Propagate(const NCollection_IndexedDataMap<TopoDS_Shape,
                                                       NCollection_List<TopoDS_Shape>,
                                                       TopTools_ShapeMapHasher>&     mapEF,
                      const TopoDS_Shape&                                            theFace,
                      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMapF)
{
  // Base for the traverse procedure.
  theMapF.Add(theFace);

  // Perform well-known width-first traverse.
  for (int anIdx = 1; anIdx <= theMapF.Extent(); ++anIdx)
  {
    const TopoDS_Shape& aFace = theMapF(anIdx);
    for (TopExp_Explorer ex(aFace, TopAbs_EDGE); ex.More(); ex.Next())
    {
      const TopoDS_Edge& edg = TopoDS::Edge(ex.Current());

      // Test if the edge is in the map (only oriented edges are present).
      const NCollection_List<TopoDS_Shape>* aList = mapEF.Seek(edg);

      if (aList == nullptr)
        continue;

      for (NCollection_List<TopoDS_Shape>::Iterator itl(*aList); itl.More(); itl.Next())
      {
        // This code assumes that shape is added to an end of the map.
        // The idea is simple: existing objects will not be added, new objects
        // will be added to an end.
        theMapF.Add(itl.Value());
      }
    }
  }
}

//=================================================================================================

Standard_EXPORT int BRepCheck_Trace(const int phase)
{
  static int BRC_Trace = 0;
  if (phase < 0)
    BRC_Trace = 0;
  else if (phase > 0)
    BRC_Trace = phase;
  return BRC_Trace;
}

void PrintShape(const TopoDS_Shape& theShape)
{
  if (!theShape.IsNull())
  {
    size_t code = TopTools_ShapeMapHasher{}(theShape);
    std::cout << TopAbs::ShapeTypeToString(theShape.ShapeType()) << " : " << code << " "
              << TopAbs::ShapeOrientationToString(theShape.Orientation()) << std::endl;
  }
}

//=================================================================================================

inline bool IsOriented(const TopoDS_Shape& S)
{
  return (S.Orientation() == TopAbs_FORWARD || S.Orientation() == TopAbs_REVERSED);
}

//=================================================================================================

BRepCheck_Shell::BRepCheck_Shell(const TopoDS_Shell& S)
    : myNbori(0),
      myCdone(false),
      myCstat(BRepCheck_NoError),
      myOdone(false),
      myOstat(BRepCheck_NoError)
{
  Init(S);
}

//=================================================================================================

void BRepCheck_Shell::Minimum()
{
  myCdone = false;
  myOdone = false;

  if (!myMin)
  {
    occ::handle<NCollection_Shared<NCollection_List<BRepCheck_Status>>> aNewList =
      new NCollection_Shared<NCollection_List<BRepCheck_Status>>();
    NCollection_List<BRepCheck_Status>& lst = **myMap.Bound(myShape, aNewList);

    // it is checked if the shell is "connected"
    TopExp_Explorer exp(myShape, TopAbs_FACE);
    int             nbface = 0;
    myMapEF.Clear();
    for (; exp.More(); exp.Next())
    {
      nbface++;
      TopExp_Explorer expe;
      for (expe.Init(exp.Current(), TopAbs_EDGE); expe.More(); expe.Next())
      {
        const TopoDS_Shape& edg = expe.Current();
        myMapEF.Bound(edg, NCollection_List<TopoDS_Shape>())->Append(exp.Current());
      }
    } // for (; exp.More(); exp.Next())

    if (nbface == 0)
    {
      BRepCheck::Add(lst, BRepCheck_EmptyShell);
    }
    else if (nbface >= 2)
    {
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> mapF;
      exp.ReInit();

      Propagate(myMapEF, exp.Current(), mapF);

      if (mapF.Extent() != nbface)
      {
        BRepCheck::Add(lst, BRepCheck_NotConnected);
      }
    } // else if (nbface >= 2)

    if (lst.IsEmpty())
    {
      lst.Append(BRepCheck_NoError);
    }

    myMapEF.Clear();
    myMin = true;
  }
}

//=================================================================================================

void BRepCheck_Shell::InContext(const TopoDS_Shape& S)
{
  occ::handle<NCollection_Shared<NCollection_List<BRepCheck_Status>>> aHList;
  {
    std::unique_lock<std::mutex> aLock =
      myMutex ? std::unique_lock<std::mutex>(*myMutex) : std::unique_lock<std::mutex>();
    if (myMap.IsBound(S))
    {
      return;
    }
    occ::handle<NCollection_Shared<NCollection_List<BRepCheck_Status>>> aNewList =
      new NCollection_Shared<NCollection_List<BRepCheck_Status>>();
    aHList = *myMap.Bound(S, aNewList);
  }
  NCollection_List<BRepCheck_Status>& lst = *aHList;

  //  for (TopExp_Explorer exp(S,TopAbs_SHELL); exp.More(); exp.Next()) {
  TopExp_Explorer exp(S, TopAbs_SHELL);
  for (; exp.More(); exp.Next())
  {
    if (exp.Current().IsSame(myShape))
    {
      break;
    }
  }
  if (!exp.More())
  {
    BRepCheck::Add(lst, BRepCheck_SubshapeNotInShape);
    return;
  }

  TopAbs_ShapeEnum styp = S.ShapeType();
  switch (styp)
  {

    case TopAbs_SOLID: {
      BRepCheck_Status fst = Closed();
      if ((fst == BRepCheck_NotClosed && S.Closed()) || (fst != BRepCheck_NoError))
      {
        BRepCheck::Add(lst, fst);
      }
      else if (!IsUnorientable())
      {
        fst = Orientation();
        BRepCheck::Add(lst, fst);
      }
    }
    break;

    default:
      break;
  }

  if (lst.IsEmpty())
  {
    lst.Append(BRepCheck_NoError);
  }
}

//=================================================================================================

void BRepCheck_Shell::Blind()
{
  if (!myBlind)
  {
    // nothing more than in the minimum
    myBlind = true;
  }
}

//=================================================================================================

BRepCheck_Status BRepCheck_Shell::Closed(const bool Update)
{
  occ::handle<NCollection_Shared<NCollection_List<BRepCheck_Status>>> aHList;
  {
    std::unique_lock<std::mutex> aLock =
      myMutex ? std::unique_lock<std::mutex>(*myMutex) : std::unique_lock<std::mutex>();
    aHList = myMap(myShape);
  }

  NCollection_List<BRepCheck_Status>& aStatusList = *aHList;
  if (myCdone)
  {
    if (Update)
    {
      BRepCheck::Add(aStatusList, myCstat);
    }

    return myCstat;
  }

  myCdone = true; // it will be done...

  NCollection_List<BRepCheck_Status>::Iterator itl(aStatusList);
  if (itl.Value() != BRepCheck_NoError)
  {
    myCstat = itl.Value();
    return myCstat; // already saved
  }

  myCstat = BRepCheck_NoError;
  //
  int                                                           aNbF;
  TopExp_Explorer                                               exp, ede;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> mapS;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>        aMEToAvoid;
  myMapEF.Clear();

  // Checks if the oriented faces of the shell give a "closed" shell,
  // i-e if each oriented edge on oriented faces is found 2 times.
  //
  // modified by NIZNHY-PKV Mon Jun  4 13:59:21 2007f
  exp.Init(myShape, TopAbs_FACE);
  for (; exp.More(); exp.Next())
  {
    const TopoDS_Shape& aF = exp.Current();
    if (IsOriented(aF))
    {
      ede.Init(exp.Current(), TopAbs_EDGE);
      for (; ede.More(); ede.Next())
      {
        const TopoDS_Shape& aE = ede.Current();
        if (!IsOriented(aE))
        {
          aMEToAvoid.Add(aE);
        }
      }
    }
  }
  // modified by NIZNHY-PKV Mon Jun  4 13:59:23 2007t
  //
  exp.Init(myShape, TopAbs_FACE);
  for (; exp.More(); exp.Next())
  {
    const TopoDS_Shape& aF = exp.Current();
    if (IsOriented(aF))
    {
      if (!mapS.Add(aF))
      {
        myCstat = BRepCheck_RedundantFace;

        if (Update)
        {
          BRepCheck::Add(aStatusList, myCstat);
        }

        return myCstat;
      }

      //
      ede.Init(exp.Current(), TopAbs_EDGE);
      for (; ede.More(); ede.Next())
      {
        const TopoDS_Shape& aE = ede.Current();
        // modified by NIZNHY-PKV Mon Jun  4 14:07:57 2007f
        // if (IsOriented(aE)) {
        if (!aMEToAvoid.Contains(aE))
        {
          myMapEF.Bound(aE, NCollection_List<TopoDS_Shape>())->Append(aF);
        }
      }
    }
  }

  //
  myNbori = mapS.Extent();
  if (myNbori >= 2)
  {
    mapS.Clear();
    // Search for the first oriented face
    TopoDS_Shape aF;
    exp.Init(myShape, TopAbs_FACE);
    for (; exp.More(); exp.Next())
    {
      aF = exp.Current();
      if (IsOriented(aF))
      {
        break;
      }
    }

    Propagate(myMapEF, aF, mapS);
  }
  //

  //
  aNbF = mapS.Extent();
  if (myNbori != aNbF)
  {
    myCstat = BRepCheck_NotConnected;
    if (Update)
    {
      BRepCheck::Add(aStatusList, myCstat);
    }
    return myCstat;
  }
  //
  //
  int i, Nbedges, nboc, nbSet;
  //
  Nbedges = myMapEF.Extent();
  for (i = 1; i <= Nbedges; ++i)
  {
    nboc = myMapEF(i).Extent();
    if (nboc == 0 || nboc >= 3)
    {
      NCollection_List<TopoDS_Shape> theSet;
      nbSet = NbConnectedSet(theSet);
      // If there is more than one closed cavity the shell is considered invalid
      // this corresponds to the criteria of a solid (not those of a shell)
      if (nbSet > 1)
      {
        myCstat = BRepCheck_InvalidMultiConnexity;
        if (Update)
        {
          BRepCheck::Add(aStatusList, myCstat);
        }

        return myCstat;
      }
    }
    else if (nboc == 1)
    {
      if (!BRep_Tool::Degenerated(TopoDS::Edge(myMapEF.FindKey(i))))
      {
        myCstat = BRepCheck_NotClosed;
        if (Update)
        {
          BRepCheck::Add(aStatusList, myCstat);
        }

        return myCstat;
      }
    }
  }

  if (Update)
  {
    BRepCheck::Add(aStatusList, myCstat);
  }
  return myCstat;
}

//=================================================================================================

BRepCheck_Status BRepCheck_Shell::Orientation(const bool Update)
{
  occ::handle<NCollection_Shared<NCollection_List<BRepCheck_Status>>> aHList;
  {
    std::unique_lock<std::mutex> aLock =
      myMutex ? std::unique_lock<std::mutex>(*myMutex) : std::unique_lock<std::mutex>();
    aHList = myMap(myShape);
  }
  NCollection_List<BRepCheck_Status>& aStatusList = *aHList;

  if (myOdone)
  {
    if (Update)
    {
      BRepCheck::Add(aStatusList, myOstat);
    }
    return myOstat;
  }
  myOdone = true;

  myOstat = Closed();
  if (myOstat != BRepCheck_NotClosed && myOstat != BRepCheck_NoError)
  {
    if (Update)
    {
      BRepCheck::Add(aStatusList, myOstat);
    }
    return myOstat;
  }

  myOstat = BRepCheck_NoError;

  // First the orientation of each face in relation to the shell is found.
  // It is used to check BRepCheck_RedundantFace

  NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher> MapOfShapeOrientation;
  TopExp_Explorer                                                 exp, ede;

  for (exp.Init(myShape, TopAbs_FACE); exp.More(); exp.Next())
  {
    if (!MapOfShapeOrientation.Bind(exp.Current(), (int)(exp.Current().Orientation())))
    {
      myOstat = BRepCheck_RedundantFace;
      if (Update)
      {
        BRepCheck::Add(aStatusList, myOstat);
      }
      else
      {
        return myOstat;
      }
    }
  }

#ifdef OCCT_DEBUG
  if (BRepCheck_Trace(0) > 1)
  {
    NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>::Iterator itt(
      MapOfShapeOrientation);
    std::cout << "La map shape Orientation :" << std::endl;
    for (; itt.More(); itt.Next())
    {
      PrintShape(itt.Key());
    }
    std::cout << std::endl;
  }
#endif

  // Then the orientation of faces by their connectivity is checked
  // BRepCheck_BadOrientationOfSubshape and
  //         BRepCheck_SubshapeNotInShape are checked;

  int                Nbedges = myMapEF.Extent();
  TopoDS_Face        Fref;
  TopAbs_Orientation orf;

  for (int i = 1; i <= Nbedges; i++)
  {

    const TopoDS_Edge& edg = TopoDS::Edge(myMapEF.FindKey(i));
    if (BRep_Tool::Degenerated(edg))
      continue;
    NCollection_List<TopoDS_Shape>&          lface = myMapEF(i);
    NCollection_List<TopoDS_Shape>::Iterator lite(lface);

    if (lface.Extent() <= 2)
    {
      lite.Initialize(lface);
      Fref = TopoDS::Face(lite.Value());

      if (!MapOfShapeOrientation.IsBound(Fref))
      {
        myOstat = BRepCheck_SubshapeNotInShape;
        if (Update)
        {
          BRepCheck::Add(aStatusList, myOstat);
        }
        // quit because no workaround for the incoherence is possible
        return myOstat;
      }
      lite.Next();

      if (lite.More()) // Edge of connectivity
      {
        // JR/Hp :
        int iorf = MapOfShapeOrientation.Find(Fref);
        orf      = (TopAbs_Orientation)iorf;
        // orf = (TopAbs_Orientation)MapOfShapeOrientation.Find(Fref);
        Fref.Orientation(orf);

        // edge is examined
        if (!lite.Value().IsSame(Fref)) // edge non "closed"
        {
          for (ede.Init(Fref, TopAbs_EDGE); ede.More(); ede.Next())
          {
            if (ede.Current().IsSame(edg))
            {
              break;
            }
          }
          TopAbs_Orientation orient = ede.Current().Orientation();
          TopoDS_Face        Fcur   = TopoDS::Face(lite.Value());
          if (!MapOfShapeOrientation.IsBound(Fcur))
          {
            myOstat = BRepCheck_SubshapeNotInShape;
            if (Update)
            {
              BRepCheck::Add(aStatusList, myOstat);
            }
            // quit because no workaround for the incoherence is possible
            return myOstat;
          }

          // JR/Hp :
          int anOriFCur = MapOfShapeOrientation.Find(Fcur);
          orf           = (TopAbs_Orientation)anOriFCur;
          //	orf = (TopAbs_Orientation)MapOfShapeOrientation.Find(Fcur);
          Fcur.Orientation(orf);

          for (ede.Init(Fcur, TopAbs_EDGE); ede.More(); ede.Next())
          {
            if (ede.Current().IsSame(edg))
            {
              break;
            }
          }
          if (ede.Current().Orientation() == orient)
          {
            // The loop is continued on the edges as many times
            // as the same edge is present in the wire

            // modified by NIZHNY-MKK  Tue Sep 30 11:11:42 2003
            bool bfound = false;
            ede.Next();
            for (; ede.More(); ede.Next())
            {
              if (ede.Current().IsSame(edg))
              {
                // modified by NIZHNY-MKK  Tue Sep 30 11:12:03 2003
                bfound = true;
                break;
              }
            }
            // if (ede.Current().Orientation() == orient) {
            // modified by NIZHNY-MKK  Thu Oct  2 17:56:47 2003
            if (!bfound || (ede.Current().Orientation() == orient))
            {
              myOstat = BRepCheck_BadOrientationOfSubshape;
              if (Update)
              {
                BRepCheck::Add(aStatusList, myOstat);
                break;
              }
              return myOstat;
            }
          }
        }
      }
    }
    else // more than two faces
    {
      int                                                    numF = 0, numR = 0;
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> Fmap;

      for (lite.Initialize(lface); lite.More(); lite.Next())
      {
        TopoDS_Face Fcur = TopoDS::Face(lite.Value());
        if (!MapOfShapeOrientation.IsBound(Fcur))
        {
          myOstat = BRepCheck_SubshapeNotInShape;
          if (Update)
          {
            BRepCheck::Add(aStatusList, myOstat);
          }
          // quit because no workaround for the incoherence is possible
          return myOstat;
        }

        int iorf = MapOfShapeOrientation.Find(Fcur);
        orf      = (TopAbs_Orientation)iorf;
        // orf = (TopAbs_Orientation)MapOfShapeOrientation.Find(Fcur);
        Fcur.Orientation(orf);

        for (ede.Init(Fcur, TopAbs_EDGE); ede.More(); ede.Next())
          if (ede.Current().IsSame(edg))
          {
            break;
          }
        if (Fmap.Contains(Fcur)) // edge is "closed" on Fcur, we meet Fcur twice
        {
          ede.Next();
          for (; ede.More(); ede.Next())
          {
            if (ede.Current().IsSame(edg))
            {
              break;
            }
          }
        }
        TopAbs_Orientation orient = ede.Current().Orientation();
        if (orient == TopAbs_FORWARD)
        {
          numF++;
        }
        else
        {
          numR++;
        }

        Fmap.Add(Fcur);
      }

      if (numF != numR)
      {
        myOstat = BRepCheck_BadOrientationOfSubshape;
        if (Update)
        {
          BRepCheck::Add(aStatusList, myOstat);
          break;
        }
        return myOstat;
      }
    }
  }

  // If at least one incorrectly oriented face has been found, it is checked if the shell can be
  // oriented.
  //          i.e. : if by modification of the orientation of a face it is possible to find
  //          a coherent orientation. (it is not possible on a Moebius band)
  //          BRepCheck_UnorientableShape is checked

  if (myOstat == BRepCheck_BadOrientationOfSubshape)
  {
    if (!Fref.IsNull())
    {
      if (Nbedges > 0)
      {
        NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> alre;
        NCollection_List<TopoDS_Shape>                         voisin;
        voisin.Append(Fref);
        alre.Clear();
        while (!voisin.IsEmpty())
        {
          Fref = TopoDS::Face(voisin.First());
          voisin.RemoveFirst();
          if (!MapOfShapeOrientation.IsBound(Fref))
          {
            myOstat = BRepCheck_SubshapeNotInShape;
            if (Update)
            {
              BRepCheck::Add(aStatusList, myOstat);
            }
            // quit because no workaround for the incoherence is possible
            return myOstat;
          }
          // JR/Hp :
          int iorf = MapOfShapeOrientation.Find(Fref);
          orf      = (TopAbs_Orientation)iorf;
          //        orf = (TopAbs_Orientation)MapOfShapeOrientation.Find(Fref);
          Fref.Orientation(orf);

#ifdef OCCT_DEBUG
          if (BRepCheck_Trace(0) > 3)
          {
            std::cout << "Fref : ";
            PrintShape(Fref);
          }
#endif

          TopExp_Explorer edFcur;
          alre.Add(Fref);

          for (ede.Init(Fref, TopAbs_EDGE); ede.More(); ede.Next())
          {
            const TopoDS_Edge&                       edg    = TopoDS::Edge(ede.Current());
            TopAbs_Orientation                       orient = edg.Orientation();
            NCollection_List<TopoDS_Shape>&          lface  = myMapEF.ChangeFromKey(edg);
            NCollection_List<TopoDS_Shape>::Iterator lite(lface);

            TopoDS_Face Fcur = TopoDS::Face(lite.Value());
            if (Fcur.IsSame(Fref))
            {
              lite.Next();
              if (lite.More())
              {
                Fcur = TopoDS::Face(lite.Value());
              }
              else
              {
                // from the free border one goes to the next edge
                continue;
              }
            }

            if (!MapOfShapeOrientation.IsBound(Fcur))
            {
              myOstat = BRepCheck_SubshapeNotInShape;
              if (Update)
              {
                BRepCheck::Add(aStatusList, myOstat);
              }
              // quit because no workaround for the incoherence is possible
              return myOstat;
            }

            // JR/Hp :
            int anOriFCur = MapOfShapeOrientation.Find(Fcur);
            orf           = (TopAbs_Orientation)anOriFCur;
            //          orf = (TopAbs_Orientation)MapOfShapeOrientation.Find(Fcur);
            Fcur.Orientation(orf);

#ifdef OCCT_DEBUG
            if (BRepCheck_Trace(0) > 3)
            {
              std::cout << "    Fcur : ";
              PrintShape(Fcur);
            }
#endif
            for (edFcur.Init(Fcur, TopAbs_EDGE); edFcur.More(); edFcur.Next())
            {
              if (edFcur.Current().IsSame(edg))
              {
                break;
              }
            }
            if (edFcur.Current().Orientation() == orient)
            {
              if (alre.Contains(Fcur))
              {
                // It is necessary to return a face that has been already examined or returned
                // if one gets nowhere, the shell cannot be oriented.
                myOstat = BRepCheck_UnorientableShape;
                if (Update)
                {
                  BRepCheck::Add(aStatusList, myOstat);
                }
                // quit, otherwise there is a risk of taking too much time.
#ifdef OCCT_DEBUG
                if (BRepCheck_Trace(0) > 3)
                {
                  orf = (TopAbs_Orientation)MapOfShapeOrientation.Find(Fcur);
                  Fcur.Orientation(orf);
                  std::cout << "    Error : this face has been already examined " << std::endl;
                  std::cout << "    Impossible to return it ";
                  PrintShape(Fcur);
                }
#endif
                return myOstat;
              }
              orf                         = TopAbs::Reverse(orf);
              MapOfShapeOrientation(Fcur) = orf;

#ifdef OCCT_DEBUG
              if (BRepCheck_Trace(0) > 3)
              {
                orf = (TopAbs_Orientation)MapOfShapeOrientation.Find(Fcur);
                Fcur.Orientation(orf);
                std::cout << "    Resulting Fcur is returned : ";
                PrintShape(Fcur);
              }
#endif
            }
            if (alre.Add(Fcur))
            {
              voisin.Append(Fcur);
            }
          }
        }
      }
    }
  }

  if (Update)
  {
    BRepCheck::Add(aStatusList, myOstat);
  }
  return myOstat;
}

//=================================================================================================

void BRepCheck_Shell::SetUnorientable()
{
  std::unique_lock<std::mutex> aLock =
    myMutex ? std::unique_lock<std::mutex>(*myMutex) : std::unique_lock<std::mutex>();
  BRepCheck::Add(*myMap(myShape), BRepCheck_UnorientableShape);
}

//=================================================================================================

bool BRepCheck_Shell::IsUnorientable() const
{
  if (myOdone)
  {
    return (myOstat != BRepCheck_NoError);
  }

  occ::handle<NCollection_Shared<NCollection_List<BRepCheck_Status>>> aHList;
  {
    std::unique_lock<std::mutex> aLock =
      myMutex ? std::unique_lock<std::mutex>(*myMutex) : std::unique_lock<std::mutex>();
    aHList = myMap(myShape);
  }
  NCollection_List<BRepCheck_Status>& aStatusList = *aHList;

  for (NCollection_List<BRepCheck_Status>::Iterator itl(aStatusList); itl.More(); itl.Next())
  {
    if (itl.Value() == BRepCheck_UnorientableShape)
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

int BRepCheck_Shell::NbConnectedSet(NCollection_List<TopoDS_Shape>& theSets)
{
  // The connections are found
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    parents;
  TopExp::MapShapesAndAncestors(myShape, TopAbs_EDGE, TopAbs_FACE, parents);
  // All faces are taken
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> theFaces;
  TopExp_Explorer                                        exsh(myShape, TopAbs_FACE);
  for (; exsh.More(); exsh.Next())
    theFaces.Add(exsh.Current());
  // The edges that are not oriented or have more than 2 connections are missing
  int                                                    iCur;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> theMultiEd;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> theUnOriEd;
  for (iCur = 1; iCur <= parents.Extent(); iCur++)
  {
    const TopoDS_Edge& Ed = TopoDS::Edge(parents.FindKey(iCur));
    if (parents(iCur).Extent() > 2)
      theMultiEd.Add(Ed);
    if (Ed.Orientation() != TopAbs_REVERSED && Ed.Orientation() != TopAbs_FORWARD)
      theUnOriEd.Add(Ed);
  }
  // Starting from multiconnected edges propagation by simple connections
  NCollection_List<TopoDS_Shape>::Iterator                         lconx1, lconx2;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itmsh(theMultiEd);
  TopoDS_Shell                                                     CurShell;
  TopoDS_Shape                                                     adFac;
  NCollection_List<TopoDS_Shape>                                   lesCur;
  BRep_Builder                                                     BRB;
  bool                                                             newCur = true;
  BRB.MakeShell(CurShell);
  for (; itmsh.More(); itmsh.Next())
  {
    const TopoDS_Shape& Ed = itmsh.Key();
    if (!theUnOriEd.Contains(Ed))
    {
      for (lconx1.Initialize(parents.FindFromKey(Ed)); lconx1.More(); lconx1.Next())
      {
        if (theFaces.Contains(lconx1.Value()))
        {
          adFac = lconx1.Value();
          BRB.Add(CurShell, adFac);
          theFaces.Remove(adFac);
          newCur = false;
          if (theFaces.IsEmpty())
            break;
          lesCur.Append(adFac);
          while (!lesCur.IsEmpty())
          {
            adFac = lesCur.First();
            lesCur.RemoveFirst();
            for (exsh.Init(adFac, TopAbs_EDGE); exsh.More(); exsh.Next())
            {
              const TopoDS_Shape& ced = exsh.Current();
              if (!theMultiEd.Contains(ced))
              {
                for (lconx2.Initialize(parents.FindFromKey(ced)); lconx2.More(); lconx2.Next())
                {
                  if (theFaces.Contains(lconx2.Value()))
                  {
                    adFac = lconx2.Value();
                    BRB.Add(CurShell, adFac);
                    theFaces.Remove(adFac);
                    newCur = false;
                    if (theFaces.IsEmpty())
                      break;
                    lesCur.Append(adFac);
                  }
                }
              }
              if (theFaces.IsEmpty())
                break;
            }
          }
          if (!newCur)
          {
            CurShell.Closed(BRep_Tool::IsClosed(CurShell));
            theSets.Append(CurShell);
            CurShell.Nullify();
            newCur = true;
            BRB.MakeShell(CurShell);
          }
        }
        if (theFaces.IsEmpty())
          break;
      }
    }
    if (theFaces.IsEmpty())
      break;
  }
  return theSets.Extent();
}
