# MiniRegressor

A small C++17 linear regression program that trains on `houses.csv`, plots its
training progress with Matplot++ and gnuplot, then predicts a house price from
feature values entered in the terminal.

## Requirements

- CMake 3.15 or newer
- A C++17 compiler and a build tool supported by CMake
- gnuplot 5.2.6 or newer, available on `PATH`
- Git, so CMake can download Matplot++ on the first configure

## Build and run

Run these commands from the project directory.

### Windows (MSYS2 UCRT64)

With MSYS2 UCRT64, CMake, and gnuplot installed at their default locations,
run these commands in PowerShell from the project directory:

```powershell
$env:Path = 'C:\msys64\ucrt64\bin;C:\Program Files\gnuplot\bin;' + $env:Path
& 'C:\Program Files\CMake\bin\cmake.exe' -S . -B build -G Ninja `
  -DCMAKE_MAKE_PROGRAM=C:/msys64/ucrt64/bin/ninja.exe `
  -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe `
  -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe
& 'C:\Program Files\CMake\bin\cmake.exe' --build build --config Release
Set-Location build
.\main.exe
```

### Linux or macOS

After installing CMake, a C++17 compiler, Git, and gnuplot:

```sh
cmake -S . -B build
cmake --build build
cd build
./main
```

## Running the Program

When you run `.\main.exe`, it asks:

```text
Select loss (1: MSE, 2: MAE) [1]: 
Select optimizer (1: SGD, 2: Batch) [1]: 
Enter learning rate [0.001]: 
```

- Press <kbd>Enter</kbd> at each prompt to accept the defaults (`MSE`, `SGD`, `0.001`).
- Or type `2` to pick `MAE` / `Batch`, or enter a custom number for learning rate.

---

The first CMake configure downloads Matplot++ and requires Git and an internet
connection. The build copies `houses.csv` into the build directory. When training finishes, press <kbd>Enter</kbd> in your terminal to view the metrics and enter feature values at the prompts. Type `q` to quit.
