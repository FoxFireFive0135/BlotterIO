#ifndef FOXFIREFIVE_BLOTTERIO_READING
#define FOXFIREFIVE_BLOTTERIO_READING

#include <BlotterIO/Types.hpp>
#include <string>

namespace BlotterIO {
	// These return:
	// 0 if all are valid
	// 1 if the header is invalid
	// 2 if the footer is invalid
	[[nodiscard]] int read_world(BlotterIO::WorldFile* file, std::string path, bool log = false);
	[[nodiscard]] int read_partial_world(BlotterIO::PartialWorldFile* file, std::string path, bool log = false);
}

#endif // FOXFIREFIVE_BLOTTERIO_READING