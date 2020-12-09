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

#pragma once

#include <vector>
#include <map>

namespace YukiWorkshop {
	class MiniBitmap {
	public:
		enum class PosMark : uint8_t {
			Begin = 0,
			End = 1,
			Probe = 2
		};
	protected:
		std::map<size_t, PosMark> used_regions_;
		size_t max_offset_ = 0;
	public:
		MiniBitmap() = default;

		[[nodiscard]] std::vector<std::pair<size_t, size_t>> used_regions() const noexcept {
			std::vector<std::pair<size_t, size_t>> ret;

			size_t idx = 0, tmp;
			for (const auto &it : used_regions_) {
				if (idx % 2)
					ret.emplace_back(tmp, it.first - tmp);
				else
					tmp = it.first;

				idx++;
			}

			return ret;
		}

		[[nodiscard]] std::vector<std::pair<size_t, size_t>> unused_regions(size_t max_offset = 0) const noexcept {
			std::vector<std::pair<size_t, size_t>> ret;

			size_t idx = 0, tmp;

			for (auto it = std::next(used_regions_.begin()); it != used_regions_.end(); it++) {
				if (idx % 2)
					ret.emplace_back(tmp, it->first - tmp);
				else
					tmp = it->first;

				idx++;
			}

			if (max_offset > max_offset_) {
				tmp = std::prev(used_regions_.end())->first;
				ret.emplace_back(tmp, max_offset - tmp);
			}

			return ret;
		}

		[[nodiscard]] size_t max_offset() const noexcept {
			return max_offset_;
		}

		[[nodiscard]] size_t used_size() const noexcept {
			size_t ret = 0;

			size_t i = 0, tmp;
			for (const auto &it : used_regions_) {
				if (i % 2)
					ret += (it.first - tmp);
				else
					tmp = it.first;

				i++;
			}

			return ret;
		}

		void mark_as_used(size_t start_offset, size_t length) {
			size_t end_offset = start_offset + length;

			auto[it_start, success_start] = used_regions_.insert({start_offset, PosMark::Begin});
			auto[it_end, success_end] = used_regions_.insert({end_offset, PosMark::End});

			if (success_start) {
				auto prev = std::prev(it_start);
				if (prev->second == PosMark::Begin) {
					it_start = prev;
				}
			} else {
				if (it_start->second == PosMark::End) {
					it_start = std::prev(it_start);
				}
			}

			if (success_end) {
				auto next = std::next(it_end);
				if (next->second == PosMark::End) {
					it_end = next;
				}
			} else {
				if (it_end->second == PosMark::Begin) {
					it_end = std::next(it_end);
				}
			}

			if (std::next(it_start) != it_end) {
				used_regions_.erase(std::next(it_start), it_end);
			}

			if (end_offset > max_offset_) {
				max_offset_ = end_offset;
			}
		}

		void mark_as_used(size_t offset) {
			mark_as_used(offset, 1);
		}

		void mark_as_unused(size_t start_offset, size_t length) {
			size_t end_offset = start_offset + length;

			auto[it_start, success_start] = used_regions_.insert({start_offset, PosMark::End});
			auto[it_end, success_end] = used_regions_.insert({end_offset, PosMark::Begin});

			if (success_start) {
				auto prev = std::prev(it_start);
				if (prev->second == PosMark::End) {
					it_start = prev;
				}
			} else {
				if (it_start->second == PosMark::Begin) {
					it_start = std::prev(it_start);
				}
			}

			if (success_end) {
				auto next = std::next(it_end);
				if (next->second == PosMark::Begin) {
					it_end = next;
				}
			} else {
				if (it_end->second == PosMark::End) {
					it_end = std::next(it_end);
				}
			}

			if (std::next(it_start) != it_end) {
				used_regions_.erase(std::next(it_start), it_end);
			}

			if (end_offset > max_offset_) {
				max_offset_ = end_offset;
			}
		}

		void mark_as_unused(size_t offset) {
			mark_as_unused(offset, 1);
		}

