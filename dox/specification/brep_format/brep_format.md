<h1><a id="specification__brep_format" class="anchor">BRep Format</a></h1>

@tableofcontents

<h2><a id="specification__brep_format_1">Introduction</a></h2>

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
  
**Note** that the format is a part of Open CASCADE Technology (OCCT).

Some data fields of the format have additional values, which are used in OCCT. 

Some data fields of the format are specific for OCCT. 

<h2><a id="specification__brep_format_2">Storage of shapes</a></h2>

*BRepTools* and *BinTools* packages contain methods *Read* and *Write* allowing to read and write a Shape to/from a stream or a file.
The methods provided by *BRepTools* package use ASCII storage format; *BinTools* package uses binary format.
Each of these methods has two arguments:
- a *TopoDS_Shape* object to be read/written;
- a stream object or a file name to read from/write to.

The following sample code reads a shape from ASCII file and writes it to a binary one:

~~~~{.cpp}
  TopoDS_Shape aShape;
  if (BRepTools::Read (aShape, "source_file.txt")) {
    BinTools::Write (aShape, "result_file.bin");
  }
~~~~
 
<h2><a id="specification__brep_format_3">Format Common Structure</a></h2>
 
  ASCII encoding is used to read/write BREP format from/to  file. The format data are stored in a file as text data.  
   
  BREP format uses the following BNF terms:

  * \<\\n\>: It is the operating-system-dependent ASCII  character sequence which separates ASCII text strings in the operating system used;
  * \<_\\n\>: = " "*\<\\n\>;
  * \<_\>: = " "+; It is a not empty sequence of space characters  with ASCII code 21h;
  * \<flag\>: = "0" | "1";
  * \<int\>: It is an integer number from -2<sup>31</sup> to 2<sup>31</sup>-1 which is written in  denary system;  
  * \<real\>: It is a real from -1.7976931348623158 $\cdot$ 10<sup>308</sup> to 1.7976931348623158 $\cdot$ 10<sup>308</sup> which is written in decimal or E form with base 10.The point is used as a delimiter of the integer and  fractional parts;
  * \<short real\>: It is a real from -3.402823 $\cdot$ 10<sup>38</sup> to 3.402823 $\cdot$ 10<sup>38</sup> which is written in decimal or E form with base 10.The point is used as a delimiter of the integer and  fractional parts;
  * \<2D point\>: = \<real\>\<_\>\<real\>;  
  * \<3D point\>: = \<real\>(\<_\>\<real)\><sup>2</sup>;  
  * \<2D direction\>: It is a \<2D point\> *x y* so that *x<sup>2</sup> + y<sup>2</sup>* = 1;  
  * \<3D direction\>: It is a \<3D point\> *x y z* so that *x<sup>2</sup> + y<sup>2</sup> + z<sup>2</sup>* = 1;  
  * \<+\>: It is an arithmetic operation of addition.
  
  The format consists of the following sections:  

  * \<content type\>;  
  * \<version\>;  
  * \<locations\>;  
  * \<geometry\>;  
  * \<shapes\>.  
 
  \<content type\> = "DBRep_DrawableShape" \<_\\n\>\<_\\n\>;  
  \<content type\> have other values [1].  
 
  \<version\> = ("CASCADE Topology V1, (c)  Matra-Datavision" | "CASCADE Topology V2, (c) Matra-Datavision" | "CASCADE Topology V3, (c) Open Cascade")\<_\\n\>;
  The difference of the versions is described in the  document.  
 
  Sections \<locations\>, \<geometry\> and \<shapes\> are described below in separate chapters of the document.  
 
<h2><a id="specification__brep_format_4">Locations</a></h2>
 
**Example**  

```
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
```
 
**BNF-like Definition**

```
    <locations> = <location header> <_\n> <location records>;  
    <location header> = "Locations" <_> <location record count>;  
    <location record count> = <int>;  
    <location records> = <location record> ^ <location record count>;  
    <location record> = <location record 1> | <location record 2>;  
    <location record 1> = "1" <_\n> <location  data 1>;  
    <location record 2> = "2" <_> <location  data 2>;  
    <location data 1> = ((<_> <real>) ^ 4 <_\n>) ^ 3;  
    <location data 2> = (<int> <_> <int> <_>)* "0" <_\n>;  
```

**Description**

\<location data 1\> is interpreted as a 3 x 4 matrix

$$
Q =
\begin{pmatrix}
q_{1,1} & q_{1,2} & q_{1,3} & q_{1,4} \\
q_{2,1} & q_{2,2} & q_{2,3} & q_{2,4} \\
q_{3,1} & q_{3,2} & q_{3,3} & q_{3,4}
\end{pmatrix}
$$

which describes transformation of 3 dimensional space and satisfies the following constraints:

$$
d \neq 0 \text{ where } d = |Q_{2}| \text{ where } Q_{2} = \begin{pmatrix}
q_{1,1} & q_{1,2} & q_{1,3} & q_{1,4} \\
q_{2,1} & q_{2,2} & q_{2,3} & q_{2,4} \\
q_{3,1} & q_{3,2} & q_{3,3} & q_{3,4}
\end{pmatrix}
$$

$$ Q_{3}^{T} = Q_{3}^{-1} \text{ where } Q_{3} = \frac{Q_{2}}{d^{1/3}} $$
 
The transformation transforms a point (x, y, z) to another point (u, v, w) by the rule:

$$
\begin{pmatrix}
u \\ v \\ w
\end{pmatrix} = 
Q\cdot(x\;y\;z\;1)^{T} =
\begin{pmatrix}
q_{1,1}\cdot x +q_{1,2}\cdot y +q_{1,3}\cdot z +q_{1,4} \\
q_{2,1}\cdot x +q_{2,2}\cdot y +q_{2,3}\cdot z +q_{2,4} \\
q_{3,1}\cdot x +q_{3,2}\cdot y +q_{3,3}\cdot z +q_{3,4}
\end{pmatrix}
$$

*Q* may be a composition of matrices for the following elementary transformations:  
 
  *  parallel translation --

$$\begin{pmatrix}
1 & 0 & 0 & q_{1,4} \\
0 & 1 & 0 & q_{2,4} \\
0 & 0 & 1 & q_{3,4}
\end{pmatrix}$$

  *  rotation around an axis with a direction *D(D<sub>x</sub>, D<sub>y</sub>, D<sub>z</sub>)* by an angle $\varphi$ --

$$
\begin{pmatrix}
D_{x}^{2} \cdot (1-cos(\varphi)) + cos(\varphi) &D_{x} \cdot D_{y} \cdot (1-cos(\varphi)) - D_{z} \cdot sin(\varphi) &D_{x} \cdot D_{z} \cdot (1-cos(\varphi)) + D_{y} \cdot sin(\varphi) &0\\
D_{x} \cdot D_{y} \cdot (1-cos(\varphi)) + D_{z} \cdot sin(\varphi) &D_{y}^{2} \cdot (1-cos(\varphi)) + cos(\varphi) &D_{y} \cdot D_{z} \cdot (1-cos(\varphi)) - D_{x} \cdot sin(\varphi) &0\\
D_{x} \cdot D_{z} \cdot (1-cos(\varphi)) - D_{y} \cdot sin(\varphi) &D_{y} \cdot D_{z} \cdot (1-cos(\varphi)) + D_{x} \cdot sin(\varphi) &D_{z}^{2} \cdot (1-cos(\varphi)) + cos(\varphi) &0
\end{pmatrix};
$$
 
  *  scaling --

$$
\begin{pmatrix} 
s & 0 & 0 & 0 \\
0 & s & 0 & 0 \\ 
0 & 0 & s & 0 
\end{pmatrix} \text{ where } 
s \in (-\infty, \infty) \setminus \lbrace 0 \rbrace
$$

  *  central symmetry --

$$
\begin{pmatrix} -1 &0 &0 &0 \\
0 &-1 &0 &0 \\
0 &0 &-1 &0 \end{pmatrix}
$$

  *  axis symmetry --

$$
\begin{pmatrix} -1 &0 &0 &0 \\
0 &-1 &0 &0 \\
0 &0 &1 &0 \end{pmatrix}
$$

  *  plane symmetry --

$$
\begin{pmatrix} 
1 & 0 & 0 & 0 \\ 
0 & 1 & 0 & 0 \\ 
0 & 0 & -1 & 0 
\end{pmatrix}
$$
 
\<location data 2\> is interpreted as a composition of locations raised to a power and placed above this \<location data 2\> in  the section \<locations\>. \<location data 2\> is a sequence $l_{1}p_{1} ... l_{n}p_{n}$ of $n \geq 0 $ integer pairs $l_{i}p_{i} \; (1 \leq i \leq n)$. \<flag\> 0 is the indicator  of the sequence end. The sequence is interpreted as a composition $L_{l_{1}}^{p_{1}} \cdot ... \cdot L_{l_{n}}^{p_{n}}$ where $L_{l_{i}}$ is a location from $l_{i}$-th \<location  record\> in the section locations. \<location record\> numbering  starts from 1.  
 
 
<h2><a id="specification__brep_format_5">Geometry</a></h2>

```
    <geometry> =  
    <2D curves>  
    <3D curves>  
    <3D polygons> 
    <polygons on triangulations>  
    <surfaces>  
    <triangulations>;  
```
 
<h3><a id="specification__brep_format_5_1">3D curves</a></h3>
 
**Example**

```
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
```
 
**BNF-like Definition**

```
    <3D curves> = <3D curve header> <_\n> <3D curve records>;  
     
    <3D curve header> = "Curves" <_> <3D curve count>;  
     
    <3D curve count> = <int>;  
     
    <3D curve records> = <3D curve record> ^ <3D curve count>;  
     
    <3D curve record> =  
    <3D curve record 1> |  
    <3D curve record 2> |  
    <3D curve record 3> |  
    <3D curve record 4> |  
    <3D curve record 5> |  
    <3D curve record 6> |  
    <3D curve record 7> |  
    <3D curve record 8> |  
    <3D curve record 9>;  
```
 
<h4><a id="specification__brep_format_5_1_1">Line - <3D curve record 1></a></h4>
 
**Example**  

```
    1 1 0 3 0 1 0   
```
 
**BNF-like Definition**

```
    <3D curve record 1> = "1" <_> <3D point> <_> <3D direction> <_\n>;  
```
 
**Description**  
 
\<3D curve record 1\> describes a line. The line data consist of a 3D point *P* and a 3D direction *D*. The line passes through the  point *P*, has  the direction *D* and  is defined by the following parametric equation:  
 
$$ C(u)=P+u \cdot D, \; u \in (-\infty,\; \infty). $$

The example record is interpreted as a line which  passes through a point *P*=(1, 0, 3), has a direction *D*=(0, 1, 0) and is defined by the  following parametric equation: $C(u)=(1,0,3)+u \cdot (0,1,0)$.  
 
 
<h4><a id="specification__brep_format_5_1_2">Circle - <3D curve record 2></a></h4>
 
**Example**  

```
    2 1 2 3 0 0 1 1 0 -0 -0 1 0 4  
```
 
**BNF-like Definition**
 
~~~~{.cpp}
    <3D curve record 2> = "2" <_> <3D circle center> <_> <3D circle N> <_> <3D circle Dx> <_> <3D circle Dy> <_> <3D circle radius> <_\n>;  
     
    <3D circle center> = <3D point>;  
     
    <3D circle N> = <3D direction>;  
     
    <3D circle Dx> = <3D direction>;  
     
    <3D circle Dy> = <3D direction>;  
     
    <3D circle radius> = <real>;  
~~~~
 
**Description**  
 
\<3D curve record 2\> describes a circle. The  circle data consist of a 3D point *P*, pairwise orthogonal 3D directions *N*, *D<sub>x</sub>* and *D<sub>y</sub>* and a non-negative real *r*. The circle has a center *P* and is located in a plane with a normal *N*. The circle has a radius *r* and is defined by the following parametric equation:  
 
$$ C(u)=P+r \cdot (cos(u) \cdot D_{x} + sin(u) \cdot D_{y}), \; u \in [o,\;2 \cdot \pi). $$  
 
The  example record is interpreted as a circle which has its center *P*=(1, 2, 3), is located in plane  with a normal *N*=(0, 0 ,1).  Directions for the circle are *D<sub>x</sub>*=(1, 0 ,0) and *D<sub>y</sub>*=(0, 1 ,0). The circle has a radius *r*=4 and is defined by  the following parametric equation: $C(u) = (1,2,3) + 4 \cdot ( cos(u) \cdot(1,0,0) + sin(u) \cdot (0,1,0) )$.  
 
 
<h4><a id="specification__brep_format_5_1_3">Ellipse - \<3D curve record 3\> </a></h4>
 
