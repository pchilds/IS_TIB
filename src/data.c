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

GtkPrintSettings *gps=NULL;

void opd(GtkWidget *wgt, gpointer dta)
{
	GArray *o1, *o2, *o3, *x, *y, *nx, *sz;
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
			x=g_array_new(FALSE, FALSE, sizeof(gdouble));
			y=g_array_new(FALSE, FALSE, sizeof(gdouble));
			sz=g_array_new(FALSE, FALSE, sizeof(gint));
			nx=g_array_new(FALSE, FALSE, sizeof(gint));
			k=0;
			g_array_append_val(sz, lc);
			g_array_append_val(sz, lc);
			g_array_append_val(nx, k);
			g_array_append_val(nx, lc);
			g_array_append_val(x, xi);
			mny=g_array_index(o2, gdouble, 0);
			g_array_append_val(y, mny);
			mxy=mny;
			for (k=1;k<lc;k++)
			{
				lcl=g_array_index(o1, gdouble, k);
				g_array_append_val(x, lcl);
				lcl=g_array_index(o2, gdouble, k);
				g_array_append_val(y, lcl);
				if (lcl>mxy) mxy=lcl;
				else if (lcl<mny) mny=lcl;
			}
			for (k=0;k<lc;k++)
			{
				lcl=g_array_index(o1, gdouble, k);
				g_array_append_val(x, lcl);
				lcl=g_array_index(o3, gdouble, k);
				g_array_append_val(y, lcl);
				if (lcl>mxy) mxy=lcl;
				else if (lcl<mny) mny=lcl;
			}
			switch (gtk_notebook_get_current_page(GTK_NOTEBOOK(nbk)))
			{
				case 1:
				fgs|=PROC_ZDT;
				plt=GTK_PLOT_LINEAR(pt2);
				gtk_plot_linear_set_data(plt, x, y, nx, sz);
				gtk_plot_linear_update_scale_pretty(pt2, xi, xf, mny, mxy);
				break;
				default:
				fgs|=PROC_LDT;
				plt=GTK_PLOT_LINEAR(pt1);
				gtk_plot_linear_set_data(plt, x, y, nx, sz);
				gtk_plot_linear_update_scale_pretty(pt1, xi, xf, mny, mxy);
				break;
			}
			{g_array_unref(x); g_array_unref(y); g_array_unref(nx); g_array_unref(sz);}
		}
		else
		{
			str=g_strdup_printf(_("Loading failed for file: %s, Error: %s."), fin, Err->message);
			gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
			{g_free(str); g_error_free(Err);}
		}
		{g_free(cts); g_free(fin);}
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
	if (gps!=NULL) gtk_print_operation_set_print_settings(pto, gps);
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
		if (gps!=NULL) g_object_unref(gps);
		gps=g_object_ref(gtk_print_operation_get_print_settings(pto));
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
					{g_free(str); g_error_free(Err);}
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
					{g_free(str); g_error_free(Err);}
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
	GArray *car, *cag, *cab, *caa;
	gchar *str, *str2, *fout;
	gdouble mny, mxy, xf, xi; 
	GError *Err=NULL;
	GKeyFile *key;
	GSList *lst;
	GtkPlot *pt;
	GtkPlotLinear *plt;
	PangoFontDescription *ds1, *ds2;

	key=g_key_file_new();
	fout=g_build_filename(g_get_user_config_dir(), PACKAGE, "session.conf", NULL);
	if (g_key_file_load_from_file(key, fout, G_KEY_FILE_KEEP_COMMENTS|G_KEY_FILE_KEEP_TRANSLATIONS, &Err))
	{
		if (g_key_file_has_key(key, "MenuItems", "DomainFirst", &Err)) gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(wk), g_key_file_get_boolean(key, "MenuItems", "DomainFirst", &Err));
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "MenuItems", "Complex", &Err))
		{
			str=g_key_file_get_string(key, "MenuItems", "Complex", &Err);
			lst=grp;
			if (!g_strcmp0(str, "MagnitudeOnly"))
			{
				gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(lst->data), TRUE);
				goto bottomofcheck1b;
			}
			else lst=lst->next;
			if (!g_strcmp0(str, "RealImaginary")) gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(lst->data), TRUE);
			else if (!g_strcmp0(str, "MagnitudePhase"))
			{
				lst=lst->next;
				gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(lst->data), TRUE);
			}
			bottomofcheck1b:
			g_free(str);
		}
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "MenuItems", "Transmission", &Err)) gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(tx), g_key_file_get_boolean(key, "MenuItems", "Transmission", &Err));
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "MenuItems", "dBs", &Err)) gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(dBs), g_key_file_get_boolean(key, "MenuItems", "dBs", &Err));
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "MenuItems", "Negated", &Err)) gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(neg), g_key_file_get_boolean(key, "MenuItems", "Negated", &Err));
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "MenuItems", "Wavenumber", &Err)) gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(kms), g_key_file_get_boolean(key, "MenuItems", "Wavenumber", &Err));
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "Params", "SpectrumStart", &Err)) gtk_spin_button_set_value(GTK_SPIN_BUTTON(wst), g_key_file_get_double(key, "Params", "SpectrumStart", &Err));
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "Params", "SpectrumStop", &Err)) gtk_spin_button_set_value(GTK_SPIN_BUTTON(wsp), g_key_file_get_double(key, "Params", "SpectrumStop", &Err));
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "Params", "Offset", &Err)) gtk_spin_button_set_value(GTK_SPIN_BUTTON(fst), g_key_file_get_double(key, "Params", "Offset", &Err));
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "Params", "ZeroPadding", &Err)) gtk_spin_button_set_value(GTK_SPIN_BUTTON(zpd), g_key_file_get_integer(key, "Params", "ZeroPadding", &Err));
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "Params", "SpatialPoints", &Err)) gtk_spin_button_set_value(GTK_SPIN_BUTTON(nz), g_key_file_get_integer(key, "Params", "SpatialPoints", &Err));
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "Params", "StartPosition", &Err)) gtk_spin_button_set_value(GTK_SPIN_BUTTON(pst), g_key_file_get_double(key, "Params", "StartPosition", &Err));
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "Params", "StopPosition", &Err)) gtk_spin_button_set_value(GTK_SPIN_BUTTON(psp), g_key_file_get_double(key, "Params", "StopPosition", &Err));
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "Params", "DifferentialCoercionMagnitude", &Err)) gtk_spin_button_set_value(GTK_SPIN_BUTTON(crm), g_key_file_get_double(key, "Params", "DifferentialCoercionMagnitude", &Err));
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "Params", "DifferentialCoercionPhase", &Err)) gtk_spin_button_set_value(GTK_SPIN_BUTTON(crp), g_key_file_get_double(key, "Params", "DifferentialCoercionPhase", &Err));
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "Params", "BeamConvolution", &Err)) gtk_spin_button_set_value(GTK_SPIN_BUTTON(cnv), g_key_file_get_double(key, "Params", "BeamConvolution", &Err));
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "Params", "CentreResonance", &Err)) gtk_spin_button_set_value(GTK_SPIN_BUTTON(cl), g_key_file_get_double(key, "Params", "CentreResonance", &Err));
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "Params", "Range", &Err)) gtk_spin_button_set_value(GTK_SPIN_BUTTON(dl), g_key_file_get_double(key, "Params", "Range", &Err));
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "Params", "SpectralPoints", &Err)) gtk_spin_button_set_value(GTK_SPIN_BUTTON(nl), g_key_file_get_integer(key, "Params", "SpectralPoints", &Err));
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "Params", "DataFolder", &Err))
		{
			g_free(fld);
			fld=g_strdup(g_key_file_get_string(key, "Params", "DataFolder", &Err));
		}
		else if (Err) g_error_free(Err);
		if (g_key_file_has_key(key, "Params", "ResultsFolder", &Err))
		{
			g_free(flr);
			flr=g_strdup(g_key_file_get_string(key, "Params", "ResultsFolder", &Err));
		}
		else if (Err) g_error_free(Err);
		plt=GTK_PLOT_LINEAR(pt1);
		pt=GTK_PLOT(pt1);
		if (g_key_file_has_key(key, "Plot", "SpectrumTextX", &Err)) str=g_key_file_get_string(key, "Plot", "SpectrumTextX", &Err);
		else
		{
			str=g_strdup(plt->xlab);
			if (Err) g_error_free(Err);
		}
		if (g_key_file_has_key(key, "Plot", "SpectrumTextY", &Err)) str2=g_key_file_get_string(key, "Plot", "SpectrumTextY", &Err);
		else
		{
			str2=g_strdup(plt->ylab);
			if (Err) g_error_free(Err);
		}
		gtk_plot_linear_set_label(plt, str, str2);
		{g_free(str); g_free(str2);}
		if (g_key_file_has_key(key, "Plot", "SpectrumLabel", &Err)) ds1=pango_font_description_from_string(g_key_file_get_string(key, "Plot", "SpectrumLabel", &Err));
		else
		{
			ds1=pango_font_description_copy(pt->lfont);
			if (Err) g_error_free(Err);
		}
		if (g_key_file_has_key(key, "Plot", "SpectrumAxis", &Err)) ds2=pango_font_description_from_string(g_key_file_get_string(key, "Plot", "SpectrumAxis", &Err));
		else
		{
			ds2=pango_font_description_copy(pt->afont);
			if (Err) g_error_free(Err);
		}
		gtk_plot_set_font(pt, ds1, ds2);
		{pango_font_description_free(ds1); pango_font_description_free(ds2);}
		car=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), 2);
		cag=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), 2);
		cab=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), 2);
		caa=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), 2);
		if (g_key_file_has_key(key, "Plot", "SpectralMagnitudeRed", &Err)) xi=g_key_file_get_double(key, "Plot", "SpectralMagnitudeRed", &Err);
		else
		{
			xi=g_array_index((pt->rd), gdouble, 0);
			if (Err) g_error_free(Err);
		}
		g_array_append_val(car, xi);
		if (g_key_file_has_key(key, "Plot", "SpectralPhaseRed", &Err)) xi=g_key_file_get_double(key, "Plot", "SpectralPhaseRed", &Err);
		else
		{
			xi=g_array_index((pt->rd), gdouble, 1);
			if (Err) g_error_free(Err);
		}
		g_array_append_val(car, xi);
		if (g_key_file_has_key(key, "Plot", "SpectralMagnitudeGreen", &Err)) xi=g_key_file_get_double(key, "Plot", "SpectralMagnitudeGreen", &Err);
		else
		{
			xi=g_array_index((pt->gr), gdouble, 0);
			if (Err) g_error_free(Err);
		}
		g_array_append_val(cag, xi);
		if (g_key_file_has_key(key, "Plot", "SpectralPhaseGreen", &Err)) xi=g_key_file_get_double(key, "Plot", "SpectralPhaseGreen", &Err);
		else
		{
			xi=g_array_index((pt->gr), gdouble, 1);
			if (Err) g_error_free(Err);
		}
		g_array_append_val(cag, xi);
		if (g_key_file_has_key(key, "Plot", "SpectralMagnitudeBlue", &Err)) xi=g_key_file_get_double(key, "Plot", "SpectralMagnitudeBlue", &Err);
		else
		{
			xi=g_array_index((pt->bl), gdouble, 0);
			if (Err) g_error_free(Err);
		}
		g_array_append_val(cab, xi);
		if (g_key_file_has_key(key, "Plot", "SpectralPhaseBlue", &Err)) xi=g_key_file_get_double(key, "Plot", "SpectralPhaseBlue", &Err);
		else
		{
			xi=g_array_index((pt->bl), gdouble, 1);
			if (Err) g_error_free(Err);
		}
		g_array_append_val(cab, xi);
		if (g_key_file_has_key(key, "Plot", "SpectralMagnitudeAlpha", &Err)) xi=g_key_file_get_double(key, "Plot", "SpectralMagnitudeAlpha", &Err);
		else
		{
			xi=g_array_index((pt->al), gdouble, 0);
			if (Err) g_error_free(Err);
		}
		g_array_append_val(caa, xi);
		if (g_key_file_has_key(key, "Plot", "SpectralPhaseAlpha", &Err)) xi=g_key_file_get_double(key, "Plot", "SpectralPhaseAlpha", &Err);
		else
		{
			xi=g_array_index((pt->al), gdouble, 1);
			if (Err) g_error_free(Err);
		}
		g_array_append_val(caa, xi);
		gtk_plot_set_colour(pt, car, cag, cab, caa);
		{g_array_unref(car); g_array_unref(cag); g_array_unref(cab); g_array_unref(caa);}
		g_object_get(G_OBJECT(pt1), "xmin", &xi, "xmax", &xf, "ymin", &mny, "ymax", &mxy, NULL);
		gtk_plot_linear_update_scale(pt1, xi, xf, mny, mxy);
		plt=GTK_PLOT_LINEAR(pt2);
		pt=GTK_PLOT(pt2);
		if (g_key_file_has_key(key, "Plot", "SpatialTextX", &Err)) str=g_key_file_get_string(key, "Plot", "SpatialTextX", &Err);
		else
		{
			str=g_strdup(plt->xlab);
			if (Err) g_error_free(Err);
		}
		if (g_key_file_has_key(key, "Plot", "SpatialTextY", &Err)) str2=g_key_file_get_string(key, "Plot", "SpatialTextY", &Err);
		else
		{
			str2=g_strdup(plt->ylab);
			if (Err) g_error_free(Err);
		}
		gtk_plot_linear_set_label(plt, str, str2);
		{g_free(str); g_free(str2);}
		if (g_key_file_has_key(key, "Plot", "SpatialLabel", &Err)) ds1=pango_font_description_from_string(g_key_file_get_string(key, "Plot", "SpatialLabel", &Err));
		else
		{
			ds1=pango_font_description_copy(pt->lfont);
			if (Err) g_error_free(Err);
		}
		if (g_key_file_has_key(key, "Plot", "SpatialAxis", &Err)) ds2=pango_font_description_from_string(g_key_file_get_string(key, "Plot", "SpatialAxis", &Err));
		else
		{
			ds2=pango_font_description_copy(pt->afont);
			if (Err) g_error_free(Err);
		}
		gtk_plot_set_font(pt, ds1, ds2);
		{pango_font_description_free(ds1); pango_font_description_free(ds2);}
		car=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), 2);
		cag=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), 2);
		cab=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), 2);
		caa=g_array_sized_new(FALSE, FALSE, sizeof(gdouble), 2);
		if (g_key_file_has_key(key, "Plot", "SpatialMagnitudeRed", &Err)) xi=g_key_file_get_double(key, "Plot", "SpatialMagnitudeRed", &Err);
		else
		{
			xi=g_array_index((pt->rd), gdouble, 0);
			if (Err) g_error_free(Err);
		}
		g_array_append_val(car, xi);
		if (g_key_file_has_key(key, "Plot", "SpatialPhaseRed", &Err)) xi=g_key_file_get_double(key, "Plot", "SpatialPhaseRed", &Err);
		else
		{
			xi=g_array_index((pt->rd), gdouble, 1);
			if (Err) g_error_free(Err);
		}
		g_array_append_val(car, xi);
		if (g_key_file_has_key(key, "Plot", "SpatialMagnitudeGreen", &Err)) xi=g_key_file_get_double(key, "Plot", "SpatialMagnitudeGreen", &Err);
		else
		{
			xi=g_array_index((pt->gr), gdouble, 0);
			if (Err) g_error_free(Err);
		}
		g_array_append_val(cag, xi);
		if (g_key_file_has_key(key, "Plot", "SpatialPhaseGreen", &Err)) xi=g_key_file_get_double(key, "Plot", "SpatialPhaseGreen", &Err);
		else
		{
			xi=g_array_index((pt->gr), gdouble, 1);
			if (Err) g_error_free(Err);
		}
		g_array_append_val(cag, xi);
		if (g_key_file_has_key(key, "Plot", "SpatialMagnitudeBlue", &Err)) xi=g_key_file_get_double(key, "Plot", "SpatialMagnitudeBlue", &Err);
		else
		{
			xi=g_array_index((pt->bl), gdouble, 0);
			if (Err) g_error_free(Err);
		}
		g_array_append_val(cab, xi);
		if (g_key_file_has_key(key, "Plot", "SpatialPhaseBlue", &Err)) xi=g_key_file_get_double(key, "Plot", "SpatialPhaseBlue", &Err);
		else
		{
			xi=g_array_index((pt->bl), gdouble, 1);
			if (Err) g_error_free(Err);
		}
		g_array_append_val(cab, xi);
		if (g_key_file_has_key(key, "Plot", "SpatialMagnitudeAlpha", &Err)) xi=g_key_file_get_double(key, "Plot", "SpatialMagnitudeAlpha", &Err);
		else
		{
			xi=g_array_index((pt->al), gdouble, 0);
			if (Err) g_error_free(Err);
		}
		g_array_append_val(caa, xi);
		if (g_key_file_has_key(key, "Plot", "SpatialPhaseAlpha", &Err)) xi=g_key_file_get_double(key, "Plot", "SpatialPhaseAlpha", &Err);
		else
		{
			xi=g_array_index((pt->al), gdouble, 1);
			if (Err) g_error_free(Err);
		}
		g_array_append_val(caa, xi);
		gtk_plot_set_colour(pt, car, cag, cab, caa);
		{g_array_unref(car); g_array_unref(cag); g_array_unref(cab); g_array_unref(caa);}
		g_object_get(G_OBJECT(pt2), "xmin", &xi, "xmax", &xf, "ymin", &mny, "ymax", &mxy, NULL);
		gtk_plot_linear_update_scale(pt2, xi, xf, mny, mxy);
		g_key_file_free(key);
	}
	else
	{
		str=g_strdup_printf(_("Loading failed for file: %s, Error: %s."), fout, (Err->message));
		gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
		{g_free(str); g_error_free(Err);}
	}
	g_free(fout);
}

