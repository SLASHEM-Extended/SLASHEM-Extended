@echo off
REM This file generates the .CAB files and packages them 
REM using ezsetup into an installer
REM
echo Generating CAB install files for ARM, MIPS, SH3
"f:\Windows CE Tools\wce300\MS Pocket PC\support\ActiveSync\windows ce application installation\cabwiz\cabwiz" slashem.inf /err makecab.err /cpu arm mips sa sh3
ezsetup -l english -i slashem.ini -r ..\slashem.nfo  -e ..\Readme.txt -o slashemCE_install-%1.exe