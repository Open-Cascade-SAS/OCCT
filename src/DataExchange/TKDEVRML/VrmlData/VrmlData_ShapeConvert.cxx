// Created on: 2007-08-04
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <VrmlData_ShapeConvert.hxx>
#include <VrmlData_Scene.hxx>
#include <VrmlData_Group.hxx>
#include <VrmlData_IndexedFaceSet.hxx>
#include <VrmlData_IndexedLineSet.hxx>
#include <VrmlData_ShapeNode.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_DataMap.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Connect.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Polygon3D.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Label.hxx>
// #include <NCollection_Sequence<TDF_Label>.hxx>
#include <TDocStd_Document.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <GeomLib.hxx>
#include <Standard_ShortReal.hxx>
#include <VrmlData_Appearance.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs_Style.hxx>
#include <XCAFDoc_VisMaterial.hxx>
#include <XCAFDoc_VisMaterialTool.hxx>

//=================================================================================================

void VrmlData_ShapeConvert::AddShape(const TopoDS_Shape& theShape, const char* theName)
{
  ShapeData aData; /* = { - compilation problem on SUN
     TCollection_AsciiString(),
     theShape,
     NULL
   };*/
  aData.Shape = theShape;
  aData.Node  = nullptr;

  if (theName)
  {
    char  buf[2048], *optr = &buf[0];
    char* eptr = &buf[sizeof(buf) - 1];
    for (const char* ptr = theName;; ptr++)
    {
      char sym = *ptr;
      if (sym == '\0' || sym == '\n' || sym == '\r')
      {
        *optr = '\0';
        break;
      }
      if (sym == '\"' || sym == '\\')
        *optr = '/';
      else if (sym == '.')
        *optr = '_';
      else
        *optr = sym;
      if (++optr >= eptr)
      {
        *optr = '\0';
        break;
      }
    }
    aData.Name = buf;
  }
  myShapes.Append(aData);
}

//=================================================================================================