**Example**  

```
    3 1 2 3 0 0 1 1 0 -0 -0 1 0 5  4  
```
 
**BNF-like Definition**

~~~~{.cpp}
    <3D curve record 3> = "3" <_> <3D ellipse center> <_> <3D ellipse N> <_> <3D ellipse Dmaj> <_> <3D ellipse Dmin> <_> <3D ellipse Rmaj> <_> <3D ellipse Rmin> <_\n>;  
     
    <3D ellipse center> = <3D point>;  
     
    <3D ellipse N> = <3D direction>;  
     
    <3D ellipse Dmaj> = <3D direction>;  
     
    <3D ellipse Dmin> = <3D direction>;  
     
    <3D ellipse Rmaj> = <real>;  
     
    <3D ellipse Rmin> = <real>;  
~~~~
 
**Description**  
 
\<3D curve record 3\> describes an ellipse. The  ellipse data consist of a 3D point *P*, pairwise orthogonal 3D directions *N*, *D<sub>maj</sub>* and *D<sub>min</sub>* and non-negative reals *r<sub>maj</sub>* and *r<sub>min</sub>* so that *r<sub>min</sub>* $\leq$ *r<sub>maj</sub>*. The ellipse has its  center *P*, is  located in plane with the normal *N*, has major and minor axis directions *D<sub>maj</sub>* and *D<sub>min</sub>*, major and minor radii *r<sub>maj</sub>* and *r<sub>min</sub>* and is defined by the following parametric equation:  
 
$$
C(u)=P+r_{maj} \cdot cos(u) \cdot D_{maj} + r_{min} \cdot sin(u) \cdot D_{min}, u \in [0, 2 \cdot \pi).
$$

The  example record is interpreted as an ellipse which has its center *P*=(1, 2, 3), is located in plane with a normal *N*=(0, 0, 1), has major and minor axis directions *D<sub>maj</sub>*=(1, 0, 0) and *D<sub>min</sub>*=(0, 1, 0), major and minor radii *r<sub>maj</sub>*=5 and *r<sub>min</sub>*=4 and is defined by  the following parametric equation:

$$ C(u) = (1,2,3) + 5 \cdot cos(u) \cdot(1,0,0) + 4 \cdot sin(u) \cdot (0,1,0) $$

<h4><a id="specification__brep_format_5_1_4">Parabola - \<3D curve record 4\> </a></h4>
 
**Example**  

```
    4 1 2 3 0 0 1 1 0 -0 -0 1 0  16  
```
 
**BNF-like Definition**  

~~~~{.cpp}
    <3D curve record 4> = "4" <_> <3D parabola origin> <_> <3D parabola N> <_> <3D parabola Dx> <_> <3D parabola Dy> <_> <3D parabola focal length> <_\n>;  
     
    <3D parabola origin> = <3D point>;  
     
    <3D parabola N> = <3D direction>;  
     
    <3D parabola Dx> = <3D direction>;  
     
    <3D parabola Dy> = <3D direction>;  
     
    <3D parabola focal length> = <real>;  
~~~~
 
**Description**  
 
\<3D curve record 4\> describes a parabola. The  parabola data consist of a 3D point *P*, pairwise orthogonal 3D directions *N*, *D<sub>x</sub>* and *D<sub>y</sub>* and a non-negative real *f*. The parabola is located in plane which passes through the point *P* and has the normal *N*. The parabola has a focus length *f* and is defined by the following parametric equation:  
 
$$ C(u)=P+\frac{u^{2}}{4 \cdot f} \cdot D_{x} + u \cdot D_{y}, u \in (-\infty,\; \infty) \Leftarrow f \neq 0; $$

$$ C(u)=P+u \cdot D_{x}, u \in (-\infty,\; \infty) \Leftarrow f = 0\;(degenerated\;case). $$ 
 
The  example record is interpreted as a parabola in plane which passes through a point *P*=(1, 2, 3) and has a normal *N*=(0, 0, 1). Directions for the parabola are *D<sub>x</sub>*=(1, 0, 0) and *D<sub>y</sub>*=(0, 1, 0). The parabola has a focus length *f*=16 and is defined by the following parametric equation: $C(u) = (1,2,3) + \frac{u^{2}}{64} \cdot (1,0,0) + u \cdot (0,1,0)$.
 
 
<h4><a id="specification__brep_format_5_1_5">Hyperbola - \<3D curve record 5\> </a></h4>
 
**Example**  

```
    5 1 2 3 0 0 1 1 0 -0 -0 1 0 5  4  
```
 
**BNF-like Definition**  

~~~~{.cpp}
    <3D curve record 5> = "5" <_> <3D hyperbola origin> <_> <3D hyperbola N> <_> <3D hyperbola Dx> <_> <3D hyperbola Dy> <_> <3D hyperbola Kx> <_> <3D hyperbola Ky> <_\n>;  
     
    <3D hyperbola origin> = <3D point>;  
     
    <3D hyperbola N> = <3D direction>;  
     
    <3D hyperbola Dx> = <3D direction>;  
     
    <3D hyperbola Dy> = <3D direction>;  
     
    <3D hyperbola Kx> = <real>;  
     
    <3D hyperbola Ky> = <real>;  
~~~~
 
**Description**
 
\<3D curve record 5\> describes a hyperbola. The  hyperbola data consist of a 3D point *P*, pairwise orthogonal 3D directions *N*, *D<sub>x</sub>* and *D<sub>y</sub>* and non-negative reals *k<sub>x</sub>* and *k<sub>y</sub>*. The hyperbola is  located in plane which passes through the point *P* and has the normal *N*. The hyperbola is defined by  the following parametric equation:  
 
$$ C(u)=P+k_{x} \cdot cosh(u) \cdot D_{x}+k_{y} \cdot sinh(u) \cdot D_{y} , u \in (-\infty,\; \infty). $$  
 
The  example record is interpreted as a hyperbola in plane which passes through a point *P*=(1, 2, 3) and has a normal *N*=(0, 0, 1). Other hyperbola data are *D<sub>x</sub>*=(1, 0, 0), *D<sub>y</sub>*=(0, 1, 0), *k<sub>x</sub>*=5 and *k<sub>y</sub>*=4. The hyperbola is defined by the following parametric equation: $C(u) = (1,2,3) + 5 \cdot cosh(u) \cdot (1,0,0) +4 \cdot sinh(u) \cdot (0,1,0)$.
 
 
<h4><a id="specification__brep_format_5_1_6">Bezier Curve - \<3D curve record 6\> </a></h4>
 
**Example**  

```
    6 1 2 0 1 0  4 1 -2 0  5 2 3  0  6   
```
 
**BNF-like Definition**

```
	<3D curve record 6> = "6" <_> <3D Bezier rational flag> <_> <3D Bezier degree> 
	<3D Bezier weight poles> <_\n>;
		
	<3D Bezier rational flag> = <flag>;
		
	<3D Bezier degree> = <int>;

	3D Bezier weight poles> = (<_> <3D Bezier weight pole>) ^ (<3D Bezier degree> <+> "1");

	<3D Bezier weight pole> = <3D point> [<_> <real>];
```
 
**Description**  
 
\<3D curve record 6\> describes a Bezier curve. The curve data consist of a rational *r*, a degree $m \leq 25$ and weight poles.  
 
The weight poles are *m*+1 3D points *B<sub>0</sub> ... B<sub>m</sub>* if the flag *r* is 0. The weight poles are *m*+1 pairs *B<sub>0</sub>h<sub>0</sub> ... B<sub>m</sub>h<sub>m</sub>* if flag *r* is 1. Here *B<sub>i</sub>* is a 3D point and *h<sub>i</sub>* is a positive real $(0 \leq i \leq m)$. $h_{i}=1\; (0 \leq i \leq m)$ if the flag *r* is 0.  
 
The Bezier curve is defined by the following  parametric equation:  
 
$$ C(u) = \frac{\sum_{i=0}^{m}B_{i} \cdot h_{i} \cdot C_{m}^{i} \cdot u^{i} \cdot (1-u)^{m-i}}{\sum_{i=0}^{m}h_{i} \cdot C_{m}^{i} \cdot u^{i} \cdot (1-u)^{m-i}},\;u \in [0,\; 1] $$

where $0^{0} \equiv 1$.  
 
The example record is interpreted as a Bezier curve with a rational flag *r*=1, degree *m*=2 and weight poles *B<sub>0</sub>*=(0, 1, 0), *h<sub>0</sub>*=4, *B<sub>1</sub>*=(1, -2, 0), *h<sub>1</sub>*=5 and *B<sub>2</sub>*=(2, 3, 0), *h<sub>2</sub>*=6. The Bezier curve is defined  by the following parametric equation:  
 
$$ C(u)=\frac{(0,1,0) \cdot 4 \cdot (1-u)^{2}+(1,-2,0) \cdot 5 \cdot 2 \cdot u \cdot (1-u) + (2,3,0) \cdot 6 \cdot u^{2} )}{4 \cdot (1-u)^{2}+5 \cdot 2 \cdot u \cdot (1-u)+6 \cdot u^{2}}. $$  
 
 
<h4><a id="specification__brep_format_5_1_7">B-Spline Curve - <3D curve record 7></a></h4>
 
**Example**  

```
    7 1 0  1 3 5  0 1 0  4 1 -2  0  5 2 3 0  6  
     0 1 0.25 1 0.5 1 0.75 1 1 1  
```
 
**BNF-like Definition**

~~~~{.cpp}
	<3D curve record 7> = "7" <_> <3D B-spline rational flag> <_> "0" <_> <3D B-spline degree> <_> 
	<3D B-spline pole count> <_> <3D B-spline multiplicity knot count> <3D B-spline weight poles> 
	<_\n> <3D B-spline multiplicity knots> <_\n>;

	<3D B-spline rational flag> = <flag>;

	<3D B-spline degree> = <int>;

	<3D B-spline pole count> = <int>;

	<3D B-spline multiplicity knot count> = <int>;

	<3D B-spline weight poles> = (<_> <3D B-spline weight pole>) ^ <3D B-spline pole count>;

	<3D B-spline weight pole> = <3D point> [<_> <real>];

	<3D B-spline multiplicity knots> = (<_> <3D B-spline multiplicity knot>) ^ <3D B-spline multiplicity knot count>;

	<3D B-spline multiplicity knot> = <real> <_> <int>;  
~~~~
 
**Description**  
 
\<3D curve record 7\> describes a B-spline curve.  The curve data consist of a rational flag *r*, a degree $m \leq 25$, pole count $n \geq 2$, multiplicity knot count *k*, weight poles and  multiplicity knots.  
 
The weight poles are *n* 3D points *B<sub>1</sub> ... B<sub>n</sub>* if the flag *r* is 0. The weight poles are *n* pairs *B<sub>1</sub>h<sub>1</sub> ... B<sub>n</sub>h<sub>n</sub>* if the flag *r* is 1. Here *B<sub>i</sub>* is a 3D point and *h<sub>i</sub>* is a positive real $(1 \leq i \leq n)$.  $h_{i}=1\; (1 \leq i \leq n)$ if the flag *r* is 0.  
 
The multiplicity knots are *k* pairs *u<sub>1</sub>q<sub>1</sub> ... u<sub>k</sub>q<sub>k</sub>*. Here *u<sub>i</sub>* is a knot with a multiplicity $q_{i} \geq 1 \; (1 \leq i \leq k)$ so that
 
$$
u_{i} < u_{i+1} (1 \leq i \leq k-1)
$$

$$
q_{1} \leq m+1,\; q_{k} \leq m+1,\; q_{i} \leq m\; (2 \leq i \leq k-1), \sum_{i=1}^{k}q_{i}=m+n+1
$$

The B-spline curve is defined by the following  parametric equation:

$$
C(u) = \frac{\sum_{i=1}^{n} B_{i} \cdot h_{i} \cdot N_{i,m+1}(u)}{\sum_{i=1}^{n} h_{i} \cdot N_{i,m+1}(u)}, \; u \in [u_{1}, \; u_{k}]
$$
 
where functions $N_{i,j}$ have the following recursion definition  by *j*:  

$$
N_{i,1}(u)=\lbrace\begin{matrix}
1\Leftarrow \bar{u}_{i} \leq u \leq \bar{u}_{i+1}\\ 
0\Leftarrow u < \bar{u}_{i} \vee  \bar{u}_{i+1} \leq u \end{matrix},\;
N_{i,j}(u)=\frac{(u-\bar{u}_{i}) \cdot N_{i,j-1}(u) }{\bar{u}_{i+j-1}-\bar{u}_{i}}+ \frac{(\bar{u}_{i+j}-u) \cdot N_{i+1,j-1}(u)}{\bar{u}_{i+j}-\bar{u}_{i+1}},\;(2 \leq j \leq m+1)
$$

