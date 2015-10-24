Name: capi-location-manager
Summary: A Location Manager library in Tizen Native API
Version: 0.6.3
Release: 1
Group: Framework/Location
License: Apache-2.0
Source0: %{name}-%{version}.tar.gz
Source1001: %{name}.manifest
BuildRequires: cmake
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(location)
BuildRequires: pkgconfig(capi-base-common)
BuildRequires: pkgconfig(capi-system-info)
BuildRequires: pkgconfig(vconf)
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
A Location Manager library in Tizen Native API


%package devel
Summary:  A Location Manager library in Tizen Native API (Development)
Group:    Location/Development
Requires: %{name} = %{version}-%{release}

%description devel
A Location Manager library in Tizen Native API (Development)


%prep
%setup -q
cp %{SOURCE1001} .

%build

export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"

MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DFULLVER=%{version} -DMAJORVER=${MAJORVER} \

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

mkdir -p %{buildroot}/usr/share/license
cp LICENSE %{buildroot}/usr/share/license/%{name}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%manifest %{name}.manifest
/usr/share/license/%{name}
%{_libdir}/libcapi-location-manager.so.*

%files devel
%{_includedir}/location/*.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libcapi-location-manager.so


%if 1
%package test
Summary:    Test application of Location Manager
Group:      Framework/maps
Requires: %{name} = %{version}-%{release}

%description test
Test application of Location Manager

%files test
%manifest test/capi-location-manager-test.manifest
/etc/smack/accesses.d/capi-location-manager-test.efl
/opt/usr/devel/location/location_test
/usr/share/packages/capi-location-manager-test.xml
%endif
