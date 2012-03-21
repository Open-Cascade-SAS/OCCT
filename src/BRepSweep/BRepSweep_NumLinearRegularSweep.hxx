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

#ifndef _BRepSweep_NumLinearRegularSweep_HeaderFile
#define _BRepSweep_NumLinearRegularSweep_HeaderFile

#ifndef _BRepSweep_Builder_HeaderFile
#include <BRepSweep_Builder.hxx>
#endif
#ifndef _TopoDS_Shape_HeaderFile
#include <TopoDS_Shape.hxx>
#endif
#ifndef _Sweep_NumShape_HeaderFile
#include <Sweep_NumShape.hxx>
#endif
#ifndef _BRepSweep_Tool_HeaderFile
#include <BRepSweep_Tool.hxx>
#endif
#ifndef _Sweep_NumShapeTool_HeaderFile
#include <Sweep_NumShapeTool.hxx>
#endif
#ifndef _BRepSweep_Array2OfShapesOfNumLinearRegularSweep_HeaderFile
#include <BRepSweep_Array2OfShapesOfNumLinearRegularSweep.hxx>
#endif
#ifndef _TColStd_Array2OfBoolean_HeaderFile
#include <TColStd_Array2OfBoolean.hxx>
#endif
#ifndef _Handle_BRepSweep_SequenceNodeOfSequenceOfShapesOfNumLinearRegularSweep_HeaderFile
#include <Handle_BRepSweep_SequenceNodeOfSequenceOfShapesOfNumLinearRegularSweep.hxx>
#endif
#ifndef _TopAbs_Orientation_HeaderFile
#include <TopAbs_Orientation.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
class Standard_NoMoreObject;
class Standard_NoSuchObject;
class Standard_RangeError;
class Standard_DomainError;
class TopoDS_Shape;
class Sweep_NumShape;
class BRepSweep_Builder;
class BRepSweep_Tool;
class Sweep_NumShapeTool;
class BRepSweep_Iterator;
class Sweep_NumShapeIterator;
class BRepSweep_Array2OfShapesOfNumLinearRegularSweep;
class BRepSweep_SequenceOfShapesOfNumLinearRegularSweep;
class BRepSweep_SequenceNodeOfSequenceOfShapesOfNumLinearRegularSweep;


#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_DefineAlloc_HeaderFile
#include <Standard_DefineAlloc.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

class BRepSweep_NumLinearRegularSweep  {

public:

  DEFINE_STANDARD_ALLOC

