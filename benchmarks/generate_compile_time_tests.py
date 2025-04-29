# Get user-defined input
start = int(input("Enter start of range (inclusive): "))
end = int(input("Enter end of range (exclusive): "))
value_count = int(input("Enter number of enum values per enum (e.g., 15): "))
base_filename = input("Enter base file name (e.g., big_enums): ").strip()
max_range = int(input("Enter the max range for MAGIC_ENUM_RANGE_MAX and ENCHANTUM_MAX_RANGE: "))

def create_test(magic_enum: bool, filename: str):
    with open(filename, "w") as f:
        f.write(f"""
#define RANGE {max_range}
#define ENCHANTUM_MAX_RANGE            RANGE
#define MAGIC_ENUM_RANGE_MAX           RANGE
#define ENCHANTUM_MIN_RANGE            (-ENCHANTUM_MAX_RANGE)
#define MAGIC_ENUM_RANGE_MIN           (-MAGIC_ENUM_RANGE_MAX)
""")
        if magic_enum:
            f.write("#include <magic_enum/magic_enum.hpp>\n\n")
        else:
            f.write("#include <enchantum/enchantum.hpp>\n\n")

        for i in range(start, end):
            enum_name = f"A_{i-start}"
            f.write(f"enum class {enum_name} : int {{\n")
            values = [f"  {enum_name}_{j} = {j}" for j in range(value_count)]
            f.write(",\n".join(values))
            f.write("\n};\n\n")

        for i in range(start, end):
            enum_name = f"A_{i-start}"
            for j in range(value_count):
                value_name = f"{enum_name}_{j}"
                if magic_enum:
                    f.write(f'static_assert(magic_enum::enum_name({enum_name}::{value_name}) == "{value_name}");\n')
                else:
                    f.write(f'static_assert(enchantum::to_string({enum_name}::{value_name}) == "{value_name}");\n')

# Generate both tests
magic_enum_file = f"magic_enum_{base_filename}.cpp"
enchantum_file = f"enchantum_{base_filename}.cpp"

create_test(True, magic_enum_file)
print(f"File '{magic_enum_file}' generated successfully with enums from {start} to {end - 1}.")

create_test(False, enchantum_file)
print(f"File '{enchantum_file}' generated successfully with enums from {start} to {end - 1}.")
