/* REXX script which prepares OS/2 NetHack build environment. *
 * Created by Pekka Rousu                                     */

'@echo off'

SAY 'Configuring OS/2 NetHack...'

/* extract the source tree top-level directory */

PARSE SOURCE nhpath
PARSE VAR nhpath forget forget nhpath
nhdrive = SUBSTR(nhpath,1,2)
nhpath = SUBSTR(nhpath,3,LASTPOS('\sys\os2', nhpath)-3)

/* Copy the makefile to src and define temporary directories */

nhdrive
'cd 'nhpath'\src'
'del Makefile >nul'

infile  = '..\sys\os2\Makefile.os2'
outfile = 'Makefile'
curline = LINEIN(infile)

DO WHILE LINES(infile)
  IF (SUBSTR(curline, 1, 5) == 'NHSRC') then
    rc = LINEOUT(outfile, 'NHSRC = 'nhpath)
  ELSE if (SUBSTR(curline, 1, 3) == 'OBJ') then
    rc = LINEOUT(outfile, 'OBJ = 'nhpath'\tmp')
  ELSE if (SUBSTR(curline, 1, 4) == 'TEMP') then
    rc = LINEOUT(outfile, 'TEMP = 'nhpath'\tmp')
  ELSE rc = LINEOUT(outfile, curline)

  if rc then do
    SAY 'Could not write 'outfile'. Setup failed!'
    exit 1
  end

  curline = LINEIN(infile)
END

'md 'nhpath'\tmp >nul  2>nul'
'md \games >nul  2>nul'
'md \games\slashem >nul  2>nul'
'md \games\slashem-x11 >nul 2>nul'

SAY 'Source tree is now prepared! Enter "dmake" to start the build process...'
