#ifndef FOXFIREFIVE_BLOTTERIO_WRITING
#define FOXFIREFIVE_BLOTTERIO_WRITING

#include <BlotterIO/Types.hpp>
#include <string>

namespace BlotterIO {
	// These return:
	// 0 if nothing went wrong
	// 1 if the file cannot be opened
	// 2 if the directory failed to be created
	[[no_discard]] int write_world(BlotterIO::WorldFile& file, std::string path, bool log = false);
	[[no_discard]] int write_partial_world(BlotterIO::PartialWorldFile& file, std::string path, bool log = false);
}

#endif // FOXFIREFIVE_BLOTTERIO_WRITING