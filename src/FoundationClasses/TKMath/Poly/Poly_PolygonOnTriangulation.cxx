// Created on: 1996-02-20
// Created by: Laurent PAINNOT
// Copyright (c) 1996-1999 Matra Datavision
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

#include <Poly_PolygonOnTriangulation.hxx>
#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Poly_PolygonOnTriangulation, Standard_Transient)

//=================================================================================================

Poly_PolygonOnTriangulation::Poly_PolygonOnTriangulation(const int theNbNodes,
                                                         const bool theHasParams)
    : myDeflection(0.0),
      myNodes(1, theNbNodes)
{
  if (theHasParams)
  {
    myParameters = new NCollection_HArray1<double>(1, theNbNodes);
  }
}

//=================================================================================================

Poly_PolygonOnTriangulation::Poly_PolygonOnTriangulation(const NCollection_Array1<int>& Nodes)
    : myDeflection(0.0),
      myNodes(1, Nodes.Length())
{
  myNodes = Nodes;
}

//=================================================================================================

Poly_PolygonOnTriangulation::Poly_PolygonOnTriangulation(const NCollection_Array1<int>& Nodes,
                                                         const NCollection_Array1<double>&    Parameters)
    : myDeflection(0.0),
      myNodes(1, Nodes.Length())
{
  myNodes                      = Nodes;
  myParameters                 = new NCollection_HArray1<double>(1, Parameters.Length());
  myParameters->ChangeArray1() = Parameters;
}

//=================================================================================================

occ::handle<Poly_PolygonOnTriangulation> Poly_PolygonOnTriangulation::Copy() const
{
  occ::handle<Poly_PolygonOnTriangulation> aCopy;
  if (myParameters.IsNull())
    aCopy = new Poly_PolygonOnTriangulation(myNodes);
  else
    aCopy = new Poly_PolygonOnTriangulation(myNodes, myParameters->Array1());
  aCopy->Deflection(myDeflection);
  return aCopy;
}

//=================================================================================================

void Poly_PolygonOnTriangulation::SetParameters(const occ::handle<NCollection_HArray1<double>>& theParameters)
{
  if (!theParameters.IsNull()
      && (theParameters->Lower() != myNodes.Lower() || theParameters->Upper() != myNodes.Upper()))
  {
    throw Standard_OutOfRange("Poly_PolygonOnTriangulation::SetParameters() - invalid array size");
  }
  myParameters = theParameters;
}

//=================================================================================================

void Poly_PolygonOnTriangulation::DumpJson(Standard_OStream& theOStream, int) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDeflection)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myNodes.Size())
  if (!myParameters.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myParameters->Size())
}
