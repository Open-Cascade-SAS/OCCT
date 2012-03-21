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

#include  <Transfer_TransientListBinder.ixx>
//#include  <TColStd.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

Transfer_TransientListBinder::Transfer_TransientListBinder  ()
{  theres = new TColStd_HSequenceOfTransient();  }

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

Transfer_TransientListBinder::Transfer_TransientListBinder
  (const Handle(TColStd_HSequenceOfTransient)& list)
{  theres = list;  }

//=======================================================================
//function : IsMultiple
//purpose  : 
//=======================================================================

Standard_Boolean  Transfer_TransientListBinder::IsMultiple () const
{  return (NbTransients() > 1);  }

//=======================================================================
//function : ResultType
//purpose  : 
//=======================================================================

Handle(Standard_Type)  Transfer_TransientListBinder::ResultType () const
{  return STANDARD_TYPE(Transfer_TransientListBinder);  }

//=======================================================================
//function : ResultTypeName
//purpose  : 
//=======================================================================

Standard_CString  Transfer_TransientListBinder::ResultTypeName () const
{  return "list(Standard_Transient)";  }


//=======================================================================
//function : AddResult
//purpose  : 
//=======================================================================

void  Transfer_TransientListBinder::AddResult (const Handle(Standard_Transient)& Transient)
{  theres->Append(Transient);  }

//=======================================================================
//function : Result
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) Transfer_TransientListBinder::Result () const
{  return theres;  }

//=======================================================================
//function : SetResult
//purpose  : 
//=======================================================================

void  Transfer_TransientListBinder::SetResult
  (const Standard_Integer num, const Handle(Standard_Transient)& Transient)
{  theres->SetValue(num,Transient);  }

//=======================================================================
//function : NbTransients
//purpose  : 
//=======================================================================

Standard_Integer  Transfer_TransientListBinder::NbTransients () const
{  return theres->Length();  }

//=======================================================================
//function : Transient
//purpose  : 
//=======================================================================

const Handle(Standard_Transient)&
  Transfer_TransientListBinder::Transient (const Standard_Integer num) const
{  return theres->Value(num);  }