where 

$$
\bar{u}_{i} = u_{j},\; (1 \leq j \leq k,\; \sum_{l=1}^{j-1} q_{l} + 1 \leq i \leq \sum_{l=1}^{j} q_{l})
$$

The example record is interpreted as a B-spline curve  with a rational flag *r*=1, a degree *m*=1, pole count *n*=3, multiplicity knot count *k*=5, weight poles *B<sub>1</sub>*=(0,1,0), *h<sub>1</sub>*=4, *B<sub>2</sub>*=(1,-2,0), *h<sub>2</sub>*=5 and *B<sub>3</sub>*=(2,3,0), *h<sub>3</sub>*=6, multiplicity knots *u<sub>1</sub>*=0, *q<sub>1</sub>*=1, *u<sub>2</sub>*=0.25, *q<sub>2</sub>*=1, *u<sub>3</sub>*=0.5, *q<sub>3</sub>*=1, *u<sub>4</sub>*=0.75, *q<sub>4</sub>*=1 and *u<sub>5</sub>*=1, *q<sub>5</sub>*=1. The B-spline curve is defined  by the following parametric equation:  

$$
C(u)=\frac{(0,1,0) \cdot 4 \cdot N_{1,2}(u) + (1,-2,0) \cdot 5 \cdot N_{2,2}(u)+(2,3,0) \cdot 6 \cdot N_{3,2}(u)}{4 \cdot N_{1,2}(u)+5 \cdot N_{2,2}(u)+6 \cdot N_{3,2}(u)}
$$

<h4><a id="specification__brep_format_5_1_8">Trimmed Curve - <3D curve record 8></a></h4>
 
**Example**  

```
    8  -4 5  
    1 1 2 3 1 0 0   
```
 
**BNF-like Definition**  

~~~~{.cpp}
	<3D curve record 8> = "8" <_> <3D trimmed curve u min> <_> <3D trimmed curve u max> <_\n> <3D curve record>;

	<3D trimmed curve u min> = <real>;

	<3D trimmed curve u max> = <real>;  
~~~~
 
**Description**  
 
\<3D curve record 8\> describes a trimmed curve.  The trimmed curve data consist of reals *u<sub>min</sub>* and *u<sub>max</sub>* and \<3D curve record\> so that *u<sub>min</sub>* < *u<sub>max</sub>*. The trimmed curve is a restriction of the base curve *B* described in the record to the segment $[u_{min},\;u_{max}]\subseteq domain(B)$. The trimmed curve is  defined by the following parametric equation:  
 
$$ C(u)=B(u),\; u \in [u_{min},\;u_{max}] $$
 
The  example record is interpreted as a trimmed curve with *u<sub>min</sub>*=-4 and *u<sub>max</sub>*=5 for the base curve $B(u)=(1,2,3)+u \cdot (1,0,0)$. The trimmed curve is  defined by the following parametric equation: $C(u)=(1,2,3)+u \cdot (1,0,0),\; u \in [-4,\; 5]$.
 
 
<h4><a id="specification__brep_format_5_1_9">Offset Curve - \<3D curve record 9\> </a></h4>
 
**Example**  

```
    9 2  
    0 1 0   
    1 1 2 3 1 0 0   
```
 
**BNF-like Definition**  

```
	<3D curve record 9> = "9" <_> <3D offset curve distance> <_\n>;
	<3D offset curve direction> <_\n>;
	<3D curve record>;

	<3D offset curve distance> = <real>;

	<3D offset curve direction> = <3D direction>;  
```
 
**Description**  
 
