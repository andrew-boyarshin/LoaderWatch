# LoaderWatch

Toolbox to assist debugging, tracing and memory inspection of Windows PE image loader (LDR) NTDLL component.

## Features
* Managed debugger for unmanaged applications
* Stack traces on exceptions with symbol name resolution
* Complete Windows 10 LDR internals .NET type model (except Enclaves)
* Windows WDM kernel driver to inject custom module to assist hooking LDR methods
* Optional Windows Registry operations logging
* Hooking of non-exported LDR methods thanks to IPC and DbgHelp

## Get Started
* Ensure latest .NET Core 5.0+ daily build is installed
* Ensure latest Visual Studio 16.2+ is installed
* Ensure latest Windows SDK and WDK are installed (18945+)
* Build the solution
* Provision **remote** (local will **not** do, use VMs) machine for WDK driver deployment
* Ensure remote machine is ready for driver installation (e.g. kernel debugger is attached), otherwise driver test signature verification will fail
* Deploy *LoaderWatch.Inject.Driver* to remote machine
* Copy the `$(SolutionDir)bin\$(Configuration)\` folder to remote machine
* Run `LoaderWatch.CLI.exe [executable]`, *executable* is `notepad.exe` by default

Note: you will likely need to start `LoaderWatch.CLI.exe` as administrator at least once for each executable you want to debug (to set `ShowSnaps` flag in registry for the executable image).

Note: you might need to create `C:\Symbols` directory to preserve PDB files.

Note: .NET Core 3.0 will not suffice.

Note: Unless you publish *LoaderWatch.CLI* as self-contained application, the exact same .NET Core version must be installed on remote machine.

Note: if you do not install kernel-mode driver, LoaderWatch will continue to work in a very limited mode (no module injection, private method hooking or features dependent on the aforementioned components).

## License
1. Whatever hell the legal intersection of all licenses in Copyright section is.
2. All code written by Andrew Boyarshin or any other LoaderWatch contributors is licensed under MIT. Enjoy.

Note: no explicit or implicit commercial usage is allowed in any way due to point 1.

Note: the Windows 10 LDR internals .NET type model is based on non-free non-public knowledge and is subject to the latest (June 2018) [Microsoft Windows 10 EULA](aka.ms/useterms). Educational use only.

## Copyright
* [MemorySharp](https://github.com/ZenLulz/MemorySharp/tree/deepening-project) - MIT
* [phlib](https://github.com/processhacker/processhacker) - [GNU GPL Version 3 with linking exception](https://github.com/processhacker/processhacker/blob/master/LICENSE.txt), dependencies: MD5 & SHA & strnatcmp sections of [COPYRIGHT.txt](https://github.com/processhacker/processhacker/blob/master/COPYRIGHT.txt), explicit opt-out of Mini-XML & PCRE & jsonc
* [injdrv](https://github.com/wbenny/injdrv) - MIT
* [Detours](https://github.com/Microsoft/Detours) - MIT
* [mdbg](https://github.com/SymbolSource/Microsoft.Samples.Debugging) - [Microsoft Software EULA](https://github.com/SymbolSource/Microsoft.Samples.Debugging/blob/master/eula.rtf)
* [PATH executable resolution routine (Stack Overflow)](https://stackoverflow.com/a/11672569) - [CC-BY-SA 3.0](https://creativecommons.org/licenses/by-sa/3.0/) ([Rupert Wood](https://stackoverflow.com/users/243245/rup) and [Serj-Tm](https://stackoverflow.com/users/1034136/serj-tm))
* [.gitignore](https://github.com/github/gitignore) - Creative Commons Zero v1.0 Universal
