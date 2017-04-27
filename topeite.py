import json
from io import BytesIO
import zipfile
import os
from os import listdir
import random

circuito = "asd"
coche = "asdasdasd"
clase = "asdasdasdasd"
with open('fichero.json') as json_data:
    d = json.load(json_data)

json_del_iz = []
json_del_de = []
json_tra_iz = []
json_tra_de = []

json_acelerador = []
json_direccion = []
json_fuerzas_g = []

json_posicion = []

first = True
cont = 1
tiempo_ant = 0.0
tiempo = 0.0
for linea in d:

    # Para comprobar si es la primera linea
    if len(linea) == 12:
      #revisar
      if (linea[10] < tiempo_ant):
         tiempo = tiempo_ant + linea[10]
      else:
         tiempo = linea[10] 
      
      if first:
         km_iniciales = linea[11]
         first = False 
      

      cont += 1

      if (cont == len(d)):
        km_final = linea[11]
        distancia_recorrida = km_final - km_iniciales
        tiempo_final = tiempo 
         
      #Tiempo, medida, latitud, longitud
      json_del_iz.append([tiempo, linea[2], linea[0], linea[1], linea[11]])
      json_del_de.append([tiempo, linea[3], linea[0], linea[1], linea[11]])
      json_tra_iz.append([tiempo, linea[4], linea[0], linea[1], linea[11]])
      json_tra_de.append([tiempo, linea[5], linea[0], linea[1], linea[11]]) 

      json_acelerador.append([tiempo, linea[7], linea[0], linea[1], linea[11]]);
      json_direccion.append([tiempo, linea[6], linea[0], linea[1], linea[11]]);

      json_fuerzas_g.append([linea[8], linea[9] ,  linea[0], linea[1], tiempo, linea[11]]); 

      # Trazada
      json_ruedas_trazada = {}
      json_ruedas_trazada["data"] = {}
      json_ruedas_trazada["data"]["DI"] = linea[2]
      json_ruedas_trazada["data"]["DD"] = linea[3]
      json_ruedas_trazada["data"]["TI"] = linea[4]
      json_ruedas_trazada["data"]["TD"] = linea[5]
      json_ruedas_trazada["label"] = "ruedas"

      json_acelerador_trazada = {}
      json_acelerador_trazada["data"] = {}
      json_acelerador_trazada["data"]["1"] = linea[7]
      json_acelerador_trazada["data"]["2"] = linea[7]
      json_acelerador_trazada["data"]["3"] = linea[7]
      json_acelerador_trazada["data"]["4"] = linea[7]

      json_direccion_trazada = {}
      json_direccion_trazada["data"] = {}
      json_direccion_trazada["data"]["Direccion"] = linea[6]
      json_direccion_trazada["label"] = "volante"

      json_fuerzas_trazada = {}
      json_fuerzas_trazada["data"] = {}
      json_fuerzas_trazada["data"]["x"] = linea[8]
      json_fuerzas_trazada["data"]["y"] = linea[9]
      json_fuerzas_trazada["label"] = "fuerzas"

      json_posicion.append([linea[0], linea[1], tiempo, linea[11], json_ruedas_trazada, json_direccion_trazada, json_fuerzas_trazada])

      tiempo_ant = tiempo 
    else:
      circuito = linea[0]
      coche = linea[1]
      clase = linea[2]


# JSON de las ruedas
fjson_ruedas = {}

data_di = {}
data_di["data"] = json_del_iz
data_di["label"] = "DI"
fjson_ruedas["Del Izquierdo"] = data_di


data_dd = {}
data_dd["data"] = json_del_de
data_dd["label"] = "DD"
fjson_ruedas["Del Derecho"] = data_dd

data_ti = {}
data_ti["data"] = json_tra_iz
data_ti["label"] = "TI"
fjson_ruedas["Tras Izquierdo"] = data_ti

data_td = {}
data_td["data"] = json_tra_de
data_td["label"] = "TD"
fjson_ruedas["Tras Derecho"] = data_td


# JSON del acelerador

fjson_acelerador = {}

data_acelerador = {}
data_acelerador["data"] = json_acelerador
data_acelerador["label"] = "1"
fjson_acelerador["1"] = data_acelerador

data_acelerador = {}
data_acelerador["data"] = json_acelerador
data_acelerador["label"] = "2"
fjson_acelerador["2"] = data_acelerador

data_acelerador = {}
data_acelerador["data"] = json_acelerador
data_acelerador["label"] = "3"
fjson_acelerador["3"] = data_acelerador

data_acelerador = {}
data_acelerador["data"] = json_acelerador
data_acelerador["label"] = "4"
fjson_acelerador["4"] = data_acelerador

# JSON de la direccion
fjson_direccion = {}

data_direccion = {}
data_direccion["data"] = json_direccion
data_direccion["label"] = "Direccion"
fjson_direccion["Direccion"] = data_direccion


# JSON de las fuerzas

fjson_fuerzas = {}

data_fuerzas = {}
data_fuerzas["data"] = json_fuerzas_g
data_fuerzas["label"] = "Fuerzas G"
fjson_fuerzas["Fuerzas"] = data_fuerzas

#Crear info.json

fjson_info = {}

millis = tiempo_final
seconds=(millis/1000)%60
seconds = int(seconds)
minutes=(millis/(1000*60))%60
minutes = int(minutes)
hours=(millis/(1000*60*60))%24

str_tiempo = "%dm %ds" % (minutes, seconds)
fjson_info["tiempo"] = str_tiempo

import time
fjson_info["fecha"] = time.strftime("%d-%m-%Y")
fjson_info["descripcion"] = "Prueba realizada en " + circuito + " usando un " + coche + "(" + clase + ")"
fjson_info["nombre"] = circuito + "-pCars"
fjson_info["metros"] = distancia_recorrida * 1000

id_sesion = random.randint(1000, 10000)

while (True):

  if not os.path.exists(str(id_sesion) + ".zip"):
    print (str(id_sesion) + ".zip")
    fjson_info["id"] = id_sesion
    break;
  else:
    id_sesion = random.randint(1000, 10000)


# Guardamos los ficheros
with open('ruedas.json', 'w') as outfile:
        json.dump(fjson_ruedas, outfile)

with open('acelerador.json', 'w') as outfile:
        json.dump(fjson_acelerador, outfile)

with open('volante.json', 'w') as outfile:
        json.dump(fjson_direccion, outfile)

with open('fuerzas.json', 'w') as outfile:
        json.dump(fjson_fuerzas, outfile)

with open('trazada.json', 'w') as outfile:
        json.dump(json_posicion, outfile)

with open('info.json', 'w') as outfile:
        json.dump(fjson_info, outfile)

dirname = "D:\idcma\Documents\GitHub\JustTelemetry-projectCars"
with zipfile.ZipFile(str(fjson_info["id"]) + ".zip", 'w') as zf:
    zf.write(os.path.join(dirname, "volante.json"), "volante.json")
    zf.write(os.path.join(dirname, "acelerador.json"), "acelerador.json")
    zf.write(os.path.join(dirname, "fuerzas.json"), "fuerzas.json")
    zf.write(os.path.join(dirname, "ruedas.json"), "ruedas.json")
    zf.write(os.path.join(dirname, "trazada.json"), "trazada.json")
    zf.write(os.path.join(dirname, "info.json"), "info.json")