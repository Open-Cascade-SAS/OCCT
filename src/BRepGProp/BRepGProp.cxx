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

#include <BRepGProp.hxx>
#include <BRepGProp_Cinert.hxx>
#include <BRepGProp_Sinert.hxx>
#include <BRepGProp_Vinert.hxx>
#include <BRepGProp_MeshProps.hxx>
#include <BRepGProp_MeshCinert.hxx>
#include <BRepGProp_VinertGK.hxx>
#include <BRepGProp_Face.hxx>
#include <BRepGProp_Domain.hxx>
#include <TopoDS.hxx>
#include <BRepAdaptor_Curve.hxx>

#include <BRep_Tool.hxx>  
#include <TopTools_MapOfShape.hxx>
#include <BRepCheck_Shell.hxx>

#ifdef OCCT_DEBUG
static Standard_Integer AffichEps = 0;
#endif

static gp_Pnt roughBaryCenter(const TopoDS_Shape& S){
  Standard_Integer i;  TopExp_Explorer ex;
  gp_XYZ xyz(0,0,0);
  for (ex.Init(S,TopAbs_VERTEX), i = 0; ex.More(); ex.Next(), i++) 
    xyz += BRep_Tool::Pnt(TopoDS::Vertex(ex.Current())).XYZ();
  if (i > 0)
  {
    xyz /= i;
  }
  else
  {
    //Try using triangulation
    ex.Init(S, TopAbs_FACE);
    for (; ex.More(); ex.Next())
    {
      const TopoDS_Shape& aF = ex.Current();
      TopLoc_Location aLocDummy;
      const Handle(Poly_Triangulation)& aTri =
        BRep_Tool::Triangulation(TopoDS::Face(aF), aLocDummy);
      if (!aTri.IsNull() && aTri->NbNodes() > 0)
      {
        xyz = aTri->Node(1).XYZ();
        if (!aLocDummy.IsIdentity())
        {
          aLocDummy.Transformation().Transforms(xyz);
        }
        break;
      }
    }
  }
  return gp_Pnt(xyz);
}



void  BRepGProp::LinearProperties(const TopoDS_Shape& S, GProp_GProps& SProps, const Standard_Boolean SkipShared,
                                  const Standard_Boolean UseTriangulation)
{
  // find the origin
  gp_Pnt P(0,0,0);
  P.Transform(S.Location());
  SProps = GProp_GProps(P);

  BRepAdaptor_Curve   BAC;
  TopTools_MapOfShape anEMap;
  TopExp_Explorer ex;
  for (ex.Init(S,TopAbs_EDGE); ex.More(); ex.Next()) {
    const TopoDS_Edge& aE = TopoDS::Edge(ex.Current());
    if(SkipShared && !anEMap.Add(aE))
    {
      continue;
    }

    Handle(TColgp_HArray1OfPnt) theNodes;
    Standard_Boolean IsGeom = BRep_Tool::IsGeometric(aE);
    if (UseTriangulation || !IsGeom)
    {
      BRepGProp_MeshCinert::PreparePolygon(aE, theNodes);
    }
    if(!theNodes.IsNull())
    {
      BRepGProp_MeshCinert MG;
      MG.SetLocation(P);
      MG.Perform(theNodes->Array1());
      SProps.Add(MG);
    }
    else
    {
      if (IsGeom)
      {
        BAC.Initialize(aE);
        BRepGProp_Cinert CG(BAC, P);
        SProps.Add(CG);
      }
    }
  }
}

