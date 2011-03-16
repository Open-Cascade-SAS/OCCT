// File:      NIS_Surface.cpp
// Created:   20.03.08 08:27
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade S.A. 2008

#include <NIS_Surface.hxx>
#include <NIS_SurfaceDrawer.hxx>
#include <NIS_Triangulated.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Surface.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Ax1.hxx>

IMPLEMENT_STANDARD_HANDLE (NIS_Surface, NIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(NIS_Surface, NIS_InteractiveObject)

//=======================================================================
//function : defaultDrawer
//purpose  : internal method (static)
//=======================================================================

inline Handle(NIS_SurfaceDrawer) defaultDrawer()
{
  const Handle(NIS_SurfaceDrawer) aDrawer =
    new NIS_SurfaceDrawer(Quantity_NOC_SLATEBLUE4);
  aDrawer->SetBackColor (Quantity_NOC_DARKGREEN);
  return aDrawer;
}

//=======================================================================
//function : NIS_Surface
//purpose  : 
//=======================================================================

NIS_Surface::NIS_Surface
                        (const Handle(Poly_Triangulation)&       theTri,
                         const Handle_NCollection_BaseAllocator& theAlloc)
: mypNodes              (NULL),
  mypNormals            (NULL),
  myNNodes              (0),
  myNTriangles          (0),
  myAlloc               (theAlloc)
{
  if (myAlloc.IsNull())
    myAlloc = NCollection_BaseAllocator::CommonBaseAllocator();
  if (theTri.IsNull() == Standard_False)
  {
    // Alocate arrays of entities
    myNNodes = 3 * theTri->NbTriangles();
    myNTriangles = theTri->NbTriangles();
    mypNodes = static_cast<Standard_ShortReal*>
      (myAlloc->Allocate(sizeof(Standard_ShortReal) * 3 * myNNodes));
    mypNormals = static_cast<Standard_ShortReal *>
      (myAlloc->Allocate(sizeof(Standard_ShortReal) * 3 * myNNodes));
    mypTriangles = static_cast<Standard_Integer*>
      (myAlloc->Allocate(sizeof(Standard_Integer) * 3 * myNTriangles));

    // Copy the data from the original triangulation.
    Standard_Integer i, iN(0), iT(0);
    const Poly_Array1OfTriangle& arrTri = theTri->Triangles();
    const TColgp_Array1OfPnt& arrNodes = theTri->Nodes();
    for (i = arrTri.Lower(); i <= arrTri.Upper(); i++) {
      Standard_Integer iNode[3];
      arrTri(i).Get(iNode[0], iNode[1], iNode[2]);
      gp_XYZ aNorm = ((arrNodes(iNode[1]).XYZ() - arrNodes(iNode[0]).XYZ()) ^
                      (arrNodes(iNode[2]).XYZ() - arrNodes(iNode[0]).XYZ()));
      const Standard_Real aMagn = aNorm.Modulus();
      if (aMagn > Precision::Confusion())
        aNorm /= aMagn;
      else
        aNorm.SetCoord(0., 0., 1.);
      mypNodes[iN+0] = static_cast<Standard_ShortReal>(arrNodes(iNode[0]).X());
      mypNodes[iN+1] = static_cast<Standard_ShortReal>(arrNodes(iNode[0]).Y());
      mypNodes[iN+2] = static_cast<Standard_ShortReal>(arrNodes(iNode[0]).Z());
      mypNodes[iN+3] = static_cast<Standard_ShortReal>(arrNodes(iNode[1]).X());
      mypNodes[iN+4] = static_cast<Standard_ShortReal>(arrNodes(iNode[1]).Y());
      mypNodes[iN+5] = static_cast<Standard_ShortReal>(arrNodes(iNode[1]).Z());
      mypNodes[iN+6] = static_cast<Standard_ShortReal>(arrNodes(iNode[2]).X());
      mypNodes[iN+7] = static_cast<Standard_ShortReal>(arrNodes(iNode[2]).Y());
      mypNodes[iN+8] = static_cast<Standard_ShortReal>(arrNodes(iNode[2]).Z());
      mypNormals[iN+0] = static_cast<Standard_ShortReal>(aNorm.X());
      mypNormals[iN+1] = static_cast<Standard_ShortReal>(aNorm.Y());
      mypNormals[iN+2] = static_cast<Standard_ShortReal>(aNorm.Z());
      mypNormals[iN+3] = static_cast<Standard_ShortReal>(aNorm.X());
      mypNormals[iN+4] = static_cast<Standard_ShortReal>(aNorm.Y());
      mypNormals[iN+5] = static_cast<Standard_ShortReal>(aNorm.Z());
      mypNormals[iN+6] = static_cast<Standard_ShortReal>(aNorm.X());
      mypNormals[iN+7] = static_cast<Standard_ShortReal>(aNorm.Y());
      mypNormals[iN+8] = static_cast<Standard_ShortReal>(aNorm.Z());
      mypTriangles[iT+0] = iT+0;
      mypTriangles[iT+1] = iT+1;
      mypTriangles[iT+2] = iT+2;
      iN += 9;
      iT += 3;
    }
  }
}

//=======================================================================
//function : NIS_Surface
//purpose  : Constructor
//=======================================================================

NIS_Surface::NIS_Surface
                            (const TopoDS_Shape&                theShape,
//                           const Standard_Real                theDeflection,
                             const Handle_NCollection_BaseAllocator& theAlloc)
  : mypNodes      (NULL),
    mypNormals    (NULL),
    mypTriangles  (NULL),
    myNNodes      (0),
    myNTriangles  (0),
    myAlloc       (theAlloc)
{
  if (myAlloc.IsNull())
    myAlloc = NCollection_BaseAllocator::CommonBaseAllocator();
  TopLoc_Location  aLoc, aLocSurf;

  // Count the nodes and triangles in faces
  TopExp_Explorer fexp (theShape, TopAbs_FACE);
  for ( ; fexp.More(); fexp.Next() )
  {
    TopoDS_Face aFace = TopoDS::Face(fexp.Current());
    
    const Handle(Poly_Triangulation)& aTriangulation
      = BRep_Tool::Triangulation (aFace, aLoc);
    const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface(aFace, aLoc);

    if (aTriangulation.IsNull() == Standard_False &&
        aSurf.IsNull() == Standard_False)
    {
      myNNodes     += aTriangulation->NbNodes();
      myNTriangles += aTriangulation->NbTriangles();
    }
  }

  // Alocate arrays of entities
  if (myNNodes && myNTriangles) {
    mypNodes = static_cast<Standard_ShortReal*>
      (myAlloc->Allocate(sizeof(Standard_ShortReal) * 3 * myNNodes));
    mypNormals = static_cast<Standard_ShortReal *>
      (myAlloc->Allocate(sizeof(Standard_ShortReal) * 3 * myNNodes));
    mypTriangles = static_cast<Standard_Integer*>
      (myAlloc->Allocate(sizeof(Standard_Integer) * 3 * myNTriangles));

    // The second loop: copy all nodes and triangles face-by-face
    const Standard_Real eps2 = Precision::Confusion()*Precision::Confusion();
    Standard_Integer nNodes (0), nTriangles (0);
    for (fexp.ReInit(); fexp.More(); fexp.Next())
    {
      const TopoDS_Face& aFace = TopoDS::Face(fexp.Current());
      const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface(aFace, aLocSurf);
      const Handle(Poly_Triangulation)& aTriangulation =
        BRep_Tool::Triangulation(aFace, aLoc);
      if (aTriangulation.IsNull() == Standard_False &&
          aSurf.IsNull() == Standard_False)
      {
        // Prepare transformation
        Standard_Integer i, aNodeInd(nNodes)/*, aNTriangles = 0*/;
        const gp_Trsf&   aTrf     = aLoc.Transformation();
        const gp_Trsf&   aTrfSurf = aLocSurf.Transformation();
        Standard_Boolean isReverse = (aFace.Orientation() == TopAbs_REVERSED);

        // Store all nodes of the current face in the data model
        const TColgp_Array1OfPnt&   tabNode = aTriangulation->Nodes();
        const TColgp_Array1OfPnt2d& tabUV   = aTriangulation->UVNodes();
        for (i = tabNode.Lower(); i <= tabNode.Upper(); i++)
        {
          Standard_Real t[3];
          tabNode(i).Transformed(aTrf).Coord (t[0], t[1], t[2]);
          //  write node to mesh data
          mypNodes[3*aNodeInd + 0] = static_cast<Standard_ShortReal>(t[0]);
          mypNodes[3*aNodeInd + 1] = static_cast<Standard_ShortReal>(t[1]);
          mypNodes[3*aNodeInd + 2] = static_cast<Standard_ShortReal>(t[2]);

          gp_Vec aD1U, aD1V;
          gp_Pnt aP;

          // Compute the surface normal at the Node.
          aSurf->D1(tabUV(i).X(), tabUV(i).Y(), aP, aD1U, aD1V);
          gp_XYZ aNorm = (aD1U.Crossed(aD1V)).XYZ();
          if (isReverse)
            aNorm.Reverse();
          const Standard_Real aMod = aNorm.SquareModulus();
          if (aMod > eps2) {
            gp_Dir aDirNorm(aNorm);
            aDirNorm.Transform(aTrfSurf);
            aDirNorm.Coord (t[0], t[1], t[2]);
          } else {
            t[0] = 0.;
            t[1] = 0.;
            t[2] = 1.;
          }
          mypNormals[3*aNodeInd + 0] = static_cast<Standard_ShortReal>(t[0]);
          mypNormals[3*aNodeInd + 1] = static_cast<Standard_ShortReal>(t[1]);
          mypNormals[3*aNodeInd + 2] = static_cast<Standard_ShortReal>(t[2]);

          aNodeInd++;
        }
        // Store all triangles of the current face in the data model
        const Poly_Array1OfTriangle& tabTri  = aTriangulation->Triangles();
        for (i = tabTri.Lower(); i <= tabTri.Upper(); i++)
        {
          Standard_Integer aN[3];
          tabTri(i).Get (aN[0], aN[1], aN[2]);
          if (((tabNode(aN[2]).XYZ() -
                tabNode(aN[0]).XYZ()) ^
               (tabNode(aN[1]).XYZ() -
                tabNode(aN[0]).XYZ())).SquareModulus() > eps2)
          {
            aN[0] += (nNodes - 1);
            aN[1] += (nNodes - 1);
            aN[2] += (nNodes - 1);
            mypTriangles[nTriangles*3 + 0] = aN[0];
            if (isReverse) {
              mypTriangles[nTriangles*3 + 1] = aN[2];
              mypTriangles[nTriangles*3 + 2] = aN[1];
            } else {
              mypTriangles[nTriangles*3 + 1] = aN[1];
              mypTriangles[nTriangles*3 + 2] = aN[2];
            }
            nTriangles++;
          }
        }
        nNodes += tabNode.Length();
      }
    }
    myNTriangles = nTriangles;
  }
}

//=======================================================================
//function : ~NIS_Surface
//purpose  : Destructor
//=======================================================================

NIS_Surface::~NIS_Surface ()
{
  if (myNNodes) {
    myNNodes = 0;
    myAlloc->Free(mypNodes);
    myAlloc->Free(mypNormals);
  }
  if (myNTriangles) {
    myNTriangles = 0;
    myAlloc->Free(mypTriangles);
  }
}

//=======================================================================
//function : DefaultDrawer
//purpose  : 
//=======================================================================

Handle(NIS_Drawer) NIS_Surface::DefaultDrawer () const
{
  return defaultDrawer();
}

//=======================================================================
//function : SetColor
//purpose  : Set the normal color for presentation.
//=======================================================================

void NIS_Surface::SetColor (const Quantity_Color&  theColor)
{
  Handle(NIS_SurfaceDrawer) aDrawer = defaultDrawer();
  aDrawer->Assign (GetDrawer());
  aDrawer->myColor[NIS_Drawer::Draw_Normal] = theColor;
  aDrawer->myColor[NIS_Drawer::Draw_Transparent] = theColor;
  SetDrawer (aDrawer);
}

//=======================================================================
//function : SetBackColor
//purpose  : Set the normal color for presentation of back side of triangles.
//=======================================================================

void NIS_Surface::SetBackColor (const Quantity_Color&  theColor)
{
  Handle(NIS_SurfaceDrawer) aDrawer = defaultDrawer();
  aDrawer->Assign (GetDrawer());
  aDrawer->myBackColor = theColor;
  SetDrawer (aDrawer);
}

//=======================================================================
//function : SetPolygonOffset
//purpose  : 
//=======================================================================

void NIS_Surface::SetPolygonOffset (const Standard_Real theValue)
{
  Handle(NIS_SurfaceDrawer) aDrawer = defaultDrawer();
  aDrawer->Assign (GetDrawer());
  aDrawer->myPolygonOffset = theValue;
  SetDrawer (aDrawer);
}

//=======================================================================
//function : SetTransparency
//purpose  : 
//=======================================================================

void NIS_Surface::SetTransparency (const Standard_Real theValue)
{
  Handle(NIS_SurfaceDrawer) aDrawer = defaultDrawer();
  aDrawer->Assign (GetDrawer());
  aDrawer->myTransparency = theValue;
  SetDrawer (aDrawer);
}

//=======================================================================
//function : Intersect
//purpose  : 
//=======================================================================

Standard_Real NIS_Surface::Intersect (const gp_Ax1&       theAxis,
                                            const Standard_Real /*over*/) const
{
  Standard_Real aResult (RealLast());
  Standard_Real start[3], dir[3];
  theAxis.Location().Coord(start[0], start[1], start[2]);
  theAxis.Direction().Coord(dir[0], dir[1], dir[2]);
  double anInter;

  for (Standard_Integer i = 0; i < myNTriangles; i++) {
    const Standard_Integer * pTri = &mypTriangles[3*i];
    if (NIS_Triangulated::tri_line_intersect (start, dir,
                                              &mypNodes[3*pTri[0]],
                                              &mypNodes[3*pTri[1]],
                                              &mypNodes[3*pTri[2]],
                                              &anInter))
      if (anInter < aResult)
        aResult = anInter;
  }

  return aResult;
}

//=======================================================================
//function : Intersect
//purpose  : 
//=======================================================================

Standard_Boolean NIS_Surface::Intersect
                                        (const Bnd_B3f&         theBox,
                                         const gp_Trsf&         theTrf,
                                         const Standard_Boolean isFullIn) const
{
  Standard_Boolean aResult (isFullIn);

  if (myNTriangles > 0) {
    for (Standard_Integer iNode = 0; iNode < myNNodes*3; iNode+=3) {
      gp_XYZ aPnt (static_cast<Standard_Real>(mypNodes[iNode+0]),
                   static_cast<Standard_Real>(mypNodes[iNode+1]),
                   static_cast<Standard_Real>(mypNodes[iNode+2]));
      theTrf.Transforms(aPnt);
      if (theBox.IsOut (aPnt) == isFullIn) {
        aResult = !isFullIn;
        break;
      }
    }
  }
  return aResult;
}

//=======================================================================
//function : computeBox
//purpose  : 
//=======================================================================

void NIS_Surface::computeBox ()
{
  NIS_Triangulated::ComputeBox(myBox, myNNodes, mypNodes);

  const Handle(NIS_SurfaceDrawer)& aDrawer =
    static_cast<const Handle(NIS_SurfaceDrawer)&> (GetDrawer());

  if (aDrawer.IsNull() == Standard_False) {
    const gp_Trsf& aTrsf = aDrawer->GetTransformation();
    myBox = myBox.Transformed(aTrsf);
  }
}
