// Created on: 1994-03-22
// Created by: GUYOT and UNTEREINER
// Copyright (c) 1994-1999 Matra Datavision
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

// 21.12.98 rln, gka S4054
// 06.01.99 pdn  transmission from ShapeTool_MakeWire back to BRepAPI_MakeWire
// 19.03.99 abv  //:q5: code improvement, unnecessary includes removed
//S4181 pdn 15.04.99 Recognition of elementary surfaces as basic.

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IGESBasic_SingleParent.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESGeom_Boundary.hxx>
#include <IGESGeom_BoundedSurface.hxx>
#include <IGESGeom_BSplineCurve.hxx>
#include <IGESGeom_BSplineSurface.hxx>
#include <IGESGeom_CircularArc.hxx>
#include <IGESGeom_CompositeCurve.hxx>
#include <IGESGeom_ConicArc.hxx>
#include <IGESGeom_CopiousData.hxx>
#include <IGESGeom_CurveOnSurface.hxx>
#include <IGESGeom_Line.hxx>
#include <IGESGeom_OffsetCurve.hxx>
#include <IGESGeom_OffsetSurface.hxx>
#include <IGESGeom_Plane.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESGeom_RuledSurface.hxx>
#include <IGESGeom_SplineCurve.hxx>
#include <IGESGeom_SplineSurface.hxx>
#include <IGESGeom_SurfaceOfRevolution.hxx>
#include <IGESGeom_TabulatedCylinder.hxx>
#include <IGESGeom_TrimmedSurface.hxx>
#include <IGESSolid_ConicalSurface.hxx>
#include <IGESSolid_CylindricalSurface.hxx>
#include <IGESSolid_EdgeList.hxx>
#include <IGESSolid_Face.hxx>
#include <IGESSolid_Loop.hxx>
#include <IGESSolid_ManifoldSolid.hxx>
#include <IGESSolid_PlaneSurface.hxx>
#include <IGESSolid_Shell.hxx>
#include <IGESSolid_SphericalSurface.hxx>
#include <IGESSolid_ToroidalSurface.hxx>
#include <IGESSolid_VertexList.hxx>
#include <IGESToBRep.hxx>
#include <IGESToBRep_AlgoContainer.hxx>
#include <Interface_Macros.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Stream.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <XSAlgo.hxx>

static Handle(IGESToBRep_AlgoContainer) theContainer; 
 
//=======================================================================                                                
//function : Init                                                                                                        
//purpose  :                                                                                                             
//=======================================================================                                                
                                                                                                                         
 void IGESToBRep::Init()                                                                                                   
{                                                                                                                        
  static Standard_Boolean init = Standard_False;                                                                         
  if (init) return;                                                                                                      
  init = Standard_True;                                                                                                  
  XSAlgo::Init();                                                                                                        
  theContainer = new IGESToBRep_AlgoContainer;                                                                             
}     
   
//=======================================================================                                                
//function : SetAlgoContainer                                                                                            
//purpose  :                                                                                                             
//=======================================================================                                                
                                                                                                                         
 void IGESToBRep::SetAlgoContainer(const Handle(IGESToBRep_AlgoContainer)& aContainer)                                       
{
  theContainer = aContainer;                                                                                             
}                                                                                                                        
                                                                                                                         
//=======================================================================                                                
//function : AlgoContainer                                                                                               
//purpose  :                                                                                                             
//=======================================================================                                                
                                                                                                                         
 Handle(IGESToBRep_AlgoContainer) IGESToBRep::AlgoContainer()                                                                
{                                                                                                                        
  return theContainer;                                                                                                   
}    

//=======================================================================
//function : IsCurveAndSurface
//purpose  : Return True if the IgesEntity can be transfered
//           by TransferCurveAndSurface
//=======================================================================
Standard_Boolean IGESToBRep::IsCurveAndSurface(const Handle(IGESData_IGESEntity)& start)
{
  //S4054
  if (start.IsNull())              return Standard_False;
  if (IsTopoCurve(start))          return Standard_True;
  if (IsTopoSurface(start))        return Standard_True;
  if (IsBRepEntity(start))         return Standard_True;
  return Standard_False;
}


