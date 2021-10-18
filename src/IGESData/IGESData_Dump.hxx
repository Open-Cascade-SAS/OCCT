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

#ifndef IGESData_Dump_HeaderFile
#define IGESData_Dump_HeaderFile

//		       --------------------------
//			    IGESData_Dump.hxx
//		       --------------------------
#include <gp_XY.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_XYZ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_GTrsf.hxx>
#include <Interface_MSG.hxx>

//  ###############################################################
//  Macros to help Dumping Parts of IGES Entities
//  (for useful and repetitive cases but which apply to different classes
//   but with similar signatures, such as Arrays)
//  Remember that the class IGESDumper processes itself individual dump of
//  IGESEntity

//  General Names are : IGESData_Dump***(S,arglist);  S being an output Stream

//  ---------------------------------------------------------------
//                          AVAILABLE MACROS

//  Dumping simple IGESEntity : see the class IGESDumper itself
//  Dumping a text as HAsciiString (either from PCollection or TCollection)
//  (manages an empty pointer) :
//  IGESData_DumpString(S,str)  displays   " "Content" " or "(undefined)"

//  Dumping Simple Data : Level must be managed by the caller
//  (general rule : Transformed Display to be used if Level > 5)

//  IGESData_DumpXY(S,XYval)               " (Xval,Yval)"         (no Transf)
//  IGESData_DumpXYT(S,XYVal,Trsf)         " (Xval,Yval)"         Z ignored
//  IGESData_DumpXYTZ(S,XYVal,Trsf,Z)      " (Xval,Yval,Zval)"    Z combined
//  IGESData_DumpXYZ(S,XYZval)             " (Xval,Yval,Zval)"    (no Transf)
//  IGESData_DumpXYZT(S,XYZval,Trsf)       " (Xval,Yval,Zval)"    (Transf)

//  Dumping Simple Data with Level : first displays Immediate Value, then
//  if Level > 5 and Transformation is not Identity, displays Transformed Value

//  IGESData_DumpXYL(S,Level,XYVal,Trsf)    " (Xval,Yval)  Transformed : (..)"
//  IGESData_DumpXYLZ(S,Level,XYVal,Trsf,Z) " (Xval,Yval,Zval)  Transformed :."
//  IGESData_DumpXYZL(S,Level,XYZval,Trsf)  " (Xval,Yval,Zval)  Transformed :."

//  Dumping Lists : general features
//      Lower and Upper are effective Values (immediate or given by functions).
//      Typically, give Lower = 1, Upper = ent->NbItems()
//      Item is the name of the access fonction (without its Index)
//      For Instance,   Item = compcurve->Curve  AND NOT  compcurve->Curve(..)
//      If Level is present, it commands more or less extensive display :
//        Level = 4, only limits are displayed
//  If it is a classic list, starting from 1 with a count (which can be 0),
//  displays "Count <upper> ..."  or "Empty". Else, display "(low - up) ..."
//        Level = 5, in addfition items are displayed shortly
//        (Entity Directory Numbers, XY/XYZ Coordinates)
//        Level > 5, in some cases, items are displayed with more details
//        (Entities with Type/Form, XY/XYZ with Transformed equivalents)

//  IGESData_DumpListVal(S,Lower,Upper,Item)   Item can be Real,Integer,
//                more generally, any type having operator << to Handle(Message_Messenger)
//  IGESData_DumpListXY(S,Lower,Upper,Item)    Item : XY without Transformation
//  IGESData_DumpListXYZ(S,Lower,Upper,Item)   Item : XYZ without Transf

//  IGESData_DumpVals(S,Level,Lower,Upper,Item)             Item : Real,Integer
//  IGESData_DumpListXYL(S,Level,Lower,Upper,Item,Trsf)     Item : XY
//  IGESData_DumpListXYLZ(S,Level,Lower,Upper,Item,Trsf,Z)  Item : XY. Z is a
//                Common Displacement
//  IGESData_DumpListXYZL(S,Level,Lower,Upper,Item,Trsf)    Item : XYZ

//  IGESData_DumpStrings(S,Level,Lower,Upper,Item)          Item : HAsciiString
//  IGESData_DumpEntities(S,Dumper,Level,Lower,Upper,Item)  Item : IGESEntity
//                Dumper is an IGESDumper which displays IGES Entities

//  Dumping Complex Arrays : only the most useful cases are taken into account
//  Doubles Arrays (Rectangles) and Single Arrays of Single Arrays (Jagged)

//  IGESData_DumpRectVals(S,Level,LowerRow,UpperRow,LowerCol,UpperCol,Item) 
//           LowerRow,LowerCol,UpperRow,UpperCol : effective values
//           Item : Real,Integer

