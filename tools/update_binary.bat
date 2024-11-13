@echo off
copy .\build\PathFinding.exe .\bin\
for /f "skip=1 tokens=*" %%A in ('Certutil -hashfile .\bin\PathFinding.exe MD5') do (
    echo %%A > .\bin\md5sum.txt
    goto :EOF
)