# Comprehensive Technical Guide to the AD5940/AD5941 AFE: From Hardware Architecture to Library Implementation

## Part 1: Hardware Architecture and Functional Blocks

This section provides a foundational, bottom-up understanding of the AD5940/AD5941 silicon. It moves beyond simply listing features by explaining the purpose of each block, its operational principles, and how its configuration impacts overall system performance.

### Section 1: Introduction to the AD5940/AD5941 Architecture

#### 1.1 Core Philosophy: A Dual-Loop System for Versatility

The AD5940/AD5941 is a high-precision, low-power Analog Front End (AFE) engineered for a broad spectrum of portable applications that demand high-precision electrochemical and impedance-based measurement techniques.[1, 2, 3] At the heart of its architecture lies a fundamental design choice: the integration of two distinct and highly specialized signal chains. This dual-loop system provides unparalleled versatility, allowing a single chip to address two disparate application domains.[1, 2, 3]

The first loop is an ultra-low-power, low-frequency signal path designed for applications where power consumption is the primary constraint. This "low-bandwidth loop" is optimized for DC-centric or low-frequency measurements (up to 200 Hz), making it ideal for "always-on" wearable devices, continuous monitoring systems, and battery-powered sensors. Its architecture is built around an ultra-low-power potentiostat and transimpedance amplifier (TIA), enabling it to maintain a sensor's bias while consuming mere microamperes of current.[2, 4] This capability is critical for applications like continuous glucose monitoring or long-term electrodermal activity (EDA) tracking, where the device must remain active for extended periods without draining the battery.

The second loop is a high-speed, wide-bandwidth signal path tailored for high-performance, dynamic AC measurements. This "high-bandwidth loop" is capable of generating and measuring signals from 0.015 Hz up to 200 kHz, making it perfectly suited for complex Electrochemical Impedance Spectroscopy (EIS) and other frequency-domain analyses.[2, 4] Applications such as battery state-of-health analysis, material sensing, and detailed body impedance analysis (BIA) rely on this loop's ability to perform frequency sweeps and extract both magnitude and phase information from a sensor's response.[1, 3]

This architectural duality presents a fundamental choice to the design engineer. The development of a power-constrained wearable device will naturally favor the low-power loop and its associated features, such as hibernate mode and the low-power potentiostat channel, which consumes just 6.5 µA.[2] Conversely, the design of a high-performance benchtop instrument or a portable diagnostic tool will leverage the high-speed loop's 200 kHz bandwidth and on-chip Discrete Fourier Transform (DFT) engine. Understanding which loop is primary to a given application is the first and most crucial step in effectively configuring the AD5940/AD5941, as it dictates the relevant set of features, power modes, and operational sequences.

#### 1.2 Functional Block Diagram Deep Dive

A thorough examination of the AD5940/AD5941 functional block diagram provides a system-level map of its capabilities and internal data flow.[2] The architecture can be conceptually divided into several key subsystems:

*   **Excitation Blocks:** This includes the two primary signal generation loops. The **Low Bandwidth AFE Loop** contains the dual-output Low Power DAC and the low-power potentiostat amplifier. The **High Bandwidth AFE Loop** is comprised of the 12-bit High-Speed DAC (HSDAC), its associated waveform generator, and the excitation buffer amplifier. These blocks are responsible for generating the precise DC or AC signals required to stimulate the external sensor.

*   **Measurement Blocks:** This encompasses the two current measurement channels. The low-power loop uses a **Low Power Transimpedance Amplifier (LPTIA)** for sensitive, low-frequency current measurements. The high-speed loop employs a **High Speed TIA (HSTIA)** to handle the wide bandwidth signals associated with impedance spectroscopy.

*   **Analog-to-Digital Conversion (ADC) Core:** At the center of the measurement path is a high-performance **16-bit Successive Approximation Register (SAR) ADC**. This block is preceded by a flexible input multiplexer (MUX), an anti-alias filter (AAF), and a Programmable Gain Amplifier (PGA), allowing it to measure a wide variety of internal and external voltage signals.

*   **Digital Processing and Control:** This digital core includes several hardware accelerators and control units. The **Sequencer** is a programmable command processor that can autonomously control the AFE, minimizing the workload on the host microcontroller. The **DFT Engine** works in conjunction with the ADC to perform complex impedance calculations. A deep **SRAM block (6 kB)** is partitioned to serve as a command FIFO for the sequencer and a data FIFO for measurement results.

*   **System and I/O:** This includes the **SPI Interface** for communication with a host MCU, a **Wake-Up Timer** for periodic measurements, an **Interrupt Controller**, and **General-Purpose Inputs/Outputs (GPIOs)** that can be controlled by the sequencer for synchronizing with external components.

*   **Power and References:** The chip integrates its own Low-Dropout Regulators (LDOs), precision voltage references (1.82 V and 2.5 V), and a comprehensive power management system that includes multiple power modes to optimize performance versus consumption.[2]

These blocks are interconnected via an **ultralow leakage Switch Matrix**, which provides the flexibility to route excitation and measurement signals to various external pins, enabling a wide range of sensor configurations from a single hardware design.

#### 1.3 Package Options and Pin Descriptions (AD5940 vs. AD5941)

The AD5940/AD5941 is offered in two distinct packages, which primarily differ in physical size and the number of available GPIO pins.[5, 2]

*   **AD5940:** Packaged in a $3.6~mm \times 4.2$ mm, 56-ball Wafer Level Chip Scale Package (WLCSP). This smaller package is optimized for space-constrained applications like wearables. It provides access to a full set of eight GPIO pins (GPIO0-GPIO7).
*   **AD5941:** Packaged in a larger $7~mm \times 7$ mm, 48-lead Lead Frame Chip Scale Package (LFCSP). This package is easier to handle in manufacturing and prototyping. It provides access to three GPIO pins (GPIO0, GPIO1, GPIO2). The AD5941W variant is AEC-Q100 qualified for automotive applications and supports an extended temperature range up to +105°C.[2]