static Standard_Real surfaceProperties(const TopoDS_Shape& S, GProp_GProps& Props, const Standard_Real Eps, const Standard_Boolean SkipShared,
                                       const Standard_Boolean UseTriangulation)
{
  Standard_Integer i;
#ifdef OCCT_DEBUG
  Standard_Integer iErrorMax = 0;
#endif
  Standard_Real ErrorMax = 0.0, Error;
  TopExp_Explorer ex; 
  gp_Pnt P(roughBaryCenter(S));
  BRepGProp_Sinert G;  G.SetLocation(P);
  BRepGProp_MeshProps MG(BRepGProp_MeshProps::Sinert);  
  MG.SetLocation(P);

  BRepGProp_Face   BF;
  BRepGProp_Domain BD;
  TopTools_MapOfShape aFMap;
  TopLoc_Location aLocDummy;

  for (ex.Init(S, TopAbs_FACE), i = 1; ex.More(); ex.Next(), i++) {
    const TopoDS_Face& F = TopoDS::Face(ex.Current());
    if (SkipShared && !aFMap.Add(F))
    {
      continue;
    }

    Standard_Boolean NoSurf = Standard_False, NoTri = Standard_False;
    {
      const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface(F, aLocDummy);
      if (aSurf.IsNull())
      {
        NoSurf = Standard_True;
      }
      const Handle(Poly_Triangulation)& aTri = BRep_Tool::Triangulation(F, aLocDummy);
      if (aTri.IsNull() || aTri->NbNodes() == 0 || aTri->NbTriangles() == 0)
      {
        NoTri = Standard_True;
      }
      if (NoTri && NoSurf)
      {
        continue;
      }
    }

    if ((UseTriangulation && !NoTri) || (NoSurf && !NoTri))
    {
      TopAbs_Orientation anOri = F.Orientation();
      const Handle(Poly_Triangulation)& aTri = BRep_Tool::Triangulation(F, aLocDummy);
      MG.Perform(aTri, aLocDummy, anOri);
      Props.Add(MG);
    }
    else
    {
      BF.Load(F);
      Standard_Boolean IsNatRestr = (F.NbChildren() == 0);
      if (!IsNatRestr) BD.Init(F);
      if (Eps < 1.0) {
        G.Perform(BF, BD, Eps);
        Error = G.GetEpsilon();
        if (ErrorMax < Error) {
          ErrorMax = Error;
#ifdef OCCT_DEBUG
          iErrorMax = i;
#endif
        }
      }
      else {
        if (IsNatRestr) G.Perform(BF);
        else G.Perform(BF, BD);
      }
      Props.Add(G);
#ifdef OCCT_DEBUG
      if(AffichEps) std::cout<<"\n"<<i<<":\tEpsArea = "<< G.GetEpsilon();
#endif
    }
  }
#ifdef OCCT_DEBUG
  if(AffichEps) std::cout<<"\n-----------------\n"<<iErrorMax<<":\tMaxError = "<<ErrorMax<<"\n";
#endif
  return ErrorMax;
}
void  BRepGProp::SurfaceProperties(const TopoDS_Shape& S, GProp_GProps& Props, const Standard_Boolean SkipShared,
                                   const Standard_Boolean UseTriangulation)
{
  // find the origin
  gp_Pnt P(0,0,0);
  P.Transform(S.Location());
  Props = GProp_GProps(P);
  surfaceProperties(S,Props,1.0, SkipShared, UseTriangulation);
}
Standard_Real BRepGProp::SurfaceProperties(const TopoDS_Shape& S, GProp_GProps& Props, const Standard_Real Eps, const Standard_Boolean SkipShared){ 
  // find the origin
  gp_Pnt P(0,0,0);  P.Transform(S.Location());
  Props = GProp_GProps(P);
  Standard_Real ErrorMax = surfaceProperties(S,Props,Eps,SkipShared, Standard_False);
  return ErrorMax;
}

//=======================================================================
//function : volumeProperties
//purpose  : 
//=======================================================================

