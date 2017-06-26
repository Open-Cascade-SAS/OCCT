// Created by: DAUTRY Philippe
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

//      	-------------------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Feb 16 1998	Creation

#ifndef TDF_AttributeIterator_HeaderFile
#define TDF_AttributeIterator_HeaderFile

#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelNodePtr.hxx>

// This class provides a way to iterates on the
// up-to-date (current) valid attributes of a label.
//
// This class should not be used for standard attributes workflow.
// Application allways knows what to search at particular label.
// So, use IsAttribute and FindAttibute methods of
// TDF_Label to check and get attributes in usual way.
// This class may be used for processing of content of labels
// in generic way, like copy-tool, specific save/restore algorithms,
// etc.
//
// Even a forgotten attribute may be found if this
// option is set. To use this possibility, look at
// the constructor.

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
  inline   Handle(TDF_Attribute) Value() const;


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

inline Handle(TDF_Attribute) TDF_AttributeIterator::Value() const
{ return myValue; }

#endif