Aside from the GPIO count and package type, the core functionality of the AD5940 and AD5941 is identical.[5] The following table details the key analog and digital pins common to both devices.[2]

| Mnemonic | Pin Type | Description |
| :---------------- | :------- | :--------------------------------------------------------------------------------------------------------------------------------------- |
| `CEO` | Analog | **Counter Electrode Output:** The output of the potentiostat amplifier, used to source or sink current in a three-electrode system. |
| `REO` | Analog | **Reference Electrode Input:** The inverting input to the potentiostat amplifier, used to sense the reference potential of the sensor. |
| `SEO` | Analog | **Sense Electrode Input:** The primary input for the TIA measurement channels. Also known as the working electrode input. |
| `DEO` | Analog | **Differential Electrode Input/Output:** A versatile pin connected to the high-speed TIA, with its own programmable `RLOAD`/`RTIA` options. |
| `RCAL0`, `RCAL1` | Analog | **Calibration Resistor Terminals:** Pins for connecting an external precision resistor (`RCAL`) used in ratiometric impedance measurements. |
| `AIN0` - `AIN6` | Analog | **Analog Input Pins:** Uncommitted voltage input channels that can be measured by the ADC. AIN6 is available on the AD5940 only. |
| `AFE1` - `AFE4` | Analog | **Uncommitted Analog Front End Pins:** Versatile pins that can be connected to the switch matrix for various routing purposes. |
| `VBIASO`, `VZEROO`| Analog | **Low Power DAC Outputs:** Decoupling capacitor pins for the 12-bit bias DAC and 6-bit zero DAC outputs, respectively. |
| `VREF_1V82` | Analog | **1.82 V Reference:** Decoupling capacitor pin for the high-precision 1.82 V reference used by the ADC and HSDAC. |
| `VREF_2V5` | Analog | **2.5 V Reference:** Decoupling capacitor pin for the 2.5 V reference used by the low-power loop. |
| `VBIAS_CAP` | Analog | **ADC Bias Voltage:** Decoupling capacitor pin for the internal 1.11 V bias voltage for the ADC's pseudo-differential inputs. |
| `AVDD`, `DVDD` | Power | Analog and Digital power supply inputs (2.8 V to 3.6 V). |
| `AGND`, `DGND` | Power | Analog and Digital ground connections. |
| `IOVDD` | Power | Digital I/O supply pin, which can be 1.8 V or 2.8 V to 3.6 V. |
| `CS`, `SCLK`, `MOSI`, `MISO` | Digital | Standard 4-wire Serial Peripheral Interface (SPI) pins for communication with the host MCU. |
| `RESET` | Digital | Active-low system reset pin. |
| `GPIO0` - `GPIO7` | Digital | General-Purpose Input/Output pins. GPIO3-GPIO7 are available on the AD5940 only. |

### Section 2: Excitation Signal Generation

#### 2.1 The Low-Power Loop: Precision DC Bias and Low-Frequency Waveforms

The low-power excitation path is the cornerstone of the AD5940/AD5941's suitability for battery-powered and continuous monitoring applications. It is engineered to provide stable, precise DC biasing and low-frequency AC signals while consuming minimal power. This loop is comprised of a unique dual-output DAC and a low-noise potentiostat amplifier.[2]

##### The Dual-Output Low Power DAC

The Low Power DAC is a string DAC architecture that provides two separate, programmable voltage outputs from a single core, referenced to the internal 2.5 V source.[2] This dual-output design is specifically tailored for potentiostatic applications.

*   **12-bit $V_{BIASO}$ Output:** This output provides a high-resolution voltage source (LSB size of approximately 537 µV) primarily used to set the noninverting input of the low-power potentiostat amplifier. It defines the potential that the potentiostat will maintain on the reference electrode (`REO`) of a sensor. Its output voltage range is 0.2 V to 2.4 V.[2]
*   **6-bit $V_{ZEROO}$ Output:** This output provides a lower-resolution voltage source (LSB size of approximately 34.4 mV) used to set the noninverting input of the Low Power TIA. This effectively sets the desired potential of the sense (or working) electrode (`SEO`). Its output range is 0.2 V to 2.366 V.[2]

The effective bias voltage across the sensor's reference and sense electrodes is the difference between these two DAC outputs ($V_{BIASO} - V_{ZEROO}$). This architecture allows for precise control over the sensor's operating point. For zero-bias sensors, both outputs are set to the same potential. For sensors requiring a specific bias, the difference can be programmed accordingly.[6]

##### The Low-Power Potentiostat

The potentiostat is a low-power, low-noise amplifier that forms a control loop to maintain a constant potential between a reference electrode and a sense electrode. In a standard three-electrode electrochemical cell, the `VBIASO` DAC output is connected to the potentiostat's noninverting input, and the `REO` pin is connected to its inverting input. The potentiostat's output drives the `CEO` pin. This configuration forces the voltage at `REO` to equal the voltage set by `VBIASO` by sourcing or sinking the necessary current through the `CEO` pin to the electrochemical cell.[2, 6] This control loop is essential for stable amperometric, voltammetric, and other electrochemical measurements.

##### Key Control Registers

The Low Power DAC is primarily configured through three registers.[2]

*   **`LPDACDAT0` (Address: `0x2120`):** This register contains the data codes for both DAC outputs. Bits [11:0] set the 12-bit `VBIASO` output, and bits set the 6-bit `VZEROO` output.
*   **`LPDACCON0` (Address: `0x2128`):** This is the main control register. Key bits include `PWDEN` (Bit 1) to power on the DAC, `RSTEN` (Bit 0) to enable writes to the data register, and `DACMDE` (Bit 5) to configure the output switches for normal or diagnostic modes.
*   **`LPDACSW0` (Address: `0x2124`):** This register allows for individual manual control over the switches at the DAC outputs, overriding the automatic modes set by `DACMDE`. This is useful for advanced configurations or diagnostic testing.

