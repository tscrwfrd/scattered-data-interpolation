#ifndef _INTERPOLATE_H
#define _INTERPOLATE_H

int griddata(double* points   , 
             double* values   , 
             int num_pts      , 
             double* ipoints  , 
             double* ivalues  , 
             int inum_pts     , 
             double fill_value);
void _linear_interp2d_facet(qhT *qh, 
                            double* ipoints, 
                            double* ipval,
                            int inum_pts, 
                            double* pval,
                            double fillvalue);
int _qhull_QVnd(double* points, 
                double* values,
                int num_pts,
                double* ipoints, 
                double* ivalues,
                int inum_pts,
                double fill_value);
void _print_all_facets(qhT *qh);

#endif
