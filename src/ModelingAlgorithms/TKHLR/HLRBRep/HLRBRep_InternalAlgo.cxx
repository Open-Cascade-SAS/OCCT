// Created on: 1997-04-17
// Created by: Christophe MARION
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

#include <HLRAlgo.hxx>
#include <HLRBRep_Data.hxx>
#include <HLRBRep_Hider.hxx>
#include <HLRBRep_InternalAlgo.hxx>
#include <HLRBRep_ShapeBounds.hxx>
#include <HLRBRep_ShapeToHLR.hxx>
#include <HLRTopoBRep_OutLiner.hxx>
#include <Standard_Transient.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Array1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HLRBRep_InternalAlgo, Standard_Transient)

//=================================================================================================

HLRBRep_InternalAlgo::HLRBRep_InternalAlgo() = default;

//=================================================================================================

HLRBRep_InternalAlgo::HLRBRep_InternalAlgo(const occ::handle<HLRBRep_InternalAlgo>& A)
{
  myDS     = A->DataStructure();
  myProj   = A->Projector();
  myShapes = A->SeqOfShapeBounds();
}

//=================================================================================================

void HLRBRep_InternalAlgo::Projector(const HLRAlgo_Projector& P)
{
  myProj = P;
}

//=================================================================================================

HLRAlgo_Projector& HLRBRep_InternalAlgo::Projector()
{
  return myProj;
}

//=================================================================================================

void HLRBRep_InternalAlgo::Update()
{
  if (!myShapes.IsEmpty())
  {
    int                        n  = myShapes.Length();
    occ::handle<HLRBRep_Data>* DS = new occ::handle<HLRBRep_Data>[n];

    int i, dv, de, df, nv = 0, ne = 0, nf = 0;

    for (i = 1; i <= n; i++)
    {
      HLRBRep_ShapeBounds& SB = myShapes(i);
      try
      {
        OCC_CATCH_SIGNALS
        DS[i - 1] = HLRBRep_ShapeToHLR::Load(SB.Shape(), myProj, myMapOfShapeTool, SB.NbOfIso());
        dv        = DS[i - 1]->NbVertices();
        de        = DS[i - 1]->NbEdges();
        df        = DS[i - 1]->NbFaces();
      }
      catch (Standard_Failure const&)
      {
        DS[i - 1] = new HLRBRep_Data(0, 0, 0);
        dv        = 0;
        de        = 0;
        df        = 0;
      }

      SB = HLRBRep_ShapeBounds(SB.Shape(), SB.ShapeData(), SB.NbOfIso(), 1, dv, 1, de, 1, df);
      nv += dv;
      ne += de;
      nf += df;
    }

    if (n == 1)
    {
      myDS = DS[0];
    }
    else
    {
      myDS = new HLRBRep_Data(nv, ne, nf);
      nv   = 0;
      ne   = 0;
      nf   = 0;

      for (i = 1; i <= n; i++)
      {
        HLRBRep_ShapeBounds& SB = myShapes(i);
        SB.Sizes(dv, de, df);
        SB.Translate(nv, ne, nf);
        myDS->Write(DS[i - 1], nv, ne, nf);
        nv += dv;
        ne += de;
        nf += df;
      }
    }

    delete[] DS;

    myDS->Update(myProj);

    HLRAlgo_EdgesBlock::MinMaxIndices     ShapMin, ShapMax, MinMaxShap;
    HLRAlgo_EdgesBlock::MinMaxIndices     TheMin, TheMax;
    NCollection_Array1<HLRBRep_EdgeData>& aEDataArray = myDS->EDataArray();
    NCollection_Array1<HLRBRep_FaceData>& aFDataArray = myDS->FDataArray();

    for (i = 1; i <= n; i++)
    {
      bool                 FirstTime = true;
      HLRBRep_ShapeBounds& SB        = myShapes(i);
      int                  v1, v2, e1, e2, f1, f2;
      SB.Bounds(v1, v2, e1, e2, f1, f2);

      for (int e = e1; e <= e2; e++)
      {
        HLRBRep_EdgeData& ed = aEDataArray.ChangeValue(e);
        HLRAlgo::DecodeMinMax(ed.MinMax(), TheMin, TheMax);
        if (FirstTime)
        {
          FirstTime = false;
          HLRAlgo::CopyMinMax(TheMin, TheMax, ShapMin, ShapMax);
        }
        else
        {
          HLRAlgo::AddMinMax(TheMin, TheMax, ShapMin, ShapMax);
        }
      }

      for (int f = f1; f <= f2; f++)
      {
        HLRBRep_FaceData& fd = aFDataArray.ChangeValue(f);
        HLRAlgo::DecodeMinMax(fd.Wires()->MinMax(), TheMin, TheMax);
        HLRAlgo::AddMinMax(TheMin, TheMax, ShapMin, ShapMax);
      }
      HLRAlgo::EncodeMinMax(ShapMin, ShapMax, MinMaxShap);
      SB.UpdateMinMax(MinMaxShap);
    }
  }
}