void sss(GtkWidget *wgt, gpointer dta)
{
	gchar *fout, *pdr, *str;
	GError *Err=NULL;
	GKeyFile *key;
	gsize size;
	GSList *lst;
	GtkPlot *pt;
	GtkPlotLinear *plt;

	pdr=g_build_filename(g_get_user_config_dir(), PACKAGE, NULL);
	if (g_mkdir_with_parents (pdr, 0700) == 0)
	{
		key=g_key_file_new();
		g_key_file_set_boolean(key, "MenuItems", "DomainFirst", gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(wk)));
		lst=grp;
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(lst->data)))
		{
			g_key_file_set_string(key, "MenuItems", "Complex", "MagnitudeOnly");
			goto bottomofcheck1b;
		}
		lst=(lst->next);
		if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(lst->data))) g_key_file_set_string(key, "MenuItems", "Complex", "RealImaginary");
		else g_key_file_set_string(key, "MenuItems", "Complex", "MagnitudePhase");
		bottomofcheck1b:
		g_key_file_set_boolean(key, "MenuItems", "Transmission", gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(tx)));
		g_key_file_set_boolean(key, "MenuItems", "dBs", gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(dBs)));
		g_key_file_set_boolean(key, "MenuItems", "Negated", gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(neg)));
		g_key_file_set_boolean(key, "MenuItems", "Wavenumber", gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(kms)));
		g_key_file_set_double(key, "Params", "SpectrumStart", gtk_spin_button_get_value(GTK_SPIN_BUTTON(wst)));
		g_key_file_set_double(key, "Params", "SpectrumStop", gtk_spin_button_get_value(GTK_SPIN_BUTTON(wsp)));
		g_key_file_set_double(key, "Params", "Offset", gtk_spin_button_get_value(GTK_SPIN_BUTTON(fst)));
		g_key_file_set_integer(key, "Params", "ZeroPadding", gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(zpd)));
		g_key_file_set_integer(key, "Params", "SpatialPoints", gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(nz)));
		g_key_file_set_double(key, "Params", "StartPosition", gtk_spin_button_get_value(GTK_SPIN_BUTTON(pst)));
		g_key_file_set_double(key, "Params", "StopPosition", gtk_spin_button_get_value(GTK_SPIN_BUTTON(psp)));
		g_key_file_set_double(key, "Params", "DifferentialCoercionMagnitude", gtk_spin_button_get_value(GTK_SPIN_BUTTON(crm)));
		g_key_file_set_double(key, "Params", "DifferentialCoercionPhase", gtk_spin_button_get_value(GTK_SPIN_BUTTON(crp)));
		g_key_file_set_double(key, "Params", "BeamConvolution", gtk_spin_button_get_value(GTK_SPIN_BUTTON(cnv)));
		g_key_file_set_double(key, "Params", "CentreResonance", gtk_spin_button_get_value(GTK_SPIN_BUTTON(cl)));
		g_key_file_set_double(key, "Params", "Range", gtk_spin_button_get_value(GTK_SPIN_BUTTON(dl)));
		g_key_file_set_integer(key, "Params", "SpectralPoints", gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(nl)));
		g_key_file_set_string(key, "Params", "DataFolder", fld);
		g_key_file_set_string(key, "Params", "ResultsFolder", flr);
		plt=GTK_PLOT_LINEAR(pt1);
		pt=GTK_PLOT(pt1);
		str=g_strdup(plt->xlab);
		g_key_file_set_string(key, "Plot", "SpectrumTextX", str);
		g_free(str);
		str=g_strdup(plt->ylab);
		g_key_file_set_string(key, "Plot", "SpectrumTextY", str);
		g_free(str);
		str=pango_font_description_to_string(pt->lfont);
		g_key_file_set_string(key, "Plot", "SpectrumLabel", str);
		g_free(str);
		str=pango_font_description_to_string(pt->afont);
		g_key_file_set_string(key, "Plot", "SpectrumAxis", str);
		g_free(str);
		g_key_file_set_double(key, "Plot", "SpectrumMagnitudeRed", g_array_index((pt->rd), gdouble, 0));
		g_key_file_set_double(key, "Plot", "SpectrumMagnitudeGreen", g_array_index((pt->gr), gdouble, 0));
		g_key_file_set_double(key, "Plot", "SpectrumMagnitudeBlue", g_array_index((pt->bl), gdouble, 0));
		g_key_file_set_double(key, "Plot", "SpectrumMagnitudeAlpha", g_array_index((pt->al), gdouble, 0));
		g_key_file_set_double(key, "Plot", "SpectrumPhaseRed", g_array_index((pt->rd), gdouble, 1));
		g_key_file_set_double(key, "Plot", "SpectrumPhaseGreen", g_array_index((pt->gr), gdouble, 1));
		g_key_file_set_double(key, "Plot", "SpectrumPhaseBlue", g_array_index((pt->bl), gdouble, 1));
		g_key_file_set_double(key, "Plot", "SpectrumPhaseAlpha", g_array_index((pt->al), gdouble, 1));
		plt=GTK_PLOT_LINEAR(pt2);
		pt=GTK_PLOT(pt2);
		str=g_strdup(plt->xlab);
		g_key_file_set_string(key, "Plot", "SpatialTextX", str);
		g_free(str);
		str=g_strdup(plt->ylab);
		g_key_file_set_string(key, "Plot", "SpatialTextY", str);
		g_free(str);
		str=pango_font_description_to_string(pt->lfont);
		g_key_file_set_string(key, "Plot", "SpatialLabel", str);
		g_free(str);
		str=pango_font_description_to_string(pt->afont);
		g_key_file_set_string(key, "Plot", "SpatialAxis", str);
		g_free(str);
		g_key_file_set_double(key, "Plot", "SpatialMagnitudeRed", g_array_index((pt->rd), gdouble, 0));
		g_key_file_set_double(key, "Plot", "SpatialMagnitudeGreen", g_array_index((pt->gr), gdouble, 0));
		g_key_file_set_double(key, "Plot", "SpatialMagnitudeBlue", g_array_index((pt->bl), gdouble, 0));
		g_key_file_set_double(key, "Plot", "SpatialMagnitudeAlpha", g_array_index((pt->al), gdouble, 0));
		g_key_file_set_double(key, "Plot", "SpatialPhaseRed", g_array_index((pt->rd), gdouble, 1));
		g_key_file_set_double(key, "Plot", "SpatialPhaseGreen", g_array_index((pt->gr), gdouble, 1));
		g_key_file_set_double(key, "Plot", "SpatialPhaseBlue", g_array_index((pt->bl), gdouble, 1));
		g_key_file_set_double(key, "Plot", "SpatialPhaseAlpha", g_array_index((pt->al), gdouble, 1));
		str=g_key_file_to_data(key, &size, NULL);
		fout=g_build_filename(pdr, "session.conf", NULL);
		g_file_set_contents(fout, str, size, &Err);
		{g_free(str); g_free(fout); g_key_file_free(key);}
		if (Err)
		{
			str=g_strdup_printf(_("Error Saving file: %s."), (Err->message));
			gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
			{g_free(str); g_error_free(Err);}
		}
	}
	else
	{
		str=g_strdup(_("Error Creating config directory."));
		gtk_statusbar_push(GTK_STATUSBAR(sbr), gtk_statusbar_get_context_id(GTK_STATUSBAR(sbr), str), str);
		g_free(str);
	}
	g_free(pdr);
}
