set -e 

TOOLCHAIN_PREFIX=$(dirname $(dirname $(which arm-none-eabi-gcc))) 
CONFIGURE_FLAGS=

export CFLAGS="-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -nostartfiles -DUSE_STDPERIPH_DRIVER" 
export LDFLAGS="" 

echo "Using toolchain prefix: ${TOOLCHAIN_PREFIX}"

[ -d libstm32 ] || git clone https://github.com/mkschreder/libstm32.git
cd libstm32

./configure --host=arm-none-eabi --prefix=${TOOLCHAIN_PREFIX}/arm-none-eabi/
make 
sudo make install
cd ..

[ -d libutype ] || git clone https://github.com/mkschreder/libutype.git
cd libutype

./configure --host=arm-none-eabi --prefix=${TOOLCHAIN_PREFIX}/arm-none-eabi/
make 
sudo make install
