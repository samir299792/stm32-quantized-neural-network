# STM32 Quantized Neural Network (Interrupt-Driven Embedded ML)

This project implements a **quantized neural network inference engine on an STM32F042 microcontroller** using fixed-point arithmetic and an interrupt-driven architecture.

The system continuously samples two analog inputs, performs **XOR classification using a neural network**, and demonstrates how embedded ML workloads can be optimized for **speed, memory efficiency, and energy consumption** on resource-constrained microcontrollers.

The project also demonstrates several important embedded firmware concepts:

- Fixed-point neural network inference
- Interrupt-driven ADC sampling
- SysTick periodic scheduling
- NVIC interrupt handling
- Hardware timing analysis using GPIO + oscilloscope
- Performance and energy benchmarking

---

## System Architecture

The firmware is structured around **interrupt-driven execution** instead of polling to maximize CPU efficiency.
