# hapIBD
 An NGS tool of haplotype based IBD detection
# Installation
```
git clone https://github.com/zwycooky/hapIBD.git
cd hapIBD && make
```
# Usage
```
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
```
Notice: hap file can be obtained by whatshap or other SNP phasing software.
## Format of input file
Example of 'haplotype.txt' file:
```
chr1    36142   A       C       36142
chr1    36243   T       C       36142
chr1    36315   A       G       36142
chr1    36316   A       T       36142
chr1    36333   C       A       36142
chr1    36335   C       G       36142
chr1    78839   A       G       78839
chr1    78884   C       T       78839
chr1    126320  G       T       126320
chr1    126404  T       C       126320
chr1    126432  C       A       126320
chr1    139327  C       T       139327
chr1    139365  T       C       139327
chr1    255193  G       A       255193
chr1    255936  C       T       255936
```
chr1    256020  T       A       255936
