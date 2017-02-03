// Created on: 1993-09-22
// Created by: Didier PIFFAULT
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


#include <AppCont_ContMatrices.hxx>
#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepLib.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <BRepMesh_Delaun.hxx>
#include <BRepMesh_Edge.hxx>
#include <BRepMesh_FastDiscret.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepMesh_Vertex.hxx>
#include <BRepTest.hxx>
#include <BRepTools.hxx>
#include <CSLib.hxx>
#include <CSLib_DerivativeStatus.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Marker3D.hxx>
#include <Draw_MarkerShape.hxx>
#include <Draw_Segment2D.hxx>
#include <DrawTrSurf.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeometryTest.hxx>
#include <gp_Pln.hxx>
#include <gp_Trsf.hxx>
#include <math.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <MeshTest.hxx>
#include <MeshTest_DrawableMesh.hxx>
#include <PLib.hxx>
#include <Poly_Connect.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Standard_Stream.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>

#include <stdio.h>
//epa Memory leaks test
//OAN: for triepoints
#ifdef _WIN32
Standard_IMPORT Draw_Viewer dout;
#endif

#define MAX2(X, Y)	(  Abs(X) > Abs(Y)? Abs(X) : Abs(Y) )
#define MAX3(X, Y, Z)	( MAX2 ( MAX2(X,Y) , Z) )



#define ONETHIRD 0.333333333333333333333333333333333333333333333333333333333333
#define TWOTHIRD 0.666666666666666666666666666666666666666666666666666666666666

#ifdef OCCT_DEBUG_MESH_CHRONO
#include <OSD_Chronometer.hxx>
Standard_Integer D0Control, D0Internal, D0Unif, D0Edges, NbControls;
OSD_Chronometer chTotal, chInternal, chControl, chUnif, chAddPoint;
OSD_Chronometer chEdges, chMaillEdges, chEtuInter, chLastControl, chStock;
OSD_Chronometer chAdd11, chAdd12, chAdd2, chUpdate, chPointValid;
OSD_Chronometer chIsos, chPointsOnIsos;
#endif

//=======================================================================
//function : incrementalmesh
//purpose  : 
//=======================================================================
static Standard_Integer incrementalmesh(Draw_Interpretor& di, Standard_Integer nbarg, const char** argv)
{
  if (nbarg < 3)
  {
    di << "\
Builds triangular mesh for the shape\n\
usage: incmesh Shape LinearDeflection [options]\n\
options:\n\
        -a val          angular deflection in deg\n\
                        (default ~28.64 deg = 0.5 rad)\n\n\
        -min            minimum size parameter limiting size of triangle's\n\
                        edges to prevent sinking into amplification in case\n\
                        of distorted curves and surfaces\n\n\
        -relative       notifies that relative deflection is used\n\
                        (switched off by default)\n\n\
        -int_vert_off   disables insertion of internal vertices into mesh\n\
                        (enabled by default)\n\
        -surf_def_off   disables control of deflection of mesh from real\n\
                        surface (enabled by default)\n\
        -parallel       enables parallel execution (switched off by default)\n\
        -adaptive       enables adaptive computation of minimal value in parametric space\n";
    return 0;
  }

  TopoDS_Shape aShape = DBRep::Get(argv[1]);
  if (aShape.IsNull())
  {
    di << " Null shapes are not allowed here\n";
    return 0;
  }

  Standard_Real aLinDeflection     = Max(Draw::Atof(argv[2]), Precision::Confusion());
  Standard_Real aAngDeflection     = 0.5;
  Standard_Real aMinSize           = Precision::Confusion();
  Standard_Boolean isRelative      = Standard_False;
  Standard_Boolean isInParallel    = Standard_False;
  Standard_Boolean isIntVertices   = Standard_True;
  Standard_Boolean isControlSurDef = Standard_True;
  Standard_Boolean isAdaptiveMin   = Standard_False;

  if (nbarg > 3)
  {
    Standard_Integer i = 3;
    while (i < nbarg)
    {
      TCollection_AsciiString aOpt(argv[i++]);
      aOpt.LowerCase();

      if (aOpt == "")
        continue;
      else if (aOpt == "-relative")
        isRelative = Standard_True;
      else if (aOpt == "-parallel")
        isInParallel = Standard_True;
      else if (aOpt == "-int_vert_off")
        isIntVertices = Standard_False;
      else if (aOpt == "-surf_def_off")
        isControlSurDef = Standard_False;
      else if (aOpt == "-adaptive")
        isAdaptiveMin   = Standard_True;
      else if (i < nbarg)
      {
        Standard_Real aVal = Draw::Atof(argv[i++]);
        if (aOpt == "-a")
          aAngDeflection = aVal * M_PI / 180.;
        else if (aOpt == "-min")
          aMinSize = aVal;
        else
          --i;
      }
    }
  }

  di << "Incremental Mesh, multi-threading "
     << (isInParallel ? "ON" : "OFF") << "\n";

  BRepMesh_FastDiscret::Parameters aMeshParams;
  aMeshParams.Deflection = aLinDeflection;
  aMeshParams.Angle = aAngDeflection;
  aMeshParams.Relative =  isRelative;
  aMeshParams.InParallel = isInParallel;
  aMeshParams.MinSize = aMinSize;
  aMeshParams.InternalVerticesMode = isIntVertices;
  aMeshParams.ControlSurfaceDeflection = isControlSurDef;
  aMeshParams.AdaptiveMin = isAdaptiveMin;
  
  BRepMesh_IncrementalMesh aMesher (aShape, aMeshParams);

  di << "Meshing statuses: ";
  Standard_Integer statusFlags = aMesher.GetStatusFlags();
  if( !statusFlags )
  {
    di << "NoError";
  }
  else
  {
    Standard_Integer i;
    for( i = 0; i < 4; i++ )
    {
      if( (statusFlags >> i) & (Standard_Integer)1 )
      {
        switch(i+1)
        {
          case 1:
            di << "OpenWire ";
            break;
          case 2:
            di << "SelfIntersectingWire ";
            break;
          case 3:
            di << "Failure ";
            break;
          case 4:
            di << "ReMesh ";
            break;
        }
      }
    }
  }

  return 0;
}

