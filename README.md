# Simple root chain

Build a ROOT chain with root files in a given directory

```bash
source makechain.sh "<DIRECTORY>/*.root" > mychain.C
```

Plots some values with

```bash
root -b -l -q mychain.C pfplots.C
```

In this example, the files are expected to contain particle flow candidates.