#### 2.2 The High-Speed Loop: Wideband AC Signal Generation

The high-speed excitation loop is the performance engine of the AD5940/AD5941, designed to generate clean, high-frequency AC signals for impedance spectroscopy and other dynamic measurement techniques. It consists of a 12-bit HSDAC, a powerful excitation amplifier, and can be driven by the on-chip digital waveform generator.[2]

##### The 12-bit High-Speed DAC (HSDAC)

The HSDAC is a 12-bit DAC core capable of generating waveforms at frequencies up to 200 kHz. It produces a differential output voltage with a full-scale range of approximately $\pm300$ mV before any gain stages. This differential signal is then passed to the excitation amplifier. The output code is set by writing to the `HSDACDAT` register, where `0x800` represents zero output, `0x200` is negative full-scale, and `0xE00` is positive full-scale.[2]

##### The Excitation Amplifier and AC Coupling

A key architectural feature is the excitation amplifier, which takes the differential AC signal from the HSDAC and conditions it for driving the sensor. Crucially, this amplifier allows the AC signal to be superimposed on a DC bias level. This is achieved by connecting the DC bias voltages, typically from the Low Power DAC (`VBIASO` and `VZEROO`), to the P (positive) and N (negative) inputs of the excitation amplifier. This enables complex measurements like EIS to be performed on an electrochemical cell that must be held at a specific DC operating potential, representing a powerful synergy between the two otherwise separate excitation loops.[2] This hybrid configuration, where components from both the low-power and high-speed loops are used simultaneously, is essential for unlocking some of the chip's most advanced capabilities. A clear, step-by-step guide is necessary to configure this mode, as it is non-obvious from a cursory review of the datasheet.

##### Power vs. Performance Modes

The high-speed loop can operate in two distinct power modes, allowing a trade-off between maximum bandwidth and current consumption. This is controlled by a single bit, `SYSHS` (Bit 0), in the `PMBW` register (Address `0x22F0`).[2]

*   **Low Power Mode (`SYSHS` = 0):** This is the default mode, intended for impedance measurements at frequencies below 80 kHz. In this mode, the analog clock (`ACLK`) for the high-speed blocks runs at 16 MHz.
*   **High Power Mode (`SYSHS` = 1):** This mode increases the bandwidth of the amplifiers to support excitation frequencies up to 200 kHz. It increases power consumption and requires the `ACLK` to be set to 32 MHz.

##### Key Control Registers

The HSDAC is configured using two primary registers.[2]

*   **`HSDACCON` (Address: `0x2010`):** This register controls the HSDAC's configuration. Key bits include `Rate` [8:1] to set the DAC update clock divider, `INAMPGNMDE` (Bit 12) to set the excitation amplifier gain (2 or 0.25), and `ATTENEN` (Bit 0) to enable a PGA stage attenuator (gain of 1 or 0.2).
*   **`HSDACDAT` (Address: `0x2048`):** This 12-bit register holds the raw data code to be output by the DAC when not using the waveform generator.

#### 2.3 The Digital Waveform Generator

The on-chip digital waveform generator provides an automated way to produce standard waveforms without requiring the host MCU to continuously write DAC codes. This is essential for generating the precise, repeatable signals needed for frequency sweeps and voltammetry.

##### Configuration and Operation

The generator is enabled by setting the `WAVEGENEN` bit (Bit 14) in the main `AFECON` register. The type of waveform is then selected using the `TYPESEL` bits [2:1] in the `WGCON` register (`0b10` for Sinusoid, `0b11` for Trapezoid).[2] Once enabled, the generator continuously outputs the configured waveform to the selected DAC (either HSDAC or LPDAC) until it is disabled.

##### Sinusoid Generation

For impedance spectroscopy, the sinusoid generator is used. It is configured via a set of dedicated registers [7, 2]:

*   **`WGFCW` (Address: `0x2030`):** The Frequency Control Word. The output frequency is calculated as $f_{OUT} = f_{ACLK} \times (\text{SINEFCW} / 2^{30})$.
*   **`WGAMPLITUDE` (Address: `0x203C`):** Sets the peak amplitude of the sine wave.
*   **`WGPHASE` (Address: `0x2034`):** Sets a programmable phase offset for the sine wave.
*   **`WGOFFSET` (Address: `0x2038`):** Adds a DC offset to the sine wave output.

##### Trapezoid/Square Wave Generation

For voltammetry applications, the trapezoid generator is used to create linear voltage ramps. A square wave can be generated by setting the slope times to zero. The waveform is defined by six parameters, each with a dedicated register [2]:

*   `WGDCLEVEL1`, `WGDCLEVEL2`: The two DC voltage levels of the trapezoid.
*   `WGDELAY1`, `WGDELAY2`: The duration the waveform holds at each DC level.
*   `WGSLOPE1`, `WGSLOPE2`: The time taken to ramp between the two DC levels.

### Section 3: The Measurement and Conversion Path

#### 3.1 Low-Power Transimpedance Amplifier (LPTIA)

The LPTIA is designed for high-sensitivity, low-bandwidth current measurements, making it the primary choice for amperometry and other DC or low-frequency electrochemical techniques. It converts the small current from the sensor's sense electrode (`SEO`) into a measurable voltage.[2] The amplifier features several programmable elements to adapt to a wide range of sensor outputs, all configured via the `LPTIACON0` register (Address `0x20EC`).[2]