//=======================================================================
//function : tessellate
//purpose  : 
//=======================================================================
static Standard_Integer tessellate (Draw_Interpretor& /*di*/, Standard_Integer nbarg, const char** argv)
{
  if (nbarg != 5)
  {
    std::cerr << "Builds regular triangulation with specified number of triangles\n"
                 "    Usage: tessellate result {surface|face} nbu nbv\n"
                 "    Triangulation is put into the face with natural bounds (result);\n"
                 "    it will have 2*nbu*nbv triangles and (nbu+1)*(nbv+1) nodes";
    return 1;
  }

  const char *aResName = argv[1];
  const char *aSrcName = argv[2];
  int aNbU = Draw::Atoi (argv[3]);
  int aNbV = Draw::Atoi (argv[4]);

  if (aNbU <= 0 || aNbV <= 0)
  {
    std::cerr << "Error: Arguments nbu and nbv must be both greater than 0\n";
    return 1;
  }

  Handle(Geom_Surface) aSurf = DrawTrSurf::GetSurface(aSrcName);
  double aUMin, aUMax, aVMin, aVMax;
  if (! aSurf.IsNull())
  {
    aSurf->Bounds (aUMin, aUMax, aVMin, aVMax);
  }
  else
  {
    TopoDS_Shape aShape = DBRep::Get(aSrcName);
    if (aShape.IsNull() || aShape.ShapeType() != TopAbs_FACE)
    {
      std::cerr << "Error: " << aSrcName << " is not a face\n";
      return 1;
    }
    TopoDS_Face aFace = TopoDS::Face (aShape);
    aSurf = BRep_Tool::Surface (aFace);
    if (aSurf.IsNull())
    {
      std::cerr << "Error: Face " << aSrcName << " has no surface\n";
      return 1;
    }

    BRepTools::UVBounds (aFace, aUMin, aUMax, aVMin, aVMax);
  }
  if (Precision::IsInfinite (aUMin) || Precision::IsInfinite (aUMax) || 
      Precision::IsInfinite (aVMin) || Precision::IsInfinite (aVMax))
  {
    std::cerr << "Error: surface has infinite parametric range, aborting\n";
    return 1;
  }

  BRepBuilderAPI_MakeFace aFaceMaker (aSurf, aUMin, aUMax, aVMin, aVMax, Precision::Confusion());
  if (! aFaceMaker.IsDone())
  {
    std::cerr << "Error: cannot build face with natural bounds, aborting\n";
    return 1;
  }
  TopoDS_Face aFace = aFaceMaker;

  // create triangulation
  int aNbNodes = (aNbU + 1) * (aNbV + 1);
  int aNbTriangles = 2 * aNbU * aNbV;
  Handle(Poly_Triangulation) aTriangulation =
    new Poly_Triangulation (aNbNodes, aNbTriangles, Standard_False);

  // fill nodes
  TColgp_Array1OfPnt &aNodes = aTriangulation->ChangeNodes();
  GeomAdaptor_Surface anAdSurf (aSurf);
  double aDU = (aUMax - aUMin) / aNbU;
  double aDV = (aVMax - aVMin) / aNbV;
  for (int iU = 0, iShift = 1; iU <= aNbU; iU++, iShift += aNbV + 1)
  {
    double aU = aUMin + iU * aDU;
    for (int iV = 0; iV <= aNbV; iV++)
    {
      double aV = aVMin + iV * aDV;
      gp_Pnt aP = anAdSurf.Value (aU, aV);
      aNodes.SetValue (iShift + iV, aP);
    }
  }

  // fill triangles
  Poly_Array1OfTriangle &aTriangles = aTriangulation->ChangeTriangles();
  for (int iU = 0, iShift = 1, iTri = 0; iU < aNbU; iU++, iShift += aNbV + 1)
  {
    for (int iV = 0; iV < aNbV; iV++)
    {
      int iBase = iShift + iV;
      Poly_Triangle aTri1 (iBase, iBase + aNbV + 2, iBase + 1);
      Poly_Triangle aTri2 (iBase, iBase + aNbV + 1, iBase + aNbV + 2);
      aTriangles.SetValue (++iTri, aTri1);
      aTriangles.SetValue (++iTri, aTri2);
    }
  }

  // put triangulation to face
  BRep_Builder B;
  B.UpdateFace (aFace, aTriangulation);

  // fill edge polygons
  TColStd_Array1OfInteger aUMinIso (1, aNbV + 1), aUMaxIso (1, aNbV + 1);
  for (int iV = 0; iV <= aNbV; iV++)
  {
    aUMinIso.SetValue (1 + iV, 1 + iV);
    aUMaxIso.SetValue (1 + iV, 1 + iV + aNbU * (1 + aNbV));
  }
  TColStd_Array1OfInteger aVMinIso (1, aNbU + 1), aVMaxIso (1, aNbU + 1);
  for (int iU = 0; iU <= aNbU; iU++)
  {
    aVMinIso.SetValue (1 + iU,  1 + iU  * (1 + aNbV));
    aVMaxIso.SetValue (1 + iU, (1 + iU) * (1 + aNbV));
  }
  Handle(Poly_PolygonOnTriangulation) aUMinPoly = new Poly_PolygonOnTriangulation (aUMinIso);
  Handle(Poly_PolygonOnTriangulation) aUMaxPoly = new Poly_PolygonOnTriangulation (aUMaxIso);
  Handle(Poly_PolygonOnTriangulation) aVMinPoly = new Poly_PolygonOnTriangulation (aVMinIso);
  Handle(Poly_PolygonOnTriangulation) aVMaxPoly = new Poly_PolygonOnTriangulation (aVMaxIso);
  for (TopExp_Explorer exp (aFace, TopAbs_EDGE); exp.More(); exp.Next())
  {
    TopoDS_Edge anEdge = TopoDS::Edge (exp.Current());
    Standard_Real aFirst, aLast;
    Handle(Geom2d_Curve) aC = BRep_Tool::CurveOnSurface (anEdge, aFace, aFirst, aLast);
    gp_Pnt2d aPFirst = aC->Value (aFirst);
    gp_Pnt2d aPLast  = aC->Value (aLast);
    if (Abs (aPFirst.X() - aPLast.X()) < 0.1 * (aUMax - aUMin)) // U=const
    {
      if (BRep_Tool::IsClosed (anEdge, aFace))
        B.UpdateEdge (anEdge, aUMinPoly, aUMaxPoly, aTriangulation);
      else
        B.UpdateEdge (anEdge, (aPFirst.X() < 0.5 * (aUMin + aUMax) ? aUMinPoly : aUMaxPoly), aTriangulation);
    }
    else // V=const
    {
      if (BRep_Tool::IsClosed (anEdge, aFace))
        B.UpdateEdge (anEdge, aVMinPoly, aVMaxPoly, aTriangulation);
      else
        B.UpdateEdge (anEdge, (aPFirst.Y() < 0.5 * (aVMin + aVMax) ? aVMinPoly : aVMaxPoly), aTriangulation);
    }
  }

  DBRep::Set (aResName, aFace);
  return 0;
}

//=======================================================================
//function : MemLeakTest
//purpose  : 
//=======================================================================

static Standard_Integer MemLeakTest(Draw_Interpretor&, Standard_Integer /*nbarg*/, const char** /*argv*/)
{
  for(int i=0;i<10000;i++)
  {
    BRepBuilderAPI_MakePolygon w(gp_Pnt(0,0,0),gp_Pnt(0,100,0),gp_Pnt(20,100,0),gp_Pnt(20,0,0));
    w.Close();     
    TopoDS_Wire wireShape( w.Wire());
    BRepBuilderAPI_MakeFace faceBuilder(wireShape);          
    TopoDS_Face f( faceBuilder.Face());
    BRepMesh_IncrementalMesh im(f,1);
    BRepTools::Clean(f);      
  }
  return 0;
}

//=======================================================================
//function : fastdiscret
//purpose  : 
//=======================================================================

static Standard_Integer fastdiscret(Draw_Interpretor& di, Standard_Integer nbarg, const char** argv)
{
  if (nbarg < 3) return 1;

  TopoDS_Shape S = DBRep::Get(argv[1]);
  if (S.IsNull()) return 1;

  const Standard_Real d = Draw::Atof(argv[2]);

  Bnd_Box B;
  BRepBndLib::Add(S,B);
  BRepMesh_FastDiscret::Parameters aParams;
  aParams.Deflection = d;
  aParams.Angle = 0.5;
  BRepMesh_FastDiscret MESH(B,aParams);

  //Standard_Integer NbIterations = MESH.NbIterations();
  //if (nbarg > 4) NbIterations = Draw::Atoi(argv[4]);
  //MESH.NbIterations() = NbIterations;

  di<<"Starting FastDiscret with :\n";
  di<<"  Deflection="<<d<<"\n";
  di<<"  Angle="<<0.5<<"\n";

  Handle(Poly_Triangulation) T;
  BRep_Builder aBuilder;
  TopExp_Explorer ex;

  // Clear existing triangulations
  for (ex.Init(S, TopAbs_FACE); ex.More(); ex.Next())
    aBuilder.UpdateFace(TopoDS::Face(ex.Current()),T);

  MESH.Perform(S);

  TopoDS_Compound aCompGood, aCompFailed, aCompViolating;

  TopLoc_Location L;
  Standard_Integer nbtriangles = 0, nbnodes = 0, nbfailed = 0, nbviolating = 0;
  Standard_Real maxdef = 0.0;
  for (ex.Init(S, TopAbs_FACE); ex.More(); ex.Next())
  {
    T = BRep_Tool::Triangulation(TopoDS::Face(ex.Current()),L);
    if (T.IsNull())
    {
      nbfailed++;
      if (aCompFailed.IsNull())
        aBuilder.MakeCompound(aCompFailed);
      aBuilder.Add(aCompFailed,ex.Current());
    }
    else
    {
      nbtriangles += T->NbTriangles();
      nbnodes += T->NbNodes();
      if (T->Deflection() > maxdef) maxdef = T->Deflection();
      if (T->Deflection() > d)
      {
        nbviolating++;
        if (aCompViolating.IsNull())
          aBuilder.MakeCompound(aCompViolating);
        aBuilder.Add(aCompViolating,ex.Current());
      }
      else
      {
        if (aCompGood.IsNull())
          aBuilder.MakeCompound(aCompGood);
        aBuilder.Add(aCompGood,ex.Current());
      }
    }
  }

  if (!aCompGood.IsNull())
  {
    char name[256];
    strcpy(name,argv[1]);
    strcat(name,"_good");
    DBRep::Set(name,aCompGood);
  }
  if (!aCompFailed.IsNull())
  {
    char name[256];
    strcpy(name,argv[1]);
    strcat(name,"_failed");
    DBRep::Set(name,aCompFailed);
  }
  if (!aCompViolating.IsNull())
  {
    char name[256];
    strcpy(name,argv[1]);
    strcat(name,"_violating");
    DBRep::Set(name,aCompViolating);
  }

  di<<"FastDiscret completed with :\n";
  di<<"  MaxDeflection="<<maxdef<<"\n";
  di<<"  NbNodes="<<nbnodes<<"\n";
  di<<"  NbTriangles="<<nbtriangles<<"\n";
  di<<"  NbFailed="<<nbfailed<<"\n";
  di<<"  NbViolating="<<nbviolating<<"\n";

  return 0;
}


