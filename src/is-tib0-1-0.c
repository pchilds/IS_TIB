/***************************************************************************
 *            is-tib.c
 *
 *  A GTK+ program for performing inverse scattering using the Topelitz Inner Bordering method
 *  Features:
 *            Graphical display of data and transformed result
 *
 *  Sat Mar  17 17:18:14 2011
 *  Copyright  2011  Paul Childs
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

/*
 * TO DO:
 *
 * 
 */

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <gdk/gdkkeysyms.h>
#include <math.h>
#include "plotlinear0-1-0.h"

#define DZE 0.00001 /* divide by zero threshold */
#define NZE -0.00001 /* negative of this */
#define MXD 16 /* maximum number of windows */
#define MXDS 256 /* the square of this */
#define MXDM 15 /* one less than MXD */
#define MY_2PI 6.2831853071795864769252867665590057683943387987502
#define NMY_PI -3.1415926535897932384626433832795028841971693993751
#define LNTOT 0.23025850929940456840179914546843642076011014886288 /* (ln10)/10 */

GtkWidget *window, *tr, *zpd, *pr, *tracmenu, *trac, *fst, *notebook, *notebook2, *plot1, *plot2, *plot3, *statusbar, *rest, *visl, *dsl, *chil;
GtkWidget *agosa, *agtl, *anosa, *sws, *dlm, *ncmp, *lcmp, *bat, *chi, *twopionx, *opttri, *trac, *trans, *dBs, *neg;
GtkWidget *bsr, *bsp, *isr, *isp, *tc, *tw, *zw, *jind, *jind2, *kind; /* widgets for windowing */
GArray *bsra, *bspa, *isra, *ispa, *tca, *twa, *zwa, *x, *specs, *yb, *stars, *xsb, *ysb, *sz, *nx, *delf, *vis, *doms, *chp; /* arrays for windowing and data */
GSList *group2=NULL; /* list for various traces available */
gint lc; /* number of data points */
guint jdim=0, kdim=0, jdimx=0, kdimx=0, jdimxf=0, kdimxf=0, satl=0, trc=1, flags=0, flagd=0; /* array indices, #of traces, trace number, and current processing state and display flags */
gulong pr_id; /* id for disabling/enabling post-transform processing */

void static help(GtkWidget *widget, gpointer data)
{
	gchar *uri, *str;
	GError *Err=NULL;

	if (data) uri=g_strdup_printf("ghelp:InvScatt-help?%s", (gchar *) data);
	else uri=g_strdup("ghelp:InvScatt-help");
	gtk_show_uri(NULL, uri, gtk_get_current_event_time (), &Err);
	if (Err)
	{
		str=g_strdup(_("Could not load help files."));
		gtk_statusbar_push(GTK_STATUSBAR(statusbar), gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), str), str);
		g_free(str);
		g_error_free(Err);
	}
}

void static about(GtkWidget *widget, gpointer data)
{
	GtkWidget *helpwin;

	helpwin=gtk_about_dialog_new();
	g_signal_connect_swapped(G_OBJECT(helpwin), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(helpwin));
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(helpwin), "0.1.0");
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(helpwin), "(c) Paul Childs, 2011");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(helpwin), _("InvScatt is a program for performing inverse scattering using the Toeplitz Inner Bordering Method."));
	gtk_widget_show(helpwin);
	g_signal_connect_swapped(G_OBJECT(helpwin), "response", G_CALLBACK(gtk_widget_destroy), G_OBJECT(helpwin));
}