//=================================================================================================

void HLRBRep_InternalAlgo::Load(const occ::handle<HLRTopoBRep_OutLiner>& S,
                                const occ::handle<Standard_Transient>&   SData,
                                const int                                nbIso)
{
  myShapes.Append(HLRBRep_ShapeBounds(S, SData, nbIso, 0, 0, 0, 0, 0, 0));
  myDS.Nullify();
}

//=================================================================================================

void HLRBRep_InternalAlgo::Load(const occ::handle<HLRTopoBRep_OutLiner>& S, const int nbIso)
{
  myShapes.Append(HLRBRep_ShapeBounds(S, nbIso, 0, 0, 0, 0, 0, 0));
  myDS.Nullify();
}

//=================================================================================================

int HLRBRep_InternalAlgo::Index(const occ::handle<HLRTopoBRep_OutLiner>& S) const
{
  int n = myShapes.Length();

  for (int i = 1; i <= n; i++)
  {
    if (myShapes(i).Shape() == S)
    {
      return i;
    }
  }

  return 0;
}

//=================================================================================================

void HLRBRep_InternalAlgo::Remove(const int I)
{
  Standard_OutOfRange_Raise_if(I == 0 || I > myShapes.Length(),
                               "HLRBRep_InternalAlgo::Remove : unknown Shape");
  myShapes.Remove(I);

  myMapOfShapeTool.Clear();
  myDS.Nullify();
}

//=================================================================================================

void HLRBRep_InternalAlgo::ShapeData(const int I, const occ::handle<Standard_Transient>& SData)
{
  Standard_OutOfRange_Raise_if(I == 0 || I > myShapes.Length(),
                               "HLRBRep_InternalAlgo::ShapeData : unknown Shape");

  myShapes(I).ShapeData(SData);
}

//=================================================================================================

NCollection_Sequence<HLRBRep_ShapeBounds>& HLRBRep_InternalAlgo::SeqOfShapeBounds()
{
  return myShapes;
}

//=================================================================================================

int HLRBRep_InternalAlgo::NbShapes() const
{
  return myShapes.Length();
}

//=================================================================================================

HLRBRep_ShapeBounds& HLRBRep_InternalAlgo::ShapeBounds(const int I)
{
  Standard_OutOfRange_Raise_if(I == 0 || I > myShapes.Length(),
                               "HLRBRep_InternalAlgo::ShapeBounds : unknown Shape");

  return myShapes(I);
}

//=================================================================================================