//=======================================================================
//function : IsBasicCurve
//purpose  : Return True if the IgesEntity can be transfered
//           by TransferBasicCurve
//=======================================================================
Standard_Boolean IGESToBRep::IsBasicCurve(const Handle(IGESData_IGESEntity)& start)
{
  //S4054
  if (start.IsNull())                                      return Standard_False;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_BSplineCurve))) return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_Line)))         return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_CircularArc)))  return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_ConicArc)))     return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_CopiousData)))  return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_SplineCurve)))  return Standard_True;
  return Standard_False;
}


//=======================================================================
//function : IsBasicSurface
//purpose  : Return True if the IgesEntity can be transfered
//           by TransferBasicSurface
//=======================================================================
Standard_Boolean IGESToBRep::IsBasicSurface(const Handle(IGESData_IGESEntity)& start)
{
  //S4054
  if (start.IsNull())                                        return Standard_False;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_BSplineSurface))) return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_SplineSurface)))  return Standard_True;
  //S4181 pdn 15.04.99 added to basic surfaces
  if (start->IsKind(STANDARD_TYPE(IGESSolid_PlaneSurface)))       return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESSolid_CylindricalSurface))) return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESSolid_ConicalSurface)))     return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESSolid_SphericalSurface)))   return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESSolid_ToroidalSurface)))   return Standard_True;
  
  return Standard_False;
}


//=======================================================================
//function : IsTopoCurve
//purpose  : Return True if the IgesEntity can be transfered
//           by TransferTopoCurve
//=======================================================================
Standard_Boolean IGESToBRep::IsTopoCurve(const Handle(IGESData_IGESEntity)& start)
{
  //S4054
  if (start.IsNull())                                        return Standard_False;
  if (IsBasicCurve(start))                                   return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_CompositeCurve))) return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_CurveOnSurface))) return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_Boundary)))       return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_Point)))          return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_OffsetCurve)))    return Standard_True;
  return Standard_False;
}


//=======================================================================
//function : IsTopoSurface
//purpose  : Return True if the IgesEntity can be transfered
//           by TransferTopoSurface
//=======================================================================
Standard_Boolean IGESToBRep::IsTopoSurface(const Handle(IGESData_IGESEntity)& start)
{
  //S4054
  if (start.IsNull())                                             return Standard_False;
  if (IsBasicSurface(start))                                      return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_TrimmedSurface)))      return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_SurfaceOfRevolution))) return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_TabulatedCylinder)))   return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_RuledSurface)))        return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_Plane)))               return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_BoundedSurface)))      return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESGeom_OffsetSurface)))       return Standard_True;
  //S4181 pdn 15.04.99 removing to basic surface
  //if (start->IsKind(STANDARD_TYPE(IGESSolid_PlaneSurface)))       return Standard_True;
//  SingleParent, cas particulier (Face Trouee : ne contient que des PLANE)
  if (start->IsKind(STANDARD_TYPE(IGESBasic_SingleParent))) {
    DeclareAndCast(IGESBasic_SingleParent,sp,start);
    if (!sp->SingleParent()->IsKind(STANDARD_TYPE(IGESGeom_Plane))) return Standard_False;
    Standard_Integer nb = sp->NbChildren();
    for (Standard_Integer i = 1; i <= nb; i ++) {
      if (!sp->Child(i)->IsKind(STANDARD_TYPE(IGESGeom_Plane))) return Standard_False;
    }
    return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : IsBRepEntity
//purpose  : Return True if the IgesEntity can be transfered
//           by TransferBRepEntity
//=======================================================================
Standard_Boolean IGESToBRep::IsBRepEntity(const Handle(IGESData_IGESEntity)& start)
{
  //S4054
  if (start.IsNull())                                         return Standard_False;
  if (start->IsKind(STANDARD_TYPE(IGESSolid_Face)))           return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESSolid_Shell)))          return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESSolid_ManifoldSolid)))  return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESSolid_VertexList)))     return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESSolid_EdgeList)))       return Standard_True;
  if (start->IsKind(STANDARD_TYPE(IGESSolid_Loop)))           return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : IGESCurveToSequenceOfIGESCurve
