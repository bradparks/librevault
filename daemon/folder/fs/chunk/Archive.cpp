/* Copyright (C) 2014-2015 Alexander Shishenko <GamePad64@gmail.com>
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
#include <util/file_util.h>
#include <util/regex_escape.h>
#include "Archive.h"

#include "Client.h"
#include "folder/fs/FSFolder.h"
#include "folder/fs/AutoIndexer.h"
#include "ChunkStorage.h"

namespace librevault {

Archive::Archive(FSFolder& dir, Client& client) :
	Loggable(dir, "Archive"),
	dir_(dir),
	client_(client) {

	switch(dir_.params().archive_type) {
		case FolderParams::ArchiveType::NO_ARCHIVE: archive_strategy_ = std::make_unique<NoArchive>(this); break;
		case FolderParams::ArchiveType::TRASH_ARCHIVE: archive_strategy_ = std::make_unique<TrashArchive>(this); break;
		case FolderParams::ArchiveType::TIMESTAMP_ARCHIVE: archive_strategy_ = std::make_unique<TimestampArchive>(this); break;
		//case FolderParams::ArchiveType::BLOCK_ARCHIVE: archive_strategy_ = std::make_unique<NoArchive>(this); break;
		default: throw std::runtime_error("Wrong Archive type");
	}
}

void Archive::archive(const fs::path& from) {
	auto file_type = fs::symlink_status(from).type();

	// Suppress unnecessary events on dir_monitor.
	if(dir_.auto_indexer) dir_.auto_indexer->prepare_deleted_assemble(dir_.normalize_path(from));

	if(file_type == fs::directory_file) {
		if(fs::is_empty(from)) // Okay, just remove this empty directory
			fs::remove(from);
		else {  // Oh, damn, this is very NOT RIGHT! So, we have DELETED directory with NOT DELETED files in it
			for(auto it = fs::directory_iterator(from); it != fs::directory_iterator(); it++)
				archive(it->path()); // TODO: Okay, this is a horrible solution
			fs::remove(from);
		}
	}else if(file_type == fs::regular_file) {
		archive_strategy_->archive(from);
	}else if(file_type == fs::symlink_file || file_type == fs::file_not_found) {
		fs::remove(from);
	}
	// TODO: else
}

// NoArchive
void Archive::NoArchive::archive(const fs::path& from) {
	fs::remove(from);
}

// TrashArchive
Archive::TrashArchive::TrashArchive(Archive* parent) :
	ArchiveStrategy(parent),
	archive_path_(parent->dir_.system_path() / "archive"),
	cleanup_process_(parent->client_.ios(), [this](PeriodicProcess& process){
		maintain_cleanup(process);
	}) {

	fs::create_directory(archive_path_);
	cleanup_process_.invoke_after(std::chrono::minutes(10));    // Start after a small delay.
}

Archive::TrashArchive::~TrashArchive() {
	cleanup_process_.wait();
}

void Archive::TrashArchive::maintain_cleanup(PeriodicProcess& process) {
	parent_->log_->trace() << parent_->log_tag() << BOOST_CURRENT_FUNCTION;

	std::list<fs::path> removed_paths;
	try {
		for(auto it = fs::recursive_directory_iterator(archive_path_); it != fs::recursive_directory_iterator(); it++) {
			unsigned time_since_archivation = time(nullptr) - fs::last_write_time(it->path());
			constexpr unsigned sec_per_day = 60 * 60 * 24;
			if(time_since_archivation >= parent_->dir_.params().archive_trash_ttl * sec_per_day && parent_->dir_.params().archive_trash_ttl != 0)
				removed_paths.push_back(it->path());
		}

		for(const fs::path& path : removed_paths)
			fs::remove(path);

		cleanup_process_.invoke_after(std::chrono::hours(24));
	}catch(std::exception& e) {
		cleanup_process_.invoke_after(std::chrono::minutes(10));    // An error occured, retry in 10 min
	}
}

void Archive::TrashArchive::archive(const fs::path& from) {
	auto archived_path = archive_path_ / fs::path(parent_->dir_.normalize_path(from));
	parent_->log_->trace() << parent_->log_tag() << "Adding an archive item: " << archived_path;
	file_move(from, archived_path);
	fs::last_write_time(archived_path, time(nullptr));
}

// TimestampArchive
Archive::TimestampArchive::TimestampArchive(Archive* parent) :
	ArchiveStrategy(parent),
	archive_path_(parent->dir_.system_path() / "archive") {

	fs::create_directory(archive_path_);
}

void Archive::TimestampArchive::archive(const fs::path& from) {
	// Add a new entry
	auto archived_path = archive_path_ / fs::path(parent_->dir_.normalize_path(from));

	time_t mtime = fs::last_write_time(from);
	std::vector<char> strftime_buf(16);
	strftime(strftime_buf.data(), strftime_buf.size(), "%Y%m%d-%H%M%S", localtime(&mtime));

	std::string suffix = std::string("~")+strftime_buf.data();

	auto timestamped_path = archived_path.stem();
	timestamped_path += boost::locale::conv::utf_to_utf<native_char_t>(suffix);
	timestamped_path += archived_path.extension();
	file_move(from, timestamped_path);

	// Remove
	std::map<std::string, fs::path> paths;
	std::regex timestamp_regex(regex_escape(from.stem().generic_string()) + R"((~\d{8}-\d{6}))" + regex_escape(from.extension().generic_string()));
	for(auto it = fs::directory_iterator(from.parent_path()); it != fs::directory_iterator(); it++) {
		std::smatch match;
		std::string generic_path = it->path().generic_string();	// To resolve stackoverflow.com/q/32164501
		std::regex_match(generic_path, match, timestamp_regex);
		if(!match.empty()) {
			paths.insert({match[1].str(), from});
		}
	}
	if(paths.size() > parent_->dir_.params().archive_timestamp_count && parent_->dir_.params().archive_timestamp_count != 0) {
		fs::remove(paths.begin()->second);
	}
}

} /* namespace librevault */