//=======================================================================
//function : triangule
//purpose  : 
//=======================================================================


class BRepMesh_Couple
{
public:
  BRepMesh_Couple() { myI1 = myI2 = 0; }
  BRepMesh_Couple(const Standard_Integer I1,
    const Standard_Integer I2)
  { myI1 = I1; myI2 = I2; }

  Standard_Integer myI1;
  Standard_Integer myI2;
};

inline Standard_Boolean IsEqual(const BRepMesh_Couple& one,
                                const BRepMesh_Couple& other)
{
  if (one.myI1 == other.myI1 &&
    one.myI2 == other.myI2) return Standard_True;
  else return Standard_False;
}

inline Standard_Integer HashCode(const BRepMesh_Couple& one,
                                 const Standard_Integer Upper)
{
  return ::HashCode((one.myI1+one.myI2), Upper);
}

typedef NCollection_Map<BRepMesh_Couple> BRepMesh_MapOfCouple;


static void AddLink(BRepMesh_MapOfCouple& aMap, 
                    Standard_Integer v1,
                    Standard_Integer v2)
{
  Standard_Integer i1 = v1;
  Standard_Integer i2 = v2;
  if(i1 > i2) {
    i1 = v2;
    i2 = v1;
  }
  aMap.Add(BRepMesh_Couple(i1,i2));
}

static void MeshStats(const TopoDS_Shape& theSape,
                      Standard_Integer& theNbTri,
                      Standard_Integer& theNbEdges,
                      Standard_Integer& theNbNodes)
{
  theNbTri = 0;
  theNbEdges = 0;
  theNbNodes = 0;

  Handle(Poly_Triangulation) T;
  TopLoc_Location L;

  for ( TopExp_Explorer ex(theSape, TopAbs_FACE); ex.More(); ex.Next()) {
    TopoDS_Face F = TopoDS::Face(ex.Current());
    T = BRep_Tool::Triangulation(F, L);
    if (!T.IsNull()) {
      theNbTri += T->NbTriangles();
      theNbNodes += T->NbNodes();

      BRepMesh_MapOfCouple aMap;
      //count number of links
      Poly_Array1OfTriangle& Trian = T->ChangeTriangles();
      for(Standard_Integer i = 1; i<=Trian.Length();i++) {
        Standard_Integer v1, v2, v3;
        Trian(i).Get(v1,v2,v3);

        AddLink(aMap, v1, v2);
        AddLink(aMap, v2, v3);
        AddLink(aMap, v3, v1);
      }

      theNbEdges+=aMap.Extent();
    }
  }
}

static Standard_Integer triangule(Draw_Interpretor& di, Standard_Integer nbarg, const char** argv)
{
  if (nbarg < 4)
    return 1;

  const char *id1 = argv[2];
  TopoDS_Shape aShape = DBRep::Get(id1);
  if (aShape.IsNull())
    return 1;

  di << argv[1] << " ";

  Standard_Real aDeflection = Draw::Atof(argv[3]);
  if (aDeflection <= 0.)
  {
    di << " Incorrect value of deflection!\n";
    return 1;
  }

  Handle(MeshTest_DrawableMesh) aDMesh = 
    new MeshTest_DrawableMesh(aShape, aDeflection);

  Draw::Set(argv[1], aDMesh);

  Standard_Integer nbn, nbl, nbe;
  MeshStats(aShape, nbe, nbl, nbn);

  di<<"(Resultat ("<<nbe<<" mailles) ("<<nbl<<" aretes) ("<<nbn<<" sommets))\n";

  // passe de verification du maillage.
  /*Standard_Integer nbc;
  for (Standard_Integer iLi=1; iLi<= DM->Mesh()->NbEdges(); iLi++) {
  const BRepMesh_Edge& ed=DM->Mesh()->Edge(iLi);
  if (ed.Movability()!=BRepMesh_Deleted) {
  nbc=struc->ElemConnectedTo(iLi).Extent();
  if (nbc != 1 && nbc != 2) di <<"ERROR MAILLAGE Edge no "<< iLi<<"\n";
  }
  }*/


  Bnd_Box aBox;

  TopExp_Explorer aFaceIt(aShape, TopAbs_FACE);
  for (; aFaceIt.More(); aFaceIt.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(aFaceIt.Current());

    TopLoc_Location aLoc = aFace.Location();
    Handle(Poly_Triangulation) aTriangulation =
      BRep_Tool::Triangulation(aFace, aLoc);

    if (!aTriangulation.IsNull())
    {
      const Standard_Integer    aLength = aTriangulation->NbNodes();
      const TColgp_Array1OfPnt& aNodes  = aTriangulation->Nodes();
      for (Standard_Integer i = 1; i <= aLength; ++i)
        aBox.Add(aNodes(i));
    }
  }

  Standard_Real aDelta = 0.;
  if (!aBox.IsVoid())
  {
    Standard_Real x, y, z, X, Y, Z;
    aBox.Get(x, y, z, X, Y, Z);

    aDelta = Max(X - x, Max(Y - y, Z - z));
    if (aDelta > 0.0)
      aDelta = aDeflection / aDelta;
  }

  di << " Ratio between deflection and total shape size is " << aDelta << "\n";

  return 0;
}

//=======================================================================
//function : addshape
//purpose  : 
//=======================================================================

Standard_Integer addshape(Draw_Interpretor&, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  Handle(MeshTest_DrawableMesh) D =
    Handle(MeshTest_DrawableMesh)::DownCast(Draw::Get(a[1]));
  if (D.IsNull()) return 1;
  TopoDS_Shape S = DBRep::Get(a[2]);
  if (S.IsNull()) return 1;

  D->Add(S);
  Draw::Repaint();

  return 0;
}


//=======================================================================
//function : smooth
//purpose  : 
//=======================================================================

/*Standard_Integer smooth(Draw_Interpretor&, Standard_Integer n, const char** a)
{
if (n < 2) return 1;
Handle(MeshTest_DrawableMesh) D =
Handle(MeshTest_DrawableMesh)::DownCast(Draw::Get(a[1]));
if (D.IsNull()) return 1;
Handle(BRepMesh_DataStructureOfDelaun) struc=
D->Mesh()->Result();
BRepMesh_Array1OfVertexOfDelaun toto(1,1);
BRepMesh_Delaun trial(struc, 
toto,
Standard_True);
trial.SmoothMesh(0.1);
Draw::Repaint();
return 0;
}
*/

//=======================================================================
//function : edges
//purpose  : 
//=======================================================================

