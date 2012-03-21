// Created on: 2001-01-29
// Created by: Alexander GRIGORIEV
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


#ifndef _Sequence_Declare_HeaderFile
#define _Sequence_Declare_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

//      Declaration of Sequence (numbered list) class.
//  Remarks on the current implementation:
//
// 1. Methods First() and Last() added
// 2. The method InsertAt(anIndex) replaces InsertBefore and InsertAfter.
//    This method never throws exception "OutOfRange". Its behaviour:
//              anIndex <= 1            => equivalent to Prepend()
//              anIndex >  Length()     => equivalent to Append()
//              else                    => equivalent to InsertBefore.

// *******************************************************************
// use the following somewhere in a header file;
//    ClassName - name of the list class to create
//    Type      - type of members of the list
// *******************************************************************

#define DECLARE_SEQUENCE( ClassName, Type )                                   \
                                                                              \
class ClassName {                                                             \
 public:                                                                      \
   inline                       ClassName       ();                           \
   inline                       ClassName       (const ClassName& anOther);   \
   inline ClassName&            operator=       (const ClassName& anOther);   \
   inline Standard_Integer      Length          () const;                     \
   inline const Type&           First           () const;                     \
   inline const Type&           Last            () const;                     \
   inline const Type&           Value           (const Standard_Integer)const;\
   inline Type&                 ChangeValue     (const Standard_Integer);     \
   inline const Type&           operator ()     (const Standard_Integer)const;\
   inline Type&                 operator ()     (const Standard_Integer);     \
                                                                              \
   Standard_EXPORT virtual      ~ClassName      ();                           \
   Standard_EXPORT void         Append          (const Type& aVal);           \
   Standard_EXPORT void         Prepend         (const Type& aVal);           \
   Standard_EXPORT void         InsertAt        (const Standard_Integer,      \
                                                 const Type& aVal);           \
   Standard_EXPORT void         Clear           ();                           \
   Standard_EXPORT void         Remove          (const Standard_Integer);     \
                                                                              \
 private:                                                                     \
   class Node {                                                               \
    private:                                                                  \
      Type              myValue;                                              \
      Node              * myPrev;                                             \
      Node              * myNext;                                             \
    public:                                                                   \
      Node              (const Type& aValue, Node * aPrv, Node * aNxt)        \
        : myValue (aValue), myPrev (aPrv), myNext (aNxt) {}                   \
      const Type& Value () const  { return myValue; }                         \
      Type& ChangeValue ()        { return myValue; }                         \
      friend class ClassName;                                                 \
   };                                                                         \
                                                                              \
   Standard_EXPORT const void * FindItem        (const Standard_Integer)const;\
   Standard_EXPORT void         Assign          (const ClassName& anOther);   \
                                                                              \
   Node                 * myFirst;                                            \
   Node                 * myLast;                                             \
   Node                 * myCurrent;                                          \
   Standard_Integer     myICur;                                               \
   Standard_Integer     myLength;                                             \
};                                                                            \
                                                                              \
inline ClassName::ClassName () :                                              \
       myFirst   (NULL),                                                      \
       myLast    (NULL),                                                      \
       myCurrent (NULL),                                                      \
       myICur    (0),                                                         \
       myLength  (0) {}                                                       \
                                                                              \
inline ClassName::ClassName (const ClassName& anOther) :                      \
       myFirst   (NULL)                                                       \
{                                                                             \
   Assign (anOther);                                                          \
}                                                                             \
                                                                              \
inline ClassName& ClassName::operator= (const ClassName& anOther)             \
{                                                                             \
   Assign (anOther);                                                          \
   return * this;                                                             \
}                                                                             \
                                                                              \
inline Standard_Integer ClassName::Length () const{                           \
   return myLength;                                                           \
}                                                                             \
                                                                              \
inline const Type& ClassName::First () const                                  \
{                                                                             \
   return myFirst -> Value (); /* exception if out of range */                \
}                                                                             \
                                                                              \
inline const Type& ClassName::Last () const                                   \
{                                                                             \
   return myLast -> Value(); /* exception if out of range */                  \
}                                                                             \
                                                                              \
inline const Type& ClassName::Value (const Standard_Integer anI) const        \
{                                                                             \
   const Node * anItem = (const Node *) FindItem (anI);                       \
   return anItem -> Value ();  /* exception if out of range */                \
}                                                                             \
                                                                              \
inline Type& ClassName::ChangeValue (const Standard_Integer anI)              \
{                                                                             \
   Node * anItem = (Node *) FindItem (anI);                                   \
   return anItem -> ChangeValue ();  /* exception if out of range */          \
}                                                                             \
                                                                              \
inline const Type& ClassName::operator() (const Standard_Integer anI) const   \
{                                                                             \
   return Value (anI);                                                        \
}                                                                             \
                                                                              \
inline Type& ClassName::operator() (const Standard_Integer anI)               \
{                                                                             \
   return ChangeValue (anI);                                                  \
}                                                                             \

