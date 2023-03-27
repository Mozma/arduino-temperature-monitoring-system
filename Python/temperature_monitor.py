import minimalmodbus
import struct

def configure_instrument(instrument):
    instrument.clear_buffers_before_each_transaction = True
    instrument.close_port_after_each_call = True

    instrument.serial.baudrate = 9600
    instrument.serial.timeout = 10

def read_temperatures(instrument):
    data = instrument.serial.read(instrument.serial.in_waiting)

    if data:
        temp_c, temp_f = struct.unpack('ff', data[:8])
        print(f"Temperature: {temp_c:.2f} \u00b0C; {temp_f:.2f} \u00b0F")

def main():
    instrument = minimalmodbus.Instrument('/dev/ttyUSB0', 1, mode = minimalmodbus.MODE_RTU)
    configure_instrument(instrument)

    while True:
        read_temperatures(instrument)

if __name__ == '__main__':
   main()