static Standard_Real volumeProperties(const TopoDS_Shape& S, GProp_GProps& Props, const Standard_Real Eps, const Standard_Boolean SkipShared,
                                      const Standard_Boolean UseTriangulation)
{
  Standard_Integer i;
#ifdef OCCT_DEBUG
  Standard_Integer iErrorMax = 0;
#endif
  Standard_Real ErrorMax = 0.0, Error = 0.0;
  TopExp_Explorer ex; 
  gp_Pnt P(roughBaryCenter(S)); 
  BRepGProp_Vinert G;  G.SetLocation(P);
  BRepGProp_MeshProps MG(BRepGProp_MeshProps::Vinert);
  MG.SetLocation(P);

  BRepGProp_Face   BF;
  BRepGProp_Domain BD;
  TopTools_MapOfShape aFwdFMap;
  TopTools_MapOfShape aRvsFMap;
  TopLoc_Location aLocDummy;

  for (ex.Init(S,TopAbs_FACE), i = 1; ex.More(); ex.Next(), i++) {
    const TopoDS_Face& F = TopoDS::Face(ex.Current());
    TopAbs_Orientation anOri = F.Orientation();
    Standard_Boolean isFwd = anOri == TopAbs_FORWARD;
    Standard_Boolean isRvs = Standard_False;
    if(!isFwd)
    {
      isRvs = anOri == TopAbs_REVERSED;
    }
    if(SkipShared)
    {
      if((isFwd && !aFwdFMap.Add(F)) || (isRvs && !aRvsFMap.Add(F)))
      {
        continue;
      }
    }
    Standard_Boolean NoSurf = Standard_False, NoTri = Standard_False;
    {
      const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface (F, aLocDummy);
      if (aSurf.IsNull())
      {
        NoSurf = Standard_True;
      }
      const Handle(Poly_Triangulation)& aTri = BRep_Tool::Triangulation(F, aLocDummy);
      if (aTri.IsNull() || aTri->NbNodes() == 0 || aTri->NbTriangles() == 0)
      {
        NoTri = Standard_True;
      }
      if (NoTri && NoSurf)
      {
        continue;
      }
    }

    if (isFwd || isRvs)
    {
      if ((UseTriangulation && !NoTri) || (NoSurf && !NoTri))
      {
        const Handle(Poly_Triangulation)& aTri = BRep_Tool::Triangulation(F, aLocDummy);
        MG.Perform(aTri, aLocDummy, anOri);
        Props.Add(MG);
      }
      else
      {
        BF.Load(F);
        Standard_Boolean IsNatRestr = (F.NbChildren () == 0);
        if (!IsNatRestr) BD.Init(F);
        if (Eps < 1.0) {
          G.Perform(BF, BD, Eps);
          Error = G.GetEpsilon();
          if (ErrorMax < Error) {
            ErrorMax = Error;
#ifdef OCCT_DEBUG
            iErrorMax = i;
#endif
          }
        }
        else {
          if (IsNatRestr) G.Perform(BF);
          else G.Perform(BF, BD);
        }
        Props.Add(G);
#ifdef OCCT_DEBUG
        if(AffichEps) std::cout<<"\n"<<i<<":\tEpsVolume = "<< G.GetEpsilon();
#endif
      }
    }
  }
#ifdef OCCT_DEBUG
  if(AffichEps) std::cout<<"\n-----------------\n"<<iErrorMax<<":\tMaxError = "<<ErrorMax<<"\n";
#endif
  return ErrorMax;
}
void  BRepGProp::VolumeProperties(const TopoDS_Shape& S, GProp_GProps& Props, const Standard_Boolean OnlyClosed, const Standard_Boolean SkipShared,
                                  const Standard_Boolean UseTriangulation)
{
  // find the origin
  gp_Pnt P(0,0,0);  P.Transform(S.Location());
  Props = GProp_GProps(P);
  if(OnlyClosed){
    TopTools_MapOfShape aShMap;
    TopExp_Explorer ex(S,TopAbs_SHELL);
    for (; ex.More(); ex.Next()) {
      const TopoDS_Shape& Sh = ex.Current();
      if(SkipShared && !aShMap.Add(Sh))
      {
        continue;
      }
      if(BRep_Tool::IsClosed(Sh)) volumeProperties(Sh,Props,1.0,SkipShared, UseTriangulation);
    }
  } else volumeProperties(S,Props,1.0,SkipShared, UseTriangulation);
}

//=======================================================================
//function : VolumeProperties
//purpose  : 
//=======================================================================

Standard_Real BRepGProp::VolumeProperties(const TopoDS_Shape& S, GProp_GProps& Props, 
  const Standard_Real Eps, const Standard_Boolean OnlyClosed, const Standard_Boolean SkipShared)
{ 
  // find the origin
  gp_Pnt P(0,0,0);  P.Transform(S.Location());
  Props = GProp_GProps(P);
  Standard_Integer i;
#ifdef OCCT_DEBUG
  Standard_Integer iErrorMax = 0;
#endif
  Standard_Real ErrorMax = 0.0, Error = 0.0;
  if(OnlyClosed){
    TopTools_MapOfShape aShMap;
    TopExp_Explorer ex(S,TopAbs_SHELL);
    for (i = 1; ex.More(); ex.Next(), i++) {
      const TopoDS_Shape& Sh = ex.Current();
      if(SkipShared && !aShMap.Add(Sh))
      {
        continue;
      }
      if(BRep_Tool::IsClosed(Sh)) {
        Error = volumeProperties(Sh,Props,Eps,SkipShared, Standard_False);
        if(ErrorMax < Error) {
          ErrorMax = Error;
#ifdef OCCT_DEBUG
          iErrorMax = i;
#endif
        }
      }
    }
  } else ErrorMax = volumeProperties(S,Props,Eps,SkipShared, Standard_False);
#ifdef OCCT_DEBUG
  if(AffichEps) std::cout<<"\n\n==================="<<iErrorMax<<":\tMaxEpsVolume = "<<ErrorMax<<"\n";
#endif
  return ErrorMax;
} 

//===========================================================================================//
// Volume properties by Gauss-Kronrod integration
//===========================================================================================//
//=======================================================================
//function : VolumePropertiesGK
//purpose  : 
//=======================================================================