/*static Standard_Integer edges (Draw_Interpretor&, Standard_Integer n, const char** a)
{
if (n < 3) return 1;

Handle(MeshTest_DrawableMesh) D =
Handle(MeshTest_DrawableMesh)::DownCast(Draw::Get(a[1]));
if (D.IsNull()) return 1;
TopoDS_Shape S = DBRep::Get(a[2]);
if (S.IsNull()) return 1;

TopExp_Explorer ex;
TColStd_SequenceOfInteger& eseq = D->Edges();
Handle(BRepMesh_FastDiscret) M = D->Mesh();
Handle(BRepMesh_DataStructureOfDelaun) DS = M->Result();
Standard_Integer e1, e2, e3, iTri;
Standard_Boolean o1, o2, o3;

// the faces
for (ex.Init(S,TopAbs_FACE);ex.More();ex.Next()) {
const BRepMesh_MapOfInteger& elems = DS->ElemOfDomain();
BRepMesh_MapOfInteger::Iterator it;
for (it.Initialize(elems); it.More(); it.Next()) {
iTri = it.Key();
const BRepMesh_Triangle& triang = M->Triangle(iTri);
if (triang.Movability()!=BRepMesh_Deleted) {
triang.Edges(e1, e2, e3, o1, o2, o3);
eseq.Append(e1);
eseq.Append(e2);
eseq.Append(e3);
}
}
}

// the edges
//for (ex.Init(S,TopAbs_EDGE,TopAbs_FACE);ex.More();ex.Next()) {
//}

Draw::Repaint();
return 0;
}
*/

//=======================================================================
//function : vertices
//purpose  : 
//=======================================================================
static Standard_Integer vertices(
  Draw_Interpretor& /*di*/, 
  Standard_Integer  /*argc*/, 
  const char**      /*argv*/)
{
  return 0;

  // TODO: OAN re-implement this command according changes in BRepMesh
  //if (argc < 3)
  //  return 1;

  //Handle(MeshTest_DrawableMesh) aDrawableMesh =
  //  Handle(MeshTest_DrawableMesh)::DownCast(Draw::Get(argv[1]));
  //if (aDrawableMesh.IsNull())
  //  return 1;

  //TopoDS_Shape aShape = DBRep::Get(argv[2]);
  //if (aShape.IsNull())
  //  return 1;

  //TColStd_SequenceOfInteger&   aVertexSeq = aDrawableMesh->Vertices();
  //Handle(BRepMesh_FastDiscret) aMesh      = aDrawableMesh->Mesh();

  //TopExp_Explorer aFaceIt(aShape, TopAbs_FACE);
  //for (; aFaceIt.More(); aFaceIt.Next())
  //{
  //  const TopoDS_Face& aFace = TopoDS::Face(aFaceIt.Current());

  //  Handle(BRepMesh_FaceAttribute) aAttribute;
  //  if (aMesh->GetFaceAttribute(aFace, aAttribute))
  //  {
  //    Handle(BRepMesh_DataStructureOfDelaun) aStructure = aAttribute->EditStructure();

  //    // Recuperate from the map of edges.
  //    const BRepMeshCol::MapOfInteger& aEdgeMap = aStructure->LinksOfDomain();

  //    // Iterator on edges.
  //    BRepMeshCol::MapOfInteger aVertices;
  //    BRepMeshCol::MapOfInteger::Iterator aEdgeIt(aEdgeMap);
  //    for (; aEdgeIt.More(); aEdgeIt.Next())
  //    {
  //      const BRepMesh_Edge& aEdge = aStructure->GetLink(aEdgeIt.Key());
  //      aVertices.Add(aEdge.FirstNode());
  //      aVertices.Add(aEdge.LastNode());
  //    }

  //    BRepMeshCol::MapOfInteger::Iterator anIt(vtx);
  //    for ( ; anIt.More(); anIt.Next() )
  //      aVertexSeq.Append(anIt.Key());
  //  }
  //}

  //Draw::Repaint();
  //return 0;
}

//=======================================================================
//function : medge
//purpose  : 
//=======================================================================

static Standard_Integer medge (Draw_Interpretor&, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;

  Handle(MeshTest_DrawableMesh) D =
    Handle(MeshTest_DrawableMesh)::DownCast(Draw::Get(a[1]));
  if (D.IsNull()) return 1;

  Standard_Integer i,j,e;
  TColStd_SequenceOfInteger& eseq = D->Edges();
  for (i = 2; i < n; i++) {
    e = Draw::Atoi(a[i]);
    if (e > 0)
      eseq.Append(e);
    else if (e < 0) {
      e = -e;
      j = 1; 
      while (j <= eseq.Length()) {
        if (eseq(j) == e) 
          eseq.Remove(j);
        else
          j++;
      }
    }
    else
      eseq.Clear();
  }

  Draw::Repaint();
  return 0;
}


//=======================================================================
//function : mvertex
//purpose  : 
//=======================================================================

static Standard_Integer mvertex (Draw_Interpretor&, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;

  Handle(MeshTest_DrawableMesh) D =
    Handle(MeshTest_DrawableMesh)::DownCast(Draw::Get(a[1]));
  if (D.IsNull()) return 1;

  Standard_Integer i,j,v;
  TColStd_SequenceOfInteger& vseq = D->Vertices();
  for (i = 2; i < n; i++) {
    v = Draw::Atoi(a[i]);
    if (v > 0)
      vseq.Append(v);
    else if (v < 0) {
      v = -v;
      j = 1;
      while (j <= vseq.Length()) {
        if (vseq(j) == v)
          vseq.Remove(v);
        else
          j++;
      }
    }
    else
      vseq.Clear();
  }
  Draw::Repaint();
  return 0;
}


//=======================================================================
//function : triangle
//purpose  : 
//=======================================================================

static Standard_Integer triangle (Draw_Interpretor&, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;

  Handle(MeshTest_DrawableMesh) D =
    Handle(MeshTest_DrawableMesh)::DownCast(Draw::Get(a[1]));
  if (D.IsNull()) return 1;

  Standard_Integer i,j,v;
  TColStd_SequenceOfInteger& tseq = D->Triangles();
  for (i = 2; i < n; i++) {
    v = Draw::Atoi(a[i]);
    if (v > 0)
      tseq.Append(v);
    else if (v < 0) {
      v = -v;
      j = 1;
      while (j <= tseq.Length()) {
        if (tseq(j) == v)
          tseq.Remove(v);
        else
          j++;
      }
    }
    else
      tseq.Clear();
  }
  Draw::Repaint();
  return 0;
}

//=======================================================================
//function : dumpvertex
//purpose  : 
//=======================================================================

