call %cd%\svnrev %cd%\revision\revision.c -i -f1.#
xcopy %cd%\svnrev.h %cd%\revision\ /y
del %cd%\svnrev.h