void static dpr(GtkWidget *widget, gpointer data)
{
	GtkWidget *helpwin, *content, *table, *vbox, *entry1, *entry2, *label, *spin1, *spin2, *hsp, *ck, *ck2, *ck3;
	GtkAdjustment *adj1, *adj2;
	PlotLinear *plt;
	gdouble xi, xf, mny, mxy, iv, clc;
	gint dx, dx2, j, k;
	/*PlotPolarBoth *plt2;*/
	
	helpwin=gtk_dialog_new_with_buttons(_("Display Properties"), GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_CLOSE, GTK_RESPONSE_CANCEL, GTK_STOCK_APPLY, GTK_RESPONSE_APPLY, NULL);
	g_signal_connect_swapped(G_OBJECT(helpwin), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(helpwin));
	gtk_widget_show(helpwin);
	content=gtk_dialog_get_content_area(GTK_DIALOG(helpwin));
	vbox=gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox);
	switch (gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook2)))
	{
		case 1:
		table=gtk_table_new(4, 2, FALSE);
		gtk_widget_show(table);
		label=gtk_label_new(_("Text size:"));
		gtk_widget_show(label);
		gtk_table_attach(GTK_TABLE(table), label, 1, 2, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		label=gtk_label_new(_("Tick label size:"));
		gtk_widget_show(label);
		gtk_table_attach(GTK_TABLE(table), label, 1, 2, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		entry1=gtk_entry_new();
		entry2=gtk_entry_new();
		hsp=gtk_hseparator_new();
		gtk_widget_show(hsp);
		label=gtk_label_new(_("X axis text:"));
		gtk_widget_show(label);
		gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		label=gtk_label_new(_("Y axis text:"));
		gtk_widget_show(label);
		gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		plt=PLOT_LINEAR(plot2);
		gtk_entry_set_text(GTK_ENTRY(entry1), g_strdup(plt->xlab));
		gtk_entry_set_text(GTK_ENTRY(entry2), g_strdup(plt->ylab));
		adj1=(GtkAdjustment *) gtk_adjustment_new((plt->lfsize), 8, 64, 1.0, 5.0, 0.0);
		adj2=(GtkAdjustment *) gtk_adjustment_new((plt->afsize), 8, 64, 1.0, 5.0, 0.0);
		spin1=gtk_spin_button_new(adj1, 0, 0);
		spin2=gtk_spin_button_new(adj2, 0, 0);
		gtk_widget_show(entry1);
		gtk_widget_show(entry2);
		gtk_widget_show(spin1);
		gtk_widget_show(spin2);
		gtk_table_attach(GTK_TABLE(table), entry1, 0, 1, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		gtk_table_attach(GTK_TABLE(table), entry2, 0, 1, 3, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		gtk_table_attach(GTK_TABLE(table), spin1, 1, 2, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		gtk_table_attach(GTK_TABLE(table), spin2, 1, 2, 3, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE, 2);
		gtk_box_pack_start(GTK_BOX(vbox), hsp, FALSE, FALSE, 2);
		if ((flags&8)!=0)
		{
			ck2=gtk_check_button_new_with_label(_("Multiple plots for Results over index j"));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ck2), (flagd&2));
			gtk_widget_show(ck2);
			gtk_box_pack_start(GTK_BOX(vbox), ck2, FALSE, FALSE, 2);
			ck3=gtk_check_button_new_with_label(_("Multiple plots for Results over index k"));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ck3), (flagd&4));
			gtk_widget_show(ck3);
			gtk_box_pack_start(GTK_BOX(vbox), ck3, FALSE, FALSE, 2);
			gtk_container_add(GTK_CONTAINER(content), vbox);
			if (gtk_dialog_run(GTK_DIALOG(helpwin))==GTK_RESPONSE_APPLY)
			{
				(plt->xlab)=g_strdup(gtk_entry_get_text(GTK_ENTRY(entry1)));
				(plt->ylab)=g_strdup(gtk_entry_get_text(GTK_ENTRY(entry2)));
				(plt->lfsize)=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin1));
				(plt->afsize)=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin2));
				if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ck2)))
				{
					if ((flagd&2)==0)
					{
						flagd^=2;
					}
				}
				else if ((flagd&2)!=0)
				{
					flagd^=2;
				}
				if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ck3)))
				{
					if ((flagd&4)==0)
					{
						flagd^=4;
					}
				}
				else if ((flagd&4)!=0)
				{
					flagd^=4;
				}
				g_object_get(G_OBJECT(plot2), "xmin", &xi, "xmax", &xf, "ymin", &mny, "ymax", &mxy, NULL);
				plot_linear_update_scale(plot2, xi, xf, mny, mxy);
			}
		}
		else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(bat)))
		{
			ck=gtk_check_button_new_with_label(_("Multiple plots for Inverse Domain"));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ck), (flagd&1));
			gtk_widget_show(ck);
			ck2=gtk_check_button_new_with_label(_("Multiple plots for Results over index j"));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ck2), (flagd&2));
			gtk_widget_show(ck2);
			gtk_box_pack_start(GTK_BOX(vbox), ck2, FALSE, FALSE, 2);
			ck3=gtk_check_button_new_with_label(_("Multiple plots for Results over index k"));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ck3), (flagd&4));
			gtk_widget_show(ck3);
			gtk_box_pack_start(GTK_BOX(vbox), ck3, FALSE, FALSE, 2);
			gtk_container_add(GTK_CONTAINER(content), vbox);
			if (gtk_dialog_run(GTK_DIALOG(helpwin))==GTK_RESPONSE_APPLY)
			{
				(plt->xlab)=g_strdup(gtk_entry_get_text(GTK_ENTRY(entry1)));
				(plt->ylab)=g_strdup(gtk_entry_get_text(GTK_ENTRY(entry2)));
				(plt->lfsize)=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin1));
				(plt->afsize)=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin2));
				g_object_get(G_OBJECT(plot2), "xmin", &xi, "xmax", &xf, "ymin", &mny, "ymax", &mxy, NULL);
				if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ck)))
				{
					if ((flagd&1)==0)
					{
						flagd^=1;
						if ((flags&2)!=0)/* merge into the one array */
						{
							dx=((plt->ydata)->len);
							dx2=0;
							g_array_free(ysb, FALSE);
							g_array_free(nx, FALSE);
							ysb=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), dx*(jdimxf+1));
							nx=g_array_sized_new(FALSE, FALSE, sizeof(gint), dx*(jdimxf+1));
							mxy=fabs(g_array_index(stars, gdouble, 0));
							g_array_append_val(ysb, mxy);
							for (j=1; j<dx; j++)
							{
								{iv=g_array_index(stars, gdouble, j); clc=g_array_index(stars, gdouble, (2*dx)-j);}
								{iv*=iv; clc*=clc;}
								iv=sqrt(iv+clc);
								if (mxy<iv) mxy=iv;
								g_array_append_val(ysb, iv);
							}
							g_array_append_val(nx, dx2);
							for (k=1; k<=jdimxf; k++)
							{
								xf=0;
								g_array_append_val(xsb, xf);
								iv=fabs(g_array_index(stars, gdouble, 2*k*dx));
								g_array_append_val(ysb, iv);
								if (mxy<iv) mxy=iv;
								for (j=1; j<dx; j++)
								{
									xf=j*g_array_index(delf, gdouble, 0);
									g_array_append_val(xsb, xf);
									{iv=g_array_index(stars, gdouble, j+(2*k*dx)); clc=g_array_index(stars, gdouble, (2*(k+1)*dx)-j);}
									{iv*=iv; clc*=clc;}
									iv=sqrt(iv+clc);
									if (mxy<iv) mxy=iv;
									g_array_append_val(ysb, iv);
								}
								g_array_append_val(sz, dx);
								dx2+=dx;
								g_array_append_val(nx, dx2);
							}
							(plt->sizes)=sz;
							(plt->ind)=nx;
							(plt->xdata)=xsb;
							(plt->ydata)=ysb;
						}
					}
				}
				else if ((flagd&1)!=0)
				{
					flagd^=1;
					if ((flags&2)!=0)/* narrow array to the one plot */
					{
						dx=g_array_index((plt->sizes), gint, 0);
						g_array_free(xsb, FALSE);
						g_array_free(ysb, FALSE);
						g_array_free(sz, FALSE);
						g_array_free(nx, FALSE);
						xsb=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), dx);
						ysb=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), dx);
						sz=g_array_new(FALSE, FALSE, sizeof(gint));
						nx=g_array_new(FALSE, FALSE, sizeof(gint));
						xf=0;
						g_array_append_val(xsb, xf);
						mxy=fabs(g_array_index(stars, gdouble, 2*jdim*dx));
						g_array_append_val(ysb, mxy);
						for (j=1; j<dx; j++)
						{
							xf=j*g_array_index(delf, gdouble, 0);
							g_array_append_val(xsb, xf);
							{iv=g_array_index(stars, gdouble, j+(2*jdim*dx)); clc=g_array_index(stars, gdouble, (2*(jdim+1)*dx)-j);}
							{iv*=iv; clc*=clc;}
							iv=sqrt(iv+clc);
							if (mxy<iv) mxy=iv;
							g_array_append_val(ysb, iv);
						}
						g_array_append_val(sz, dx);
						dx=0;
						g_array_append_val(nx, dx);
						(plt->sizes)=sz;
						(plt->ind)=nx;
						(plt->xdata)=xsb;
						(plt->ydata)=ysb;
					}
				}
				plot_linear_update_scale(plot2, xi, xf, mny, mxy);
				if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ck2))) flagd|=2;
				else if ((flagd&2)!=0) flagd^=2;
				if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ck3))) flagd|=4;
				else if ((flagd&4)!=0) flagd^=4;
			}
		}
		else
		{
			ck=gtk_check_button_new_with_label(_("Multiple plots for Inverse Domain"));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ck), (flagd&1));
			gtk_widget_show(ck);
			gtk_box_pack_start(GTK_BOX(vbox), ck, FALSE, FALSE, 2);
			gtk_container_add(GTK_CONTAINER(content), vbox);
			if (gtk_dialog_run(GTK_DIALOG(helpwin))==GTK_RESPONSE_APPLY)
			{
				(plt->xlab)=g_strdup(gtk_entry_get_text(GTK_ENTRY(entry1)));
				(plt->ylab)=g_strdup(gtk_entry_get_text(GTK_ENTRY(entry2)));
				(plt->lfsize)=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin1));
				(plt->afsize)=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin2));
				g_object_get(G_OBJECT(plot2), "xmin", &xi, "xmax", &xf, "ymin", &mny, "ymax", &mxy, NULL);
				if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ck)))
				{
					if ((flagd&1)==0)
					{
						flagd^=1;
						if ((flags&2)!=0)/* merge into the one array */
						{
							dx=((plt->ydata)->len);
							dx2=0;
							g_array_free(ysb, FALSE);
							g_array_free(nx, FALSE);
							ysb=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), dx*(jdimxf+1));
							nx=g_array_sized_new(FALSE, FALSE, sizeof(gint), dx*(jdimxf+1));
							mxy=fabs(g_array_index(stars, gdouble, 0));
							g_array_append_val(ysb, mxy);
							for (j=1; j<dx; j++)
							{
								{iv=g_array_index(stars, gdouble, j); clc=g_array_index(stars, gdouble, (2*dx)-j);}
								{iv*=iv; clc*=clc;}
								iv=sqrt(iv+clc);
								if (mxy<iv) mxy=iv;
								g_array_append_val(ysb, iv);
							}
							g_array_append_val(nx, dx2);
							for (k=1; k<=jdimxf; k++)
							{
								xf=0;
								g_array_append_val(xsb, xf);
								iv=fabs(g_array_index(stars, gdouble, 2*k*dx));
								g_array_append_val(ysb, iv);
								if (mxy<iv) mxy=iv;
								for (j=1; j<dx; j++)
								{
									xf=j*g_array_index(delf, gdouble, 0);
									g_array_append_val(xsb, xf);
									{iv=g_array_index(stars, gdouble, j+(2*k*dx)); clc=g_array_index(stars, gdouble, (2*(k+1)*dx)-j);}
									{iv*=iv; clc*=clc;}
									iv=sqrt(iv+clc);
									if (mxy<iv) mxy=iv;
									g_array_append_val(ysb, iv);
								}
								g_array_append_val(sz, dx);
								dx2+=dx;
								g_array_append_val(nx, dx2);
							}
							(plt->sizes)=sz;
							(plt->ind)=nx;
							(plt->xdata)=xsb;
							(plt->ydata)=ysb;
						}
					}
				}
				else if ((flagd&1)!=0)
				{
					flagd^=1;
					if ((flags&2)!=0)/* narrow array to the one plot */
					{
						dx=g_array_index((plt->sizes), gint, 0);
						g_array_free(xsb, FALSE);
						g_array_free(ysb, FALSE);
						g_array_free(sz, FALSE);
						g_array_free(nx, FALSE);
						xsb=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), dx);
						ysb=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), dx);
						sz=g_array_new(FALSE, FALSE, sizeof(gint));
						nx=g_array_new(FALSE, FALSE, sizeof(gint));
						xf=0;
						g_array_append_val(xsb, xf);
						mxy=fabs(g_array_index(stars, gdouble, 2*jdim*dx));
						g_array_append_val(ysb, mxy);
						for (j=1; j<dx; j++)
						{
							xf=j*g_array_index(delf, gdouble, 0);
							g_array_append_val(xsb, xf);
							{iv=g_array_index(stars, gdouble, j+(2*jdim*dx)); clc=g_array_index(stars, gdouble, (2*(jdim+1)*dx)-j);}
							{iv*=iv; clc*=clc;}
							iv=sqrt(iv+clc);
							if (mxy<iv) mxy=iv;
							g_array_append_val(ysb, iv);
						}
						g_array_append_val(sz, dx);
						dx=0;
						g_array_append_val(nx, dx);
						(plt->sizes)=sz;
						(plt->ind)=nx;
						(plt->xdata)=xsb;
						(plt->ydata)=ysb;
					}
				}
				plot_linear_update_scale(plot2, xi, xf, mny, mxy);
			}
		}
		gtk_widget_destroy(helpwin);
		break;
		default:
		table=gtk_table_new(4, 2, FALSE);
		gtk_widget_show(table);
		label=gtk_label_new(_("Text size:"));
		gtk_widget_show(label);
		gtk_table_attach(GTK_TABLE(table), label, 1, 2, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		label=gtk_label_new(_("Tick label size:"));
		gtk_widget_show(label);
		gtk_table_attach(GTK_TABLE(table), label, 1, 2, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		entry1=gtk_entry_new();
		entry2=gtk_entry_new();
		hsp=gtk_hseparator_new();
		gtk_widget_show(hsp);
		label=gtk_label_new(_("X axis text:"));
		gtk_widget_show(label);
		gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		label=gtk_label_new(_("Y axis text:"));
		gtk_widget_show(label);
		gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		plt=PLOT_LINEAR(plot1);
		gtk_entry_set_text(GTK_ENTRY(entry1), g_strdup(plt->xlab));
		gtk_entry_set_text(GTK_ENTRY(entry2), g_strdup(plt->ylab));
		adj1=(GtkAdjustment *) gtk_adjustment_new((plt->lfsize), 8, 64, 1.0, 5.0, 0.0);
		adj2=(GtkAdjustment *) gtk_adjustment_new((plt->afsize), 8, 64, 1.0, 5.0, 0.0);
		spin1=gtk_spin_button_new(adj1, 0, 0);
		spin2=gtk_spin_button_new(adj2, 0, 0);
		gtk_widget_show(entry1);
		gtk_widget_show(entry2);
		gtk_widget_show(spin1);
		gtk_widget_show(spin2);
		gtk_table_attach(GTK_TABLE(table), entry1, 0, 1, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		gtk_table_attach(GTK_TABLE(table), entry2, 0, 1, 3, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		gtk_table_attach(GTK_TABLE(table), spin1, 1, 2, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		gtk_table_attach(GTK_TABLE(table), spin2, 1, 2, 3, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
		gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE, 2);
		gtk_box_pack_start(GTK_BOX(vbox), hsp, FALSE, FALSE, 2);
		if ((flags&8)!=0)
		{
			ck2=gtk_check_button_new_with_label(_("Multiple plots for Results over index j"));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ck2), (flagd&2));
			gtk_widget_show(ck2);
			gtk_box_pack_start(GTK_BOX(vbox), ck2, FALSE, FALSE, 2);
			ck3=gtk_check_button_new_with_label(_("Multiple plots for Results over index k"));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ck3), (flagd&4));
			gtk_widget_show(ck3);
			gtk_box_pack_start(GTK_BOX(vbox), ck3, FALSE, FALSE, 2);
			gtk_container_add(GTK_CONTAINER(content), vbox);
			if (gtk_dialog_run(GTK_DIALOG(helpwin))==GTK_RESPONSE_APPLY)
			{
				(plt->xlab)=g_strdup(gtk_entry_get_text(GTK_ENTRY(entry1)));
				(plt->ylab)=g_strdup(gtk_entry_get_text(GTK_ENTRY(entry2)));
				(plt->lfsize)=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin1));
				(plt->afsize)=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin2));
				if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ck2)))
				{
					if ((flagd&2)==0)
					{
						flagd^=2;
					}
				}
				else if ((flagd&2)!=0)
				{
					flagd^=2;
				}
				if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ck3)))
				{
					if ((flagd&4)==0)
					{
						flagd^=4;
					}
				}
				else if ((flagd&4)!=0)
				{
					flagd^=4;
				}
				g_object_get(G_OBJECT(plot1), "xmin", &xi, "xmax", &xf, "ymin", &mny, "ymax", &mxy, NULL);
				plot_linear_update_scale(plot1, xi, xf, mny, mxy);
			}
		}
		else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(bat)))
		{
			ck=gtk_check_button_new_with_label(_("Multiple plots for Inverse Domain"));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ck), (flagd&1));
			gtk_widget_show(ck);
			gtk_box_pack_start(GTK_BOX(vbox), ck, FALSE, FALSE, 2);
			ck2=gtk_check_button_new_with_label(_("Multiple plots for Results over index j"));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ck2), (flagd&2));
			gtk_widget_show(ck2);
			gtk_box_pack_start(GTK_BOX(vbox), ck2, FALSE, FALSE, 2);
			ck3=gtk_check_button_new_with_label(_("Multiple plots for Results over index k"));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ck3), (flagd&4));
			gtk_widget_show(ck3);
			gtk_box_pack_start(GTK_BOX(vbox), ck3, FALSE, FALSE, 2);
			gtk_container_add(GTK_CONTAINER(content), vbox);
			if (gtk_dialog_run(GTK_DIALOG(helpwin))==GTK_RESPONSE_APPLY)
			{
				(plt->xlab)=g_strdup(gtk_entry_get_text(GTK_ENTRY(entry1)));
				(plt->ylab)=g_strdup(gtk_entry_get_text(GTK_ENTRY(entry2)));
				(plt->lfsize)=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin1));
				(plt->afsize)=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin2));
				if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ck)))
				{
					if ((flagd&1)==0)
					{
						flagd^=1;
						if ((flags&2)!=0)/* merge into the one array */
						{
							plt=PLOT_LINEAR(plot2);
							dx=((plt->ydata)->len);
							dx2=0;
							g_array_free(ysb, FALSE);
							g_array_free(nx, FALSE);
							ysb=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), dx*(jdimxf+1));
							nx=g_array_sized_new(FALSE, FALSE, sizeof(gint), dx*(jdimxf+1));
							mxy=fabs(g_array_index(stars, gdouble, 0));
							g_array_append_val(ysb, mxy);
							for (j=1; j<dx; j++)
							{
								{iv=g_array_index(stars, gdouble, j); clc=g_array_index(stars, gdouble, (2*dx)-j);}
								{iv*=iv; clc*=clc;}
								iv=sqrt(iv+clc);
								if (mxy<iv) mxy=iv;
								g_array_append_val(ysb, iv);
							}
							g_array_append_val(nx, dx2);
							for (k=1; k<=jdimxf; k++)
							{
								xf=0;
								g_array_append_val(xsb, xf);
								iv=fabs(g_array_index(stars, gdouble, 2*k*dx));
								g_array_append_val(ysb, iv);
								if (mxy<iv) mxy=iv;
								for (j=1; j<dx; j++)
								{
									xf=j*g_array_index(delf, gdouble, 0);
									g_array_append_val(xsb, xf);
									{iv=g_array_index(stars, gdouble, j+(2*k*dx)); clc=g_array_index(stars, gdouble, (2*(k+1)*dx)-j);}
									{iv*=iv; clc*=clc;}
									iv=sqrt(iv+clc);
									if (mxy<iv) mxy=iv;
									g_array_append_val(ysb, iv);
								}
								g_array_append_val(sz, dx);
								dx2+=dx;
								g_array_append_val(nx, dx2);
							}
							(plt->sizes)=sz;
							(plt->ind)=nx;
							(plt->xdata)=xsb;
							(plt->ydata)=ysb;
							plot_linear_update_scale_pretty(plot2, 0, xf, 0, mxy);
						}
					}
				}
				else if ((flagd&1)!=0)/* narrow array to one plot */
				{
					flagd^=1;
					if ((flags&2)!=0)
					{
						plt=PLOT_LINEAR(plot2);
						dx=g_array_index((plt->sizes), gint, 0);
						g_array_free(xsb, FALSE);
						g_array_free(ysb, FALSE);
						g_array_free(sz, FALSE);
						g_array_free(nx, FALSE);
						xsb=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), dx);
						ysb=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), dx);
						sz=g_array_new(FALSE, FALSE, sizeof(gint));
						nx=g_array_new(FALSE, FALSE, sizeof(gint));
						xf=0;
						g_array_append_val(xsb, xf);
						mxy=fabs(g_array_index(stars, gdouble, 2*jdim*dx));
						g_array_append_val(ysb, mxy);
						for (j=1; j<dx; j++)
						{
							xf=j*g_array_index(delf, gdouble, 0);
							g_array_append_val(xsb, xf);
							{iv=g_array_index(stars, gdouble, j+(2*jdim*dx)); clc=g_array_index(stars, gdouble, (2*(jdim+1)*dx)-j);}
							{iv*=iv; clc*=clc;}
							iv=sqrt(iv+clc);
							if (mxy<iv) mxy=iv;
							g_array_append_val(ysb, iv);
						}
						g_array_append_val(sz, dx);
						dx=0;
						g_array_append_val(nx, dx);
						(plt->sizes)=sz;
						(plt->ind)=nx;
						(plt->xdata)=xsb;
						(plt->ydata)=ysb;
						plot_linear_update_scale_pretty(plot2, 0, xf, 0, mxy);
					}
				}
				if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ck2))) flagd|=2;
				else if ((flagd&2)!=0) flagd^=2;
				if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ck3))) flagd|=4;
				else if ((flagd&4)!=0) flagd^=4; 
				g_object_get(G_OBJECT(plot1), "xmin", &xi, "xmax", &xf, "ymin", &mny, "ymax", &mxy, NULL);
				plot_linear_update_scale(plot1, xi, xf, mny, mxy);
			}
		}
		else
		{
			ck=gtk_check_button_new_with_label(_("Multiple plots for Inverse Domain"));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ck), (flagd&1));
			gtk_widget_show(ck);
			gtk_box_pack_start(GTK_BOX(vbox), ck, FALSE, FALSE, 2);
			gtk_container_add(GTK_CONTAINER(content), vbox);
			if (gtk_dialog_run(GTK_DIALOG(helpwin))==GTK_RESPONSE_APPLY)
			{
				(plt->xlab)=g_strdup(gtk_entry_get_text(GTK_ENTRY(entry1)));
				(plt->ylab)=g_strdup(gtk_entry_get_text(GTK_ENTRY(entry2)));
				(plt->lfsize)=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin1));
				(plt->afsize)=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin2));
				if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ck)))
				{
					if ((flagd&1)==0)
					{
						flagd^=1;
						if ((flags&2)!=0)/* merge into the one array */
						{
							plt=PLOT_LINEAR(plot2);
							dx=((plt->ydata)->len);
							dx2=0;
							g_array_free(ysb, FALSE);
							g_array_free(nx, FALSE);
							ysb=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), dx*(jdimxf+1));
							nx=g_array_sized_new(FALSE, FALSE, sizeof(gint), dx*(jdimxf+1));
							mxy=fabs(g_array_index(stars, gdouble, 0));
							g_array_append_val(ysb, mxy);
							for (j=1; j<dx; j++)
							{
								{iv=g_array_index(stars, gdouble, j); clc=g_array_index(stars, gdouble, (2*dx)-j);}
								{iv*=iv; clc*=clc;}
								iv=sqrt(iv+clc);
								if (mxy<iv) mxy=iv;
								g_array_append_val(ysb, iv);
							}
							g_array_append_val(nx, dx2);
							for (k=1; k<=jdimxf; k++)
							{
								xf=0;
								g_array_append_val(xsb, xf);
								iv=fabs(g_array_index(stars, gdouble, 2*k*dx));
								g_array_append_val(ysb, iv);
								if (mxy<iv) mxy=iv;
								for (j=1; j<dx; j++)
								{
									xf=j*g_array_index(delf, gdouble, 0);
									g_array_append_val(xsb, xf);
									{iv=g_array_index(stars, gdouble, j+(2*k*dx)); clc=g_array_index(stars, gdouble, (2*(k+1)*dx)-j);}
									{iv*=iv; clc*=clc;}
									iv=sqrt(iv+clc);
									if (mxy<iv) mxy=iv;
									g_array_append_val(ysb, iv);
								}
								g_array_append_val(sz, dx);
								dx2+=dx;
								g_array_append_val(nx, dx2);
							}
							(plt->sizes)=sz;
							(plt->ind)=nx;
							(plt->xdata)=xsb;
							(plt->ydata)=ysb;
							plot_linear_update_scale_pretty(plot2, 0, xf, 0, mxy);
						}
					}
				}
				else if ((flagd&1)!=0)
				{
					flagd^=1;
					if ((flags&2)!=0)/* narrow array to just one plot */
					{
						plt=PLOT_LINEAR(plot2);
						dx=g_array_index((plt->sizes), gint, 0);
						g_array_free(xsb, FALSE);
						g_array_free(ysb, FALSE);
						g_array_free(sz, FALSE);
						g_array_free(nx, FALSE);
						xsb=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), dx);
						ysb=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), dx);
						sz=g_array_new(FALSE, FALSE, sizeof(gint));
						nx=g_array_new(FALSE, FALSE, sizeof(gint));
						xf=0;
						g_array_append_val(xsb, xf);
						mxy=fabs(g_array_index(stars, gdouble, 2*jdim*dx));
						g_array_append_val(ysb, mxy);
						for (j=1; j<dx; j++)
						{
							xf=j*g_array_index(delf, gdouble, 0);
							g_array_append_val(xsb, xf);
							{iv=g_array_index(stars, gdouble, j+(2*jdim*dx)); clc=g_array_index(stars, gdouble, (2*(jdim+1)*dx)-j);}
							{iv*=iv; clc*=clc;}
							iv=sqrt(iv+clc);
							if (mxy<iv) mxy=iv;
							g_array_append_val(ysb, iv);
						}
						g_array_append_val(sz, dx);
						dx=0;
						g_array_append_val(nx, dx);
						(plt->sizes)=sz;
						(plt->ind)=nx;
						(plt->xdata)=xsb;
						(plt->ydata)=ysb;
						plot_linear_update_scale_pretty(plot2, 0, xf, 0, mxy);
					}
				}
				g_object_get(G_OBJECT(plot1), "xmin", &xi, "xmax", &xf, "ymin", &mny, "ymax", &mxy, NULL);
				plot_linear_update_scale(plot1, xi, xf, mny, mxy);
			}
		}
		gtk_widget_destroy(helpwin);
		break;
	}
}

