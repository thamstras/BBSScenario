#include <string>
#include <vector>
#include <iostream>
#include <cstdint>

/*
EX	00	Generic
DP	01	Land of Departure
SW	02	Dwarf Woodlands
CD	03	Castle of Dreams
SB	04	Enchanted Dominion
YT	05	Mysterious Tower
RG	06	Radiant Garden
JB	07	Jungle Book (JP version)
JB	07	Dark World (FM version)
HE	08	Olympus Coliseum
LS	09	Deep Space
DI	0A	Destiny Island
PP	0B	Neverland
DC	0C	Disney Town
KG	0D	Keyblade Graveyard
14	0E	unused
VS	0F	Mirage Arena
BD	10	Command Board
WM	11	World Map
WP	12	Winnie the Pooh
19	13	unused
20	14	unused
21	15	unused
22	16	unused
JF	17	Jump Festa
*/

void PrintWorld(std::ostream& str, uint16_t worldID)
{
	switch (worldID)
	{
#define WORLDCASE(ID, X) case ID: str << "WORLD_" #X; break
		WORLDCASE(0x00, EX);
		WORLDCASE(0x01, DP);
		WORLDCASE(0x02, SW);
		WORLDCASE(0x03, CD);
		WORLDCASE(0x04, SB);
		WORLDCASE(0x05, YT);
		WORLDCASE(0x06, RG);
		WORLDCASE(0x07, JB);
		WORLDCASE(0x08, HE);
		WORLDCASE(0x09, LS);
		WORLDCASE(0x0A, DI);
		WORLDCASE(0x0B, PP);
		WORLDCASE(0x0C, DC);
		WORLDCASE(0x0D, KG);
		WORLDCASE(0x0E, 14);
		WORLDCASE(0x0F, VS);
		WORLDCASE(0x10, BD);
		WORLDCASE(0x11, WM);
		WORLDCASE(0x12, WP);
		WORLDCASE(0x13, 19);
		WORLDCASE(0x14, 20);
		WORLDCASE(0x15, 21);
		WORLDCASE(0x16, 22);
		WORLDCASE(0x17, JF);
#undef WORLDCASE
	}
}

enum WorldFlag
{
	_CODE_END = 0,
	_CODE_NEW_EPISODE = 1,
	_CODE_OLD_EPISODE = 2,
	_CODE_HIDDEN = 3,
	_CODE_DARKNESS = 4,
	_CODE_FREE = 5,
	_CODE_LANDABLE = 6,
	_CODE_SHIELD = 7,
	_CODE_CLEARED = 8,
	_CODE_NEXT = 9,
	_CODE_ROUTELESS = 10,
	_CODE_DISCONNECT = 11,
	_CODE_CONNECT = 12,
	_CODE_DICE_OPEN = 13,
	_CODE_REFUSE = 14,
	_CODE_UNKNOWN = 15,
	_CODE_KNOWN = 16,
	_CODE_MESSAGE = 17,
	_CODE_UNCLEAR = 18
};

