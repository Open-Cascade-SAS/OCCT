// File:      NIS_Triangulated.cxx
// Created:   17.07.07 08:46
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007


#include <NIS_Triangulated.hxx>
#include <NIS_TriangulatedDrawer.hxx>
#include <NIS_InteractiveContext.hxx>
#include <gp_Ax1.hxx>
#include <Precision.hxx>

IMPLEMENT_STANDARD_HANDLE  (NIS_Triangulated, NIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT (NIS_Triangulated, NIS_InteractiveObject)

inline Handle(NIS_TriangulatedDrawer) defaultDrawer ()
{
  return new NIS_TriangulatedDrawer (Quantity_NOC_RED);
}

//=======================================================================
//function : NIS_Triangulated()
//purpose  : Constructor
//=======================================================================

NIS_Triangulated::NIS_Triangulated
                        (const Standard_Integer                   nNodes,
                         const Handle(NCollection_BaseAllocator)& theAlloc)
  : myType              (Type_None),
    mypNodes            (0L),
    mypTriangles        (0L),
    mypLines            (0L),
    mypPolygons         (0L),
    myNNodes            (0),
    myNTriangles        (0),
    myNPolygons         (0),
    myNLineNodes        (0),
    myAlloc             (0L),
    myIsDrawPolygons    (Standard_False)
{
  
  if (theAlloc.IsNull())
    myAlloc = NCollection_BaseAllocator::CommonBaseAllocator().operator->();
  else
    myAlloc = theAlloc.operator->();
  allocateNodes (nNodes);
}

//=======================================================================
//function : Clear
//purpose  : Reset all internal data members and structures
//=======================================================================

void NIS_Triangulated::Clear ()
{
  if (myNNodes) {
    myNNodes = 0;
    myAlloc->Free(mypNodes);
    mypNodes = 0L;
  }
  if (myNTriangles) {
    myNTriangles = 0;
    myAlloc->Free(mypTriangles);
    mypTriangles = 0L;
  }
  if (myNLineNodes) {
    myNLineNodes = 0;
    myAlloc->Free( mypLines);
    mypLines = 0L;
  }
  if (myNPolygons) {
    for (Standard_Integer i = 0; i < myNPolygons; i++)
      myAlloc->Free(mypPolygons[i]);
    myAlloc->Free(mypPolygons);
    myNPolygons = 0;
    mypPolygons = 0L;
  }
  myType = Type_None;
  myIsDrawPolygons = Standard_False;
}

//=======================================================================
//function : SetPolygonsPrs
//purpose  : 
//=======================================================================

void NIS_Triangulated::SetPolygonsPrs (const Standard_Integer nPolygons,
                                       const Standard_Integer nNodes)
{
  if (nPolygons <= 0)
    myType &= ~Type_Polygons;
  else {
    myType |= Type_Polygons;
    if (myNPolygons) {
      for (Standard_Integer i = 0; i < myNPolygons; i++)
        myAlloc->Free(mypPolygons[i]);
      myAlloc->Free(mypPolygons);
    }
    myNPolygons = nPolygons;
    mypPolygons = static_cast<Standard_Integer **>
      (myAlloc->Allocate(sizeof(Standard_Integer *)*nPolygons));
    allocateNodes (nNodes);
  }
}

//=======================================================================
//function : SetTriangulationPrs
//purpose  : 
//=======================================================================

void NIS_Triangulated::SetTriangulationPrs (const Standard_Integer nTri,
                                            const Standard_Integer nNodes)
{
  if (nTri <= 0)
    myType &= ~Type_Triangulation;
  else {
    myType |= Type_Triangulation;
    if (myNTriangles)
      myAlloc->Free(mypTriangles);
    myNTriangles = nTri;
    mypTriangles = static_cast<Standard_Integer *>
      (myAlloc->Allocate(sizeof(Standard_Integer)*3*nTri));
    allocateNodes (nNodes);
  }
}

//=======================================================================
//function : SetLinePrs
//purpose  : 
//=======================================================================

void NIS_Triangulated::SetLinePrs (const Standard_Integer nPoints,
                                   const Standard_Boolean isClosed,
                                   const Standard_Integer nNodes)
{
  if (nPoints <= 0)
    myType &= ~(Type_Loop | Type_Line);
  else {
    myType |= Type_Line;
    if (isClosed)
      myType |= Type_Loop;
    if (myNLineNodes)
      myAlloc->Free(mypLines);
    myType &= ~Type_Segments;
    myNLineNodes = nPoints;
    mypLines = static_cast<Standard_Integer *>
      (myAlloc->Allocate(sizeof(Standard_Integer)*nPoints));
    allocateNodes (nNodes);
  }
}

//=======================================================================
//function : SetSegmentPrs
//purpose  : 
//=======================================================================

void NIS_Triangulated::SetSegmentPrs (const Standard_Integer nSegments,
                                      const Standard_Integer nNodes)
{
  if (nSegments <= 0)
    myType &= ~(Type_Loop | Type_Segments);
  else {
    myType |= Type_Segments;
    if (myNLineNodes)
      myAlloc->Free(mypLines);
    myType &= ~(Type_Line | Type_Loop);
    myNLineNodes = nSegments*2;
    mypLines = static_cast<Standard_Integer *>
      (myAlloc->Allocate(sizeof(Standard_Integer)*myNLineNodes));
    allocateNodes (nNodes);
  }
}

//=======================================================================
//function : ~NIS_Triangulated
//purpose  : Destructor
//=======================================================================

NIS_Triangulated::~NIS_Triangulated ()
{
  Clear();
}

//=======================================================================
//function : DefaultDrawer
//purpose  : 
//=======================================================================

Handle(NIS_Drawer) NIS_Triangulated::DefaultDrawer () const
{
  return defaultDrawer();
}

//=======================================================================
//function : ComputeBox
//purpose  : 
//=======================================================================

void NIS_Triangulated::ComputeBox (Bnd_B3f&                  theBox,
                                   const Standard_Integer    nNodes,
                                   const Standard_ShortReal* pNodes)
{
  theBox.Clear();
  if (nNodes > 0) {
    Standard_ShortReal aBox[6] = {
      pNodes[0], pNodes[1], pNodes[2],
      pNodes[0], pNodes[1], pNodes[2]
    };
    for (Standard_Integer i = 1; i < nNodes; i++) {
      const Standard_ShortReal * pNode = &pNodes[i*3];
      if (aBox[0] > pNode[0])
        aBox[0] = pNode[0];
      else if (aBox[3] < pNode[0])
        aBox[3] = pNode[0];
      if (aBox[1] > pNode[1])
        aBox[1] = pNode[1];
      else if (aBox[4] < pNode[1])
        aBox[4] = pNode[1];
      if (aBox[2] > pNode[2])
        aBox[2] = pNode[2];
      else if (aBox[5] < pNode[2])
        aBox[5] = pNode[2];
    }
    theBox.Add (gp_XYZ (Standard_Real(aBox[0]),
                        Standard_Real(aBox[1]),
                        Standard_Real(aBox[2])));
    theBox.Add (gp_XYZ (Standard_Real(aBox[3]),
                        Standard_Real(aBox[4]),
                        Standard_Real(aBox[5])));
  }
}

//=======================================================================
//function : computeBox
//purpose  : 
//=======================================================================

void NIS_Triangulated::computeBox ()
{
  ComputeBox (myBox, myNNodes, mypNodes);
}

//=======================================================================
//function : SetNode
//purpose  : 
//=======================================================================

void NIS_Triangulated::SetNode       (const Standard_Integer  ind,
                                      const gp_XYZ&           thePnt)
{
  if (ind >= myNNodes)
    Standard_OutOfRange::Raise ("NIS_Triangulated::SetNode");
  Standard_ShortReal * pNode = &mypNodes[3*ind];
  pNode[0] = Standard_ShortReal(thePnt.X());
  pNode[1] = Standard_ShortReal(thePnt.Y());
  pNode[2] = Standard_ShortReal(thePnt.Z());
}

//=======================================================================
//function : SetNode
//purpose  : 
//=======================================================================

void NIS_Triangulated::SetNode       (const Standard_Integer  ind,
                                      const gp_XY&            thePnt)
{
  if (ind >= myNNodes)
    Standard_OutOfRange::Raise ("NIS_Triangulated::SetNode");
  Standard_ShortReal * pNode = &mypNodes[3*ind];
  pNode[0] = Standard_ShortReal(thePnt.X());
  pNode[1] = Standard_ShortReal(thePnt.Y());
  pNode[2] = 0.f;
}

//=======================================================================
//function : SetTriangle
//purpose  : 
//=======================================================================

void NIS_Triangulated::SetTriangle   (const Standard_Integer  ind,
                                      const Standard_Integer  iNode0,
                                      const Standard_Integer  iNode1,
                                      const Standard_Integer  iNode2)
{
  if (ind >= myNTriangles)
    Standard_OutOfRange::Raise ("NIS_Triangulated::SetTriangle");
  Standard_Integer * pTri = &mypTriangles[3*ind];
  pTri[0] = iNode0;
  pTri[1] = iNode1;
  pTri[2] = iNode2;
}

//=======================================================================
//function : SetLineNode
//purpose  : 
//=======================================================================

void NIS_Triangulated::SetLineNode      (const Standard_Integer ind,
                                         const Standard_Integer iNode)
{
  if (ind >= myNLineNodes)
    Standard_OutOfRange::Raise ("NIS_Triangulated::SetTriangle");
  mypLines[ind] = iNode;
}

//=======================================================================
//function : SetPolygon
//purpose  : 
//=======================================================================

Standard_Integer* NIS_Triangulated::SetPolygon (const Standard_Integer  ind,
                                                const Standard_Integer  theSz)
{
  if (ind >= myNPolygons)
    Standard_OutOfRange::Raise ("NIS_Triangulated::SetPolygon");
  Standard_Integer * anArray  = static_cast <Standard_Integer *>
    (myAlloc->Allocate (sizeof(Standard_Integer) * (theSz+1)));
  mypPolygons[ind] = anArray;
  anArray[0] = theSz;
  return &anArray[1];
}

//=======================================================================
//function : SetDrawPolygons
//purpose  : 
//=======================================================================

void NIS_Triangulated::SetDrawPolygons (const Standard_Boolean isDrawPolygons,
                                        const Standard_Boolean isUpdateViews)
{
  if (myIsDrawPolygons != isDrawPolygons) {
    Handle(NIS_TriangulatedDrawer) aDrawer = defaultDrawer();
    aDrawer->Assign (GetDrawer());
    aDrawer->myIsDrawPolygons = isDrawPolygons;
    SetDrawer (aDrawer);
    myIsDrawPolygons = isDrawPolygons;
  }
  if (isUpdateViews)
    GetDrawer()->GetContext()->UpdateViews();
}

//=======================================================================
//function : SetColor
//purpose  : Set the normal color for presentation.
//=======================================================================

void NIS_Triangulated::SetColor (const Quantity_Color&  theColor,
                                 const Standard_Boolean isUpdateViews)
{
  Handle(NIS_TriangulatedDrawer) aDrawer = defaultDrawer();
  aDrawer->Assign (GetDrawer());
  aDrawer->myColor[NIS_Drawer::Draw_Normal] = theColor;
  aDrawer->myColor[NIS_Drawer::Draw_Transparent] = theColor;
  SetDrawer (aDrawer);
  if (isUpdateViews)
    GetDrawer()->GetContext()->UpdateViews();
}

//=======================================================================
//function : GetColor
//purpose  : Get Normal, Transparent or Hilighted color of the presentation.
//=======================================================================

Quantity_Color NIS_Triangulated::GetColor (const NIS_Drawer::DrawType theDrawType) const
{
    Handle(NIS_TriangulatedDrawer) aDrawer = 
        Handle(NIS_TriangulatedDrawer)::DownCast( GetDrawer() );
    if (aDrawer.IsNull() == Standard_False)
    {
        return aDrawer->myColor[theDrawType];
    }
    return Quantity_Color(); // return null color object
}

//=======================================================================
//function : SetHilightColor
//purpose  : Set the color for hilighted presentation.
//=======================================================================

void NIS_Triangulated::SetHilightColor (const Quantity_Color&  theColor,
                                        const Standard_Boolean isUpdateViews)
{
  Handle(NIS_TriangulatedDrawer) aDrawer = defaultDrawer();
  aDrawer->Assign (GetDrawer());
  aDrawer->myColor[NIS_Drawer::Draw_Hilighted] = theColor;
  SetDrawer (aDrawer);
  if (isUpdateViews)
    GetDrawer()->GetContext()->UpdateViews();
}

//=======================================================================
//function : SetDynHilightColor
//purpose  : Set the color for dynamic hilight presentation.
//=======================================================================

void NIS_Triangulated::SetDynHilightColor(const Quantity_Color&  theColor,
                                          const Standard_Boolean isUpdateViews)
{
  Handle(NIS_TriangulatedDrawer) aDrawer = defaultDrawer();
  aDrawer->Assign (GetDrawer());
  aDrawer->myColor[NIS_Drawer::Draw_DynHilighted] = theColor;
  SetDrawer (aDrawer);
  if (isUpdateViews)
    GetDrawer()->GetContext()->UpdateViews();
}

//=======================================================================
//function : SetLineWidth
//purpose  : Set the width of line presentations in pixels.
//=======================================================================

void NIS_Triangulated::SetLineWidth (const Standard_Real    theWidth,
                                     const Standard_Boolean isUpdateViews)
{
  Handle(NIS_TriangulatedDrawer) aDrawer = defaultDrawer();
  aDrawer->Assign (GetDrawer());
  aDrawer->myLineWidth = (Standard_ShortReal) theWidth;
  SetDrawer (aDrawer);
  if (isUpdateViews)
    GetDrawer()->GetContext()->UpdateViews();
}

//=======================================================================
//function : Intersect
//purpose  : 
//=======================================================================

Standard_Boolean NIS_Triangulated::Intersect
                                        (const Bnd_B3f&         theBox,
                                         const gp_Trsf&         theTrf,
                                         const Standard_Boolean isFullIn) const
{
  Standard_Boolean aResult (isFullIn);

  if ((myType & Type_Triangulation) && myIsDrawPolygons == Standard_False)
    for (Standard_Integer iNode = 0; iNode < myNNodes*3; iNode+=3) {
      gp_XYZ aPnt ((Standard_Real)mypNodes[iNode+0],
                   (Standard_Real)mypNodes[iNode+1],
                   (Standard_Real)mypNodes[iNode+2]);
      theTrf.Transforms(aPnt);
      if (theBox.IsOut (aPnt)) {
        if (isFullIn) {
          aResult = Standard_False;
          break;
        }
      } else {
        if (isFullIn == Standard_False) {
          aResult = Standard_True;
          break;
        }
      }
    }
  if (aResult == isFullIn) {
    if (myType & Type_Segments) {
      for (Standard_Integer i = 0; i < myNLineNodes; i+=2) {
        const gp_Pnt aPnt[2] = {
          gp_Pnt (mypNodes[3*mypLines[i+0]+0],
                  mypNodes[3*mypLines[i+0]+1],
                  mypNodes[3*mypLines[i+0]+2]).Transformed(theTrf),
          gp_Pnt (mypNodes[3*mypLines[i+1]+0],
                  mypNodes[3*mypLines[i+1]+1],
                  mypNodes[3*mypLines[i+1]+2]).Transformed(theTrf)
        };
        if (isFullIn) {
          if (seg_box_included (theBox, aPnt) == 0) {
            aResult = Standard_False;
            break;
          }
        } else {
          if (seg_box_intersect (theBox, aPnt)) {
            aResult = Standard_True;
            break;
          }
        }
      }
    } else if (myType & Type_Line) {
      for (Standard_Integer i = 1; i < myNLineNodes; i++) {
        const gp_Pnt aPnt[2] = {
          gp_Pnt (mypNodes[3*mypLines[i-1]+0],
                  mypNodes[3*mypLines[i-1]+1],
                  mypNodes[3*mypLines[i-1]+2]).Transformed(theTrf),
          gp_Pnt (mypNodes[3*mypLines[i+0]+0],
                  mypNodes[3*mypLines[i+0]+1],
                  mypNodes[3*mypLines[i+0]+2]).Transformed(theTrf)
        };
        if (isFullIn) {
          if (seg_box_included (theBox, aPnt) == 0) {
            aResult = Standard_False;
            break;
          }
        } else {
          if (seg_box_intersect (theBox, aPnt)) {
            aResult = Standard_True;
            break;
          }
        }
      }
      if (aResult == isFullIn && (myType & Type_Loop)) {
        const gp_Pnt aPntLast[2] = {
          gp_Pnt (mypNodes[3*mypLines[myNLineNodes-1]+0],
                  mypNodes[3*mypLines[myNLineNodes-1]+1],
                  mypNodes[3*mypLines[myNLineNodes-1]+2]).Transformed(theTrf),
          gp_Pnt (mypNodes[3*mypLines[0]+0],
                  mypNodes[3*mypLines[0]+1],
                  mypNodes[3*mypLines[0]+2]).Transformed(theTrf)
        };
        if (isFullIn) {
          if (seg_box_included (theBox, aPntLast) == 0)
            aResult = Standard_False;
        } else {
          if (seg_box_intersect (theBox, aPntLast))
            aResult = Standard_True;
        }
      }
    } else if ((myType & Type_Polygons) && myIsDrawPolygons) {
      for (Standard_Integer iPoly = 0; iPoly < myNPolygons; iPoly++) {
        const Standard_Integer nNodes = * mypPolygons[iPoly];
        const Standard_Integer * arrNodes = mypPolygons[iPoly] + 1;
        for (Standard_Integer i = 1; i < nNodes; i++) {
          const gp_Pnt aPnt[2] = {
            gp_Pnt (mypNodes[3*arrNodes[i-1]+0],
                    mypNodes[3*arrNodes[i-1]+1],
                    mypNodes[3*arrNodes[i-1]+2]).Transformed(theTrf),
            gp_Pnt (mypNodes[3*arrNodes[i+0]+0],
                    mypNodes[3*arrNodes[i+0]+1],
                    mypNodes[3*arrNodes[i+0]+2]).Transformed(theTrf)
          };
          if (isFullIn) {
            if (seg_box_included (theBox, aPnt) == 0) {
              aResult = Standard_False;
              break;
            }
          } else {
            if (seg_box_intersect (theBox, aPnt)) {
              aResult = Standard_True;
              break;
            }
          }
        }
        if (aResult == isFullIn) {
          const gp_Pnt aPntLast[2] = {
            gp_Pnt (mypNodes[3*arrNodes[nNodes-1]+0],
                    mypNodes[3*arrNodes[nNodes-1]+1],
                    mypNodes[3*arrNodes[nNodes-1]+2]).Transformed(theTrf),
            gp_Pnt (mypNodes[3*arrNodes[0]+0],
                    mypNodes[3*arrNodes[0]+1],
                    mypNodes[3*arrNodes[0]+2]).Transformed(theTrf)
          };
          if (isFullIn) {
            if (seg_box_included (theBox, aPntLast) == 0)
              aResult = Standard_False;
          } else {
            if (seg_box_intersect (theBox, aPntLast))
              aResult = Standard_True;
          }
        }
      }
    }
  }
  return aResult;
}

//=======================================================================
//function : Intersect
//purpose  : 
//=======================================================================

Standard_Real NIS_Triangulated::Intersect (const gp_Ax1&       theAxis,
                                           const Standard_Real theOver) const
{
  Standard_Real aResult (RealLast());
  Standard_Real start[3], dir[3];
  theAxis.Location().Coord(start[0], start[1], start[2]);
  theAxis.Direction().Coord(dir[0], dir[1], dir[2]);
  double anInter;

  if ((myType & Type_Triangulation) && (myIsDrawPolygons == Standard_False))
    for (Standard_Integer i = 0; i < myNTriangles; i++) {
      const Standard_Integer * pTri = &mypTriangles[3*i];
      if (tri_line_intersect (start, dir,
                              &mypNodes[3*pTri[0]],
                              &mypNodes[3*pTri[1]],
                              &mypNodes[3*pTri[2]],
                              &anInter))
        if (anInter < aResult)
          aResult = anInter;
    }

  const Standard_Real anOver2 = theOver*theOver;
  if (myType & Type_Segments) {
    for (Standard_Integer i = 0; i < myNLineNodes; i+=2) {
      if (seg_line_intersect (theAxis.Location().XYZ(),
                              theAxis.Direction().XYZ(), anOver2,
                              &mypNodes[3*mypLines[i+0]],
                              &mypNodes[3*mypLines[i+1]],
                              &anInter))
        if (anInter < aResult)
          aResult = anInter;
    }
  } else if (myType & Type_Line) {
    for (Standard_Integer i = 1; i < myNLineNodes; i++) {
      if (seg_line_intersect (theAxis.Location().XYZ(),
                              theAxis.Direction().XYZ(), anOver2,
                              &mypNodes[3*mypLines[i-1]],
                              &mypNodes[3*mypLines[i-0]],
                              &anInter))
        if (anInter < aResult)
          aResult = anInter;
    }
    if (myType & Type_Loop)
      if (seg_line_intersect (theAxis.Location().XYZ(),
                              theAxis.Direction().XYZ(), anOver2,
                              &mypNodes[3*mypLines[myNLineNodes-1]],
                              &mypNodes[3*mypLines[0]],
                              &anInter))
        if (anInter < aResult)
          aResult = anInter;
  }

  if ((myType & Type_Polygons) && myIsDrawPolygons) {
    for (Standard_Integer iPoly = 0; iPoly < myNPolygons; iPoly++) {
      const Standard_Integer nNodes = * mypPolygons[iPoly];
      const Standard_Integer * arrNodes = mypPolygons[iPoly] + 1;
      for (Standard_Integer i = 1; i < nNodes; i++) {
        if (seg_line_intersect (theAxis.Location().XYZ(),
                                theAxis.Direction().XYZ(), anOver2,
                                &mypNodes[3*arrNodes[i-1]],
                                &mypNodes[3*arrNodes[i-0]],
                                &anInter))
          if (anInter < aResult)
            aResult = anInter;
      }
      if (seg_line_intersect (theAxis.Location().XYZ(),
                              theAxis.Direction().XYZ(), anOver2,
                              &mypNodes[3*arrNodes[nNodes-1]],
                              &mypNodes[3*arrNodes[0]],
                              &anInter))
        if (anInter < aResult)
          aResult = anInter;
    }
  }
  return aResult;
};

/* =====================================================================
Function   : determinant
Purpose    : Calculate the minor of the given matrix, defined by the columns
             specified by values c1, c2, c3
Parameters : 
Returns    : determinant value
History    : 
======================================================================== */

static double determinant (const double a[3][4],
                           const int    c1,
                           const int    c2,
                           const int    c3)
{
  return a[0][c1]*a[1][c2]*a[2][c3] +
         a[0][c2]*a[1][c3]*a[2][c1] +
         a[0][c3]*a[1][c1]*a[2][c2] -
         a[0][c3]*a[1][c2]*a[2][c1] -
         a[0][c2]*a[1][c1]*a[2][c3] -
         a[0][c1]*a[1][c3]*a[2][c2];
}

/* =====================================================================
Function   : tri_line_intersect
Purpose    : Intersect a triangle with a line
Parameters : start  - coordinates of the origin of the line
             dir    - coordinates of the direction of the line (normalized)
             V0     - first vertex of the triangle
             V1     - second vertex of the triangle
             V2     - third vertex of the triangle
             tInter - output value, contains the parameter of the intersection
                      point on the line (if found). May be NULL pointer
Returns    : int = 1 if intersection found, 0 otherwise
======================================================================== */

int NIS_Triangulated::tri_line_intersect (const double      start[3],
                                          const double      dir[3],
                                          const float       V0[3],
                                          const float       V1[3],
                                          const float       V2[3],
                                          double            * tInter)
{
  int res = 0;
  const double conf = 1E-15;

  const double array[][4] = {
    { -dir[0],
      double(V1[0] - V0[0]), double(V2[0] - V0[0]), start[0] - double(V0[0]) },
    { -dir[1],
      double(V1[1] - V0[1]), double(V2[1] - V0[1]), start[1] - double(V0[1]) },
    { -dir[2],
      double(V1[2] - V0[2]), double(V2[2] - V0[2]), start[2] - double(V0[2]) }
  };

  const double d  = determinant( array, 0, 1, 2 );
  const double dt = determinant( array, 3, 1, 2 );
 
  if (d > conf) {
    const double da = determinant( array, 0, 3, 2 );
    if (da > -conf) {
      const double db = determinant( array, 0, 1, 3 );
      res = (db > -conf && da+db <= d+conf);
    }
  } else if (d < -conf) {
    const double da = determinant( array, 0, 3, 2 );
    if (da < conf) {
      const double db = determinant( array, 0, 1, 3 );
      res = (db < conf && da+db >= d-conf);
    }
  }
  if (res && tInter)
    *tInter = dt / d;

  return res;
}

/* =====================================================================
Function   : seg_line_intersect
Purpose    : Intersect a segment with a line
Parameters : start  - coordinates of the origin of the line
             dir    - coordinates of the direction of the line (normalized)
             over2  - maximal square distance between the line and the segment
                      for intersection state
             V0     - first vertex of the triangle
             V1     - second vertex of the triangle
             tInter - output value, contains the parameter of the intersection
                      point on the line (if found). May be NULL pointer
Returns    : int = 1 if intersection found, 0 otherwise
======================================================================== */

int NIS_Triangulated::seg_line_intersect (const gp_XYZ&     aStart,
                                          const gp_XYZ&     aDir,
                                          const double      over2,
                                          const float       V0[3],
                                          const float       V1[3],
                                          double            * tInter)
{
  int res = 0;
  const gp_XYZ aDirSeg (V1[0]-V0[0], V1[1]-V0[1], V1[2]-V0[2]);
  gp_XYZ aDirN = aDirSeg ^ aDir;
  Standard_Real aMod2 = aDirN.SquareModulus();
  if (aMod2 < Precision::Confusion() * 0.001) {
    const gp_XYZ aDelta0 (V0[0]-aStart.X(), V0[1]-aStart.Y(), V0[2]-aStart.Z());
    if ((aDelta0 ^ aDir).SquareModulus() < over2) {
      res = 1;
      const gp_XYZ aDelta1 (V1[0]-aStart.X(),V1[1]-aStart.Y(),V1[2]-aStart.Z());
      if (tInter)
        * tInter = Min (aDelta0 * aDir, aDelta1 * aDir);
    }
  } else {
    // distance between two unlimited lines
    const gp_XYZ aPnt0 (V0[0], V0[1], V0[2]);
    const Standard_Real aDistL = (aDirN * aPnt0) - aDirN * aStart;
    if (aDistL*aDistL < over2 * aMod2) {
      const gp_XYZ aPnt1 (V1[0], V1[1], V1[2]);
      Standard_Real aDist[3] = {
        ((aPnt0 - aStart) ^ aDir).Modulus(),
        ((aPnt1 - aStart) ^ aDir).Modulus(),
        0.
      };
      // Find the intermediate point by interpolation using the distances from
      // the end points.
      const gp_XYZ aPntI =
        (aPnt0 * aDist[1] + aPnt1 * aDist[0]) / (aDist[0] + aDist[1]);
      aDist[2] = ((aPntI - aStart) ^ aDir).Modulus();
      if (aDist[2] < aDist[0] && aDist[2] < aDist[1]) {
        if (aDist[2]*aDist[2] < over2) {
          res = 1;
          if (tInter)
            * tInter = (aPntI - aStart) * aDir;
        }
      } else if (aDist[0] < aDist[1]) {
        if (aDist[0] * aDist[0] < over2) {
          res = 1;
          if (tInter)
            * tInter = (aPnt0 - aStart) * aDir;
        }
      } else {
        if (aDist[1] * aDist[1] < over2) {
          res = 1;
          if (tInter)
            * tInter = (aPnt1 - aStart) * aDir;
        }
      }
    }
  }
  return res;
}

//=======================================================================
//function : seg_box_intersect
//purpose  : 
//=======================================================================

int NIS_Triangulated::seg_box_intersect (const Bnd_B3f& theBox,
                                         const gp_Pnt thePnt[2])
{
  int aResult (1);
  if ((thePnt[1].XYZ() - thePnt[0].XYZ()).SquareModulus()
      < Precision::Confusion() * 0.0001)
    aResult = 0;
  else {
    const gp_Dir aDir (thePnt[1].XYZ() - thePnt[0].XYZ());
    if (theBox.IsOut (gp_Ax1(thePnt[0], aDir), Standard_True) ||
        theBox.IsOut (gp_Ax1(thePnt[1],-aDir), Standard_True))
    aResult = 0;
  }
  return aResult;
}

//=======================================================================
//function : seg_box_included
//purpose  : 
//=======================================================================

int NIS_Triangulated::seg_box_included  (const Bnd_B3f& theBox,
                                         const gp_Pnt thePnt[2])
{
  int aResult (0);
  if ((thePnt[1].XYZ() - thePnt[0].XYZ()).SquareModulus()
      > Precision::Confusion() * 0.0001)
  {
    aResult = (theBox.IsOut(thePnt[0].XYZ()) == Standard_False &&
               theBox.IsOut(thePnt[1].XYZ()) == Standard_False);
  }
  return aResult;
}

//=======================================================================
//function : allocateNodes
//purpose  : 
//=======================================================================

void NIS_Triangulated::allocateNodes (const Standard_Integer nNodes)
{
  if (nNodes > 0) {
    if (myNNodes > 0)
      myAlloc->Free(mypNodes);
    myNNodes = nNodes;
    mypNodes = static_cast<Standard_ShortReal*>
      (myAlloc->Allocate(sizeof(Standard_ShortReal)*3*nNodes));
  }
}