occ::handle<VrmlData_Geometry> VrmlData_ShapeConvert::makeTShapeNode(
  const TopoDS_Shape&    theShape,
  const TopAbs_ShapeEnum theShapeType,
  TopLoc_Location&       theLoc)
{
  occ::handle<VrmlData_Geometry> aTShapeNode = nullptr;
  const bool                     isReverse   = (theShape.Orientation() == TopAbs_REVERSED);

  TopoDS_Shape aTestedShape;
  aTestedShape.TShape(theShape.TShape());
  aTestedShape.Orientation(isReverse ? TopAbs_REVERSED : TopAbs_FORWARD);
  switch (theShapeType)
  {
    case TopAbs_FACE: {
      const TopoDS_Face& aFace = TopoDS::Face(theShape);
      if (!aFace.IsNull())
      {
        occ::handle<Poly_Triangulation> aTri = BRep_Tool::Triangulation(aFace, theLoc);

        const occ::handle<VrmlData_Geometry>* pNode = myRelMap.Seek(aTestedShape);
        if (pNode)
        {
          aTShapeNode = *pNode;
          break;
        }

        if (!aTri.IsNull())
        {
          TopoDS_Shape aTestedShapeRev = aTestedShape;
          aTestedShapeRev.Orientation(isReverse ? TopAbs_FORWARD : TopAbs_REVERSED);
          occ::handle<VrmlData_IndexedFaceSet>  aFaceSetToReuse;
          const occ::handle<VrmlData_Geometry>* pRevNode = myRelMap.Seek(aTestedShapeRev);
          if (pRevNode)
            aFaceSetToReuse = occ::down_cast<VrmlData_IndexedFaceSet>(*pRevNode);

          occ::handle<VrmlData_Coordinate> aCoordToReuse;
          if (!aFaceSetToReuse.IsNull())
            aCoordToReuse = aFaceSetToReuse->Coordinates();

          aTShapeNode = triToIndexedFaceSet(aTri, aFace, aCoordToReuse);
          myScene.AddNode(aTShapeNode, false);
          // Bind the converted face
          myRelMap.Bind(aTestedShape, aTShapeNode);
        }
      }
    }
    break;
    case TopAbs_WIRE: {
      const TopoDS_Wire& aWire = TopoDS::Wire(theShape);
      if (!aWire.IsNull())
      {
      }
    }
    break;
    case TopAbs_EDGE: {
      const TopoDS_Edge& aEdge = TopoDS::Edge(theShape);
      if (!aEdge.IsNull())
      {
        const occ::handle<VrmlData_Geometry>* pEdgeNode = myRelMap.Seek(aTestedShape);
        if (pEdgeNode)
        {
          aTShapeNode = *pEdgeNode;
          break;
        }
        // Check the presence of reversly oriented Edge. It can also be used
        // because we do not distinguish the orientation for edges.
        aTestedShape.Orientation(isReverse ? TopAbs_FORWARD : TopAbs_REVERSED);
        pEdgeNode = myRelMap.Seek(aTestedShape);
        if (pEdgeNode)
        {
          aTShapeNode = *pEdgeNode;
          break;
        }

        // try to find PolygonOnTriangulation
        occ::handle<Poly_PolygonOnTriangulation> aPT;
        occ::handle<Poly_Triangulation>          aT;
        TopLoc_Location                          aL;
        BRep_Tool::PolygonOnTriangulation(aEdge, aPT, aT, aL);

        // If PolygonOnTriangulation was found -> get the Polygon3D
        occ::handle<Poly_Polygon3D> aPol;
        if (!aPT.IsNull() && !aT.IsNull() && aPT->HasParameters())
        {
          BRepAdaptor_Curve                        aCurve(aEdge);
          occ::handle<NCollection_HArray1<double>> aPrs    = aPT->Parameters();
          int                                      nbNodes = aPT->NbNodes();
          NCollection_Array1<gp_Pnt>               arrNodes(1, nbNodes);
          NCollection_Array1<double>               arrUVNodes(1, nbNodes);

          for (int j = 1; j <= nbNodes; j++)
          {
            arrUVNodes(j) = aPrs->Value(aPrs->Lower() + j - 1);
            arrNodes(j)   = aCurve.Value(arrUVNodes(j));
          }
          aPol = new Poly_Polygon3D(arrNodes, arrUVNodes);
          aPol->Deflection(aPT->Deflection());
        }
        else
        {
          aPol = BRep_Tool::Polygon3D(aEdge, aL);

          // If polygon was not found -> generate it
          if (aPol.IsNull())
          {
            BRepAdaptor_Curve aCurve(aEdge);
            const double      aFirst = aCurve.FirstParameter();
            const double      aLast  = aCurve.LastParameter();

            GCPnts_TangentialDeflection TD(aCurve, aFirst, aLast, myDeflAngle, myDeflection, 2);
            const int                   nbNodes = TD.NbPoints();

            NCollection_Array1<gp_Pnt> arrNodes(1, nbNodes);
            NCollection_Array1<double> arrUVNodes(1, nbNodes);
            for (int j = 1; j <= nbNodes; j++)
            {
              arrNodes(j)   = TD.Value(j);
              arrUVNodes(j) = TD.Parameter(j);
            }
            aPol = new Poly_Polygon3D(arrNodes, arrUVNodes);
            aPol->Deflection(myDeflection);
          }
        }

        if (!aPol.IsNull())
        {
          aTShapeNode = polToIndexedLineSet(aPol);
          myScene.AddNode(aTShapeNode, false);
          // Bind the converted face
          myRelMap.Bind(aTestedShape, aTShapeNode);
        }
      }
    }
    break;
    default:
      break;
  }

  return aTShapeNode;
}

//=================================================================================================