void static prt(GtkWidget *widget, gpointer data)
{
	GtkWidget *wfile;
	GtkFileFilter *filter;
	gchar *str, *fout=NULL;

	switch (gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook2)))
	{
		case 2:
		if ((flags&12)==12)
		{ /* need to add polar capability */
			wfile=gtk_file_chooser_dialog_new(_("Select Image File"), GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
			g_signal_connect(G_OBJECT(wfile), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(wfile));
			gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(wfile), TRUE);
			filter=gtk_file_filter_new();
			gtk_file_filter_set_name(filter, "Encapsulated Postscript (EPS)");
			gtk_file_filter_add_pattern(filter, "*.eps");
			gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(wfile), filter);
			if (gtk_dialog_run(GTK_DIALOG(wfile))==GTK_RESPONSE_ACCEPT)
			{
				fout=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wfile));
				plot_linear_print_eps(plot3, fout);
				g_free(fout);
			}
			gtk_widget_destroy(wfile);
		}
		else if ((flags&2)!=0)
		{
			wfile=gtk_file_chooser_dialog_new(_("Select Image File"), GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
			g_signal_connect(G_OBJECT(wfile), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(wfile));
			gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(wfile), TRUE);
			filter=gtk_file_filter_new();
			gtk_file_filter_set_name(filter, "Encapsulated Postscript (EPS)");
			gtk_file_filter_add_pattern(filter, "*.eps");
			gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(wfile), filter);
			if (gtk_dialog_run(GTK_DIALOG(wfile))==GTK_RESPONSE_ACCEPT)
			{
				fout=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wfile));
				plot_linear_print_eps(plot2, fout);
				g_free(fout);
			}
			gtk_widget_destroy(wfile);
		}
		else if ((flags&1)!=0)
		{
			wfile=gtk_file_chooser_dialog_new(_("Select Image File"), GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
			g_signal_connect(G_OBJECT(wfile), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(wfile));
			gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(wfile), TRUE);
			filter=gtk_file_filter_new();
			gtk_file_filter_set_name(filter, "Encapsulated Postscript (EPS)");
			gtk_file_filter_add_pattern(filter, "*.eps");
			gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(wfile), filter);
			if (gtk_dialog_run(GTK_DIALOG(wfile))==GTK_RESPONSE_ACCEPT)
			{
				fout=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wfile));
				plot_linear_print_eps(plot1, fout);
				g_free(fout);
			}
			gtk_widget_destroy(wfile);
		}
		else
		{
			gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook2), 0);
			str=g_strdup(_("No available image."));
			gtk_statusbar_push(GTK_STATUSBAR(statusbar), gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), str), str);
			g_free(str);
		}
		break;
		case 1:
		if ((flags&2)!=0)
		{
			wfile=gtk_file_chooser_dialog_new(_("Select Image File"), GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
			g_signal_connect(G_OBJECT(wfile), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(wfile));
			gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(wfile), TRUE);
			filter=gtk_file_filter_new();
			gtk_file_filter_set_name(filter, "Encapsulated Postscript (EPS)");
			gtk_file_filter_add_pattern(filter, "*.eps");
			gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(wfile), filter);
			if (gtk_dialog_run(GTK_DIALOG(wfile))==GTK_RESPONSE_ACCEPT)
			{
				fout=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wfile));
				plot_linear_print_eps(plot2, fout);
				g_free(fout);
			}
			gtk_widget_destroy(wfile);
		}
		else if ((flags&1)!=0)
		{
			wfile=gtk_file_chooser_dialog_new(_("Select Image File"), GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
			g_signal_connect(G_OBJECT(wfile), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(wfile));
			gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(wfile), TRUE);
			filter=gtk_file_filter_new();
			gtk_file_filter_set_name(filter, "Encapsulated Postscript (EPS)");
			gtk_file_filter_add_pattern(filter, "*.eps");
			gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(wfile), filter);
			if (gtk_dialog_run(GTK_DIALOG(wfile))==GTK_RESPONSE_ACCEPT)
			{
				fout=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wfile));
				plot_linear_print_eps(plot1, fout);
				g_free(fout);
			}
			gtk_widget_destroy(wfile);
		}
		else
		{
			gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook2), 0);
			str=g_strdup(_("No available image."));
			gtk_statusbar_push(GTK_STATUSBAR(statusbar), gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), str), str);
			g_free(str);
		}
		break;
		default:
		if ((flags&1)!=0)
		{
			wfile=gtk_file_chooser_dialog_new(_("Select Image File"), GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
			g_signal_connect(G_OBJECT(wfile), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(wfile));
			gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(wfile), TRUE);
			filter=gtk_file_filter_new();
			gtk_file_filter_set_name(filter, "Encapsulated Postscript (EPS)");
			gtk_file_filter_add_pattern(filter, "*.eps");
			gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(wfile), filter);
			if (gtk_dialog_run(GTK_DIALOG(wfile))==GTK_RESPONSE_ACCEPT)
			{
				fout=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wfile));
				plot_linear_print_eps(plot1, fout);
				g_free(fout);
			}
			gtk_widget_destroy(wfile);
		}
		else
		{
			str=g_strdup(_("No available image."));
			gtk_statusbar_push(GTK_STATUSBAR(statusbar), gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), str), str);
			g_free(str);
		}
		break;
	}
}

