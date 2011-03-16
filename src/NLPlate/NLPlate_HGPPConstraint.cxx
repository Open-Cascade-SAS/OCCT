// File:	NLPlate_HGPPConstraint.cxx
// Created:	Fri Apr 10 17:05:49 1998
// Author:	Andre LIEUTIER
//		<alr@sgi63>


#include <NLPlate_HGPPConstraint.ixx>


 void NLPlate_HGPPConstraint::SetUVFreeSliding(const Standard_Boolean UVFree) 
{
}

 void NLPlate_HGPPConstraint::SetIncrementalLoadAllowed(const Standard_Boolean ILA) 
{
}

 void NLPlate_HGPPConstraint::SetActiveOrder(const Standard_Integer ActiveOrder) 
{
  myActiveOrder = ActiveOrder;
}

 void NLPlate_HGPPConstraint::SetUV(const gp_XY& UV) 
{
  myUV = UV;
}
 void NLPlate_HGPPConstraint::SetOrientation(const Standard_Integer Orient) 
{
}

 void NLPlate_HGPPConstraint::SetG0Criterion(const Standard_Real TolDist) 
{
}

 void NLPlate_HGPPConstraint::SetG1Criterion(const Standard_Real TolAng) 
{
}

 void NLPlate_HGPPConstraint::SetG2Criterion(const Standard_Real TolCurv) 
{
}

 void NLPlate_HGPPConstraint::SetG3Criterion(const Standard_Real TolG3) 
{
}

 Standard_Boolean NLPlate_HGPPConstraint::UVFreeSliding() const
{
  return Standard_False;
}

 Standard_Boolean NLPlate_HGPPConstraint::IncrementalLoadAllowed() const
{
  return Standard_False;
}

const gp_XY& NLPlate_HGPPConstraint::UV() const
{
  return myUV;
}

static const gp_XYZ XYZnull(0.,0.,0.);
const gp_XYZ& NLPlate_HGPPConstraint::G0Target() const
{
  return XYZnull;
  //cette methode ne devrait pas etre appelee
}

static const Plate_D1 D1null(XYZnull,XYZnull);
const Plate_D1& NLPlate_HGPPConstraint::G1Target() const
{
  return D1null;
  //cette methode ne devrait pas etre appelee
}

static const Plate_D2 D2null(XYZnull,XYZnull,XYZnull);
const Plate_D2& NLPlate_HGPPConstraint::G2Target() const
{
  return D2null;
  //cette methode ne devrait pas etre appelee
}

static const Plate_D3 D3null(XYZnull,XYZnull,XYZnull,XYZnull);
const Plate_D3& NLPlate_HGPPConstraint::G3Target() const
{
  return D3null;
  //cette methode ne devrait pas etre applee
}

 Standard_Integer NLPlate_HGPPConstraint::Orientation() 
{
  return 0;
}
 Standard_Real NLPlate_HGPPConstraint::G0Criterion() const
{
  return 0.;
}

 Standard_Real NLPlate_HGPPConstraint::G1Criterion() const
{
  return 0.;
}

 Standard_Real NLPlate_HGPPConstraint::G2Criterion() const
{
  return 0.;
}

 Standard_Real NLPlate_HGPPConstraint::G3Criterion() const
{
  return 0.;
}