void VrmlData_ShapeConvert::Convert(const bool   theExtractFaces,
                                    const bool   theExtractEdges,
                                    const double theDeflection,
                                    const double theDeflAngle)
{
  // const double aDeflection =
  //   theDeflection < 0.0001 ? 0.0001 : theDeflection;

  myDeflection = theDeflection < 0.0001 ? 0.0001 : theDeflection;
  myDeflAngle  = theDeflAngle;

  bool             Extract[2]   = {theExtractFaces, theExtractEdges};
  TopAbs_ShapeEnum ShapeType[2] = {TopAbs_FACE, TopAbs_EDGE};
  int              i;

  const occ::handle<NCollection_IncAllocator> anAlloc = new NCollection_IncAllocator;

  // Relocation map for converted shapes. We should distinguish both TShape
  // and Orientation in this map.
  // NCollection_DataMap <TopoDS_Shape,occ::handle<VrmlData_Geometry>>
  //  aRelMap (100, anAlloc);
  myRelMap = NCollection_DataMap<TopoDS_Shape, occ::handle<VrmlData_Geometry>>(100, anAlloc);

  NCollection_List<ShapeData>::Iterator anIter(myShapes);
  for (; anIter.More(); anIter.Next())
  {
    ShapeData&              aData   = anIter.ChangeValue();
    TCollection_AsciiString aGrName = aData.Name;
    aGrName.ChangeAll(' ', '_');
    aGrName.ChangeAll('#', '_');
    occ::handle<VrmlData_Group> aGroup = new VrmlData_Group(myScene, aGrName.ToCString());
    myScene.AddNode(aGroup);

    for (i = 0; i < 2; ++i)
    {

      if (!Extract[i])
        continue;

      TopExp_Explorer anExp(aData.Shape, ShapeType[i]);
      for (; anExp.More(); anExp.Next())
      {
        const TopoDS_Shape&            aShape = anExp.Current();
        TopLoc_Location                aLoc;
        occ::handle<VrmlData_Geometry> aTShapeNode = makeTShapeNode(aShape, ShapeType[i], aLoc);
        if (!aTShapeNode.IsNull())
        {
          const occ::handle<VrmlData_ShapeNode> aShapeNode =
            new VrmlData_ShapeNode(myScene, nullptr);
          aShapeNode->SetAppearance(ShapeType[i] == TopAbs_FACE ? defaultMaterialFace()
                                                                : defaultMaterialEdge());
          myScene.AddNode(aShapeNode, false);
          aShapeNode->SetGeometry(aTShapeNode);
          if (aLoc.IsIdentity())
            // Store the shape node directly into the main Group.
            aGroup->AddNode(aShapeNode);
          else
          {
            // Create a Transform grouping node
            occ::handle<VrmlData_Group> aTrans = new VrmlData_Group(myScene, nullptr, true);
            gp_Trsf                     aTrsf(aLoc);
            if (fabs(myScale - 1.) > Precision::Confusion())
            {
              const gp_XYZ aTransl = aTrsf.TranslationPart() * myScale;
              aTrsf.SetTranslationPart(aTransl);
            }
            aTrans->SetTransform(aTrsf);
            myScene.AddNode(aTrans, false);
            aGroup->AddNode(aTrans);

            // Store the shape node under the transform.
            aTrans->AddNode(aShapeNode);
          }
        }
      }
    }
  }
  myShapes.Clear();
}

//=================================================================================================

