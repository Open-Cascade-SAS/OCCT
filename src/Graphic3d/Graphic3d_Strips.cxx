#include <Graphic3d_Strips.ixx>

/*
				TRIANGLE_STRIP
*/


/*
      Algorithm to find Triangle-Strips in a triangles graph.

  A triangle graph is a set of vertices (numbered from 1 to nvertices)
and of triangles (numbered from 1  to ntriangles), each  triangle is a
triplet of vertices.

  A  triangle-strip  is  a  strip of  adjacent  triangles that can  be
described as a   list of vertices.  The  strip   v1,v2,v3,v4,v5,v6,...
describes the triangles (v1,v2,v3), (v2,v3,v4), (v3,v4,v5), (v4,v5,v6)
...

  Triangle-strips  are an economic  way to pass  around triangles to a
hardware shader as they require an average of one vertex per triangle.

  The purpose  of  this algorithm is to  break a triangle graph into a
(minimal cardinality) set of triangle strips.

It is purely topological, triangles are triplets of long integers.
There is no limit of size, memory is allocated from free store.
The quantity allocated during the algorithm is about
  66 * t bytes where t is the number of triangles.

( the description of the algorithm can be found at the end of the file
  starting with  "COMMENTS")
*/


/*******************************/
/*  GLOBAL TYPES AND VARIABLES */
/*******************************/

#include <stddef.h>
#include <Standard.hxx>

#define NULLTRIANGLE 0
#define DELETED 0

/* the array of triangles is the basic data structure to folow strips
   it is allocated from free store */
typedef struct {
  int  v[3];  /* the three vertices of the triangle */
  int  tn[3]; /* neighbouring triangles */
  int ivn[3]; /* the index of the neigbouring vertex */
  int  state; /* the last strip crossing the triangle, 0 means deleted */
} triangle;

triangle *trianglesptr;
int TrianglesPtrSize;

/* remarks about the above structure :
 Triangles are ranging from 1 two nbtriangles, triangle 0 will always
 be deleted.
 A state of 0 means the triangle is deleted from the graph.
 The vertices are v[0],v[1],v[2]
 To get the neigbour of the triangle on the other side of the edge
 v[i],v[j] just pick tn[i+j-1] and ivn[i+j-1].
 If tn[i+j-1] is 0 there is no neighbour.
 ivn is the index (0,1,2) of the vertex of the neighbouring triangle
 tn[] which is not shared with this triangle.
*/

/* the number of triangles */
int  nbtriangles;

/* a strip is described by a triangle and the index of the two last
  NBVERTICES of the triangle in the strip */
typedef struct {
  int  t;     /* the triangle */
  int iv1,iv2; /* the last NBVERTICES of t in the strip, in 0,1,2 */
} stript;

/* the current strip position is saved in this variable */
/* between calls to to GET_VERTEX */
static stript current_stript;

/* this index is used to label the last strip under exploration */
static int  last_stript;

/* tell this dumb compiler that stript_next returns  nothing */
void stript_next(stript *st);
int  stript_score(stript* pstrip, int *plength);

/********************************************************************/
/*                                                                  */
/* STRIPT_INIT : get data and build the triangles array             */
/*                                                                  */
/********************************************************************/

