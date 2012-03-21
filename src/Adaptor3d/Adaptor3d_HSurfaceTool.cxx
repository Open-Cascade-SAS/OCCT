// Created by: Laurent BUCHARD
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <Adaptor3d_HSurfaceTool.ixx>

Standard_Integer Adaptor3d_HSurfaceTool::NbSamplesU(const Handle(Adaptor3d_HSurface)& S)
{
  switch (S->GetType())
  {
    case GeomAbs_Plane: return 2;
    case GeomAbs_BezierSurface: return (3 + S->NbUPoles());
    case GeomAbs_BSplineSurface:
    {
      const Standard_Integer nbs = S->NbUKnots() * S->UDegree();
      return (nbs < 2 ? 2 : nbs);
    }
    case GeomAbs_Torus: return 20;
  }
  return 10;
}

Standard_Integer Adaptor3d_HSurfaceTool::NbSamplesV(const Handle(Adaptor3d_HSurface)& S)
{
  switch (S->GetType())
  {
    case GeomAbs_Plane: return 2;
    case GeomAbs_BezierSurface: return (3 + S->NbVPoles());
    case GeomAbs_BSplineSurface: 
    {
      const Standard_Integer nbs = S->NbVKnots() * S->VDegree();
      return (nbs < 2 ? 2 : nbs);
    }
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_SurfaceOfRevolution:
    case GeomAbs_SurfaceOfExtrusion: return 15;
  }
  return 10;
}

Standard_Integer Adaptor3d_HSurfaceTool::NbSamplesU(const Handle(Adaptor3d_HSurface)& S,
                                                    const Standard_Real u1,
                                                    const Standard_Real u2)
{
  const Standard_Integer nbs = NbSamplesU(S);
  Standard_Integer n = nbs;
  if(nbs>10)
  { 
    const Standard_Real uf = FirstUParameter(S);
    const Standard_Real ul = LastUParameter(S);
    n *= (Standard_Integer)((u2-u1)/(ul-uf));
    if (n>nbs || n>50) n = nbs;
    if (n<5)   n = 5;
  }
  return n;
}

Standard_Integer Adaptor3d_HSurfaceTool::NbSamplesV(const Handle(Adaptor3d_HSurface)& S,
                                                    const Standard_Real v1,
                                                    const Standard_Real v2)
{
  const Standard_Integer nbs = NbSamplesV(S);
  Standard_Integer n = nbs;
  if(nbs>10)
  {
    const Standard_Real vf = FirstVParameter(S);
    const Standard_Real vl = LastVParameter(S);
    n *= (Standard_Integer)((v2-v1)/(vl-vf));
    if (n>nbs || n>50) n = nbs;
    if (n<5)   n = 5;
  }
  return n;
}
