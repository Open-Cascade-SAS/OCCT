// Created on: 1997-04-14
// Created by: VAUTHIER Jean-Claude
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <MNaming_NamedShapeStorageDriver.ixx>

#include <MgtBRep.hxx>

#include <PNaming_NamedShape.hxx>

#include <PColStd_HArray1OfInteger.hxx>
#include <PTColStd_TransientPersistentMap.hxx>

#include <PTopoDS_HArray1OfShape1.hxx>

#include <Standard_DomainError.hxx>

#include <TNaming_NamedShape.hxx>
#include <TNaming_Evolution.hxx>
#include <TNaming_Iterator.hxx>
#include <CDM_MessageDriver.hxx>
#include <TopoDS_Shape.hxx>

static Standard_Integer EvolutionInt (const TNaming_Evolution);


//=======================================================================
//function : MNaming_NamedShapeStorageDriver
//purpose  : 
//=======================================================================

MNaming_NamedShapeStorageDriver::MNaming_NamedShapeStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MNaming_NamedShapeStorageDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MNaming_NamedShapeStorageDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(TNaming_NamedShape);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MNaming_NamedShapeStorageDriver::NewEmpty() const
{
  return new PNaming_NamedShape ();
}



//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MNaming_NamedShapeStorageDriver::Paste (
   const Handle(TDF_Attribute)&        Source,
   const Handle(PDF_Attribute)&        Target,
   const Handle(MDF_SRelocationTable)& RelocTable) const

{
  Handle(TNaming_NamedShape) S =
    Handle(TNaming_NamedShape)::DownCast (Source);
  Handle(PNaming_NamedShape) PAttribute =
    Handle(PNaming_NamedShape)::DownCast(Target);

  PTColStd_TransientPersistentMap& TPMap = RelocTable->OtherTable();

  TNaming_Evolution evol = S->Evolution();

  //--------------------------------------------------------------
  //Provisoire pour avoir le nombre de shapes et initialiser Target
  Standard_Integer NbShapes = 0;
  for (TNaming_Iterator SItr (S); SItr.More (); SItr.Next ()) NbShapes++;
  //--------------------------------------------------------------

  if (NbShapes == 0) return;

  Handle(PTopoDS_HArray1OfShape1)  OldPShapes =
    new PTopoDS_HArray1OfShape1(1,NbShapes);
  Handle(PTopoDS_HArray1OfShape1)  NewPShapes =
    new PTopoDS_HArray1OfShape1(1,NbShapes);

  PTopoDS_Shape1 NewPShape;
  PTopoDS_Shape1 OldPShape;
  Standard_Integer i = 1;
  
  for (TNaming_Iterator SIterator(S) ;SIterator.More(); SIterator.Next()) {
    const TopoDS_Shape& OldShape = SIterator.OldShape();
    const TopoDS_Shape& NewShape = SIterator.NewShape();

    if ( evol != TNaming_PRIMITIVE ) {
      MgtBRep::Translate1(OldShape, TPMap, OldPShape,
			  MgtBRep_WithoutTriangle);
    }
    else OldPShape.Nullify();
    OldPShapes->SetValue(i,OldPShape);

    if (evol != TNaming_DELETE) {
      MgtBRep::Translate1(NewShape, TPMap, NewPShape,
			  MgtBRep_WithoutTriangle);
    }
    else NewPShape.Nullify();
    NewPShapes->SetValue(i,NewPShape);
    i++;
  }
  PAttribute->OldShapes(OldPShapes);
  PAttribute->NewShapes(NewPShapes);
  PAttribute->ShapeStatus(EvolutionInt(evol));
  PAttribute->Version    (S->Version());

  NewPShape.Nullify();
  OldPShape.Nullify();
}


Standard_Integer EvolutionInt(const TNaming_Evolution i)
{
  switch(i) {
    case TNaming_PRIMITIVE    : return 0;
    case TNaming_GENERATED    : return 1;
    case TNaming_MODIFY       : return 2;
    case TNaming_DELETE       : return 3;
    case TNaming_SELECTED     : return 4;
    case TNaming_REPLACE      : return 2; //case TNaming_REPLACE      : return 5; for compatibility
  default:
    Standard_DomainError::Raise("TNaming_Evolution; enum term unknown");
  }
  return 0; // To avoid compilation error message.
}

