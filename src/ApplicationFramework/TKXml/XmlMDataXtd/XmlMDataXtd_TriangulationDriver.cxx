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

#include <XmlMDataXtd_TriangulationDriver.hxx>
#include <Message_Messenger.hxx>
#include <NCollection_LocalArray.hxx>
#include <Standard_Type.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>
#include <TDataXtd_Triangulation.hxx>
#include <LDOM_OSStream.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataXtd_TriangulationDriver, XmlMDF_ADriver)
IMPLEMENT_DOMSTRING(TriangString, "triangulation")
IMPLEMENT_DOMSTRING(NullString, "null")
IMPLEMENT_DOMSTRING(ExistString, "exists")

//=================================================================================================

XmlMDataXtd_TriangulationDriver::XmlMDataXtd_TriangulationDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, NULL)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMDataXtd_TriangulationDriver::NewEmpty() const
{
  return new TDataXtd_Triangulation();
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool XmlMDataXtd_TriangulationDriver::Paste(const XmlObjMgt_Persistent&       theSource,
                                            const occ::handle<TDF_Attribute>& theTarget,
                                            XmlObjMgt_RRelocationTable&) const
{
  const XmlObjMgt_Element&            element   = theSource;
  occ::handle<TDataXtd_Triangulation> attribute = occ::down_cast<TDataXtd_Triangulation>(theTarget);

  // Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString triangStatus = element.getAttribute(::TriangString());
  if (triangStatus == NULL || triangStatus.Type() != LDOMBasicString::LDOM_AsciiDoc
      || strcmp(triangStatus.GetString(), ::ExistString().GetString()))
  {
    // No triangulation.
    return true;
  }

  // Get mesh as a string.
  const XmlObjMgt_DOMString& data = XmlObjMgt::GetStringValue(element);
  std::stringstream          stream(std::string(data.GetString()));

  int    i, n1, n2, n3;
  int    nbNodes, nbTriangles, hasUV;
  double deflection, x, y, z;

  stream >> nbNodes >> nbTriangles >> hasUV;
  GetReal(stream, deflection);

  NCollection_Array1<gp_Pnt>   Nodes(1, nbNodes);
  NCollection_Array1<gp_Pnt2d> UVNodes(1, nbNodes);

  for (i = 1; i <= nbNodes; i++)
  {
    GetReal(stream, x);
    GetReal(stream, y);
    GetReal(stream, z);
    Nodes(i).SetCoord(x, y, z);
  }

  if (hasUV)
  {
    for (i = 1; i <= nbNodes; i++)
    {
      GetReal(stream, x);
      GetReal(stream, y);
      UVNodes(i).SetCoord(x, y);
    }
  }

  // read the triangles
  NCollection_Array1<Poly_Triangle> Triangles(1, nbTriangles);
  for (i = 1; i <= nbTriangles; i++)
  {
    stream >> n1 >> n2 >> n3;
    Triangles(i).Set(n1, n2, n3);
  }

  occ::handle<Poly_Triangulation> PT;
  if (hasUV)
    PT = new Poly_Triangulation(Nodes, UVNodes, Triangles);
  else
    PT = new Poly_Triangulation(Nodes, Triangles);
  PT->Deflection(deflection);

  attribute->Set(PT);

  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataXtd_TriangulationDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                            XmlObjMgt_Persistent&             theTarget,
                                            XmlObjMgt_SRelocationTable&) const
{
  const occ::handle<TDataXtd_Triangulation> attribute =
    occ::down_cast<TDataXtd_Triangulation>(theSource);
  if (attribute->Get().IsNull())
    theTarget.Element().setAttribute(::TriangString(), ::NullString());
  else
  {
    theTarget.Element().setAttribute(::TriangString(), ::ExistString());

    int i, n1, n2, n3;

    // Analyse the size of the triangulation
    // (to allocate properly the string array).
    const occ::handle<Poly_Triangulation>& PT          = attribute->Get();
    int                                    nbNodes     = PT->NbNodes();
    int                                    nbTriangles = PT->NbTriangles();
    int                                    size        = PT->NbNodes();
    // clang-format off
    size *= 3 * 25; // 3 coordinates for a node * 25 characters are used to represent a coordinate (double) in XML
    if (PT->HasUVNodes()) 
      size += 2 * 25 * nbNodes; // 2 coordinates for a 2D node * 25 characters are used to represent a coordinate (double) in XML
    // clang-format on
    size += 3 * 10 * nbTriangles; // space for triangles
    size *= 2;                    // just in case :-)
    if (!size)
      size = 1;

    // Allocate a string stream.
    LDOM_OSStream stream(size);
    stream.precision(17);

    stream << nbNodes << " " << nbTriangles << " ";
    stream << ((PT->HasUVNodes()) ? "1" : "0") << " ";

    stream << PT->Deflection() << "\n";

    // write the 3d nodes
    for (i = 1; i <= nbNodes; i++)
    {
      const gp_Pnt aNode = PT->Node(i);
      stream << aNode.X() << " " << aNode.Y() << " " << aNode.Z() << " ";
    }

    if (PT->HasUVNodes())
    {
      for (i = 1; i <= nbNodes; i++)
      {
        const gp_Pnt2d aNode2d = PT->UVNode(i);
        stream << aNode2d.X() << " " << aNode2d.Y() << " ";
      }
    }

    for (i = 1; i <= nbTriangles; i++)
    {
      PT->Triangle(i).Get(n1, n2, n3);
      stream << n1 << " " << n2 << " " << n3 << " ";
    }

    stream << std::ends;

    // clang-format off
    char* dump = (char*)stream.str(); // copying! Don't forget to delete it.
    // clang-format on
    XmlObjMgt::SetStringValue(theTarget, dump, true);
    delete[] dump;
  }
}

//=================================================================================================

void XmlMDataXtd_TriangulationDriver::GetReal(Standard_IStream& IS, double& theValue) const
{
  theValue = 0.;
  if (IS.eof())
    return;

  char buffer[256];
  buffer[0]                 = '\0';
  std::streamsize anOldWide = IS.width(256);
  IS >> buffer;
  IS.width(anOldWide);
  theValue = Strtod(buffer, NULL);
}
