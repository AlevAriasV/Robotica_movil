
#Importacion de paquetes

import numpy as np
import matplotlib.pyplot as plt
from scipy import stats



#Rellenado de informacion
data = {} #diccionario que almacenara los valores del sensor 
dist = [15,30,45,60,75] #distancias reales en cm
num = "" #cadenas de string para capturar cada distancia del sensor
list = [] #aqui se almacenas las distancias del sensor
for i in dist:

    distsensor = open(str(i)  + "cm.txt","r")
    texto = distsensor.read()
    distsensor.close

    for j in texto:

        if j != '\n':
            num = num + j
        else:
            list.append(float(num))
            num = ""

    data[i] = list

    list = []
    


# Preparar listas para todos los puntos (para la regresión)
all_x = []
all_y = []

# Listas para medias y varianzas
x_vals = sorted(data.keys())
medias = []
varianzas = []

# Calcular media y varianza para cada x
for x, y_list in data.items():
    # Convertir a array de numpy para cálculos
    y_array = np.array(y_list)
    media = np.mean(y_array)
    var = np.var(y_array)
    
    medias.append(media)
    varianzas.append(var)
    
    # Agregar todos los puntos para la regresión
    all_x.extend([x] * len(y_list))
    all_y.extend(y_list)

# Convertir a arrays para la regresión
all_x = np.array(all_x)
all_y = np.array(all_y)

# Realizar regresión lineal con todos los puntos
pendiente, intercepto, r_value, p_value, std_err = stats.linregress(all_x, all_y)
regresionlineal = pendiente * all_x + intercepto

# Crear la gráfica
plt.figure(figsize=(10, 6))

# 1. Graficar todos los puntos
plt.scatter(all_x, all_y, label='Mediciones del sensor', color='gray')

# 2. Graficar medias y varianzas para cada x
for x, mean, var in zip(x_vals, medias, varianzas):
    # Línea vertical para la media
    plt.plot([x, x], [mean - np.sqrt(var), mean + np.sqrt(var)],  
             color='red', linewidth=2, label='Desviación estándar' if x == x_vals[0] else "") 
    plt.scatter(x, mean, color='blue', zorder=5, label='Media' if x == x_vals[0] else "")

# 3. Graficar la línea de regresión
plt.plot(all_x, regresionlineal, color='green', label=f'Regresión lineal de las mediciones del sensor')

# Personalizar la gráfica
plt.xlabel('Distancias reales en cm')
plt.ylabel('Distancias medidas por el sensor en cm')
plt.legend()
plt.title('Mediciones sensor')
plt.grid(True)
plt.show()
plt.savefig("Grafica.jpg", bbox_inches='tight')