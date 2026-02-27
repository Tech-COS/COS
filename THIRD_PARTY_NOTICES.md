
# Third-Party Notices

This file documents third-party projects that inspired or influenced the Compatible Operating System (COS) project.

---

## Technology Watch/Tech Scouting

The following projects were reviewed as part of a technology watch to inform and inspire key design decisions.\
By studying how similar tools handle binary execution, compatibility, and storage, this watch aims to identify approaches that could strengthen the architecture and implementation of the current project.

Action Plan: take the paragraph explaining the selling points.

### Similar Projects:

1. **ReactOS**
   - Open-source reimplementation of Windows NT
   - Can natively run many Win32 applications
   - Platform: Custom OS
   - https://reactos.org/
   - Last Update: March 21rst, 2025
   - Status: Open Source

2. **CrossOver**
   - Commercial version of Wine with improved compatibility/support
   - Adds GUI and enterprise support
   - Platform: Linux, macOS
   - https://www.codeweavers.com/crossover
   - Last Update: March 11th, 2025
   - Status: Closed Source/Commercial

3. **DOSBox**
   - Emulates MS-DOS for running legacy DOS binaries on modern OSes
   - Ideal for old games and software
   - Platform: Windows, Linux, macOS
   - Website: https://www.dosbox.com/
   - Last Update: June 26th, 2019
   - Status: Open Source

4. **Box86 / Box64**
   - Userspace x86 and x86_64 emulators for ARM platforms (e.g., Raspberry Pi)
   - Allows running native Linux x86 apps on ARM
   - Platform: Linux on ARM
   - Website: https://box86.org/
   - Last Update: March 10th, 2025
   - Status: Open Source

5. **Darling**
   - macOS compatibility layer for Linux (similar to Wine)
   - Aims to run macOS binaries on Linux
   - Platform: Linux
   - Website: https://www.darlinghq.org/
   - Last Progress Report: Q2 2023
   - Last Github Update: Last week
   - Status: Open Source

6. **Proton**
   - Valve’s fork of Wine tailored for Steam games on Linux
   - Includes enhancements like DXVK and VKD3D
   - Platform: Linux (Steam)
   - Website: https://github.com/ValveSoftware/Proton
   - Last Update: April 30th, 2025
   - Status: Open Source

7. **Rosetta 2**
   - Apple’s binary translator to run x86_64 apps on ARM-based Macs
   - Built into macOS for Apple Silicon
   - Platform: macOS (M1, M2 chips)
   - Website: https://support.apple.com/en-us/102527
   - Clean Room Reverse Engineering: https://github.com/FFRI/ProjectChampollion
   - Last Update: ?
   - Status: Closed Source/Commercial

8. **CheerpX**
   - WebAssembly-based virtualization of x86 Linux/Windows binaries
   - Enables legacy apps in the browser
   - Platform: Web
   - Website: https://cheerpx.io/
   - Last Update: November 13th, 2024
   - Status: Closed Source/Commercial

9. **WINE**
   - Compatibility layer for running Windows applications on Unix-like operating systems
   - Enables Windows apps on Linux, macOS, and more without emulation
   - Platform: Linux, macOS, BSD
   - Website: https://www.winehq.org/
   - Last Update: May 16th, 2025
   - Status: Open Source

10. **XWine1**
   - Xbox One compatibility layer for Windows PCs
   - Runs select Xbox One games natively on Windows without full emulation
   - Platform: Windows
   - Website: https://x.com/XWineOne
   - Last Update: September 8th, 2024
   - Status: Closed Source


## Initialization Sequence

The only portion of the COS project that derives from external sources is the **initialization sequence** used during early kernel setup.\
While no code has been directly copied, the design of this sequence was **inspired by several open-source operating system projects**.\
The relevant projects include:

### Referenced Projects

1. **littleosbook**
   - **URL**: https://github.com/littleosbook/littleosbook
   - **License**: Creative Commons Attribution 3.0
   - **Use**: Educational reference for higher half model.

2. **OSDev**
   - **URL**: https://wiki.osdev.org/
   - **License**: Custom: https://wiki.osdev.org/OSDev_Wiki:License
   - **Use**: Educational reference for bootloader-to-kernel flow.

3. **FrOS Blog**
   - **URL**: https://github.com/FRosner/outpourings/blob/master/FrOS/blog_post_3.md
   - **License**: None/All rights reserved
   - **Use**: IDT and PIC Initialisation technique.

> The COS initialization sequence has been **re-implemented from scratch**, using only public documentation and available source code as a conceptual foundation.\
No direct source code from the listed projects has been included in the COS source tree.

---

## Fully Original Components

The following components of COS were developed independently and are **entirely original**:

- **Memory Management Module** – Handles virtual address mapping and structured memory paging.
- **Bytecode Loader** – Loads raw binary instructions directly from disk image into memory for execution.
- **Custom Ext4 Handler** – A partial Ext4 parser built from the official specification with no third-party source reuse.
- **Kernel Command Line Interface (CLI)** – Interprets user input and exposes kernel functionality.
- **Documentation & Testing Framework** – All test plans, user documentation, and design documentation are authored uniquely for COS.

---

## Summary

All source code and documentation in COS is governed by the MIT License unless otherwise explicitly stated.\
COS complies with all referenced licenses and includes this notice to ensure attribution and transparency.\
There should be **no license conflicts** between COS and its reference materials.
