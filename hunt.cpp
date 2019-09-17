// PROJECT IDENTIFIER = 40FB54C86566B9DDEAB902CC80E8CE85C1C62AAD

#include <getopt.h>
#include <string>
#include <deque>
#include <vector>
#include <tuple>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;

bool withinBounds(int row, int col, int size)
{
	if(row >= 0 && row < size && col >= 0 && col < size)
	{
		return true;
	}
	return false;
}

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
				cap_type = optarg[0];
				break;

			case 'f':
				if (std::string(optarg) != "queue" && std::string(optarg) != "stack")
				{
					exit(1);
				}
				first_type = optarg[0];
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
				order = str;
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
				show_path = *optarg;
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

private:
	/*
	*
	 * THESE FUNCTIONS READ FROM A MAP OR A LIST AND UPDATE THE area_map MEMBER
	*
	 */

	 // read_data helper function 1
	// REQUIRES: area_map is already default initialized with proper size
	// THIS FUNCTION UPDATES area_map
	void read_from_map()
	{
		char symb;
		// Loop over rows
		for (int row = 0; row < map_size; ++row)
		{
			// Loop over columns
			for (int col = 0; col < map_size; ++col)
			{
				cin >> symb;
				area_map[row][col].symbol = symb;
				// UPDATE START AND TREASURE LOCATIONS
				/*if(symb == '$')
				{
					treasure = { row, col };
				}*/
				if (symb == '@')
				{
					start = { row, col };
				}
			} //for
		} //for
	} //read_from_map

	// read_data helper function 2
	// REQUIRES: area_map is already default initialized with proper size
	// THIS FUNCTION UPDATES area_map
	void read_from_list()
	{
		int row;
		int col;
		char symb;
		while (cin >> row >> col >> symb)
		{
			area_map[row][col].symbol = symb;
			// UPDATE START AND TREASURE LOCATIONS
			/*if(symb == '$')
			{
				treasure = { row, col };
			}
			if(symb == '@')*/
			{
				start = { row, col };
			}
		}
	}
	
	// Helper function to check if a location is land
	bool is_land(int row, int col) const
	{
		char symb = area_map[row][col].symbol;
		if (symb == 'o' || symb == '$')
		{
			return true;
		}
		return false;
	}

	// Helper function to check if a location is water
	bool is_water(int row, int col) const
	{
		char symb = area_map[row][col].symbol;
		if (symb == '.')
		{
			return true;
		}
		return false;
	}
	
	// Helper Function for First-Mate Search
	// UPDATES direction of discovered land
	// RETURNS: coordinates of discovered land, or -1, -1 if none found
	std::pair<int, int> search_land(char dir, std::pair<int, int> loc)
	{
		int newRow = loc.first;
		int newCol = loc.second;

		// Update search coordinates based on direction
		if (dir == 'n')
		{
			newRow -= 1;
		}
		else if (dir == 's')
		{
			newRow += 1;
		}
		else if (dir == 'e')
		{
			newCol += 1;
		}
		else
		{
			newCol -= 1;
		}

		// Check if within bounds
		if (withinBounds(newRow, newCol, map_size))
		{
			// Check if land and not discovered
			if (is_land(newRow, newCol) && !area_map[newRow][newCol].disc)
			{
				// Update direction and disc
				area_map[newRow][newCol].dir = dir;
				area_map[newRow][newCol].disc = true;
				return { newRow, newCol };
			}
		}
		return { -1, -1 };
	}
	
	// THIS FUNCTION TAKES IN A STARTING LAND POSITION AND SEARCHES
	// CONTIGUOUS LAND ACCORDING TO SEARCH ORDER
	// UPDATES: Treasure if found
	// REQUIRES: start.direction has already been updated, start is a land loc
	// RETURNS: True if treasure found, false if no treasure found
	bool first_mate_search(std::pair<int, int> start)
	{
		
		// Push start to be first element in land_cont
		land_cont.push_front(start);
		// Special case if start is treasure
		if(area_map[start.first][start.second].symbol == '$')
		{
			treasure.first = start.first;
			treasure.second = start.second;
			++land_count;
			return true;
		}
		// While not empty, search
		while(!land_cont.empty())
		{
			
			std::pair<int, int> current = land_cont.front();
			land_cont.pop_front(); //land_cont now empty
			++land_count;
				
			// Discover adjacent land in order
			for(int i = 0; i < 4; ++i)
			{
				// Call helper function
				std::pair<int, int> land = search_land(order[i], current);
				
				// If found
				if(land.first != -1 && land.second != -1)
				{
					// Check if treasure
					if (area_map[land.first][land.second].symbol == '$')
					{
						treasure = land;
						return true;
					} 
					
					// Add to container accordingly
					if (first_type == 's') 
					{
						land_cont.push_front(land);
					}
					else if (first_type == 'q')
					{
						land_cont.push_back(land);
					}
						

				} //if found
			} //for
		}
		// If you get to this point, container is empty, no treasure was found.
		return false;
	}

	// Helper function for captain_search
	// UPDATES: dir and disc
	// RETURNS: location if discovered, -1, -1 if nothing new found
	std::pair<int, int> search_water(char dir, std::pair<int, int> loc)
	{
		int new_row = loc.first;
		int new_col = loc.second;

		// Update search coordinates based on direction
		if (dir == 'n')
		{
			new_row -= 1;
		}
		else if (dir == 's')
		{
			new_row += 1;
		}
		else if (dir == 'e')
		{
			new_col += 1;
		}
		else
		{
			new_col -= 1;
		}

		// Check if within bounds
		if(withinBounds(new_row, new_col, map_size))
		{
			// Check if not discovered and not impassable
			if(area_map[new_row][new_col].symbol != '#' && !area_map[new_row][new_col].disc)
			{
				// Update dir and disc
				area_map[new_row][new_col].disc = true;
				area_map[new_row][new_col].dir = dir;
				return { new_row, new_col };
			}
		}

		// Else
		return { -1, -1 }; 
	}