void Graphic3d_Strips :: STRIPT_INIT ( const Standard_Integer NBVERTICES,
									   const TColStd_Array1OfInteger& TABTRIANGLES )  
{
 /* In  order to build  the triangles array  we  will  use a temporary
array  : edges. This array  is of  length NBVERTICES.  Each entry is a
pointer  to a  list of  structures  of  the  type edge. This structure
describes  an edge by : the  second vertex  of the  edge   and the two
triangles adjacent to the edge, the starting vertex of the edge is the
entry of the array edges. The smallest vertex index of an edge is used
to index it in the edges array */


  int NBTRIANG = (int) TABTRIANGLES.Length() / 3;

  typedef struct edg {
    struct edg *next; /* next edge in the list for a vertex */
    int  v;           /* the second vertex of the edge */
    int  tn[2];       /* neighbour triangles */
    int ivn[2];       /* index of third vertex of neighbour triangles */
  } edge;

  edge **edges;
  edge *cedge;
  int  ivert,triang;
  int  vmin,vmax;
  int ivthird;
  int TedgesSize;

  int i,j;

  /* copy the number and initialize a few */
  nbtriangles = NBTRIANG;
  last_stript = 1;

  /* allocate the array edges 
     vertices are ranging from 1 to NBVERTICES */
  TedgesSize = (NBVERTICES+1) * sizeof(edge*);
  edges = (edge**) Standard::Allocate(TedgesSize);
  for (ivert=0;ivert<= NBVERTICES; ivert++) {
    edges[ivert] = NULL;
  }

  /* allocate the array triangles from 0 to nbtriangles */
  TrianglesPtrSize = (nbtriangles+1)*sizeof(triangle);
  trianglesptr = (triangle*) Standard::Allocate (TrianglesPtrSize);
  trianglesptr[0].state = DELETED;
  trianglesptr[0].tn[0] = NULLTRIANGLE;
  trianglesptr[0].tn[1] = NULLTRIANGLE;
  trianglesptr[0].tn[2] = NULLTRIANGLE;
  trianglesptr[0].ivn[0] = 0;
  trianglesptr[0].ivn[1] = 0;
  trianglesptr[0].ivn[2] = 0;


  /* copy the triangles into the arrays */
  for (triang=1;triang<=nbtriangles;triang++) {

    /* copy the vertices */
    trianglesptr[triang].state = 1;
    for (j=0;j<3;j++) 
	  trianglesptr[triang].v[j] = TABTRIANGLES(3*(triang-1)+j+1);
      
    /* insert the edges in the edges array */
    for (j=0;j<3;j++) {
      if (trianglesptr[triang].v[j] <= trianglesptr[triang].v[(j+1)%3]) 
	  {
		vmin = trianglesptr[triang].v[j];
		vmax = trianglesptr[triang].v[(j+1)%3];
      }
      else 
	  {
		vmax = trianglesptr[triang].v[j];
		vmin = trianglesptr[triang].v[(j+1)%3];
      }
      ivthird = (j+2)%3;

      /* the edge is inserted in the array at the entry for the 
         smallest vertex */

      /* first search if there is an entry for this edge */
      cedge = edges[vmin];
      while(cedge != NULL) {
	if (cedge->v == vmax) 
	  break;
	cedge = cedge->next;
      }
      /* if the edge was not found, create it */
      if (cedge == NULL) {
	cedge = (edge*) Standard::Allocate (sizeof(edge));
	cedge->next = edges[vmin];
	edges[vmin] = cedge;
	cedge->v = vmax;
	cedge->tn[0] = triang;
	cedge->ivn[0] = ivthird;
	cedge->tn[1] = 0;
	cedge->ivn[1] = 0;
      }
      else {
	cedge->tn[1] = triang;
	cedge->ivn[1] = ivthird;
      }
    }
  }

  /* now complete the triangles array (neighbours) using the edges */
  /* array */

  for (triang=1;triang<=nbtriangles;triang++) {
    /* on each edge of the triangle : find the neighbour */
    for (j=0;j<3;j++) {
      if (trianglesptr[triang].v[j] <= trianglesptr[triang].v[(j+1)%3]) {
	vmin = trianglesptr[triang].v[j];
	vmax = trianglesptr[triang].v[(j+1)%3];
      }
      else {
	vmax = trianglesptr[triang].v[j];
	vmin = trianglesptr[triang].v[(j+1)%3];
      }

      /* search the entry for the edge */
      cedge = edges[vmin];
      while(cedge->v != vmax) {
	cedge = cedge->next;
      }

      /* find the neighbouring triangle */
      i = 0;
      if (cedge->tn[0] == triang) i = 1;
      trianglesptr[triang].tn[(2*j)%3] = cedge->tn[i];
      trianglesptr[triang].ivn[(2*j)%3] = cedge->ivn[i];
    }
  }

  /* destroy the edges array which has done it's duty */
  for (ivert = 1; ivert <= NBVERTICES; ivert++) {
    while(edges[ivert] != NULL) {
      cedge = edges[ivert];
      edges[ivert] = cedge->next;
      Standard::Free((void*&)cedge);
    }
  }
  Standard::Free((void*&)edges);
}


/********************************************************************/
/*                                                                  */
/* STRIPT_GET_STRIP : find the next strip                           */
/*                                                                  */
/********************************************************************/

void Graphic3d_Strips :: STRIPT_GET_STRIP ( Standard_Integer& NBTRIANGLES,
										    Standard_Integer& V1,
											Standard_Integer& V2 )
{

  int  btriang;   /* the triangle with the lowest number of neigbours */
  int  triang;
  int  tr;
  int bneib,neib;
  stript cstrip;   /* the current strip */
  int   cscore;    /* it's score */
  int  cleng;      /* it's length */
  /* the best strip is stored in current_strip */
  int  blength; /* the best strip length */
  int  bscore;  /* the best strip score */
  
  int i;

  /* first find the triangle with the lowest number of neighbours */
  btriang = 0;
  bneib = 4;
  for (triang=1; triang<=nbtriangles; triang++) {
    if (trianglesptr[triang].state != 0) {
      neib = 0;
      for (i=0;i<3;i++) {
	tr = trianglesptr[triang].tn[i];
	if ((tr != 0) && (trianglesptr[tr].state != 0)) {
	  neib++;
	}
      }
      if (neib < bneib) {
	bneib = neib;
	btriang = triang;
	/* a triangle with 0 or one neighbours is fine */
	if (neib <= 1) break;
      }
    }
  }

  /* if none was found stop the process and free the memory */
  if (btriang == 0) 
  {
    NBTRIANGLES = 0;
    current_stript.t = 0;
    Standard::Free((void*&)trianglesptr);
    return;
  }

  /* now search the best strip from this triangle 
     the strip with the biggest score.
     If score are even the biggest length win */

  /* try 0,1,2 */
  current_stript.t = btriang;
  current_stript.iv1 = 1;
  current_stript.iv2 = 2;
  bscore = stript_score(&current_stript,&blength);

  /* try 1,2,0 */
  cstrip.t = btriang;
  cstrip.iv1 = 2;
  cstrip.iv2 = 0;
  cscore = stript_score(&cstrip,&cleng);
  if ((cscore > bscore)  ||
      ((cscore == bscore) && (cleng > blength))){
    bscore = cscore;
    blength = cleng;
    current_stript.t = cstrip.t;
    current_stript.iv1 = cstrip.iv1;
    current_stript.iv2 = cstrip.iv2;
  }

  /* try 2,0,1 */
  cstrip.t = btriang;
  cstrip.iv1 = 0;
  cstrip.iv2 = 1;
  cscore = stript_score(&cstrip,&cleng);
  if ((cscore > bscore) ||
      ((cscore == bscore) && (cleng > blength))){
    bscore = cscore;
    blength = cleng;
    current_stript.t = cstrip.t;
    current_stript.iv1 = cstrip.iv1;
    current_stript.iv2 = cstrip.iv2;
  }

  /* return the best strip */
  NBTRIANGLES = blength;
  triang = current_stript.t;
  V2 = trianglesptr[triang].v[current_stript.iv1];
  V1 = trianglesptr[triang].v[3-current_stript.iv1-current_stript.iv2];
  return;
}


