# mkfat
A basic MS-DOS disk image formatter

Currently supports FAT16 and FAT32

Yes, I know it's badly made

Usage:
./mkfat <12,16,32> <outputfile> [-Srootdir] [-Bbootfile] [-Rreservedfile] [-Vvolumelabel]

Options
 - '-S' source/root directory
 - '-B' custom bootsector file
 - '-R' custom reserved area data
 - '-V' set volume label