occ::handle<VrmlData_Geometry> VrmlData_ShapeConvert::triToIndexedFaceSet(
  const occ::handle<Poly_Triangulation>&  theTri,
  const TopoDS_Face&                      theFace,
  const occ::handle<VrmlData_Coordinate>& theCoord)
{
  int       i;
  const int nNodes(theTri->NbNodes());
  const int nTriangles(theTri->NbTriangles());

  // protection against creation degenerative triangles
  int                               nbTri = 0;
  NCollection_Array1<Poly_Triangle> aTriangles(1, nTriangles);
  for (i = 0; i < nTriangles; i++)
  {
    int idx[3];
    theTri->Triangle(i + 1).Get(idx[0], idx[1], idx[2]);
    if (idx[0] == idx[1] || idx[0] == idx[2] || idx[1] == idx[2])
    {
      continue;
    }
    nbTri++;
    aTriangles.SetValue(nbTri, theTri->Triangle(i + 1));
  }
  aTriangles.Resize(1, nbTri, true);

  const occ::handle<VrmlData_IndexedFaceSet> aFaceSet =
    new VrmlData_IndexedFaceSet(myScene,
                                nullptr, // no name
                                true,    // IsCCW
                                false,   // IsSolid
                                false);  // IsConvex
  const occ::handle<NCollection_IncAllocator>& anAlloc = myScene.Allocator();
  const bool isReverse                                 = (theFace.Orientation() == TopAbs_REVERSED);

  // Create the array of triangles
  const int** arrPolygons = static_cast<const int**>(anAlloc->Allocate(nbTri * sizeof(const int*)));
  aFaceSet->SetPolygons(nbTri, arrPolygons);

  // Store the triangles
  for (i = 0; i < nbTri; i++)
  {
    int* aPolygon = static_cast<int*>(anAlloc->Allocate(4 * sizeof(int)));
    aPolygon[0]   = 3;
    aTriangles(i + 1).Get(aPolygon[1], aPolygon[2], aPolygon[3]);
    aPolygon[1]--;
    if (isReverse)
    {
      const int aTmp = aPolygon[2] - 1;
      aPolygon[2]    = aPolygon[3] - 1;
      aPolygon[3]    = aTmp;
    }
    else
    {
      aPolygon[2]--;
      aPolygon[3]--;
    }
    arrPolygons[i] = aPolygon;
  }

  // Create the Coordinates node
  if (!theCoord.IsNull())
    aFaceSet->SetCoordinates(theCoord);
  else
  {
    gp_XYZ* arrNodes = static_cast<gp_XYZ*>(anAlloc->Allocate(nNodes * sizeof(gp_XYZ)));
    for (i = 0; i < nNodes; i++)
    {
      arrNodes[i] = theTri->Node(i + 1).XYZ() * myScale;
    }

    const occ::handle<VrmlData_Coordinate> aCoordNode =
      new VrmlData_Coordinate(myScene, nullptr, nNodes, arrNodes);
    myScene.AddNode(aCoordNode, false);
    aFaceSet->SetCoordinates(aCoordNode);
  }

  // Create the Normals node if theTri has normals
  if (theTri->HasNormals())
  {
    gp_XYZ* arrVec = static_cast<gp_XYZ*>(anAlloc->Allocate(nNodes * sizeof(gp_XYZ)));
    NCollection_Vec3<float> aVec3;
    for (i = 0; i < nNodes; i++)
    {
      theTri->Normal(i + 1, aVec3);
      gp_XYZ aNormal(aVec3.x(), aVec3.y(), aVec3.z());
      if (isReverse)
      {
        aNormal.Reverse();
      }
      arrVec[i] = aNormal;
    }
    const occ::handle<VrmlData_Normal> aNormalNode =
      new VrmlData_Normal(myScene, nullptr, nNodes, arrVec);
    myScene.AddNode(aNormalNode, false);
    aFaceSet->SetNormals(aNormalNode);
    return occ::handle<VrmlData_Geometry>(aFaceSet);
  }

  Poly_Connect PC(theTri);
  // Create the Normals node (if UV- values are available)
  TopLoc_Location                 aLoc;
  constexpr double                aConf2   = Precision::SquareConfusion();
  const occ::handle<Geom_Surface> aSurface = BRep_Tool::Surface(theFace, aLoc);
  if (theTri->HasUVNodes() && !aSurface.IsNull())
  {
    if (aSurface->IsCNu(1) && aSurface->IsCNv(1))
    {
      gp_XYZ* arrVec = static_cast<gp_XYZ*>(anAlloc->Allocate(nNodes * sizeof(gp_XYZ)));

      // Compute the normal vectors
      double Tol = std::sqrt(aConf2);
      for (i = 0; i < nNodes; i++)
      {
        const gp_Pnt2d aUV = theTri->UVNode(i + 1);
        gp_Dir         aNormal;
        if (GeomLib::NormEstim(aSurface, aUV, Tol, aNormal) > 1)
        {
          // Try to estimate as middle normal of adjacent triangles
          int n[3];

          gp_XYZ eqPlan(0., 0., 0.);
          for (PC.Initialize(i + 1); PC.More(); PC.Next())
          {
            aTriangles(PC.Value()).Get(n[0], n[1], n[2]);
            gp_XYZ v1(theTri->Node(n[1]).Coord() - theTri->Node(n[0]).Coord());
            gp_XYZ v2(theTri->Node(n[2]).Coord() - theTri->Node(n[1]).Coord());
            gp_XYZ vv = v1 ^ v2;

            double mod = vv.Modulus();
            if (mod < Tol)
              continue;

            eqPlan += vv / mod;
          }

          if (eqPlan.SquareModulus() > gp::Resolution())
            aNormal = gp_Dir(eqPlan);
        }
        if (isReverse)
          aNormal.Reverse();

        if (aNormal.X() * aNormal.X() < aConf2)
          aNormal.SetX(0.);
        if (aNormal.Y() * aNormal.Y() < aConf2)
          aNormal.SetY(0.);
        if (aNormal.Z() * aNormal.Z() < aConf2)
          aNormal.SetZ(0.);

        arrVec[i] = aNormal.XYZ();
      }

      const occ::handle<VrmlData_Normal> aNormalNode =
        new VrmlData_Normal(myScene, nullptr, nNodes, arrVec);
      myScene.AddNode(aNormalNode, false);
      aFaceSet->SetNormals(aNormalNode);
    }
  }

  return occ::handle<VrmlData_Geometry>(aFaceSet);
}

//=======================================================================
// function : polToIndexedLineSet
// purpose  : single polygon3D => IndexedLineSet
//=======================================================================

