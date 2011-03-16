#include <gp_XYZ.hxx>
#include <gp_Pnt.hxx>
#include <Bnd_Box.hxx>
#define Coord gp_XYZ
#define Point gp_Pnt
#define Bound Bnd_Box
#include <BndLib_Compute.gxx>
#undef Coord 
#undef Point 
#undef Bound 
