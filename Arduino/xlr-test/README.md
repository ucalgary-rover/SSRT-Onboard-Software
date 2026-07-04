# XLR Connectivity Test

Set up 2 Arduinos to test XLR serial connectivity. Both Arduinos need to be plugged into the laptop via USB simultaneously.

**Wiring:**
- Arduino A (xlr_tx): D3 → XLR pin 1, GND → XLR pin 3
- Arduino B (xlr_rx): D2 → XLR pin 2, GND → XLR pin 3

- `xlr_tx.ino`: upload to Arduino A, forwards Serial Monitor input out to XLR (TX on D3)
- `xlr_rx.ino`: upload to Arduino B, reads from XLR (RX on D2), prints to Serial Monitor
