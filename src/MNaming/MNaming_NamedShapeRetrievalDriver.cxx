// Created on: 1997-04-14
// Created by: VAUTHIER Jean-Claude
// Copyright (c) 1997-1999 Matra Datavision
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



#include <MNaming_NamedShapeRetrievalDriver.ixx>

#include <MgtBRep.hxx>

#include <PCollection_HAsciiString.hxx>

#include <PColStd_HArray1OfInteger.hxx>

#include <PNaming_NamedShape.hxx>

#include <PTopoDS_HArray1OfShape1.hxx>
#include <PTopoDS_Shape1.hxx>

#include <TDF_Data.hxx>
#include <TDF_Label.hxx>

#include <TNaming_NamedShape.hxx>
#include <TNaming_Evolution.hxx>
#include <TNaming_Builder.hxx>
#include <CDM_MessageDriver.hxx>
#include <TopoDS_Shape.hxx>

static TNaming_Evolution EvolutionEnum(const Standard_Integer);


//=======================================================================
//function : MNaming_NamedShapeRetrievalDriver
//purpose  : 
//=======================================================================

MNaming_NamedShapeRetrievalDriver::MNaming_NamedShapeRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{}


//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MNaming_NamedShapeRetrievalDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MNaming_NamedShapeRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PNaming_NamedShape);
  return sourceType;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MNaming_NamedShapeRetrievalDriver::NewEmpty() const

{
  return new TNaming_NamedShape ();
}



//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MNaming_NamedShapeRetrievalDriver::Paste (
   const Handle(PDF_Attribute)&        Source,
   const Handle(TDF_Attribute)&        Target,
   const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PNaming_NamedShape) S = Handle(PNaming_NamedShape)::DownCast (Source);
  Handle(TNaming_NamedShape) T = Handle(TNaming_NamedShape)::DownCast (Target);
  Standard_Integer NbShapes = S->NbShapes();

  //Handle (TDF_Data) TDF = RelocTable->Target ();
  TDF_Label         Label = Target->Label ();
  //TDF_Insertor      TDFIns(Label);
  TNaming_Builder   Bld   (Label);
  if (NbShapes == 0) return;

  TNaming_Evolution evol  = EvolutionEnum(S->ShapeStatus());
  T->SetVersion(S->Version()); // apres creation Builder qui a mis la version a 1.

  Handle(PTopoDS_HArray1OfShape1)  OldPShapes  = S->OldShapes();
  Handle(PTopoDS_HArray1OfShape1)  NewPShapes  = S->NewShapes();

  TopoDS_Shape OldShape;
  TopoDS_Shape NewShape;

  PTColStd_PersistentTransientMap& PTMap = RelocTable->OtherTable();

  for (Standard_Integer i = 1; i <= NbShapes; i++) {
    const PTopoDS_Shape1& NewPShape = NewPShapes->Value(i);
    const PTopoDS_Shape1& OldPShape = OldPShapes->Value(i);

    if ( evol != TNaming_PRIMITIVE ) {
      MgtBRep::Translate1(OldPShape, PTMap, OldShape,
			  MgtBRep_WithoutTriangle);
    }

    if (evol != TNaming_DELETE) {
      MgtBRep::Translate1(NewPShape, PTMap, NewShape,
			  MgtBRep_WithoutTriangle);
    }

    switch (evol) {
    case TNaming_PRIMITIVE    : {
      Bld.Generated(NewShape); break;
    }
    case TNaming_GENERATED    : {
      Bld.Generated(OldShape,NewShape); break;
    }
    case TNaming_MODIFY       : {
      Bld.Modify(OldShape,NewShape); break;
    }
    case TNaming_DELETE       : {
      Bld.Delete (OldShape); break;
    }
    case TNaming_SELECTED     : {
      Bld.Select(NewShape, OldShape); break;
    }
    case TNaming_REPLACE      :{
      Bld.Replace(OldShape,NewShape); break;
    }  
      default :
	Standard_DomainError::Raise("TNaming_Evolution; enum term unknown");
    }
    OldShape.Nullify();
    NewShape.Nullify();
  }
//  cout<<endl;  - vbu le 28/10/1998
}



TNaming_Evolution EvolutionEnum(const Standard_Integer i)
{
  switch(i)
    {
    case 0 : return TNaming_PRIMITIVE;
    case 1 : return TNaming_GENERATED;
    case 2 : return TNaming_MODIFY;
    case 3 : return TNaming_DELETE;
    case 4 : return TNaming_SELECTED;
    case 5 : return TNaming_REPLACE;
   default:
      Standard_DomainError::Raise("TNaming_Evolution; integer value without enum term equivalence");
    }
  return TNaming_PRIMITIVE; // To avoid compilation error message.
}

