// Created on: 1995-05-05
// Created by: Christophe MARION
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

#include <HLRAlgo_PolyAlgo.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <CSLib.hxx>
#include <CSLib_DerivativeStatus.hxx>
#include <CSLib_NormalStatus.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <HLRAlgo_BiPoint.hxx>
#include <HLRAlgo_EdgeStatus.hxx>
#include <HLRAlgo_PolyData.hxx>
#include <HLRAlgo_PolyInternalData.hxx>
#include <HLRAlgo_PolyMask.hxx>
#include <HLRAlgo_PolyShellData.hxx>
#include <HLRBRep_PolyAlgo.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Stream.hxx>
#include <Standard_Type.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HLRBRep_PolyAlgo,Standard_Transient)

enum
{
  NMsk_Vert =  1,
  NMsk_OutL =  2,
  NMsk_Norm =  4,
  NMsk_Fuck =  8,
  NMsk_Edge = 16,
  NMsk_Move = 32
};

#ifdef OCCT_DEBUG
static Standard_Integer DoTrace = Standard_False; 
static Standard_Integer DoError = Standard_False; 
#endif
//=======================================================================
//function : HLRBRep_PolyAlgo
//purpose  :
//=======================================================================
HLRBRep_PolyAlgo::HLRBRep_PolyAlgo()
: myDebug     (Standard_False),
  myTolSta    (0.1),
  myTolEnd    (0.9),
  myTolAngular(0.001)
{
  myAlgo = new HLRAlgo_PolyAlgo();
}

//=======================================================================
//function : HLRBRep_PolyAlgo
//purpose  :
//=======================================================================
HLRBRep_PolyAlgo::HLRBRep_PolyAlgo (const Handle(HLRBRep_PolyAlgo)& theOther)
{
  myDebug      = theOther->Debug();
  myTolAngular = theOther->TolAngular();
  myTolSta     = theOther->TolCoef();
  myTolEnd     = 1.0 - myTolSta;
  myAlgo       = theOther->Algo();
  myProj       = theOther->Projector();

  const Standard_Integer aNbShapes = theOther->NbShapes();
  for (Standard_Integer i = 1; i <= aNbShapes; ++i)
  {
    Load (theOther->Shape (i));
  }
}

//=======================================================================
//function : HLRBRep_PolyAlgo
//purpose  :
//=======================================================================
HLRBRep_PolyAlgo::HLRBRep_PolyAlgo (const TopoDS_Shape& theShape)
: myDebug     (Standard_False),
  myTolSta    (0.1),
  myTolEnd    (0.9),
  myTolAngular(0.001)
{
  myShapes.Append (theShape);
  myAlgo = new HLRAlgo_PolyAlgo();
}

//=======================================================================
//function : Shape
//purpose  :
//=======================================================================
TopoDS_Shape& HLRBRep_PolyAlgo::Shape (const Standard_Integer theIndex)
{
  return myShapes.ChangeValue (theIndex);
}

//=======================================================================
//function : Remove
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::Remove (const Standard_Integer theIndex)
{
  Standard_OutOfRange_Raise_if (theIndex == 0 || theIndex > myShapes.Length(),
				"HLRBRep_PolyAlgo::Remove : unknown Shape");
  myShapes.Remove (theIndex);
  myAlgo->Clear();
  myEMap.Clear();
  myFMap.Clear();
}

//=======================================================================
//function : Index
//purpose  :
//=======================================================================
Standard_Integer HLRBRep_PolyAlgo::Index (const TopoDS_Shape& theShape) const
{
  Standard_Integer i = 1;
  for (TopTools_SequenceOfShape::Iterator aShapeIter (myShapes); aShapeIter.More(); aShapeIter.Next(), ++i)
  {
    if (aShapeIter.Value() == theShape)
    {
      return i;
    }
  }
  return 0;
}

//=======================================================================
//function : Update
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::Update()
{
  myAlgo->Clear();
  myEMap.Clear();
  myFMap.Clear();

  const TopoDS_Shape aShape = MakeShape();
  if (aShape.IsNull())
  {
    return;
  }

  Standard_Boolean isIsoledF = false, isIsoledE = false;
  const Standard_Integer aNbShell = InitShape (aShape, isIsoledF, isIsoledE);
  if (aNbShell <= 0)
  {
    return;
  }

  TopExp::MapShapes (aShape, TopAbs_EDGE, myEMap);
  TopExp::MapShapes (aShape, TopAbs_FACE, myFMap);
  const Standard_Integer aNbEdge = myEMap.Extent();
  const Standard_Integer aNbFace = myFMap.Extent();
  TColStd_Array1OfInteger   anES (0, aNbEdge); // index of the Shell
  NCollection_Array1<Handle(HLRAlgo_PolyData)> aPD (0, aNbFace);
  NCollection_Array1<Handle(HLRAlgo_PolyInternalData)> aPID (0, aNbFace);
  TopTools_MapOfShape aShapeMap1, aShapeMap2;
  NCollection_Array1<Handle(HLRAlgo_PolyShellData)>& aShell = myAlgo->ChangePolyShell();
  Standard_Integer iShell = 0;
  for (TopExp_Explorer aShellIter (aShape, TopAbs_SHELL); aShellIter.More(); aShellIter.Next())
  {
    StoreShell (aShellIter.Current(), iShell, aShell,
                Standard_False, Standard_False,
                anES, aPD, aPID, aShapeMap1, aShapeMap2);
  }
  if (isIsoledF)
  {
    StoreShell (aShape, iShell, aShell, isIsoledF, Standard_False,
                anES, aPD, aPID, aShapeMap1, aShapeMap2);
  }
  if (isIsoledE)
  {
    StoreShell (aShape, iShell, aShell, Standard_False, isIsoledE,
                anES, aPD, aPID, aShapeMap1, aShapeMap2);
  }
  myAlgo->Update();
}

//=======================================================================
//function : MakeShape
//purpose  :
//=======================================================================
TopoDS_Shape HLRBRep_PolyAlgo::MakeShape() const
{
  if (myShapes.IsEmpty())
  {
    return TopoDS_Shape();
  }

  BRep_Builder aBuilder;
  TopoDS_Shape aComp;
  aBuilder.MakeCompound (TopoDS::Compound (aComp));
  for (TopTools_SequenceOfShape::Iterator aShapeIter (myShapes); aShapeIter.More(); aShapeIter.Next())
  {
    aBuilder.Add (aComp, aShapeIter.Value());
  }
  return aComp;
}

//=======================================================================
//function : InitShape
//purpose  :
//=======================================================================
Standard_Integer HLRBRep_PolyAlgo::InitShape (const TopoDS_Shape& theShape,
                                              Standard_Boolean& theIsoledF,
                                              Standard_Boolean& theIsoledE)
{
  TopTools_MapOfShape aShapeMap0;
  Standard_Integer aNbShell = 0;
  theIsoledF = Standard_False;
  theIsoledE = Standard_False;
  TopExp_Explorer aFaceIter;
  TopLoc_Location aLoc;

  for (TopExp_Explorer aShellIter (theShape, TopAbs_SHELL); aShellIter.More(); aShellIter.Next())
  {
    Standard_Boolean hasTrian = Standard_False;
    for (aFaceIter.Init (aShellIter.Current(), TopAbs_FACE); aFaceIter.More(); aFaceIter.Next())
    {
      const TopoDS_Face& aFace = TopoDS::Face (aFaceIter.Current());
      if (!BRep_Tool::Triangulation (aFace, aLoc).IsNull())
      {
        if (aShapeMap0.Add (aFace))
        {
          hasTrian = Standard_True;
        }
      }
    }
    if (hasTrian)
    {
      ++aNbShell;
    }
  }

  for (aFaceIter.Init (theShape, TopAbs_FACE, TopAbs_SHELL); aFaceIter.More() && !theIsoledF; aFaceIter.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face (aFaceIter.Current());
    if (!BRep_Tool::Triangulation (aFace, aLoc).IsNull())
    {
      if (aShapeMap0.Add (aFace))
      {
        theIsoledF = Standard_True;
      }
    }
  }
  if (theIsoledF)
  {
    ++aNbShell;
  }

  for (TopExp_Explorer anEdgeIter (theShape, TopAbs_EDGE, TopAbs_FACE); anEdgeIter.More() && !theIsoledE; anEdgeIter.Next())
  {
    theIsoledE = Standard_True;
  }
  if (theIsoledE)
  {
    ++aNbShell;
  }
  if (aNbShell > 0)
  {
    myAlgo->Init (aNbShell);
  }
  return aNbShell;
}