occ::handle<VrmlData_Geometry> VrmlData_ShapeConvert::polToIndexedLineSet(
  const occ::handle<Poly_Polygon3D>& thePol)
{
  int                                          i;
  const int                                    nNodes(thePol->NbNodes());
  const NCollection_Array1<gp_Pnt>&            arrPolyNodes = thePol->Nodes();
  const occ::handle<NCollection_IncAllocator>& anAlloc      = myScene.Allocator();

  const occ::handle<VrmlData_IndexedLineSet> aLineSet =
    new VrmlData_IndexedLineSet(myScene, nullptr);

  // Create the array of polygons (1 member)
  const int** arrPolygons = static_cast<const int**>(anAlloc->Allocate(sizeof(const int*)));
  aLineSet->SetPolygons(1, arrPolygons);

  // Store the polygon
  int* aPolygon = static_cast<int*>(anAlloc->Allocate((nNodes + 1) * sizeof(int)));
  aPolygon[0]   = nNodes;
  for (i = 1; i <= nNodes; i++)
    aPolygon[i] = i - 1;
  arrPolygons[0] = aPolygon;

  // Create the Coordinates node
  gp_XYZ* arrNodes = static_cast<gp_XYZ*>(anAlloc->Allocate(nNodes * sizeof(gp_XYZ)));
  for (i = 0; i < nNodes; i++)
    arrNodes[i] = arrPolyNodes(i + 1).XYZ() * myScale;

  const occ::handle<VrmlData_Coordinate> aCoordNode =
    new VrmlData_Coordinate(myScene, nullptr, nNodes, arrNodes);
  myScene.AddNode(aCoordNode, false);
  aLineSet->SetCoordinates(aCoordNode);

  return occ::handle<VrmlData_Geometry>(aLineSet);
}

//=================================================================================================

occ::handle<VrmlData_Appearance> VrmlData_ShapeConvert::defaultMaterialFace() const
{
  static char                      aNodeName[] = "__defaultMaterialFace";
  occ::handle<VrmlData_Appearance> anAppearance =
    occ::down_cast<VrmlData_Appearance>(myScene.FindNode(aNodeName));
  if (anAppearance.IsNull())
  {
    const occ::handle<VrmlData_Material> aMaterial =
      new VrmlData_Material(myScene, nullptr, 1.0, 0.022, 0.);
    aMaterial->SetDiffuseColor(Quantity_Color(0.780392, 0.568627, 0.113725, Quantity_TOC_sRGB));
    aMaterial->SetEmissiveColor(Quantity_Color(0.329412, 0.223529, 0.027451, Quantity_TOC_sRGB));
    aMaterial->SetSpecularColor(Quantity_Color(0.992157, 0.941176, 0.807843, Quantity_TOC_sRGB));
    myScene.AddNode(aMaterial, false);
    anAppearance = new VrmlData_Appearance(myScene, aNodeName);
    anAppearance->SetMaterial(aMaterial);
    myScene.AddNode(anAppearance, false);
  }
  return anAppearance;
}

//=================================================================================================

occ::handle<VrmlData_Appearance> VrmlData_ShapeConvert::defaultMaterialEdge() const
{
  static char                      aNodeName[] = "__defaultMaterialEdge";
  occ::handle<VrmlData_Appearance> anAppearance =
    occ::down_cast<VrmlData_Appearance>(myScene.FindNode(aNodeName));
  if (anAppearance.IsNull())
  {
    const occ::handle<VrmlData_Material> aMaterial =
      new VrmlData_Material(myScene, nullptr, 0.2, 0.2, 0.2);
    aMaterial->SetDiffuseColor(Quantity_Color(0.2, 0.7, 0.2, Quantity_TOC_RGB));
    aMaterial->SetEmissiveColor(Quantity_Color(0.2, 0.7, 0.2, Quantity_TOC_RGB));
    aMaterial->SetSpecularColor(Quantity_Color(0.2, 0.7, 0.2, Quantity_TOC_RGB));
    myScene.AddNode(aMaterial, false);
    anAppearance = new VrmlData_Appearance(myScene, aNodeName);
    anAppearance->SetMaterial(aMaterial);
    myScene.AddNode(anAppearance, false);
  }
  return anAppearance;
}