void static upg(GtkWidget *widget, gpointer data)
{
	PlotLinear *plt;
	gdouble dt, xi, xf, mny, mxy;
	gint j, sz;
	GSList *list;

	trc=satl;
	list=group2;
	while (list)
	{
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(list->data))) break;
		list=list->next;
		trc--;
	}
	plt=PLOT_LINEAR(plot1);
	sz=g_array_index((plt->sizes), gint, 0);/* adjust accordingly with multitrace considerations*/
	g_array_free(yb, TRUE);
	yb=g_array_new(FALSE, FALSE, sizeof(gdouble));
	dt=g_array_index(specs, gdouble, trc-1);
	mny=dt;
	mxy=dt;
	g_array_append_val(yb, dt);
	for (j=1; j<sz; j++)
	{
		dt=g_array_index(specs, gdouble, trc-1+(j*satl));
		g_array_append_val(yb, dt);
		if (dt<mny) mny=dt;
		else if (dt>mxy) mxy=dt;
	}
	(plt->ydata)=yb;
	xi=g_array_index(x, gdouble, 0);
	xf=g_array_index(x, gdouble, (lc-1));
	plot_linear_update_scale(plot1, xi, xf, mny, mxy);
}

void static pltmv(PlotLinear *plot, gpointer data)
{
	gchar *str;

	str=g_strdup_printf(_("x: %f, y: %f"), (plot->xps), (plot->yps));
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), str), str);
	g_free(str);
}

