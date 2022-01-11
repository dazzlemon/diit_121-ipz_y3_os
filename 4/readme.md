# How to compile on linux
## 1. Get mxe:
git clone https://github.com/mxe/mxe.git

## 2. Install [Build dependencies](https://mxe.cc/#requirements)

## 2. Build Qt5 for Windows
cd mxe && make qtbase

## 3. Modify path
export PATH=&ltmxe root&gt/usr/bin:$PATH

## 4. Build
sh update_make

make

# How to run on linux
sh run &lt test.txt or other filename &gt