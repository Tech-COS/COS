# Compatible Operating System (COS)

All listed commands assume one is at the root of this repository.

## Project Deliverables

### Industrialize a technical component of the project

MemoryManagement Module: https://github.com/Tek-EIP/MemoryManagement
Documentation: https://github.com/Tek-EIP/Documentation

### Create a mini‑tool, SDK, or open API around the project

FileSystem Module: https://github.com/Tek-EIP/FileSystem
Documentation: https://github.com/Tek-EIP/Documentation

## Documentation

One may find the documentation of the project under the Github Wiki associated with this repository.   
https://github.com/Tek-EIP/Documentation/wiki

## Building
To build the project, one must install several dependencies first.

* Under Fedora:

    * Install the following packages with dnf: ``grub2``, ``grub2-tools-extra``, ``qemu``, ``make``, ``gcc``, ``nasm`` and ``xorriso``.

    * Download the x86_64 toolchain: https://github.com/lordmilko/i686-elf-tools/releases/download/13.2.0/x86_64-elf-tools-linux.zip

    * Copy all resulting files in /usr.

* Under Arch Linux:

    * Install the following packages with pacman: ``qemu-desktop`` (or ``qemu-full`` if it doesn't work right away), ``grub``, ``xorriso``, ``mtools``, ``make``, ``gcc`` and ``nasm``.

    * Install the following packages from the Arch User Repository (AUR): ``x86_64-elf-gcc`` and ``x86_64-elf-binutils``.

Then, run the ``make`` command to build the project.\
To build the unit tests, run ``make test_run`` instead.

## Running the project

**A RAW DISK IMAGE MUST BE MADE.**\
To do so, one may use the command: ``dd if=/dev/zero of=test_disk  bs=1M  count=128``\
Then, in the FileSystem submodule, run the ``make cos_mkfs`` command to build the mkfs binary.\
Using the command ``./cos_mkfs test_disk`` will format the image file to an incomplete version of the EXT4 specification.\
You may copy the resulting compiled binaries located in the ``build/bin`` folder of the main repository into the ``test_disk`` image by mounting it under an OS supporting the EXT4 specification.

**NB: OUR TESTS SEEM TO INDICATE THAT THE DISK IMAGE MUST BE UNMOUNTED BEFORE THE FILES ARE COMMITED TO DISK !!**

Then, run the following command: ``qemu-system-x86_64 -boot d -cdrom cos.iso -m 512 -drive file=test_disk,format=raw,index=0,if=ide -monitor stdio -rtc base=utc,driftfix=slew -cpu max,+sse,+sse2,+sse3,enforce``

As of now, the unit_tests aren't unified.\
After they are built, one may run ``./test_run`` and ``./MemoryManagement/test_run`` instead.

## Functionality
The ``clear`` command clears the terminal window.\
The ``launch`` command executes a program in RAM.\
The ``ls`` command lists files in a directory.\
The ``touch`` command creates a new file in a directory.\
The ``stat`` command returns a file's metadata.\
The ``mv`` command moves a file from one directory into another.\
The ``cat`` command displays the contents of a file.\


## Debug
* Using VSCode:

    * To display all registers at the current time with vscode, one can type ``-exec info registers`` on the debug console.

* Using a UNIX Terminal:

    * The following command must be run in a first terminal window: ``qemu-system-x86_64 -S -gdb tcp::1234 -boot d -cdrom cos.iso -m 512 -drive file=test_disk,format=raw,index=0,if=ide -monitor stdio -rtc base=utc,driftfix=slew -cpu max,+sse,+sse2,+sse3,enforce``

    * One may then run the following command in a second terminal window: ``gdb ./isodir/boot/cos.bin``

    * Under gdb, execute: ``target remote localhost:1234``