void static opd(GtkWidget *widget, gpointer data)
{
	PlotLinear *plt;
	/*
	 PlotPolarBoth *plt2;
	 */
	GtkWidget *wfile, *dialog, *cont, *trace, *table, *label;
	gdouble xi, xf, lcl, mny, mxy, idelf, iv, vzt, vt, ivd, ivdt, tcn, twd, phi, phio, phia, dst, ddp, pn, cn, tp, ct;
	guint j, k, l, m, sal, st, sp, kib;
	gint n, zp, lcib;
	gchar s[5];
	gchar *contents, *contents2, *str, *fin=NULL;
	gchar **strary, **strary2, **strat, **strat2;
	GSList *list;
	GError *Err=NULL;

	wfile=gtk_file_chooser_dialog_new(_("Select Data File"), GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	g_signal_connect(G_OBJECT(wfile), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(wfile));
	if (gtk_dialog_run(GTK_DIALOG(wfile))==GTK_RESPONSE_ACCEPT)
	{
		fin=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wfile));
		if (g_file_get_contents(fin, &contents, NULL, &Err))
		{
			strary=g_strsplit_set(contents, "\r\n", 0);
			sal=g_strv_length(strary);
			g_array_free(x, TRUE);
			g_array_free(yb, TRUE);
			g_array_free(specs, TRUE);
			if ((flags&8)!=0)
			{
				gtk_notebook_remove_page(GTK_NOTEBOOK(notebook2), 2);
				rest=gtk_table_new(4, 2, FALSE);
				gtk_widget_show(rest);
				label=gtk_label_new(_("Visibility"));
				gtk_table_attach(GTK_TABLE(rest), label, 0, 1, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
				gtk_widget_show(label);
				visl=gtk_label_new("");
				gtk_table_attach(GTK_TABLE(rest), visl, 0, 1, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
				gtk_widget_show(visl);
				label=gtk_label_new(_("Domain Shift"));
				gtk_table_attach(GTK_TABLE(rest), label, 1, 2, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
				gtk_widget_show(label);
				dsl=gtk_label_new("");
				gtk_table_attach(GTK_TABLE(rest), dsl, 1, 2, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
				gtk_widget_show(dsl);
				label=gtk_label_new(_("Analysis Results"));
				gtk_notebook_append_page(GTK_NOTEBOOK(notebook2), rest, label);
				flags^=8;
			}
			flags|=1;
			x=g_array_new(FALSE, FALSE, sizeof(gdouble));
			yb=g_array_new(FALSE, FALSE, sizeof(gdouble));
			specs=g_array_new(FALSE, FALSE, sizeof(gdouble));
			if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(anosa))) {k=2; lc=-1;}
			else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(sws))) {k=0; lc=-1;}
			else {k=0; lc=0;}
			while (k<sal)
			{
				if (!strary[k]) {k++; continue;}
				g_strchug(strary[k]);
				if (!g_strcmp0("", strary[k])) {k++; continue;}
				if (!(g_ascii_isdigit(strary[k][0])|(g_str_has_prefix(strary[k],"-")))) {k++; continue;}
				if (lc<0) {lc++; k++; continue;}
				strat=g_strsplit_set(strary[k], "\t,", 0);
				lcl=g_ascii_strtod(g_strstrip(strat[0]), NULL);
				g_array_append_val(x, lcl);
				if (lc==0)
				{
					satl=g_strv_length(strat);
					if (!strat[1]) lcl=0;
					else lcl=g_ascii_strtod(g_strstrip(strat[1]), NULL);
					mny=lcl;
					mxy=lcl;
				}
				else
				{
					if (!strat[1]) lcl=0;
					else lcl=g_ascii_strtod(g_strstrip(strat[1]), NULL);
					if (lcl<mny) mny=lcl;
					else if (lcl>mxy) mxy=lcl;
				}
				g_array_append_val(specs, lcl);
				g_array_append_val(yb, lcl);
				for (l=2; l<satl; l++)
				{
					if (!strat[l]) lcl=0;
					else lcl=g_ascii_strtod(g_strstrip(strat[l]), NULL);
					g_array_append_val(specs, lcl);
				}
				g_strfreev(strat);
				lc++;
				k++;
			}
			g_strfreev(strary);
			satl--;
			j=g_slist_length(group2);
			while (j<satl)
			{
				j++;
				g_snprintf(s, 4, "%d", j);
				trace=gtk_radio_menu_item_new_with_label(group2, s);
				group2=gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(trace));
				g_signal_connect(G_OBJECT(trace), "toggled", G_CALLBACK(upg), NULL);
				gtk_menu_shell_append(GTK_MENU_SHELL(tracmenu), trace);
				gtk_widget_show(trace);
			}
			while (j>satl)
			{
				list=group2->next;
				gtk_widget_destroy(group2->data);
				group2=list;
				j--;
			}
			gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
			gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook2), 0);
			g_signal_connect(G_OBJECT(tr), "clicked", G_CALLBACK(trs), NULL);
			if (g_signal_handler_is_connected(G_OBJECT(pr), pr_id)) g_signal_handler_disconnect(G_OBJECT(pr), pr_id);
			str=g_strdup_printf(_("File: %s successfully loaded"), fin);
			gtk_statusbar_push(GTK_STATUSBAR(statusbar), gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), str), str);
			g_free(str);
			plt=PLOT_LINEAR(plot1);
			sz=g_array_new(FALSE, FALSE, sizeof(gint));
			nx=g_array_new(FALSE, FALSE, sizeof(gint));
			g_array_append_val(sz, lc);/* adjust if multiple traces desired */
			(plt->sizes)=sz;
			zp=0;
			g_array_append_val(nx, zp);
			(plt->ind)=nx;
			(plt->xdata)=x;
			(plt->ydata)=yb;
			xi=g_array_index(x, gdouble, 0);
			xf=g_array_index(x, gdouble, (lc-1));
			plot_linear_update_scale_pretty(plot1, xi, xf, mny, mxy);
		}
		else
		{
			str=g_strdup_printf(_("Loading failed for file: %s, Error: %s"), fin, (gchar *) Err);
			gtk_statusbar_push(GTK_STATUSBAR(statusbar), gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), str), str);
			g_free(str);
			g_error_free(Err);
		}
		g_free(contents);
		g_free(fin);
	}
	gtk_widget_destroy(wfile);
}

