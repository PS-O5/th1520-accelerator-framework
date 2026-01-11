# RISC-V C910 [TH1520] Accelerator Development Platform (On haitus till Jan'26 end)

A minimal, bare-metal development framework for the T-Head C910 (RISC-V 64-bit) processor, running FreeRTOS. This project serves as a baseline for developing and testing custom hardware accelerators (RoCC/MMIO). This project explores a full-stack **Host–Accelerator architecture** for accelerating equivariant neural networks (MACE)


## Project Structure

```text
.
├── app/            # User application and FreeRTOSConfig
├── freertos/       # FreeRTOS Kernel source
├── platform/       # Board Support Package (BSP)
│   ├── c910/       # Startup code, Linker scripts, IRQ handling
│   └── drivers/    # Custom Accelerator drivers
└── Makefile        # Build system


Prerequisites
Toolchain: Xuantie RISC-V GNU Toolchain (supporting rv64gcv0p7).

Hardware/Sim: C910-based SoC or QEMU instance.

Build Instructions
Set Toolchain Path: Ensure riscv64-unknown-elf-gcc is in your PATH.

Build Firmware:

Bash

make
This generates build/firmware.elf.

Configuration
Memory Map: Edit platform/c910/linker.ld to match your target RAM layout.

Peripherals: UART and Timer addresses are defined in platform/c910/.

Vector Extensions: The C910 uses RVV 0.7.1. Ensure ARCH_FLAGS in Makefile matches your hardware support.


