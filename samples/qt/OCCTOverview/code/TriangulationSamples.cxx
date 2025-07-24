// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#include "TriangulationSamples.h"

#include "MakeBottle.h"

#include <AIS_Shape.hxx>
#include <AIS_Triangulation.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Poly_Triangulation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

void TriangulationSamples::ExecuteSample (const TCollection_AsciiString& theSampleName)
{
  Standard_Boolean anIsSamplePresent = Standard_True;
  FindSourceCode(theSampleName);
  if (theSampleName == "Triangulation3dSample")
  {
    Triangulation3dSample();
  }
  else
  {
    myResult << "No function found: " << theSampleName;
    myCode += TCollection_AsciiString("No function found: ") + theSampleName;
    anIsSamplePresent = Standard_False;
  }
  myIsProcessed = anIsSamplePresent;
}

void TriangulationSamples::Triangulation3dSample()
{
  TopoDS_Shape aBottle = MakeBottle(50, 70, 30);
  BRepMesh_IncrementalMesh(aBottle, 1);

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);

  Standard_Integer aNbTriangles(0);
  for (TopExp_Explorer anExplorer(aBottle, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    TopoDS_Face aFace = TopoDS::Face(anExplorer.Current());
    TopLoc_Location aLocation;
    Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation(aFace, aLocation);

    for (Standard_Integer i = 1; i <= aTriangulation->NbTriangles(); i++)
    {
      const Poly_Triangle trian = aTriangulation->Triangle (i);
      Standard_Integer index1, index2, index3, M = 0, N = 0;
      trian.Get(index1, index2, index3);

      for (Standard_Integer j = 1; j <= 3; j++)
      {
        switch (j)
        {
          case 1:
            M = index1;
            N = index2;
            break;
          case 2:
            N = index3;
            break;
          case 3:
            M = index2;
        }

        BRepBuilderAPI_MakeEdge anEdgeMaker(aTriangulation->Node (M), aTriangulation->Node (N));
        if (anEdgeMaker.IsDone())
        {
          aBuilder.Add(aCompound, anEdgeMaker.Edge());
        }
      }
    }
    Handle(AIS_Triangulation) anAisTriangulation = new AIS_Triangulation(aTriangulation);
    aNbTriangles += aTriangulation->NbTriangles();
    myObject3d.Append(anAisTriangulation);
  }

  Handle(AIS_Shape)  anAisCompound = new AIS_Shape(aCompound);
  myObject3d.Append(anAisCompound);

  Handle(AIS_Shape) AISBottle = new AIS_Shape(aBottle);
  myObject3d.Append(AISBottle);

  myResult << "Compute the triangulation on a shape: " << aNbTriangles;
}