void static upj(GtkWidget *widget, gpointer data)
{
	/*
	 * Checks if j index spinner has increased to a new value and fills parameter array values accordingly
	 * updates front panel to display parameters for new j,k values
	 * If transform has been performed and in single plot mode, changes the graph in plot 2
	 * If processing has been performed, updates the displayed value/plot
	 */
	PlotLinear *plt2, *plt3;
	guint j, k, sz2;
	gdouble num, num2, num3, num4, num5, num6, num7;
	gdouble *ptr;
	gchar s[10];

	jdim=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
	if (jdim>jdimx)
	{
		num=gtk_spin_button_get_value(GTK_SPIN_BUTTON(isr));
		num2=gtk_spin_button_get_value(GTK_SPIN_BUTTON(isp));
		num3=gtk_spin_button_get_value(GTK_SPIN_BUTTON(tc));
		num4=gtk_spin_button_get_value(GTK_SPIN_BUTTON(tw));
		num5=gtk_spin_button_get_value(GTK_SPIN_BUTTON(zw));
		num6=gtk_spin_button_get_value(GTK_SPIN_BUTTON(bsr));
		num7=gtk_spin_button_get_value(GTK_SPIN_BUTTON(bsp));
		for (j=(jdimx+1); j<=jdim; j++)
		{
			for (k=0; k<kdimx; k++)
			{
				ptr=&g_array_index(isra, gdouble, j+(k*MXD));
				*ptr=num;
				ptr=&g_array_index(ispa, gdouble, j+(k*MXD));
				*ptr=num2;
				ptr=&g_array_index(tca, gdouble, j+(k*MXD));
				*ptr=num3;
				ptr=&g_array_index(twa, gdouble, j+(k*MXD));
				*ptr=num4;
			}
			g_array_append_val(isra, num);
			g_array_append_val(ispa, num2);
			g_array_append_val(tca, num3);
			g_array_append_val(twa, num4);
			g_array_append_val(zwa, num5);
			g_array_append_val(bsra, num6);
			g_array_append_val(bspa, num7);
		}
		jdimx=jdim;
	}
	else
	{
		num=g_array_index(isra, gdouble, (jdim+(kdim*MXD)));
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(isr), num);
		num=g_array_index(ispa, gdouble, (jdim+(kdim*MXD)));
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(isp), num);
		num=g_array_index(tca, gdouble, (jdim+(kdim*MXD)));
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(tc), num);
		num=g_array_index(twa, gdouble, (jdim+(kdim*MXD)));
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(tw), num);
		num=g_array_index(zwa, gdouble, jdim);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(zw), num);
		num6=g_array_index(bsra, gdouble, jdim);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(bsr), num6);
		num6=g_array_index(bspa, gdouble, jdim);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(bsp), num6);
	}
	if (jdim<=jdimxf)
	{
		if (((flags&2)!=0)&&((flagd&1)==0))
		{
			plt2=PLOT_LINEAR(plot2);
			sz2=g_array_index((plt2->sizes), gint, 0);/* check if this works with desired multitrace capability */
			g_array_free(xsb, TRUE);
			g_array_free(ysb, TRUE);
			xsb=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), sz2);
			ysb=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), sz2);
			for (j=0; j<sz2; j++)
			{
				num=j*g_array_index(delf, gdouble, jdim);
				g_array_append_val(xsb, num);
			}
			j*=2*(jdim);
			num2=fabs(g_array_index(stars, gdouble, j));
			g_array_append_val(ysb, num2);
			while (j<(((2*jdim)+1)*sz2))
			{
				num6=g_array_index(stars, gdouble, j);
				num6*=num6;
				num7=g_array_index(stars, gdouble, (2*sz2)-j);
				num7*=num7;
				num6+=num7;
				num6=sqrt(num6);
				if (num2<num6) num2=num6;
				g_array_append_val(ysb, num6);
				j++;
			}
			(plt2->xdata)=xsb;
			(plt2->ydata)=ysb;
			plot_linear_update_scale_pretty(plot2, 0, num, 0, num2);
		}
		if ((flags&4)!=0)
		{
			if ((flags&8)!=0)
			{
				if ((flagd&2)==0)
				{
					if ((flagd&4)==0)
					{
						if ((flags&32)!=0)
						{ /* change polar plot3 */
						}
						else
						{ /* change plot3 */
						}
					}
					else/* multiplot over k */
					{
						if ((flags&32)!=0)
						{ /* change polar plot3 */
						}
						else
						{ /* change plot3 */
						}
					}
				}
			}
			else
			{
				num=g_array_index(vis, gdouble, (jdim+(kdim*MXD)));
				g_snprintf(s, 7, "%f", num);
				gtk_label_set_text(GTK_LABEL(visl), s);
				num=g_array_index(doms, gdouble, (jdim+(kdim*MXD)));
				g_snprintf(s, 9, "%f", num);
				gtk_label_set_text(GTK_LABEL(dsl), s);
				if ((flags&16)!=0)
				{
					num=g_array_index(chp, gdouble, (jdim+(kdim*MXD)));
					g_snprintf(s, 8, "%f", num);
					gtk_label_set_text(GTK_LABEL(chil), s);
				}
			}
		}
	}
}

void static upk(GtkWidget *widget, gpointer data)
{
	/*
	 * Checks if k index spinner has increased to a new value and fills parameter array values accordingly
	 * updates front panel to display parameters for new j,k values
	 * If processing has been performed, updates the displayed value/plot
	 */
	PlotLinear *plt3;
	guint j;
	gdouble num, num2, num3, num4;
	gchar s[10];

	kdim=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
	if (kdim>kdimx)
	{
		num=gtk_spin_button_get_value(GTK_SPIN_BUTTON(isr));
		num2=gtk_spin_button_get_value(GTK_SPIN_BUTTON(isp));
		num3=gtk_spin_button_get_value(GTK_SPIN_BUTTON(tc));
		num4=gtk_spin_button_get_value(GTK_SPIN_BUTTON(tw));
		for (j=0; j<(MXD*(kdim-kdimx)); j++)
		{
			g_array_append_val(isra, num);
			g_array_append_val(ispa, num2);
			g_array_append_val(tca, num3);
			g_array_append_val(twa, num4);
		}
		kdimx=kdim;
	}
	else
	{
		num=g_array_index(isra, gdouble, (jdim+(kdim*MXD)));
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(isr), num);
		num=g_array_index(ispa, gdouble, (jdim+(kdim*MXD)));
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(isp), num);
		num=g_array_index(tca, gdouble, (jdim+(kdim*MXD)));
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(tc), num);
		num=g_array_index(twa, gdouble, (jdim+(kdim*MXD)));
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(tw), num);
	}
	if (kdim<=kdimxf)
	{
		if ((flags&4)!=0)
		{
			if ((flags&8)!=0)
			{
				if ((flagd&4)==0)
				{
					if ((flagd&2)==0)
					{
						if ((flags&32)!=0)
						{ /* change polar plot3 */
						}
						else
						{ /* change plot3 */
						}
					}
					else/* multiplot over j */
					{
						if ((flags&32)!=0)
						{ /* change polar plot3 */
						}
						else
						{ /* change plot3 */
						}
					}
				}
			}
			else
			{
				num=g_array_index(vis, gdouble, (jdim+(kdim*MXD)));
				g_snprintf(s, 7, "%f", num);
				gtk_label_set_text(GTK_LABEL(visl), s);
				num=g_array_index(doms, gdouble, (jdim+(kdim*MXD)));
				g_snprintf(s, 9, "%f", num);
				gtk_label_set_text(GTK_LABEL(dsl), s);
				if ((flags&16)!=0)
				{
					num=g_array_index(chp, gdouble, (jdim+(kdim*MXD)));
					g_snprintf(s, 8, "%f", num);
					gtk_label_set_text(GTK_LABEL(chil), s);
				}
			}
		}
	}
}

