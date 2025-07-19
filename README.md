# Previsão de Tráfego com Redes Bayesianas

-Precisa do eigen3(para rodar a biblioteca bayonet):
 `sudo apt install libeigen3-dev`


-Comandos para compilar:
 `git clone --recursive https://github.com/RianAbdias/Previsao_Trafego.git`
 `cd Previsao_Trafego`
 `mkdir build && cd build`
 `cmake ..`
 `make`
 `./traffic`


-Caso não tenha baixado o submódulo(biblioteca usada) use :
 `git submodule update --init --recursive`
