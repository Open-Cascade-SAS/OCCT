// File:	TDF_AttributeIterator.hxx
//      	-------------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1998

// Version:	0.0
// History:	Version	Date		Purpose
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
