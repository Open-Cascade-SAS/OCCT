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


#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <OSD_Path.hxx>
#include <OSD_OpenFile.hxx>
#include <RWStl.hxx>
#include <StlAPI_Writer.hxx>
#include <StlMesh_Mesh.hxx>
#include <StlTransfer.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <Poly_Triangulation.hxx>

StlAPI_Writer::StlAPI_Writer()
{
  theStlMesh = new StlMesh_Mesh;
  theASCIIMode = Standard_True;
}

Standard_Boolean& StlAPI_Writer::ASCIIMode() 
{
  return theASCIIMode;
}

// Auxiliary tools
namespace
{
  // Tool to get triangles from triangulation taking into account face
  // orientation and location
  class TriangleAccessor
  {
  public:
    TriangleAccessor (const TopoDS_Face& aFace)
    {
      TopLoc_Location aLoc;
      myPoly = BRep_Tool::Triangulation (aFace, aLoc);
      myTrsf = aLoc.Transformation();
      myNbTriangles = (myPoly.IsNull() ? 0 : myPoly->Triangles().Length());
      myInvert = (aFace.Orientation() == TopAbs_REVERSED);
      if (myTrsf.IsNegative())
        myInvert = ! myInvert;
    }

    int NbTriangles () const { return myNbTriangles; } 

    // get i-th triangle and outward normal
    void GetTriangle (int iTri, gp_Vec &theNormal, gp_Pnt &thePnt1, gp_Pnt &thePnt2, gp_Pnt &thePnt3)
    {
      // get positions of nodes
      int iNode1, iNode2, iNode3;
      myPoly->Triangles()(iTri).Get (iNode1, iNode2, iNode3);
      thePnt1 = myPoly->Nodes()(iNode1);
      thePnt2 = myPoly->Nodes()(myInvert ? iNode3 : iNode2);
      thePnt3 = myPoly->Nodes()(myInvert ? iNode2 : iNode3);

      // apply transormation if not identity
      if (myTrsf.Form() != gp_Identity)
      {
        thePnt1.Transform (myTrsf);
        thePnt2.Transform (myTrsf);
        thePnt3.Transform (myTrsf);
      }

      // calculate normal
      theNormal = (thePnt2.XYZ() - thePnt1.XYZ()) ^ (thePnt3.XYZ() - thePnt1.XYZ());
      Standard_Real aNorm = theNormal.Magnitude();
      if (aNorm > gp::Resolution())
        theNormal /= aNorm;
    }

  private:
    Handle(Poly_Triangulation) myPoly;
    gp_Trsf myTrsf;
    int myNbTriangles;
    bool myInvert;
  };

  // convert to float and, on big-endian platform, to little-endian representation
  inline float convertFloat (Standard_Real aValue)
  {
#ifdef OCCT_BINARY_FILE_DO_INVERSE
    return OSD_BinaryFile::InverseShortReal ((float)aValue);
#else
    return (float)aValue;
#endif
  }
}

StlAPI_ErrorStatus StlAPI_Writer::Write(const TopoDS_Shape& theShape, const Standard_CString theFileName)
{
  // open file
  FILE* aFile = OSD_OpenFile (theFileName, "wb");
  if (!aFile)
    return StlAPI_CannotOpenFile;

  // write
  if (theASCIIMode)
  {
    // header 
    Fprintf (aFile, "solid shape, STL ascii file, created with Open CASCADE Technology\n");

    // facets
    for (TopExp_Explorer exp (theShape, TopAbs_FACE); exp.More(); exp.Next())
    {
      TriangleAccessor aTool (TopoDS::Face (exp.Current()));
      for (int iTri = 1; iTri <= aTool.NbTriangles(); iTri++)
      {
        gp_Vec aNorm;
        gp_Pnt aPnt1, aPnt2, aPnt3;
        aTool.GetTriangle (iTri, aNorm, aPnt1, aPnt2, aPnt3);

        Fprintf (aFile,
          " facet normal %12e %12e %12e\n"
          "   outer loop\n"
          "     vertex %12e %12e %12e\n"
          "     vertex %12e %12e %12e\n"
          "     vertex %12e %12e %12e\n"
          "   endloop\n"
          " endfacet\n",
          aNorm.X(), aNorm.Y(), aNorm.Z(),
          aPnt1.X(), aPnt1.Y(), aPnt1.Z(),
          aPnt2.X(), aPnt2.Y(), aPnt2.Z(),
          aPnt3.X(), aPnt3.Y(), aPnt3.Z());
      }
    }

    // footer
    Fprintf (aFile, "endsolid shape\n");
  }
  else
  {
    // header block (meaningless 80 bytes)
    Fprintf (aFile, "%-80.80s", "STL binary file, created with Open CASCADE Technology");

    // number of facets
    int32_t aNbTri = 0;
    for (TopExp_Explorer exp (theShape, TopAbs_FACE); exp.More(); exp.Next())
    {
      TopLoc_Location aLoc;
      Handle(Poly_Triangulation) aPoly =
        BRep_Tool::Triangulation (TopoDS::Face (exp.Current()), aLoc);
      if (! aPoly.IsNull())
        aNbTri += aPoly->NbTriangles();
    }
    // suppose that number of triangles must be little endian...
#ifdef OCCT_BINARY_FILE_DO_INVERSE
    aNbTri = OSD_BinaryFile::InverseInteger (aNbTri);
#endif
    fwrite (&aNbTri, sizeof(int32_t), 1, aFile);

    // facets
    struct Facet {
      float nx, ny, nz;
      float x1, y1, z1;
      float x2, y2, z2;
      float x3, y3, z3;
      uint16_t dummy;
    } f;
    f.dummy = 0;
    for (TopExp_Explorer exp (theShape, TopAbs_FACE); exp.More(); exp.Next())
    {
      TriangleAccessor aTool (TopoDS::Face (exp.Current()));
      for (int iTri = 1; iTri <= aTool.NbTriangles(); iTri++)
      {
        gp_Vec aNorm;
        gp_Pnt aPnt1, aPnt2, aPnt3;
        aTool.GetTriangle (iTri, aNorm, aPnt1, aPnt2, aPnt3);

        f.nx = convertFloat (aNorm.X());
        f.ny = convertFloat (aNorm.Y());
        f.nz = convertFloat (aNorm.Z());

        f.x1 = convertFloat (aPnt1.X());
        f.y1 = convertFloat (aPnt1.Y());
        f.z1 = convertFloat (aPnt1.Z());

        f.x2 = convertFloat (aPnt2.X());
        f.y2 = convertFloat (aPnt2.Y());
        f.z2 = convertFloat (aPnt2.Z());

        f.x3 = convertFloat (aPnt3.X());
        f.y3 = convertFloat (aPnt3.Y());
        f.z3 = convertFloat (aPnt3.Z());

        fwrite (&f, 50 /* 50 bytes per facet */, 1, aFile);
      }
    }
  }

  fclose (aFile);
  return ferror(aFile) ? StlAPI_WriteError : StlAPI_StatusOK;
}

