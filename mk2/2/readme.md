# How to compile on linux
## 1. Get mxe:
git clone https://github.com/mxe/mxe.git

## 2. Install [Build dependencies](https://mxe.cc/#requirements)

## 2. Build Qt5 for Windows
cd mxe && make qtbase

## 3. Modify path
export PATH=<mxe root>/usr/bin:$PATH

## 4. Build
sh update_make
make

# How to run on linux
sh run <test.txt or other filename>