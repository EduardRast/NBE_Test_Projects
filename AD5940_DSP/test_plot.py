import matplotlib.pyplot as plt

def parse_differential_current(data_string):
    """
    Parses a string containing 'differential current: ' followed by 
    space-separated numbers into a list of floats.
    """
    try:
        # Split the string by the colon to separate the label from the data
        # The part after the colon (index 1) contains the numbers
        numbers_part = data_string.split(':')[1]
        raw_text_numbers = numbers_part.strip().split()
        
        # 3. Slice to keep only the first 100 items
        # If the list is shorter than 100, this safely returns all of them.
        first_100_text = raw_text_numbers[:]
        # Split the string into individual text numbers, then convert to floats
        # .strip() removes leading/trailing whitespace
        # .split() splits by whitespace by default
        current_list = [float(x) for x in first_100_text]
        
        return current_list
    except IndexError:
        print("Error: Format does not contain a colon.")
        return []
    except ValueError:
        print("Error: Could not convert data to numbers.")
        return []

# The input string
raw_data = "all measurements: -0.016 0.001 -0.003 0.002 0.003 0.001 0.003 -0.003 0.002 -0.004 0.002 -0.004 0.003 -0.003 0.003 -0.003 0.002 -0.003 0.002 -0.003 0.002 -0.003 0.002 -0.003 0.002 -0.003 0.003 -0.003 0.003 -0.003 0.003 -0.003 0.002 -0.003 0.002 -0.004 0.002 -0.003 0.002 -0.004 0.002 -0.004 0.002 -0.003 0.002 -0.003 0.002 -0.003 0.002 -0.003 0.002 -0.003 0.002 -0.003 0.002 -0.003 0.002 -0.003 0.002 -0.003 0.002 -0.003 0.002 -0.004 0.002 -0.004 0.002 -0.004 0.002 -0.004 0.002 -0.003 0.002 -0.003 0.002 -0.004 0.001 -0.004 0.002 -0.004 0.001 -0.004 0.003 -0.003 0.002 -0.003 0.002 -0.003 0.002 -0.004 0.002 -0.004 0.002 -0.004 0.002 -0.004 0.002 -0.004 0.002 -0.003 0.002 -0.004 0.002 -0.004 0.001 -0.004 0.002 -0.004 0.002 -0.004 0.003 -0.003 0.002 -0.003 0.002 -0.004 0.002 -0.004 0.002 -0.004 0.002 -0.004 0.002 -0.004 0.002 -0.004 0.002 -0.004 0.002 -0.004 0.002 -0.005 0.002 -0.005 0.002 -0.005 0.002 -0.005 0.002 -0.004 0.002 -0.004 0.002 -0.004 0.002 -0.005 0.002 -0.004 0.002 -0.004 0.002 -0.004 0.002 -0.004 0.002 -0.004 0.002 -0.005 0.002 -0.005 "




# Parse the data
result = parse_differential_current(raw_data)

# Output the results
print(f"Original String: {raw_data}")
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