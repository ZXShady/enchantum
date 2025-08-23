enum_start = 0
step_count = 1
is_enum_class = True

test_cases = [
    {
        "value_count": 200,
        "base_filename": "big_enums",
        "enum_count": 32,
        "min_range": -256,
        "max_range": 256
    },
    {
        "value_count": 16,
        "base_filename": "small_enums",
        "enum_count": 200,
        "min_range": -128,
        "max_range": 128
    },
    {
        "value_count": 16,
        "base_filename": "small_enums_large_range",
        "enum_count": 200,
        "min_range": -1024,
        "max_range": 1024
    },
    {
        "value_count": 50,
        "base_filename": "ideal_range",
        "enum_count": 100,
        "min_range": 0,
        "max_range": 50
    }
]

libs = {
    "magic_enum": {
        "include_path": "magic_enum/magic_enum.hpp",
        "to_string": "magic_enum::enum_name",
        "min_macro": "MAGIC_ENUM_RANGE_MIN",
        "max_macro": "MAGIC_ENUM_RANGE_MAX"
    },
    "conjure_enum": {
        "include_path": "fix8/conjure_enum.hpp",
        "to_string": "enum_to_string_conjure_wrapper",
        "min_macro": "FIX8_CONJURE_ENUM_MIN_VALUE",
        "max_macro": "FIX8_CONJURE_ENUM_MAX_VALUE"
    },
    "enchantum": {
        "include_path": "enchantum/enchantum.hpp",
        "to_string": "enchantum::to_string",
        "min_macro": "ENCHANTUM_MIN_RANGE",
        "max_macro": "ENCHANTUM_MAX_RANGE"
    },
    "simple_enum": {
        "include_path": "simple_enum/simple_enum.hpp",
        "to_string": "simple_enum::enum_name",
        "min_macro": "",
        "max_macro": ""
    }
}

def create_tests(lib_name: str, lib_config: dict, filename: str, enum_count: int, value_count: int, min_range: int, max_range: int):
    with open(filename, "w") as f:
        if lib_config["min_macro"] and lib_config["max_macro"]:
            f.write(f"#define {lib_config['min_macro']} ({min_range})\n")
            f.write(f"#define {lib_config['max_macro']} ({max_range})\n")
        if lib_name == "conjure_enum":
            f.write(f"#define FIX8_CONJURE_ENUM_MINIMAL 1\n")
        
        f.write(f'#include <{lib_config["include_path"]}>\n\n')

        if lib_name == "conjure_enum":
            f.write("""
template<typename T>
constexpr auto enum_to_string_conjure_wrapper(T e) {
    return FIX8::conjure_enum<T>::enum_to_string(e);
}
""")

        # Enum declarations
        for i in range(enum_start, enum_count):
            enum_name = f"A_{i}"
            enum_type = "enum class" if is_enum_class else "enum"
            f.write(f"{enum_type} {enum_name} : int {{\n")
            values = [f"    {enum_name}_{j} = {j},\n" for j in range(0, value_count, step_count)]
            f.writelines(values)
            if lib_name == "simple_enum":
                f.write(f"    first = {min_range},\n")
                f.write(f"    last = {max_range}\n")
            f.write("};\n\n")

        f.write("""
template<typename T>
volatile T var = T{};

const char* volatile pointer = nullptr; 
int main() {\n
""")
        for i in range(enum_start, enum_count):
            enum_name = f"A_{i}"
            value_name = f"{enum_name}_0"
            f.write(f'    pointer = {lib_config["to_string"]}(var<{enum_name}>).data();\n')
        f.write("    return 0;\n}\n")

    print(f"Generated '{filename}' for '{lib_name}' with {enum_count - enum_start} enums.")

for case in test_cases:
    for lib_name, lib_config in libs.items():
        filename = f"{lib_name}_{case['base_filename']}.cpp"
        create_tests(
            lib_name=lib_name,
            lib_config=lib_config,
            filename=filename,
            enum_count=case["enum_count"],
            value_count=case["value_count"],
            min_range=case["min_range"],
            max_range=case["max_range"]
        )
