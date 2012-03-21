// Created by: DAUTRY Philippe
// Copyright (c) 1998-1999 Matra Datavision
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

//      	-------------------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Feb 16 1998	Creation


#ifndef _TDF_Attribute_HeaderFile
#include <TDF_Attribute.hxx>
#endif
#ifndef _TDF_Label_HeaderFile
#include <TDF_Label.hxx>
#endif

#ifndef TDF_AttributeIterator_HeaderFile
#define TDF_AttributeIterator_HeaderFile

#ifndef _Handle_TDF_Attribute_HeaderFile
#include <Handle_TDF_Attribute.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
#ifndef _TDF_LabelNodePtr_HeaderFile
#include <TDF_LabelNodePtr.hxx>
#endif
class TDF_Attribute;
class TDF_Label;


#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

// This class provides a way to iterates on the
// up-to-date (current) valid attributes of a label.
//
// Even a Forgotten attribute may be found if this
// option is set. To use this possibility, look at
// the constructor.
// 
// Sorry: nobody needs this iterator but its friends!

class TDF_AttributeIterator  {

public:

  // Methods PUBLIC
  // 
  Standard_EXPORT TDF_AttributeIterator();
  Standard_EXPORT TDF_AttributeIterator
    (const TDF_Label& aLabel,
     const Standard_Boolean withoutForgotten = Standard_True);
  Standard_EXPORT TDF_AttributeIterator
    (const TDF_LabelNodePtr aLabelNode,
     const Standard_Boolean withoutForgotten = Standard_True);
  Standard_EXPORT   void Initialize
    (const TDF_Label& aLabel,
     const Standard_Boolean withoutForgotten = Standard_True) ;
  inline   Standard_Boolean More() const;
  Standard_EXPORT   void Next() ;
  inline   TDF_Attribute * Value() const;


protected:

 // Methods PROTECTED
 // 


 // Fields PROTECTED
 //


private: 

 // Methods PRIVATE
 // 
  void goToNext (const Handle(TDF_Attribute)& anAttr);


 // Fields PRIVATE
 //
  TDF_Attribute    * myValue;
  Standard_Boolean myWithoutForgotten;
};


// other inline functions and methods (like "C++: function call" methods)
//

inline Standard_Boolean TDF_AttributeIterator::More() const
{ return (myValue != 0L); }

inline TDF_Attribute * TDF_AttributeIterator::Value() const
{ return myValue; }

#endif