/********************************************************************/
/*                                                                  */
/* STRIPT_GET_VERTEX : get next vertex & triangle in current strip   */
/*                                                                  */
/********************************************************************/

void Graphic3d_Strips :: STRIPT_GET_VERTEX ( Standard_Integer& VERTEX,
											 Standard_Integer& TRIANGLE )  
{
  int  triang;
  triang = current_stript.t;
  /* delete this triangle */
  trianglesptr[triang].state = 0;
  TRIANGLE = triang;
  VERTEX = trianglesptr[triang].v[current_stript.iv2];
  stript_next(&current_stript);
  return;
}



/********************************************************************/
/*                                                                  */
/* stript_score  : find the start of a strip and it's lenght         */
/*                returns the score of the strip                    */
/*                                                                  */
/********************************************************************/

int stript_score(stript* pstrip, int *plength)
{
/* st is set to the beginning of the strip and the length of the strip 
   is returned. The strip is explored in two directions, if it loops
   on itself it is detected. */
/* the score is a value to optimise. The number of boundary triangles */
 /* in a strip seems to be a nice choice. */
  stript cstrip,savstrip;
  int  length;
  int  score;
  int i;
  int  triang;

  length = 0;
  score = 0;
  last_stript++; /* this is used to mark triangles in this strip */

  /* go in the first direction */
  cstrip.t = pstrip->t;
  cstrip.iv1 = pstrip->iv1;
  cstrip.iv2 = pstrip->iv2;
  while ((cstrip.t != 0) &&                           /* - on a boundary */
	 (trianglesptr[cstrip.t].state != 0) &&          /* - deleted */
	 (trianglesptr[cstrip.t].state != last_stript)) { /* - on the same */
						      /* strip */
    trianglesptr[cstrip.t].state = last_stript;
    /* increment the length */
    length++;
    /* compute the score */
    /* increment the score if the triangle has less than three */
    /* neigbours */
    for (i=0;i<3;i++)  {
      triang = trianglesptr[cstrip.t].tn[i];
      if ((triang == 0) || (trianglesptr[triang].state == 0)) {
	score++;
	break;
      }
    }
    /* next in the strip */
    stript_next(&cstrip);
  }
  /* go in the reversed direction */
  cstrip.t = pstrip->t;
  cstrip.iv1 = pstrip->iv1;
  cstrip.iv2 = 3 - pstrip->iv2 - pstrip->iv1;
  /* save the position of the strip before moving  */
  savstrip.t = cstrip.t;
  savstrip.iv1 = cstrip.iv1;
  savstrip.iv2 = cstrip.iv2;
  stript_next(&cstrip);
  while ((cstrip.t != 0) && 
	 (trianglesptr[cstrip.t].state != 0) &&
	 (trianglesptr[cstrip.t].state != last_stript)) {
    trianglesptr[cstrip.t].state = last_stript;
    /* save the position of the strip before moving  */
    savstrip.t = cstrip.t;
    savstrip.iv1 = cstrip.iv1;
    savstrip.iv2 = cstrip.iv2;
    /* increment the length */
    length++;
    /* compute the score */
    /* increment the score if the triangle has less than three */
    /* neigbours */
    for (i=0;i<3;i++)  {
      triang = trianglesptr[cstrip.t].tn[i];
      if ((triang == 0) || (trianglesptr[triang].state == 0)) {
	score++;
	break;
      }
    }
    /* next in the strip */
    stript_next(&cstrip);
  }

  /* reverse in the good direction the saved position */
  pstrip->t = savstrip.t;
  pstrip->iv1 = savstrip.iv1;
  pstrip->iv2 = 3 - savstrip.iv1 - savstrip.iv2;
  *plength = length;
  return score;
}

/********************************************************************/
/*                                                                  */
/* stript_next : jump to next triangle in a strip                    */
/*                                                                  */
/********************************************************************/

void stript_next(stript *st) 
{
/* st points toward a triangle and a vertex ordering defining a unique */
/* it's content is changed for the next triangle in the strip */
/* the triangle may becomes 0 if there was no neighbour */

  int  triang,ntriang;
  int i,j;

  triang = st->t;

  if (triang == 0) {
    st->t = 0;
    st->iv1 = 0;
    st->iv2 = 0;
    return;
  }
  /* get the neighbouring triangle */
  i = st->iv1+st->iv2-1;
  ntriang = trianglesptr[triang].tn[i];
  /* if there is no neighbour */
  if (ntriang == 0) {
    st->t = 0;
    st->iv1 = 0;
    st->iv2 = 0;
    return;
  }
  /* compute the new index for the last vertex */
  j = 0;
  while(trianglesptr[triang].v[st->iv2] != trianglesptr[ntriang].v[j]) {
    j++;
  }
  st->t = ntriang;
  st->iv1 = j;
  st->iv2 = trianglesptr[triang].ivn[i];

  return;
}

/*******************************************************************/
/*********						  **********/
/*********		COMMENTS			  **********/
/*********						  **********/
/*******************************************************************/


