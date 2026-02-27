
# Third-Party Notices

This file documents third-party projects that inspired or influenced the Compatible Operating System (COS) project.

---

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
