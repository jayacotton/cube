/* 2 d geometry types */

typedef float double_t;

typedef struct Point2Struct
{
  double_t x, y;
} Point2 ;

typedef Point2 Vector2;

typedef struct IntPoint2Struct
{
  int x, y;
} IntPoint2;

typedef struct Matrix2Struct
{
#ifdef GCC
  double_t element[3][3];
#else
  double_t element[9];
#endif
} Matrix3;

typedef struct Box2dStruct
{
  Point2 min, max;
} Box2;

/* 3d geomentry types */

typedef struct Point3Struct
{
  double_t x, y, z;
} Point3;

typedef Point3 Vector3;

typedef struct IntPoint3Struct
{
  int x, y, z;
} IntPoint3;

typedef struct Matrix4Struct
{
#ifdef GCC
  double_t element[4][4];
#else
  double_t element[16];
#endif
} Matrix4;

typedef struct Box3dStruct
{
  Point3 min, max;
} Box3;


/* macros */

#define ABS(a) (((a)<0)?-(a):(a))
#define SGN(a) (((a)<0)?-1:0)
#define SQR(a) ((a)*(a))
#define ROUND(a) (((a)<0?(int)(a+.5):-(int)(.5-a))