std::ostream& operator<<(std::ostream& str, WorldFlag flag)
{
	switch (flag)
	{
#define PRINTCASE(X) case X: str << #X; break
		PRINTCASE(_CODE_END);
		PRINTCASE(_CODE_NEW_EPISODE);
		PRINTCASE(_CODE_OLD_EPISODE);
		PRINTCASE(_CODE_HIDDEN);
		PRINTCASE(_CODE_DARKNESS);
		PRINTCASE(_CODE_FREE);
		PRINTCASE(_CODE_LANDABLE);
		PRINTCASE(_CODE_SHIELD);
		PRINTCASE(_CODE_CLEARED);
		PRINTCASE(_CODE_NEXT);
		PRINTCASE(_CODE_ROUTELESS);
		PRINTCASE(_CODE_DISCONNECT);
		PRINTCASE(_CODE_CONNECT);
		PRINTCASE(_CODE_DICE_OPEN);
		PRINTCASE(_CODE_REFUSE);
		PRINTCASE(_CODE_UNKNOWN);
		PRINTCASE(_CODE_KNOWN);
		PRINTCASE(_CODE_MESSAGE);
		PRINTCASE(_CODE_UNCLEAR);
	}
	return str;
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "No path!" << std::endl;
		return 1;
	}
	else if (argc > 2)
	{
		std::cerr << "Too many params (need quotes?)" << std::endl;
		return 1;
	}

	FILE* fp = fopen(argv[1], "rb");
	if (fp == 0)
	{
		std::cerr << "Failed to open file: " << strerror(errno) << std::endl;
		return 1;
	}

	fseek(fp, 0, SEEK_END);
	long fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	uint8_t* fileBuffer = (uint8_t*)malloc(fileSize);
	if (fileBuffer == nullptr)
	{
		std::cerr << "Malloc fail." << std::endl;
		fclose(fp);
		return 1;
	}

	size_t readSize = fread(fileBuffer, 1, fileSize, fp);
	if (readSize < fileSize)
	{
		std::cerr << "Read less than file size." << std::endl;
		free(fileBuffer);
		fclose(fp);
		return 1;
	}

	uint32_t version = *(uint32_t*)(fileBuffer + 0);
	uint32_t eventCount = *(uint32_t*)(fileBuffer + 4);

	std::cout << "Version: " << version << std::endl << "EventCount: " << eventCount << std::endl;

	std::vector<std::pair<uint32_t, uint32_t>> eventMap = std::vector<std::pair<uint32_t, uint32_t>>();
	uint32_t* eventList = (uint32_t*)(fileBuffer + 8);
	for (int i = 0; i < eventCount; i++)
	{
		uint32_t ident = eventList[(2 * i) + 0];
		uint32_t offset = eventList[(2 * i) + 1];
		eventMap.push_back(std::make_pair(ident, offset));
	}

	for each (auto& eventInfo in eventMap)
	{
		std::cout << "EVENT IDENT " << std::hex << eventInfo.first << std::dec;
		
		WorldFlag lastCmd = _CODE_END;
		uint16_t* pCode = (uint16_t*)(fileBuffer + eventInfo.second);
		do
		{
			uint16_t val = *(pCode); pCode++;
			WorldFlag cmd = (WorldFlag)(val & 0x00FF);
			uint16_t size = (val & 0xFF00) >> 8;
			std::cout << std::endl << "\t" << cmd << "(" << size << ")";
			
			lastCmd = cmd;

			if (cmd == _CODE_END) break;

			// SHOULDN'T HAPPEN, GAME COMPLETELY IGNORES THIS
			if (cmd == _CODE_OLD_EPISODE) continue;

			if (cmd == _CODE_NEW_EPISODE)
			{
				for (int i = 0; i < size; i++)
				{
					uint16_t dat = *(pCode); pCode++;
					uint8_t worldID = (uint8_t)((dat & 0x00FF) >> 0);
					uint8_t episodeID = (uint8_t)((dat & 0xFF00) >> 8);
					std::cout << " ";
					PrintWorld(std::cout, (uint16_t)worldID);
					std::cout << "(" << (uint16_t)episodeID << ")";
				}
				continue;
			}

			if (cmd == _CODE_MESSAGE)
			{
				std::cout << " DATA:" << std::hex;
				for (int i = 0; i < size; i++)
				{
					std::cout << " " << *(pCode); pCode++;
				}
				std::cout << std::dec;
				continue;
			}

			if (cmd == _CODE_CONNECT || cmd == _CODE_DISCONNECT || cmd == _CODE_ROUTELESS)
			{
				std::cout << " ROUTES:";
				for (int i = 0; i < size; i++)
				{
					std::cout << " " << *(pCode); pCode++;
				}
				continue;
			}

			// everything else
			//std::cout << " WORLDS:";
			for (int i = 0; i < size; i++)
			{
				std::cout << " ";
				PrintWorld(std::cout, *(pCode)); pCode++;
			}


		} while (lastCmd != _CODE_END);
		std::cout << std::endl;
	}
}