public:
	// THIS FUNCTION TAKES IN A STARTING WATER POSITION AND SEARCHES
	// LAND/WATER NEARBY
	// IF LAND IS FOUND, THE FIRST MATE IS DISPATCHED
	// RETURNS: True if treasure found, false if no treasure found
	bool captain_search()
	{
		// VERBOSE
		if(verbose)
		{
			cout << "Treasure hunt started at: "
				<< start.first << "," << start.second << "\n";
		}
		
		// Push start to first element in sea_cont
		sea_cont.push_front(start);
		while(!sea_cont.empty())
		{
			std::pair<int, int> current = sea_cont.front();
			sea_cont.pop_front(); // sea_cont now empty
			++water_count;
			// Discover adjacent locations in order
			for(int i = 0; i < 4; ++i)
			{
				std::pair<int, int> loc = search_water(order[i], current);
				// If valid
				if(withinBounds(loc.first, loc.second, map_size))
				{
					// If land, call first-mate
					if(is_land(loc.first, loc.second))
					{
						// VERBOSE
						if (verbose)
						{
							cout << "Went ashore at: " << loc.first << ","
								<< loc.second << "\n" << "Searching island... ";
						}
						
						bool found = first_mate_search(loc);
						++dispatches;
						
						// If first-mate returns true, return true
						if(found)
						{
							// VERBOSE
							if (verbose)
							{
								cout << "party found treasure at " << treasure.first
									<< "," << treasure.second << "\n";
							}
							
							return true;
						} // if found
						
						// VERBOSE
						if (verbose)
						{
							cout << "party returned with no treasure\n";
						}
					} // if land
					
					// If water, push to container
					else if(is_water(loc.first, loc.second))
					{
						if(cap_type == 's')
						{
							sea_cont.push_front(loc);
						}
						else if(cap_type == 'q')
						{
							sea_cont.push_back(loc);
						}
					}
				} //if valid
			} //for
			
		} // while

		// If you get to this point, sea_cont is empty, no treasure has been found.

		// VERBOSE
		if (verbose)
		{
			cout << "Treasure hunt failed\n";
		}
		return false;
	}

	// Prints additional statistics
	void print_stats() const
	{
		if(stats){
			cout << "--- STATS ---\n"
				<< "Starting location: " << start.first << "," << start.second << "\n"
				<< "Water locations investigated: " << water_count << "\n"
				<< "Land locations investigated: " << land_count << "\n"
				<< "Went ashore: " << dispatches << "\n";

			// If treasure was found
			if (treasure.first != -1 && treasure.second != -1)
			{
				cout << "Path length: " << path_len << "\n"
					<< "Treasure Location: " << treasure.first << "," << treasure.second
					<< "\n" << "--- STATS ---\n";
			}
		}
	}

	// Prints whether treasure was found
	void print_results() const
	{
		// If treasure not found
		if(treasure.first == -1 || treasure.second == -1)
		{
			int count = water_count + land_count;
			cout << "No treasure found after investigating "
				<< count << "locations." << endl;
		}
		
		else
		{
			cout << "Treasure found at " << treasure.first << ","
				<< treasure.second << " with path length " << path_len
				<< ".\n";
		}
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
		cout << "Captain Container: " << cap_type << endl;
		cout << "First Mate Container: " << first_type << endl;
		cout << "Hunt Order: " << order << endl;
		cout << "Verbose: " << verbose << endl;
		cout << "Statistics: " << stats << endl;
		cout << "Show Path: " << show_path << endl;
	}


private:

	/*
	 * PRIVATE MEMBER VARIABLES
	 */
	
	// This struct is used to store information about a given location on the map.
	//		- A 2-D array used as a map will be populated with this struct.
	struct Location
	{
		bool disc = false;
		char symbol = '.'; // either '#', 'o', '$', '@', or  '.'
	
		// either 'n', 's', 'e', or 'w'. Moved [direction] to get to this location.
		char dir = '0'; 
	};

	// This will be the container used on sea
	std::deque< std::pair<int, int> > sea_cont;
	
	// Container used on land
	std::deque< std::pair<int, int> > land_cont;

	// 2-D array of Locations, index with vec[row][col]
	std::vector<std::vector<Location> > area_map;

	// Variables to be read in/ defaults
	int map_size = 0;
	std::pair<int, int> start;
	std::pair<int, int> treasure = {-1, -1};
	std::string order = "nesw";
	char cap_type = 's';
	char first_type = 'q';
	bool verbose = false;
	bool stats = false;
	char show_path = '0'; //default is no displayed path
	// Variables to keep track of stats
	int water_count = 0;
	int land_count = 0;
	int dispatches = 0;
	int path_len = 0;
	
};




// Driver Function
int main(int argc, char *argv[])
{
	std::ios_base::sync_with_stdio(false);

	// Create a hunt, read options, and read input file
	Hunt hunt;
	hunt.get_options(argc, argv);
	hunt.read_data();
	//print_area_map();
	//hunt.print_options();
	bool found = hunt.captain_search();
	
	
	return 0;
}