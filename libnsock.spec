Summary: A convenient socket library
Name: libnsock
Version: 0.9.6
Release: 3
License: GPL
Group: System Environment/Libraries
URL: ftp://qoop.org/libnsock/
Source0: ftp://qoop.org/libnsock/libnsock-%{version}.tgz
# Source1:
# Patch0:
Buildroot: %{_tmppath}/%{name}-%{version}-root
# BuildRequires: ncurses-devel
# Obsoletes: 


%package devel
Summary: Development libraries for programs written to use the nsock library.
Group: Development/Libraries
Requires: libnsock = %{version}


%description
The nsock library is a convenient socket library that handles many
common socket coding practices transparently.  This includes SSL,
virtual host use, IPv6 and more.


%description devel
The libnsock-devel package contains the include files and static
libraries necessary for developing programs which will use the nsock
library,


%prep
%setup -q
#%patch0 -p1 -b .p0


%build
%configure --enable-shared
make


%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}


%post -p /sbin/ldconfig


%postun -p /sbin/ldconfig


%clean
rm -rf %{buildroot}


%files
%defattr(-,root,root)
%doc CHANGES TODO
%{_libdir}/libnsock.so.*


%files devel
%defattr(-,root,root)
# %doc
%{_libdir}/libnsock.a
%{_libdir}/libnsock.so
/usr/include/nsock



%changelog
* Fri Jun 11 2004 Joshua J. Drake <libnsock@qoop.org>
- initial creation
