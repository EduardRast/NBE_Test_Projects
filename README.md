# NBE Test Projects 

## Analysis of electrical components

1. Microneedle array 
Reference: https://www.nature.com/articles/s41551-022-00887-1

Electronics used: 
- AD5940 electrochemical analogue front end ([AD5940BCBZ-RL, Analog Devices](https://www.analog.com/media/en/technical-documentation/data-sheets/AD5940-5941.pdf))
- CYW20736S Bluetooth low energy (BLE) system-in-package (SiP) module (CYW20736S, Cypress Semiconductor)
- (LDO) regulator (LP5907UVX-2.8/ NOPB, Texas Instruments)
- Li-ion battery charger ([LTC4124EV#TRMPBF, Analog Devices](https://www.analog.com/media/en/technical-documentation/data-sheets/LTC4124.pdf))
    * Newer version of the charger ([TPS7A20](https://www.ti.com/lit/ds/symlink/tps7a20.pdf?ts=1759335357789))
- wireless charging coil (WR202020-18M8-G, TDK) 
- Li-ion coin cell battery (RJD2430C1ST1, Illinois Capacitor)

------
10/03 TODO: 
- find a chip that is a combination of nRF52840 and CYW20736S for the project
    * Done, mcu to use: nRF52840
- look at the schematics of the AD5940BCBZ-RL from the examples to see the differences/similarities 
    * done: no diference, can implement with some adaptations
- create a schematic for the testboard of AD5940
