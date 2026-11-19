@echo off
"C:\Program Files (x86)\Dev-Cpp\MinGW64\bin\gcc.exe" -std=c11 -Wall -Wextra -pedantic main.c veiculos.c temporario.c -o estacionamento.exe && estacionamento.exe