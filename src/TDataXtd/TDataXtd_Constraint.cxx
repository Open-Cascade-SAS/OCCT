// Created on: 2009-04-06
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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


#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataStd.hxx>
#include <TDataStd_Real.hxx>
#include <TDataXtd.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TDataXtd_ConstraintEnum.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataXtd_Constraint,TDF_Attribute)

// for symmetry midpoint the third argument is the axis or the point
//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataXtd_Constraint::GetID () 
{ 
  static Standard_GUID TDataXtd_ConstraintID("2a96b602-ec8b-11d0-bee7-080009dc3333");
  return TDataXtd_ConstraintID; 
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataXtd_Constraint) TDataXtd_Constraint::Set (const TDF_Label& L)
{
  Handle (TDataXtd_Constraint) A;
  if (!L.FindAttribute (TDataXtd_Constraint::GetID (), A)) {    
    A = new TDataXtd_Constraint ();
    L.AddAttribute(A);
  }
  return A;
}


//=======================================================================
//function : TDataXtd_Constraint
//purpose  : 
//=======================================================================

TDataXtd_Constraint::TDataXtd_Constraint()
     : myType           (TDataXtd_RADIUS),
       myIsReversed     (Standard_False),
       myIsInverted     (Standard_False),
       myIsVerified     (Standard_True)
{}

 //=======================================================================
 //function : Set
 //purpose  : 
 //=======================================================================

 void TDataXtd_Constraint::Set(const TDataXtd_ConstraintEnum type,
 			      const Handle(TNaming_NamedShape)& G1) 
 {
   // OCC2932 correction
   if(myType == type)
   {
     Handle(TNaming_NamedShape) aShape =
       Handle(TNaming_NamedShape)::DownCast(myGeometries[0]);
     if (aShape.IsNull() == Standard_False && G1.IsNull() == Standard_False)
       if (aShape -> Get() == G1 -> Get()) 
         return;
   }

   Backup();
   myType = type;
   myGeometries[0] = G1;
 }

// =======================================================================
 //function : Set
 //purpose  : 
 //=======================================================================

 void TDataXtd_Constraint::Set(const TDataXtd_ConstraintEnum type,
 			       const Handle(TNaming_NamedShape)& G1,
 			       const Handle(TNaming_NamedShape)& G2) 
 {   
   // OCC2932 correction
   if(myType == type)
   {
     Handle(TNaming_NamedShape) aShape1 =
       Handle(TNaming_NamedShape)::DownCast(myGeometries[0]);
     Handle(TNaming_NamedShape) aShape2 =
       Handle(TNaming_NamedShape)::DownCast(myGeometries[1]);
     if (aShape1.IsNull() == Standard_False && G1.IsNull() == Standard_False &&
         aShape2.IsNull() == Standard_False && G2.IsNull() == Standard_False)
       if (aShape1->Get() == G1->Get() && aShape2->Get() == G2->Get())
         return;
   }

   Backup(); 
   myType = type;
   myGeometries[0] = G1; 
   myGeometries[1] = G2;
 }

 //=======================================================================
 //function : Set
 //purpose  : 
 //=======================================================================

 void TDataXtd_Constraint::Set(const TDataXtd_ConstraintEnum type,
 			      const Handle(TNaming_NamedShape)& G1,
 			      const Handle(TNaming_NamedShape)& G2,
 			      const Handle(TNaming_NamedShape)& G3) 
 {   
   // OCC2932 correction
   if (myType == type)
   {
     Handle(TNaming_NamedShape) aShape1 =
       Handle(TNaming_NamedShape)::DownCast(myGeometries[0]);
     Handle(TNaming_NamedShape) aShape2 =
       Handle(TNaming_NamedShape)::DownCast(myGeometries[1]);
     Handle(TNaming_NamedShape) aShape3 =
       Handle(TNaming_NamedShape)::DownCast(myGeometries[2]);
     if (aShape1.IsNull() == Standard_False && G1.IsNull() == Standard_False &&
         aShape2.IsNull() == Standard_False && G2.IsNull() == Standard_False &&
         aShape3.IsNull() == Standard_False && G3.IsNull() == Standard_False)
       if (aShape1->Get() == G1->Get() &&
           aShape2->Get() == G2->Get() &&
           aShape3->Get() == G3->Get())
         return;
   }

   Backup(); 
   myType = type;
   myGeometries[0] = G1; 
   myGeometries[1] = G2;  
   myGeometries[2] = G3;
 }

 //=======================================================================
 //function : Set
 //purpose  : 
 //=======================================================================

 void TDataXtd_Constraint::Set(const TDataXtd_ConstraintEnum type,
			      const Handle(TNaming_NamedShape)& G1,
 			      const Handle(TNaming_NamedShape)& G2,
 			      const Handle(TNaming_NamedShape)& G3,
 			      const Handle(TNaming_NamedShape)& G4)
 {   
   // OCC2932 correction
   if (myType == type)
   {
     Handle(TNaming_NamedShape) aShape1 =
       Handle(TNaming_NamedShape)::DownCast(myGeometries[0]);
     Handle(TNaming_NamedShape) aShape2 =
       Handle(TNaming_NamedShape)::DownCast(myGeometries[1]);
     Handle(TNaming_NamedShape) aShape3 =
       Handle(TNaming_NamedShape)::DownCast(myGeometries[2]);
     Handle(TNaming_NamedShape) aShape4 =
       Handle(TNaming_NamedShape)::DownCast(myGeometries[3]);
     if (aShape1.IsNull() == Standard_False && G1.IsNull() == Standard_False &&
         aShape2.IsNull() == Standard_False && G2.IsNull() == Standard_False &&
         aShape3.IsNull() == Standard_False && G3.IsNull() == Standard_False &&
         aShape4.IsNull() == Standard_False && G4.IsNull() == Standard_False)
       if (aShape1->Get() == G1->Get() &&
           aShape2->Get() == G2->Get() &&
           aShape3->Get() == G3->Get() &&
           aShape3->Get() == G4->Get())
         return;
   }

   Backup(); 
   myType = type;
   myGeometries[0] = G1; 
   myGeometries[1] = G2;   
   myGeometries[2] = G3;
   myGeometries[3] = G4;  
 }

