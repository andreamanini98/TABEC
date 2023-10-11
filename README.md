# utotparser
This tool aims at providing a way of checking the emptiness of non-reset-test Timed Automata, as well as serving as a conversion tool from UPPAAL to tChecker syntax.
Due to the limited scope of our usage, the tool can convert a limited set of the possible TAs one can create in UPPAAL.

Furthermore, when designing TAs in UPPAAL, the following conventions have to be respected:
* Do not add any comment to locations or transitions
* UPPAAL files must be saved with the .xml extension
* In UPPAAL final states must have a color (all states with a color will be marked as final)
* Clock declarations must be written like this: `clock x, y, z;`
* Invariants must be written like this: `x < 2 && y == 1`
* Guards must be written like this: `x < 2 && y > 1`

Please remember to keep blank spaces between clocks and other symbols if you then want to perform nrt condition detection.

Once cloned, in order to get the executable, navigate inside the corresponding folder and perform the following commands:
```
mkdir build
cd build
cmake -DTCHECKER_BIN_PATH="/path/to/tChecker/bin" ..
make
```
Please be careful when performing the above commands, since the path to the tChecker executabe will be retrieved by the path given during the build process.  Here, `"/path/to/tChecker/bin"` is the path to the bin folder of your actual tChecker installation (the folder containing the four tools provided by tChecker).

In order to start the tool, one can decide to simply launch the executable:
```
./utotparser
```
Notice that in this way, the source .xml files must be inserted in the `inputFiles` folder, while relatives outputs will be found in the `outputFiles` folder (the ones located in the utotparser directory).

Additionally, one can also specify the paths where source files are located, as well as the output destination, by passing them as command-line arguments:
```
./utotparser -src /path/to/source/directory -dst /path/to/destination/directory
```

Finally, different options can further be specified: <br>
The option `-jsn` can be included to output the json files obtained during the conversion:
```
./utotparser -jsn
```
The option `-nrt` can be included to translate only nrtTAs:
```
./utotparser -nrt
```

The above options can be specified in any order, e.g. one could write for example:
```
./utotparser -nrt -src /path/to/source/directory -jsn
```
**IMPORTANT**: altough options can be specified in any order, always keep in mind that paths for specifying input and output directories must be specified right after their respective option. Thus, something like `./utotparser -src /path/to/source/directory` is correct, while  `./utotparser -src -nrt /path/to/source/directory` will not work.

When launched, all .xml files in the chosen source directory will be converted in .tck files in the chosen output directory.

Please stick to these executions options, no side-checks are done if different command-line arguments are given.
