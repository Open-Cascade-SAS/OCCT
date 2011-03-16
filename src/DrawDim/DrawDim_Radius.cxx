// File:	DrawDim_Radius.cxx
// Created:	Mon Apr 21 14:40:47 1997
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>


#include <DrawDim_Radius.ixx>
#include <BRepAdaptor_Surface.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Circ.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <GC_MakeCircle.hxx>
#include <TopoDS.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Circle.hxx>

//=======================================================================
//function : DrawDim_Radius
//purpose  : 
//=======================================================================

DrawDim_Radius::DrawDim_Radius(const TopoDS_Face& cylinder)
{
  myCylinder = cylinder;
}

//=======================================================================
//function : Cylinder
//purpose  : 
//=======================================================================

const TopoDS_Face& DrawDim_Radius::Cylinder() const
{
  return myCylinder;
}

//=======================================================================
//function : Cylinder
//purpose  : 
//=======================================================================

void DrawDim_Radius::Cylinder(const TopoDS_Face& face) 
{
  myCylinder = face;
}


//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DrawDim_Radius::DrawOn(Draw_Display& dis) const
{
  // input  
  TopoDS_Shape myFShape = myCylinder;

  // output
  gp_Pnt myPosition;
  gp_Circ myCircle;

//=======================================================================
//function : ComputeOneFaceRadius
//purpose  : 
//=======================================================================

//void AIS_RadiusDimension::ComputeOneFaceRadius(const Handle(Prs3d_Presentation)& aPresentation)
//{

  cout << "entree dans computeonefaceradius"<< endl;
  BRepAdaptor_Surface surfAlgo (TopoDS::Face(myFShape));
  Standard_Real uFirst, uLast, vFirst, vLast;
  uFirst = surfAlgo.FirstUParameter();
  uLast = surfAlgo.LastUParameter();
  vFirst = surfAlgo.FirstVParameter();
  vLast = surfAlgo.LastVParameter();
  Standard_Real uMoy = (uFirst + uLast)/2;
  Standard_Real vMoy = (vFirst + vLast)/2;
  gp_Pnt curpos ;
  surfAlgo.D0(uMoy, vMoy, curpos);
  const Handle(Geom_Surface)& surf = surfAlgo.Surface().Surface();
  Handle(Geom_Curve) aCurve;
  if (surf->DynamicType() == STANDARD_TYPE(Geom_ToroidalSurface)) {
    aCurve = surf->UIso(uMoy);
    uFirst = vFirst;
    uLast = vLast;
  }
  else {
    aCurve = surf->VIso(vMoy);
  }

  if (aCurve->DynamicType() == STANDARD_TYPE(Geom_Circle)) {
    myCircle = Handle(Geom_Circle)::DownCast(aCurve)->Circ();
  } // if (aCurve->DynamicType() ...

  else {
    // compute a circle from 3 points on "aCurve"
    gp_Pnt P1, P2;
    surfAlgo.D0(uFirst, vMoy, P1);
    surfAlgo.D0(uLast, vMoy, P2);
    GC_MakeCircle mkCirc(P1, curpos, P2);
    myCircle = mkCirc.Value()->Circ();
  } // else ...

  myPosition = curpos;

  // DISPLAY
  // Add(myText, curpos, mCircle, uFirst, uLast)    

  dis.Draw(myCircle,uFirst,uLast);  
  dis.DrawMarker(myPosition, Draw_Losange);
}
