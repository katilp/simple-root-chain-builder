# Simple root chain

Build a ROOT chain with root files in a given directory

```bash
source makechain.sh "<DIRECTORY>/*.root" > mychain.C
```

Run an example analysis on files containing PF candidates with

```bash
root -b -l -q mychain.C pfplots.C
```
