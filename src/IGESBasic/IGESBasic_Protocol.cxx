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

#include <IGESBasic_Protocol.ixx>

#include <IGESBasic_AssocGroupType.hxx>
#include <IGESBasic_ExternalRefFile.hxx>
#include <IGESBasic_ExternalRefFileIndex.hxx>
#include <IGESBasic_ExternalRefFileName.hxx>
#include <IGESBasic_ExternalRefLibName.hxx>
#include <IGESBasic_ExternalRefName.hxx>
#include <IGESBasic_ExternalReferenceFile.hxx>
#include <IGESBasic_Group.hxx>
#include <IGESBasic_GroupWithoutBackP.hxx>
#include <IGESBasic_Hierarchy.hxx>
#include <IGESBasic_Name.hxx>
#include <IGESBasic_OrderedGroup.hxx>
#include <IGESBasic_OrderedGroupWithoutBackP.hxx>
#include <IGESBasic_SingleParent.hxx>
#include <IGESBasic_SingularSubfigure.hxx>
#include <IGESBasic_SubfigureDef.hxx>

#include <IGESData.hxx>
#include <IGESData_Protocol.hxx>

static int deja = 0;
static Handle(Standard_Type) atype01,atype02,atype03,atype04,atype05,atype06,
  atype07,atype08,atype09,atype10,atype11,atype12,atype13,atype14,atype15,atype16;

    IGESBasic_Protocol::IGESBasic_Protocol ()
{
  if (deja) return;  deja = 1;
  atype01 = STANDARD_TYPE(IGESBasic_AssocGroupType);
  atype02 = STANDARD_TYPE(IGESBasic_ExternalRefFile);
  atype03 = STANDARD_TYPE(IGESBasic_ExternalRefFileIndex);
  atype04 = STANDARD_TYPE(IGESBasic_ExternalRefFileName);
  atype05 = STANDARD_TYPE(IGESBasic_ExternalRefLibName);
  atype06 = STANDARD_TYPE(IGESBasic_ExternalRefName);
  atype07 = STANDARD_TYPE(IGESBasic_ExternalReferenceFile);
  atype08 = STANDARD_TYPE(IGESBasic_Group);
  atype09 = STANDARD_TYPE(IGESBasic_GroupWithoutBackP);
  atype10 = STANDARD_TYPE(IGESBasic_Hierarchy);
  atype11 = STANDARD_TYPE(IGESBasic_Name);
  atype12 = STANDARD_TYPE(IGESBasic_OrderedGroup);
  atype13 = STANDARD_TYPE(IGESBasic_OrderedGroupWithoutBackP);
  atype14 = STANDARD_TYPE(IGESBasic_SingleParent);
  atype15 = STANDARD_TYPE(IGESBasic_SingularSubfigure);
  atype16 = STANDARD_TYPE(IGESBasic_SubfigureDef);
}

    Standard_Integer IGESBasic_Protocol::NbResources () const
      {  return 1;  }

    Handle(Interface_Protocol) IGESBasic_Protocol::Resource
  (const Standard_Integer num) const
{
  Handle(Interface_Protocol) res = IGESData::Protocol();
  return res;
}

    Standard_Integer IGESBasic_Protocol::TypeNumber
  (const Handle(Standard_Type)& atype) const
{
  if      (atype == atype01) return  1;
  else if (atype == atype02) return  2;
  else if (atype == atype03) return  3;
  else if (atype == atype04) return  4;
  else if (atype == atype05) return  5;
  else if (atype == atype06) return  6;
  else if (atype == atype07) return  7;
  else if (atype == atype08) return  8;
  else if (atype == atype09) return  9;
  else if (atype == atype10) return 10;
  else if (atype == atype11) return 11;
  else if (atype == atype12) return 12;
  else if (atype == atype13) return 13;
  else if (atype == atype14) return 14;
  else if (atype == atype15) return 15;
  else if (atype == atype16) return 16;
  return 0;
}