 // Methods PUBLIC
 // 
Standard_EXPORT virtual  void Delete() ;
Standard_EXPORT virtual ~BRepSweep_NumLinearRegularSweep()
{
  Delete();
}

Standard_EXPORT virtual  TopoDS_Shape MakeEmptyVertex(const TopoDS_Shape& aGenV,const Sweep_NumShape& aDirV)  = 0;
Standard_EXPORT virtual  TopoDS_Shape MakeEmptyDirectingEdge(const TopoDS_Shape& aGenV,const Sweep_NumShape& aDirE)  = 0;
Standard_EXPORT virtual  TopoDS_Shape MakeEmptyGeneratingEdge(const TopoDS_Shape& aGenE,const Sweep_NumShape& aDirV)  = 0;
Standard_EXPORT virtual  void SetParameters(const TopoDS_Shape& aNewFace,TopoDS_Shape& aNewVertex,const TopoDS_Shape& aGenF,const TopoDS_Shape& aGenV,const Sweep_NumShape& aDirV)  = 0;
Standard_EXPORT virtual  void SetDirectingParameter(const TopoDS_Shape& aNewEdge,TopoDS_Shape& aNewVertex,const TopoDS_Shape& aGenV,const Sweep_NumShape& aDirE,const Sweep_NumShape& aDirV)  = 0;
Standard_EXPORT virtual  void SetGeneratingParameter(const TopoDS_Shape& aNewEdge,TopoDS_Shape& aNewVertex,const TopoDS_Shape& aGenE,const TopoDS_Shape& aGenV,const Sweep_NumShape& aDirV)  = 0;
Standard_EXPORT virtual  TopoDS_Shape MakeEmptyFace(const TopoDS_Shape& aGenS,const Sweep_NumShape& aDirS)  = 0;
Standard_EXPORT virtual  void SetPCurve(const TopoDS_Shape& aNewFace,TopoDS_Shape& aNewEdge,const TopoDS_Shape& aGenF,const TopoDS_Shape& aGenE,const Sweep_NumShape& aDirV,const TopAbs_Orientation orien)  = 0;
Standard_EXPORT virtual  void SetGeneratingPCurve(const TopoDS_Shape& aNewFace,TopoDS_Shape& aNewEdge,const TopoDS_Shape& aGenE,const Sweep_NumShape& aDirE,const Sweep_NumShape& aDirV,const TopAbs_Orientation orien)  = 0;
Standard_EXPORT virtual  void SetDirectingPCurve(const TopoDS_Shape& aNewFace,TopoDS_Shape& aNewEdge,const TopoDS_Shape& aGenE,const TopoDS_Shape& aGenV,const Sweep_NumShape& aDirE,const TopAbs_Orientation orien)  = 0;
Standard_EXPORT virtual  TopAbs_Orientation DirectSolid(const TopoDS_Shape& aGenS,const Sweep_NumShape& aDirS)  = 0;
Standard_EXPORT virtual  Standard_Boolean GGDShapeIsToAdd(const TopoDS_Shape& aNewShape,const TopoDS_Shape& aNewSubShape,const TopoDS_Shape& aGenS,const TopoDS_Shape& aSubGenS,const Sweep_NumShape& aDirS) const = 0;
Standard_EXPORT virtual  Standard_Boolean GDDShapeIsToAdd(const TopoDS_Shape& aNewShape,const TopoDS_Shape& aNewSubShape,const TopoDS_Shape& aGenS,const Sweep_NumShape& aDirS,const Sweep_NumShape& aSubDirS) const = 0;
Standard_EXPORT virtual  Standard_Boolean SeparatedWires(const TopoDS_Shape& aNewShape,const TopoDS_Shape& aNewSubShape,const TopoDS_Shape& aGenS,const TopoDS_Shape& aSubGenS,const Sweep_NumShape& aDirS) const = 0;
Standard_EXPORT virtual  TopoDS_Shape SplitShell(const TopoDS_Shape& aNewShape) const;
Standard_EXPORT virtual  void SetContinuity(const TopoDS_Shape& aGenS,const Sweep_NumShape& aDirS)  = 0;
Standard_EXPORT virtual  Standard_Boolean HasShape(const TopoDS_Shape& aGenS,const Sweep_NumShape& aDirS) const = 0;
Standard_EXPORT   TopoDS_Shape Shape(const TopoDS_Shape& aGenS,const Sweep_NumShape& aDirS) ;
Standard_EXPORT   TopoDS_Shape Shape(const TopoDS_Shape& aGenS) ;
Standard_EXPORT   TopoDS_Shape Shape() ;
Standard_EXPORT   TopoDS_Shape FirstShape() ;
Standard_EXPORT   TopoDS_Shape LastShape() ;
Standard_EXPORT   TopoDS_Shape FirstShape(const TopoDS_Shape& aGenS) ;
Standard_EXPORT   TopoDS_Shape LastShape(const TopoDS_Shape& aGenS) ;
Standard_EXPORT   Standard_Boolean Closed() const;





protected:

 // Methods PROTECTED
 // 
Standard_EXPORT BRepSweep_NumLinearRegularSweep(const BRepSweep_Builder& aBuilder,const TopoDS_Shape& aGenShape,const Sweep_NumShape& aDirWire);


 // Fields PROTECTED
 //
BRepSweep_Builder myBuilder;
TopoDS_Shape myGenShape;
Sweep_NumShape myDirWire;
BRepSweep_Tool myGenShapeTool;
Sweep_NumShapeTool myDirShapeTool;
BRepSweep_Array2OfShapesOfNumLinearRegularSweep myShapes;
TColStd_Array2OfBoolean myBuiltShapes;


private: 

 // Methods PRIVATE
 // 


 // Fields PRIVATE
 //


};





// other Inline functions and methods (like "C++: function call" methods)
//


#endif
