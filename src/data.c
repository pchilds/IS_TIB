/***************************************************************************
 *            data.c
 *
 *  Sat Dec  4 17:18:14 2010
 *  Copyright  2010  Paul Childs
 *  <pchilds@physics.org>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
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

#include "data.h"

GtkPrintSettings *pst=NULL;

void opd(GtkWidget *wgt, gpointer dta)
{
	GArray *o1, *o2, *o3;
	gchar *cts=NULL, *fin=NULL, *str;
	gchar **sat=NULL, **sta=NULL;
	gdouble xi, xf, mny, mxy, lcl;
	gdouble *dpr;
	GError *Err=NULL;
	gint k, lc, sal;
	gint *ipr;
	GtkWidget *wfl;
	GtkPlotLinear *plt;

	wfl=gtk_file_chooser_dialog_new(_("Select Data File"), GTK_WINDOW(dta), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(wfl), FALSE);
	gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER(wfl), FALSE);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(wfl), fld);
	g_signal_connect(G_OBJECT(wfl), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(wfl));
	if (gtk_dialog_run(GTK_DIALOG(wfl))==GTK_RESPONSE_ACCEPT)
	{
		g_free(fld);
		g_free(flr);
		fld=gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(wfl));
		flr=g_strdup(fld);
		fin=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wfl));
		if (g_file_get_contents(fin, &cts, NULL, &Err))
		{
			sta=g_strsplit_set(cts, "\r\n", 0);
			sal=g_strv_length(sta);
			{lc=0; k=0;}
			o1=g_array_new(FALSE, FALSE, sizeof(gdouble));
			o2=g_array_new(FALSE, FALSE, sizeof(gdouble));
			o3=g_array_new(FALSE, FALSE, sizeof(gdouble));
			if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(wk)))
			{
				if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(r0)))
				{
					while (k<sal)
					{
						if (!sta[k]) {k++; continue;}
						g_strchug(sta[k]);
						if (!g_strcmp0("", sta[k])) {k++; continue;}
						if (!(g_ascii_isdigit(sta[k][0])|(g_str_has_prefix(sta[k],"-")))) {k++; continue;}
						if (lc<0) {lc++; k++; continue;}
						sat=g_strsplit_set(sta[k], ",\t", 0);
						xf=g_ascii_strtod(g_strstrip(sat[1]), NULL);
						g_array_append_val(o2, xf);
						xf=0;
						g_array_append_val(o3, xf);
						xf=g_ascii_strtod(g_strstrip(sat[0]), NULL);
						g_array_append_val(o1, xf);
						g_strfreev(sat);
						{k++; lc++;}
					}
				}
				else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(ri)))
				{
					while (k<sal)
					{
						if (!sta[k]) {k++; continue;}
						g_strchug(sta[k]);
						if (!g_strcmp0("", sta[k])) {k++; continue;}
						if (!(g_ascii_isdigit(sta[k][0])|(g_str_has_prefix(sta[k],"-")))) {k++; continue;}
						if (lc<0) {lc++; k++; continue;}
						sat=g_strsplit_set(sta[k], ",\t", 0);
						xi=g_ascii_strtod(g_strstrip(sat[1]), NULL);
						xf=g_ascii_strtod(g_strstrip(sat[2]), NULL);
						lcl=sqrt((xi*xi)+(xf*xf));
						g_array_append_val(o2, lcl);
						lcl=atan2(xf, xi);
						g_array_append_val(o3, lcl);
						xf=g_ascii_strtod(g_strstrip(sat[0]), NULL);
						g_array_append_val(o1, xf);
						g_strfreev(sat);
						{k++; lc++;}
					}
				}
				else
				{
					while (k<sal)
					{
						if (!sta[k]) {k++; continue;}
						g_strchug(sta[k]);
						if (!g_strcmp0("", sta[k])) {k++; continue;}
						if (!(g_ascii_isdigit(sta[k][0])|(g_str_has_prefix(sta[k],"-")))) {k++; continue;}
						if (lc<0) {lc++; k++; continue;}
						sat=g_strsplit_set(sta[k], ",\t", 0);
						xf=g_ascii_strtod(g_strstrip(sat[1]), NULL);
						g_array_append_val(o2, xf);
						xf=g_ascii_strtod(g_strstrip(sat[2]), NULL);
						g_array_append_val(o3, xf);
						xf=g_ascii_strtod(g_strstrip(sat[0]), NULL);
						g_array_append_val(o1, xf);
						g_strfreev(sat);
						{k++; lc++;}
					}
				}
				xi=g_array_index(o1, gdouble, 0);
			}
			else
			{
				if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(r0)))
				{
					while (k<sal)
					{
						if (!sta[k]) {k++; continue;}
						g_strchug(sta[k]);
						if (!g_strcmp0("", sta[k])) {k++; continue;}
						if (!(g_ascii_isdigit(sta[k][0])|(g_str_has_prefix(sta[k],"-")))) {k++; continue;}
						if (lc<0) {lc++; k++; continue;}
						sat=g_strsplit_set(sta[k], ",\t", 0);
						xf=g_ascii_strtod(g_strstrip(sat[0]), NULL);
						g_array_append_val(o2, xf);
						xf=0;
						g_array_append_val(o3, xf);
						g_strfreev(sat);
						{k++; lc++;}
					}
				}
				else if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(ri)))
				{
					while (k<sal)
					{
						if (!sta[k]) {k++; continue;}
						g_strchug(sta[k]);
						if (!g_strcmp0("", sta[k])) {k++; continue;}
						if (!(g_ascii_isdigit(sta[k][0])|(g_str_has_prefix(sta[k],"-")))) {k++; continue;}
						if (lc<0) {lc++; k++; continue;}
						sat=g_strsplit_set(sta[k], ",\t", 0);
						xi=g_ascii_strtod(g_strstrip(sat[0]), NULL);
						xf=g_ascii_strtod(g_strstrip(sat[1]), NULL);
						lcl=sqrt((xi*xi)+(xf*xf));
						g_array_append_val(o2, lcl);
						lcl=atan2(xf, xi);
						g_array_append_val(o3, lcl);
						g_strfreev(sat);
						{k++; lc++;}
					}
				}
				else
				{
					while (k<sal)
					{
						if (!sta[k]) {k++; continue;}
						g_strchug(sta[k]);
						if (!g_strcmp0("", sta[k])) {k++; continue;}
						if (!(g_ascii_isdigit(sta[k][0])|(g_str_has_prefix(sta[k],"-")))) {k++; continue;}
						if (lc<0) {lc++; k++; continue;}
						sat=g_strsplit_set(sta[k], ",\t", 0);
						xf=g_ascii_strtod(g_strstrip(sat[0]), NULL);
						g_array_append_val(o2, xf);
						xf=g_ascii_strtod(g_strstrip(sat[1]), NULL);
						g_array_append_val(o3, xf);
						g_strfreev(sat);
						{k++; lc++;}
					}
				}
				xi=gtk_spin_button_get_value(GTK_SPIN_BUTTON(wst));
				xf=(gtk_spin_button_get_value(GTK_SPIN_BUTTON(wsp))-xi)/(lc-1);/*check for /0 error*/
				lcl=xi;
				for (k=0;k<lc;k++)
				{
					g_array_append_val(o1, lcl);
					lcl+=xf;
				}
				xf=(xf*(lc-1))+xi;
			}
			g_strfreev(sta);
			str=g_strdup_printf(_("File: %s successfully loaded."), fin);
			gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
			g_free(str);
			switch (gtk_notebook_get_current_page(GTK_NOTEBOOK(nbk)))
			{
				case 1:
				{g_array_free(z, TRUE); g_array_free(kp, TRUE); g_array_free(sz2, TRUE); g_array_free(nx2, TRUE);}
				z=g_array_new(FALSE, FALSE, sizeof(gdouble));
				kp=g_array_new(FALSE, FALSE, sizeof(gdouble));
				sz2=g_array_new(FALSE, FALSE, sizeof(gint));
				nx2=g_array_new(FALSE, FALSE, sizeof(gint));
				k=0;
				g_array_append_val(sz2, lc);
				g_array_append_val(sz2, lc);
				g_array_append_val(nx2, k);
				g_array_append_val(nx2, lc);
				g_array_append_val(z, xi);
				mny=g_array_index(o2, gdouble, 0);
				g_array_append_val(kp, mny);
				mxy=mny;
				for (k=1;k<lc;k++)
				{
					lcl=g_array_index(o1, gdouble, k);
					g_array_append_val(z, lcl);
					lcl=g_array_index(o2, gdouble, k);
					g_array_append_val(kp, lcl);
					if (lcl>mxy) mxy=lcl;
					else if (lcl<mny) mny=lcl;
				}
				for (k=0;k<lc;k++)
				{
					lcl=g_array_index(o1, gdouble, k);
					g_array_append_val(z, lcl);
					lcl=g_array_index(o3, gdouble, k);
					g_array_append_val(kp, lcl);
					if (lcl>mxy) mxy=lcl;
					else if (lcl<mny) mny=lcl;
				}
				fgs|=PROC_ZDT;
				plt=GTK_PLOT_LINEAR(pt2);
				{(plt->sizes)=sz2; (plt->ind)=nx2; (plt->xdata)=z; (plt->ydata)=kp;}
				gtk_plot_linear_update_scale_pretty(pt2, xi, xf, mny, mxy);
				break;
				default:
				{g_array_free(l, TRUE); g_array_free(R, TRUE); g_array_free(sz1, TRUE); g_array_free(nx1, TRUE);}
				l=g_array_new(FALSE, FALSE, sizeof(gdouble));
				R=g_array_new(FALSE, FALSE, sizeof(gdouble));
				sz1=g_array_new(FALSE, FALSE, sizeof(gint));
				nx1=g_array_new(FALSE, FALSE, sizeof(gint));
				k=0;
				g_array_append_val(sz1, lc);
				g_array_append_val(sz1, lc);
				g_array_append_val(nx1, k);
				g_array_append_val(nx1, lc);
				g_array_append_val(l, xi);
				mny=g_array_index(o2, gdouble, 0);
				g_array_append_val(R, mny);
				mxy=mny;
				for (k=1;k<lc;k++)
				{
					lcl=g_array_index(o1, gdouble, k);
					g_array_append_val(l, lcl);
					lcl=g_array_index(o2, gdouble, k);
					g_array_append_val(R, lcl);
					if (lcl>mxy) mxy=lcl;
					else if (lcl<mny) mny=lcl;
				}
				for (k=0;k<lc;k++)
				{
					lcl=g_array_index(o1, gdouble, k);
					g_array_append_val(l, lcl);
					lcl=g_array_index(o3, gdouble, k);
					g_array_append_val(R, lcl);
					if (lcl>mxy) mxy=lcl;
					else if (lcl<mny) mny=lcl;
				}
				fgs|=PROC_LDT;
				plt=GTK_PLOT_LINEAR(pt1);
				{(plt->sizes)=sz1; (plt->ind)=nx1; (plt->xdata)=l; (plt->ydata)=R;}
				gtk_plot_linear_update_scale_pretty(pt1, xi, xf, mny, mxy);
				break;
			}
		}
		else
		{
			str=g_strdup_printf(_("Loading failed for file: %s, Error: %s."), fin, (gchar*) Err);
			gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
			g_free(str);
			g_error_free(Err);
		}
		g_free(cts);
		g_free(fin);
	}
	gtk_widget_destroy(wfl);
}

