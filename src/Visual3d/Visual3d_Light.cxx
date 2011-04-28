/***********************************************************************

     FUNCTION :
     ----------
        File Visual3d_Light :

	Declaration of variables specific to light sources

     NOTES:
     ----------

	A light source is defined by :
	- type
	- color
	- reduction factors  ( for positional and spot only )
	- its angle ( for spot only )
	- its concentration ( for spot only )
	- its direction ( directional and spot only )
	- its position ( positional and spot only )

	It is active in a view, in the associated context.

     ATTENTION:
     ----------

	- AngleCone is given in radian [Pex] while OpenGl works in 
	degreees. The limits for Pex are [0,PI] while for OpenGl this is [0,90].
	- Two reduction factors are used with Pex while OpenGl uses three.
	- The ereduction factors have range [0.0,1.0] for Pex and 
	for OpenGl the range is [0.0,n] 
	- The concentration varies from [0.,1.0] for Pex to [0,128] for OpenGl.	

************************************************************************/

/*----------------------------------------------------------------------*/
/*
 * Includes
 */ 

#include <Visual3d_Light.ixx>

#include <Graphic3d_GraphicDriver.hxx>

/*----------------------------------------------------------------------*/

Visual3d_Light::Visual3d_Light ():
MyType (Visual3d_TOLS_AMBIENT) {

	MyCLight.WsId		= -1;
	MyCLight.ViewId		= 0; /* not used */
	MyCLight.LightType	= int (MyType);
	MyCLight.Headlight      = 0;

Quantity_Color White (Quantity_NOC_WHITE);

	MyCLight.Color.r	= float (White.Red ());
	MyCLight.Color.g	= float (White.Green ());
	MyCLight.Color.b	= float (White.Blue ());

	MyCLight.LightId	=
		Graphic3d_GraphicDriver::Light (MyCLight, Standard_False);

}

/*----------------------------------------------------------------------*/

Visual3d_Light::Visual3d_Light (const Quantity_Color& Color):
MyType (Visual3d_TOLS_AMBIENT) {

	MyCLight.WsId		= -1;
	MyCLight.ViewId		= 0; /* not used */
	MyCLight.LightType	= int (MyType);
	MyCLight.Headlight      = 0;

	MyCLight.Color.r	= float (Color.Red ());
	MyCLight.Color.g	= float (Color.Green ());
	MyCLight.Color.b	= float (Color.Blue ());

	MyCLight.LightId	=
		Graphic3d_GraphicDriver::Light (MyCLight, Standard_False);

}

/*----------------------------------------------------------------------*/

Visual3d_Light::Visual3d_Light(const Quantity_Color& Color,const Graphic3d_Vector& Direction,const Standard_Boolean Headlight): MyType (Visual3d_TOLS_DIRECTIONAL) {

	if (Direction.LengthZero ())
		Visual3d_LightDefinitionError::Raise
			("Bad value for LightDirection");

	MyCLight.WsId		= -1;
	MyCLight.ViewId		= 0; /* not used */
	MyCLight.LightType	= int (MyType);
	MyCLight.Headlight      = Headlight? 1:0;

Standard_Real Norme, X, Y, Z;

	Color.Values (X, Y, Z, Quantity_TOC_RGB);
	MyCLight.Color.r	= float (X);
	MyCLight.Color.g	= float (Y);
	MyCLight.Color.b	= float (Z);

	Direction.Coord (X, Y, Z);
	Norme	= Sqrt (X*X+Y*Y+Z*Z);
	// Direction.LengthZero () == Standard_False
	MyCLight.Direction.x	= float (X/Norme);
	MyCLight.Direction.y	= float (Y/Norme);
	MyCLight.Direction.z	= float (Z/Norme);

	MyCLight.LightId	=
		Graphic3d_GraphicDriver::Light (MyCLight, Standard_False);

}

/*----------------------------------------------------------------------*/

