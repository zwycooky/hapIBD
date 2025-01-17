# hapIBD
 An NGS tool of haplotype based IBD detection
# Installation
``
git clone https://github.com/zwycooky/hapIBD.git
cd hapIBD
make
``
# Usage
``
Usage:
  ./hapIBD -OPTIONS VALUES

options:
--input options
    -i FILE         bam file
    -p FILE         hap file

--output options
    -o STR          outputprefix

--criteria options
    -w INT          window length (bp) [default: 1000000]
    -s INT          window step (bp)   [default: 1000000]
    -N INT          minimum read numbers of hap block [default: 12]
    -n INT          minimum phased numbers of phased snps for reads [default: 6]

--performance options
    -a INT          cpus cores used for the analysis [default: 10]
``
hap file can be obtained by whatshap or other SNP phasing software.
