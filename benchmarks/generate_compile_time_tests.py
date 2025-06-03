start = 0
step_count = 1
end = int(input("Enter enum count: "))
is_enum_class_input = input("Enum classes? (True/False): ").strip().lower()
is_enum_class = is_enum_class_input == "true"
value_count = int(input("Enter number of enum values per enum (e.g., 15): "))
base_filename = input("Enter base file name (e.g., big_enums): ").strip()
min_range = int(input("Enter the min range for MAGIC_ENUM_RANGE_MIN and ENCHANTUM_MIN_RANGE: "))
max_range = int(input("Enter the max range for MAGIC_ENUM_RANGE_MAX and ENCHANTUM_MAX_RANGE: "))

# Configuration for each enum reflection library
libs = {
    "magic_enum": {
        "include_path": "magic_enum/magic_enum.hpp",
        "to_string": "magic_enum::enum_name",
        "min_macro": "MAGIC_ENUM_RANGE_MIN",
        "max_macro": "MAGIC_ENUM_RANGE_MAX"
    },
   #"conjure_enum": {
   #    "include_path": "fix8/conjure_enum.hpp",
   #    "to_string": "enum_to_string_conjure_wrapper",
   #    "min_macro": "FIX8_CONJURE_ENUM_MIN_VALUE",
   #    "max_macro": "FIX8_CONJURE_ENUM_MAX_VALUE"
   #},
    "enchantum": {
        "include_path": "enchantum/enchantum.hpp",
        "to_string": "enchantum::to_string",
        "min_macro": "ENCHANTUM_MIN_RANGE",
        "max_macro": "ENCHANTUM_MAX_RANGE"
    },
    #"simple_enum": {
    #    "include_path": "simple_enum/simple_enum.hpp",
    #    "to_string": "simple_enum::enum_name",
    #    "min_macro": "",
    #    "max_macro": ""
    #}
}

def create_tests(lib_name: str, lib_config: dict, filename: str):
    with open(filename, "w") as f:
        if lib_config["min_macro"] and lib_config["max_macro"]:
            f.write(f"""\
#define {lib_config["min_macro"]} ({min_range})
#define {lib_config["max_macro"]} ({max_range})
""")

        f.write(f'#include <{lib_config["include_path"]}>\n\n')
        if lib_name == "conjure_enum":
            f.write("""
template<typename T>
constexpr auto enum_to_string_conjure_wrapper(T e) {
    return conjure_enum<T>::enum_to_string(e);
}
            """)
        # Write enum definitions
        for i in range(start, end):
            enum_name = f"A_{i}"
            enum_type = "enum class" if is_enum_class else "enum"
            f.write(f"{enum_type} {enum_name} : int {{\n")
            values = [f"{enum_name}_{j} = {j}," for j in range(0,value_count,step_count)]
            f.write("".join(values))
            f.write(f"""
first = {enum_name}_0,
last = {enum_name}_{list(range(0, value_count, step_count))[-1]}
}};
""")

        for i in range(start, end):
            enum_name = f"A_{i}"
            for j in range(0,value_count,step_count):
                value_name = f"{enum_name}_{j}"
                f.write(f'static_assert({lib_config["to_string"]}({enum_name}::{value_name}) == "{value_name}");\n')

    print(f"File '{filename}' generated for '{lib_name}' with {end - start} enums.")

# generate benchmarks for each library
for lib_name, lib_config in libs.items():
    create_tests(lib_name, lib_config, f"{lib_name}_{base_filename}.cpp")

input("Press enter to exit...")
