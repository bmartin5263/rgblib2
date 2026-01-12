# rgblib esp-idf

## Conventions


## Build
When running a new terminal session, navigate to the esp-idf source folder and run `. ./export.sh`

Run app with `idf.py flash monitor`

Set a new target with `idf.py set-target esp32c6`

Refresh configuration after changing defaults with `rm sdkconfig && idf.py reconfigure`

Quit monitoring with `CTRL-T, CTRL-X`

Run `./scripts/update-cmake-sources.sh` when adding/removing files to quickly update CmakeLists.txt