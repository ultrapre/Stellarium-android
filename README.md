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
* 2K - 4K - 8K textures from [solarsystemscope.com](https://www.solarsystemscope.com/) distributed under *Attribution 4.0 International (CC BY 4.0)*

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
    - [ ] Material themed
    - [X] Full HD support for planet textures

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

## Using a release

**! Important !**
If updating from a previous release do not forget to delete the *stellarium* folder present in your internal storage.

See the lastest releases from the [releases page](https://github.com/raymas/Stellarium-android/releases)

## Localization guide

TODO

## Change Log

### October 9, 2020

Search and locate; new Type; Nebula data changer; bug fix (label show when size=0); remove unuse panel.

## 深空观测特别版V1.1

1.NGC搜索 / 回车定位 / 列表长度5+
2.可选：新版数据，新版类型
3.修复尺寸=0不显示bug
4.外部ini配置深空天体数据源
5.去除planes轨道线
6.去除quickbar无用的选项
7.测试外置

### June 11, 2020

DeepSky Special, extended info of DSO

## 深空观测特别版V1.0

1.修正深空类型等，从7种类型增加到19种类型。
2.自定义深空目录路径。
ngc2000.dat、ngc2000names.dat均可以自己设计放在/sdcard/stellarium/中。
3.修正了NGC/IC的所有数据。
4.增加了bmag、b-v mag、详细类型（比如I2r）、德雷尔描述（详细描述一个天体）
5.增加了1000+明亮的非NGC/IC深空，可以支持6位数数量级别的深空数量
6.支持按照梅西耶、NGC、Caldwell显示或不显示目标
7.支持按照星系、疏散星团等深空类型显示或不显示目标
8.支持观测清单，观测记录；可以按照设置显示或不显示已观测或者观测清单中的目标。

### January 2, 2020

Starlink magnitude updated. 2K and 8K support for major celestial bodies. Newline correction for Quasars, Exoplanets and MeteoShowers informations.

### June 2, 2019

Upgrading Main app and texture manager to implement QtConcurrent and Thread for increases speed.
Not enough fast.

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
