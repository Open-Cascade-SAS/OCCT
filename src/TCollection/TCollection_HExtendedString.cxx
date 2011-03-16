//Copyright:      Matra Datavision 1992,1993
//Created By      M. MERCIEN Dec,15 1992
//Modified: C. LEYNADIER Nov,21 1997 (Token et ChangeString)

#include <TCollection_AsciiString.hxx>
#include <TCollection_HExtendedString.ixx>
#include <TCollection_HAsciiString.hxx>
#include <Standard_ExtCharacter.hxx>
#include <Standard_ExtString.hxx>

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HExtendedString::TCollection_HExtendedString(){}


// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HExtendedString::TCollection_HExtendedString
        (const Standard_CString message):myString(message)
{}

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HExtendedString::TCollection_HExtendedString
        (const Standard_ExtString message):myString(message)
{}

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HExtendedString::TCollection_HExtendedString
        (const Standard_ExtCharacter aChar):myString(aChar)
{}

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HExtendedString::TCollection_HExtendedString
          (const Standard_Integer length,const Standard_ExtCharacter filler )
          :myString(length,filler)
{}

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HExtendedString::TCollection_HExtendedString
        (const TCollection_ExtendedString& astring):myString(astring)
{}

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HExtendedString::TCollection_HExtendedString
        (const Handle(TCollection_HAsciiString)& astring)
        :myString(astring->String())
{
}

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
TCollection_HExtendedString::TCollection_HExtendedString
        (const Handle(TCollection_HExtendedString)& astring)
        :myString(astring->ChangeString())
{
}

// ---------------------------------------------------------------------------
// AssignCat
// ----------------------------------------------------------------------------
void  TCollection_HExtendedString::AssignCat
     (const Handle(TCollection_HExtendedString)& other) 
{
   myString.AssignCat(other->ChangeString());
}

// ---------------------------------------------------------------------------
// Cat
// ----------------------------------------------------------------------------
Handle(TCollection_HExtendedString)  TCollection_HExtendedString::Cat
     (const Handle(TCollection_HExtendedString)& other)  const
{
   return new TCollection_HExtendedString(myString.Cat(other->ChangeString() ) );
}

// ----------------------------------------------------------------------------
// ChangeAll
// ----------------------------------------------------------------------------
void TCollection_HExtendedString::ChangeAll
                       (const Standard_ExtCharacter aChar,
                        const Standard_ExtCharacter NewChar)
{
   myString.ChangeAll(aChar,NewChar);
}

// ----------------------------------------------------------------------------
// IsEmpty
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_HExtendedString::IsEmpty() const
{
   return (myString.Length() == 0);
}

// ----------------------------------------------------------------------------
// Clear
// ----------------------------------------------------------------------------
void TCollection_HExtendedString::Clear()
{
   myString.Clear();
}

// ----------------------------------------------------------------------------
// Insert a Standard_ExtCharacter before 'where'th Standard_ExtCharacter
// ----------------------------------------------------------------------------
void TCollection_HExtendedString::Insert(const Standard_Integer where,
                                 const Standard_ExtCharacter what)
{
   myString.Insert(where,what);
}

// ----------------------------------------------------------------------------
// Insert
// ----------------------------------------------------------------------------
void TCollection_HExtendedString::Insert(const Standard_Integer where,
                          const Handle(TCollection_HExtendedString)& what)
{
   myString.Insert(where,what->ChangeString());
}

// ----------------------------------------------------------------------------
// IsLess
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_HExtendedString::IsLess(
        const Handle(TCollection_HExtendedString)& other) const
{
   return myString.IsLess(other->ChangeString());
}

// ----------------------------------------------------------------------------
// IsGreater
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_HExtendedString::IsGreater
    (const Handle(TCollection_HExtendedString)& other) const
{
   return myString.IsGreater(other->ChangeString());
}

// ----------------------------------------------------------------------------
// IsAscii
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_HExtendedString::IsAscii() const 
{
   return myString.IsAscii();
}

// ----------------------------------------------------------------------------
// Length
// ----------------------------------------------------------------------------
Standard_Integer TCollection_HExtendedString::Length() const
{
   return myString.Length();
}

// ----------------------------------------------------------------------------
// Remove
// ----------------------------------------------------------------------------
void TCollection_HExtendedString::Remove (const Standard_Integer where,
                                  const Standard_Integer ahowmany)
{
   myString.Remove(where,ahowmany);
}