void prb(GtkPrintOperation *pto, GtkPrintContext *ctx, int p_n)
{
	gtk_print_operation_set_current_page(pto, 0);
	gtk_print_operation_set_has_selection(pto, FALSE);
}

void prf(GtkPrintOperation *pto, GtkPrintContext *ctx, int p_n)
{
	cairo_t *cr=gtk_print_context_get_cairo_context(ctx);
	gint fg;
	gchar *str;

	if (p_n) return;
	fg=gtk_notebook_get_current_page(GTK_NOTEBOOK(nbk))+1;
	if ((fgs&fg)!=0)
	{
	}
	else if ((fgs&(fg^3))!=0)
	{
	}
	else
	{
		gtk_notebook_set_current_page(GTK_NOTEBOOK(nbk), 0);
		str=g_strdup(_("No available image."));
		gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
		g_free(str);
	}
}

void prg(GtkWidget *wgt, gpointer dta)
{
	GtkWidget *wfl;
	GtkFileFilter *eps, *png, *svg, *flt;
	gchar *str, *f1=NULL, *f2=NULL;
	gint fg;

	fg=gtk_notebook_get_current_page(GTK_NOTEBOOK(nbk))+1;
	if ((fgs&fg)!=0)
	{
		wfl=gtk_file_chooser_dialog_new(_("Select Image File"), GTK_WINDOW(dta), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
		g_signal_connect(G_OBJECT(wfl), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(wfl));
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(wfl), flr);
		gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER(wfl), FALSE);
		gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(wfl), TRUE);
		png=gtk_file_filter_new();
		gtk_file_filter_set_name(png, "Portable Network Graphics (PNG)");
		gtk_file_filter_add_mime_type(png, "image/png");
		gtk_file_filter_add_mime_type(png, "application/png");
		gtk_file_filter_add_mime_type(png, "application/x-png");
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(wfl), png);
		eps=gtk_file_filter_new();
		gtk_file_filter_set_name(eps, "Encapsulated Postscript (EPS)");
		gtk_file_filter_add_mime_type(eps, "application/postscript");
		gtk_file_filter_add_mime_type(eps, "application/eps");
		gtk_file_filter_add_mime_type(eps, "image/eps");
		gtk_file_filter_add_mime_type(eps, "image/x-eps");
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(wfl), eps);
		svg=gtk_file_filter_new();
		gtk_file_filter_set_name(svg, "Scalable Vector Graphics (SVG)");
		gtk_file_filter_add_mime_type(svg, "image/svg+xml");
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(wfl), svg);
		if (gtk_dialog_run(GTK_DIALOG(wfl))==GTK_RESPONSE_ACCEPT)
		{
			g_free(flr);
			flr=gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(wfl));
			f1=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wfl));
			flt=gtk_file_chooser_get_filter(GTK_FILE_CHOOSER(wfl));
			if (fg==1)
			{
				if (g_str_has_suffix(f1, ".eps")) gtk_plot_linear_print_eps(pt1, f1);
				else if (g_str_has_suffix(f1, ".svg")) gtk_plot_linear_print_svg(pt1, f1);
				else if (g_str_has_suffix(f1, ".png")) gtk_plot_linear_print_png(pt1, f1);
				else if (flt==eps)
				{
					f2=g_strconcat(f1, ".eps", NULL);
					gtk_plot_linear_print_eps(pt1, f2);
					g_free(f2);
				}
				else if (flt==svg)
				{
					f2=g_strconcat(f1, ".svg", NULL);
					gtk_plot_linear_print_svg(pt1, f2);
					g_free(f2);
				}
				else
				{
					f2=g_strconcat(f1, ".png", NULL);
					gtk_plot_linear_print_png(pt1, f2);
					g_free(f2);
				}
			}
			else
			{
				if (g_str_has_suffix(f1, ".eps")) gtk_plot_linear_print_eps(pt2, f1);
				else if (g_str_has_suffix(f1, ".svg")) gtk_plot_linear_print_svg(pt2, f1);
				else if (g_str_has_suffix(f1, ".png")) gtk_plot_linear_print_png(pt2, f1);
				else if (flt==eps)
				{
					f2=g_strconcat(f1, ".eps", NULL);
					gtk_plot_linear_print_eps(pt2, f2);
					g_free(f2);
				}
				else if (flt==svg)
				{
					f2=g_strconcat(f1, ".svg", NULL);
					gtk_plot_linear_print_svg(pt2, f2);
					g_free(f2);
				}
				else
				{
					f2=g_strconcat(f1, ".png", NULL);
					gtk_plot_linear_print_png(pt2, f2);
					g_free(f2);
				}
			}
			g_free(f1);
		}
		gtk_widget_destroy(wfl);
	}
	else if ((fgs&(fg^3))!=0)
	{
		wfl=gtk_file_chooser_dialog_new(_("Select Image File"), GTK_WINDOW(dta), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
		g_signal_connect(G_OBJECT(wfl), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(wfl));
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(wfl), flr);
		gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER(wfl), FALSE);
		gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(wfl), TRUE);
		png=gtk_file_filter_new();
		gtk_file_filter_set_name(png, "Portable Network Graphics (PNG)");
		gtk_file_filter_add_mime_type(png, "image/png");
		gtk_file_filter_add_mime_type(png, "application/png");
		gtk_file_filter_add_mime_type(png, "application/x-png");
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(wfl), png);
		eps=gtk_file_filter_new();
		gtk_file_filter_set_name(eps, "Encapsulated Postscript (EPS)");
		gtk_file_filter_add_mime_type(eps, "application/postscript");
		gtk_file_filter_add_mime_type(eps, "application/eps");
		gtk_file_filter_add_mime_type(eps, "image/eps");
		gtk_file_filter_add_mime_type(eps, "image/x-eps");
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(wfl), eps);
		svg=gtk_file_filter_new();
		gtk_file_filter_set_name(svg, "Scalable Vector Graphics (SVG)");
		gtk_file_filter_add_mime_type(svg, "image/svg+xml");
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(wfl), svg);
		if (gtk_dialog_run(GTK_DIALOG(wfl))==GTK_RESPONSE_ACCEPT)
		{
			g_free(flr);
			flr=gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(wfl));
			f1=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wfl));
			flt=gtk_file_chooser_get_filter(GTK_FILE_CHOOSER(wfl));
			if (fg==1)
			{
				if (g_str_has_suffix(f1, ".eps")) gtk_plot_linear_print_eps(pt2, f1);
				else if (g_str_has_suffix(f1, ".svg")) gtk_plot_linear_print_svg(pt2, f1);
				else if (g_str_has_suffix(f1, ".png")) gtk_plot_linear_print_png(pt2, f1);
				else if (flt==eps)
				{
					f2=g_strconcat(f1, ".eps", NULL);
					gtk_plot_linear_print_eps(pt2, f2);
					g_free(f2);
				}
				else if (flt==svg)
				{
					f2=g_strconcat(f1, ".svg", NULL);
					gtk_plot_linear_print_svg(pt2, f2);
					g_free(f2);
				}
				else
				{
					f2=g_strconcat(f1, ".png", NULL);
					gtk_plot_linear_print_png(pt2, f2);
					g_free(f2);
				}
			}
			else
			{
				if (g_str_has_suffix(f1, ".eps")) gtk_plot_linear_print_eps(pt1, f1);
				else if (g_str_has_suffix(f1, ".svg")) gtk_plot_linear_print_svg(pt1, f1);
				else if (g_str_has_suffix(f1, ".png")) gtk_plot_linear_print_png(pt1, f1);
				else if (flt==eps)
				{
					f2=g_strconcat(f1, ".eps", NULL);
					gtk_plot_linear_print_eps(pt1, f2);
					g_free(f2);
				}
				else if (flt==svg)
				{
					f2=g_strconcat(f1, ".svg", NULL);
					gtk_plot_linear_print_svg(pt1, f2);
					g_free(f2);
				}
				else
				{
					f2=g_strconcat(f1, ".png", NULL);
					gtk_plot_linear_print_png(pt1, f2);
					g_free(f2);
				}
			}
			g_free(f1);
		}
		gtk_widget_destroy(wfl);
	}
	else
	{
		gtk_notebook_set_current_page(GTK_NOTEBOOK(nbk), 0);
		str=g_strdup(_("No available image."));
		gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
		g_free(str);
	}
}

