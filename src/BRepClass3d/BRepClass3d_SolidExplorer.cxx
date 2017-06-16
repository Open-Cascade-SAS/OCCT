// Created on: 1994-03-10
// Created by: Laurent BUCHARD
// Copyright (c) 1994-1999 Matra Datavision
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

// Modifed:     Porting NT 7-5-97 DPF (stdio.h)
//              Apr 16 2002 eap, classification against infinite solid (occ299)


//  Modified by skv - Thu Sep  4 12:29:30 2003 OCC578

//-- Process the case of a hole!!
#define REJECTION 1

//-- To printf on NT

#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <BRepClass3d_DataMapIteratorOfMapOfInter.hxx>
#include <BRepClass3d_SolidExplorer.hxx>
#include <BRepClass_Edge.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepClass_FacePassiveClassifier.hxx>
#include <BRepTools.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <ElCLib.hxx>
#include <Extrema_ExtPS.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <IntCurvesFace_Intersector.hxx>
#include <Precision.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopExp.hxx>

#include <stdio.h>
//OCC454(apo)->
//<-OCC454(apo)
//=======================================================================
//function : FindAPointInTheFace
//purpose  : Compute a point P in the face  F. Param is a Real in
//           ]0,1[ and   is  used to  initialise  the algorithm. For
//           different values , different points are returned.
//=======================================================================
Standard_Boolean BRepClass3d_SolidExplorer::FindAPointInTheFace
(const TopoDS_Face& _face,
 gp_Pnt& APoint_,
 Standard_Real& param_)
{
  Standard_Real u,v;
  Standard_Boolean r = FindAPointInTheFace(_face,APoint_,u,v,param_);
  return r;
}

//=======================================================================
//function : FindAPointInTheFace
//purpose  : 
//=======================================================================

Standard_Boolean BRepClass3d_SolidExplorer::FindAPointInTheFace
(const TopoDS_Face& _face,
 gp_Pnt& APoint_,
 Standard_Real& u_, Standard_Real& v_,
 Standard_Real& param_)
{
  gp_Vec aVecD1U, aVecD1V;
  return FindAPointInTheFace (_face, APoint_, u_, v_, param_, aVecD1U, aVecD1V);
}

//=======================================================================
//function : FindAPointInTheFace
//purpose  : 
//=======================================================================

