////////////////////////////////////////////////////////////
//                                                        //
//  https://wiki.logic.world/wiki/Blotter_File_Format/v7  //
//                                                        //
////////////////////////////////////////////////////////////
//
//  uint32_t = 4 byte int, float, component address
//  uint16_t = 2 byte int
//  uint8_t = Byte, booleans
//
////////////////////////////////////////////////////////////

#ifndef FOXFIREFIVE_BLOTTERIO_TYPES
#define FOXFIREFIVE_BLOTTERIO_TYPES

#include <cstdint>
#include <vector>

namespace BlotterIO {
	struct Position {
		uint32_t x = 0;
		uint32_t y = 0;
		uint32_t z = 0;
	};

	struct Rotation {
		uint32_t x = 0;
		uint32_t y = 0;
		uint32_t z = 0;
		uint32_t w = 0;
	};

	// https://wiki.logic.world/wiki/Blotter_File_Format/v7#Version_%2816_bytes%29
	// https://learn.microsoft.com/en-us/dotnet/api/system.version.-ctor?view=net-10.0#system-version-ctor%28system-int32-system-int32-system-int32-system-int32%29
	struct Version {
		uint32_t major = 0;
		uint32_t minor = 0;
		uint32_t build = 0;
		uint32_t revision = 0;
	};

	struct ModVersion {
		std::vector<uint8_t> id = {}; // Text ID
		Version version = {}; // Version
	};

	struct ComponentID {
		uint16_t id = 0; // Numerical ID
		std::vector<uint8_t> text_id = {}; // Text ID
	};

	struct SaveInfo {
		uint8_t save_version = 0; // Save format version
		Version game_version = {}; // Game version
		uint8_t save_type = 0; // Save type
		uint32_t component_count = 0; // Number of components
		uint32_t wire_count = 0; // Number of wires

		std::vector<ModVersion> mod_versions = {}; // Mod versions
		std::vector<ComponentID> component_ids = {}; // Component IDs
	};

	struct ComponentData {
		uint32_t address = 0; // Address of the component
		uint32_t parent_address = 0; // Address of the components parent

		uint16_t id = 0; // Component ID
		Position position = {}; // Local fixed position
		Rotation rotation = {}; // Local rotation

		std::vector<uint32_t> inputs; // Input circuit state IDs
		std::vector<uint32_t> outputs; // Output circuit state IDs
		std::vector<uint8_t> custom_data = {}; // custom data
	};

	struct PegAddress {
		uint8_t type = 0; // Peg type
		uint32_t address = 0; // Component address for the component of the peg
		uint32_t index = 0; // Index of the peg on the component
	};

	struct WireData {
		PegAddress start = {}; // First point
		PegAddress end = {}; // Second point
		uint32_t id = 0; // Circuit state ID
		uint32_t angle = 0; // Rotation
	};

	/// <summary>
	/// Object data for worlds
	/// </summary>
	struct WorldData {
		std::vector<ComponentData> component_data = {};
		std::vector<WireData> wire_data = {};
		std::vector<uint8_t> states = {};
	};

	/// <summary>
	/// Object data for partial worlds
	/// </summary>
	struct PartialWorldData {
		std::vector<ComponentData> component_data = {};
		std::vector<WireData> wire_data = {};
		std::vector<uint32_t> states = {};
	};

	/// <summary>
	/// World files represent an entire playable world space, may have zero or more root components at different positions/rotations in the world, use the ".logicworld" file extension, and stores all circuit states contiguously, from circuit state 0 through the highest state.
	/// These are just the "data.logicworld" files in your saves.
	/// </summary>
	struct WorldFile {
		SaveInfo info = {};
		WorldData data = {};
	};

	/// <summary>
	/// Partial world files represent a substructure within a playable world space, can be loaded and added to a world with each root at an arbitrary position/rotation, must have at least one root component, use ".partialworld" file extension, and has a list of circuit state indexes that are On (All indexes not listed here are inferred as being Off)
	/// These are just the "data.partialworld" files in your subassemblies.
	/// </summary>
	struct PartialWorldFile {
		SaveInfo info = {};
		PartialWorldData data = {};
	};
}

#endif // FOXFIREFIVE_BLOTTERIO_TYPES