/* 
  Architecture
  ************

The present C implementation was designed to be called from FORTRAN
with the following syntaxes :

1. STRIP_INIT(int * NBVERTICES,int * NBTRIANGLES,int * TABTRIANGLES)

   This is the initiating call where :
   NBVERTICES is the number of vertices
   NBTRIANGLES is the number of triangles
   TABTRIANGLES is the table describing the triangles

   This  function copies  the arguments to  nvertices, ntriangles, and
build an inner table of triangles (triangles) were the neighbours of a
triangle can be found easily.


IMPORTANT WARNINGS
  Vertices and Triangles are in the range 1...NBVERTICES
  1...NBTRIANGLES
  Double arrays are FORTRAN arrays so the three vertices of triangle I
  are found in TABTRIANGLES[I+J-1] where J = 0,1,2
  The FORTRAN array is declared as TABTRIANGLES(3,NBTRIANGLES)


  2.  STRIP_GET_STRIP(int * NBTRIANGLES,int * V1,int * V2)
      STRIP_GET_VERTEX(int * VERTEX,int * TRIANGLE)

      Both functions are used to get the strips. each iteration of the
     GET_STRIP brings a new strip wre NBTRIANGLES is the number of triangles
     in the strip (i.e the number  of  vertices is NBTRIANGLES plus two) and  V1, V2
     are the  two first vertices.  NBTRIANGLES becomes   zero when the last
     strip has been read.  GET_VERTEX is  used two get the successives
     vertices of a strip, each  vertex is associated with  a triangle.
     This start with  the third vertex, the two   first  are given  by
     GET_STRIP.

     An example of correct call from C to read the strips is

     while(1) {
       STRIP_GET_STRIP(&nb_triangles,&v[1],&v[2]);
       if (nb_triangles == 0) break;
       for (i=3;i <= ( nb_triangles+2 ); i++) {
         STRIP_GET_VERTEX(&v[i],&t[i]);
	 }
       }

       Unwise calls to those functions will generally return zero but
       are not recommanded.

       The data are not checked for coherence, but a zero
       number of triangle will give a zero number of strips.
      
*/

/*
OUTLINE OF THE ALGORITHM
************************

The algorithm is purely topological.  No coordinates  are given,  it's
input are the number  of  vertices, the number  of triangles,  and for
each triangle a triplet of vertices indices.

Let us consider a   triangle T =   (V1,  V2, V3),  this  triangle  has
neighbours in the triangle graphs, let us call them T12, T23, T31. T12
is the triangle sharing the edge V1,V2 with T, etc...  Of course those
three triangles may not exist  in the graph in  this case T is "on the
border" or even "in a corner".

The key remark is that at most three  triangle-strips may cross T they
are : T12-T-T23, T23-T-T31,  T31-T-T12. Once three adjacent  triangles
are given the entire strip  is uniquely  defined, the orientation of a
strip is meaningless as if you reverse it you get the same strip.
To describe a current position in a strip you need a triangle and the
two last vertices of the triangle in the strip.

Our algorithm (more precisely heuristic) is the following : 
- Find a triangle with the lowest number of neighbours.
- List the three (or less) strips crossing this triangle.
- Chose the best among them and remove from the graph all the
triangles in this strip.
  The best strip is rated with a "score", the score we used is the
  number of triangles in the strip which have less than three
  neighbours  (they are on the "border") in case of score equality the
  longest strip is selected.
- Reiterate the process until there are no triangles left.

There are no demonstrations of  the optimality  of this algorithm, but
it seems to give expected results on regular graphs which are the most
commonly fed to it. On a rectangular array of squares, each square cut
in  two triangles, it  will generate strips parallels  to  the longest
side of the rectangle.

*/

/*
Implementation
**************

First the STRIP_INIT function stores the triangles in a data structure
(the triangles array   allocated on free  store),  containing for each
triangle it's three neighbours and the third vertex for each neighbour
(a zero neighbour is inexistant), a triangle get's also a state 1.  To
build this  array a temporary  array "edges" is build giving  for each
edge (pair of vertices) the two neghbouring triangles.

Then at  each  call    of STRIP_GET_STRIP  a  triangle    with minimum
neighbours is first chosen.  For   the three possible strips  crossing
the triangle the strip_score function is called  which brings back the
start of the strip, the length and the score.  The strip-next function
is used to jump to  the next triangle  in a strip.   The best strip is
chosen, stored in the current_strip  and returned. 


Each   call  to   STRIP_GET_VERTEX  increment  the   the current-strip
structure  to the next triangle in   the  strip,  using the strip_next
function. The triangle is deleted from the graph and returned.

The last call  to STRIP_GET_STRIP returns  the  triangles array to the
free store.


*/


/*
				QUADRANGLE_STRIP
*/

/*
      Algorithm to find Quadragle-Strips in a quadrangles graph.

  A quadrangle graph is a set of vertices (numbered from 1 to nbvertices)
and of quadrangles (numbered from 1  to nbquadrangles), each  quadrangle is a
quadruplet of vertices.

  A quadrangle-strip is a strip of adjacent quadrangles that can be
described as a list of vertices.
  The  strip v1, v2, v3, v4, v5, v6, v7, v8 ...
describes quadrangles (v1, v2, v4, v3), (v4, v3, v5, v6), (v5, v6, v8, v7) ...
	1-3-5-7
	| | | |
	2-4-6-8

  Quadrangle-strips  are an economic  way to pass quadrangles to a
hardware renderer as they require an average of two vertex per quadrangle.

  The purpose  of  this algorithm is to  break a quadrangle graph into a
(minimal cardinality) set of quadrangle strips.

It is purely topological, quadrangles are quadruplets of integers.
There is no limit of size, memory is allocated from free store.
The quantity allocated during the algorithm is about
  (17*sizeof(int)+align)*q bytes where q is the number of quadrangles
and align is system-dependent alignment.

( the description of the algorithm can be found at the end of the file
  starting with  "COMMENTS")
*/


