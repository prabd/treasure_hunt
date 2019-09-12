// PROJECT IDENTIFIER = 40FB54C86566B9DDEAB902CC80E8CE85C1C62AAD

#include <deque>
#include <vector>
#include <tuple>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;

class Hunt
{



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
	char displayType = '0'; //default is no display
	
	
};




// Driver Function
int main(int argc, char *argv[])
{
	
	return 0;
}