# utotparser
This tool aims at providing a way of checking the emptiness of Timed Automata (one can restrict the checking only to non-reset-test TAs, see below), as well as serving as a conversion tool from UPPAAL to tChecker syntax.
Due to the limited scope of our usage, the tool can convert a limited set of the possible TAs one can create in UPPAAL.

<h2>How to install</h2>

Once cloned, in order to get the executables, navigate inside the corresponding folder and perform the following commands:
```
mkdir build
cd build
cmake -DTCHECKER_BIN_PATH="/path/to/tChecker/bin" ..
make
```
Please be careful when performing the above commands, since the path to the tChecker executable will be retrieved by the path given during the build process.  Here, `"/path/to/tChecker/bin"` is the path to the bin folder of your actual tChecker installation (the folder containing the four tools provided by tChecker).<br>
Once the steps above have been completed, exit from the build directory by using the `cd ..` command. You can find the executables in the `executables` directory.

<h2>Using the converter</h2>

In order to start the converter tool, one can decide to simply launch the executable:
```
./converter
```
Notice that in this way, the source .xml files must be inserted in the `inputFiles` folder, while relatives outputs will be found in the `outputFiles` folder (the ones located in the utotparser directory).

Additionally, one can also specify the paths where source files are located, as well as the output destination, by passing them as command-line arguments:
```
./converter -src /path/to/source/directory -dst /path/to/destination/directory
```

Finally, different options can further be specified: <br>
The option `-jsn` can be included to output the json files obtained during the conversion:
```
./converter -jsn
```
The option `-nrt` can be included to translate only nrtTAs:
```
./converter -nrt
```

The above options can be specified in any order, e.g. one could write for example:
```
./converter -nrt -src /path/to/source/directory -jsn
```
**IMPORTANT**: altough options can be specified in any order, always keep in mind that paths for specifying input and output directories must be specified right after their respective option. Thus, something like `./converter -src /path/to/source/directory` is correct, while  `./converter -src -nrt /path/to/source/directory` will not work.

When launched, all .xml files in the chosen source directory will be converted in .tck files in the chosen output directory.

Please stick to these executions options, no side-checks are done if different command-line arguments are given.<br>
Please note that the converter will work only with `.xml` files.

<h3>Additional nrtTA design conventions</h3>

Furthermore, when designing TAs in UPPAAL, the following conventions have to be respected:
* Do not add any comment to locations or transitions.
* UPPAAL files must be saved with the .xml extension.
* In UPPAAL final states must have a color (all states with a color will be marked as final).
* Clock declarations must be written like this: `clock x, y, z;`.
* Invariants must be written like this: `x < 2 && y == 1`.
* Guards must be written like this: `x < 2 && y > 1`.

Please remember to keep blank spaces between clocks and other symbols if you then want to perform nrt condition detection.

<h2>Using the checker</h2>

In order to see if a TA's language is empty or not, you can call the checker tool by using the following command:
```
./checker
```
It is possible also in this case to specify the directory from which to take the TAs to check have to be taken. This can be done by adding the following option:
```
./checker -dst /path/to/tck_files/directory
```
In this case we still use the `-dst` option since, normally, files by the checker will be taken from the destination directory of the converter.<br>
Please note that the checker will work only with `.tck` files.

<h2>Minor things</h2>

Other little things one may be aware of:
* Since the program uses colors in the cli, if you want to see them, please use a compatible terminal.<br>
* Testing have been done on MacOS Sononma 14.0 only. Should work also on other Unix-based OSs. Not tested under Windows.
