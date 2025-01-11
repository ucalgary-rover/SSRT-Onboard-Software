import serial
from pyubx2 import UBXReader
import time
import asyncio

GNSS_PORT = ""         # Serial port the GNSS is using, adjust as needed
GNSS_BAUDRATE = 38400       # Baud rate for GNSS
GNSS_PERIOD = 0.5           # Polling period in seconds

# Last year Marc's code
def read_gnss_data(serial_port, period, last_valid_position=None, debug_messages=False):
    """
    Polls GNSS for data. Processes, delays for the intended period, and error handles for getting latitude and longitude.

    Args:
        serial_port: Serial port connected to the GNSS device.
        period: Period between polls in seconds.
        last_valid_position: Last known valid position, initialized to None.
        debug_messages: If True, debug messages are printed, default is False.

    Yields:
        tuple: A tuple of floats containing latitude and longitude.
    """
    ubr = UBXReader(serial_port)

    def delay_thread():
        while True:
            time.sleep(period)
            yield ()

    delay_gen = delay_thread()

    while True:
        try:
            # Read and parse GNSS data
            (raw_data, parsed_data) = ubr.read()

            # Check if the data contains valid latitude and longitude
            if hasattr(parsed_data, "lat") and hasattr(parsed_data, "lon"):
                if parsed_data.lat is not None and parsed_data.lon is not None:
                    last_valid_position = (parsed_data.lat, parsed_data.lon)
                    if debug_messages:
                        print(f"Current Position: Latitude {parsed_data.lat}, Longitude {parsed_data.lon}")
            else:
                if debug_messages:
                    print("Received bad data. Using last valid position.")

        except Exception as e:
            if debug_messages:
                print(f"Error processing data: {e}. Using last valid position.")

        # Introduce the delay
        next(delay_gen)

        # Yield the last valid position
        yield last_valid_position


def main():
    # Open the serial port for GNSS
    with serial.Serial(port=GNSS_PORT, baudrate=GNSS_BAUDRATE, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE) as serial_port:
        # Initialize the GNSS data generator
        gen = read_gnss_data(serial_port, GNSS_PERIOD)

if __name__ == "__main__":
    asyncio.run(main())
