
#include "stdio.h"
#define CMDOPTS 50


   //   EXAMPLE INPUT POINTS AND FACETS
   // +++++++++++++++++++++++++++++++++++++++++++++++++
   //
   //           *(0,6)
   //
   //
   //                           * (3,4)
   //
   //
   //             * (1,3)
   //
   //                                * (4,2)
   //
   //                    *(2,1)
   //
   //                             X (3, 0) <<-- point location
   //     *
   //   (-1,-1)
   //
   //                           * (3,-2)
   //
   // +++++++++++++++++++++++++++++++++++++++++++++++++
   //
   //  FACETS: (1, 3) -> (0, 6) -> (-1,-1)
   //  FACETS: (3, 4) -> (1, 3) -> ( 0, 6)
   //  FACETS: (2, 1) -> (3,-2) -> ( 4, 2)
   //  FACETS: (2, 1) -> (3,-2) -> (-1,-1)
   //  FACETS: (2, 1) -> (1, 3) -> (-1,-1)
   //  FACETS: (2, 1) -> (3, 4) -> ( 1, 3)
   //  FACETS: (2, 1) -> (3, 4) -> ( 4, 2)
   //  ======================================


coordT* create_points();
double* create_func_values();
void linear_interp2d_facet(qhT *qh, int dims, double* f_val);
void print_all_facets(qhT *qh);
  
int main()
{

   printf(" \n\n GET IT ON!! \n\n ");
   
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
   
   int num_points = 7;
   int dims = 2;
   
   // grab given grid points
   coordT* allpts = create_points();
   // make some values
   double* f_val = create_func_values();
   
   // not sure what this means yet
   qh->NOerrexit = False;

   // commandline option for:
   //   d - Delaunay triangulation by lifting points to a paraboloid
   char noptions[CMDOPTS] = "QVn d";
   qh_initflags(qh, noptions);
   
   // true for delaunay and will aloow read-in points
   qh->PROJECTdelaunay = True;
   
   // second initialization with points
   qh_init_B(qh, allpts, num_points, dims, ismalloc);
   
   // convex hull
   qh_qhull(qh);
   
   // triangluate hull points
   qh_triangulate(qh);
   
   
   // ---------- FINDING DELAUNAY TRIANGLE -----------
   
   // coordT single_point[2];
   // realT bestdist;
   // boolT isoutside;
   
   // // find triangle for this point
   // single_point[0] = 3.0;
   // single_point[1] = 0.0;
   
   // // bumps-up dimension to calculate projections
   // qh_setdelaunay(qh, dims+1, 1, single_point);
   
   // // find facet that contains point
   // facetT *facet = qh_findbestfacet(qh, single_point, True, &bestdist, &isoutside);
   
   
   linear_interp2d_facet(qh, dims, f_val);
   
   // printf(" facet id: %u,  \n", facet->id);
  
   print_all_facets(qh);
   
   free(allpts);

}

/**
*
*
*/
void print_all_facets(qhT *qh)
{
   facetT *facet;
   vertexT *vertex;
   
   // Iterating through all valid triangles
   facet = qh->facet_list;
   unsigned int ct = 0;
   while (facet->id != 0) {

    // only want triangle projected on 2D surface
    if (!facet->upperdelaunay) {

      printf("\nfacet id: %u,  count: %u \n", facet->id, ct);
      
      vertex = facet->vertices->e[0].p;
      printf("0 vertex coords: %lf, %lf  \n", vertex->point[0], vertex->point[1]);

      vertex = facet->vertices->e[1].p;
      printf("1 vertex coords: %lf, %lf  \n", vertex->point[0], vertex->point[1]);

      vertex = facet->vertices->e[2].p;
      printf("2 vertex coords: %lf, %lf  \n", vertex->point[0], vertex->point[1]);

      ct = ct + 1;
    }
    facet = facet->next;
    
   }
  
}


/**
* Linear interpolation using barycentric coordinates
*
*/
void linear_interp2d_facet(qhT *qh, int dims, double* f_val)
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

/**
* Test function that creates 2D points. 
*/
coordT* create_points()
{

   coordT* T;

   T = malloc(sizeof(coordT*)*14);
   
   T[0] = 0.0;
   T[1] = 6.0;
   
   T[2] = -1.0;
   T[3] = -1.0;
   
   T[4] = 1.0;
   T[5] = 3.0;
   
   T[6] = 2.0;
   T[7] = 1.0;
   
   T[8] = 3.0;
   T[9] = 4.0;
   
   T[10] = 4.0;
   T[11] = 2.0;
   
   T[12] = 3.0;
   T[13] = -2.0;

   

   return T;
}

/**
 * Test function that creates function values 
 * 
 * 
 */
double* create_func_values()
{
   double* f_val = malloc(sizeof(double*)*7);

   f_val[0] = 3.4;
   f_val[1] = 3.4;
   f_val[2] = 3.4;
   f_val[3] = 3.4;
   f_val[4] = 3.4;
   f_val[5] = 3.4;
   f_val[6] = 3.4;
   
   return f_val;

}