*   **Programmable Gain Resistor (`RTIA`):** The `TIAGAIN` bits [9:5] select an internal gain resistor ranging from 200 $\Omega$ to 512 k$\Omega$. This allows the user to scale the output voltage to match the expected current range of the sensor, maximizing the dynamic range of the subsequent ADC measurement.
*   **Programmable Load Resistor (`RLOAD`):** The `TIARL` bits select an internal load resistor. This is particularly useful for biasing certain types of sensors.
*   **Protection Diodes:** The LPTIA includes back-to-back protection diodes that can be switched in parallel with the `RTIA` resistor. These diodes prevent the amplifier output from saturating during large current transients, such as when switching gain ranges, though they can introduce leakage current if the voltage across them becomes significant.[2]
*   **Force/Sense Connections:** To maintain accuracy with high gain (large `RTIA`) values, the feedback path uses force and sense connections. This technique mitigates voltage drops across the internal switches used for selecting the `RTIA` value, ensuring the full gain is applied.[2]

#### 3.2 High-Speed Transimpedance Amplifier (HSTIA)

The HSTIA is the counterpart to the LPTIA, designed for measuring wide-bandwidth AC signals up to 200 kHz. It is the essential measurement block for impedance spectroscopy.[2] Its configuration provides flexibility in signal routing and gain selection.

*   **Input Signal Selection:** The HSTIA's inverting input can be connected to various pins via the programmable switch matrix, most commonly the `SEO` pin or the `DEO` pin. The `DEO` pin has its own dedicated set of programmable `RLOAD` and `RTIA` resistors, configured via the `DEORESCON` register, offering an alternative measurement path.[2]
*   **Gain Resistor Selection (`RTIA`):** The main `RTIA` bank, used for inputs other than `DEO`, is configured via the `RTIACON` bits [3:0] in the `HSRTIACON` register, with values from 200 $\Omega$ to 160 k$\Omega$.[2]
*   **Common-Mode Voltage:** The noninverting input of the HSTIA sets its output common-mode voltage. This can be set to the internal 1.11 V reference (`VBIAS_CAP`) or, crucially, to the `VZEROO` output from the low-power DAC. This latter option is used during EIS on a biased sensor to maintain the correct DC potential on the sense electrode.[2] This is configured via the `VBIASSEL` bits in the `HSTIACON` register.

#### 3.3 The 16-bit SAR ADC

The ADC is the final stage of the analog measurement path, responsible for converting the conditioned voltage signals into digital data for processing.[2]

##### Architecture and Transfer Function

The ADC core is a 16-bit, charge-redistribution SAR that can sample at up to 800 kSPS in low-power mode or 1.6 MSPS in high-power mode. It operates in a pseudo-differential mode, where the positive input is the selected measurement channel and the negative input is typically tied to a stable internal bias voltage of 1.11 V (from the `VBIAS_CAP` pin). The ideal transfer function for converting the raw 16-bit unsigned ADC code (`ADCDAT`) to an input voltage ($V_{IN}$) is given by [2]:

$V_{IN} = \frac{V_{REF}}{PGA\_G} \times (\frac{ADCDAT - 0x8000}{2^{15}}) + V_{BIAS\_CAP}$

For the factory-calibrated PGA gain of 1.5, a correction factor is used:

$V_{IN} = \frac{1.835 V}{1.5} \times (\frac{ADCDAT - 0x8000}{2^{15}}) + V_{BIAS\_CAP}$

where $V_{REF}$ is the 1.82 V reference and $PGA\_G$ is the selected gain.

##### Input Multiplexer (MUX)

The ADC's flexibility comes from its extensive input multiplexer, which is configured via the `ADCCON` register (Address `0x21A8`). This MUX allows the ADC to measure a wide array of signals.[8, 2]

*   **Positive Input (`MUXSELP`, Bits [5:0]):** Can be connected to external voltage pins (`AIN0`-`AIN6`, `CEO`, `REO`, `SEO`, `DEO`), the outputs of the TIAs (`LPTIA_P`, HSTIA positive output), or internal diagnostic nodes (supply voltages, temperature sensor, reference voltages).
*   **Negative Input (`MUXSELN`, Bits [12:8]):** Can be connected to external pins, the negative nodes of the TIAs, or, most commonly, the stable `VBIAS_CAP` pin (`0b01000`).

The MUX selection is one of the most frequent and critical configuration steps. Consolidating the scattered information from the datasheet into an application-focused table drastically reduces developer effort and prevents common errors.

##### Programmable Gain Amplifier (PGA)

Located before the ADC core, the PGA allows the input signal to be amplified to better utilize the ADC's full dynamic range. The gain is set via the `GNPGA` bits in the `ADCCON` register, with available gains of 1, 1.5, 2, 4, and 9.[2]

#### 3.4 ADC Post-Processing Pipeline

After conversion, the raw ADC data can be passed through a configurable digital post-processing pipeline to filter noise, reduce data rate, or perform complex calculations in hardware.[2]

##### Sinc3 and Sinc2 Filters

These are digital low-pass filters that operate by averaging (decimating) the high-speed ADC output. They are configured via the `ADCFILTERCON` register. The `SINC3OSR` bits set the oversampling rate for the Sinc3 filter (typically 2, 4, or 5), and the `SINC2OSR` bits set the rate for the Sinc2 filter. Using these filters significantly improves the signal-to-noise ratio (SNR) at the cost of a lower output data rate.[2]

##### 50/60 Hz Notch Filter

For very low-frequency DC measurements susceptible to mains power line noise, a dedicated notch filter can be enabled by setting `SINC2EN` (Bit 16) in the `AFECON` register. This filter is implemented as part of the Sinc2 filter stage and provides significant rejection at 50 Hz and 60 Hz.[2]

##### The DFT Engine

