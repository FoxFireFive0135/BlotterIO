#include <BlotterIO/Reading.hpp>
#include <BlotterIO/Writing.hpp>
#include <BlotterIO/Formatting.hpp>

int main(int argc, char* argv[]) {
	BlotterIO::WorldFile world = {};
	std::string path = "TestData/OddWorld/data.logicworld";

	int r = BlotterIO::read_world(&world, path, true);
	if (r != 0) return r;

	BlotterIO::print_save_info(world.info);

	return 0;
}