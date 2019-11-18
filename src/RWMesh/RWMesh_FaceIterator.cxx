// Copyright (c) 2017-2019 OPEN CASCADE SAS
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

#include <RWMesh_FaceIterator.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs.hxx>

// =======================================================================
// function : RWMesh_FaceIterator
// purpose  :
// =======================================================================
RWMesh_FaceIterator::RWMesh_FaceIterator (const TDF_Label&       theLabel,
                                          const TopLoc_Location& theLocation,
                                          const Standard_Boolean theToMapColors,
                                          const XCAFPrs_Style&   theStyle)
: myDefStyle (theStyle),
  myToMapColors (theToMapColors),
  mySLTool  (1, 1e-12),
  myNodes   (NULL),
  myNormals (NULL),
  myNodeUVs (NULL),
  myHasNormals (false),
  myIsMirrored (false),
  myHasFaceColor (false)
{
  TopoDS_Shape aShape;
  if (!XCAFDoc_ShapeTool::GetShape (theLabel, aShape)
   ||  aShape.IsNull())
  {
    return;
  }

  aShape.Location (theLocation);
  myFaceIter.Init (aShape, TopAbs_FACE);

  if (theToMapColors)
  {
    dispatchStyles (theLabel, theLocation, theStyle);
  }

  Next();
}

// =======================================================================
// function : dispatchStyles
// purpose  :
// =======================================================================
void RWMesh_FaceIterator::dispatchStyles (const TDF_Label&       theLabel,
                                          const TopLoc_Location& theLocation,
                                          const XCAFPrs_Style&   theStyle)
{
  TopLoc_Location aDummyLoc;
  XCAFPrs_IndexedDataMapOfShapeStyle aStyles;
  XCAFPrs::CollectStyleSettings (theLabel, aDummyLoc, aStyles);

  Standard_Integer aNbTypes[TopAbs_SHAPE] = {};
  for (Standard_Integer aTypeIter = TopAbs_FACE; aTypeIter >= TopAbs_COMPOUND; --aTypeIter)
  {
    if (aTypeIter != TopAbs_FACE
     && aNbTypes[aTypeIter] == 0)
    {
      continue;
    }

    for (XCAFPrs_IndexedDataMapOfShapeStyle::Iterator aStyleIter (aStyles); aStyleIter.More(); aStyleIter.Next())
    {
      const TopoDS_Shape&    aKeyShape     = aStyleIter.Key();
      const TopAbs_ShapeEnum aKeyShapeType = aKeyShape.ShapeType();
      if (aTypeIter == TopAbs_FACE)
      {
        ++aNbTypes[aKeyShapeType];
      }
      if (aTypeIter != aKeyShapeType)
      {
        continue;
      }

      XCAFPrs_Style aCafStyle = aStyleIter.Value();
      if (!aCafStyle.IsSetColorCurv()
         && theStyle.IsSetColorCurv())
      {
        aCafStyle.SetColorCurv (theStyle.GetColorCurv());
      }
      if (!aCafStyle.IsSetColorSurf()
         && theStyle.IsSetColorSurf())
      {
        aCafStyle.SetColorSurf (theStyle.GetColorSurfRGBA());
      }
      if (aCafStyle.Material().IsNull()
       && !theStyle.Material().IsNull())
      {
        aCafStyle.SetMaterial (theStyle.Material());
      }

      TopoDS_Shape aKeyShapeLocated = aKeyShape.Located (theLocation);
      if (aKeyShapeType == TopAbs_FACE)
      {
        myStyles.Bind (aKeyShapeLocated, aCafStyle);
      }
      else
      {
        for (TopExp_Explorer aFaceIter (aKeyShapeLocated, TopAbs_FACE); aFaceIter.More(); aFaceIter.Next())
        {
          if (!myStyles.IsBound (aFaceIter.Current()))
          {
            myStyles.Bind (aFaceIter.Current(), aCafStyle);
          }
        }
      }
    }
  }
}

// =======================================================================
// function : normal
// purpose  :
// =======================================================================
gp_Dir RWMesh_FaceIterator::normal (Standard_Integer theNode)
{
  gp_Dir aNormal (gp::DZ());
  if (myNormals != NULL)
  {
    const Standard_Integer aNodeIndex = theNode - myNodes->Lower();
    const Graphic3d_Vec3 aNormVec3 (myNormals->Value (myNormals->Lower() + aNodeIndex * 3),
                                    myNormals->Value (myNormals->Lower() + aNodeIndex * 3 + 1),
                                    myNormals->Value (myNormals->Lower() + aNodeIndex * 3 + 2));
    if (aNormVec3.Modulus() != 0.0f)
    {
      aNormal.SetCoord (aNormVec3.x(), aNormVec3.y(), aNormVec3.z());
    }
  }
  else if (myHasNormals
        && myNodeUVs != NULL)
  {
    const gp_XY& anUV = myNodeUVs->Value (theNode).XY();
    mySLTool.SetParameters (anUV.X(), anUV.Y());
    if (mySLTool.IsNormalDefined())
    {
      aNormal = mySLTool.Normal();
    }
  }
  return aNormal;
}

// =======================================================================
// function : Next
// purpose  :
// =======================================================================
void RWMesh_FaceIterator::Next()
{
  for (; myFaceIter.More(); myFaceIter.Next())
  {
    myFace       = TopoDS::Face (myFaceIter.Current());
    myPolyTriang = BRep_Tool::Triangulation (myFace, myFaceLocation);
    myTrsf       = myFaceLocation.Transformation();
    if (myPolyTriang.IsNull()
     || myPolyTriang->Triangles().Length() == 0)
    {
      resetFace();
      continue;
    }

    initFace();
    myFaceIter.Next();
    return;
  }

  resetFace();
}

// =======================================================================
// function : initFace
// purpose  :
// =======================================================================
void RWMesh_FaceIterator::initFace()
{
  myHasNormals   = false;
  myHasFaceColor = false;
  myIsMirrored   = myTrsf.VectorialPart().Determinant() < 0.0;
  myNormals = NULL;
  myNodeUVs = NULL;

  myNodes = &myPolyTriang->Nodes();
  if (myPolyTriang->HasNormals())
  {
    myNormals = &myPolyTriang->Normals();
    myHasNormals = true;
  }
  if (myPolyTriang->HasUVNodes())
  {
    myNodeUVs = &myPolyTriang->UVNodes();
    if (!myHasNormals)
    {
      TopoDS_Face aFaceFwd = TopoDS::Face (myFace.Oriented (TopAbs_FORWARD));
      aFaceFwd.Location (TopLoc_Location());
      TopLoc_Location aLoc;
      if (!BRep_Tool::Surface (aFaceFwd, aLoc).IsNull())
      {
        myFaceAdaptor.Initialize (aFaceFwd, false);
        mySLTool.SetSurface (myFaceAdaptor);
        myHasNormals = true;
      }
    }
  }
  if (!myToMapColors)
  {
    return;
  }

  if (!myStyles.Find (myFace, myFaceStyle))
  {
    myFaceStyle = myDefStyle;
  }

  if (!myFaceStyle.Material().IsNull())
  {
    myHasFaceColor = true;
    myFaceColor = myFaceStyle.Material()->BaseColor();
  }
  else if (myFaceStyle.IsSetColorSurf())
  {
    myHasFaceColor = true;
    myFaceColor = myFaceStyle.GetColorSurfRGBA();
  }
}