The Discrete Fourier Transform (DFT) engine is a hardware accelerator that is fundamental to impedance measurement. When enabled via the `DFTEN` bit in `AFECON`, it processes a block of ADC samples and calculates the real and imaginary components of the signal at a single frequency—the frequency of the excitation waveform. This offloads the host MCU from performing computationally intensive FFTs or other transform algorithms. The number of ADC samples used for the calculation (from 4 to 16,384) is set by the `DFTNUM` bits in the `DFTCON` register.[2]

#### Table: ADC Input MUX Configuration for Common Measurements

| Measurement Type | Positive Input (`MUXSELP` Value) | Negative Input (`MUXSELN` Value) | `ADCCON` Register Setting (P/N) |
| :------------------------------------------------- | :------------------------------- | :------------------------------- | :------------------------------ |
| Differential Voltage (AIN2 vs. AIN3) | `0b000111` (AIN3) | `0b000110` (AIN2) | `0x0607` |
| Low-Power DC Current (LPTIA with LPF) | `0b100001` (LPTIA_P) | `0b000010` (LPTIA_N) | `0x0221` |
| High-Speed AC Current (HSTIA) | `0b000001` (HSTIA Positive) | `0b000001` (HSTIA Negative) | `0x0101` |
| Internal Temperature Sensor | `0b01011` (Temp Sensor) | `0b01011` (Temp Sensor Negative) | `0x0B0B` |
| Voltage on `REO` vs. ADC Bias | `0b011010` (RE0) | `0b01000` (VBIAS_CAP) | `0x081A` |
| Excitation Voltage (P-Node of Excitation Amp) | `0b100100` (Excitation P-Node) | `0b01000` (VBIAS_CAP) | `0x0824` |

### Section 4: System Control and Management

#### 4.1 The Programmable Switch Matrix

The switch matrix is the physical implementation of the AD5940/AD5941's flexibility. It is a complex web of ultralow-leakage analog switches that allows the user to programmatically connect the AFE's internal functional blocks to its external pins. A misunderstanding of this matrix will prevent any measurement from being performed correctly. The matrix is primarily used to configure the high-speed loop for different impedance measurement topologies (e.g., 2-wire, 3-wire, 4-wire) and for connecting a calibration resistor (`RCAL`) into the measurement path.[2]

##### Switch Groups and Function

The matrix is organized into four main groups of switches, each connecting a specific internal node to a set of external pins [2]:

*   **Dx/DR0 Switches:** Connect the output of the high-speed **D**AC excitation amplifier to pins like `CEO`, `AIN1-3`, `SEO`, and `RCAL0` (via DR0).
*   **Px/Pxx Switches:** Connect the noninverting (**P**ositive) input of the excitation amplifier to pins like `REO`, `CEO`, `SEO`, `DEO`, and `RCAL0` (via PR0).
*   **Nx/Nxx Switches:** Connect the inverting (**N**egative) input of the excitation amplifier to pins like `AIN0-3`, `SEO`, and `RCAL1` (via NR1).
*   **Tx/TR1 Switches:** Connect the inverting input of the High-Speed **T**IA to pins like `AIN0-3`, `SEO`, `DEO`, and `RCAL1` (via TR1).

##### Control Methods

There are two distinct methods for controlling these switches, selected by the `SWSOURCESEL` bit (Bit 16) in the `SWCON` register.[2]

1.  **Grouped Control (`SWSOURCESEL` = 0):** This is a simplified method where a 4-bit field in the `SWCON` register (`DMUXCON`, `PMUXCON`, `NMUXCON`, `TMUXCON`) selects one pin for each group to be connected, opening all other switches in that group. This is convenient for simple, fixed configurations.
2.  **Individual Control (`SWSOURCESEL` = 1):** This method provides full flexibility, allowing any combination of switches to be opened or closed simultaneously. Each switch group has a corresponding `xSWFULLCON` register (e.g., `DSWFULLCON`, `PSWFULLCON`) where each bit maps directly to an individual switch. This method is required for more complex setups, such as 4-wire measurements.

To avoid configuration errors, it is highly recommended to create application-specific "wiring diagrams" that clearly show which switches must be closed for a given measurement (e.g., 4-wire BIA), and then provide the exact register values needed to achieve that state. This approach directly bridges the gap between the application requirement and the low-level hardware configuration.

#### 4.2 The AFE Sequencer

The sequencer acts as a simple, programmable co-processor, or "brain," for the AFE. Its primary purpose is to execute a pre-defined list of commands to control the analog blocks with cycle-accurate timing. This offloads the host MCU from low-level, timing-critical tasks, which is especially important in systems where the host may be running a non-real-time operating system.[5, 9, 2]

##### Sequencer Architecture

The sequencer fetches and executes commands sequentially from a dedicated portion of the on-chip 6 kB SRAM known as the Command FIFO. It can perform register writes and timed waits, but it cannot read registers or make conditional branches. Its operation is deterministic and tied directly to the AFE's system clock, ensuring precise timing between operations like starting a waveform, waiting for it to settle, and triggering an ADC conversion.[2]

##### Command Formats

The sequencer understands two 32-bit command formats [2]:

*   **Write Command (MSB = 1):** ``
    This command writes the 24-bit data value to the AFE register specified by the 7-bit address. The address field corresponds to bits [8:2] of the full register address.
*   **Timer Command (MSBs = 00 or 01):** `` for a `Wait` command, or `` for a `Timeout` command.
    A `Wait` command pauses the sequencer for the specified number of `ACLK` cycles. A `Timeout` command starts a background timer that can trigger an interrupt, useful for detecting if a sequence has stalled.

##### Memory Management and FIFO

The 6 kB SRAM is flexibly partitioned between the Command FIFO (for storing sequences) and the Data FIFO (for storing measurement results). The size of each partition is configured using the `CMD_MEM_SEL` and `DATA_MEM_SEL` bits in the `CMDDATACON` register. The Data FIFO source (e.g., ADC data, DFT result) is selected via the `DATAFIFOSRCSEL` bits in the `FIFOCON` register.[2]

