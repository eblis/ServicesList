for /F "tokens=5-9* delims=. " %%i in (docs\ServicesList_readme.txt) do (call :Pack %%i %%j %%k %%l; exit)

:Pack
d:\usr\PowerArchiver\pacl\pacomp.exe -a -c2 "ServicesList %1.%2.%3.%4 x32.zip" @files_release.txt
d:\usr\PowerArchiver\pacl\pacomp.exe -p -a -c2 "ServicesList %1.%2.%3.%4 x32.zip" docs\*.* *.caca
call "pack symbols.bat" ServicesList ServicesList %1.%2.%3.%4
exit

error:
echo "Error packing WhenWasIt"