void static upa2(GtkWidget *widget, gpointer data)
{
	gdouble *ptr;

	ptr=&g_array_index((GArray*)data, gdouble, jdim+(kdim*MXD));
	*ptr=gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
}

void static upa1(GtkWidget *widget, gpointer data)
{
	gdouble *ptr;

	ptr=&g_array_index((GArray*)data, gdouble, jdim);
	*ptr=gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
}

void static reset(GtkWidget *widget, gpointer data)
{
	gdouble num;

	num=g_array_index(bsra, gdouble, 0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(bsr), num);
	num=g_array_index(bspa, gdouble, 0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(bsp), num);
	g_array_free(bsra, TRUE);
	g_array_free(bspa, TRUE);
	g_array_free(zwa, TRUE);
	g_array_free(isra, TRUE);
	g_array_free(ispa, TRUE);
	g_array_free(tca, TRUE);
	g_array_free(twa, TRUE);
	jdim=0;
	jdimx=0;
	kdim=0;
	kdimx=0;
	num=0;
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(jind), 0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(kind), 0);
	bsra=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXD);
	g_array_append_val(bspa, num);
	bspa=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXD);
	g_array_append_val(bsra, num);
	zwa=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXD);
	isra=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXDS);
	ispa=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXDS);
	tca=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXDS);
	twa=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXDS);
}

void static reset2(GtkWidget *widget, gpointer data)
{
	gdouble num, num2, num3, num4, num5;
	guint j;

	num=g_array_index(isra, gdouble, 0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(isr), num);
	num2=g_array_index(ispa, gdouble, 0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(isp), num2);
	num3=g_array_index(tca, gdouble, 0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(tc), num3);
	num4=g_array_index(twa, gdouble, 0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(tw), num4);
	num5=g_array_index(zwa, gdouble, 0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(zw), num5);
	g_array_free(zwa, TRUE);
	g_array_free(isra, TRUE);
	g_array_free(ispa, TRUE);
	g_array_free(tca, TRUE);
	g_array_free(twa, TRUE);
	jdim=0;
	kdim=0;
	kdimx=0;
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(jind2), 0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(kind), 0);
	zwa=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXD);
	isra=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXDS);
	ispa=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXDS);
	tca=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXDS);
	twa=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXDS);
	for (j=0; j<=jdimx; j++)
	{
		g_array_append_val(isra, num);
		g_array_append_val(ispa, num2);
		g_array_append_val(tca, num3);
		g_array_append_val(twa, num4);
		g_array_append_val(zwa, num5);
	}
}

