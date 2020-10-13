<p style="text-align: center;">
    <img src="android/res/mipmap-xxxhdpi/ic_launcher.png">
</p>

# Stellarium-android
A modded version of Stellarium-Mobile

Fork of [chengxinlun's Stellarium-Mobile](https://github.com/chengxinlun/Stellarium-android)

Mobile platform specific source code http://stelladata.noctua-software.com/stellarium-mobile/stellarium-mobile-1.29.tar.gz 
Assets from https://code.launchpad.net/stellarium/
Note that changes have been made to the source code and assets (mainly use uncompressed PC version assets instead of compressed ones)

Copyright: Original Stellarium dev team

## Features

* Extend default star catalog limiting magnitude to 13.5
* Allow custom configuration files and catalogs (Put them in ```/sdcard/stellarium```. Same file structure to ```mobileData/data```)
* Tweak UI size for better handling on mobile
* Correct for magnetic declination angle (Magnetic North is not real North)
* Higher quality texture compared to official Stellarium-Mobile

## Planned features

- Planets
    - [X] Draw orbit when selected

- Satellite
    - [X] Support for Starlink.
    - [X] Reduced hint sizes, show when visible and magnitude low.
    - [X] Special yellow color for ISS.
    - [X] Draw orbit when selected.

- Quasars
    - [X] Add support for quasars
    - [X] Online updated

- Exoplanets
    - [X] Add support for exoplanets
    - [X] Online updated

- Meteor Showers
    - [X] Add support for meteors
    - [X] Online updated

- Telescope Control
    - [ ] Add support for telescope control
    - [ ] Edit UI to support it

- UI
    - [ ] Improve search dialog

- Performance
    - [ ] 30fps stable

## Prerequisites

* Qt-for-android (Known to work on qt-5.11 and 5.12)

Please note that for version ealier than 5.10, API for permission models for Android 6.0+ were not correctly implemented in Qt.
Notably, Qt did not provide any API for requesting ```read/write to external storage``` permission. This permission is essential
to the application.

## Building APK

Set up android SDK and NDK, and environment in QtCreator.
Open ```stellarium.pro``` in QtCreator.
Push the Build button.

If the building process runs into problems while compiling ```dummy.cpp```, simply copy the file to build directory and rename it ```moc_predefs.h```.

## Localization guide

TODO

## Change Log

update

### October 13, 2020

update comets (GVB or MPC)

### October 10, 2020

add dss survey.

Search and locate; new Type; Nebula data changer; bug fix (label show when size=0); remove unuse panel.

### June 2, 2019

Upgrading Main app and texture manager to implement QtConcurrent and Thread for increases speed.
Not enough rapid.

### June 1, 2019

Editing Satellite module for better visualization
Adding support for Quasars and Exoplanets

### April 3, 2019

Update to mobile source code to 1.29.6
Clean up unused files

### June 21, 2018

Android 8.0 support
Runtime permission request

### May 4, 2017

New feature: magnetic declination correction

### February 20, 2017

Minor bug fix
