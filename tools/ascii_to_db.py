def string_to_hex_ascii(input_string):
    hex_ascii_list = ['db 0x{:02X}'.format(ord(char)) for char in input_string][::-1]
    return '\n'.join(hex_ascii_list)

# Take input from the user
user_input = input("Enter a string: ")

# Convert the string to the desired format
output_result = string_to_hex_ascii(user_input)

# Print the result
print(output_result)
