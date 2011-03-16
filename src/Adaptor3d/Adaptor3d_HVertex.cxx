#include <Adaptor3d_HVertex.ixx>

#include <Precision.hxx>
#include <ElCLib.hxx>
#include <Adaptor2d_HCurve2d.hxx>


Adaptor3d_HVertex::Adaptor3d_HVertex ()
{}


Adaptor3d_HVertex::Adaptor3d_HVertex (const gp_Pnt2d& P,
				  const TopAbs_Orientation Or,
				  const Standard_Real Resolution):
       myPnt(P),myTol(Resolution),myOri(Or)
{}


gp_Pnt2d Adaptor3d_HVertex::Value ()
{
  return myPnt;
}

Standard_Real Adaptor3d_HVertex::Parameter (const Handle(Adaptor2d_HCurve2d)& C)
{
  return ElCLib::Parameter(C->Line(),myPnt);
}

Standard_Real Adaptor3d_HVertex::Resolution (const Handle(Adaptor2d_HCurve2d)&)
{
  return myTol;
}

TopAbs_Orientation Adaptor3d_HVertex::Orientation ()
{
  return myOri;
}


Standard_Boolean Adaptor3d_HVertex::IsSame(const Handle(Adaptor3d_HVertex)& Other)
{
  return (myPnt.Distance(Other->Value())<= Precision::Confusion());
}

