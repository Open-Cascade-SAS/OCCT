// Created: 2009-02-02
// 
// Copyright (c) 2009-2013 OPEN CASCADE SAS
// 
// This file is part of commercial software by OPEN CASCADE SAS, 
// furnished in accordance with the terms and conditions of the contract 
// and with the inclusion of this copyright notice. 
// This file or any part thereof may not be provided or otherwise 
// made available to any third party. 
// 
// No ownership title to the software is transferred hereby. 
// 
// OPEN CASCADE SAS makes no representation or warranties with respect to the 
// performance of this software, and specifically disclaims any responsibility 
// for any damages, special or consequential, connected with its use. 

#include <Geom2dConvert_PPoint.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <Precision.hxx>

//=======================================================================
//function : Geom2dConvert_PPoint
//purpose  : Constructor
//=======================================================================

Geom2dConvert_PPoint::Geom2dConvert_PPoint (const Standard_Real      theParameter,
                                            const Adaptor2d_Curve2d& theAdaptor)
  : myParameter (theParameter)
{
  theAdaptor.D1(theParameter, myPoint, myD1);
}

//=======================================================================
//function : Geom2dConvert_PPoint::operator ==
//purpose  : Compare two values of this type.
//=======================================================================

Standard_Boolean Geom2dConvert_PPoint::operator ==
                (const Geom2dConvert_PPoint& theOther) const
{
  return (fabs(myParameter - theOther.Parameter()) <= Precision::PConfusion());
}

//=======================================================================
//function : Geom2dConvert_PPoint::operator !=
//purpose  : Compare two values of this type.
//=======================================================================

Standard_Boolean Geom2dConvert_PPoint::operator !=
                (const Geom2dConvert_PPoint& theOther) const
{
  return (fabs(myParameter - theOther.Parameter()) > Precision::PConfusion());
}
