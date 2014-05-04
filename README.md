Lizarx
======

### Short description

> Lizarx will be an portable Unix-like Microkernel. 
The OS will also use dynamic linked shared objects to minimize Overhead between different Userspace Drivers, which are BTW written in C++.

### Usage

./build.sh # builds Lizarx with toolchain(use this, when building the first time)

./build.sh --no-toolchain # builds Lizarx without toolchain(use this, when you made small changes outside the toolchain )

./build.sh --arch-x86_64 # builds Lizarx with toolchain for x86_64 target(use this, when building the first time)

./build.sh --no-toolchain --arch-x86_64 # builds Lizarx without toolchain for x86_64 target(use this, when you made small changes outside the toolchain )

### Features

- [x] GDT
- [x] IDT
- [x] PMM
- [x] VMM
- [x] Multitasking
- [x] Multithreading(no pthreads wrapper yet and also not tested, but supposed to work)
- [x] Userspace
- [x] full OS-specific toolchain(including cross-compiler libc and newlib port)
- [x] Dynamic linker(ld.a)
- [x] full X86_64 port
- [x] basic SMP support(including SMP scheduling)
- [x] SYSCALL/SYSRET syscall(x86_64 only)
- [ ] VFS in C++
- [ ] tmpFS (should be a shared library of the VFS)
- [ ] unix like shell in C++ with OO semantics
- [ ] don't know yet :D