//=======================================================================
// function : addShape
// purpose  : Adds the shape from the document
//=======================================================================
void VrmlData_ShapeConvert::addShape(const occ::handle<VrmlData_Group>&   theParent,
                                     const TDF_Label&                     theLabel,
                                     const occ::handle<TDocStd_Document>& theDoc)
{
  occ::handle<XCAFDoc_ShapeTool>       aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDoc->Main());
  occ::handle<XCAFDoc_ColorTool>       aColorTool = XCAFDoc_DocumentTool::ColorTool(theDoc->Main());
  occ::handle<XCAFDoc_VisMaterialTool> aMatTool =
    XCAFDoc_DocumentTool::VisMaterialTool(theDoc->Main());

  NCollection_DataMap<TopoDS_Shape, TDF_Label> aChildShapeToLabels;
  NCollection_Sequence<TDF_Label>              aChildLabels;
  aShapeTool->GetSubShapes(theLabel, aChildLabels);
  for (NCollection_Sequence<TDF_Label>::Iterator aChildIter(aChildLabels); aChildIter.More();
       aChildIter.Next())
  {
    const TDF_Label& aChildLabel = aChildIter.Value();
    TopoDS_Shape     aChildShape;
    if (aShapeTool->GetShape(aChildLabel, aChildShape))
    {
      aChildShapeToLabels.Bind(aChildShape, aChildLabel);
    }
  }

  const TopoDS_Shape          aShape = aShapeTool->GetShape(theLabel);
  occ::handle<VrmlData_Group> aGroup = nullptr;
  TopExp_Explorer             anExp(aShape, TopAbs_FACE);
  int                         nbFaces = 0;
  for (; anExp.More(); anExp.Next())
  {
    nbFaces++;
  }
  occ::handle<TDataStd_Name> aNameAttribute;
  theLabel.FindAttribute(TDataStd_Name::GetID(), aNameAttribute);
  if (nbFaces > 1)
  {
    if (!aNameAttribute.IsNull())
    {
      TCollection_AsciiString aName = aNameAttribute->Get();
      aName.ChangeAll(' ', '_');
      aName.ChangeAll('#', '_');
      aGroup = new VrmlData_Group(myScene, aName.ToCString());
    }
    else
    {
      aGroup = new VrmlData_Group(myScene, nullptr);
    }
    myScene.AddNode(aGroup, theParent.IsNull());
    if (!theParent.IsNull())
    {
      theParent->AddNode(aGroup);
    }
  }

  anExp.Init(aShape, TopAbs_FACE);
  for (; anExp.More(); anExp.Next())
  {
    TopLoc_Location                aLoc;
    occ::handle<VrmlData_Geometry> aTShapeNode = makeTShapeNode(anExp.Current(), TopAbs_FACE, aLoc);
    if (!aTShapeNode.IsNull())
    {
      occ::handle<VrmlData_ShapeNode> aShapeNode = nullptr;
      if (aGroup.IsNull() && !aNameAttribute.IsNull())
      {
        TCollection_AsciiString aName = aNameAttribute->Get();
        aName.ChangeAll(' ', '_');
        aName.ChangeAll('#', '_');
        aShapeNode = new VrmlData_ShapeNode(myScene, aName.ToCString());
      }
      else
      {
        aShapeNode = new VrmlData_ShapeNode(myScene, nullptr);
      }

      // set color
      XCAFPrs_Style      aStyle;
      Quantity_ColorRGBA aColor;
      TDF_Label          aLabel, anAttribLab;
      if (aChildShapeToLabels.Find(anExp.Current(), aLabel))
      {
        occ::handle<XCAFDoc_VisMaterial> aVisMat = aMatTool->GetShapeMaterial(aLabel);
        if (!aVisMat.IsNull() && !aVisMat->IsEmpty())
        {
          anAttribLab = aVisMat->Label();
          aStyle.SetMaterial(aVisMat);
        }
        else if (aColorTool->GetColor(aLabel, XCAFDoc_ColorSurf, anAttribLab)
                 || aColorTool->GetColor(aLabel, XCAFDoc_ColorGen, anAttribLab))
        {
          aColorTool->GetColor(anAttribLab, aColor);
          aStyle.SetColorSurf(aColor);
        }
      }
      if (!aStyle.IsSetColorSurf() && aStyle.Material().IsNull())
      {
        occ::handle<XCAFDoc_VisMaterial> aVisMat = aMatTool->GetShapeMaterial(theLabel);
        if (!aVisMat.IsNull() && !aVisMat->IsEmpty())
        {
          anAttribLab = aVisMat->Label();
          aStyle.SetMaterial(aVisMat);
        }
        if (aColorTool->GetColor(theLabel, XCAFDoc_ColorSurf, anAttribLab)
            || aColorTool->GetColor(theLabel, XCAFDoc_ColorGen, anAttribLab))
        {
          aColorTool->GetColor(anAttribLab, aColor);
          aStyle.SetColorSurf(aColor);
        }
      }
      if (!aStyle.IsSetColorSurf() && aStyle.Material().IsNull())
      {
        aShapeNode->SetAppearance(defaultMaterialFace());
      }
      else
      {
        aShapeNode->SetAppearance(makeMaterialFromStyle(aStyle, anAttribLab));
      }

      myScene.AddNode(aShapeNode, theParent.IsNull() && aGroup.IsNull());
      aShapeNode->SetGeometry(aTShapeNode);
      if (aLoc.IsIdentity())
      {
        // Store the shape node directly into the main Group.
        if (!aGroup.IsNull())
        {
          aGroup->AddNode(aShapeNode);
        }
        else if (!theParent.IsNull())
        {
          theParent->AddNode(aShapeNode);
        }
      }
      else
      {
        // Create a Transform grouping node
        occ::handle<VrmlData_Group> aTrans = new VrmlData_Group(myScene, nullptr, true);
        gp_Trsf                     aTrsf(aLoc);
        if (fabs(myScale - 1.) > Precision::Confusion())
        {
          const gp_XYZ aTransl = aTrsf.TranslationPart() * myScale;
          aTrsf.SetTranslationPart(aTransl);
        }
        aTrans->SetTransform(aTrsf);
        myScene.AddNode(aTrans, theParent.IsNull() && aGroup.IsNull());
        if (!aGroup.IsNull())
        {
          aGroup->AddNode(aTrans);
        }
        else if (!theParent.IsNull())
        {
          theParent->AddNode(aTrans);
        }
        // Store the shape node under the transform.
        aTrans->AddNode(aShapeNode);
      }
    }
  }
}

