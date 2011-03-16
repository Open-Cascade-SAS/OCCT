//-- File       : HLRBRep_BSurfaceTool.cxx 
//-- Created    : Wed Jui 7 18:00:00 1993
//-- Author     : Laurent BUCHARD
//--              <lbr@nonox>
//-- Copyright:	 Matra Datavision 1993

#include <HLRBRep_BSurfaceTool.ixx>
#include <BRepAdaptor_Surface.hxx>

//=======================================================================
//function : NbSamplesU
//purpose  : 
//=======================================================================

Standard_Integer
HLRBRep_BSurfaceTool::NbSamplesU(const BRepAdaptor_Surface& S)
{ 
  Standard_Integer nbs;
  GeomAbs_SurfaceType typS = S.GetType();
  switch(typS) { 
  case GeomAbs_Plane:
    {
      nbs = 2;
    }
    break;
  case GeomAbs_BezierSurface: 
    {
      nbs =  3 + S.NbUPoles();
    }
    break;
  case GeomAbs_BSplineSurface: 
    {
      nbs = S.NbUKnots();
      nbs*= S.UDegree();
      if(nbs < 2) nbs=2;
      
    }
    break;
  case GeomAbs_Torus: 
    {
      nbs = 20;
    }
    break;
  case GeomAbs_Cylinder:
  case GeomAbs_Cone:
  case GeomAbs_Sphere:
  case GeomAbs_SurfaceOfRevolution:
  case GeomAbs_SurfaceOfExtrusion:
    {
      nbs = 10;
    }
    break;
    
  default: 
    {
      nbs = 10;
    }
    break;
  }
  return(nbs);
}

//=======================================================================
//function : NbSamplesV
//purpose  : 
//=======================================================================

Standard_Integer
HLRBRep_BSurfaceTool::NbSamplesV(const BRepAdaptor_Surface& S)
{ 
  Standard_Integer nbs;
  GeomAbs_SurfaceType typS = S.GetType();
  switch(typS) { 
  case GeomAbs_Plane:
    {
      nbs = 2;
    }
    break;
  case GeomAbs_BezierSurface: 
    {
      nbs =  3 + S.NbVPoles();
    }
    break;
  case GeomAbs_BSplineSurface: 
    {
      nbs = S.NbVKnots();
      nbs*= S.VDegree();
      if(nbs < 2) nbs=2;
      
    }
    break;
  case GeomAbs_Cylinder:
  case GeomAbs_Cone:
  case GeomAbs_Sphere:
  case GeomAbs_Torus:
  case GeomAbs_SurfaceOfRevolution:
  case GeomAbs_SurfaceOfExtrusion:
    {
      nbs = 15;
    }
    break;
    
  default: 
    {
      nbs = 10;
    }
    break;
  }
  return(nbs);
}

//=======================================================================
//function : NbSamplesU
//purpose  : 
//=======================================================================

Standard_Integer
HLRBRep_BSurfaceTool::NbSamplesU(const BRepAdaptor_Surface& S,
				 const Standard_Real u1,
				 const Standard_Real u2)
{ 
  Standard_Integer nbs = NbSamplesU(S);
  Standard_Integer n = nbs;
  if(nbs>10) { 
    Standard_Real uf = FirstUParameter(S);
    Standard_Real ul = LastUParameter(S);
    n*= (Standard_Integer)((u2-u1)/(uf-ul));
    if(n>nbs) n = nbs;
    if(n<5)   n = 5;
  }
  return(n);
}

//=======================================================================
//function : NbSamplesV
//purpose  : 
//=======================================================================

Standard_Integer
HLRBRep_BSurfaceTool::NbSamplesV(const BRepAdaptor_Surface& S,
				 const Standard_Real v1,
				 const Standard_Real v2)
{ 
  Standard_Integer nbs = NbSamplesV(S);
  Standard_Integer n = nbs;
  if(nbs>10) { 
    Standard_Real vf = FirstVParameter(S);
    Standard_Real vl = LastVParameter(S);
    n*= (Standard_Integer)((v2-v1)/(vf-vl));
    if(n>nbs) n = nbs;
    if(n<5)   n = 5;
  }
  return(n);
}

