// File:	TDataStd_Directory.cxx
// Created:	Fri Jun 25 14:42:27 1999
// Author:	Sergey RUIN


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


