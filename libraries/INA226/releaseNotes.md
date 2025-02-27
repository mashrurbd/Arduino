
# INA226 library

## Release notes

## 0.4.0   2022-08-XX
- fix #16 - change error to warning for max current
  setMaxCurrentShunt now returns an int indicating OK == 0
  otherwise error. See INA226.h for error codes.
- updated readme.md
- updated unit tests (error codes)


## 0.3.0   2022-07-01
- fix #14 RESET FLAG
- add getAddress()


## 0.2.0   2022-02-02
- fix #11 normalize
- fix #13 simplify sign handling shunt and current
- add releaseNotes.md
- **reset()** also resets the calibration (current_lsb, maxCurrent and shunt), 
thereby forcing the user to redo the calibration call with **setMaxCurrentShunt()**.
- fixes issue #11 => a factor 10 bug in current_lsb.
- some edits in readme.md.
- added **bool isCalibrated()**.


## 0.1.6   2021-12-20
- update library.json, 
- license, 
- minor edits

## 0.1.5   2021-11-05
- update build-CI, 
- add badges in readme.md
- fix address in constructor

## 0.1.4   2021-08-07  
- fix getCurrent()

## 0.1.3   2021-06-22  
- add getCurrentLSB_uA()
- improve examples
- fix for calibration

## 0.1.2   2021-06-22
- add check of parameters of several functions
- add unit tests
- add getShunt()
- add getMaxCurrent()

## 0.1.1   2021-06-21  
- improved calibration
- added functions

## 0.1.0   2021-05-18  
- initial version