Visual3d_Light::Visual3d_Light (const Quantity_Color& Color, const Graphic3d_Vertex& Position, const Standard_Real Fact1, const Standard_Real Fact2):
MyType (Visual3d_TOLS_POSITIONAL) {

	if ( (Fact1 == 0.0) && (Fact2 == 0.0) )
		Visual3d_LightDefinitionError::Raise
			("Bad value for LightAttenuation");

	if ( (Fact1 < 0.0) && (Fact1 > 1.0) )
		Visual3d_LightDefinitionError::Raise
			("Bad value for LightAttenuation");

	if ( (Fact2 < 0.0) && (Fact2 > 1.0) )
		Visual3d_LightDefinitionError::Raise
			("Bad value for LightAttenuation");

	MyCLight.WsId		= -1;
	MyCLight.ViewId		= 0; /* not used */
	MyCLight.LightType	= int (MyType);
	MyCLight.Headlight      = 0;

	MyCLight.Color.r	= float (Color.Red ());
	MyCLight.Color.g	= float (Color.Green ());
	MyCLight.Color.b	= float (Color.Blue ());

	MyCLight.Position.x	= float (Position.X ());
	MyCLight.Position.y	= float (Position.Y ());
	MyCLight.Position.z	= float (Position.Z ());

	MyCLight.Attenuation[0] = float (Fact1);
	MyCLight.Attenuation[1] = float (Fact2);

	MyCLight.LightId	=
		Graphic3d_GraphicDriver::Light (MyCLight, Standard_False);

}

/*----------------------------------------------------------------------*/

Visual3d_Light::Visual3d_Light (const Quantity_Color& Color, const Graphic3d_Vertex& Position, const Graphic3d_Vector& Direction, const Standard_Real Concentration, const Standard_Real Fact1, const Standard_Real Fact2, const Standard_Real AngleCone):
MyType (Visual3d_TOLS_SPOT) {

	if (Direction.LengthZero ())
		Visual3d_LightDefinitionError::Raise
			("Bad value for LightDirection");

	if ( (Concentration < 0.0) || (Concentration > 1.0) )
		Visual3d_LightDefinitionError::Raise
			("Bad value for LightConcentration");

	if ( (Fact1 == 0.0) && (Fact2 == 0.0) )
		Visual3d_LightDefinitionError::Raise
			("Bad value for LightAttenuation");

	if ( (Fact1 < 0.0) && (Fact1 > 1.0) )
		Visual3d_LightDefinitionError::Raise
			("Bad value for LightAttenuation");

	if ( (Fact2 < 0.0) && (Fact2 > 1.0) )
		Visual3d_LightDefinitionError::Raise
			("Bad value for LightAttenuation");

	if (Visual3d_Light::IsValid (AngleCone)) {

		MyCLight.WsId		= -1;
		MyCLight.ViewId		= 0; /* not used */
		MyCLight.LightType	= int (MyType);
		MyCLight.Headlight      = 0;

Standard_Real X, Y, Z;

		Color.Values (X, Y, Z, Quantity_TOC_RGB);
		MyCLight.Color.r	= float (X);
		MyCLight.Color.g	= float (Y);
		MyCLight.Color.b	= float (Z);

		Position.Coord (X, Y, Z);
		MyCLight.Position.x	= float (X);
		MyCLight.Position.y	= float (Y);
		MyCLight.Position.z	= float (Z);

		Direction.Coord (X, Y, Z);
		MyCLight.Direction.x	= float (X);
		MyCLight.Direction.y	= float (Y);
		MyCLight.Direction.z	= float (Z);

		MyCLight.Concentration	= float (Concentration);

		MyCLight.Attenuation[0] = float (Fact1);
		MyCLight.Attenuation[1] = float (Fact2);

		MyCLight.Angle		= float (AngleCone);

		MyCLight.LightId	=
		  Graphic3d_GraphicDriver::Light (MyCLight, Standard_False);

	}
	else
		Visual3d_LightDefinitionError::Raise
			("Bad value for LightAngle");

}

/*----------------------------------------------------------------------*/

Quantity_Color Visual3d_Light::Color () const {

Quantity_Color AColor  (Standard_Real (MyCLight.Color.r),
			Standard_Real (MyCLight.Color.g),
			Standard_Real (MyCLight.Color.b),
			Quantity_TOC_RGB);

	return (AColor);

}

/*----------------------------------------------------------------------*/

Visual3d_TypeOfLightSource Visual3d_Light::LightType () const {

	return (MyType);

}

/*----------------------------------------------------------------------*/
Standard_Boolean Visual3d_Light::Headlight () const {
  return MyCLight.Headlight==0? Standard_False:Standard_True;
}

/*----------------------------------------------------------------------*/

void Visual3d_Light::Values (Quantity_Color& Color) const {

Quantity_Color AColor  (Standard_Real (MyCLight.Color.r),
			Standard_Real (MyCLight.Color.g),
			Standard_Real (MyCLight.Color.b),
			Quantity_TOC_RGB);

	if (MyType == Visual3d_TOLS_AMBIENT)
		Color		= AColor;
	else Visual3d_LightDefinitionError::Raise
		("Light Type != Visual3d_TOLS_AMBIENT");

}

