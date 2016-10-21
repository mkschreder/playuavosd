set -e 

PREFIX=$PWD/staging_dir
CONFIGURE_FLAGS=

export CFLAGS="-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -nostartfiles -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -ffunction-sections" 
export LDFLAGS="" 

[ -d libstm32 ] || git clone https://github.com/mkschreder/libstm32.git
cd libstm32

./configure --host=arm-none-eabi --prefix=${PREFIX}
make 
make install
cd ..

[ -d libutype ] || git clone https://github.com/mkschreder/libutype.git
cd libutype

./configure --host=arm-none-eabi --prefix=${PREFIX}
make 
make install