#### 4.3 System Clocks, Power Modes, and Resets

##### Clocking Architecture

The AD5940/AD5941 provides several clocking options to balance performance and power consumption [2]:

*   **High-Frequency Oscillator (HFOSC):** An internal oscillator, configurable for 16 MHz (for low-power mode) or 32 MHz (for high-power mode). This is the primary system clock (`ACLK`).
*   **Low-Frequency Oscillator (LFOSC):** An internal 32.768 kHz oscillator used exclusively for the Wake-Up Timer.
*   **External Crystal/Clock:** An external 16 MHz or 32 MHz crystal or clock source can be used for higher precision timing.

##### Power Mode Management

The AFE supports several power modes to drastically reduce power consumption when not actively measuring [2]:

*   **Active Mode (Low/High Power):** All necessary blocks are powered on for measurement. High Power mode (>80 kHz) consumes more current but enables the full 200 kHz bandwidth.
*   **Hibernate Mode:** A very low-power state where most of the chip is powered down. Critically, the low-power DAC, potentiostat, and 32 kHz oscillator can remain active to maintain a sensor's DC bias, consuming as little as 6.5 µA.[2] The device can be woken from this state by the Wake-Up Timer.
*   **Shutdown Mode:** The lowest power state, where all blocks, including SRAM, are powered down.

##### System Resets

The chip can be reset in two ways [2]:

*   **Power-On Reset (POR):** An internal circuit that automatically resets the chip when the `DVDD` supply voltage rises above its trip level (typically 1.62 V).
*   **External Reset:** A dedicated `RESET` pin, which, when driven low for at least 1 µs, will trigger a full system reset.

## Part 2: Library Usage and Measurement Procedures

This part provides a top-down, practical guide focused on getting the AFE operational quickly using the official `ad5940lib` and then applying it to common measurement scenarios.

### Section 5: Getting Started with the `ad5940lib`

#### 5.1 Library Architecture and Philosophy

The `ad5940lib` is a C-language firmware library provided by Analog Devices to abstract the complexity of the AD5940/AD5941. With over 180 registers, direct register-level programming can be intricate and error-prone. The library's core philosophy is to offer a "register-less" approach, providing higher-level functions and data structures that configure entire functional blocks (like the ADC filter chain or a waveform generator) with a single API call.[5]

The library also handles critical, non-obvious initializations by default. Upon startup, it automatically performs tasks such as disabling the internal watchdog timer, gating unused clocks to save power, enabling SRAM data retention during hibernate mode, and setting a crucial undocumented register (`0x0C08`) required for the fast FIFO read command to function correctly. These built-in initializations save developers from common pitfalls that can be difficult to debug.[5]

#### 5.2 Platform Porting Guide

The `ad5940lib` is designed to be portable across different host microcontrollers. To adapt the library to a new hardware platform (e.g., moving from the official ADuCM3029 to an ESP32 or STM32), the developer must implement a small set of low-level hardware abstraction functions. These functions form the bridge between the library's logic and the specific MCU's peripherals.[5, 10]

The mandatory functions to be implemented are:
*   `void AD5940_CsClr(void);`: Drives the SPI Chip Select (CS) pin low.
*   `void AD5940_CsSet(void);`: Drives the SPI Chip Select (CS) pin high.
*   `void AD5940_RstClr(void);`: Drives the AFE's RESET pin low.
*   `void AD5940_RstSet(void);`: Drives the AFE's RESET pin high.
*   `void AD5940_Delay10us(uint32_t time);`: A blocking delay function, where `time` is in units of 10 microseconds.
*   `void AD5940_ReadWriteNBytes(unsigned char *pSendBuffer, unsigned char *pRecvBuff, unsigned long length);`: The core SPI transaction function that simultaneously writes data from `pSendBuffer` and reads data into `pRecvBuff` for a given `length` in bytes.

In addition to these functions, an interrupt service routine (ISR) must be set up on the host MCU, connected to one of the AD5940's GPIO pins (typically GPIO0), to handle interrupts from the AFE.

#### 5.3 Board Setup and Project Configuration

For initial development and evaluation, the official Analog Devices evaluation boards are recommended. The setup typically consists of two boards [6, 11, 12]:

1.  **EVAL-ADICUP3029:** The motherboard containing the ADuCM3029 Arm Cortex-M3 microcontroller, which acts as the host MCU.
2.  **EVAL-AD5940BIOZ or EVAL-AD5940ELCZ:** An Arduino-form-factor shield that plugs into the EVAL-ADICUP3029. The BIOZ board is tailored for bio-impedance applications (EDA, BIA, ECG), while the ELCZ board is designed for general electrochemical measurements (gas sensing, liquid analysis).

To compile and run the example projects, a specific toolchain is required. The official examples are verified with the Keil MDK and IAR Embedded Workbench IDEs. For Keil, it is critical to install ARM Compiler Version 5, as it may not be included by default in newer Keil installations.[13] Both IDEs require the installation of the ADuCM3029 Device Support Pack (CMSIS-Pack) to correctly build the projects.

### Section 6: Core Operational Workflow

#### 6.1 System Initialization: The Critical First Step

Regardless of whether the `ad5940lib` is used, a specific sequence of register writes must be performed immediately after every power-on or hardware reset. This sequence configures undocumented or critical internal settings required for the chip to operate reliably. Failure to perform this initialization will result in non-functional or unpredictable behavior.[14, 2]

##### Table: Mandatory Initialization Sequence

