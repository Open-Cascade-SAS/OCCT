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

#include <StlAPI_Writer.hxx>

#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <OSD_Path.hxx>
#include <OSD_OpenFile.hxx>
#include <RWStl.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <Poly_Triangulation.hxx>

//=============================================================================
//function : StlAPI_Writer
//purpose  :
//=============================================================================
StlAPI_Writer::StlAPI_Writer()
: myASCIIMode (Standard_True)
{
  //
}

//=============================================================================
//function : Write
//purpose  :
//=============================================================================
Standard_Boolean StlAPI_Writer::Write (const TopoDS_Shape&    theShape,
                                       const Standard_CString theFileName)
{
  Standard_Integer aNbNodes = 0;
  Standard_Integer aNbTriangles = 0;

  // calculate total number of the nodes and triangles
  for (TopExp_Explorer anExpSF (theShape, TopAbs_FACE); anExpSF.More(); anExpSF.Next())
  {
    TopLoc_Location aLoc;
    Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation (TopoDS::Face (anExpSF.Current()), aLoc);
    if (! aTriangulation.IsNull())
    {
      aNbNodes += aTriangulation->NbNodes ();
      aNbTriangles += aTriangulation->NbTriangles ();
    }
  }

  // create temporary triangulation
  Handle(Poly_Triangulation) aMesh = new Poly_Triangulation (aNbNodes, aNbTriangles, Standard_False);

  // fill temporary triangulation
  Standard_Integer aNodeOffset = 0;
  Standard_Integer aTriangleOffet = 0;
  for (TopExp_Explorer anExpSF (theShape, TopAbs_FACE); anExpSF.More(); anExpSF.Next())
  {
    TopLoc_Location aLoc;
    Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation (TopoDS::Face (anExpSF.Current()), aLoc);

    const TColgp_Array1OfPnt& aNodes = aTriangulation->Nodes();
    const Poly_Array1OfTriangle& aTriangles = aTriangulation->Triangles();

    // copy nodes
    gp_Trsf aTrsf = aLoc.Transformation();
    for (Standard_Integer aNodeIter = aNodes.Lower(); aNodeIter <= aNodes.Upper(); ++aNodeIter)
    {
      gp_Pnt aPnt = aNodes (aNodeIter);
      aPnt.Transform (aTrsf);
      aMesh->ChangeNode (aNodeIter + aNodeOffset) = aPnt;
    }

    // copy triangles
    const TopAbs_Orientation anOrientation = anExpSF.Current().Orientation();
    for (Standard_Integer aTriIter = aTriangles.Lower(); aTriIter <= aTriangles.Upper(); ++aTriIter)
    {
      Poly_Triangle aTri = aTriangles (aTriIter);

      Standard_Integer anId[3];
      aTri.Get (anId[0], anId[1], anId[2]);
      if (anOrientation == TopAbs_REVERSED)
      {
        // Swap 1, 2.
        Standard_Integer aTmpIdx = anId[1];
        anId[1] = anId[2];
        anId[2] = aTmpIdx;
      }

      // Update nodes according to the offset.
      anId[0] += aNodeOffset;
      anId[1] += aNodeOffset;
      anId[2] += aNodeOffset;

      aTri.Set (anId[0], anId[1], anId[2]);
      aMesh->ChangeTriangle (aTriIter + aTriangleOffet) =  aTri;
    }

    aNodeOffset += aNodes.Size();
    aTriangleOffet += aTriangles.Size();
  }

  OSD_Path aPath (theFileName);
  return myASCIIMode
       ? RWStl::WriteAscii  (aMesh, aPath)
       : RWStl::WriteBinary (aMesh, aPath);
}