/*----------------------------------------------------------------------*/

void Visual3d_Light::Values (Quantity_Color& Color, Graphic3d_Vector& Direction) const {

Quantity_Color AColor  (Standard_Real (MyCLight.Color.r),
			Standard_Real (MyCLight.Color.g),
			Standard_Real (MyCLight.Color.b),
			Quantity_TOC_RGB);

Graphic3d_Vector ADirection    (Standard_Real (MyCLight.Direction.x),
				Standard_Real (MyCLight.Direction.y),
				Standard_Real (MyCLight.Direction.z));

	if (MyType == Visual3d_TOLS_DIRECTIONAL) {
		Color		= AColor;
		Direction	= ADirection;
	}
	else Visual3d_LightDefinitionError::Raise
		("Light Type != Visual3d_TOLS_DIRECTIONAL");

}

/*----------------------------------------------------------------------*/

void Visual3d_Light::Values (Quantity_Color& Color, Graphic3d_Vertex& Position, Standard_Real& Fact1, Standard_Real& Fact2) const {

Quantity_Color AColor  (Standard_Real (MyCLight.Color.r),
			Standard_Real (MyCLight.Color.g),
			Standard_Real (MyCLight.Color.b),
			Quantity_TOC_RGB);

Graphic3d_Vertex APosition     (Standard_Real (MyCLight.Position.x),
				Standard_Real (MyCLight.Position.y),
				Standard_Real (MyCLight.Position.z));

	if (MyType == Visual3d_TOLS_POSITIONAL) {
		Color		= AColor;
		Position	= APosition;
		Fact1		= Standard_Real (MyCLight.Attenuation[0]);
		Fact2		= Standard_Real (MyCLight.Attenuation[1]);
	}
	else Visual3d_LightDefinitionError::Raise
		("Light Type != Visual3d_TOLS_POSITIONAL");

}

/*----------------------------------------------------------------------*/

void Visual3d_Light::Values (Quantity_Color& Color, Graphic3d_Vertex& Position, Graphic3d_Vector& Direction, Standard_Real& Concentration, Standard_Real& Fact1, Standard_Real& Fact2, Standard_Real& AngleCone) const {

Quantity_Color AColor  (Standard_Real (MyCLight.Color.r),
			Standard_Real (MyCLight.Color.g),
			Standard_Real (MyCLight.Color.b),
			Quantity_TOC_RGB);

Graphic3d_Vertex APosition     (Standard_Real (MyCLight.Position.x),
				Standard_Real (MyCLight.Position.y),
				Standard_Real (MyCLight.Position.z));

Graphic3d_Vector ADirection    (Standard_Real (MyCLight.Direction.x),
				Standard_Real (MyCLight.Direction.y),
				Standard_Real (MyCLight.Direction.z));

	if (MyType == Visual3d_TOLS_SPOT) {
		Color		= AColor;
		Position	= APosition;
		Direction	= ADirection;
		Concentration	= Standard_Real (MyCLight.Concentration);
		Fact1		= Standard_Real (MyCLight.Attenuation[0]);
		Fact2		= Standard_Real (MyCLight.Attenuation[1]);
		AngleCone	= Standard_Real (MyCLight.Angle);
	}
	else Visual3d_LightDefinitionError::Raise
		("Light Type != Visual3d_TOLS_SPOT");

}

/*----------------------------------------------------------------------*/

void Visual3d_Light::SetAngle (const Standard_Real AngleCone) {

	if (! Visual3d_Light::IsValid (AngleCone))
		Visual3d_LightDefinitionError::Raise
			("Bad value for LightAngle");

	if (MyType == Visual3d_TOLS_SPOT) {
		MyCLight.Angle	= float (AngleCone);

		MyCLight.LightId	=
		  Graphic3d_GraphicDriver::Light (MyCLight, Standard_True);

	}
	else Visual3d_LightDefinitionError::Raise
		("Light Type != Visual3d_TOLS_SPOT");

}

/*----------------------------------------------------------------------*/

void Visual3d_Light::SetAttenuation1 (const Standard_Real Fact1) {

	if ( (Fact1 < 0.0) && (Fact1 > 1.0) )
		Visual3d_LightDefinitionError::Raise
			("Bad value for LightAttenuation");

	if ( (MyType == Visual3d_TOLS_POSITIONAL) ||
	     (MyType == Visual3d_TOLS_SPOT) ) {
		MyCLight.Attenuation[0] = float (Fact1);

		MyCLight.LightId	=
		  Graphic3d_GraphicDriver::Light (MyCLight, Standard_True);

	}
	else Visual3d_LightDefinitionError::Raise
	("Light Type != Visual3d_TOLS_POSITIONAL and != Visual3d_TOLS_SPOT");

}

