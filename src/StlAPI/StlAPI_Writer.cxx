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
#include <Message.hxx>
#include <OSD_OpenFile.hxx>
#include <RWStl.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <Poly_Triangulation.hxx>

//=================================================================================================

StlAPI_Writer::StlAPI_Writer()
    : myASCIIMode(Standard_True)
{
  //
}

//=================================================================================================

Standard_Boolean StlAPI_Writer::Write(const TopoDS_Shape&          theShape,
                                      const Standard_CString       theFileName,
                                      const Message_ProgressRange& theProgress)
{
  Standard_Integer aNbNodes     = 0;
  Standard_Integer aNbTriangles = 0;

  // calculate total number of the nodes and triangles
  for (TopExp_Explorer anExpSF(theShape, TopAbs_FACE); anExpSF.More(); anExpSF.Next())
  {
    TopLoc_Location            aLoc;
    Handle(Poly_Triangulation) aTriangulation =
      BRep_Tool::Triangulation(TopoDS::Face(anExpSF.Current()), aLoc);
    if (!aTriangulation.IsNull())
    {
      aNbNodes += aTriangulation->NbNodes();
      aNbTriangles += aTriangulation->NbTriangles();
    }
  }

  if (aNbTriangles == 0)
  {
    // No triangulation on the shape
    return Standard_False;
  }

  // create temporary triangulation
  Handle(Poly_Triangulation) aMesh = new Poly_Triangulation(aNbNodes, aNbTriangles, Standard_False);
  // count faces missing triangulation
  Standard_Integer aNbFacesNoTri = 0;
  // fill temporary triangulation
  Standard_Integer aNodeOffset    = 0;
  Standard_Integer aTriangleOffet = 0;
  for (TopExp_Explorer anExpSF(theShape, TopAbs_FACE); anExpSF.More(); anExpSF.Next())
  {
    const TopoDS_Shape&        aFace = anExpSF.Current();
    TopLoc_Location            aLoc;
    Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation(TopoDS::Face(aFace), aLoc);
    if (aTriangulation.IsNull())
    {
      ++aNbFacesNoTri;
      continue;
    }

    // copy nodes
    gp_Trsf aTrsf = aLoc.Transformation();
    for (Standard_Integer aNodeIter = 1; aNodeIter <= aTriangulation->NbNodes(); ++aNodeIter)
    {
      gp_Pnt aPnt = aTriangulation->Node(aNodeIter);
      aPnt.Transform(aTrsf);
      aMesh->SetNode(aNodeIter + aNodeOffset, aPnt);
    }

    // copy triangles
    const TopAbs_Orientation anOrientation = anExpSF.Current().Orientation();
    for (Standard_Integer aTriIter = 1; aTriIter <= aTriangulation->NbTriangles(); ++aTriIter)
    {
      Poly_Triangle aTri = aTriangulation->Triangle(aTriIter);

      Standard_Integer anId[3];
      aTri.Get(anId[0], anId[1], anId[2]);
      if (anOrientation == TopAbs_REVERSED)
      {
        // Swap 1, 2.
        Standard_Integer aTmpIdx = anId[1];
        anId[1]                  = anId[2];
        anId[2]                  = aTmpIdx;
      }

      // Update nodes according to the offset.
      anId[0] += aNodeOffset;
      anId[1] += aNodeOffset;
      anId[2] += aNodeOffset;

      aTri.Set(anId[0], anId[1], anId[2]);
      aMesh->SetTriangle(aTriIter + aTriangleOffet, aTri);
    }

    aNodeOffset += aTriangulation->NbNodes();
    aTriangleOffet += aTriangulation->NbTriangles();
  }

  OSD_Path         aPath(theFileName);
  Standard_Boolean isDone = (myASCIIMode ? RWStl::WriteAscii(aMesh, aPath, theProgress)
                                         : RWStl::WriteBinary(aMesh, aPath, theProgress));

  if (isDone && (aNbFacesNoTri > 0))
  {
    // Print warning with number of faces missing triangulation
    TCollection_AsciiString aWarningMsg =
      TCollection_AsciiString("Warning: ") + TCollection_AsciiString(aNbFacesNoTri)
      + TCollection_AsciiString((aNbFacesNoTri == 1) ? " face has" : " faces have")
      + TCollection_AsciiString(" been skipped due to null triangulation");
    Message::SendWarning(aWarningMsg);
  }

  return isDone;
}