//=======================================================================
//function : SetPlane
//purpose  : 
//=======================================================================
void TDataXtd_Constraint::SetPlane(const Handle(TNaming_NamedShape)& plane)
{
  // OCC2932 correction
  if (! myPlane.IsNull() && ! plane.IsNull() && myPlane->Get() == plane->Get())
      return;

  Backup();
  myPlane = plane;
}

//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================
const Handle(TNaming_NamedShape)&  TDataXtd_Constraint::GetPlane() const 
{
  return myPlane;
}

//=======================================================================
//function : SetType 
//purpose  : 
//=======================================================================

void TDataXtd_Constraint::SetType (const TDataXtd_ConstraintEnum CTR) 
{  
  // OCC2932 correction
  if(myType == CTR) return;

  Backup();
  myType = CTR;
}


//=======================================================================
//function : GetType
//purpose  : 
//=======================================================================

TDataXtd_ConstraintEnum TDataXtd_Constraint::GetType () const
{
  return myType;
}


//=======================================================================
//function : ClearGeometries
//purpose  : 
//=======================================================================

void TDataXtd_Constraint::ClearGeometries () 
{   
  // OCC2932 correction
  if(myGeometries[0].IsNull() && myGeometries[1].IsNull() && 
     myGeometries[2].IsNull() && myGeometries[3].IsNull()) 
    return;


  Backup();
  (myGeometries [0]).Nullify ();
  (myGeometries [1]).Nullify ();
  (myGeometries [2]).Nullify ();
  (myGeometries [3]).Nullify ();
}


//=======================================================================
//function : SetGeometry
//purpose  : 
//=======================================================================

void TDataXtd_Constraint::SetGeometry (const Standard_Integer Index, 
				       const Handle(TNaming_NamedShape)& G) 
{  
  // OCC2932 correction
  Handle(TNaming_NamedShape) aGeom =
    Handle(TNaming_NamedShape)::DownCast(myGeometries[Index - 1]);
  if (aGeom.IsNull() == Standard_False && G.IsNull() == Standard_False) 
    if (aGeom -> Get() == G->Get())
      return;

  Backup();
  myGeometries [Index-1] = G;
}


//=======================================================================
//function : GetGeometry
//purpose  : 
//=======================================================================

Handle(TNaming_NamedShape) TDataXtd_Constraint::GetGeometry
                                        (const Standard_Integer Index) const
{
  return Handle(TNaming_NamedShape)::DownCast (myGeometries [Index-1]);
}


//=======================================================================
//function : NbGeometries
//purpose  : 
//=======================================================================

Standard_Integer TDataXtd_Constraint::NbGeometries () const 
{
  Standard_Integer num_geom = 0 ;
  while (num_geom < 4 && ! myGeometries[num_geom].IsNull()) {
      num_geom += 1 ;
  }
  return num_geom ;
}


//=======================================================================
//function : IsDimension
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Constraint::IsDimension () const 
{
  return !myValue.IsNull();
}


//=======================================================================
//function : IsPlanar
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Constraint::IsPlanar () const 
{
  return !myPlane.IsNull();
}

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void TDataXtd_Constraint::SetValue (const Handle(TDataStd_Real)& V) 
{
  // OCC2932 correction
  if (myValue.IsNull() == Standard_False && V.IsNull() == Standard_False)
    if(myValue->Get() == V->Get()) return;

  Backup();
  myValue = V;
}

//=======================================================================
//function : GetValue
//purpose  : 
//=======================================================================