int main(int argc, char *argv[])
{
	GtkAdjustment *adj;
	GtkWidget *vbox, *mnb, *mnu, *smnu, *mni, *hpane, *table, *label, *butt;
	GtkAccelGroup *accel_group=NULL;
	GSList *group=NULL, *group3=NULL;
	gdouble fll=0;

	bindtextdomain(PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);
	gtk_init(&argc, &argv);
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), _("Inverse Scattering"));
	g_signal_connect_swapped(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	vbox=gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show(vbox);
	mnb=gtk_menu_bar_new();
	gtk_box_pack_start(GTK_BOX(vbox), mnb, FALSE, FALSE, 2);
	gtk_widget_show(mnb);
	mnu=gtk_menu_new();
	accel_group=gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
	mni=gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, NULL);
	gtk_widget_add_accelerator(mni, "activate", accel_group, GDK_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(opd), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE, NULL);
	gtk_widget_add_accelerator(mni, "activate", accel_group, GDK_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(sav), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_image_menu_item_new_from_stock(GTK_STOCK_PRINT, NULL);
	gtk_widget_add_accelerator(mni, "activate", accel_group, GDK_p, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(prt), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, accel_group);
	gtk_widget_add_accelerator(mni, "activate", accel_group, GDK_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(gtk_main_quit), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_menu_item_new_with_mnemonic(_("_File"));
	gtk_widget_show(mni);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mni), mnu);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnb), mni);
	mnu=gtk_menu_new();
	smnu=gtk_menu_new();
	agosa=gtk_radio_menu_item_new_with_label(group, "Agilent OSA");
	group=gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(agosa));
	gtk_menu_shell_append(GTK_MENU_SHELL(smnu), agosa);
	gtk_widget_show(agosa);
	agtl=gtk_radio_menu_item_new_with_label(group, "Agilent Tunable Laser");
	group=gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(agtl));
	gtk_menu_shell_append(GTK_MENU_SHELL(smnu), agtl);
	gtk_widget_show(agtl);
	anosa=gtk_radio_menu_item_new_with_label(group, "Ando OSA");
	group=gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(anosa));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(anosa), TRUE);
	gtk_menu_shell_append(GTK_MENU_SHELL(smnu), anosa);
	gtk_widget_show(anosa);
	sws=gtk_radio_menu_item_new_with_label(group, "JDSUniphase Swept Wavelength System");
	group=gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(sws));
	gtk_menu_shell_append(GTK_MENU_SHELL(smnu), sws);
	gtk_widget_show(sws);
	dlm=gtk_radio_menu_item_new_with_label(group, _("Raw Delimited Data"));
	group=gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(dlm));
	gtk_menu_shell_append(GTK_MENU_SHELL(smnu), dlm);
	gtk_widget_show(dlm);
	mni=gtk_menu_item_new_with_label(_("Input Data Format:"));
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mni), smnu);
	tracmenu=gtk_menu_new();
	trac=gtk_menu_item_new_with_label(_("Trace:"));
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), trac);
	gtk_widget_show(trac);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(trac), tracmenu);
	dBs=gtk_check_menu_item_new_with_label(_("Data in dBs?"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(dBs), TRUE);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), dBs);
	gtk_widget_show(dBs);
	neg=gtk_check_menu_item_new_with_label(_("Negate?"));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(neg), FALSE);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), neg);
	gtk_widget_show(neg);
	mni=gtk_menu_item_new_with_label(_("Display Properties:"));
	gtk_widget_add_accelerator(mni, "activate", accel_group, GDK_F2, 0, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(dpr), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_menu_item_new_with_mnemonic(_("_Properties"));
	gtk_widget_show(mni);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mni), mnu);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnb), mni);
	mnu=gtk_menu_new();
	mni=gtk_menu_item_new_with_label(_("Instructions"));
	gtk_widget_add_accelerator(mni, "activate", accel_group, GDK_F1, 0, GTK_ACCEL_VISIBLE);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(help), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, NULL);
	g_signal_connect(G_OBJECT(mni), "activate", G_CALLBACK(about), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnu), mni);
	gtk_widget_show(mni);
	mni=gtk_menu_item_new_with_mnemonic(_("_Help"));
	gtk_widget_show(mni);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mni), mnu);
	gtk_menu_shell_append(GTK_MENU_SHELL(mnb), mni);
	gtk_menu_item_right_justify(GTK_MENU_ITEM(mni));
	bsra=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXD);
	g_array_append_val(bsra, fll);
	bspa=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXD);
	g_array_append_val(bspa, fll);
	zwa=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXD);
	isra=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXDS);
	ispa=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXDS);
	tca=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXDS);
	twa=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXDS);
	hpane=gtk_hpaned_new();
	gtk_widget_show(hpane);
	gtk_container_add(GTK_CONTAINER(vbox), hpane);
	notebook=gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
	table=gtk_table_new(5, 3, FALSE);
	gtk_widget_show(table);
	label=gtk_label_new(_("Spectrum Start:"));
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(label);
	adj=(GtkAdjustment *) gtk_adjustment_new(0, -32767, 32768, 1.0, 5.0, 0.0);
	bsr=gtk_spin_button_new(adj, 0.5, 3);
	g_signal_connect(G_OBJECT(bsr), "value-changed", G_CALLBACK(upa1), (gpointer) bsra);
	gtk_table_attach(GTK_TABLE(table), bsr, 0, 1, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(bsr);
	label=gtk_label_new(_("Spectrum Stop:"));
	gtk_table_attach(GTK_TABLE(table), label, 1, 2, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(label);
	adj=(GtkAdjustment *) gtk_adjustment_new(0, -32767, 32768, 1.0, 5.0, 0.0);
	bsp=gtk_spin_button_new(adj, 0.5, 3);
	g_signal_connect(G_OBJECT(bsp), "value-changed", G_CALLBACK(upa2), (gpointer) bspa);
	gtk_table_attach(GTK_TABLE(table), bsp, 1, 2, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(bsp);
	label=gtk_label_new(_("Offset:"));
	gtk_table_attach(GTK_TABLE(table), label, 1, 2, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(label);
	adj=(GtkAdjustment *) gtk_adjustment_new(0, -32767, 32768, 1.0, 5.0, 0.0);
	fst=gtk_spin_button_new(adj, 0.5, 2);
	gtk_table_attach(GTK_TABLE(table), fst, 1, 2, 3, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(fst);
	label=gtk_label_new(_("j index:"));
	gtk_table_attach(GTK_TABLE(table), label, 2, 3, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(label);
	adj=(GtkAdjustment *) gtk_adjustment_new(0, 0, MXDM, 1.0, 5.0, 0.0);
	jind=gtk_spin_button_new(adj, 0, 0);
	g_signal_connect(G_OBJECT(jind), "value-changed", G_CALLBACK(upj), NULL);
	gtk_table_attach(GTK_TABLE(table), jind, 2, 3, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(jind);
	label=gtk_label_new(_("Zero Padding 2^:"));
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(label);
	adj=(GtkAdjustment *) gtk_adjustment_new(12, 0, 31, 1.0, 5.0, 0.0);
	zpd=gtk_spin_button_new(adj, 0, 0);
	gtk_table_attach(GTK_TABLE(table), zpd, 0, 1, 3, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(zpd);
	butt=gtk_button_new_with_label(_("Reset\nArrays"));
	g_signal_connect(G_OBJECT(butt), "clicked", G_CALLBACK(reset), NULL);
	gtk_table_attach(GTK_TABLE(table), butt, 2, 3, 2, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(butt);
	tr=gtk_button_new_with_label(_("Reconstruct Profile"));
	gtk_table_attach(GTK_TABLE(table), tr, 0, 3, 4, 5, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(tr);
	label=gtk_label_new(_("IS control"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), table, label);
	table=gtk_table_new(6, 3, FALSE);
	gtk_widget_show(table);
	label=gtk_label_new(_("Inverse Spectrum Start:"));
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(label);
	adj=(GtkAdjustment *) gtk_adjustment_new(0, -32767, 32768, 1.0, 5.0, 0.0);
	isr=gtk_spin_button_new(adj, 0.5, 3);
	g_signal_connect(G_OBJECT(isr), "value-changed", G_CALLBACK(upa2), (gpointer) isra);
	gtk_table_attach(GTK_TABLE(table), isr, 0, 1, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(isr);
	label=gtk_label_new(_("Inverse Spectrum Stop:"));
	gtk_table_attach(GTK_TABLE(table), label, 1, 2, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(label);
	adj=(GtkAdjustment *) gtk_adjustment_new(0, -32767, 32768, 1.0, 5.0, 0.0);
	isp=gtk_spin_button_new(adj, 0.5, 3);
	g_signal_connect(G_OBJECT(isp), "value-changed", G_CALLBACK(upa2), (gpointer) ispa);
	gtk_table_attach(GTK_TABLE(table), isp, 1, 2, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(isp);
	label=gtk_label_new(_("Triangle Centre:"));
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(label);
	adj=(GtkAdjustment *) gtk_adjustment_new(0, -32767, 32768, 1.0, 5.0, 0.0);
	tc=gtk_spin_button_new(adj, 0.5, 3);
	g_signal_connect(G_OBJECT(tc), "value-changed", G_CALLBACK(upa2), (gpointer) tca);
	gtk_table_attach(GTK_TABLE(table), tc, 0, 1, 3, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(tc);
	label=gtk_label_new(_("Triangle Full Width:"));
	gtk_table_attach(GTK_TABLE(table), label, 1, 2, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(label);
	adj=(GtkAdjustment *) gtk_adjustment_new(2, 0, 65535, 1.0, 5.0, 0.0);
	tw=gtk_spin_button_new(adj, 0.5, 3);
	g_signal_connect(G_OBJECT(tw), "value-changed", G_CALLBACK(upa2), (gpointer) twa);
	gtk_table_attach(GTK_TABLE(table), tw, 1, 2, 3, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(tw);
	label=gtk_label_new(_("DC Peak Width:"));
	gtk_table_attach(GTK_TABLE(table), label, 1, 2, 4, 5, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(label);
	adj=(GtkAdjustment *) gtk_adjustment_new(2, 0, 65535, 1.0, 5.0, 0.0);
	zw=gtk_spin_button_new(adj, 0.5, 3);
	g_signal_connect(G_OBJECT(zw), "value-changed", G_CALLBACK(upa1), (gpointer) zwa);
	gtk_table_attach(GTK_TABLE(table), zw, 1, 2, 5, 6, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(zw);
	label=gtk_label_new(_("j index:"));
	gtk_table_attach(GTK_TABLE(table), label, 2, 3, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(label);
	adj=(GtkAdjustment *) gtk_adjustment_new(0, 0, 0, 1.0, 5.0, 0.0);
	jind2=gtk_spin_button_new(adj, 0, 0);
	g_signal_connect(G_OBJECT(jind2), "value-changed", G_CALLBACK(upj), NULL);
	gtk_table_attach(GTK_TABLE(table), jind2, 2, 3, 1, 2, GTK_FILL|GTK_SHRINK, GTK_FILL|GTK_SHRINK, 2, 2);
	gtk_widget_show(jind2);
	label=gtk_label_new(_("k index:"));
	gtk_table_attach(GTK_TABLE(table), label, 2, 3, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(label);
	adj=(GtkAdjustment *) gtk_adjustment_new(0, 0, 0, 1.0, 5.0, 0.0);
	kind=gtk_spin_button_new(adj, 0, 0);
	g_signal_connect(G_OBJECT(kind), "value-changed", G_CALLBACK(upk), NULL);
	gtk_table_attach(GTK_TABLE(table), kind, 2, 3, 3, 4, GTK_FILL|GTK_SHRINK, GTK_FILL|GTK_SHRINK, 2, 2);
	gtk_widget_show(kind);
	butt=gtk_button_new_with_label(_("Reset\nArrays"));
	g_signal_connect(G_OBJECT(butt), "clicked", G_CALLBACK(reset2), NULL);
	gtk_table_attach(GTK_TABLE(table), butt, 2, 3, 4, 6, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(butt);
	pr=gtk_button_new_with_label(_("Calculate\nSpectrum"));
	gtk_table_attach(GTK_TABLE(table), pr, 0, 1, 4, 6, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(pr);
	label=gtk_label_new(_("T-matrix control"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), table, label);
	gtk_widget_show(notebook);
	gtk_paned_add1(GTK_PANED(hpane), notebook);
	notebook2=gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook2), GTK_POS_TOP);
	table=gtk_table_new(1, 1, FALSE);
	gtk_widget_show(table);
	plot1=plot_linear_new();
	((PLOT_LINEAR(plot1))->xlab)=g_strdup(_("Wavelength (nm)"));
	((PLOT_LINEAR(plot1))->xlab)=g_strdup(_("Reflection (dB)"));
	g_signal_connect(plot1, "moved", G_CALLBACK(pltmv), NULL);
	gtk_widget_show(plot1);
	gtk_table_attach(GTK_TABLE(table), plot1, 0, 1, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK |GTK_EXPAND, 2, 2);
	label=gtk_label_new(_("Reflection Spectrum"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook2), table, label);
	table=gtk_table_new(1, 1, FALSE);
	gtk_widget_show(table);
	plot2=plot_linear_new();
	((PLOT_LINEAR(plot2))->xlab)=g_strdup(_("Position (mm)"));
	((PLOT_LINEAR(plot2))->ylab)=g_strdup(_("Coupling Coefficient (/mm)"));
	g_signal_connect(plot2, "moved", G_CALLBACK(pltmv), NULL);
	gtk_widget_show(plot2);
	gtk_table_attach(GTK_TABLE(table), plot2, 0, 1, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	label=gtk_label_new(_("Spatial Profile"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook2), table, label);
	gtk_widget_show(notebook2);
	gtk_paned_add2(GTK_PANED(hpane), notebook2);
	statusbar=gtk_statusbar_new();
	gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, FALSE, 2);
	gtk_widget_show(statusbar);
	x=g_array_new(FALSE, FALSE, sizeof(gdouble));
	yb=g_array_new(FALSE, FALSE, sizeof(gdouble));
	specs=g_array_new(FALSE, FALSE, sizeof(gdouble));
	stars=g_array_new(FALSE, FALSE, sizeof(gdouble));
	xsb=g_array_new(FALSE, FALSE, sizeof(gdouble));
	ysb=g_array_new(FALSE, FALSE, sizeof(gdouble));
	delf=g_array_new(FALSE, FALSE, sizeof(gdouble));
	vis=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXDS);
	doms=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXDS);
	chp=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), MXDS);
	gtk_widget_show(window);
	gtk_main();
	return 0;
}
