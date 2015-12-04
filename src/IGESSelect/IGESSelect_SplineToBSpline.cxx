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
#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_CopyControl.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_SplineToBSpline,IFSelect_Transformer)

IGESSelect_SplineToBSpline::IGESSelect_SplineToBSpline
  (const Standard_Boolean tryC2)
{
  thetryc2 = tryC2;  thefound = Standard_False;
}

Standard_Boolean  IGESSelect_SplineToBSpline::OptionTryC2 () const
{
  return thetryc2;
}


Standard_Boolean  IGESSelect_SplineToBSpline::Perform
  (const Interface_Graph& G, const Handle(Interface_Protocol)&,
   Interface_CheckIterator& checks,
   Handle(Interface_InterfaceModel)& newmod)
{
  Standard_Integer nbe = G.Size();
  thefound = Standard_False;
  themap.Nullify();
  for (Standard_Integer i = 1; i <= nbe; i ++) {
    DeclareAndCast(IGESData_IGESEntity,ent,G.Entity(i));
    if (ent.IsNull()) continue;
    Standard_Integer it = ent->TypeNumber();
    if (it == 112 || it == 126) {
      thefound = Standard_True;
#ifdef OCCT_DEBUG
      cout<<"IGESSelect_SplineToBSpline : n0."<<i
	<< (it == 112 ? ", Curve" : ", Surface")<<" to convert"<<endl;
#endif
    }
  }
  newmod.Nullify();
  if (!thefound) return Standard_True;

//  Il faudrait convertir ...
  checks.CCheck(0)->AddFail("IGESSelect_SplineToBSpline : not yet implemented");
  return Standard_False;
}


Standard_Boolean  IGESSelect_SplineToBSpline::Updated
  (const Handle(Standard_Transient)& entfrom,
   Handle(Standard_Transient)& entto) const
{
  if (!thefound) {
    entto = entfrom;
    return Standard_True;
  }
  if (themap.IsNull()) return Standard_False;
  return themap->Search(entfrom,entto);
}


TCollection_AsciiString  IGESSelect_SplineToBSpline::Label () const
{
  if (thetryc2) return TCollection_AsciiString
    ("Convert Spline Forms to BSpline, trying to recover C1-C2 continuity");
  else return TCollection_AsciiString ("Convert Spline Forms to BSpline");
}