\<3D curve record 9\> describes an offset curve. The offset curve data consist of a distance *d*, a 3D direction *D* and a \<3D curve record\>. The offset curve is the result of offsetting the base curve *B* described in the record to the distance *d* along the vector $[B'(u),\; D] \neq \vec{0}$. The offset curve is defined  by the following parametric equation:  
 
$$ C(u)=B(u)+d \cdot \frac{[B'(u),\; D]}{|[B'(u),\; D]|},\; u \in domain(B) . $$   
 
The example record is interpreted as an offset curve with a distance *d*=2, direction *D*=(0, 1, 0), base curve $B(u)=(1,2,3)+u \cdot (1,0,0)$ and defined by the  following parametric equation: $C(u)=(1,2,3)+u \cdot (1,0,0)+2 \cdot (0,0,1)$.  
 
<h3><a id="specification__brep_format_5_2">Surfaces</a></h3>
 
**Example**  

```
    Surfaces 6  
    1 0 0 0 1 0 -0 0 0 1 0 -1 0   
    1 0 0 0 -0 1 0 0 0 1 1 0 -0   
    1 0 0 3 0 0 1 1 0 -0 -0 1 0   
    1 0 2 0 -0 1 0 0 0 1 1 0 -0   
    1 0 0 0 0 0 1 1 0 -0 -0 1 0   
    1 1 0 0 1 0 -0 0 0 1 0 -1 0   
```
 
**BNF-like Definition**  

```
	<surfaces> = <surface header> <_\n> <surface records>;

	<surface header> = “Surfaces” <_> <surface count>;

	<surface records> = <surface record> ^ <surface count>;

	<surface record> =
	<surface record 1> |
	<surface record 2> |
	<surface record 3> |
	<surface record 4> |
	<surface record 5> |
	<surface record 6> |
	<surface record 7> |
	<surface record 8> |
	<surface record 9> |
	<surface record 10> |
	<surface record 11>;  
```
 
<h4><a id="specification__brep_format_5_2_1">Plane - < surface record 1 ></a></h4>

**Example**  

```
    1 0 0 3 0 0 1 1 0 -0 -0 1 0   
```
 
**BNF-like Definition**

```
	<surface record 1> = "1" <_> <3D point> (<_> <3D direction>) ^ 3 <_\n>;
```
 
**Description**  
 
\<surface record 1\> describes a plane. The plane  data consist of a 3D point *P* and pairwise orthogonal 3D directions *N*, *D<sub>u</sub>* and *D<sub>v</sub>*. The plane passes through the  point *P*, has  the normal *N* and  is defined by the following parametric equation:  
 
$$ S(u,v)=P+u \cdot D_{u}+v \cdot D_{v},\; (u,\;v) \in (-\infty,\; \infty) \times (-\infty,\; \infty). $$  
 
The example record is interpreted as a plane which  passes through a point *P*=(0, 0, 3), has a normal *N*=(0, 0, 1) and is defined by the  following parametric equation: $S(u,v)=(0,0,3)+u \cdot (1,0,0) + v \cdot (0,1,0)$.
 
 
<h4><a id="specification__brep_format_5_2_2">Cylinder - \< surface record 2 \> </a></h4>
 
**Example**  

```
    2 1 2 3 0 0 1 1 0 -0 -0 1 0 4  
```
 
**BNF-like Definition**  

```
    <surface record 2> = "2" <_> <3D point> (<_> <3D direction>) ^ 3 <_> <real> <_\n>;
```
 
**Description**  
 
\<surface record 2\> describes a cylinder. The  cylinder data consist of a 3D point *P*, pairwise orthogonal 3D directions *D<sub>v</sub>*, *D<sub>X</sub>* and *D<sub>Y</sub>* and a non-negative real *r*. The cylinder axis  passes through the point *P* and has the direction *D<sub>v</sub>*. The cylinder has  the radius *r* and  is defined by the following parametric equation:  
 
$$S(u,v)=P+r \cdot (cos(u) \cdot D_{x}+sin(u) \cdot D_{y} )+v \cdot D_{v},\; (u,v) \in [0,\; 2 \cdot \pi) \times (-\infty,\; \infty).$$
 
The  example record is interpreted as a cylinder which axis passes through a point *P*=(1, 2, 3) and has a direction *D<sub>v</sub>*=(0, 0, 1). Directions for the  cylinder are *D<sub>X</sub>*=(1,0,0) and *D<sub>Y</sub>*=(0,1,0). The cylinder has a radius *r*=4 and is defined by the following  parametric equation: $S(u,v)=(1,2,3)+4 \cdot ( cos(u) \cdot D_{X} + sin(u) \cdot D_{Y} ) + v \cdot D_{v}.$  
 
 
<h4><a id="specification__brep_format_5_2_3">Cone - \< surface record 3 \> </a></h4>
 
**Example**  

```
    3 1 2 3 0 0 1 1 0 -0 -0 1 0 4  
    0.75
```
 
 
**BNF-like Definition**

```
    <surface record 3> = "3" <_> <3D point> (<_> <3D direction>) ^ 3 (<_> <real>) ^ 2 <_\n>;  
```
 
**Description**  
 
\<surface record 3\> describes a cone. The cone  data consist of a 3D point *P*, pairwise orthogonal 3D directions *D<sub>Z</sub>*, *D<sub>X</sub>* and *D<sub>Y</sub>*, a non-negative real *r* and a real $\varphi \in (-\pi /2,\; \pi/2)/\lbrace 0 \rbrace$. The cone axis passes  through the point *P* and  has the direction *D<sub>Z</sub>*. The plane which passes through the point *P* and is parallel to directions *D<sub>X</sub>* and *D<sub>Y</sub>* is the cone referenced  plane. The cone section by the plane is a circle with the radius *r*. The direction from  the point *P* to  the cone apex is $-sgn(\varphi) \cdot D_{Z}$.  The cone has a half-angle$|\varphi|$ and is defined by the following  parametric equation:  
 
$$ S(u,v)=P+(r+v \cdot sin(\varphi)) \cdot (cos(u) \cdot D_{X}+sin(u) \cdot D_{Y})+v \cdot cos(\varphi) \cdot D_{Z}, (u,v) \in [0,\; 2 \cdot \pi) \times (-\infty,\; \infty) . $$  
 
The example record is interpreted as a cone with an axis  which passes through a point *P*=(1, 2, 3) and has a direction *D<sub>Z</sub>*=(0, 0, 1). Other cone data are *D<sub>X</sub>*=(1, 0, 0), *D<sub>Y</sub>*=(0, 1, 0), *r*=4 and $\varphi = 0.75$. The cone is defined by the  following parametric equation:

$$ S(u,v)=(1,2,3)+( 4 + v \cdot sin(0.75)) \cdot ( cos(u) \cdot (1,0,0) + sin(u) \cdot (0,1,0) ) + v \cdot cos(0.75) \cdot (0,0,1) . $$
 

<h4><a id="specification__brep_format_5_2_4">Sphere - \< surface record 4 \> </a></h4>
 
**Example**  

```
    4 1 2 3 0 0 1 1 0 -0 -0 1 0 4  
```
 
**BNF-like Definition**

```
    <surface record 4> = "4" <_> <3D point> (<_> <3D direction>) ^ 3 <_> <real> <_\n>;  
```
 
**Description**  
 
\<surface record 4\> describes a sphere. The  sphere data consist of a 3D point *P*, pairwise orthogonal 3D directions *D<sub>Z</sub>*, *D<sub>X</sub>* and *D<sub>Y</sub>* and a non-negative real *r*. The sphere has the center *P*, radius *r* and  is defined by the following parametric equation:  
 
$$ S(u,v)=P+r \cdot cos(v) \cdot (cos(u) \cdot D_{x}+sin(u) \cdot D_{y} ) +r \cdot sin(v) \cdot D_{Z},\; (u,v) \in [0,\;2 \cdot \pi) \times [-\pi /2,\; \pi /2] . $$  
 
The  example record is interpreted as a sphere with its center *P*=(1, 2, 3). Directions for the sphere are  *D<sub>Z</sub>*=(0, 0, 1), *D<sub>X</sub>*=(1, 0, 0) and *D<sub>Y</sub>*=(0, 1, 0). The sphere has a radius  *r*=4 and is  defined by the following parametric equation:

$$ S(u,v)=(1,2,3)+ 4 \cdot cos(v) \cdot ( cos(u) \cdot (1,0,0) + sin(u) \cdot (0,1,0) ) + 4 \cdot sin(v) \cdot (0,0,1) . $$   
 
 
<h4><a id="specification__brep_format_5_2_5">Torus - \< surface record 5 \> </a></h4>
 
**Example**  

```
    5 1 2 3 0 0 1 1 0 -0 -0 1 0 8  4  
```
 
**BNF-like Definition**

```
    <surface record 5> = "5" <_> <3D point> (<_> <3D direction>) ^ 3 (<_> <real>) ^ 2 <_\n>;  
```
 
**Description**  
 
\<surface record 5\> describes a torus. The torus  data consist of a 3D point *P*, pairwise orthogonal 3D directions *D<sub>Z</sub>*, *D<sub>X</sub>* and *D<sub>Y</sub>* and non-negative reals *r<sub>1</sub>* and *r<sub>2</sub>*. The torus axis  passes through the point *P* and has the direction *D<sub>Z</sub>*. *r<sub>1</sub>* is the distance from the  torus circle center to the axis. The torus circle has the radius *r<sub>2</sub>*. The torus is defined  by the following parametric equation:  
 
$$ S(u,v)=P+(r_{1}+r_{2} \cdot cos(v)) \cdot (cos(u) \cdot D_{x}+sin(u) \cdot D_{y} ) +r_{2} \cdot sin(v) \cdot D_{Z},\; (u,v) \in [0,\;2 \cdot \pi) \times [0,\; 2 \cdot \pi) . $$    
 
The example record is interpreted as a torus with an axis which passes through a point *P*=(1, 2, 3) and has a direction *D<sub>Z</sub>*=(0, 0, 1). *D<sub>X</sub>*=(1, 0, 0), *D<sub>Y</sub>*=(0, 1, 0), *r<sub>1</sub>*=8 and *r<sub>2</sub>*=4 for the torus. The torus is defined  by the following parametric equation:

$$ S(u,v)=(1,2,3)+ (8+4 \cdot cos(v)) \cdot ( cos(u) \cdot (1,0,0) + sin(u) \cdot (0,1,0) ) + 4 \cdot sin(v) \cdot (0,0,1) . $$   
 
 
<h4><a id="specification__brep_format_5_2_6">Linear Extrusion - \< surface record 6 \> </a></h4>
 
**Example**  

```
    6 0 0.6 0.8   
    2 1 2 3 0 0 1 1 0 -0 -0 1 0 4  
```
 
**BNF-like Definition**

```
    <surface record 6> = "6" <_> <3D direction> <_\n> <3D curve record>;  
```
 
**Description**  
 
\<surface record 6\> describes a linear extrusion  surface. The surface data consist of a 3D direction *D<sub>v</sub>* and a \<3D curve  record\>. The linear extrusion surface has the direction *D<sub>v</sub>*, the base curve *C* described in the  record and is defined by the following parametric equation:  
 
$$ S(u,v)=C(u)+v \cdot D_{v},\; (u,v) \in domain(C) \times (-\infty,\; \infty) . $$     
 
The example record is interpreted as a linear  extrusion surface with a direction *D<sub>v</sub>*=(0, 0.6, 0.8). The base curve is a circle for the  surface. The surface is defined by the following parametric equation:

$$ S(u,v)=(1,2,3)+4 \cdot (cos(u) \cdot (1,0,0)+sin(u) \cdot (0,1,0))+v \cdot (0, 0.6, 0.8),\; (u,v) \in [0,\; 2 \cdot \pi) \times (-\infty,\; \infty). $$  
 
 
<h4><a id="specification__brep_format_5_2_7">Revolution Surface - \< surface record 7 \> </a></h4>
 
**Example**  

```
    7 -4 0 3 0 1 0   
    2 1 2 3 0 0 1 1 0 -0 -0 1 0 4  
```
 
**BNF-like Definition**

```
    <surface record 7> = "7" <_> <3D point> <_> <3D direction> <_\n> <3D curve record>;  
```
 
**Description**  
 
\<surface record 7\> describes a revolution  surface. The surface data consist of a 3D point *P*, a 3D direction *D* and a \<3D curve  record\>. The surface axis passes through the point *P* and has the direction *D*. The base curve *C* described by the  record and the axis are coplanar. The surface is defined by the following  parametric equation:  
 
$$ S(u,v)= P+V_{D}(v)+cos(u) \cdot (V(v)-V_{D}(v))+sin(u) \cdot [D,V(v)],\;(u,v) \in [0,\; 2 \cdot \pi)\times domain(C) $$ 
 
where $V(v)=C(v)-P, V_{D}(v)=(D,V(v)) \cdot D$.  
 
The example record is interpreted as a revolution surface with an axis which passes through a point *P*=(-4, 0, 3) and has a direction *D*=(0, 1, 0). The base curve is a circle  for the surface. The surface is defined by the following parametric equation:  

$$ S(u,v)= (-4,0,3)+V_{D}(v)+cos(u) \cdot (V(v)-V_{D}(v))+sin(u) \cdot [(0,1,0),V(v)],\;(u,v) \in [0,\; 2 \cdot \pi)\times [0,\; 2 \cdot \pi) $$ 
 
where $V(v)=(5,2,0)+4 \cdot (cos(v) \cdot (1,0,0)+sin(v) \cdot (0,1,0)), V_{D}(v)=((0,1,0),V(v)) \cdot (0,1,0)$.  
 
 
<h4><a id="specification__brep_format_5_2_8">Bezier Surface - \< surface record 8 \> </a></h4>
 
**Example**  

```
    8 1 1 2 1 0 0 1  7 1 0 -4  10    
    0 1 -2  8 1 1 5  11   
    0 2 3  9 1 2 6  12   
```
 
**BNF-like Definition**

~~~~{.cpp}
	<surface record 8> = "8" <_> <Bezier surface u rational flag> <_> <Bezier surface v rational flag> <_> <Bezier surface u degree> <_> <Bezier surface v degree> <_> 
	<Bezier surface weight poles>;

	<Bezier surface u rational flag> = <flag>;

	<Bezier surface v rational flag> = <flag>;

	<Bezier surface u degree> = <int>;

	<Bezier surface v degree> = <int>;

	<Bezier surface weight poles> =
	(<Bezier surface weight pole group> <_\n>) ^ (<Bezier surface u degree> <+> "1");

	<Bezier surface weight pole group> = <Bezier surface weight pole>
	(<_> <Bezier surface weight pole>) ^ <Bezier surface v degree>;

	<Bezier surface weight pole> = <3D point> [<_> <real>];  
~~~~
 
**Description**  
 
\<surface record 8\> describes a Bezier surface.  The surface data consist of a u rational flag *r<sub>u</sub>*, v rational flag *r<sub>v</sub>*, u degree $m_{u} \leq 25$, v degree $m_{v} \leq 25$ and weight poles.  
 
The weight poles are $(m_{u}+1) \cdot (m_{v}+1)$ 3D points $B_{i,j}\; ((i,j) \in \lbrace 0,...,m_{u} \rbrace \times \lbrace 0,...,m_{v} \rbrace)$ if $r_{u}+r_{v}=0$. The weight poles are $(m_{u}+1) \cdot (m_{v}+1)$ pairs $B_{i,j}h_{i,j}\; ((i,j) \in \lbrace 0,...,m_{u} \rbrace \times \lbrace 0,...,m_{v} \rbrace)$ if $r_{u}+r_{v} \neq 0$. Here $B_{i,j}$ is a 3D point and $h_{i,j}$ is a positive real $((i,j) \in \lbrace 0,...,m_{u} \rbrace \times \lbrace 0,...,m_{v} \rbrace)$. $h_{i,j}=1\; ((i,j) \in \lbrace 0,...,m_{u} \rbrace \times \lbrace 0,...,m_{v} \rbrace)$ if $r_{u}+r_{v} = 0$.
 
The Bezier surface is defined by the following  parametric equation:  
 
$$ S(u,v)=\frac{\sum_{i=0}^{m_{u}} \sum_{j=0}^{m_{v}} B_{i,j} \cdot h_{i,j} \cdot C_{m_{u}}^{i} \cdot u^{i} \cdot (1-u)^{m_{u}-i} \cdot C_{m_{v}}^{j} \cdot v^{j} \cdot (1-v)^{m_{v}-j}}{\sum_{i=0}^{m_{u}} \sum_{j=0}^{m_{v}} h_{i,j} \cdot C_{m_{u}}^{i} \cdot u^{i} \cdot (1-u)^{m_{u}-i} \cdot C_{m_{v}}^{j} \cdot v^{j} \cdot (1-v)^{m_{v}-j}}, (u,v) \in [0,1] \times [0,1] $$   
 
where $0^{0} \equiv 1$.
 
The example record is interpreted as a Bezier surface  with a u rational flag *r<sub>u</sub>*=1, v rational flag *r<sub>v</sub>*=1, u degree *m<sub>u</sub>*=2, v degree *m<sub>v</sub>*=1, weight poles *B<sub>0,0</sub>*=(0, 0, 1), *h<sub>0,0</sub>*=7, *B<sub>0,1</sub>*=(1, 0, -4), *h<sub>0,1</sub>*=10, *B<sub>1,0</sub>*=(0, 1, -2), *h<sub>1,0</sub>*=8, *B<sub>1,1</sub>*=(1, 1, 5), *h<sub>1,1</sub>*=11, *B<sub>2,0</sub>*=(0, 2, 3), *h<sub>2,0</sub>*=9 and *B<sub>2,1</sub>*=(1, 2, 6), *h<sub>2,1</sub>*=12. The surface is defined by  the following parametric equation:  
 
$$
\begin{align}
S(u,v)= [ (0,0,1) \cdot 7 \cdot (1-u)^{2} \cdot (1-v)+(1,0,-4) \cdot 10 \cdot (1-u)^{2} \cdot v+ (0,1,-2) \cdot 8 \cdot 2 \cdot u \cdot (1-u) \cdot (1-v) + \\
(1,1,5) \cdot 11 \cdot 2 \cdot u \cdot (1-u)  \cdot v+ (0,2,3) \cdot 9 \cdot u^{2} \cdot (1-v)+(1,2,6) \cdot 12 \cdot u^{2} \cdot v] \div [7 \cdot (1-u)^{2} \cdot (1-v)+ \\
10 \cdot (1-u)^{2} \cdot v+ 8 \cdot 2 \cdot u \cdot (1-u) \cdot (1-v)+ 11 \cdot 2 \cdot u \cdot (1-u) \cdot v+ 9 \cdot u^{2} \cdot (1-v)+12 \cdot u^{2} \cdot v ]
\end{align}
$$

<h4><a id="specification__brep_format_5_2_9">B-spline Surface - \< surface record 9 \> </a></h4>
 
**Example**  

```
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
```
 
**BNF-like Definition**

```
	<surface record 9> = "9" <_> <B-spline surface u rational flag> <_> 
	<B-spline surface v rational flag> <_> "0" <_> "0" <_> <B-spline surface u degree> <_> 
	<B-spline surface v degree> <_> <B-spline surface u pole count> <_> 
	<B-spline surface v pole count> <_> <B-spline surface u multiplicity knot count> <_> 
	<B-spline surface v multiplicity knot count> <_> <B-spline surface weight poles> <_\n> 
	<B-spline surface u multiplicity knots> <_\n> <B-spline surface v multiplicity knots>;

	<B-spline surface u rational flag> = <flag>;

	<B-spline surface v rational flag> = <flag>;

	<B-spline surface u degree> = <int>;

	<B-spline surface v degree> = <int>;

	<B-spline surface u pole count> = <int>;

	<B-spline surface v pole count> = <int>;

	<B-spline surface u multiplicity knot count> = <int>;

	<B-spline surface v multiplicity knot count> = <int>;

	<B-spline surface weight poles> =
	(<B-spline surface weight pole group> <_\n>) ^ <B-spline surface u pole count>;

	<B-spline surface weight pole group> =
	(<B-spline surface weight pole> <_>) ^ <B-spline surface v pole count>;

	<B-spline surface weight pole> = <3D point> [<_> <real>];

	<B-spline surface u multiplicity knots> =
	(<B-spline surface u multiplicity knot> <_\n>) ^ <B-spline surface u multiplicity knot count>;

	<B-spline surface u multiplicity knot> = <real> <_> <int>;

	<B-spline surface v multiplicity knots> =
	(<B-spline surface v multiplicity knot> <_\n>) ^ <B-spline surface v multiplicity knot count>;

	<B-spline surface v multiplicity knot> = <real> <_> <int>;
```
 
**Description**  
 
\<surface record 9\> describes a B-spline surface.  The surface data consist of a u rational flag *r<sub>u</sub>*, v rational flag *r<sub>v</sub>*, u degree $m_{u} \leq 25$, v degree $m_{v} \leq 25$, u pole count $n_{u} \geq 2$, v pole count $n_{v} \geq 2$, u multiplicity knot count *k<sub>u</sub>*, v multiplicity knot count *k<sub>v</sub>*, weight poles, u multiplicity  knots, v multiplicity knots.  
 
The weight poles are $n_{u} \cdot n_{v}$ 3D points $B_{i,j}\; ((i,j) \in \lbrace 1,...,n_{u} \rbrace \times \lbrace 1,...,n_{v} \rbrace)$ if $r_{u}+r_{v}=0$. The weight poles are $n_{u} \cdot n_{v}$ pairs $B_{i,j}h_{i,j}\; ((i,j) \in \lbrace 1,...,n_{u} \rbrace \times \lbrace 1,...,n_{v} \rbrace)$ if $r_{u}+r_{v} \neq 0$. Here $B_{i,j}$ is a 3D point and $h_{i,j} $ is a positive real $((i,j) \in \lbrace 1,...,n_{u} \rbrace \times \lbrace 1,...,n_{v} \rbrace)$. $h_{i,j}=1\; ((i,j) \in \lbrace 1,...,n_{u} \rbrace \times \lbrace 1,...,n_{v} \rbrace)$ if $r_{u}+r_{v} = 0$.
 
The u multiplicity knots are *k<sub>u</sub>* pairs $u_{1}q_{1} ... u_{k_{u}}q_{k_{u}}$. Here $u_{i}$ is a knot with multiplicity $q_{i} \geq 1 \;(1\leq i\leq k_{u})$ so that  
 
$$ u_{i} < u_{i+1} \; (1\leq i\leq k_{u}-1), \\
q_{1} \leq m_{u}+1,\; q_{k_{u}} \leq m_{u}+1,\; q_{i} \leq m_{u}\; (2\leq i\leq k_{u}-1),\; \sum_{i=1}^{k_{u}}q_{i}=m_{u}+n_{u}+1. $$  
 
The v multiplicity knots are *k<sub>v</sub>* pairs $v_{1}t_{1} ... v_{k_{v}}t_{k_{v}}$. Here $v_{j}$ is a knot with multiplicity $t_{i} \geq  1\;(1\leq i\leq k_{v})$ so that  
 
$$ v_{j} < v_{j+1} \; (1\leq j\leq k_{v}-1), \\
t_{1} \leq m_{v}+1,\; t_{k_{v}} \leq m_{v}+1,\; t_{j} \leq m_{v}\; (2\leq j\leq k_{v}-1),\; \sum_{j=1}^{k_{v}}t_{j}=m_{v}+n_{v}+1. $$ 
 
The B-spline surface is defined by the following  parametric equation:  
 
$$ S(u,v)=\frac{\sum_{i=1}^{n_{u}} \sum_{j=1}^{n_{v}} B_{i,j} \cdot h_{i,j} \cdot N_{i,m_{u}+1}(u) \cdot M_{j,m_{v}+1}(v)}{\sum_{i=1}^{n_{u}} \sum_{j=1}^{n_{v}} h_{i,j} \cdot N_{i,m_{u}+1}(u) \cdot M_{j,m_{v}+1}(v)}, (u,v) \in [u_{1},u_{k_{u}}] \times [v_{1},v_{k_{v}}] $$     
 
where functions *N<sub>i,j</sub>* and *M<sub>i,j</sub>* have the following recursion definition by *j*:  

$$
\begin{align}
N_{i,1}(u)= \lbrace\begin{matrix}
1\Leftarrow \bar{u}_{i} \leq u \leq \bar{u}_{i+1}
0\Leftarrow u < \bar{u}_{i} \vee  \bar{u}_{i+1} \leq u \end{matrix},\; \\
N_{i,j}(u)=\frac{(u-\bar{u}_{i}) \cdot N_{i,j-1}(u) }{\bar{u}_{i+j-1}-\bar{u}_{i}}+  
\frac{(\bar{u}_{i+j}-u) \cdot N_{i+1,j-1}(u)}{\bar{u}_{i+j}-\bar{u}_{i+1}},\;(2 \leq j \leq m_{u}+1), \; \\
M_{i,1}(v)=\lbrace\begin{matrix}
1\Leftarrow \bar{v}_{i} \leq v \leq \bar{v}_{i+1}\\ 
0\Leftarrow v < \bar{v}_{i} \vee  \bar{v}_{i+1} \leq v \end{matrix},\; \\
M_{i,j}(v)=\frac{(v-\bar{v}_{i}) \cdot M_{i,j-1}(v) }{\bar{v}_{i+j-1}-\bar{v}_{i}}+ \frac{(\bar{v}_{i+j}-v) \cdot M_{i+1,j-1}(v)}{\bar{v}_{i+j}-\bar{v}_{i+1}},\;(2 \leq j \leq m_{v}+1); 
\end{align}
$$

where

$$ \bar{u}_{i}=u_{j}\; (1 \leq j \leq k_{u},\; \sum_{l=1}^{j-1}q_{l} \leq i \leq \sum_{l=1}^{j}q_{l}), \\
 \bar{v}_{i}=v_{j}\; (1 \leq j \leq k_{v},\; \sum_{l=1}^{j-1}t_{l} \leq i \leq \sum_{l=1}^{j}t_{l}); $$  
 
The example record is interpreted as a B-spline  surface with a u rational flag *r<sub>u</sub>*=1, v rational flag *r<sub>v</sub>*=1, u degree *m<sub>u</sub>*=1, v degree *m<sub>v</sub>*=1, u pole count *n<sub>u</sub>*=3, v pole count *n<sub>v</sub>*=2, u multiplicity knot count *k<sub>u</sub>*=5, v multiplicity knot count *k<sub>v</sub>*=4, weight poles *B<sub>1,1</sub>*=(0, 0, 1), *h<sub>1,1</sub>*=7, *B<sub>1,2</sub>*=(1, 0, -4), *h<sub>1,2</sub>*=10, *B<sub>2,1</sub>*=(0, 1, -2), *h<sub>2,1</sub>*=8, *B<sub>2,2</sub>*=(1, 1, 5), *h<sub>2,2</sub>*=11, *B<sub>3,1</sub>*=(0, 2, 3), *h<sub>3,1</sub>*=9 and *B<sub>3,2</sub>*=(1, 2, 6), *h<sub>3,2</sub>*=12, u multiplicity knots *u<sub>1</sub>*=0, *q<sub>1</sub>*=1, *u<sub>2</sub>*=0.25, *q<sub>2</sub>*=1, *u<sub>3</sub>*=0.5, *q<sub>3</sub>*=1, *u<sub>4</sub>*=0.75, *q<sub>4</sub>*=1 and *u<sub>5</sub>*=1, *q<sub>5</sub>*=1, v multiplicity  knots *v<sub>1</sub>*=0, *r<sub>1</sub>*=1, *v<sub>2</sub>*=0.3, *r<sub>2</sub>*=1, *v<sub>3</sub>*=0.7, *r<sub>3</sub>*=1 and *v<sub>4</sub>*=1, *r<sub>4</sub>*=1. The B-spline surface is defined  by the following parametric equation:  
 
$$ 
\begin{align} 
S(u,v)= [ (0,0,1) \cdot 7 \cdot N_{1,2}(u) \cdot M_{1,2}(v)+(1,0,-4) \cdot 10 \cdot N_{1,2}(u) \cdot M_{2,2}(v)+ \\
(0,1,-2) \cdot 8 \cdot N_{2,2}(u) \cdot M_{1,2}(v)+(1,1,5) \cdot 11 \cdot N_{2,2}(u) \cdot M_{2,2}(v)+ \\
(0,2,3) \cdot 9 \cdot N_{3,2}(u) \cdot M_{1,2}(v)+(1,2,6) \cdot 12 \cdot N_{3,2}(u) \cdot M_{2,2}(v)] \div \\
[7 \cdot N_{1,2}(u) \cdot M_{1,2}(v)+10 \cdot N_{1,2}(u) \cdot M_{2,2}(v)+ 8 \cdot N_{2,2}(u) \cdot M_{1,2}(v)+ \\ 
11 \cdot N_{2,2}(u) \cdot M_{2,2}(v)+ 9 \cdot N_{3,2}(u) \cdot M_{1,2}(v)+12 \cdot N_{3,2}(u) \cdot M_{2,2}(v) ] 
\end{align} 
$$

<h4><a id="specification__brep_format_5_2_10">Rectangular Trim Surface - < surface  record 10 ></a></h4>
 
**Example**  

```
    10 -1 2 -3 4  
    1 1 2 3 0 0 1 1 0 -0 -0 1 0   
```
 
**BNF-like Definition**

```
	<surface record 10> = "10" <_> <trim surface u min> <_> <trim surface u max> <_> 
	<trim surface v min> <_> <trim surface v max> <_\n> <surface record>;

	<trim surface u min> = <real>;

	<trim surface u max> = <real>;

	<trim surface v min> = <real>;

	<trim surface v max> = <real>;  
```
 
**Description**  
 
\<surface  record 10\> describes a rectangular trim  surface. The surface data consist of reals *u<sub>min</sub>*, *u<sub>max</sub>*, *v<sub>min</sub>* and *v<sub>max</sub>* and a \<surface record\> so that *u<sub>min</sub>* < *u<sub>max</sub>* and *v<sub>min</sub>* < *v<sub>max</sub>*. The rectangular  trim surface is a restriction of the base surface *B* described in the record to the set $[u_{min},u_{max}] \times [v_{min},v_{max}] \subseteq domain(B)$. The rectangular  trim surface is defined by the following parametric equation:  
 
$$ S(u,v)=B(u,v),\; (u,v) \in [u_{min},u_{max}] \times [v_{min},v_{max}] . $$  
 
The example record is interpreted as a rectangular  trim surface to the set [-1, 2]x[-3, 4] for the base surface $B(u,v)=(1,2,3)+u \cdot (1,0,0)+v \cdot (0,1,0)$. The rectangular  trim surface is defined by the following parametric equation: $B(u,v)=(1,2,3)+u \cdot (1,0,0)+ v \cdot (0,1,0),\; (u,v) \in [-1,2] \times [-3,4]$. 
 
 
<h4><a id="specification__brep_format_5_2_11">Offset Surface - \< surface record 11 \> </a></h4>
 
**Example**  
 
```
    11 -2  
    1 1 2 3 0 0 1 1 0 -0 -0 1 0   
```
 
**BNF-like Definition**

```
	<surface record 11> = "11" <_> <surface record distance> <_\n> <surface record>;

	<surface record distance> = <real>;  
```
 
**Description**  
 
\<surface record 11\> describes an offset surface.  
The offset surface data consist of a distance *d* and a \<surface record\>. The  offset surface is the result of offsetting the base surface *B* described in the record to the distance *d* along the normal *N* of  surface *B*.  The offset surface is defined by the following parametric equation:  
 
$$ S(u,v)=B(u,v)+d \cdot N(u,v),\; (u,v) \in domain(B) . \\
N(u,v) = [S'_{u}(u,v),S'_{v}(u,v)] $$

if $[S'_{u}(u,v),S'_{v}(u,v)] \neq \vec{0}$.  
 
The example record is interpreted as an offset surface  with a distance *d*=-2 and  base surface $B(u,v)=(1,2,3)+u \cdot (1,0,0)+v \cdot (0,1,0)$.  The offset surface is defined by the following parametric equation: $S(u,v)=(1,2,3)+u \cdot (1,0,0)+v \cdot (0,1,0)-2 \cdot (0,0,1)$.
 
 
<h3><a id="specification__brep_format_5_3">2D curves</a></h3>
 
**Example**  

```
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
```
 
**BNF-like Definition**

```
	<2D curves> = <2D curve header> <_\n> <2D curve records>;

	<2D curve header> = "Curve2ds" <_> <2D curve count>;

	<2D curve count> = <int>;

	<2D curve records> = <2D curve record> ^ <2D curve count>;

	<2D curve record> =
	<2D curve record 1> |
	<2D curve record 2> |
	<2D curve record 3> |
	<2D curve record 4> |
	<2D curve record 5> |
	<2D curve record 6> |
	<2D curve record 7> |
	<2D curve record 8> |
	<2D curve record 9>;  
```
 
<h4><a id="specification__brep_format_5_3_1">Line - \<2D curve record 1\> </a></h4>
 
**Example**  

```
    1 3 0 0 -1   
```
 
**BNF-like Definition**

```
    <2D curve record 1> = "1" <_> <2D point> <_> <2D direction> <_\n>;  
```
 
**Description**  
 
\<2D curve record 1\> describes a line. The line  data consist of a 2D point *P* and a 2D direction *D*. The line passes through the point  *P*, has the  direction *D* and  is defined by the following parametric equation:  
 
$$ C(u)=P+u \cdot D, \; u \in (-\infty,\; \infty). $$  
 
The example record is interpreted as a line which  passes through a point *P*=(3,0), has a direction *D*=(0,-1) and is defined by the  following parametric equation: $C(u)=(3,0)+ u \cdot (0,-1)$.  
 
 
<h4><a id="specification__brep_format_5_3_2">Circle - \<2D curve record 2\> </a></h4>
 
**Example**  

```
    2 1 2 1 0 -0 1 3  
```
 
**BNF-like Definition**

~~~~{.cpp}
	<2D curve record 2> = "2" <_> <2D circle center> <_> <2D circle Dx> <_> <2D circle Dy> <_> <2D circle radius> <_\n>;

	<2D circle center> = <2D point>;

	<2D circle Dx> = <2D direction>;

	<2D circle Dy> = <2D direction>;

	<2D circle radius> = <real>;  
~~~~
 
**Description**  
 
\<2D curve record 2\> describes a circle. The  circle data consist of a 2D point *P*, orthogonal 2D directions *D<sub>x</sub>* and *D<sub>y</sub>* and a non-negative  real *r*. The circle has a center *P*. The circle plane is parallel to directions *D<sub>x</sub>* and *D<sub>y</sub>*. The  circle has a radius *r* and is defined by the following  parametric equation:  
 
$$ C(u)=P+r \cdot (cos(u) \cdot D_{x} + sin(u) \cdot D_{y}),\; u \in [0,\; 2 \cdot \pi) . $$ 
 
The example record is interpreted as a circle which  has a center *P*=(1,2).  The circle plane is parallel to directions *D<sub>x</sub>*=(1,0) and *D<sub>y</sub>*=(0,1). The circle has a radius *r*=3 and is defined by  the following parametric equation: $C(u)=(1,2)+3 \cdot (cos(u) \cdot (1,0) + sin(u) \cdot (0,1))$.  
 
 
<h4><a id="specification__brep_format_5_3_3">Ellipse - \<2D curve record 3\> </a></h4>
 
**Example**  

```
    3 1 2 1 0 -0 1 4 3  
```
 
**BNF-like Definition**

```
	<2D curve record 3> = "3" <_> <2D ellipse center> <_> <2D ellipse Dmaj> <_> 
	<2D ellipse Dmin> <_> <2D ellipse Rmaj> <_> <2D ellipse Rmin> <_\n>;

	<2D ellipse center> = <2D point>;

	<2D ellipse Dmaj> = <2D direction>;

	<2D ellipse Dmin> = <2D direction>;

	<2D ellipse Rmaj> = <real>;

	<2D ellipse Rmin> = <real>; 
```
 
**Description**  
 
\<2D curve record 3\> describes an ellipse. The  ellipse data are 2D point *P*, orthogonal 2D directions *D<sub>maj</sub>* and *D<sub>min</sub>* and non-negative reals *r<sub>maj</sub>* and *r<sub>min</sub>* that *r<sub>maj</sub>* $\leq$ *r<sub>min</sub>*. The ellipse has a center *P*, major and  minor axis directions *D<sub>maj</sub>* and *D<sub>min</sub>*, major and minor radii *r<sub>maj</sub>* and *r<sub>min</sub>* and is defined by  the following parametric equation:  
 
$$ C(u)=P+r_{maj} \cdot cos(u) \cdot D_{maj}+r_{min} \cdot sin(u) \cdot D_{min},\; u \in [0,\; 2 \cdot \pi) . $$  
 
The example record is interpreted as an ellipse which  has a center *P*=(1,2),  major and minor axis directions *D<sub>maj</sub>*=(1,0) and *D<sub>min</sub>*=(0,1), major and minor radii *r<sub>maj</sub>*=4 and *r<sub>min</sub>*=3 and is defined by  the following parametric equation: $C(u)=(1,2)+4 \cdot cos(u) \cdot (1,0)+3 \cdot sin(u) \cdot (0,1)$.  
 
 
<h4><a id="specification__brep_format_5_3_4">Parabola - \<2D curve record 4\> </a></h4>
 
**Example**  

```
    4 1 2 1 0 -0 1 16  
```
 
**BNF-like Definition**

```
	<2D curve record 4> = "4" <_> <2D parabola origin> <_> <2D parabola Dx> <_> 
	<2D parabola Dy> <_> <2D parabola focal length> <_\n>;

	<2D parabola origin> = <2D point>;

	<2D parabola Dx> = <2D direction>;

	<2D parabola Dy> = <2D direction>;

	<2D parabola focal length> = <real>;  
```
 
**Description**  
 
\<2D curve record 4\> describes a parabola. The  parabola data consist of a 2D point *P*, orthogonal 2D directions *D<sub>x</sub>* and *D<sub>y</sub>* and a non-negative  real *f*. The parabola coordinate system has its origin *P* and axis directions *D<sub>x</sub>* and *D<sub>y</sub>*. The parabola has a focus  length *f* and  is defined by the following parametric equation:  
 
$$ C(u)=P+\frac{u^{2}}{4 \cdot f} \cdot D_{x}+u \cdot D_{y},\; u \in (-\infty,\; \infty) \Leftarrow f \neq 0;\\
C(u)=P+u \cdot D_{x},\; u \in (-\infty,\; \infty) \Leftarrow f = 0\; (degenerated\;case). $$ 
 
The example record is interpreted as a parabola in plane which passes through a point *P*=(1,2) and is parallel to directions *D<sub>x</sub>*=(1,0) and *D<sub>y</sub>*=(0,1). The parabola has a focus length *f*=16 and  is defined by the following parametric equation: $C(u)=(1,2)+ \frac{u^{2}}{64} \cdot (1,0)+u \cdot (0,1)$.  
 
 
<h4><a id="specification__brep_format_5_3_5">Hyperbola - \<2D curve record 5\> </a></h4>
**Example**  
 
5 1 2 1 0 -0 1 3 4  
 

**BNF-like Definition**

```
	<2D curve record 5> = "5" <_> <2D hyperbola origin> <_> <2D hyperbola Dx> <_> 
	<2D hyperbola Dy> <_> <2D hyperbola Kx> <_> <2D hyperbola Ky> <_\n>;

	<2D hyperbola origin> = <2D point>;

	<2D hyperbola Dx> = <2D direction>;

	<2D hyperbola Dy> = <2D direction>;

	<2D hyperbola Kx> = <real>;

	<2D hyperbola Ky> = <real>;  
```
 
**Description**  
 
\<2D curve record 5\> describes a hyperbola. The  hyperbola data consist of a 2D point *P*, orthogonal 2D directions *D<sub>x</sub>* and *D<sub>y</sub>* and non-negative reals *k<sub>x</sub>* and *k<sub>y</sub>*. The hyperbola coordinate system has origin *P* and axis directions *D<sub>x</sub>* and *D<sub>y</sub>*. The hyperbola is defined by  the following parametric equation:  
 
$$ C(u)=P+k_{x} \cdot cosh(u) D_{x}+k_{y} \cdot sinh(u) \cdot D_{y},\; u \in (-\infty,\; \infty). $$  
 
The example record is interpreted as a hyperbola with  coordinate system which has origin *P*=(1,2) and axis directions *D<sub>x</sub>*=(1,0) and *D<sub>y</sub>*=(0,1). Other data for the hyperbola are *k<sub>x</sub>*=5 and *k<sub>y</sub>*=4. The hyperbola is defined  by the following parametric equation: $C(u)=(1,2)+3 \cdot cosh(u) \cdot (1,0)+4 \cdot sinh(u) \cdot (0,1)$.  
 
 
<h4><a id="specification__brep_format_5_3_6">Bezier Curve - <2D curve record 6></a></h4>
 
**Example**  

```
6 1 2 0 1  4 1 -2  5 2 3  6   
```
 
**BNF-like Definition**

```
	<2D curve record 6> = "6" <_> <2D Bezier rational flag> <_> <2D Bezier degree> 
	<2D Bezier weight poles> <_\n>;

	<2D Bezier rational flag> = <flag>;

	<2D Bezier degree> = <int>;

	<2D Bezier weight poles> = (<_> <2D Bezier weight pole>) ^ (<2D Bezier degree> <+> “1”);

	<2D Bezier weight pole> = <2D point> [<_> <real>];
```
 
**Description**  
 
\<2D curve record 6\> describes a Bezier curve.  The curve data consist of a rational flag *r*, a degree $m \leq 25$ and weight poles.  
 
The weight poles are *m*+1 2D points *B<sub>0</sub> ... B<sub>m</sub>* if the flag *r* is 0. The weight poles are *m*+1 pairs *B<sub>0</sub>h<sub>0</sub> ... B<sub>m</sub>h<sub>m</sub>* if the flag *r* is 1. Here *B<sub>i</sub>* is a 2D point and *h<sub>i</sub>* is a positive real $(0\leq i\leq m)$. *h<sub>i</sub>*=1 $(0\leq i\leq m)$ if the flag *r* is 0.  
 
The Bezier curve is defined by the following  parametric equation:  
 
$$ C(u)= \frac{\sum_{i=0}^{m} B_{i} \cdot h_{i} \cdot C_{m}^{i} \cdot u^{i} \cdot (1-u)^{m-i}}{\sum_{i=0}^{m} h_{i} \cdot C_{m}^{i} \cdot u^{i} \cdot (1-u)^{m-i}},\; u \in [0,1] $$  
 
where $0^{0} \equiv 1$.  
 
The example record is interpreted as a Bezier curve  with a rational flag *r*=1, a degree *m*=2 and weight poles *B<sub>0</sub>*=(0,1), *h<sub>0</sub>*=4, *B<sub>1</sub>*=(1,-2), *h<sub>1</sub>*=5 and *B<sub>2</sub>*=(2,3), *h<sub>2</sub>*=6. The Bezier curve is defined  by the following parametric equation:  
 
$$ C(u)= \frac{(0,1) \cdot 4 \cdot (1-u)^{2}+(1,-2) \cdot 5 \cdot 2 \cdot u \cdot (1-u)+(2,3) \cdot 6 \cdot u^{2}}{ 4 \cdot (1-u)^{2}+5 \cdot 2 \cdot u \cdot (1-u)+6 \cdot u^{2}} . $$  
 
 
<h4><a id="specification__brep_format_5_3_7">B-spline Curve -  <2D curve record 7></a></h4>
 
**Example**  

```
7 1 0  1 3 5  0 1  4 1 -2  5  2 3  6  
 0 1 0.25 1 0.5 1 0.75 1 1 1  
```
 
**BNF-like Definition**

~~~~{.cpp}
	<2D curve record 7> = "7" <_> <2D B-spline rational flag> <_> "0" <_> <2D B-spline degree> <_> <2D B-spline pole count> <_> <2D B-spline multiplicity knot count> <2D B-spline weight poles> <_\n> <2D B-spline multiplicity knots> <_\n>;

	<2D B-spline rational flag> = <flag>;

	<2D B-spline degree> = <int>;

	<2D B-spline pole count> = <int>;

	<2D B-spline multiplicity knot count> = <int>;

	<2D B-spline weight poles> = <2D B-spline weight pole> ^ <2D B-spline pole count>;

	<2D B-spline weight pole> = <_> <2D point> [<_> <real>];

	<2D B-spline multiplicity knots> =
	<2D B-spline multiplicity knot> ^ <2D B-spline multiplicity knot count>;

	<2D B-spline multiplicity knot> = <_> <real> <_> <int>;
~~~~
 
**Description**  
 
\<2D curve record 7\> describes a B-spline curve.  The curve data consist of a rational flag *r*, a degree $m \leq 25$, a pole count $n \geq 2$, a multiplicity knot count *k*, weight poles and  multiplicity knots.  
 
The weight poles are *n* 2D points *B<sub>1</sub> ... B<sub>n</sub>* if the flag *r* is 0. The weight  poles are *n* pairs  *B<sub>1</sub>h<sub>1</sub> ... B<sub>n</sub>h<sub>n</sub>* if the flag  *r* is 1. Here *B<sub>i</sub>* is a 2D point and *h<sub>i</sub>* is a positive real $(1\leq i\leq n)$. *h<sub>i</sub>*=1 $(1\leq i\leq n) $ if the flag *r* is 0.  
 
The multiplicity knots are *k* pairs *u<sub>1</sub>q<sub>1</sub> ... u<sub>k</sub>q<sub>k</sub>*. Here *u<sub>i</sub>* is a knot with multiplicity $q_{i} \geq 1\; (1 \leq i \leq k)$ so that  
 
$$ u_{i} < u_{i+1}\; (1 \leq i \leq k-1), \\
q_{1} \leq m+1,\; q_{k} \leq m+1,\; q_{i} \leq m\; (2 \leq i \leq k-1),\; \sum_{i=1}^{k}q_{i}=m+n+1 . $$  
 
The B-spline curve is defined by the following  parametric equation:  
 
$$ C(u)= \frac{\sum_{i=1}^{n} B_{i} \cdot h_{i} \cdot N_{i,m+1}(u) }{\sum_{i=1}^{n} h_{i} \cdot N_{i,m+1}(u)},\; u \in [u_{1},\; u_{k}] $$

where functions *N<sub>i,j</sub>* have the following recursion definition  by *j*
 
$$ N_{i,1}(u)=\lbrace\begin{matrix}
1\Leftarrow \bar{u}_{i} \leq u \leq \bar{u}_{i+1}\\ 
0\Leftarrow u < \bar{u}_{i} \vee  \bar{u}_{i+1} \leq u \end{matrix},\;
N_{i,j}(u)=\frac{(u-\bar{u}_{i}) \cdot N_{i,j-1}(u) }{\bar{u}_{i+j-1}-\bar{u}_{i}}+ \frac{(\bar{u}_{i+j}-u) \cdot N_{i+1,j-1}(u)}{\bar{u}_{i+j}-\bar{u}_{i+1}},\;(2 \leq j \leq m+1) $$ 
 
where  
 
$$\bar{u}_{i}=u_{j}\; (1\leq j\leq k,\; \sum_{l=1}^{j-1}q_{l}+1 \leq i \leq \sum_{l=1}^{j}q_{l})$$
 
The example record is interpreted as a B-spline curve with a rational flag *r*=1, a degree *m*=1, a pole count *n*=3, a multiplicity knot count *k*=5, weight poles *B<sub>1</sub>*=(0,1), *h<sub>1</sub>*=4, *B<sub>2</sub>*=(1,-2), *h<sub>2</sub>*=5 and *B<sub>3</sub>*=(2,3), *h<sub>3</sub>*=6 and multiplicity knots *u<sub>1</sub>*=0, *q<sub>1</sub>*=1, *u<sub>2</sub>*=0.25, *q<sub>2</sub>*=1, *u<sub>3</sub>*=0.5, *q<sub>3</sub>*=1, *u<sub>4</sub>*=0.75, *q<sub>4</sub>*=1 and *u<sub>5</sub>*=1, *q<sub>5</sub>*=1. The B-spline curve is defined  by the following parametric equation:  
 
$$ C(u)= \frac{(0,1) \cdot 4 \cdot N_{1,2}(u)+(1,-2) \cdot 5 \cdot N_{2,2}(u)+(2,3) \cdot 6 \cdot N_{3,2}(u)}{ 4 \cdot N_{1,2}(u)+5 \cdot N_{2,2}(u)+6 \cdot N_{3,2}(u)} . $$ 
 
 
<h4><a id="specification__brep_format_5_3_8">Trimmed Curve - \<2D curve record 8\> </a></h4>
 
**Example**  

```
    8 -4 5  
    1 1 2 1 0   
```
 
**BNF-like Definition**

```
	<2D curve record 8> = "8" <_> <2D trimmed curve u min> <_> <2D trimmed curve u max> <_\n> 
	<2D curve record>;

	<2D trimmed curve u min> = <real>;

	<2D trimmed curve u max> = <real>;
```
 
**Description**
 
\<2D curve record 8\> describes a trimmed curve. The trimmed curve data consist of reals *u<sub>min</sub>* and *u<sub>max</sub>* and a \<2D curve record\> so that *u<sub>min</sub>* < *u<sub>max</sub>*. The trimmed curve  is a restriction of the base curve *B* described in the record to the segment $[u_{min},\;u_{max}]\subseteq domain(B)$. The trimmed curve is  defined by the following parametric equation:  
 
$$ C(u)=B(u),\; u \in [u_{min},\;u_{max}] . $$ 
 
The example record is interpreted as a trimmed curve with *u<sub>min</sub>*=-4, *u<sub>max</sub>*=5 and base curve $B(u)=(1,2)+u \cdot (1,0)$. The trimmed curve is defined by the following parametric equation: $C(u)=(1,2)+u \cdot (1,0),\; u \in [-4,5]$.  
 
 
<h4><a id="specification__brep_format_5_3_9">Offset Curve - \<2D curve record 9\> </a></h4>
 
**Example**  

```
    9 2  
    1 1 2 1 0   
```
 
**BNF-like Definition**
 
```
<2D curve record 9> = "9" <_> <2D offset curve distance> <_\n> <2D curve record>;

<2D offset curve distance> = <real>;
```
 
**Description**  
 
\<2D curve record 9\> describes an offset curve. The offset curve data consist of a distance *d* and a \<2D curve record\>. The offset curve is the result of offsetting the base curve *B* described in the record to the distance *d* along  the vector $(B'_{Y}(u),\; -B'_{X}(u)) \neq \vec{0}$ where $B(u)=(B'_{X}(u),\; B'_{Y}(u))$. The offset curve is defined by the following parametric equation:  
 
$$ C(u)=B(u)+d \cdot (B'_{Y}(u),\; -B'_{X}(u)),\; u \in domain(B) . $$  
 
The example record is interpreted as an offset curve  with a distance *d*=2 and  base curve $B(u)=(1,2)+u \cdot (1,0)$ and is defined by the following parametric equation: $C(u)=(1,2)+u \cdot (1,0)+2 \cdot (0,-1)$.  
 
<h3><a id="specification__brep_format_5_4">3D polygons</a></h3>
 
**Example**  

```
    Polygon3D 1  
    2 1  
    0.1  
    1 0 0 2 0 0   
    0 1   
```
 
**BNF-like Definition**

```
	<3D polygons> = <3D polygon header> <_\n> <3D polygon records>;

	<3D polygon header> = "Polygon3D" <_> <3D polygon record count>;

	<3D polygon records> = <3D polygon record> ^ <3D polygon record count>;

	<3D polygon record> =
	<3D polygon node count> <_> <3D polygon flag of parameter presence> <_\n>
	<3D polygon deflection> <_\n>
	<3D polygon nodes> <_\n>
	[<3D polygon parameters> <_\n>];

	<3D polygon node count> = <int>;

	<3D polygon flag of parameter presence> = <flag>;

	<3D polygon deflection> = <real>;

	<3D polygon nodes> = (<3D polygon node> <_>) ^ <3D polygon node count>;

	<3D polygon node> = <3D point>;

	<3D polygon u parameters> = (<3D polygon u parameter> <_>) ^ <3D polygon node count>;

	<3D polygon u parameter> = <real>;
```
 
**Description**  
 
\<3D polygons\> record describes a 3D polyline *L* which approximates a 3D curve *C*.  The polyline data consist of a node count $m \geq 2$, a parameter presence flag *p*, a deflection $  \geq 0$, nodes $N_{i}\; (1\leq i \leq m)$ and parameters $u_{i}\; (1\leq i \leq m)$. The parameters are present only if *p*=1. The polyline *L* passes  through the nodes. The deflection *d* describes the deflection of polyline *L* from the curve *C*:  
 
$$ \underset{P \in C}{max}\; \underset{Q \in L}{min}|Q-P| \leq d . $$  
 
The parameter $u_{i}\; (1\leq i \leq m)$ is the parameter of the node *N<sub>i</sub>* on the curve *C*:  
 
$$ C(u_{i})=N_{i} . $$  
 
The example record describes a polyline from *m*=2 nodes with a parameter presence flag *p*=1, a deflection *d*=0.1, nodes *N<sub>1</sub>*=(1,0,0) and *N<sub>2</sub>*=(2,0,0) and parameters *u<sub>1</sub>*=0 and *u<sub>2</sub>*=1.  
 
 
<h3><a id="specification__brep_format_6_4">Triangulations</a></h3>
 
**Example**  

```
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
```
 
**BNF-like Definition**

~~~~{.cpp}
	<triangulations> = <triangulation header> <_\n> <triangulation records>;

	<triangulation header> = "Triangulations" <_> <triangulation count>;

	<triangulation records> = <triangulation record> ^ <triangulation count>;

	<triangulation record> = <triangulation node count> <_> <triangulation triangle count> <_> <triangulation parameter presence flag> [<_> <need to write normals flag>] <_> <triangulation deflection> <_\n>
	<triangulation nodes> [<_> <triangulation u v parameters>] <_> <triangulation triangles> [<_> <triangulation normals>] <_\n>;

	<triangulation node count> = <int>;

	<triangulation triangle count> = <int>;

	<triangulation parameter presence flag> = <flag>;

	<need to write normals flag> = <flag>;

	<triangulation deflection> = <real>;

	<triangulation nodes> = (<triangulation node> <_>) ^ <triangulation node count>;

	<triangulation node> = <3D point>;

	<triangulation u v parameters> =	
	(<triangulation u v parameter pair> <_>) ^ <triangulation node count>;

	<triangulation u v parameter pair> = <real> <_> <real>;

	<triangulation triangles> = (<triangulation triangle> <_>) ^ <triangulation triangle count>;

	<triangulation triangle> = <int> <_> <int> <_> <int>;

	<triangulation normals> = (<triangulation normal> <_>) ^ <triangulation node count> ^ 3;

	<triangulation normal> = <short real>.
~~~~
 
**Description**  

\<triangulation u v parameters\> are used in version 2 or later.
\<need to write normals flag\> and \<triangulation normals\> are used in version 3.
 
\<triangulation record\> describes a triangulation  *T* which  approximates a surface *S*. The triangulation data consist of a node  count $m \geq 3$, a triangle count $k \geq 1$, a parameter  presence flag *p*, a deflection $d \geq 0$, nodes $N_{i}\; (1\leq i \leq m)$, parameter pairs $u_{i}\; v_{i}\; (1\leq i \leq m)$, triangles $n_{j,1}\; n_{j,2}\; n_{j,3}\; (1\leq j \leq k,\; n_{j,l} \in \lbrace1,...,m \rbrace\; (1\leq l\leq 3))$. The parameters are present  only if *p*=1. The deflection describes the triangulation deflection from the surface:  
 
$$ \underset{P \in S}{max}\; \underset{Q \in T}{min}|Q-P| \leq d . $$  
 
The parameter pair $u_{i}\; v_{i}\; (1\leq i \leq m)$ describes the parameters of node *N<sub>i</sub>* on the surface:  
 
$$ S(u_{i},v_{i})=N_{i} . $$  
 
The triangle $n_{j,1}\; n_{j,2}\; n_{j,3}\; (1\leq j \leq k)$ is interpreted as a triangle of nodes $N_{n_{j},1}\; N_{n_{j},2}$ and $ N_{n_{j},3} $ with circular traversal of the nodes in the order $N_{n_{j},1}\;  N_{n_{j},2}$ and $N_{n_{j},3}$. From any side of the triangulation *T* all its triangles  have the same direction of the node circular traversal: either clockwise or  counterclockwise.  
 
Triangulation record  

``` 
    4 2 1 0  
    0 0 0 0 0 3 0 2 3 0 2 0 0 0 3  0 3 -2 0 -2 2 4 3 2 1 4   
```

describes a triangulation with *m*=4 nodes, *k*=2 triangles, parameter presence flag *p*=1, deflection *d*=0, nodes *N<sub>1</sub>*=(0,0,0), *N<sub>2</sub>*=(0,0,3), *N<sub>3</sub>*=(0,2,3) and *N<sub>4</sub>*=(0,2,0), parameters (*u<sub>1</sub>*, *v<sub>1</sub>*)=(0,0), (*u<sub>2</sub>*, *v<sub>2</sub>*)=(3,0), (*u<sub>3</sub>*, *v<sub>3</sub>*)=(3,-2) and (*u<sub>4</sub>*, *v<sub>4</sub>*)=(0,-2), and triangles (*n<sub>1,1</sub>*, *n<sub>1,2</sub>*, *n<sub>1,3</sub>*)=(2,4,3) and (*n<sub>2,1</sub>*, *n<sub>2,2</sub>*, *n<sub>2,3</sub>*)=(2,1,4). From the point (1,0,0) ((-1,0,0)) the triangles have clockwise (counterclockwise) direction of the node circular traversal.  
 
 
<h3><a id="specification__brep_format_6_5">Polygons on triangulations</a></h3>
 
**Example**  

```
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
```
 
**BNF-like Definition**

```
	<polygons on triangulations> = <polygons on triangulations header> <_\n>
	<polygons on triangulations records>;

	<polygons on triangulations header> =
	"PolygonOnTriangulations" <_> <polygons on triangulations record count>;

	<polygons on triangulations record count> = <int>;

	<polygons on triangulations records> =
	<polygons on triangulations record> ^ <polygons on triangulations record count>;

	<polygons on triangulations record> =
	<polygons on triangulations node count> <_> <polygons on triangulations node numbers> <_\n>
	"p" <_> <polygons on triangulations deflection> <_>
	<polygons on triangulations parameter presence flag>
	[<_> <polygons on triangulations u parameters>] <_\n>;

	<polygons on triangulations node count> = <int>;

	<polygons on triangulations node numbers> =
	<polygons on triangulations node number> ^ <polygons on triangulations node count>;

	<polygons on triangulations node number> = <int>;

	<polygons on triangulations deflection> = <real>;

	<polygons on triangulations parameter presence flag> = <flag>;

	<polygons on triangulations u parameters> =
	(<polygons on triangulations u parameter> <_>) ^ <polygons on triangulations node count>;

	<polygons on triangulations u parameter> = <real>;
```
 
**Description**  
 
\<polygons on triangulations\> describes a polyline  *L* on a triangulation  which approximates a curve *C*. The polyline data consist of a node  count $m \geq 2$,  node numbers $n_{i} \geq 1$,  deflection $d \geq 0$, a parameter presence flag *p* and parameters $u_{i}\; (1\leq i\leq m)$. The parameters are present only if *p*=1. The  deflection *d* describes  the deflection of polyline *L* from the curve *C*:  
 
$$ \underset{P \in C}{max}\; \underset{Q \in L}{min}|Q-P| \leq d . $$  
 
Parameter $u_{i}\; (1\leq i\leq m)$ is *n<sub>i</sub>*-th node *C(u<sub>i</sub>)* parameter on curve *C*.  
 
 
<h3><a id="specification__brep_format_6_6">Geometric Sense of a Curve</a></h3>
 
Geometric sense of curve *C* described above is determined by the direction  of parameter *u* increasing.  
 
 
<h2><a id="specification__brep_format_7">Shapes</a></h2>
 
An example of section shapes and a whole  *.brep file are given in chapter 7 [Appendix](#specification__brep_format_8).  
 
 
**BNF-like Definition**

```
	<shapes> = <shape header> <_\n> <shape records> <_\n> <shape final record>;

	<shape header> = "TShapes" <_> <shape count>;

	<shape count> = <int>;

	<shape records> = <shape record> ^ <shape count>;

	<shape record> = <shape subrecord> <_\n> <shape flag word> <_\n> <shape subshapes> <_\n>;

	<shape flag word> = <flag> ^ 7;

	<shape subshapes> = (<shape subshape> <_>)* "*";

	<shape subshape> =
	<shape subshape orientation> <shape subshape number> <_> <shape location number>;

	<shape subshape orientation> = "+" | "-" | "i" | "e";

	<shape subshape number> = <int>;

	<shape location number> = <int>;

	<shape final record> = <shape subshape>;

	<shape subrecord> =
	("Ve" <_\n> <vertex data> <_\n>) |
	("Ed" <_\n> <edge data> <_\n>) |
	("Wi" <_\n> <_\n>) |
	("Fa" <_\n> <face data>) |
	("Sh" <_\n> <_\n>) |
	("So" <_\n> <_\n>) |
	("CS" <_\n> <_\n>) |
	("Co" <_\n> <_\n>);
```
 
**Description**  
 
\<shape flag word\> $f_{1}\; f_{2}\; f_{3}\; f_{4}\; f_{5}\; f_{6}\; f_{7}$ \<flag\>s $f_{i}\;(1\leq i \leq 7)$ are interpreted as shape  flags in the following way:  

  *  $f_{1}$ -- free;  
  *  $f_{2}$ -- modified;  
  *  $f_{3}$ -- IGNORED(version 1 only) \\ checked (version 2 or later);  
  *  $f_{4}$ -- orientable;  
  *  $f_{5}$ -- closed;  
  *  $f_{6}$ -- infinite;  
  *  $f_{7}$ -- convex.  
  
The flags are used in a special way [1].  
 
\<shape subshape orientation\> is interpreted in  the following way:  

  *  + -- forward;  
  *  - -- reversed;  
  *  i -- internal;  
  *  e -- external.  
  
\<shape subshape orientation\> is used in a special  way [1].  
 
\<shape subshape number\> is the number of a \<shape record\> which is located in this section above the \<shape subshape  number\>. \<shape record\> numbering is backward and starts from 1.  
 
\<shape subrecord\> types are interpreted in the  following way: 
 
  *  "Ve" -- vertex;  
  *  "Ed" -- edge;  
  *  "Wi" -- wire;  
  *  "Fa" -- face;  
  *  "Sh" -- shell;  
  *  "So" -- solid;  
  *  "CS" -- compsolid;  
  *  "Co" -- compound.  
 
\<shape final record\> determines the orientation  and location for the whole model.  
 
<h3><a id="specification__brep_format_7_1">Common Terms</a></h3>
 
The terms below are used by \<vertex data\>, \<edge data\> and \<face data\>.  
 
 
**BNF-like Definition**

```
	<location number> = <int>;

	<3D curve number> = <int>;

	<surface number> = <int>;

	<2D curve number> = <int>;

	<3D polygon number> = <int>;

	<triangulation number> = <int>;

	<polygon on triangulation number> = <int>;

	<curve parameter minimal and maximal values> = <real> <_> <real>;

	<curve values for parameter minimal and maximal values> =
	real> <_> <real> <_> <real> <_> <real>;
```
 
**Description**  
 
\<location number\> is the number of \<location record\> from section locations. \<location record\> numbering  starts from 1. \<location number\> 0 is interpreted  as the identity location.  
 
\<3D curve number\> is the number of a \<3D curve record\> from subsection \<3D curves\> of section \<geometry\>. \<3D curve record\> numbering starts from 1.  
 
\<surface number\> is the number of a \<surface record\> from subsection \<surfaces\> of section \<geometry\>. \<surface record\> numbering starts from 1.  
 
\<2D curve number\> is the number of a \<2D curve record\> from subsection \<2D curves\> of section \<geometry\>. \<2D curve record\> numbering starts from 1.  
 
\<3D polygon number\> is the number of a \<3D polygon record\> from subsection \<3D polygons\> of section \<geometry\>.  \<3D polygon record\> numbering starts from 1.  
 
\<triangulation number\> is the number of a \<triangulation record\> from subsection \<triangulations\> of section \<geometry\>. \<triangulation record\> numbering starts from 1.  
 
\<polygon on triangulation\> number is the number of a \<polygons on triangulations record\> from subsection \<polygons on  triangulations\> of section \<geometry\>.  
\<polygons on triangulations record\> numbering  starts from 1.  
 
\<curve parameter minimal and maximal values\> *u<sub>min</sub>* and *u<sub>max</sub>* are the curve parameter *u* bounds: *u<sub>min</sub>* $\leq$ *u* $\leq$ *u<sub>max</sub>*.  
 
\<curve  values for parameter minimal and maximal values\> *u<sub>min</sub>* and *u<sub>max</sub>* are real pairs *x<sub>min</sub> y<sub>min</sub>* and *x<sub>max</sub> y<sub>max</sub>* that (*x<sub>min</sub>*, *y<sub>min</sub>*)= *C* (*u<sub>min</sub>*) and (*x<sub>max</sub>*, *y<sub>max</sub>*)= *C* (*u<sub>max</sub>*) where *C* is a parametric  equation of the curve.  
 
 
<h3><a id="specification__brep_format_7_2">Vertex data</a></h3>
 
**BNF-like Definition**

```
	<vertex data> = <vertex data tolerance> <_\n> <vertex data 3D representation> <_\n> 
	<vertex data representations>;

	<vertex data tolerance> = <real>;

	<vertex data 3D representation> = <3D point>;

	<vertex data representations> = (<vertex data representation> <_\n>)* "0 0";

	<vertex data representation> = <vertex data representation u parameter> <_>
	<vertex data representation data> <_> <location number>;

	<vertex data representation u parameter> = <real>;

	<vertex data representation data> =
	("1" <_> <vertex data representation data 1>) |
	("2" <_> <vertex data representation data 2>) |
	("3" <_> <vertex data representation data 3>);

	<vertex data representation data 1> = <3D curve number>;

	<vertex data representation data 2> = <2D curve number> <_> <surface number>;

	<vertex data representation data 3> =
	<vertex data representation v parameter> <_> <surface number>;

	<vertex data representation v parameter> = <real>;
```
 
**Description**  
 
The usage of \<vertex data representation u  parameter\> *U* is  described below.  
 
\<vertex data representation data 1\> and  parameter *U* describe  the position of the vertex *V* on a 3D curve *C*. Parameter *U* is a parameter of the  vertex *V* on the  curve *C*: *C(u)=V*.  
 
\<vertex data representation data 2\> and  parameter *U* describe  the position of the vertex *V* on a 2D curve *C* which is located on a  surface. Parameter *U* is a parameter of the  vertex *V* on the  curve *C*: *C(u)=V*.  
 
\<vertex data representation data 3\> and  parameter *u* describe  the position of the vertex *V* on a surface *S* through \<vertex data  representation v parameter\> *v*: *S(u,v)=V*.  
 
\<vertex data tolerance\> *t* describes the maximum distance  from the vertex *V* to the set *R* of vertex *V* representations:  
 
$$ \underset{P \in R }{max} |P-V| \leq t . $$  
 
 
<h3><a id="specification__brep_format_7_3">Edge data</a></h3>
 
**BNF-like Definition**

~~~~{.cpp}
	<edge data> = <_> <edge data tolerance> <_> <edge data same parameter flag> <_> edge data same range flag> <_> <edge data degenerated flag> <_\n> <edge data representations>;

	<edge data tolerance> = <real>;

	<edge data same parameter flag> = <flag>;

	<edge data same range flag> = <flag>;

	<edge data degenerated flag> = <flag>;

	<edge data representations> = (<edge data representation> <_\n>)* "0";

	<edge data representation> =
	"1" <_> <edge data representation data 1>
	"2" <_> <edge data representation data 2>
	"3" <_> <edge data representation data 3>
	"4" <_> <edge data representation data 4>
	"5" <_> <edge data representation data 5>
	"6" <_> <edge data representation data 6>
	"7" <_> <edge data representation data 7>;

	<edge data representation data 1> = <3D curve number> <_> <location number> <_>
	<curve parameter minimal and maximal values>;

	<edge data representation data 2> = <2D curve number> <_> <surface number> <_> 
	<location number> <_> <curve parameter minimal and maximal values>
	[<_\n> <curve values for parameter minimal and maximal values>];

	<edge data representation data 3> = (<2D curve number> <_>) ^ 2 <continuity order> <_> <surface number> <_> <location number> <_> <curve parameter minimal and maximal values> <\n> <curve values for parameter minimal and maximal values>];

	<continuity order> = "C0" | "C1" | "C2" | "C3" | "CN" | "G1" | "G2".

	<edge data representation data 4> =
	<continuity order> (<_> <surface number> <_> <location number>) ^ 2;

	<edge data representation data 5> = <3D polygon number> <_> <location number>;

	<edge data representation data 6> =
	<polygon on triangulation number> <_> <triangulation number> <_> <location number>;

	<edge data representation data 7> = (<polygon on triangulation number> <_>) ^ 2 
	<triangulation number> <_> <location number>;
~~~~
 
**Description**  
 
Flags \<edge data same parameter flag\>, \<edge data same range flag\> and \<edge data degenerated flag\> are used in a special  way [1].  
 
\<edge data representation data 1\> describes a 3D curve.  
 
\<edge data representation data 2\> describes a 2D curve on a surface.  
\<curve values for parameter minimal and maximal  values\> are used in version 2 or later.  
 
\<edge data representation data 3\> describes a 2D  curve on a closed surface.  
\<curve values for parameter minimal and maximal  values\> are used in version 2 or later.  
 
\<edge data representation data 5\> describes a 3D polyline.  
 
\<edge data representation data 6\> describes a polyline  on a triangulation.  
 
\<edge data tolerance\> *t* describes the maximum distance from the edge *E* to  the set *R* of  edge *E* representations:  
 
$$ \underset{C \in R}{max}\;\underset{P \in E}{max}\;\underset{Q \in C}{min}|Q-P| \leq t $$  
 
 
<h3><a id="specification__brep_format_7_4">Face data</a></h3>
 
**BNF-like Definition**

~~~~{.cpp}
	<face data> = <face data natural restriction flag> <_> <face data tolerance> <_> <surface number> <_> <location number> <\n> ["2" <_> <triangulation number>];

	<face data natural restriction flag> = <flag>;

	<face data tolerance> = <real>;
~~~~
 
**Description**  
 
\<face data\> describes a surface *S* of face *F* and a triangulation *T* of face *F*. The surface *S* may be empty: \<surface  number\> = 0.  
 
\<face data tolerance\> *t* describes the maximum distance  from the face *F* to  the surface *S*:  
 
$$ \underset{P \in F}{max}\;\underset{Q \in S}{min}|Q-P| \leq t $$ 

Flag \<face data natural restriction flag\> is  used in a special way [1].  


<h2><a id="specification__brep_format_8">Appendix</a></h2>
 
  This chapter contains a *.brep file example.  
 
```
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
```