void HLRBRep_InternalAlgo::InitEdgeStatus()
{
  bool                 visible;
  HLRBRep_FaceIterator faceIt;

  NCollection_Array1<HLRBRep_EdgeData>& aEDataArray = myDS->EDataArray();
  NCollection_Array1<HLRBRep_FaceData>& aFDataArray = myDS->FDataArray();
  int                                   ne          = myDS->NbEdges();
  int                                   nf          = myDS->NbFaces();

  for (int e = 1; e <= ne; e++)
  {
    HLRBRep_EdgeData& ed = aEDataArray.ChangeValue(e);
    if (ed.Selected())
    {
      ed.Status().ShowAll();
    }
  }
  //  for (int f = 1; f <= nf; f++) {
  int f;
  for (f = 1; f <= nf; f++)
  {
    HLRBRep_FaceData& fd = aFDataArray.ChangeValue(f);
    if (fd.Selected())
    {

      for (faceIt.InitEdge(fd); faceIt.MoreEdge(); faceIt.NextEdge())
      {
        HLRBRep_EdgeData* edf = &(myDS->EDataArray().ChangeValue(faceIt.Edge()));
        if (edf->Selected())
        {
          edf->Status().HideAll();
        }
      }
    }
  }

  for (f = 1; f <= nf; f++)
  {
    HLRBRep_FaceData& fd = aFDataArray.ChangeValue(f);
    visible              = true;
    if (fd.Selected() && fd.Closed())
    {
      if (fd.Side())
      {
        visible = false;
      }
      else if (!fd.WithOutL())
      {
        switch (fd.Orientation())
        {
          case TopAbs_REVERSED:
            visible = fd.Back();
            break;
          case TopAbs_FORWARD:
            visible = !fd.Back();
            break;
          case TopAbs_EXTERNAL:
          case TopAbs_INTERNAL:
            visible = true;
            break;
        }
      }
    }
    if (visible)
    {

      for (faceIt.InitEdge(fd); faceIt.MoreEdge(); faceIt.NextEdge())
      {
        int               E   = faceIt.Edge();
        HLRBRep_EdgeData* edf = &(myDS->EDataArray().ChangeValue(E));
        if (edf->Selected() && !edf->Vertical())
        {
          edf->Status().ShowAll();
        }
      }
    }
  }
}

//=================================================================================================

void HLRBRep_InternalAlgo::Select()
{
  if (!myDS.IsNull())
  {
    NCollection_Array1<HLRBRep_EdgeData>& aEDataArray = myDS->EDataArray();
    NCollection_Array1<HLRBRep_FaceData>& aFDataArray = myDS->FDataArray();
    const int                             ne          = myDS->NbEdges();
    const int                             nf          = myDS->NbFaces();

    for (int e = 1; e <= ne; e++)
    {
      HLRBRep_EdgeData& ed = aEDataArray.ChangeValue(e);
      ed.Selected(true);
    }

    for (int f = 1; f <= nf; f++)
    {
      HLRBRep_FaceData& fd = aFDataArray.ChangeValue(f);
      fd.Selected(true);
    }
  }
}

//=================================================================================================

void HLRBRep_InternalAlgo::Select(const int I)
{
  if (!myDS.IsNull())
  {
    Standard_OutOfRange_Raise_if(I == 0 || I > myShapes.Length(),
                                 "HLRBRep_InternalAlgo::Select : unknown Shape");

    int v1, v2, e1, e2, f1, f2;
    myShapes(I).Bounds(v1, v2, e1, e2, f1, f2);

    NCollection_Array1<HLRBRep_EdgeData>& aEDataArray = myDS->EDataArray();
    NCollection_Array1<HLRBRep_FaceData>& aFDataArray = myDS->FDataArray();
    int                                   ne          = myDS->NbEdges();
    const int                             nf          = myDS->NbFaces();

    for (int e = 1; e <= ne; e++)
    {
      HLRBRep_EdgeData& ed = aEDataArray.ChangeValue(e);
      ed.Selected(e >= e1 && e <= e2);
    }

    for (int f = 1; f <= nf; f++)
    {
      HLRBRep_FaceData& fd = aFDataArray.ChangeValue(f);
      fd.Selected(f >= f1 && f <= f2);
    }
  }
}

//=================================================================================================

