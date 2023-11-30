# Adds all files in the input directory to the chain
# If no input is given, adds everything in the local directory
# NB that wildcard * might not work as expected in the input
echo {
echo "    TChain *chain=new TChain(\"Events\");"
files=$(ls $1)
for f in $files; do echo "    chain->Add(\"$f\");"; done
echo }
