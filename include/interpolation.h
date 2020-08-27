#ifndef _INTERPOLATE_H
#define _INTERPOLATE_H

void griddata(double* points, double* values, double fill_value, int num_pts);
void _linear_interp2d_facet(qhT *qh, int dims, double* f_val);
void _qhull(double* points, double* values, double fill_value, int num_pts);

#endif