| Register Address | Data to Write |
| :--------------- | :------------ |
| `0x0908` | `0x02C9` |
| `0x0C08` | `0x206C` |
| `0x21F0` | `0x0010` |
| `0x0410` | `0x02C9` |
| `0x0A28` | `0x0009` |
| `0x238C` | `0x0104` |
| `0x0A04` | `0x4859` |
| `0x0A04` | `0xF27B` |
| `0x0A00` | `0x8009` |
| `0x22F0` | `0x0000` |

This sequence is of paramount importance as it addresses silicon-level configurations not fully detailed in the main body of the datasheet. For instance, the write to address `0x0C08` is essential for enabling the fast FIFO read command (`SPICMD_READFIFO`).[5, 14] Providing this sequence explicitly is a high-value action that prevents significant debugging challenges.

#### 6.2 SPI Communication Protocol in Practice

All communication with the AD5940/AD5941 is conducted via a standard 4-wire SPI interface, with the host MCU acting as the initiator. The SPI clock (`SCLK`) must not exceed 16 MHz.[2]

##### Register Access

Accessing a register is a two-transaction process [2]:
1.  **Set Address:** The host initiates an SPI transaction, sends the command byte `0x20` (`SPICMD_SETADDR`), followed by the 16-bit address of the target register.
2.  **Read/Write Data:** The host initiates a second transaction.
    *   To **write**, it sends the command byte `0x2D` (`SPICMD_WRITEREG`), followed by the 16-bit or 32-bit data to be written.
    *   To **read**, it sends the command byte `0x6D` (`SPICMD_READREG`), sends dummy bytes to clock the bus, and reads the 16-bit or 32-bit data returned on the `MISO` line.

##### Optimized FIFO Read Protocol

While the Data FIFO can be read by accessing the `DATAFIFORD` register using the standard read protocol, this is inefficient for reading large blocks of data. A faster, dedicated protocol uses the `0x5F` (`SPICMD_READFIFO`) command. This protocol has a specific timing requirement due to internal bus latency.[2] After sending the command byte, the host must transmit **six dummy bytes** while keeping CS low. After the sixth dummy byte, valid data from the FIFO will begin to appear on the `MISO` line. The host can then continue clocking the SPI bus to stream data out of the FIFO continuously. Special care must be taken to avoid a FIFO underflow error when reading the final two data words in the FIFO.[2]

#### 6.3 Data Retrieval and Interpretation: Decoding the FIFO

Measurement results are stored in the Data FIFO as 32-bit words. Each word is a packed structure containing the data itself along with metadata and an error-correction code.[2]

##### Data FIFO Word Format

The 32-bit word is structured as follows [2]:
``

*   **ECC (Error-Correction Code):** A 7-bit code calculated from the lower 25 bits. It allows the host to verify the integrity of the read data. The algorithm is a parity check across specific bit groups.[2]
*   **Sequence ID:** Identifies which of the four possible sequences (SEQ0-SEQ3) generated this data point.
*   **Channel ID:** Identifies the source of the data within the AFE.
*   **Data:** The 16-bit measurement result. For DFT results, this field is expanded to 18 bits (using two additional bits from the Channel ID field), and the data is in two's complement format.[2]

##### Table: Channel ID Mapping

| Channel ID (Bits) | Data Source |
| :------------------------ | :--------------------------------------------------------------------------- |
| `0b11111xx` | DFT Result |
| `0b11110xx` | Mean value from the statistics block |
| `0b11101xx` | Variance value from the statistics block |
| `0b1xxxxxx` | Sinc2 filter result. The lower 6 bits reflect the `MUXSELP` setting. |
| `0b0xxxxxx` | Sinc3 filter result (or raw ADC data). The lower 6 bits reflect the `MUXSELP` setting. |

#### 6.4 Calibration Procedures in Software

To achieve the highest precision, the AD5940/AD5941's signal paths should be calibrated to remove offset and gain errors. The chip provides dedicated registers for storing calibration coefficients for each major path.[2]

##### HSDAC Calibration

The HSDAC can be calibrated using an external precision resistor (`RCAL`).[2] The general procedure is:
1.  **Offset Calibration:** Configure the switch matrix to connect the HSDAC output across `RCAL`. Set the HSDAC to its mid-scale code (`0x800`). Measure the differential voltage across `RCAL` using the ADC. Adjust the appropriate offset register (`DACOFFSET` or `DACOFFSETATTEN`) until the measured voltage is as close to zero as possible.
2.  **Gain Calibration:** Set the HSDAC to its positive or negative full-scale code (e.g., `0xE00`). Measure the voltage across `RCAL`. Calculate the gain error based on the expected voltage versus the measured voltage. Adjust the `DACGAIN` register to correct for this error.

##### ADC Channel Calibration

Each ADC path (e.g., LPTIA input, HSTIA input, voltage inputs at different PGA gains) has its own pair of offset and gain calibration registers (e.g., `ADCOFFSETLPTIA`, `ADCGNLPTIA`).[2] The procedure is analogous to DAC calibration:
1.  **Offset Calibration:** Configure the ADC channel and apply a zero-volt or known low-voltage input. Read the ADC result and adjust the corresponding `ADCOFFSETxx` register until the output code reflects the zero input.
2.  **Gain Calibration:** Apply a known full-scale input voltage or current. Read the ADC result and adjust the corresponding `ADCGAINxx` register until the output code accurately represents the full-scale input.

### Section 7: Application Measurement Examples

The true power of the AD5940/AD5941 is realized by combining its various blocks to perform complex measurements. The `ad5940-examples` repository provides the most valuable resource for practical implementation by demonstrating the core logic for these applications.[13]

#### 7.1 Amperometric Measurement (Potentiostat)

This is a fundamental electrochemical measurement used in applications like gas sensing. It involves applying a constant bias voltage and measuring the resulting current.[6]