/*
Standard_Integer dumpvertex(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
if (argc < 2) return 1;

Handle(MeshTest_DrawableMesh) D =
Handle(MeshTest_DrawableMesh)::DownCast(Draw::Get(argv[1]));
if (D.IsNull()) return 1;

Handle(BRepMesh_DataStructureOfDelaun) struc = D->Mesh()->Result();

Standard_Integer in=1;
if (argc>=3) {
in=Draw::Atoi(argv[2]);
in=Max(1,in);
}
Standard_Integer nbn=in;
if (argc>=4) {
nbn=Draw::Atoi(argv[3]);
nbn=Min(nbn,struc->NbNodes());
}

for (; in<=nbn; in++) {
BRepMesh_Vertex nod=struc->GetNode(in);
di<<"(node "<<in<<" (uv "<<nod.Coord().X()
<<" "<<nod.Coord().Y()<<") (3d "
<<nod.Location3d()<<") ";
printdegree(nod.Movability(), di);
di<<" (edgeconex";
BRepMesh_ListOfInteger::Iterator tati(struc->LinkNeighboursOf(in));
for (; tati.More(); tati.Next()) di<<" "<<tati.Value();
di << "))\n";
}
di <<"\n";
return 0;
}

//=======================================================================
//function : dumpedge
//purpose  : 
//=======================================================================

Standard_Integer dumpedge(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
if (argc < 2) return 1;

Handle(MeshTest_DrawableMesh) D =
Handle(MeshTest_DrawableMesh)::DownCast(Draw::Get(argv[1]));
if (D.IsNull()) return 1;

Handle(BRepMesh_DataStructureOfDelaun) struc=D->Mesh()->Result();
Standard_Integer il=1;
if (argc>=3) {
il=Draw::Atoi(argv[2]);
il=Max(1, il);
}
Standard_Integer nbl=il;
if (argc>=4) {
nbl=Draw::Atoi(argv[3]);
nbl=Min(nbl, struc->NbLinks());
}

for (; il<=nbl; il++) {
BRepMesh_Edge edg=struc->GetLink(il);
di << "(edge "<<il<<" ("<<edg.FirstNode()<<" "<<edg.LastNode()
<<" ";
printdegree(edg.Movability(), di);
di<<") (triconex";
const BRepMesh_PairOfIndex& pair = struc->ElemConnectedTo(il);
for (Standard_Integer j = 1, jn = pair.Extent(); j <= jn; j++)
di<<" "<<pair.Index(j);
di << "))\n";
}
di <<"\n";
return 0;
}

//=======================================================================
//function : dumptriangle
//purpose  : 
//=======================================================================

Standard_Integer dumptriangle(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
if (argc < 2) return 1;

Handle(MeshTest_DrawableMesh) D =
Handle(MeshTest_DrawableMesh)::DownCast(Draw::Get(argv[1]));
if (D.IsNull()) return 1;

Handle(BRepMesh_DataStructureOfDelaun) struc=D->Mesh()->Result();
Standard_Integer ie=1;
if (argc>=3) {
ie=Draw::Atoi(argv[2]);
ie=Max(1, ie);
}
Standard_Integer nbe=ie;
if (argc>=4) {
nbe=Draw::Atoi(argv[3]);
nbe=Min(nbe, struc->NbElements());
}

Standard_Integer e1, e2, e3;
Standard_Boolean o1, o2, o3;

for (; ie<=nbe; ie++) {
BRepMesh_Triangle tri=struc->GetElement(ie);
tri.Edges(e1, e2, e3, o1, o2, o3); 
if (o1) e1=-e1;
if (o2) e2=-e2;
if (o3) e3=-e3;
di<<" (maille "<<ie<<" (links "<<e1<<" "
<<e2<<" "<<e3<<")";
printdegree(tri.Movability(), di);
di<<")\n";
}
di << "\n";
return 0;
}
*/

//=======================================================================
//function : trianglesinfo
//purpose  : 
//=======================================================================
static Standard_Integer trianglesinfo(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n != 2) return 1;
  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull()) return 1;
  TopExp_Explorer ex;
  Handle(Poly_Triangulation) T;
  TopLoc_Location L;

  Standard_Real MaxDeflection = 0.0;
  Standard_Integer nbtriangles = 0, nbnodes = 0;
  for (ex.Init(S, TopAbs_FACE); ex.More(); ex.Next()) {
    TopoDS_Face F = TopoDS::Face(ex.Current());
    T = BRep_Tool::Triangulation(F, L);
    if (!T.IsNull()) {
      nbtriangles += T->NbTriangles();
      nbnodes += T->NbNodes();
      if (T->Deflection() > MaxDeflection)
        MaxDeflection = T->Deflection();
    }
  }

  di<<"\n";
  di<<"This shape contains " <<nbtriangles<<" triangles.\n";
  di<<"                    " <<nbnodes    <<" nodes.\n";
  di<<"Maximal deflection " <<MaxDeflection<<"\n";
  di<<"\n";
#ifdef OCCT_DEBUG_MESH_CHRONO
  Standard_Real tot, addp, unif, contr, inter;
  Standard_Real edges, mailledges, etuinter, lastcontrol, stock;
  Standard_Real add11, add12, add2, upda, pointvalid;
  Standard_Real isos, pointsisos;
  chTotal.Show(tot); chAddPoint.Show(addp); chUnif.Show(unif); 
  chControl.Show(contr); chInternal.Show(inter);
  chEdges.Show(edges); chMaillEdges.Show(mailledges);
  chEtuInter.Show(etuinter); chLastControl.Show(lastcontrol); 
  chStock.Show(stock);
  chAdd11.Show(add11); chAdd12.Show(add12); chAdd2.Show(add2); chUpdate.Show(upda);
  chPointValid.Show(pointvalid); chIsos.Show(isos); chPointsOnIsos.Show(pointsisos);

  if (tot > 0.00001) {
    di <<"temps total de maillage:     "<<tot        <<" seconds\n";
    di <<"dont: \n";
    di <<"discretisation des edges:    "<<edges      <<" seconds---> "<< 100*edges/tot      <<" %\n";
    di <<"maillage des edges:          "<<mailledges <<" seconds---> "<< 100*mailledges/tot <<" %\n";
    di <<"controle et points internes: "<<etuinter   <<" seconds---> "<< 100*etuinter/tot   <<" %\n";
    di <<"derniers controles:          "<<lastcontrol<<" seconds---> "<< 100*lastcontrol/tot<<" %\n";
    di <<"stockage dans la S.D.        "<<stock      <<" seconds---> "<< 100*stock/tot      <<" %\n";
    di << "\n";
    di <<"et plus precisement: \n";
    di <<"Add 11ere partie :           "<<add11     <<" seconds---> "<<100*add11/tot      <<" %\n";
    di <<"Add 12ere partie :           "<<add12     <<" seconds---> "<<100*add12/tot      <<" %\n";
    di <<"Add 2eme partie :            "<<add2      <<" seconds---> "<<100*add2/tot       <<" %\n";
    di <<"Update :                     "<<upda      <<" seconds---> "<<100*upda/tot       <<" %\n";
    di <<"AddPoint :                   "<<addp      <<" seconds---> "<<100*addp/tot       <<" %\n";
    di <<"UniformDeflection            "<<unif      <<" seconds---> "<<100*unif/tot       <<" %\n";
    di <<"Controle :                   "<<contr     <<" seconds---> "<<100*contr/tot      <<" %\n";
    di <<"Points Internes:             "<<inter     <<" seconds---> "<<100*inter/tot      <<" %\n";
    di <<"calcul des isos et du, dv:   "<<isos      <<" seconds---> "<<100*isos/tot       <<" %\n";
    di <<"calcul des points sur isos:  "<<pointsisos<<" seconds---> "<<100*pointsisos/tot <<" %\n";
    di <<"IsPointValid:                "<<pointvalid<<" seconds---> "<<100*pointvalid/tot <<" %\n";
    di << "\n";


    di <<"nombre d'appels de controle apres points internes          : "<< NbControls << "\n";
    di <<"nombre de points sur restrictions                          : "<< D0Edges    << "\n";
    di <<"nombre de points calcules par UniformDeflection            : "<< D0Unif     << "\n";
    di <<"nombre de points calcules dans InternalVertices            : "<< D0Internal << "\n";
    di <<"nombre de points calcules dans Control                     : "<< D0Control  << "\n";
    if (nbnodes-D0Edges != 0) { 
      Standard_Real ratio = (Standard_Real)(D0Internal+D0Control)/ (Standard_Real)(nbnodes-D0Edges);
      di <<"---> Ratio: (D0Internal+D0Control) / (nbNodes-nbOnEdges)   : "<< ratio      << "\n";
    }

    di << "\n";

    chTotal.Reset(); chAddPoint.Reset(); chUnif.Reset(); 
    chControl.Reset(); chInternal.Reset();
    chEdges.Reset(); chMaillEdges.Reset();
    chEtuInter.Reset(); chLastControl.Reset(); 
    chStock.Reset();
    chAdd11.Reset(); chAdd12.Reset(); chAdd2.Reset(); chUpdate.Reset();
    chPointValid.Reset(); chIsos.Reset(); chPointsOnIsos.Reset();

  }
#endif
  return 0;
}

//=======================================================================
//function : veriftriangles
//purpose  : 
//=======================================================================

