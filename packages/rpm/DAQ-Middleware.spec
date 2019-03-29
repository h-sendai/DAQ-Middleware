%define daqmw_dir daqmw
%{!?python_sitelib: %define python_sitelib %(python -c "from distutils.sysconfig import get_python_lib; print get_python_lib()")}

Summary: DAQ Middleware
Name: DAQ-Middleware
Version: 1.4.3
Release: 0%{?dist}
Group: Development/Libraries
Source: http://daqmw.kek.jp/src/DAQ-Middleware-%{version}.tar.gz
URL: http://daqmw.kek.jp/
License: Eclipse Public License
BuildRoot: %{_tmppath}/%{name}-root

BuildRequires: OpenRTM-aist >= 1.0.0-8
%if 0%{?rhel} >= 6
Requires: OpenRTM-aist >= 1.0.0-8 xalan-c-devel xerces-c-devel /etc/ld.so.conf.d libuuid-devel mod_wsgi tkinter boost-devel
%else
Requires: OpenRTM-aist >= 1.0.0-8 xalan-c-devel xerces-c-devel /etc/ld.so.conf.d mod_python tkinter boost-devel
%endif



#BuildRequires: omniORB, omniORB-devel, python, OpenRTM-aist
#Requires: omniORB, omniORB-utils, omniORB-devel, omniORB-servers, omniORB-bootscripts, omniORB-doc, python, OpenRTM-aist
#

%description
This is DAQ-Middleware, a middleware for Data Acquisition System.

%prep
%setup -q

%build
# export CFLAGS="$RPM_OPT_FLAGS -fPIC"
# %%configure
make

%install
rm -rf ${RPM_BUILD_ROOT}
mkdir -p ${RPM_BUILD_ROOT}
mkdir -p ${RPM_BUILD_ROOT}/etc/ld.so.conf.d
make install DESTDIR=$RPM_BUILD_ROOT

# make install prefix=$RPM_BUILD_ROOT/usr
# mkdir -p $RPM_BUILD_ROOT/etc/ld.so.conf.d
# echo "%{_libdir}/daqmw" > $RPM_BUILD_ROOT/etc/ld.so.conf.d/daqmiddleware-%{_arch}.conf

%clean
rm -rf ${RPM_BUILD_ROOT}

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
/etc/ld.so.conf.d/*
%{_bindir}/*
%{_libexecdir}/%{daqmw_dir}/*
%{_libdir}/%{daqmw_dir}/*
%{_includedir}/%{daqmw_dir}/*
%{_datadir}/%{daqmw_dir}/*
%{_var}/www/html/%{daqmw_dir}/*
%{_sysconfdir}/httpd/conf.d/daq.conf
%{python_sitelib}/%{daqmw_dir}/*
%{python_sitelib}/daqmw.pth

#%%{_libdir}/%%{daqmw_dir}/*
#%%{_includedir}/%%{daqmw_dir}/*
#%%{_datadir}/%%{daqmw_dir}/*

#
# %%doc README COPYING
# %%{_bindir}/*
# /%%{_lib}/lib*.so.*
# %%{_mandir}/*/*

%changelog
* Thu Mar 14 2019 Hiroshi Sendai
- Prepare for 1.4.3 release

* Mon Jan 16 2017 Hiroshi Sendai
- Build for 1.4.2 release

* Tue Jun 28 2016 Hiroshi Sendai
- Prepare for release 1.4.2

* Wed Jul 22 2015 Hiroshi Sendai
- add -i option for run.py to include IP addresses in log filename.

* Fri Jul 03 2015 Hiroshi Sendai
- Bug Fix: run.py -T did not work when remote boot was used.

* Tue May 12 2015 Hiroshi Sendai
- Bug Fix: DAQ-Operator in Web mode consumed CPU (100%) after DAQ-Component reloaded
  by run.py

* Tue Feb 17 2015 Hiroshi Sendai
- Prepare for next release (1.4.0)
- Eliminated rpmbuild "bogus date" warnings due to inconsistent weekday,
  by assuming the date is correct and changing the weekday.
  The script is at:
  https://bugzilla.redhat.com/show_bug.cgi?id=1000019

* Wed Dec 24 2014 Hiroshi Sendai
- Rebuild for 1.3.1

* Thu Nov 28 2013 Hiroshi Sendai
- Rebuild for 1.3.0

* Thu Sep 19 2013 Hiroshi Sendai
- Include mod_wsgi or mod_python in run dependencies.
- Prepare for next release.

* Wed Jul 31 2013 Hiroshi Sendai
- Rebuild for 1.2.2

* Mon Jul 29 2013 Hiroshi Sendai
- Rebuild for 1.2.2

* Fri Jul 26 2013 Hiroshi Sendai
- Rebuild for 1.2.2

* Tue Jun 25 2013 Hiroshi Sendai
- Prepare for 1.2.2.

* Fri Sep 28 2012 Hiroshi Sendai
- Rebuild for 1.2.1 RELEASE.

* Wed Sep 19 2012 Hiroshi Sendai
- Prepare for 1.2.1.

* Fri Jun 29 2012 Hiroshi Sendai
- Release 1.2.0 build.

* Mon Apr 16 2012 Hiroshi Sendai
- 1st trial for DAQ-Middleware 1.2.0

* Wed Apr 11 2012 Hiroshi Sendai
- Fix compile error on Sock library when using gcc 4.7 due to
  no <unistd.h>.

* Tue Mar 13 2012 Hiroshi Sendai
- Fix buffer overflow in DaqOperatorComp at event_byte_size.

* Tue Feb 07 2012 Hiroshi Sendai
- makefile's in src/DaqOperator/ changed.

* Wed Jan 18 2012 Hiroshi Sendai
- Add daqcom.
- Update RPM Spec file for daqcom.

* Tue Jan 17 2012 Hiroshi Sendai
- Release test for 1.1.2.
- Fix daq.js bug (send SYN every time we click the buttons) and update operatorPanel0.html for this fix.

* Thu Dec 01 2011 Hiroshi Sendai
- Build Release RPM (1.1.1-7).

* Tue Nov 29 2011 Hiroshi Sendai
- Reduce sleep time in daq_dummy not to timeout in ParameterClient.

* Thu Nov 17 2011 Hiroshi Sendai
- Build Release RPM for 1.1.1.

* Fri Nov 11 2011 Hiroshi Sendai
- Release process for 1.1.1.

* Wed Nov 02 2011 Hiroshi Sendai
- Fix memory leak on DaqOperator again.

* Thu Oct 20 2011 Hiroshi Sendai
- Fix memory leak on DaqOpeartor in configuration parser.

* Tue Oct 18 2011 Hiroshi Sendai
- First trial to fix browser close, reopen cycle.  (Known Bug: Firefox allocate 
huge memory (over 1GB and more)

* Fri Oct 14 2011 Hiroshi Sendai
- First trial to fix memory leak on set_status().

* Fri Jun 17 2011 Hiroshi Sendai
- Release 1.1.0

* Tue Feb 08 2011 Hiroshi Sendai
- Release 1.0.2

* Fri Jan 21 2011 Hiroshi Sendai
- Release 1.0.1

* Fri Jun 18 2010 Hiroshi Sendai
- Add /etc/ld.so.conf.d/daqmiddleware-arch.conf
* Thu Jun 17 2010 Hiroshi Sendai
- First trial.
* Tue Jun 15 2010 Hiroshi Sendai
- Create
