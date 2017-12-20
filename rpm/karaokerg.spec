Name:           karaokerg
Version:        0.11.0 
Release:        1%{?dist}
Summary:        Tool to apply ReplayGain adjustments to mp3+g zip files 

License:        GPL
URL:            https://karaokerg.org
Source0:        karaokerg-0.11.0.tar.bz2 

BuildRequires:  qt5-qtbase-devel
Requires:       qt5-qtbase mp3gain

%description
Simple GUI tool to do ReplayGain analysis and adjustments to mp3's inside mp3+g zip karaoke collections

%prep
%setup -q


%build
%{qmake_qt5} PREFIX=$RPM_BUILD_ROOT/usr
%make_build


%install
%make_install

%files
/usr/bin/KaraokeRG
/usr/share/applications/karaokerg.desktop
#/usr/share/pixmaps/okjicon.svg

%changelog
* Wed Dec 20 2017 T. Isaac Lightburn <isaac@hozed.net>
- 
