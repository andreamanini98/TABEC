# utotparser
This tool aims at providing a way of checking the emptiness of non-reset-test Timed Automata, as well as serving as a conversion tool from UPPAAL to tChecker syntax.
Due to the limited scope of our usage, the tool can convert a limited set of the possible TAs one can create in UPPAAL.

Furthermore, when designing TAs in UPPAAL, the following conventions has to be respected:
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

Additionally, one can also specify the paths where source files are located, as well as the output destination, by passing them as command-line argments:
```
./utotparser /path/to/source/directory /path/to/destination/directory
```

Finally, different options can further be specified: <br>
The option `-j` can be included to output the json files obtained during the conversion:
```
./utotparser /path/to/source/directory /path/to/destination/directory -j
```
The option `-nrt` can be included to translate only nrtTAs:
```
./utotparser /path/to/source/directory /path/to/destination/directory -nrt
```

Please note that options can be used even when custom paths are specified:
```
./utotparser -j
```
```
./utotparser -nrt
```


When launched, all .xml files in the chosen source directory will be converted in .tck files in the chosen output directory.

Please stick to these executions options, no side-checks are done if different command-line arguments are given (in type or number!).
Parameters are not yet supported, so they will be translated "as-is", but they're not accepted in tChecker's syntax.
