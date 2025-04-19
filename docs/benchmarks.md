## Compile Time Benchmark Results

*Note: These were ran 3 times and took the average.*

### Range: -128 to 128 with to_string/enum_name calls for 200 small enums with 16 members from 0-15 values

| LIBRARY        | TIME                | NOTES                                |
|----------------|---------------------|--------------------------------------|
| `magic_enum`   | 01:20.626 minutes    |                                      |
| `enchantum`    | 33.416 seconds       | `ENCHANTUM_SEARCH_ITER_COUNT` = 32   |

---

### Range: -256 to 256 with to_string/enum_name calls for 32 big enums with 200 members from 0-199 values

| LIBRARY        | TIME                | NOTES                                |
|----------------|---------------------|--------------------------------------|
| `magic_enum`   | 37.032 seconds      |                                      |
| `enchantum`    | 22.212 seconds      | `ENCHANTUM_SEARCH_ITER_COUNT` = 32   |

### Range: -1024 to 1024 with to_string/enum_name calls for 200 small enums with 16 members from 0-15 values

| LIBRARY        | TIME                | NOTES                                |
|----------------|---------------------|--------------------------------------|
| `magic_enum`   | ~20 minutes and still not finished I got bored and killed the compiler seconds      |                                      |
| `enchantum`    | 03:12.947 minutes      | `ENCHANTUM_SEARCH_ITER_COUNT` = 32   |