/*----------------------------------------------------------------------*/

void Visual3d_Light::SetAttenuation2 (const Standard_Real Fact2) {

	if ( (Fact2 < 0.0) && (Fact2 > 1.0) )
		Visual3d_LightDefinitionError::Raise
			("Bad value for LightAttenuation");

	if ( (MyType == Visual3d_TOLS_POSITIONAL) ||
	     (MyType == Visual3d_TOLS_SPOT) ) {
		MyCLight.Attenuation[1] = float (Fact2);

		MyCLight.LightId	=
		  Graphic3d_GraphicDriver::Light (MyCLight, Standard_True);

	}
	else Visual3d_LightDefinitionError::Raise
	("Light Type != Visual3d_TOLS_POSITIONAL and != Visual3d_TOLS_SPOT");

}

/*----------------------------------------------------------------------*/

void Visual3d_Light::SetColor (const Quantity_Color& Color) {

	MyCLight.Color.r	= float (Color.Red ());
	MyCLight.Color.g	= float (Color.Green ());
	MyCLight.Color.b	= float (Color.Blue ());

	MyCLight.LightId	=
		  Graphic3d_GraphicDriver::Light (MyCLight, Standard_True);

}

/*----------------------------------------------------------------------*/

void Visual3d_Light::SetConcentration (const Standard_Real Concentration) {

	if ( (Concentration < 0.0) || (Concentration > 1.0) )
		Visual3d_LightDefinitionError::Raise
			("Bad value for LightConcentration");

	if (MyType == Visual3d_TOLS_SPOT) {
		MyCLight.Concentration	= float (Concentration);

		MyCLight.LightId	=
		  Graphic3d_GraphicDriver::Light (MyCLight, Standard_True);
	}
	else Visual3d_LightDefinitionError::Raise
		("Light Type != Visual3d_TOLS_SPOT");

}

/*----------------------------------------------------------------------*/

void Visual3d_Light::SetDirection (const Graphic3d_Vector& Direction) {

	if (Direction.LengthZero ())
		Visual3d_LightDefinitionError::Raise
			("Bad value for LightDirection");

	if ( (MyType == Visual3d_TOLS_DIRECTIONAL) ||
	     (MyType == Visual3d_TOLS_SPOT) ) {

Standard_Real Norme, X, Y, Z;
		Direction.Coord (X, Y, Z);
		Norme	= Sqrt (X*X+Y*Y+Z*Z);
		// Direction.LengthZero () == Standard_False
		MyCLight.Direction.x	= float (X/Norme);
		MyCLight.Direction.y	= float (Y/Norme);
		MyCLight.Direction.z	= float (Z/Norme);

		MyCLight.LightId	=
		  Graphic3d_GraphicDriver::Light (MyCLight, Standard_True);

	}
	else Visual3d_LightDefinitionError::Raise
	("Light Type != Visual3d_TOLS_DIRECTIONAL and != Visual3d_TOLS_SPOT");

}

/*----------------------------------------------------------------------*/

void Visual3d_Light::SetPosition (const Graphic3d_Vertex& Position) {

	if ( (MyType == Visual3d_TOLS_POSITIONAL) ||
	     (MyType == Visual3d_TOLS_SPOT) ) {

		MyCLight.Position.x	= float (Position.X ());
		MyCLight.Position.y	= float (Position.Y ());
		MyCLight.Position.z	= float (Position.Z ());

		MyCLight.LightId	=
		  Graphic3d_GraphicDriver::Light (MyCLight, Standard_True);

	}
	else Visual3d_LightDefinitionError::Raise
	("Light Type != Visual3d_TOLS_POSITIONAL and != Visual3d_TOLS_SPOT");

}

/*----------------------------------------------------------------------*/

Standard_Integer Visual3d_Light::Limit () {

	// Old method, replaced by GraphicDriver::InquireLightLimit ()
	return 0;

}

/*----------------------------------------------------------------------*/

Standard_Integer Visual3d_Light::Identification () const {

	return (Standard_Integer (MyCLight.LightId));

}

/*----------------------------------------------------------------------*/

Standard_Boolean Visual3d_Light::IsValid (const Standard_Real AAngle) {

	return ( (AAngle <  Standard_PI) && (AAngle >= 0.0) );

}

/*----------------------------------------------------------------------*/
