%define name	xpkg
%define version	1.0.5
%define release	1

%define	prefix	/usr

Name: 		%{name}
Version: 	%{version}
Release: 	%{release}
Copyright: 	GPL
Group: 		Development/Tools
Url: 		http://xqt.sourceforge.net/
Source: 	%{name}-%{version}.tar.gz
Summary: 	A cross compilation tools for X/Qt server project.
Vendor: 	X/Qt Project
Packager: 	Takuya Murakami <tmurakam@mtd.biglobe.ne.jp>
BuildRoot: 	%{_tmppath}/%{name}-%{version}-root
BuildArch:	noarch
Requires:	ruby >= 1.6

%description
This is a small utility for X/Qt server project to cross compile the
module and create the package files (ipkg and so on).

%package config-xqt
Group: 		Development/Tools
Summary: 	xpkg configuration files for X/Qt.
Requires:	xpkg

%description config-xqt
Xpkg configuration files for X/Qt server project.

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
%doc %{prefix}/share/man/man[1-9]/*
%doc %{prefix}/share/man/ja/man[1-9]/*
%{prefix}/bin/*
%{prefix}/lib/xpkg
/etc/xpkg/dot.xpkg.sample

%files config-xqt
%defattr(-,root,root)
%config /etc/xpkg/config-xqt.def
%config /etc/xpkg/config-xqt.site
%config /etc/xpkg/config.rb

%changelog
* Tue Jul 1 2005 Takuya Murakami <tmurakam@mtd.biglobe.ne.jp> 1.0.0
- Add some utilities

* Tue Jun 28 2005 Takuya Murakami <tmurakam@mtd.biglobe.ne.jp> 1.0.0
- Initial
