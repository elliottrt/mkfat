# mkfat
A basic MS-DOS disk image formatter

Supports FAT12, FAT16, and FAT32

Yes, I know it's badly made

Usage:
`./mkfat <12,16,32> <outputfile> [-Srootdir] [-Bbootfile] [-Rreservedfile] [-Vvolumelabel] [-v|--verbose]`

 - `-S` -- set source/root directory
 - `-B` -- set custom bootsector file
 - `-R` -- set custom reserved area data
 - `-V` -- set volume label
 - `-v` `--verbose` --  print verbose output
