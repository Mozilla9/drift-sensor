call %cd%\svnrev %cd%\revision\revision.c -i -f13.#
xcopy %cd%\svnrev.h %cd%\revision\ /y
del %cd%\svnrev.h