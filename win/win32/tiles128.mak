default: all

all: ..\binary\tiles128.bmp

clean:
	-del ..\win\share\mon128.txt
	-del ..\win\share\obj128.txt
	-del ..\win\share\oth128.txt

#==========================================================
# Building the tiles file tile.bmp, tile32.bmp, tile128.bmp
#==========================================================

..\src\tiles128.bmp : ..\win\share\mon128.txt ..\win\share\obj128.txt \
			 ..\win\share\oth128.txt ..\util\tile2bmp.exe
	   chdir ..\src
	   ..\util\tile2bmp.exe tiles128.bmp ..\win\share\mon128.txt ..\win\share\obj128.txt \
			 ..\win\share\oth128.txt 
	   chdir ..\build

..\binary\tiles128.bmp: ..\src\tiles128.bmp
	@copy ..\src\tiles128.bmp ..\binary\tiles128.bmp

..\win\share\mon128.txt:	..\util\txtbg.exe ..\win\share\absurd\mon128ab.txt
	   ..\util\txtbg ..\win\share\absurd\mon128ab.txt ..\win\share\mon128.txt

..\win\share\obj128.txt:	..\util\txtbg.exe ..\win\share\absurd\obj128ab.txt
	   ..\util\txtbg ..\win\share\absurd\obj128ab.txt ..\win\share\obj128.txt

..\win\share\oth128.txt:	..\util\txtbg.exe ..\win\share\absurd\oth128ab.txt
	   ..\util\txtbg ..\win\share\absurd\oth128ab.txt ..\win\share\oth128.txt


