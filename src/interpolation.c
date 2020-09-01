#include "../include/interpolation.h"
#define CMDOPTS 50
#define DIMS2D 2


/**
 * Entry point and wrapper function to QHull and linear interpolation
 *
 * @param points     array of 2D point coordinates
 * @param values     array of values for each 2D points e.g. f(x,y) = values[0]
 * @param num_pts    number of 2D points (values & points)
 * @param ipoints    array of 2D interpolated points coordinates 
 * @param ivalues    array of 2D interpolated points values
 * @param inum_pts   number of 2D interpolated points (values & points)
 * @param fill_value fill value for points outisde convex hull
 * @return rtn_code       return code - normal 0, Error -1 
 */
int griddata(double* points   , 
             double* values   , 
             int num_pts      , 
             double* ipoints  , 
             double* ivalues  , 
             int inum_pts     , 
             double fill_value)
{
   
   int rtn_code = _qhull_QVnd(points, 
                              values, 
                              num_pts, 
                              ipoints, 
                              ivalues, 
                              inum_pts, 
                              fill_value);
   
   if(rtn_code < 0){
      return -1;
   } else
      return 0;
   
}


/**
 * QHull 'QVn d' functionality: (1) convex hull, (2) delaunay triangluation 
 *                      (3) barycentric coordinate interpolation
 * 
 * @param points     array of 2D points
 * @param values     array of values for each 2D points e.g. f(x,y) = values[0]
 * @param num_pts    number of 2D points (values & points)
 * @param ipoints    array of 2D interpolated points coordinates 
 * @param ivalues    array of 2D interpolated points values
 * @param inum_pts   number of 2D interpolated points (values & points)
 * @param fill_value fill value for points outisde convex hull
 * @return rtn_code       return code - normal 0, Error -1
 */
int _qhull_QVnd(double* points, 
                double* values,
                int num_pts,
                double* ipoints, 
                double* ivalues,
                int inum_pts,
                double fill_value)
{
   //   d - Delaunay triangulation by lifting points to a paraboloid
   char noptions[CMDOPTS] = "QVn d";
   
   // new instance for qhull
   qhT qh_qh;
   qhT *qh= &qh_qh;
   
   // struture checks
   QHULL_LIB_CHECK
   
   // True if qh_freeqhull should 'free(array)'
   boolT ismalloc = False; 
   
   // initiate qhull - 
   // legacy call to arm variables that are generally used for command line 
   qh_init_A(qh, stdin, stdout, stderr, 0, NULL);
   int exitcode = setjmp(qh->errexit);
   
   // not sure what this means yet
   qh->NOerrexit = False;

   // commandline option for:
   qh_initflags(qh, noptions);
   
   // true for delaunay and will aloow read-in points
   qh->PROJECTdelaunay = True;

   coordT* allpts = points;
   
   // second initialization with points
   qh_init_B(qh, allpts, num_pts, DIMS2D, ismalloc);
   
   // convex hull
   qh_qhull(qh);
   
   // triangluate hull points
   qh_triangulate(qh);

   // interpolate with known point values
   _linear_interp2d_facet(qh, ipoints, ivalues, inum_pts, values, fill_value);
     
   _print_all_facets(qh);
   
   free(allpts);
   
   if(exitcode < 0)
      return -1;
   else
      return 0;
   
}

/**
 * Linear interpolation using barycentric coordinates
 *
 * @param qh      qhull reference
 * @param ipoints    array of 2D interpolated points coordinates 
 * @param ipval    array of 2D interpolated points values
 * @param inum_pts   number of 2D interpolated points (values & points)
 * @param pval   array of values for each 2D points
 * @param fill_value fill value for points outisde convex hull
 */
void _linear_interp2d_facet(qhT *qh, 
                            double* ipoints, 
                            double* ipval,
                            int inum_pts, 
                            double* pval,
                            double fill_value)
{
   facetT *facet;
   vertexT *vertex;
   coordT single_point[2];
   realT bestdist;
   boolT isoutside;
   int rtn_code = 0;
   int ncoords = inum_pts*2;

   for(int i = 0; i < ncoords; i+=2 ){
   
      single_point[0] = ipoints[i];
      single_point[1] = ipoints[i+1];
      int idx = i/2;

      // bumps-up dimension to calculate projections
      qh_setdelaunay(qh, DIMS2D+1, 1, single_point);
   
      // find facet that contains point
      facet = qh_findbestfacet(qh, single_point, True, &bestdist, &isoutside);
      
      if (isoutside > 0){
      
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

         /* 
            Converting cartesian to barycentric coordinates
            https://en.wikipedia.org/wiki/Barycentric_coordinate_system
            
            denomr = ((y2-y3)*(x1-x3) + (x3-x2)*(y1-y3))
            L1 = ((y2-y3)*(x-x3) + (x3-x2)*(y-y3)) / denomr
            L2 = ((y3-y1)*(x-x3) + (x1-x3)*(y-y3)) / denomr
            L3 = 1.0 - L1 - L2
         */
         
         double denom = y2*x1 - y2*x3 -y3*x1 + x3*y1 - x2*y1 + x2*y3;

         double L1 = y2*x - y2*x3 - y3*x + x3*y - x2*y + x2*y3;
         L1 = L1 / denom;

         double L2 = y3*x - y1*x + y1*x3 + x1*y - x1*y3 - x3*y;
         L2 = L2 / denom;

         double L3 = 1 - L1 - L2;
         
         // apply barycentric coordinates
         double f = L1*pval[0] + L2*pval[1] + L3*pval[2];
         ipval[idx] = f;
         
         #ifndef DPRINT
            printf("point: (%lf, %lf) \n", x, y );
            printf(" -- facet coords:  (%lf, %lf) -> (%lf, %lf)-> (%lf, %lf) \n", 
                   x1, y1, x2, y2, x3, y3);
            printf(" -- LAMDA: %lf, %lf, %lf \n", L1, L2, L3);
            printf(" -- interpolated value: %lf \n", f);
         #endif
     
      } else {
      
         double x = single_point[0];
         double y = single_point[1];
      
         ipval[idx] = fill_value;
         
         #ifndef DPRINT
            printf("OUTSIDE point: (%lf, %lf) \n", x, y );
            printf(" -- fill value: %lf \n", fill_value);
         #endif
      }
      
   } // end for
    
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





