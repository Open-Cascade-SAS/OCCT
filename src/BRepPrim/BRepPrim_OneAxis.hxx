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

#ifndef _BRepPrim_OneAxis_HeaderFile
#define _BRepPrim_OneAxis_HeaderFile

#ifndef _BRepPrim_Builder_HeaderFile
#include <BRepPrim_Builder.hxx>
#endif
#ifndef _gp_Ax2_HeaderFile
#include <gp_Ax2.hxx>
#endif
#ifndef _Standard_Real_HeaderFile
#include <Standard_Real.hxx>
#endif
#ifndef _TopoDS_Shell_HeaderFile
#include <TopoDS_Shell.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
#ifndef _TopoDS_Vertex_HeaderFile
#include <TopoDS_Vertex.hxx>
#endif
#ifndef _TopoDS_Edge_HeaderFile
#include <TopoDS_Edge.hxx>
#endif
#ifndef _TopoDS_Wire_HeaderFile
#include <TopoDS_Wire.hxx>
#endif
#ifndef _TopoDS_Face_HeaderFile
#include <TopoDS_Face.hxx>
#endif
class Standard_DomainError;
class Standard_OutOfRange;
class TopoDS_Shell;
class TopoDS_Face;
class TopoDS_Wire;
class TopoDS_Edge;
class TopoDS_Vertex;
class BRepPrim_Builder;
class gp_Ax2;
class gp_Pnt2d;


#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_DefineAlloc_HeaderFile
#include <Standard_DefineAlloc.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

class BRepPrim_OneAxis  {

public:

  DEFINE_STANDARD_ALLOC

 // Methods PUBLIC
 // 
Standard_EXPORT virtual  void Delete() ;
Standard_EXPORT virtual ~BRepPrim_OneAxis()
//Standard_EXPORT virtual ~()
{
  Delete();
}

Standard_EXPORT   void SetMeridianOffset(const Standard_Real MeridianOffset = 0) ;
Standard_EXPORT  const gp_Ax2& Axes() const;
Standard_EXPORT   void Axes(const gp_Ax2& A) ;
Standard_EXPORT   Standard_Real Angle() const;
Standard_EXPORT   void Angle(const Standard_Real A) ;
Standard_EXPORT   Standard_Real VMin() const;
Standard_EXPORT   void VMin(const Standard_Real V) ;
Standard_EXPORT   Standard_Real VMax() const;
Standard_EXPORT   void VMax(const Standard_Real V) ;
Standard_EXPORT virtual  TopoDS_Face MakeEmptyLateralFace() const = 0;
Standard_EXPORT virtual  TopoDS_Edge MakeEmptyMeridianEdge(const Standard_Real Ang) const = 0;
Standard_EXPORT virtual  void SetMeridianPCurve(TopoDS_Edge& E,const TopoDS_Face& F) const = 0;
Standard_EXPORT virtual  gp_Pnt2d MeridianValue(const Standard_Real V) const = 0;
Standard_EXPORT virtual  Standard_Boolean MeridianOnAxis(const Standard_Real V) const;
Standard_EXPORT virtual  Standard_Boolean MeridianClosed() const;
Standard_EXPORT virtual  Standard_Boolean VMaxInfinite() const;
Standard_EXPORT virtual  Standard_Boolean VMinInfinite() const;
Standard_EXPORT virtual  Standard_Boolean HasTop() const;
Standard_EXPORT virtual  Standard_Boolean HasBottom() const;
Standard_EXPORT virtual  Standard_Boolean HasSides() const;
Standard_EXPORT  const TopoDS_Shell& Shell() ;
Standard_EXPORT  const TopoDS_Face& LateralFace() ;
Standard_EXPORT  const TopoDS_Face& TopFace() ;
Standard_EXPORT  const TopoDS_Face& BottomFace() ;
Standard_EXPORT  const TopoDS_Face& StartFace() ;
Standard_EXPORT  const TopoDS_Face& EndFace() ;
Standard_EXPORT  const TopoDS_Wire& LateralWire() ;
Standard_EXPORT  const TopoDS_Wire& LateralStartWire() ;
Standard_EXPORT  const TopoDS_Wire& LateralEndWire() ;
Standard_EXPORT  const TopoDS_Wire& TopWire() ;
Standard_EXPORT  const TopoDS_Wire& BottomWire() ;
Standard_EXPORT  const TopoDS_Wire& StartWire() ;
Standard_EXPORT  const TopoDS_Wire& AxisStartWire() ;
Standard_EXPORT  const TopoDS_Wire& EndWire() ;
Standard_EXPORT  const TopoDS_Wire& AxisEndWire() ;
Standard_EXPORT  const TopoDS_Edge& AxisEdge() ;
Standard_EXPORT  const TopoDS_Edge& StartEdge() ;
Standard_EXPORT  const TopoDS_Edge& EndEdge() ;
Standard_EXPORT  const TopoDS_Edge& StartTopEdge() ;
Standard_EXPORT  const TopoDS_Edge& StartBottomEdge() ;
Standard_EXPORT  const TopoDS_Edge& EndTopEdge() ;
Standard_EXPORT  const TopoDS_Edge& EndBottomEdge() ;
Standard_EXPORT  const TopoDS_Edge& TopEdge() ;
Standard_EXPORT  const TopoDS_Edge& BottomEdge() ;
Standard_EXPORT  const TopoDS_Vertex& AxisTopVertex() ;
Standard_EXPORT  const TopoDS_Vertex& AxisBottomVertex() ;
Standard_EXPORT  const TopoDS_Vertex& TopStartVertex() ;
Standard_EXPORT  const TopoDS_Vertex& TopEndVertex() ;
Standard_EXPORT  const TopoDS_Vertex& BottomStartVertex() ;
Standard_EXPORT  const TopoDS_Vertex& BottomEndVertex() ;





protected:

 // Methods PROTECTED
 // 
Standard_EXPORT BRepPrim_OneAxis(const BRepPrim_Builder& B,const gp_Ax2& A,const Standard_Real VMin,const Standard_Real VMax);


 // Fields PROTECTED
 //
BRepPrim_Builder myBuilder;


private: 

 // Methods PRIVATE
 // 


 // Fields PRIVATE
 //
gp_Ax2 myAxes;
Standard_Real myAngle;
Standard_Real myVMin;
Standard_Real myVMax;
Standard_Real myMeridianOffset;
TopoDS_Shell myShell;
Standard_Boolean ShellBuilt;
TopoDS_Vertex myVertices[6];
Standard_Boolean VerticesBuilt[6];
TopoDS_Edge myEdges[9];
Standard_Boolean EdgesBuilt[9];
TopoDS_Wire myWires[9];
Standard_Boolean WiresBuilt[9];
TopoDS_Face myFaces[5];
Standard_Boolean FacesBuilt[5];


};





// other Inline functions and methods (like "C++: function call" methods)
//


#endif
