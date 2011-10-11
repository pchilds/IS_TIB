/***************************************************************************
 *            plotlinear.c
 *
 *  A GTK+ widget that plots data
 *  version 0.1.0
 *  Features:
 *            automatic wiggle insertion
 *            multiple plot capability
 *            optimal visualisation of axes
 *            zoomable ranges
 *            signal emission for mouse movement
 *
 *  Sat Dec  4 17:18:14 2010
 *  Copyright  2010  Paul Childs
 *  <pchilds@physics.org>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include <gtk/gtk.h>
#include <math.h>
#include <cairo-ps.h>
#include <cairo-svg.h>
#include "plotlinear0-1-0.h"

#define PLOT_LINEAR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), PLOT_TYPE_LINEAR, PlotLinearPrivate))
#define ARP 0.05 /* Proportion of the graph occupied by arrows */
#define IRTR 0.577350269 /* 1/square root 3 */
#define MY_2PI 6.2831853071795864769252867665590057683943387987502
#define WGP 0.08 /* Proportion of the graph the wiggles occupy */
#define WFP 0.01 /* Proportion of wiggle that is flat to axis */
#define WMP 0.045 /* the mean of these */
#define WHP 0.020207259 /* wiggle height proportion */
#define DZE 0.00001 /* divide by zero threshold */
#define NZE -0.00001 /* negative of this */
#define FAC 0.05 /* floating point accuracy check for logarithms etc */
#define NAC 0.95 /* conjugate of this */
#define JT 5 /* major tick length */
#define JTI 6 /* this incremented */
#define NT 3 /* minor tick length */
#define ZS 0.5 /* zoom scale */
#define ZSC 0.5 /* 1 minus this */
#define UZ 2 /* inverse of this */
#define UZC 1 /* this minus 1 */
typedef enum
{
	PLOT_LINEAR_BORDERS_LT = 1 << 0,
	PLOT_LINEAR_BORDERS_RT = 1 << 1,
	PLOT_LINEAR_BORDERS_DN = 1 << 2,
	PLOT_LINEAR_BORDERS_UP = 1 << 3
} PlotLinearBorders;
typedef enum
{
	PLOT_LINEAR_AXES_LW = 1 << 0,
	PLOT_LINEAR_AXES_RW = 1 << 1,
	PLOT_LINEAR_AXES_DW = 1 << 2,
	PLOT_LINEAR_AXES_UW = 1 << 3,
	PLOT_LINEAR_AXES_LR = 1 << 4,
	PLOT_LINEAR_AXES_LT = 1 << 5
} PlotLinearAxes;
G_DEFINE_TYPE (PlotLinear, plot_linear, GTK_TYPE_DRAWING_AREA);
enum {PROP_0, PROP_BXN, PROP_BXX, PROP_BYN, PROP_BYX, PROP_XTJ, PROP_YTJ, PROP_XTN, PROP_YTN, PROP_FA};
enum {MOVED, LAST_SIGNAL};
static guint plot_linear_signals[LAST_SIGNAL]={0};
typedef struct _PlotLinearPrivate PlotLinearPrivate;
struct xs {gdouble xmin, ymin, xmax, ymax;};
struct tk {guint xj, yj, xn, yn;};
struct _PlotLinearPrivate {struct xs bounds, rescale; struct tk ticks, range; guint flaga, flagr;};

static void drawz(GtkWidget *wgt, cairo_t *cr)
{
	gint xw;
	gdouble dt;
	PlotLinear *plt;

	xw=(wgt->allocation.width);
	plt=PLOT_LINEAR(wgt);
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_set_line_width(cr, 1);
	cairo_rectangle(cr, xw-21.5, 0.5, 10, 10);
	cairo_rectangle(cr, xw-10.5, 0.5, 10, 10);
	cairo_move_to(cr, xw-9, 5.5);
	cairo_line_to(cr, xw-2, 5.5);
	cairo_move_to(cr, xw-5.5, 2);
	cairo_line_to(cr, xw-5.5, 9);
	if (((plt->zmode)&PLOT_LINEAR_ZOOM_OUT)==0)
	{
		cairo_move_to(cr, xw-6.5, 2.5);
		cairo_line_to(cr, xw-5.5, 2);
		cairo_line_to(cr, xw-4.5, 2.5);
		cairo_move_to(cr, xw-2.5, 4.5);
		cairo_line_to(cr, xw-2, 5.5);
		cairo_line_to(cr, xw-2.5, 6.5);
		cairo_move_to(cr, xw-6.5, 8.5);
		cairo_line_to(cr, xw-5.5, 9);
		cairo_line_to(cr, xw-4.5, 8.5);
		cairo_move_to(cr, xw-8.5, 4.5);
		cairo_line_to(cr, xw-9, 5.5);
		cairo_line_to(cr, xw-8.5, 6.5);
	}
	else
	{
		cairo_move_to(cr, xw-7.5, 3.5);
		cairo_line_to(cr, xw-3.5, 7.5);
		cairo_move_to(cr, xw-7.5, 7.5);
		cairo_line_to(cr, xw-3.5, 3.5);
	}
	cairo_stroke(cr);
	if (((plt->zmode)&PLOT_LINEAR_ZOOM_SGL)!=0)
	{
		cairo_move_to(cr, xw-20, 2);
		cairo_line_to(cr, xw-13, 9);
		cairo_move_to(cr, xw-20, 9);
		cairo_line_to(cr, xw-13, 2);
		cairo_stroke(cr);
	}
	else
	{
		cairo_save(cr);
		dt=1;
		cairo_set_dash(cr, &dt, 1, 0);
		if (((plt->zmode)&PLOT_LINEAR_ZOOM_VRT)!=0)
		{
			cairo_move_to(cr, xw-20, 2.5);
			cairo_line_to(cr, xw-13, 2.5);
			cairo_move_to(cr, xw-20, 8.5);
			cairo_line_to(cr, xw-13, 8.5);
			cairo_stroke(cr);
		}
		if (((plt->zmode)&PLOT_LINEAR_ZOOM_HZT)!=0)
		{
			cairo_move_to(cr, xw-19.5, 2);
			cairo_line_to(cr, xw-19.5, 9);
			cairo_move_to(cr, xw-13.5, 2);
			cairo_line_to(cr, xw-13.5, 9);
			cairo_stroke(cr);
		}
		cairo_restore(cr);
	}
}

