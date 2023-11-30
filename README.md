# Simple root chain

Build a ROOT chain with files from a local or given directory

```bash
source makechain.sh <DIRECTORY> > mychain.C
```

Wildcard * might not work as expected. No check done on the file type.

Run an example analysis on files containing PF candidates with

```bash
root -b -l -q mychain.C pfplots.C
```
