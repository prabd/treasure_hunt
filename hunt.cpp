// PROJECT IDENTIFIER = 40FB54C86566B9DDEAB902CC80E8CE85C1C62AAD

#include <getopt.h>
#include <iomanip>
#include <string>
#include <deque>
#include <vector>
#include <tuple>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;

class Hunt
{
	/*
	 * THIS FUNCTION READS COMMAND LINE AND UPDATES THE PARAMETERS OF THE HUNT
	 */
	void get_options(int argc, char *argv[])
	{
		int option_index = 0, option = 0;

		// Don't display getopt error messages about options
		opterr = false;


		// use getopt to find command line options
		struct option longOpts[] = { { "help", no_argument, nullptr, 'h' },
									{"captain", required_argument, nullptr, 'c'},
									{"first-mate", required_argument, nullptr, 'f'},
									{"hunt-order", required_argument, nullptr, 'o'},
									{ "verbose", no_argument, nullptr, 'v' },
									{ "stats", no_argument, nullptr, 's' },
									{ "show-path", required_argument, nullptr, 'p' },
									{ nullptr, 0, nullptr, '\0' } };


		while ((option = getopt_long(argc, argv, "hc:f:o:vsp:", longOpts, &option_index)) != -1) {
			switch (option) {
			case 'c':
				if (std::string(optarg) != "queue" && std::string(optarg) != "stack")
				{
					exit(1);
				}
				captainCont = optarg[0];
				break;

			case 'f':
				if (std::string(optarg) != "queue" && std::string(optarg) != "stack")
				{
					exit(1);
				}
				firstCont = optarg[0];
				break;

			case 'o':
			{
				// Check for no repeats
				std::string str = optarg;
				if (str.size() != 4) {
					exit(1);
				}
				for (size_t i = 0; i < str.size(); ++i)
				{
					int count = 0;
					for (size_t j = 0; j < str.size(); ++j)
					{
						if (str[i] == str[j])
						{
							++count;
						} //if
					} //for
					if (count != 1)
					{
						exit(1);
					} //if
				} //for
				searchOrder = str;
				break;
			}
			case 'v':
				verbose = true;
				break;
			case 's':
				stats = true;
				break;
			case 'p':
				if(std::string(optarg) != "M" && std::string(optarg) != "L")
				{
					exit(1);
				}
				showPath = *optarg;
				break;
				
			// TODO
			// Update help message
			case 'h':
				std::cout << "This program reads a CSV file that contains song names,\n"
					<< "the artist who wrote them, and the number of plays each song\n"
					<< "has on Spotify.  It then outputs the number of songs specified\n"
					<< "in the command line arguments (the print option), which\n"
					<< "defaults to 2, sorted by the option specified (one of name,\n"
					<< "artist, or listens).\n"
					<< "Usage: \'./project0\n\t[--listens | -l]\n"
					<< "\t[--name | -n]\n"
					<< "\t[--artist | -a]\n"
					<< "\t[--print | -p] <# of songs to print>\n"
					<< "\t[--help | -h]\n"
					<< "\t< <CSV Music File>\'" << std::endl;
				exit(0);
				
			default:
				exit(1);
			} //switch
		} //while
	} //get_options

	/*
	 *TODO
	 * THESE FUNCTIONS READ FROM A MAP OR A LIST AND UPDATE THE area_map MEMBER
	 * 
	 */

	// Read_data helper function 1
	// REQUIRES: area_map is already default initialized with proper size
	// THIS FUNCTION UPDATES area_map
	void read_from_map()
	{
		char symbol;
		// Loop over rows
		for(int row = 0; row < map_size; ++row)
		{
			// Loop over columns
			for(int col = 0; col < map_size; ++col)
			{
				cin >> symbol;
				area_map[row][col].symbol = symbol;
				// UPDATE START AND TREASURE LOCATIONS
				if(symbol == '$')
				{
					treasure = { row, col };
				}
				if(symbol == '@')
				{
					start = { row, col };
				}
			} //for
		} //for
	} //read_from_map

	// Read file helper function 2
	// REQUIRES: area_map is already default initialized with proper size
	// THIS FUNCTION UPDATES area_map
	void read_from_list()
	{
		int row;
		int col;
		char symb;
		while(cin >> row >> col >> symb)
		{
			area_map[row][col].symbol = symb;
			if(symb == '$')
			{
				treasure = { row, col };
			}
			if(symb == '@')
			{
				start = { row, col };
			}
		}
	}

	

	// THIS FUNCTION DETERMINES FILE-TYPE AND SIZE, RESERVES VECTOR SPACE,
	// AND CALLS HELPER
	void read_data()
	{
		std::string input;
		bool is_comment = true;
		// Loop to filter out commented lines from cin
		while(is_comment)
		{
			getline(cin, input);
			if(input[0] != '#')
			{
				is_comment = false;
			} //if
		} //for
		// input currently contains file-type specifier

		// Determine size
		cin >> map_size;

		// Default Location
		Location loc;
		
		// Resize col vector
		std::vector<Location> col;
		col.resize(map_size, loc);
		// Resize row vector, now containing copies of col
		area_map.resize(map_size, col);

		// Update area_map according to file-type
		if(input[0] == 'M' && input.size() == 1)
		{
			read_from_map();
		}
		else if(input[0] == 'L' && input.size() == 1)
		{
			read_from_list();
		}
	}

	


private:

	/*
	 * PRIVATE MEMBER VARIABLES
	 */
	
	// This struct is used to store information about a given location on the map.
	//		- A 2-D array used as a map will be populated with this struct.
	struct Location
	{
		bool discovered = false;
		char symbol = '.'; // either '#', 'o', '$', '@', or  '.'
	
		// either 'n', 's', 'e', or 'w'. Moved [direction] to get to this location.
		char direction = '0'; 
	};

	// This will be the container used on sea
	std::deque< std::tuple<int, int> > sea_container;
	
	// Container used on land
	std::deque< std::tuple<int, int> > land_container;

	// 2-D array of Locations, index with vec[row][col]
	std::vector<std::vector<Location> > area_map;

	// Variables to be read in/ defaults
	int map_size = 0;
	std::tuple<int, int> start;
	std::tuple<int, int> treasure;
	std::string searchOrder = "nesw";
	char captainCont = 's';
	char firstCont = 'q';
	bool verbose = false;
	bool stats = false;
	char showPath = '0'; //default is no displayed path
	
	
};




// Driver Function
int main(int argc, char *argv[])
{
	std::ios_base::sync_with_stdio(false);

	
	return 0;
}