//  ---------------------------------------------------------------

#define IGESData_DumpString(S,str) \
if (str.IsNull()) S << "(undefined)";\
  else {  S << '"' << str->String() << '"';  }

#define IGESData_DumpXY(S,XYval) \
 S << " (" << XYval.X() << "," << XYval.Y() << ")"

#define IGESData_DumpXYZ(S,XYZval) \
  S << " (" << XYZval.X() << "," << XYZval.Y() << "," << XYZval.Z() << ")"


#define IGESData_DumpXYT(S,XYval,Trsf) \
{\
  gp_XYZ XYZval(XYval.X(),XYval.Y(),0.);\
  Trsf.Transforms(XYZval);\
  IGESData_DumpXY(S,XYZval);\
}

#define IGESData_DumpXYTZ(S,XYval,Trsf,Z) \
{\
  gp_XYZ XYZval(XYval.X(),XYval.Y(),Z);\
  Trsf.Transforms(XYZval);\
  IGESData_DumpXYZ(S,XYZval);\
}

#define IGESData_DumpXYZT(S,XYZval,Trsf) \
{\
  gp_XYZ XYZTval(XYZval.X(),XYZval.Y(),XYZval.Z());\
  Trsf.Transforms(XYZTval);\
  IGESData_DumpXYZ(S,XYZTval);\
}


#define IGESData_DumpXYL(S,Level,XYval,Trsf) \
{\
  IGESData_DumpXY(S,XYval);\
  if (Level > 5 && Trsf.Form() != gp_Identity) {\
    S << "  Transformed :";\
    IGESData_DumpXYT(S,XYval,Trsf);\
  }\
}

#define IGESData_DumpXYLZ(S,Level,XYval,Trsf,Z) \
{\
  IGESData_DumpXY(S,XYval);\
  if (Level > 5 && Trsf.Form() != gp_Identity) {\
    S << "  Transformed :";\
    IGESData_DumpXYTZ(S,XYval,Trsf,Z);\
  }\
}

#define IGESData_DumpXYZL(S,Level,XYZval,Trsf) \
{\
  IGESData_DumpXYZ(S,XYZval);\
  if (Level > 5 && Trsf.Form() != gp_Identity) {\
    S << "  Transformed :";\
    IGESData_DumpXYZT(S,XYZval,Trsf);\
  }\
}


#define IGESData_DumpListHeader(S,lower,upper) \
{\
  if (lower > upper) S << " (Empty List)";\
  else if (lower == 1) S << " (Count : " << upper << ")";\
  else S << " (" << lower << " - " << upper << ")";\
}


#define IGESData_DumpListVal(S,lower,upper,item) \
{\
  Standard_Integer lo = lower;  Standard_Integer up = upper;\
  IGESData_DumpListHeader(S,lo,up);\
  S << " :";\
  for (Standard_Integer iopa = lo; iopa <= up; iopa ++)  S << " " << item(iopa);\
}

#define IGESData_DumpListXY(S,lower,upper,item) \
{\
  Standard_Integer lo = lower;  Standard_Integer up = upper;\
  IGESData_DumpListHeader(S,lo,up);\
  S << " :";\
  for (Standard_Integer iopa = lo; iopa <= up; iopa ++) IGESData_DumpXY(S,item(iopa));\
}

#define IGESData_DumpListXYZ(S,lower,upper,item) \
{\
  Standard_Integer lo = lower;  Standard_Integer up = upper;\
  IGESData_DumpListHeader(S,lo,up);\
  S << " :";\
  for (Standard_Integer iopa = lo; iopa <= up; iopa ++) IGESData_DumpXYZ(S,item(iopa));\
}


#define IGESData_DumpVals(S,Level,lower,upper,item) \
{\
  Standard_Integer lo = lower;  Standard_Integer up = upper;\
  IGESData_DumpListHeader(S,lo,up);\
  if (lo > up) {}\
  else if (Level == 4 || Level == -4) S <<" [content : ask level > 4]";\
  else if (Level > 0) {\
    S << " :";\
    for (Standard_Integer iopa = lo; iopa <= up; iopa ++)  S << " " << item(iopa);\
  }\
}

