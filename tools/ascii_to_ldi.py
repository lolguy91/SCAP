def generate_assembly_code(input_string):
    assembly_code = []

    for char in input_string:
        hex_code = hex(ord(char))[2:].upper()
        assembly_code.append(f"ldi A 0x{hex_code}")
        assembly_code.append(f"st A 0xFF00")

    # Add newline, carriage return, and null byte
    assembly_code.extend([
        "ldi A 0x0A",  # ASCII code for newline
        "st A 0xFF00",
        "ldi A 0x0D",  # ASCII code for carriage return
        "st A 0xFF00"
    ])

    return '\n'.join(assembly_code)

# Example usage:
input_string = "time for a dump and die <3"
result = generate_assembly_code(input_string)
print(result)