static Standard_Real volumePropertiesGK(const TopoDS_Shape     &theShape,
  GProp_GProps     &theProps,
  const Standard_Real     theTol,
  const Standard_Boolean  IsUseSpan,
  const Standard_Boolean  CGFlag,
  const Standard_Boolean  IFlag, const Standard_Boolean SkipShared)
{
  TopExp_Explorer  anExp;
  anExp.Init(theShape, TopAbs_FACE);

  Standard_Real aTol = theTol;

  // Compute properties.
  gp_Pnt           aLoc(roughBaryCenter(theShape)); 
  BRepGProp_VinertGK aVProps;
  BRepGProp_Face   aPropFace(IsUseSpan);
  BRepGProp_Domain aPropDomain;
  Standard_Real    aLocalError;
  Standard_Real    anError = 0.;
  TopTools_MapOfShape aFwdFMap;
  TopTools_MapOfShape aRvsFMap;
  TopLoc_Location aLocDummy;

  aVProps.SetLocation(aLoc);

  for (; anExp.More(); anExp.Next()) {
    TopoDS_Face aFace = TopoDS::Face(anExp.Current());
    TopAbs_Orientation anOri = aFace.Orientation();
    Standard_Boolean isFwd = anOri == TopAbs_FORWARD;
    Standard_Boolean isRvs = Standard_False;
    if(!isFwd)
    {
      isRvs = anOri == TopAbs_REVERSED;
    }
    if(SkipShared)
    {
      if((isFwd && !aFwdFMap.Add(aFace)) || (isRvs && !aRvsFMap.Add(aFace)))
      {
        continue;
      }
    }
    {
      const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface (aFace, aLocDummy);
      if (aSurf.IsNull())
      {
        // skip faces without geometry
        continue;
      }
    }

    if (isFwd || isRvs){
        aPropFace.Load(aFace);

        Standard_Boolean IsNatRestr = (aFace.NbChildren () == 0);
        if(IsNatRestr)
          aLocalError = aVProps.Perform(aPropFace, aTol, CGFlag, IFlag);
        else {
          aPropDomain.Init(aFace);
          aLocalError = aVProps.Perform(aPropFace, aPropDomain, aTol, CGFlag, IFlag);
        }

        if (aLocalError < 0.)
          return aLocalError;

        anError += aLocalError;
        theProps.Add(aVProps);
    }
  }

  return anError;
}

//=======================================================================
//function : VolumePropertiesGK
//purpose  : 
//=======================================================================

Standard_Real BRepGProp::VolumePropertiesGK(const TopoDS_Shape     &S,
  GProp_GProps     &Props, 
  const Standard_Real     Eps,
  const Standard_Boolean  OnlyClosed,
  const Standard_Boolean  IsUseSpan,
  const Standard_Boolean  CGFlag,
  const Standard_Boolean  IFlag, const Standard_Boolean SkipShared)
{ 
  gp_Pnt        P(0,0,0);
  Standard_Real anError = 0.;

  P.Transform(S.Location());
  Props = GProp_GProps(P);

  if(OnlyClosed) {
    // To select closed shells.
    TopExp_Explorer  anExp;
    TopTools_ListOfShape aClosedShells;
    TopTools_MapOfShape aShMap;

    anExp.Init(S, TopAbs_SHELL);

    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Shape &aShell = anExp.Current();
      if(SkipShared && !aShMap.Add(aShell))
      {
        continue;
      }

      BRepCheck_Shell aChecker(TopoDS::Shell(aShell));
      BRepCheck_Status aStatus = aChecker.Closed(Standard_False);

      if(aStatus == BRepCheck_NoError) 
        aClosedShells.Append(aShell);

    }

    if (aClosedShells.IsEmpty())
      return -1.;

    // Compute the properties for each closed shell.
    Standard_Real aTol    = Eps;
    Standard_Real aLocalError;
    TopTools_ListIteratorOfListOfShape anIter(aClosedShells);

    for (; anIter.More(); anIter.Next()) {
      const TopoDS_Shape &aShell = anIter.Value();

      aLocalError = volumePropertiesGK(aShell, Props, aTol, IsUseSpan, CGFlag, IFlag,SkipShared);

      if (aLocalError < 0)
        return aLocalError;

      anError += aLocalError;
    }

  } else
    anError = volumePropertiesGK(S, Props, Eps, IsUseSpan, CGFlag, IFlag,SkipShared);

  Standard_Real vol = Props.Mass();
  if(vol > Epsilon(1.)) anError /= vol;
  return anError;
}

//=======================================================================
//function : VolumeProperties
//purpose  : 
//=======================================================================

