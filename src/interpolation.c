#include "../include/interpolation.h"
#include "../qhull/src/libqhull_r/qhull_ra.h"

/**
 * Linear interpolation using barycentric coordinates
 *
 * @param qh      qhull reference
 * @param ipoints array of 2D interpolated points coordinates
 * @param ipval   array of 2D interpolated points values
 * @param inum_pts number of 2D interpolated points (values & points)
 * @param pval     array of values for each 2D points
 * @param fill_value fill value for points outisde convex hull
 */
static void linear_interp2d_facet(qhT *qh,
                                  double* ipoints,
                                  double* ipval,
                                  int inum_pts,
                                  double* pval,
                                  double fill_value) {

  // facet reference
  facetT *facet;

  // vertices references
  vertexT *vertex, **vertexp;

  // coordinates
  coordT single_point[2];

  // coords always in 2D
  int ncoords = inum_pts * 2;

  // loop through all coordinates!
  for (int i = 0; i < ncoords; i += 2) {

    single_point[0] = ipoints[i];
    single_point[1] = ipoints[i + 1];
    int idx = i / 2;

    // facet list reference
    facet = qh->facet_list;

    // flag to mark points inside convex hull
    bool inside_hull = false;

    // default value
    ipval[idx] = -9999.0;

    while (facet->id != 0) {

      if (!facet->upperdelaunay) {

        double tript_x[3];
        double tript_y[3];

        int count = 0;

        FOREACHvertex_(facet->vertices) {
          tript_x[count] = vertex->point[0];
          tript_y[count] = vertex->point[1];
          ++count;
        }// end collecting triangle points

        // More efficient with this procedure:
        // https://math.stackexchange.com/questions/51326/determining-if-an-arbitrary-point-lies-inside-a-triangle-defined-by-three-points


        double x = single_point[0];
        double y = single_point[1];

        vertex = facet->vertices->e[0].p;
        double x1 = tript_x[0];
        double y1 = tript_y[0];
        double f1 = pval[qh_pointid(qh, vertex->point)];

        vertex = facet->vertices->e[1].p;
        double x2 = tript_x[1];
        double y2 = tript_y[1];
        double f2 = pval[qh_pointid(qh, vertex->point)];

        vertex = facet->vertices->e[2].p;
        double x3 = tript_x[2];
        double y3 = tript_y[2];
        double f3 = pval[qh_pointid(qh, vertex->point)];

        /*
           Converting Cartesian to barycentric coordinates
           https://en.wikipedia.org/wiki/Barycentric_coordinate_system

           denomr = ((y2-y3)*(x1-x3) + (x3-x2)*(y1-y3))
           L1 = ((y2-y3)*(x-x3) + (x3-x2)*(y-y3)) / denomr
           L2 = ((y3-y1)*(x-x3) + (x1-x3)*(y-y3)) / denomr
           L3 = 1.0 - L1 - L2
         */

        double denom = y2*x1 - y2*x3 - y3*x1 + x3*y1 - x2*y1 + x2*y3;

        double L1 = y2*x - y2*x3 - y3*x + x3*y - x2*y + x2*y3;
        L1 = L1/denom;

        double L2 = y3*x - y1*x + y1*x3 + x1*y - x1*y3 - x3*y;
        L2 = L2/denom;

        double L3 = 1 - L1 - L2;


        if (L1 > 0.0 && L2 > 0.0 && L3 > 0.0) {
          inside_hull = true;

          // apply barycentric coordinates
          double f = L1*f1 + L2*f2 + L3*f3;
          ipval[idx] = f;
          break;

        }


      } // end !upperdelauney

      facet = facet->next;
    }// end looking through facets

    // if false, current point is not inside any triangle
    // and not inside convex hull.
    if(!inside_hull){
      ipval[idx] = fill_value;
      fprintf(stdout, " -- WARNING:: point (%f, %f) outside convex hull \n",
              single_point[0], single_point[1]);

    }


  } // end for

}

/**
 * Interpolates unstrucutred 2-D data.  This function does not check for
 * conditions where points form lines or skinny triangle elements - which may produce
 * undefined behaviors.
 *
 * @param   points   the x,y coordinates of the points with known values
 * @param   value    the known values
 * @param   num_pts  the number of known values, or use following:
 *                   int num_pts  = sizeof(values)/sizeof(values[0]); should be at least three points
 * @param   ipoints  an array of the x,y  coordinate of the point(s) whose unknown value to
 *                   interpolate
 * @param   ivalues   The value array for qhull interpolation results
 * @param   inum_pts  The number of unknown values. or use following:
 *                    int inum_pts = sizeof(ipoints)/(2 * sizeof(ipoints[0]));
 * @param   fill_Value  the fill value to use
 */
int griddata(double* points,
             double* values,
             int num_pts,
             double* ipoints,
             double* ivalues,
             int inum_pts,
             double fill_value) {

  const int DIMS2D  = 2;

  if (num_pts < 4) {
    fprintf(stdout, " -- ERROR: Qhull needs a minimum of four points.\n");
    return INTERP_MIN_ERROR;

  } else if (inum_pts < 1) {
    fprintf(stdout," -- ERROR: implementation needs a minimum ");
    fprintf(stdout, "of one interpolated point location.\n");
    return INTERP_MIN_ERROR;

  }

  //   d - Delaunay triangulation by lifting points to a paraboloid
  //   J  - slightly joggles the point location
  //   char noptions[CMDOPTS] = "QVn d";
  char *noptions = "QVn QJ d";

  // new instance for qhull
  qhT qh_qh;
  qhT *qh = &qh_qh;

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

  // true for delaunay and will allow read-in points
  qh->PROJECTdelaunay = True;

  coordT* allpts = points;

  // second initialization with points
  qh_init_B(qh, allpts, num_pts, DIMS2D, ismalloc);

  // convex hull
  qh_qhull(qh);

  // triangluate hull points
  qh_triangulate(qh);

  // interpolate with known point values
  linear_interp2d_facet(qh, ipoints, ivalues, inum_pts, values, fill_value);

  if (exitcode < 0)
    return -1;
  else
    return 0;

}





