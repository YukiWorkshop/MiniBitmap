/*
    This file is part of MiniBitmap.

    Copyright (C) 2020 Yukino Song
    Copyright (C) 2020 ReimuNotMoe

    This program is free software: you can redistribute it and/or modify
    it under the terms of the Apache License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <MiniBitmap.hpp>

#include <iostream>

#include <cassert>

using namespace YukiWorkshop;

int main() {
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

	return 0;
}