//=======================================================================
// function : addInstance
// purpose  : Adds the reference from the document
//=======================================================================
void VrmlData_ShapeConvert::addInstance(const occ::handle<VrmlData_Group>&   theParent,
                                        const TDF_Label&                     theLabel,
                                        const occ::handle<TDocStd_Document>& theDoc)
{
  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDoc->Main());

  const TopLoc_Location       aLoc   = aShapeTool->GetLocation(theLabel);
  occ::handle<VrmlData_Group> aTrans = nullptr;
  if (!aLoc.IsIdentity())
  {
    // Create a Transform grouping node
    aTrans = new VrmlData_Group(myScene, nullptr, true);
    gp_Trsf aTrsf(aLoc);
    if (fabs(myScale - 1.) > Precision::Confusion())
    {
      const gp_XYZ aTransl = aTrsf.TranslationPart() * myScale;
      aTrsf.SetTranslationPart(aTransl);
    }
    aTrans->SetTransform(aTrsf);
    myScene.AddNode(aTrans, theParent.IsNull());
    if (!theParent.IsNull())
    {
      theParent->AddNode(aTrans);
    }
  }

  occ::handle<TDataStd_Name> aNameAttribute;
  theLabel.FindAttribute(TDataStd_Name::GetID(), aNameAttribute);

  TDF_Label aRefLabel;
  aShapeTool->GetReferredShape(theLabel, aRefLabel);
  occ::handle<TDataStd_Name> aRefNameAttribute;
  aRefLabel.FindAttribute(TDataStd_Name::GetID(), aRefNameAttribute);

  if (aShapeTool->IsSimpleShape(aRefLabel))
  {
    addShape((aTrans.IsNull() ? theParent : aTrans), aRefLabel, theDoc);
  }
  else if (aShapeTool->IsAssembly(aRefLabel))
  {
    addAssembly((aTrans.IsNull() ? theParent : aTrans), aRefLabel, theDoc, aTrans.IsNull());
  }
}

//=======================================================================
// function : addAssembly
// purpose  : Adds the assembly from the document
//=======================================================================
void VrmlData_ShapeConvert::addAssembly(const occ::handle<VrmlData_Group>&   theParent,
                                        const TDF_Label&                     theLabel,
                                        const occ::handle<TDocStd_Document>& theDoc,
                                        const bool                           theNeedCreateGroup)
{
  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDoc->Main());

  occ::handle<VrmlData_Group> anAssembly = nullptr;
  if (theNeedCreateGroup)
  {
    occ::handle<TDataStd_Name> aNameAttribute;
    theLabel.FindAttribute(TDataStd_Name::GetID(), aNameAttribute);
    if (!aNameAttribute.IsNull())
    {
      TCollection_AsciiString aName = aNameAttribute->Get();
      aName.ChangeAll(' ', '_');
      aName.ChangeAll('#', '_');
      anAssembly = new VrmlData_Group(myScene, aName.ToCString());
    }
    else
    {
      anAssembly = new VrmlData_Group(myScene, nullptr);
    }
    TopLoc_Location aLoc = aShapeTool->GetLocation(theLabel);
    if (!aLoc.IsIdentity())
    {
      gp_Trsf aTrsf(aLoc);
      if (fabs(myScale - 1.) > Precision::Confusion())
      {
        const gp_XYZ aTransl = aTrsf.TranslationPart() * myScale;
        aTrsf.SetTranslationPart(aTransl);
      }
      anAssembly->SetTransform(aTrsf);
    }
    myScene.AddNode(anAssembly, theParent.IsNull());
    if (!theParent.IsNull())
    {
      theParent->AddNode(anAssembly);
    }
  }

  NCollection_Sequence<TDF_Label> aChildLabels;
  aShapeTool->GetComponents(theLabel, aChildLabels);
  for (NCollection_Sequence<TDF_Label>::Iterator aChildIter(aChildLabels); aChildIter.More();
       aChildIter.Next())
  {
    const TDF_Label& aChildLabel = aChildIter.Value();
    if (aShapeTool->IsAssembly(aChildLabel))
    {
      addAssembly((anAssembly.IsNull() ? theParent : anAssembly),
                  aChildLabel,
                  theDoc,
                  anAssembly.IsNull());
    }
    else if (aShapeTool->IsReference(aChildLabel))
    {
      addInstance((anAssembly.IsNull() ? theParent : anAssembly), aChildLabel, theDoc);
    }
    else if (aShapeTool->IsSimpleShape(aChildLabel))
    {
      addShape((anAssembly.IsNull() ? theParent : anAssembly), aChildLabel, theDoc);
    }
  }
}

