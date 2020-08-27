#include "../include/interpolation.h"
#define CMDOPTS 50
#define DIMS 2

/**
 * Entry point and wrapper function to QHull and linear interpolation
 * @param points     array of 2D points
 * @param values     array of values for each 2D points e.g. f(x,y) = values[0]
 * @param fill_value fill value for points outisde convex hull
 * @param num_pts    number of 2D points (values & points)
 */
void griddata(double* points, double* values, double fill_value, int num_pts)
{
   _qhull(points, values, fill_value, num_pts);
}

/**
 * QHull functionality: (1) convex hull, (2) delaunay triangluation 
 *                      (3) barycentric coordinate interpolation
 * 
 * @param points     array of 2D points
 * @param values     array of values for each 2D points e.g. f(x,y) = values[0]
 * @param fill_value fill value for points outisde convex hull
 * @param num_pts    number of 2D points (values & points)
 */
void _qhull(double* points, double* values, double fill_value, int num_pts)
{
   // new instance for qhull
   qhT qh_qh;
   qhT *qh= &qh_qh;
   
   // struture checks
   QHULL_LIB_CHECK
   
   // True if qh_freeqhull should 'free(array)'
   boolT ismalloc = False; 
   
   // initiate qhull - 
   // I think this is legacy call to arm variables that are generally used 
   // for command line 
   qh_init_A(qh, stdin, stdout, stderr, 0, NULL);
   int exitcode = setjmp(qh->errexit);
   
   // not sure what this means yet
   qh->NOerrexit = False;

   // commandline option for:
   //   d - Delaunay triangulation by lifting points to a paraboloid
   char noptions[CMDOPTS] = "QVn d";
   qh_initflags(qh, noptions);
   
   // true for delaunay and will aloow read-in points
   qh->PROJECTdelaunay = True;

   coordT* allpts = points;
   
   // second initialization with points
   qh_init_B(qh, allpts, num_pts, DIMS, ismalloc);
   
   // convex hull
   qh_qhull(qh);
   
   // triangluate hull points
   qh_triangulate(qh);

   // interpolate with known point values
   _linear_interp2d_facet(qh, DIMS, values);
     
   _print_all_facets(qh);
   
   free(allpts);
   
}

/**
 * Linear interpolation using barycentric coordinates
 *
 * @param qh      qhull reference
 * @param dims    number of dimensions (current implementaton is 2 only!)
 * @param f_val   array of values for each 2D points
 */
void _linear_interp2d_facet(qhT *qh, int dims, double* f_val)
{
   vertexT *vertex;
   coordT single_point[2];
   realT bestdist;
   boolT isoutside;
   
   // find triangle for this point
   single_point[0] = 3.0;
   single_point[1] = 0.0;
   
   // bumps-up dimension to calculate projections
   qh_setdelaunay(qh, dims+1, 1, single_point);
   
   // find facet that contains point
   facetT *facet = qh_findbestfacet(qh, single_point, True, &bestdist, &isoutside);

   double x = single_point[0];
   double y = single_point[1];

   vertex = facet->vertices->e[0].p;
   double x1 = vertex->point[0];
   double y1 = vertex->point[1];

   vertex = facet->vertices->e[1].p;
   double x2 = vertex->point[0];
   double y2 = vertex->point[1];

   vertex = facet->vertices->e[2].p;
   double x3 = vertex->point[0];
   double y3 = vertex->point[1];

   printf(" ><><><><> %lf, %lf, %lf \n", x1, x2, x3);
   printf(" <><><><>< %lf, %lf, %lf \n", y1, y2, y3);

   double dnmr = y2*x1 - y2*x3 -y3*x1 + x3*y1 - x2*y1 + x2*y3;

   double lamda1 = y2*x - y2*x3 - y3*x + x2*y - x2*y + x2*y3;
   lamda1 = lamda1 / dnmr;

   double lamda2 = y3*x - y1*x + y1*x3 + x1*y - x1*y3 - x3*y;
   lamda2 = lamda2 / dnmr;

   double lamda3 = 1 - lamda1 - lamda2;

   printf(" <><><> %lf, %lf, %lf \n", lamda1, lamda2, lamda3);

   // apply barycentric coordinates
   double f = lamda1*f_val[0] + lamda2*f_val[1] + lamda3*f_val[2];
   printf(" <><> interp: %lf \n", f);

}