static Standard_Real volumePropertiesGK(const TopoDS_Shape     &theShape,
  GProp_GProps     &theProps,
  const gp_Pln           &thePln,
  const Standard_Real     theTol,
  const Standard_Boolean  IsUseSpan,
  const Standard_Boolean  CGFlag,
  const Standard_Boolean  IFlag, const Standard_Boolean SkipShared)
{
  TopExp_Explorer  anExp;
  anExp.Init(theShape, TopAbs_FACE);

  Standard_Real aTol = theTol;

  // Compute properties.
  gp_Pnt           aLoc(roughBaryCenter(theShape)); 
  BRepGProp_VinertGK aVProps;
  BRepGProp_Face   aPropFace(IsUseSpan);
  BRepGProp_Domain aPropDomain;
  Standard_Real    aLocalError;
  Standard_Real    anError = 0.;
  TopTools_MapOfShape aFwdFMap;
  TopTools_MapOfShape aRvsFMap;
  TopLoc_Location aLocDummy;

  aVProps.SetLocation(aLoc);

  for (; anExp.More(); anExp.Next()) {
    TopoDS_Face aFace = TopoDS::Face(anExp.Current());
    TopAbs_Orientation anOri = aFace.Orientation();
    Standard_Boolean isFwd = anOri == TopAbs_FORWARD;
    Standard_Boolean isRvs = Standard_False;
    if(!isFwd)
    {
      isRvs = anOri == TopAbs_REVERSED;
    }
    if(SkipShared)
    {
      if((isFwd && !aFwdFMap.Add(aFace)) || (isRvs && !aRvsFMap.Add(aFace)))
      {
        continue;
      }
    }
    {
      const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface (aFace, aLocDummy);
      if (aSurf.IsNull())
      {
        // skip faces without geometry
        continue;
      }
    }

    if (isFwd || isRvs){
        aPropFace.Load(aFace);

        Standard_Boolean IsNatRestr = (aFace.NbChildren () == 0);
        if(IsNatRestr)
          aLocalError = aVProps.Perform(aPropFace, thePln, aTol, CGFlag, IFlag);
        else {
          aPropDomain.Init(aFace);
          aLocalError = aVProps.Perform(aPropFace, aPropDomain, thePln, aTol, CGFlag, IFlag);
        }

        if (aLocalError < 0.)
          return aLocalError;

        anError += aLocalError;
        theProps.Add(aVProps);
    }
  }

  return anError;
}

//=======================================================================
//function : VolumeProperties
//purpose  : 
//=======================================================================

Standard_Real BRepGProp::VolumePropertiesGK(const TopoDS_Shape     &S,
  GProp_GProps     &Props,
  const gp_Pln           &thePln,
  const Standard_Real     Eps,
  const Standard_Boolean  OnlyClosed,
  const Standard_Boolean  IsUseSpan,
  const Standard_Boolean  CGFlag,
  const Standard_Boolean  IFlag, const Standard_Boolean SkipShared)
{ 
  gp_Pnt        P(0,0,0);
  Standard_Real anError = 0.;

  P.Transform(S.Location());
  Props = GProp_GProps(P);

  if(OnlyClosed) {
    // To select closed shells.
    TopExp_Explorer  anExp;
    TopTools_ListOfShape aClosedShells;
    TopTools_MapOfShape aShMap;

    anExp.Init(S, TopAbs_SHELL);

    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Shape &aShell = anExp.Current();
      if(SkipShared && !aShMap.Add(aShell))
      {
        continue;
      }

      BRepCheck_Shell aChecker(TopoDS::Shell(aShell));
      BRepCheck_Status aStatus = aChecker.Closed(Standard_False);

      if(aStatus == BRepCheck_NoError) 
        aClosedShells.Append(aShell);

    }

    if (aClosedShells.IsEmpty())
      return -1.;

    // Compute the properties for each closed shell.
    Standard_Real aTol    = Eps;
    Standard_Real aLocalError;
    TopTools_ListIteratorOfListOfShape anIter(aClosedShells);

    for (; anIter.More(); anIter.Next()) {
      const TopoDS_Shape &aShell = anIter.Value();

      aLocalError = volumePropertiesGK(aShell, Props, thePln, aTol, IsUseSpan, CGFlag, IFlag,SkipShared);

      if (aLocalError < 0)
        return aLocalError;

      anError += aLocalError;
    }
  } else
    anError = volumePropertiesGK(S, Props, thePln, Eps, IsUseSpan, CGFlag, IFlag,SkipShared);

  Standard_Real vol = Props.Mass();
  if(vol > Epsilon(1.)) anError /= vol;

  return anError;
}