Standard_Boolean BRepClass3d_SolidExplorer::FindAPointInTheFace
(const TopoDS_Face& _face,
 gp_Pnt& APoint_,
 Standard_Real& u_, Standard_Real& v_,
 Standard_Real& param_,
 gp_Vec& theVecD1U,
 gp_Vec& theVecD1V)
{
  TopoDS_Face face = _face;
  face.Orientation (TopAbs_FORWARD);

  TopExp_Explorer faceexplorer;
  BRepAdaptor_Curve2d c;
  gp_Vec2d T;
  gp_Pnt2d P;

  for (faceexplorer.Init(face,TopAbs_EDGE); 
       faceexplorer.More(); 
       faceexplorer.Next())
  {
    TopoDS_Edge Edge = TopoDS::Edge (faceexplorer.Current());
    c.Initialize (Edge, face);
    c.D1((c.LastParameter() - c.FirstParameter()) * param_ + c.FirstParameter(),P,T);

    Standard_Real x = T.X();
    Standard_Real y = T.Y();
    if (Edge.Orientation() == TopAbs_FORWARD)
    {
      T.SetCoord (-y,  x);
    }
    else
    {
      T.SetCoord ( y, -x);
    }
    Standard_Real ParamInit = RealLast();
    Standard_Real TolInit   = 0.00001;
    Standard_Boolean APointExist = Standard_False;

    BRepClass_FacePassiveClassifier FClassifier;

    T.Normalize();
    P.SetCoord (P.X() + TolInit * T.X(), P.Y() + TolInit * T.Y());
    FClassifier.Reset (gp_Lin2d (P, T), ParamInit, RealEpsilon()); //-- Length and Tolerance #######

    TopExp_Explorer otherfaceexplorer;
    Standard_Integer aNbEdges = 0;
    for (otherfaceexplorer.Init (face, TopAbs_EDGE);
         otherfaceexplorer.More(); 
         otherfaceexplorer.Next(), ++aNbEdges)
    {
      TopoDS_Edge OtherEdge = TopoDS::Edge (otherfaceexplorer.Current());
      if (OtherEdge.Orientation() != TopAbs_EXTERNAL && OtherEdge != Edge)
      {
        BRepClass_Edge AEdge (OtherEdge, face);
        FClassifier.Compare (AEdge, OtherEdge.Orientation());
        if (FClassifier.ClosestIntersection())
        {
          if(ParamInit > FClassifier.Parameter())
          {
            ParamInit = FClassifier.Parameter();
            APointExist = Standard_True;
          }
        }
      }
    }

    if (aNbEdges == 1)
    {
      BRepClass_Edge AEdge (Edge, face);
      FClassifier.Compare (AEdge, Edge.Orientation());
      if (FClassifier.ClosestIntersection())
      {
        if (ParamInit > FClassifier.Parameter())
        {
          ParamInit = FClassifier.Parameter();
          APointExist = Standard_True;
        }
      }
    }

    while (APointExist)
    { 
      ParamInit *= 0.41234;
      u_ = P.X() + ParamInit* T.X();
      v_ = P.Y() + ParamInit* T.Y();

      //Additional check
      BRepTopAdaptor_FClass2d Classifier(face, Precision::Confusion());
      gp_Pnt2d aPnt2d(u_, v_);
      TopAbs_State StateOfResultingPoint = Classifier.Perform(aPnt2d);
      if (StateOfResultingPoint != TopAbs_IN)
        return Standard_False;
      
      BRepAdaptor_Surface s;
      s.Initialize (face, Standard_False);
      s.D1 (u_, v_, APoint_, theVecD1U, theVecD1V);

      if(theVecD1U.CrossMagnitude(theVecD1V) > gp::Resolution())
        return Standard_True;

      if(ParamInit < Precision::PConfusion())
        return Standard_False;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : PointInTheFace
//purpose  : 
//=======================================================================

Standard_Boolean BRepClass3d_SolidExplorer::PointInTheFace
(const TopoDS_Face& Face,
 gp_Pnt& APoint_,
 Standard_Real& u_, Standard_Real& v_,
 Standard_Real& param_,
 Standard_Integer& IndexPoint,
 const Handle(BRepAdaptor_HSurface)& surf,
 const Standard_Real U1,
 const Standard_Real V1,
 const Standard_Real U2,
 const Standard_Real V2) const
{
  gp_Vec aVecD1U, aVecD1V;
  return PointInTheFace (Face, APoint_, u_, v_, param_, IndexPoint, surf,
                         U1, V1, U2, V2, aVecD1U, aVecD1V);
}

//=======================================================================
//function : ClassifyUVPoint
//purpose  : 
//=======================================================================

TopAbs_State BRepClass3d_SolidExplorer::ClassifyUVPoint
                   (const IntCurvesFace_Intersector& theIntersector,
                    const Handle(BRepAdaptor_HSurface)& theSurf,
                    const gp_Pnt2d& theP2d) const
{
  // first find if the point is near an edge/vertex
  gp_Pnt aP3d = theSurf->Value(theP2d.X(), theP2d.Y());
  BRepClass3d_BndBoxTreeSelectorPoint aSelectorPoint(myMapEV);
  aSelectorPoint.SetCurrentPoint(aP3d);
  Standard_Integer aSelsVE = myTree.Select(aSelectorPoint);
  if (aSelsVE > 0)
  {
    // The point is inside the tolerance area of vertices/edges => return ON state.
    return TopAbs_ON;
  }
  return theIntersector.ClassifyUVPoint(theP2d);
}

//=======================================================================
//function : PointInTheFace
//purpose  : 
//=======================================================================

Standard_Boolean BRepClass3d_SolidExplorer::PointInTheFace
(const TopoDS_Face& Face,
 gp_Pnt& APoint_,
 Standard_Real& u_, Standard_Real& v_,
 Standard_Real& param_,
 Standard_Integer& IndexPoint,
 const Handle(BRepAdaptor_HSurface)& surf,
 const Standard_Real U1,
 const Standard_Real V1,
 const Standard_Real U2,
 const Standard_Real V2,
 gp_Vec& theVecD1U,
 gp_Vec& theVecD1V) const
{
  Standard_Real u,du = (U2-U1)/6.0;
  Standard_Real v,dv = (V2-V1)/6.0;
  if(du<1e-12) du=1e-12;
  if(dv<1e-12) dv=1e-12;
  Standard_Boolean IsNotUper = !surf->IsUPeriodic(), IsNotVper = !surf->IsVPeriodic();
  Standard_Integer NbPntCalc=0;
  if(myMapOfInter.IsBound(Face)) { 
    void *ptr = (void*)(myMapOfInter.Find(Face));
    Standard_Boolean IsInside = Standard_True;
    if(IsNotUper)
    {
      IsInside = (u_ >= U1) && (u_ <= U2);
    }
    if(IsNotVper)
    {
      IsInside &= (v_ >= V1) && (v_ <= V2);
    }
    if(ptr) { 
      const IntCurvesFace_Intersector& TheIntersector = (*((IntCurvesFace_Intersector *)ptr));
      // Check if the point is already in the face
      if (IsInside && (ClassifyUVPoint(TheIntersector, surf, gp_Pnt2d(u_, v_)) == TopAbs_IN)) {
        gp_Pnt aPnt;
        surf->D1(u_, v_, aPnt, theVecD1U, theVecD1V);
        if (aPnt.SquareDistance(APoint_) < Precision::Confusion() * Precision::Confusion())
          return Standard_True;
      }

      //-- Take 4 points in each Quarter of surface
      //-- -> Index : 1 -> 16
      //-- 
      //--
      //--  Then take a matrix of points on a tight grid
      //--
      for(u=du+(U1+U2)*0.5; u<U2; u+=du) {          //--  0  X    u increases
        for(v=dv+(V1+V2)*0.5; v<V2; v+=dv) {        //--  0  0    v increases
          if(++NbPntCalc>=IndexPoint) { 
            if (ClassifyUVPoint(TheIntersector, surf, gp_Pnt2d(u, v)) == TopAbs_IN) {
              u_=u; v_=v;
              surf->D1 (u, v, APoint_, theVecD1U, theVecD1V);
              IndexPoint = NbPntCalc;
              return(Standard_True);
            }
          }
        }
      }
          
      for(u=-du+(U1+U2)*0.5; u>U1; u-=du) {         //--  0  0    u decreases
        for(v=-dv+(V1+V2)*0.5; v>V1; v-=dv) {       //--  X  0    v decreases
          if(++NbPntCalc>=IndexPoint) {
            if (ClassifyUVPoint(TheIntersector, surf, gp_Pnt2d(u, v)) == TopAbs_IN) {
              u_=u; v_=v;
              surf->D1 (u, v, APoint_, theVecD1U, theVecD1V);
              IndexPoint = NbPntCalc;
              return(Standard_True);
            }
          }
        }
      }
      for(u=-du+(U1+U2)*0.5; u>U1; u-=du) {         //--  X  0    u decreases
        for(v=dv+(V1+V2)*0.5; v<V2; v+=dv) {        //--  0  0    v increases
          if(++NbPntCalc>=IndexPoint) { 
            if (ClassifyUVPoint(TheIntersector, surf, gp_Pnt2d(u, v)) == TopAbs_IN) {
              u_=u; v_=v;
              surf->D1 (u, v, APoint_, theVecD1U, theVecD1V);
              IndexPoint = NbPntCalc;
              return(Standard_True);
            }
          }
        }
      }
      for(u=du+(U1+U2)*0.5; u<U2; u+=du) {         //--  0  0     u increases
        for(v=-dv+(V1+V2)*0.5; v>V1; v-=dv) {      //--  0  X     v decreases
          if(++NbPntCalc>=IndexPoint) {
            if (ClassifyUVPoint(TheIntersector, surf, gp_Pnt2d(u, v)) == TopAbs_IN) {
              u_=u; v_=v;
              surf->D1 (u, v, APoint_, theVecD1U, theVecD1V);
              IndexPoint = NbPntCalc;
              return(Standard_True);
            }
          }
        }
      }
      //-- the remainder
      du = (U2-U1)/37.0;
      dv = (V2-V1)/37.0;
      if(du<1e-12) du=1e-12;
      if(dv<1e-12) dv=1e-12;
      
      for(u=du+U1; u<U2; u+=du) { 
        for(v=dv+V1; v<V2; v+=dv) {
          if(++NbPntCalc>=IndexPoint) {
            if (ClassifyUVPoint(TheIntersector, surf, gp_Pnt2d(u, v)) == TopAbs_IN) {
              u_=u; v_=v;
              surf->D1 (u, v, APoint_, theVecD1U, theVecD1V);
              IndexPoint = NbPntCalc;
              return(Standard_True);
            }
          }
        }
      }
      u=(U1+U2)*0.5;
      v=(V1+V2)*0.5;
      if(++NbPntCalc>=IndexPoint) {
        if (ClassifyUVPoint(TheIntersector, surf, gp_Pnt2d(u, v)) == TopAbs_IN) {
          u_=u; v_=v;
          surf->D1 (u, v, APoint_, theVecD1U, theVecD1V);
          IndexPoint = NbPntCalc;
          return(Standard_True);
        }
      }
    }
    IndexPoint = NbPntCalc;
  }
  else { 
    //printf("BRepClass3d_SolidExplorer Face not found ds the map \n");
  }

  return BRepClass3d_SolidExplorer
    ::FindAPointInTheFace (Face,APoint_, u_, v_, param_, theVecD1U, theVecD1V);
}

//=======================================================================
//function : LimitInfiniteUV
//purpose  : Limit infinite parameters
//=======================================================================
static void LimitInfiniteUV (Standard_Real& U1,
                             Standard_Real& V1,
                             Standard_Real& U2,
                             Standard_Real& V2)
{
  Standard_Boolean
    infU1 = Precision::IsNegativeInfinite(U1),
    infV1 = Precision::IsNegativeInfinite(V1),
    infU2 = Precision::IsPositiveInfinite(U2),
    infV2 = Precision::IsPositiveInfinite(V2);

  if (infU1) U1 = -1e10;
  if (infV1) V1 = -1e10;
  if (infU2) U2 =  1e10;
  if (infV2) V2 =  1e10;
}
//=======================================================================
//function : IsInfiniteUV
//purpose  : 
//=======================================================================
static Standard_Integer IsInfiniteUV (Standard_Real& U1, 
                                      Standard_Real& V1,
                                      Standard_Real& U2, 
                                      Standard_Real& V2) 
{
  Standard_Integer aVal = 0;

  if (Precision::IsInfinite(U1))
    aVal |= 1;

  if (Precision::IsInfinite(V1))
    aVal |= 2;

  if (Precision::IsInfinite(U2))
    aVal |= 4;

  if (Precision::IsInfinite(V2))
    aVal |= 8;

  return aVal;
}
//<-OCC454
//  Modified by skv - Tue Sep 16 13:50:39 2003 OCC578 End
//=======================================================================
//function : OtherSegment
//purpose  : Returns  in <L>, <Par>  a segment having at least
//           one  intersection  with  the  shape  boundary  to
//           compute  intersections. 
//           The First Call to this method returns a line which
//           point to a point of the first face of the shape.
//           The Second Call provide a line to the second face
//           and so on. 
//=======================================================================
Standard_Integer BRepClass3d_SolidExplorer::OtherSegment(const gp_Pnt& P, 
                                                         gp_Lin& L, 
                                                         Standard_Real& _Par) 
{
  const Standard_Real TolU = Precision::PConfusion();
  const Standard_Real TolV = TolU;

  TopoDS_Face         face;
  TopExp_Explorer     faceexplorer;
  gp_Pnt APoint;
  gp_Vec aVecD1U, aVecD1V;
  Standard_Real maxscal=0;
  Standard_Boolean ptfound=Standard_False;
  Standard_Real Par;
  Standard_Real _u,_v;
  Standard_Integer IndexPoint=0;
  Standard_Integer NbPointsOK=0;
  Standard_Integer NbFacesInSolid=0;
  Standard_Boolean aRestr = Standard_True;
  Standard_Boolean aTestInvert = Standard_False;

  for(;;) { 
    myFirstFace++; 
    faceexplorer.Init(myShape,TopAbs_FACE);
    // look for point on face starting from myFirstFace
//  Modified by skv - Thu Sep  4 14:31:12 2003 OCC578 Begin
//     while (faceexplorer.More()) {
    NbFacesInSolid = 0;
    for (; faceexplorer.More(); faceexplorer.Next()) {
//  Modified by skv - Thu Sep  4 14:31:12 2003 OCC578 End
      NbFacesInSolid++;
      if (myFirstFace > NbFacesInSolid) continue;
      face = TopoDS::Face(faceexplorer.Current());

      Handle(BRepAdaptor_HSurface) surf = new BRepAdaptor_HSurface();
      if(aTestInvert)
      {
        BRepTopAdaptor_FClass2d aClass(face, Precision::Confusion());
        if(aClass.PerformInfinitePoint() == TopAbs_IN)
        {
          aRestr = Standard_False;
          if(myMapOfInter.IsBound(face))
          {
            myMapOfInter.UnBind(face);
            void *ptr = (void *)(new IntCurvesFace_Intersector(face, Precision::Confusion(),
                                                               aRestr, Standard_False));
            myMapOfInter.Bind(face,ptr);
          }
        }
        else
        {
          aRestr = Standard_True;
        }
      }
      surf->ChangeSurface().Initialize(face, aRestr);
      Standard_Real U1,V1,U2,V2;
      U1 = surf->FirstUParameter();
      V1 = surf->FirstVParameter();
      U2 = surf->LastUParameter();
      V2 = surf->LastVParameter();
      face.Orientation(TopAbs_FORWARD);
      //
      //avoid process faces from uncorrected shells
      const Standard_Real eps = Precision::PConfusion();
      Standard_Real epsU = Max(eps * Max(Abs(U2), Abs(U1)), eps);
      Standard_Real epsV = Max(eps * Max(Abs(V2), Abs(V1)), eps);
      if( Abs (U2 - U1) < epsU || Abs(V2 - V1) < epsV) {
        return 2;
      }
      //
      Standard_Real svmyparam=myParamOnEdge;
      //
      // Check if the point is on the face or the face is infinite.
      Standard_Integer anInfFlag = IsInfiniteUV(U1,V1,U2,V2);
      // default values
      _u = (U1 + U2) * 0.5;
      _v = (V1 + V2) * 0.5;

      GeomAdaptor_Surface GA(BRep_Tool::Surface(face));
      Extrema_ExtPS Ext(P, GA, TolU, TolV);
      //
      if (Ext.IsDone() && Ext.NbExt() > 0) {
        Standard_Integer i, iNear,  iEnd;
        Standard_Real  aUx, aVx, Dist2, Dist2Min;
        Extrema_POnSurf aPx;
        //
        iNear = 1;
        Dist2Min = Ext.SquareDistance(1);
        iEnd = Ext.NbExt();
        for (i = 2; i <= iEnd; i++) {
          aPx=Ext.Point(i);
          aPx.Parameter(aUx, aVx);
          if (aUx>=U1 && aUx<=U2 && aVx>=V1 && aVx<=V2) {
            Dist2 = Ext.SquareDistance(i);
            if (Dist2 < Dist2Min) {
              Dist2Min = Dist2; 
              iNear = i;
            }
          }
        }
        //
        Standard_Real aDist2Tresh=1.e-24;
        //
        if (Dist2Min<aDist2Tresh) {
          if (anInfFlag) {
            return 1;
          } 
          else {
            BRepClass_FaceClassifier classifier2d;
            Standard_Real            aU;
            Standard_Real            aV;

            (Ext.Point(iNear)).Parameter(aU, aV);

            gp_Pnt2d aPuv(aU, aV);

            classifier2d.Perform(face,aPuv,Precision::PConfusion());

            TopAbs_State aState = classifier2d.State();

            if (aState == TopAbs_IN || aState == TopAbs_ON) {
              return 1;
            }
            else {
              return 3; // skv - the point is on surface but outside face.
            }
          }
        }
        if (anInfFlag) {
          APoint = (Ext.Point(iNear)).Value();
          gp_Vec V(P,APoint);
          _Par = V.Magnitude(); 
          L = gp_Lin(P,V);
          ptfound=Standard_True;
          return 0;
        }

        // set the parameters found by extrema
        aPx = Ext.Point(iNear);
        aPx.Parameter(_u, _v);
        APoint = aPx.Value();
      }
      //The point is not ON the face or surface. The face is restricted.
      // find point in a face not too far from a projection of P on face
      do {
        if (PointInTheFace (face, APoint, _u, _v, myParamOnEdge, ++IndexPoint, surf,
                            U1, V1, U2, V2,
                            aVecD1U, aVecD1V))
        {
          ++NbPointsOK;
          gp_Vec V (P, APoint);
          Par = V.Magnitude();
          if (Par > gp::Resolution() &&
              aVecD1U.Magnitude() > gp::Resolution() &&
              aVecD1V.Magnitude() > gp::Resolution())
          {
            gp_Vec Norm = aVecD1U.Crossed (aVecD1V);
            Standard_Real tt = Norm.Magnitude();
            if (tt > gp::Resolution())
            {
              tt = Abs (Norm.Dot (V)) / (tt * Par);
              if (tt > maxscal)
              {
                maxscal = tt;
                L = gp_Lin (P, V);
                _Par = Par;
                ptfound = Standard_True;
                if (maxscal>0.2)
                {
                  myParamOnEdge=svmyparam;
                  return 0;
                }
              }
            }
          }
        }
      }
      while(IndexPoint<200 && NbPointsOK<16);

      myParamOnEdge=svmyparam;
      if(maxscal>0.2) {                  
        return 0;
      }


      //  Modified by skv - Thu Sep  4 14:32:14 2003 OCC578 Begin
      // Next is done in the main for(..) loop.
      //       faceexplorer.Next();
      //  Modified by skv - Thu Sep  4 14:32:14 2003 OCC578 End
      IndexPoint = 0;

      Standard_Boolean encoreuneface = faceexplorer.More();
      if(ptfound==Standard_False && encoreuneface==Standard_False) { 
        if(myParamOnEdge < 0.0001) { 
          //-- This case takes place when the point is on the solid
          //-- and this solid is reduced to a face 
          gp_Pnt PBidon(P.X()+1.0,P.Y(),P.Z());
          gp_Vec V(P,PBidon);
          Par= 1.0;
          _Par=Par;
          L  = gp_Lin(P,V);
          return 0;
        }
      }
    } //-- Exploration of the faces   

    if(NbFacesInSolid==0) { 
      _Par=0.0;
      myReject=Standard_True;
#ifdef OCCT_DEBUG
      cout<<"\nWARNING : BRepClass3d_SolidExplorer.cxx  (Solid without face)"<<endl;
#endif  
      return 0;
    }

    if(ptfound) {
      return 0;
    }
    myFirstFace = 0;
    if(myParamOnEdge==0.512345)  myParamOnEdge = 0.4;
    else if(myParamOnEdge==0.4)  myParamOnEdge = 0.6; 
    else if(myParamOnEdge==0.6)  myParamOnEdge = 0.3; 
    else if(myParamOnEdge==0.3)  myParamOnEdge = 0.7; 
    else if(myParamOnEdge==0.7)  myParamOnEdge = 0.2; 
    else if(myParamOnEdge==0.2)  myParamOnEdge = 0.8; 
    else if(myParamOnEdge==0.8)  myParamOnEdge = 0.1; 
    else if(myParamOnEdge==0.1)  myParamOnEdge = 0.9;
    //
    else {
      myParamOnEdge*=0.5;  
      if(myParamOnEdge < 0.0001) { 
        gp_Pnt PBidon(P.X()+1.0,P.Y(),P.Z());
        gp_Vec V(P,PBidon);
        Par= 1.0;
        _Par=Par;
        L  = gp_Lin(P,V);
        return 0;
      }
    }
    aTestInvert = Standard_True;
  } //-- for(;;) { ...  } 
}

//  Modified by skv - Thu Sep  4 12:30:14 2003 OCC578 Begin
//=======================================================================
//function : GetFaceSegmentIndex
//purpose  : Returns the index of face for which last segment is calculated.
//=======================================================================

Standard_Integer BRepClass3d_SolidExplorer::GetFaceSegmentIndex() const
{
  return myFirstFace;
}
//  Modified by skv - Thu Sep  4 12:30:14 2003 OCC578 End

//=======================================================================
//function : PointInTheFace
//purpose  : 
//=======================================================================

Standard_Boolean BRepClass3d_SolidExplorer::PointInTheFace
(const TopoDS_Face& _face,
 gp_Pnt& APoint_,
 Standard_Real& u_, Standard_Real& v_,
 Standard_Real& param_,
 Standard_Integer& IndexPoint) const  
{   
  TopoDS_Face Face = _face;
  Face.Orientation(TopAbs_FORWARD);
  Handle(BRepAdaptor_HSurface) surf = new BRepAdaptor_HSurface();
  surf->ChangeSurface().Initialize(Face);
  Standard_Real U1,V1,U2,V2;//,u,v;
  U1 = surf->FirstUParameter();
  V1 = surf->FirstVParameter();
  U2 = surf->LastUParameter();
  V2 = surf->LastVParameter();
  LimitInfiniteUV (U1,V1,U2,V2);
  return(PointInTheFace(Face,APoint_,u_,v_,param_,IndexPoint,surf,U1,V1,U2,V2));
}

//=======================================================================
//function : FindAPointInTheFace
//purpose  : 
//=======================================================================

Standard_Boolean BRepClass3d_SolidExplorer::FindAPointInTheFace
(const TopoDS_Face& _face,
 gp_Pnt& APoint_,
 Standard_Real& u_,Standard_Real& v_) 
{
  Standard_Real param = 0.1;
  Standard_Boolean r = FindAPointInTheFace(_face,APoint_,u_,v_,param);
  return r;
}

//=======================================================================
//function : FindAPointInTheFace
//purpose  : 
//=======================================================================

Standard_Boolean BRepClass3d_SolidExplorer::FindAPointInTheFace
(const TopoDS_Face& _face,
 gp_Pnt& APoint_) 
{ Standard_Real u,v;
  Standard_Boolean r = FindAPointInTheFace(_face,APoint_,u,v);
  return r;
}

//=======================================================================
//function : FindAPointInTheFace
//purpose  : 
//=======================================================================

Standard_Boolean BRepClass3d_SolidExplorer::FindAPointInTheFace
(const TopoDS_Face& _face,
 Standard_Real& u_,Standard_Real& v_) 
{ gp_Pnt APoint;
  Standard_Boolean r = FindAPointInTheFace(_face,APoint,u_,v_);
  return r;
}

//=======================================================================
//function : BRepClass3d_SolidExplorer
//purpose  : 
//=======================================================================

BRepClass3d_SolidExplorer::BRepClass3d_SolidExplorer() 
{
}

//=======================================================================
//function : BRepClass3d_SolidExplorer
//purpose  : 
//=======================================================================

BRepClass3d_SolidExplorer::BRepClass3d_SolidExplorer(const TopoDS_Shape& S)
{
  InitShape(S);
}

//=======================================================================
//function : ~BRepClass3d_SolidExplorer
//purpose  :
//=======================================================================

BRepClass3d_SolidExplorer::~BRepClass3d_SolidExplorer()
{
 Destroy() ;
}

//=======================================================================
//function : Destroy
//purpose  : C++: alias ~
//=======================================================================

void BRepClass3d_SolidExplorer::Destroy() { 
  BRepClass3d_DataMapIteratorOfMapOfInter iter(myMapOfInter);
  for(;iter.More();iter.Next()) { 
    void *ptr=iter.Value();
    if(ptr) { 
      delete (IntCurvesFace_Intersector *)ptr;
      myMapOfInter.ChangeFind(iter.Key()) = NULL;
    }
  }
  myMapOfInter.Clear();
}

//=======================================================================
//function : InitShape
//purpose  : 
//=======================================================================

void BRepClass3d_SolidExplorer::InitShape(const TopoDS_Shape& S)
{
  myMapEV.Clear();
  myTree.Clear();

  myShape = S;
  myFirstFace = 0;
  myParamOnEdge = 0.512345;
  //-- Exploring of the Map and removal of allocated objects
  
  
  BRepClass3d_DataMapIteratorOfMapOfInter iter(myMapOfInter);
  for(;iter.More();iter.Next()) { 
    void *ptr=iter.Value();
    if(ptr) { 
      delete (IntCurvesFace_Intersector *)ptr;
      myMapOfInter.ChangeFind(iter.Key()) = NULL;
    }
  }
  
  myMapOfInter.Clear();
  
  myReject = Standard_True; //-- case of infinite solid (without any face)

  TopExp_Explorer Expl;
  for(Expl.Init(S,TopAbs_FACE);
      Expl.More();
      Expl.Next()) { 
    const TopoDS_Face Face = TopoDS::Face(Expl.Current());
    void *ptr = (void *)(new IntCurvesFace_Intersector(Face,Precision::Confusion(),Standard_True, Standard_False));
    myMapOfInter.Bind(Face,ptr);
    myReject=Standard_False;  //-- at least one face in the solid 
  }
  
#ifdef OCCT_DEBUG
  if(myReject) { 
    cout<<"\nWARNING : BRepClass3d_SolidExplorer.cxx  (Solid without face)"<<endl;
  }
#endif

#if REJECTION
  BRepBndLib::Add(myShape,myBox);
#endif

  // since the internal/external parts should be avoided in tree filler,
  // there is no need to add these parts in the EV map as well
  TopExp_Explorer aExpF(myShape, TopAbs_FACE);
  for (; aExpF.More(); aExpF.Next()) {
    const TopoDS_Shape& aF = aExpF.Current();
    //
    TopAbs_Orientation anOrF = aF.Orientation();
    if (anOrF == TopAbs_INTERNAL || anOrF == TopAbs_EXTERNAL) {
      continue;
    }
    //
    TopExp_Explorer aExpE(aF, TopAbs_EDGE);
    for (; aExpE.More(); aExpE.Next()) {
      const TopoDS_Shape& aE = aExpE.Current();
      //
      TopAbs_Orientation anOrE = aE.Orientation();
      if (anOrE == TopAbs_INTERNAL || anOrE == TopAbs_EXTERNAL) {
        continue;
      }
      //
      if (BRep_Tool::Degenerated(TopoDS::Edge(aE))) {
        continue;
      }
      //
      TopExp::MapShapes(aE, myMapEV);
    }
  }
  //
  // Fill mapEV with vertices and edges from shape
  NCollection_UBTreeFiller <Standard_Integer, Bnd_Box> aTreeFiller (myTree);
  //
  Standard_Integer i, aNbEV = myMapEV.Extent();
  for (i = 1; i <= aNbEV; ++i) {
    const TopoDS_Shape& aS = myMapEV(i);
    //
    Bnd_Box aBox;
    BRepBndLib::Add(aS, aBox);
    aTreeFiller.Add(i, aBox);
  }
  aTreeFiller.Fill();
}

//=======================================================================
//function : Reject
//purpose  : Should return True if P outside of bounding vol. of the shape
//=======================================================================

//Standard_Boolean  BRepClass3d_SolidExplorer::Reject(const gp_Pnt& P) const 
Standard_Boolean  BRepClass3d_SolidExplorer::Reject(const gp_Pnt& ) const 
{
  return(myReject);  // case of solid without face 
}

//=======================================================================
//function : InitShell
//purpose  : Starts an exploration of the shells.
//=======================================================================

void BRepClass3d_SolidExplorer::InitShell()  
{
  myShellExplorer.Init(myShape,TopAbs_SHELL);
}

//=======================================================================
//function : MoreShell
//purpose  : Returns True if there is a current shell. 
//=======================================================================

Standard_Boolean BRepClass3d_SolidExplorer::MoreShell() const 
{ 
  return(myShellExplorer.More());
}

//=======================================================================
//function : NextShell
//purpose  : Sets the explorer to the next shell.
//=======================================================================

void BRepClass3d_SolidExplorer::NextShell() 
{ 
  myShellExplorer.Next();
}

//=======================================================================
//function : CurrentShell
//purpose  : Returns the current shell.
//=======================================================================

TopoDS_Shell BRepClass3d_SolidExplorer::CurrentShell() const 
{ 
  return(TopoDS::Shell(myShellExplorer.Current()));
}

//=======================================================================
//function : RejectShell
//purpose  : Returns True if the Shell is rejected.
//=======================================================================

Standard_Boolean BRepClass3d_SolidExplorer::RejectShell(const gp_Lin& ) const
{ 
  return(Standard_False); 
}

//=======================================================================
//function : InitFace
//purpose  : Starts an exploration of the faces of the current shell.
//=======================================================================

void BRepClass3d_SolidExplorer::InitFace()  
{
  myFaceExplorer.Init(TopoDS::Shell(myShellExplorer.Current()),TopAbs_FACE);
}

//=======================================================================
//function : MoreFace
//purpose  : Returns True if current face in current shell. 
//=======================================================================

Standard_Boolean BRepClass3d_SolidExplorer::MoreFace() const 
{ 
  return(myFaceExplorer.More());
}

//=======================================================================
//function : NextFace
//purpose  : Sets the explorer to the next Face of the current shell.
//=======================================================================

void BRepClass3d_SolidExplorer::NextFace() 
{ 
  myFaceExplorer.Next();
}

//=======================================================================
//function : CurrentFace
//purpose  : Returns the current face.
//=======================================================================

TopoDS_Face BRepClass3d_SolidExplorer::CurrentFace() const 
{ 
  return(TopoDS::Face(myFaceExplorer.Current()));
}

//=======================================================================
//function : RejectFace
//purpose  : returns True if the face is rejected.
//=======================================================================

Standard_Boolean BRepClass3d_SolidExplorer::RejectFace(const gp_Lin& ) const 
{ 
  return(Standard_False); 
}

#ifdef OCCT_DEBUG
#include <TopAbs_State.hxx>
#endif

//=======================================================================
//function : Segment
//purpose  : Returns  in <L>, <Par>  a segment having at least
//           one  intersection  with  the  shape  boundary  to
//           compute  intersections. 
//=======================================================================
Standard_Integer  BRepClass3d_SolidExplorer::Segment(const gp_Pnt& P, 
                                                     gp_Lin& L, 
                                                     Standard_Real& Par)  
{
  Standard_Integer bRetFlag;
  myFirstFace = 0;
  bRetFlag=OtherSegment(P,L,Par);
  return bRetFlag;
}

//=======================================================================
//function : Intersector
//purpose  : 
//=======================================================================

IntCurvesFace_Intersector&  BRepClass3d_SolidExplorer::Intersector(const TopoDS_Face& F) const  { 
  void *ptr = (void*)(myMapOfInter.Find(F));
  IntCurvesFace_Intersector& curr = (*((IntCurvesFace_Intersector *)ptr));
  return curr;
}

//=======================================================================
//function : Box
//purpose  : 
//=======================================================================

const Bnd_Box& BRepClass3d_SolidExplorer::Box() const { 
  return(myBox);
}

//=======================================================================
//function : DumpSegment
//purpose  : 
//=======================================================================

void BRepClass3d_SolidExplorer::DumpSegment(const gp_Pnt&,
                                            const gp_Lin&,
                                            const Standard_Real,
                                            const TopAbs_State) const
{
#ifdef OCCT_DEBUG
 
#endif
}

const TopoDS_Shape& BRepClass3d_SolidExplorer::GetShape() const 
{ 
  return(myShape);
}
