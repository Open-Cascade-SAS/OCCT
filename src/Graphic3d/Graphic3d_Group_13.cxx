// Created on: 2000-06-16
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <Graphic3d_Group.jxx>
#include <Graphic3d_Group.pxx>

#include <Graphic3d_ArrayOfPoints.hxx>
#include <gp_Pnt.hxx>

// =======================================================================
// function : AddPrimitiveArray
// purpose  :
// =======================================================================

void Graphic3d_Group :: AddPrimitiveArray ( const Handle(Graphic3d_ArrayOfPrimitives)& elem,const Standard_Boolean EvalMinMax )  
{
	if (IsDeleted () ) return;

        if( !elem->IsValid() ) return;

	if( !MyContainsFacet && 
		(elem->Type() != Graphic3d_TOPA_POLYLINES) &&
		(elem->Type() != Graphic3d_TOPA_SEGMENTS) &&
		(elem->Type() != Graphic3d_TOPA_POINTS) ) {
	  MyStructure->GroupsWithFacet (+1);
	  MyContainsFacet = Standard_True;
	}

	MyIsEmpty	= Standard_False;

	MyListOfPArray.Append(elem);

	// Min-Max Update
	if (EvalMinMax) {
	  Standard_Real x,y,z;

	  for (int k=1; k<=elem->VertexNumber() ; k++) {
	    elem->Vertice(k,x,y,z);

	    if (x < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal(x);
	    if (y < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal(y);
	    if (z < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal(z);
	    if (x > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal(x);
	    if (y > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal(y);
	    if (z > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal(z);
	  }
	}

	MyGraphicDriver->PrimitiveArray(MyCGroup, elem->Array());

	Update ();
}

// =======================================================================
// function : Marker
// purpose  :
// =======================================================================

void Graphic3d_Group::Marker (const Graphic3d_Vertex& thePoint,
                              const Standard_Boolean  theToEvalMinMax)
{
  Handle(Graphic3d_ArrayOfPoints) aPoints = new Graphic3d_ArrayOfPoints (1);
  aPoints->AddVertex (thePoint.X(), thePoint.Y(), thePoint.Z());
  AddPrimitiveArray (aPoints, theToEvalMinMax);
}

// =======================================================================
// function : UserDraw
// purpose  :
// =======================================================================

void Graphic3d_Group :: UserDraw ( const Standard_Address AnObject,				   
				   const Standard_Boolean EvalMinMax,
				   const Standard_Boolean ContainsFacet )
{
	if (IsDeleted ()) return;

	// Without this modification, the group assumes the primitive contains
	// no polygons and does not require the Z-buffer for display.
	if (!MyContainsFacet && ContainsFacet) {
	  MyStructure->GroupsWithFacet (+1);
	  MyContainsFacet = Standard_True;
	}

	MyIsEmpty	= Standard_False;

        Graphic3d_CUserDraw AUserDraw;
        AUserDraw.Data = AnObject;
        AUserDraw.Bounds = ( EvalMinMax? &MyBounds : NULL );
	MyGraphicDriver->UserDraw (MyCGroup, AUserDraw);

	Update ();
}

// =======================================================================
// function : SetFlippingOptions
// purpose  :
// =======================================================================

void Graphic3d_Group::SetFlippingOptions (const Standard_Boolean theIsEnabled,
                                          const gp_Ax2&          theRefPlane)
{
  MyGraphicDriver->SetFlippingOptions (MyCGroup, theIsEnabled, theRefPlane);
}

// =======================================================================
// function : SetStencilTestOptions
// purpose  :
// =======================================================================
void Graphic3d_Group::SetStencilTestOptions (const Standard_Boolean theIsEnabled)
{
  MyGraphicDriver->SetStencilTestOptions (MyCGroup, theIsEnabled);
}