static Standard_Integer veriftriangles(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 2) return 1;
  Standard_Boolean quiet = 1;
  if (n == 3) quiet = 0;
  TopoDS_Shape Sh = DBRep::Get(a[1]);
  if (Sh.IsNull()) return 1;
  TopExp_Explorer ex;
  Handle(Poly_Triangulation) T;
  TopLoc_Location L;
  Standard_Integer i, n1, n2, n3;
  gp_Pnt2d mitri, v1, v2, v3, mi2d1, mi2d2, mi2d3;
  gp_XYZ vecEd1, vecEd2, vecEd3;
  //  Standard_Real dipo, dm, dv, d1, d2, d3, defle;
  Standard_Real dipo, dv, d1, d2, d3, defle;
  Handle(Geom_Surface) S;
  Standard_Integer nbface = 0;
  gp_Pnt PP;

  for (ex.Init(Sh, TopAbs_FACE); ex.More(); ex.Next()) {
    TopoDS_Face F = TopoDS::Face(ex.Current());
    nbface++;
    T = BRep_Tool::Triangulation(F, L);
    Standard_Real deflemax = 0, deflemin = 1.e100;
    if (!T.IsNull()) {
      Standard_Real defstock = T->Deflection();
      const Poly_Array1OfTriangle& triangles  = T->Triangles();
      const TColgp_Array1OfPnt2d&  Nodes2d    = T->UVNodes();
      const TColgp_Array1OfPnt&    Nodes      = T->Nodes();

      S = BRep_Tool::Surface(F, L);

      for(i = 1; i <= triangles.Length(); i++) {
        if (F.Orientation() == TopAbs_REVERSED) 
          triangles(i).Get(n1,n3,n2);
        else 
          triangles(i).Get(n1,n2,n3);

        const gp_XY& xy1 = Nodes2d(n1).XY();
        const gp_XY& xy2 = Nodes2d(n2).XY();
        const gp_XY& xy3 = Nodes2d(n3).XY();

        mi2d1.SetCoord((xy2.X()+xy3.X())*0.5, 
          (xy2.Y()+xy3.Y())*0.5);
        mi2d2.SetCoord((xy1.X()+xy3.X())*0.5, 
          (xy1.Y()+xy3.Y())*0.5);
        mi2d3.SetCoord((xy1.X()+xy2.X())*0.5, 
          (xy1.Y()+xy2.Y())*0.5);

        gp_XYZ p1 = Nodes(n1).Transformed(L.Transformation()).XYZ();
        gp_XYZ p2 = Nodes(n2).Transformed(L.Transformation()).XYZ();
        gp_XYZ p3 = Nodes(n3).Transformed(L.Transformation()).XYZ();

        vecEd1=p2-p1;
        vecEd2=p3-p2;
        vecEd3=p1-p3;
        d1=vecEd1.SquareModulus();
        d2=vecEd2.SquareModulus();
        d3=vecEd3.SquareModulus();

        if (d1!=0. && d2!=0. && d3!=0.) {
          gp_XYZ equa(vecEd1^vecEd2);
          dv=equa.Modulus();
          if (dv>0.) {
            equa.SetCoord(equa.X()/dv, equa.Y()/dv, equa.Z()/dv);
            dipo=equa*p1;


            mitri.SetCoord(ONETHIRD*(xy1.X()+xy2.X()+xy3.X()),
              ONETHIRD*(xy1.Y()+xy2.Y()+xy3.Y()));
            v1.SetCoord(ONETHIRD*mi2d1.X()+TWOTHIRD*xy1.X(), 
              ONETHIRD*mi2d1.Y()+TWOTHIRD*xy1.Y());
            v2.SetCoord(ONETHIRD*mi2d2.X()+TWOTHIRD*xy2.X(), 
              ONETHIRD*mi2d2.Y()+TWOTHIRD*xy2.Y());
            v3.SetCoord(ONETHIRD*mi2d3.X()+TWOTHIRD*xy3.X(), 
              ONETHIRD*mi2d3.Y()+TWOTHIRD*xy3.Y());

            S->D0(mi2d1.X(), mi2d1.Y(), PP);
            PP = PP.Transformed(L.Transformation());
            defle = Abs((equa*PP.XYZ())-dipo);
            deflemax = Max(deflemax, defle);
            deflemin = Min(deflemin, defle);

            S->D0(mi2d2.X(), mi2d2.Y(), PP);
            PP = PP.Transformed(L.Transformation());
            defle = Abs((equa*PP.XYZ())-dipo);
            deflemax = Max(deflemax, defle);
            deflemin = Min(deflemin, defle);

            S->D0(mi2d3.X(), mi2d3.Y(), PP);
            PP = PP.Transformed(L.Transformation());
            defle = Abs((equa*PP.XYZ())-dipo);
            deflemax = Max(deflemax, defle);
            deflemin = Min(deflemin, defle);

            S->D0(v1.X(), v1.Y(), PP);
            PP = PP.Transformed(L.Transformation());
            defle = Abs((equa*PP.XYZ())-dipo);
            deflemax = Max(deflemax, defle);
            deflemin = Min(deflemin, defle);

            S->D0(v2.X(), v2.Y(), PP);
            PP = PP.Transformed(L.Transformation());
            defle = Abs((equa*PP.XYZ())-dipo);
            deflemax = Max(deflemax, defle);
            deflemin = Min(deflemin, defle);

            S->D0(v3.X(), v3.Y(), PP);
            PP = PP.Transformed(L.Transformation());
            defle = Abs((equa*PP.XYZ())-dipo);
            deflemax = Max(deflemax, defle);
            deflemin = Min(deflemin, defle);

            S->D0(mitri.X(), mitri.Y(), PP);
            PP = PP.Transformed(L.Transformation());
            defle = Abs((equa*PP.XYZ())-dipo);
            deflemax = Max(deflemax, defle);
            deflemin = Min(deflemin, defle);

            if (defle > defstock) {
              di <<"face "<< nbface <<" deflection = " << defle <<" pour "<<defstock <<" stockee.\n";
            }
          }
        }
      }
      if (!quiet) {
        di <<"face "<< nbface<<", deflemin = "<< deflemin<<", deflemax = "<<deflemax<<"\n";
      }

    }
  }


  return 0;
}




//=======================================================================
//function : tri2d
//purpose  : 
//=======================================================================

Standard_Integer tri2d(Draw_Interpretor&, Standard_Integer n, const char** a)
{

  if (n != 2) return 1;
  TopoDS_Shape aLocalShape = DBRep::Get(a[1]);
  TopoDS_Face F = TopoDS::Face(aLocalShape);
  //  TopoDS_Face F = TopoDS::Face(DBRep::Get(a[1]));
  if (F.IsNull()) return 1;
  Handle(Poly_Triangulation) T;
  TopLoc_Location L;

  T = BRep_Tool::Triangulation(F, L);
  if (!T.IsNull()) {
    // Build the connect tool
    Poly_Connect pc(T);

    Standard_Integer i,j, nFree, nInternal, nbTriangles = T->NbTriangles();
    Standard_Integer t[3];

    // count the free edges
    nFree = 0;
    for (i = 1; i <= nbTriangles; i++) {
      pc.Triangles(i,t[0],t[1],t[2]);
      for (j = 0; j < 3; j++)
        if (t[j] == 0) nFree++;
    }

    // allocate the arrays
    TColStd_Array1OfInteger Free(1,2*nFree);
    nInternal = (3*nbTriangles - nFree) / 2;
    TColStd_Array1OfInteger Internal(0,2*nInternal);

    Standard_Integer fr = 1, in = 1;
    const Poly_Array1OfTriangle& triangles = T->Triangles();
    Standard_Integer nodes[3];
    for (i = 1; i <= nbTriangles; i++) {
      pc.Triangles(i,t[0],t[1],t[2]);
      triangles(i).Get(nodes[0],nodes[1],nodes[2]);
      for (j = 0; j < 3; j++) {
        Standard_Integer k = (j+1) % 3;
        if (t[j] == 0) {
          Free(fr)   = nodes[j];
          Free(fr+1) = nodes[k];
          fr += 2;
        }
        // internal edge if this triangle has a lower index than the adjacent
        else if (i < t[j]) {
          Internal(in)   = nodes[j];
          Internal(in+1) = nodes[k];
          in += 2;
        }
      }
    }

    // Display the edges
    if (T->HasUVNodes()) {
      const TColgp_Array1OfPnt2d& Nodes2d = T->UVNodes();

      Handle(Draw_Segment2D) Seg;

      // free edges
      Standard_Integer nn;
      nn = Free.Length() / 2;
      for (i = 1; i <= nn; i++) {
        Seg = new Draw_Segment2D(Nodes2d(Free(2*i-1)),
          Nodes2d(Free(2*i)),
          Draw_rouge);
        dout << Seg;
      }

      // internal edges

      nn = nInternal;
      for (i = 1; i <= nn; i++) {
        Seg = new Draw_Segment2D(Nodes2d(Internal(2*i-1)),
          Nodes2d(Internal(2*i)),
          Draw_bleu);
        dout << Seg;
      }
    }
    dout.Flush();
  }

  return 0;
}