/*******************************/
/*  GLOBAL TYPES AND VARIABLES */
/*******************************/

#define NULLQUADRANGLE 0

/* the array of quadrangles is the basic data structure to follow strips
   it is allocated from free store */
typedef struct {
  int  v[4];  /* the four vertices of the quadrangle */
  int  qn[4]; /* neighbouring quadrangles */
  int ivn[4][2]; /* the index of two neighbouring vertice [q][v]*/
  int  state; /* the last strip crossing the quadrangle, 0 means deleted */
} quadrangle;

quadrangle *quadranglesptr;
int QuadranglesPtrSize;

/* the number of quadrangles */
int  nbquadrangles;

/* remarks about the above structure :
 Quadrangles are ranging from 1 two nbquadrangles, quadrangle 0 will always
 be deleted.
 A state of 0 means the quadrangle is deleted from the graph.
 The NBVERTICES are v[0], v[1], v[2], v[3].
 To get the neigbour of the quadrangle on the other side of the edge
 v[i], v[j] just pick qn[i+j-1] and ivn[i+j-1][0], ivn[i+j-1][1].
 If qn[i+j-1] is 0 there is no neighbour.
 ivn is the index (0, 1, 2, 3)(0, 1) of two vertice of the neighbouring
 quadrangle qn[] which are not shared with this quadrangle.
*/

/* a strip is described by a quadrangle and the index of the two last
  NBVERTICES of the quadrangle in the strip */
typedef struct {
  int  q;     /* the quadrangle */
  int iv2, iv3; /* the last NBVERTICES of q in the strip, in (0, 1, 2, 3) */
} stripq;

/* the current strip position is saved in this variable */
/* between calls to to STRIPQ_GET_NEXT */
 static stripq current_stripq;

/* this index is used to label the last strip under exploration */
static int  last_stripq;

/* tell this dumb compiler that stripq_next returns  nothing */
void stripq_next(stripq *st);
int stripq_score(stripq *pstrip, int *plength);

/********************************************************************/
/*                                                                  */
/* STRIPQ_INIT : get data and build the quadrangles array           */
/*                                                                  */
/********************************************************************/