void prt(GtkWidget *wgt, gpointer dta)
{
	GtkPrintOperation *pto;
	GtkPageSetup *pps;
	GtkPrintOperationResult res;
	GError *Err=NULL;
	gchar *str;

	pto=gtk_print_operation_new();
	if (pst!=NULL) gtk_print_operation_set_print_settings(pto, pst);
	/*pps=gtk_print_operation_get_default_page_setup(pto);
	gtk_page_setup_set_orientation(pps, GTK_PAGE_ORIENTATION_LANDSCAPE);
	gtk_print_operation_set_default_page_setup(pto, pps);*/
	g_signal_connect(pto, "begin_print", G_CALLBACK(prb), NULL);
	g_signal_connect(pto, "draw_page", G_CALLBACK(prf), NULL);
	res=gtk_print_operation_run(pto, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, GTK_WINDOW(dta), &Err);
	if (res==GTK_PRINT_OPERATION_RESULT_ERROR)
	{
		str=g_strdup_printf(_("An error occured while printing: %s."), (Err->message));
		gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
		g_free(str);
		g_error_free(Err);
	}
	else if (res==GTK_PRINT_OPERATION_RESULT_APPLY)
	{
		if (pst!=NULL) g_object_unref(pst);
		pst=g_object_ref(gtk_print_operation_get_print_settings(pto));
	}
	g_object_unref(pto);
}

