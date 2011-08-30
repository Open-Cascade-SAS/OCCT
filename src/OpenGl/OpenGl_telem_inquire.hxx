#ifndef OPENGL_TELEM_INQUIRE_H
#define OPENGL_TELEM_INQUIRE_H

typedef struct {
  Tint        number;        /* number of Tints in list */
  Tint        *integers;        /* list of integers */
} Tintlst;

typedef struct {
  Tint        x_dim;        /* dimension (number of divisions) along X */
  Tint        y_dim;        /* dimension (number of divisions) along Y */
} Tdim;

typedef union
{
  Tint       idata;        /* integer valued data */
  Tfloat     fdata;        /* float valued data */
  Tchar      *data;        /* %%TEMP - use for application data for now */
  TEL_POINT  *pts3;        /* list of 3d points */

  struct 
  {
    TEL_POINT   ref_pt;  /* reference text point */
    TEL_POINT   anno;    /* annotation pt/offset */
    Techar      *string; /* text string */
  } atext3;

  Tint            size;
  Tint            interior_style; /* interior style */
  Tint            edge_flag;      /* edge flag */
  Tintlst         name_set;       /* name sets */
  Tmatrix3        mat3;           /* 3d transformation matrix */

  struct 
  {
    Tmatrix3     mat3;         /* 3d transformation matrix */
    TComposeType compose_type; /* composition type */
  } local_xform_3;

  struct 
  {
    Tint        vrtflag;        /* per vertex data flag */
    Tint        num_bounds;     /* number of lines in the set */
    Tint        *bounds;        /* list of number of points in each bound */
    TEL_POINT   *points;        /* array of points */
    TEL_COLOUR  *colours;       /* array of vertex colour values */
  } lineset3data;

  struct 
  {
    Tint        shpflag;           /* primitive shape flag */
    Tint        faflag;            /* per fill area data flag */
    Tint        vrtflag;           /* per vertex data flag */
    TEL_POINT   *gnormal;          /* geometric normal */
    Tint        num_points;        /* number of vertices in fill area */
    tel_colour  facet_colour_val;  /* facet colour value */
    TEL_POINT   *points;           /* array of points */
    TEL_COLOUR  *colours;          /* array of vertex colour values */
    TEL_POINT   *vnormals;         /* array of vertex normals */
  } fillarea3data;

  struct 
  {
    Tint        shpflag;           /* primitive shape flag */
    Tint        faflag;            /* per fill area data flag */
    Tint        vrtflag;           /* per vertex data flag */
    Tint        edgflag;         /* edge flag */
    TEL_POINT   *gnormal;          /* geometric normal */
    Tint        *edgvis;          /* edge visibility per edge */
    Tint        num_bounds;        /* number of bounds in fill area set */
    tel_colour  facet_colour_val;  /* facet colour value */
    Tint        *bounds;           /* bounds */
    TEL_POINT   *points;           /* array of points */
    TEL_COLOUR  *colours;          /* array of vertex colour values */
    TEL_POINT   *vnormals;         /* array of vertex normals */
  } fillareaset3data;

  struct 
  {
    Tint        fctflag;            /* per facet data flag */
    Tint        vrtflag;            /* per vertex data flag */
    Tint        num_facets;         /* number of facets, (num_points-2) */
    TEL_POINT   *gnormals;          /* geometric normals */
    TEL_COLOUR  *facet_colour_vals; /* facet colour value */
    TEL_POINT   *points;            /* array of points for all bounds */
    TEL_COLOUR  *colours;           /* array of vertex colour values */
    TEL_POINT   *vnormals;      /* array of vertex normals for all bounds */
  } trianglestrip3data;

  struct 
  {
    Tint        shpflag;            /* primitive shape flag */
    Tint        fctflag;            /* per facet data flag */
    Tint        vrtflag;            /* per vertex data flag */
    Tdim        dim;                /* number of cols by rows */
    TEL_POINT   *gnormals;          /* geometric normals */
    TEL_COLOUR  *facet_colour_vals; /* facet colour value */
    TEL_POINT   *points;            /* array of points for all bounds */
    TEL_COLOUR  *colours;           /* array of vertex colour values */
    TEL_POINT   *vnormals;      /* array of vertex normals for all bounds */
  } quadmesh3data;

  struct 
  {
    Tint       shpflag;   /* primitive shape flag */
    Tint       fctflag;   /* per facet data flag */
    Tint       vrtflag;   /* per vertex data flag */
    Tint       edgflag;   /* per edge data flag */
    TEL_POINT  *gnormals; /* geometric normals */
    Tint       *edgvis;   /* array of edge visibility flags for all edges */
    Tint       num_vertices;/*number of fill areas in the set */
    TEL_COLOUR *facet_colour_vals; /* facet colour values */
    TEL_POINT  *points;            /* array of vertices */
    TEL_COLOUR *colours;           /* array of vertex colour values */
    TEL_POINT  *vnormals;     /* array of vertex normals for all vertices */
    Tint       num_bounds;    /* number of bounds */
    Tint       *bounds;       /* list of number of points in each bound */
    Tint       *indices;      /* list of vertex indices for all bounds */
  } indexedpolygons3data;

  /*  depth cue index             */            /* -- idata              -- */
  /*  surface properties          */
  /*  back surface properties     */
  TEL_SURF_PROP  surf_prop;         /* Front or back surface properties */
  /*  interior shading method     */            /* -- idata              -- */
  /*  interior lighting method    */            /* -- idata              -- */

  struct 
  {
    Tintlst        on;        /* list of activated lights */
    Tintlst        off;       /* list of deactivated lights */
  } light_source_state;

  struct {
    Tint        distinguish;   /* distinguish */
    TelCullMode cull;          /* culling mode */
  } face_processing_mode;

  /*  polyline, polymarker, text, interior, back interior, edge colour */
  TEL_COLOUR gnl_colour;
} Teldata;

typedef struct
{
  Tint    size;
  Tint    act_size;
  Tchar   *buf;
  Teldata *data;
} TEL_INQ_CONTENT, *tel_inq_content;

/* keys to be used for inquiry */
#define INQ_GET_SIZE_ID    1  /* key.data.ldata will contain size */
#define INQ_GET_CONTENT_ID 2  /* key.data.pdata is pointer to Teldata */

/* element,   size */
extern TStatus TelInqCurElemTypeSize( TelType *, Tint * );

/* size of buffer, buffer,  actual_size, data */
extern TStatus TelInqCurElemContent( Tint, Tchar *, Tint *,      Teldata * );

#endif
