# MiniBitmap
Simple bitmap implementation in C++ for free/used space marking.

## Requirements
- C++17

## Features
- Header only
- Supports serialize/deserialize inner data structures

## Usage
```cpp
#include <MiniBitmap.hpp>

using namespace YukiWorkshop;
```

```cpp
MiniBitmap m;

m.mark_as_used(0, 10);
m.mark_as_unused(2, 5);

m.mark_as_used(20, 30);
m.mark_as_used(50, 20);

m.mark_as_unused(33);
m.mark_as_unused(66);

auto data = m.serialize();

std::cout << "Serialized size: " << data.size() << "\n";

MiniBitmap m2;
m2.deserialize(data);

assert(!m2.is_used(11, 8));

puts("Used:");
for (auto &it : m2.used_regions()) {
    std::cout << "offset: " << it.first << ", length: " << it.second << "\n";
}

puts("Unused:");
for (auto &it : m2.unused_regions(70)) {
    std::cout << "offset: " << it.first << ", length: " << it.second << "\n";
}

std::cout << "Max offset: " << m2.max_offset() << "\n";
std::cout << "Total used size: " << m2.used_size() << "\n";
```

Results:
```
Serialized size: 11
Used:
offset: 0, length: 2
offset: 7, length: 3
offset: 20, length: 13
offset: 34, length: 32
offset: 67, length: 3
Unused:
offset: 2, length: 5
offset: 10, length: 10
offset: 33, length: 1
offset: 66, length: 1
Max offset: 70
Total used size: 53
```
## License
Apache 2.0