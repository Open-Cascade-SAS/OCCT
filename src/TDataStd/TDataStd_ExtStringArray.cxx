// File:	TDataStd_ExtStringArray.cxx
// Created:	Wed Jan 16 10:10:40 2002
// Author:	Michael PONIKAROV
//		<mpv@covox>


#include <TDataStd_ExtStringArray.ixx>
#include <TDataStd_DeltaOnModificationOfExtStringArray.hxx>
#include <TDF_DefaultDeltaOnModification.hxx>

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_ExtStringArray::GetID() 
{ 
  static Standard_GUID anExtStringArrayID ("2a96b624-ec8b-11d0-bee7-080009dc3333");
  return anExtStringArrayID; 
}


//=======================================================================
//function : TDataStd_ExtStringArray::TDataStd_ExtStringArray
//purpose  : 
//=======================================================================

TDataStd_ExtStringArray::TDataStd_ExtStringArray() 
    : myIsDelta(Standard_False){}
     
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TDataStd_ExtStringArray::Init(const Standard_Integer lower,
				const Standard_Integer upper)
{
  Backup();
  myValue = new TColStd_HArray1OfExtendedString(lower, upper, "");
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataStd_ExtStringArray) TDataStd_ExtStringArray::Set (
					  const TDF_Label& label,
					  const Standard_Integer lower,
					  const Standard_Integer upper,
					  const Standard_Boolean isDelta) 

{
  Handle(TDataStd_ExtStringArray) A;
  if (!label.FindAttribute (TDataStd_ExtStringArray::GetID(), A)) {
    A = new TDataStd_ExtStringArray;
    A->Init (lower, upper);
    A->SetDelta(isDelta); 
    label.AddAttribute(A);
  }
  else if (lower != A->Lower() || upper != A->Upper())
  {
    A->Init (lower, upper); 
  }
  return A;
}


//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void TDataStd_ExtStringArray::SetValue(const Standard_Integer index, const TCollection_ExtendedString& value) 
{
  if(myValue.IsNull()) return;
  if( myValue->Value(index) == value)
    return; 

  Backup();
  myValue->SetValue(index, value);
}


//=======================================================================
//function : GetValue
//purpose  : 
//=======================================================================

TCollection_ExtendedString TDataStd_ExtStringArray::Value (const Standard_Integer index) const 
{
  if(myValue.IsNull()) return TCollection_ExtendedString();
  return myValue->Value(index); 
}



//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_ExtStringArray::Lower (void) const 
{ 
  if(myValue.IsNull()) return 0;
  return myValue->Lower(); 
}


//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_ExtStringArray::Upper (void) const 
{ 
  if(myValue.IsNull()) return 0;
  return myValue->Upper(); 
}


//=======================================================================
//function : Length
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_ExtStringArray::Length (void) const 
{
  if(myValue.IsNull()) return 0;
  return myValue->Length(); 
}



//=======================================================================
//function : ChangeArray
//purpose  : If value of <newArray> differs from <myValue>, Backup 
//         : performed and myValue refers to new instance of HArray1OfExtendedString
//         : that holds <newArray>
//=======================================================================

void TDataStd_ExtStringArray::ChangeArray(const Handle(TColStd_HArray1OfExtendedString)& newArray,
					  const Standard_Boolean isCheckItems) 
{
  Standard_Integer aLower    = newArray->Lower();
  Standard_Integer anUpper   = newArray->Upper();
  Standard_Boolean aDimEqual = Standard_False;
  Standard_Integer i;

  if (Lower() == aLower && Upper() == anUpper ) {
    aDimEqual = Standard_True;
    Standard_Boolean isEqual = Standard_True;
    if(isCheckItems) {
      for(i = aLower; i <= anUpper; i++) {
	if(myValue->Value(i) != newArray->Value(i)) {
	  isEqual = Standard_False;
	  break;
	}
      }
      if(isEqual)
	return;
    }
  }

  Backup();

// Handles of myValue of current and backuped attributes will be different!!!
  if(myValue.IsNull() || !aDimEqual) 
    myValue = new TColStd_HArray1OfExtendedString(aLower, anUpper);

  for(i = aLower; i <= anUpper; i++) 
    myValue->SetValue(i, newArray->Value(i));
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_ExtStringArray::ID () const { return GetID(); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataStd_ExtStringArray::NewEmpty () const
{  
  return new TDataStd_ExtStringArray(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataStd_ExtStringArray::Restore(const Handle(TDF_Attribute)& With) 
{
  Standard_Integer i, lower, upper;
  Handle(TDataStd_ExtStringArray) anArray = Handle(TDataStd_ExtStringArray)::DownCast(With);
  if(!anArray->myValue.IsNull()) {
    lower = anArray->Lower();
    upper = anArray->Upper(); 
    myValue = new TColStd_HArray1OfExtendedString(lower, upper);
    for(i = lower; i<=upper; i++)
      myValue->SetValue(i, anArray->Value(i));
    myIsDelta = anArray->myIsDelta;
  }
  else
    myValue.Nullify();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataStd_ExtStringArray::Paste (const Handle(TDF_Attribute)& Into,
				   const Handle(TDF_RelocationTable)& ) const
{
  if(!myValue.IsNull()) {
    Handle(TDataStd_ExtStringArray) anAtt = Handle(TDataStd_ExtStringArray)::DownCast(Into);
    if(!anAtt.IsNull()) {
      anAtt->ChangeArray( myValue, Standard_False );
      anAtt->SetDelta(myIsDelta);
    }
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataStd_ExtStringArray::Dump (Standard_OStream& anOS) const
{  
  anOS << "ExtStringArray :";
  if(!myValue.IsNull()) {
    Standard_Integer i, lower, upper;
    lower = myValue->Lower();
    upper = myValue->Upper();
    for(i = lower; i<=upper; i++)
      anOS << "\t" <<myValue->Value(i)<<endl;
  }
  anOS << " Delta is " << myIsDelta ? "ON":"OFF";
  anOS << endl;
  return anOS;
}

//=======================================================================
//function : DeltaOnModification
//purpose  : 
//=======================================================================

Handle(TDF_DeltaOnModification) TDataStd_ExtStringArray::DeltaOnModification
(const Handle(TDF_Attribute)& OldAttribute) const
{
  if(myIsDelta)
    return new TDataStd_DeltaOnModificationOfExtStringArray(*((Handle(TDataStd_ExtStringArray)*)&OldAttribute));
  else return new TDF_DefaultDeltaOnModification(OldAttribute);
}

