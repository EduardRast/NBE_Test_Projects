import test_plot
import test_pyserial
import matplotlib.pyplot as plt


if __name__ == '__main__':

    print("Hello there")

    target_string = test_pyserial.read_serial_until_differential(port_name='/dev/tty.usbmodem1401', baud_rate=115200)
    result = test_plot.parse_differential_current(target_string)

    # Output the results
    print(f"Original String: {target_string}")
    print(f"Parsed List: {result}")

    if result:
        print(f"Type of elements: {type(result[0])}")

        # Plot the data
        plt.figure(figsize=(10, 6))
        plt.plot(result, linestyle='-', color='blue', label='Current')
        plt.title('Differential Current Readings')
        plt.xlabel('Sample Index')
        plt.ylabel('Current Value')
        plt.grid(True)
        plt.legend()
        plt.show()

