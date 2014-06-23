Summary:	Provides a Toeplitz Inner border inverse scattering solver for optical media
Name:		is-tib
Version:	0.1.0
Release:	1%{?dist}
License:	GPLv2+
Group:		Applications/Engineering
Source:		%{name}-%{version}.tar.gz
BuildRequires:	automake, autoconf, fftw-devel >= 3.0, gettext, gnome-doc-utils, gtk2 >= 2.18, gtk2plot, pkgconfig
Requires:	fftw >= 3.0, gtk2 >= 2.18, gtk2plot

%description
%{name} provides a GUI for displaying reflection spectra and 
transforming them into the required spatial refractive index 
modulation profiles. The method used is the Toeplitz Inner 
bordering method developed by Belai et. al. which performs 
Inverse scattering quickly, O(N^2), and accurately (the 
Gel'fand-Levitan-Marchenko Equations are solved exactly. 
A direct scattering (transfer matrix based) method is also 
included to allow system constraints to be included and 
modelled as well as for adaptively altering profiles.

%prep
%setup -q

%build
%configure
make %{?_smp_mflags}

%install
%make_install
desktop-file-install --dir=${RPM_BUILD_ROOT}%{_datadir}/applications is_tib.desktop
%find_lang %{name}

%files -f %{name}.lang
%defattr(-,root,root)
%doc COPYING COPYING-DOCS README
%{_bindir}/InvScatt
%{_datadir}/applications/is_tib.desktop
%{_datadir}/gnome/help/InvScatt-help/C/InvScatt-help.xml
%{_datadir}/omf/InvScatt-help/InvScatt-help-C.omf
%{_datadir}/pixmaps/triple.png

%changelog
* Mon Jun 23 2014 Paul Childs <pchilds@physics.org> 0.1.0-1
- Initial build