const Handle(TDataStd_Real)& TDataXtd_Constraint::GetValue () const
{
  return myValue;
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataXtd_Constraint::ID () const { return GetID(); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataXtd_Constraint::NewEmpty () const
{  
  return new TDataXtd_Constraint (); 
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataXtd_Constraint::Restore(const Handle(TDF_Attribute)& with) 
{
  Handle(TDataXtd_Constraint) CTR =Handle(TDataXtd_Constraint)::DownCast(with); 
  myGeometries [0] = CTR->GetGeometry (1);
  myGeometries [1] = CTR->GetGeometry (2);
  myGeometries [2] = CTR->GetGeometry (3);
  myGeometries [3] = CTR->GetGeometry (4);
  myType = CTR->GetType ();
  myValue = CTR->GetValue ();
  myIsVerified = CTR->Verified();
  myIsInverted = CTR->Inverted();
  myIsReversed = CTR->Reversed();
  myPlane = CTR->GetPlane();
}



//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataXtd_Constraint::Paste (const Handle(TDF_Attribute)& into,
				 const Handle(TDF_RelocationTable)& RT) const
{  
  Handle(TDataXtd_Constraint) CTR =Handle(TDataXtd_Constraint)::DownCast (into);
  Handle(TNaming_NamedShape) G1, G2, G3, G4, plane;  
  RT->HasRelocation (myGeometries[0], G1);
  CTR->SetGeometry (1, G1);
  RT->HasRelocation (myGeometries[1], G2);
  CTR->SetGeometry (2, G2);
  RT->HasRelocation (myGeometries[2], G3);
  CTR->SetGeometry (3, G3);
  RT->HasRelocation (myGeometries[3], G4);
  CTR->SetGeometry (4, G4);

  RT->HasRelocation (myPlane, plane);
  CTR->SetPlane(plane);

  Handle(TDataStd_Real) Value;
  RT->HasRelocation (myValue,Value);
  CTR->SetValue (Value);

  CTR->SetType (myType);
  CTR->Verified(Verified());
  CTR->Inverted(Inverted());
  CTR->Reversed(Reversed());
}    


//=======================================================================
//function : References
//purpose  : 
//=======================================================================

void TDataXtd_Constraint::References(const Handle(TDF_DataSet)& DS) const

{ 
//bidouille en attendant traitement des contraintes d assemblage en dehors de la part 
// l attribut placement devrait oriente vers les contraintes de placement en dehors
  Standard_Integer Lim;
  if (myType >= TDataXtd_MATE && myType<=TDataXtd_FACES_ANGLE)  Lim =1;
  else Lim =3;

  for (Standard_Integer i=0; i<=Lim; i++) {
    if (!myGeometries [i].IsNull()) DS->AddAttribute (myGeometries[i]);
  }

  if (!myValue.IsNull()) DS->AddAttribute (myValue);
  if (!myPlane.IsNull()) DS->AddAttribute (myPlane);
}

//=======================================================================
//function : Verified
//purpose  : 
//=======================================================================
void TDataXtd_Constraint::Verified(const Standard_Boolean status)
{
  // OCC2932 correction
  if(myIsVerified == status) return;

  Backup();
  myIsVerified = status;
}

//=======================================================================
//function : Verified
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Constraint::Verified() const 
{
  return myIsVerified;
}

//=======================================================================
//function : Reversed
//purpose  : 
//=======================================================================
void TDataXtd_Constraint::Reversed(const Standard_Boolean status)
{
  // OCC2932 correction
  if(myIsReversed == status ) return;

  Backup();
  myIsReversed = status;
}

//=======================================================================
//function : Reversed
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Constraint::Reversed() const 
{
  return myIsReversed;
}

//=======================================================================
//function : Inverted
//purpose  : 
//=======================================================================
void TDataXtd_Constraint::Inverted(const Standard_Boolean status)
{
  // OCC2932 correction
  if(myIsInverted == status) return;

  Backup();
  myIsInverted = status;
}

//=======================================================================
//function : Inverted
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Constraint::Inverted() const 
{
  return myIsInverted;
}


//=======================================================================
//function : CollectChildConstraints
//purpose  : 
//=======================================================================

void TDataXtd_Constraint::CollectChildConstraints(const TDF_Label& aLabel,
						  TDF_LabelList& LL)  
{
  TDF_ChildIterator it(aLabel,Standard_True);
  Handle(TDataXtd_Constraint) aConstraint;
  for (; it.More(); it.Next()) {
    if (it.Value().FindAttribute(TDataXtd_Constraint::GetID(), aConstraint)) {
      LL.Append(it.Value());
    }
  }
  
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataXtd_Constraint::Dump (Standard_OStream& anOS) const
{  
  anOS << "Constraint ";
  TDataXtd::Print(GetType(),anOS);
  return anOS;
}

