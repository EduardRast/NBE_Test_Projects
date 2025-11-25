import serial
import time

def read_serial_until_differential(port_name, baud_rate=115200):
    log_data = []
    
    try:
        # 1. Open the connection
        print(f"Attempting to connect to {port_name}...")
        ser = serial.Serial(port_name, baud_rate, timeout=1)
        
        # --- THE FIX ---
        # We wait 2 seconds to allow the MCU to finish resetting 
        # and for the USB connection to stabilize.
        print("Connection opened. Waiting for device to stabilize...")
        time.sleep(2) 
        
        # Clear buffers to ensure we are reading fresh data
        ser.reset_input_buffer()
        print("Listening for data...")

        while True:
            try:
                # Check if there is data waiting
                if ser.in_waiting > 0:
                    line = ser.readline().decode('utf-8', errors='ignore').strip()

                    if line:
                        log_data.append(line)
                        print(f"Received: {line}")

                        # Check for the target word
                        if line.lower().startswith("differential"):
                            ser.close()
                            print("\nTarget found. Closing connection.")
                            return line
                            
            except OSError as e:
                # Catch the "Device not configured" if it happens mid-stream
                print(f"\nDevice error detected (did it unplug?): {e}")
                break
                
            # Small sleep to save CPU
            time.sleep(0.01)

    except serial.SerialException as e:
        print(f"Error connecting to serial port: {e}")
        return None
    except KeyboardInterrupt:
        print("Program stopped by user.")
        if 'ser' in locals() and ser.is_open:
            ser.close()
        return None

# --- Usage ---

# # If /dev/cu.usbmodem1401 keeps failing, try finding the /dev/tty.usbmodem1401 equivalent
# # But usually, the time.sleep(2) fixes the issue on the 'cu' port.
# target_string = read_serial_until_differential(port_name='/dev/tty.usbmodem1401', baud_rate=115200)

# print("-" * 30)
# if target_string:
#     print(f"Final Result: {target_string}")
# else:
#     print("Process finished without finding target.")