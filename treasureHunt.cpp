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

class TreasureHunt
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
									{"scuba-diver", required_argument, nullptr, 'd'},
									{"hunt-order", required_argument, nullptr, 'o'},
									{ "verbose", no_argument, nullptr, 'v' },
									{ "stats", no_argument, nullptr, 's' },
									{ "show-path", required_argument, nullptr, 'p' },
									{ nullptr, 0, nullptr, '\0' } };


		while ((option = getopt_long(argc, argv, "hc:d:o:vsp:",
				longOpts, &option_index)) != -1) {
			switch (option) {
			case 'c':
				if (std::string(optarg) != "queue" && std::string(optarg) != "stack")
				{
					exit(1);
				}
				cap_type = optarg[0];
				break;

			case 'd':
				if (std::string(optarg) != "queue" && std::string(optarg) != "stack")
				{
					exit(1);
				}
				scuba_type = optarg[0];
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
				if(show_path == 'M' || show_path == 'L')
				{
					exit(1);
				}
				show_path = *optarg;
				break;
				
			case 'h':
				std::cout << "This program reads an input file that contains the file type,\n"
					<< "map size, and details of the search area. It then attempts to find the\n"
					<< "hidden treasure. It will output stats and a path, if you so choose.\n"
					<< "Usage: \'./hunt\n\t[--captain | -c] <queue / stack>\n"
					<< "\t[--scuba-diver | -d] <queue / stack>\n"
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
		if(input[0] == 'M')
		{
			read_from_map();
		}
		else if(input[0] == 'L')
		{
			read_from_list();
		}
		return;
	}

private:

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
			*/
			if(symb == '@')
			{
				start = { row, col };
			}
		}
	}
	
	// Helper function to check if a location is reef
	bool is_reef(int row, int col) const
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
	
	// Helper Function for Scuba-diver Search
	// UPDATES direction of discovered reef
	// RETURNS: coordinates of discovered reef, or -1, -1 if none found
	std::pair<int, int> search_reef(char dir, std::pair<int, int> loc)
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
			// Check if reef and not discovered
			if (is_reef(newRow, newCol) && !area_map[newRow][newCol].disc)
			{
				// Update direction and disc
				area_map[newRow][newCol].dir = dir;
				area_map[newRow][newCol].disc = true;
				// cout << "Discovered reef at " << newRow << "," << newCol << "\n";
				return { newRow, newCol };
				
			}
		}
		return { -1, -1 };
	}
	
	// THIS FUNCTION TAKES IN A STARTING Reef POSITION AND SEARCHES
	// CONTIGUOUS reef ACCORDING TO SEARCH ORDER
	// UPDATES: Treasure if found
	// REQUIRES: start.direction has already been updated, start is a reef loc
	// RETURNS: True if treasure found, false if no treasure found
	bool scuba_diver_search(std::pair<int, int> start)
	{
		
		// Push start to be first element in reef_cont
		reef_cont.push_front(start);
		// Special case if start is treasure
		if(area_map[start.first][start.second].symbol == '$')
		{
			treasure.first = start.first;
			treasure.second = start.second;
			++reef_count;
			return true;
		}
		// While not empty, search
		while(!reef_cont.empty())
		{
			
			std::pair<int, int> current = reef_cont.front();
			reef_cont.pop_front(); //reef_cont now empty
			++reef_count;
				
			// Discover adjacent reef in order
			for(int i = 0; i < 4; ++i)
			{
				// Call helper function
				std::pair<int, int> reef = search_reef(order[i], current);
				
				// If found
				if(reef.first != -1 && reef.second != -1)
				{
					// Check if treasure
					if (area_map[reef.first][reef.second].symbol == '$')
					{
						treasure = reef;
						++reef_count;
						return true;
					} 
					
					// Add to container accordingly
					if (scuba_type == 's') 
					{
						reef_cont.push_front(reef);
					}
					else if (scuba_type == 'q')
					{
						reef_cont.push_back(reef);
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
				// cout << "Discovered water at " << new_row << "," << new_col << "\n";
				return { new_row, new_col };
			}
		}

		// Else
		return { -1, -1 }; 
	}

public:
	// THIS FUNCTION TAKES IN A STARTING WATER POSITION AND SEARCHES
	// REEF/WATER NEARBY
	// IF Reef IS FOUND, THE SCUBA DIVER IS DISPATCHED
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
					// If reef, call scuba diver
					if(is_reef(loc.first, loc.second))
					{
						// VERBOSE
						if (verbose)
						{
							cout << "Went underwater at: " << loc.first << ","
								<< loc.second << "\n" << "Searching reef... ";
						}
						
						bool found = scuba_diver_search(loc);
						++dispatches;
						
						// If scuba diver returns true, return true
						if(found)
						{
							// VERBOSE
							if (verbose)
							{
								cout << "party found treasure at " << treasure.first
									<< "," << treasure.second << ".\n";
							}
							
							return true;
						} // if found
						
						// VERBOSE
						if (verbose)
						{
							cout << "party returned with no treasure.\n";
						}
					} // if reef
					
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
				<< "Reef locations investigated: " << reef_count << "\n"
				<< "Went underwater: " << dispatches << "\n";

			// If treasure was found
			if (treasure.first != -1 && treasure.second != -1)
			{
				cout << "Path length: " << path_len << "\n"
					<< "Treasure location: " << treasure.first << "," << treasure.second
					<< "\n";
			}
			cout << "--- STATS ---\n";
		}
	}

	// Prints whether treasure was found
	void print_results() const
	{
		// If treasure not found
		if(treasure.first == -1 || treasure.second == -1)
		{
			int count = water_count + reef_count;
			cout << "No treasure found after investigating "
				<< count << " locations.\n";
		}
		
		else
		{
			cout << "Treasure found at " << treasure.first << ","
				<< treasure.second << " with path length " << path_len
				<< ".\n";
		}
	}

	// Prints contents of area_map
	// HELPER FOR show_path
	void print_area_map()
	{
		for (int row = 0; row < map_size; ++row)
		{
			for (int col = 0; col < map_size; ++col)
			{
				cout << area_map[row][col].symbol;
			} //for
			cout << endl;
		} //for
	}

	//THIS FUNCTION EDITS area_map and prints
	void show_map(std::vector<std::pair<int,int > > &vec)
	{
		if(show_path != 'M')
		{
			return;
		}
		// Mark treasure as X
		area_map[treasure.first][treasure.second].symbol = 'X';
		// Loop thru vector, starting from location after treasure
		for(size_t i = 1; i < vec.size(); ++i)
		{
			// If previous loc was a different row, make current symbol =  '|'
			// If previous loc was different col, make current symbol '-'
			int prev_row = vec[i - 1].first;
			int prev_col = vec[i - 1].second;
			int row = vec[i].first;
			int col = vec[i].second;
			
			if(prev_row != row)
			{
				// If previous symbol is -, make it a plus
				if(area_map[prev_row][prev_col].symbol == '-')
				{
					area_map[prev_row][prev_col].symbol = '+';
					if (i != vec.size() - 1) {
						area_map[row][col].symbol = '|';
					}
				}
				else
				{
					if (i != vec.size() - 1) {
						area_map[row][col].symbol = '|';
					}
				}
			}
			else if(prev_col != col)
			{
				// If previous symbol is |, make it a column
				if (area_map[prev_row][prev_col].symbol == '|')
				{
					area_map[prev_row][prev_col].symbol = '+';
					if (i != vec.size() - 1) {
						area_map[row][col].symbol = '-';
					}
				}
				else
				{
					if (i != vec.size() - 1) {
						area_map[row][col].symbol = '-';
					}
				}
			}
		}
		// Print
		print_area_map();
	}
	
	//THIS FUNCTION PRINTS LOCATIONS VISITED
	void show_list(std::vector<std::pair<int, int> > &vec_in)
	{
		if(show_path != 'L')
		{
			return;
		}
		bool searching = false;
		cout << "Sail:\n";
		// End of vector is start
		for(int i = int(vec_in.size()) - 1; i >= 0; --i)
		{
			if(is_reef(vec_in[i].first, vec_in[i].second))
			{
				if(!searching)
				{
					cout << "Search:\n";
					searching = true;
				}
			}
			cout << vec_in[i].first << "," << vec_in[i].second << "\n";
		}
		
	}
	
	// Backtrace function helper
	std::pair<int, int> move_to_prev(int row, int col, char dir)
	{
		if(dir == 'n')
		{
			return { row + 1, col };
		}
		else if(dir == 's')
		{
			return { row - 1, col };
		}
		else if(dir == 'w')
		{
			return { row, col + 1 };
		}
		else
		{
			return { row, col - 1 };
		}
	}
	
	// THIS FUNCTION BACKTRACES FROM TREASURE TO START
	// REQUIRES: TREASURE HAS BEEN FOUND
	// MODIFIES: path_len
	// RETURNS: vector of locations in path, with index 0 being the treasure,
	//			and end index being start.
	std::vector<std::pair<int, int> > backtrace_path()
	{
		// Push treasure to first location in vec
		std::vector<std::pair<int, int> > vec;
		vec.push_back(treasure);
		
		std::pair<int, int> current = treasure;
		char dir = area_map[current.first][current.second].dir;
		// While current is not the start
		while(area_map[current.first][current.second].symbol != '@')
		{
			// Move to previous location
			current = move_to_prev(current.first, current.second, dir);
			// Update dir
			dir = area_map[current.first][current.second].dir;
			// Push new current to vector
			vec.push_back(current);
		}
		// If you get to this point, current is now the start location
		
		// Path-len is equal to length of the vector
		path_len = int(vec.size()) - 1;
		
		return vec;
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
	
	// Container used on reef
	std::deque< std::pair<int, int> > reef_cont;

	// 2-D array of Locations, index with vec[row][col]
	std::vector<std::vector<Location> > area_map;

	// Variables to be read in/ defaults
	int map_size = 0;
	std::pair<int, int> start;
	std::pair<int, int> treasure = {-1, -1};
	std::string order = "nesw";
	char cap_type = 's';
	char scuba_type = 'q';
	bool verbose = false;
	bool stats = false;
	char show_path = '0'; //default is no displayed path
	// Variables to keep track of stats
	int water_count = 0;
	int reef_count = 0;
	int dispatches = 0;
	int path_len = 0;
	
};




// Driver Function
int main(int argc, char *argv[])
{
	std::ios_base::sync_with_stdio(false);


	// Create a hunt, read options, and read input file
	TreasureHunt treasureHunt;
	treasureHunt.get_options(argc, argv);
	treasureHunt.read_data();
	//hunt.print_area_map();
	//hunt.print_options();
	bool found = treasureHunt.captain_search(); // Will print verbose if specified

	std::vector<std::pair<int, int>> locations;
	// Create container to store visited locations
	// Run only if found
	if (found)
	{
		locations = treasureHunt.backtrace_path();
	}
	// Run always
	treasureHunt.print_stats(); // Prints Stats
	// Run only if found
	if (found)
	{
		treasureHunt.show_list(locations);
		treasureHunt.show_map(locations);
	}

	// Run always
	treasureHunt.print_results();


	
	return 0;
}