//=======================================================================
//function : wavefront
//purpose  : 
//=======================================================================

static Standard_Integer wavefront(Draw_Interpretor&, Standard_Integer nbarg, const char** argv)
{
  if (nbarg < 2) return 1;

  TopoDS_Shape S = DBRep::Get(argv[1]);
  if (S.IsNull()) return 1;

  // creation du maillage s'il n'existe pas.

  Bnd_Box B;
  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  BRepBndLib::Add(S, B);
  B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  Standard_Real aDeflection = 
    MAX3( aXmax-aXmin , aYmax-aYmin , aZmax-aZmin) * 0.004;

  BRepMesh_IncrementalMesh aMesh (S, aDeflection);


  TopLoc_Location L;
  TopExp_Explorer ex;

  Standard_Integer i, nbface = 0;
  Standard_Boolean OK = Standard_True;
  gp_Vec D1U,D1V;
  gp_Vec D2U,D2V,D2UV;
  gp_Dir Nor;
  gp_Pnt P;
  Standard_Real U, V;
  CSLib_DerivativeStatus aStatus;
  CSLib_NormalStatus NStat;
  Standard_Real x, y, z;
  Standard_Integer n1, n2, n3;
  Standard_Integer k1, k2, k3;

  char ffile[100];

  if (nbarg == 3) {
    strcpy(ffile, argv[2]);
    strcat(ffile, ".obj");
  }
  else  strcpy(ffile, "wave.obj");
  FILE* outfile = fopen(ffile, "w");


  fprintf(outfile, "%s  %s\n%s %s\n\n", "# CASCADE   ","MATRA DATAVISION", "#", ffile);

  Standard_Integer nbNodes, totalnodes = 0, nbpolygons = 0;
  for (ex.Init(S, TopAbs_FACE); ex.More(); ex.Next()) {
    nbface++;
    TopoDS_Face F = TopoDS::Face(ex.Current());
    Handle(Poly_Triangulation) Tr = BRep_Tool::Triangulation(F, L);

    if (!Tr.IsNull()) {
      nbNodes = Tr->NbNodes();
      const TColgp_Array1OfPnt& Nodes = Tr->Nodes();

      // les noeuds.
      for (i = 1; i <= nbNodes; i++) {
        gp_Pnt Pnt = Nodes(i).Transformed(L.Transformation());
        x = Pnt.X();
        y = Pnt.Y();
        z = Pnt.Z();
        fprintf(outfile, "%s      %f  %f  %f\n", "v", x, y, z);
      }

      fprintf(outfile, "\n%s    %d\n\n", "# number of vertex", nbNodes);


      // les normales.

      if (Tr->HasUVNodes()) {
        const TColgp_Array1OfPnt2d& UVNodes = Tr->UVNodes();
        BRepAdaptor_Surface BS(F, Standard_False);

        for (i = 1; i <= nbNodes; i++) {
          U = UVNodes(i).X();
          V = UVNodes(i).Y();

          BS.D1(U,V,P,D1U,D1V);
          CSLib::Normal (D1U, D1V, Precision::Angular(), aStatus, Nor);
          if (aStatus != CSLib_Done) {
            BS.D2(U,V,P,D1U,D1V,D2U,D2V,D2UV);
            CSLib::Normal(D1U,D1V,D2U,D2V,D2UV,Precision::Angular(),OK,NStat,Nor);
          }
          if (F.Orientation() == TopAbs_REVERSED) Nor.Reverse();

          fprintf(outfile, "%s      %f  %f  %f\n", "vn", Nor.X(), Nor.Y(), Nor.Z());
        }

        fprintf(outfile, "\n%s    %d\n\n", "# number of vertex normals", nbNodes);
      }

      fprintf(outfile, "%s    %d\n", "s", nbface);

      // les triangles.
      Standard_Integer nbTriangles = Tr->NbTriangles();
      const Poly_Array1OfTriangle& triangles = Tr->Triangles();


      for (i = 1; i <= nbTriangles; i++) {
        if (F.Orientation()  == TopAbs_REVERSED)
          triangles(i).Get(n1, n3, n2);
        else 
          triangles(i).Get(n1, n2, n3);
        k1 = n1+totalnodes;
        k2 = n2+totalnodes;
        k3 = n3+totalnodes;
        fprintf(outfile, "%s %d%s%d %d%s%d %d%s%d\n", "fo", k1,"//", k1, k2,"//", k2, k3,"//", k3);
      }
      nbpolygons += nbTriangles;
      totalnodes += nbNodes;

      fprintf(outfile, "\n%s    %d\n", "# number of smooth groups", nbface);
      fprintf(outfile, "\n%s    %d\n", "# number of polygons", nbpolygons);

    }
  }

  fclose(outfile);

  return 0;
}


//=======================================================================
//function : onetriangulation
//purpose  : 
//=======================================================================

Standard_Integer onetriangulation(Draw_Interpretor&, Standard_Integer /*nbarg*/, const char** /*argv*/)
{

  /*

  if (nbarg < 2) return 1;

  TopoDS_Shape S = DBRep::Get(argv[1]);
  if (S.IsNull()) return 1;

  Handle(Poly_Triangulation) TFinale;
  char name[100];
  Standard_Integer nbshell = 0;

  TopExp_Explorer ex, exs, ex2;

  for (ex.Init(S, TopAbs_SHELL); ex.More(); ex.Next()) {
  nbshell++;
  TopoDS_Shell Sh = TopoDS::Shell(ex.Current());

  for (exs.Init(Sh, TopAbs_Face); exs.More(); exs.Next()) {
  TopoDS_Face F = TopoDS::Face(exs.Current());
  Handle(Poly_Triangulation) T = BRep_Tool::Triangulation(F, L);

  for (ex2.Init(F, TopAbs_EDGE); ex2.More(); ex2.Next()) {
  TopoDS_Edge edge = TopoDS::Edge(ex2.Current());
  const TColgp_Array1OfPnt& Nodes = T->Nodes();
  const Poly_Array1OfTriangle& triangles = T->Triangles();

  if (mapedges.IsBound(edge)) {
  const TColStd_ListOfTransient& L = edges.Find(edge);
  const Handle(Poly_PolygonOnTriangulation)& P = 
  *(Handle(Poly_PolygonOnTriangulation)*)&(L.First());
  const TColStd_Array1OfInteger& NOD = P->Nodes();

  }
  }
  }

  Sprintf(name, "%s_%i", "tr", nbshell);
  DrawTrSurf::Set(name, TFinale);

  }

  */
  return 0;
}


#if 0

//=======================================================================
//function : vb
//purpose  : 
//=======================================================================

