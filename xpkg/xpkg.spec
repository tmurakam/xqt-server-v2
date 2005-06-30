%define name	xpkg
%define version	1.0.0
%define release	1

%define	prefix	/usr

Name: 		%{name}
Version: 	%{version}
Release: 	%{release}
Copyright: 	GPL
Group: 		Development/Tools
Url: 		http://xqt.sourceforge.net/
Source: 	%{name}-%{version}.tar.gz
Summary: 	A cross compilation tools for X/Qt project.
Vendor: 	X/Qt Project
Packager: 	Takuya Murakami <tmurakam@mtd.biglobe.ne.jp>
BuildRoot: 	%{_tmppath}/%{name}-%{version}-root
BuildArch:	noarch
Requires:	ruby >= 1.6

%description
This is a small utility for X/Qt project to cross compile the
module and create the package files (ipkg and so on).

%package config-xqt
Group: 		Development/Tools
Summary: 	xpkg configuration files for X/Qt.
Requires:	xpkg

%description config-xqt
Xpkg configuration files for X/Qt project.

%prep

%setup

%build

%install
[ $RPM_BUILD_ROOT != / ] && rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

%post

%clean

%files
%defattr(-,root,root)

%doc AUTHORS COPYING README INSTALL ChangeLog
%doc %{prefix}/man/man[1-9]/*
%{prefix}/bin/*
%{prefix}/lib/xpkg

%files config-xqt
%defattr(-,root,root)
%config /etc/xpkg

%changelog
* Tue Jun 28 2005 Takuya Murakami <tmurakam@mtd.biglobe.ne.jp> 0.1
- Initial
