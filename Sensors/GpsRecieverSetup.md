# Setting up `gpsd` on Ubuntu Linux

This guide walks you through installing and configuring `gpsd` (GPS Daemon) on Ubuntu Linux.  
`gpsd` is a service daemon that monitors one or more GPS devices connected to your computer and makes their data available to client applications.

## Prerequisites

- A GPS receiver (USB or serial)
- Ubuntu Linux (20.04 or newer recommended)
- Terminal access with `sudo` privileges


## Installation
Install required modules
```bash
sudo apt update
sudo apt install gpsd gpsd-clients
```

## Starting GPSD

### Starting GPSD manually
Enter device port used in place of [device]
Run gpsd service
```bash
gpsd /dev/[device] -n -N -D 3
```

### Starting on boot
Ensure that gpsd is configured via /etc/default/gpsd. To do so:

```bash
sudo nano /etc/default/gpsd
```
And modify the existing parameters.

GPSD can be configured to start on boot via systemctl service:

```bash
sudo systemctl restart gpsd
sudo systemctl enable gpsd
```

## Testing
Open a command line application to test read from the GPS devices connected

```bash
cgps 
```
Or if you'd like to know which port it's reading on
```bash
gpsmon
```

Check status of gpsd socket

```bash
systemctl status gpsd.socket
```

## Connection
Client applications typically communicate with gpsd via a TCP/IP port, port **2947** by default. Use Rover NUC network IP and 2947 as port to connect to GPSD NMEA stream.

## Notes
If using gpsd config, make sure that -G parameter is set and the ListenStream parameter exposes an empty address (e.g 0.0.0.0.2947) in one of these files to ensure gps data can be accessed over the network: 
- /etc/systemd/system/sockets.target.wants/gpsd.socket
- /lib/systemd/system/gpsd.socket
- /usr/lib/systemd/system/gpsd.socket