void sav(GtkWidget *wgt, gpointer dta)
{
	GtkWidget *wfl, *dlg, *cnt, *lbl;
	GtkPlotLinear *plt;
	gchar *cts, *str, *st2, *fot=NULL;
	gchar s1[10], s2[10], s3[10], s4[10];
	gint j, k, sz4, nx4, fg;
	gdouble num, num2, num3;
	GError *Err=NULL;

	fg=gtk_notebook_get_current_page(GTK_NOTEBOOK(nbk))+1;
	if ((fgs&fg)!=0)
	{
		wfl=gtk_file_chooser_dialog_new(_("Select Data File"), GTK_WINDOW(dta), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
		g_signal_connect(G_OBJECT(wfl), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(wfl));
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(wfl), flr);
		gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(wfl), TRUE);
		if (gtk_dialog_run(GTK_DIALOG(wfl))==GTK_RESPONSE_ACCEPT)
		{
			g_free(flr);
			flr=gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(wfl));
			fot=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wfl));
			dlg=gtk_dialog_new_with_buttons(_("Parameter selection"), GTK_WINDOW(wfl), GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT, _("Real/Imaginary"), 1, _("Magnitude/Phase"), 2, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
			lbl=gtk_label_new(_("Select Parameter to save:"));
			gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG (dlg))), lbl);
			switch (gtk_dialog_run(GTK_DIALOG(dlg)))
			{
				case 1:
				if (fg==1) {plt=GTK_PLOT_LINEAR(pt1); cts=g_strdup(_("SPECTRL_D\tREAL_VAL \tIMAG_VAL "));}
				else {plt=GTK_PLOT_LINEAR(pt2); cts=g_strdup(_("SPATIAL_D\tREAL_VAL \tIMAG_VAL "));}
				sz4=g_array_index((plt->sizes), gint, 0);
				nx4=g_array_index((plt->ind), gint, 1);
				for (j=0; j<sz4; j++)
				{
					g_snprintf(s1, 10, "%f", g_array_index((plt->xdata), gdouble, j));
					num=g_array_index((plt->ydata), gdouble, j);
					num2=g_array_index((plt->ydata), gdouble, nx4+j);
					num3=num*sin(num2);
					num*=cos(num2);
					g_snprintf(s2, 10, "%f", num);
					g_snprintf(s3, 10, "%f", num3);
					st2=g_strjoin("\t", s1, s2, s3, NULL);
					str=g_strdup(cts);
					g_free(cts);
					cts=g_strjoin(DLMT, str, st2, NULL);
					{g_free(str); g_free(st2);}
				}
				g_file_set_contents(fot, cts, -1, &Err);
				g_free(cts);
				if (Err)
				{
					str=g_strdup_printf(_("Error Saving file: %s."), (Err->message));
					gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
					g_free(str);
					g_error_free(Err);
				}
				g_free(fot);
				break;
				case 2:
				if (fg==1) {plt=GTK_PLOT_LINEAR(pt1); cts=g_strdup(_("SPECTRL_D\tMAGNITUDE\tPHASE    "));}
				else {plt=GTK_PLOT_LINEAR(pt2); cts=g_strdup(_("SPATIAL_D\tMAGNITUDE\tPHASE    "));}
				sz4=g_array_index((plt->sizes), gint, 0);
				nx4=g_array_index((plt->ind), gint, 1);
				for (j=0; j<sz4; j++)
				{
					g_snprintf(s1, 10, "%f", g_array_index((plt->xdata), gdouble, j));
					g_snprintf(s2, 10, "%f", g_array_index((plt->ydata), gdouble, j));
					g_snprintf(s3, 10, "%f", g_array_index((plt->ydata), gdouble, nx4+j));
					st2=g_strjoin("\t", s1, s2, s3, NULL);
					str=g_strdup(cts);
					g_free(cts);
					cts=g_strjoin(DLMT, str, st2, NULL);
					{g_free(str); g_free(st2);}
				}
				g_file_set_contents(fot, cts, -1, &Err);
				g_free(cts);
				if (Err)
				{
					str=g_strdup_printf(_("Error Saving file: %s."), (Err->message));
					gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
					g_free(str);
					g_error_free(Err);
				}
				g_free(fot);
				break;
				default:
				break;
			}
		}
		gtk_widget_destroy(wfl);
	}
	else if ((fgs&(fg^3))!=0)
	{
		wfl=gtk_file_chooser_dialog_new(_("Select Data File"), GTK_WINDOW(dta), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
		g_signal_connect(G_OBJECT(wfl), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(wfl));
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(wfl), flr);
		gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(wfl), TRUE);
		if (gtk_dialog_run(GTK_DIALOG(wfl))==GTK_RESPONSE_ACCEPT)
		{
			g_free(flr);
			flr=gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(wfl));
			fot=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wfl));
			dlg=gtk_dialog_new_with_buttons(_("Parameter selection"), GTK_WINDOW(wfl), GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT, _("Real/Imaginary"), 1, _("Magnitude/Phase"), 2, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
			lbl=gtk_label_new(_("Select Parameter to save:"));
			gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG (dlg))), lbl);
			switch (gtk_dialog_run(GTK_DIALOG(dlg)))
			{
				case 1:
				if (fg==1) {plt=GTK_PLOT_LINEAR(pt2); cts=g_strdup(_("SPATIAL_D\tREAL_VAL \tIMAG_VAL "));}
				else {plt=GTK_PLOT_LINEAR(pt1); cts=g_strdup(_("SPECTRL_D\tREAL_VAL \tIMAG_VAL "));}
				sz4=g_array_index((plt->sizes), gint, 0);
				nx4=g_array_index((plt->ind), gint, 1);
				for (j=0; j<sz4; j++)
				{
					g_snprintf(s1, 10, "%f", g_array_index((plt->xdata), gdouble, j));
					num=g_array_index((plt->ydata), gdouble, j);
					num2=g_array_index((plt->ydata), gdouble, nx4+j);
					num3=num*sin(num2);
					num*=cos(num2);
					g_snprintf(s2, 10, "%f", num);
					g_snprintf(s3, 10, "%f", num3);
					st2=g_strjoin("\t", s1, s2, s3, NULL);
					str=g_strdup(cts);
					g_free(cts);
					cts=g_strjoin(DLMT, str, st2, NULL);
					{g_free(str); g_free(st2);}
				}
				g_file_set_contents(fot, cts, -1, &Err);
				g_free(cts);
				if (Err)
				{
					str=g_strdup_printf(_("Error Saving file: %s."), (Err->message));
					gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
					g_free(str);
					g_error_free(Err);
				}
				g_free(fot);
				break;
				case 2:
				if (fg==1) {plt=GTK_PLOT_LINEAR(pt2); cts=g_strdup(_("SPATIAL_D\tMAGNITUDE\tPHASE    "));}
				else {plt=GTK_PLOT_LINEAR(pt1); cts=g_strdup(_("SPECTRL_D\tMAGNITUDE\tPHASE    "));}
				sz4=g_array_index((plt->sizes), gint, 0);
				nx4=g_array_index((plt->ind), gint, 1);
				for (j=0; j<sz4; j++)
				{
					g_snprintf(s1, 10, "%f", g_array_index((plt->xdata), gdouble, j));
					g_snprintf(s2, 10, "%f", g_array_index((plt->ydata), gdouble, j));
					g_snprintf(s3, 10, "%f", g_array_index((plt->ydata), gdouble, nx4+j));
					st2=g_strjoin("\t", s1, s2, s3, NULL);
					str=g_strdup(cts);
					g_free(cts);
					cts=g_strjoin(DLMT, str, st2, NULL);
					{g_free(str); g_free(st2);}
				}
				g_file_set_contents(fot, cts, -1, &Err);
				g_free(cts);
				if (Err)
				{
					str=g_strdup_printf(_("Error Saving file: %s."), (Err->message));
					gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
					g_free(str);
					g_error_free(Err);
				}
				g_free(fot);
				break;
				default:
				break;
			}
		}
		gtk_widget_destroy(wfl);
	}
	else
	{
		gtk_notebook_set_current_page(GTK_NOTEBOOK(nbk), 0);
		str=g_strdup(_("No available data."));
		gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
		g_free(str);
	}
}

