// Created on: 1995-04-24
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _ChFiDS_FilSpine_HeaderFile
#define _ChFiDS_FilSpine_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColgp_SequenceOfXY.hxx>
#include <Law_Laws.hxx>
#include <ChFiDS_Spine.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class TopoDS_Edge;
class TopoDS_Vertex;
class gp_XY;
class Law_Function;
class Law_Composite;
class ChFiDS_HElSpine;


class ChFiDS_FilSpine;
DEFINE_STANDARD_HANDLE(ChFiDS_FilSpine, ChFiDS_Spine)

//! Provides  data specific to  the fillets -
//! vector or rule  of evolution (C2).
class ChFiDS_FilSpine : public ChFiDS_Spine
{

public:

  
  Standard_EXPORT ChFiDS_FilSpine();
  
  Standard_EXPORT ChFiDS_FilSpine(const Standard_Real Tol);
  
  Standard_EXPORT virtual void Reset (const Standard_Boolean AllData = Standard_False) Standard_OVERRIDE;
  
  //! initializes the constant vector on edge E.
  Standard_EXPORT void SetRadius (const Standard_Real Radius, const TopoDS_Edge& E);
  
  //! resets the constant vector  on   edge E.
  Standard_EXPORT void UnSetRadius (const TopoDS_Edge& E);
  
  //! initializes the  vector on Vertex V.
  Standard_EXPORT void SetRadius (const Standard_Real Radius, const TopoDS_Vertex& V);
  
  //! resets the vector on Vertex V.
  Standard_EXPORT void UnSetRadius (const TopoDS_Vertex& V);
  
  //! initializes the vector on the point of parameter W.
  Standard_EXPORT void SetRadius (const gp_XY& UandR, const Standard_Integer IinC);
  
  //! initializes the constant vector on all spine.
  Standard_EXPORT void SetRadius (const Standard_Real Radius);
  
  //! initializes the rule of evolution on all spine.
  Standard_EXPORT void SetRadius (const Handle(Law_Function)& C, const Standard_Integer IinC);
  
  //! returns true if the radius is constant
  //! all along the spine.
  Standard_EXPORT Standard_Boolean IsConstant() const;
  
  //! returns true if the radius is constant
  //! all along the edge E.
  Standard_EXPORT Standard_Boolean IsConstant (const Standard_Integer IE) const;
  
  //! returns the radius if the fillet is constant
  //! all along the spine.
  Standard_EXPORT Standard_Real Radius() const;
  
  //! returns the radius if the fillet is constant
  //! all along the edge E.
  Standard_EXPORT Standard_Real Radius (const Standard_Integer IE) const;
  
  //! returns the radius if the fillet is constant
  //! all along the edge E.
  Standard_EXPORT Standard_Real Radius (const TopoDS_Edge& E) const;
  
  Standard_EXPORT virtual void AppendElSpine (const Handle(ChFiDS_HElSpine)& Els) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Law_Composite) Law (const Handle(ChFiDS_HElSpine)& Els) const;
  
  //! returns the elementary law
  Standard_EXPORT Handle(Law_Function)& ChangeLaw (const TopoDS_Edge& E);
  
  //! returns the maximum radius if the fillet is non-constant
  Standard_EXPORT Standard_Real MaxRadFromSeqAndLaws() const;




  DEFINE_STANDARD_RTTIEXT(ChFiDS_FilSpine,ChFiDS_Spine)

protected:




private:

  
  Standard_EXPORT Handle(Law_Composite) ComputeLaw (const Handle(ChFiDS_HElSpine)& Els);
  
  Standard_EXPORT void AppendLaw (const Handle(ChFiDS_HElSpine)& Els);

  TColgp_SequenceOfXY parandrad;
  Law_Laws laws;


};







#endif // _ChFiDS_FilSpine_HeaderFile