//purpose  : Creates a sequence of IGES curves from IGES curve:
//           - if curve is CompositeCurve its components are recursively added,
//           - if curve is ordinary IGES curve it is simply added
//           - otherwise (Null or not curve) it is ignored
//remark   : if sequence is Null it is created, otherwise it is appended
//returns  : number of curves in sequence
//example  : (A B (C (D ( E F) G) H)) -> (A B C D E F G H)
//=======================================================================

 Standard_Integer IGESToBRep::IGESCurveToSequenceOfIGESCurve(const Handle(IGESData_IGESEntity)& curve,
							     Handle(TColStd_HSequenceOfTransient)& sequence) 
{
  if (sequence.IsNull()) sequence = new TColStd_HSequenceOfTransient;
  if (!curve.IsNull()) {
    if (curve->IsKind (STANDARD_TYPE (IGESGeom_CompositeCurve))) {
      Handle(IGESGeom_CompositeCurve) comp = Handle(IGESGeom_CompositeCurve)::DownCast (curve);
      for (Standard_Integer i = 1; i <= comp->NbCurves(); i++) {
	Handle(TColStd_HSequenceOfTransient) tmpsequence;
	IGESCurveToSequenceOfIGESCurve (comp->Curve (i), tmpsequence);
	sequence->Append (tmpsequence);
      }
    }
    else if (IGESToBRep::IsTopoCurve (curve) &&
	     ! curve->IsKind (STANDARD_TYPE (IGESGeom_Point)))
      sequence->Append (curve);
  }
  return sequence->Length();
}

//=======================================================================
//function : TransferPCurve
//purpose  : Copies pcurve on face <face> from <fromedge> to <toedge>
//           If <toedge> already has pcurve on that <face>, <toedge> becomes
//           a seam-edge; if both pcurves are not SameRange, the SameRange is
//           called. Returns False if pcurves are not made SameRange
//           Making <toedge> SameParameter should be done outside the method (???)
//=======================================================================

 Standard_Boolean IGESToBRep::TransferPCurve (const TopoDS_Edge& fromedge, const TopoDS_Edge& toedge, const TopoDS_Face& face)
{
  Standard_Boolean result = Standard_True;
  Standard_Real olda, oldb, a, b;
  Handle(Geom2d_Curve) oldpcurve = BRep_Tool::CurveOnSurface (toedge,   face, olda, oldb),
                          pcurve = BRep_Tool::CurveOnSurface (fromedge, face, a,    b   );
  BRep_Builder B;
  if (!oldpcurve.IsNull()) {
    if (olda != a || oldb != b) {
      try {
        OCC_CATCH_SIGNALS
	Handle(Geom2d_Curve) newpcurve;
	GeomLib::SameRange (Precision::PConfusion(), oldpcurve, olda, oldb, a, b, newpcurve);
	if (!newpcurve.IsNull()) {
	  olda = a; oldb = b; oldpcurve = newpcurve;
	}
	else {
#ifdef OCCT_DEBUG
	  cout << "Warning: IGESToBRep::TransferPCurve: pcurves are not SameRange" << endl;
#endif
	  result = Standard_False;
	}
      }
      catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
	cout << "\n**IGESToBRep::TransferPCurve: Exception in SameRange : "; 
	anException.Print(cout);
#endif
	(void)anException;
	result = Standard_False;
      }
    }
    if (toedge.Orientation() == TopAbs_FORWARD)
      B.UpdateEdge (toedge,
		    Handle(Geom2d_Curve)::DownCast (pcurve->Copy()), 
		    Handle(Geom2d_Curve)::DownCast (oldpcurve->Copy()), face, 0);
    else
      B.UpdateEdge (toedge,
		    Handle(Geom2d_Curve)::DownCast (oldpcurve->Copy()), 
		    Handle(Geom2d_Curve)::DownCast (pcurve->Copy()), face, 0);
  }
  else {
    olda = a; oldb = b;
    B.UpdateEdge (toedge, Handle(Geom2d_Curve)::DownCast (pcurve->Copy()), face, 0);
  }
  B.Range (toedge, face, a, b);
  Standard_Real first, last;
  if (!BRep_Tool::Curve (toedge, first, last).IsNull() && (first != a || last != b))
    B.SameRange (toedge, Standard_False);
  else
    B.SameRange (toedge, Standard_True);
  return result;
}
