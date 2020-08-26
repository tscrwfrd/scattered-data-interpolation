
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
void delaunay_triangles();
  
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
   
   vertexT *vertex, **vertexp;
   int k;
   
   FOREACHvertex_(facet->vertices) {
    for (k=0; k < dims; k++)
      printf("%5.2f ", vertex->point[k]);
    printf("\n");
  }
  printf(" facet id: %u,  \n", facet->id);
  
  
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
   
   free(allpts);

}


void delaunay_triangles()
{



}

/**
* creating 2D points
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