*   **Hardware Setup:** A three-electrode sensor is connected with its counter, reference, and sense electrodes wired to the `CEO`, `REO`, and `SEO` pins, respectively.
*   **Software Configuration (Low-Power Loop):**
    1.  **Initialize AFE:** Perform the mandatory system initialization.
    2.  **Power Up Blocks:** Enable the low-power reference, LPDAC, potentiostat, and LPTIA.
    3.  **Set Bias Voltage:** Write the desired codes to the `VBIASO` and `VZEROO` fields in the `LPDACDAT0` register. For a zero-bias sensor, these are set to produce the same voltage (e.g., 1.1 V).[6]
    4.  **Configure LPTIA:** Select an appropriate `RTIA` gain resistor in the `LPTIACON0` register based on the expected sensor current.
    5.  **Configure ADC:** Set the ADC MUX (`ADCCON`) to measure the output of the LPTIA (e.g., `MUXSELP` = `LPTIA_P`, `MUXSELN` = `LPTIA_N`).
    6.  **Start Conversion:** Enable the ADC to begin continuous conversions.
    7.  **Read Data:** Read the current measurement results from the Data FIFO.

#### 7.2 Body Impedance Analysis (BIA) with Frequency Sweep

This example demonstrates a key bio-impedance application, using the high-speed loop to measure impedance across a range of frequencies.[13, 2, 15] A 4-wire (Kelvin) measurement is used for highest accuracy.

*   **Hardware Setup:** Four electrodes are used: two for current injection (`F+`, `F-`) and two for voltage sensing (`S+`, `S-`). An external precision resistor `RCAL` is also required.
*   **Switch Matrix Configuration:** The switch matrix must be configured to route the signals for a 4-wire measurement. This is a complex configuration requiring individual switch control. For example: `F+` to `CEO` (D-node), `S+` to `REO` (P-node), `S-` to `AINx` (N-node), and `F-` to the HSTIA input (T-node).
*   **Sequencer Program:** An autonomous frequency sweep is best implemented with the sequencer. A typical sequence for a single frequency point would be:
    1.  **Write `WGFCW`:** Set the desired sinusoid frequency.
    2.  **Write `SWCON`:** Configure the switch matrix to connect the excitation signal across the unknown impedance ($Z_{UNKNOWN}$).
    3.  **Wait:** Delay for the signal to settle.
    4.  **Write `AFECON`:** Trigger the ADC and DFT engine to measure $Z_{UNKNOWN}$.
    5.  **Wait:** Delay for the DFT to complete.
    6.  **Write `SWCON`:** Reconfigure the switch matrix to connect the excitation signal across `RCAL`.
    7.  **Wait:** Delay for settling.
    8.  **Write `AFECON`:** Trigger the ADC and DFT engine to measure `RCAL`.
    9.  **End/Sleep:** The sequence ends, and the host MCU is interrupted to retrieve the two DFT results.
*   **Data Analysis:** The host MCU reads the two complex results (Real/Imaginary pairs) from the FIFO. The unknown impedance is calculated using a ratiometric formula: $Z_{UNKNOWN} = \frac{\text{DFT Result}_{UNKNOWN}}{\text{DFT Result}_{CAL}} \times R_{CAL}$. This process is repeated for each frequency in the sweep.

#### 7.3 Using the Sequencer for Autonomous Low-Power Sensing

This example showcases the AFE's full potential for ultra-low-power, long-term monitoring, ideal for wearables.

*   **Concept:** The AFE is programmed to spend most of its time in hibernate mode. The on-chip Wake-Up Timer periodically wakes the device to execute a short, pre-programmed measurement sequence, after which the device automatically returns to hibernate mode.
*   **Configuration [2]:**
    1.  **Load Sequence:** A measurement sequence (e.g., a simple amperometric reading) is loaded into the Command FIFO (SRAM).
    2.  **Configure Wake-Up Timer:** The sleep duration is written to the `SEQxSLEEPx` registers, and the wake duration (time allowed for the sequence to run) is written to `SEQxWUPx`.
    3.  **Configure Sequence Order:** The `SEQORDER` register is programmed to define which sequence(s) to run upon wakeup.
    4.  **Enable Sequencer Sleep:** The `SEQTRGSLP` register is enabled, allowing the last command in a sequence to put the chip into hibernate mode.
    5.  **Start Timer:** The Wake-Up Timer is enabled. The device will now operate autonomously, waking, measuring, and sleeping according to the programmed schedule, only interrupting the host MCU when the Data FIFO reaches a certain threshold.

### Conclusions

The Analog Devices AD5940/AD5941 is a highly integrated and exceptionally versatile Analog Front End, uniquely architected to serve two distinct but complementary domains: ultra-low-power sensing and high-performance impedance analysis. Its dual-loop design, comprising a low-bandwidth path for power-sensitive applications and a high-bandwidth path for dynamic AC measurements, provides engineers with a single-chip solution for a vast range of applications, from wearable health monitors to portable chemical analyzers.

Successful implementation of the AD5940/AD5941 hinges on a deep understanding of its core architectural components and their interactions. The programmable switch matrix, while complex, is the key to unlocking the chip's flexibility, enabling diverse sensor topologies. The on-chip sequencer is not merely a convenience but an essential tool for achieving the cycle-accurate timing and autonomous operation required for both high-performance and ultra-low-power applications.

While the hardware offers extensive capabilities, its complexity is effectively managed by the `ad5940lib` software library. This library provides a crucial abstraction layer that handles non-obvious initializations and simplifies the configuration of the AFE's numerous registers. For developers, a hybrid approach is often most effective: leveraging the library for rapid prototyping and system setup, while maintaining a register-level understanding to optimize performance, debug complex interactions, and port the solution to custom hardware platforms. By mastering both the underlying hardware architecture and the practical application of the software library, engineers can fully exploit the power and precision of the AD5940/AD5941 to create next-generation sensing devices.