#define IGESData_DumpListXYL(S,Level,lower,upper,item,Trsf) \
{\
  Standard_Integer lo = lower;  Standard_Integer up = upper;\
  IGESData_DumpListHeader(S,lo,up);\
  if (lo > up) {}\
  else if (Level == 4 || Level == -4)\
    S <<" [content : ask level > 4, transformed : level > 5]";\
  else if (Level > 0) {\
    S << " :";\
    for (Standard_Integer iopa = lo; iopa <= up; iopa ++) IGESData_DumpXY(S,item(iopa));\
    if (Trsf.Form() != gp_Identity) {\
      S << "\n Transformed :";\
      if (Level == 5) S <<" [ask level > 5]";\
      else\
	for (Standard_Integer jopa = lo; jopa <= up; jopa ++)\
	  IGESData_DumpXYT(S,item(jopa),Trsf);\
    }\
  }\
}

#define IGESData_DumpListXYLZ(S,Level,lower,upper,item,Trsf,Z) \
{\
  Standard_Integer lo = lower;  Standard_Integer up = upper;\
  IGESData_DumpListHeader(S,lo,up);\
  if (lo > up) {}\
  else if (Level == 4 || Level == -4)\
    S <<" [content : ask level > 4, transformed : level > 5]";\
  else if (Level > 0) {\
    S << " :";\
    for (Standard_Integer iopa = lo; iopa <= up; iopa ++) IGESData_DumpXY(S,item(iopa));\
    if (Trsf.Form() != gp_Identity) {\
      S << "\n Transformed :";\
      if (Level == 5) S <<" [ask level > 5]";\
      else\
	for (Standard_Integer jopa = lo; jopa <= up; jopa ++)\
	  IGESData_DumpXYTZ(S,item(jopa),Trsf,Z);\
    }\
  }\
}


#define IGESData_DumpListXYZL(S,Level,lower,upper,item,Trsf) \
{\
  Standard_Integer lo = lower;  Standard_Integer up = upper;\
  IGESData_DumpListHeader(S,lo,up);\
  if (lo > up) {}\
  else if (Level == 4 || Level == -4)\
    S <<" [content : ask level > 4, transformed : level > 5]";\
  else if (Level > 0) {\
    S << " :";\
    for (Standard_Integer iopa = lo; iopa <= up; iopa ++) IGESData_DumpXYZ(S,item(iopa));\
    if (Trsf.Form() != gp_Identity) {\
      S << "\n Transformed :";\
      if (Level == 5) S <<" [ask level > 5]";\
      else\
	for (Standard_Integer jopa = lo; jopa <= up; jopa ++)\
	  IGESData_DumpXYZT(S,item(jopa),Trsf);\
    }\
  }\
}


#define IGESData_DumpStrings(S,Level,lower,upper,item) \
{\
  Standard_Integer lo = lower;  Standard_Integer up = upper;\
  IGESData_DumpListHeader(S,lo,up);\
  if (lo > up) {}\
  else if (Level == 4 || Level == -4) S <<" [content : ask level > 4]";\
  else if (Level > 0) {\
    S << " :";\
    for (Standard_Integer iopa = lo; iopa <= up; iopa ++)\
      {  S << "\n["<<Interface_MSG::Blanks(iopa,3)<<iopa<<"]:\"" << item(iopa)->String() << '"';  }\
    S << "\n";\
  }\
}

#define IGESData_DumpEntities(S,dumper,Level,lower,upper,item) \
{\
  Standard_Integer lo = lower;  Standard_Integer up = upper;\
  IGESData_DumpListHeader(S,lo,up);\
  if (lo > up) {}\
  else if (Level == 4 || Level == -4) S <<" [content : ask level > 4]";\
  else if (Level > 0) {\
    S << " :";\
    for (Standard_Integer iopa = lo; iopa <= up; iopa ++) {\
      if (Level == 5) {  S << " " ;           dumper.PrintDNum  (item(iopa),S); }\
      else            {  S << "\n["<<Interface_MSG::Blanks(iopa,3)<<iopa<<"]:"; dumper.PrintShort (item(iopa),S); }\
    }\
  }\
}


#define  IGESData_DumpRectVals(S,Level,LowCol,UpCol,LowRow,UpRow,Item)  \
{\
  int loco = LowCol; int upc = UpCol;  int lor = LowRow; int upr = UpRow;\
  S <<" (Row :"<< lor <<" - "<< upr <<" ; Col :"<< loco <<" - "<< upc <<")";\
  if (loco > upc || lor > upr) {}\
  else if (Level == 4 || Level == -4) S <<" [content : ask level > 4]";\
  else if (Level > 0) {\
    S << "\n";\
    for (int ir = lor; ir <= upr; ir ++) {\
      S << "Row "<<ir<<":[";\
      for (int ic = loco; ic <= upc; ic ++)  S << " " << Item(ic,ir);\
      S << " ]\n";\
    }\
  }\
}

#endif
