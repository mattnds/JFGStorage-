#if 0

//----------------------------------------------------------------------------

const int NumVerticesCube = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

extern vec4 points_cube[NumVerticesCube];
extern vec4 colors[NumVerticesCube];
extern vec3 normals_cube[NumVerticesCube];


void colorcube();


//----------------------------------------------------------------------------

const int segments = 128;
const int NumVerticesCylinder = segments * 6 + segments * 3 * 2;

extern vec4 points_cylinder[NumVerticesCylinder];
extern vec3 normals_cylinder[NumVerticesCylinder];

void colortube();


//----------------------------------------------------------------------------

const int segments2 = 512;
const int NumVerticesSphere = segments2 * 6 * (segments2 - 2) + segments2 * 3 * 2;

extern vec4 points_sphere[NumVerticesSphere];
extern vec3 normal_sphere[NumVerticesSphere];

void colorsphere();

//----------------------------------------------------------------------------

const int tsegments = 512;
const int NumVerticesTorus = tsegments * 6 * (tsegments - 2) + tsegments * 3 * 2;

extern vec4 points_torus[NumVerticesTorus];
extern vec3 normals_torus[NumVerticesTorus];

void colortorus();

//----------------------------------------------------------------------------

const int ssegments = 512;
const int NumVerticesSphere = ssegments * 6 * (ssegments - 2) + ssegments * 3 * 2;

extern vec4 points_sphere[NumVerticesSphere];
extern vec3 normals_sphere[NumVerticesSphere];

void colorbube();

#endif

#if 0

//----------------------------------------------------------------------------

const int NumVerticesCube = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

extern vec4 points_cube[NumVerticesCube];
extern vec4 colors[NumVerticesCube];
extern vec3 normals_cube[NumVerticesCube];

void colorcube();


//----------------------------------------------------------------------------

const int segments = 128;
const int NumVerticesCylinder = segments * 6 + segments * 3 * 2;

extern vec4 points_cylinder[NumVerticesCylinder];
extern vec3 normals_cylinder[NumVerticesCylinder];

void colortube();


//----------------------------------------------------------------------------

const int ssegments = 512;
const int NumVerticesSphere = ssegments * 6 * (ssegments - 2) + ssegments * 3 * 2;

extern vec4 points_sphere[NumVerticesSphere];
extern vec3 normals_sphere[NumVerticesSphere];

void colorbube();


//----------------------------------------------------------------------------

const int tsegments = 512;
const int NumVerticesTorus = tsegments * 6 * (tsegments - 2) + tsegments * 3 * 2;

extern vec4 points_torus[NumVerticesTorus];
extern vec3 normals_torus[NumVerticesTorus];

void colortorus();





#endif

#if 1

//----------------------------------------------------------------------------

const int NumVerticesCube = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

extern vec4 points_cube[NumVerticesCube];
extern vec4 colors[NumVerticesCube];
extern vec3 normals_cube[NumVerticesCube];
extern vec2 tex_coords_cube[NumVerticesCube];

void colorcube();


//----------------------------------------------------------------------------

const int segments = 64;
const int NumVerticesCylinder = segments * 6 + segments * 3 * 2;

extern vec4 points_cylinder[NumVerticesCylinder];
extern vec3 normals_cylinder[NumVerticesCylinder];
extern vec2 tex_coords_cylinder[NumVerticesCylinder];

void colortube();


//----------------------------------------------------------------------------

const int ssegments = 64;
const int NumVerticesSphere = ssegments * 6 * (ssegments - 2) + ssegments * 3 * 2;

extern vec4 points_sphere[NumVerticesSphere];
extern vec3 normals_sphere[NumVerticesSphere];
extern vec2 tex_coords_sphere[NumVerticesSphere];

void colorbube();

#endif