Standard_Integer vb(Draw_Interpretor& di, Standard_Integer nbarg, const char** argv)
{
  Standard_Integer NbPoints = 1, Deg = 1;

  for (Deg = 1; Deg <= 25; Deg++) {
    for (NbPoints = 1; NbPoints <= 24; NbPoints++) {

      math_Vector GaussP(1, NbPoints), GaussW(1, NbPoints);
      math_Vector TheWeights(1, NbPoints), VBParam(1, NbPoints);
      math_Matrix VB(1, Deg+1, 1, NbPoints);

      math::GaussPoints(NbPoints, GaussP);

      Standard_Integer i, j, classe = Deg+1, cl1 = Deg;

      // calcul et mise en ordre des parametres et des poids:
      for (i = 1; i <= NbPoints; i++) {
        if (i <=  (NbPoints+1)/2) {
          VBParam(NbPoints-i+1)  = 0.5*(1 + GaussP(i));
        }
        else {
          VBParam(i-(NbPoints+1)/2)  = 0.5*(1 + GaussP(i));
        }
      }


      // Calcul du VB (Valeur des fonctions de Bernstein):
      for (i = 1; i <= classe; i++) {
        for (j = 1; j <= NbPoints; j++) {
          VB(i,j)=PLib::Binomial(cl1,i-1)*Pow((1-VBParam(j)),classe-i)*Pow(VBParam(j),i-1);
        }
      }


      for (i = 1; i <= classe; i++) {
        for (j = 1; j <= NbPoints; j++) {
          di<< VB(i, j) << ", ";
        }
      }
      di << "\n\n";
    }
  }
  return 0;
}  
//=======================================================================
//function : extrema
//purpose  : 
//=======================================================================

Standard_Integer extrema(Draw_Interpretor& di, Standard_Integer nbarg, const char** argv)
{


  Handle(Geom_Curve) C = DrawTrSurf::GetCurve(argv[1]);

  Standard_Real X, Y, Z, U0;
  X = Draw::Atof(argv[2]);
  Y = Draw::Atof(argv[3]);
  Z = Draw::Atof(argv[4]);
  U0 = Draw::Atof(argv[5]);

  gp_Pnt P(X, Y, Z);
  GeomAdaptor_Curve GC(C);
  Standard_Real tol = 1.e-09;
  Extrema_LocateExtPC ext(P, GC, U0, tol);

  if (ext.IsDone()) {
    gp_Pnt P1 = ext.Point().Value();
    di <<"distance =  "<<ext.Value() << "\n";
    di <<"point =     "<<P1.X()<<" "<<P1.Y()<<" "<< P1.Z()<< "\n";
    di <<"parametre = "<<ext.Point().Parameter()<<"\n";
  }

  return 0;
}

#endif


//=======================================================================
//function : triedgepoints
//purpose  : 
//=======================================================================

Standard_Integer triedgepoints(Draw_Interpretor& di, Standard_Integer nbarg, const char** argv)
{
  if( nbarg < 2 )
    return 1;

  for( Standard_Integer i = 1; i < nbarg; i++ )
  {
    TopoDS_Shape aShape = DBRep::Get(argv[i]);
    if ( aShape.IsNull() )
      continue;

    Handle(Poly_PolygonOnTriangulation) aPoly;
    Handle(Poly_Triangulation)          aT;
    TopLoc_Location                     aLoc;
    TopTools_MapOfShape                 anEdgeMap;
    TopTools_MapIteratorOfMapOfShape    it;
    
    if( aShape.ShapeType() == TopAbs_EDGE )
    {
      anEdgeMap.Add( aShape );
    }
    else
    {
      TopExp_Explorer ex(aShape, TopAbs_EDGE);
      for(; ex.More(); ex.Next() )
        anEdgeMap.Add( ex.Current() );
    }

    if ( anEdgeMap.Extent() == 0 )
      continue;

    char newname[1024];
    strcpy(newname,argv[i]);
    char* p = newname;
    while (*p != '\0') p++;
    *p = '_';
    p++;

    Standard_Integer nbEdge = 1;
    for(it.Initialize(anEdgeMap); it.More(); it.Next())
    {
      BRep_Tool::PolygonOnTriangulation(TopoDS::Edge(it.Key()), aPoly, aT, aLoc);
      if ( aT.IsNull() || aPoly.IsNull() )
        continue;
      
      const TColgp_Array1OfPnt&      Nodes   = aT->Nodes();
      const TColStd_Array1OfInteger& Indices = aPoly->Nodes();
      const Standard_Integer         nbnodes = Indices.Length();

      for( Standard_Integer j = 1; j <= nbnodes; j++ )
      {
        gp_Pnt P3d = Nodes(Indices(j));
        if( !aLoc.IsIdentity() )
          P3d.Transform(aLoc.Transformation());

        if( anEdgeMap.Extent() > 1 )
          Sprintf(p,"%d_%d",nbEdge,j);
        else
          Sprintf(p,"%d",j);
	      DBRep::Set( newname, BRepBuilderAPI_MakeVertex(P3d) );
	      di.AppendElement(newname);
      }
      nbEdge++;
    }
  }
  return 0;
}

//=======================================================================
//function : correctnormals
//purpose  : Corrects normals in shape triangulation nodes (...)
//=======================================================================
Standard_Integer correctnormals (Draw_Interpretor& theDI, 
                            Standard_Integer /*theNArg*/, 
                            const char** theArgVal)
{
  TopoDS_Shape S = DBRep::Get(theArgVal[1]);

  //Use "correctnormals shape"

  
  if(!BRepLib::EnsureNormalConsistency(S))
  {
    theDI << "Normals have not been changed!\n";
  }
  else
  {
    theDI << "Some corrections in source shape have been made!\n";
  }

  return 0;
}

//=======================================================================
void  MeshTest::Commands(Draw_Interpretor& theCommands)
//=======================================================================
{
  Draw::Commands(theCommands);
  BRepTest::AllCommands(theCommands);
  GeometryTest::AllCommands(theCommands);
  MeshTest::PluginCommands(theCommands);
  const char* g;

  g = "Mesh Commands";

  theCommands.Add("incmesh","Builds triangular mesh for the shape, run w/o args for help",__FILE__, incrementalmesh, g);
  theCommands.Add("tessellate","Builds triangular mesh for the surface, run w/o args for help",__FILE__, tessellate, g);
  theCommands.Add("MemLeakTest","MemLeakTest",__FILE__, MemLeakTest, g);
  theCommands.Add("fastdiscret","fastdiscret shape deflection",__FILE__, fastdiscret, g);
  theCommands.Add("mesh","mesh result Shape deflection",__FILE__, triangule, g);
  theCommands.Add("addshape","addshape meshname Shape [deflection]",__FILE__, addshape, g);
  //theCommands.Add("smooth","smooth meshname",__FILE__, smooth, g);
  //theCommands.Add("edges","edges mesh shape, highlight the edges",__FILE__,edges, g);
  theCommands.Add("vertices","vertices mesh shape, highlight the vertices",__FILE__,vertices, g);
  theCommands.Add("medge","medge mesh [-]index (0 to clear all)",__FILE__,medge, g);
  theCommands.Add("mvertex","mvertex mesh [-]index (0 to clear all)",__FILE__,mvertex, g);
  theCommands.Add("triangle","triangle mesh [-]index (0 to clear all)",__FILE__,triangle, g);
  //theCommands.Add("dumpvertex","dumpvertex mesh [index]",__FILE__,dumpvertex, g);
  //theCommands.Add("dumpedge","dumpedge mesh [index]",__FILE__,dumpedge, g);
  //theCommands.Add("dumptriangle","dumptriangle mesh [index]",__FILE__,dumptriangle, g);

  theCommands.Add("tri2d", "tri2d facename",__FILE__, tri2d, g);
  theCommands.Add("trinfo","trinfo name, print triangles information on objects",__FILE__,trianglesinfo,g);
  theCommands.Add("veriftriangles","veriftriangles name, verif triangles",__FILE__,veriftriangles,g);
  theCommands.Add("wavefront","wavefront name",__FILE__, wavefront, g);
  theCommands.Add("onetriangulation","onetriangulation name",__FILE__, onetriangulation, g);
  theCommands.Add("triepoints", "triepoints shape1 [shape2 ...]",__FILE__, triedgepoints, g);

  theCommands.Add("correctnormals", "correctnormals shape",__FILE__, correctnormals, g);

#if 0
  theCommands.Add("extrema","extrema ",__FILE__, extrema, g);
  theCommands.Add("vb","vb ",__FILE__, vb, g);
#endif
}
