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
public:
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
				std::cout << "This program reads an input file that contains the file type,\n"
					<< "map size, and details of the search area. It then attempts to find the\n"
					<< "hidden treasure. It will output stats and a path, if you so choose.\n"
					<< "Usage: \'./hunt\n\t[--captain | -c] <queue / stack>\n"
					<< "\t[--first-mate | -f] <queue / stack>\n"
					<< "\t[--hunt-order | -o] <directions>\n"
					<< "\t[--verbose | -v]\n"
					<< "\t[--stats | -s]\n"
					<< "\t[--show-path | -p] <M / L>\n"
					<< "\t[--help | -h]\n"
					<< "\t< <input file>\n"
					<< "\t> <output file>\'" << std::endl;
				exit(0);
				
			default:
				exit(1);
			} //switch
		} //while
	} //get_options

	/*
	 *
	 * THESE FUNCTIONS READ FROM A MAP OR A LIST AND UPDATE THE area_map MEMBER
	 * 
	 */

	// Read_data helper function 1
	// REQUIRES: area_map is already default initialized with proper size
	// THIS FUNCTION UPDATES area_map
	void read_from_map()
	{
		char symb;
		// Loop over rows
		for(int row = 0; row < map_size; ++row)
		{
			// Loop over columns
			for(int col = 0; col < map_size; ++col)
			{
				cin >> symb;
				area_map[row][col].symbol = symb;
				// UPDATE START AND TREASURE LOCATIONS
				if(symb == '$')
				{
					treasure = { row, col };
				}
				if(symb == '@')
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
			// UPDATE START AND TREASURE LOCATIONS
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
		return;
	}

	/**
	 * FOR DEBUGGING PURPOSES
	 */
	void print_area_map()
	{
		cout << endl;
		for(int row = 0; row < map_size; ++row)
		{
			for(int col = 0; col < map_size; ++col)
			{
				cout << area_map[row][col].symbol;
			} //for
			cout << endl;
		} //for
		cout << endl;
		cout << "Start: " << start.first << start.second << endl;
		cout << "Treasure: " << treasure.first << treasure.second << endl;
	}
	
	void print_options() const
	{
		cout << "Options: " << endl;
		cout << "Captain Container: " << captainCont << endl;
		cout << "First Mate Container: " << firstCont << endl;
		cout << "Hunt Order: " << searchOrder << endl;
		cout << "Verbose: " << verbose << endl;
		cout << "Statistics: " << stats << endl;
		cout << "Show Path: " << showPath << endl;
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
	std::pair<int, int> start;
	std::pair<int, int> treasure = {-1, -1};
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

	Hunt hunt;
	hunt.get_options(argc, argv);
	hunt.read_data();
	hunt.print_area_map();
	hunt.print_options();
	return 0;
}