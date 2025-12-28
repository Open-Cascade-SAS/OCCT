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

#include <IGESData_IGESEntity.hxx>
#include <IGESSelect_SplineToBSpline.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_CopyControl.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_SplineToBSpline, IFSelect_Transformer)

IGESSelect_SplineToBSpline::IGESSelect_SplineToBSpline(const bool tryC2)
{
  thetryc2 = tryC2;
  thefound = false;
}

bool IGESSelect_SplineToBSpline::OptionTryC2() const
{
  return thetryc2;
}

bool IGESSelect_SplineToBSpline::Perform(const Interface_Graph& G,
                                         const occ::handle<Interface_Protocol>&,
                                         Interface_CheckIterator&               checks,
                                         occ::handle<Interface_InterfaceModel>& newmod)
{
  int nbe  = G.Size();
  thefound = false;
  themap.Nullify();
  for (int i = 1; i <= nbe; i++)
  {
    DeclareAndCast(IGESData_IGESEntity, ent, G.Entity(i));
    if (ent.IsNull())
      continue;
    int it = ent->TypeNumber();
    if (it == 112 || it == 126)
    {
      thefound = true;
#ifdef OCCT_DEBUG
      std::cout << "IGESSelect_SplineToBSpline : n0." << i << (it == 112 ? ", Curve" : ", Surface")
                << " to convert" << std::endl;
#endif
    }
  }
  newmod.Nullify();
  if (!thefound)
    return true;

  //  Il faudrait convertir ...
  checks.CCheck(0)->AddFail("IGESSelect_SplineToBSpline : not yet implemented");
  return false;
}

bool IGESSelect_SplineToBSpline::Updated(const occ::handle<Standard_Transient>& entfrom,
                                         occ::handle<Standard_Transient>&       entto) const
{
  if (!thefound)
  {
    entto = entfrom;
    return true;
  }
  if (themap.IsNull())
    return false;
  return themap->Search(entfrom, entto);
}

TCollection_AsciiString IGESSelect_SplineToBSpline::Label() const
{
  if (thetryc2)
    return TCollection_AsciiString(
      "Convert Spline Forms to BSpline, trying to recover C1-C2 continuity");
  else
    return TCollection_AsciiString("Convert Spline Forms to BSpline");
}
