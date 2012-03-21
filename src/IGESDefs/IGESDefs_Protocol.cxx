// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <IGESDefs_Protocol.ixx>

#include <IGESDefs_AssociativityDef.hxx>
#include <IGESDefs_MacroDef.hxx>
#include <IGESDefs_UnitsData.hxx>
#include <IGESDefs_AttributeDef.hxx>
#include <IGESDefs_TabularData.hxx>
#include <IGESDefs_GenericData.hxx>
#include <IGESDefs_AttributeTable.hxx>

#include  <IGESGraph.hxx>
#include  <IGESGraph_Protocol.hxx>

static int deja = 0;
static Handle(Standard_Type) atype1,atype2,atype3,atype4,atype5,atype6,atype7;

    IGESDefs_Protocol::IGESDefs_Protocol ()
{
  if (deja) return;  deja = 1;
  atype1 = STANDARD_TYPE(IGESDefs_AssociativityDef);
  atype2 = STANDARD_TYPE(IGESDefs_AttributeDef);
  atype3 = STANDARD_TYPE(IGESDefs_AttributeTable);
  atype4 = STANDARD_TYPE(IGESDefs_GenericData);
  atype5 = STANDARD_TYPE(IGESDefs_MacroDef);
  atype6 = STANDARD_TYPE(IGESDefs_TabularData);
  atype7 = STANDARD_TYPE(IGESDefs_UnitsData);
}

    Standard_Integer IGESDefs_Protocol::NbResources () const
      {  return 1;  }

    Handle(Interface_Protocol) IGESDefs_Protocol::Resource
  (const Standard_Integer num) const
{
  Handle(Interface_Protocol) res = IGESGraph::Protocol();
  return res;
}

    Standard_Integer IGESDefs_Protocol::TypeNumber
  (const Handle(Standard_Type)& atype) const
{   
  if      (atype == atype1) return 1;
  else if (atype == atype2) return 2;
  else if (atype == atype3) return 3;
  else if (atype == atype4) return 4;
  else if (atype == atype5) return 5;
  else if (atype == atype6) return 6;
  else if (atype == atype7) return 7;
  return 0;
}
