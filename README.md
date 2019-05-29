# Emperor's Clothes

A DirectShow filter that hides idle cursor.

When playing a video, it is annoying to have the mouse cursor hanging over the screen. While some video players offer option to hide the cursor during playback (e.g. [PotPlayer](https://user-images.githubusercontent.com/975235/58520991-6194b780-816f-11e9-9b4a-12b413d1a740.jpg)), this feature is not available elsewhere (most notably in [MPC-BE window mode](https://sourceforge.net/p/mpcbe/tickets/210/)). Instead of reinventing the wheel in each player, this repository offers a generic DirectShow filter solution. All players that support DirectShow filters can enjoy the hide cursor feature. This filter should not conflict with players that already have the similar functionality fully or partially.

This filter completely passes through video and audio data. Thus it is safe to put it anywhere in the filter chain (except in certain combos such as [BlueskyFRC](http://bluesky23.yukishigure.com/en/BlueskyFRC.html) with Zero-Copy mode and [madVR](http://madvr.com/)).

This filter activates when there is video data and during playback. It deactivates whenever the playback is paused. User can customize the hiding delay in filter property dialog.

## Install

* Unpack
* Run install.bat to register the filter `emperors_clothes.ax`
* Enable the filter `Emperor's Clothes` in video player

## Uninstall

* Run uninstall.bat to unregister the filter and clean up user data

## Build

A build script `build.bat` is included to automate the process. It obtains dependencies and triggers compiling. The project depends on the DirectShow filter base classes from https://github.com/microsoft/Windows-classic-samples. Microsoft has not updated the sample for long time, and the sample solution is still on Visual Studio 2005. One needs to upgrade the solution before building it.
