// File:	GeomToStep_MakeConic.cxx
// Created:	Mon Jun 21 11:20:10 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>

#include <GeomToStep_MakeConic.ixx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_Conic.hxx>
#include <GeomToStep_MakeConic.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Parabola.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Parabola.hxx>
#include <GeomToStep_MakeCircle.hxx>
#include <GeomToStep_MakeEllipse.hxx>
#include <GeomToStep_MakeHyperbola.hxx>
#include <GeomToStep_MakeParabola.hxx>

//=============================================================================
// Creation d' une Conic de prostep a partir d' une Conic de Geom
//=============================================================================

GeomToStep_MakeConic::GeomToStep_MakeConic ( const Handle(Geom_Conic)& C)
{
  done = Standard_True;
  if (C->IsKind(STANDARD_TYPE(Geom_Circle))) {
    Handle(Geom_Circle) Cer = Handle(Geom_Circle)::DownCast(C);
    GeomToStep_MakeCircle MkCircle(Cer);
    theConic = MkCircle.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom_Ellipse))) {
    Handle(Geom_Ellipse) Ell = Handle(Geom_Ellipse)::DownCast(C);
    GeomToStep_MakeEllipse MkEllipse(Ell);
    theConic = MkEllipse.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom_Hyperbola))) {
    Handle(Geom_Hyperbola) Hyp = Handle(Geom_Hyperbola)::DownCast(C);
    GeomToStep_MakeHyperbola MkHyperbola(Hyp);
    theConic = MkHyperbola.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom_Parabola))) {
    Handle(Geom_Parabola) Par = Handle(Geom_Parabola)::DownCast(C);
    GeomToStep_MakeParabola MkParabola(Par);
    theConic = MkParabola.Value();
  }
  else {
#ifdef DEBUG
    cout << "3D Curve Type   : " << C->DynamicType() << endl;
#endif
    done = Standard_False;
  }
}	 

//=============================================================================
// Creation d' une Conic2d de prostep a partir d' une Conic de Geom2d
//=============================================================================

GeomToStep_MakeConic::GeomToStep_MakeConic ( const Handle(Geom2d_Conic)& C)
{
  done = Standard_True;
  if (C->IsKind(STANDARD_TYPE(Geom2d_Circle))) {
    Handle(Geom2d_Circle) Cer = Handle(Geom2d_Circle)::DownCast(C);
    GeomToStep_MakeCircle MkCircle(Cer);
    theConic = MkCircle.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom2d_Ellipse))) {
    Handle(Geom2d_Ellipse) Ell = Handle(Geom2d_Ellipse)::DownCast(C);
    GeomToStep_MakeEllipse MkEllipse(Ell);
    theConic = MkEllipse.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom2d_Hyperbola))) {
    Handle(Geom2d_Hyperbola) Hyp = Handle(Geom2d_Hyperbola)::DownCast(C);
    GeomToStep_MakeHyperbola MkHyperbola(Hyp);
    theConic = MkHyperbola.Value();
  }
  else if (C->IsKind(STANDARD_TYPE(Geom2d_Parabola))) {
    Handle(Geom2d_Parabola) Par = Handle(Geom2d_Parabola)::DownCast(C);
    GeomToStep_MakeParabola MkParabola(Par);
    theConic = MkParabola.Value();
  }
  else {
    // Attention : Other 2d conics shall be implemented ...
    //             To be performed later !
#ifdef DEBUG
    cout << "2D conic not yet implemented" << endl;
#endif
    done = Standard_False;
  }
}	 

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Conic) &
      GeomToStep_MakeConic::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theConic;
}