		bool is_used(size_t start_offset, size_t length) {
			size_t end_offset = start_offset + length;

			auto[it_start, success_start] = used_regions_.insert({start_offset, PosMark::Probe});
			if (success_start) {
				if (std::prev(it_start)->second == PosMark::Begin) {
					used_regions_.erase(it_start);
					return true;
				}
			} else {
				if (it_start->second == PosMark::Begin) {
					return true;
				}
			}

			auto[it_end, success_end] = used_regions_.insert({end_offset, PosMark::Probe});
			if (success_end) {
				if (std::next(it_end)->second == PosMark::End) {
					if (success_start) used_regions_.erase(it_start);
					used_regions_.erase(it_end);
					return true;
				}
			} else {
				if (it_end->second == PosMark::End) {
					if (success_start) used_regions_.erase(it_start);
					return true;
				}
			}

			if (std::next(it_start) == it_end) {
				if (success_start) used_regions_.erase(it_start);
				if (success_end) used_regions_.erase(it_end);
				return false;
			}

			if (success_start) used_regions_.erase(it_start);
			if (success_end) used_regions_.erase(it_end);
			return true;
		}

		bool is_used(size_t offset) {
			return is_used(offset, 1);
		}

		std::vector<uint8_t> serialize() {
			std::vector<uint8_t> ret;

			if (max_offset_ <= UINT8_MAX + 1) {
				ret.push_back(0);

				for (const auto &it : used_regions_) {
					uint8_t val = it.first;
					ret.insert(ret.end(), ((uint8_t *) (&val)), ((uint8_t *) (&val)) + sizeof(uint8_t));
				}
			} else if (max_offset_ <= UINT16_MAX + 1) {
				ret.push_back(1);

				for (const auto &it : used_regions_) {
					uint16_t val = it.first;
					ret.insert(ret.end(), ((uint8_t *) (&val)), ((uint8_t *) (&val)) + sizeof(uint16_t));
				}
			} else if (max_offset_ <= UINT32_MAX + 1) {
				ret.push_back(2);

				for (const auto &it : used_regions_) {
					uint32_t val = it.first;
					ret.insert(ret.end(), ((uint8_t *) (&val)), ((uint8_t *) (&val)) + sizeof(uint32_t));
				}
			} else {
				ret.push_back(3);

				for (const auto &it : used_regions_) {
					uint64_t val = it.first;
					ret.insert(ret.end(), ((uint8_t *) (&val)), ((uint8_t *) (&val)) + sizeof(uint64_t));
				}
			}

			return ret;
		}

		void deserialize(const void *data, size_t len) {
			if (len < 3) {
				throw std::logic_error("bad length");
			}

			auto *cur_pos = (const uint8_t *) data;
			auto *end_pos = cur_pos + len;

			auto mode = cur_pos[0];
			cur_pos++;

			uint idx = 0;

			switch (mode) {
				case 0:
					while (cur_pos < end_pos) {
						used_regions_.insert({*(uint8_t *) cur_pos, idx % 2 ? PosMark::End : PosMark::Begin});
						idx++;
						cur_pos += sizeof(uint8_t);
					}

					break;
				case 1:
					while (cur_pos < end_pos) {
						used_regions_.insert({*(uint16_t *) cur_pos, idx % 2 ? PosMark::End : PosMark::Begin});
						idx++;
						cur_pos += sizeof(uint16_t);
					}

					break;
				case 2:
					while (cur_pos < end_pos) {
						used_regions_.insert({*(uint32_t *) cur_pos, idx % 2 ? PosMark::End : PosMark::Begin});
						idx++;
						cur_pos += sizeof(uint32_t);
					}

					break;
				case 3:
					while (cur_pos < end_pos) {
						used_regions_.insert({*(uint64_t *) cur_pos, idx % 2 ? PosMark::End : PosMark::Begin});
						idx++;
						cur_pos += sizeof(uint64_t);
					}

					break;
				default:
					throw std::logic_error("bad mode");
			}

			max_offset_ = std::prev(used_regions_.end())->first;
		}

		void deserialize(const std::vector<uint8_t> &data) {
			deserialize(data.data(), data.size());
		}
	};

}