void Graphic3d_Strips :: STRIPQ_INIT ( const Standard_Integer NBNBVERTICES,
									   const Standard_Integer NBQUADRANG,
									   const TColStd_SequenceOfInteger& TABQUADRANGLES )  
{

 /* In  order to build  the quadrangles array  we  will  use a temporary
array: edges. This array  is of  length NBNBVERTICES.  Each entry is a
pointer  to a  list of  structures  of  the  type edge. This structure
describes  an edge by: the  second vertex  of the  edge   and the two
quadrangles adjacent to the edge, the starting vertex of the edge is the
entry of the array edges. The smallest vertex index of an edge is used
to index it in the edges array */

  typedef struct edg {
    struct edg *next; /* next edge in the list for a vertex */
    int  v;           /* the second vertex of the edge */
    int  qn[2];       /* neighbour quadrangles */
    int ivn[2][2];    /* index of two vertice of neighbour quadrangles [q][v]*/
  } edge;

  edge **edges;
  edge *cedge;
  int  ivert, quadrang;
  int  vmin, vmax;
  int  iv3, iv4;
  int QedgesSize;

  int i, j;

  /* copy the number and initialize a few */
  nbquadrangles = NBQUADRANG;
  last_stripq = 1;

  /* allocate the array edges 
     NBVERTICES are ranging from 1 to NBNBVERTICES */
  QedgesSize = (NBNBVERTICES+1) * sizeof(edge*);
  edges = (edge**) Standard::Allocate (QedgesSize);
  for (ivert=0; ivert<= NBNBVERTICES; ivert++) {
    edges[ivert] = NULL;
  }

  /* allocate the array quadrangles from 0 to nbquadrangles */
  QuadranglesPtrSize = (nbquadrangles+1)*sizeof(quadrangle);
  quadranglesptr = (quadrangle*) Standard::Allocate (QuadranglesPtrSize);
  quadranglesptr[0].v[0] = 0;
  quadranglesptr[0].v[1] = 0;
  quadranglesptr[0].v[2] = 0;
  quadranglesptr[0].v[3] = 0;
  quadranglesptr[0].qn[0] = NULLQUADRANGLE;
  quadranglesptr[0].qn[1] = NULLQUADRANGLE;
  quadranglesptr[0].qn[2] = NULLQUADRANGLE;
  quadranglesptr[0].qn[3] = NULLQUADRANGLE;
  quadranglesptr[0].ivn[0][0] = 0;
  quadranglesptr[0].ivn[0][1] = 0;
  quadranglesptr[0].ivn[1][0] = 0;
  quadranglesptr[0].ivn[1][1] = 0;
  quadranglesptr[0].ivn[2][0] = 0;
  quadranglesptr[0].ivn[2][1] = 0;
  quadranglesptr[0].ivn[3][0] = 0;
  quadranglesptr[0].ivn[3][1] = 0;
  quadranglesptr[0].state = DELETED;

  /* copy the quadrangles into the arrays */
  for (quadrang=1; quadrang<=nbquadrangles; quadrang++)
  {
    /* copy the NBVERTICES */
    quadranglesptr[quadrang].state = 1;
    for (j=0; j<4; j++)
      quadranglesptr[quadrang].v[j] = TABQUADRANGLES(4*(quadrang-1)+j+1);
    /* insert the edges in the edges array */
    for (j=0; j<4; j++)
    {
      if (quadranglesptr[quadrang].v[j] <= quadranglesptr[quadrang].v[(j+1)%4])
      {
	vmin = quadranglesptr[quadrang].v[j];
	vmax = quadranglesptr[quadrang].v[(j+1)%4];
      }
      else
      {
	vmax = quadranglesptr[quadrang].v[j];
	vmin = quadranglesptr[quadrang].v[(j+1)%4];
      }
      iv3 = (j+2)%4;
      iv4 = (j+3)%4;
      /* the edge is inserted in the array at the entry for the 
         smallest vertex */
      /* first search if there is an entry for this edge */
      cedge = edges[vmin];
      while(cedge != NULL)
      {
	if (cedge->v == vmax) 
	  break;
	cedge = cedge->next;
      }
      /* if the edge was not found, create it */
      if (cedge == NULL) {
	cedge = (edge*) Standard::Allocate (sizeof(edge));
	cedge->next = edges[vmin];
	edges[vmin] = cedge;
	cedge->v = vmax;
	cedge->qn[0] = quadrang;
	cedge->ivn[0][0] = iv3;
	cedge->ivn[0][1] = iv4;
	cedge->qn[1] = 0;
	cedge->ivn[1][0] = 0;
	cedge->ivn[1][1] = 0;
      }
      else
      {
	cedge->qn[1] = quadrang;
	cedge->ivn[1][0] = iv3;
	cedge->ivn[1][1] = iv4;
      }
    }
  }
  /* now complete the quadrangles array (neighbours) using the edges array */
  for (quadrang=1; quadrang<=nbquadrangles; quadrang++)
  {
    /* on each edge of the quadrangle: find the neighbour */
    for (j=0; j<4; j++)
    {
      if (quadranglesptr[quadrang].v[j] <= quadranglesptr[quadrang].v[(j+1)%4])
      {
	vmin = quadranglesptr[quadrang].v[j];
	vmax = quadranglesptr[quadrang].v[(j+1)%4];
      }
      else
      {
	vmax = quadranglesptr[quadrang].v[j];
	vmin = quadranglesptr[quadrang].v[(j+1)%4];
      }
      /* search the entry for the edge */
      cedge = edges[vmin];
      while(cedge->v != vmax)
	cedge = cedge->next;
      /* find the neighbouring quadrangle */
      i = 0;
      if (cedge->qn[0] == quadrang)
	i = 1;
      quadranglesptr[quadrang].qn[j] = cedge->qn[i];
      quadranglesptr[quadrang].ivn[j][0] = cedge->ivn[i][0];
      quadranglesptr[quadrang].ivn[j][1] = cedge->ivn[i][1];
    }
  }
  /* destroy the edges array which has done it's duty */
  for (ivert = 1; ivert <= NBNBVERTICES; ivert++)
  {
    while(edges[ivert] != NULL)
    {
      cedge = edges[ivert];
      edges[ivert] = cedge->next;
      Standard::Free((void*&)cedge);
    }
  }
  Standard::Free((void*&)edges);
}


/********************************************************************/
/*                                                                  */
/* STRIPQ_GET_STRIP : find the next strip                           */
/*                                                                  */
/********************************************************************/

void Graphic3d_Strips :: STRIPQ_GET_STRIP ( Standard_Integer& NBQUAD,Standard_Integer& V1,
										    Standard_Integer& V2 )
{
  int  bquadrang; /* the quadrangle with the lowest number of neigbours */
  int  quadrang;
  int  quad;
  int  bneib, neib;
  stripq cstrip;   /* the current strip */
  int  cscore;    /* it's score */
  int  cleng;     /* it's length */
  /* the best strip is stored in current_strip */
  int  blength;   /* the best strip length */
  int  bscore;    /* the best strip score */
  int  i;

  /* first find the quadrangle with the lowest number of neighbours */
  bquadrang = 0;
  bneib = 5;
  for (quadrang=1; quadrang<=nbquadrangles; quadrang++)
  {
    if (quadranglesptr[quadrang].state != 0)
    {
      neib = 0;
      for (i=0; i<4; i++)
      {
	quad = quadranglesptr[quadrang].qn[i];
	if ((quad != 0) && (quadranglesptr[quad].state != 0))
	  neib++;
      }
      if (neib < bneib)
      {
	bneib = neib;
	bquadrang = quadrang;
	/* a quadrangle with 0 or one neighbours is fine */
	if (neib <= 1)
	  break;
      }
    }
  }
  /* if none was found stop the process and free the memory */
  if (bquadrang == 0)
  {
    NBQUAD = 0;
    current_stripq.q = 0;
    Standard::Free((void*&)quadranglesptr);
    return;
  }
  /* Now search the best strip from this quadrangle 
     the strip with the biggest score.
     If score were even the biggest length win. */
  /* try 0, 1, 2, 3 */
  current_stripq.q = bquadrang;
  current_stripq.iv2 = 2;
  current_stripq.iv3 = 3;
  bscore = stripq_score(&current_stripq, &blength);
  /* try 1, 2, 3, 0 */
  cstrip.q = bquadrang;
  cstrip.iv2 = 3;
  cstrip.iv3 = 0;
  cscore = stripq_score(&cstrip, &cleng);
  if ((cscore > bscore)  ||
      ((cscore == bscore) && (cleng > blength)))
  {
    bscore = cscore;
    blength = cleng;
    current_stripq.q = cstrip.q;
    current_stripq.iv2 = cstrip.iv2;
    current_stripq.iv3 = cstrip.iv3;
  }
  /* return the best strip */
  NBQUAD = blength;
  quadrang = current_stripq.q;

  V1 = quadranglesptr[quadrang].v[(current_stripq.iv2+2)%4];
  V2 = quadranglesptr[quadrang].v[(current_stripq.iv3+2)%4];
  return;
}

