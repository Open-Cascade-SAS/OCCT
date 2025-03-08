// Created on: 1995-03-07
// Created by: Laurent PAINNOT
// Copyright (c) 1995-1999 Matra Datavision
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

#include <Poly_Polygon3D.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Poly_Polygon3D, Standard_Transient)

//=================================================================================================

Poly_Polygon3D::Poly_Polygon3D(const Standard_Integer theNbNodes,
                               const Standard_Boolean theHasParams)
    : myDeflection(0.0),
      myNodes(1, theNbNodes)
{
  if (theHasParams)
  {
    myParameters = new TColStd_HArray1OfReal(1, theNbNodes);
  }
}

//=================================================================================================

Poly_Polygon3D::Poly_Polygon3D(const TColgp_Array1OfPnt& Nodes)
    : myDeflection(0.),
      myNodes(1, Nodes.Length())
{
  Standard_Integer i, j = 1;
  for (i = Nodes.Lower(); i <= Nodes.Upper(); i++)
    myNodes(j++) = Nodes(i);
}

//=================================================================================================

Poly_Polygon3D::Poly_Polygon3D(const TColgp_Array1OfPnt& Nodes, const TColStd_Array1OfReal& P)
    : myDeflection(0.),
      myNodes(1, Nodes.Length())

{
  myParameters = new TColStd_HArray1OfReal(1, P.Length());
  Standard_Integer i, j = 1;
  for (i = Nodes.Lower(); i <= Nodes.Upper(); i++)
  {
    myNodes(j) = Nodes(i);
    myParameters->SetValue(j, P(i));
    j++;
  }
}

//=================================================================================================

Handle(Poly_Polygon3D) Poly_Polygon3D::Copy() const
{
  Handle(Poly_Polygon3D) aCopy;
  if (myParameters.IsNull())
    aCopy = new Poly_Polygon3D(myNodes);
  else
    aCopy = new Poly_Polygon3D(myNodes, myParameters->Array1());
  aCopy->Deflection(myDeflection);
  return aCopy;
}

//=================================================================================================

void Poly_Polygon3D::DumpJson(Standard_OStream& theOStream, Standard_Integer) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDeflection)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myNodes.Size())
  if (!myParameters.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myParameters->Size())
}