// *******************************************************************
// use the following in a translation unit (*.cxx);
//
// *******************************************************************
#define IMPLEMENT_SEQUENCE( ClassName, Type )                                 \
const void * ClassName::FindItem (const Standard_Integer anI) const           \
{                                                                             \
   if (anI < 1 || anI > myLength) return NULL;                                \
   Standard_Integer aCounter;                                                 \
   Node * aCurrent = (Node *) myCurrent;                                      \
   Standard_Boolean aDir (Standard_False);                                    \
   if (aCurrent == NULL) {                                                    \
      aCurrent = myFirst;                                                     \
      aCounter = anI - 1;                                                     \
      aDir = Standard_True;                                                   \
   }else{                                                                     \
      aCounter = Abs (anI - myICur);                                          \
      if (anI <= aCounter) {                                                  \
         aCurrent = myFirst;                                                  \
         aCounter = anI - 1;                                                  \
         aDir = Standard_True;                                                \
      }else if (myLength - anI < aCounter) {                                  \
         aCurrent = myLast;                                                   \
         aCounter = myLength - anI;                                           \
      }else if (anI > myICur)                                                 \
        aDir = Standard_True;                                                 \
   }                                                                          \
   if (aDir)                                                                  \
     while (aCounter--) aCurrent = aCurrent -> myNext;                        \
   else                                                                       \
     while (aCounter--) aCurrent = aCurrent -> myPrev;                        \
   (Standard_Integer&) myICur = anI;                                          \
   (Node *&) myCurrent = aCurrent;                                            \
   return aCurrent;                                                           \
}                                                                             \
                                                                              \
ClassName::~ClassName ()                                                      \
{                                                                             \
   Clear ();                                                                  \
}                                                                             \
                                                                              \
void ClassName::Append (const Type& aVal)                                     \
{                                                                             \
   Node * anItem = new Node (aVal, myLast, NULL);                             \
   if (myLength == 0)                                                         \
     myFirst = anItem;                                                        \
   else                                                                       \
     myLast -> myNext = anItem;                                               \
   myLast = anItem;                                                           \
   myLength++;                                                                \
}                                                                             \
                                                                              \
void ClassName::Prepend (const Type& aVal)                                    \
{                                                                             \
   Node * anItem = new Node (aVal, NULL, myFirst);                            \
   if (myLength == 0)                                                         \
     myLast = anItem;                                                         \
   else                                                                       \
     myFirst -> myPrev = anItem;                                              \
   myFirst = anItem;                                                          \
   myLength++;                                                                \
   if (myICur > 0) myICur++;                                                  \
}                                                                             \
                                                                              \
void ClassName::InsertAt (const Standard_Integer anI, const Type& aVal)       \
{                                                                             \
   if (anI <= 1) Prepend (aVal);                                              \
   else if (anI > myLength) Append (aVal);                                    \
   else if (FindItem(anI)) {                                                  \
      Node * anItem = new Node (aVal, myCurrent -> myPrev, myCurrent);        \
      myCurrent -> myPrev = anItem;                                           \
      if (anItem -> myPrev) anItem -> myPrev -> myNext = anItem;              \
      myLength++;                                                             \
      myICur++;                                                               \
   } else ;                                                                   \
}                                                                             \
                                                                              \
void ClassName::Clear ()                                                      \
{                                                                             \
   while (myFirst) {                                                          \
      Node * aCurr = myFirst -> myNext;                                       \
      delete myFirst;                                                         \
      myFirst = aCurr;                                                        \
   }                                                                          \
   myFirst = myLast = myCurrent = NULL;                                       \
   myLength = 0;                                                              \
   myICur = 0;                                                                \
}                                                                             \
                                                                              \
void ClassName::Remove (const Standard_Integer anI)                           \
{                                                                             \
   Node * anItem = (Node *) FindItem (anI);                                   \
   if (anItem) {                                                              \
      if (myCurrent -> myPrev) {                                              \
         myCurrent -> myPrev -> myNext = myCurrent -> myNext;                 \
      }                                                                       \
      if (myCurrent -> myNext) {                                              \
         myCurrent -> myNext -> myPrev = myCurrent -> myPrev;                 \
         myCurrent = myCurrent -> myNext;                                     \
      }else{                                                                  \
         myCurrent = myCurrent -> myPrev;                                     \
         myICur--;                                                            \
      }                                                                       \
      if (myFirst == anItem) myFirst = myFirst -> myNext;                     \
      if (myLast == anItem)  myLast = myLast -> myPrev;                       \
      delete anItem;                                                          \
      myLength--;                                                             \
   }                                                                          \
}                                                                             \
                                                                              \
void ClassName::Assign (const ClassName& anOther)                             \
{                                                                             \
   Clear ();                                                                  \
   if (anOther.Length () == 0) return;                                        \
   myFirst = new Node (anOther.First(), NULL, NULL);                          \
   Node * aPrevious = myFirst;                                                \
   myLength = 1;                                                              \
   while (myLength < anOther.Length()) {                                      \
      myLength++;                                                             \
      Node * aCurrent = new Node (anOther.Value(myLength), aPrevious, NULL);  \
      aPrevious = aPrevious -> myNext = aCurrent;                             \
   }                                                                          \
   myLast = aPrevious;                                                        \
}                                                                             \

#endif