/********************************************************************/
/*                                                                  */
/* STRIPQ_GET_NEXT : get next vertex & quadrangle in current strip  */
/*                                                                  */
/********************************************************************/
void Graphic3d_Strips :: STRIPQ_GET_NEXT ( Standard_Integer& VERTEX1,
										   Standard_Integer& VERTEX2,
										   Standard_Integer& QUADRANGLE )  
{
  int	quadrang = current_stripq.q;
  /* delete this quadrangle */
  quadranglesptr[quadrang].state = 0;
  QUADRANGLE = quadrang;
  /* reversed */
  VERTEX2 = quadranglesptr[quadrang].v[current_stripq.iv2];
  VERTEX1 = quadranglesptr[quadrang].v[current_stripq.iv3];
  stripq_next(&current_stripq);
  return;
}

/********************************************************************/
/*                                                                  */
/* stripq_score  : find the start of a strip and it's length         */
/*                returns the score of the strip                    */
/*                                                                  */
/********************************************************************/
int stripq_score(stripq *pstrip, int *plength)
{
/* st is set to the beginning of the strip and the length of the strip 
   is returned. The strip is explored in two directions, if it loops
   on itself it is detected. */
/* The score is a value to optimise. The number of boundary quadrangles */
/* in a strip seems to be a nice choice. */
  stripq	cstrip, savstrip;
  int	length;
  int	score;
  int	i;
  int	quadrang;

  length = 0;
  score = 0;
  last_stripq++; /* this is used to mark quadrangles in this strip */

  /* go forwards till possible... */
  cstrip.q = pstrip->q;
  cstrip.iv2 = pstrip->iv2;
  cstrip.iv3 = pstrip->iv3;
  while ((cstrip.q != 0) &&                             /* on a boundary */
	 (quadranglesptr[cstrip.q].state != 0) &&       /* deleted */
	 (quadranglesptr[cstrip.q].state != last_stripq))/* on the same strip */
  {
    quadranglesptr[cstrip.q].state = last_stripq;
    /* increment the length */
    length++;
    /* compute the score */
    /* increment the score if the quadrangle has less than four neighbours */
    for (i=0; i<4; i++)
    {
      quadrang = quadranglesptr[cstrip.q].qn[i];
      if ((quadrang == 0) || (quadranglesptr[quadrang].state == 0))
      {
	score++;
	break;
      }
    }
    /* next in the strip */
    stripq_next(&cstrip);
  }
  /* turn back... */
  cstrip.q = pstrip->q;
  cstrip.iv2 = (pstrip->iv2+2)%4;
  cstrip.iv3 = (pstrip->iv3+2)%4;
  /* ... but save the position of the strip before moving */
  savstrip.q = cstrip.q;
  savstrip.iv2 = cstrip.iv2;
  savstrip.iv3 = cstrip.iv3;
  stripq_next(&cstrip);
  while ((cstrip.q != 0) && 
	 (quadranglesptr[cstrip.q].state != 0) &&
	 (quadranglesptr[cstrip.q].state != last_stripq))
  {
    quadranglesptr[cstrip.q].state = last_stripq;
    /* save the position of the strip each time before moving */
    savstrip.q = cstrip.q;
    savstrip.iv2 = cstrip.iv2;
    savstrip.iv3 = cstrip.iv3;
    /* increment the length */
    length++;
    /* compute the score */
    /* increment the score if the quadrangle has less than four neighbours */
    for (i=0; i<4; i++)
    {
      quadrang = quadranglesptr[cstrip.q].qn[i];
      if ((quadrang == 0) || (quadranglesptr[quadrang].state == 0))
      {
	score++;
	break;
      }
    }
    /* next in the strip */
    stripq_next(&cstrip);
  }
  /* ... back end reached. Now turn forward again at recent saved position. */
  pstrip->q = savstrip.q;
  pstrip->iv2 = (savstrip.iv2+2)%4;
  pstrip->iv3 = (savstrip.iv3+2)%4;
  *plength = length;
  return score;
}

/********************************************************************/
/*                                                                  */
/* stripq_next : jump to next quadrangle in a strip                 */
/*                                                                  */
/********************************************************************/

void stripq_next(stripq *st) 
{ /* st points toward a quadrangle and a vertex ordering defining a unique. */
  /* Its content is changed for the next quadrangle in the strip. */
  /* The quadrangle may become 0 if there was no neighbour. */
  int	quadrang=st->q;	/* current */
  int	i=st->iv2;
  int	nquadrang=quadranglesptr[quadrang].qn[i];	/* neighbour */

  if (!quadrang || !nquadrang)
  { /* There is no neighbour on this edge. */
    st->q = 0;
    st->iv2 = 0;
    st->iv3 = 0;
  }
  else
  { /* Compute the new index for the last vertex. */
    st->q = nquadrang;
    st->iv2 = quadranglesptr[quadrang].ivn[i][0];
    st->iv3 = quadranglesptr[quadrang].ivn[i][1];
  }
}