void HLRBRep_InternalAlgo::SelectEdge(const int I)
{
  if (!myDS.IsNull())
  {
    Standard_OutOfRange_Raise_if(I == 0 || I > myShapes.Length(),
                                 "HLRBRep_InternalAlgo::SelectEdge : unknown Shape");

    int v1, v2, e1, e2, f1, f2;
    myShapes(I).Bounds(v1, v2, e1, e2, f1, f2);

    NCollection_Array1<HLRBRep_EdgeData>& aEDataArray = myDS->EDataArray();
    int                                   ne          = myDS->NbEdges();

    for (int e = 1; e <= ne; e++)
    {
      HLRBRep_EdgeData& ed = aEDataArray.ChangeValue(e);
      ed.Selected(e >= e1 && e <= e2);
    }
  }
}

//=================================================================================================

void HLRBRep_InternalAlgo::SelectFace(const int I)
{
  if (!myDS.IsNull())
  {
    Standard_OutOfRange_Raise_if(I == 0 || I > myShapes.Length(),
                                 "HLRBRep_InternalAlgo::SelectFace : unknown Shape");

    int v1, v2, e1, e2, f1, f2;
    myShapes(I).Bounds(v1, v2, e1, e2, f1, f2);

    NCollection_Array1<HLRBRep_FaceData>& aFDataArray = myDS->FDataArray();
    int                                   nf          = myDS->NbFaces();

    for (int f = 1; f <= nf; f++)
    {
      HLRBRep_FaceData& fd = aFDataArray.ChangeValue(f);
      fd.Selected(f >= f1 && f <= f2);
    }
  }
}

//=================================================================================================

void HLRBRep_InternalAlgo::ShowAll()
{
  if (!myDS.IsNull())
  {
    NCollection_Array1<HLRBRep_EdgeData>& aEDataArray = myDS->EDataArray();
    int                                   ne          = myDS->NbEdges();

    for (int ie = 1; ie <= ne; ie++)
    {
      HLRBRep_EdgeData& ed = aEDataArray.ChangeValue(ie);
      ed.Status().ShowAll();
    }
  }
}

//=================================================================================================

void HLRBRep_InternalAlgo::ShowAll(const int I)
{
  if (!myDS.IsNull())
  {
    Standard_OutOfRange_Raise_if(I == 0 || I > myShapes.Length(),
                                 "HLRBRep_InternalAlgo::ShowAll : unknown Shape");

    Select(I);

    NCollection_Array1<HLRBRep_EdgeData>& aEDataArray = myDS->EDataArray();
    int                                   ne          = myDS->NbEdges();

    for (int e = 1; e <= ne; e++)
    {
      HLRBRep_EdgeData& ed = aEDataArray.ChangeValue(e);
      if (ed.Selected())
      {
        ed.Status().ShowAll();
      }
    }
  }
}

//=================================================================================================

void HLRBRep_InternalAlgo::HideAll()
{
  if (!myDS.IsNull())
  {
    NCollection_Array1<HLRBRep_EdgeData>& aEDataArray = myDS->EDataArray();
    int                                   ne          = myDS->NbEdges();

    for (int ie = 1; ie <= ne; ie++)
    {
      HLRBRep_EdgeData& ed = aEDataArray.ChangeValue(ie);
      ed.Status().HideAll();
    }
  }
}

//=================================================================================================

void HLRBRep_InternalAlgo::HideAll(const int I)
{
  if (!myDS.IsNull())
  {
    Standard_OutOfRange_Raise_if(I == 0 || I > myShapes.Length(),
                                 "HLRBRep_InternalAlgo::HideAll : unknown Shape");

    Select(I);

    NCollection_Array1<HLRBRep_EdgeData>& aEDataArray = myDS->EDataArray();
    int                                   ne          = myDS->NbEdges();

    for (int e = 1; e <= ne; e++)
    {
      HLRBRep_EdgeData& ed = aEDataArray.ChangeValue(e);
      if (ed.Selected())
      {
        ed.Status().HideAll();
      }
    }
  }
}

//=================================================================================================

