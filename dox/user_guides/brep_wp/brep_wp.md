 BRep Format Description  {#occt_brep_format}
========================

@tableofcontents 

@section occt_brep_format_1 Introduction

  BREP format is used to store 3D models and allows to store a model which consists 
  of vertices, edges, wires, faces, shells, solids,  compsolids, compounds, edge triangulations, 
  face triangulations, polylines on  triangulations, space location and orientation. 
  Any set of such models may be  stored as a single model which is a compound of the models.  
   
  The format is described in an order which is convenient for understanding 
  rather than in the order the format parts follow each other. 
  BNF-like definitions are used in this document. 
  Most of the chapters contain BREP format descriptions in the following order:  
  
  * format file fragment to illustrate the part;  
  * BNF-like definition of the part;  
  * detailed description of the part.  
 
@section occt_brep_format_2 Format Common Structure
 
  ASCII encoding is used to read/write BREP format from/to  file. The format data are stored in a file as text data.  
   
  BREP format uses the following BNF terms:

  * <\n>;  
  * <_\n>;  
  * <_>;  
  * <flag>;  
  * <int>;  
  * <real>;  
  * <2D point>;  
  * <3D point>;  
  * <2D direction>;  
  * <3D direction>;  
  * <+>;  
 
  * \n is the operating-system-dependent ASCII  character sequence which separates ASCII text strings in the operating system used.  
  * _\n = " "* \n;  
  * _ = " "+;  
  * _ is a not empty sequence of space characters  with ASCII code 21h.  
  * flag = "0" | "1";  
  * int is an integer number from @image html brep_wp_image003.gif to @image html brep_wp_image004.gif which is written in  denary system.  
  * real is a real from @image html brep_wp_image005.gif to @image html brep_wp_image006.gif 
   which is written in decimal  or E form with base 10. 
   
  The point is used as a delimiter of the integer and  fractional parts.  
 
  * 2D point = real _ real;  
  * 3D point = real (_ real)  ^ 2;  
  * 2D direction is a 2D point @image html brep_wp_image007.gif so that @image html brep_wp_image008.gif.  
  * 3D direction is a 3D point @image html brep_wp_image009.gif so that @image html brep_wp_image010.gif.  
   + is an arithmetic operation of addition.  
 
  The format consists of the following sections:  

  * content type;  
  * version;  
  * locations;  
  * geometry;  
  * shapes.  
 
  content type = "DBRep_DrawableShape" _\n  _\n;  
  content type have other values [1].  
 
  version = ("CASCADE Topology V1, (c)  Matra-Datavision" | "CASCADE Topology V2, (c) Matra-Datavision")  _\n;  
  
  The difference of the versions is described in the  document.  
 
  Sections <locations>, <geometry> and <shapes> are described below in separate chapters of the document.  
 
@section occt_brep_format_3 Section locations
 
**Example**  

@verbatim 
    Locations  3  
    1  
                   0               0               1               0   
                   1               0               0               0   
                   0               1               0               0   
    1  
                   1               0               0               4   
                   0               1               0               5   
                   0               0               1               6   
    2   1 1 2 1 0  
@endverbatim
 
**BNF-like Definition**

@verbatim
    locations = location header _\n  location records;  
    location header = "Locations" _  location record count;  
    location record count = int;  
    location records = location record ^  location record count;  
    location record = location record 1 |  location record 2;  
    location record 1 = "1" _\n location  data 1;  
    location record 2 = "2" _ location  data 2;  
    location data 1 = ((_ real) ^ 4  _\n) ^ 3;  
    location data 2 = (int _  int _)* "0" _\n;  
@endverbatim

****Description****

*location data 1* is interpreted as a 3 x 4 matrix @image html brep_wp_image011.gif 
which describes transformation of 3 dimensional space and satisfies the following constraints:  
 
  * @image html brep_wp_image012.gif where @image html brep_wp_image013.gif where @image html brep_wp_image014.gif;  
  * @image html brep_wp_image015.gif where @image html brep_wp_image016.gif.  
 
The transformation transforms a point 

@image html brep_wp_image017.gif 

to another point 

@image html brep_wp_image018.gif 

by the rule:
 
@image html brep_wp_image019.gif.  
@image html brep_wp_image020.gif 

may be a composition of matrices for the following elementary transformations:  
 
  *  parallel translation – @image html brep_wp_image021.gif;  
  *  rotation around an axis with a direction @image html brep_wp_image022.gif by an angle @image html brep_wp_image023.gif –  
 
@image html brep_wp_image024.gif;  
 
  *  scaling –          @image html brep_wp_image025.gif where @image html brep_wp_image026.gif;  
  *  central symmetry – @image html brep_wp_image027.gif;  
  *  axis symmetry –    @image html brep_wp_image028.gif;  
  *  plane symmetry –   @image html brep_wp_image029.gif.  
 
*location data 2* is interpreted as a composition of locations raised to a power and placed above this location data 2 in  the section locations. location data 2 is a sequence @image html brep_wp_image030.gif of @image html brep_wp_image031.gif integer pairs @image html brep_wp_image032.gif (@image html brep_wp_image033.gif). flag 0 is the indicator  of the sequence end. The sequence is interpreted as a composition @image html brep_wp_image034.gif where @image html brep_wp_image035.gif is a location from @image html brep_wp_image036.gif-th location  record in the section locations. location record numbering  starts from1.  
 
 
@section occt_brep_format_4 Section geometry

@verbatim
    geometry =  
    2D curves  
    3D curves  
    3D polygons  
    polygons on triangulations  
    surfaces  
    triangulations;  
@endverbatim 
 
@subsection occt_brep_format_4_1  Subsection 3D curves
 
**Example**

@verbatim
    Curves 13  
    1 0 0 0 0 0 1   
    1 0 0 3 -0 1 0   
    1 0 2 0 0 0 1   
    1 0 0 0 -0 1 0   
    1 1 0 0 0 0 1   
    1 1 0 3 0 1 0   
    1 1 2 0 0 0 1   
    1 1 0 0 -0 1 0   
    1 0 0 0 1 0 -0   
    1 0 0 3 1 0 -0   
    1 0 2 0 1 0 -0   
    1 0 2 3 1 0 -0   
    1 1 0 0 1 0 0   
@endverbatim 
 
**BNF-like Definition**

@verbatim
    3D curves = 3D curve header  _\n 3D curve records;  
     
    3D curve header = "Curves" _ 3D  curve count;  
     
    3D curve count = int;  
     
    3D curve records = 3D curve record ^  3D curve count;  
     
    3D curve record =  
    3D curve record 1 |  
    3D curve record 2 |  
    3D curve record 3 |  
    3D curve record 4 |  
    3D curve record 5 |  
    3D curve record 6 |  
    3D curve record 7 |  
    3D curve record 8 |  
    3D curve record 9;  
 
 
@subsubsection occt_brep_format_4_1_1 3D curve record 1 – Line
 
**Example**  

@verbatim
    1 1 0 3 0 1 0   
@endverbatim
 
**BNF-like Definition**

@verbatim
    3D curve record 1 = "1" _ 3D  point _ 3D direction _\n;  
@endverbatim
 
**Description**  
 
3D curve record 1 describes a line. The line  data consist of a 3D point @image html brep_wp_image037.gif and a 3D direction @image html brep_wp_image038.gif. The line passes through the  point @image html brep_wp_image037.gif, has  the direction @image html brep_wp_image038.gif and  is defined by the following parametric equation:  
 
@image html brep_wp_image039.gif, @image html brep_wp_image040.gif.  
 
The **Example** record is interpreted as a line which  passes through a point @image html brep_wp_image041.gif, has a direction @image html brep_wp_image042.gif and is defined by the  following parametric equation: @image html brep_wp_image043.gif.  
 
 
@subsubsection occt_brep_format_4_1_2 3D curve record 2 – Circle
 
**Example**  

@verbatim 
    2 1 2 3 0 0 1 1 0 -0 -0 1 0 4  
@endverbatim 
 
**BNF-like Definition**
 
@verbatim
    3D curve record 2 = "2" _ 3D  circle center _ 3D circle N _ 3D circle Dx _    3D circle Dy _ 3D circle radius _\n;  
     
    3D circle center = 3D point;  
     
    3D circle N = 3D direction;  
     
    3D circle Dx = 3D direction;  
     
    3D circle Dy = 3D direction;  
     
    3D circle radius = real;  
@endverbatim 
 
**Description**  
 
3D curve record 2 describes a circle. The  circle data consist of a 3D point @image html brep_wp_image037.gif, pairwise orthogonal 3D directions @image html brep_wp_image044.gif, @image html brep_wp_image045.gif and @image html brep_wp_image046.gif and a non-negative real @image html brep_wp_image047.gif. The circle has a center  @image html brep_wp_image037.gif and is  located in a plane with a normal @image html brep_wp_image044.gif. The circle has a radius @image html brep_wp_image047.gif and is defined by  the following parametric equation:  
 
@image html brep_wp_image048.gif, @image html brep_wp_image049.gif.  
 
The  example record is interpreted as a circle which has its center @image html brep_wp_image050.gif, is located in plane  with a normal @image html brep_wp_image051.gif.  Directions for the circle are @image html brep_wp_image052.gif and @image html brep_wp_image053.gif. The circle has a radius @image html brep_wp_image054.gif and is defined by  the following parametric equation: @image html brep_wp_image055.gif.  
 
 
@subsubsection occt_brep_format_4_1_3 3D curve record 3 – Ellipse
 
**Example**  

@verbatim
    3 1 2 3 0 0 1 1 0 -0 -0 1 0 5  4  
@endverbatim
 
**BNF-like Definition**

@verbatim 
    3D curve record 3 = "3" _ 3D  ellipse center _ 3D ellipse N _ 3D ellipse Dmaj  _ 3D ellipse Dmin _ 3D ellipse Rmaj _  3D ellipse Rmin _\n;  
     
    3D ellipse center = 3D point;  
     
    3D ellipse N = 3D direction;  
     
    3D ellipse Dmaj = 3D direction;  
     
    3D ellipse Dmin = 3D direction;  
     
    3D ellipse Rmaj = real;  
     
    3D ellipse Rmin = real;  
@endverbatim 
 
**Description**  
 
3D curve record 3 describes an ellipse. The  ellipse data consist of a 3D point @image html brep_wp_image037.gif, pairwise orthogonal 3D directions @image html brep_wp_image044.gif, @image html brep_wp_image056.gif and @image html brep_wp_image057.gif and non-negative reals @image html brep_wp_image058.gif and @image html brep_wp_image059.gif so that @image html brep_wp_image060.gif. The ellipse has its  center @image html brep_wp_image037.gif, is  located in plane with the normal @image html brep_wp_image044.gif, has major and minor axis directions @image html brep_wp_image056.gif and @image html brep_wp_image057.gif, major and minor  radii @image html brep_wp_image058.gif and @image html brep_wp_image059.gif and is defined by  the following parametric equation:  
 
@image html brep_wp_image061.gif, @image html brep_wp_image049.gif.  
 
The  example record is interpreted as an ellipse which has its center @image html brep_wp_image050.gif, is located in plane  with a normal @image html brep_wp_image051.gif,  has major and minor axis directions @image html brep_wp_image062.gif and @image html brep_wp_image063.gif, major and minor radii @image html brep_wp_image064.gif and @image html brep_wp_image065.gif and is defined by  the following parametric equation: @image html brep_wp_image066.gif.  
 
 
@subsubsection occt_brep_format_4_1_4 3D curve record 4 – Parabola
 
**Example**  

@verbatim 
    4 1 2 3 0 0 1 1 0 -0 -0 1 0  16  
@endverbatim 
 
**BNF-like Definition**  

@verbatim
    3D curve record 4 = "4" _ 3D  parabola origin _ 3D parabola N _   3D parabola Dx _ 3D parabola Dy _ 3D  parabola focal length _\n;  
     
    3D parabola origin = 3D point;  
     
    3D parabola N = 3D direction;  
     
    3D parabola Dx = 3D direction;  
     
    3D parabola Dy = 3D direction;  
     
    3D parabola focal length = real;  
@endverbatim 
 
**Description**  
 
3D curve record 4 describes a parabola. The  parabola data consist of a 3D point @image html brep_wp_image037.gif, pairwise orthogonal 3D directions @image html brep_wp_image044.gif, @image html brep_wp_image045.gif and @image html brep_wp_image046.gif and a non-negative real @image html brep_wp_image067.gif. The parabola is  located in plane which passes through the point @image html brep_wp_image037.gif and has the normal @image html brep_wp_image044.gif. The parabola has a focus  length @image html brep_wp_image067.gif and  is defined by the following parametric equation:  
 
@image html brep_wp_image068.gif, @image html brep_wp_image069.gif &Uuml; @image html brep_wp_image070.gif;  
 
@image html brep_wp_image071.gif, @image html brep_wp_image069.gif &Uuml; @image html brep_wp_image072.gif (degenerated case).  
 
The  example record is interpreted as a parabola in plane which passes through a  point @image html brep_wp_image073.gif and  has a normal @image html brep_wp_image074.gif.  Directions for the parabola are @image html brep_wp_image075.gif and @image html brep_wp_image076.gif. The parabola has a focus length @image html brep_wp_image077.gif and is defined by  the following parametric equation: @image html brep_wp_image078.gif.  
 
 
@subsubsection occt_brep_format_4_1_5 3D curve record 5 – Hyperbola
 
**Example**  

@verbatim
    5 1 2 3 0 0 1 1 0 -0 -0 1 0 5  4  
@verbatim
 
**BNF-like Definition**  

@verbatim
    3D curve record 5 = "5" _ 3D  hyperbola origin _ 3D hyperbola N _   3D hyperbola Dx _ 3D hyperbola Dy _ 3D  hyperbola Kx _ 3D hyperbola Ky _\n;  
     
    3D hyperbola origin = 3D point;  
     
    3D hyperbola N = 3D direction;  
     
    3D hyperbola Dx = 3D direction;  
     
    3D hyperbola Dy = 3D direction;  
     
    3D hyperbola Kx = real;  
     
    3D hyperbola Ky = real;  
@endverbatim
 
Descripton  
 
3D curve record 5 describes a hyperbola. The  hyperbola data consist of a 3D point @image html brep_wp_image037.gif, pairwise orthogonal 3D directions @image html brep_wp_image044.gif, @image html brep_wp_image045.gif and @image html brep_wp_image046.gif and non-negative reals @image html brep_wp_image079.gif and @image html brep_wp_image080.gif. The hyperbola is  located in plane which passes through the point @image html brep_wp_image037.gif and has the normal @image html brep_wp_image044.gif. The hyperbola is defined by  the following parametric equation:  
 
@image html brep_wp_image081.gif, @image html brep_wp_image069.gif.  
 
The  example record is interpreted as a hyperbola in plane which passes through a  point @image html brep_wp_image073.gif and  has a normal @image html brep_wp_image074.gif.  Other hyperbola data are @image html brep_wp_image075.gif, @image html brep_wp_image076.gif, @image html brep_wp_image082.gif and @image html brep_wp_image083.gif. The hyperbola is defined by the  following parametric equation: @image html brep_wp_image084.gif.  
 
 
@subsubsection occt_brep_format_4_1_6 3D curve record 6 – Bezier Curve
 
**Example**  

@verbatim
    6 1 2 0 1 0  4 1 -2 0  5 2 3  0  6   
@verbatim
 
**BNF-like Definition**

@verbatim
    3D curve record 6 = "6" _ 3D  Bezier rational flag _ 3D Bezier degree   3D Bezier weight poles _\n;  
     
    3D Bezier rational flag = flag;  
     
    3D Bezier degree = int;  
     
    3D Bezier weight poles = (_ 3D  Bezier weight pole) ^ (3D Bezier degree + "1");  
     
    3D Bezier weight pole = 3D point  [_ real];  
@verbatim
 
**Description**  
 
3D curve record 6 describes a Bezier curve.  The curve data consist of a rational flag @image html brep_wp_image047.gif, a degree @image html brep_wp_image085.gif and weight poles.  
 
The weight poles are @image html brep_wp_image086.gif 3D points @image html brep_wp_image087.gif if the flag @image html brep_wp_image047.gif is 0. The weight  poles are @image html brep_wp_image086.gif pairs  @image html brep_wp_image088.gif if flag @image html brep_wp_image047.gif is 1. Here @image html brep_wp_image089.gif is a 3D point and @image html brep_wp_image090.gif is a positive real (@image html brep_wp_image091.gif). @image html brep_wp_image092.gif (@image html brep_wp_image093.gif) if the flag @image html brep_wp_image047.gif is 0.  
 
The Bezier curve is defined by the following  parametric equation:  
 
@image html brep_wp_image094.gif, @image html brep_wp_image095.gif  
 
where @image html brep_wp_image096.gif.  
 
The example record is interpreted as a Bezier curve  with a rational flag @image html brep_wp_image097.gif, degree @image html brep_wp_image098.gif and weight poles @image html brep_wp_image099.gif, @image html brep_wp_image100.gif, @image html brep_wp_image101.gif, @image html brep_wp_image102.gif and @image html brep_wp_image103.gif, @image html brep_wp_image104.gif. The Bezier curve is defined  by the following parametric equation:  
 
@image html brep_wp_image105.gif.  
 
 
@subsubsection occt_brep_format_4_1_7 3D curve record 7 – B-spline Curve
 
**Example**  

@verbatim
    7 1 0  1 3 5  0 1 0  4 1 -2  0  5 2 3 0  6  
     0 1 0.25 1 0.5 1 0.75 1 1 1  
@endverbatim
 
**BNF-like Definition**

@verbatim
    3D curve record 7 = "7" _ 3D  B-spline rational flag _ "0" _ 3D B-spline degree _   3D B-spline pole count _ 3D B-spline multiplicity knot  count 3D B-spline weight poles   _\n 3D B-spline multiplicity knots _\n;  
     
    3D B-spline rational flag = flag;  
     
    3D B-spline degree = int;  
     
    3D B-spline pole count = int;  
     
    3D B-spline multiplicity knot count = int;  
     
    3D B-spline weight poles = (_ 3D  B-spline weight pole) ^ 3D B-spline pole count;  
     
    3D B-spline weight pole = 3D point  [_ real];  
     
    3D B-spline multiplicity knots =  
    (_ 3D B-spline multiplicity knot) ^  3D B-spline multiplicity knot count;  
     
    3D B-spline multiplicity knot = real  _ int;  
@endverbatim
 
**Description**  
 
3D curve record 7 describes a B-spline curve.  The curve data consist of a rational flag @image html brep_wp_image047.gif, a degree @image html brep_wp_image085.gif, pole count @image html brep_wp_image106.gif, multiplicity knot count @image html brep_wp_image107.gif, weight poles and  multiplicity knots.  
 
The weight poles are @image html brep_wp_image108.gif 3D points @image html brep_wp_image109.gif if the flag @image html brep_wp_image047.gif is 0. The weight  poles are @image html brep_wp_image108.gif pairs  @image html brep_wp_image110.gif if the flag  @image html brep_wp_image047.gif is 1. Here @image html brep_wp_image089.gif is a 3D point and @image html brep_wp_image090.gif is a positive real (@image html brep_wp_image111.gif). @image html brep_wp_image092.gif (@image html brep_wp_image111.gif) if the flag @image html brep_wp_image047.gif is 0.  
 
The multiplicity knots are @image html brep_wp_image107.gif pairs @image html brep_wp_image112.gif. Here @image html brep_wp_image113.gif is a knot with a multiplicity  @image html brep_wp_image114.gif (@image html brep_wp_image115.gif) so that  
 
@image html brep_wp_image116.gif (@image html brep_wp_image117.gif),  
@image html brep_wp_image118.gif, @image html brep_wp_image119.gif, @image html brep_wp_image120.gif (@image html brep_wp_image121.gif), @image html brep_wp_image122.gif.  
 
The B-spline curve is defined by the following  parametric equation:  
 
@image html brep_wp_image123.gif, @image html brep_wp_image124.gif  
 
where functions @image html brep_wp_image125.gif have the following recursion definition  by @image html brep_wp_image126.gif  
 
@image html brep_wp_image127.gif, @image html brep_wp_image128.gif (@image html brep_wp_image129.gif)  
 
where  
 
@image html brep_wp_image130.gif (@image html brep_wp_image131.gif, @image html brep_wp_image132.gif).  
 
The example record is interpreted as a B-spline curve  with a rational flag @image html brep_wp_image097.gif, a degree @image html brep_wp_image133.gif, pole count @image html brep_wp_image134.gif, multiplicity knot count @image html brep_wp_image135.gif, weight poles @image html brep_wp_image136.gif, @image html brep_wp_image137.gif, @image html brep_wp_image138.gif, @image html brep_wp_image139.gif and @image html brep_wp_image140.gif, @image html brep_wp_image141.gif, multiplicity knots @image html brep_wp_image142.gif, @image html brep_wp_image143.gif, @image html brep_wp_image144.gif, @image html brep_wp_image145.gif, @image html brep_wp_image146.gif, @image html brep_wp_image147.gif, @image html brep_wp_image148.gif, @image html brep_wp_image149.gif and @image html brep_wp_image150.gif, @image html brep_wp_image151.gif. The B-spline curve is defined  by the following parametric equation:  
 
@image html brep_wp_image152.gif.  
 
 
@subsubsection occt_brep_format_4_1_8 3D curve record 8 – Trimmed Curve
 
**Example**  

@verbatim
    8  4 -5  
    1 1 2 3 1 0 0   
@endverbatim
 
**BNF-like Definition**  

@verbatim
    3D curve record 8 = "8" _ 3D  trimmed curve u min _ 3D trimmed curve u max _\n  3D curve record;  
     
    3D trimmed curve u min = real;  
     
    3D trimmed curve u max = real;  
@endverbatim
 
**Description**  
 
3D curve record 8 describes a trimmed curve.  The trimmed curve data consist of reals @image html brep_wp_image153.gif and @image html brep_wp_image154.gif and 3D curve record so that @image html brep_wp_image155.gif. The trimmed curve  is a restriction of the base curve @image html brep_wp_image156.gif described in the record to the segment @image html brep_wp_image157.gif. The trimmed curve is  defined by the following parametric equation:  
 
@image html brep_wp_image158.gif, @image html brep_wp_image159.gif.  
 
The  example record is interpreted as a trimmed curve with @image html brep_wp_image160.gif and @image html brep_wp_image161.gif for the base curve @image html brep_wp_image162.gif. The trimmed curve is  defined by the following parametric equation: @image html brep_wp_image163.gif, @image html brep_wp_image164.gif.  
 
 
@subsubsection occt_brep_format_4_1_9 3D curve record 9 – Offset Curve
 
**Example**  

@verbatim
    9 2  
    0 1 0   
    1 1 2 3 1 0 0   
@endverbatim
 
**BNF-like Definition**  

@verbatim
    3D  curve record 9 = "9" _ 3D offset curve distance _\n  
    3D  offset curve direction _\n  
    3D  curve record;  
     
    3D  offset curve distance = real;  
     
    3D  offset curve direction = 3D direction;  
@endverbatim
 
**Description**  
 
3D  curve record 9 describes an offset curve. The offset curve data consist of a  distance @image html brep_wp_image165.gif, a  3D direction @image html brep_wp_image038.gif and  a 3D curve record. The offset curve is the result of offsetting the base  curve @image html brep_wp_image156.gif described  in the record to the distance @image html brep_wp_image165.gif along the vector @image html brep_wp_image166.gif. The offset curve is defined  by the following parametric equation:  
 
@image html brep_wp_image167.gif, @image html brep_wp_image168.gif.  
 
The  example record is interpreted as an offset curve with a distance @image html brep_wp_image169.gif, direction @image html brep_wp_image170.gif, base curve @image html brep_wp_image162.gif and defined by the  following parametric equation: @image html brep_wp_image171.gif.  
 
@subsection occt_brep_format_4_2  Subsection surfaces
 
**Example**  

@verbatim
    Surfaces 6  
    1 0 0 0 1 0 -0 0 0 1 0 -1 0   
    1 0 0 0 -0 1 0 0 0 1 1 0 -0   
    1 0 0 3 0 0 1 1 0 -0 -0 1 0   
    1 0 2 0 -0 1 0 0 0 1 1 0 -0   
    1 0 0 0 0 0 1 1 0 -0 -0 1 0   
    1 1 0 0 1 0 -0 0 0 1 0 -1 0   
@endverbatim
 
**BNF-like Definition**  

@verbatim
    surfaces = surface header _\n  surface records;  
     
    surface header = "Surfaces" _  surface count;  
     
    surface records = surface record ^  surface count;  
     
    surface record =  
    surface record 1 |  
    surface record 2 |  
    surface record 3 |  
    surface record 4 |  
    surface record 5 |  
    surface record 6 |  
    surface record 7 |  
    surface record 8 |  
    surface record 9 |  
    surface record 10 |  
    surface record 11;  
@verbatim
 
@subsubsection occt_brep_format_4_2_1 surface record 1 – Plane
 
**Example**  

@verbatim
    1 0 0 3 0 0 1 1 0 -0 -0 1 0   
@endverbatim
 
**BNF-like Definition**

@verbatim
surface record 1 = "1" _ 3D  point (_ 3D direction) ^ 3 _\n;  
@endverbatim
 
**Description**  
 
surface record 1 describes a plane. The plane  data consist of a 3D point @image html brep_wp_image037.gif and pairwise orthogonal 3D directions @image html brep_wp_image044.gif, @image html brep_wp_image172.gif and @image html brep_wp_image173.gif. The plane passes through the  point @image html brep_wp_image037.gif, has  the normal @image html brep_wp_image174.gif and  is defined by the following parametric equation:  
 
@image html brep_wp_image175.gif, @image html brep_wp_image176.gif.  
 
The example record is interpreted as a plane which  passes through a point @image html brep_wp_image177.gif, has a normal @image html brep_wp_image074.gif and is defined by the  following parametric equation: @image html brep_wp_image178.gif.  
 
 
@subsubsection occt_brep_format_4_2_2 surface record 2 – Cylinder
 
**Example**  

@verbatim
    2 1 2 3 0 0 1 1 0 -0 -0 1 0 4  
@endverbatim
 
**BNF-like Definition**  

@verbatim
    surface record 2 = "2" _ 3D  point (_ 3D direction) ^ 3 _ real  _\n;  
@endverbatim
 
**Description**  
 
surface record 2 describes a cylinder. The  cylinder data consist of a 3D point @image html brep_wp_image037.gif, pairwise orthogonal 3D directions @image html brep_wp_image173.gif, @image html brep_wp_image045.gif and @image html brep_wp_image046.gif and a non-negative real @image html brep_wp_image047.gif. The cylinder axis  passes through the point @image html brep_wp_image037.gif and has the direction @image html brep_wp_image173.gif. The cylinder has  the radius @image html brep_wp_image179.gif and  is defined by the following parametric equation:  
 
@image html brep_wp_image180.gif, @image html brep_wp_image181.gif.  
 
The  example record is interpreted as a cylinder which axis passes through a point @image html brep_wp_image073.gif and has a direction @image html brep_wp_image182.gif. Directions for the  cylinder are @image html brep_wp_image075.gif and  @image html brep_wp_image076.gif. The  cylinder has a radius @image html brep_wp_image054.gif and is defined by the following  parametric equation: @image html brep_wp_image183.gif.  
 
 
@subsubsection occt_brep_format_4_2_3 surface record 3 – Cone
 
**Example**  

@verbatim
    3 1 2 3 0 0 1 1 0 -0 -0 1 0 4  
    0.75
@endverbatim
 
 
**BNF-like Definition**

@verbatim
    surface record 3 = "3" _ 3D  point (_ 3D direction) ^ 3 (_ real) ^ 2  _\n;  
@endverbatim
 
**Description**  
 
surface record 3 describes a cone. The cone  data consist of a 3D point @image html brep_wp_image037.gif, pairwise orthogonal 3D directions @image html brep_wp_image184.gif, @image html brep_wp_image045.gif and @image html brep_wp_image046.gif, a non-negative real @image html brep_wp_image047.gif and a real @image html brep_wp_image185.gif. The cone axis passes  through the point @image html brep_wp_image037.gif and  has the direction @image html brep_wp_image184.gif.  The plane which passes through the point @image html brep_wp_image037.gif and is parallel to directions @image html brep_wp_image045.gif and @image html brep_wp_image046.gif is the cone referenced  plane. The cone section by the plane is a circle with the radius @image html brep_wp_image047.gif. The direction from  the point @image html brep_wp_image037.gif to  the cone apex is @image html brep_wp_image186.gif.  The cone has a half-angle @image html brep_wp_image187.gif and is defined by the following  parametric equation:  
 
@image html brep_wp_image188.gif, @image html brep_wp_image189.gif.  
 
The example record is interpreted as a cone with an axis  which passes through a point @image html brep_wp_image073.gif and has a direction @image html brep_wp_image190.gif. Other cone data are @image html brep_wp_image075.gif, @image html brep_wp_image076.gif, @image html brep_wp_image191.gif and @image html brep_wp_image192.gif. The cone is defined by the  following parametric equation:  
@image html brep_wp_image193.gif.  
 
 
@subsubsection occt_brep_format_4_2_4 surface record 4 – Sphere
 
**Example**  

@verbatim
    4 1 2 3 0 0 1 1 0 -0 -0 1 0 4  
@endverbatim
 
**BNF-like Definition**

@verbatim
    surface record 4 = "4" _ 3D  point (_ 3D direction) ^ 3 _ real  _\n;  
@endverbatim
 
**Description**  
 
surface record 4 describes a sphere. The  sphere data consist of a 3D point @image html brep_wp_image037.gif, pairwise orthogonal 3D directions @image html brep_wp_image184.gif, @image html brep_wp_image045.gif and @image html brep_wp_image046.gif and a non-negative real @image html brep_wp_image047.gif. The sphere has the  center @image html brep_wp_image194.gif,  radius @image html brep_wp_image179.gif and  is defined by the following parametric equation:  
 
@image html brep_wp_image195.gif, @image html brep_wp_image196.gif.  
 
The  example record is interpreted as a sphere with its center @image html brep_wp_image073.gif. Directions for the sphere are  @image html brep_wp_image190.gif, @image html brep_wp_image075.gif and @image html brep_wp_image076.gif. The sphere has a radius  @image html brep_wp_image191.gif and is  defined by the following parametric equation:  
@image html brep_wp_image197.gif.  
 
 
@subsubsection occt_brep_format_4_2_5 surface record 5 – Torus
 
**Example**  

@verbatim
    5 1 2 3 0 0 1 1 0 -0 -0 1 0 8  4  
@endverbatim
 
**BNF-like Definition**

@verbatim
    surface record 5 = "5" _ 3D  point (_ 3D direction) ^ 3 (_ real) ^ 2  _\n;  
@endverbatim
 
**Description**  
 
surface record 5 describes a torus. The torus  data consist of a 3D point @image html brep_wp_image037.gif, pairwise orthogonal 3D directions @image html brep_wp_image184.gif, @image html brep_wp_image045.gif and @image html brep_wp_image046.gif and non-negative reals @image html brep_wp_image198.gif and @image html brep_wp_image199.gif. The torus axis  passes through the point @image html brep_wp_image037.gif and has the direction @image html brep_wp_image184.gif. @image html brep_wp_image198.gif is the distance from the  torus circle center to the axis. The torus circle has the radius @image html brep_wp_image199.gif. The torus is defined  by the following parametric equation:  
 
@image html brep_wp_image200.gif, @image html brep_wp_image201.gif.  
 
The example record is interpreted as a torus with an axis  which passes through a point @image html brep_wp_image073.gif and has a direction @image html brep_wp_image190.gif. @image html brep_wp_image075.gif, @image html brep_wp_image076.gif, @image html brep_wp_image202.gif and @image html brep_wp_image203.gif for the torus. The torus is defined  by the following parametric equation:  
@image html brep_wp_image204.gif.  
 
 
@subsubsection occt_brep_format_4_2_6 surface record 6 – Linear Extrusion
 
**Example**  

@verbatim
    6 0 0.6 0.8   
    2 1 2 3 0 0 1 1 0 -0 -0 1 0 4  
@endverbatim
 
**BNF-like Definition**

@verbatim
    surface record 6 = "6" _ 3D  direction _\n 3D curve record;  
@endverbatim
 
**Description**  
 
surface record 6 describes a linear extrusion  surface. The surface data consist of a 3D direction @image html brep_wp_image173.gif and a 3D curve  record. The linear extrusion surface has the direction @image html brep_wp_image173.gif, the base curve @image html brep_wp_image205.gif described in the  record and is defined by the following parametric equation:  
 
@image html brep_wp_image206.gif, @image html brep_wp_image207.gif.  
 
The example record is interpreted as a linear  extrusion surface with a direction @image html brep_wp_image208.gif. The base curve is a circle for the  surface. The surface is defined by the following parametric equation: @image html brep_wp_image209.gif, @image html brep_wp_image189.gif.  
 
 
@subsubsection occt_brep_format_4_2_7 surface record 7 – Revolution Surface
 
**Example**  

@verbatim
    7 -4 0 3 0 1 0   
    2 1 2 3 0 0 1 1 0 -0 -0 1 0 4  
@endverbatim
 
**BNF-like Definition**

@verbatim
    surface record 7 = "7" _ 3D  point _ 3D direction _\n 3D curve record;  
@endverbatim
 
**Description**  
 
surface record 7 describes a revolution  surface. The surface data consist of a 3D point @image html brep_wp_image037.gif, a 3D direction @image html brep_wp_image038.gif and a 3D curve  record. The surface axis passes through the point @image html brep_wp_image037.gif and has the direction @image html brep_wp_image038.gif. The base curve @image html brep_wp_image210.gif described by the  record and the axis are coplanar. The surface is defined by the following  parametric equation:  
 
@image html brep_wp_image211.gif, @image html brep_wp_image212.gif  
 
where @image html brep_wp_image213.gif, @image html brep_wp_image214.gif.  
 
The example record is interpreted as a revolution  surface with an axis which passes through a point @image html brep_wp_image215.gif and has a direction @image html brep_wp_image170.gif. The base curve is a circle  for the surface. The surface is defined by the following parametric equation:  
@image html brep_wp_image216.gif, @image html brep_wp_image201.gif where @image html brep_wp_image217.gif, @image html brep_wp_image218.gif.  
 
 
@subsubsection occt_brep_format_4_2_8 surface record 8 – Bezier Surface
 
**Example**  

@verbatim
    8 1 1 2 1 0 0 1  7 1 0 -4  10    
    0 1 -2  8 1 1 5  11   
    0 2 3  9 1 2 6  12   
@endverbatim
 
**BNF-like Definition**

@verbatim
    surface record 8 = "8" _ Bezier  surface u rational flag _ Bezier surface v rational flag  _ Bezier surface u degree _ Bezier surface v  degree _   Bezier surface weight poles;  
     
    Bezier surface u rational flag = flag;  
     
    Bezier surface v rational flag = flag;  
     
    Bezier surface u degree = int;  
     
    Bezier surface v degree = int;  
     
    Bezier surface weight poles =  
    (Bezier surface weight pole group _\n)  ^ (Bezier surface u degree + "1");  
     
    Bezier surface weight pole group = Bezier  surface weight pole  
    (_ Bezier surface weight pole) ^  Bezier surface v degree;  
     
    Bezier surface weight pole = 3D point  [_ real];  
    @endverbatim
 
**Description**  
 
surface record 8 describes a Bezier surface.  The surface data consist of a u rational flag @image html brep_wp_image219.gif, v rational flag @image html brep_wp_image220.gif, u degree @image html brep_wp_image221.gif, v degree @image html brep_wp_image222.gif and weight poles.  
 
The weight poles are @image html brep_wp_image223.gif 3D points @image html brep_wp_image224.gif (@image html brep_wp_image225.gif) if @image html brep_wp_image226.gif. The weight poles are @image html brep_wp_image223.gif pairs @image html brep_wp_image227.gif (@image html brep_wp_image225.gif) if @image html brep_wp_image228.gif. Here @image html brep_wp_image224.gif is a 3D point and @image html brep_wp_image229.gif is a positive real (@image html brep_wp_image225.gif). @image html brep_wp_image230.gif (@image html brep_wp_image225.gif) if @image html brep_wp_image226.gif.  
 
The Bezier surface is defined by the following  parametric equation:  
 
@image html brep_wp_image231.gif, @image html brep_wp_image232.gif  
 
where @image html brep_wp_image096.gif.  
 
The example record is interpreted as a Bezier surface  with a u rational flag @image html brep_wp_image233.gif, v rational flag @image html brep_wp_image234.gif, u degree @image html brep_wp_image235.gif, v degree @image html brep_wp_image236.gif, weight poles @image html brep_wp_image237.gif, @image html brep_wp_image238.gif, @image html brep_wp_image239.gif, @image html brep_wp_image240.gif, @image html brep_wp_image241.gif, @image html brep_wp_image242.gif, @image html brep_wp_image243.gif, @image html brep_wp_image244.gif, @image html brep_wp_image245.gif, @image html brep_wp_image246.gif and @image html brep_wp_image247.gif, @image html brep_wp_image248.gif. The surface is defined by  the following parametric equation:  
 
@image html brep_wp_image249.gif  
 
 
@subsubsection occt_brep_format_4_2_9 surface record 9 – B-spline Surface
 
**Example**  

@verbatim
    9  1 1 0 0 1 1 3 2 5 4 0 0 1  7 1 0 -4  10   
    0  1 -2  8 1 1 5  11   
    0  2 3  9 1 2 6  12   
     
    0  1  
    0.25  1  
    0.5  1  
    0.75  1  
    1  1  
     
    0  1  
    0.3  1  
    0.7  1  
    1 1  
@endverbatim
 
**BNF-like Definition**

@verbatim
    surface record 9 = "9" _ B-spline  surface u rational flag _   B-spline surface v rational flag _ "0" _ "0" _  B-spline surface u degree _   B-spline surface v degree _ B-spline surface u pole  count _   B-spline surface v pole count _ B-spline surface u  multiplicity knot count _   B-spline surface v multiplicity knot count _ B-spline  surface weight poles _\n   B-spline surface u multiplicity knots _\n B-spline surface  v multiplicity knots;  
     
    B-spline surface u rational flag =  flag;  
     
    B-spline surface v rational flag = flag;  
     
    B-spline surface u degree = int;  
     
    B-spline surface v degree = int;  
     
    B-spline surface u pole count = int;  
     
    B-spline surface v pole count = int;  
     
    B-spline surface u multiplicity knot count =  int;  
     
    B-spline surface v multiplicity knot count =  int;  
     
    B-spline surface weight poles =  
    (B-spline surface weight pole group  _\n) ^ B-spline surface u pole count;  
     
    B-spline surface weight pole group =  
    (B-spline surface weight pole _) ^  B-spline surface v pole count;  
     
    B-spline surface weight pole = 3D  point [_ real];  
     
    B-spline surface u multiplicity knots =  
    (B-spline surface u multiplicity knot  _\n) ^ B-spline surface u multiplicity knot count;  
     
    B-spline surface u multiplicity knot =  real _ int;  
     
    B-spline surface v multiplicity knots =  
    (B-spline surface v multiplicity knot  _\n) ^ B-spline surface v multiplicity knot count;  
     
    B-spline surface v multiplicity knot =  real _ int;  
@endverbatim
 
**Description**  
 
surface record 9 describes a B-spline surface.  The surface data consist of a u rational flag @image html brep_wp_image219.gif, v rational flag @image html brep_wp_image220.gif, u degree @image html brep_wp_image221.gif, v degree @image html brep_wp_image222.gif, u pole count @image html brep_wp_image250.gif, v pole count @image html brep_wp_image251.gif, u multiplicity knot count @image html brep_wp_image252.gif, v multiplicity knot count @image html brep_wp_image253.gif, weight poles, u multiplicity  knots, v multiplicity knots.  
 
The weight poles are @image html brep_wp_image254.gif 3D points @image html brep_wp_image224.gif (@image html brep_wp_image255.gif) if @image html brep_wp_image226.gif. The weight poles are @image html brep_wp_image254.gif pairs @image html brep_wp_image227.gif (@image html brep_wp_image255.gif) if @image html brep_wp_image228.gif. Here @image html brep_wp_image224.gif is a 3D point and @image html brep_wp_image229.gif is a positive real (@image html brep_wp_image255.gif). @image html brep_wp_image230.gif (@image html brep_wp_image255.gif) if @image html brep_wp_image226.gif.  
 
The u multiplicity knots are @image html brep_wp_image252.gif pairs @image html brep_wp_image256.gif. Here @image html brep_wp_image113.gif is a knot with multiplicity @image html brep_wp_image114.gif (@image html brep_wp_image257.gif) so that  
 
@image html brep_wp_image116.gif (@image html brep_wp_image258.gif),  
@image html brep_wp_image259.gif, @image html brep_wp_image260.gif, @image html brep_wp_image261.gif (@image html brep_wp_image262.gif), @image html brep_wp_image263.gif.  
 
The v multiplicity knots are @image html brep_wp_image253.gif pairs @image html brep_wp_image264.gif. Here @image html brep_wp_image265.gif is a knot with multiplicity @image html brep_wp_image266.gif (@image html brep_wp_image267.gif) so that  
 
@image html brep_wp_image268.gif (@image html brep_wp_image269.gif),  
@image html brep_wp_image270.gif, @image html brep_wp_image271.gif, @image html brep_wp_image272.gif (@image html brep_wp_image273.gif), @image html brep_wp_image274.gif.  
 
The B-spline surface is defined by the following  parametric equation:  
 
@image html brep_wp_image275.gif, @image html brep_wp_image276.gif  
 
where functions @image html brep_wp_image277.gif and @image html brep_wp_image278.gif have the following recursion definition  by @image html brep_wp_image279.gif  
 
@image html brep_wp_image127.gif, @image html brep_wp_image128.gif (@image html brep_wp_image280.gif);  
 
@image html brep_wp_image281.gif, @image html brep_wp_image282.gif (@image html brep_wp_image283.gif);  
where  
 
@image html brep_wp_image284.gif (@image html brep_wp_image285.gif,@image html brep_wp_image286.gif),  
 
@image html brep_wp_image287.gif (@image html brep_wp_image288.gif,@image html brep_wp_image289.gif).  
 
The example record is interpreted as a B-spline  surface with a u rational flag @image html brep_wp_image233.gif, v rational flag @image html brep_wp_image234.gif, u degree @image html brep_wp_image290.gif, v degree @image html brep_wp_image236.gif, u pole count @image html brep_wp_image291.gif, v pole count @image html brep_wp_image292.gif, u multiplicity knot count @image html brep_wp_image293.gif, v multiplicity knot count @image html brep_wp_image294.gif, weight poles @image html brep_wp_image295.gif, @image html brep_wp_image296.gif, @image html brep_wp_image297.gif, @image html brep_wp_image298.gif, @image html brep_wp_image299.gif, @image html brep_wp_image300.gif, @image html brep_wp_image301.gif, @image html brep_wp_image302.gif, @image html brep_wp_image303.gif, @image html brep_wp_image304.gif and @image html brep_wp_image305.gif, @image html brep_wp_image306.gif, u multiplicity  knots @image html brep_wp_image142.gif, @image html brep_wp_image143.gif, @image html brep_wp_image144.gif, @image html brep_wp_image145.gif, @image html brep_wp_image146.gif, @image html brep_wp_image147.gif, @image html brep_wp_image148.gif, @image html brep_wp_image149.gif and @image html brep_wp_image150.gif, @image html brep_wp_image151.gif, v multiplicity  knots @image html brep_wp_image307.gif, @image html brep_wp_image308.gif, @image html brep_wp_image309.gif, @image html brep_wp_image310.gif, @image html brep_wp_image311.gif, @image html brep_wp_image312.gif and @image html brep_wp_image313.gif, @image html brep_wp_image314.gif. The B-spline surface is defined  by the following parametric equation:  
 
@image html brep_wp_image315.gif  
 
 
@subsubsection occt_brep_format_4_2_10  surface  record 10 – Rectangular Trim Surface
 
**Example**  

@verbatim
    10 -1 2 -3 4  
    1 1 2 3 0 0 1 1 0 -0 -0 1 0   
@endverbatim
 
**BNF-like Definition**

@verbatim
    surface record 10 = "10" _ trim  surface u min _ trim surface u max _   trim surface v min _ trim surface v max _\n  surface record;  
     
    trim surface u min = real;  
     
    trim surface u max = real;  
     
    trim surface v min = real;  
     
    trim surface v max = real;  
@verbatim
 
**Description**  
 
surface record 10 describes a rectangular trim  surface. The surface data consist of reals @image html brep_wp_image153.gif, @image html brep_wp_image154.gif, @image html brep_wp_image316.gif and @image html brep_wp_image317.gif and a surface record so that @image html brep_wp_image155.gif and @image html brep_wp_image318.gif. The rectangular  trim surface is a restriction of the base surface @image html brep_wp_image156.gif described in the record to the set @image html brep_wp_image319.gif. The rectangular  trim surface is defined by the following parametric equation:  
 
@image html brep_wp_image320.gif, @image html brep_wp_image321.gif.  
 
The example record is interpreted as a rectangular  trim surface to the set @image html brep_wp_image322.gif for the base surface @image html brep_wp_image323.gif. The rectangular  trim surface is defined by the following parametric equation: @image html brep_wp_image324.gif, @image html brep_wp_image325.gif.  
 
 
@subsubsection occt_brep_format_4_2_11 surface record 11 – Offset Surface
 
**Example**  
 
@verbatim
    11 -2  
    1 1 2 3 0 0 1 1 0 -0 -0 1 0   
@verbatim
 
**BNF-like Definition**

@verbatim
    surface record 11 = "11" _ surface  record distance _\n surface record;  
 
    surface record distance = real;  
@verbatim
 
**Description**  
 
surface record 11 describes an offset surface.  
The offset surface data consist of a distance @image html brep_wp_image165.gif and a surface record. The  offset surface is the result of offsetting the base surface @image html brep_wp_image326.gif described in the record to the  distance @image html brep_wp_image165.gif along  the normal @image html brep_wp_image044.gif of  surface @image html brep_wp_image156.gif.  The offset surface is defined by the following parametric equation:  
 
@image html brep_wp_image327.gif, @image html brep_wp_image328.gif.  
 
@image html brep_wp_image329.gif  
if @image html brep_wp_image330.gif.  
 
The example record is interpreted as an offset surface  with a distance @image html brep_wp_image331.gif and  base surface @image html brep_wp_image323.gif.  The offset surface is defined by the following parametric equation: @image html brep_wp_image332.gif.  
 
 
@subsection occt_brep_format_4_3  Subsection 2D curves
 
**Example**  

@verbatim
    Curve2ds 24  
    1 0 0 1 0   
    1 0 0 1 0   
    1 3 0 0 -1   
    1 0 0 0 1   
    1 0 -2 1 0   
    1 0 0 1 0   
    1 0 0 0 -1   
    1 0 0 0 1   
    1 0 0 1 0   
    1 0 1 1 0   
    1 3 0 0 -1   
    1 1 0 0 1   
    1 0 -2 1 0   
    1 0 1 1 0   
    1 0 0 0 -1   
    1 1 0 0 1   
    1 0 0 0 1   
    1 0 0 1 0   
    1 3 0 0 1   
    1 0 0 1 0   
    1 0 0 0 1   
    1 0 2 1 0   
    1 3 0 0 1   
    1 0 2 1 0   
@endverbatim
 
**BNF-like Definition**

@verbatim
    2D curves = 2D curve header  _\n 2D curve records;  
     
    2D curve header = "Curve2ds" _ 2D  curve count;  
     
    2D curve count = int;  
     
    2D curve records = 2D curve record ^  2D curve count;  
     
    2D curve record =  
    2D curve record 1 |  
    2D curve record 2 |  
    2D curve record 3 |  
    2D curve record 4 |  
    2D curve record 5 |  
    2D curve record 6 |  
    2D curve record 7 |  
    2D curve record 8 |  
    2D curve record 9;  
@endverbatim
 
@subsubsection occt_brep_format_4_3_1 2D curve record 1 – Line
 
**Example**  

@verbatim
    1 3 0 0 -1   
@verbatim
 
**BNF-like Definition**

@verbatim
    2D curve record 1 = "1" _ 2D  point _ 2D direction _\n;  
@endverbatim
 
**Description**  
 
2D curve record 1 describes a line. The line  data consist of a 2D point @image html brep_wp_image037.gif and a 2D direction @image html brep_wp_image038.gif. The line passes through the point  @image html brep_wp_image037.gif, has the  direction @image html brep_wp_image038.gif and  is defined by the following parametric equation:  
 
@image html brep_wp_image333.gif, @image html brep_wp_image069.gif.  
 
The example record is interpreted as a line which  passes through a point@image html brep_wp_image334.gif, has a direction @image html brep_wp_image335.gif and is defined by the  following parametric equation: @image html brep_wp_image336.gif.  
 
 
@subsubsection occt_brep_format_4_3_2 2D curve record 2 – Circle
 
**Example**  

@verbatim
    2 1 2 1 0 -0 1 3  
@endverbatim
 
**BNF-like Definition**

@verbatim
    2D curve record 2 = "2" _ 2D  circle center _ 2D circle Dx _ 2D circle Dy  _ 2D circle radius _\n;  
     
    2D circle center = 2D point;  
     
    2D circle Dx = 2D direction;  
     
    2D circle Dy = 2D direction;  
     
    2D circle radius = real;  
@endverbatim
 
**Description**  
 
2D curve record 2 describes a circle. The  circle data consist of a 2D point @image html brep_wp_image037.gif, orthogonal 2D directions @image html brep_wp_image045.gif and @image html brep_wp_image046.gif and a non-negative  real @image html brep_wp_image047.gif. The  circle has a center @image html brep_wp_image037.gif. The circle plane is parallel to  directions @image html brep_wp_image045.gif and  @image html brep_wp_image046.gif. The  circle has a radius @image html brep_wp_image047.gif and is defined by the following  parametric equation:  
 
@image html brep_wp_image337.gif, @image html brep_wp_image338.gif.  
 
The example record is interpreted as a circle which  has a center @image html brep_wp_image339.gif.  The circle plane is parallel to directions @image html brep_wp_image340.gif and @image html brep_wp_image341.gif. The circle has a radius @image html brep_wp_image342.gif and is defined by  the following parametric equation: @image html brep_wp_image343.gif.  
 
 
@subsubsection occt_brep_format_4_3_3 2D curve record 3 – Ellipse
 
**Example**  

@verbatim
    3 1 2 1 0 -0 1 4 3  
@verbatim
 
**BNF-like Definition**

@verbatim
    2D curve record 3 = "3" _ 2D  ellipse center _ 2D ellipse Dmaj _   2D ellipse Dmin _ 2D ellipse Rmaj _ 2D  ellipse Rmin _\n;  
     
    2D ellipse center = 2D point;  
     
    2D ellipse Dmaj = 2D direction;  
     
    2D ellipse Dmin = 2D direction;  
     
    2D ellipse Rmaj = real;  
     
    2D ellipse Rmin = real;  
@endverbatim
 
**Description**  
 
2D curve record 3 describes an ellipse. The  ellipse data are 2D point @image html brep_wp_image037.gif, orthogonal 2D directions @image html brep_wp_image056.gif and @image html brep_wp_image057.gif and non-negative  reals @image html brep_wp_image344.gif and @image html brep_wp_image345.gif that @image html brep_wp_image346.gif. The ellipse has a center  @image html brep_wp_image037.gif, major and  minor axis directions @image html brep_wp_image056.gif and @image html brep_wp_image057.gif, major and minor radii @image html brep_wp_image058.gif and @image html brep_wp_image345.gif and is defined by  the following parametric equation:  
 
@image html brep_wp_image347.gif, @image html brep_wp_image338.gif.  
 
The example record is interpreted as an ellipse which  has a center @image html brep_wp_image339.gif,  major and minor axis directions @image html brep_wp_image348.gif and @image html brep_wp_image349.gif, major and minor radii @image html brep_wp_image350.gif and @image html brep_wp_image351.gif and is defined by  the following parametric equation: @image html brep_wp_image352.gif.  
 
 
@subsubsection occt_brep_format_4_3_4 2D curve record 4 – Parabola
 
**Example**  

@verbatim
    4 1 2 1 0 -0 1 16  
@endverbatim
 
**BNF-like Definition**

@verbatim
    2D curve record 4 = "4" _ 2D  parabola origin _ 2D parabola Dx _   2D parabola Dy _ 2D parabola focal length _\n;  
     
    2D parabola origin = 2D point;  
     
    2D parabola Dx = 2D direction;  
     
    2D parabola Dy = 2D direction;  
     
    2D parabola focal length = real;  
@endverbatim
 
**Description**  
 
2D curve record 4 describes a parabola. The  parabola data consist of a 2D point @image html brep_wp_image037.gif, orthogonal 2D directions @image html brep_wp_image045.gif and @image html brep_wp_image046.gif and a non-negative  real @image html brep_wp_image067.gif. The  parabola coordinate system has its origin @image html brep_wp_image037.gif and axis directions @image html brep_wp_image045.gif and @image html brep_wp_image046.gif. The parabola has a focus  length @image html brep_wp_image067.gif and  is defined by the following parametric equation:  
 
@image html brep_wp_image068.gif, @image html brep_wp_image069.gif &Uuml; @image html brep_wp_image070.gif;  
 
@image html brep_wp_image071.gif, @image html brep_wp_image069.gif &Uuml; @image html brep_wp_image072.gif (degenerated case).  
 
The example record is interpreted as a parabola in  plane which passes through a point @image html brep_wp_image339.gif and is parallel to directions @image html brep_wp_image340.gif and @image html brep_wp_image341.gif. The parabola has a focus  length @image html brep_wp_image077.gif and  is defined by the following parametric equation: @image html brep_wp_image353.gif.  
 
 
@subsubsection occt_brep_format_4_3_5 2D curve record 5 – Hyperbola
 
**Example**  
 
5 1 2 1 0 -0 1 3 4  
 

**BNF-like Definition**

@verbatim
    2D curve record 5 = "5" _ 2D  hyperbola origin _ 2D hyperbola Dx _   2D hyperbola Dy _ 2D hyperbola Kx _ 2D  hyperbola Ky _\n;  
     
    2D hyperbola origin = 2D point;  
     
    2D hyperbola Dx = 2D direction;  
     
    2D hyperbola Dy = 2D direction;  
     
    2D hyperbola Kx = real;  
     
    2D hyperbola Ky = real;  
@endverbatim
 
**Description**  
 
2D curve record 5 describes a hyperbola. The  hyperbola data consist of a 2D point @image html brep_wp_image037.gif, orthogonal 2D directions @image html brep_wp_image045.gif and @image html brep_wp_image046.gif and non-negative  reals @image html brep_wp_image079.gif and @image html brep_wp_image080.gif. The hyperbola  coordinate system has origin @image html brep_wp_image037.gif and axis directions @image html brep_wp_image045.gif and @image html brep_wp_image046.gif. The hyperbola is defined by  the following parametric equation:  
 
@image html brep_wp_image081.gif, @image html brep_wp_image069.gif.  
 
The example record is interpreted as a hyperbola with  coordinate system which has origin @image html brep_wp_image339.gif and axis directions @image html brep_wp_image354.gif and @image html brep_wp_image341.gif. Other data for the hyperbola  are @image html brep_wp_image082.gif and @image html brep_wp_image083.gif. The hyperbola is defined  by the following parametric equation: @image html brep_wp_image355.gif.  
 
 
@subsubsection occt_brep_format_4_3_6 2D curve record 6 – Bezier Curve
 
**Example**  

@verbatim
6 1 2 0 1  4 1 -2  5 2 3  6   
@endverbatim
 
**BNF-like Definition**

@verbatim
2D curve record 6 = "6" _ 2D  Bezier rational flag _ 2D Bezier degree   2D Bezier weight poles _\n;  
 
2D Bezier rational flag = flag;  
 
2D Bezier degree = int;  
 
2D Bezier weight poles = (_ 2D  Bezier weight pole) ^ (2D Bezier degree + "1");  
 
2D Bezier weight pole = 2D point  [_ real];  
@endverbatim
 
**Description**  
 
2D curve record 6 describes a Bezier curve.  The curve data consist of a rational flag @image html brep_wp_image047.gif, a degree @image html brep_wp_image085.gif and weight poles.  
 
The weight poles are @image html brep_wp_image086.gif 2D points @image html brep_wp_image087.gif if the flag @image html brep_wp_image047.gif is 0. The weight  poles are @image html brep_wp_image086.gif pairs  @image html brep_wp_image088.gif if the flag  @image html brep_wp_image047.gif is 1. Here @image html brep_wp_image089.gif is a 2D point and @image html brep_wp_image090.gif is a positive real (@image html brep_wp_image093.gif). @image html brep_wp_image092.gif (@image html brep_wp_image093.gif) if the flag @image html brep_wp_image047.gif is 0.  
 
The Bezier curve is defined by the following  parametric equation:  
 
@image html brep_wp_image094.gif, @image html brep_wp_image095.gif  
 
where @image html brep_wp_image096.gif.  
 
The example record is interpreted as a Bezier curve  with a rational flag @image html brep_wp_image097.gif, a degree @image html brep_wp_image098.gif and weight poles @image html brep_wp_image356.gif, @image html brep_wp_image100.gif, @image html brep_wp_image357.gif, @image html brep_wp_image102.gif and @image html brep_wp_image358.gif, @image html brep_wp_image104.gif. The Bezier curve is defined  by the following parametric equation:  
 
@image html brep_wp_image359.gif.  
 
 
@subsubsection occt_brep_format_4_3_7 2D curve record 7 – B-spline Curve
 
**Example**  

@verbatim
7 1 0  1 3 5  0 1  4 1 -2  5  2 3  6  
 0 1 0.25 1 0.5 1 0.75 1 1 1  
@endverbatim
 
**BNF-like Definition**

@verbatim
    2D curve record 7 = "7" _ 2D  B-spline rational flag _ "0" _ 2D B-spline degree _  2D B-spline pole count _ 2D B-spline multiplicity knot  count 2D B-spline weight poles _\n 2D B-spline  multiplicity knots _\n;  
     
    2D B-spline rational flag = flag;  
     
    2D B-spline degree = int;  
     
    2D B-spline pole count = int;  
     
    2D B-spline multiplicity knot count = int;  
     
    2D B-spline weight poles = 2D B-spline  weight pole ^ 2D B-spline pole count;  
     
    2D B-spline weight pole = _ 2D  point [_ real];  
     
    2D B-spline multiplicity knots =  
    2D B-spline multiplicity knot ^ 2D  B-spline multiplicity knot count;  
     
    2D B-spline multiplicity knot = _  real _ int;  
@endverbatim
 
**Description**  
 
2D curve record 7 describes a B-spline curve.  The curve data consist of a rational flag @image html brep_wp_image047.gif, a degree @image html brep_wp_image085.gif, a pole count @image html brep_wp_image106.gif, a multiplicity knot count @image html brep_wp_image107.gif, weight poles and  multiplicity knots.  
 
The weight poles are @image html brep_wp_image108.gif 2D points @image html brep_wp_image109.gif if the flag @image html brep_wp_image047.gif is 0. The weight  poles are @image html brep_wp_image108.gif pairs  @image html brep_wp_image110.gif if the flag  @image html brep_wp_image047.gif is 1. Here @image html brep_wp_image089.gif is a 2D point and @image html brep_wp_image090.gif is a positive real (@image html brep_wp_image111.gif). @image html brep_wp_image092.gif (@image html brep_wp_image111.gif) if the flag @image html brep_wp_image047.gif is 0.  
 
The multiplicity knots are @image html brep_wp_image107.gif pairs @image html brep_wp_image112.gif. Here @image html brep_wp_image113.gif is a knot with multiplicity @image html brep_wp_image114.gif (@image html brep_wp_image115.gif) so that  
 
@image html brep_wp_image360.gif (@image html brep_wp_image361.gif),  
@image html brep_wp_image362.gif, @image html brep_wp_image363.gif, @image html brep_wp_image364.gif (@image html brep_wp_image365.gif), @image html brep_wp_image366.gif.  
 
The B-spline curve is defined by the following  parametric equation:  
 
@image html brep_wp_image367.gif, @image html brep_wp_image368.gif  
 
where functions @image html brep_wp_image125.gif have the following recursion definition  by @image html brep_wp_image126.gif  
 
@image html brep_wp_image127.gif, @image html brep_wp_image128.gif (@image html brep_wp_image129.gif)  
 
where  
 
@image html brep_wp_image284.gif (@image html brep_wp_image369.gif,@image html brep_wp_image286.gif).  
 
The example record is interpreted as a B-spline curve  with a rational flag @image html brep_wp_image097.gif, a degree @image html brep_wp_image133.gif, a pole count @image html brep_wp_image134.gif, a multiplicity knot count @image html brep_wp_image135.gif, weight poles @image html brep_wp_image370.gif, @image html brep_wp_image137.gif, @image html brep_wp_image371.gif, @image html brep_wp_image139.gif and @image html brep_wp_image372.gif, @image html brep_wp_image141.gif and multiplicity knots @image html brep_wp_image373.gif, @image html brep_wp_image374.gif, @image html brep_wp_image375.gif, @image html brep_wp_image376.gif, @image html brep_wp_image377.gif, @image html brep_wp_image378.gif, @image html brep_wp_image379.gif, @image html brep_wp_image380.gif and @image html brep_wp_image381.gif, @image html brep_wp_image382.gif. The B-spline curve is defined  by the following parametric equation:  
 
@image html brep_wp_image383.gif.  
 
 
@subsubsection occt_brep_format_4_3_8 2D curve record 8 – Trimmed Curve
 
**Example**  

@verbatim
    8 -4 5  
    1 1 2 1 0   
@endverbatim
 
**BNF-like Definition**

@verbatim
    2D curve record 8 = "8" _ 2D  trimmed curve u min _ 2D trimmed curve u max _\n   2D curve record;  
 
    2D trimmed curve u min = real;  
 
    2D trimmed curve u max = real;  
@endverbatim
 
**Description**  
 
2D curve record 8 describes a trimmed curve.  The trimmed curve data consist of reals @image html brep_wp_image153.gif and @image html brep_wp_image154.gif and a 2D curve record so that @image html brep_wp_image155.gif. The trimmed curve  is a restriction of the base curve @image html brep_wp_image326.gif described in the record to the segment @image html brep_wp_image157.gif. The trimmed curve is  defined by the following parametric equation:  
 
@image html brep_wp_image158.gif, @image html brep_wp_image159.gif.  
 
The example record is interpreted as a trimmed curve with @image html brep_wp_image160.gif, @image html brep_wp_image161.gif and base curve @image html brep_wp_image384.gif. The trimmed curve is  defined by the following parametric equation: @image html brep_wp_image385.gif, @image html brep_wp_image164.gif.  
 
 
@subsubsection occt_brep_format_4_3_9 2D curve record 9 – Offset Curve
 
**Example**  

@verbatim
    9 2  
    1 1 2 1 0   
@endverbatim
 
**BNF-like Definition**
 
@verbatim
2D curve record 9 = "9" _ 2D  offset curve distance _\n 2D curve record;  
 
2D offset curve distance = real;  
@endverbatim
 
**Description**  
 
2D curve record 9 describes an offset curve.  The offset curve data consist of a distance @image html brep_wp_image165.gif and a 2D curve record. The  offset curve is the result of offsetting the base curve @image html brep_wp_image156.gif described in the record to the  distance @image html brep_wp_image165.gif along  the vector @image html brep_wp_image386.gif where  @image html brep_wp_image387.gif. The offset  curve is defined by the following parametric equation:  
 
@image html brep_wp_image388.gif, @image html brep_wp_image168.gif.  
 
The example record is interpreted as an offset curve  with a distance 
@image html brep_wp_image169.gif and  base curve @image html brep_wp_image384.gif and 
is defined by the following parametric equation: @image html brep_wp_image389.gif.  
 
@subsection occt_brep_format_4_4 Subsection 3D polygons
 
**Example**  

@verbatim
    Polygon3D 1  
    2 1  
    0.1  
    1 0 0 2 0 0   
    0 1   
@endverbatim
 
**BNF-like Definition**

@verbatim
    3D polygons = 3D polygon header  _\n 3D polygon records;  
     
    3D polygon header = "Polygon3D" _  3D polygon record count;  
     
    3D polygon records = 3D polygon record  ^ 3D polygon record count;  
     
    3D polygon record =  
    3D polygon node count _ 3D polygon  flag of parameter presence _\n  
    3D polygon deflection _\n  
    3D polygon nodes _\n  
    [3D polygon parameters _\n];  
     
    3D polygon node count = int;  
     
    3D polygon flag of parameter presence =  flag;  
     
    3D polygon deflection = real;  
     
    3D polygon nodes = (3D polygon node  _) ^ 3D polygon node count;  
     
    3D polygon node = 3D point;  
     
    3D polygon u parameters = (3D polygon u  parameter _) ^ 3D polygon node count;  
     
    3D polygon u parameter = real;  
@endverbatim
 
**Description**  
 
3D polygon record describes a 3D polyline @image html brep_wp_image390.gif which approximates a  3D curve @image html brep_wp_image205.gif.  The polyline data consist of a node count @image html brep_wp_image391.gif, a parameter presence flag @image html brep_wp_image392.gif, a deflection @image html brep_wp_image393.gif, nodes @image html brep_wp_image394.gif (@image html brep_wp_image395.gif) and parameters @image html brep_wp_image113.gif (@image html brep_wp_image395.gif). The parameters are present  only if@image html brep_wp_image396.gif. The  polyline @image html brep_wp_image390.gif passes  through the nodes. The deflection @image html brep_wp_image165.gif describes the deflection of polyline @image html brep_wp_image390.gif from the curve @image html brep_wp_image205.gif:  
 
@image html brep_wp_image397.gif.  
 
The parameter @image html brep_wp_image113.gif (@image html brep_wp_image395.gif) is the parameter of the node @image html brep_wp_image394.gif on the curve @image html brep_wp_image205.gif:  
 
@image html brep_wp_image398.gif.  
 
The example record describes a polyline from @image html brep_wp_image098.gif nodes with a parameter  presence flag @image html brep_wp_image396.gif,  a deflection @image html brep_wp_image399.gif,  nodes @image html brep_wp_image400.gif and @image html brep_wp_image401.gif and parameters @image html brep_wp_image142.gif and @image html brep_wp_image402.gif.  
 
 
@subsection occt_brep_format_4_5 Subsection triangulations
 
**Example**  

@verbatim
    Triangulations 6  
    4 2 1 0  
    0 0 0 0 0 3 0 2 3 0 2 0 0 0 3  0 3 -2 0 -2 2 4 3 2 1 4   
    4 2 1 0  
    0 0 0 1 0 0 1 0 3 0 0 3 0 0 0  1 3 1 3 0 3 2 1 3 1 4   
    4 2 1 0  
    0 0 3 0 2 3 1 2 3 1 0 3 0 0 0  2 1 2 1 0 3 2 1 3 1 4   
    4 2 1 0  
    0 2 0 1 2 0 1 2 3 0 2 3 0 0 0  1 3 1 3 0 3 2 1 3 1 4   
    4 2 1 0  
    0 0 0 0 2 0 1 2 0 1 0 0 0 0 0  2 1 2 1 0 3 2 1 3 1 4   
    4 2 1 0  
    1 0 0 1 0 3 1 2 3 1 2 0 0 0 3  0 3 -2 0 -2 2 4 3 2 1 4   
@endverbatim
 
**BNF-like Definition**

@verbatim
    triangulations = triangulation header  _\n triangulation records;  
     
    triangulation header = "Triangulations"  _ triangulation count;  
     
    triangulation records = triangulation  record ^ triangulation count;  
     
    triangulation record = triangulation node  count _ triangulation triangle count _ triangulation  parameter presence flag _ triangulation deflection  _\n   triangulation nodes [_ triangulation u v parameters]  _ triangulation triangles _\n;  
     
    triangulation node count = int;  
     
    triangulation triangle count = int;  
     
    triangulation parameter presence flag =  flag;  
     
    triangulation deflection = real;  
     
    triangulation nodes = (triangulation  node _) ^ triangulation node count;  
     
    triangulation node = 3D point;  
     
    triangulation u v parameters =             (triangulation u v parameter pair _) ^ triangulation node  count;  
     
    triangulation u v parameter pair =  real _ real;  
     
    triangulation triangles = (triangulation  triangle _) ^ triangulation triangle count;  
     
    triangulation triangle = int _  int _ int.  
@endverbatim
 
**Description**  
 
triangulation record describes a triangulation  @image html brep_wp_image403.gif which  approximates a surface @image html brep_wp_image404.gif. The triangulation data consist of a node  count @image html brep_wp_image405.gif, a triangle  count @image html brep_wp_image406.gif, a parameter  presence flag @image html brep_wp_image392.gif,  a deflection @image html brep_wp_image393.gif,  nodes @image html brep_wp_image394.gif (@image html brep_wp_image395.gif), parameter pairs @image html brep_wp_image407.gif (@image html brep_wp_image395.gif), triangles @image html brep_wp_image408.gif (@image html brep_wp_image131.gif, @image html brep_wp_image409.gif (@image html brep_wp_image410.gif)). The parameters are present  only if @image html brep_wp_image396.gif.  The deflection describes the triangulation deflection from the surface:  
 
@image html brep_wp_image411.gif.  
 
The parameter pair @image html brep_wp_image407.gif (@image html brep_wp_image395.gif) describes the parameters of node @image html brep_wp_image394.gif on the surface:  
 
@image html brep_wp_image412.gif.  
 
The triangle @image html brep_wp_image408.gif (@image html brep_wp_image131.gif) is interpreted as a triangle of nodes @image html brep_wp_image413.gif, @image html brep_wp_image414.gif and @image html brep_wp_image415.gif with circular traversal of  the nodes in the order @image html brep_wp_image413.gif, @image html brep_wp_image414.gif and @image html brep_wp_image415.gif. From any side of the triangulation @image html brep_wp_image403.gif all its triangles  have the same direction of the node circular traversal: either clockwise or  counterclockwise.  
 
Triangulation record  

@verbatim 
    4 2 1 0  
    0 0 0 0 0 3 0 2 3 0 2 0 0 0 3  0 3 -2 0 -2 2 4 3 2 1 4   
@endverbatim

describes a triangulation with @image html brep_wp_image416.gif nodes, @image html brep_wp_image417.gif triangles, parameter presence  flag @image html brep_wp_image396.gif,  deflection @image html brep_wp_image418.gif,  nodes @image html brep_wp_image419.gif, @image html brep_wp_image420.gif, @image html brep_wp_image421.gif and @image html brep_wp_image422.gif, parameters @image html brep_wp_image423.gif, @image html brep_wp_image424.gif, @image html brep_wp_image425.gif and @image html brep_wp_image426.gif, and triangles @image html brep_wp_image427.gif and @image html brep_wp_image428.gif. From the point @image html brep_wp_image429.gif (@image html brep_wp_image430.gif) the triangles have clockwise  (counterclockwise) direction of the node circular traversal.  
 
 
@subsection occt_brep_format_4_6 Subsection polygons on triangulations
 
**Example**  

@verbatim
    PolygonOnTriangulations 24  
    2 1 2   
    p 0.1 1 0 3   
    2 1 4   
    p 0.1 1 0 3   
    2 2 3   
    p 0.1 1 0 2   
    2 1 2   
    p 0.1 1 0 2   
    2 4 3   
    p 0.1 1 0 3   
    2 1 4   
    p 0.1 1 0 3   
    2 1 4   
    p 0.1 1 0 2   
    2 1 2   
    p 0.1 1 0 2   
    2 1 2   
    p 0.1 1 0 3   
    2 2 3   
    p 0.1 1 0 3   
    2 2 3   
    p 0.1 1 0 2   
    2 4 3   
    p 0.1 1 0 2   
    2 4 3   
    p 0.1 1 0 3   
    2 2 3   
    p 0.1 1 0 3   
    2 1 4   
    p 0.1 1 0 2   
    2 4 3   
    p 0.1 1 0 2   
    2 1 2   
    p 0.1 1 0 1   
    2 1 4   
    p 0.1 1 0 1   
    2 4 3   
    p 0.1 1 0 1   
    2 1 4   
    p 0.1 1 0 1   
    2 1 2   
    p 0.1 1 0 1   
    2 2 3   
    p 0.1 1 0 1   
    2 4 3   
    p 0.1 1 0 1   
    2 2 3   
    p 0.1 1 0 1   
@endverbatim
 
**BNF-like Definition**

@verbatim
    polygons on triangulations = polygons on  triangulations header _\n  
    polygons on triangulations records;  
     
    polygons on triangulations header =  
    "PolygonOnTriangulations" _ polygons on  triangulations record count;  
     
    polygons on triangulations record count =  int;  
     
    polygons on triangulations records =  
    polygons on triangulations record ^ polygons  on triangulations record count;  
     
    polygons on triangulations record =  
    polygons on triangulations node count  _ polygons on triangulations node numbers _\n  
    "p" _ polygons on triangulations  deflection _  
    polygons on triangulations parameter presence  flag  
    [_ polygons on triangulations u  parameters] _\n;  
     
    polygons on triangulations node count =  int;  
     
    polygons on triangulations node numbers =  
    polygons on triangulations node number ^  polygons on triangulations node count;  
     
    polygons on triangulations node number =  int;  
     
    polygons on triangulations deflection =  real;  
     
    polygons on triangulations parameter presence  flag = flag;  
     
    polygons on triangulations u parameters =  
    (polygons on triangulations u parameter  _) ^ polygons on triangulations node count;  
     
    polygons on triangulations u parameter =  real;  
@endverbatim
 
**Description**  
 
polygons on triangulations describes a polyline  @image html brep_wp_image390.gif on a triangulation  which approximates a curve @image html brep_wp_image205.gif. The polyline data consist of a node  count @image html brep_wp_image391.gif,  node numbers @image html brep_wp_image431.gif,  deflection @image html brep_wp_image393.gif,  a parameter presence flag @image html brep_wp_image392.gif and parameters @image html brep_wp_image432.gif (@image html brep_wp_image395.gif). The parameters are present  only if @image html brep_wp_image396.gif. The  deflection @image html brep_wp_image165.gif describes  the deflection of polyline @image html brep_wp_image390.gif from the curve @image html brep_wp_image205.gif:  
 
@image html brep_wp_image397.gif.  
 
Parameter @image html brep_wp_image113.gif (@image html brep_wp_image395.gif) is @image html brep_wp_image433.gif-th node @image html brep_wp_image434.gif parameter on curve @image html brep_wp_image205.gif.  
 
 
@subsection occt_brep_format_4_7 Geometric Sense of a Curve
 
Geometric sense of curve @image html brep_wp_image210.gif described above is determined by the direction  of parameter @image html brep_wp_image435.gif increasing.  
 
 
@section occt_brep_format_5 Section shapes
 
An example of section shapes and a whole  *.brep file are given in chapter 7 "Appendix".  
 
 
**BNF-like Definition**

@verbatim
    shapes = shape header  _\n shape records _\n shape final record;  
     
    shape header = "TShapes" _  shape count;  
     
    shape count = int;  
     
    shape records = shape  record ^ shape count;  
     
    shape record = shape subrecord  _\n shape flag word _\n shape subshapes  _\n;  
     
    shape flag word = flag ^  7;  
     
    shape subshapes = (shape  subshape _)* "*";  
     
    shape subshape =  
    shape subshape orientation shape subshape  number _ shape location number;  
     
    shape subshape orientation = "+" |  "-" | "i" | "e";  
     
    shape subshape number =  int;  
     
    shape location number =  int;  
     
    shape final record = shape  subshape;  
     
    shape subrecord =  
    ("Ve" _\n vertex data  _\n) |  
    ("Ed" _\n edge data  _\n) |  
    ("Wi" _\n _\n) |  
    ("Fa" _\n face data) |  
    ("Sh" _\n _\n) |  
    ("So" _\n _\n) |  
    ("CS" _\n _\n) |  
    ("Co" _\n _\n);  
@endverbatim
 
**Description**  
 
shape flag word @image html brep_wp_image436.gif flags @image html brep_wp_image437.gif 
(@image html brep_wp_image438.gif) are interpreted as shape  flags in the following way:  

  *  @image html brep_wp_image439.gif – free;  
  *  @image html brep_wp_image440.gif – modified;  
  *  @image html brep_wp_image441.gif – IGNORED (version   *  / checked  (version   * ;  
  *  @image html brep_wp_image442.gif – orientable;  
  *  @image html brep_wp_image443.gif – closed;  
  *  @image html brep_wp_image444.gif – infinite;  
  *  @image html brep_wp_image445.gif – convex.  
  
The flags are used in a special way [1].  
 
shape subshape orientation is interpreted in  the following way:  

  *  + – forward;  
  *  - – reversed;  
  *  i – internal;  
  *  e – external.  
  
shape subshape orientation is used in a special  way [1].  
 
shape subshape number is the number of a shape  record which is located in this section above the shape subshape  number. shape record numbering is backward and starts from 1.  
 
shape subrecord types are interpreted in the  following way: 
 
  *  "Ve" – vertex;  
  *  "Ed" – edge;  
  *  "Wi" – wire;  
  *  "Fa" – face;  
  *  "Sh" – shell;  
  *  "So" – solid;  
  *  "CS" – compsolid;  
  *  "Co" – compound.  
 
shape final record determines the orientation  and location for the whole model.  
 
@subsection occt_brep_format_5_1  Common Terms
 
The terms below are used by vertex data, edge  data and face data.  
 
 
**BNF-like Definition**

@verbatim
    location number = int;  
     
    3D curve number = int;  
     
    surface number = int;  
     
    2D curve number = int;  
     
    3D polygon number = int;  
     
    triangulation number =  int;  
     
    polygon on triangulation number =  int;  
     
    curve parameter minimal and maximal  values = real _ real;  
     
    curve values for parameter minimal and maximal  values =  
    real _ real _ real  _ real;  
@endverbatim
 
**Description**  
 
location number is the number of location  record from section locations. location record numbering  starts from 1. location number 0 is interpreted  as the identity location.  
 
3D curve number is the number of a 3D  curve record from subsection 3D curves of section geometry.  3D curve record numbering starts from 1.  
 
surface number is the number of a surface  record from subsection surfaces of section geometry. surface  record numbering starts from 1.  
 
2D curve number is the number of a 2D  curve record from subsection 2D curves of section geometry.  2D curve record numbering starts from 1.  
 
3D polygon number is the number of a 3D  polygon record from subsection 3D polygons of section geometry.  3D polygon record numbering starts from 1.  
 
triangulation number is the number of a triangulation  record from subsection triangulations of section geometry. triangulation  record numbering starts from 1.  
 
polygon on triangulation number is the number  of a polygons on triangulations record from subsection polygons on  triangulations of section geometry.  
polygons on triangulations record numbering  starts from 1.  
 
curve parameter minimal and maximal values @image html brep_wp_image446.gif and @image html brep_wp_image447.gif are the curve  parameter@image html brep_wp_image448.gif  bounds: @image html brep_wp_image449.gif.  
 
curve  values for parameter minimal and maximal values @image html brep_wp_image446.gif and @image html brep_wp_image447.gif are real pairs @image html brep_wp_image450.gif and @image html brep_wp_image451.gif that @image html brep_wp_image452.gif and @image html brep_wp_image453.gif where @image html brep_wp_image205.gif is a parametric  equation of the curve.  
 
 
@subsection occt_brep_format_5_2 vertex data
 
**BNF-like Definition**

@verbatim
    vertex data = vertex data tolerance  _\n vertex data 3D representation _\n   vertex data representations;  
     
    vertex data tolerance = real;  
     
    vertex data 3D representation = 3D point;  
     
    vertex data representations = (vertex data  representation _\n)* "0 0";  
     
    vertex data representation = vertex data  representation u parameter _  
    vertex data representation data _  location number;  
     
    vertex data representation u parameter =  real;  
     
    vertex data representation data =  
    ("1" _ vertex data representation data    *  |  
    ("2" _ vertex data representation data    *  |  
    ("3" _ vertex data representation data    * ;  
     
    vertex data representation data 1 = 3D  curve number;  
     
    vertex data representation data 2 = 2D  curve number _ surface number;  
     
    vertex data representation data 3 =  
    vertex data representation v parameter  _ surface number;  
     
    vertex data representation v parameter =  real;  
@endverbatim
 
**Description**  
 
The usage of vertex data representation u  parameter @image html brep_wp_image448.gif is  described below.  
 
vertex data representation data 1 and  parameter @image html brep_wp_image448.gif describe  the position of the vertex @image html brep_wp_image454.gif on a 3D curve @image html brep_wp_image205.gif. Parameter @image html brep_wp_image448.gif is a parameter of the  vertex @image html brep_wp_image454.gif on the  curve @image html brep_wp_image205.gif: @image html brep_wp_image455.gif.  
 
vertex data representation data 2 and  parameter @image html brep_wp_image448.gif describe  the position of the vertex @image html brep_wp_image454.gif on a 2D curve @image html brep_wp_image205.gif which is located on a  surface. Parameter @image html brep_wp_image448.gif is  a parameter of the vertex @image html brep_wp_image454.gif on the curve @image html brep_wp_image205.gif: @image html brep_wp_image455.gif.  
 
vertex data representation data 3 and  parameter @image html brep_wp_image435.gif describe  the position of the vertex @image html brep_wp_image454.gif on a surface @image html brep_wp_image404.gif through vertex data  representation v parameter @image html brep_wp_image456.gif: @image html brep_wp_image457.gif.  
 
vertex data tolerance @image html brep_wp_image458.gif describes the maximum distance  from the vertex @image html brep_wp_image454.gif to  the set @image html brep_wp_image459.gif* *of vertex @image html brep_wp_image454.gif representations:  
 
@image html brep_wp_image460.gif.  
 
 
@subsection occt_brep_format_5_3 edge data
 
**BNF-like Definition**

@verbatim
    edge data = _ edge data tolerance  _ edge data same parameter flag _ edge data same range  flag _ edge data degenerated flag _\n edge data  representations;  
     
    edge data tolerance = real;  
     
    edge data same parameter flag = flag;  
     
    edge data same range flag = flag;  
     
    edge data degenerated flag = flag;  
     
    edge data representations = (edge data  representation _\n)* "0";  
     
    edge data representation =  
    "1" _ edge data representation data 1  
    "2" _ edge data representation data 2  
    "3" _ edge data representation data 3  
    "4" _ edge data representation data 4  
    "5" _ edge data representation data 5  
    "6" _ edge data representation data 6  
    "7" _ edge data representation data 7;  
     
    edge data representation data 1 = 3D curve  number _ location number _  
    curve parameter minimal and maximal values;  
     
    edge data representation data 2 = 2D curve  number _ surface number _   location number _ curve parameter minimal and maximal values  
    [_\n curve values for parameter minimal  and maximal values];  
     
    edge data representation data 3 = (2D  curve number _) ^ 2 continuity order _ surface  number _ location number _ curve parameter  minimal and maximal values \n curve values for parameter  minimal and maximal values];  
     
    continuity order = "C0" | "C1" | "C2" | "C3" |  "CN" | "G1" | "G2".  
     
    edge data representation data 4 =  
    continuity order (_ surface number  _ location number) ^ 2;  
     
    edge data representation data 5 = 3D  polygon number _ location number;  
     
    edge data representation data 6 =  
    polygon on triangulation number _ triangulation  number _ location number;  
     
    edge data representation data 7 = (polygon  on triangulation number _) ^ 2   triangulation number _ location number;  
@endverbatim
 
**Description**  
 
Flags edge data same parameter flag, edge  data same range flag and edge data degenerated flag are used in a special  way [1].  
 
edge data representation data 1 describes a 3D  curve.  
 
edge data representation data 2 describes a 2D  curve on a surface.  
curve values for parameter minimal and maximal  values are used only in version 2.  
 
edge data representation data 3 describes a 2D  curve on a closed surface.  
curve values for parameter minimal and maximal  values are used only in version 2.  
 
edge data representation data 5 describes a 3D  polyline.  
 
edge data representation data 6 describes a polyline  on a triangulation.  
 
edge data tolerance @image html brep_wp_image458.gif describes the maximum distance  from the edge @image html brep_wp_image461.gif to  the set @image html brep_wp_image459.gif of  edge @image html brep_wp_image461.gif representations:  
 
@image html brep_wp_image462.gif.  
 
 
@subsection occt_brep_format_5_4  face data
 
**BNF-like Definition**

@verbatim
    face data = face data natural restriction  flag _ face data tolerance _ surface number  _ location number \n ["2" _ triangulation  number];  
     
    face data natural restriction flag =  flag;  
     
    face data tolerance = real;  
@endverbatim
 
**Description**  
 
face data describes a surface @image html brep_wp_image404.gif of face @image html brep_wp_image463.gif and a triangulation @image html brep_wp_image403.gif of face @image html brep_wp_image463.gif. The surface @image html brep_wp_image404.gif may be empty: surface  number = 0.  
 
face data tolerance @image html brep_wp_image458.gif describes the maximum distance  from the face @image html brep_wp_image463.gif to  the surface @image html brep_wp_image404.gif:  
 
@image html brep_wp_image464.gif.  

  Flag face data natural restriction flag is  used in a special way [1].  

@section occt_brep_format_6 References
 
  The format is part of Open CASCADE Technology (OCCT).
  Some data fields of the format have additional values which are used in OCCT. 
  Some data fields of the format are specific for OCCT.
 
@section occt_brep_format_7 Appendix
 
  This chapter contains a *.brep file example.  
 
@verbatim
    DBRep_DrawableShape  
     
    CASCADE Topology V1, (c)  Matra-Datavision  
    Locations 3  
    1  
                  0                0               1               0   
                  1                0               0               0   
                  0                1               0               0   
    1  
                  1                0               0               4   
                  0                1               0               5   
                  0                0               1               6   
    2  1 1 2 1 0  
    Curve2ds 24  
    1 0 0 1 0   
    1 0 0 1 0   
    1 3 0 0 -1   
    1 0 0 0 1   
    1 0 -2 1 0   
    1 0 0 1 0   
    1 0 0 0 -1   
    1 0 0 0 1   
    1 0 0 1 0   
    1 0 1 1 0   
    1 3 0 0 -1   
    1 1 0 0 1   
    1 0 -2 1 0   
    1 0 1 1 0   
    1 0 0 0 -1   
    1 1 0 0 1   
    1 0 0 0 1   
    1 0 0 1 0   
    1 3 0 0 1   
    1 0 0 1 0   
    1 0 0 0 1   
    1 0 2 1 0   
    1 3 0 0 1   
    1 0 2 1 0   
    Curves 13  
    1 0 0 0 0 0 1   
    1 0 0 3 -0 1 0   
    1 0 2 0 0 0 1   
    1 0 0 0 -0 1 0   
    1 1 0 0 0 0 1   
    1 1 0 3 0 1 0   
    1 1 2 0 0 0 1   
    1 1 0 0 -0 1 0   
    1 0 0 0 1 0 -0   
    1 0 0 3 1 0 -0   
    1 0 2 0 1 0 -0   
    1 0 2 3 1 0 -0   
    1 1 0 0 1 0 0   
    Polygon3D 1  
    2 1  
    0.1  
    1 0 0 2 0 0   
    0 1   
    PolygonOnTriangulations 24  
    2 1 2   
    p 0.1 1 0 3   
    2 1 4   
    p 0.1 1 0 3   
    2 2 3   
    p 0.1 1 0 2   
    2 1 2   
    p 0.1 1 0 2   
    2 4 3   
    p 0.1 1 0 3   
    2 1 4   
    p 0.1 1 0 3   
    2 1 4   
    p 0.1 1 0 2   
    2 1 2   
    p 0.1 1 0 2   
    2 1 2   
    p 0.1 1 0 3   
    2 2 3   
    p 0.1 1 0 3   
    2 2 3   
    p 0.1 1 0 2   
    2 4 3   
    p 0.1 1 0 2   
    2 4 3   
    p 0.1 1 0 3   
    2 2 3   
    p 0.1 1 0 3   
    2 1 4   
    p 0.1 1 0 2   
    2 4 3   
    p 0.1 1 0 2   
    2 1 2   
    p 0.1 1 0 1   
    2 1 4   
    p 0.1 1 0 1   
    2 4 3   
    p 0.1 1 0 1   
    2 1 4   
    p 0.1 1 0 1   
    2 1 2   
    p 0.1 1 0 1   
    2 2 3   
    p 0.1 1 0 1   
    2 4 3   
    p 0.1 1 0 1   
    2 2 3   
    p 0.1 1 0 1   
    Surfaces 6  
    1 0 0 0 1 0 -0 0 0 1 0 -1 0   
    1 0 0 0 -0 1 0 0 0 1 1 0 -0   
    1 0 0 3 0 0 1 1 0 -0 -0 1 0   
    1 0 2 0 -0 1 0 0 0 1 1 0 -0   
    1 0 0 0 0 0 1 1 0 -0 -0 1 0   
    1 1 0 0 1 0 -0 0 0 1 0 -1 0   
    Triangulations 6  
    4 2 1 0  
    0 0 0 0 0 3 0 2 3 0 2 0 0 0 3  0 3 -2 0 -2 2 4 3 2 1 4   
    4 2 1 0  
    0 0 0 1 0 0 1 0 3 0 0 3 0 0 0  1 3 1 3 0 3 2 1 3 1 4   
    4 2 1 0  
    0 0 3 0 2 3 1 2 3 1 0 3 0 0 0  2 1 2 1 0 3 2 1 3 1 4   
    4 2 1 0  
    0 2 0 1 2 0 1 2 3 0 2 3 0 0 0  1 3 1 3 0 3 2 1 3 1 4   
    4 2 1 0  
    0 0 0 0 2 0 1 2 0 1 0 0 0 0 0  2 1 2 1 0 3 2 1 3 1 4   
    4 2 1 0  
    1 0 0 1 0 3 1 2 3 1 2 0 0 0 3  0 3 -2 0 -2 2 4 3 2 1 4   
     
    TShapes 39  
    Ve  
    1e-007  
    0 0 3  
    0 0  
     
    0101101  
    *  
    Ve  
    1e-007  
    0 0 0  
    0 0  
     
    0101101  
    *  
    Ed  
     1e-007 1 1 0  
    1  1 0 0 3  
    2  1 1 0 0 3  
    2  2 2 0 0 3  
    6  1 1 0  
    6  2 2 0  
    0  
     
    0101000  
    -39 0 +38 0 *  
    Ve  
    1e-007  
    0 2 3  
    0 0  
     
    0101101  
    *  
    Ed  
     1e-007 1 1 0  
    1  2 0 0 2  
    2  3 1 0 0 2  
    2  4 3 0 0 2  
    6  3 1 0  
    6  4 3 0  
    0  
     
    0101000  
    -36 0 +39 0 *  
    Ve  
    1e-007  
    0 2 0  
    0 0  
     
    0101101  
    *  
    Ed  
     1e-007 1 1 0  
    1  3 0 0 3  
    2  5 1 0 0 3  
    2  6 4 0 0 3  
    6  5 1 0  
    6  6 4 0  
    0  
     
    0101000  
    -36 0 +34 0 *  
    Ed  
     1e-007 1 1 0  
    1  4 0 0 2  
    2  7 1 0 0 2  
    2  8 5 0 0 2  
    6  7 1 0  
    6  8 5 0  
    0  
     
    0101000  
    -34 0 +38 0 *  
    Wi  
     
    0101000  
    -37 0 -35 0 +33 0 +32 0 *  
    Fa  
    0  1e-007 1 0  
    2  1  
    0101000  
    +31 0 *  
    Ve  
    1e-007  
    1 0 3  
    0 0  
     
    0101101  
    *  
    Ve  
    1e-007  
    1 0 0  
    0 0  
     
    0101101  
    *  
    Ed  
     1e-007 1 1 0  
    1  5 0 0 3  
    2  9 6 0 0 3  
    2  10 2 0 0 3  
    6  9 6 0  
    6  10 2 0  
    0  
     
    0101000  
    -29 0 +28 0 *  
    Ve  
    1e-007  
    1 2 3  
    0 0  
     
    0101101  
    *  
    Ed  
     1e-007 1 1 0  
    1  6 0 0 2  
    2  11 6 0 0 2  
    2  12 3 0 0 2  
    6  11 6 0  
    6  12 3 0  
    0  
     
    0101000  
    -26 0 +29 0 *  
    Ve  
    1e-007  
    1 2 0  
    0 0  
     
    0101101  
    *  
    Ed  
     1e-007 1 1 0  
    1  7 0 0 3  
    2  13 6 0 0 3  
    2  14 4 0 0 3  
    6  13 6 0  
    6  14 4 0  
    0  
     
    0101000  
    -26 0 +24 0 *  
    Ed  
     1e-007 1 1 0  
    1  8 0 0 2  
    2  15 6 0 0 2  
    2  16 5 0 0 2  
    6  15 6 0  
    6  16 5 0  
    0  
     
    0101000  
    -24 0 +28 0 *  
    Wi  
     
    0101000  
    -27 0 -25 0 +23 0 +22 0 *  
    Fa  
    0  1e-007 6 0  
    2  6  
    0101000  
    +21 0 *  
    Ed  
     1e-007 1 1 0  
    1  9 0 0 1  
    2  17 2 0 0 1  
    2  18 5 0 0 1  
    6  17 2 0  
    6  18 5 0  
    0  
     
    0101000  
    -28 0 +38 0 *  
    Ed  
     1e-007 1 1 0  
    1  10 0 0 1  
    2  19 2 0 0 1  
    2  20 3 0 0 1  
    6  19 2 0  
    6  20 3 0  
    0  
     
    0101000  
    -29 0 +39 0 *  
    Wi  
     
    0101000  
    -19 0 -27 0 +18 0 +37 0 *  
    Fa  
    0  1e-007 2 0  
    2  2  
    0101000  
    +17 0 *  
    Ed  
     1e-007 1 1 0  
    1  11 0 0 1  
    2  21 4 0 0 1  
    2  22 5 0 0 1  
    6  21 4 0  
    6  22 5 0  
    0  
     
    0101000  
    -24 0 +34 0 *  
    Ed  
     1e-007 1 1 0  
    1  12 0 0 1  
    2  23 4 0 0 1  
    2  24 3 0 0 1  
    6  23 4 0  
    6  24 3 0  
    0  
     
    0101000  
    -26 0 +36 0 *  
    Wi  
     
    0101000  
    -15 0 -23 0 +14 0 +33 0 *  
    Fa  
    0  1e-007 4 0  
    2  4  
    0101000  
    +13 0 *  
    Wi  
     
    0101000  
    -32 0 -15 0 +22 0 +19 0 *  
    Fa  
    0  1e-007 5 0  
    2  5  
    0101000  
    +11 0 *  
    Wi  
     
    0101000  
    -35 0 -14 0 +25 0 +18 0 *  
    Fa  
    0  1e-007 3 0  
    2  3  
    0101000  
    +9 0 *  
    Sh  
     
    0101100  
    -30 0 +20 0 -16 0 +12 0 -10 0  +8 0 *  
    So  
     
    0100000  
    +7 0 *  
    CS  
     
    0101000  
    +6 3 *  
    Ve  
    1e-007  
    1 0 0  
    0 0  
     
    0101101  
    *  
    Ve  
    1e-007  
    2 0 0  
    0 0  
     
    0101101  
    *  
    Ed  
     1e-007 1 1 0  
    1  13 0 0 1  
    5  1 0  
    0  
     
    0101000  
    +4 0 -3 0 *  
    Co  
     
    1100000  
    +5 0 +2 0 *  
     
    +1 0   
    0  
@endverbatim