static void draw(GtkWidget *wgt, cairo_t *cr)
{
	PlotLinearPrivate *prv;
	PlotLinear *plt;
	gint j, k, xw, yw, xr, xr2, yr, yr2, xa, ya, xl, yl, xu, yu, tf, tz, to, tn, tnn, xv, yv, xvn, yvn, dtt, tx, wd, hg, ft, lt, xt;
	gdouble vv, wv, zv, av, dt, lr1, lr2, dlx, dly;
	guint lr3;
	gchar *st1=NULL;
	gchar lbl[10];
	PangoLayout *lyt;
	cairo_matrix_t mt2, mt3;

	{mt2.xx=0; mt2.xy=1; mt2.yx=-1; mt2.yy=0; mt2.x0=0; mt2.y0=0;}/* initialise */
	{mt3.xx=0; mt3.xy=-1; mt3.yx=1; mt3.yy=0; mt3.x0=0; mt3.y0=0;}
	plt=PLOT_LINEAR(wgt);
	xw=(wgt->allocation.width);
	yw=(wgt->allocation.height);
	prv=PLOT_LINEAR_GET_PRIVATE(plt);
	(prv->flaga)&=(PLOT_LINEAR_AXES_LT|PLOT_LINEAR_AXES_LR);
	dlx=((prv->bounds.xmax)-(prv->bounds.xmin))/(prv->ticks.xj);
	dly=((prv->bounds.ymax)-(prv->bounds.ymin))/(prv->ticks.yj);
	lyt=pango_cairo_create_layout(cr);
	pango_layout_set_font_description(lyt, (plt->lfont));
	st1=g_strconcat((plt->xlab), (plt->ylab), NULL);
	pango_layout_set_text(lyt, st1, -1);
	pango_layout_get_pixel_size(lyt, &wd, &dtt);
	g_free(st1);
	g_object_unref(lyt);
	lyt=pango_cairo_create_layout(cr);
	pango_layout_set_font_description(lyt, (plt->afont));
	st1=g_strdup("27");
	pango_layout_set_text(lyt, st1, -1);
	pango_layout_get_pixel_size(lyt, &wd, &hg);
	dtt+=hg;
	g_free(st1);
	g_object_unref(lyt);
	xr=MIN(xw*ARP,dtt);
	xr2=(xr-2)*IRTR;
	yr=MIN(yw*ARP,dtt);
	yr2=(yr-2)*IRTR;
	dtt+=JTI;
	lyt=pango_cairo_create_layout(cr);
	pango_layout_set_font_description(lyt, (plt->afont));
	if ((prv->bounds.ymax)<=DZE) /* determine positions of axes */
	{
		(prv->flaga)|=PLOT_LINEAR_AXES_LT;
		lr3=3;
		if ((prv->bounds.ymin)<-1) lr3+=floor(log10(-(prv->bounds.ymin)));
		if ((plt->ydp)!=0) g_snprintf(lbl, lr3+(plt->ydp)+1, "%f", (prv->bounds.ymin));
		else g_snprintf(lbl, lr3, "%f", (prv->bounds.ymin));
		pango_layout_set_text(lyt, lbl, -1);
		pango_layout_get_pixel_size(lyt, &wd, &hg);
		yl=yw-(wd/2)-1; /* allow space for lower label */
		ya=dtt;
		yu=(yl-ya)*((prv->bounds.ymax)/(prv->bounds.ymin));
		if (yu>(yw*WGP)) {(prv->flaga)|=PLOT_LINEAR_AXES_DW; yu=yw*WGP;}
		yu+=ya;
	}
	else if ((prv->bounds.ymin)>=NZE)
	{
		yu=yr;
		ya=yw-dtt;
		yl=(ya-yu)*((prv->bounds.ymin)/(prv->bounds.ymax));
		if (yl>yw*WGP) {(prv->flaga)|=PLOT_LINEAR_AXES_UW; yl=(yw*WGP);}
		yl=-yl;
		yl+=ya;
	}
	else if (((prv->flaga)&PLOT_LINEAR_AXES_LT)!=0)
	{
		if ((prv->bounds.ymin)+(prv->bounds.ymax)<=0)
		{
			lr3=3;
			if ((prv->bounds.ymin)<-1) lr3+=floor(log10(-(prv->bounds.ymin)));
			if ((plt->ydp)!=0) g_snprintf(lbl, lr3+(plt->ydp)+1, "%f", (prv->bounds.ymin));
			else g_snprintf(lbl, lr3, "%f", (prv->bounds.ymin));
			pango_layout_set_text(lyt, lbl, -1);
			pango_layout_get_pixel_size(lyt, &wd, &hg);
			yl=yw-(wd/2)-1; /* allow space for lower label */
			yu=(yl-dtt)*((prv->bounds.ymax)/(prv->bounds.ymin));
			yu+=dtt;
			if (yu<yr) {yu=yr; ya=((yl*(prv->bounds.ymax))-(yr*(prv->bounds.ymin)))/((prv->bounds.ymax)-(prv->bounds.ymin));}
			else ya=dtt;
		}
		else
		{
			yl=yw;
			yu=yr;
			ya=((yl*(prv->bounds.ymax))-(yu*(prv->bounds.ymin)))/((prv->bounds.ymax)-(prv->bounds.ymin));
			if (ya>(yw-xr2))
			{
				yl*=(yw-xr2)/ya;
				ya=yw-xr2;
			}
		}
	}
	else
	{
		yu=yr;
		ya=yw-dtt;
		yl=(yu-ya)*((prv->bounds.ymin)/(prv->bounds.ymax));
		yl+=ya;
		if (yl>yw)
		{
			yl=yw;
			if ((prv->bounds.ymin)+(prv->bounds.ymax)<=0)
			{
				lr3=3;
				if ((prv->bounds.ymin)<-1) lr3+=floor(log10(-(prv->bounds.ymin)));
				if ((plt->ydp)!=0) g_snprintf(lbl, lr3+(plt->ydp)+1, "%f", (prv->bounds.ymin));
				else g_snprintf(lbl, lr3, "%f", (prv->bounds.ymin));
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				yl+=wd/2;
				yl--; /* allow space for lower label */
			}
			ya=((yl*(prv->bounds.ymax))-(yu*(prv->bounds.ymin)))/((prv->bounds.ymax)-(prv->bounds.ymin));
		}
	}
	g_object_unref(lyt);
	lyt=pango_cairo_create_layout(cr);
	pango_layout_set_font_description(lyt, (plt->afont));
	if (prv->bounds.xmax<=DZE)
	{
		(prv->flaga)|=PLOT_LINEAR_AXES_LR;
		lr3=3;
		if ((prv->bounds.xmin)<-1) lr3+=floor(log10(-(prv->bounds.xmin)));
		if ((plt->xdp)!=0) g_snprintf(lbl, lr3+(plt->xdp)+1, "%f", (prv->bounds.xmin));
		else g_snprintf(lbl, lr3, "%f", (prv->bounds.xmin));
		pango_layout_set_text(lyt, lbl, -1);
		pango_layout_get_pixel_size(lyt, &wd, &hg);
		xl=wd/2; /* allow space for left label */
		xa=xw-dtt;
		xu=(xa-xl)*((prv->bounds.xmax)/(prv->bounds.xmin));
		if (xu>(xw*WGP)) {(prv->flaga)|=PLOT_LINEAR_AXES_LW; xu=xw*WGP;}
		xu=-xu;
		xu+=xa;
	}
	else if (prv->bounds.xmin>=NZE)
	{
		xu=xw-xr;
		xa=dtt;
		xl=(xu-xa)*((prv->bounds.xmin)/(prv->bounds.xmax));
		if (xl>(xw*WGP)) {(prv->flaga)|=PLOT_LINEAR_AXES_RW; xl=(xw*WGP);}
		xl+=xa;
	}
	else if (((prv->flaga)&PLOT_LINEAR_AXES_LR)!=0)
	{
		if ((prv->bounds.xmin)+(prv->bounds.xmax)<=0)
		{
			lr3=3;
			if ((prv->bounds.xmin)<-1) lr3+=floor(log10(-(prv->bounds.xmin)));
			if ((plt->xdp)!=0) g_snprintf(lbl, lr3+(plt->xdp)+1, "%f", (prv->bounds.xmin));
			else g_snprintf(lbl, lr3, "%f", (prv->bounds.xmin));
			pango_layout_set_text(lyt, lbl, -1);
			pango_layout_get_pixel_size(lyt, &wd, &hg);
			xl=wd/2; /* allow space for left label */
			xa=xw-dtt;
			xu=(xa-xl)*((prv->bounds.xmax)/(prv->bounds.xmin));
			xu+=dtt;
			if (xu<xr)
			{
				xu=xw-xr;
				xa=((xl*(prv->bounds.xmax))-(xu*(prv->bounds.xmin)))/((prv->bounds.xmax)-(prv->bounds.xmin));
			}
			else xu=xw-xu;
		}
		else
		{
			xu=xw-xr;
			xa=-xu*((prv->bounds.xmin)/((prv->bounds.xmax)-(prv->bounds.xmin)));
			if (xa<yr2)
			{
				xl=xu*((yr2-xa)/(xu-xa));
				xa=yr2;
			}
			else xl=0;
		}
	}
	else
	{
		xu=xw-xr;
		xa=dtt;
		xl=(xu-xa)*((prv->bounds.xmin)/(prv->bounds.xmax));
		xl+=xa;
		if (xl<0)
		{
			if ((prv->bounds.xmin)+(prv->bounds.xmax)<=0)
			{
				lr3=3;
				if ((prv->bounds.xmin)<-1) lr3+=floor(log10(-(prv->bounds.xmin)));
				if ((plt->xdp)!=0) g_snprintf(lbl, lr3+(plt->xdp)+1, "%f", (prv->bounds.xmin));
				else g_snprintf(lbl, lr3, "%f", (prv->bounds.xmin));
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				xl=wd/2; /* allow space for left label */
				xa=((xl*(prv->bounds.xmax))-(xu*(prv->bounds.xmin)))/((prv->bounds.xmax)-(prv->bounds.xmin));
			}
			else {xl=0; xa=-xu*((prv->bounds.xmin)/((prv->bounds.xmax)-(prv->bounds.xmin)));}
		}
	}
	g_object_unref(lyt);
	{(prv->range.xj)=xl; (prv->range.yj)=yu; (prv->range.xn)=xu; (prv->range.yn)=yl;}
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_set_line_width(cr, 2);
	cairo_move_to(cr, 0, ya);
	if (((prv->flaga)&PLOT_LINEAR_AXES_LW)!=0) /* draw x wiggles */
	{
		cairo_line_to(cr, xa-(WGP*xw), ya);
		cairo_curve_to(cr, xa-(WMP*xw), ya-(WHP*xw), xa-(WMP*xw), ya+(WHP*xw), xa-(WFP*xw), ya);
	}
	else if (((prv->flaga)&PLOT_LINEAR_AXES_RW)!=0)
	{
		cairo_line_to(cr, xa+(WFP*xw), ya);
		cairo_curve_to(cr, xa+(WMP*xw), ya-(WHP*xw), xa+(WMP*xw), ya+(WHP*xw), xa+(WGP*xw), ya);
	}
	xr--;
	xr--;
	cairo_line_to(cr, xw, ya); /* draw x axis */
	cairo_line_to(cr, xw-xr, ya-xr2); /* draw x arrow */
	cairo_move_to(cr, xw, ya);
	cairo_line_to(cr, xw-xr, ya+xr2);
	cairo_move_to(cr, xa, yw);
	if (((prv->flaga)&PLOT_LINEAR_AXES_DW)!=0) /* draw y wiggles */
	{
		cairo_line_to(cr, xa, ya+(WGP*yw));
		cairo_curve_to(cr, xa-(WHP*yw), ya+(WMP*yw), xa+(WHP*yw), ya+(WMP*yw), xa, ya+(WFP*yw));
	}
	else if (((prv->flaga)&PLOT_LINEAR_AXES_UW)!=0)
	{
		cairo_line_to(cr, xa, ya-(WFP*yw));
		cairo_curve_to(cr, xa-(WHP*yw), ya-(WMP*yw), xa+(WHP*yw), ya-(WMP*yw), xa, ya-(WGP*yw));
	}
	{yr--; yr--;}
	cairo_line_to(cr, xa, 0); /* draw y axis */
	cairo_line_to(cr, xa-yr2, yr); /* draw y arrow */
	cairo_move_to(cr, xa, 0);
	cairo_line_to(cr, xa+yr2, yr);
	cairo_stroke(cr);
	lyt=pango_cairo_create_layout(cr); /* draw ticks, grid and labels */
	pango_layout_set_font_description(lyt, (plt->lfont));
	pango_layout_set_text(lyt, (plt->xlab), -1);
	pango_layout_get_pixel_size(lyt, &wd, &hg);
	dt=5;
	if (((prv->flaga)&PLOT_LINEAR_AXES_LW)!=0)
	{
		to=xl;
		if (((prv->flaga)&PLOT_LINEAR_AXES_LT)!=0)
		{
			cairo_move_to(cr, (xu+xl-wd)/2, ya-dtt);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_move_to(cr, to, ya);
			cairo_line_to(cr, to, ya-JT);
			cairo_stroke(cr);
			lyt=pango_cairo_create_layout(cr);
			pango_layout_set_font_description(lyt, (plt->afont));
			lr1=round((prv->bounds.xmin)*exp(G_LN10*(plt->xdp)));
			if ((prv->bounds.xmin)>DZE)
			{
				lr1+=0.1;
				lr1*=exp(-G_LN10*(plt->xdp));
				if (lr1>=1)
				{
					lr2=floor(log10(lr1));
					lr2=log10(lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->xdp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
					else g_snprintf(lbl, (plt->xdp)+lr3+3, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->xdp)+3, "%f", lr1);
			}
			else if ((prv->bounds.xmin)<NZE)
			{
				lr1-=0.1;
				lr1*=exp(-G_LN10*(plt->xdp));
				if (lr1<=-1)
				{
					lr2=log10(-lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->xdp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
					else g_snprintf(lbl, (plt->xdp)+lr3+4, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->xdp)+4, "%f", lr1);
			}
			else g_snprintf(lbl, 2, "%f", 0.0);
			pango_layout_set_text(lyt, lbl, -1);
			pango_layout_get_pixel_size(lyt, &wd, &hg);
			cairo_move_to(cr, to-(wd/2), ya-JTI-hg);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_set_line_width(cr, 1);
			cairo_save(cr);
			cairo_set_dash(cr, &dt, 1, 0);
			cairo_move_to(cr, to, yu);
			cairo_line_to(cr, to, yl);
			cairo_stroke(cr);
			cairo_restore(cr);
			for (j=1; j<=(prv->ticks.xj); j++)
			{
				tn=xl+(((xu-xl)*j)/(prv->ticks.xj));
				for (k=1; k<(prv->ticks.xn); k++)
				{
					tnn=to+(((tn-to)*k)/(prv->ticks.xn));
					cairo_move_to(cr, tnn, ya);
					cairo_line_to(cr, tnn, ya-NT);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, tn, ya);
				cairo_line_to(cr, tn, ya-JT);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.xmin)+(j*dlx);
				lr1=round(lr2*exp(G_LN10*(plt->xdp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, tn-(wd/2), ya-JTI-hg);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, tn, yu);
				cairo_line_to(cr, tn, yl);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
		}
		else
		{
			cairo_move_to(cr, (xu+xl-wd)/2, ya+dtt-hg);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_move_to(cr, to, ya);
			cairo_line_to(cr, to, ya+JT);
			cairo_stroke(cr);
			lyt=pango_cairo_create_layout(cr);
			pango_layout_set_font_description(lyt, (plt->afont));
			lr1=round((prv->bounds.xmin)*exp(G_LN10*(plt->xdp)));
			if ((prv->bounds.xmin)>DZE)
			{
				lr1+=0.1;
				lr1*=exp(-G_LN10*(plt->xdp));
				if (lr1>=1)
				{
					lr2=floor(log10(lr1));
					lr2=log10(lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->xdp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
					else g_snprintf(lbl, (plt->xdp)+lr3+3, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->xdp)+3, "%f", lr1);
			}
			else if ((prv->bounds.xmin)<NZE)
			{
				lr1-=0.1;
				lr1*=exp(-G_LN10*(plt->xdp));
				if (lr1<=-1)
				{
					lr2=log10(-lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->xdp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
					else g_snprintf(lbl, (plt->xdp)+lr3+4, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->xdp)+4, "%f", lr1);
			}
			else g_snprintf(lbl, 2, "%f", 0.0);
			pango_layout_set_text(lyt, lbl, -1);
			pango_layout_get_pixel_size(lyt, &wd, &hg);
			cairo_move_to(cr, to-(wd/2), ya+JTI);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_set_line_width(cr, 1);
			cairo_save(cr);
			cairo_set_dash(cr, &dt, 1, 0);
			cairo_move_to(cr, to, yu);
			cairo_line_to(cr, to, yl);
			cairo_stroke(cr);
			cairo_restore(cr);
			for (j=1; j<=(prv->ticks.xj); j++)
			{
				tn=xl+(((xu-xl)*j)/(prv->ticks.xj));
				for (k=1; k<(prv->ticks.xn); k++)
				{
					tnn=to+(((tn-to)*k)/(prv->ticks.xn));
					cairo_move_to(cr, tnn, ya);
					cairo_line_to(cr, tnn, ya+NT);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, tn, ya);
				cairo_line_to(cr, tn, ya+JT);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.xmin)+(j*dlx);
				lr1=round(lr2*exp(G_LN10*(plt->xdp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, tn-(wd/2), ya+JTI);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, tn, yu);
				cairo_line_to(cr, tn, yl);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
		}
	}
	else if (((prv->flaga)&PLOT_LINEAR_AXES_RW)!=0)
	{
		to=xl;
		if (((prv->flaga)&PLOT_LINEAR_AXES_LT)!=0)
		{
			cairo_move_to(cr, (xu+xl-wd)/2, ya-dtt);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_move_to(cr, to, ya);
			cairo_line_to(cr, to, ya-JT);
			cairo_stroke(cr);
			lyt=pango_cairo_create_layout(cr);
			pango_layout_set_font_description(lyt, (plt->afont));
			lr1=round((prv->bounds.xmin)*exp(G_LN10*(plt->xdp)));
			if ((prv->bounds.xmin)>DZE)
			{
				lr1+=0.1;
				lr1*=exp(-G_LN10*(plt->xdp));
				if (lr1>=1)
				{
					lr2=floor(log10(lr1));
					lr2=log10(lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->xdp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
					else g_snprintf(lbl, (plt->xdp)+lr3+3, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->xdp)+3, "%f", lr1);
			}
			else if ((prv->bounds.xmin)<NZE)
			{
				lr1-=0.1;
				lr1*=exp(-G_LN10*(plt->xdp));
				if (lr1<=-1)
				{
					lr2=log10(-lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->xdp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
					else g_snprintf(lbl, (plt->xdp)+lr3+4, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->xdp)+4, "%f", lr1);
			}
			else g_snprintf(lbl, 2, "%f", 0.0);
			pango_layout_set_text(lyt, lbl, -1);
			pango_layout_get_pixel_size(lyt, &wd, &hg);
			cairo_move_to(cr, to-(wd/2), ya-JTI-hg);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_set_line_width(cr, 1);
			cairo_save(cr);
			cairo_set_dash(cr, &dt, 1, 0);
			cairo_move_to(cr, to, yu);
			cairo_line_to(cr, to, yl);
			cairo_stroke(cr);
			cairo_restore(cr);
			for (j=1; j<(prv->ticks.xj); j++)
			{
				tn=xl+(((xu-xl)*j)/(prv->ticks.xj));
				for (k=1; k<(prv->ticks.xn); k++)
				{
					tnn=to+(((tn-to)*k)/(prv->ticks.xn));
					cairo_move_to(cr, tnn, ya);
					cairo_line_to(cr, tnn, ya-NT);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, tn, ya);
				cairo_line_to(cr, tn, ya-JT);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.xmin)+(j*dlx);
				lr1=round(lr2*exp(G_LN10*(plt->xdp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, tn-(wd/2), ya-JTI-hg);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, tn, yu);
				cairo_line_to(cr, tn, yl);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
			for (k=1; k<(prv->ticks.xn); k++)
			{
				tnn=to+(((xu-to)*k)/(prv->ticks.xn));
				cairo_move_to(cr, tnn, ya);
				cairo_line_to(cr, tnn, ya-NT);
			}
			cairo_stroke(cr);
			cairo_set_line_width(cr, 2);
			cairo_move_to(cr, xu, ya);
			cairo_line_to(cr, xu, ya-JT);
			cairo_stroke(cr);
			cairo_set_line_width(cr, 1);
			cairo_save(cr);
			cairo_set_dash(cr, &dt, 1, 0);
			cairo_move_to(cr, xu, yu);
			cairo_line_to(cr, xu, yl);
			cairo_stroke(cr);
			cairo_restore(cr);
		}
		else
		{
			cairo_move_to(cr, (xu+xl-wd)/2, ya+dtt-hg);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_move_to(cr, to, ya);
			cairo_line_to(cr, to, ya+JT);
			cairo_stroke(cr);
			lyt=pango_cairo_create_layout(cr);
			pango_layout_set_font_description(lyt, (plt->afont));
			lr1=round((prv->bounds.xmin)*exp(G_LN10*(plt->xdp)));
			if ((prv->bounds.xmin)>DZE)
			{
				lr1+=0.1;
				lr1*=exp(-G_LN10*(plt->xdp));
				if (lr1>=1)
				{
					lr2=floor(log10(lr1));
					lr2=log10(lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->xdp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
					else g_snprintf(lbl, (plt->xdp)+lr3+3, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->xdp)+3, "%f", lr1);
			}
			else if ((prv->bounds.xmin)<NZE)
			{
				lr1-=0.1;
				lr1*=exp(-G_LN10*(plt->xdp));
				if (lr1<=-1)
				{
					lr2=log10(-lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->xdp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
					else g_snprintf(lbl, (plt->xdp)+lr3+4, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->xdp)+4, "%f", lr1);
			}
			else g_snprintf(lbl, 2, "%f", 0.0);
			pango_layout_set_text(lyt, lbl, -1);
			pango_layout_get_pixel_size(lyt, &wd, &hg);
			cairo_move_to(cr, to-(wd/2), ya+JTI);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_set_line_width(cr, 1);
			cairo_save(cr);
			cairo_set_dash(cr, &dt, 1, 0);
			cairo_move_to(cr, to, yu);
			cairo_line_to(cr, to, yl);
			cairo_stroke(cr);
			cairo_restore(cr);
			for (j=1; j<(prv->ticks.xj); j++)
			{
				tn=xl+(((xu-xl)*j)/(prv->ticks.xj));
				for (k=1; k<(prv->ticks.xn); k++)
				{
					tnn=to+(((tn-to)*k)/(prv->ticks.xn));
					cairo_move_to(cr, tnn, ya);
					cairo_line_to(cr, tnn, ya+NT);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, tn, ya);
				cairo_line_to(cr, tn, ya+JT);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.xmin)+(j*dlx);
				lr1=round(lr2*exp(G_LN10*(plt->xdp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, tn-(wd/2), ya+JTI);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, tn, yu);
				cairo_line_to(cr, tn, yl);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
			for (k=1; k<(prv->ticks.xn); k++)
			{
				tnn=to+(((xu-to)*k)/(prv->ticks.xn));
				cairo_move_to(cr, tnn, ya);
				cairo_line_to(cr, tnn, ya+NT);
			}
			cairo_stroke(cr);
			cairo_set_line_width(cr, 2);
			cairo_move_to(cr, xu, ya);
			cairo_line_to(cr, xu, ya+JT);
			cairo_stroke(cr);
			cairo_set_line_width(cr, 1);
			cairo_save(cr);
			cairo_set_dash(cr, &dt, 1, 0);
			cairo_move_to(cr, xu, yu);
			cairo_line_to(cr, xu, yl);
			cairo_stroke(cr);
			cairo_restore(cr);
		}
	}
	else if ((xu+xl)>=(2*xa))
	{
		to=xu;
		tz=((xu+1-xa)*(prv->ticks.xj))/(xu-xl);
		if (tz==0) tf=xu;
		else tf=xu-(((xu-xa)*(prv->ticks.xj))/tz);
		if (((prv->flaga)&PLOT_LINEAR_AXES_LT)!=0)
		{
			cairo_move_to(cr, (xu+xa-wd)/2, ya-dtt);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_move_to(cr, to, ya);
			cairo_line_to(cr, to, ya-JT);
			cairo_stroke(cr);
			cairo_set_line_width(cr, 1);
			cairo_save(cr);
			cairo_set_dash(cr, &dt,1, 0);
			cairo_move_to(cr, to, yu);
			cairo_line_to(cr, to, yl);
			cairo_stroke(cr);
			cairo_restore(cr);
			for (j=1; j<tz; j++)
			{
				tn=xu-(((xu-tf)*j)/(prv->ticks.xj));
				for (k=1; k<(prv->ticks.xn); k++)
				{
					tnn=to-(((to-tn)*k)/(prv->ticks.xn));
					cairo_move_to(cr, tnn, ya);
					cairo_line_to(cr, tnn, ya-NT);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, tn, ya);
				cairo_line_to(cr, tn, ya-JT);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.xmax)-(j*dlx*(xu-tf)/(xu-xl));
				lr1=round(lr2*exp(G_LN10*(plt->xdp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, tn-(wd/2), ya-JTI-hg);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, tn, yu);
				cairo_line_to(cr, tn, yl);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
			tn=xu-(((xu-tf)*tz)/(prv->ticks.xj));
			for (k=1; k<(prv->ticks.xn); k++)
			{
				tnn=to-(((to-tn)*k)/(prv->ticks.xn));
				cairo_move_to(cr, tnn, ya);
				cairo_line_to(cr, tnn, ya-NT);
			}
			cairo_stroke(cr);
			to=tn;
			for (j=(tz+1); j<(prv->ticks.xj); j++)
			{
				tn=xu-(((xu-tf)*j)/(prv->ticks.xj));
				for (k=1; k<(prv->ticks.xn); k++)
				{
					tnn=to-(((to-tn)*k)/(prv->ticks.xn));
					cairo_move_to(cr, tnn, ya);
					cairo_line_to(cr, tnn, ya-NT);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, tn, ya);
				cairo_line_to(cr, tn, ya-JT);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.xmax)-(j*dlx*(xu-tf)/(xu-xl));
				lr1=round(lr2*exp(G_LN10*(plt->xdp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, tn-(wd/2), ya-JTI-hg);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, tn, yu);
				cairo_line_to(cr, tn, yl);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
			tnn=to-(((to-tf)*k)/(prv->ticks.xn));
			for (k=1; (tnn>=xl)&&(k<=(prv->ticks.xn)); k++)
			{
				cairo_move_to(cr, tnn, ya);
				cairo_line_to(cr, tnn, ya-NT);
				tnn=to-(((to-tf)*k)/(prv->ticks.xn));
			}
		}
		else
		{
			cairo_move_to(cr, (xu+xa-wd)/2, ya+dtt-hg);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_move_to(cr, to, ya);
			cairo_line_to(cr, to, ya+JT);
			cairo_stroke(cr);
			cairo_set_line_width(cr, 1);
			cairo_save(cr);
			cairo_set_dash(cr, &dt, 1, 0);
			cairo_move_to(cr, to, yu);
			cairo_line_to(cr, to, yl);
			cairo_stroke(cr);
			cairo_restore(cr);
			for (j=1; j<tz; j++)
			{
				tn=xu-(((xu-tf)*j)/(prv->ticks.xj));
				for (k=1; k<(prv->ticks.xn); k++)
				{
					tnn=to-(((to-tn)*k)/(prv->ticks.xn));
					cairo_move_to(cr, tnn, ya);
					cairo_line_to(cr, tnn, ya+NT);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, tn, ya);
				cairo_line_to(cr, tn, ya+JT);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.xmax)-(j*dlx*(xu-tf)/(xu-xl));
				lr1=round(lr2*exp(G_LN10*(plt->xdp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, tn-(wd/2), ya+JTI);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, tn, yu);
				cairo_line_to(cr, tn, yl);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
			tn=xu-(((xu-tf)*tz)/(prv->ticks.xj));
			for (k=1; k<(prv->ticks.xn); k++)
			{
				tnn=to-(((to-tn)*k)/(prv->ticks.xn));
				cairo_move_to(cr, tnn, ya);
				cairo_line_to(cr, tnn, ya+NT);
			}
			cairo_stroke(cr);
			to=tn;
			for (j=(tz+1); j<(prv->ticks.xj); j++)
			{
				tn=xu-(((xu-tf)*j)/(prv->ticks.xj));
				for (k=1; k<(prv->ticks.xn); k++)
				{
					tnn=to-(((to-tn)*k)/(prv->ticks.xn));
					cairo_move_to(cr, tnn, ya);
					cairo_line_to(cr, tnn, ya+NT);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, tn, ya);
				cairo_line_to(cr, tn, ya+JT);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.xmax)-(j*dlx*(xu-tf)/(xu-xl));
				lr1=round(lr2*exp(G_LN10*(plt->xdp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, tn-(wd/2), ya+JTI);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, tn, yu);
				cairo_line_to(cr, tn, yl);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
			tnn=to-(((to-tf)*k)/(prv->ticks.xn));
			for (k=1; (tnn>=xl)&&(k<=(prv->ticks.xn)); k++)
			{
				cairo_move_to(cr, tnn, ya);
				cairo_line_to(cr, tnn, ya+NT);
				tnn=to-(((to-tf)*k)/(prv->ticks.xn));
			}
		}
	}
	else
	{
		to=xl;
		tz=((xa+1-xl)*(prv->ticks.xj))/(xu-xl);
		if (tz==0) tf=xl;
		else tf=xl+(((xa-xl)*(prv->ticks.xj))/tz);
		if (((prv->flaga)&PLOT_LINEAR_AXES_LT)!=0)
		{
			cairo_move_to(cr, (xa+xl-wd)/2, ya-dtt);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_move_to(cr, to, ya);
			cairo_line_to(cr, to, ya-JT);
			cairo_stroke(cr);
			lyt=pango_cairo_create_layout(cr);
			pango_layout_set_font_description(lyt, (plt->afont));
			lr1=round((prv->bounds.xmin)*exp(G_LN10*(plt->xdp)));
			if ((prv->bounds.xmin)>DZE)
			{
				lr1+=0.1;
				lr1*=exp(-G_LN10*(plt->xdp));
				if (lr1>=1)
				{
					lr2=floor(log10(lr1));
					lr2=log10(lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->xdp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
					else g_snprintf(lbl, (plt->xdp)+lr3+3, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->xdp)+3, "%f", lr1);
			}
			else if ((prv->bounds.xmin)<NZE)
			{
				lr1-=0.1;
				lr1*=exp(-G_LN10*(plt->xdp));
				if (lr1<=-1)
				{
					lr2=log10(-lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->xdp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
					else g_snprintf(lbl, (plt->xdp)+lr3+4, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->xdp)+4, "%f", lr1);
			}
			else g_snprintf(lbl, 2, "%f", 0.0);
			pango_layout_set_text(lyt, lbl, -1);
			pango_layout_get_pixel_size(lyt, &wd, &hg);
			cairo_move_to(cr, to-(wd/2), ya-JTI-hg);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_set_line_width(cr, 1);
			cairo_save(cr);
			cairo_set_dash(cr, &dt, 1, 0);
			cairo_move_to(cr, to, yu);
			cairo_line_to(cr, to, yl);
			cairo_stroke(cr);
			cairo_restore(cr);
			for (j=1; j<tz; j++)
			{
				tn=xl+(((tf-xl)*j)/(prv->ticks.xj));
				for (k=1; k<(prv->ticks.xn); k++)
				{
					tnn=to+(((tn-to)*k)/(prv->ticks.xn));
					cairo_move_to(cr, tnn, ya);
					cairo_line_to(cr, tnn, ya-NT);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, tn, ya);
				cairo_line_to(cr, tn, ya-JT);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.xmin)+(j*dlx*(tf-xl)/(xu-xl));
				lr1=round(lr2*exp(G_LN10*(plt->xdp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, tn-(wd/2), ya-JTI-hg);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, tn, yu);
				cairo_line_to(cr, tn, yl);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
			tn=xl+(((tf-xl)*tz)/(prv->ticks.xj));
			for (k=1; k<(prv->ticks.xn); k++)
			{
				tnn=to+(((tn-to)*k)/(prv->ticks.xn));
				cairo_move_to(cr, tnn, ya);
				cairo_line_to(cr, tnn, ya-NT);
			}
			cairo_stroke(cr);
			to=tn;
			for (j=(tz+1); j<(prv->ticks.xj); j++)
			{
				tn=xl+(((tf-xl)*j)/(prv->ticks.xj));
				for (k=1; k<(prv->ticks.xn); k++)
				{
					tnn=to+(((tn-to)*k)/(prv->ticks.xn));
					cairo_move_to(cr, tnn, ya);
					cairo_line_to(cr, tnn, ya-NT);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, tn, ya);
				cairo_line_to(cr, tn, ya-JT);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.xmin)+(j*dlx*(tf-xl)/(xu-xl));
				lr1=round(lr2*exp(G_LN10*(plt->xdp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, tn-(wd/2), ya-JTI-hg);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, tn, yu);
				cairo_line_to(cr, tn, yl);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
			tnn=to+(((tf-to)*k)/(prv->ticks.xn));
			for (k=1; (tnn<=xu)&&(k<=(prv->ticks.xn)); k++)
			{
				cairo_move_to(cr, tnn, ya);
				cairo_line_to(cr, tnn, ya-NT);
				tnn=to+(((tf-to)*k)/(prv->ticks.xn));
			}
		}
		else
		{
			cairo_move_to(cr, (xa+xl-wd)/2, ya+dtt-hg);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_move_to(cr, to, ya);
			cairo_line_to(cr, to, ya+JT);
			cairo_stroke(cr);
			lyt=pango_cairo_create_layout(cr);
			pango_layout_set_font_description(lyt, (plt->afont));
			lr1=round((prv->bounds.xmin)*exp(G_LN10*(plt->xdp)));
			if ((prv->bounds.xmin)>DZE)
			{
				lr1+=0.1;
				lr1*=exp(-G_LN10*(plt->xdp));
				if (lr1>=1)
				{
					lr2=floor(log10(lr1));
					lr2=log10(lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->xdp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
					else g_snprintf(lbl, (plt->xdp)+lr3+3, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->xdp)+3, "%f", lr1);
			}
			else if ((prv->bounds.xmin)<NZE)
			{
				lr1-=0.1;
				lr1*=exp(-G_LN10*(plt->xdp));
				if (lr1<=-1)
				{
					lr2=log10(-lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->xdp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
					else g_snprintf(lbl, (plt->xdp)+lr3+4, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->xdp)+4, "%f", lr1);
			}
			else g_snprintf(lbl, 2, "%f", 0.0);
			pango_layout_set_text(lyt, lbl, -1);
			pango_layout_get_pixel_size(lyt, &wd, &hg);
			cairo_move_to(cr, to-(wd/2), ya+JTI);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_set_line_width(cr, 1);
			cairo_save(cr);
			cairo_set_dash(cr, &dt, 1, 0);
			cairo_move_to(cr, to, yu);
			cairo_line_to(cr, to, yl);
			cairo_stroke(cr);
			cairo_restore(cr);
			for (j=1; j<tz; j++)
			{
				tn=xl+(((tf-xl)*j)/(prv->ticks.xj));
				for (k=1; k<(prv->ticks.xn); k++)
				{
					tnn=to+(((tn-to)*k)/(prv->ticks.xn));
					cairo_move_to(cr, tnn, ya);
					cairo_line_to(cr, tnn, ya+NT);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, tn, ya);
				cairo_line_to(cr, tn, ya+JT);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.xmin)+(j*dlx*(tf-xl)/(xu-xl));
				lr1=round(lr2*exp(G_LN10*(plt->xdp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, tn-(wd/2), ya+JTI);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, tn, yu);
				cairo_line_to(cr, tn, yl);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
			tn=xl+(((tf-xl)*tz)/(prv->ticks.xj));
			for (k=1; k<(prv->ticks.xn); k++)
			{
				tnn=to+(((tn-to)*k)/(prv->ticks.xn));
				cairo_move_to(cr, tnn, ya);
				cairo_line_to(cr, tnn, ya+NT);
			}
			cairo_stroke(cr);
			to=tn;
			for (j=(tz+1); j<(prv->ticks.xj); j++)
			{
				tn=xl+(((tf-xl)*j)/(prv->ticks.xj));
				for (k=1; k<(prv->ticks.xn); k++)
				{
					tnn=to+(((tn-to)*k)/(prv->ticks.xn));
					cairo_move_to(cr, tnn, ya);
					cairo_line_to(cr, tnn, ya+NT);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, tn, ya);
				cairo_line_to(cr, tn, ya+JT);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.xmin)+(j*dlx*(tf-xl)/(xu-xl));
				lr1=round(lr2*exp(G_LN10*(plt->xdp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->xdp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->xdp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->xdp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->xdp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, tn-(wd/2), ya+JTI);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, tn, yu);
				cairo_line_to(cr, tn, yl);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
			tnn=to+(((tf-to)*k)/(prv->ticks.xn));
			for (k=1; (tnn<=xu)&&(k<=(prv->ticks.xn)); k++)
			{
				cairo_move_to(cr, tnn, ya);
				cairo_line_to(cr, tnn, ya+NT);
				tnn=to+(((tf-to)*k)/(prv->ticks.xn));
			}
		}
	}
	cairo_stroke(cr);
	lyt=pango_cairo_create_layout(cr);
	pango_layout_set_font_description(lyt, (plt->lfont));
	pango_layout_set_text(lyt, (plt->ylab), -1);
	pango_layout_get_pixel_size(lyt, &wd, &hg);
	if (((prv->flaga)&PLOT_LINEAR_AXES_DW)!=0)
	{
		to=yu;
		if (((prv->flaga)&PLOT_LINEAR_AXES_LR)!=0)
		{
			cairo_move_to(cr, xa+dtt, (yl+yu-wd)/2);
			cairo_set_matrix(cr, &mt3);
			pango_cairo_update_layout(cr, lyt);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_identity_matrix(cr);
			lyt=pango_cairo_create_layout(cr);
			pango_layout_set_font_description(lyt, (plt->afont));
			lr1=round((prv->bounds.ymax)*exp(G_LN10*(plt->ydp)));
			if ((prv->bounds.ymax)>DZE)
			{
				lr1+=0.1;
				lr1*=exp(-G_LN10*(plt->ydp));
				if (lr1>=1)
				{
					lr2=floor(log10(lr1));
					lr2=log10(lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->ydp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
					else g_snprintf(lbl, (plt->ydp)+lr3+3, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->ydp)+3, "%f", lr1);
			}
			else if ((prv->bounds.ymax)<NZE)
			{
				lr1-=0.1;
				lr1*=exp(-G_LN10*(plt->ydp));
				if (lr1<=-1)
				{
					lr2=log10(-lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->ydp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
					else g_snprintf(lbl, (plt->ydp)+lr3+4, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->ydp)+4, "%f", lr1);
			}
			else g_snprintf(lbl, 2, "%f", 0.0);
			pango_layout_set_text(lyt, lbl, -1);
			pango_layout_get_pixel_size(lyt, &wd, &hg);
			cairo_move_to(cr, xa+JTI+hg, to-(wd/2));
			cairo_set_matrix(cr, &mt3);
			pango_cairo_update_layout(cr, lyt);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_identity_matrix(cr);
			cairo_move_to(cr, xa, to);
			cairo_line_to(cr, xa+JT, to);
			cairo_stroke(cr);
			cairo_set_line_width(cr, 1);
			cairo_save(cr);
			cairo_set_dash(cr, &dt, 1, 0);
			cairo_move_to(cr, xl, to);
			cairo_line_to(cr, xu, to);
			cairo_stroke(cr);
			cairo_restore(cr);
			for (j=1; j<=(prv->ticks.yj); j++)
			{
				tn=yu+(((yl-yu)*j)/(prv->ticks.yj));
				for (k=1; k<(prv->ticks.yn); k++)
				{
					tnn=to+(((tn-to)*k)/(prv->ticks.yn));
					cairo_move_to(cr, xa, tnn);
					cairo_line_to(cr, xa+NT, tnn);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, xa, tn);
				cairo_line_to(cr, xa+JT, tn);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.ymax)-(j*dly);
				lr1=round(lr2*exp(G_LN10*(plt->ydp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, xa+JTI+hg, tn-(wd/2));
				cairo_set_matrix(cr, &mt3);
				pango_cairo_update_layout(cr, lyt);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_identity_matrix(cr);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, xl, tn);
				cairo_line_to(cr, xu, tn);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
		}
		else
		{
			cairo_move_to(cr, xa-dtt, (yl+yu+wd)/2);
			cairo_set_matrix(cr, &mt2);
			pango_cairo_update_layout(cr, lyt);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_identity_matrix(cr);
			lyt=pango_cairo_create_layout(cr);
			pango_layout_set_font_description(lyt, (plt->afont));
			lr1=round((prv->bounds.ymax)*exp(G_LN10*(plt->ydp)));
			if ((prv->bounds.ymax)>DZE)
			{
				lr1+=0.1;
				lr1*=exp(-G_LN10*(plt->ydp));
				if (lr1>=1)
				{
					lr2=floor(log10(lr1));
					lr2=log10(lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->ydp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
					else g_snprintf(lbl, (plt->ydp)+lr3+3, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->ydp)+3, "%f", lr1);
			}
			else if ((prv->bounds.ymax)<NZE)
			{
				lr1-=0.1;
				lr1*=exp(-G_LN10*(plt->ydp));
				if (lr1<=-1)
				{
					lr2=log10(-lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->ydp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
					else g_snprintf(lbl, (plt->ydp)+lr3+4, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->ydp)+4, "%f", lr1);
			}
			else g_snprintf(lbl, 2, "%f", 0.0);
			pango_layout_set_text(lyt, lbl, -1);
			pango_layout_get_pixel_size(lyt, &wd, &hg);
			cairo_move_to(cr, xa-JTI-hg, to+(wd/2));
			cairo_set_matrix(cr, &mt2);
			pango_cairo_update_layout(cr, lyt);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_identity_matrix(cr);
			cairo_move_to(cr, xa, to);
			cairo_line_to(cr, xa-JT, to);
			cairo_stroke(cr);
			cairo_set_line_width(cr, 1);
			cairo_save(cr);
			cairo_set_dash(cr, &dt, 1, 0);
			cairo_move_to(cr, xl, to);
			cairo_line_to(cr, xu, to);
			cairo_stroke(cr);
			cairo_restore(cr);
			for (j=1; j<=(prv->ticks.yj); j++)
			{
				tn=yu+(((yl-yu)*j)/(prv->ticks.yj));
				for (k=1; k<(prv->ticks.yn); k++)
				{
					tnn=to+(((tn-to)*k)/(prv->ticks.yn));
					cairo_move_to(cr, xa, tnn);
					cairo_line_to(cr, xa-NT, tnn);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, xa, tn);
				cairo_line_to(cr, xa-JT, tn);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.ymax)-(j*dly);
				lr1=round(lr2*exp(G_LN10*(plt->ydp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, xa-JTI-hg, tn+(wd/2));
				cairo_set_matrix(cr, &mt2);
				pango_cairo_update_layout(cr, lyt);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_identity_matrix(cr);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, xl, tn);
				cairo_line_to(cr, xu, tn);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
		}
	}
	else if (((prv->flaga)&PLOT_LINEAR_AXES_UW)!=0)
	{
		to=yu;
		if (((prv->flaga)&PLOT_LINEAR_AXES_LR)!=0)
		{
			cairo_move_to(cr, xa+dtt, (yl+yu-wd)/2);
			cairo_set_matrix(cr, &mt3);
			pango_cairo_update_layout(cr, lyt);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_identity_matrix(cr);
			cairo_move_to(cr, xa, to);
			cairo_line_to(cr, xa+JT, to);
			cairo_stroke(cr);
			cairo_set_line_width(cr, 1);
			cairo_save(cr);
			cairo_set_dash(cr, &dt, 1, 0);
			cairo_move_to(cr, xl, to);
			cairo_line_to(cr, xu, to);
			cairo_stroke(cr);
			cairo_restore(cr);
			for (j=1; j<=(prv->ticks.yj); j++)
			{
				tn=yu+(((yl-yu)*j)/(prv->ticks.yj));
				for (k=1; k<(prv->ticks.yn); k++)
				{
					tnn=to+(((tn-to)*k)/(prv->ticks.yn));
					cairo_move_to(cr, xa, tnn);
					cairo_line_to(cr, xa+NT, tnn);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, xa, tn);
				cairo_line_to(cr, xa+JT, tn);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.ymax)-(j*dly);
				lr1=round(lr2*exp(G_LN10*(plt->ydp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, xa+JTI+hg, tn-(wd/2));
				cairo_set_matrix(cr, &mt3);
				pango_cairo_update_layout(cr, lyt);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_identity_matrix(cr);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, xl, tn);
				cairo_line_to(cr, xu, tn);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
		}
		else
		{
			cairo_move_to(cr, xa-dtt, (yl+yu+wd)/2);
			cairo_set_matrix(cr, &mt2);
			pango_cairo_update_layout(cr, lyt);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_identity_matrix(cr);
			cairo_move_to(cr, xa, to);
			cairo_line_to(cr, xa-JT, to);
			cairo_stroke(cr);
			cairo_set_line_width(cr, 1);
			cairo_save(cr);
			cairo_set_dash(cr, &dt, 1, 0);
			cairo_move_to(cr, xl, to);
			cairo_line_to(cr, xu, to);
			cairo_stroke(cr);
			cairo_restore(cr);
			for (j=1; j<=(prv->ticks.yj); j++)
			{
				tn=yu+(((yl-yu)*j)/(prv->ticks.yj));
				for (k=1; k<(prv->ticks.yn); k++)
				{
					tnn=to+(((tn-to)*k)/(prv->ticks.yn));
					cairo_move_to(cr, xa, tnn);
					cairo_line_to(cr, xa-NT, tnn);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, xa, tn);
				cairo_line_to(cr, xa-JT, tn);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.ymax)-(j*dly);
				lr1=round(lr2*exp(G_LN10*(plt->ydp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, xa-JTI-hg, tn+(wd/2));
				cairo_set_matrix(cr, &mt2);
				pango_cairo_update_layout(cr, lyt);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_identity_matrix(cr);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, xl, tn);
				cairo_line_to(cr, xu, tn);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
		}
	}
	else if ((yu+yl)<=(2*ya))
	{
		to=yu;
		tz=((ya+1-yu)*(prv->ticks.yj))/(yl-yu);
		if (tz==0) tf=yu;
		else tf=yu+(((ya-yu)*(prv->ticks.yj))/tz);
		if (((prv->flaga)&PLOT_LINEAR_AXES_LR)!=0)
		{
			cairo_move_to(cr, xa+dtt, (ya+yu-wd)/2);
			cairo_set_matrix(cr, &mt3);
			pango_cairo_update_layout(cr, lyt);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_identity_matrix(cr);
			cairo_move_to(cr, xa, to);
			cairo_line_to(cr, xa+JT, to);
			cairo_stroke(cr);
			cairo_set_line_width(cr, 1);
			cairo_save(cr);
			cairo_set_dash(cr, &dt, 1, 0);
			cairo_move_to(cr, xl, to);
			cairo_line_to(cr, xu, to);
			cairo_stroke(cr);
			cairo_restore(cr);
			for (j=1; j<tz; j++)
			{
				tn=yu+(((yl-yu)*j)/(prv->ticks.yj));
				for (k=1; k<(prv->ticks.yn); k++)
				{
					tnn=to+(((tn-to)*k)/(prv->ticks.yn));
					cairo_move_to(cr, xa, tnn);
					cairo_line_to(cr, xa+NT, tnn);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, xa, tn);
				cairo_line_to(cr, xa+JT, tn);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.ymax)-(j*dly*(tf-yu)/(yl-yu));
				lr1=round(lr2*exp(G_LN10*(plt->ydp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, xa+JTI+hg, tn-(wd/2));
				cairo_set_matrix(cr, &mt3);
				pango_cairo_update_layout(cr, lyt);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_identity_matrix(cr);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, xl, tn);
				cairo_line_to(cr, xu, tn);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
			tn=yu+(((yl-yu)*tz)/(prv->ticks.yj));
			for (k=1; k<(prv->ticks.yn); k++)
			{
				tnn=to+(((tn-to)*k)/(prv->ticks.yn));
				cairo_move_to(cr, xa, tnn);
				cairo_line_to(cr, xa+NT, tnn);
			}
			cairo_stroke(cr);
			to=tn;
			for (j=(tz+1); j<(prv->ticks.yj); j++)
			{
				tn=yu+(((yl-yu)*j)/(prv->ticks.yj));
				for (k=1; k<(prv->ticks.yn); k++)
				{
					tnn=to+(((tn-to)*k)/(prv->ticks.yn));
					cairo_move_to(cr, xa, tnn);
					cairo_line_to(cr, xa+NT, tnn);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, xa, tn);
				cairo_line_to(cr, xa+JT, tn);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.ymax)-(j*dly*(tf-yu)/(yl-yu));
				lr1=round(lr2*exp(G_LN10*(plt->ydp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, xa+JTI+hg, tn-(wd/2));
				cairo_set_matrix(cr, &mt3);
				pango_cairo_update_layout(cr, lyt);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_identity_matrix(cr);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, xl, tn);
				cairo_line_to(cr, xu, tn);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
			tnn=to+(((tf-to)*k)/(prv->ticks.yn));
			for (k=1; (tnn<=yl)&&(k<=(prv->ticks.yn)); k++)
			{
				cairo_move_to(cr, xa, tnn);
				cairo_line_to(cr, xa+NT, tnn);
				tnn=to+(((tf-to)*k)/(prv->ticks.yn));
			}
		}
		else
		{
			cairo_move_to(cr, xa-dtt, (ya+yu+wd)/2);
			cairo_set_matrix(cr, &mt2);
			pango_cairo_update_layout(cr, lyt);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_identity_matrix(cr);
			cairo_move_to(cr, xa, to);
			cairo_line_to(cr, xa-JT, to);
			cairo_stroke(cr);
			cairo_set_line_width(cr, 1);
			cairo_save(cr);
			cairo_set_dash(cr, &dt, 1, 0);
			cairo_move_to(cr, xl, to);
			cairo_line_to(cr, xu, to);
			cairo_stroke(cr);
			cairo_restore(cr);
			for (j=1; j<tz; j++)
			{
				tn=yu+(((yl-yu)*j)/(prv->ticks.yj));
				for (k=1; k<(prv->ticks.yn); k++)
				{
					tnn=to+(((tn-to)*k)/(prv->ticks.yn));
					cairo_move_to(cr, xa, tnn);
					cairo_line_to(cr, xa-NT, tnn);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, xa, tn);
				cairo_line_to(cr, xa-JT, tn);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.ymax)-(j*dly*(tf-yu)/(yl-yu));
				lr1=round(lr2*exp(G_LN10*(plt->ydp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, xa-JTI-hg, tn+(wd/2));
				cairo_set_matrix(cr, &mt2);
				pango_cairo_update_layout(cr, lyt);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_identity_matrix(cr);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, xl, tn);
				cairo_line_to(cr, xu, tn);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
			tn=yu+(((yl-yu)*tz)/(prv->ticks.yj)); /*SIGFPE here?*/
			for (k=1; k<(prv->ticks.yn); k++)
			{
				tnn=to+(((tn-to)*k)/(prv->ticks.yn));
				cairo_move_to(cr, xa, tnn);
				cairo_line_to(cr, xa-NT, tnn);
			}
			cairo_stroke(cr);
			to=tn;
			for (j=(tz+1); j<(prv->ticks.yj); j++)
			{
				tn=yu+(((yl-yu)*j)/(prv->ticks.yj));
				for (k=1; k<(prv->ticks.yn); k++)
				{
					tnn=to+(((tn-to)*k)/(prv->ticks.yn));
					cairo_move_to(cr, xa, tnn);
					cairo_line_to(cr, xa-NT, tnn);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, xa, tn);
				cairo_line_to(cr, xa-JT, tn);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.ymax)-(j*dly*(tf-yu)/(yl-yu));
				lr1=round(lr2*exp(G_LN10*(plt->ydp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, xa-JTI-hg, tn+(wd/2));
				cairo_set_matrix(cr, &mt2);
				pango_cairo_update_layout(cr, lyt);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_identity_matrix(cr);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, xl, tn);
				cairo_line_to(cr, xu, tn);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
			tnn=to+(((tf-to)*k)/(prv->ticks.yn));
			for (k=1; (tnn<=yl)&&(k<=(prv->ticks.yn)); k++)
			{
				cairo_move_to(cr, xa, tnn);
				cairo_line_to(cr, xa-NT, tnn);
				tnn=to+(((tf-to)*k)/(prv->ticks.yn));
			}
		}
	}
	else
	{
		to=yl;
		tz=((yl+1-ya)*(prv->ticks.yj))/(yl-yu);
		if (tz==0) tf=yl;
		else tf=yl-(((yl-ya)*(prv->ticks.yj))/tz);
		if (((prv->flaga)&PLOT_LINEAR_AXES_LR)!=0)
		{
			cairo_move_to(cr, xa+dtt, (yl+ya-wd)/2);
			cairo_set_matrix(cr, &mt3);
			pango_cairo_update_layout(cr, lyt);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_identity_matrix(cr);
			lyt=pango_cairo_create_layout(cr);
			pango_layout_set_font_description(lyt, (plt->afont));
			lr1=round((prv->bounds.ymin)*exp(G_LN10*(plt->ydp)));
			if ((prv->bounds.ymin)>DZE)
			{
				lr1+=0.1;
				lr1*=exp(-G_LN10*(plt->ydp));
				if (lr1>=1)
				{
					lr2=floor(log10(lr1));
					lr2=log10(lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->ydp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
					else g_snprintf(lbl, (plt->ydp)+lr3+3, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->ydp)+3, "%f", lr1);
			}
			else if ((prv->bounds.ymin)<NZE)
			{
				lr1-=0.1;
				lr1*=exp(-G_LN10*(plt->ydp));
				if (lr1<=-1)
				{
					lr2=log10(-lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->ydp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
					else g_snprintf(lbl, (plt->ydp)+lr3+4, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->ydp)+4, "%f", lr1);
			}
			else g_snprintf(lbl, 2, "%f", 0.0);
			pango_layout_set_text(lyt, lbl, -1);
			pango_layout_get_pixel_size(lyt, &wd, &hg);
			cairo_move_to(cr, xa+JTI+hg, to-(wd/2));
			cairo_set_matrix(cr, &mt3);
			pango_cairo_update_layout(cr, lyt);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_identity_matrix(cr);
			cairo_move_to(cr, xa, to);
			cairo_line_to(cr, xa+JT, to);
			cairo_stroke(cr);
			cairo_set_line_width(cr, 1);
			cairo_save(cr);
			cairo_set_dash(cr, &dt, 1, 0);
			cairo_move_to(cr, xl, to);
			cairo_line_to(cr, xu, to);
			cairo_stroke(cr);
			cairo_restore(cr);
			for (j=1; j<tz; j++)
			{
				tn=yl-(((yl-yu)*j)/(prv->ticks.yj));
				for (k=1; k<(prv->ticks.yn); k++)
				{
					tnn=to-(((to-tn)*k)/(prv->ticks.yn));
					cairo_move_to(cr, xa, tnn);
					cairo_line_to(cr, xa+NT, tnn);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, xa, tn);
				cairo_line_to(cr, xa+JT, tn);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.ymin)+(j*dly*(yl-tf)/(yl-yu));
				lr1=round(lr2*exp(G_LN10*(plt->ydp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, xa+JTI+hg, tn-(wd/2));
				cairo_set_matrix(cr, &mt3);
				pango_cairo_update_layout(cr, lyt);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_identity_matrix(cr);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, xl, tn);
				cairo_line_to(cr, xu, tn);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
			tn=yl-(((yl-yu)*tz)/(prv->ticks.yj));
			for (k=1; k<(prv->ticks.yn); k++)
			{
				tnn=to-(((to-tn)*k)/(prv->ticks.yn));
				cairo_move_to(cr, xa, tnn);
				cairo_line_to(cr, xa+NT, tnn);
			}
			cairo_stroke(cr);
			to=tn;
			for (j=(tz+1); j<(prv->ticks.yj); j++)
			{
				tn=yl-(((yl-yu)*j)/(prv->ticks.yj));
				for (k=1; k<(prv->ticks.yn); k++)
				{
					tnn=to-(((to-tn)*k)/(prv->ticks.yn));
					cairo_move_to(cr, xa, tnn);
					cairo_line_to(cr, xa+NT, tnn);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, xa, tn);
				cairo_line_to(cr, xa+JT, tn);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.ymin)+(j*dly*(yl-tf)/(yl-yu));
				lr1=round(lr2*exp(G_LN10*(plt->ydp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, xa+JTI+hg, tn-(wd/2));
				cairo_set_matrix(cr, &mt3);
				pango_cairo_update_layout(cr, lyt);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_identity_matrix(cr);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, xl, tn);
				cairo_line_to(cr, xu, tn);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
			tnn=to-(((to-tf)*k)/(prv->ticks.yn));
			for (k=1; (tnn>=yu)&&(k<=(prv->ticks.yn)); k++)
			{
				cairo_move_to(cr, xa, tnn);
				cairo_line_to(cr, xa+NT, tnn);
				tnn=to-(((to-tf)*k)/(prv->ticks.yn));
			}
		}
		else
		{
			cairo_move_to(cr, xa-dtt, (yl+ya+wd)/2);
			cairo_set_matrix(cr, &mt2);
			pango_cairo_update_layout(cr, lyt);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_identity_matrix(cr);
			lyt=pango_cairo_create_layout(cr);
			pango_layout_set_font_description(lyt, (plt->afont));
			lr1=round((prv->bounds.ymin)*exp(G_LN10*(plt->ydp)));
			if ((prv->bounds.ymin)>DZE)
			{
				lr1+=0.1;
				lr1*=exp(-G_LN10*(plt->ydp));
				if (lr1>=1)
				{
					lr2=floor(log10(lr1));
					lr2=log10(lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->ydp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
					else g_snprintf(lbl, (plt->ydp)+lr3+3, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->ydp)+3, "%f", lr1);
			}
			else if ((prv->bounds.ymin)<NZE)
			{
				lr1-=0.1;
				lr1*=exp(-G_LN10*(plt->ydp));
				if (lr1<=-1)
				{
					lr2=log10(-lr1);
					if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
					else lr3=(guint)ceil(lr2);
					if ((plt->ydp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
					else g_snprintf(lbl, (plt->ydp)+lr3+4, "%f", lr1);
				}
				else g_snprintf(lbl, (plt->ydp)+4, "%f", lr1);
			}
			else g_snprintf(lbl, 2, "%f", 0.0);
			pango_layout_set_text(lyt, lbl, -1);
			pango_layout_get_pixel_size(lyt, &wd, &hg);
			cairo_move_to(cr, xa-JTI-hg, to+(wd/2));
			cairo_set_matrix(cr, &mt2);
			pango_cairo_update_layout(cr, lyt);
			pango_cairo_show_layout(cr, lyt);
			g_object_unref(lyt);
			cairo_identity_matrix(cr);
			cairo_move_to(cr, xa, to);
			cairo_line_to(cr, xa-JT, to);
			cairo_stroke(cr);
			cairo_set_line_width(cr, 1);
			cairo_save(cr);
			cairo_set_dash(cr, &dt, 1, 0);
			cairo_move_to(cr, xl, to);
			cairo_line_to(cr, xu, to);
			cairo_stroke(cr);
			cairo_restore(cr);
			for (j=1; j<tz; j++)
			{
				tn=yl-(((yl-yu)*j)/(prv->ticks.yj));
				for (k=1; k<(prv->ticks.yn); k++)
				{
					tnn=to-(((to-tn)*k)/(prv->ticks.yn));
					cairo_move_to(cr, xa, tnn);
					cairo_line_to(cr, xa-NT, tnn);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, xa, tn);
				cairo_line_to(cr, xa-JT, tn);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.ymin)+(j*dly*(yl-tf)/(yl-yu));
				lr1=round(lr2*exp(G_LN10*(plt->ydp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, xa-JTI-hg, tn-(wd/2));
				cairo_set_matrix(cr, &mt2);
				pango_cairo_update_layout(cr, lyt);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_identity_matrix(cr);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, xl, tn);
				cairo_line_to(cr, xu, tn);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
			tn=yl-(((yl-yu)*tz)/(prv->ticks.yj));
			for (k=1; k<(prv->ticks.yn); k++)
			{
				tnn=to-(((to-tn)*k)/(prv->ticks.yn));
				cairo_move_to(cr, xa, tnn);
				cairo_line_to(cr, xa-NT, tnn);
			}
			cairo_stroke(cr);
			to=tn;
			for (j=(tz+1); j<(prv->ticks.yj); j++)
			{
				tn=yl-(((yl-yu)*j)/(prv->ticks.yj));
				for (k=1; k<(prv->ticks.yn); k++)
				{
					tnn=to-(((to-tn)*k)/(prv->ticks.yn));
					cairo_move_to(cr, xa, tnn);
					cairo_line_to(cr, xa-NT, tnn);
				}
				cairo_stroke(cr);
				cairo_set_line_width(cr, 2);
				cairo_move_to(cr, xa, tn);
				cairo_line_to(cr, xa-JT, tn);
				cairo_stroke(cr);
				lyt=pango_cairo_create_layout(cr);
				pango_layout_set_font_description(lyt, (plt->afont));
				lr2=(prv->bounds.ymin)+(j*dly*(yl-tf)/(yl-yu));
				lr1=round(lr2*exp(G_LN10*(plt->ydp)));
				if (lr2>DZE)
				{
					lr1+=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1>=1)
					{
						lr2=floor(log10(lr1));
						lr2=log10(lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+2, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+3, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+3, "%f", lr1);
				}
				else if (lr2<NZE)
				{
					lr1-=0.1;
					lr1*=exp(-G_LN10*(plt->ydp));
					if (lr1<=-1)
					{
						lr2=log10(-lr1);
						if (fmod(lr2,1)<NAC) lr3=(guint)lr2;
						else lr3=(guint)ceil(lr2);
						if ((plt->ydp)==0) g_snprintf(lbl, lr3+3, "%f", lr1);
						else g_snprintf(lbl, (plt->ydp)+lr3+4, "%f", lr1);
					}
					else g_snprintf(lbl, (plt->ydp)+4, "%f", lr1);
				}
				else g_snprintf(lbl, 2, "%f", 0.0);
				pango_layout_set_text(lyt, lbl, -1);
				pango_layout_get_pixel_size(lyt, &wd, &hg);
				cairo_move_to(cr, xa-JTI-hg, tn-(wd/2));
				cairo_set_matrix(cr, &mt2);
				pango_cairo_update_layout(cr, lyt);
				pango_cairo_show_layout(cr, lyt);
				g_object_unref(lyt);
				cairo_identity_matrix(cr);
				cairo_set_line_width(cr, 1);
				cairo_save(cr);
				cairo_set_dash(cr, &dt, 1, 0);
				cairo_move_to(cr, xl, tn);
				cairo_line_to(cr, xu, tn);
				cairo_stroke(cr);
				cairo_restore(cr);
				to=tn;
			}
			tnn=to-(((to-tf)*k)/(prv->ticks.yn));
			for (k=1; (tnn>=yu)&&(k<=(prv->ticks.yn)); k++)
			{
				cairo_move_to(cr, xa, tnn);
				cairo_line_to(cr, xa-NT, tnn);
				tnn=to-(((to-tf)*k)/(prv->ticks.yn));
			}
		}
	}
	cairo_stroke(cr);
	if (((plt->flagd)&PLOT_LINEAR_DISP_LIN)!=0)
	{
		cairo_set_line_width(cr, (plt->linew));
		if (((plt->flagd)&PLOT_LINEAR_DISP_PTS)!=0) /* lines and points */
		{
			for (k=0; k<(plt->ind->len); k++)
			{
				lt=g_array_index((plt->sizes), gint, k);
				if (lt>0)
				{
					ft=g_array_index((plt->ind), gint, k);
					lt+=ft;
					dtt=fmod(k,(plt->rd->len));
					vv=g_array_index((plt->rd), gdouble, dtt);
					wv=g_array_index((plt->gr), gdouble, dtt);
					zv=g_array_index((plt->bl), gdouble, dtt);
					av=g_array_index((plt->al), gdouble, dtt);
					cairo_set_source_rgba(cr, vv, wv, zv, av);
					xv=xl+((xu-xl)*(g_array_index((plt->xdata), gdouble, ft)-(prv->bounds.xmin))/((prv->bounds.xmax)-(prv->bounds.xmin)));
					yv=yl+((yu-yl)*(g_array_index((plt->ydata), gdouble, ft)-(prv->bounds.ymin))/((prv->bounds.ymax)-(prv->bounds.ymin)));
					if (xv<xl)
					{
						if (yv>yl) xt=(PLOT_LINEAR_BORDERS_LT|PLOT_LINEAR_BORDERS_DN);
						else if (yv<yu) xt=(PLOT_LINEAR_BORDERS_LT|PLOT_LINEAR_BORDERS_UP);
						else xt=PLOT_LINEAR_BORDERS_LT;
					}
					else if (xv>xu)
					{
						if (yv>yl) xt=(PLOT_LINEAR_BORDERS_RT|PLOT_LINEAR_BORDERS_DN);
						else if (yv<yu) xt=(PLOT_LINEAR_BORDERS_RT|PLOT_LINEAR_BORDERS_DN);
						else xt=PLOT_LINEAR_BORDERS_RT;
					}
					else if (yv>yl)xt=PLOT_LINEAR_BORDERS_DN;
					else if (yv<yu) xt=PLOT_LINEAR_BORDERS_UP;
					else
					{
						xt=0;
						cairo_arc(cr, xv, yv, (plt->ptsize), 0, MY_2PI);
						cairo_fill(cr);
						cairo_move_to(cr, xv, yv);
					}
					for (j=1+ft; j<lt; j++)
					{
						xvn=xl+((xu-xl)*(g_array_index((plt->xdata), gdouble, j)-(prv->bounds.xmin))/((prv->bounds.xmax)-(prv->bounds.xmin)));
						yvn=yl+((yu-yl)*(g_array_index((plt->ydata), gdouble, j)-(prv->bounds.ymin))/((prv->bounds.ymax)-(prv->bounds.ymin)));
						if (xvn<xl)
						{
							if (yvn>yl)
							{
								if (xt==0)
								{
									tx=((yvn-yv)*(xl-xvn))/(xvn-xv);
									tx+=yvn;
									if (tx>yl)
									{
										tx=((xvn-xv)*(yl-yvn))/(yvn-yv);
										tx+=xvn;
										cairo_line_to(cr, tx, yl);
									}
									else cairo_line_to(cr, xl, tx);
									cairo_stroke(cr);
								}
								xt=(PLOT_LINEAR_BORDERS_LT|PLOT_LINEAR_BORDERS_DN);
							}
							else if (yvn<yu)
							{
								if (xt==0)
								{
									tx=((yvn-yv)*(xl-xvn))/(xvn-xv);
									tx+=yvn;
									if (tx<yu)
									{
										tx=((xvn-xv)*(yu-yvn))/(yvn-yv);
										tx+=xvn;
										cairo_line_to(cr, tx, yu);
									}
									else cairo_line_to(cr, xl, tx);
									cairo_stroke(cr);
								}
								xt=(PLOT_LINEAR_BORDERS_LT|PLOT_LINEAR_BORDERS_UP);
							}
							else if (xt==0)
							{
								tx=((yvn-yv)*(xl-xvn))/(xvn-xv);
								tx+=yvn;
								cairo_line_to(cr, xl, tx);
								cairo_stroke(cr);
								xt=PLOT_LINEAR_BORDERS_LT;
							}
							else if (xt==PLOT_LINEAR_BORDERS_DN)
							{
								tx=((yvn-yv)*(xl-xvn))/(xvn-xv);
								tx+=yvn;
								if (tx>yl)
								{
									cairo_move_to(cr, xl, tx);
									tx=((xvn-xv)*(yl-yv))/(yvn-yv);
									tx+=xv;
									cairo_line_to(cr, tx, yl);
									cairo_stroke(cr);
								}
								xt=PLOT_LINEAR_BORDERS_LT;
							}
							else if (xt==PLOT_LINEAR_BORDERS_UP)
							{
								tx=((yvn-yv)*(xl-xvn))/(xvn-xv);
								tx+=yvn;
								if (tx<yu)
								{
									cairo_move_to(cr, xl, tx);
									tx=((xvn-xv)*(yu-yv))/(yvn-yv);
									tx+=xv;
									cairo_line_to(cr, tx, yu);
									cairo_stroke(cr);
								}
								xt=PLOT_LINEAR_BORDERS_LT;
							}
							else xt=PLOT_LINEAR_BORDERS_LT;
						}
						else if (xvn>xu)
						{
							if (yvn>yl)
							{
								if (xt==0)
								{
									tx=((yvn-yv)*(xu-xv))/(xvn-xv);
									tx+=yv;
									if (tx>yl)
									{
										tx=((xvn-xv)*(yl-yv))/(yvn-yv);
										tx+=xv;
										cairo_line_to(cr, tx, yl);
									}
									else cairo_line_to(cr, xu, tx);
									cairo_stroke(cr);
								}
								xt=(PLOT_LINEAR_BORDERS_RT|PLOT_LINEAR_BORDERS_DN);
							}
							else if (yvn<yu)
							{
								if (xt==0)
								{
									tx=((yvn-yv)*(xu-xv))/(xvn-xv);
									tx+=yv;
									if (tx<yu)
									{
										tx=((xvn-xv)*(yu-yv))/(yvn-yv);
										tx+=xv;
										cairo_line_to(cr, tx, yu);
									}
									else cairo_line_to(cr, xu, tx);
									cairo_stroke(cr);
								}
								xt=(PLOT_LINEAR_BORDERS_RT|PLOT_LINEAR_BORDERS_UP);
							}
							if (xt==0)
							{
								tx=((yvn-yv)*(xu-xv))/(xvn-xv);
								tx+=yv;
								cairo_line_to(cr, xu, tx);
								cairo_stroke(cr);
								xt=PLOT_LINEAR_BORDERS_RT;
							}
							else if (xt==PLOT_LINEAR_BORDERS_DN)
							{
								tx=((yvn-yv)*(xu-xv))/(xvn-xv);
								tx+=yv;
								if (tx>yl)
								{
									cairo_move_to(cr, xu, tx);
									tx=((xvn-xv)*(yl-yv))/(yvn-yv);
									tx+=xv;
									cairo_line_to(cr, tx, yl);
									cairo_stroke(cr);
								}
								xt=PLOT_LINEAR_BORDERS_RT;
							}
							else if (xt==PLOT_LINEAR_BORDERS_UP)
							{
								tx=((yvn-yv)*(xu-xv))/(xvn-xv);
								tx+=yv;
								if (tx>yu)
								{
									cairo_move_to(cr, xu, tx);
									tx=((xvn-xv)*(yu-yv))/(yvn-yv);
									tx+=xv;
									cairo_line_to(cr, tx, yu);
									cairo_stroke(cr);
								}
								xt=PLOT_LINEAR_BORDERS_RT;
							}
							else xt=PLOT_LINEAR_BORDERS_RT;
						}
						else if (yvn>yl)
						{
							if (xt==0)
							{
								tx=((xvn-xv)*(yl-yvn))/(yvn-yv);
								tx+=xvn;
								cairo_line_to(cr, tx, yl);
								cairo_stroke(cr);
							}
							else if (xt==PLOT_LINEAR_BORDERS_LT)
							{
								tx=((xvn-xv)*(yl-yvn))/(yvn-yv);
								tx+=xvn;
								if (tx<xl)
								{
									cairo_move_to(cr, tx, yl);
									tx=((yvn-yv)*(xl-xv))/(xvn-xv);
									tx+=yv;
									cairo_line_to(cr, xl, tx);
									cairo_stroke(cr);
								}
							}
							else if (xt==PLOT_LINEAR_BORDERS_RT)
							{
								tx=((xvn-xv)*(yl-yvn))/(yvn-yv);/* SIGFPE here. change? */
								tx+=xvn;
								if (tx>xu)
								{
									cairo_move_to(cr, tx, yl);
									tx=((yvn-yv)*(xu-xvn))/(xvn-xv);
									tx+=yvn;
									cairo_line_to(cr, xu, tx);
									cairo_stroke(cr);
								}
							}
							xt=PLOT_LINEAR_BORDERS_DN;
						}
						else if (yvn<yu)
						{
							if (xt==0)
							{
								tx=((xvn-xv)*(yu-yvn))/(yvn-yv);
								tx+=xvn;
								cairo_line_to(cr, tx, yu);	
								cairo_stroke(cr);
							}
							else if (xt==PLOT_LINEAR_BORDERS_LT)
							{
								tx=((xvn-xv)*(yu-yvn))/(yvn-yv);
								tx+=xvn;
								if (tx<xl)
								{
									cairo_move_to(cr, tx, yu);
									tx=((yvn-yv)*(xl-xv))/(xvn-xv);
									tx+=yv;
									cairo_line_to(cr, xl, tx);
									cairo_stroke(cr);
								}
							}
							else if (xt==PLOT_LINEAR_BORDERS_RT)
							{
								tx=((xvn-xv)*(yu-yvn))/(yvn-yv);
								tx+=xvn;
								if (tx>xu)
								{
									cairo_move_to(cr, tx, yu);
									tx=((yvn-yv)*(xu-xvn))/(xvn-xv);
									tx+=yvn;
									cairo_line_to(cr, xu, tx);
									cairo_stroke(cr);
								}
							}
							xt=PLOT_LINEAR_BORDERS_UP;
						}
						else /* within range */
						{
							if ((xt&PLOT_LINEAR_BORDERS_LT)!=0)
							{
								if ((xt&PLOT_LINEAR_BORDERS_DN)!=0)
								{
									tx=((yvn-yv)*(xl-xv))/(xvn-xv);
									tx+=yv;
									if (tx>yl)
									{
										tx=((xvn-xv)*(yl-yv))/(yvn-yv);
										tx+=xv;
										cairo_move_to(cr, tx, yl);
									}
									else cairo_move_to(cr, xl, tx);
								}
								else if ((xt&PLOT_LINEAR_BORDERS_UP)!=0)
								{
									tx=((yvn-yv)*(xl-xv))/(xvn-xv);
									tx+=yv;
									if (tx<yu)
									{
										tx=((xvn-xv)*(yu-yv))/(yvn-yv);
										tx+=xv;
										cairo_move_to(cr, tx, yu);
									}
									else cairo_move_to(cr, xl, tx);
								}
								else
									{
									tx=((yvn-yv)*(xl-xv))/(xvn-xv);
									tx+=yv;
									cairo_move_to(cr, xl, tx);
								}
							}
							else if ((xt&PLOT_LINEAR_BORDERS_RT)!=0)
							{
								if ((xt&PLOT_LINEAR_BORDERS_DN)!=0)
								{
									tx=((yvn-yv)*(xu-xvn))/(xvn-xv);
									tx+=yvn;
									if (tx>yl)
									{
										tx=((xvn-xv)*(yl-yvn))/(yvn-yv);
										tx+=xvn;
										cairo_move_to(cr, tx, yl);
									}
									else cairo_move_to(cr, xl, tx);
								}
								else if ((xt&PLOT_LINEAR_BORDERS_DN)!=0)
								{
									tx=((yvn-yv)*(xu-xvn))/(xvn-xv);
									tx+=yvn;
									if (tx<yu)
									{
										tx=((xvn-xv)*(yu-yvn))/(yvn-yv);
										tx+=xvn;
										cairo_move_to(cr, tx, yu);
									}
									else cairo_move_to(cr, xl, tx);
								}
								else
								{
									tx=((yvn-yv)*(xu-xvn))/(xvn-xv);
									tx+=yvn;
									cairo_move_to(cr, xu, tx);
								}
							}
							else if ((xt&PLOT_LINEAR_BORDERS_DN)!=0)
							{
								tx=((xvn-xv)*(yl-yv))/(yvn-yv);
								tx+=xv;
								cairo_move_to(cr, tx, yl);
							}
							else if ((xt&PLOT_LINEAR_BORDERS_UP)!=0)
							{
								tx=((xvn-xv)*(yu-yv))/(yvn-yv);
								tx+=xv;
								cairo_move_to(cr, tx, yu);
							}
							cairo_line_to(cr, xvn, yvn);
							cairo_stroke(cr);
							cairo_arc(cr, xvn, yvn, (plt->ptsize), 0, MY_2PI);
							cairo_fill(cr);
							cairo_move_to(cr, xvn, yvn);
							xt=0;
						}
						xv=xvn;
						yv=yvn;
					}
				}
			}
		}
		else /* lines only */
		{
			for (k=0;k<(plt->ind->len);k++)
			{
				lt=g_array_index((plt->sizes), gint, k);
				if (lt>0)
				{
					ft=g_array_index((plt->ind), gint, k);
					lt+=ft;
					dtt=fmod(k,(plt->rd->len));
					vv=g_array_index((plt->rd), gdouble, dtt);
					wv=g_array_index((plt->gr), gdouble, dtt);
					zv=g_array_index((plt->bl), gdouble, dtt);
					av=g_array_index((plt->al), gdouble, dtt);
					cairo_set_source_rgba(cr, vv, wv, zv, av);
					xv=xl+((xu-xl)*(g_array_index((plt->xdata), gdouble, ft)-(prv->bounds.xmin))/((prv->bounds.xmax)-(prv->bounds.xmin)));/*segmentation fault here*/
					yv=yl+((yu-yl)*(g_array_index((plt->ydata), gdouble, ft)-(prv->bounds.ymin))/((prv->bounds.ymax)-(prv->bounds.ymin)));
					if (xv<xl)
					{
						if (yv>yl) xt=(PLOT_LINEAR_BORDERS_LT|PLOT_LINEAR_BORDERS_DN);
						else if (yv<yu) xt=(PLOT_LINEAR_BORDERS_LT|PLOT_LINEAR_BORDERS_UP);
						else xt=PLOT_LINEAR_BORDERS_LT;
					}
					else if (xv>xu)
					{
						if (yv>yl) xt=(PLOT_LINEAR_BORDERS_RT|PLOT_LINEAR_BORDERS_DN);
						else if (yv<yu) xt=(PLOT_LINEAR_BORDERS_RT|PLOT_LINEAR_BORDERS_UP);
						else xt=PLOT_LINEAR_BORDERS_RT;
					}
					else if (yv>yl)xt=PLOT_LINEAR_BORDERS_DN;
					else if (yv<yu) xt=PLOT_LINEAR_BORDERS_UP;
					else {xt=0; cairo_move_to(cr, xv, yv);}
					for (j=1+ft; j<lt; j++)
					{
						xvn=xl+((xu-xl)*(g_array_index(plt->xdata, gdouble, j)-(prv->bounds.xmin))/((prv->bounds.xmax)-(prv->bounds.xmin)));
						yvn=yl+((yu-yl)*(g_array_index(plt->ydata, gdouble, j)-(prv->bounds.ymin))/((prv->bounds.ymax)-(prv->bounds.ymin)));
						if (xvn<xl)
						{
							if (yvn>yl)
							{
								if (xt==0)
								{
									tx=xvn-xv;
									if ((tx<DZE)&&(tx>NZE)) cairo_line_to(cr, xvn, yl);
									else
									{
										tx=((yvn-yv)*(xl-xvn))/tx;
										tx+=yvn;
										if (tx>yl)
										{
											tx=((xvn-xv)*(yl-yvn))/(yvn-yv);
											tx+=xvn;
											cairo_line_to(cr, tx, yl);
										}
										else cairo_line_to(cr, xl, tx);
									}
									cairo_stroke(cr);
								}
								xt=(PLOT_LINEAR_BORDERS_LT|PLOT_LINEAR_BORDERS_DN);
							}
							else if (yvn<yu)
							{
								if (xt==0)
								{
									tx=xvn-xv;
									if ((tx<DZE)&&(tx>NZE)) cairo_line_to(cr, xvn, yu);
									else
									{
										tx=((yvn-yv)*(xl-xvn))/tx;
										tx+=yvn;
										if (tx<yu)
										{
											tx=((xvn-xv)*(yu-yvn))/(yvn-yv);
											tx+=xvn;
											cairo_line_to(cr, tx, yu);
										}
										else cairo_line_to(cr, xl, tx);
									}
									cairo_stroke(cr);
								}
								xt=(PLOT_LINEAR_BORDERS_LT|PLOT_LINEAR_BORDERS_UP);
							}
							else if (xt==0)
							{
								tx=xvn-xv;
								if ((tx<DZE)&&(tx>NZE)) cairo_line_to(cr, xl, yvn);
								else
								{
									tx=((yvn-yv)*(xl-xvn))/tx;
									tx+=yvn;
									cairo_line_to(cr, xl, tx);
								}
								cairo_stroke(cr);
								xt=PLOT_LINEAR_BORDERS_LT;
							}
							else if (xt==PLOT_LINEAR_BORDERS_DN)
							{
								tx=((yvn-yv)*(xl-xvn))/(xvn-xv);
								tx+=yvn;
								if (tx>yl)
								{
									cairo_move_to(cr, xl, tx);
									tx=((xvn-xv)*(yl-yv))/(yvn-yv);
									tx+=xv;
									cairo_line_to(cr, tx, yl);
									cairo_stroke(cr);
								}
								xt=PLOT_LINEAR_BORDERS_LT;
							}
							else if (xt==PLOT_LINEAR_BORDERS_UP)
							{
								tx=((yvn-yv)*(xl-xvn))/(xvn-xv);
								tx+=yvn;
								if (tx<yu)
								{
									cairo_move_to(cr, xl, tx);
									tx=((xvn-xv)*(yu-yv))/(yvn-yv);
									tx+=xv;
									cairo_line_to(cr, tx, yu);
									cairo_stroke(cr);
								}
								xt=PLOT_LINEAR_BORDERS_LT;
							}
							else xt=PLOT_LINEAR_BORDERS_LT;
						}
						else if (xvn>xu)
						{
							if (yvn>yl)
							{
								if (xt==0)
								{
									tx=((yvn-yv)*(xu-xv))/(xvn-xv);
									tx+=yv;
									if (tx>yl)
									{
										tx=((xvn-xv)*(yl-yv))/(yvn-yv);
										tx+=xv;
										cairo_line_to(cr, tx, yl);
									}
									else cairo_line_to(cr, xu, tx);
									cairo_stroke(cr);
								}
								xt=(PLOT_LINEAR_BORDERS_RT|PLOT_LINEAR_BORDERS_DN);
							}
							else if (yvn<yu)
							{
								if (xt==0)
								{
									tx=((yvn-yv)*(xu-xv))/(xvn-xv);
									tx+=yv;
									if (tx<yu)
									{
										tx=((xvn-xv)*(yu-yv))/(yvn-yv);
										tx+=xv;
										cairo_line_to(cr, tx, yu);
									}
									else cairo_line_to(cr, xu, tx);
									cairo_stroke(cr);
								}
								xt=(PLOT_LINEAR_BORDERS_RT|PLOT_LINEAR_BORDERS_UP);
							}
							if (xt==0)
							{
								tx=((yvn-yv)*(xu-xv))/(xvn-xv);
								tx+=yv;
								cairo_line_to(cr, xu, tx);
								cairo_stroke(cr);
								xt=PLOT_LINEAR_BORDERS_RT;
							}
							else if (xt==PLOT_LINEAR_BORDERS_DN)
							{
								tx=((yvn-yv)*(xu-xv))/(xvn-xv);
								tx+=yv;
								if (tx>yl)
								{
									cairo_move_to(cr, xu, tx);
									tx=((xvn-xv)*(yl-yv))/(yvn-yv);
									tx+=xv;
									cairo_line_to(cr, tx, yl);
									cairo_stroke(cr);
								}
								xt=PLOT_LINEAR_BORDERS_RT;
							}
							else if (xt==PLOT_LINEAR_BORDERS_UP)
							{
								tx=((yvn-yv)*(xu-xv))/(xvn-xv);
								tx+=yv;
								if (tx>yu)
								{
									cairo_move_to(cr, xu, tx);
									tx=((xvn-xv)*(yu-yv))/(yvn-yv);
									tx+=xv;
									cairo_line_to(cr, tx, yu);
									cairo_stroke(cr);
								}
								xt=PLOT_LINEAR_BORDERS_RT;
							}
							else xt=PLOT_LINEAR_BORDERS_RT;
						}
						else if (yvn>yl)
						{
							if (xt==0)
							{
								tx=((xvn-xv)*(yl-yvn))/(yvn-yv);
								tx+=xvn;
								cairo_line_to(cr, tx, yl);
								cairo_stroke(cr);
							}
							else if (xt==PLOT_LINEAR_BORDERS_LT)
							{
								tx=((xvn-xv)*(yl-yvn))/(yvn-yv);
								tx+=xvn;
								if (tx<xl)
								{
									cairo_move_to(cr, tx, yl);
									tx=((yvn-yv)*(xl-xv))/(xvn-xv);
									tx+=yv;
									cairo_line_to(cr, xl, tx);
									cairo_stroke(cr);
								}
							}
							else if (xt==PLOT_LINEAR_BORDERS_RT)
							{
								tx=((xvn-xv)*(yl-yvn))/(yvn-yv);
								tx+=xvn;
								if (tx>xu)
								{
									cairo_move_to(cr, tx, yl);
									tx=((yvn-yv)*(xu-xvn))/(xvn-xv);
									tx+=yvn;
									cairo_line_to(cr, xu, tx);
									cairo_stroke(cr);
								}
							}
							xt=PLOT_LINEAR_BORDERS_DN;
						}
						else if (yvn<yu)
						{
							if (xt==0)
							{
								tx=((xvn-xv)*(yu-yvn))/(yvn-yv);
								tx+=xvn;
								cairo_line_to(cr, tx, yu);	
								cairo_stroke(cr);
							}
							else if (xt==PLOT_LINEAR_BORDERS_LT)
							{
								tx=((xvn-xv)*(yu-yvn))/(yvn-yv);
								tx+=xvn;
								if (tx<xl)
								{
									cairo_move_to(cr, tx, yu);
									tx=((yvn-yv)*(xl-xv))/(xvn-xv);
									tx+=yv;
									cairo_line_to(cr, xl, tx);
									cairo_stroke(cr);
								}
							}
							else if (xt==PLOT_LINEAR_BORDERS_RT)
							{
								tx=((xvn-xv)*(yu-yvn))/(yvn-yv);
								tx+=xvn;
								if (tx>xu)
								{
									cairo_move_to(cr, tx, yu);
									tx=((yvn-yv)*(xu-xvn))/(xvn-xv);
									tx+=yvn;
									cairo_line_to(cr, xu, tx);
									cairo_stroke(cr);
								}
							}
							xt=PLOT_LINEAR_BORDERS_UP;
						}
						else /* within range */
						{
							if ((xt&PLOT_LINEAR_BORDERS_LT)!=0)
							{
								if ((xt&PLOT_LINEAR_BORDERS_DN)!=0)
								{
									tx=((yvn-yv)*(xl-xv))/(xvn-xv);
									tx+=yv;
									if (tx>yl)
									{
										tx=((xvn-xv)*(yl-yv))/(yvn-yv);
										tx+=xv;
										cairo_move_to(cr, tx, yl);
									}
									else cairo_move_to(cr, xl, tx);
								}
								else if ((xt&PLOT_LINEAR_BORDERS_UP)!=0)
								{
									tx=((yvn-yv)*(xl-xv))/(xvn-xv);
									tx+=yv;
									if (tx<yu)
									{
										tx=((xvn-xv)*(yu-yv))/(yvn-yv);
										tx+=xv;
										cairo_move_to(cr, tx, yu);
									}
									else cairo_move_to(cr, xl, tx);
								}
								else
								{
									tx=((yvn-yv)*(xl-xv))/(xvn-xv);
									tx+=yv;
									cairo_move_to(cr, xl, tx);
								}
							}
							else if ((xt&PLOT_LINEAR_BORDERS_RT)!=0)
							{
								if ((xt&PLOT_LINEAR_BORDERS_DN)!=0)
								{
									tx=((yvn-yv)*(xu-xvn))/(xvn-xv);
									tx+=yvn;
									if (tx>yl)
									{
										tx=((xvn-xv)*(yl-yvn))/(yvn-yv);
										tx+=xvn;
										cairo_move_to(cr, tx, yl);
									}
									else cairo_move_to(cr, xl, tx);
								}
								else if ((xt&PLOT_LINEAR_BORDERS_UP)!=0)
								{
									tx=((yvn-yv)*(xu-xvn))/(xvn-xv);
									tx+=yvn;
									if (tx<yu)
									{
										tx=((xvn-xv)*(yu-yvn))/(yvn-yv);
										tx+=xvn;
										cairo_move_to(cr, tx, yu);
									}
									else cairo_move_to(cr, xl, tx);
								}
								else
								{
									tx=((yvn-yv)*(xu-xvn))/(xvn-xv);
									tx+=yvn;
									cairo_move_to(cr, xu, tx);
								}
							}
							else if ((xt&PLOT_LINEAR_BORDERS_DN)!=0)
							{
								tx=((xvn-xv)*(yl-yv))/(yvn-yv);
								tx+=xv;
								cairo_move_to(cr, tx, yl);
							}
							else if ((xt&PLOT_LINEAR_BORDERS_UP)!=0)
							{
								tx=((xvn-xv)*(yu-yv))/(yvn-yv);
								tx+=xv;
								cairo_move_to(cr, tx, yu);
							}
							cairo_line_to(cr, xvn, yvn);
							xt=0;
						}
						xv=xvn;
						yv=yvn;
					}
					cairo_stroke(cr);
				}
			}
		}
	}
	else if (((plt->flagd)&PLOT_LINEAR_DISP_PTS)!=0) /* points only */
	{
		for (k=0;k<(plt->ind->len);k++)
		{
			lt=g_array_index((plt->sizes), gint, k);
			if (lt>0)
			{
				ft=g_array_index((plt->ind), gint, k);
				lt+=ft;
				dtt=fmod(k,(plt->rd->len));
				vv=g_array_index((plt->rd), gdouble, dtt);
				wv=g_array_index((plt->gr), gdouble, dtt);
				zv=g_array_index((plt->bl), gdouble, dtt);
				av=g_array_index((plt->al), gdouble, dtt);
				cairo_set_source_rgba(cr, vv, wv, zv, av);
				ft=g_array_index((plt->ind), gint, k);
				lt=g_array_index((plt->sizes), gint, k)+ft;
				xv=xl+((xu-xl)*(g_array_index(plt->xdata, gdouble, ft)-(prv->bounds.xmin))/((prv->bounds.xmax)-(prv->bounds.xmin)));
				yv=yl+((yu-yl)*(g_array_index(plt->ydata, gdouble, ft)-(prv->bounds.ymin))/((prv->bounds.ymax)-(prv->bounds.ymin)));
				if ((yv<=yl)&&(yv>=yu)&&(xv>=xl)&&(xv<=xu))
				{
					cairo_arc(cr, xv, yv, (plt->ptsize), 0, MY_2PI);
					cairo_fill(cr);
				}
				for (j=1+ft; j<lt; j++)
				{
					xv=xl+((xu-xl)*(g_array_index(plt->xdata, gdouble, j)-(prv->bounds.xmin))/((prv->bounds.xmax)-(prv->bounds.xmin)));
					yv=yl+((yu-yl)*(g_array_index(plt->ydata, gdouble, j)-(prv->bounds.ymin))/((prv->bounds.ymax)-(prv->bounds.ymin)));
					if ((yv<=yl)&&(yv>=yu)&&(xv>=xl)&&(xv<=xu))
					{
						cairo_arc(cr, xv, yv, (plt->ptsize), 0, MY_2PI);
						cairo_fill(cr);
					}
				}
			}
		}
	}
}

static void plot_linear_redraw(GtkWidget *wgt)
{
	GdkRegion *rgn;

	if (!(wgt->window)) return;
	rgn=gdk_drawable_get_clip_region(wgt->window);
	gdk_window_invalidate_region((wgt->window), rgn, TRUE);
	gdk_window_process_updates((wgt->window), TRUE);
	gdk_region_destroy(rgn);
}

gboolean plot_linear_update_scale(GtkWidget *wgt, gdouble xn, gdouble xx, gdouble yn, gdouble yx)
{
	PlotLinearPrivate *prv;

	prv=PLOT_LINEAR_GET_PRIVATE(wgt);
	(prv->bounds.xmin)=xn;
	(prv->bounds.xmax)=xx;
	(prv->bounds.ymin)=yn;
	(prv->bounds.ymax)=yx;
	plot_linear_redraw(wgt);
	return FALSE;
}

gboolean plot_linear_update_scale_pretty(GtkWidget *wgt, gdouble xl, gdouble xu, gdouble yl, gdouble yu)
{
	PlotLinearPrivate *prv;
	PlotLinear *plt;
	gdouble num, num3, xn, xx, yn, yx;
	gint num2, lt, ut, tk;

	if (xl>xu) {xn=xu; xx=xl;}
	else {xn=xl; xx=xu;}
	if (yl>yu) {yn=yu; yx=yl;}
	else {yn=yl; yx=yu;}
	plt=PLOT_LINEAR(wgt);
	prv=PLOT_LINEAR_GET_PRIVATE(wgt);
	num3=(xx-xn)/6;
	num=log10(num3);
	if (num>=0)
	{
		num2=(gint)num;
		num=fmod(num,1);
	}
	else
	{
		num2=(gint)floor(num);
		num=fmod(num,1);
		num++;
	}
	if (num==0)
	{
		lt=(gint)floor(xn/num3);
		ut=(gint)ceil(xx/num3);
		tk=(ut-lt);
		if (tk>6)
		{
			num3*=2;
			lt=(gint)floor(xn/num3);
			ut=(gint)ceil(xx/num3);
			tk=(ut-lt);
		}
		(prv->bounds.xmin)=(num3*(gdouble)lt);
		(prv->bounds.xmax)=(num3*(gdouble)ut);
		if (tk<=0) (prv->ticks.xj)=1;
		else (prv->ticks.xj)=tk;
	}
	else if (num<0.301029997)
	{
		num=exp(G_LN10*num2);
		num3=2*num;
		lt=(gint)floor(xn/num3);
		ut=(gint)ceil(xx/num3);
		tk=(ut-lt);
		if (tk>6)
		{
			num3=5*num;
			lt=(gint)floor(xn/num3);
			ut=(gint)ceil(xx/num3);
			tk=(ut-lt);
			if (tk>6)
			{
				num3*=2;
				lt=(gint)floor(xn/num3);
				ut=(gint)ceil(xx/num3);
				tk=(ut-lt);
			}
		}
		(prv->bounds.xmin)=(num3*(gdouble)lt);
		(prv->bounds.xmax)=(num3*(gdouble)ut);
		if (tk<=0) (prv->ticks.xj)=1;
		else (prv->ticks.xj)=tk;
	}
	else if (num<0.698970005)
	{
		num=exp(G_LN10*num2);
		num3=5*num;
		lt=(gint)floor(xn/num3);
		ut=(gint)ceil(xx/num3);
		tk=(ut-lt);
		if (tk>6)
		{
			num3*=2;
			lt=(gint)floor(xn/num3);
			ut=(gint)ceil(xx/num3);
			tk=(ut-lt);
			if (tk>6)
			{
				num3*=2;
				lt=(gint)floor(xn/num3);
				ut=(gint)ceil(xx/num3);
				tk=(ut-lt);
			}
		}
		(prv->bounds.xmin)=(num3*(gdouble)lt);
		(prv->bounds.xmax)=(num3*(gdouble)ut);
		if (tk<=0) (prv->ticks.xj)=1;
		else (prv->ticks.xj)=tk;
	}
	else
	{
		num=G_LN10*(++num2);
		num3=exp(num);
		lt=(gint)floor(xn/num3);
		ut=(gint)ceil(xx/num3);
		tk=(ut-lt);
		if (tk>6)
		{
			num3*=2;
			lt=(gint)floor(xn/num3);
			ut=(gint)ceil(xx/num3);
			tk=(ut-lt);
		}
		(prv->bounds.xmin)=(num3*(gdouble)lt);
		(prv->bounds.xmax)=(num3*(gdouble)ut);
		if (tk<=0) (prv->ticks.xj)=1;
		else (prv->ticks.xj)=tk;
	}
	if (num3<1)
	{
		num=-log10(num3);
		if (fmod(num,1)<FAC) (plt->xdp)=(guint)num;
		else (plt->xdp)=(guint)ceil(num);
	}
	else (plt->xdp)=0;
	num3=(yx-yn)/6;
	num=log10(num3);
	num2=(gint)floor(num);
	num=fmod(num,1);
	if (num==0)
	{
		lt=(gint)floor(yn/num3);
		ut=(gint)ceil(yx/num3);
		tk=(ut-lt);
		if (tk>6)
		{
			num3*=2;
			lt=(gint)floor(yn/num3);
			ut=(gint)ceil(yx/num3);
			tk=(ut-lt);
		}
		(prv->bounds.ymin)=(num3*(gdouble)lt);
		(prv->bounds.ymax)=(num3*(gdouble)ut);
		if (tk<=0) (prv->ticks.yj)=1;
		else (prv->ticks.yj)=tk;
	}
	else if (num<0.301029997)
	{
		num=exp(G_LN10*num2);
		num3=2*num;
		lt=(gint)floor(yn/num3);
		ut=(gint)ceil(yx/num3);
		tk=(ut-lt);
		if (tk>6)
		{
			num3=5*num;
			lt=(gint)floor(yn/num3);
			ut=(gint)ceil(yx/num3);
			tk=(ut-lt);
			if (tk>6)
			{
				num3*=2;
				lt=(gint)floor(yn/num3);
				ut=(gint)ceil(yx/num3);
				tk=(ut-lt);
			}
		}
		(prv->bounds.ymin)=(num3*(gdouble)lt);
		(prv->bounds.ymax)=(num3*(gdouble)ut);
		if (tk<=0) (prv->ticks.yj)=1;
		else (prv->ticks.yj)=tk;
	}
	else if (num<0.698970005)
	{
		num=exp(G_LN10*num2);
		num3=5*num;
		lt=(gint)floor(yn/num3);
		ut=(gint)ceil(yx/num3);
		tk=(ut-lt);
		if (tk>6)
		{
			num3*=2;
			lt=(gint)floor(yn/num3);
			ut=(gint)ceil(yx/num3);
			tk=(ut-lt);
			if (tk>6)
			{
				num3*=2;
				lt=(gint)floor(yn/num3);
				ut=(gint)ceil(yx/num3);
				tk=(ut-lt);
			}
		}
		(prv->bounds.ymin)=(num3*(gdouble)lt);
		(prv->bounds.ymax)=(num3*(gdouble)ut);
		if (tk<=0) (prv->ticks.yj)=1;
		else (prv->ticks.yj)=tk;
	}
	else
	{
		num=G_LN10*(++num2);
		num3=exp(num);
		lt=(gint)floor(yn/num3);
		ut=(gint)ceil(yx/num3);
		tk=(ut-lt);
		if (tk>6)
		{
			num3*=2;
			lt=(gint)floor(yn/num3);
			ut=(gint)ceil(yx/num3);
			tk=(ut-lt);
		}
		(prv->bounds.ymin)=(num3*(gdouble)lt);
		(prv->bounds.ymax)=(num3*(gdouble)ut);
		if (tk<=0) (prv->ticks.yj)=1;
		else (prv->ticks.yj)=tk;
	}
	if (num3<1)
	{
		num=-log10(num3);
		if (fmod(num,1)<FAC) (plt->ydp)=(guint)num;
		else (plt->ydp)=(guint)ceil(num);
	}
	else (plt->ydp)=0;
	plot_linear_redraw(wgt);
	return FALSE;
}

gboolean plot_linear_print_eps(GtkWidget *wgt, gchar* fot)
{
	cairo_t *cr;
	cairo_surface_t *sfc;

	sfc=cairo_ps_surface_create(fot, (gdouble) (wgt->allocation.width), (gdouble) (wgt->allocation.height));
	cairo_ps_surface_set_eps(sfc, TRUE);
	cairo_ps_surface_restrict_to_level(sfc, CAIRO_PS_LEVEL_2);
	cr=cairo_create(sfc);
	draw(wgt, cr);
	cairo_surface_show_page(sfc);
	cairo_destroy(cr);
	cairo_surface_finish(sfc);
	cairo_surface_destroy(sfc);
	return FALSE;
}

gboolean plot_linear_print_png(GtkWidget *wgt, gchar* fot)
{
	cairo_t *cr;
	cairo_surface_t *sfc;

	sfc=cairo_image_surface_create(CAIRO_FORMAT_ARGB32, (gdouble) (wgt->allocation.width), (gdouble) (wgt->allocation.height));
	cr=cairo_create(sfc);
	draw(wgt, cr);
	cairo_surface_write_to_png(sfc, fot);
	cairo_destroy(cr);
	cairo_surface_destroy(sfc);
	return FALSE;
}

gboolean plot_linear_print_svg(GtkWidget *wgt, gchar* fot)
{
	cairo_t *cr;
	cairo_surface_t *sfc;

	sfc=cairo_svg_surface_create(fot, (gdouble) (wgt->allocation.width), (gdouble) (wgt->allocation.height));
	cr=cairo_create(sfc);
	draw(wgt, cr);
	cairo_destroy(cr);
	cairo_surface_destroy(sfc);
	return FALSE;
}

static gboolean plot_linear_button_press(GtkWidget *wgt, GdkEventButton *vnt)
{
	PlotLinearPrivate *prv;
	PlotLinear *plt;
	gint d;

	prv=PLOT_LINEAR_GET_PRIVATE(wgt);
	plt=PLOT_LINEAR(wgt);
	if ((prv->flagr)==0)
	{
		(prv->rescale.xmin)=(prv->bounds.xmin);
		(prv->rescale.ymin)=(prv->bounds.ymin);
		if (((plt->zmode)&(PLOT_LINEAR_ZOOM_SGL|PLOT_LINEAR_ZOOM_HZT))!=0)
		{
			if ((vnt->x)>=(prv->range.xn)) (prv->rescale.xmin)=(prv->bounds.xmax);
			else
			{
				d=(vnt->x)-(prv->range.xj);
				if (d>0) {(prv->rescale.xmin)+=(((prv->bounds.xmax)-(prv->bounds.xmin))*d)/((prv->range.xn)-(prv->range.xj)); (prv->flagr)=1;}
			}
		}
		if (((plt->zmode)&(PLOT_LINEAR_ZOOM_SGL|PLOT_LINEAR_ZOOM_VRT))!=0)
		{
			if ((vnt->y)<=(prv->range.yj)) (prv->rescale.ymin)=(prv->bounds.ymax);
			else
			{
				d=(prv->range.yn)-(vnt->y);
				if (d>0) {(prv->rescale.ymin)+=(((prv->bounds.ymax)-(prv->bounds.ymin))*d)/((prv->range.yn)-(prv->range.yj)); (prv->flagr)=1;}
			}
		}
	}
	return FALSE;
}

static gboolean plot_linear_motion_notify(GtkWidget *wgt, GdkEventMotion *vnt)
{
	PlotLinearPrivate *prv;
	PlotLinear *plt;
	gdouble dx, dy;

	prv=PLOT_LINEAR_GET_PRIVATE(wgt);
	plt=PLOT_LINEAR(wgt);
	dx=((vnt->x)-(prv->range.xj))/((prv->range.xn)-(prv->range.xj));
	dy=((prv->range.yn)-(vnt->y))/((prv->range.yn)-(prv->range.yj));
	if ((dx>=0)&&(dy>=0)&&(dx<=1)&&(dy<=1))
	{
		(plt->xps)=((prv->bounds.xmax)*dx)+((prv->bounds.xmin)*(1-dx));
		(plt->yps)=((prv->bounds.ymax)*dy)+((prv->bounds.ymin)*(1-dy));
		g_signal_emit(plt, plot_linear_signals[MOVED], 0);
	}
	return FALSE;
}

static gboolean plot_linear_button_release(GtkWidget *wgt, GdkEventButton *vnt)
{
	PlotLinearPrivate *prv;
	PlotLinear *plt;
	gint d, xw;
	gdouble xn, xx, yn, yx, s;

	prv=PLOT_LINEAR_GET_PRIVATE(wgt);
	plt=PLOT_LINEAR(wgt);
	if ((prv->flagr)==1)
	{
		if (((plt->zmode)&PLOT_LINEAR_ZOOM_SGL)==0)
		{
			(prv->rescale.xmax)=(prv->bounds.xmax);
			(prv->rescale.ymax)=(prv->bounds.ymax);
			if (((plt->zmode)&PLOT_LINEAR_ZOOM_HZT)!=0)
			{
				if ((vnt->x)<=(prv->range.xj)) (prv->rescale.xmax)=(prv->bounds.xmin);
				else
				{
					d=(prv->range.xn)-(vnt->x);
					if (d>0) (prv->rescale.xmax)-=(((prv->bounds.xmax)-(prv->bounds.xmin))*d)/((prv->range.xn)-(prv->range.xj));
				}
			}
			if (((plt->zmode)&PLOT_LINEAR_ZOOM_VRT)!=0)
			{
				if ((vnt->y)>=(prv->range.yn)) (prv->rescale.ymax)=(prv->bounds.ymin);
				else
				{
					d=(vnt->y)-(prv->range.yj);
					if (d>0) (prv->rescale.ymax)-=(((prv->bounds.ymax)-(prv->bounds.ymin))*d)/((prv->range.yn)-(prv->range.yj));
				}
			}
			xn=(prv->rescale.xmax)-(prv->rescale.xmin);
			yn=(prv->rescale.ymax)-(prv->rescale.ymin);
			if (((xn>DZE)||(xn<NZE))&&((yn>DZE)||(yn<NZE)))
			{
				if (((plt->zmode)&PLOT_LINEAR_ZOOM_OUT)==0) plot_linear_update_scale_pretty(wgt, (prv->rescale.xmin), (prv->rescale.xmax), (prv->rescale.ymin), (prv->rescale.ymax));
				else
				{
					s=((prv->bounds.xmax)-(prv->bounds.xmin))/xn;
					if (s>0)
					{
						xn=((prv->bounds.xmin)-(prv->rescale.xmin))*s;
						xn+=(prv->bounds.xmin);
						xx=((prv->bounds.xmax)-(prv->rescale.xmax))*s;
						xx+=(prv->bounds.xmax);
						s=((prv->bounds.ymax)-(prv->bounds.ymin))/yn;
						if (s>0)
						{
							yn=((prv->bounds.ymin)-(prv->rescale.ymin))*s;
							yn+=(prv->bounds.ymin);
							yx=((prv->bounds.ymax)-(prv->rescale.ymax))*s;
							yx+=(prv->bounds.ymax);
							plot_linear_update_scale_pretty(wgt, xn, xx, yn, yx);
						}
						else if (s<0)
						{
							yn=((prv->rescale.ymax)-(prv->bounds.ymin))*s;
							yn+=(prv->bounds.ymin);
							yn=((prv->rescale.ymin)-(prv->bounds.ymax))*s;
							yx+=(prv->bounds.ymax);
							plot_linear_update_scale_pretty(wgt, xn, xx, yn, yx);
						}
					}
					else if (s<0)
					{
						xn=((prv->rescale.xmax)-(prv->bounds.xmin))*s;
						xn+=(prv->bounds.xmin);
						xn=((prv->rescale.xmin)-(prv->bounds.xmax))*s;
						xx+=(prv->bounds.xmax);
						s=((prv->bounds.ymax)-(prv->bounds.ymin))/yn;
						if (s>0)
						{
							yn=((prv->bounds.ymin)-(prv->rescale.ymin))*s;
							yn+=(prv->bounds.ymin);
							yn=((prv->bounds.ymax)-(prv->rescale.ymax))*s;
							yx+=(prv->bounds.ymax);
							plot_linear_update_scale_pretty(wgt, xn, xx, yn, yx);
						}
						else if (s<0)
						{
							yn=((prv->rescale.ymax)-(prv->bounds.ymin))*s;
							yn+=(prv->bounds.ymin);
							yn=((prv->rescale.ymin)-(prv->bounds.ymax))*s;
							yx+=(prv->bounds.ymax);
							plot_linear_update_scale_pretty(wgt, xn, xx, yn, yx);
						}
					}
				}
			}
		}
		else if (((plt->zmode)&PLOT_LINEAR_ZOOM_OUT)==0)
		{
			xn=ZS*(prv->rescale.xmin);
			xx=xn;
			xn+=ZSC*(prv->bounds.xmin);
			xx+=ZSC*(prv->bounds.xmax);
			yn=ZS*(prv->rescale.ymin);
			yx=yn;
			yn+=ZSC*(prv->bounds.ymin);
			yx+=ZSC*(prv->bounds.ymax);
			plot_linear_update_scale_pretty(wgt, xn, xx, yn, yx);
		}
		else
		{
			xn=-UZC*(prv->rescale.xmin);
			xx=xn;
			xn+=UZ*(prv->bounds.xmin);
			xx+=UZ*(prv->bounds.xmax);
			yn=-UZC*(prv->rescale.ymin);
			yx=yn;
			yn+=UZ*(prv->bounds.ymin);
			yx+=UZ*(prv->bounds.ymax);
			plot_linear_update_scale_pretty(wgt, xn, xx, yn, yx);
		}
		(prv->flagr)=0;
	}
	else if ((vnt->y)<=11)
	{
		xw=(wgt->allocation.width);
		if ((vnt->x)>=xw-22)
		{
			if ((vnt->x)>=xw-11)
			{
				(plt->zmode)^=PLOT_LINEAR_ZOOM_OUT;
				plot_linear_redraw(wgt);
			}
			else if (((plt->zmode)&PLOT_LINEAR_ZOOM_SGL)!=0)
			{
				(plt->zmode)&=PLOT_LINEAR_ZOOM_OUT;
				plot_linear_redraw(wgt);
			}
			else
			{
				{(plt->zmode)++; (plt->zmode)++;}
				plot_linear_redraw(wgt);
			}
		}
	}
	return FALSE;
}

static void plot_linear_finalise(PlotLinear *plt)
{
	if (plt->xlab) g_free(plt->xlab);
	if (plt->ylab) g_free(plt->ylab);
	if (plt->afont) pango_font_description_free(plt->afont);
	if (plt->lfont) pango_font_description_free(plt->lfont);
	if (plt->xdata) g_array_free((plt->xdata), FALSE);
	if (plt->ydata) g_array_free((plt->ydata), FALSE);
	if (plt->ind) g_array_free((plt->ind), FALSE);
	if (plt->sizes) g_array_free((plt->sizes), FALSE);
	if (plt->rd) g_array_free((plt->rd), FALSE);
	if (plt->gr) g_array_free((plt->gr), FALSE);
	if (plt->bl) g_array_free((plt->bl), FALSE);
	if (plt->al) g_array_free((plt->al), FALSE);
}

static void plot_linear_set_property(GObject *obj, guint pid, const GValue *val, GParamSpec *psp)
{
	PlotLinearPrivate *prv;

	prv=PLOT_LINEAR_GET_PRIVATE(obj);
	switch (pid)
	{
		case PROP_BXN:
		{
			prv->bounds.xmin=g_value_get_double(val);
			break;
		}
		case PROP_BXX:
		{
			prv->bounds.xmax=g_value_get_double(val);
			break;
		}
		case PROP_BYN:
		{
			prv->bounds.ymin=g_value_get_double(val);
			break;
		}
		case PROP_BYX:
		{
			prv->bounds.ymax=g_value_get_double(val);
			break;
		}
		case PROP_XTJ:
		{
			prv->ticks.xj=g_value_get_uint(val);
			break;
		}
		case PROP_YTJ:
		{
			prv->ticks.yj=g_value_get_uint(val);
			break;
		}
		case PROP_XTN:
		{
			prv->ticks.xn=g_value_get_uint(val);
			break;
		}
		case PROP_YTN:
		{
			prv->ticks.yn=g_value_get_uint(val);
			break;
		}
		case PROP_FA:
		{
			prv->flaga=g_value_get_uint(val);
			break;
		}
		default:
		{
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, pid, psp);
			break;
		}
	}
}

static void plot_linear_get_property(GObject *obj, guint pid, GValue *val, GParamSpec *psp)
{
	PlotLinearPrivate *prv;

	prv=PLOT_LINEAR_GET_PRIVATE(obj);
	switch (pid)
	{
		case PROP_BXN:
		{
			g_value_set_double(val, prv->bounds.xmin);
			break;
		}
		case PROP_BXX:
		{
			g_value_set_double(val, prv->bounds.xmax);
			break;
		}
		case PROP_BYN:
		{
			g_value_set_double(val, prv->bounds.ymin);
			break;
		}
		case PROP_BYX:
		{
			g_value_set_double(val, prv->bounds.ymax);
			break;
		}
		case PROP_XTJ:
		{
			g_value_set_uint(val, prv->ticks.xj);
			break;
		}
		case PROP_YTJ:
		{
			g_value_set_uint(val, prv->ticks.yj);
			break;
		}
		case PROP_XTN:
		{
			g_value_set_uint(val, prv->ticks.xn);
			break;
		}
		case PROP_YTN:
		{
			g_value_set_uint(val, prv->ticks.yn);
			break;
		}
		case PROP_FA:
		{
			g_value_set_uint(val, prv->flaga);
			break;
		}
		default:
		{
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, pid, psp);
			break;
		}
	}
}

static gboolean plot_linear_expose(GtkWidget *wgt, GdkEventExpose *vnt)
{
	cairo_t *cr;

	cr=gdk_cairo_create(wgt->window);
	cairo_rectangle(cr, vnt->area.x, vnt->area.y, vnt->area.width, vnt->area.height);
	cairo_clip(cr);
	draw(wgt, cr);
	drawz(wgt, cr);
	cairo_destroy(cr);
	return FALSE;
}

static void plot_linear_class_init(PlotLinearClass *klass)
{
	GObjectClass *obj_klass;
	GtkWidgetClass *widget_klass;

	obj_klass=G_OBJECT_CLASS(klass);
	g_type_class_add_private(obj_klass, sizeof(PlotLinearPrivate));
	(obj_klass->finalize)=(GObjectFinalizeFunc) plot_linear_finalise;
	(obj_klass->set_property)=plot_linear_set_property;
	(obj_klass->get_property)=plot_linear_get_property;
	g_object_class_install_property(obj_klass, PROP_BXN, g_param_spec_double("xmin", "Minimum x value", "Minimum value for the horizontal scale", G_MININT, G_MAXINT, 0, G_PARAM_READWRITE));
	g_object_class_install_property(obj_klass, PROP_BXX, g_param_spec_double("xmax", "Maximum x value", "Maximum value for the horizontal scale", G_MININT, G_MAXINT, 0, G_PARAM_READWRITE));
	g_object_class_install_property(obj_klass, PROP_BYN, g_param_spec_double("ymin", "Minimum y value", "Minimum value for the vertical scale", G_MININT, G_MAXINT, 0, G_PARAM_READWRITE));
	g_object_class_install_property(obj_klass, PROP_BYX, g_param_spec_double("ymax", "Maximum y value", "Maximum value for the vertical scale", G_MININT, G_MAXINT, 0, G_PARAM_READWRITE));
	g_object_class_install_property(obj_klass, PROP_XTJ, g_param_spec_uint("xbigticks", "Major x ticks-1", "Number of gaps between major ticks for the horizontal scale-1", 1, G_MAXINT, 4, G_PARAM_READWRITE));
	g_object_class_install_property(obj_klass, PROP_YTJ, g_param_spec_uint("ybigticks", "Major y ticks-1", "Number of gaps between major ticks for the vertical scale-1", 1, G_MAXINT, 4, G_PARAM_READWRITE));
	g_object_class_install_property(obj_klass, PROP_XTN, g_param_spec_uint("xsmallticks", "Minor x ticks+1", "Number of unlabelled tick divisions between major ticks for the horizontal scale+1", 1, G_MAXINT, 5, G_PARAM_READWRITE));
	g_object_class_install_property(obj_klass, PROP_YTN, g_param_spec_uint("ysmallticks", "Minor y ticks+1", "Number of unlabelled ticks divisions between major ticks for the vertical scale+1", 1, G_MAXINT, 5, G_PARAM_READWRITE));
	g_object_class_install_property(obj_klass, PROP_FA, g_param_spec_flags("aflag", "Axis Flags", "Flags for axes behaviour: 32 = Labels right, 16 = Labels above, 8 = Wiggle on top, 4 = Wiggle underneath, 2 = Wiggle on Right, 1 = Wiggle on left", G_TYPE_FLAGS, 0, G_PARAM_READWRITE));
	widget_klass=GTK_WIDGET_CLASS(klass);
	(widget_klass->button_press_event)=plot_linear_button_press;
	(widget_klass->motion_notify_event)=plot_linear_motion_notify;
	(widget_klass->button_release_event)=plot_linear_button_release;
	(widget_klass->expose_event)=plot_linear_expose;
	plot_linear_signals[MOVED]=g_signal_new("moved", G_OBJECT_CLASS_TYPE(obj_klass), G_SIGNAL_RUN_FIRST, G_STRUCT_OFFSET (PlotLinearClass, moved), NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

static void plot_linear_init(PlotLinear *plt)
{
	gdouble val;
	gint dx;
	PlotLinearPrivate *prv;

	gtk_widget_add_events(GTK_WIDGET(plt), GDK_BUTTON_PRESS_MASK|GDK_POINTER_MOTION_MASK|GDK_BUTTON_RELEASE_MASK);
	prv=PLOT_LINEAR_GET_PRIVATE(plt);
	{(prv->bounds.xmin)=0; (prv->bounds.xmax)=1; (prv->bounds.ymin)=0; (prv->bounds.ymax)=1;}
	{(prv->ticks.xj)=4; (prv->ticks.yj)=4; (prv->ticks.xn)=5; (prv->ticks.yn)=5;}
	{(prv->range.xj)=0; (prv->range.yj)=0; (prv->range.xn)=1; (prv->range.yn)=1;}
	{(plt->xdp)=2; (plt->ydp)=2;}
	{(prv->flaga)=0; (prv->flagr)=0;}
	{(plt->xdata)=g_array_new(FALSE, FALSE, sizeof(gdouble)); (plt->ydata)=g_array_new(FALSE, FALSE, sizeof(gdouble)); (plt->ind)=g_array_sized_new(FALSE, TRUE, sizeof(gint), 1); (plt->sizes)=g_array_sized_new(FALSE, TRUE, sizeof(gint), 1);}
	{(plt->xlab)=g_strdup("Domain"); (plt->ylab)=g_strdup("Amplitude");}
	{(plt->flagd)=PLOT_LINEAR_DISP_LIN; (plt->ptsize)=5; (plt->linew)=2;}
	(plt->zmode)=(PLOT_LINEAR_ZOOM_VRT|PLOT_LINEAR_ZOOM_HZT);
	{(plt->xps)=0; (plt->yps)=0;}
	{(plt->afont)=pango_font_description_new(); (plt->lfont)=pango_font_description_new();}
	{pango_font_description_set_family((plt->afont), "sans"); pango_font_description_set_family((plt->lfont), "sans");}
	{pango_font_description_set_style((plt->afont), PANGO_STYLE_NORMAL); pango_font_description_set_style((plt->lfont), PANGO_STYLE_NORMAL);}
	{pango_font_description_set_size((plt->afont), 12*PANGO_SCALE); pango_font_description_set_size((plt->lfont), 12*PANGO_SCALE);}
	(plt->rd)=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), 7);
	(plt->gr)=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), 7);
	(plt->bl)=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), 7);
	(plt->al)=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), 7);
	val=0;
	g_array_append_val((plt->rd), val);
	g_array_append_val((plt->gr), val);
	g_array_append_val((plt->bl), val);
	g_array_append_val((plt->gr), val);
	g_array_append_val((plt->bl), val);
	g_array_append_val((plt->bl), val);
	val=1;
	g_array_append_val((plt->rd), val);
	g_array_append_val((plt->gr), val);
	g_array_append_val((plt->bl), val);
	val=0;
	g_array_append_val((plt->rd), val);
	g_array_append_val((plt->gr), val);
	g_array_append_val((plt->bl), val);
	g_array_append_val((plt->rd), val);
	val=1.0;
	g_array_append_val((plt->gr), val);
	g_array_append_val((plt->bl), val);
	g_array_append_val((plt->rd), val);
	g_array_append_val((plt->gr), val);
	g_array_append_val((plt->bl), val);
	val=0;
	g_array_append_val((plt->rd), val);
	g_array_append_val((plt->gr), val);
	val=1;
	g_array_append_val((plt->rd), val);
	val=0.8;
	g_array_append_val((plt->al), val);
	g_array_append_val((plt->al), val);
	g_array_append_val((plt->al), val);
	g_array_append_val((plt->al), val);
	g_array_append_val((plt->al), val);
	g_array_append_val((plt->al), val);
	g_array_append_val((plt->al), val);
}

GtkWidget *plot_linear_new(void) {return g_object_new(PLOT_TYPE_LINEAR, NULL);}