void HLRBRep_InternalAlgo::PartialHide()
{
  if (!myDS.IsNull())
  {
    int i, n = myShapes.Length();

    for (i = 1; i <= n; i++)
    {
      Hide(i);
    }

    Select();
  }
}

//=================================================================================================

void HLRBRep_InternalAlgo::Hide()
{
  if (!myDS.IsNull())
  {
    int i, j, n = myShapes.Length();

    for (i = 1; i <= n; i++)
    {
      Hide(i);
    }

    for (i = 1; i <= n; i++)
    {
      for (j = 1; j <= n; j++)
      {
        if (i != j)
        {
          Hide(i, j);
        }
      }
    }

    Select();
  }
}

//=================================================================================================

void HLRBRep_InternalAlgo::Hide(const int I)
{
  if (!myDS.IsNull())
  {
    Standard_OutOfRange_Raise_if(I == 0 || I > myShapes.Length(),
                                 "HLRBRep_InternalAlgo::Hide : unknown Shape");

    Select(I);
    InitEdgeStatus();
    HideSelected(I, true);
  }
}

//=================================================================================================

void HLRBRep_InternalAlgo::Hide(const int I, const int J)
{
  if (!myDS.IsNull())
  {
    Standard_OutOfRange_Raise_if(I == 0 || I > myShapes.Length() || J == 0 || J > myShapes.Length(),
                                 "HLRBRep_InternalAlgo::Hide : unknown Shapes");

    if (I == J)
    {
      Hide(I);
    }
    else
    {
      HLRAlgo_EdgesBlock::MinMaxIndices* MinMaxShBI = &myShapes(I).MinMax();
      HLRAlgo_EdgesBlock::MinMaxIndices* MinMaxShBJ = &myShapes(J).MinMax();
      if (((MinMaxShBJ->Max[0] - MinMaxShBI->Min[0]) & 0x80008000) == 0
          && ((MinMaxShBI->Max[0] - MinMaxShBJ->Min[0]) & 0x80008000) == 0
          && ((MinMaxShBJ->Max[1] - MinMaxShBI->Min[1]) & 0x80008000) == 0
          && ((MinMaxShBI->Max[1] - MinMaxShBJ->Min[1]) & 0x80008000) == 0
          && ((MinMaxShBJ->Max[2] - MinMaxShBI->Min[2]) & 0x80008000) == 0
          && ((MinMaxShBI->Max[2] - MinMaxShBJ->Min[2]) & 0x80008000) == 0
          && ((MinMaxShBJ->Max[3] - MinMaxShBI->Min[3]) & 0x80008000) == 0
          && ((MinMaxShBI->Max[3] - MinMaxShBJ->Min[3]) & 0x80008000) == 0
          && ((MinMaxShBJ->Max[4] - MinMaxShBI->Min[4]) & 0x80008000) == 0
          && ((MinMaxShBI->Max[4] - MinMaxShBJ->Min[4]) & 0x80008000) == 0
          && ((MinMaxShBJ->Max[5] - MinMaxShBI->Min[5]) & 0x80008000) == 0
          && ((MinMaxShBI->Max[5] - MinMaxShBJ->Min[5]) & 0x80008000) == 0
          && ((MinMaxShBJ->Max[6] - MinMaxShBI->Min[6]) & 0x80008000) == 0
          && ((MinMaxShBJ->Max[7] - MinMaxShBI->Min[7]) & 0x80008000) == 0)
      {
        SelectEdge(I);
        SelectFace(J);
        HideSelected(I, false);
      }
    }
  }
}

//=================================================================================================

