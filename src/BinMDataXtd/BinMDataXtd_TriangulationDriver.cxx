// Created on: 2016-11-10
// Created by: Anton KOZULIN
// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <BinMDataXtd_TriangulationDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDataXtd_Triangulation.hxx>
#include <TDF_Attribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataXtd_TriangulationDriver,BinMDF_ADriver)

//=======================================================================
//function : BinMDataXtd_TriangulationDriver
//purpose  : Constructor
//=======================================================================
BinMDataXtd_TriangulationDriver::BinMDataXtd_TriangulationDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
  : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataXtd_Triangulation)->Name())
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataXtd_TriangulationDriver::NewEmpty() const
{
  return new TDataXtd_Triangulation();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean BinMDataXtd_TriangulationDriver::Paste(const BinObjMgt_Persistent&  theSource,
                                                        const Handle(TDF_Attribute)& theTarget,
                                                        BinObjMgt_RRelocationTable&  ) const
{
  Handle(TDataXtd_Triangulation) attrubute = Handle(TDataXtd_Triangulation)::DownCast(theTarget);

  Standard_Integer i;
  Standard_Real deflection, x, y, z;
  Standard_Integer n1, n2, n3;
  Standard_Integer nbNodes(0), nbTriangles(0);
  Standard_Boolean hasUV(Standard_False);
  gp_Pnt p;

  theSource >> nbNodes;
  theSource >> nbTriangles;
  theSource >> hasUV;
  theSource >> deflection;

  // allocate the mesh
  Handle(Poly_Triangulation) PT = new Poly_Triangulation(nbNodes, nbTriangles, hasUV);

  // deflection
  PT->Deflection(deflection);

  // read nodes
  for (i = 1; i <= nbNodes; i++)
  {
    theSource >> x;
    theSource >> y;
    theSource >> z;
    PT->ChangeNode(i).SetCoord(x, y, z);
  }

  // read 2d nodes
  if (hasUV)
  {
    for (i = 1; i <= nbNodes; i++)
    {
      theSource >> x;
      theSource >> y;
      PT->ChangeUVNode(i).SetCoord(x,y);
    }
  }

  // read triangles
  for (i = 1; i <= nbTriangles; i++)
  {
    theSource >> n1;
    theSource >> n2;
    theSource >> n3;
    PT->ChangeTriangle(i).Set(n1, n2, n3);
  }

  // set triangulation to Ocaf attribute
  attrubute->Set(PT);
  return !PT.IsNull();
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataXtd_TriangulationDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                            BinObjMgt_Persistent&        theTarget,
                                            BinObjMgt_SRelocationTable&  ) const
{
  const Handle(TDataXtd_Triangulation) attribute = Handle(TDataXtd_Triangulation)::DownCast(theSource);
  const Handle(Poly_Triangulation)& PT = attribute->Get();
  if (!PT.IsNull())
  {
    Standard_Integer nbNodes = PT->NbNodes();
    Standard_Integer nbTriangles = PT->NbTriangles();
    Standard_Integer n1, n2, n3;

    // write number of elements
    theTarget << nbNodes;
    theTarget << nbTriangles;
    theTarget << (PT->HasUVNodes() ? 1 : 0);
    // write the deflection
    theTarget << PT->Deflection();

    // write 3d nodes
    Standard_Integer i;
    for (i = 1; i <= nbNodes; i++)
    {
      theTarget << PT->Node(i).X();
      theTarget << PT->Node(i).Y();
      theTarget << PT->Node(i).Z();
    }

    // write 2d nodes
    if (PT->HasUVNodes())
    {
      for (i = 1; i <= nbNodes; i++)
      {
        theTarget << PT->UVNode(i).X();
        theTarget << PT->UVNode(i).Y();
      }
    }

    // Write triangles
    const Poly_Array1OfTriangle& Triangles = PT->Triangles();
    for (i = 1; i <= nbTriangles; i++)
    {
      Triangles(i).Get(n1, n2, n3);
      theTarget << n1;
      theTarget << n2;
      theTarget << n3;
    }
  }
}
