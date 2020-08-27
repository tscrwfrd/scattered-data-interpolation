#include "../include/interpolation.h"
#include "stdio.h"

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


double* create_points();
double* create_func_values();
void _print_all_facets(qhT *qh);

  
int main()
{
   printf("\nNew approach... \n\n");

   double* pts = create_points();
   double* f_val = create_func_values();
   double fill_value = 3.5;

   griddata(pts, f_val, fill_value, 7);

}


/**
*
* Helper function that prints all triangles within convex hull
* @param   qh
*/
void _print_all_facets(qhT *qh)
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
* Test function that creates 2D points. 
*/
double* create_points()
{

   double* T;

   T = malloc(sizeof(double*)*14);
   
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




