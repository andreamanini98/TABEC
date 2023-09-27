# utotparser
This tool aims at providing a conversion from UPPAAL to tChecker syntax.
Due to the limited scope of our usage, the tool can convert a limited set of the possible TAs one can create in UPPAAL.

Furthermore, when designing TAs in UPPAAL, the following conventions has to be respected:
* UPPAAL files must be saved with the .xml extension
* In UPPAAL final states must have a color (all states with a color will be marked as final)
* Clock declarations must be written like this: `clock x, y, z;`
* Invariants must be written like this: `x<2&&y==1`
* Guards must be written like this: `x<2&&y>1`

In order to start the tool, one can decide to simply launch the executable:
```
./utotparser
```
Notice that in this way, the source .xml files must be inserted in the `inputFiles` folder, while relatives outputs will be found in the `outputFiles` folder (the ones located in the utotparser directory).

Additionally, one can also specify the paths where source files are located, as well as the output destination, by passing them as command-line argments:
```
./utotparser /path/to/source/directory /path/to/destination/directory
```

Finally, the option `-j` can also be included to output the json files obtained during the conversion:
```
./utotparser /path/to/source/directory /path/to/destination/directory -j
```

When launched, all .xml files in the chosen source directory will be converted in .tck files in the chosen output directory.

Please stick to these executions options, no side-checks are done if different command-line arguments are given (in type or number!).