//=================================================================================================

void VrmlData_ShapeConvert::ConvertDocument(const occ::handle<TDocStd_Document>& theDoc)
{
  occ::handle<XCAFDoc_ShapeTool> aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDoc->Main());

  NCollection_Sequence<TDF_Label> aFreeShapeLabels;
  aShapeTool->GetFreeShapes(aFreeShapeLabels);

  occ::handle<VrmlData_Group> aGroup = nullptr;
  if (aFreeShapeLabels.Size() > 1)
  {
    aGroup = new VrmlData_Group(myScene, nullptr);
    myScene.AddNode(aGroup);
  }

  for (NCollection_Sequence<TDF_Label>::Iterator aRootIter(aFreeShapeLabels); aRootIter.More();
       aRootIter.Next())
  {
    const TDF_Label& aFreeShapeLabel = aRootIter.Value();
    if (aShapeTool->IsAssembly(aFreeShapeLabel))
    {
      addAssembly(aGroup, aFreeShapeLabel, theDoc, true);
    }
    else if (aShapeTool->IsReference(aFreeShapeLabel))
    {
      addInstance(aGroup, aFreeShapeLabel, theDoc);
    }
    else if (aShapeTool->IsSimpleShape(aFreeShapeLabel))
    {
      addShape(aGroup, aFreeShapeLabel, theDoc);
    }
  }
}

//=================================================================================================

occ::handle<VrmlData_Appearance> VrmlData_ShapeConvert::makeMaterialFromStyle(
  const XCAFPrs_Style& theStyle,
  const TDF_Label&     theAttribLab) const
{
  const Quantity_ColorRGBA aColor =
    !theStyle.Material().IsNull() ? theStyle.Material()->BaseColor() : theStyle.GetColorSurfRGBA();

  TCollection_AsciiString    aNodeName = "_materialFace_";
  occ::handle<TDataStd_Name> aNameAttribute;
  if (theAttribLab.FindAttribute(TDataStd_Name::GetID(), aNameAttribute))
  {
    aNodeName.AssignCat(aNameAttribute->Get());
    int n = aNodeName.Search(" ");
    if (n > 0)
    {
      aNodeName = aNodeName.SubString(1, n - 1);
    }
  }
  else
  {
    NCollection_Vec3<double> aColor_sRGB;
    aColor.GetRGB().Values(aColor_sRGB.r(), aColor_sRGB.g(), aColor_sRGB.b(), Quantity_TOC_sRGB);
    aNodeName.AssignCat(aColor_sRGB.r());
    aNodeName.AssignCat("_");
    aNodeName.AssignCat(aColor_sRGB.g());
    aNodeName.AssignCat("_");
    aNodeName.AssignCat(aColor_sRGB.b());
  }

  occ::handle<VrmlData_Appearance> anAppearance =
    occ::down_cast<VrmlData_Appearance>(myScene.FindNode(aNodeName.ToCString()));
  if (anAppearance.IsNull())
  {
    occ::handle<VrmlData_Material> aMaterial = new VrmlData_Material(myScene, nullptr);
    aMaterial->SetDiffuseColor(aColor.GetRGB());
    myScene.AddNode(aMaterial, false);
    anAppearance = new VrmlData_Appearance(myScene, aNodeName.ToCString());
    anAppearance->SetMaterial(aMaterial);
    myScene.AddNode(anAppearance, false);
  }

  return anAppearance;
}