void ssr(GtkWidget *wgt, gpointer dta)
{
	GtkWidget *wfl;
	gchar *cts, *str, *fin=NULL;
	GError *Err=NULL;

	wfl=gtk_file_chooser_dialog_new(_("Select Session File"), GTK_WINDOW(dta), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	g_signal_connect(G_OBJECT(wfl), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(wfl));
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(wfl), flr);
	if (gtk_dialog_run(GTK_DIALOG(wfl))==GTK_RESPONSE_ACCEPT)
	{
		g_free(flr);
		flr=gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(wfl));
		fin=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wfl));
		if (g_file_get_contents(fin, &cts, NULL, &Err))
		{
		}
		else
		{
			str=g_strdup_printf(_("Loading failed for file: %s, Error: %s."), fin, (Err->message));
			gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
			g_free(str);
			g_error_free(Err);
		}
		g_free(cts);
		g_free(fin);
	}
	gtk_widget_destroy(wfl);
}

void sss(GtkWidget *wgt, gpointer dta)
{
	GtkWidget *wfl;
	GSList *lst;
	gchar *cts, *ct2, *str, *fot=NULL;
	guint16 fv, fv2;
	GError *Err=NULL;

	wfl=gtk_file_chooser_dialog_new(_("Select Session File"), GTK_WINDOW(dta), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	g_signal_connect(G_OBJECT(wfl), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(wfl));
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(wfl), flr);
	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(wfl), TRUE);
	if (gtk_dialog_run(GTK_DIALOG(wfl))==GTK_RESPONSE_ACCEPT)
	{
		g_free(flr);
		flr=gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(wfl));
		str=g_strdup("#menuitems");
		{fv=0; /*lst=group4;*/}/*need to ensure fv values don't conflict with DLMT, \t \0 etc */
		/*while (lst)
		{
			if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(lst->data))) break;
			{lst=(lst->next); fv++;}
		}*/
		{fv<<=2; /*lst=group5;*/}
		/*while (lst)
		{
			if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(lst->data))) break;
			{lst=(lst->next); fv++;}
		}*/
		{fv<<=2; /*lst=group3;*/}
		/*while (lst)
		{
			if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(lst->data))) break;
			{lst=(lst->next); fv++;}
		}*/
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(tx))) fv|=256;
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(neg))) fv|=512;
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(dBs))) fv|=1024;
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(kms))) fv|=2048;
		{fv2=0; /*lst=group2;*/}
		/*while (lst)
		{
			if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(lst->data))) break;
			{lst=(lst->next); fv2++;}
		}*/
		{fv2<<=8; lst=grp;}
		while (lst)
		{
			if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(lst->data))) break;
			{lst=(lst->next); fv2++;}
		}
		str=g_strjoin("\t", str, (gchar) fv, (gchar) fv2, NULL);
		ct2=g_strdup(cts);
		g_free(cts);
		cts=g_strjoin(DLMT, ct2, str, NULL);
		g_free(str);
		fot=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wfl));
		g_file_set_contents(fot, cts, -1, &Err);
		g_free(cts);
		if (Err)
		{
			str=g_strdup_printf(_("Error Saving file: %s."), (Err->message));
			gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
			g_free(str);
			g_error_free(Err);
		}
		g_free(cts);
		g_free(fot);
	}
	gtk_widget_destroy(wfl);
}