// ----------------------------------------------------------------------------
// RemoveAll
// ----------------------------------------------------------------------------
void TCollection_HExtendedString::RemoveAll(const Standard_ExtCharacter what)
{
   myString.RemoveAll(what);
}

// ----------------------------------------------------------------------------
// SetValue
// ----------------------------------------------------------------------------
void TCollection_HExtendedString::SetValue(
           const Standard_Integer where,const Standard_ExtCharacter what)
{
   myString.SetValue(where,what);
}

// ----------------------------------------------------------------------------
// SetValue
// ---------------------------------------------------------------------------
void TCollection_HExtendedString::SetValue(const Standard_Integer where,
                       const Handle(TCollection_HExtendedString)& what)
{
   myString.SetValue(where, what->ChangeString());
}

// ----------------------------------------------------------------------------
// Split
// ----------------------------------------------------------------------------
Handle(TCollection_HExtendedString) TCollection_HExtendedString::Split
              (const Standard_Integer where)
{
   return new TCollection_HExtendedString(myString.Split(where));
}

// ----------------------------------------------------------------------------
// Search
// ----------------------------------------------------------------------------
Standard_Integer TCollection_HExtendedString::Search
                       (const Handle(TCollection_HExtendedString)& what) const
{
   return  myString.Search(what->ChangeString());
}

// ----------------------------------------------------------------------------
// SearchFromEnd
// ----------------------------------------------------------------------------
Standard_Integer TCollection_HExtendedString::SearchFromEnd
                       (const Handle(TCollection_HExtendedString)& what) const
{
   return  myString.SearchFromEnd(what->ChangeString());
}

// ----------------------------------------------------------------------------
// Token
// ----------------------------------------------------------------------------
Handle(TCollection_HExtendedString) TCollection_HExtendedString::Token
         (const Standard_ExtString separators,const Standard_Integer whichone) const
{
   return new TCollection_HExtendedString(myString.Token(separators,whichone));
}

// ----------------------------------------------------------------------------
// ToExtString
// ----------------------------------------------------------------------------
const Standard_ExtString TCollection_HExtendedString::ToExtString() const
{ 
   return (myString.ToExtString());
}

// ----------------------------------------------------------------------------
// Trunc
// ----------------------------------------------------------------------------
void TCollection_HExtendedString::Trunc(const Standard_Integer ahowmany)
{
   myString.Trunc(ahowmany);
}

// ----------------------------------------------------------------------------
// Value
// ----------------------------------------------------------------------------
Standard_ExtCharacter TCollection_HExtendedString::Value
             (const Standard_Integer where) const
{
   return myString.Value(where);
}

// ----------------------------------------------------------------------------
// String
// ----------------------------------------------------------------------------
TCollection_ExtendedString TCollection_HExtendedString::String() const
{
   return myString;
}


//---------------------------------------------------------------------
// Print
//---------------------------------------------------------------------
void TCollection_HExtendedString::Print(Standard_OStream& S) const
{
  S << "begin class HExtendedString "<<endl;
  myString.Print(S);
}

// ----------------------------------------------------------------------------
// ShallowCopy
// ----------------------------------------------------------------------------
Handle(TCollection_HExtendedString) TCollection_HExtendedString::ShallowCopy() const
{
   Handle(TCollection_HExtendedString) thecopy = 
                new TCollection_HExtendedString;
   for (Standard_Integer i = 1 ; i <= Length() ; i++) 
          thecopy->Insert(i,Value(i));
   return thecopy;
}

//---------------------------------------------------------------------
// ShallowDump
//---------------------------------------------------------------------
void TCollection_HExtendedString::ShallowDump(Standard_OStream& S) const
{
  S << "begin class HExtendedString "<<endl;
  myString.Print(S);
}

// ----------------------------------------------------------------------------
// Issamestate
// ----------------------------------------------------------------------------
Standard_Boolean TCollection_HExtendedString::IsSameState
   (const Handle(TCollection_HExtendedString)& other) const
 {
   Handle(TCollection_HExtendedString) H;
   H = Handle(TCollection_HExtendedString)::DownCast(other);
   return ( myString == H->ChangeString() );
 }


TCollection_ExtendedString& TCollection_HExtendedString::ChangeString() const
{
  return (TCollection_ExtendedString&)myString;
}
