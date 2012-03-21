// Created on: 2001-08-24
// Created by: Alexnder GRIGORIEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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

// modified     13.04.2009 Sergey Zaritchny

#include <BinMDataXtd_ConstraintDriver.ixx>

#include <TDataXtd_Constraint.hxx>
#include <TDataStd_Real.hxx>
#include <TNaming_NamedShape.hxx>

//=======================================================================
//function : BinMDataXtd_ConstraintDriver
//purpose  : Constructor
//=======================================================================
BinMDataXtd_ConstraintDriver::BinMDataXtd_ConstraintDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : BinMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataXtd_ConstraintDriver::NewEmpty() const
{
  return (new TDataXtd_Constraint());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataXtd_ConstraintDriver::Paste
                        (const BinObjMgt_Persistent&  theSource,
                         const Handle(TDF_Attribute)& theTarget,
                         BinObjMgt_RRelocationTable&  theRelocTable) const
{
  Handle(TDataXtd_Constraint) aC = 
    Handle(TDataXtd_Constraint)::DownCast(theTarget);

  Standard_Integer aNb;

  // value
  if (! (theSource >> aNb))
    return Standard_False;
  if (aNb > 0)
  {
    Handle(TDataStd_Real) aTValue;
    if (theRelocTable.IsBound(aNb))
      aTValue = Handle(TDataStd_Real)::DownCast(theRelocTable.Find(aNb));
    else
    {
      aTValue = new TDataStd_Real;
      theRelocTable.Bind(aNb, aTValue);
    }
    aC->SetValue(aTValue);
  }

  // geometries
  Standard_Integer NbGeom;
  if (! (theSource >> NbGeom))
    return Standard_False;
  Standard_Integer iG = 1;
  while (iG <= NbGeom)
  {
    if (! (theSource >> aNb))
      return Standard_False;
    if (aNb > 0)
    {
      Handle(TNaming_NamedShape) aG;
      if (theRelocTable.IsBound(aNb))
        aG = Handle(TNaming_NamedShape)::DownCast(theRelocTable.Find(aNb));
      else
      {
        aG = new TNaming_NamedShape;
        theRelocTable.Bind(aNb, aG);
      }
      aC->SetGeometry (iG++, aG);
    }
  }

  // plane
  if (! (theSource >> aNb))
    return Standard_False;
  if (aNb > 0)
  {
    Handle(TNaming_NamedShape) aTPlane;
    if (theRelocTable.IsBound(aNb))
      aTPlane = Handle(TNaming_NamedShape)::DownCast(theRelocTable.Find(aNb));
    else
    {
      aTPlane = new TNaming_NamedShape;
      theRelocTable.Bind(aNb, aTPlane);
    }
    aC->SetPlane(aTPlane);
  }

  // constraint type
  Standard_Integer aType;
  if (! (theSource >> aType))
    return Standard_False;
  aC->SetType( (TDataXtd_ConstraintEnum) aType );

  // flags
  Standard_Integer flags;
  if (! (theSource >> flags))
    return Standard_False;
  aC->Verified( flags & 1);
  aC->Inverted( flags & 2);
  aC->Reversed( flags & 4);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataXtd_ConstraintDriver::Paste
                        (const Handle(TDF_Attribute)& theSource,
                         BinObjMgt_Persistent&        theTarget,
                         BinObjMgt_SRelocationTable&  theRelocTable) const
{
  Handle(TDataXtd_Constraint) aC =
    Handle(TDataXtd_Constraint)::DownCast(theSource);

  Standard_Integer aNb;

  // value
  Handle(TDataStd_Real) aValue = aC->GetValue();
  if (!aValue.IsNull())
    aNb = theRelocTable.Add(aValue);    // create and/or get index
  else
    aNb = -1;
  theTarget << aNb;

  // geometries
  Standard_Integer NbGeom = aC->NbGeometries();
  theTarget << NbGeom;
  Standard_Integer iG;
  for (iG = 1; iG <= NbGeom; iG++)
  {
    Handle(TNaming_NamedShape) aG = aC->GetGeometry(iG);
    if (!aG.IsNull())
      aNb = theRelocTable.Add(aG);
    else
      aNb = -1;
    theTarget << aNb;
  }

  // plane
  Handle(TNaming_NamedShape) aTPlane = aC->GetPlane();
  if (!aTPlane.IsNull())
    aNb = theRelocTable.Add(aTPlane);
  else
    aNb = -1;
  theTarget << aNb;

  // constraint type
  theTarget << (Standard_Integer) aC->GetType();

  // flags
  Standard_Integer flags = 0;
  if (aC->Verified()) flags |= 1;
  if (aC->Inverted()) flags |= 2;
  if (aC->Reversed()) flags |= 4;
  theTarget << flags;
}
