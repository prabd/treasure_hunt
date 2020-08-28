<!-- PROJECT SHIELDS -->
[![Contributors][contributors-shield]][contributors-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]

<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/prabd/treasure_hunt">
    <img src="https://escapetrails.co/wp-content/uploads/2020/03/Treasure-hunt.jpg" alt="Logo" width="160" height="80">
  </a>
</p>


<!-- TABLE OF CONTENTS -->
## Table of Contents

* [About the Project](#about-the-project)
    * [The Situation](#the-situation)
    * [Input File](#input-file)
        * [Symbols](#symbols)
        * [Map](#map)
        * [List](#list)
* [Getting Started](#getting-started)
  * [Installation](#installation)
* [Usage](#usage)
    * [CLI](#cli)
        * [Options](#options)
* [Testing](#testing)
* [Roadmap](#roadmap)
* [License](#license)
* [Contact](#contact)


<!-- ABOUT THE PROJECT -->
## About The Project

This project explores the use of depth-first and breadth-first search (DFS, BFS) in finding a path, if possible, from a given start point to a given end point.

### The Situation
The pirate ship Hundred Sunny is out looking for treasure in the vast ocean. The captain defaults to DFS (using a stack) and can only search above water. To look for treasure in the coral reefs underwater, the captain sends out the scuba diver, who can only search coral reefs and defaults to BFS (using a queue). Given an input map, the program then attempts to search for treasure, and outputs statistics and actions to the terminal.

### Input File
The input file gives a description of the terrain at each coordinate, which the program then runs its treasure hunt on.
The input file can be structured one of two ways: as a `Map`, or as a `List`.

#### Symbols
Various symbols are used to describe the terrain:
* **.**  --> Water
* **o** --> Coral Reef
* **#** --> Blocked
* **@** --> Start
* **$** --> Treasure

The Start location is always on water, and Treasure can only be found in the coral reefs underwater.

#### Map
One way the input file can be structured is as a map, like the following:
```sh
M
5
.$.oo
.....
ooooo
....@
.....
```
The `M` signifies that the format is a Map, and the `5` signifies the 5x5 area.

#### List
The second way the input file can be structured is as a list, like the following:
```sh
L
4
0 0 $
0 2 o
0 3 o
1 1 o
2 0 o
3 0 o
3 3 @
3 1 o
1 3 .
```
The `L` signifies that the format is a List, and the `4` specifies the 4x4 area.


<!-- GETTING STARTED -->
## Getting Started
To get a local copy up and running follow these simple steps.

### Installation

1. Clone the repo
```sh
git clone https://github.com/prabd/treasure_hunt.git
```
2. Navigate into the directory
```sh
cd treasure_hunt/
```
3. Build the executable
```sh
make
```


<!-- USAGE EXAMPLES -->
## Usage

### Help
```sh 
./treasureHunt -h
./treasureHunt --help
```

### CLI
The general way to run the program is:
```sh
./treasureHunt [options] < INPUTFILE
```

It's important to note that the input redirection is used to feed the input.

#### Options
There are several options to modify the program behavior:
* **-h, --help**
  * Prints help message and quits program
* **-c, --captain <queue/stack>**
  * Sets captain search container (default is stack)
* **-d, --scuba-diver <queue/stack>**
  * Sets scuba diver search container (default is queue)
* **-v, --verbose**
  * Prints verbose output as program runs
* **-s, --stats**
  * Prints stats after search is completed
* **-p, --show-path <M/L>**
  * Shows path as Map or List, if path to treasure found

<!-- Testing -->
## Testing

The general way to run the program is:
```sh
./treasureHunt [options] < INPUTFILE
```

### Example 1
To set the captain to queue, diver to stack, print all output, and show the path as a map:
```sh
./treasureHunt -c queue -d stack -v -s -p M < samples/sampleSmall.map.txt
```

### Example 2
To use default search containers and redirect output to another file:
```sh
./treasureHunt -v -s -p L < samples/sampleSmall.map.txt > output.txt
```

<!-- ROADMAP -->
## Roadmap

Possible Future Improvements
* Add third algorithm for searching on land
* Save output directly to file


<!-- LICENSE -->
## License

Distributed under the MIT License.

<!-- CONTACT -->
## Contact

Prab Dhaliwal - prabd62@gmail.com

Project Link: [https://github.com/prabd/treasure_hunt](https://github.com/prabd/treasure_hunt)

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/badge/contributors-1-green.svg
[contributors-url]: https://shields.io/
[license-shield]: https://img.shields.io/github/license/othneildrew/Best-README-Template.svg?style=flat-square
[license-url]: https://github.com/prabd/treasure_hunt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=flat-square&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/prab-dhaliwal-1603531a0
