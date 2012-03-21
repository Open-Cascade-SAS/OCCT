// Created on: 1999-06-25
// Created by: Sergey RUIN
// Copyright (c) 1999-1999 Matra Datavision
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



#include <TDataStd_Directory.ixx>
#include <TDataStd.hxx>
#include <TDF_TagSource.hxx>
#include <Standard_DomainError.hxx>

//=======================================================================
//function : Find
//purpose  : 
//=======================================================================

Standard_Boolean TDataStd_Directory::Find (const TDF_Label& current,
					  Handle(TDataStd_Directory)& D) 
{  
  TDF_Label L = current;
  Handle(TDataStd_Directory) dir;
  if (L.IsNull()) return Standard_False; 

  while (1) {
    if(L.FindAttribute(TDataStd_Directory::GetID(), dir)) break; 
    L = L.Father();
    if (L.IsNull()) break; 
  }
  
  if (!dir.IsNull()) { 
    D = dir;
    return Standard_True; 
  }
  return Standard_False; 
}


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_Directory::GetID() 
{
  static Standard_GUID TDataStd_DirectoryID("2a96b61f-ec8b-11d0-bee7-080009dc3333");
  return TDataStd_DirectoryID;
}


//=======================================================================
//function : New
//purpose  : 
//=======================================================================

Handle(TDataStd_Directory) TDataStd_Directory::New (const TDF_Label& L)
{  
  if (L.HasAttribute()) {
    Standard_DomainError::Raise("TDataStd_Directory::New : not an empty label");
  }
  Handle(TDataStd_Directory) A = new TDataStd_Directory ();
  L.AddAttribute(A);                        
  TDF_TagSource::Set(L);     
  return A;
}


//=======================================================================
//function : TDataStd_AddDirectory
//purpose  : 
//=======================================================================

Handle(TDataStd_Directory) TDataStd_Directory::AddDirectory(const Handle(TDataStd_Directory)& dir)
{
  TDF_Label newLabel = TDF_TagSource::NewChild ( dir->Label() );
  Handle(TDataStd_Directory) A = TDataStd_Directory::New (newLabel );
  return A;
}


//=======================================================================
//function : TDataStd_MakeObjectLabel
//purpose  : 
//=======================================================================

TDF_Label TDataStd_Directory::MakeObjectLabel(const Handle(TDataStd_Directory)& dir)
{
  return TDF_TagSource::NewChild ( dir->Label() );
}

//=======================================================================
//function : TDataStd_Directory
//purpose  : 
//=======================================================================

TDataStd_Directory::TDataStd_Directory()
{
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_Directory::ID() const
{ return GetID(); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataStd_Directory::NewEmpty () const
{  
  return new TDataStd_Directory(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataStd_Directory::Restore(const Handle(TDF_Attribute)& with) 
{  
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataStd_Directory::Paste (const Handle(TDF_Attribute)& into,
			   const Handle(TDF_RelocationTable)& RT) const
{  
}

//=======================================================================
//function : References
//purpose  : 
//=======================================================================

void TDataStd_Directory::References (const Handle(TDF_DataSet)& DS) const
{  
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataStd_Directory::Dump (Standard_OStream& anOS) const
{  
  anOS << "Directory";
  return anOS;
}


