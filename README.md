# Turbo C like <conio.h> for windows and Linux

This library implements (parts) the of old Turbo C conio.h
See header file for suported functions.

To avoid name conflicts a prefix "c_" was added into the original functions.


## Original documentation:

http://docs.embarcadero.com/products/rad_studio/radstudio2007/RS2007_helpupdates/HUpdate4/EN/html/devwin32/coniohpart_xml.html


## Windows 10
Obs: Windows 10 suports console virtual terminal sequences

## Lua wrapper (luarocks)

This fork provides a API for Lua scripts and can be stalled with "luarocks".


### Installation

Download the mod from [GitHub](https://github.com/joe7575/conio/archive/master.zip),
extract the file from the ZIP archive.

Change your directory  to `conio-master` and install the Lua library with:

```
luarocks make
```

For Linux like systems, use:

```
sudo luarocks make
```

For the installation of 'luarocks' (if not already available),
see [luarocks](https://luarocks.org/)

### Current Status

The library is tested on Linux/Ubuntu so far.
It probably won't work under Windows yet...

## History

### v1.0.0 (2024-04-01)

- Forked from [thradams/conio](https://github.com/thradams/conio)
- Lua wrapper/API and demo "test.lua" added