void HLRBRep_InternalAlgo::HideSelected(const int I, const bool SideFace)
{
  HLRBRep_ShapeBounds& SB = myShapes(I);
  int                  v1, v2, e1, e2, f1, f2;
  SB.Bounds(v1, v2, e1, e2, f1, f2);

  if (e2 >= e1)
  {
    myDS->InitBoundSort(SB.MinMax(), e1, e2);
    HLRBRep_Hider                         Cache(myDS);
    NCollection_Array1<HLRBRep_FaceData>& aFDataArray = myDS->FDataArray();
    const int                             nf          = myDS->NbFaces();

    if (nf == 0)
    {
      return;
    }

    if (SideFace)
    {
      for (int f = 1; f <= nf; ++f)
      {
        HLRBRep_FaceData& fd = aFDataArray.ChangeValue(f);
        if (fd.Selected() && fd.Side())
        {
          Cache.OwnHiding(f);
        }
      }
    }

    //--
    NCollection_Array1<int>    Val(1, nf);
    NCollection_Array1<double> Size(1, nf);
    NCollection_Array1<int>    Index(1, nf);

    for (int f = 1; f <= nf; ++f)
    {
      HLRBRep_FaceData& fd = aFDataArray.ChangeValue(f);
      if (fd.Plane())
      {
        Val(f) = 10;
      }
      else if (fd.Cylinder())
      {
        Val(f) = 9;
      }
      else if (fd.Cone())
      {
        Val(f) = 8;
      }
      else if (fd.Sphere())
      {
        Val(f) = 7;
      }
      else if (fd.Torus())
      {
        Val(f) = 6;
      }
      else
      {
        Val(f) = 0;
      }
      if (fd.Cut())
      {
        Val(f) -= 10;
      }
      if (fd.Side())
      {
        Val(f) -= 100;
      }
      if (fd.WithOutL())
      {
        Val(f) -= 20;
      }

      Size(f) = fd.Size();
    }

    for (int tt = 1; tt <= nf; tt++)
    {
      Index(tt) = tt;
    }

    //-- ======================================================================
    /*    bool TriOk; //-- a refaire
        do {
          int t,tp1;
          TriOk=true;
          for(t=1,tp1=2;t<nf;t++,tp1++) {
        if(Val(Index(t))<Val(Index(tp1))) {
          int q=Index(t); Index(t)=Index(tp1); Index(tp1)=q;
          TriOk=false;
        }
        else if(Val(Index(t))==Val(Index(tp1))) {
          if(Size(Index(t))<Size(Index(tp1))) {
            int q=Index(t); Index(t)=Index(tp1); Index(tp1)=q;
            TriOk=false;
          }
        }
          }
        }
        while(TriOk==false);
    */
    //-- ======================================================================
    if (nf > 2)
    {
      int i, ir, k, l;
      int rra;
      l  = (nf >> 1) + 1;
      ir = nf;
      for (;;)
      {
        if (l > 1)
        {
          rra = Index(--l);
        }
        else
        {
          rra       = Index(ir);
          Index(ir) = Index(1);
          if (--ir == 1)
          {
            Index(1) = rra;
            break;
          }
        }
        i = l;
        k = l + l;
        while (k <= ir)
        {
          if (k < ir)
          {
            if (Val(Index(k)) > Val(Index(k + 1)))
            {
              k++;
            }
            else if (Val(Index(k)) == Val(Index(k + 1)))
            {
              if (Size(Index(k)) > Size(Index(k + 1)))
              {
                k++;
              }
            }
          }
          if (Val(rra) > Val(Index(k)))
          {
            Index(i) = Index(k);
            i        = k;
            k <<= 1;
          }
          else if ((Val(rra) == Val(Index(k))) && (Size(rra) > Size(Index(k))))
          {
            Index(i) = Index(k);
            i        = k;
            k <<= 1;
          }
          else
          {
            k = ir + 1;
          }
        }
        Index(i) = rra;
      }
    }

    for (int f = 1; f <= nf; ++f)
    {
      const int         fi = Index(f);
      HLRBRep_FaceData& fd = aFDataArray.ChangeValue(fi);
      if (fd.Selected() && fd.Hiding())
      {
        Cache.Hide(fi, myMapOfShapeTool);
      }
    }
  }
}

//=================================================================================================

occ::handle<HLRBRep_Data> HLRBRep_InternalAlgo::DataStructure() const
{
  return myDS;
}