//=======================================================================
//function : StoreShell
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::StoreShell (const TopoDS_Shape& theShape,
                                   Standard_Integer& theIShell,
                                   NCollection_Array1<Handle(HLRAlgo_PolyShellData)>& theShell,
                                   const Standard_Boolean theIsoledF,
                                   const Standard_Boolean theIsoledE,
                                   TColStd_Array1OfInteger& theES,
                                   NCollection_Array1<Handle(HLRAlgo_PolyData)>& thePD,
                                   NCollection_Array1<Handle(HLRAlgo_PolyInternalData)>& thePID,
                                   TopTools_MapOfShape& theShapeMap1,
                                   TopTools_MapOfShape& theShapeMap2)
{
  TopLoc_Location aLoc;
  TopExp_Explorer aFaceExp, anEdgeExp;
  Standard_Integer aNbFaceShell = 0;
  Standard_Boolean isClosed = Standard_False;
  const gp_Trsf& aProjTrsf = myProj.Transformation();
  {
    const gp_XYZ& aTrsfVec = aProjTrsf.TranslationPart();
    TLoc[0] = aTrsfVec.X();
    TLoc[1] = aTrsfVec.Y();
    TLoc[2] = aTrsfVec.Z();

    const gp_Mat& aTrsfMat = aProjTrsf.VectorialPart();
    TMat[0][0] = aTrsfMat.Value (1, 1);
    TMat[0][1] = aTrsfMat.Value (1, 2);
    TMat[0][2] = aTrsfMat.Value (1, 3);
    TMat[1][0] = aTrsfMat.Value (2, 1);
    TMat[1][1] = aTrsfMat.Value (2, 2);
    TMat[1][2] = aTrsfMat.Value (2, 3);
    TMat[2][0] = aTrsfMat.Value (3, 1);
    TMat[2][1] = aTrsfMat.Value (3, 2);
    TMat[2][2] = aTrsfMat.Value (3, 3);
  }
  {
    const gp_Trsf& aTrsfInv   = myProj.InvertedTransformation();
    const gp_XYZ& aTrsfInvVec = aTrsfInv.TranslationPart();
    TILo[0] = aTrsfInvVec.X();
    TILo[1] = aTrsfInvVec.Y();
    TILo[2] = aTrsfInvVec.Z();

    const gp_Mat& aTrsfInvMat = aTrsfInv.VectorialPart();
    TIMa[0][0] = aTrsfInvMat.Value (1, 1);
    TIMa[0][1] = aTrsfInvMat.Value (1, 2);
    TIMa[0][2] = aTrsfInvMat.Value (1, 3);
    TIMa[1][0] = aTrsfInvMat.Value (2, 1);
    TIMa[1][1] = aTrsfInvMat.Value (2, 2);
    TIMa[1][2] = aTrsfInvMat.Value (2, 3);
    TIMa[2][0] = aTrsfInvMat.Value (3, 1);
    TIMa[2][1] = aTrsfInvMat.Value (3, 2);
    TIMa[2][2] = aTrsfInvMat.Value (3, 3);
  }
  if (!theIsoledE)
  {
    if (!theIsoledF)
    {
      isClosed = theShape.Closed();
      if (!isClosed)
      {
        TopTools_IndexedMapOfShape anEM;
        TopExp::MapShapes (theShape, TopAbs_EDGE, anEM);
        const Standard_Integer aNbEdge = anEM.Extent();
        NCollection_Array1<Standard_Integer> aFlagArray (1, Max(aNbEdge, 1));
        aFlagArray.Init (0);
        for (anEdgeExp.Init (theShape, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
        {
          const TopoDS_Edge& anEdge = TopoDS::Edge (anEdgeExp.Current());
          const Standard_Integer anEdgeIndex = anEM.FindIndex (anEdge);
          TopAbs_Orientation anEdgeOrient = anEdge.Orientation();
          if (!BRep_Tool::Degenerated (anEdge))
          {
            if      (anEdgeOrient == TopAbs_FORWARD ) { aFlagArray[anEdgeIndex] += 1; }
            else if (anEdgeOrient == TopAbs_REVERSED) { aFlagArray[anEdgeIndex] -= 1; }
          }
        }
        isClosed = Standard_True;
	
        for (Standard_Integer anEdgeIter = 1; anEdgeIter <= aNbEdge && isClosed; ++anEdgeIter)
        {
          isClosed = (aFlagArray[anEdgeIter] == 0);
        }
      }
      
      aFaceExp.Init (theShape, TopAbs_FACE);
    }
    else
    {
      aFaceExp.Init (theShape, TopAbs_FACE, TopAbs_SHELL);
    }
    
    for (; aFaceExp.More(); aFaceExp.Next())
    {
      const TopoDS_Face& aFace = TopoDS::Face (aFaceExp.Current());
      if (!BRep_Tool::Triangulation (aFace, aLoc).IsNull())
      {
        if (theShapeMap1.Add (aFace))
        {
          ++aNbFaceShell;
        }
      }
    }
  }
  if (aNbFaceShell > 0 || theIsoledE)
  {
    ++theIShell;
    theShell.SetValue (theIShell, new HLRAlgo_PolyShellData (aNbFaceShell));
  }

  if (aNbFaceShell > 0)
  {
    const Handle(HLRAlgo_PolyShellData)& aPsd = theShell.ChangeValue (theIShell);
    Standard_Integer iFace = 0;
    if (!theIsoledF) { aFaceExp.Init (theShape, TopAbs_FACE); }
    else             { aFaceExp.Init (theShape, TopAbs_FACE, TopAbs_SHELL); }
    
    for (; aFaceExp.More(); aFaceExp.Next())
    {
      const TopoDS_Face& aFace = TopoDS::Face (aFaceExp.Current());
      const Handle(Poly_Triangulation)& aTr = BRep_Tool::Triangulation (aFace, aLoc);
      if (!aTr.IsNull())
      {
        if (theShapeMap2.Add (aFace))
        {
          iFace++;
          const Standard_Integer aFaceIndex = myFMap.FindIndex (aFace);
          const bool isReversed = aFace.Orientation() == TopAbs_REVERSED;
          gp_Trsf aTT = aLoc.Transformation();
          aTT.PreMultiply (aProjTrsf);
          {
            const gp_XYZ& aTTrsfVec = aTT.TranslationPart();
            TTLo[0] = aTTrsfVec.X();
            TTLo[1] = aTTrsfVec.Y();
            TTLo[2] = aTTrsfVec.Z();

            const gp_Mat& aTTrsfMat = aTT.VectorialPart();
            TTMa[0][0] = aTTrsfMat.Value (1, 1);
            TTMa[0][1] = aTTrsfMat.Value (1, 2);
            TTMa[0][2] = aTTrsfMat.Value (1, 3);
            TTMa[1][0] = aTTrsfMat.Value (2, 1);
            TTMa[1][1] = aTTrsfMat.Value (2, 2);
            TTMa[1][2] = aTTrsfMat.Value (2, 3);
            TTMa[2][0] = aTTrsfMat.Value (3, 1);
            TTMa[2][1] = aTTrsfMat.Value (3, 2);
            TTMa[2][2] = aTTrsfMat.Value (3, 3);
          }
          const Standard_Integer aNbNodes = aTr->NbNodes();
          const Standard_Integer aNbTris  = aTr->NbTriangles();
          thePD.SetValue  (aFaceIndex, new HLRAlgo_PolyData());
          aPsd->PolyData().SetValue (iFace, thePD.Value (aFaceIndex));
          thePID.SetValue (aFaceIndex, new HLRAlgo_PolyInternalData (aNbNodes, aNbTris));
          const Handle(HLRAlgo_PolyInternalData)& aPid = thePID.ChangeValue (aFaceIndex);
          if (Handle(Geom_Surface) aSurf = BRep_Tool::Surface (aFace))
          {
            if (Handle(Geom_RectangularTrimmedSurface) aRectTrimSurf = Handle(Geom_RectangularTrimmedSurface)::DownCast (aSurf))
            {
              aSurf = aRectTrimSurf->BasisSurface();
            }
            GeomAdaptor_Surface aSurfAdapt (aSurf);
            aPid->Planar (aSurfAdapt.GetType() == GeomAbs_Plane);
          }
          else
          {
            aPid->Planar (false);
          }

          HLRAlgo_Array1OfTData& aTData = aPid->TData();
          HLRAlgo_Array1OfPISeg& aPISeg = aPid->PISeg();
          HLRAlgo_Array1OfPINod& aPINod = aPid->PINod();
          for (Standard_Integer aTriIter = 1; aTriIter <= aNbTris; ++aTriIter)
          {
            const Poly_Triangle&  aPolyTri = aTr->Triangle (aTriIter);
            HLRAlgo_TriangleData& aTriData = aTData.ChangeValue (aTriIter);
            aTriData.Flags = 0;
            if (isReversed)
            {
              aPolyTri.Get (aTriData.Node3, aTriData.Node2, aTriData.Node1);
            }
            else
            {
              aPolyTri.Get (aTriData.Node1, aTriData.Node2, aTriData.Node3);
            }
          }

          for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
          {
            const gp_Pnt& aPnt = aTr->Node (aNodeIter);
            const Handle(HLRAlgo_PolyInternalNode)& aPolyINode   = aPINod.ChangeValue (aNodeIter);
            HLRAlgo_PolyInternalNode::NodeData&     aNod1RValues = aPolyINode->Data();
            HLRAlgo_PolyInternalNode::NodeIndices&  aNodIndices  = aPolyINode->Indices();
            aNodIndices.NdSg = 0;
            aNodIndices.Flag = 0;
            aNod1RValues.Point = aPnt.XYZ();
            TTMultiply (aNod1RValues.Point);
          }
          aPid->UpdateLinks (aTData, aPISeg, aPINod);
          if (aTr->HasUVNodes())
          {
            const bool hasSurf = BRep_Tool::IsGeometric (aFace);
            myBSurf.Initialize (aFace, Standard_False);
            for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
            {
              const Handle(HLRAlgo_PolyInternalNode)& aPolyINode   = aPINod.ChangeValue (aNodeIter);
              HLRAlgo_PolyInternalNode::NodeIndices&  aNodIndices  = aPolyINode->Indices();
              HLRAlgo_PolyInternalNode::NodeData&     aNod1RValues = aPolyINode->Data();
              if (aTr->HasUVNodes())
              {
                aNod1RValues.UV = aTr->UVNode (aNodeIter).XY();
              }
              if (aTr->HasNormals())
              {
                aNod1RValues.Normal = aTr->Normal (aNodeIter).XYZ();
              }

              if ((aTr->HasNormals()
              || (hasSurf && aTr->HasUVNodes()))
              && Normal (aNodeIter, aNodIndices, aNod1RValues, aTData, aPISeg, aPINod, Standard_False))
              {
                aNodIndices.Flag |=  NMsk_Norm;
              }
              else
              {
                aNodIndices.Flag &= ~NMsk_Norm;
                aNod1RValues.Scal = 0;
              }
            }
          }
        #ifdef OCCT_DEBUG
          else if (DoError)
          {
            std::cout << " HLRBRep_PolyAlgo::StoreShell : Face ";
            std::cout << aFaceIndex << " non triangulated" << std::endl;
          }
        #endif

          for (Standard_Integer aTriIter = 1; aTriIter <= aNbTris; ++aTriIter)
          {
            HLRAlgo_TriangleData& aTriData = aTData.ChangeValue (aTriIter);
            const Handle(HLRAlgo_PolyInternalNode)& aPN1 = aPINod.ChangeValue (aTriData.Node1);
            const Handle(HLRAlgo_PolyInternalNode)& aPN2 = aPINod.ChangeValue (aTriData.Node2);
            const Handle(HLRAlgo_PolyInternalNode)& aPN3 = aPINod.ChangeValue (aTriData.Node3);
            OrientTriangle (aTriIter, aTriData,
                            aPN1->Indices(), aPN1->Data(),
                            aPN2->Indices(), aPN2->Data(),
                            aPN3->Indices(), aPN3->Data());
          }
        }
      }
#ifdef OCCT_DEBUG
      else if (DoError)
      {
        std::cout << "HLRBRep_PolyAlgo::StoreShell : Face ";
        std::cout << iFace << " deja stockee" << std::endl;
      }
#endif
    }

    const Standard_Integer aNbFaces = myFMap.Extent();
    HLRAlgo_ListOfBPoint& aList = aPsd->Edges();
    TopTools_IndexedDataMapOfShapeListOfShape EF;
    TopExp::MapShapesAndAncestors (theShape, TopAbs_EDGE, TopAbs_FACE, EF);
    for (Standard_Integer aFaceIter = 1; aFaceIter <= aNbFaces; ++aFaceIter)
    {
      if (!thePID.Value (aFaceIter).IsNull())
      {
        for (anEdgeExp.Init (myFMap (aFaceIter), TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
        {
          TopoDS_Edge anEdge = TopoDS::Edge (anEdgeExp.Current());
          if (theShapeMap1.Add (anEdge))
          {
            Standard_Integer anEdgeIndex = myEMap.FindIndex (anEdge);
            theES.SetValue (anEdgeIndex, theIShell);
            Standard_Integer anIndexE = EF.FindIndex (anEdge);
            if (anIndexE > 0)
            {
              TopTools_ListOfShape& LS = EF (anIndexE);
              InitBiPointsWithConnexity (anEdgeIndex, anEdge, aList, thePID, LS, Standard_True);
            }
            else
            {
              TopTools_ListOfShape LS;
              InitBiPointsWithConnexity (anEdgeIndex, anEdge, aList, thePID, LS, Standard_False);
            }
          }
        }
      }
    }
    InsertOnOutLine (thePID);
    CheckFrBackTriangles (aList, thePID);
    UpdateOutLines (aList, thePID);
    UpdateEdgesBiPoints (aList, thePID, isClosed);
    UpdatePolyData (thePD, thePID, isClosed);

    for (Standard_Integer aFaceIter = 1; aFaceIter <= aNbFaces; ++aFaceIter)
    {
      thePID.ChangeValue (aFaceIter).Nullify();
    }
  }
  else if (theIsoledE)
  {
    const Handle(HLRAlgo_PolyShellData)& aPsd = theShell.ChangeValue (theIShell);
    HLRAlgo_ListOfBPoint& aList = aPsd->Edges();
    for (anEdgeExp.Init (theShape, TopAbs_EDGE, TopAbs_FACE); anEdgeExp.More(); anEdgeExp.Next())
    {
      TopoDS_Edge anEdge = TopoDS::Edge (anEdgeExp.Current());
      if (theShapeMap1.Add (anEdge))
      {
        Standard_Integer anEdgeIndex = myEMap.FindIndex (anEdge);
        theES.SetValue (anEdgeIndex, theIShell);
        TopTools_ListOfShape aLS;
        InitBiPointsWithConnexity (anEdgeIndex, anEdge, aList, thePID, aLS, Standard_False);
      }
    }
  }
}

//=======================================================================
//function : Normal
//purpose  :
//=======================================================================
Standard_Boolean HLRBRep_PolyAlgo::Normal (const Standard_Integer theNodeIndex,
                                           HLRAlgo_PolyInternalNode::NodeIndices& theNodIndices,
                                           HLRAlgo_PolyInternalNode::NodeData& theNod1RValues,
                                           HLRAlgo_Array1OfTData& theTriData,
                                           HLRAlgo_Array1OfPISeg& thePISeg,
                                           HLRAlgo_Array1OfPINod& thePINod,
                                           const Standard_Boolean theToOrient) const
{
  if (theNod1RValues.Normal.SquareModulus() < Precision::Confusion())
  {
    gp_Vec aD1U, aD1V;
    gp_Pnt aPnt;
    CSLib_DerivativeStatus aStatus = CSLib_D1IsNull;
    myBSurf.D1 (theNod1RValues.UV.X(), theNod1RValues.UV.Y(), aPnt, aD1U, aD1V);
    gp_Dir aNorm;
    CSLib::Normal (aD1U, aD1V, Precision::Angular(), aStatus, aNorm);
    if (aStatus != CSLib_Done)
    {
      gp_Vec aD2U, aD2V, aD2UV;
      bool isOK = false;
      CSLib_NormalStatus aNromStatus;
      myBSurf.D2 (theNod1RValues.UV.X(), theNod1RValues.UV.Y(), aPnt, aD1U, aD1V, aD2U, aD2V, aD2UV);
      CSLib::Normal (aD1U, aD1V, aD2U, aD2V, aD2UV,
                     Precision::Angular(), isOK, aNromStatus, aNorm);
      if (!isOK)
      {
        return false;
      }
    }
    theNod1RValues.Normal = aNorm.XYZ();
  }

  TMultiply (theNod1RValues.Normal, myProj.Perspective());

  gp_XYZ anAverNorm;
  if (AverageNormal (theNodeIndex, theNodIndices, theTriData, thePISeg, thePINod, anAverNorm))
  {
    if (theNod1RValues.Normal * anAverNorm < 0)
    {
      theNod1RValues.Normal.Reverse();
    }

    gp_XYZ anEyeDir (0.0, 0.0, -1.0);
    if (myProj.Perspective())
    {
      anEyeDir.SetCoord (theNod1RValues.Point.X(),
                         theNod1RValues.Point.Y(),
                         theNod1RValues.Point.Z() - myProj.Focus());
      const Standard_Real anEyeMod = anEyeDir.Modulus();
      if (anEyeMod > 0.0)
      {
        anEyeDir /= anEyeMod;
      }
    }
    theNod1RValues.Scal = (theNod1RValues.Normal * anEyeDir);
  }
  else
  {
    theNod1RValues.Scal = 0;
    theNod1RValues.Normal = gp_XYZ(1., 0., 0.);
#ifdef OCCT_DEBUG
    if (DoError) {
      std::cout << "HLRBRep_PolyAlgo::Normal : AverageNormal error";
      std::cout << std::endl;
    }
#endif
  }
  if (theNod1RValues.Scal > 0)
  {
    if ( theNod1RValues.Scal < myTolAngular)
    {
      theNod1RValues.Scal  = 0;
      theNodIndices.Flag |= NMsk_OutL;
    }
  }
  else
  {
    if (-theNod1RValues.Scal < myTolAngular)
    {
      theNod1RValues.Scal  = 0;
      theNodIndices.Flag |= NMsk_OutL;
    }
  }

  if (theToOrient)
  {
    UpdateAroundNode (theNodeIndex, theNodIndices, theTriData, thePISeg, thePINod);
  }
  return Standard_True;
}

//=======================================================================
//function : AverageNormal
//purpose  :
//=======================================================================
Standard_Boolean HLRBRep_PolyAlgo::AverageNormal (const Standard_Integer iNode,
                                                  HLRAlgo_PolyInternalNode::NodeIndices& theNodeIndices,
                                                  HLRAlgo_Array1OfTData& theTData,
                                                  HLRAlgo_Array1OfPISeg& thePISeg,
                                                  HLRAlgo_Array1OfPINod& thePINod,
                                                  Standard_Real& theX,
                                                  Standard_Real& theY,
                                                  Standard_Real& theZ) const
{
  Standard_Boolean isOK = Standard_False;
  Standard_Integer jNode = 0, kNode, iiii;
  theX = 0;
  theY = 0;
  theZ = 0;
  iiii = theNodeIndices.NdSg;
  while (iiii != 0 && !isOK)
  {
    HLRAlgo_PolyInternalSegment& aSegIndices = thePISeg.ChangeValue (iiii);
    const Standard_Integer iTri1 = aSegIndices.Conex1;
    const Standard_Integer iTri2 = aSegIndices.Conex2;
    if (iTri1 != 0) { AddNormalOnTriangle (iTri1, iNode, jNode, theTData, thePINod, theX, theY, theZ, isOK); }
    if (iTri2 != 0) { AddNormalOnTriangle (iTri2, iNode, jNode, theTData, thePINod, theX, theY, theZ, isOK); }
    if (aSegIndices.LstSg1 == iNode) { iiii = aSegIndices.NxtSg1; }
    else                             { iiii = aSegIndices.NxtSg2; }
  }

  if (jNode != 0)
  {
    iiii = theNodeIndices.NdSg;
    
    while (iiii != 0 && !isOK)
    {
      HLRAlgo_PolyInternalSegment& aSegIndices = thePISeg.ChangeValue(iiii);
      const Standard_Integer iTri1 = aSegIndices.Conex1;
      const Standard_Integer iTri2 = aSegIndices.Conex2;
      if (iTri1 != 0) { AddNormalOnTriangle (iTri1, jNode, kNode, theTData, thePINod, theX, theY, theZ, isOK); }
      if (iTri2 != 0) { AddNormalOnTriangle (iTri2, jNode, kNode, theTData, thePINod, theX, theY, theZ, isOK); }
      if (aSegIndices.LstSg1 == jNode) { iiii = aSegIndices.NxtSg1; }
      else                             { iiii = aSegIndices.NxtSg2; }
    }
  }
  const Standard_Real aD = sqrt (theX * theX + theY * theY + theZ * theZ);
  if (isOK && aD < 1.e-10)
  {
    isOK = Standard_False;
#ifdef OCCT_DEBUG
    if (DoError)
    {
      std::cout << "HLRAlgo_PolyInternalData:: inverted normals on ";
      std::cout << "node " << iNode << std::endl;
    }
#endif
  }
  return isOK;
}

//=======================================================================
//function : AddNormalOnTriangle
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::AddNormalOnTriangle (const Standard_Integer theITri,
                                            const Standard_Integer theINode,
                                            Standard_Integer& theJNode,
                                            HLRAlgo_Array1OfTData& theTData,
                                            HLRAlgo_Array1OfPINod& thePINod,
                                            Standard_Real& theX,
                                            Standard_Real& theY,
                                            Standard_Real& theZ,
                                            Standard_Boolean& theIsOK) const
{
  const HLRAlgo_TriangleData& aTriangle = theTData.Value (theITri);
  const HLRAlgo_PolyInternalNode::NodeData& aNod1RValues = thePINod.Value (aTriangle.Node1)->Data();
  const HLRAlgo_PolyInternalNode::NodeData& aNod2RValues = thePINod.Value (aTriangle.Node2)->Data();
  const HLRAlgo_PolyInternalNode::NodeData& aNod3RValues = thePINod.Value (aTriangle.Node3)->Data();
  const gp_XYZ aD1 = aNod2RValues.Point - aNod1RValues.Point;
  const Standard_Real aD1Norm = aD1.Modulus();
  if (aD1Norm < 1.e-10)
  {
    if      (aTriangle.Node1 == theINode) { theJNode = aTriangle.Node2; }
    else if (aTriangle.Node2 == theINode) { theJNode = aTriangle.Node1; }
  }
  else
  {
    const gp_XYZ aD2 = aNod3RValues.Point - aNod2RValues.Point;
    const Standard_Real aD2Norm = aD2.Modulus();
    if (aD2Norm < 1.e-10)
    {
      if      (aTriangle.Node2 == theINode) { theJNode = aTriangle.Node3; }
      else if (aTriangle.Node3 == theINode) { theJNode = aTriangle.Node2; }
    }
    else
    {
      const gp_XYZ aD3 = aNod1RValues.Point - aNod3RValues.Point;
      const Standard_Real aD3Norm = aD3.Modulus();
      if (aD3Norm < 1.e-10)
      {
        if      (aTriangle.Node3 == theINode) { theJNode = aTriangle.Node1; }
        else if (aTriangle.Node1 == theINode) { theJNode = aTriangle.Node3; }
      }
      else
      {
        const gp_XYZ aDN = (1 / (aD1Norm * aD2Norm)) * (aD1 ^ aD2);
        const Standard_Real aDNNorm = aDN.Modulus();
        if (aDNNorm > 1.e-10)
        {
          theIsOK = Standard_True;
          theX += aDN.X();
          theY += aDN.Y();
          theZ += aDN.Z();
        }
      }
    }
  }
}

//=======================================================================
//function : InitBiPointsWithConnexity
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::InitBiPointsWithConnexity (const Standard_Integer theIEdge,
                                                  TopoDS_Edge& theEdge,
                                                  HLRAlgo_ListOfBPoint& theList,
                                                  NCollection_Array1<Handle(HLRAlgo_PolyInternalData)>& thePID,
                                                  TopTools_ListOfShape& theLS,
                                                  const Standard_Boolean theIsConnex)
{
  Standard_Real X1  , Y1  , Z1  , X2  , Y2  , Z2  ;
  Standard_Real XTI1, YTI1, ZTI1, XTI2, YTI2, ZTI2;
  Standard_Real U1 = 0.0, U2 = 0.0;
  Handle(Poly_PolygonOnTriangulation) aHPol[2];
  TopLoc_Location aLoc;
  myBCurv.Initialize (theEdge);
  if (theIsConnex)
  {
    const Standard_Integer aNbConnex = theLS.Extent();
    if (aNbConnex == 1)
    {
      TopTools_ListIteratorOfListOfShape itn (theLS);
      const TopoDS_Face& aF1 = TopoDS::Face (itn.Value());
      const Standard_Integer i1 = myFMap.FindIndex (aF1);
      const Handle(Poly_Triangulation)& aTr1 = BRep_Tool::Triangulation (aF1, aLoc);
      aHPol[0] = BRep_Tool::PolygonOnTriangulation (theEdge, aTr1, aLoc);
      const Handle(HLRAlgo_PolyInternalData)& pid1 = thePID.Value (i1);
      if (!aHPol[0].IsNull())
      {
        myPC.Initialize (theEdge, aF1);
        const Handle(TColStd_HArray1OfReal)& par = aHPol[0]->Parameters();
        const TColStd_Array1OfInteger&     aPol1 = aHPol[0]->Nodes();
        const Standard_Integer aNbPol  = aPol1.Upper();
        HLRAlgo_Array1OfTData* aTData1 = &pid1->TData();
        HLRAlgo_Array1OfPISeg* aPISeg1 = &pid1->PISeg();
        HLRAlgo_Array1OfPINod* aPINod1 = &pid1->PINod();
        const Handle(HLRAlgo_PolyInternalNode)& pi1p1 = aPINod1->ChangeValue (aPol1 (1    ));
        HLRAlgo_PolyInternalNode::NodeIndices*  aNode11Indices = &pi1p1->Indices();
        HLRAlgo_PolyInternalNode::NodeData*     aNod11RValues  = &pi1p1->Data();
        const Handle(HLRAlgo_PolyInternalNode)& pi1p2 = aPINod1->ChangeValue (aPol1 (aNbPol));
        HLRAlgo_PolyInternalNode::NodeIndices*  aNode12Indices = &pi1p2->Indices();
        HLRAlgo_PolyInternalNode::NodeData*     aNod12RValues  = &pi1p2->Data();
        aNode11Indices->Flag |=  NMsk_Vert;
        aNode12Indices->Flag |=  NMsk_Vert;
	
        for (Standard_Integer iPol = 1; iPol <= aNbPol; iPol++)
        {
          const Handle(HLRAlgo_PolyInternalNode)& pi1pA = aPINod1->ChangeValue (aPol1 (iPol));
          HLRAlgo_PolyInternalNode::NodeIndices& aNodeIndices1A = pi1pA->Indices();
          HLRAlgo_PolyInternalNode::NodeData& Nod1ARValues = pi1pA->Data();
          if (aNodeIndices1A.Edg1 == 0 || aNodeIndices1A.Edg1 == theIEdge)
          {
            aNodeIndices1A.Edg1 = theIEdge;
            Nod1ARValues.PCu1 = par->Value (iPol);
          }
          else
          {
            aNodeIndices1A.Edg2 = theIEdge;
            Nod1ARValues.PCu2 = par->Value (iPol);
          }
        }
	
        Standard_Integer i1p2 = aPol1 (1);
        aNode12Indices = aNode11Indices;
        aNod12RValues  = aNod11RValues;
        XTI2 = X2 = aNod12RValues->Point.X();
        YTI2 = Y2 = aNod12RValues->Point.Y();
        ZTI2 = Z2 = aNod12RValues->Point.Z();
        if      (aNode12Indices->Edg1 == theIEdge) { U2 = aNod12RValues->PCu1; }
        else if (aNode12Indices->Edg2 == theIEdge) { U2 = aNod12RValues->PCu2; }
      #ifdef OCCT_DEBUG
        else
        {
          std::cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
          std::cout << "Parameter error on Node " << i1p2 << std::endl;
        }
      #endif
        aNode12Indices->Flag |= NMsk_Edge;
        TIMultiply (XTI2, YTI2, ZTI2);
        if (aPol1 (1) == aPol1 (aNbPol)
         && myPC.IsPeriodic())
        {
          U2 = U2 - myPC.Period();
        }
	
        if (aNbPol == 2 && BRep_Tool::Degenerated (theEdge))
        {
          CheckDegeneratedSegment (*aNode11Indices, *aNod11RValues,
                                   *aNode12Indices, *aNod12RValues);
          UpdateAroundNode (aPol1 (1     ), *aNode11Indices, *aTData1, *aPISeg1, *aPINod1);
          UpdateAroundNode (aPol1 (aNbPol), *aNode12Indices, *aTData1, *aPISeg1, *aPINod1);
        }
        else
        {
          for (Standard_Integer iPol = 2; iPol <= aNbPol; iPol++)
          {
            const Standard_Integer i1p1 = i1p2;
            aNode11Indices = aNode12Indices;
            aNod11RValues  = aNod12RValues;
            i1p2 = aPol1 (iPol);
            const Handle(HLRAlgo_PolyInternalNode)& pi1p2iPol = aPINod1->ChangeValue (aPol1 (iPol));
            aNode12Indices = &pi1p2iPol->Indices();
            aNod12RValues  = &pi1p2iPol->Data();
          #ifdef OCCT_DEBUG
            if (DoError)
            {
              if (aNod11RValues->Normal.X() * aNod12RValues->Normal.X() +
                  aNod11RValues->Normal.Y() * aNod12RValues->Normal.Y() +
                  aNod11RValues->Normal.Z() * aNod12RValues->Normal.Z() < 0)
              {
                std::cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
                std::cout << "Too big angle between " << i1p1 << std::setw(6);
                std::cout << " and " << i1p2 << std::setw(6);
                std::cout << " in face " << i1 << std::endl;
              }
            }
          #endif
            X1   = X2;
            Y1   = Y2;
            Z1   = Z2;
            XTI1 = XTI2;
            YTI1 = YTI2;
            ZTI1 = ZTI2;
            U1   = U2;
            XTI2 = X2 = aNod12RValues->Point.X();
            YTI2 = Y2 = aNod12RValues->Point.Y();
            ZTI2 = Z2 = aNod12RValues->Point.Z();
            if      (aNode12Indices->Edg1 == theIEdge) { U2 = aNod12RValues->PCu1; }
            else if (aNode12Indices->Edg2 == theIEdge) { U2 = aNod12RValues->PCu2; }
          #ifdef OCCT_DEBUG
            else
            {
              std::cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
              std::cout << "Parameter error on Node " << i1p2 << std::endl;
            }
          #endif
            aNode12Indices->Flag |= NMsk_Edge;
            TIMultiply (XTI2, YTI2, ZTI2);
            Interpolation (theList,
                           X1  , Y1  , Z1  , X2  , Y2  , Z2  ,
                           XTI1, YTI1, ZTI1, XTI2, YTI2, ZTI2,
                           theIEdge, U1, U2,
                           *aNode11Indices, *aNod11RValues,
                           *aNode12Indices, *aNod12RValues,
                           i1p1, i1p2, i1, pid1, aTData1, aPISeg1, aPINod1);
          }
        }
      }
    #ifdef OCCT_DEBUG
      else if (DoError)
      {
        std::cout << "HLRBRep_PolyAlgo::InitBiPointsWithConnexity : Edge ";
        std::cout << theIEdge << " connex 1 sans PolygonOnTriangulation" << std::endl;
      }
    #endif
    }
    else if (aNbConnex == 2)
    {
      TopTools_ListIteratorOfListOfShape itn (theLS);
      const TopoDS_Face&     aF1 = TopoDS::Face (itn.Value());
      const Standard_Integer i1  = myFMap.FindIndex (aF1);
      const Handle(Poly_Triangulation)& aTr1 = BRep_Tool::Triangulation (aF1, aLoc);
      aHPol[0] = BRep_Tool::PolygonOnTriangulation (theEdge, aTr1, aLoc);
      itn.Next();
      const TopoDS_Face&     aF2 = TopoDS::Face (itn.Value());
      const Standard_Integer i2  = myFMap.FindIndex (aF2);
      if (i1 == i2) { theEdge.Reverse(); }
      const Handle(Poly_Triangulation)& aTr2 = BRep_Tool::Triangulation (aF2, aLoc);
      aHPol[1] = BRep_Tool::PolygonOnTriangulation (theEdge, aTr2, aLoc);
      GeomAbs_Shape rg = BRep_Tool::Continuity (theEdge, aF1, aF2);
      const Handle(HLRAlgo_PolyInternalData)& pid1 = thePID.Value (i1);
      const Handle(HLRAlgo_PolyInternalData)& pid2 = thePID.Value (i2);
      if (!aHPol[0].IsNull()
       && !aHPol[1].IsNull())
      {
        myPC.Initialize (theEdge, aF1);
        const TColStd_Array1OfInteger&      aPol1 = aHPol[0]->Nodes();
        const TColStd_Array1OfInteger&      aPol2 = aHPol[1]->Nodes();
        const Handle(TColStd_HArray1OfReal)& par = aHPol[0]->Parameters();
        const Standard_Integer aNbPol1 = aPol1.Upper();
        HLRAlgo_Array1OfTData* aTData1 = &pid1->TData();
        HLRAlgo_Array1OfPISeg* aPISeg1 = &pid1->PISeg();
        HLRAlgo_Array1OfPINod* aPINod1 = &pid1->PINod();
        HLRAlgo_Array1OfTData* aTData2 = &pid2->TData();
        HLRAlgo_Array1OfPISeg* aPISeg2 = &pid2->PISeg();
        HLRAlgo_Array1OfPINod* aPINod2 = &pid2->PINod();
        const Handle(HLRAlgo_PolyInternalNode)* pi1p1 = &aPINod1->ChangeValue (aPol1 (1));
        HLRAlgo_PolyInternalNode::NodeIndices*  aNode11Indices = &(*pi1p1)->Indices();
        HLRAlgo_PolyInternalNode::NodeData*     aNod11RValues  = &(*pi1p1)->Data();
        const Handle(HLRAlgo_PolyInternalNode)* pi1p2nbPol1 = &aPINod1->ChangeValue (aPol1 (aNbPol1));
        HLRAlgo_PolyInternalNode::NodeIndices*  aNode12Indices = &(*pi1p2nbPol1)->Indices();
        HLRAlgo_PolyInternalNode::NodeData*     aNod12RValues  = &(*pi1p2nbPol1)->Data();
        const Handle(HLRAlgo_PolyInternalNode)* pi2p1 = &aPINod2->ChangeValue (aPol2 (1));
        HLRAlgo_PolyInternalNode::NodeIndices*  aNod21Indices = &(*pi2p1)->Indices();
        HLRAlgo_PolyInternalNode::NodeData*     aNod21RValues = &(*pi2p1)->Data();
        const Handle(HLRAlgo_PolyInternalNode)* pi2p2 = &aPINod2->ChangeValue (aPol2 (aNbPol1));
        HLRAlgo_PolyInternalNode::NodeIndices*  aNod22Indices = &(*pi2p2)->Indices();
        HLRAlgo_PolyInternalNode::NodeData*     aNod22RValues = &(*pi2p2)->Data();
        aNode11Indices->Flag |=  NMsk_Vert;
        aNode12Indices->Flag |=  NMsk_Vert;
        aNod21Indices->Flag |=  NMsk_Vert;
        aNod22Indices->Flag |=  NMsk_Vert;
	
        for (Standard_Integer iPol = 1; iPol <= aNbPol1; iPol++)
        {
          const Handle(HLRAlgo_PolyInternalNode)* pi1pA = &aPINod1->ChangeValue (aPol1 (iPol));
          HLRAlgo_PolyInternalNode::NodeIndices*  aNod1AIndices = &(*pi1pA)->Indices();
          HLRAlgo_PolyInternalNode::NodeData*     aNod1ARValues = &(*pi1pA)->Data();
          const Handle(HLRAlgo_PolyInternalNode)* pi2pA = &aPINod2->ChangeValue (aPol2 (iPol));
          HLRAlgo_PolyInternalNode::NodeIndices*  aNod2AIndices = &(*pi2pA)->Indices();
          HLRAlgo_PolyInternalNode::NodeData*     aNod2ARValues = &(*pi2pA)->Data();
          Standard_Real PCu = par->Value(iPol);
          if (aNod1AIndices->Edg1 == 0 || aNod1AIndices->Edg1 == theIEdge)
          {
            aNod1AIndices->Edg1 = theIEdge;
            aNod1ARValues->PCu1 = PCu;
          }
          else
          {
            aNod1AIndices->Edg2 = theIEdge;
            aNod1ARValues->PCu2 = PCu;
          }
          if (aNod2AIndices->Edg1 == 0 || aNod2AIndices->Edg1 == theIEdge)
          {
            aNod2AIndices->Edg1 = theIEdge;
            aNod2ARValues->PCu1 = PCu;
          }
          else
          {
            aNod2AIndices->Edg2 = theIEdge;
            aNod2ARValues->PCu2 = PCu;
          }
        }

        Standard_Integer i1p2 = aPol1 (1);
        aNode12Indices = aNode11Indices;
        aNod12RValues  = aNod11RValues;
        Standard_Integer i2p2 = aPol2 (1);
        aNod22Indices = aNod21Indices;
        aNod22RValues = aNod21RValues;
        XTI2 = X2 = aNod12RValues->Point.X();
        YTI2 = Y2 = aNod12RValues->Point.Y();
        ZTI2 = Z2 = aNod12RValues->Point.Z();
        if      (aNode12Indices->Edg1 == theIEdge) { U2 = aNod12RValues->PCu1; }
        else if (aNode12Indices->Edg2 == theIEdge) { U2 = aNod12RValues->PCu2; }
      #ifdef OCCT_DEBUG
        else
        {
          std::cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
          std::cout << "Parameter error on Node " << i1p2 << std::endl;
        }
      #endif
        aNode12Indices->Flag |= NMsk_Edge;
        aNod22Indices ->Flag |= NMsk_Edge;
        TIMultiply (XTI2, YTI2, ZTI2);
        if (aPol1 (1) == aPol1 (aNbPol1) && myPC.IsPeriodic())
        {
          U2 = U2 - myPC.Period();
        }
	
        if (aNbPol1 == 2 && BRep_Tool::Degenerated (theEdge))
        {
          CheckDegeneratedSegment (*aNode11Indices, *aNod11RValues,
                                   *aNode12Indices, *aNod12RValues);
          CheckDegeneratedSegment (*aNod21Indices,  *aNod21RValues,
                                   *aNod22Indices,  *aNod22RValues);
          UpdateAroundNode (aPol1 (1     ),  *aNode11Indices, *aTData1, *aPISeg1, *aPINod1);
          UpdateAroundNode (aPol1 (aNbPol1), *aNode12Indices, *aTData1, *aPISeg1, *aPINod1);
          UpdateAroundNode (aPol2 (1     ),  *aNod21Indices,  *aTData2, *aPISeg2, *aPINod2);
          UpdateAroundNode (aPol2 (aNbPol1), *aNod22Indices,  *aTData2, *aPISeg2, *aPINod2);
        }
        else
        {
          for (Standard_Integer iPol = 2; iPol <= aNbPol1; iPol++)
          {
            const Standard_Integer i1p1 = i1p2;
            aNode11Indices = aNode12Indices;
            aNod11RValues  = aNod12RValues;
            const Standard_Integer i2p1 = i2p2;
            aNod21Indices = aNod22Indices;
            aNod21RValues = aNod22RValues;
            i1p2 = aPol1 (iPol);
            const Handle(HLRAlgo_PolyInternalNode)* pi1p2iPol = &aPINod1->ChangeValue (aPol1 (iPol));
            aNode12Indices = &(*pi1p2iPol)->Indices();
            aNod12RValues  = &(*pi1p2iPol)->Data();
            i2p2 = aPol2 (iPol);
            const Handle(HLRAlgo_PolyInternalNode)* pi2p2iPol = &aPINod2->ChangeValue (aPol2 (iPol));
            aNod22Indices = &(*pi2p2iPol)->Indices();
            aNod22RValues = &(*pi2p2iPol)->Data();
          #ifdef OCCT_DEBUG
            if (DoError)
            {
              if (aNod11RValues->Normal.X() * aNod12RValues->Normal.X() +
                  aNod11RValues->Normal.Y() * aNod12RValues->Normal.Y() +
                  aNod11RValues->Normal.Z() * aNod12RValues->Normal.Z() < 0)
              {
                std::cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
                std::cout << "To big angle between " << i1p1 << std::setw(6);
                std::cout << " and " << i1p2 << std::setw(6);
                std::cout << " in face " << i1 << std::endl;
              }
              if (aNod21RValues->Normal.X() * aNod22RValues->Normal.X() +
                  aNod21RValues->Normal.Y() * aNod22RValues->Normal.Y() +
                  aNod21RValues->Normal.Z() * aNod22RValues->Normal.Z() < 0)
              {
                std::cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
                std::cout << "To big angle between " << i2p1 << std::setw(6);
                std::cout << " and " << i2p2 << std::setw(6);
                std::cout<< " in face " << i2 << std::endl;
              }
            }
          #endif
            X1   = X2;
            Y1   = Y2;
            Z1   = Z2;
            XTI1 = XTI2;
            YTI1 = YTI2;
            ZTI1 = ZTI2;
            U1   = U2;
            XTI2 = X2 = aNod12RValues->Point.X();
            YTI2 = Y2 = aNod12RValues->Point.Y();
            ZTI2 = Z2 = aNod12RValues->Point.Z();
            if      (aNode12Indices->Edg1 == theIEdge) { U2 = aNod12RValues->PCu1; }
            else if (aNode12Indices->Edg2 == theIEdge) { U2 = aNod12RValues->PCu2; }
          #ifdef OCCT_DEBUG
            else
            {
              std::cout << " HLRBRep_PolyAlgo::InitBiPointsWithConnexity : ";
              std::cout << "Parameter error on Node " << i1p2 << std::endl;
            }
          #endif
            aNode12Indices->Flag |= NMsk_Edge;
            aNode12Indices->Flag |= NMsk_Edge;
            TIMultiply (XTI2, YTI2, ZTI2);
            Interpolation (theList,
                           X1  , Y1  , Z1  , X2  , Y2  , Z2  ,
                           XTI1, YTI1, ZTI1, XTI2, YTI2, ZTI2,
                           theIEdge, U1, U2, rg,
                           *aNode11Indices, *aNod11RValues,
                           *aNode12Indices, *aNod12RValues,
                           i1p1, i1p2, i1, pid1, aTData1, aPISeg1, aPINod1,
                           *aNod21Indices, *aNod21RValues,
                           *aNod22Indices, *aNod22RValues,
                           i2p1, i2p2, i2, pid2, aTData2, aPISeg2, aPINod2);
          }
        }
      }
    #ifdef OCCT_DEBUG
      else if (DoError)
      {
        std::cout << "HLRBRep_PolyAlgo::InitBiPointsWithConnexity : Edge ";
        std::cout << theIEdge << " connect 2 without PolygonOnTriangulation" << std::endl;
      }
    #endif
    }
  }
  else
  {  // no connexity
    const Handle(Poly_Polygon3D)& aPolyg = BRep_Tool::Polygon3D (theEdge, aLoc);
    if (!aPolyg.IsNull())
    {
      const TColgp_Array1OfPnt& aPol = aPolyg->Nodes();
      gp_Trsf aTT = aLoc.Transformation();
      const gp_Trsf& aProjTrsf = myProj.Transformation();
      aTT.PreMultiply (aProjTrsf);
      {
        const gp_XYZ& aTTrsfVec = aTT.TranslationPart();
        TTLo[0] = aTTrsfVec.X();
        TTLo[1] = aTTrsfVec.Y();
        TTLo[2] = aTTrsfVec.Z();
        const gp_Mat& aTTrsfMat = aTT.VectorialPart();
        TTMa[0][0] = aTTrsfMat.Value (1, 1);
        TTMa[0][1] = aTTrsfMat.Value (1, 2);
        TTMa[0][2] = aTTrsfMat.Value (1, 3);
        TTMa[1][0] = aTTrsfMat.Value (2, 1);
        TTMa[1][1] = aTTrsfMat.Value (2, 2);
        TTMa[1][2] = aTTrsfMat.Value (2, 3);
        TTMa[2][0] = aTTrsfMat.Value (3, 1);
        TTMa[2][1] = aTTrsfMat.Value (3, 2);
        TTMa[2][2] = aTTrsfMat.Value (3, 3);
      }
      const Standard_Integer aNbPol1 = aPol.Upper();
      const gp_XYZ& aP1 = aPol(1).XYZ();
      X2 = aP1.X();
      Y2 = aP1.Y();
      Z2 = aP1.Z();
      TTMultiply (X2, Y2, Z2);
      XTI2 = X2;
      YTI2 = Y2;
      ZTI2 = Z2;
      TIMultiply (XTI2, YTI2, ZTI2);
      
      for (Standard_Integer jPol = 2; jPol <= aNbPol1; jPol++)
      {
        X1   = X2;
        Y1   = Y2;
        Z1   = Z2;
        XTI1 = XTI2;
        YTI1 = YTI2;
        ZTI1 = ZTI2;
        const gp_XYZ& aP2 = aPol(jPol).XYZ();
        X2 = aP2.X();
        Y2 = aP2.Y();
        Z2 = aP2.Z();
        TTMultiply (X2, Y2, Z2);
        XTI2 = X2;
        YTI2 = Y2;
        ZTI2 = Z2;
        TIMultiply (XTI2, YTI2, ZTI2);
        theList.Prepend (HLRAlgo_BiPoint (XTI1, YTI1, ZTI1, XTI2, YTI2, ZTI2,
                                          X1  , Y1  , Z1  , X2  , Y2  , Z2,
                                          theIEdge, 0));
      }
    }
#ifdef OCCT_DEBUG
    else if (DoError)
    {
      std::cout << "HLRBRep_PolyAlgo::InitBiPointsWithConnexity : Edge ";
      std::cout << theIEdge << " Isolated, without Polygone 3D" << std::endl;
    }
#endif
  }
}

//=======================================================================
//function : Interpolation
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::Interpolation (HLRAlgo_ListOfBPoint& theList,
                                      Standard_Real& theX1,
                                      Standard_Real& theY1,
                                      Standard_Real& theZ1,
                                      Standard_Real& theX2,
                                      Standard_Real& theY2,
                                      Standard_Real& theZ2,
                                      Standard_Real& theXTI1,
                                      Standard_Real& theYTI1,
                                      Standard_Real& theZTI1,
                                      Standard_Real& theXTI2,
                                      Standard_Real& theYTI2,
                                      Standard_Real& theZTI2,
                                      const Standard_Integer theIEdge,
                                      Standard_Real& theU1,
                                      Standard_Real& theU2,
                                      HLRAlgo_PolyInternalNode::NodeIndices& theNod11Indices,
                                      HLRAlgo_PolyInternalNode::NodeData& theNod11RValues,
                                      HLRAlgo_PolyInternalNode::NodeIndices& theNod12Indices,
                                      HLRAlgo_PolyInternalNode::NodeData& theNod12RValues,
                                      const Standard_Integer theI1p1,
                                      const Standard_Integer theI1p2,
                                      const Standard_Integer theI1,
                                      const Handle(HLRAlgo_PolyInternalData)& thePid1,
                                      HLRAlgo_Array1OfTData*& theTData1,
                                      HLRAlgo_Array1OfPISeg*& thePISeg1,
                                      HLRAlgo_Array1OfPINod*& thePINod1) const
{
  Standard_Boolean mP3P1 = false;
  Standard_Real X3 = 0.0, Y3 = 0.0, Z3 = 0.0, XTI3 = 0.0, YTI3 = 0.0, ZTI3 = 0.0, coef3 = 0.0, U3 = 0.0;
//  gp_Pnt P3, PT3;
  Standard_Boolean insP3 = Interpolation (theU1, theU2, theNod11RValues, theNod12RValues,
                                          X3, Y3, Z3, XTI3, YTI3, ZTI3, coef3, U3, mP3P1);
  MoveOrInsertPoint (theList,
                     theX1  , theY1  , theZ1  , theX2  , theY2  , theZ2  ,
                     theXTI1, theYTI1, theZTI1, theXTI2, theYTI2, theZTI2,
                     theIEdge, theU1, theU2,
                     theNod11Indices, theNod11RValues,
                     theNod12Indices, theNod12RValues,
                     theI1p1, theI1p2, theI1, thePid1, theTData1, thePISeg1, thePINod1,
                     X3, Y3, Z3, XTI3, YTI3, ZTI3, coef3, U3, insP3, mP3P1, 0);
}

//=======================================================================
//function : Interpolation
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::Interpolation (HLRAlgo_ListOfBPoint& theList,
                                      Standard_Real& theX1,
                                      Standard_Real& theY1,
                                      Standard_Real& theZ1,
                                      Standard_Real& theX2,
                                      Standard_Real& theY2,
                                      Standard_Real& theZ2,
                                      Standard_Real& theXTI1,
                                      Standard_Real& theYTI1,
                                      Standard_Real& theZTI1,
                                      Standard_Real& theXTI2,
                                      Standard_Real& theYTI2,
                                      Standard_Real& theZTI2,
                                      const Standard_Integer theIEdge,
                                      Standard_Real& theU1,
                                      Standard_Real& theU2,
                                      const GeomAbs_Shape theRg,
                                      HLRAlgo_PolyInternalNode::NodeIndices& theNod11Indices,
                                      HLRAlgo_PolyInternalNode::NodeData& theNod11RValues,
                                      HLRAlgo_PolyInternalNode::NodeIndices& theNod12Indices,
                                      HLRAlgo_PolyInternalNode::NodeData& theNod12RValues,
                                      const Standard_Integer theI1p1,
                                      const Standard_Integer theI1p2,
                                      const Standard_Integer theI1,
                                      const Handle(HLRAlgo_PolyInternalData)& thePid1,
                                      HLRAlgo_Array1OfTData*& theTData1,
                                      HLRAlgo_Array1OfPISeg*& thePISeg1,
                                      HLRAlgo_Array1OfPINod*& thePINod1,
                                      HLRAlgo_PolyInternalNode::NodeIndices& theNod21Indices,
                                      HLRAlgo_PolyInternalNode::NodeData& theNod21RValues,
                                      HLRAlgo_PolyInternalNode::NodeIndices& theNod22Indices,
                                      HLRAlgo_PolyInternalNode::NodeData& theNod22RValues,
                                      const Standard_Integer theI2p1,
                                      const Standard_Integer theI2p2,
                                      const Standard_Integer theI2,
                                      const Handle(HLRAlgo_PolyInternalData)& thePid2,
                                      HLRAlgo_Array1OfTData*& theTData2,
                                      HLRAlgo_Array1OfPISeg*& thePISeg2,
                                      HLRAlgo_Array1OfPINod*& thePINod2) const
{
  Standard_Boolean mP3P1 = false, mP4P1 = false;
  Standard_Real X3 = 0.0, Y3 = 0.0, Z3 = 0.0, XTI3 = 0.0, YTI3 = 0.0, ZTI3 = 0.0, coef3 = 0.0, U3 = 0.0;
  Standard_Real X4 = 0.0, Y4 = 0.0, Z4 = 0.0, XTI4 = 0.0, YTI4 = 0.0, ZTI4 = 0.0, coef4 = 0.0, U4 = 0.0;
//  gp_Pnt P3, PT3, P4, PT4;
  Standard_Integer flag = 0;
  if (theRg >= GeomAbs_G1) { flag += 1; }
  if (theRg >= GeomAbs_G2) { flag += 2; }
  const bool insP3 = Interpolation (theU1, theU2, theNod11RValues, theNod12RValues,
                                    X3, Y3, Z3, XTI3, YTI3, ZTI3, coef3, U3, mP3P1);
  const bool insP4 = Interpolation (theU1, theU2, theNod21RValues, theNod22RValues,
                                    X4, Y4, Z4, XTI4, YTI4, ZTI4, coef4, U4, mP4P1);
  const bool isOK = insP3 || insP4;
  if (isOK)
  {
    if      (!insP4)                               // p1 i1p3 p2
    {
      MoveOrInsertPoint (theList,
                         theX1  , theY1  , theZ1  , theX2  , theY2  , theZ2  ,
                         theXTI1, theYTI1, theZTI1, theXTI2, theYTI2, theZTI2,
                         theIEdge, theU1, theU2,
                         theNod11Indices, theNod11RValues,
                         theNod12Indices, theNod12RValues,
                         theI1p1, theI1p2, theI1, thePid1, theTData1, thePISeg1, thePINod1,
                         theNod21Indices, theNod21RValues,
                         theNod22Indices, theNod22RValues,
                         theI2p1, theI2p2, theI2, thePid2, theTData2, thePISeg2, thePINod2,
                         X3, Y3, Z3, XTI3, YTI3, ZTI3, coef3, U3, insP3, mP3P1, flag);
    }
    else if (!insP3)                               // p1 i2p4 p2
    {
      MoveOrInsertPoint (theList,
                         theX1  , theY1  , theZ1  , theX2  , theY2  , theZ2  ,
                         theXTI1, theYTI1, theZTI1, theXTI2, theYTI2, theZTI2,
                         theIEdge, theU1, theU2,
                         theNod21Indices, theNod21RValues,
                         theNod22Indices, theNod22RValues,
                         theI2p1, theI2p2, theI2, thePid2, theTData2, thePISeg2, thePINod2,
                         theNod11Indices, theNod11RValues,
                         theNod12Indices, theNod12RValues,
                         theI1p1, theI1p2, theI1, thePid1, theTData1, thePISeg1, thePINod1,
                         X4, Y4, Z4, XTI4, YTI4, ZTI4, coef4, U4, insP4, mP4P1, flag);
    }
    else if (Abs(coef4 - coef3) < myTolSta)       // p1 i1p3-i2p4 p2
    {
      MoveOrInsertPoint (theList,
                         theX1  , theY1  , theZ1  , theX2  , theY2  , theZ2  ,
                         theXTI1, theYTI1, theZTI1, theXTI2, theYTI2, theZTI2,
                         theIEdge, theU1, theU2,
                         theNod21Indices, theNod21RValues,
                         theNod22Indices, theNod22RValues,
                         theI2p1, theI2p2, theI2, thePid2, theTData2, thePISeg2, thePINod2,
                         theNod11Indices, theNod11RValues,
                         theNod12Indices, theNod12RValues,
                         theI1p1, theI1p2, theI1, thePid1, theTData1, thePISeg1, thePINod1,
                         X4, Y4, Z4, XTI4, YTI4, ZTI4, coef4, U4, insP4, mP4P1, flag);
    }
    else if (coef4 < coef3)                        // p1 i2p4 i1p3 p2
    {
      MoveOrInsertPoint (theList,
                         theX1  , theY1  , theZ1  , theX2  , theY2  , theZ2  ,
                         theXTI1, theYTI1, theZTI1, theXTI2, theYTI2, theZTI2,
                         theIEdge, theU1, theU2,
                         theNod21Indices, theNod21RValues,
                         theNod22Indices, theNod22RValues,
                         theI2p1, theI2p2, theI2, thePid2, theTData2, thePISeg2, thePINod2,
                         theNod11Indices, theNod11RValues,
                         theNod12Indices, theNod12RValues,
                         theI1p1, theI1p2, theI1, thePid1, theTData1, thePISeg1, thePINod1,
                         X4, Y4, Z4, XTI4, YTI4, ZTI4, coef4, U4, insP4, mP4P1,
                         X3, Y3, Z3, XTI3, YTI3, ZTI3, coef3, U3, insP3, mP3P1, flag);
    }
    else                                           // p1 i1p3 i2p4 p2
    {
      MoveOrInsertPoint (theList,
                         theX1  , theY1  , theZ1  , theX2  , theY2  , theZ2  ,
                         theXTI1, theYTI1, theZTI1, theXTI2, theYTI2, theZTI2,
                         theIEdge, theU1, theU2,
                         theNod11Indices, theNod11RValues,
                         theNod12Indices, theNod12RValues,
                         theI1p1, theI1p2, theI1, thePid1, theTData1, thePISeg1, thePINod1,
                         theNod21Indices, theNod21RValues,
                         theNod22Indices, theNod22RValues,
                         theI2p1, theI2p2, theI2, thePid2, theTData2, thePISeg2, thePINod2,
                         X3, Y3, Z3, XTI3, YTI3, ZTI3, coef3, U3, insP3, mP3P1,
                         X4, Y4, Z4, XTI4, YTI4, ZTI4, coef4, U4, insP4, mP4P1, flag);
    }
  }
  else                                             // p1 p2
  {
    theList.Prepend (HLRAlgo_BiPoint (theXTI1, theYTI1, theZTI1, theXTI2, theYTI2, theZTI2,
                                      theX1  , theY1  , theZ1  , theX2  , theY2  , theZ2  , theIEdge,
                                      theI1  , theI1p1, theI1p2, theI2  , theI2p1, theI2p2, flag));
  }
}

//=======================================================================
//function : Interpolation
//purpose  :
//=======================================================================
Standard_Boolean HLRBRep_PolyAlgo::Interpolation (const Standard_Real theU1,
                                                  const Standard_Real theU2,
                                                  HLRAlgo_PolyInternalNode::NodeData& theNod1RValues,
                                                  HLRAlgo_PolyInternalNode::NodeData& theNod2RValues,
                                                  Standard_Real& theX3,
                                                  Standard_Real& theY3,
                                                  Standard_Real& theZ3,
                                                  Standard_Real& theXTI3,
                                                  Standard_Real& theYTI3,
                                                  Standard_Real& theZTI3,
                                                  Standard_Real& theCoef3,
                                                  Standard_Real& theU3,
                                                  Standard_Boolean& themP3P1) const
{
  if (NewNode (theNod1RValues, theNod2RValues, theCoef3, themP3P1))
  {
    theU3 = theU1 + (theU2 - theU1) * theCoef3;
    const gp_Pnt& aP3 = myBCurv.Value (theU3);
    theXTI3 = theX3 = aP3.X();
    theYTI3 = theY3 = aP3.Y();
    theZTI3 = theZ3 = aP3.Z();
    TMultiply (theX3, theY3, theZ3);
    return Standard_True;
  }

  theX3 = theY3 = theZ3 = theXTI3 = theYTI3 = theZTI3 = theCoef3 = theU3 = 0.0;
  return Standard_False;
}

//=======================================================================
//function : MoveOrInsertPoint
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::MoveOrInsertPoint (HLRAlgo_ListOfBPoint& theList,
                                          Standard_Real& theX1,
                                          Standard_Real& theY1,
                                          Standard_Real& theZ1,
                                          Standard_Real& theX2,
                                          Standard_Real& theY2,
                                          Standard_Real& theZ2,
                                          Standard_Real& theXTI1,
                                          Standard_Real& theYTI1,
                                          Standard_Real& theZTI1,
                                          Standard_Real& theXTI2,
                                          Standard_Real& theYTI2,
                                          Standard_Real& theZTI2,
                                          const Standard_Integer theIEdge,
                                          Standard_Real& theU1,
                                          Standard_Real& theU2,
                                          HLRAlgo_PolyInternalNode::NodeIndices& theNod11Indices,
                                          HLRAlgo_PolyInternalNode::NodeData& theNod11RValues,
                                          HLRAlgo_PolyInternalNode::NodeIndices& theNod12Indices,
                                          HLRAlgo_PolyInternalNode::NodeData& theNod12RValues,
                                          const Standard_Integer theI1p1,
                                          const Standard_Integer theI1p2,
                                          const Standard_Integer theI1,
                                          const Handle(HLRAlgo_PolyInternalData)& thePid1,
                                          HLRAlgo_Array1OfTData*& theTData1,
                                          HLRAlgo_Array1OfPISeg*& thePISeg1,
                                          HLRAlgo_Array1OfPINod*& thePINod1,
                                          const Standard_Real theX3,
                                          const Standard_Real theY3,
                                          const Standard_Real theZ3,
                                          const Standard_Real theXTI3,
                                          const Standard_Real theYTI3,
                                          const Standard_Real theZTI3,
                                          const Standard_Real theCoef3,
                                          const Standard_Real theU3,
                                          const Standard_Boolean theInsP3,
                                          const Standard_Boolean themP3P1,
                                          const Standard_Integer theFlag) const
{
  HLRAlgo_Array1OfTData* aTData2 = NULL;
  HLRAlgo_Array1OfPISeg* aPISeg2 = NULL;
  HLRAlgo_Array1OfPINod* aPINod2 = NULL;
  Standard_Boolean anIns3 = theInsP3;
  if (anIns3 && themP3P1)                               // P1 ---> P3
  {
    if (!(theNod11Indices.Flag & NMsk_Vert) && theCoef3 < myTolSta)
    {
      anIns3 = Standard_False;
      ChangeNode (theI1p1, theI1p2,
                  theNod11Indices, theNod11RValues,
                  theNod12Indices, theNod12RValues,
                  theCoef3, theX3, theY3, theZ3, Standard_True,
                  *theTData1, *thePISeg1, *thePINod1);
      theX1   = theX3;
      theY1   = theY3;
      theZ1   = theZ3;
      theXTI1 = theXTI3;
      theYTI1 = theYTI3;
      theZTI1 = theZTI3;
      theU1   = theU3;
      theNod11RValues.Point = gp_XYZ(theX3, theY3, theZ3);
      if      (theNod11Indices.Edg1 == theIEdge) { theNod11RValues.PCu1 = theU3; }
      else if (theNod11Indices.Edg2 == theIEdge) { theNod11RValues.PCu2 = theU3; }
#ifdef OCCT_DEBUG
      else
      {
        std::cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
        std::cout << "Parameter error on Node " << theI1p1 << std::endl;
      }
#endif
      theNod11RValues.Scal  = 0;
      theNod11Indices.Flag |= NMsk_OutL;
      UpdateAroundNode (theI1p1, theNod11Indices, *theTData1, *thePISeg1, *thePINod1);
      HLRAlgo_BiPoint::PointsT& aPoints = theList.First().Points();
      aPoints.PntP2 = gp_XYZ(theX3, theY3, theZ3);
      aPoints.Pnt2 = gp_XYZ(theXTI3, theYTI3, theZTI3);
    }
  }
  if (anIns3 && !themP3P1)                              // P2 ---> P3
  {
    if (!(theNod12Indices.Flag & NMsk_Vert) && theCoef3 > myTolEnd)
    {
      anIns3 = Standard_False;
      ChangeNode (theI1p1, theI1p2,
                  theNod11Indices, theNod11RValues,
                  theNod12Indices, theNod12RValues,
                  theCoef3, theX3, theY3, theZ3, Standard_False,
                  *theTData1, *thePISeg1, *thePINod1);
      theX2   = theX3;
      theY2   = theY3;
      theZ2   = theZ3;
      theXTI2 = theXTI3;
      theYTI2 = theYTI3;
      theZTI2 = theZTI3;
      theU2   = theU3;
      theNod12RValues.Point = gp_XYZ (theX3, theY3, theZ3);
      if      (theNod12Indices.Edg1 == theIEdge) { theNod12RValues.PCu1 = theU3; }
      else if (theNod12Indices.Edg2 == theIEdge) { theNod12RValues.PCu2 = theU3; }
#ifdef OCCT_DEBUG
      else
      {
        std::cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
        std::cout << "Parameter error on Node " << theI1p2 << std::endl;
      }
#endif
      theNod12RValues.Scal  = 0;
      theNod12Indices.Flag |= NMsk_OutL;
      UpdateAroundNode (theI1p2, theNod12Indices, *theTData1, *thePISeg1, *thePINod1);
    }
  }
  if (anIns3)                                        // p1 i1p3 p2
  {
    Standard_Integer anI1p3 = thePid1->AddNode (theNod11RValues, theNod12RValues, thePINod1, aPINod2, theCoef3, theX3, theY3, theZ3);
    const Handle(HLRAlgo_PolyInternalNode)& aPi1p3 = thePINod1->ChangeValue  (anI1p3);
    HLRAlgo_PolyInternalNode::NodeIndices& aNod13Indices = aPi1p3->Indices();
    HLRAlgo_PolyInternalNode::NodeData& aNod13RValues = aPi1p3->Data();
    aNod13Indices.Edg1  = theIEdge;
    aNod13RValues.PCu1  = theU3;
    aNod13RValues.Scal  = 0;
    aNod13Indices.Flag |= NMsk_OutL;
    aNod13Indices.Flag |= NMsk_Edge;
    thePid1->UpdateLinks (theI1p1, theI1p2, anI1p3,
                          theTData1, aTData2, thePISeg1, aPISeg2, thePINod1, aPINod2);
    UpdateAroundNode (anI1p3, aNod13Indices, *theTData1, *thePISeg1, *thePINod1);
    theList.Prepend (HLRAlgo_BiPoint (theXTI1, theYTI1, theZTI1, theXTI3, theYTI3, theZTI3,
                                      theX1  , theY1  , theZ1  , theX3  , theY3  , theZ3  ,   theIEdge,
                                      theI1  , theI1p1, anI1p3, theFlag));
    theList.Prepend (HLRAlgo_BiPoint (theXTI3, theYTI3, theZTI3, theXTI2, theYTI2, theZTI2,
                                      theX3  , theY3  , theZ3  , theX2  , theY2  , theZ2  ,   theIEdge,
                                      theI1  , anI1p3, theI1p2, theFlag));
  }
  else                                             // p1 p2
  {
    theList.Prepend (HLRAlgo_BiPoint (theXTI1, theYTI1, theZTI1, theXTI2, theYTI2, theZTI2,
                                      theX1  , theY1  , theZ1  , theX2  , theY2  , theZ2  ,   theIEdge,
                                      theI1  , theI1p1, theI1p2, theFlag));
  }
}

//=======================================================================
//function : MoveOrInsertPoint
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::MoveOrInsertPoint (HLRAlgo_ListOfBPoint& theList,
                                          Standard_Real& theX1,
                                          Standard_Real& theY1,
                                          Standard_Real& theZ1,
                                          Standard_Real& theX2,
                                          Standard_Real& theY2,
                                          Standard_Real& theZ2,
                                          Standard_Real& theXTI1,
                                          Standard_Real& theYTI1,
                                          Standard_Real& theZTI1,
                                          Standard_Real& theXTI2,
                                          Standard_Real& theYTI2,
                                          Standard_Real& theZTI2,
                                          const Standard_Integer theIEdge,
                                          Standard_Real& theU1,
                                          Standard_Real& theU2,
                                          HLRAlgo_PolyInternalNode::NodeIndices& theNod11Indices,
                                          HLRAlgo_PolyInternalNode::NodeData& theNod11RValues,
                                          HLRAlgo_PolyInternalNode::NodeIndices& theNod12Indices,
                                          HLRAlgo_PolyInternalNode::NodeData& theNod12RValues,
                                          const Standard_Integer theI1p1,
                                          const Standard_Integer theI1p2,
                                          const Standard_Integer theI1,
                                          const Handle(HLRAlgo_PolyInternalData)& thePid1,
                                          HLRAlgo_Array1OfTData*& theTData1,
                                          HLRAlgo_Array1OfPISeg*& thePISeg1,
                                          HLRAlgo_Array1OfPINod*& thePINod1,
                                          HLRAlgo_PolyInternalNode::NodeIndices& theNod21Indices,
                                          HLRAlgo_PolyInternalNode::NodeData& theNod21RValues,
                                          HLRAlgo_PolyInternalNode::NodeIndices& theNod22Indices,
                                          HLRAlgo_PolyInternalNode::NodeData& theNod22RValues,
                                          const Standard_Integer theI2p1,
                                          const Standard_Integer theI2p2,
                                          const Standard_Integer theI2,
                                          const Handle(HLRAlgo_PolyInternalData)& thePid2,
                                          HLRAlgo_Array1OfTData*& theTData2,
                                          HLRAlgo_Array1OfPISeg*& thePISeg2,
                                          HLRAlgo_Array1OfPINod*& thePINod2,
                                          const Standard_Real theX3,
                                          const Standard_Real theY3,
                                          const Standard_Real theZ3,
                                          const Standard_Real theXTI3,
                                          const Standard_Real theYTI3,
                                          const Standard_Real theZTI3,
                                          const Standard_Real theCoef3,
                                          const Standard_Real theU3,
                                          const Standard_Boolean theInsP3,
                                          const Standard_Boolean themP3P1,
                                          const Standard_Integer theFlag) const
{
  Standard_Boolean anIns3 = theInsP3;
  if (anIns3 && themP3P1)                              // P1 ---> P3
  {
    if (!(theNod11Indices.Flag & NMsk_Vert) && theCoef3 < myTolSta)
    {
      anIns3 = Standard_False;
      ChangeNode (theI1p1, theI1p2,
                  theNod11Indices, theNod11RValues,
                  theNod12Indices, theNod12RValues,
                  theCoef3, theX3, theY3, theZ3, Standard_True,
                  *theTData1, *thePISeg1, *thePINod1);
      ChangeNode (theI2p1, theI2p2,
                  theNod21Indices, theNod21RValues,
                  theNod22Indices, theNod22RValues,
                  theCoef3, theX3, theY3, theZ3, Standard_True,
                  *theTData2, *thePISeg2, *thePINod2);
      theX1   = theX3;
      theY1   = theY3;
      theZ1   = theZ3;
      theXTI1 = theXTI3;
      theYTI1 = theYTI3;
      theZTI1 = theZTI3;
      theU1   = theU3;
      theNod11RValues.Point = gp_XYZ (theX3, theY3, theZ3);
      if      (theNod11Indices.Edg1 == theIEdge) { theNod11RValues.PCu1 = theU3; }
      else if (theNod11Indices.Edg2 == theIEdge) { theNod11RValues.PCu2 = theU3; }
#ifdef OCCT_DEBUG
      else
      {
        std::cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
        std::cout << "Parameter error on Node " << theI1p1 << std::endl;
      }
#endif
      theNod11RValues.Scal  = 0;
      theNod11Indices.Flag |= NMsk_OutL;
      UpdateAroundNode (theI1p1, theNod11Indices, *theTData1, *thePISeg1, *thePINod1);
      theNod21RValues.Point = gp_XYZ (theX3, theY3, theZ3);
      if      (theNod21Indices.Edg1 == theIEdge) { theNod21RValues.PCu1 = theU3; }
      else if (theNod21Indices.Edg2 == theIEdge) { theNod21RValues.PCu2 = theU3; }
#ifdef OCCT_DEBUG
      else
      {
        std::cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
        std::cout << "Parameter error on Node " << theI2p1 << std::endl;
      }
#endif
      theNod21RValues.Scal  = 0;
      theNod21Indices.Flag |= NMsk_OutL;
      UpdateAroundNode (theI2p1, theNod21Indices, *theTData2, *thePISeg2, *thePINod2);
      HLRAlgo_BiPoint::PointsT& aPoints = theList.First().Points();
      aPoints.PntP2 = gp_XYZ(theX3, theY3, theZ3);
      aPoints.Pnt2 = gp_XYZ(theXTI3, theYTI3, theZTI3);
    }
  }
  if (anIns3 && !themP3P1)                             // P2 ---> P3
  {
    if (!(theNod12Indices.Flag & NMsk_Vert) && theCoef3 > myTolEnd)
    {
      anIns3 = Standard_False;
      ChangeNode (theI1p1, theI1p2,
                  theNod11Indices, theNod11RValues,
                  theNod12Indices, theNod12RValues,
                  theCoef3, theX3, theY3, theZ3, Standard_False,
                  *theTData1, *thePISeg1, *thePINod1);
      ChangeNode (theI2p1, theI2p2,
                  theNod21Indices, theNod21RValues,
                  theNod22Indices, theNod22RValues,
                  theCoef3, theX3, theY3, theZ3, Standard_False,
                  *theTData2, *thePISeg2, *thePINod2);
      theX2   = theX3;
      theY2   = theY3;
      theZ2   = theZ3;
      theXTI2 = theXTI3;
      theYTI2 = theYTI3;
      theZTI2 = theZTI3;
      theU2   = theU3;
      theNod12RValues.Point = gp_XYZ(theX3, theY3, theZ3);
      if      (theNod12Indices.Edg1 == theIEdge) { theNod12RValues.PCu1 = theU3; }
      else if (theNod12Indices.Edg2 == theIEdge) { theNod12RValues.PCu2 = theU3; }
#ifdef OCCT_DEBUG
      else
      {
        std::cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
        std::cout << "Parameter error on Node " << theI1p2 << std::endl;
      }
#endif
      theNod12RValues.Scal  = 0;
      theNod12Indices.Flag |= NMsk_OutL;
      UpdateAroundNode (theI1p2, theNod12Indices, *theTData1, *thePISeg1, *thePINod1);
      theNod22RValues.Point = gp_XYZ (theX3, theY3, theZ3);
      if      (theNod22Indices.Edg1 == theIEdge) { theNod22RValues.PCu1 = theU3; }
      else if (theNod22Indices.Edg2 == theIEdge) { theNod22RValues.PCu2 = theU3; }
#ifdef OCCT_DEBUG
      else
      {
        std::cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
        std::cout << "Parameter error on Node " << theI2p2 << std::endl;
      }
#endif
      theNod22RValues.Scal = 0;
      theNod22Indices.Flag |=  NMsk_OutL;
      UpdateAroundNode (theI2p2, theNod22Indices, *theTData2, *thePISeg2, *thePINod2);
    }
  }
  if (anIns3)                                       // p1 i1p3 p2
  {
    Standard_Integer anI1p3 = thePid1->AddNode (theNod11RValues, theNod12RValues, thePINod1, thePINod2, theCoef3, theX3, theY3, theZ3);
    Standard_Integer anI2p3 = thePid2->AddNode (theNod21RValues, theNod22RValues, thePINod2, thePINod1, theCoef3, theX3, theY3, theZ3);
    const Handle(HLRAlgo_PolyInternalNode)& aPi1p3 = thePINod1->ChangeValue (anI1p3);
    HLRAlgo_PolyInternalNode::NodeIndices& aNod13Indices = aPi1p3->Indices();
    HLRAlgo_PolyInternalNode::NodeData& aNod13RValues = aPi1p3->Data();
    const Handle(HLRAlgo_PolyInternalNode)& aPi2p3 = thePINod2->ChangeValue (anI2p3);
    HLRAlgo_PolyInternalNode::NodeIndices& aNod23Indices = aPi2p3->Indices();
    HLRAlgo_PolyInternalNode::NodeData& aNod23RValues = aPi2p3->Data();
    aNod13Indices.Edg1  = theIEdge;
    aNod13RValues.PCu1  = theU3;
    aNod13RValues.Scal  = 0;
    aNod13Indices.Flag |= NMsk_OutL;
    aNod13Indices.Flag |= NMsk_Edge;
    aNod23Indices.Edg1  = theIEdge;
    aNod23RValues.PCu1  = theU3;
    aNod23RValues.Scal  = 0;
    aNod23Indices.Flag |= NMsk_OutL;
    aNod23Indices.Flag |= NMsk_Edge;
    thePid1->UpdateLinks (theI1p1, theI1p2, anI1p3,
                          theTData1, theTData2, thePISeg1, thePISeg2, thePINod1, thePINod2);
    thePid2->UpdateLinks (theI2p1, theI2p2, anI2p3,
                          theTData2, theTData1, thePISeg2, thePISeg1, thePINod2, thePINod1);
    UpdateAroundNode (anI1p3, aNod13Indices, *theTData1, *thePISeg1, *thePINod1);
    UpdateAroundNode (anI2p3, aNod23Indices, *theTData2, *thePISeg2, *thePINod2);
    theList.Prepend (HLRAlgo_BiPoint (theXTI1, theYTI1, theZTI1, theXTI3, theYTI3, theZTI3,
                                      theX1  , theY1  , theZ1  , theX3  , theY3  , theZ3  ,   theIEdge,
                                      theI1  , theI1p1, anI1p3,  theI2  , theI2p1, anI2p3, theFlag));
    theList.Prepend (HLRAlgo_BiPoint (theXTI3, theYTI3, theZTI3, theXTI2, theYTI2, theZTI2,
                                      theX3  , theY3  , theZ3  , theX2  , theY2  , theZ2  ,   theIEdge,
                                      theI1  , anI1p3,  theI1p2, theI2  , anI2p3, theI2p2, theFlag));
  }
  else                                             // p1 p2
  {
    theList.Prepend (HLRAlgo_BiPoint (theXTI1, theYTI1, theZTI1, theXTI2, theYTI2, theZTI2,
                                      theX1  , theY1  , theZ1  , theX2  , theY2  , theZ2  ,   theIEdge,
                                      theI1  , theI1p1, theI1p2, theI2  , theI2p1, theI2p2, theFlag));
  }
}

//=======================================================================
//function : MoveOrInsertPoint
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::MoveOrInsertPoint (HLRAlgo_ListOfBPoint& theList,
                                          Standard_Real& theX1,
                                          Standard_Real& theY1,
                                          Standard_Real& theZ1,
                                          Standard_Real& theX2,
                                          Standard_Real& theY2,
                                          Standard_Real& theZ2,
                                          Standard_Real& theXTI1,
                                          Standard_Real& theYTI1,
                                          Standard_Real& theZTI1,
                                          Standard_Real& theXTI2,
                                          Standard_Real& theYTI2,
                                          Standard_Real& theZTI2,
                                          const Standard_Integer theIEdge,
                                          Standard_Real& theU1,
                                          Standard_Real& theU2,
                                          HLRAlgo_PolyInternalNode::NodeIndices& theNod11Indices,
                                          HLRAlgo_PolyInternalNode::NodeData& theNod11RValues,
                                          HLRAlgo_PolyInternalNode::NodeIndices& theNod12Indices,
                                          HLRAlgo_PolyInternalNode::NodeData& theNod12RValues,
                                          const Standard_Integer theI1p1,
                                          const Standard_Integer theI1p2,
                                          const Standard_Integer theI1,
                                          const Handle(HLRAlgo_PolyInternalData)& thePid1,
                                          HLRAlgo_Array1OfTData*& theTData1,
                                          HLRAlgo_Array1OfPISeg*& thePISeg1,
                                          HLRAlgo_Array1OfPINod*& thePINod1,
                                          HLRAlgo_PolyInternalNode::NodeIndices& theNod21Indices,
                                          HLRAlgo_PolyInternalNode::NodeData& theNod21RValues,
                                          HLRAlgo_PolyInternalNode::NodeIndices& theNod22Indices,
                                          HLRAlgo_PolyInternalNode::NodeData& theNod22RValues,
                                          const Standard_Integer theI2p1,
                                          const Standard_Integer theI2p2,
                                          const Standard_Integer theI2,
                                          const Handle(HLRAlgo_PolyInternalData)& thePid2,
                                          HLRAlgo_Array1OfTData*& theTData2,
                                          HLRAlgo_Array1OfPISeg*& thePISeg2,
                                          HLRAlgo_Array1OfPINod*& thePINod2,
                                          const Standard_Real theX3,
                                          const Standard_Real theY3,
                                          const Standard_Real theZ3,
                                          const Standard_Real theXTI3,
                                          const Standard_Real theYTI3,
                                          const Standard_Real theZTI3,
                                          const Standard_Real theCoef3,
                                          const Standard_Real theU3,
                                          const Standard_Boolean theInsP3,
                                          const Standard_Boolean themP3P1,
                                          const Standard_Real theX4,
                                          const Standard_Real theY4,
                                          const Standard_Real theZ4,
                                          const Standard_Real theXTI4,
                                          const Standard_Real theYTI4,
                                          const Standard_Real theZTI4,
                                          const Standard_Real theCoef4,
                                          const Standard_Real theU4,
                                          const Standard_Boolean theInsP4,
                                          const Standard_Boolean themP4P1,
                                          const Standard_Integer theFlag) const
{
  Standard_Boolean anIns3 = theInsP3;
  Standard_Boolean anIns4 = theInsP4;
  if (anIns3 && themP3P1)                              // P1 ---> P3
  {
    if (!(theNod11Indices.Flag & NMsk_Vert) && theCoef3 < myTolSta)
    {
      anIns3 = Standard_False;
      ChangeNode (theI1p1, theI1p2,
                  theNod11Indices, theNod11RValues,
                  theNod12Indices, theNod12RValues,
                  theCoef3, theX3, theY3, theZ3, Standard_True,
                  *theTData1, *thePISeg1, *thePINod1);
      ChangeNode (theI2p1, theI2p2,
                  theNod21Indices, theNod21RValues,
                  theNod22Indices, theNod22RValues,
                  theCoef3, theX3, theY3, theZ3, Standard_True,
                  *theTData2, *thePISeg2, *thePINod2);
      theX1   = theX3;
      theY1   = theY3;
      theZ1   = theZ3;
      theXTI1 = theXTI3;
      theYTI1 = theYTI3;
      theZTI1 = theZTI3;
      theU1   = theU3;
      theNod11RValues.Point = gp_XYZ (theX3, theY3, theZ3);
      if      (theNod11Indices.Edg1 == theIEdge) { theNod11RValues.PCu1 = theU3; }
      else if (theNod11Indices.Edg2 == theIEdge) { theNod11RValues.PCu2 = theU3; }
#ifdef OCCT_DEBUG
      else
      {
        std::cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
        std::cout << "Parameter error on Node " << theI1p1 << std::endl;
      }
#endif
      theNod11RValues.Scal  = 0;
      theNod11Indices.Flag |= NMsk_OutL;
      UpdateAroundNode (theI1p1, theNod11Indices, *theTData1, *thePISeg1, *thePINod1);
      theNod21RValues.Point = gp_XYZ (theX3, theY3, theZ3);
      if      (theNod21Indices.Edg1 == theIEdge) { theNod21RValues.PCu1 = theU3; }
      else if (theNod21Indices.Edg2 == theIEdge) { theNod21RValues.PCu2 = theU3; }
#ifdef OCCT_DEBUG
      else
      {
        std::cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
        std::cout << "Parameter error on Node " << theI2p1 << std::endl;
      }
#endif
      theNod21RValues.Scal  = 0;
      theNod21Indices.Flag |= NMsk_OutL;
      UpdateAroundNode (theI2p1, theNod21Indices, *theTData2, *thePISeg2, *thePINod2);
      HLRAlgo_BiPoint::PointsT& aPoints = theList.First().Points();
      aPoints.PntP2 = gp_XYZ (theX3, theY3, theZ3);
      aPoints.Pnt2 = gp_XYZ (theXTI3, theYTI3, theZTI3);
    }
  }
  if (anIns4 && !themP4P1)                             // P2 ---> P4
  {
    if (!(theNod12Indices.Flag & NMsk_Vert) && theCoef4 > myTolEnd)
    {
      anIns4 = Standard_False;
      ChangeNode (theI2p1, theI2p2,
                  theNod21Indices, theNod21RValues,
                  theNod22Indices, theNod22RValues,
                  theCoef4, theX4, theY4, theZ4, Standard_False,
                  *theTData2, *thePISeg2, *thePINod2);
      ChangeNode (theI1p1, theI1p2,
                  theNod11Indices, theNod11RValues,
                  theNod12Indices, theNod12RValues,
                  theCoef4, theX4, theY4, theZ4, Standard_False,
                  *theTData1, *thePISeg1, *thePINod1);
      theX2   = theX4;
      theY2   = theY4;
      theZ2   = theZ4;
      theXTI2 = theXTI4;
      theYTI2 = theYTI4;
      theZTI2 = theZTI4;
      theU2   = theU4;
      theNod12RValues.Point = gp_XYZ (theX4, theY4, theZ4);
      if      (theNod12Indices.Edg1 == theIEdge) { theNod12RValues.PCu1 = theU4; }
      else if (theNod12Indices.Edg2 == theIEdge) { theNod12RValues.PCu2 = theU4; }
#ifdef OCCT_DEBUG
      else
      {
        std::cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
        std::cout << "Parameter error on Node " << theI1p2 << std::endl;
      }
#endif
      theNod12RValues.Scal  = 0;
      theNod12Indices.Flag |= NMsk_OutL;
      UpdateAroundNode (theI1p2, theNod12Indices, *theTData1, *thePISeg1, *thePINod1);
      theNod22RValues.Point = gp_XYZ (theX4, theY4, theZ4);
      if      (theNod22Indices.Edg1 == theIEdge) { theNod22RValues.PCu1 = theU4; }
      else if (theNod22Indices.Edg2 == theIEdge) { theNod22RValues.PCu2 = theU4; }
#ifdef OCCT_DEBUG
      else
      {
        std::cout << " HLRBRep_PolyAlgo::MoveOrInsertPoint : ";
        std::cout << "Parameter error on Node " << theI2p2 << std::endl;
      }
#endif
      theNod22RValues.Scal  = 0;
      theNod22Indices.Flag |= NMsk_OutL;
      UpdateAroundNode (theI2p2, theNod22Indices, *theTData2, *thePISeg2, *thePINod2);
    }
  }
  if (anIns3 || anIns4)
  {
    if      (!anIns4)                                // p1 i1p3 p2
    {
      MoveOrInsertPoint (theList,
                         theX1  , theY1  , theZ1  , theX2  , theY2  , theZ2  ,
                         theXTI1, theYTI1, theZTI1, theXTI2, theYTI2, theZTI2,
                         theIEdge, theU1, theU2,
                         theNod11Indices, theNod11RValues,
                         theNod12Indices, theNod12RValues,
                         theI1p1, theI1p2, theI1, thePid1, theTData1, thePISeg1, thePINod1,
                         theNod21Indices, theNod21RValues,
                         theNod22Indices, theNod22RValues,
                         theI2p1, theI2p2, theI2, thePid2, theTData2, thePISeg2, thePINod2,
                         theX3, theY3, theZ3, theXTI3, theYTI3, theZTI3, theCoef3, theU3, theInsP3, themP3P1, theFlag);
    }
    else if (!anIns3)                                // p1 i2p4 p2
    {
      MoveOrInsertPoint (theList,
                         theX1  , theY1  , theZ1  , theX2  , theY2  , theZ2  ,
                         theXTI1, theYTI1, theZTI1, theXTI2, theYTI2, theZTI2,
                         theIEdge, theU1, theU2,
                         theNod21Indices, theNod21RValues,
                         theNod22Indices, theNod22RValues,
                         theI2p1, theI2p2, theI2, thePid2, theTData2, thePISeg2, thePINod2,
                         theNod11Indices, theNod11RValues,
                         theNod12Indices, theNod12RValues,
                         theI1p1, theI1p2, theI1, thePid1, theTData1, thePISeg1, thePINod1,
                         theX4, theY4, theZ4, theXTI4, theYTI4, theZTI4, theCoef4, theU4, theInsP4, themP4P1, theFlag);
    }
    else                                          // p1 i1p3 i2p4 p2
    {
      Standard_Integer anI1p3 = thePid1->AddNode (theNod11RValues, theNod12RValues, thePINod1, thePINod2, theCoef3, theX3, theY3, theZ3);
      Standard_Integer anI2p3 = thePid2->AddNode (theNod21RValues, theNod22RValues, thePINod2, thePINod1, theCoef3, theX3, theY3, theZ3);
      Standard_Integer anI1p4 = thePid1->AddNode (theNod11RValues, theNod12RValues, thePINod1, thePINod2, theCoef4, theX4, theY4, theZ4);
      Standard_Integer anI2p4 = thePid2->AddNode (theNod21RValues, theNod22RValues, thePINod2, thePINod1, theCoef4, theX4, theY4, theZ4);
      const Handle(HLRAlgo_PolyInternalNode)& aPi1p3 = thePINod1->ChangeValue (anI1p3);
      HLRAlgo_PolyInternalNode::NodeIndices& aNod13Indices = aPi1p3->Indices();
      HLRAlgo_PolyInternalNode::NodeData& aNod13RValues = aPi1p3->Data();
      const Handle(HLRAlgo_PolyInternalNode)& aPi1p4 = thePINod1->ChangeValue (anI1p4);
      HLRAlgo_PolyInternalNode::NodeIndices& aNod14Indices = aPi1p4->Indices();
      HLRAlgo_PolyInternalNode::NodeData& aNod14RValues = aPi1p4->Data();
      const Handle(HLRAlgo_PolyInternalNode)& aPi2p3 = thePINod2->ChangeValue (anI2p3);
      HLRAlgo_PolyInternalNode::NodeIndices& aNod23Indices = aPi2p3->Indices();
      HLRAlgo_PolyInternalNode::NodeData& aNod23RValues = aPi2p3->Data();
      const Handle(HLRAlgo_PolyInternalNode)& aPi2p4 =  thePINod2->ChangeValue (anI2p4);
      HLRAlgo_PolyInternalNode::NodeIndices& aNod24Indices = aPi2p4->Indices();
      HLRAlgo_PolyInternalNode::NodeData& aNod24RValues = aPi2p4->Data();
      aNod13Indices.Edg1  = theIEdge;
      aNod13RValues.PCu1  = theU3;
      aNod13RValues.Scal  = 0;
      aNod13Indices.Flag |= NMsk_OutL;
      aNod13Indices.Flag |= NMsk_Edge;
      aNod23Indices.Edg1  = theIEdge;
      aNod23RValues.PCu1  = theU3;
      aNod23RValues.Scal  = 0;
      aNod23Indices.Flag |= NMsk_OutL;
      aNod23Indices.Flag |= NMsk_Edge;
      aNod14Indices.Edg1  = theIEdge;
      aNod14RValues.PCu1  = theU4;
      aNod14RValues.Scal  = 0;
      aNod14Indices.Flag |= NMsk_OutL;
      aNod14Indices.Flag |= NMsk_Edge;
      aNod24Indices.Edg1  = theIEdge;
      aNod24RValues.PCu1  = theU4;
      aNod24RValues.Scal  = 0;
      aNod24Indices.Flag |= NMsk_OutL;
      aNod24Indices.Flag |= NMsk_Edge;
      thePid1->UpdateLinks (theI1p1, theI1p2, anI1p3,
                            theTData1, theTData2, thePISeg1, thePISeg2, thePINod1, thePINod2);
      thePid2->UpdateLinks (theI2p1, theI2p2, anI2p3,
                            theTData2, theTData1, thePISeg2, thePISeg1, thePINod2, thePINod1);
      thePid2->UpdateLinks (anI2p3, theI2p2, anI2p4,
                            theTData2, theTData1, thePISeg2, thePISeg1, thePINod2, thePINod1);
      thePid1->UpdateLinks (anI1p3, theI1p2, anI1p4,
                            theTData1, theTData2, thePISeg1, thePISeg2, thePINod1, thePINod2);
      UpdateAroundNode (anI1p3, aNod13Indices, *theTData1, *thePISeg1, *thePINod1);
      UpdateAroundNode (anI2p3, aNod23Indices, *theTData2, *thePISeg2, *thePINod2);
      UpdateAroundNode (anI1p4, aNod14Indices, *theTData1, *thePISeg1, *thePINod1);
      UpdateAroundNode (anI2p4, aNod24Indices, *theTData2, *thePISeg2, *thePINod2);
      theList.Prepend (HLRAlgo_BiPoint (theXTI1, theYTI1, theZTI1, theXTI3, theYTI3, theZTI3,
                                        theX1  , theY1  , theZ1  , theX3  , theY3  , theZ3  , theIEdge,
                                        theI1  , theI1p1, anI1p3,  theI2  , theI2p1, anI2p3,  theFlag));
      theList.Prepend (HLRAlgo_BiPoint (theXTI3, theYTI3, theZTI3, theXTI4, theYTI4, theZTI4,
                                        theX3  , theY3  , theZ3  , theX4  , theY4  , theZ4  , theIEdge,
                                        theI1  , anI1p3,  anI1p4,  theI2,   anI2p3,  anI2p4,  theFlag));
      theList.Prepend (HLRAlgo_BiPoint (theXTI4, theYTI4, theZTI4, theXTI2, theYTI2, theZTI2,
                                        theX4  , theY4  , theZ4  , theX2  , theY2  , theZ2  , theIEdge,
                                        theI1  , anI1p4,  theI1p2, theI2  , anI2p4,  theI2p2, theFlag));
    }
  }
  else                                             // p1 p2
  {
    theList.Prepend (HLRAlgo_BiPoint (theXTI1, theYTI1, theZTI1, theXTI2, theYTI2, theZTI2,
                                      theX1  , theY1  , theZ1  , theX2  , theY2  , theZ2  , theIEdge,
                                      theI1  , theI1p1, theI1p2, theI2  , theI2p1, theI2p2, theFlag));
  }
}

//=======================================================================
//function : InsertOnOutLine
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::InsertOnOutLine (NCollection_Array1<Handle(HLRAlgo_PolyInternalData)>& thePID)
{
  HLRAlgo_Array1OfTData* aTData2 = NULL;
  HLRAlgo_Array1OfPISeg* aPISeg2 = NULL;
  HLRAlgo_Array1OfPINod* aPINod2 = NULL;

  TopLoc_Location aLoc;
  Standard_Boolean mP3P1 = false;
  Standard_Real aU3, aV3, aCoef3, X3 = 0., Y3 = 0., Z3 = 0.;

  const gp_Trsf& aProjTrsf = myProj.Transformation();

  const Standard_Integer aNbFaces = myFMap.Extent();
  for (Standard_Integer aFaceIter = 1; aFaceIter <= aNbFaces; ++aFaceIter)
  {
    const Handle(HLRAlgo_PolyInternalData)& aPid = thePID.ChangeValue (aFaceIter);
    if (aPid.IsNull())
    {
      continue;
    }

    bool isIntOutL = Standard_False;
    HLRAlgo_Array1OfTData* aTData1 = &aPid->TData();
    HLRAlgo_Array1OfPISeg* aPISeg1 = &aPid->PISeg();
    HLRAlgo_Array1OfPINod* aPINod1 = &aPid->PINod();
    TopoDS_Shape aLocalShape = myFMap (aFaceIter);
    const TopoDS_Face& aFace = TopoDS::Face (aLocalShape);
    myBSurf.Initialize (aFace, Standard_False);
    myGSurf = BRep_Tool::Surface (aFace, aLoc);
    {
      gp_Trsf aTT = aLoc.Transformation();
      aTT.PreMultiply (aProjTrsf);
      const gp_XYZ& aTTrsfVec = aTT.TranslationPart();
      TTLo[0] = aTTrsfVec.X();
      TTLo[1] = aTTrsfVec.Y();
      TTLo[2] = aTTrsfVec.Z();
      const gp_Mat& aTTrsfMat = aTT.VectorialPart();
      TTMa[0][0] = aTTrsfMat.Value (1, 1);
      TTMa[0][1] = aTTrsfMat.Value (1, 2);
      TTMa[0][2] = aTTrsfMat.Value (1, 3);
      TTMa[1][0] = aTTrsfMat.Value (2, 1);
      TTMa[1][1] = aTTrsfMat.Value (2, 2);
      TTMa[1][2] = aTTrsfMat.Value (2, 3);
      TTMa[2][0] = aTTrsfMat.Value (3, 1);
      TTMa[2][1] = aTTrsfMat.Value (3, 2);
      TTMa[2][2] = aTTrsfMat.Value (3, 3);
    }

#ifdef OCCT_DEBUG
    if (DoTrace)
    {
      std::cout << " InsertOnOutLine : NbTData " << aPid->NbTData() << std::endl;
      std::cout << " InsertOnOutLine : NbPISeg " << aPid->NbPISeg() << std::endl;
      std::cout << " InsertOnOutLine : NbPINod " << aPid->NbPINod() << std::endl;
    }
#endif

    const Standard_Integer aNbSegs = aPid->NbPISeg();
    for (Standard_Integer aSegIter = 1; aSegIter <= aNbSegs; ++aSegIter)
    {
      HLRAlgo_PolyInternalSegment& aSegIndices = aPISeg1->ChangeValue (aSegIter);
      //	Standard_Boolean Cutted = Standard_False;
      if (aSegIndices.Conex1 != 0 && aSegIndices.Conex2 != 0)
      {
        const Standard_Integer ip1 = aSegIndices.LstSg1;
        const Standard_Integer ip2 = aSegIndices.LstSg2;
        const Handle(HLRAlgo_PolyInternalNode)& aPip1 = aPINod1->ChangeValue (ip1);
        HLRAlgo_PolyInternalNode::NodeIndices& aNod1Indices = aPip1->Indices();
        HLRAlgo_PolyInternalNode::NodeData& aNod1RValues = aPip1->Data();
        const Handle(HLRAlgo_PolyInternalNode)& aPip2 = aPINod1->ChangeValue (ip2);
        HLRAlgo_PolyInternalNode::NodeIndices& aNod2Indices = aPip2->Indices();
        HLRAlgo_PolyInternalNode::NodeData& aNod2RValues = aPip2->Data();
        if (aNod1Indices.Flag & NMsk_OutL && aNod2Indices.Flag & NMsk_OutL)
        {
          isIntOutL = Standard_True;
        }
        else if ((aNod1RValues.Scal >=  myTolAngular &&
                  aNod2RValues.Scal <= -myTolAngular) ||
                  (aNod2RValues.Scal >=  myTolAngular &&
                  aNod1RValues.Scal <= -myTolAngular))
        {
          isIntOutL = Standard_True;
          bool isInsP3 = NewNode (aNod1RValues, aNod2RValues, aCoef3, mP3P1);
          if (isInsP3)
          {
            UVNode (aNod1RValues, aNod2RValues, aCoef3, aU3, aV3);
            if (!myGSurf.IsNull())
            {
              const gp_Pnt aPT3 = myGSurf->Value (aU3, aV3);
              X3 = aPT3.X();
              Y3 = aPT3.Y();
              Z3 = aPT3.Z();
            }
            else
            {
              // simple averaging - this could be improved
              const Standard_Real aCoef2 = 1.0 - aCoef3;
              const gp_Pnt aPT3 = aCoef2 * aNod1RValues.Point + aCoef3 * aNod2RValues.Point;
              X3 = aPT3.X();
              Y3 = aPT3.Y();
              Z3 = aPT3.Z();
            }
            TTMultiply (X3, Y3, Z3);
          }

          if (isInsP3 && mP3P1)                         // P1 ---> P3
          {
            if ((aNod1Indices.Flag & NMsk_Edge) == 0 && aCoef3 < myTolSta)
            {
              isInsP3 = Standard_False;
              ChangeNode (ip1, ip2,
                          aNod1Indices, aNod1RValues,
                          aNod2Indices, aNod2RValues,
                          aCoef3, X3, Y3, Z3, Standard_True,
                          *aTData1, *aPISeg1, *aPINod1);
              aNod1RValues.Scal  = 0;
              aNod1Indices.Flag |= NMsk_OutL;
            }
          }
          if (isInsP3 && !mP3P1)                        // P2 ---> P3
          {
            if ((aNod2Indices.Flag & NMsk_Edge) == 0 && aCoef3 > myTolEnd)
            {
              isInsP3 = Standard_False;
              ChangeNode (ip1, ip2,
                          aNod1Indices, aNod1RValues,
                          aNod2Indices, aNod2RValues,
                          aCoef3, X3, Y3, Z3, Standard_False,
                          *aTData1, *aPISeg1, *aPINod1);
              aNod2RValues.Scal  = 0;
              aNod2Indices.Flag |= NMsk_OutL;
            }
          }
          if (isInsP3)                                  // p1 ip3 p2
          {
            const Standard_Integer ip3 = aPid->AddNode (aNod1RValues, aNod2RValues, aPINod1, aPINod2,
                                                        aCoef3, X3, Y3, Z3);
            const Handle(HLRAlgo_PolyInternalNode)& aPip3 = aPINod1->ChangeValue(ip3);
            HLRAlgo_PolyInternalNode::NodeIndices& aNod3Indices = aPip3->Indices();
            HLRAlgo_PolyInternalNode::NodeData& aNod3RValues = aPip3->Data();
            aPid->UpdateLinks (ip1, ip2, ip3,
                               aTData1, aTData2, aPISeg1, aPISeg2, aPINod1, aPINod2);
            UpdateAroundNode (ip3, aNod3Indices, *aTData1, *aPISeg1, *aPINod1);
            aNod3RValues.Scal  = 0;
            aNod3Indices.Flag |= NMsk_OutL;
          }
        }
      }
    }
    if (isIntOutL)
    {
      aPid->IntOutL (Standard_True);
    }

#ifdef OCCT_DEBUG
    if (DoTrace)
    {
      std::cout << " InsertOnOutLine : NbTData " << aPid->NbTData() << std::endl;
      std::cout << " InsertOnOutLine : NbPISeg " << aPid->NbPISeg() << std::endl;
      std::cout << " InsertOnOutLine : NbPINod " << aPid->NbPINod() << std::endl;
    }
#endif
  }
}

//=======================================================================
//function : CheckFrBackTriangles
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::CheckFrBackTriangles (HLRAlgo_ListOfBPoint& theList,
                                             NCollection_Array1<Handle(HLRAlgo_PolyInternalData)>& thePID)
{
  Standard_Real X1 =0.,Y1 =0.,X2 =0.,Y2 =0.,X3 =0.,Y3 =0.;
  Standard_Real D1,D2,D3;
  Standard_Real dd,dX,dY,nX,nY;

  HLRAlgo_Array1OfTData* aTData1 = NULL;
  HLRAlgo_Array1OfPISeg* aPISeg1 = NULL;
  HLRAlgo_Array1OfPINod* aPINod1 = NULL;
  HLRAlgo_Array1OfTData* aTData2 = NULL;
  HLRAlgo_Array1OfPISeg* aPISeg2 = NULL;
  HLRAlgo_Array1OfPINod* aPINod2 = NULL;

  const Standard_Integer aNbFaces = myFMap.Extent();
  Standard_Boolean isModif = Standard_True;
  Standard_Integer iLoop = 0;
  
  while (isModif && iLoop < 4)
  {
    iLoop++;
    isModif = false;
    bool isFrBackInList = false;
    for (Standard_Integer aFaceIter = 1; aFaceIter <= aNbFaces; ++aFaceIter)
    {
      const Handle(HLRAlgo_PolyInternalData)& pid = thePID.ChangeValue (aFaceIter);
      if (pid.IsNull())
      {
        continue;
      }

      const Standard_Integer aNbTris = pid->NbTData();
      HLRAlgo_Array1OfTData* aTData = &pid->TData();
      HLRAlgo_Array1OfPISeg* aPISeg = &pid->PISeg();
      HLRAlgo_Array1OfPINod* aPINod = &pid->PINod();
      for (Standard_Integer aTriIter = 1; aTriIter <= aNbTris; ++aTriIter)
      {
        HLRAlgo_TriangleData* tdata = &aTData->ChangeValue (aTriIter);
        if ((tdata->Flags & HLRAlgo_PolyMask_FMskSide) == 0 &&
            (tdata->Flags & HLRAlgo_PolyMask_FMskFrBack))
        {
        #ifdef OCCT_DEBUG
          if (DoTrace)
          {
            std::cout << " face : " << aFaceIter << " , triangle " << aTriIter << std::endl;
          }
        #endif
          isModif = Standard_True;
          const Handle(HLRAlgo_PolyInternalNode)* pi1p1 = &aPINod->ChangeValue (tdata->Node1);
          HLRAlgo_PolyInternalNode::NodeIndices* aNod11Indices = &(*pi1p1)->Indices();
          HLRAlgo_PolyInternalNode::NodeData* aNod11RValues = &(*pi1p1)->Data();
          const Handle(HLRAlgo_PolyInternalNode)* pi1p2 = &aPINod->ChangeValue (tdata->Node2);
          HLRAlgo_PolyInternalNode::NodeIndices* aNod12Indices = &(*pi1p2)->Indices();
          HLRAlgo_PolyInternalNode::NodeData* aNod12RValues = &(*pi1p2)->Data();
          const Handle(HLRAlgo_PolyInternalNode)* pi1p3 = &aPINod->ChangeValue (tdata->Node3);
          HLRAlgo_PolyInternalNode::NodeIndices* aNod13Indices = &(*pi1p3)->Indices();
          HLRAlgo_PolyInternalNode::NodeData* aNod13RValues = &(*pi1p3)->Data();
          D1 = 0.; D2 = 0.; D3 = 0.;
          if (((aNod11Indices->Flag & NMsk_Edge) == 0 || iLoop > 1) &&
              ((aNod11Indices->Flag & NMsk_OutL) == 0 || iLoop > 1) &&
              ((aNod11Indices->Flag & NMsk_Vert) == 0))
          {
            dX = aNod13RValues->Point.X() - aNod12RValues->Point.X();
            dY = aNod13RValues->Point.Y() - aNod12RValues->Point.Y();
            D1 = dX * dX + dY * dY;
            D1 = sqrt(D1);
            nX = - dY / D1; nY =   dX / D1;
            dX = aNod11RValues->Point.X() - aNod12RValues->Point.X();
            dY = aNod11RValues->Point.Y() - aNod12RValues->Point.Y();
            dd = - (dX * nX + dY * nY);
            if (dd < 0) { dd -= D1 * 0.01; }
            else        { dd += D1 * 0.01; }
            X1 = nX * dd; Y1 = nY * dd;
          }
          if (((aNod12Indices->Flag & NMsk_Edge) == 0 || iLoop > 1) &&
              ((aNod12Indices->Flag & NMsk_OutL) == 0 || iLoop > 1) &&
              ((aNod12Indices->Flag & NMsk_Vert) == 0))
          {
            dX = aNod11RValues->Point.X() - aNod13RValues->Point.X();
            dY = aNod11RValues->Point.Y() - aNod13RValues->Point.Y();
            D2 = dX * dX + dY * dY;
            D2 = sqrt(D2);
            nX = - dY / D2; nY =   dX / D2;
            dX = aNod12RValues->Point.X() - aNod13RValues->Point.X();
            dY = aNod12RValues->Point.Y() - aNod13RValues->Point.Y();
            dd = - (dX * nX + dY * nY);
            if (dd < 0) { dd -= D2 * 0.01; }
            else        { dd += D2 * 0.01; }
            X2 = nX * dd; Y2 = nY * dd;
          }
          if (((aNod13Indices->Flag & NMsk_Edge) == 0 || iLoop > 1) &&
              ((aNod13Indices->Flag & NMsk_OutL) == 0 || iLoop > 1) &&
              ((aNod13Indices->Flag & NMsk_Vert) == 0))
          {
            dX = aNod12RValues->Point.X() - aNod11RValues->Point.X();
            dY = aNod12RValues->Point.Y() - aNod11RValues->Point.Y();
            D3 = dX * dX + dY * dY;
            D3 = sqrt(D3);
            nX = - dY / D3; nY =   dX / D3;
            dX = aNod13RValues->Point.X() - aNod11RValues->Point.X();
            dY = aNod13RValues->Point.Y() - aNod11RValues->Point.Y();
            dd = - (dX * nX + dY * nY);
            if (dd < 0) { dd -= D3 * 0.01; }
            else        { dd += D3 * 0.01; }
            X3 = nX * dd; Y3 = nY * dd;
          }
          if      (D1 > D2 && D1 > D3)
          {
            aNod11RValues->Point.ChangeCoord(1) += X1;
            aNod11RValues->Point.ChangeCoord(2) += Y1;
            aNod11Indices->Flag |= NMsk_Move;
            UpdateAroundNode (tdata->Node1, *aNod11Indices, *aTData, *aPISeg, *aPINod);
            isFrBackInList = Standard_True;
          #ifdef OCCT_DEBUG
            if (DoTrace)
            {
              std::cout << tdata->Node1 << " modifies  : DX,DY ";
              std::cout << X1 << " , " << Y1 << std::endl;
            }
          #endif
          }
          else if (D2 > D3 && D2 > D1)
          {
            aNod12RValues->Point.ChangeCoord(1) += X2;
            aNod12RValues->Point.ChangeCoord(2) += Y2;
            aNod12Indices->Flag |= NMsk_Move;
            UpdateAroundNode (tdata->Node2, *aNod12Indices, *aTData, *aPISeg, *aPINod);
            isFrBackInList = Standard_True;
          #ifdef OCCT_DEBUG
            if (DoTrace)
            {
              std::cout << tdata->Node2 << " modifies  : DX,DY ";
              std::cout << X2 << " , " << Y2 << std::endl;
            }
          #endif
          }
          else if (D3 > D1 && D3 > D2)
          {
            aNod13RValues->Point.ChangeCoord(1) += X3;
            aNod13RValues->Point.ChangeCoord(2) += Y3;
            aNod13Indices->Flag |= NMsk_Move;
            UpdateAroundNode (tdata->Node3, *aNod13Indices, *aTData, *aPISeg, *aPINod);
            isFrBackInList = Standard_True;
          #ifdef OCCT_DEBUG
            if (DoTrace)
            {
              std::cout << tdata->Node3 << " modifies  : DX,DY ";
              std::cout << X3 << " , " << Y3 << std::endl;
            }
          #endif
          }
        #ifdef OCCT_DEBUG
          else if (DoTrace)
          {
            std::cout << "modification error" << std::endl;
          }
        #endif
        }
      }
    }
    if (isFrBackInList)
    {
      for (HLRAlgo_ListIteratorOfListOfBPoint aBPointIter (theList); aBPointIter.More(); aBPointIter.Next())
      {
        HLRAlgo_BiPoint& BP = aBPointIter.ChangeValue();
        HLRAlgo_BiPoint::IndicesT& theIndices = BP.Indices();
        if (theIndices.FaceConex1 != 0)
        {
          const Handle(HLRAlgo_PolyInternalData)& pid1 = thePID.Value (theIndices.FaceConex1);
          aTData1 = &pid1->TData();
          aPISeg1 = &pid1->PISeg();
          aPINod1 = &pid1->PINod();
        }
        if (theIndices.FaceConex2 != 0)
        {
          if (theIndices.FaceConex1 == theIndices.FaceConex2)
          {
            aTData2 = aTData1;
            aPISeg2 = aPISeg1;
            aPINod2 = aPINod1;
          }
          else
          {
            const Handle(HLRAlgo_PolyInternalData)& pid2 = thePID.Value (theIndices.FaceConex2);
            aTData2 = &pid2->TData();
            aPISeg2 = &pid2->PISeg();
            aPINod2 = &pid2->PINod();
          }
        }
        if (theIndices.FaceConex1 != 0)
        {
          HLRAlgo_PolyInternalNode::NodeIndices* aNod11Indices = &aPINod1->ChangeValue (theIndices.Face1Pt1)->Indices();
          if (aNod11Indices->Flag & NMsk_Move)
          {
          #ifdef OCCT_DEBUG
            if (DoTrace)
            {
              std::cout << theIndices.Face1Pt1 << " modifies 11" << std::endl;
            }
          #endif
            const HLRAlgo_PolyInternalNode::NodeData& aNod11RValues = aPINod1->Value (theIndices.Face1Pt1)->Data();
            HLRAlgo_BiPoint::PointsT& aPoints = BP.Points();
            aPoints.Pnt1 = aPoints.PntP1 = aNod11RValues.Point;
            TIMultiply (aPoints.Pnt1);
            if (theIndices.FaceConex2 != 0)
            {
              HLRAlgo_PolyInternalNode::NodeIndices& aNod12Indices = aPINod2->ChangeValue (theIndices.Face2Pt1)->Indices();
              HLRAlgo_PolyInternalNode::NodeData& aNod12RValues = aPINod2->ChangeValue (theIndices.Face2Pt1)->Data();
              aNod12RValues.Point.ChangeCoord(1) = aNod11RValues.Point.X();
              aNod12RValues.Point.ChangeCoord(2) = aNod11RValues.Point.Y();
              UpdateAroundNode (theIndices.Face2Pt1, aNod12Indices,
                                *aTData2, *aPISeg2, *aPINod2);
            }
          }
          aNod11Indices = &aPINod1->ChangeValue (theIndices.Face1Pt2)->Indices();
          if (aNod11Indices->Flag & NMsk_Move)
          {
          #ifdef OCCT_DEBUG
            if (DoTrace)
            {
              std::cout << theIndices.Face1Pt2 << " modifies 12" << std::endl;
            }
          #endif
            const HLRAlgo_PolyInternalNode::NodeData& aNod11RValues = aPINod1->Value (theIndices.Face1Pt2)->Data();
            HLRAlgo_BiPoint::PointsT& aPoints = BP.Points();
            aPoints.Pnt2 = aPoints.PntP2 = aNod11RValues.Point;
            TIMultiply(aPoints.Pnt2);
            if (theIndices.FaceConex2 != 0)
            {
              HLRAlgo_PolyInternalNode::NodeIndices& aNod12Indices = aPINod2->ChangeValue (theIndices.Face2Pt2)->Indices();
              HLRAlgo_PolyInternalNode::NodeData& aNod12RValues = aPINod2->ChangeValue (theIndices.Face2Pt2)->Data();
              aNod12RValues.Point.ChangeCoord(1) = aNod11RValues.Point.X();
              aNod12RValues.Point.ChangeCoord(2) = aNod11RValues.Point.Y();
              UpdateAroundNode (theIndices.Face2Pt2, aNod12Indices,
                                *aTData2, *aPISeg2, *aPINod2);
            }
          }
        }
        if (theIndices.FaceConex2 != 0)
        {
          const Handle(HLRAlgo_PolyInternalData)& pid2 = thePID.Value (theIndices.FaceConex2);
          aPINod2 = &pid2->PINod();
          HLRAlgo_PolyInternalNode::NodeIndices* aNod11Indices = &aPINod2->ChangeValue (theIndices.Face2Pt1)->Indices();
          if (aNod11Indices->Flag & NMsk_Move)
          {
          #ifdef OCCT_DEBUG
            if (DoTrace)
            {
              std::cout << theIndices.Face2Pt1 << " modifies 21" << std::endl;
            }
          #endif
            const HLRAlgo_PolyInternalNode::NodeData& aNod11RValues = aPINod2->Value (theIndices.Face2Pt1)->Data();
            HLRAlgo_BiPoint::PointsT& aPoints = BP.Points();
            aPoints.Pnt1 = aPoints.PntP1 = aNod11RValues.Point;
            TIMultiply(aPoints.Pnt1);
            if (theIndices.FaceConex1 != 0)
            {
              HLRAlgo_PolyInternalNode::NodeIndices& aNod12Indices = aPINod1->ChangeValue (theIndices.Face1Pt1)->Indices();
              HLRAlgo_PolyInternalNode::NodeData& aNod12RValues = aPINod1->ChangeValue (theIndices.Face1Pt1)->Data();
              aNod12RValues.Point.ChangeCoord(1) = aNod11RValues.Point.X();
              aNod12RValues.Point.ChangeCoord(2) = aNod11RValues.Point.Y();
              UpdateAroundNode (theIndices.Face1Pt1, aNod12Indices,
                                *aTData1, *aPISeg1, *aPINod1);
            }
          }
          aNod11Indices = &aPINod2->ChangeValue (theIndices.Face2Pt2)->Indices();
          if (aNod11Indices->Flag & NMsk_Move)
          {
          #ifdef OCCT_DEBUG
            if (DoTrace)
            {
              std::cout << theIndices.Face2Pt2 << " modifies 22" << std::endl;
            }
          #endif
            const HLRAlgo_PolyInternalNode::NodeData* aNod11RValues = &aPINod2->Value (theIndices.Face2Pt2)->Data();
            HLRAlgo_BiPoint::PointsT& aPoints = BP.Points();
            aPoints.Pnt2 = aPoints.PntP2 = aNod11RValues->Point;
            TIMultiply (aPoints.Pnt2);
            if (theIndices.FaceConex1 != 0)
            {
              HLRAlgo_PolyInternalNode::NodeIndices& aNod12Indices = aPINod1->ChangeValue (theIndices.Face1Pt2)->Indices();
              HLRAlgo_PolyInternalNode::NodeData& aNod12RValues = aPINod1->ChangeValue (theIndices.Face1Pt2)->Data();
              aNod12RValues.Point.ChangeCoord(1) = aNod11RValues->Point.X();
              aNod12RValues.Point.ChangeCoord(2) = aNod11RValues->Point.Y();
              UpdateAroundNode (theIndices.Face1Pt2, aNod12Indices,
                                *aTData1, *aPISeg1, *aPINod1);
            }
          }
        }
      }

      for (Standard_Integer aFaceIter = 1; aFaceIter <= aNbFaces; ++aFaceIter)
      {
        const Handle(HLRAlgo_PolyInternalData)& aPid = thePID.ChangeValue (aFaceIter);
        if (!aPid.IsNull())
        {
          const Standard_Integer aNbNodes = aPid->NbPINod();
          HLRAlgo_Array1OfPINod& aPINod = aPid->PINod();
          for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
          {
            HLRAlgo_PolyInternalNode::NodeIndices& aNod11Indices = aPINod.ChangeValue (aNodeIter)->Indices();
            aNod11Indices.Flag &= ~NMsk_Move;
          }
        }
      }
    }
  }
}

//=======================================================================
//function : FindEdgeOnTriangle
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::FindEdgeOnTriangle (const HLRAlgo_TriangleData& theTriangle,
                                           const Standard_Integer theIp1,
                                           const Standard_Integer theIp2,
                                           Standard_Integer& theJtrouv,
                                           Standard_Boolean& theIsDirect) const
{
  Standard_Integer n1 = theTriangle.Node1;
  Standard_Integer n2 = theTriangle.Node2;
  Standard_Integer n3 = theTriangle.Node3;
  if      (theIp1 == n1 && theIp2 == n2)
  {
    theJtrouv = 0;
    theIsDirect = Standard_True;
    return;
  }
  else if (theIp2 == n1 && theIp1 == n2)
  {
    theJtrouv = 0;
    theIsDirect = Standard_False;
    return;
  }
  else if (theIp1 == n2 && theIp2 == n3)
  {
    theJtrouv = 1;
    theIsDirect = Standard_True;
    return;
  }
  else if (theIp2 == n2 && theIp1 == n3)
  {
    theJtrouv = 1;
    theIsDirect = Standard_False;
    return;
  }
  else if (theIp1 == n3 && theIp2 == n1)
  {
    theJtrouv = 2;
    theIsDirect = Standard_True;
    return;
  }
  else if (theIp2 == n3 && theIp1 == n1)
  {
    theJtrouv = 2;
    theIsDirect = Standard_False;
    return;
  }
}

//=======================================================================
//function : ChangeNode
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::ChangeNode (const Standard_Integer theIp1,
                                   const Standard_Integer theIp2,
                                   HLRAlgo_PolyInternalNode::NodeIndices& theNod1Indices,
                                   HLRAlgo_PolyInternalNode::NodeData& theNod1RValues,
                                   HLRAlgo_PolyInternalNode::NodeIndices& theNod2Indices,
                                   HLRAlgo_PolyInternalNode::NodeData& theNod2RValues,
                                   const Standard_Real theCoef1,
                                   const Standard_Real theX3,
                                   const Standard_Real theY3,
                                   const Standard_Real theZ3,
                                   const Standard_Boolean theIsFirst,
                                   HLRAlgo_Array1OfTData& theTData,
                                   HLRAlgo_Array1OfPISeg& thePISeg,
                                   HLRAlgo_Array1OfPINod& thePINod) const
{
  const Standard_Real aCoef2 = 1.0 - theCoef1;
  if (theIsFirst)
  {
    theNod1RValues.Point = gp_XYZ (theX3, theY3, theZ3);
    theNod1RValues.UV = aCoef2 * theNod1RValues.UV + theCoef1 * theNod2RValues.UV;
    theNod1RValues.Scal = theNod1RValues.Scal * aCoef2 + theNod2RValues.Scal * theCoef1;
    const gp_XYZ aXYZ = aCoef2 * theNod1RValues.Normal + theCoef1 * theNod2RValues.Normal;
    const Standard_Real aNorm = aXYZ.Modulus();
    if (aNorm > 0)
    {
      theNod1RValues.Normal = (1 / aNorm) * aXYZ;
    }
    else
    {
      theNod1RValues.Normal = gp_XYZ(1., 0., 0.);
#ifdef OCCT_DEBUG
      if (DoError)
      {
        std::cout << "HLRBRep_PolyAlgo::ChangeNode between " << theIp1;
        std::cout << " and " << theIp2 << std::endl;
      }
#endif
    }
    UpdateAroundNode (theIp1, theNod1Indices, theTData, thePISeg, thePINod);
  }
  else
  {
    theNod2RValues.Point = gp_XYZ (theX3, theY3, theZ3);
    theNod2RValues.UV = aCoef2 * theNod1RValues.UV + theCoef1 * theNod2RValues.UV;
    theNod2RValues.Scal = theNod1RValues.Scal * aCoef2 + theNod2RValues.Scal * theCoef1;
    const gp_XYZ aXYZ = aCoef2 * theNod1RValues.Normal + theCoef1 * theNod2RValues.Normal;
    const Standard_Real aNorm = aXYZ.Modulus();
    if (aNorm > 0)
    {
      theNod2RValues.Normal = (1 / aNorm) * aXYZ;
    }
    else
    {
      theNod2RValues.Normal = gp_XYZ(1., 0., 0.);
#ifdef OCCT_DEBUG
      if (DoError)
      {
        std::cout << "HLRBRep_PolyAlgo::ChangeNode between " << theIp2;
        std::cout << " and " << theIp1 << std::endl;
      }
#endif
    }
    UpdateAroundNode (theIp2, theNod2Indices, theTData, thePISeg, thePINod);
  }
}

//=======================================================================
//function : UpdateAroundNode
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::UpdateAroundNode (const Standard_Integer theINode,
                                         HLRAlgo_PolyInternalNode::NodeIndices& theNod1Indices,
                                         HLRAlgo_Array1OfTData& theTData,
                                         HLRAlgo_Array1OfPISeg& thePISeg,
                                         HLRAlgo_Array1OfPINod& thePINod) const
{
  Standard_Integer iiii = theNod1Indices.NdSg;
  while (iiii != 0)
  {
    HLRAlgo_PolyInternalSegment& aSegIndices = thePISeg.ChangeValue (iiii);
    const Standard_Integer iTri1 = aSegIndices.Conex1;
    const Standard_Integer iTri2 = aSegIndices.Conex2;
    if (iTri1 != 0)
    {
      HLRAlgo_TriangleData& aTriangle = theTData.ChangeValue (iTri1);
      const Handle(HLRAlgo_PolyInternalNode)& aPN1 = thePINod.ChangeValue (aTriangle.Node1);
      const Handle(HLRAlgo_PolyInternalNode)& aPN2 = thePINod.ChangeValue (aTriangle.Node2);
      const Handle(HLRAlgo_PolyInternalNode)& aPN3 = thePINod.ChangeValue (aTriangle.Node3);
      HLRAlgo_PolyInternalNode::NodeIndices& aNod1Indices = aPN1->Indices();
      HLRAlgo_PolyInternalNode::NodeIndices& aNod2Indices = aPN2->Indices();
      HLRAlgo_PolyInternalNode::NodeIndices& aNod3Indices = aPN3->Indices();
      HLRAlgo_PolyInternalNode::NodeData& aNod1RValues = aPN1->Data();
      HLRAlgo_PolyInternalNode::NodeData& aNod2RValues = aPN2->Data();
      HLRAlgo_PolyInternalNode::NodeData& aNod3RValues = aPN3->Data();
      OrientTriangle (iTri1, aTriangle,
                      aNod1Indices, aNod1RValues,
                      aNod2Indices, aNod2RValues,
                      aNod3Indices, aNod3RValues);
    }
    if (iTri2 != 0)
    {
      HLRAlgo_TriangleData& aTriangle2 = theTData.ChangeValue (iTri2);
      const Handle(HLRAlgo_PolyInternalNode)& aPN1 = thePINod.ChangeValue (aTriangle2.Node1);
      const Handle(HLRAlgo_PolyInternalNode)& aPN2 = thePINod.ChangeValue (aTriangle2.Node2);
      const Handle(HLRAlgo_PolyInternalNode)& aPN3 = thePINod.ChangeValue (aTriangle2.Node3);
      HLRAlgo_PolyInternalNode::NodeIndices& aNod1Indices = aPN1->Indices();
      HLRAlgo_PolyInternalNode::NodeIndices& aNod2Indices = aPN2->Indices();
      HLRAlgo_PolyInternalNode::NodeIndices& aNod3Indices = aPN3->Indices();
      HLRAlgo_PolyInternalNode::NodeData& aNod1RValues = aPN1->Data();
      HLRAlgo_PolyInternalNode::NodeData& aNod2RValues = aPN2->Data();
      HLRAlgo_PolyInternalNode::NodeData& aNod3RValues = aPN3->Data();
      OrientTriangle (iTri2, aTriangle2,
                      aNod1Indices, aNod1RValues,
                      aNod2Indices, aNod2RValues,
                      aNod3Indices, aNod3RValues);
    }
    if (aSegIndices.LstSg1 == theINode) { iiii = aSegIndices.NxtSg1; }
    else                                { iiii = aSegIndices.NxtSg2; }
  }
}

//=======================================================================
//function : OrientTriangle
//purpose  :
//=======================================================================
void  HLRBRep_PolyAlgo::OrientTriangle (const Standard_Integer theITri,
                                        HLRAlgo_TriangleData& theTriangle,
                                        HLRAlgo_PolyInternalNode::NodeIndices& theNod1Indices,
                                        HLRAlgo_PolyInternalNode::NodeData& theNod1RValues,
                                        HLRAlgo_PolyInternalNode::NodeIndices& theNod2Indices,
                                        HLRAlgo_PolyInternalNode::NodeData& theNod2RValues,
                                        HLRAlgo_PolyInternalNode::NodeIndices& theNod3Indices,
                                        HLRAlgo_PolyInternalNode::NodeData& theNod3RValues) const
{
  Standard_Boolean o1 = (theNod1Indices.Flag & NMsk_OutL) != 0;
  Standard_Boolean o2 = (theNod2Indices.Flag & NMsk_OutL) != 0;
  Standard_Boolean o3 = (theNod3Indices.Flag & NMsk_OutL) != 0;
  theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskFlat;
  theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskOnOutL;
  if (o1 && o2 && o3)
  {
    theTriangle.Flags |=  HLRAlgo_PolyMask_FMskSide;
    theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskBack;
    theTriangle.Flags |=  HLRAlgo_PolyMask_FMskOnOutL;
#ifdef OCCT_DEBUG
    if (DoTrace)
    {
      std::cout << "HLRBRep_PolyAlgo::OrientTriangle : OnOutL";
      std::cout << " triangle " << theITri << std::endl;
    }
#else
    (void )theITri;
#endif
  }
  else
  {
    const Standard_Real s1 = theNod1RValues.Scal;
    const Standard_Real s2 = theNod2RValues.Scal;
    const Standard_Real s3 = theNod3RValues.Scal;
    Standard_Real as1 = s1;
    Standard_Real as2 = s2;
    Standard_Real as3 = s3;
    if (s1 < 0) { as1 = -s1; }
    if (s2 < 0) { as2 = -s2; }
    if (s3 < 0) { as3 = -s3; }
    Standard_Real  s = 0;
    Standard_Real as = 0;
    if (!o1            ) {s = s1; as = as1;}
    if (!o2 && as < as2) {s = s2; as = as2;}
    if (!o3 && as < as3) {s = s3; as = as3;}
    if (s > 0)
    {
      theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskSide;
      theTriangle.Flags |=  HLRAlgo_PolyMask_FMskBack;
    }
    else
    {
      theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskSide;
      theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskBack;
    }
    gp_XYZ aD12 = theNod2RValues.Point - theNod1RValues.Point;
    const Standard_Real aD12Norm = aD12.Modulus();
    if (aD12Norm <= 1.e-10)
    {
#ifdef OCCT_DEBUG
      if (DoTrace)
      {
        std::cout << "HLRBRep_PolyAlgo::OrientTriangle : Flat";
        std::cout << " triangle " << theITri << std::endl;
      }
#endif
      theTriangle.Flags |=  HLRAlgo_PolyMask_FMskFlat;
      theTriangle.Flags |=  HLRAlgo_PolyMask_FMskSide;
      theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskBack;
    }
    else
    {
      gp_XYZ aD23 = theNod3RValues.Point - theNod2RValues.Point;
      const Standard_Real aD23Norm = aD23.Modulus();
      if (aD23Norm < 1.e-10)
      {
      #ifdef OCCT_DEBUG
        if (DoTrace)
        {
          std::cout << "HLRBRep_PolyAlgo::OrientTriangle : Flat";
          std::cout << " triangle " << theITri << std::endl;
        }
      #endif
        theTriangle.Flags |=  HLRAlgo_PolyMask_FMskFlat;
        theTriangle.Flags |=  HLRAlgo_PolyMask_FMskSide;
        theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskBack;
      }
      else
      {
        const gp_XYZ aD31 = theNod1RValues.Point - theNod3RValues.Point;
        const Standard_Real aD31Norm = aD31.Modulus();
        if (aD31Norm < 1.e-10)
        {
        #ifdef OCCT_DEBUG
          if (DoTrace)
          {
            std::cout << "HLRBRep_PolyAlgo::OrientTriangle : Flat";
            std::cout << " triangle " << theITri << std::endl;
          }
        #endif
          theTriangle.Flags |=  HLRAlgo_PolyMask_FMskFlat;
          theTriangle.Flags |=  HLRAlgo_PolyMask_FMskSide;
          theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskBack;
        }
        else
        {
          aD12 *= 1 / aD12Norm;
          aD23 *= 1 / aD23Norm;
          gp_XYZ aD = aD12 ^ aD23;
          const Standard_Real aDNorm = aD.Modulus();
          if (aDNorm < 1.e-5)
          {
          #ifdef OCCT_DEBUG
            if (DoTrace)
            {
              std::cout << "HLRBRep_PolyAlgo::OrientTriangle : Flat";
              std::cout << " triangle " << theITri << std::endl;
            }
          #endif
            theTriangle.Flags |=  HLRAlgo_PolyMask_FMskFlat;
            theTriangle.Flags |=  HLRAlgo_PolyMask_FMskSide;
            theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskBack;
          }
          else
          {
            Standard_Real o;
            if (myProj.Perspective())
            {
              aD *= 1 / aDNorm;
              o = aD.Z() * myProj.Focus() - aD * theNod1RValues.Point;
            }
            else
            {
              o = aD.Z() / aDNorm;
            }
            if (o < 0)
            {
              theTriangle.Flags |=  HLRAlgo_PolyMask_FMskOrBack;
              o = -o;
            }
            else
            {
              theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskOrBack;
            }
            if (o < 1.e-10)
            {
              theTriangle.Flags |=  HLRAlgo_PolyMask_FMskSide;
              theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskBack;
            }
          }
        }
      }
    }
  }

  if ((!(theTriangle.Flags & HLRAlgo_PolyMask_FMskBack) &&  (theTriangle.Flags & HLRAlgo_PolyMask_FMskOrBack)) ||
      ( (theTriangle.Flags & HLRAlgo_PolyMask_FMskBack) && !(theTriangle.Flags & HLRAlgo_PolyMask_FMskOrBack)))
  {
    theTriangle.Flags |=  HLRAlgo_PolyMask_FMskFrBack;
  }
  else 
  {
    theTriangle.Flags &= ~HLRAlgo_PolyMask_FMskFrBack;
  }
}

//=======================================================================
//function : Triangles
//purpose  :
//=======================================================================
Standard_Boolean HLRBRep_PolyAlgo::Triangles (const Standard_Integer theIp1,
                                              const Standard_Integer theIp2,
                                              HLRAlgo_PolyInternalNode::NodeIndices& theNod1Indices,
                                              HLRAlgo_Array1OfPISeg*& thePISeg,
                                              Standard_Integer& theITri1,
                                              Standard_Integer& theITri2) const
{
  Standard_Integer iiii = theNod1Indices.NdSg;
  while (iiii != 0)
  {
    HLRAlgo_PolyInternalSegment& aSegIndices = thePISeg->ChangeValue (iiii);
    if (aSegIndices.LstSg1 == theIp1)
    {
      if (aSegIndices.LstSg2 == theIp2)
      {
        theITri1 = aSegIndices.Conex1;
        theITri2 = aSegIndices.Conex2;
        return Standard_True;
      }
      else
      {
        iiii = aSegIndices.NxtSg1;
      }
    }
    else
    {
      if (aSegIndices.LstSg1 == theIp2)
      {
        theITri1 = aSegIndices.Conex1;
        theITri2 = aSegIndices.Conex2;
        return Standard_True;
      }
      else
      {
        iiii = aSegIndices.NxtSg2;
      }
    }
  }
  theITri1 = 0;
  theITri2 = 0;
#ifdef OCCT_DEBUG
  if (DoError)
  {
    std::cout << "HLRBRep_PolyAlgo::Triangles : error";
    std::cout << " between " << theIp1 << " and " << theIp2 << std::endl;
  }
#endif
  return Standard_False;
}

//=======================================================================
//function : NewNode
//purpose  :
//=======================================================================
Standard_Boolean HLRBRep_PolyAlgo::NewNode (HLRAlgo_PolyInternalNode::NodeData& theNod1RValues,
                                            HLRAlgo_PolyInternalNode::NodeData& theNod2RValues,
                                            Standard_Real& theCoef1,
                                            Standard_Boolean& theToMoveP1) const
{
  const Standard_Real aTolAng = myTolAngular * 0.5;
  if ((theNod1RValues.Scal >= aTolAng && theNod2RValues.Scal <= -aTolAng) ||
      (theNod2RValues.Scal >= aTolAng && theNod1RValues.Scal <= -aTolAng))
  {
    theCoef1 = theNod1RValues.Scal / (theNod2RValues.Scal - theNod1RValues.Scal);
    if (theCoef1 < 0) { theCoef1 = -theCoef1; }
    theToMoveP1 = theCoef1 < 0.5;
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : UVNode
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::UVNode (HLRAlgo_PolyInternalNode::NodeData& theNod1RValues,
                               HLRAlgo_PolyInternalNode::NodeData& theNod2RValues,
                               const Standard_Real theCoef1,
                               Standard_Real& theU3,
                               Standard_Real& theV3) const
{
  const Standard_Real aCoef2 = 1.0 - theCoef1;
  const gp_XY aUV3 = aCoef2 * theNod1RValues.UV + theCoef1 * theNod2RValues.UV;
  theU3 = aUV3.X();
  theV3 = aUV3.Y();
}

//=======================================================================
//function : CheckDegeneratedSegment
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::CheckDegeneratedSegment (HLRAlgo_PolyInternalNode::NodeIndices& theNod1Indices,
                                                HLRAlgo_PolyInternalNode::NodeData& theNod1RValues,
                                                HLRAlgo_PolyInternalNode::NodeIndices& theNod2Indices,
                                                HLRAlgo_PolyInternalNode::NodeData& theNod2RValues) const
{
  theNod1Indices.Flag |=  NMsk_Fuck;
  theNod2Indices.Flag |=  NMsk_Fuck;
  if ((theNod1RValues.Scal >= myTolAngular && theNod2RValues.Scal <= -myTolAngular) ||
      (theNod2RValues.Scal >= myTolAngular && theNod1RValues.Scal <= -myTolAngular))
  {
    theNod1RValues.Scal  = 0.;
    theNod1Indices.Flag |= NMsk_OutL;
    theNod2RValues.Scal  = 0.;
    theNod2Indices.Flag |= NMsk_OutL;
  }
}

//=======================================================================
//function : UpdateOutLines
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::UpdateOutLines (HLRAlgo_ListOfBPoint& theList,
                                       NCollection_Array1<Handle(HLRAlgo_PolyInternalData)>& thePID)
{
  Standard_Real X1  ,Y1  ,Z1  ,X2  ,Y2  ,Z2;
  Standard_Real XTI1,YTI1,ZTI1,XTI2,YTI2,ZTI2;
  const Standard_Integer aNbFaces = myFMap.Extent();
  for (Standard_Integer aFaceIter = 1; aFaceIter <= aNbFaces; ++aFaceIter)
  {
    const Handle(HLRAlgo_PolyInternalData)& aPid = thePID.ChangeValue (aFaceIter);
    if (aPid.IsNull())
    {
      continue;
    }

    if (!aPid->IntOutL())
    {
      continue;
    }

    HLRAlgo_Array1OfTData& aTData = aPid->TData();
    HLRAlgo_Array1OfPISeg& aPISeg = aPid->PISeg();
    HLRAlgo_Array1OfPINod& aPINod = aPid->PINod();
    Standard_Integer j,it1,it2,tn1,tn2,tn3,pd,pf;
    Standard_Boolean isOutl = false;
    const Standard_Integer aNbSegs = aPid->NbPISeg();
    for (Standard_Integer aSegIter = 1; aSegIter <= aNbSegs; ++aSegIter)
    {
      const HLRAlgo_PolyInternalSegment& psg = aPISeg.Value (aSegIter);
      it1 = psg.Conex1;
      it2 = psg.Conex2;
      if (it1 != 0 && it2 != 0 && it1 != it2)   // debile but sure !
      {
        HLRAlgo_TriangleData& aTriangle  = aTData.ChangeValue (it1);
        HLRAlgo_TriangleData& aTriangle2 = aTData.ChangeValue (it2);
        if      (!(aTriangle.Flags & HLRAlgo_PolyMask_FMskSide) && !(aTriangle2.Flags & HLRAlgo_PolyMask_FMskSide))
        {
          isOutl =  (aTriangle.Flags & HLRAlgo_PolyMask_FMskBack) !=  (aTriangle2.Flags & HLRAlgo_PolyMask_FMskBack);
        }
        else if ( (aTriangle.Flags & HLRAlgo_PolyMask_FMskSide) &&  (aTriangle2.Flags & HLRAlgo_PolyMask_FMskSide))
        {
          isOutl = Standard_False;
        }
        else if (  aTriangle.Flags & HLRAlgo_PolyMask_FMskSide)
        {
          isOutl = !(aTriangle.Flags & HLRAlgo_PolyMask_FMskFlat) && !(aTriangle2.Flags & HLRAlgo_PolyMask_FMskBack);
        }
        else
        {
          isOutl = !(aTriangle2.Flags & HLRAlgo_PolyMask_FMskFlat) && !(aTriangle.Flags & HLRAlgo_PolyMask_FMskBack);
        }
	    
        if (isOutl)
        {
          pd = psg.LstSg1;
          pf = psg.LstSg2;
          tn1 = aTriangle.Node1;
          tn2 = aTriangle.Node2;
          tn3 = aTriangle.Node3;
          if (!(aTriangle.Flags & HLRAlgo_PolyMask_FMskSide) && (aTriangle.Flags & HLRAlgo_PolyMask_FMskOrBack))
          {
            j   = tn1;
            tn1 = tn3;
            tn3 = j;
          }
          if      ((tn1 == pd && tn2 == pf) || (tn1 == pf && tn2 == pd))
          {
            aTriangle.Flags |=  HLRAlgo_PolyMask_EMskOutLin1;
          }
          else if ((tn2 == pd && tn3 == pf) || (tn2 == pf && tn3 == pd))
          {
            aTriangle.Flags |=  HLRAlgo_PolyMask_EMskOutLin2;
          }
          else if ((tn3 == pd && tn1 == pf) || (tn3 == pf && tn1 == pd))
          {
            aTriangle.Flags |=  HLRAlgo_PolyMask_EMskOutLin3;
          }
        #ifdef OCCT_DEBUG
          else if (DoError)
          {
            std::cout << "HLRAlgo_PolyInternalData::UpdateOutLines";
            std::cout << " : segment not found" << std::endl;
          }
        #endif
          tn1 = aTriangle2.Node1;
          tn2 = aTriangle2.Node2;
          tn3 = aTriangle2.Node3;
          if (!(aTriangle2.Flags & HLRAlgo_PolyMask_FMskSide) && (aTriangle2.Flags & HLRAlgo_PolyMask_FMskOrBack))
          {
            j   = tn1;
            tn1 = tn3;
            tn3 = j;
          }
          if      ((tn1 == pd && tn2 == pf) || (tn1 == pf && tn2 == pd))
          {
            aTriangle2.Flags |=  HLRAlgo_PolyMask_EMskOutLin1;
          }
          else if ((tn2 == pd && tn3 == pf) || (tn2 == pf && tn3 == pd))
          {
            aTriangle2.Flags |=  HLRAlgo_PolyMask_EMskOutLin2;
          }
          else if ((tn3 == pd && tn1 == pf) || (tn3 == pf && tn1 == pd))
          {
            aTriangle2.Flags |=  HLRAlgo_PolyMask_EMskOutLin3;
          }
        #ifdef OCCT_DEBUG
          else if (DoError)
          {
            std::cout << "HLRAlgo_PolyInternalData::UpdateOutLines";
            std::cout << " : segment not found" << std::endl;
          }
        #endif
          const HLRAlgo_PolyInternalNode::NodeData& aNod1RValues = aPINod.Value (pd)->Data();
          const HLRAlgo_PolyInternalNode::NodeData& aNod2RValues = aPINod.Value (pf)->Data();
          XTI1 = X1 = aNod1RValues.Point.X();
          YTI1 = Y1 = aNod1RValues.Point.Y();
          ZTI1 = Z1 = aNod1RValues.Point.Z();
          XTI2 = X2 = aNod2RValues.Point.X();
          YTI2 = Y2 = aNod2RValues.Point.Y();
          ZTI2 = Z2 = aNod2RValues.Point.Z();
          TIMultiply (XTI1, YTI1, ZTI1);
          TIMultiply (XTI2, YTI2, ZTI2);
          theList.Append (HLRAlgo_BiPoint (XTI1, YTI1, ZTI1, XTI2, YTI2, ZTI2,
                                           X1  , Y1  , Z1  , X2  , Y2  , Z2  ,
                                           aFaceIter, aFaceIter, pd, pf, aFaceIter, pd, pf, 12));
        }
      }
    }
  }
}

//=======================================================================
//function : UpdateEdgesBiPoints
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::UpdateEdgesBiPoints (HLRAlgo_ListOfBPoint& theList,
                                            const NCollection_Array1<Handle(HLRAlgo_PolyInternalData)>& thePID,
                                            const Standard_Boolean theIsClosed)
{
  Standard_Integer itri1, itri2, tbid;
  for (HLRAlgo_ListIteratorOfListOfBPoint aBPntIter (theList); aBPntIter.More(); aBPntIter.Next())
  {
    HLRAlgo_BiPoint& aBP = aBPntIter.ChangeValue();
    const HLRAlgo_BiPoint::IndicesT& aIndices = aBP.Indices();
    if (aIndices.FaceConex1 != 0 && aIndices.FaceConex2 != 0)
    {
      const Handle(HLRAlgo_PolyInternalData)& aPid1 = thePID.Value (aIndices.FaceConex1);
      const Handle(HLRAlgo_PolyInternalData)& aPid2 = thePID.Value (aIndices.FaceConex2);
      HLRAlgo_Array1OfPISeg* aPISeg1 = &aPid1->PISeg();
      HLRAlgo_Array1OfPISeg* aPISeg2 = &aPid2->PISeg();
      HLRAlgo_PolyInternalNode::NodeIndices& aNod11Indices = aPid1->PINod().ChangeValue (aIndices.Face1Pt1)->Indices();
      HLRAlgo_PolyInternalNode::NodeIndices& aNod21Indices = aPid2->PINod().ChangeValue (aIndices.Face2Pt1)->Indices();
      Triangles (aIndices.Face1Pt1, aIndices.Face1Pt2, aNod11Indices, aPISeg1, itri1, tbid);
      Triangles (aIndices.Face2Pt1, aIndices.Face2Pt2, aNod21Indices, aPISeg2, itri2, tbid);

      if (itri1 != 0 && itri2 != 0)
      {
        if (aIndices.FaceConex1 != aIndices.FaceConex2 || itri1 != itri2)
        {
          HLRAlgo_Array1OfTData* aTData1 = &aPid1->TData();
          HLRAlgo_Array1OfTData* aTData2 = &aPid2->TData();
          HLRAlgo_TriangleData& aTriangle  = aTData1->ChangeValue (itri1);
          HLRAlgo_TriangleData& aTriangle2 = aTData2->ChangeValue (itri2);
          if (theIsClosed)
          {
            if (((aTriangle.Flags & HLRAlgo_PolyMask_FMskBack) && (aTriangle2.Flags & HLRAlgo_PolyMask_FMskBack)) ||
                ((aTriangle.Flags & HLRAlgo_PolyMask_FMskSide) && (aTriangle2.Flags & HLRAlgo_PolyMask_FMskSide)) ||
                ((aTriangle.Flags & HLRAlgo_PolyMask_FMskBack) && (aTriangle2.Flags & HLRAlgo_PolyMask_FMskSide)) ||
                ((aTriangle.Flags & HLRAlgo_PolyMask_FMskSide) && (aTriangle2.Flags & HLRAlgo_PolyMask_FMskBack)))
            {
              aBP.Hidden (Standard_True);
            }
          }

          Standard_Boolean isOutl;
          if      (!(aTriangle.Flags & HLRAlgo_PolyMask_FMskSide) && !(aTriangle2.Flags & HLRAlgo_PolyMask_FMskSide))
          {
            isOutl =  (aTriangle.Flags & HLRAlgo_PolyMask_FMskBack) !=  (aTriangle2.Flags & HLRAlgo_PolyMask_FMskBack);
          }
          else if ( (aTriangle.Flags & HLRAlgo_PolyMask_FMskSide) &&  (aTriangle2.Flags & HLRAlgo_PolyMask_FMskSide))
          {
            isOutl = Standard_False;
          }
          else if ( (aTriangle.Flags & HLRAlgo_PolyMask_FMskSide))
          {
            isOutl = !(aTriangle.Flags & HLRAlgo_PolyMask_FMskFlat) && !(aTriangle2.Flags & HLRAlgo_PolyMask_FMskBack);
          }
          else
          {
            isOutl = !(aTriangle2.Flags & HLRAlgo_PolyMask_FMskFlat) && !(aTriangle.Flags & HLRAlgo_PolyMask_FMskBack);
          }
          aBP.OutLine (isOutl);
        }
      }
#ifdef OCCT_DEBUG
      else if (DoError)
      {
        std::cout << "HLRBRep_PolyAlgo::UpdateEdgesBiPoints : error ";
        std::cout << " between " << aIndices.FaceConex1 << std::setw(6);
        std::cout << " and " << aIndices.FaceConex2 << std::endl;
      }
#endif
    }
  }
}

//=======================================================================
//function : UpdatePolyData
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::UpdatePolyData (NCollection_Array1<Handle(HLRAlgo_PolyData)>& thePD,
                                       NCollection_Array1<Handle(HLRAlgo_PolyInternalData)>& thePID,
                                       const Standard_Boolean theIsClosed)
{
  const Standard_Integer aNbFaces = myFMap.Extent();
  for (Standard_Integer aFaceIter = 1; aFaceIter <= aNbFaces; ++aFaceIter)
  {
    const Handle(HLRAlgo_PolyInternalData)& aPid = thePID.ChangeValue (aFaceIter);
    if (aPid.IsNull())
    {
      continue;
    }

    const Standard_Integer aNbNodes = aPid->NbPINod();
    const Standard_Integer aNbTris  = aPid->NbTData();
    Handle(TColgp_HArray1OfXYZ)    aHNodes = new TColgp_HArray1OfXYZ   (1, aNbNodes);
    Handle(HLRAlgo_HArray1OfTData) aHTData = new HLRAlgo_HArray1OfTData(1, aNbTris);
    TColgp_Array1OfXYZ&    aNodes = aHNodes->ChangeArray1();
    HLRAlgo_Array1OfTData& aTrian = aHTData->ChangeArray1();
    HLRAlgo_Array1OfTData& aTData = aPid->TData();
    HLRAlgo_Array1OfPINod& aPINod = aPid->PINod();
    Standard_Integer aNbHidden = 0;

    for (Standard_Integer aNodeIter = 1; aNodeIter <= aNbNodes; ++aNodeIter)
    {
      const HLRAlgo_PolyInternalNode::NodeData& aNod1RValues = aPINod.Value (aNodeIter)->Data();
      aNodes.SetValue (aNodeIter, aNod1RValues.Point);
    }

    for (Standard_Integer aTriIter = 1; aTriIter <= aNbTris; ++aTriIter)
    {
      HLRAlgo_TriangleData& anOT = aTData.ChangeValue (aTriIter);
      HLRAlgo_TriangleData& aNT  = aTrian.ChangeValue (aTriIter);
      if (!(anOT.Flags & HLRAlgo_PolyMask_FMskSide))
      {
      #ifdef OCCT_DEBUG
        if ((anOT.Flags & HLRAlgo_PolyMask_FMskFrBack) && DoTrace)
        {
          std::cout << "HLRBRep_PolyAlgo::ReverseBackTriangle :";
          std::cout << " face " << aFaceIter << std::setw(6);
          std::cout << " triangle " << aTriIter << std::endl;
        }
      #endif
        if (anOT.Flags & HLRAlgo_PolyMask_FMskOrBack)
        {
          Standard_Integer j = anOT.Node1;
          anOT.Node1 = anOT.Node3;
          anOT.Node3 = j;
          anOT.Flags |= HLRAlgo_PolyMask_FMskBack;
        }
        else
        {
          anOT.Flags &= ~HLRAlgo_PolyMask_FMskBack;
          //Tri1Flags |= HLRAlgo_PolyMask_FMskBack;//OCC349
        }
      }
      aNT.Node1 = anOT.Node1;
      aNT.Node2 = anOT.Node2;
      aNT.Node3 = anOT.Node3;
      aNT.Flags = anOT.Flags;
      if (!(aNT.Flags & HLRAlgo_PolyMask_FMskSide) &&
          (!(aNT.Flags & HLRAlgo_PolyMask_FMskBack) || !theIsClosed))
      {
        aNT.Flags |=  HLRAlgo_PolyMask_FMskHiding;
        ++aNbHidden;
      }
      else
      {
        aNT.Flags &= ~HLRAlgo_PolyMask_FMskHiding;
      }
    }

    Handle(HLRAlgo_HArray1OfPHDat) aHPHDat;
    if (aNbHidden > 0)
    {
      aHPHDat = new HLRAlgo_HArray1OfPHDat (1, aNbHidden);
    }

    const Handle(HLRAlgo_PolyData)& aPd = thePD.ChangeValue (aFaceIter);
    aPd->HNodes (aHNodes);
    aPd->HTData (aHTData);
    aPd->HPHDat (aHPHDat);
    aPd->FaceIndex (aFaceIter);
  }
}

//=======================================================================
//function : TMultiply
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::TMultiply (Standard_Real& theX,
                                  Standard_Real& theY,
                                  Standard_Real& theZ,
                                  const Standard_Boolean theVPO) const
{
  Standard_Real Xt = TMat[0][0] * theX + TMat[0][1] * theY + TMat[0][2] * theZ + (theVPO ? 0 : TLoc[0]);//OCC349
  Standard_Real Yt = TMat[1][0] * theX + TMat[1][1] * theY + TMat[1][2] * theZ + (theVPO ? 0 : TLoc[1]);//OCC349
  theZ             = TMat[2][0] * theX + TMat[2][1] * theY + TMat[2][2] * theZ + (theVPO ? 0 : TLoc[2]);//OCC349
  theX             = Xt;
  theY             = Yt;
}

//=======================================================================
//function : TTMultiply
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::TTMultiply (Standard_Real& theX,
                                   Standard_Real& theY,
                                   Standard_Real& theZ,
                                   const Standard_Boolean theVPO) const
{
  Standard_Real Xt = TTMa[0][0] * theX + TTMa[0][1] * theY + TTMa[0][2] * theZ + (theVPO ? 0 : TTLo[0]);//OCC349
  Standard_Real Yt = TTMa[1][0] * theX + TTMa[1][1] * theY + TTMa[1][2] * theZ + (theVPO ? 0 : TTLo[1]);//OCC349
  theZ             = TTMa[2][0] * theX + TTMa[2][1] * theY + TTMa[2][2] * theZ + (theVPO ? 0 : TTLo[2]);//OCC349
  theX             = Xt;
  theY             = Yt;
}

//=======================================================================
//function : TIMultiply
//purpose  :
//=======================================================================
void HLRBRep_PolyAlgo::TIMultiply (Standard_Real& theX,
                                   Standard_Real& theY,
                                   Standard_Real& theZ,
                                   const Standard_Boolean theVPO) const
{
  Standard_Real Xt = TIMa[0][0] * theX + TIMa[0][1] * theY + TIMa[0][2] * theZ + (theVPO ? 0 : TILo[0]);//OCC349
  Standard_Real Yt = TIMa[1][0] * theX + TIMa[1][1] * theY + TIMa[1][2] * theZ + (theVPO ? 0 : TILo[1]);//OCC349
  theZ             = TIMa[2][0] * theX + TIMa[2][1] * theY + TIMa[2][2] * theZ + (theVPO ? 0 : TILo[2]);//OCC349
  theX             = Xt;
  theY             = Yt;
}

//=======================================================================
//function : Hide
//purpose  :
//=======================================================================
HLRAlgo_BiPoint::PointsT& HLRBRep_PolyAlgo::Hide (HLRAlgo_EdgeStatus& theStatus,
                                                  TopoDS_Shape& theShape,
                                                  Standard_Boolean& theReg1,
                                                  Standard_Boolean& theRegn,
                                                  Standard_Boolean& theOutl,
                                                  Standard_Boolean& theIntl)
{
  Standard_Integer anIndex = 0;
  HLRAlgo_BiPoint::PointsT& aPoints = myAlgo->Hide (theStatus, anIndex, theReg1, theRegn, theOutl, theIntl);
  theShape = theIntl
           ? myFMap (anIndex)
           : myEMap (anIndex);
  return aPoints;
}

//=======================================================================
//function : Show
//purpose  :
//=======================================================================
HLRAlgo_BiPoint::PointsT& HLRBRep_PolyAlgo::Show (TopoDS_Shape& theShape,
                                                  Standard_Boolean& theReg1,
                                                  Standard_Boolean& theRegn,
                                                  Standard_Boolean& theOutl,
                                                  Standard_Boolean& theIntl)
{
  Standard_Integer anIndex = 0;
  HLRAlgo_BiPoint::PointsT& aPoints = myAlgo->Show (anIndex, theReg1, theRegn, theOutl, theIntl);
  theShape = theIntl
           ? myFMap (anIndex)
           : myEMap (anIndex);
  return aPoints;
}

//=======================================================================
//function : OutLinedShape
//purpose  :
//=======================================================================
TopoDS_Shape HLRBRep_PolyAlgo::OutLinedShape (const TopoDS_Shape& theShape) const
{
  if (theShape.IsNull())
  {
    return TopoDS_Shape();
  }

  TopoDS_Shape aResult;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound (TopoDS::Compound (aResult));
  aBuilder.Add (aResult, theShape);
  if (myFMap.IsEmpty())
  {
    return aResult;
  }

  TopTools_MapOfShape aMap;
  {
    TopExp_Explorer aShapeExp;
    for (aShapeExp.Init (theShape, TopAbs_EDGE); aShapeExp.More(); aShapeExp.Next())
    {
      aMap.Add (aShapeExp.Current());
    }
    for (aShapeExp.Init (theShape, TopAbs_FACE); aShapeExp.More(); aShapeExp.Next())
    {
      aMap.Add (aShapeExp.Current());
    }
  }

  const NCollection_Array1<Handle(HLRAlgo_PolyShellData)>& aShell = myAlgo->PolyShell();
  const Standard_Integer aNbShells = aShell.Upper();
  HLRAlgo_ListIteratorOfListOfBPoint aBPntIter;
  for (Standard_Integer aShellIter = 1; aShellIter <= aNbShells; ++aShellIter)
  {
    const HLRAlgo_ListOfBPoint& aList = aShell.Value (aShellIter)->Edges();
    for (aBPntIter.Initialize (aList); aBPntIter.More(); aBPntIter.Next())
    {
      HLRAlgo_BiPoint& aBP = aBPntIter.ChangeValue();
      if (aBP.IntLine())
      {
        const HLRAlgo_BiPoint::IndicesT& aIndices = aBP.Indices();
        if (aMap.Contains (myFMap (aIndices.ShapeIndex)))
        {
          const HLRAlgo_BiPoint::PointsT& aPoints = aBP.Points();
          aBuilder.Add (aResult, BRepLib_MakeEdge (aPoints.Pnt1, aPoints.Pnt2));
        }
      }
    }
  }

  return aResult;
}