/*******************************************************************/
/*********						  **********/
/*********		COMMENTS			  **********/
/*********						  **********/
/*******************************************************************/


/* 
  Architecture
  ************

The present C implementation was designed to be called from FORTRAN
with the following syntaxes:

1. STRIPQ_INIT(int *NBNBVERTICES, int *NBQUADRANGLES, int *TABQUADRANGLES)

   This is the initiating call where:
   NBNBVERTICES is the number of NBVERTICES
   NBQUADRNGLES is the number of quadrangles
   TABQUADRANGLES is the table describing quadrangles

   This  function copies  its arguments to  nbNBVERTICES, nquadrangles, and
build an inner table of quadrangles, where neighbours of a
quadrangle can be found easily.


IMPORTANT WARNINGS
  NBVERTICES and Quadrangles are in the range 1...NBNBVERTICES and 1...NBQUADRANGLES
  Double arrays are FORTRAN arrays so the three NBVERTICES of quadrangle I
  are found in TABQUADRANGLES[I+J-1] where J = 0, 1, 2, 3.
  The FORTRAN array is declared as TABQUADRANGLES(4, NBQUADRANGLES)


  2.  STRIPQ_GET_STRIP(int *NBQUAD, int *V1, int *V2)
      STRIPQ_GET_NEXT(int *VERTEX1, int *VERTEX2, int *QUADRANGLE)
???
      Both functions are used to get the strips. Each iteration of the
     GET_STRIP brings a new strip where NBQUAD is the number of quadrangles
     in the strip (NB: number of NBVERTICES would be NBQUADS*2+2) and  V1, V2
     are the  two first NBVERTICES.  NBQUADS becomes   zero when the last
     strip has been read.  STRIPQ_GET_NEXT is  used to get the successive
     NBVERTICES of a strip, each two vertice are associated with next quadrangle.
     This start with  the 3d and 4th vertice, the two first are given  by
     STRIPQ_GET_STRIP.

     An example of correct call from C to read the strips is

     while(1)
       {
	 STRIPQ_GET_STRIP(&nbquad, &v[0], &v[1]);
       if (nbquad == 0)
	 break;
       for (i=1; i <= nbquad; i++)
       {
         STRIPQ_GET_NEXT(&v[i*2], &v[i*2+1], &q[i]);
       }
     }

       Unwise calls to those functions will generally return zero but
       are not recommanded.

       The data are not checked for coherence, but a zero
       number of quadrangle will give a zero number of strips.
      
*/

/*
OUTLINE OF THE ALGORITHM
************************

The algorithm is purely topological.  No coordinates  are given,  its
input are the number  of  NBVERTICES, the number of quadangles, and for
each quadrangle a quadruplet of NBVERTICES indices.

Let us consider a   quadrangle Q=(V1, V2, V3, V4), this quadrangle has
neighbours in the quadrangle graphs, let us call them Q12, Q23, Q34 and Q41.
Q12 is the quadrangle sharing the edge [V1, V2] with T, etc... Of course those
fouree quadrangles may not exist in the graph, in this case T is "on the
border" or even "in a corner".

The key remark is that at most two quadrangle-strips may cross Q, they
are: Q12-Q-Q34, Q23-Q-Q41. Once four adjacent quadrangles
are given the entire strip is uniquely defined. The orientation of a
strip is meaningless as if you reverse it you would get the same strip.
To describe a current position in a strip you need a quadrangle and the
two last NBVERTICES of the quadrangle in the strip.

Our algorithm (more precisely heuristic) is the following:
- Find a quadrangle with the lowest number of neighbours.
- List the four (or less) strips crossing this quadrangle.
- Chose the best among them and remove from the graph all the
quadrangles in this strip.
  The best strip is rated with a "score", the score we used is the
  number of quadrangles in the strip which have less than four
  neighbours  (they are on the "border") in case of score equality the
  longest strip is selected.
- Reiterate the process until there are no quadrangles left.

There are no demonstrations of  the optimality  of this algorithm, but
it seems to give expected results on regular graphs which are the most
commonly fed to it.
*/

/*
Implementation
**************

First the STRIPQ_INIT function stores the quadrangles in a data structure
(the quadrangles array allocated on free  store),  containing for each
quadrangle its four neighbours, then third and fourth vertice for each neighbour
(a zero neighbour is inexistant), a quadrangle gets also a state 1.  To
build this  array a temporary  array "edges" is build giving  for each
edge (pair of NBVERTICES) the two neghbouring quadrangles.

Then at  each  call    of STRIPQ_GET_STRIP  a  quadrangle  with minimum
neighbours is first chosen.  For   the four possible strips  crossing
the quadrangle the strip_score function is called  which brings back the
start of the strip, the length and the score.  The strip-next function
is used to jump to  the next quadrangle  in a strip.   The best strip is
chosen, stored in the current_strip  and returned. 


Each   call  to   STRIPQ_GET_NEXT  increments the current-strip
structure  to the next quadrangle in   the  strip,  using the strip_next
function. The quadrangle is deleted from the graph and returned.

The last call  to STRIPQ_GET_STRIP frees  the  quadrangles array to the
free store.


*/
