/* Copyright (C) 2016 Alexander Shishenko <GamePad64@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "BandwidthCounter.h"

namespace librevault {

BandwidthCounter::BandwidthCounter() : down_bytes_(0), down_bytes_blocks_(0), down_bytes_last_(0), down_bytes_blocks_last_(0),
    up_bytes_(0), up_bytes_blocks_(0), up_bytes_last_(0), up_bytes_blocks_last_(0) {}

BandwidthCounter::Stats BandwidthCounter::heartbeat() {
	Stats stats;

	std::chrono::duration<float> since_last = std::chrono::high_resolution_clock::now() - last_heartbeat;
	stats.down_bandwidth_ = float(down_bytes_last_.exchange(0)) / since_last.count();
	stats.down_bandwidth_blocks_ = float(down_bytes_blocks_last_.exchange(0)) / since_last.count();
	stats.up_bandwidth_ = float(up_bytes_last_.exchange(0)) / since_last.count();
	stats.up_bandwidth_blocks_ = float(up_bytes_blocks_last_.exchange(0)) / since_last.count();

	stats.down_bytes_ = down_bytes_;
	stats.down_bytes_blocks_ = down_bytes_blocks_;
	stats.up_bytes_ = up_bytes_;
	stats.up_bytes_blocks_ = up_bytes_blocks_;

	last_heartbeat = std::chrono::high_resolution_clock::now();

	return stats;
}

void BandwidthCounter::add_down(uint64_t bytes) {
	down_bytes_ += bytes;
	down_bytes_last_ += bytes;
}

void BandwidthCounter::add_down_blocks(uint64_t bytes) {
	down_bytes_blocks_ += bytes;
	down_bytes_blocks_last_ += bytes;
}

void BandwidthCounter::add_up(uint64_t bytes) {
	up_bytes_ += bytes;
	up_bytes_last_ += bytes;
}

void BandwidthCounter::add_up_blocks(uint64_t bytes) {
	up_bytes_blocks_ += bytes;
	up_bytes_blocks_last_ += bytes;
}

} /* namespace librevault */
