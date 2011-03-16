// ColoredShape.cpp: implementation of the CColoredShape class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ColoredShape.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CColoredShape::CColoredShape()
{
	m_colorName = Quantity_NOC_RED;
}


CColoredShape::CColoredShape(const Quantity_NameOfColor aColor, const TopoDS_Shape& aShape)
{
	m_colorName   = aColor;
	m_shapeObject = aShape;
}

IMPLEMENT_SERIAL(CColoredShape, CObject,1);

// This schema contains all the Persistent Geometry and Topology
#include <ShapeSchema.hxx>  

// Tools to store TopoDS_Shape
#include <MgtBRep.hxx>
#include <PTopoDS_HShape.hxx>
#include <PTColStd_TransientPersistentMap.hxx>
#include <TopoDS_Shape.hxx>

// Tools to put Persistent Object in an archive
#include <FSD_Archive.hxx>
#include <Storage_Data.hxx>
#include <Storage_HSeqOfRoot.hxx>
#include <Storage_Root.hxx>
#include <PTColStd_PersistentTransientMap.hxx>

void CColoredShape::Serialize(CArchive & ar)
{
	CObject::Serialize(ar);

	// an I/O driver
	FSD_Archive f( &ar );

	// the applicative Schema containing Persistent Topology and Geometry
	// Note that it inherits from the class Storage_Schema
	Handle(ShapeSchema) s = new ShapeSchema;

	if ( ar.IsStoring() ) 
	{   
		// Store the color in the archive
		ar << m_colorName;

		//Create the persistent Shape
		PTColStd_TransientPersistentMap aMap;

		Handle(PTopoDS_HShape) aPShape = 
			MgtBRep::Translate(m_shapeObject, aMap, MgtBRep_WithoutTriangle);

		// Store the Persistent shape in the archive
		Handle(Storage_Data) d = new Storage_Data;
		d->AddRoot("ObjectName", aPShape);
		s->Write( f, d);

		// Check
		if (d->ErrorStatus() != Storage_VSOk) 
		{
			::MessageBox(NULL, " Error while writing... ", " Error ",MB_OK) ;
		}
		
	}
	else
	{
		// Read the Color from the archive
		Standard_Integer tmp;

		ar >> tmp;
		m_colorName = (Quantity_NameOfColor) tmp;

		// Read the Persistent Shape from the archive
		Handle(Storage_Data) d = s->Read( f );
		Handle(Storage_HSeqOfRoot)  roots = d->Roots();
		Handle(Standard_Persistent) p;
		Handle(Storage_Root) r;
		Handle(PTopoDS_HShape) aPShape;

		r = roots->Value(1);
		p = r->Object();
		aPShape  = Handle(PTopoDS_HShape)::DownCast(p);

		// Create the shape
		PTColStd_PersistentTransientMap aMap;

		MgtBRep::Translate(aPShape,aMap,m_shapeObject,MgtBRep_WithoutTriangle);
	}
}

void CColoredShape::Display(Handle(AIS_InteractiveContext)& anAIScontext)
{
	Handle(AIS_Shape) ais = new AIS_Shape(m_shapeObject);

	anAIScontext->SetColor(ais, m_colorName);
	anAIScontext->Display(ais, Standard_False);

}
