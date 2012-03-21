// Created on: 1993-09-14
// Created by: GG
// Copyright (c) 1993-1999 Matra Datavision
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




//-Version

//-Design       Declaration des variables specifiques aux Entries de
//              Fontures de texts

//-Warning      Une entry est definie par un index associee a une Font style

//-References

//-Language     C++ 2.0

//-Declarations

// for the class
#include <Aspect_FontMapEntry.ixx>
#include <Aspect_Units.hxx>

//-Aliases

//-Global data definitions

//      MyType                  : FontStyle from Aspect
//      MyIndex                 : Integer from Standard
//      MyTypeIsDef             : Boolean from Standard
//      MyIndexIsDef            : Boolean from Standard

//-Constructors

//-Destructors

//-Methods, in order

Aspect_FontMapEntry::Aspect_FontMapEntry() {

        MyTypeIsDef     = Standard_True;
        MyIndexIsDef    = Standard_True;
        MyIndex         = 0;
        MyType.SetValues (Aspect_TOF_DEFAULT,3.0 MILLIMETER);

}

Aspect_FontMapEntry::Aspect_FontMapEntry (const Standard_Integer index, const Aspect_FontStyle &style) {

        MyTypeIsDef     = Standard_True;
        MyIndexIsDef    = Standard_True;
        MyIndex         = index;
        MyType          = style;

}

Aspect_FontMapEntry::Aspect_FontMapEntry (const Aspect_FontMapEntry& entry) {

        if ( !entry.MyTypeIsDef || !entry.MyIndexIsDef ) {
                Aspect_BadAccess::Raise("Unallocated FontMapEntry") ;
        }
        else {
                MyTypeIsDef     = Standard_True;
                MyIndexIsDef    = Standard_True;
                MyIndex         = entry.MyIndex;
                MyType          = entry.MyType;
        }
}

void Aspect_FontMapEntry::SetValue (const Standard_Integer index, const Aspect_FontStyle &style) {

        MyTypeIsDef     = Standard_True;
        MyIndexIsDef    = Standard_True;
        MyIndex         = index;
        MyType          = style;

}

void Aspect_FontMapEntry::SetValue (const Aspect_FontMapEntry& entry) {

        if ( !entry.MyTypeIsDef || !entry.MyIndexIsDef ) {
                Aspect_BadAccess::Raise("Unallocated FontMapEntry") ;
        }
        else {
                MyTypeIsDef     = Standard_True;
                MyIndexIsDef    = Standard_True;
                MyIndex         = entry.MyIndex;
                MyType          = entry.MyType;
        }

}

void Aspect_FontMapEntry::SetType (const Aspect_FontStyle &style) {

        MyTypeIsDef     = Standard_True;
        MyType          = style;

}

const Aspect_FontStyle& Aspect_FontMapEntry::Type () const {

        if ( !MyTypeIsDef || !MyIndexIsDef ) 
                Aspect_BadAccess::Raise("Unallocated FontMapEntry") ;

        return MyType;

}

void Aspect_FontMapEntry::SetIndex (const Standard_Integer index) {

        MyTypeIsDef     = Standard_True;
        MyIndex         = index;

}

Standard_Integer Aspect_FontMapEntry::Index () const {

        if ( !MyTypeIsDef || !MyIndexIsDef ) 
                Aspect_BadAccess::Raise("Unallocated FontMapEntry");

        return MyIndex;

}

Standard_Boolean Aspect_FontMapEntry::IsAllocated () const {

        return ( MyTypeIsDef && MyIndexIsDef ) ; 

}

void Aspect_FontMapEntry::Free () {

        MyTypeIsDef     = Standard_False;
        MyIndexIsDef    = Standard_False;

}

void Aspect_FontMapEntry::Dump () const
{
  Aspect_TypeOfFont style = MyType.Style();
  Standard_CString string = MyType.Value();
  Standard_Real size = MyType.Size();
  Standard_Real slant = MyType.Slant();
  Standard_Integer length = MyType.Length();
  Standard_Boolean capsheight = MyType.CapsHeight();

  cout << " Aspect_FontMapEntry::Dump ()\n";
  cout << "      MyTypeIsDef : " << (MyTypeIsDef ? "True\n" : "False\n");
  cout << "      MyIndexIsDef : " << (MyIndexIsDef ? "True\n" : "False\n");
  cout << "      FontStyle : " << (Standard_Integer) style << " Size : " << size
       << " Slant : " << slant << endl;
  cout << "      CapsHeight : " << (capsheight ? "True\n" : "False\n");
  cout << "      FontStyle length : " << length << "\n";
  if( length ) cout << "      FontString : " << string << "\n";
  MyType.Dump ();
  cout << " ------------------------------" << endl << flush;
}
