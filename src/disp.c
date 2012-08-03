/***************************************************************************
 *            disp.c
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

#include "disp.h"

GtkWidget *bt1, *bt2, *bt3, *bt4, *cs1, *cs2, *en1, *en2, *en3, *en4, *nb2, *jix, *jix2;
GdkColormap *cmp;

void dpa(GtkWidget *wgt, gpointer dta)
{
	gchar *str;
	GdkColor clr;
	gdouble mny, mxy, xi, xf;
	gdouble *dpr;
	GtkPlotLinear *plt;
	guint16 alp;

	switch (gtk_notebook_get_current_page(GTK_NOTEBOOK(nb2)))
	{
		case 1:
		plt=GTK_PLOT_LINEAR(pt2);
		g_free(plt->xlab);
		g_free(plt->ylab);
		pango_font_description_free(plt->lfont);
		pango_font_description_free(plt->afont);
		(plt->xlab)=g_strdup(gtk_entry_get_text(GTK_ENTRY(en3)));
		(plt->ylab)=g_strdup(gtk_entry_get_text(GTK_ENTRY(en4)));
		str=g_strdup(gtk_font_button_get_font_name(GTK_FONT_BUTTON(bt3)));
		(plt->lfont)=pango_font_description_from_string(str);
		g_free(str);
		str=g_strdup(gtk_font_button_get_font_name(GTK_FONT_BUTTON(bt4)));
		(plt->afont)=pango_font_description_from_string(str);
		g_free(str);
		gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(cs2), &clr);
		alp=gtk_color_selection_get_current_alpha(GTK_COLOR_SELECTION(cs2));
		if (gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(jix2)))
		{
			xi=g_array_index((plt->rd), gdouble, 0);
			g_array_free(r2, TRUE);
			r2=g_array_new(FALSE, FALSE, sizeof(gdouble));
			g_array_append_val(r2, xi);
			xi=((gdouble) (clr.red))/65535;
			g_array_append_val(r2, xi);
			xi=g_array_index((plt->gr), gdouble, 0);
			g_array_free(g2, TRUE);
			g2=g_array_new(FALSE, FALSE, sizeof(gdouble));
			g_array_append_val(g2, xi);
			xi=((gdouble) (clr.green))/65535;
			g_array_append_val(g2, xi);
			xi=g_array_index((plt->bl), gdouble, 0);
			g_array_free(b2, TRUE);
			b2=g_array_new(FALSE, FALSE, sizeof(gdouble));
			g_array_append_val(b2, xi);
			xi=((gdouble) (clr.blue))/65535;
			g_array_append_val(b2, xi);
			xi=g_array_index((plt->al), gdouble, 0);
			g_array_free(a2, TRUE);
			a2=g_array_new(FALSE, FALSE, sizeof(gdouble));
			g_array_append_val(a2, xi);
			xi=((gdouble) alp)/65535;
			g_array_append_val(a2, xi);
		}
		else
		{
			xf=g_array_index((plt->rd), gdouble, 1);
			g_array_free(r2, TRUE);
			r2=g_array_new(FALSE, FALSE, sizeof(gdouble));
			xi=((gdouble) (clr.red))/65535;
			g_array_append_val(r2, xi);
			g_array_append_val(r2, xf);
			xf=g_array_index((plt->gr), gdouble, 1);
			g_array_free(g2, TRUE);
			g2=g_array_new(FALSE, FALSE, sizeof(gdouble));
			xi=((gdouble) (clr.green))/65535;
			g_array_append_val(g2, xi);
			g_array_append_val(g2, xf);
			xf=g_array_index((plt->bl), gdouble, 1);
			g_array_free(b2, TRUE);
			b2=g_array_new(FALSE, FALSE, sizeof(gdouble));
			xi=((gdouble) (clr.blue))/65535;
			g_array_append_val(b2, xi);
			g_array_append_val(b2, xf);
			xf=g_array_index((plt->al), gdouble, 1);
			g_array_free(a2, TRUE);
			a2=g_array_new(FALSE, FALSE, sizeof(gdouble));
			xi=((gdouble) alp)/65535;
			g_array_append_val(a2, xi);
			g_array_append_val(a2, xf);
		}
		{(plt->rd)=r2; (plt->gr)=g2; (plt->bl)=b2; (plt->al)=a2;}
		g_object_get(G_OBJECT(pt2), "xmin", &xi, "xmax", &xf, "ymin", &mny, "ymax", &mxy, NULL);
		gtk_plot_linear_update_scale(pt2, xi, xf, mny, mxy);
		break;
		default:
		plt=GTK_PLOT_LINEAR(pt1);
		g_free(plt->xlab);
		g_free(plt->ylab);
		pango_font_description_free(plt->lfont);
		pango_font_description_free(plt->afont);
		(plt->xlab)=g_strdup(gtk_entry_get_text(GTK_ENTRY(en1)));
		(plt->ylab)=g_strdup(gtk_entry_get_text(GTK_ENTRY(en2)));
		str=g_strdup(gtk_font_button_get_font_name(GTK_FONT_BUTTON(bt1)));
		(plt->lfont)=pango_font_description_from_string(str);
		g_free(str);
		str=g_strdup(gtk_font_button_get_font_name(GTK_FONT_BUTTON(bt2)));
		(plt->afont)=pango_font_description_from_string(str);
		g_free(str);
		gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(cs1), &clr);
		alp=gtk_color_selection_get_current_alpha(GTK_COLOR_SELECTION(cs1));
		if (gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(jix)))
		{
			xi=g_array_index((plt->rd), gdouble, 0);
			g_array_free(r1, TRUE);
			r1=g_array_new(FALSE, FALSE, sizeof(gdouble));
			g_array_append_val(r1, xi);
			xi=((gdouble) (clr.red))/65535;
			g_array_append_val(r1, xi);
			xi=g_array_index((plt->gr), gdouble, 0);
			g_array_free(g1, TRUE);
			g1=g_array_new(FALSE, FALSE, sizeof(gdouble));
			g_array_append_val(g1, xi);
			xi=((gdouble) (clr.green))/65535;
			g_array_append_val(g1, xi);
			xi=g_array_index((plt->bl), gdouble, 0);
			g_array_free(b1, TRUE);
			b1=g_array_new(FALSE, FALSE, sizeof(gdouble));
			g_array_append_val(b1, xi);
			xi=((gdouble) (clr.blue))/65535;
			g_array_append_val(b1, xi);
			xi=g_array_index((plt->al), gdouble, 0);
			g_array_free(a1, TRUE);
			a1=g_array_new(FALSE, FALSE, sizeof(gdouble));
			g_array_append_val(a1, xi);
			xi=((gdouble) alp)/65535;
			g_array_append_val(a1, xi);
		}
		else
		{
			xf=g_array_index((plt->rd), gdouble, 1);
			g_array_free(r1, TRUE);
			r1=g_array_new(FALSE, FALSE, sizeof(gdouble));
			xi=((gdouble) (clr.red))/65535;
			g_array_append_val(r1, xi);
			g_array_append_val(r1, xf);
			xf=g_array_index((plt->gr), gdouble, 1);
			g_array_free(g1, TRUE);
			g1=g_array_new(FALSE, FALSE, sizeof(gdouble));
			xi=((gdouble) (clr.green))/65535;
			g_array_append_val(g1, xi);
			g_array_append_val(g1, xf);
			xf=g_array_index((plt->bl), gdouble, 1);
			g_array_free(b1, TRUE);
			b1=g_array_new(FALSE, FALSE, sizeof(gdouble));
			xi=((gdouble) (clr.blue))/65535;
			g_array_append_val(b1, xi);
			g_array_append_val(b1, xf);
			xf=g_array_index((plt->al), gdouble, 1);
			g_array_free(a1, TRUE);
			a1=g_array_new(FALSE, FALSE, sizeof(gdouble));
			xi=((gdouble) alp)/65535;
			g_array_append_val(a1, xi);
			g_array_append_val(a1, xf);
		}
		{(plt->rd)=r1; (plt->gr)=g1; (plt->bl)=b1; (plt->al)=a1;}
		g_object_get(G_OBJECT(pt1), "xmin", &xi, "xmax", &xf, "ymin", &mny, "ymax", &mxy, NULL);
		gtk_plot_linear_update_scale(pt1, xi, xf, mny, mxy);
		break;
	}
}

void upj(GtkWidget *wgt, gpointer dta)
{
	GdkColor cl1;
	guint16 alp;
	GtkPlotLinear *plt;

	plt=GTK_PLOT_LINEAR(pt1);
	if (gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(wgt)))
	{
		(cl1.red)=(guint16) (65535*g_array_index((plt->rd), gdouble, 1));
		(cl1.green)=(guint16) (65535*g_array_index((plt->gr), gdouble, 1));
		(cl1.blue)=(guint16) (65535*g_array_index((plt->bl), gdouble, 1));
		alp=(guint16) (65535*g_array_index((plt->al), gdouble, 1));
	}
	else
	{
		(cl1.red)=(guint16) (65535*g_array_index((plt->rd), gdouble, 0));
		(cl1.green)=(guint16) (65535*g_array_index((plt->gr), gdouble, 0));
		(cl1.blue)=(guint16) (65535*g_array_index((plt->bl), gdouble, 0));
		alp=(guint16) (65535*g_array_index((plt->al), gdouble, 0));
	}
	gdk_colormap_alloc_color(cmp, &cl1, FALSE, TRUE);
	gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(cs1), &cl1);
	gtk_color_selection_set_current_alpha(GTK_COLOR_SELECTION(cs1), alp);
}

void upj2(GtkWidget *wgt, gpointer dta)
{
	GdkColor cl1;
	guint16 alp;
	GtkPlotLinear *plt;

	plt=GTK_PLOT_LINEAR(pt2);
	if (gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(wgt)))
	{
		(cl1.red)=(guint16) (65535*g_array_index((plt->rd), gdouble, 1));
		(cl1.green)=(guint16) (65535*g_array_index((plt->gr), gdouble, 1));
		(cl1.blue)=(guint16) (65535*g_array_index((plt->bl), gdouble, 1));
		alp=(guint16) (65535*g_array_index((plt->al), gdouble, 1));
	}
	else
	{
		(cl1.red)=(guint16) (65535*g_array_index((plt->rd), gdouble, 0));
		(cl1.green)=(guint16) (65535*g_array_index((plt->gr), gdouble, 0));
		(cl1.blue)=(guint16) (65535*g_array_index((plt->bl), gdouble, 0));
		alp=(guint16) (65535*g_array_index((plt->al), gdouble, 0));
	}
	gdk_colormap_alloc_color(cmp, &cl1, FALSE, TRUE);
	gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(cs2), &cl1);
	gtk_color_selection_set_current_alpha(GTK_COLOR_SELECTION(cs2), alp);
}

void dpr(GtkWidget *wgt, gpointer dta)
{
	AtkObject *awg, *all;
	gchar *str;
	GdkColor cl1, cl2;
	gdouble iv;
	gint val;
	GtkAdjustment *adj;
	GtkWidget *btt, *hbx, *hwn, *lbl, *spr, *tbl, *vbx;
	guint16 alp;
	GtkPlotLinear *plt;

	hwn=gtk_dialog_new_with_buttons(_("Display Properties"), GTK_WINDOW(dta), GTK_DIALOG_DESTROY_WITH_PARENT, NULL);
	g_signal_connect_swapped(G_OBJECT(hwn), "destroy", G_CALLBACK(gtk_widget_destroy), G_OBJECT(hwn));
	btt=gtk_button_new_from_stock(GTK_STOCK_CLOSE);
	gtk_widget_show(btt);
	g_signal_connect_swapped(G_OBJECT(btt), "clicked", G_CALLBACK(gtk_widget_destroy), G_OBJECT(hwn));
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(hwn)->action_area), btt);
	btt=gtk_button_new_from_stock(GTK_STOCK_APPLY);
	gtk_widget_show(btt);
	g_signal_connect(G_OBJECT(btt), "clicked", G_CALLBACK(dpa), NULL);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(hwn)->action_area), btt);
	nb2=gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(nb2), GTK_POS_TOP);
	tbl=gtk_table_new(4, 2, FALSE);
	gtk_widget_show(tbl);
	plt=GTK_PLOT_LINEAR(pt1);
	lbl=gtk_label_new(_("Axis label font:"));
	gtk_widget_show(lbl);
	gtk_table_attach(GTK_TABLE(tbl), lbl, 1, 2, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	str=pango_font_description_to_string(plt->lfont);
	bt1=gtk_font_button_new_with_font(str);
	g_free(str);
	gtk_font_button_set_show_style(GTK_FONT_BUTTON(bt1), TRUE);
	gtk_font_button_set_show_size(GTK_FONT_BUTTON(bt1), TRUE);
	gtk_font_button_set_use_font(GTK_FONT_BUTTON(bt1), TRUE);
	gtk_font_button_set_use_size(GTK_FONT_BUTTON(bt1), FALSE);
	gtk_font_button_set_title(GTK_FONT_BUTTON(bt1), _("Font Selection for Axis Labels"));
	gtk_widget_show(bt1);
	gtk_table_attach(GTK_TABLE(tbl), bt1, 1, 2, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	awg=gtk_widget_get_accessible(bt1);
	all=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(all, ATK_RELATION_LABEL_FOR, awg);
	atk_object_add_relationship(awg, ATK_RELATION_LABELLED_BY, all);
	lbl=gtk_label_new(_("Tick label font:"));
	gtk_widget_show(lbl);
	gtk_table_attach(GTK_TABLE(tbl), lbl, 1, 2, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	str=pango_font_description_to_string(plt->afont);
	bt2=gtk_font_button_new_with_font(str);
	g_free(str);
	gtk_font_button_set_show_style(GTK_FONT_BUTTON(bt2), TRUE);
	gtk_font_button_set_show_size(GTK_FONT_BUTTON(bt2), TRUE);
	gtk_font_button_set_use_font(GTK_FONT_BUTTON(bt2), TRUE);
	gtk_font_button_set_use_size(GTK_FONT_BUTTON(bt2), FALSE);
	gtk_font_button_set_title(GTK_FONT_BUTTON(bt2), _("Font Selection for Tick Mark Labels"));
	gtk_widget_show(bt2);
	gtk_table_attach(GTK_TABLE(tbl), bt2, 1, 2, 3, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	awg=gtk_widget_get_accessible(bt2);
	all=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(all, ATK_RELATION_LABEL_FOR, awg);
	atk_object_add_relationship(awg, ATK_RELATION_LABELLED_BY, all);
	lbl=gtk_label_new(_("X axis text:"));
	gtk_widget_show(lbl);
	gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 1, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	en1=gtk_entry_new();
	str=g_strdup(plt->xlab);
	gtk_entry_set_text(GTK_ENTRY(en1), str);
	g_free(str);
	gtk_widget_show(en1);
	gtk_table_attach(GTK_TABLE(tbl), en1, 0, 1, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	awg=gtk_widget_get_accessible(en1);
	all=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(all, ATK_RELATION_LABEL_FOR, awg);
	atk_object_add_relationship(awg, ATK_RELATION_LABELLED_BY, all);
	lbl=gtk_label_new(_("Y axis text:"));
	gtk_widget_show(lbl);
	gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 1, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	en2=gtk_entry_new();
	str=g_strdup(plt->ylab);
	gtk_entry_set_text(GTK_ENTRY(en2), str);
	g_free(str);
	gtk_widget_show(en2);
	gtk_table_attach(GTK_TABLE(tbl), en2, 0, 1, 3, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	awg=gtk_widget_get_accessible(en2);
	all=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(all, ATK_RELATION_LABEL_FOR, awg);
	atk_object_add_relationship(awg, ATK_RELATION_LABELLED_BY, all);
	lbl=gtk_label_new("Index of Plot:");
	gtk_widget_show(lbl);
	gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 1, 4, 5, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	adj=(GtkAdjustment*) gtk_adjustment_new(0, 0, 1, 1.0, 1.0, 0.0);
	jix=gtk_spin_button_new(adj, 0, 0);
	g_signal_connect(G_OBJECT(jix), "value-changed", G_CALLBACK(upj), NULL);
	gtk_table_attach(GTK_TABLE(tbl), jix, 1, 2, 4, 5, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(jix);
	awg=gtk_widget_get_accessible(jix);
	all=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(all, ATK_RELATION_LABEL_FOR, awg);
	atk_object_add_relationship(awg, ATK_RELATION_LABELLED_BY, all);
	vbx=gtk_vbox_new(FALSE, 2);
	gtk_widget_show(vbx);
	gtk_box_pack_start(GTK_BOX(vbx), tbl, FALSE, FALSE, 2);
	spr=gtk_vseparator_new();
	gtk_widget_show(spr);
	cmp=gdk_colormap_get_system();
	(cl1.red)=(guint16) (65535*g_array_index((plt->rd), gdouble, 0));
	(cl1.green)=(guint16) (65535*g_array_index((plt->gr), gdouble, 0));
	(cl1.blue)=(guint16) (65535*g_array_index((plt->bl), gdouble, 0));
	alp=(guint16) (65535*g_array_index((plt->al), gdouble, 0));
	gdk_colormap_alloc_color(cmp, &cl1, FALSE, TRUE);
	cs1=gtk_color_selection_new();
	gtk_color_selection_set_has_opacity_control(GTK_COLOR_SELECTION(cs1), TRUE);
	gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(cs1), &cl1);
	gtk_color_selection_set_current_alpha(GTK_COLOR_SELECTION(cs1), alp);
	gtk_color_selection_set_has_palette(GTK_COLOR_SELECTION(cs1), TRUE);
	gtk_widget_show(cs1);
	hbx=gtk_hbox_new(FALSE, 2);
	gtk_widget_show(hbx);
	gtk_box_pack_start(GTK_BOX(hbx), vbx, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(hbx), spr, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(hbx), cs1, FALSE, FALSE, 2);
	lbl=gtk_label_new(_("Spectrum"));
	gtk_notebook_append_page(GTK_NOTEBOOK(nb2), hbx, lbl);
	tbl=gtk_table_new(4, 2, FALSE);
	gtk_widget_show(tbl);
	plt=GTK_PLOT_LINEAR(pt2);
	lbl=gtk_label_new(_("Axis label font:"));
	gtk_widget_show(lbl);
	gtk_table_attach(GTK_TABLE(tbl), lbl, 1, 2, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	str=pango_font_description_to_string(plt->lfont);
	bt3=gtk_font_button_new_with_font(str);
	g_free(str);
	gtk_font_button_set_show_style(GTK_FONT_BUTTON(bt3), TRUE);
	gtk_font_button_set_show_size(GTK_FONT_BUTTON(bt3), TRUE);
	gtk_font_button_set_use_font(GTK_FONT_BUTTON(bt3), TRUE);
	gtk_font_button_set_use_size(GTK_FONT_BUTTON(bt3), FALSE);
	gtk_font_button_set_title(GTK_FONT_BUTTON(bt3), _("Font Selection for Axis Labels"));
	gtk_widget_show(bt3);
	gtk_table_attach(GTK_TABLE(tbl), bt3, 1, 2, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	awg=gtk_widget_get_accessible(bt3);
	all=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(all, ATK_RELATION_LABEL_FOR, awg);
	atk_object_add_relationship(awg, ATK_RELATION_LABELLED_BY, all);
	lbl=gtk_label_new(_("Tick label font:"));
	gtk_widget_show(lbl);
	gtk_table_attach(GTK_TABLE(tbl), lbl, 1, 2, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	str=pango_font_description_to_string(plt->afont);
	bt4=gtk_font_button_new_with_font(str);
	g_free(str);
	gtk_font_button_set_show_style(GTK_FONT_BUTTON(bt4), TRUE);
	gtk_font_button_set_show_size(GTK_FONT_BUTTON(bt4), TRUE);
	gtk_font_button_set_use_font(GTK_FONT_BUTTON(bt4), TRUE);
	gtk_font_button_set_use_size(GTK_FONT_BUTTON(bt4), FALSE);
	gtk_font_button_set_title(GTK_FONT_BUTTON(bt4), _("Font Selection for Tick Mark Labels"));
	gtk_widget_show(bt4);
	gtk_table_attach(GTK_TABLE(tbl), bt4, 1, 2, 3, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	awg=gtk_widget_get_accessible(bt4);
	all=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(all, ATK_RELATION_LABEL_FOR, awg);
	atk_object_add_relationship(awg, ATK_RELATION_LABELLED_BY, all);
	lbl=gtk_label_new(_("X axis text:"));
	gtk_widget_show(lbl);
	gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 1, 0, 1, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	en3=gtk_entry_new();
	str=g_strdup(plt->xlab);
	gtk_entry_set_text(GTK_ENTRY(en3), str);
	g_free(str);
	gtk_widget_show(en3);
	gtk_table_attach(GTK_TABLE(tbl), en3, 0, 1, 1, 2, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	awg=gtk_widget_get_accessible(en3);
	all=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(all, ATK_RELATION_LABEL_FOR, awg);
	atk_object_add_relationship(awg, ATK_RELATION_LABELLED_BY, all);
	lbl=gtk_label_new(_("Y axis text:"));
	gtk_widget_show(lbl);
	gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 1, 2, 3, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	en4=gtk_entry_new();
	str=g_strdup(plt->ylab);
	gtk_entry_set_text(GTK_ENTRY(en4), str);
	g_free(str);
	gtk_widget_show(en4);
	gtk_table_attach(GTK_TABLE(tbl), en4, 0, 1, 3, 4, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	awg=gtk_widget_get_accessible(en4);
	all=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(all, ATK_RELATION_LABEL_FOR, awg);
	atk_object_add_relationship(awg, ATK_RELATION_LABELLED_BY, all);
	lbl=gtk_label_new("Index of Plot:");
	gtk_widget_show(lbl);
	gtk_table_attach(GTK_TABLE(tbl), lbl, 0, 1, 4, 5, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	adj=(GtkAdjustment*) gtk_adjustment_new(0, 0, 1, 1.0, 1.0, 0.0);
	jix2=gtk_spin_button_new(adj, 0, 0);
	g_signal_connect(G_OBJECT(jix2), "value-changed", G_CALLBACK(upj2), NULL);
	gtk_table_attach(GTK_TABLE(tbl), jix2, 1, 2, 4, 5, GTK_FILL|GTK_SHRINK|GTK_EXPAND, GTK_FILL|GTK_SHRINK|GTK_EXPAND, 2, 2);
	gtk_widget_show(jix2);
	awg=gtk_widget_get_accessible(jix2);
	all=gtk_widget_get_accessible(GTK_WIDGET(lbl));
	atk_object_add_relationship(all, ATK_RELATION_LABEL_FOR, awg);
	atk_object_add_relationship(awg, ATK_RELATION_LABELLED_BY, all);
	vbx=gtk_vbox_new(FALSE, 2);
	gtk_widget_show(vbx);	
	gtk_box_pack_start(GTK_BOX(vbx), tbl, FALSE, FALSE, 2);
	spr=gtk_vseparator_new();
	gtk_widget_show(spr);
	cs2=gtk_color_selection_new();
	(cl2.red)=(guint16) (65535*g_array_index((plt->rd), gdouble, 0));
	(cl2.green)=(guint16) (65535*g_array_index((plt->gr), gdouble, 0));
	(cl2.blue)=(guint16) (65535*g_array_index((plt->bl), gdouble, 0));
	alp=(guint16) (65535*g_array_index((plt->al), gdouble, 0));
	gdk_colormap_alloc_color(cmp, &cl2, FALSE, TRUE);
	gtk_color_selection_set_has_opacity_control(GTK_COLOR_SELECTION(cs2), FALSE);
	gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(cs2), &cl2);
	gtk_color_selection_set_has_palette(GTK_COLOR_SELECTION(cs2), TRUE);
	gtk_color_selection_set_current_alpha(GTK_COLOR_SELECTION(cs2), alp);
	gtk_widget_show(cs2);
	hbx=gtk_hbox_new(FALSE, 2);
	gtk_widget_show(hbx);
	gtk_box_pack_start(GTK_BOX(hbx), vbx, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(hbx), spr, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(hbx), cs2, FALSE, FALSE, 2);
	lbl=gtk_label_new(_("Spatial Structure"));
	gtk_notebook_append_page(GTK_NOTEBOOK(nb2), hbx, lbl);
	val=gtk_notebook_get_current_page(GTK_NOTEBOOK(nbk));
	gtk_notebook_set_current_page(GTK_NOTEBOOK(nb2), val);
	gtk_widget_show(nb2);
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(hwn)->vbox), nb2);
	gtk_widget_show(hwn);
}
