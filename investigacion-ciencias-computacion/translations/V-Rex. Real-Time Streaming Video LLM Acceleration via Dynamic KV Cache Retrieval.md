## V-Rex: Aceleración LLM de transmisión de video en tiempo real mediante recuperación dinámica de caché KV

Donghyuk Kim, Sejeong Yang, Wonjin Shin y Joo-Young Kim KAIST

Daejeon, República de Corea

{ kar02040, 02yangsj, 2swj1202, jooyoung1203 } @kaist.ac.kr

Resumen -Los modelos de lenguaje grande (LLM) de transmisión de video son cada vez más
Se utiliza para tareas multimodales en tiempo real, como subtítulos de vídeo, preguntas.
contestador automático, agentes conversacionales y realidad aumentada. Sin embargo, estos
Los modelos enfrentan desafíos fundamentales de memoria y computación porque
sus cachés de valor clave (KV) crecen sustancialmente con la transmisión continua
entrada de vídeo. Este proceso requiere una etapa iterativa de prellenado, que es
una característica única de los LLM de transmisión de video. Obras anteriores reducen el exceso
sobrecarga de caché mediante la utilización del algoritmo de recuperación de caché KV, que
descarga el caché KV completo a la memoria o almacenamiento de la CPU, luego selectivamente
busca las entradas más relevantes. Sin embargo, debido a su iteración
etapa de precarga, sufren de limitaciones significativas, incluyendo
cálculo extenso, transferencia sustancial de datos y degradación en
precisión. Fundamentalmente, este problema se agrava en el caso de la implementación perimetral,
que es el objetivo principal de estos modelos. La huella de la memoria
excede la capacidad de la memoria a los pocos minutos de las transmisiones de video, lo que hace que
latencia, inferencia energéticamente eficiente inviable.

En este trabajo proponemos V-Rex, el primer software-hardware codiseñado
acelerador que aborda de manera integral tanto algorítmica como hardware
Cuellos de botella en la inferencia LLM de transmisión de video. En esencia, V-Rex
presenta ReSV, un algoritmo de recuperación de caché KV dinámico sin entrenamiento.
ReSV explota la agrupación de tokens basada en similitudes temporales y espaciales para
reducir el exceso de memoria caché KV en fotogramas de vídeo y dinámicamente
ajusta la selección de tokens por capa de transformador y presta atención a
Minimizar el número de tokens seleccionados. Para realizar plenamente estos
Beneficios algorítmicos, V-Rex ofrece un hardware compacto y de baja latencia.
acelerador con un motor dinámico de recuperación de caché KV (DRE), que incluye
unidades informáticas basadas en nivel de bits y salida temprana, así como jerárquicas
Gestión de memoria caché KV. Evaluado según los puntos de referencia COIN, V-Rex logra
tiempo real sin precedentes de 3,9-8,3 FPS y transmisión con bajo consumo de energía
Inferencia de video LLM sobre la implementación de borde con una pérdida de precisión insignificante.
Mientras que DRE sólo representa el 2,2% de la energía y el 2,0% del área, el sistema
ofrece una aceleración de 1,9-19,7 × y una eficiencia energética de 3,1-18,5 ×
mejoras sobre AGX Orin GPU. Este trabajo es el primero en
abordar de manera integral la recuperación de caché KV a través de algoritmos y hardware,
permitiendo la inferencia LLM de transmisión de video en tiempo real en recursos limitados
dispositivos de borde, con un claro potencial para una implementación escalable en grandes
escalar entornos de servidores.

## I. INTRODUCCIÓN

Recientemente, la demanda de inteligencia artificial que pueda comprender y
interpretar diversas modalidades (es decir, texto, imagen, vídeo y voz) y
responder a consultas ha sido una fuerza impulsora en el aprendizaje automático
investigación. Como resultado, han surgido grandes modelos multimodales (LMM) [42].
como soluciones prometedoras en diversas industrias de IA. En particular, la transmisión
Los modelos de lenguaje grande en video (LLM) han ganado una atención significativa para
su capacidad para comprender conjuntamente las modalidades de vídeo y texto en
en tiempo real. Los LLM de transmisión de video demuestran una amplia gama de tareas,
incluyendo subtítulos de vídeo, respuesta a preguntas, agentes conversacionales,
y realidad aumentada. [4], [32], [38]. A diferencia de los LLM de vídeo sin conexión [29],[33], [45], procesa transmisiones de vídeo en tiempo real y responde a las solicitudes de los usuarios.
consultas, que se ejecuta principalmente en dispositivos perimetrales. Debido al video continuo
entrada que requiere un proceso iterativo de fotogramas de vídeo, cálculo y
el uso de la memoria aumenta sustancialmente. Hace que las cachés de valores clave (KV)
excede rápidamente la capacidad de memoria de la GPU y procesa videos largos
Las transmisiones en tiempo real se vuelven poco prácticas.

Las optimizaciones de caché KV existentes no coinciden fundamentalmente para
streaming y cargas de trabajo interactivas. Métodos destructivos, como
poda [37], compresión [12], [19], [36] y cuantificación [13], [17],
[21], [34] corren el riesgo de descartar permanentemente información que, si bien
irrelevante para la consulta actual, puede ser esencial para consultas futuras,
alterar la continuidad conversacional. Un enfoque más prometedor, KV
La recuperación de caché [6], [16], [24] evita este problema al descargar el
caché completo en la memoria o almacenamiento de la CPU y obteniendo solo tokens relevantes,
reduciendo así el uso de memoria de la GPU y manteniendo respuestas coherentes
para secuencias de entrada más extendidas. Aunque es eficaz para reducir la memoria.
Para su uso, dependen de enlaces con ancho de banda limitado, como PCIe (4-32 GB/s),
que son mucho más lentos que el ancho de banda de la memoria de la GPU (1-2 TB/s). Así,
La recuperación selectiva es necesaria para evitar una transferencia de datos grave.

Sin embargo, los algoritmos de recuperación actuales, diseñados para la generación de texto
etapa, funciona mal en la etapa de precarga iterativa de transmisión
vídeo. Además, su dependencia de la selección top-k fija, que es una
primitivo computacionalmente regular y compatible con GPU, presenta
ineficiencias algorítmicas. Esta estrategia estática ignora los factores altamente
importancia variable de los tokens en las capas del transformador y la atención
cabezas [7], [36], [41]. La aplicación de una política de k fijo prioriza el hardware
conveniencia sobre la necesidad del algoritmo, lo que lleva a problemas sistémicos.
Ineficiencias: ya sea buscar demasiado tokens redundantes o desperdiciar PCIe
ancho de banda y energía, o subestimar los críticos, degradando
precisión. Abordar este desafío requiere más que un algoritmo
retocar. Exige una nueva optimización del hardware.

Presentamos V-Rex, el primer acelerador LLM de video en streaming diseñado para
abordar la gran memoria y los requisitos computacionales del caché KV.
Encarna este principio de codiseño de software y hardware a través de la
innovaciones estrechamente integradas, como se muestra en la Figura 1. En el software
nivel, proponemos ReSV un algoritmo de recuperación de caché KV sin entrenamiento que
percibe y explota de forma inteligente las características únicas del vídeo
datos. Reduce significativamente la cantidad de tokens recuperados para el
etapa iterativa de precarga. La agrupación de claves hash-bit de ReSV proporciona una
Mecanismo computacionalmente liviano para identificar y agrupar tokens con
alta similitud espacio-temporal, reduciendo drásticamente la redundancia
sin cálculos costosos. Sobre esta base, su acumulado ponderado
El umbral de suma (WiCSum) actúa como un mecanismo adaptativo, dinámicamente
seleccionar los tokens más críticos de forma detallada, por capas y
cabeza a cabeza, moviéndose mucho más allá de las rígidas limitaciones del top-k fijo.
A nivel de hardware, presentamos la recuperación dinámica de caché KV.
motor (DRE), un acelerador compacto que sirve como habilitador esencial
para ReSV. La unidad de predicción de caché KV (KVPU) de DRE es específicamente
diseñado para ejecutar procesos detallados, dependientes de datos y condicionales.
operaciones de ReSV, como agrupación a nivel de bits y umbralización con
clasificación de salida temprana, que causaría una severa desaceleración ysubutilización en una GPU. Además, la unidad de gestión de caché KV
(KVMU) de DRE complementa esto administrando el ancho de banda PCIe de manera eficiente
y reducir el movimiento general de datos durante la recuperación. Al descargar estos
tareas irregulares a una unidad especializada, V-Rex permite que el motor LLM principal
para operar con la máxima eficiencia.

Fig. 1. Descripción general del acelerador V-Rex

<!-- imagen -->

Las contribuciones clave de este trabajo son las siguientes:

- Proponemos V-Rex, el primer acelerador codiseñado por software y hardware que aborda fundamentalmente la gran memoria y el cuello de botella computacional del caché KV resultante de la etapa de precarga iterativa en los LLM de video en streaming.
- Presentamos ReSV, un algoritmo de recuperación novedoso y sin entrenamiento diseñado para LLM de transmisión de video que aprovecha la agrupación de caché de similitud espacial-temporal y la selección dinámica de caché que reduce el tráfico de caché KV con una pérdida de precisión insignificante.
- Desarrollamos DRE, una unidad de hardware eficiente que acelera las operaciones irregulares de ReSV, haciendo práctica la recuperación inteligente y detallada en plataformas con recursos limitados, consumiendo solo el 2,0% del área total del chip. Se puede integrar con cualquier GPU, NPU y acelerador LLM existentes gracias a su alta adaptabilidad.
- Demostramos a través de una evaluación integral que

Fig. 2. Descripción general de Streaming Video LLM

<!-- imagen -->

V-Rex permite la inferencia en tiempo real (3,9-8,3 FPS) en dispositivos periféricos,
logrando hasta 19,7 × aceleración y 18,5 × ahorro de energía en un estado.
Línea base de GPU de última generación.

##II. ANTECEDENTES Y MOTIVACIONES

## A. Flujo de trabajo y arquitectura de LLM de transmisión de video

La Figura 2 presenta una descripción general del LLM de vídeo en streaming. A diferencia de fuera de línea
modelos, procesa la entrada de vídeo en tiempo real y genera
narración en respuesta directa a las consultas de los usuarios. Los usuarios pueden emitir una serie
de consultas, incluyendo seguimientos que dependen tanto de anteriores
interacciones y el contexto de vídeo en evolución. En consecuencia, la información
de segmentos de video anteriores es vital para producir contenido consciente del contexto.
respuestas a futuras consultas. Esta necesidad operativa subraya la
importancia de los algoritmos avanzados de gestión de caché KV que preservan y
utilizar el contexto visual previo, en lugar de confiar en los convencionales.
métodos de optimización (es decir, poda, fusión y cuantificación) [12],
[13], [17], [19], [21], [34], [36], [37] que pueden descartar información
esencial para interacciones posteriores.

La Figura 3 muestra la arquitectura modelo de streaming de vídeo LLM. un
streaming de video LLM consta de tres módulos principales: una torre de visión, una
Proyector MLP y un LLM. La torre de visión (por ejemplo, CLIP [27], SigLIP
[44]) transforma cada fotograma de vídeo en incrustaciones numéricas que capturan
características espaciales y temporales. El proyector MLP adapta la dimensión de
estas incorporaciones, lo que permite una integración perfecta con la entrada de LLM
espacio. El LLM procesa información visual y consultas de los usuarios, generando
fichas de salida. Para el LLM, modelos como Llama-3 [8] y Qwen3 [40]
se utilizan a menudo.

El LLM de video en streaming primero realiza una etapa de prellenado iterativo que
procesa secuencialmente entradas de video y tokens de preguntas, un distintivo
Mecanismo exclusivo para manejar transmisiones de video en tiempo real. Desde que se tomaron muestras
Los fotogramas de una transmisión de vídeo en tiempo real llegan secuencialmente y no se pueden
agrupados en lotes, cada cuadro se procesa individualmente a través de un
cálculo previo repetido de LLM. Cada etapa de precarga asiste a la anterior.
Caché KV para el cálculo de autoatención y genera caché KVentradas que se acumulan incrementalmente. Este caché KV crece linealmente
con el tiempo, siguiendo una complejidad O ( N 2 T ), donde N 2 representa la
resolución espacial y T denota duración temporal. En particular, este caché
facilita el procesamiento de fotogramas futuros y es crucial para
generar respuestas precisas a las preguntas de los usuarios, ya que las consultas pueden
información visual de referencia que abarca múltiples fotogramas. Cuando el usuario
ingresa una consulta, la pregunta del usuario se tokeniza y se procesa únicamente
a través del LLM. En la etapa de generación, genera una producción basada en
tanto los cachés KV de cuadros acumulados como los tokens de preguntas, por lo tanto
manteniendo la coherencia contextual.

Fig. 3. Arquitectura modelo de Streaming Video LLM

<!-- imagen -->

## B. Principios de recuperación de caché KV

La Figura 4 (a) muestra la sobrecarga de la caché KV de VideoLLM-Online [4]
cuando se utiliza Llama-3 8B como modelo principal. La caché KV aumenta con
duración del vídeo y supera la capacidad de memoria de la GPU en cuestión de minutos, lo que hace
la implementación perimetral no es práctica. Investigaciones anteriores intentan reducir la caché KV
tamaño mediante poda y fusión, pero se queda corto para la transmisión de video
LLM en entornos de múltiples turnos. Descartar segmentos del caché da como resultado
respuestas inexactas a consultas secuenciales de los usuarios. Por el contrario, la caché KV
La recuperación preserva toda la información anterior y permite la selección.
cálculo, reduciendo así los requisitos de memoria y preservando al mismo tiempo el modelo.
precisión. Esto se logra a través de un proceso de tres etapas durante
inferencia. (1) Descarga: primero se transfiere todo el caché KV a
Memoria o almacenamiento de la CPU. (2) Selección: solo se seleccionan tokens relevantes
para la consulta. (3) Búsqueda previa: estas entradas KV seleccionadas se recuperan
a la memoria de la GPU por adelantado para el cálculo de atención. este diseño
Garantiza tres resultados esenciales: 1) Mantiene la integridad contextual.
en consultas de varios turnos, 2) minimiza los requisitos de memoria de la GPU y
3) reduce el cálculo al limitar el procesamiento a lo más relevante
subconjunto de la caché por consulta. Por lo tanto, la recuperación de caché KV ofrece tanto
escalabilidad y coherencia para LLM de transmisión de video en tiempo real.

##III. DESAFÍOS DE LA RECUPERACIÓN DE CACHÉ KV

## A. Por qué las recuperaciones de KV se quedan cortas en los LLM de vídeo en streaming

Aplicación de técnicas de recuperación de caché KV existentes a los LLM de transmisión de video
plantea limitaciones fundamentales que no han sido abordadas en anteriores
Funciona. Por ejemplo, InfiniGen [16] es un algoritmo representativo que
Oculta efectivamente la latencia de recuperación durante la etapa de generación del LLM.
Sin embargo, en escenarios LLM de video en tiempo real, esta ventaja tiene
impacto mínimo porque tales sistemas están dominados por un proceso iterativo
etapa de precarga, que utiliza cachés KV, impulsadas por entradas continuas
marcos y consultas de varios turnos. InfiniGen y otros enfoques similares
operan exclusivamente durante la generación y por lo tanto no abordan la
cuello de botella predominante durante el prellenado, donde la mayor parte del caché KV
se produce la recuperación y la generación.

Fig. 4. (a) Huella de memoria de Streaming Video LLM en una configuración de 10 FPS en el lote 4. (b) Desglose de latencia de extremo a extremo de Streaming Video LLM. (c) Sobrecarga de latencia de recuperación de KV del método de recuperación SOTA [16] en la etapa de prellenado con una longitud de secuencia de caché de 40 KV.

<!-- imagen -->

Analizamos el desglose de la latencia de extremo a extremo del streaming de vídeo LLM
usando InfiniGen en una GPU NVIDIA A100 modelando el trabajo promedio
escenario en el punto de referencia COIN (es decir, 26 fotogramas, 25 fichas de preguntas y
39 tokens de respuesta), asumiendo una longitud específica de la secuencia de caché KVha sido precalculado y se mantiene activamente, como se muestra en la Figura 4
(b). Los resultados revelan que a medida que aumenta la duración del vídeo, el número de
Los tokens de caché KV acumulados crecen rápidamente, lo que hace que el precarga sea el más grande.
colaborador. Con una longitud de secuencia de caché de 80 K, se toma el 83 % de la latencia.
por la etapa de precarga y el 74% de esta latencia de precarga es tomada por el KV
recuperación de caché, lo que confirma que es el verdadero cuello de botella. Desde el anterior
método de recuperación sólo optimiza la etapa de generación, fundamentalmente
no logra abordar los cuellos de botella más críticos de memoria y rendimiento en
LLM de transmisión de video. Sin abordar directamente la gestión de caché de KV
durante el llenado previo frecuente, no es posible lograr ganancias prácticas
en la eficiencia de la memoria o la capacidad de respuesta del sistema bajo cargas de trabajo de transmisión.

## B. Limitaciones de la adaptación de algoritmos de recuperación

Adaptación de algoritmos de recuperación orientados a GPU (FlexGen [30], InfiniGen,
ReKV [6]) a las etapas de precarga de LLM de video en streaming causa importantes
ineficiencia debido al cálculo de predicción de KV y la transferencia de datos CPU-GPU
arriba. La sobrecarga de cálculo para la predicción de KV aumenta a medida que el KV
La secuencia de caché aumenta. Además, en escenarios de streaming, la consulta
La matriz consta de varios tokens, cada uno de los cuales requiere una caché KV diferente.
entradas, lo que requiere presupuestos simbólicos mayores que los de texto
generación. Para ilustrar empíricamente estas cuestiones, medimos la
Desglose de la latencia de la transmisión de video LLM con una longitud de secuencia de caché de 40 KV
cuando se adopta InfiniGen para la etapa de precarga con un presupuesto simbólico de 10K
en una GPU NVIDIA A100, como se muestra en la Figura 4 (c). El caché KV
La recuperación incluye tanto el cálculo de predicción de KV como la transferencia de memoria.
latencias. Los resultados muestran que el cálculo de recuperación de caché de KV solo
representa el 23%. Sin embargo, representa el 85% de la latencia total,
donde el 40% se atribuye al cálculo de la predicción de KV y el 39% al
Recuperación de caché KV de la memoria de la CPU. También confirmamos este problema.
con otros métodos de recuperación SOTA (es decir, FlexGen y ReKV),
mostrando una tendencia similar. Además, esta cuestión se vuelve más
grave a medida que aumenta la longitud de la secuencia del token, lo que provoca un KV más grande
cálculo de predicción y recuperación de datos. Estos resultados resaltan que
Los algoritmos existentes orientados a GPU no pueden manejar de manera eficiente un gran volumen de precarga.
cargas de trabajo de transmisión. Abordar este cuello de botella requiere fundamentalmente
nuevos enfoques.

Fig. 5. Optimización del codiseño de software y hardware de V-Rex

<!-- imagen -->

## C. Inflexibilidad de la selección fija Top-K

Muchos algoritmos orientados a GPU, incluidos InfiniGen y ReKV, favorecen top-k
selección en la gestión de caché KV para aprovechar lo predecible
Asignación de recursos y procesamiento paralelo eficiente inherente a la GPU.
arquitectura. Sin embargo, este enfoque estático impone
limitaciones para los LLM de transmisión de video. Fundamentalmente, las matrices de puntuación que
determinar la importancia del token varía ampliamente entre diferentes transformadores
capas y cabezas de atención, lo que refleja que diversas características son
capturados a lo largo de ellos. En consecuencia, se utiliza un conjunto diferente de tokens.
seleccionado como importante por cada capa y cabeza únicas. Estudios previos han
Se ha demostrado que la selección fija de top-k con frecuencia da como resultado tokens redundantes.
o pérdida de tokens relevantes, ya que el K óptimo cambia por capa y cabeza
[7], [36], [41].

Estas ineficiencias se ven exacerbadas en entornos de streaming de borde,
donde los presupuestos de memoria son limitados y se aplican estrictas restricciones de latencia.
En tales contextos, el aprovisionamiento excesivo de caché KV debido a top-k inflexiblepolíticas conllevan una sobrecarga de recursos evitable y tiempos de respuesta más prolongados,
socavando la escalabilidad del sistema y la eficiencia energética. Además, el
La naturaleza de la transmisión de video LLM requiere que los datos de video se transmitan,
y la longitud de la secuencia aumenta en tiempo real, lo que requiere la
ajuste adaptativo del número de tokens seleccionados para garantizar
eficiencia y precisión. Con este fin, V-Rex está diseñado explícitamente para
abordar estos desafíos, proporcionando información detallada y basada en la importancia
selección dinámica que reduce el tamaño de la caché KV y el costo de recuperación para
inferencia práctica y en tiempo real incluso en el borde con recursos limitados
plataformas.

Fig. 6. Descripción general del algoritmo ReSV

<!-- imagen -->

##IV. V-REX: ESTRATEGIA UNIFICADA DE CODISEÑO SW-HW

Para abordar los desafíos de los LLM en transmisión de video, proponemos V-Rex, un
Solución codiseñada software-hardware. La figura 5 ilustra cómo cada
El componente de nuestra pila de optimización reduce la latencia de la capa del decodificador. en
A nivel de software, V-Rex implementa ReSV, un caché KV mejorado
Algoritmo de recuperación que selecciona y recupera eficientemente solo los datos más
entradas relevantes de la memoria o almacenamiento de la CPU, donde se encuentran los cachés completos
descargado. Mejora los métodos anteriores mediante el uso de clave hash-bit
agrupación y umbralización de WiCSum. Aprovechando el alto temporal y
similitud espacial en fotogramas de vídeo, el algoritmo logra un peso ligero
cálculo y selección eficiente de KV. A nivel de hardware, V-Rex
integra unidades compactas para acelerar estas operaciones y minimizar
gastos generales de recuperación. Desacopla estas operaciones del LLM principal.
canal de cálculo, lo que permite ocultar la latencia y de un extremo a otro
optimización.

## A. ReSV: recuperación de caché KV eficiente y precisa

ReSV es un algoritmo sin entrenamiento diseñado para optimizar la caché KV
recuperación durante la etapa iterativa de llenado previo de los LLM de video en tiempo real. como
Como se muestra en la Figura 6, consta de dos etapas principales: recuperación de KV y
ejecución. En la etapa de recuperación, la predicción de KV se realiza sobre la marcha
inmediatamente después de la generación QKV para capturar la naturaleza dinámica del
caché. Los tokens KV seleccionados se obtienen previamente para la siguiente capa de decodificador,
ocultar la latencia de recuperación durante la ejecución. La predicción de KV implica dos pasos.
Primero, la agrupación de claves de bits hash agrupa tokens similares usando
operaciones bit a bit, generando claves representativas (grupo de claves) mediante
promedio dentro de cada grupo. Esto reduce el cálculo al limitar
atención a las claves representativas. Segundo, umbral de WiCSum
Selecciona dinámicamente el grupo de claves más importante, adaptándose a diferentes
distribuciones de datos entre capas y cabezas de atención, a diferencia de top-k fijo
métodos. En la etapa de ejecución, el modelo presta una ligera atención.
usando solo los clusters seleccionados, reduciendo significativamente la memoria y
calcule obteniendo solo las entradas KV esenciales.

Fig. 7. (a) Mapa de calor de similitud de coseno de tokens clave entre cuadros adyacentes (b) Gráfico de dispersión entre similitud de coseno y distancia Hamming de bits hash. Medido en la clave de la tercera capa utilizando el conjunto de datos COIN.

<!-- imagen -->

## B. Agrupación de claves hash-bit para una agrupación rápida por similitud

La razón fundamental para la agrupación de claves hash-bit radica en la alta similitud
entre fichas en marcos adyacentes, como se muestra en la Figura 7 (a). Aprovechando
esta propiedad, el método realiza una agrupación espacio-temporal de claves
cachés para reducir eficientemente la redundancia entre tramas. A diferencia de la fusión
métodos que reemplazan múltiples tokens con una única representación que
requiere una mayor precisión, este enfoque conserva los valores de los tokens originalespara la etapa de ejecución. De este modo, evita operaciones costosas como las de alta
similitud de coseno dimensional mediante el uso de dimensiones ultrabajas
representaciones (≤ 0,5% de la dimensión original) y peso ligero
cálculos de distancia de hamming hash-bit. La Figura 7 (b) demuestra que nuestro
La distancia de hamming de hash-bit puede seguir eficazmente la tendencia del coseno
similitud, teniendo un valor de correlación de 0,8, que es suficiente para realizar
agrupamiento.

El proceso de agrupación consta de dos pasos principales: generación de hashbit
y agrupamiento de distancia de Hamming, como se muestra en la Figura 8. En el hash-bit
En el paso de generación, el cálculo se realiza cada vez que llega una nueva trama.
La matriz clave, obtenida después de aplicar la incrustación de posición rotativa.
operación al marco actual, sufre una reducción de dimensionalidad en
dos direcciones para reducir significativamente la sobrecarga de lo siguiente
agrupamiento de distancia de Hamming. Un conjunto de hiperplanos aleatorios de N hp es
generado, y la matriz clave se multiplica por estos hiperplanos,
produciendo una matriz de dimensiones reducidas Key hp con incrustación de N hp
dimensiones. Luego, cada elemento de Key hp se binariza: valores menores que
o igual a cero se establecen en 0, y los valores mayores que cero se establecen en
1, convirtiendo cada elemento en un solo bit para formar el bit hash clave.

A continuación, se realiza la agrupación de distancias de Hamming. Implica calcular
distancia de hamming entre el hash-bit de clave actual recién generado y
el hash-bit del grupo de claves combinado, que incluye datos anteriores y actuales
Bits de hash clave. La distancia de Hamming se calcula realizando un bit-
operación XOR inteligente entre tokens y contando el número de diferentes
bits. Tokens con distancias por debajo de un umbral definido por hiperparámetros (
Th hp ) están agrupados. Los resultados finales de la agrupación se almacenan en un hash.
Tabla de clúster (HC) que contiene el índice de clúster, el índice de token y la clave.
clúster, bit hash del clúster de claves y recuento de tokens. Este método permite
agrupamiento con muy bajo computacional

## 1. Generación de bits hash

Resultado

<!-- imagen -->

Fig. 8. Flujo de datos de agrupación de claves Hash-bit

<!-- imagen -->

## 4. Umbral de suma acumulativa ponderada (WiCSum)

<!-- imagen -->

¡Índices simbólicos seleccionados!

Fig. 9. Flujo de datos del umbral de suma acumulativa ponderada

gastos generales que normalmente crecen con el recuento de tokens mientras se mantienen altos
Precisión de agrupamiento, lo que lo hace adecuado para una caché KV eficiente.
selección en LLM de vídeo en streaming.

## C. Selección dinámica de tokens a través del umbral WiCSum

El umbral de WiCSum es un algoritmo de selección dinámica desarrollado para
abordar las diversas distribuciones de puntajes que ocurren en diferentes
capas y cabezas de atención. A diferencia de los métodos estáticos top-k que seleccionan un
número fijo de tokens independientemente de su importancia, WiCSum
El umbral determina dinámicamente el número de tokens a seleccionar.
cada capa y cabeza. Este enfoque dinámico permite obtener resultados más precisos y
Recuperación eficiente de caché KV, minimizando la memoria innecesaria y
sobrecarga computacional, lo que permite una baja latencia y una alta
eficiencia.

La Figura 9 muestra el flujo de datos general, compuesto de dos pasos principales: Consulta ×
Cálculo del clúster clave T y verificación de umbrales. En el primer paso, el
El algoritmo calcula la multiplicación de matrices entre la consulta actual.
vectores y el grupo de claves representativo generado por el anterior
Etapa de agrupación de claves hash-bit. Debido a que este cálculo utiliza sólo el
valores representativos del grupo de claves en lugar de todo el caché de claves,
reduce significativamente la sobrecarga computacional. El resultado de estooperación es la matriz de conglomerados de puntuación, que refleja la relevancia de
cada grupo de claves a la consulta actual.

Fig. 10. Arquitectura general de V-Rex

<!-- imagen -->

En el paso de verificación del umbral, elementos importantes en el grupo de puntuación
Se seleccionan las matrices. Para cada fila i de la matriz, calcula un
suma ponderada (Sum i) multiplicando cada puntuación por su correspondiente
recuento de tokens y sumar los resultados, como se muestra en la Ecuación 1. Esto
La suma ponderada se utiliza luego para calcular un umbral ( Th wics ) mediante
multiplicándolo por un hiperparámetro de relación predefinido ( Th r -wics ), como
se muestra en la Ecuación 2. Luego, cada fila del grupo de puntuación se ordena en
orden descendente, donde σ es la permutación de clasificación. A partir de la
valor del grupo de puntuación más alto, la suma ponderada con el recuento de tokens es
acumulado hasta el mínimo t , cuando Acc i ( t ) supera el umbral
Th wics i, como se muestra en la Ecuación 3. Los índices de los conglomerados seleccionados
hasta este punto se agregan en todas las filas, y estos seleccionados
Los índices de clúster luego se asignan nuevamente a los índices de token originales usando
la tabla HC para producir el conjunto final de tokens seleccionados.

<!-- fórmula-no-decodificada -->

<!-- fórmula-no-decodificada -->

<!-- fórmula-no-decodificada -->

## ARQUITECTURA DE HARDWARE DE V. V-REX

El ReSV reduce efectivamente la cantidad de tokens necesarios.
Sin embargo, las operaciones principales introducidas por ReSV presentan
Ineficiencias en las GPU. Estas ineficiencias surgen de 1) condicional
y cálculo dependiente de datos de la agrupación y umbralización de ReSV,
lo que dificulta explotar plenamente el paralelismo, y 2) irregular
y caché KV escasa que se recupera de la memoria SSD y CPU, lo que provoca
subutilización del ancho de banda PCIe, lo que resulta en una mayor latencia.
Para abordar estos desafíos, presentamos V-Rex con baja latencia,
Motor compacto de recuperación de caché KV diseñado específicamente para admitir la
patrones computacionales únicos de ReSV y optimizar el uso intensivo de memoria
Recuperación de KV mediante el manejo eficiente del acceso irregular a la memoria
patrones. Además, se puede integrar fácilmente con los existentes.
hardware, incluidas GPU y NPU, para una alta adaptabilidad.

## A. Descripción general de la arquitectura

La Figura 10 ilustra el sistema de hardware de V-Rex, que consta de tres
Componentes principales: el acelerador V-Rex, la memoria externa y el almacenamiento.
o memoria de CPU para el caché KV completo. El acelerador V-Rex, que
comprende el motor de ejecución LLM (LXE) y DRE, es responsable de la
Tareas computacionales primarias requeridas por los LLM de transmisión de video. el
El flujo de ejecución procede de la siguiente manera: 1 LXE genera hashbits para la corriente
claves de marco, unidad de clúster de 2 bits hash (HCU) realiza distancia de hamming
agrupación y actualiza la tabla HC, 3 LXE calcula el clúster Q × K T y luego
enviar grupo de puntuación a la unidad de umbral WiCSum (WTU), 4 WTU ejecuta WiCSum
umbral con clasificación de salida temprana, determinando qué entradas de tokens
recuperar, 5 KVMU precarga entradas KV seleccionadas del almacenamiento y 6
Los tokens KV recuperados se utilizan en atención.

Motor de ejecución de LLM. LXE procesa las operaciones principales de LLM y dos
cálculos de ReSV. La generación de hash-bit y Query × Key T
El cálculo del clúster de ReSV se procesa en LXE, ya que implica principalmente
multiplicaciones de matrices y cálculos de vectores. El LXE se basa en el
Arquitectura central de la LPU [23], que integra un motor de producto punto.
(DPE) para multiplicación de matrices de alto rendimiento y procesamiento de vectores
(VPE) para operaciones vectoriales eficientes, ambos con precisión BF16.DPE se compone de árboles MAC N DPE -h, que reciben entradas N DPE -w. el
VPE se compone de unidades vectoriales N V PE -h y acepta N V PE -w
entradas.

## B. Motor dinámico de recuperación de caché KV (DRE)

El DRE consta de una KVPU y una KVMU, que son responsables de la
cálculos y gestión de memoria necesarios durante la caché KV dinámica
recuperación. La KVPU integra HCU y WTU, que en conjunto
Acelere las operaciones más críticas para la latencia en la recuperación de caché KV.
Al descargar estas tareas del proceso de cómputo principal, V-Rex
reduce significativamente los cuellos de botella computacionales y de obtención de datos.

Fig. 11. Flujo de datos de clasificación de salida anticipada

<!-- imagen -->

Unidad de clúster de bits hash. En el núcleo de la KVPU, la HCU es responsable
para ejecutar el proceso de agrupación de claves de bits hash. La HCU es una compacta
módulo informático, que consta de tres componentes principales: un hash actual
memoria de bits, una memoria de bits hash caché de clave y N HCU -h XOR paralelo
acumuladores, cada uno capaz de procesar N entradas HCU -w. La UHC
recibe los vectores de bits hash clave del LXE y los almacena en el
memoria de bits hash actual. Al mismo tiempo, recupera el hash-bit de la caché de claves.
agrupa desde la tabla HC y los almacena en el bit hash de caché de claves
memoria. Ambos están estructurados como matrices de bits para permitir una
operaciones paralelas.

Para realizar la agrupación, la HCU inicia el cálculo de Hamming
distancias entre los vectores de bits hash actuales y la caché de claves almacenadas
clusters de bits hash. Este proceso utiliza acumuladores XOR para identificar
diferencias entre los bits correspondientes en las matrices. el
Los acumuladores luego suman el número de bits diferentes para calcular el
distancia de Hamming para cada comparación. Comparando el Hamming calculado
distancias con el umbral predefinido Th hd, la HCU eficientemente
agrupa tokens con patrones de bits hash similares en grupos. Entonces, el
Los resultados de la agrupación se almacenan en la tabla HC. Este hardware acelerado
Este enfoque permite una agrupación en clústeres rápida y energéticamente eficiente utilizando bit a bit.
operadores, respaldando los requisitos de área baja para la implementación de borde.

Unidad de umbral WiCSum. La WTU acelera la verificación del umbral de WiCSum,
permitiendo el cálculo de selección de baja latencia. Consta de múltiples
Núcleos WTU paralelos, cada uno equipado con memoria de puntuación, memoria de conteo de tokens,
y una unidad informática dedicada para el establecimiento de umbrales. Cada núcleo incluye
Clasificadores de cubetas superior e inferior, multiplicadores, un árbol sumador y una cubeta
actualizador de rango. Las operaciones más intensivas desde el punto de vista computacional, clasificación
y la verificación de umbrales, son manejados eficientemente por el flujo de datos de la WTU,
que utiliza clasificación de salida temprana. Combina dos operaciones en un fino-
tubería granulada y termina la clasificación en el medio cuando excede
el umbral, como se muestra en la Figura 11. Este proceso se divide en dos
Pasos principales: el paso de preproceso y el paso de selección de token. en el
paso previo al proceso, los núcleos WTU precalculan los valores necesarios para el token
paso de selección, como la suma ponderada de puntuaciones y recuentos de tokens para
cada fila, los valores de puntuación mínimo/máximo y el umbral Th wics. Durante
En el paso de selección de token, el proceso comienza con el depósito que contiene
el rango de puntuación más alto. La WTU realiza clasificación por cubos, suma acumulativa,
y verificación de umbrales en la tubería. El tipo cubeta, que es altamente
paralelizable, utiliza la información preprocesada para determinar la altura superior
e inferiores para cada depósito, y los clasificadores generan máscaras de bits
indicando qué puntuaciones se encuentran dentro del rango actual. El seleccionadoLuego, los valores se multiplican y se suman para calcular la suma ponderada, que
Se compara con Th wics para decidir si salir o continuar. tan temprano
El mecanismo de salida es eficaz porque un pequeño número de puntuaciones grandes
normalmente representan la mayor parte de la suma ponderada (promedio 16% por
fila), permitiendo alcanzar el umbral rápidamente comenzando con el
cubos más altos.

Fig. 12. Sistema de memoria jerárquico y mapeo de memoria por clústeres

<!-- imagen -->

## C. Unidad de gestión de caché KV

La KVMU administra la memoria caché de KV para los LLM de transmisión de video. es
responsable de dos funciones principales: memoria caché KV jerárquica
gestión y mapeo de memoria basado en clústeres hash. En primer lugar, KVMU supervisa una
sistema de memoria jerárquico, como se ilustra en la Figura 12, para
gestionar el movimiento de datos entre la memoria de V-Rex, la memoria de la CPU y el almacenamiento.
Las entradas recientes de la caché de KV se almacenan en la memoria de V-Rex para un acceso rápido.
Cuando el tamaño total del caché KV en la memoria de V-Rex excede un
Capacidad máxima predefinida, las entradas más antiguas se descargan a la CPU.
memoria o almacenamiento. Estas entradas descargadas se pueden recuperar de la CPU
memoria o almacenamiento y se devuelve a la memoria de V-Rex cuando sea necesario para
cálculo. Este sistema de memoria jerárquico garantiza que tanto los más
Las entradas de caché KV recientes y recuperadas están disponibles para su cálculo,
mientras que los datos más antiguos o menos críticos se mantienen fuera del chip para
reducir la sobrecarga de memoria.

En segundo lugar, KVMU implementa una estrategia de mapeo de memoria eficiente basada en
grupos de hash. Para maximizar la utilización del ancho de banda PCIe, los tokens que
pertenecen al mismo grupo hash, se agrupan y almacenan en ubicaciones contiguas
direcciones de memoria. La agrupación se lleva a cabo íntegramente dentro del
caché KV reciente, lo que elimina cualquier necesidad de acceder a la CPU o al almacenamiento para
agrupación con el caché descargado. Este arreglo permite más
uso eficiente del ancho de banda de la memoria, ya que múltiples tokens del mismo
El clúster se puede transferir juntos en una sola operación. cada vez nuevo
Las entradas de caché de KV se generan para una trama, KVMU reordena y almacena
almacenarlos en la memoria de acuerdo con los últimos resultados de agrupación. Porque KVMU
maneja este reordenamiento internamente, el caché KV se almacena en un
moda de transmisión, y cualquier latencia asociada con la reorganización es
efectivamente oculto. Aunque este mapeo de memoria es técnicamente factible
En GPU convencionales, es muy poco práctico porque requiere una precisión
Reorganización detallada de datos en línea. Este proceso conlleva importantes
sobrecarga de latencia que en última instancia anula los beneficios de la optimización
diseño, ya que implica cálculos frecuentes por capas y diseños irregulares,
operaciones de dispersión que requieren mucha memoria. Para ello, KVMU garantiza que
Los LLM de transmisión de video pueden acceder a datos de caché críticos con baja latencia,
mantener una huella de memoria reducida y utilizar el ancho de banda disponible
de manera óptima a través de estos dos mecanismos.

##VI. EVALUACIÓN

## A. Configuración experimental

Rendimiento. Para evaluar el rendimiento del sistema de hardware de V-Rex, utilizamos
desarrolló un simulador de nivel de ciclo personalizado. Para el rendimiento de DRAM, nosotros
DRAMSim3 integrado [18], un simulador de DRAM ampliamente utilizado y para SSD
rendimiento, incorporamos MQSim [35], un simulador de SSD. Para precisar
movimiento de datos del modelo entre la memoria de la CPU y la memoria de la GPU, los datos reales
El ancho de banda de transferencia se modela utilizando una GPU NVIDIA A100 [3] y un AGX.
Orin GPU [2], ambos incorporados al simulador. Nosotros
comparó V-Rex con dos GPU representativas: un dispositivo de borde (JetsonAGX Orin) y una GPU de servidor (NVIDIA A100), que utilizan un sistema idéntico y
parámetros de rendimiento, resumidos en la Tabla I. Para el escenario de borde,
Se creó una instancia de V-Rex con ocho núcleos, utilizando PCIe de 4 GB/s con
SSD M.2 NVMe para descargar la caché KV y LPDDR5 de 204,8 GB/s de
Autobús de 256 bits. Para el escenario del servidor, V-Rex utilizó 48 núcleos, logrando
un total de 319 TFLOPS, con 1935 GB/s HBM2e de bus de 5120 bits y 32 GB/s
PCIe con descarga de caché KV a memoria de CPU basada en DDR4. Para el
transmisión de video LLM, todos los experimentos emplean Llama-3 8B como columna vertebral
modelo y SigLIP-ViT-L-384 [44] como codificador de visión.

Potencia/Área. Un único núcleo V-Rex está configurado como N DPE -h =64, N DPE -w
=64, N V PE -h =1, N V PE -w =64, N HCU -h =1, N HCU -w =16, N WTU -h
=1, y N WTU -w =16. Fue implementado en RTL y sintetizado usando
Sinopsis

TABLA I ESPECIFICACIONES DE HARDWARE DE GPUS Y V-REX|                       | Borde | Borde | Servidor | Servidor ||-----------------------|------------------------|---------------------|---------------------|---------------------|
|                       | NVIDIA Jetson AGX Orin | V-Rex 8 | Nvidia A100 | V-Rex 48 || Número de núcleos V-Rex |                        | 8 |                     | 48 || Rendimiento máximo 1 | 54 | 53,3 | 312 | 319,5 || Ancho de banda de memoria | LPDDR5 - 204,8 GB/s | LPDDR5 - 204,8 GB/s | HBM2e - 1935 GB/s | HBM2e - 1935 GB/s || Capacidad de memoria | 32 GB | 32 GB | 80 GB | 80 GB || Ancho de banda PCIe | PCIe3.0 x4 4GB/s | PCIe3.0 x4 4GB/s | PCIe 4.0 x16 32 GB/s | PCIe 4.0 x16 32 GB/s || Consumo de energía 2 | -40W | -35W | -300W | -203,68W |1: FP16 para AGX, BF16 para V-Rex y A100, @ 0,8 V 800 MHz, 2: V-Rex,
DRAM, PCIe y almacenamiento incluidos

Compilador de diseño en un proceso de 14 nm. Funciona de forma fiable a 0,8 V y 800
MHz sin violaciones de temporización en condiciones nominales, según lo confirmado por
Análisis de temporización estática previo al diseño. El comportamiento de la DRAM (HBM2e, DDR4) fue
modelado utilizando DRAMSim3, y los datos de energía LPDDR5 se tomaron del proveedor
informes [11], [15]. La potencia PCIe se estimó en 3 W por carril a plena capacidad.
La carga y la potencia del SSD se basaron en las especificaciones de Kioxia BG6 [1]. GPU
Las mediciones de potencia se obtuvieron a través de NVIDIA-SMI y tegrastats [25],
[26]. Todos estos parámetros se integraron en nuestro simulador personalizado para
evaluación precisa a nivel del sistema. Esta configuración garantiza una imagen realista y
comparación justa con las GPU de servidor y de borde de referencia.

## B. Evaluación de desempeño y eficiencia

Latencia. Para evaluar el rendimiento de V-Rex para los LLM de transmisión de video, nosotros
comparó su latencia en el procesamiento de cuadros y la generación de texto con
Cuatro métodos de recuperación basados en top-k en GPU tanto de borde como de servidor. FlexGen
[30] sirve como base, descargando cachés KV a la memoria de la CPU (A100)
o almacenamiento (AGX Orin). InfiniGen [16] recupera tokens solo durante
generación, InfiniGenP extiende esto al prellenado, y ReKV [6] realiza
selección a nivel de fotograma. Todas las líneas de base realizan predicciones de KV en el
capa de atención anterior para precargar cachés KV, búsqueda superpuesta
latencia con cálculo. Variamos los tamaños de caché KV (1K, 5K, 10K, 20K,
40K) usando COIN [31], calibrando la relación de selección de cada método para que coincida
exactitud de referencia. La latencia se midió como latencia por cuadro durante
procesamiento de fotogramas y tiempo por token de salida (TPOT) durante el texto
generación.

La comparación de latencia en la GPU perimetral se muestra en la Figura 13 (a). como token
aumentos de longitud, latencia por cuadro y aumento de TPOT en todos los anteriores
métodos debido a un mayor cálculo de atención, mayor selección
gastos generales y transferencias KV más grandes, impulsadas por top-k fijo que requiere alta
ratios de selección de tokens. En consecuencia, ninguna de las configuraciones de GPU perimetrales
AGX+FlexGen, InfiniGen, InfiniGenP o ReKV logran tiempo real
rendimiento en secuencias más largas, con la brecha ampliándose a medida que la longitud del token
crece. En la etapa de procesamiento de cuadros, la abundancia de tokens de consulta
exige ratios de recuperación más altos que en la generación de texto, ya que cada
El token de consulta requiere recuperación. AGX+InfiniGen y AGX+InfiniGenP son
incluso más lento que la línea base FlexGen debido a un nivel de token detallado
selección que introduce una importante sobrecarga de preprocesamiento. AGX+ReKV
La selección aproximada a nivel de fotograma ofrece ganancias de latencia modestas, pero aún así
requiere seleccionar muchas fichas para mantener la precisión, limitando su
efectividad.

Por el contrario, V-Rex 8 permite la inferencia de transmisión en tiempo real (≥ 2 FPS)
incluso con secuencias largas y lotes grandes. Con un tamaño de lote de 1, por
las latencias de trama son 121 ms, 123 ms, 198 ms, 200 ms y 254 ms para caché
tamaños de 1K, 5K, 10K, 20K y 40K, respectivamente. Mantiene 3.9-8.3
FPS en todos los tamaños para implementación perimetral en tiempo real, logrando 2,2-7,3 ×
aceleraciones sobre AGX+FlexGen. Cuando el tamaño del lote aumenta a 4, se acelera
aumentar a 2,1-13,8 × . En la generación de texto, las latencias TPOT son más bajas, de 89 a
97 ms, lo que produce aceleraciones de 1,9 a 15,1 ×. Estos beneficios surgen de minimizar
volumen KV seleccionado a través de ReSV y aprovechando el cómputo de alta velocidad de DRE y
movimiento de datos. Para evaluar la escalabilidad, probamos un V-Rex 48 y un A100
GPU para comparación a nivel de servidor (Figura 13 (b)). V-Rex alcanza 20-48 mslatencia por cuadro, con aceleraciones de 2,6 a 7,3 × en tamaño de lote 1. En lote
tamaño 8, las aceleraciones aumentan a 3,4-19,7 ×, lo que demuestra un fuerte paralelo
eficiencia. Las latencias TPOT de 14-15 ms producen aceleraciones de 2,8-16,8 ×. Estos
Los resultados muestran que V-Rex reduce significativamente la latencia en ambos fotogramas.
procesamiento y generación de texto para streaming de video LLMs sobre el borde y
GPU de servidor.

Fig. 13. Comparación de eficiencia energética y latencia por cuadro y TPOT versus (a) GPU Edge y (b) GPU de servidor. Se aplican los métodos de referencia de FlexGen, InfiniGen, InfiniGenP y ReKV. Barrimos la longitud de la secuencia de caché KV de 1K a 40K.

<!-- imagen -->

Fig. 14. Comparación del desglose de la latencia de un extremo a otro

<!-- imagen -->

Desglose de la latencia E2E. Como se muestra en la Figura 14, evaluamos la latencia
desglose de AGX Orin y V-Rex 8 en un LLM de video en streaming de extremo a extremo
escenario, utilizando un caso promedio del punto de referencia COIN. Los resultados
demostrar que AGX+FlexGen no logra mitigar este crecimiento explosivo, como
así como optimizaciones solo de software (es decir, InfiniGenP y ReKV), que
incluso funciona más lento que FlexGen de 1K a 20K debido a la predicción de KV
arriba. Por otro lado, la principal ganancia de rendimiento de nuestro trabajo
surge de reducir la sobrecarga de la etapa iterativa de prellenado,
aumentando la brecha de rendimiento a medida que aumenta la secuencia de caché KV. esto
da como resultado una reducción de hasta 5,4 veces en la latencia de un extremo a otro. Por
Al gestionar eficazmente la caché KV durante el prellenado, nuestro método mantiene un
latencia constante incluso a medida que crece el caché.

Fig. 15. Comparación de rendimiento frente al acelerador SOTA LLM

Eficiencia Energética. La Figura 13 muestra que la eficiencia energética de V-Rex aumenta
crece con la longitud del token, gracias a la reducción de la transferencia de datos. Con tamaño de lote
1 durante el procesamiento de cuadros, V-Rex logra entre 5,5 y 10,2 veces más energía
eficiencia sobre AGX+FlexGen para tamaños de caché KV de 1K a 40K; con
tamaño de lote 4, la ganancia aumenta a 3,1-12,8 ×. En la generación de texto, el
la mejora es aún más pronunciada, alcanzando 4,3-18,5 ×. esto
La ventaja se amplifica en las GPU de servidor, donde el consumo de energía es
más alto. En comparación con A100+FlexGen, V-Rex alcanza 9,0-29,7 veces más
eficiencia energética durante el procesamiento de marcos con tamaño de lote 1. En lote
talla 8, ofrece 1,1-1,4 TOPS/W, logrando ganancias de 5,9-52,2 ×. En texto
En generación, la eficiencia energética alcanza 13,2-70,6 × . Estas mejoras
surgen de dos factores clave: la capacidad de ReSV para minimizar los datos recuperados
volumen y el soporte del módulo KVMU para datos eficientes y de gran ancho de banda
buscando. Como resultado, el consumo de energía para las transferencias de datos basadas en PCIe
se reduce significativamente. En general, V-Rex ofrece resultados sustancialmente más altos
eficiencia energética que los métodos de recuperación basados en GPU de última generación,
lo que la convierte en una solución atractiva para la aceleración de LLM en streaming de vídeo.

Comparación con el acelerador SOTA. La Figura 15 compara el rendimiento de
V-Rex 8 y Oaken [13], un acelerador LLM de última generación que utiliza 4 bits
Cuantización de caché KV, en la GPU NVIDIA AGX Orin. En esta configuración, AGX
Orin ejecuta FlexGen sin descarga de KV, mientras que Oaken lo aplica en línea
cuantización. Con una longitud de secuencia corta (1K), VRex alcanza hasta 1,5 ×
y 1,1 × FPS más alto que AGX Orin y Oaken, respectivamente, debido a la total
búsquedas de almacenamiento superpuestas y cálculo de atención reducido. como
La longitud de la secuencia aumenta, AGX Orin encuentra falta de memoria (OOM)
los errores primero, impulsados por el creciente caché de KV. Oaken, con su cuantificado
caché, maneja secuencias más largas y mantiene un mayor rendimiento, perotodavía falla más allá de los 20 000 tokens debido a límites de memoria. Por el contrario, el V-Rex
la recuperación eficiente le permite operar de manera confiable más allá de 20K tokens,
manteniendo 7 FPS incluso en secuencias de gran duración.

Fig. 16. Estudio de ablación y descomposición de la latencia de V-Rex

<!-- imagen -->

Estudio de ablación y aplicación Desglose de latencia. Esta evaluación ilustra cómo cada
La optimización de V-Rex contribuye a reducir la latencia y la energía
consumo durante el procesamiento de cuadros. Primero presenta ganancias acumuladas.
a medida que se aplica cada optimización, seguido de un desglose de latencia que muestra
qué componentes de ejecución se ven afectados por cada esquema. Implementamos
AGX+ReSV aplicando ReSV en la GPU AGX Orin y evaluando V-Rex 8 mediante
Habilitación incremental de optimizaciones en un caché de 40K con tamaño de lote.
1. V-Rex 8 KVPU incluye el KVPU, mientras que V-Rex 8 All incorpora todos
optimizaciones, incluido KVMU. Los resultados demuestran claramente la capacidad de la GPU.
ineficiencia y resaltar la necesidad de un código de diseño de software-hardware.

Como se muestra en la Figura 16, AGX+ReSV reduce la latencia general en 2,8 × por
ocultando la mayor parte de los gastos generales de recuperación bajo el cálculo. Sin embargo, debido a
Predicción KV compleja, como condicional y dependiente de datos.
cálculo para agrupamiento y umbralización, todavía representa el 48%
de latencia total, lo que limita todo el potencial del algoritmo en la GPU. con
Optimización a nivel de hardware, V-Rex 8 KVPU reduce la latencia de predicción de KV
gastos generales hasta el 0,5% (desde el 23% del cálculo total), logrando un 6,0 ×
aceleración y reducción de energía de 9,2 × mediante operación de predicción superpuesta
con cálculo LLM utilizando las rápidas operaciones bit a bit de HCU y WTU
clasificación de salida temprana. V-Rex 8 All mejora aún más el rendimiento al
aumentando la utilización del ancho de banda PCIe, alcanzando una aceleración de 8,1 × y
10,2 × ahorro de energía. Aunque KVMU introduce cierta sobrecarga de memoria debido
a la tabla HC, ocupa solo el 1,67% del caché KV completo con un
promedio de 32 tokens por grupo. Cada optimización de V-Rex contribuye
incrementalmente al rendimiento y la eficiencia energética. En particular, ReSV solo
es insuficiente; El efecto combinado de ReSV y DRE es esencial para
Realice completamente una recuperación eficiente de caché KV para LLM de transmisión de video.

Fig. 17. Análisis del uso del ancho de banda de memoria de V-Rex 48

<!-- imagen -->

Fig. 18. Análisis del modelo de línea del techo en AGX Orin y V-Rex 8

<!-- imagen -->

## C. Análisis de ancho de banda para computación concurrente

Para mostrar que la predicción y recuperación de KV se pueden superponer completamente con
Cálculos de LLM, analizamos el uso de ancho de banda de cada cálculo.
sobre una capa de etapa de procesamiento de cuadros, como se muestra en la Figura 17.
demuestra que la memoria se comparte efectivamente entre concurrentes
operaciones. La predicción de KV se ejecuta simultáneamente con la
operación de atención. Aunque aumenta brevemente el uso de ancho de banda a
600GB/s, su corta duración permite ocultarlo por completo. el kv
La recuperación, que transfiere datos de la memoria de la CPU a la DRAM, se ejecuta para la mayoría
del tiempo de ejecución pero sólo consume alrededor del 1% del ancho de banda.
Debido a que la recuperación de caché de KV tiene un cuello de botella debido al ancho de banda PCIe, que es aproximadamente
1% del ancho de banda de DRAM, permite que la recuperación de KV se ejecute simultáneamente
con atención y cálculos FFN con una sobrecarga mínima.

## D. Análisis del modelo de línea del techo

La Figura 18 ilustra un análisis del modelo de línea de techo del procesamiento del marco.
etapa para tres sistemas de nivel de borde: AGX+FlexGen, AGX+ReKV y nuestro
propuesto V-Rex 8. Este análisis utiliza una carga de trabajo con una longitud de caché KV
de 40K y un tamaño de lote de 4, lo que produce una intensidad operativa promediode 15,2 Op/B. El resultado revela una disparidad significativa en los logros alcanzados.
rendimiento a través de los sistemas. AGX+FlexGen exhibe el más bajo
rendimiento, alcanzando sólo el 6,6% de su máximo teórico. esta grave
La subutilización se atribuye a la lenta comunicación PCIe, que
crea un cuello de botella durante la recuperación de la caché KV. Por lo tanto, optimizar la
El cálculo de inferencia LLM por sí solo es ineficaz sin resolver el
cuello de botella fundamental de E/S. AGX+ReKV emplea un mecanismo de recuperación para
lograr un mayor rendimiento, alcanzando aproximadamente

TABLA II EVALUACIÓN DE LA PRECISIÓN DEL MODELO Y RELACIÓN DE RECUPERACIÓN| Precisión superior del punto de referencia COIN ↑ | Precisión superior del punto de referencia COIN ↑ | Precisión superior del punto de referencia COIN ↑ | Precisión superior del punto de referencia COIN ↑ | Precisión superior del punto de referencia COIN ↑ | Precisión superior del punto de referencia COIN ↑ | Precisión superior del punto de referencia COIN ↑ ||-----------------------------------|-----------------------------------|-----------------------------------|-----------------------------------|-----------------------------------|-----------------------------------|-----------------------------------|
| Método aplicado | Recuperación @Frame | Paso | Siguiente | Tarea | Proc.                             | Proc.+ || VídeoLLM-Online | X | 62.1 | 49,0 | 92,5 | 49,5 | 51,6 || Infinito | X | 62.1 | 48,3 | 92,2 | 49,5 | 51,0 || InfinigenP | O | 58,6 | 45,6 | 91,5 | 46,4 | 50,2 || ReKV | O | 59,9 | 46,3 | 91,3 | 47,6 | 50,0 || ReSV de V-Rex | O | 62,0 | 47,5 | 92,2 | 48,2 | 50,5 |Fig. 19. Estudio de ablación de ReSV| Proporción de recuperación [Etapa de procesamiento de marcos / Etapa de generación de texto] Proporción de recuperación [Etapa de procesamiento de marcos / Etapa de generación de texto] | Proporción de recuperación [Etapa de procesamiento de marcos / Etapa de generación de texto] Proporción de recuperación [Etapa de procesamiento de marcos / Etapa de generación de texto] | Proporción de recuperación [Etapa de procesamiento de marcos / Etapa de generación de texto] Proporción de recuperación [Etapa de procesamiento de marcos / Etapa de generación de texto] | Proporción de recuperación [Etapa de procesamiento de marcos / Etapa de generación de texto] Proporción de recuperación [Etapa de procesamiento de marcos / Etapa de generación de texto] | Proporción de recuperación [Etapa de procesamiento de marcos / Etapa de generación de texto] Proporción de recuperación [Etapa de procesamiento de marcos / Etapa de generación de texto] | Proporción de recuperación [Etapa de procesamiento de marcos / Etapa de generación de texto] Proporción de recuperación [Etapa de procesamiento de marcos / Etapa de generación de texto] | Proporción de recuperación [Etapa de procesamiento de marcos / Etapa de generación de texto] Proporción de recuperación [Etapa de procesamiento de marcos / Etapa de generación de texto] ||-------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------|| Método aplicado Método aplicado | Promedio Promedio                                                                                                                           | Paso Paso | Siguiente Siguiente | Tarea Tarea | Proc. Proc.                                                                                                                         | Proc.+ Proc.+ || Infinigen Infinigen | 100/6,8 100/6,8 | 100/6,2 100/6,2 | 100/6,7 100/6,7 | 100/4,0 100/4,0 | 100/8,5 100/8,5 | 100/8,6 100/8,6 || InfinigenP InfinigenP | 50,8/6,8 50,8/6,8 | 50,8/6,2 50,8/6,2 | 50,8/6,7 50,8/6,7 | 51,0/4,0 51,0/4,0 | 50,6 / 8,5 50,6 / 8,5 | 50,7/8,6 50,7/8,6 || ReKV ReKV | 58,4/31,2 58,4/31,2 | 59,7 / 33,4 59,7 / 33,4 | 56,7 / 34,5 56,7 / 34,5 | 51,4/13,6 51,4/13,6 | 61,7 / 36,7 61,7 / 36,7 | 62,5 / 37,9 62,5 / 37,9 || ReSV de V-Rex ReSV de V-Rex | 32,7/2,5 32,7/2,5 | 34,3/2,4 34,3/2,4 | 32,4/2,8 32,4/2,8 | 25,1/1,4 25,1/1,4 | 35,5/2,9 35,5/2,9 | 36,1/2,9 36,1/2,9 |<!-- imagen -->

15% del pico. Sin embargo, al ser una optimización puramente basada en software,
sigue siendo ineficiente. Finalmente, nuestro VRex propuesto demuestra una
rendimiento notable al 71,5% de su máximo teórico, marcando un
10,8 × mejora con respecto a AGX+FlexGen. Confirma que V-Rex efectivamente
Resuelve las ineficiencias inherentes a los sistemas convencionales basados en GPU.

## E. Análisis comparativo de precisión

Carga de trabajo. Para demostrar la flexibilidad y precisión de ReSV,
evaluó y comparó el rendimiento de los métodos de recuperación existentes
(es decir, InfiniGen, InfiniGenP y ReKV) utilizando cinco puntos de referencia del
Conjunto de datos de monedas. VideoLLMOnline [4] se utilizó como línea de base sin ningún
optimización de recuperación aplicada. Para este experimento, los métodos existentes
se configuraron para seleccionar hasta el 50% de los tokens con su top-k fijo
mecanismo, mientras que ReSV utilizó un umbral en su operación WiCSum que era
ajustado empíricamente para garantizar la precisión, configurando N hp =32, Th wics
a 0,3 y Th hp =7.

Exactitud. La Tabla II resume los resultados. El ReSV de V-Rex supera
otros métodos de recuperación, lo que demuestra la tasa de recuperación más baja mientras
logrando la mayor precisión general. Comparado con la vainilla base
modelo (VideoLLM-Online), ReSV exhibe solo una precisión promedio marginal
caída del 0,8%. Además, ReSV reduce significativamente la recuperación
relación, ya que puede adoptar diversas distribuciones de puntuación de diversas tareas.
Durante la etapa de procesamiento de cuadros, la tasa de recuperación promedio varía
del 25,1% al 36,1%, y durante la etapa de generación del texto varía
entre el 1,4% y el 2,9%. Esta variabilidad pone de relieve que el umbral
El mecanismo en ReSV adapta efectivamente la selección de tokens de acuerdo con cada
características de la tarea.

Por el contrario, InfiniGen mantiene una precisión comparable a la del modelo estándar.
modelo, pero no realiza la recuperación durante el procesamiento del marco
etapa, lo que lo hace poco práctico para la inferencia en tiempo real. InfiniGenP
recupera aproximadamente el 50% de los tokens durante la etapa de procesamiento del marco,
lo que conduce a una degradación sustancial de la precisión de hasta el 3,4%. ReKV,
que emplea selección por fotogramas, requiere un gran volumen de datos recuperados.
Caché KV para las etapas de procesamiento de cuadros y generación de texto.
mantener la precisión como InfiniGenP, degradando así la eficiencia. en
resumen, la agrupación de claves hash-bit y las técnicas de umbralización WiCSum
de ReSV permiten una adaptación dinámica a la distribución de datos, de manera efectiva
seleccionando la cantidad mínima de tokens preservando la precisión. esto
hace que ReSV sea particularmente adecuado para aplicaciones en tiempo real y con recursos limitados.
transmisión de video inferencia LLM.

Fig. 20. Comparación del ratio de recuperación por capa y por cabeza

<!-- imagen -->

Eficiencia ReSV. Realizamos un estudio de ablación incrementalmente
aplicando las optimizaciones de ReSV. La Figura 19 muestra la precisión promedio en
Puntos de referencia COIN y latencia de procesamiento de cuadros en un tamaño de caché de 40K.
En primer lugar, ReSV sin aplicar agrupación mejora la latencia en 1,6 veces más.
la línea de base, lo que provoca una caída menor en la precisión del 0,3%, originada en
el cálculo de la atención a la luz. En segundo lugar, ReSV, que además
incorpora agrupamiento de bits hash, logra una aceleración de 9,4 ×, acompañado
con una reducción de precisión del 0,8%. Esta importante aceleración proviene de
reducir la búsqueda y el cálculo de la clave completa para WiCSum
cálculo de umbrales agrupando el caché de claves.

La Figura 20 presenta la proporción de datos recuperados en una capa y por capa.
Base principal de un vídeo de muestra de COIN. A diferencia de InfiniGenP y ReKV,que recuperan un número fijo de tokens de caché KV de manera uniforme en todos
capas y cabezas, ReSV exhibe una distribución diversa en el token
relación de recuperación. Ciertas capas que requieren menos tokens exhiben
tasas de selección del 4,2%, mientras que las capas más críticas con mayor token
importancia demuestra alrededor del 44,0%. Esta variabilidad también puede ser
observado entre los jefes de atención. Permite a ReSV mantener niveles más altos
precisión al recuperar 3,0 × menos tokens en promedio en comparación con
ReKV, lo que resulta en una eficiencia superior en comparación con el top-k fijo
mecanismos.

## Gastos generales de hardware de F. V-Rex

Potencia y Área. La Tabla III resume el consumo de energía y el área.
avería para V-Rex equipado con un solo núcleo. Un único núcleo V-Rex
Ocupa 1,89 mm 2 y consume 2,61 W, equipado con memoria en chip de
384 KB para LXE y 20,125 KB para DRE. Cuando se escala a mayor
configuraciones, el área de V-Rex 8 es 15,12 mm 2 , que es
sustancialmente más pequeño que la GPU AGX Orin (200 mm 2 ). En particular, V-Rex
48

CUADRO III DESGLOSE DE SUPERFICIE Y POTENCIA| Componentes de hardware | Componentes de hardware |   Área[mm 2 ] | Área[mm 2 ] |   Potencia [mW] | Potencia [mW] ||-----------------------|-----------------------|---------------|---------------|--------------|--------------|
|                       | DPE |          1,37 | 72,79% |      2311.39 | 88,58% ||                       | PU |          0,14 | 7,18% |       122,06 | 4,68% ||                       | Memoria en chip |          0,34 | 17,98% |       118,94 | 4,56% ||                       | KVPU-HCU |          0,01 | 0,28% |         2,99 | 0,11% ||                       | KVPU-WTU 0,02 | 1,23% |        39.04 | 1,49% ||                       | KVMU |          0,01 | 0,53% |        15.01 | 0,58% ||                       | Totales |          1,89 | 100% |      2609.43 | 100% |Ocupa 90,57 mm 2 , bastante menos que la GPU A100 (826 mm 2 ).
Incluyendo la potencia total del sistema, V-Rex 8 consume 35 W, alcanzando un 11,4%
menor consumo de energía que la GPU AGX Orin (40 W), mientras que V-Rex 48
consume 203,68 W, lo que demuestra un consumo de energía un 32,1% menor que el
GPU A100 (300 W), como se detalla en la Tabla I. El hardware adicional
Los gastos generales de DRE son mínimos y representan solo el 2,4% del total del chip.
energía y el 2,0% del área total, lo que se puede atribuir a la
Algoritmo efectivo de recuperación de caché KV. Su diseño compacto permite
integración eficiente con cualquier GPU, NPU y LLM existentes
aceleradores.

##VII. TRABAJO RELACIONADO Y DISCUSIÓN

Optimización de LLM de transmisión de video. VideoLLMOnline [4] presenta una
Sistema de interacción de vídeo en streaming que ajusta el muestreo de fotogramas y
resolución. LiveVLM [24] se centra en la memoria reutilizable a corto y largo plazo
tokens para una comprensión eficiente del marco. QuickVideo [28] aprovecha
La decodificación paralela de la CPU y la inferencia de GPU se superponen para lograr un extremo a otro.
aceleraciones. VidMoD [37] reduce la carga de trabajo de procesamiento visual utilizando mezcla-
cálculo de profundidad, omitiendo dinámicamente capas innecesarias. mientras
Estos sistemas mejoran la eficiencia del procesamiento de cuadros, aún sufren
de repetidas transferencias de caché KV y sobrecarga de memoria durante consultas múltiples
cargas de trabajo de transmisión. Por el contrario, V-Rex integra agrupación y
Umbral dinámico directamente en la etapa de prellenado para gestionar KV.
transferencias de manera más efectiva.

Gestión y compresión de KVCache. Para abordar el creciente costo de la memoria
de cachés de KV, varios métodos reducen el tamaño de KV o recuperan selectivamente
fichas relevantes. LeanKV [46] emplea caché KV de precisión múltiple
Cuantización, donde se combinan claves de alta precisión y valores de baja precisión.
usado. MiniCache [19] y PyramidInfer [41] comprimen según capas
importancia, pero deja la similitud entre fotogramas sin explorar. Generación de caché [20]
explota un mecanismo de códec de vídeo tradicional pero requiere una preparación previa costosa.
sobrecarga de procesamiento para compresión, que no es adecuada para transmisión
aplicaciones. Todos estos métodos utilizan compresión fija y poda.
Políticas que no pueden adaptarse a los patrones de atención en evolución en el video.
corrientes. ReSV agrupa tokens similares en distintos marcos y su dinámica
La selección de tokens por capa y cabezal produce una recuperación más precisa sin
sacrificar la integridad del contexto.

Aceleradores de hardware de transformadores. Los trabajos anteriores se centraron principalmente en
optimizaciones computacionales y de ancho de banda de memoria para acelerar los LLM.
DFX [10] y SpecEE [39]

objetivo acelerar la etapa de generación ligada a la memoria. BitMoD [5] y
LUT Tensor Core [22] reduce la sobrecarga computacional con un bit bajo
método de inferencia. Mientras que AiF [14] demuestra el procesamiento en flash para
LLM en el dispositivo, no incorpora mecanismos de recuperación dinámica.
Oaken [13] introduce la cuantificación de caché KV, que aumenta efectivamente
la capacidad máxima de caché pero no aborda los aspectos fundamentales
Problema estructural de errores OOM causados por el crecimiento ilimitado de la caché. Nuestro
El método se puede aplicar además de dichas técnicas anteriores para mejorar
Optimice la inferencia de LLM en streaming de vídeo y aborde desafíos más allá
expansión simple del tamaño de la caché. Para modelos orientados a la visión, aceleradores.
como Adaptiv [43] y EXION [9] se centran principalmente en mejorar la computación
densidad. Sin embargo, estos diseños no abordan directamente el crecimiento de KV.
sobrecarga de caché exclusiva de la transmisión de video LLM.

##VIII. CONCLUSIÓN

Presentamos V-Rex, el primer acelerador de extremo a extremo y caché KVsolución de gestión adaptada para LLM en streaming de vídeo con KV dinámico
recuperación de caché. Nuestras contribuciones abarcan el algoritmo y el hardware.
niveles. A través de ReSV, un método de recuperación de KV dinámico y sin entrenamiento,
V-Rex reduce el volumen de caché de KV con una caída de precisión insignificante. para apoyar
ReSV de manera eficiente, diseñamos una unidad de hardware compacta, DRE, que admite
Computación de baja latencia y eficiencia energética. VRex permite el tiempo real
inferencia de transmisión, logrando 3.98.3 FPS en una implementación de borde con
1,9-19,7 × aceleración y 3,1-18,5 × ganancias de eficiencia energética y se extiende
a 2,6-19,7 × ganancias de aceleración y 5,9-70,6 × ganancias de eficiencia energética sobre
una GPU de servidor. A medida que la recuperación de la caché KV se vuelve cada vez más crucial en
LLM de contexto largo y streaming, creemos que V-Rex presenta un prometedor
dirección para futuras investigaciones en LLM en tiempo real y energéticamente eficiente
aceleración, particularmente para entornos de borde con recursos limitados y
implementación escalable.

##AGRADECIMIENTOS

Este trabajo fue apoyado por el Institute for Information &amp; comunicaciones
Subvención de Promoción de Tecnología (IITP) financiada por el gobierno de Corea (MSIT)
(Nº RS-2025-02264029, Integración y Validación de una IA
Sistema de inferencia y capacitación para centros de datos basado en semiconductores y No.
IITP-2025RS-2023-00256472, Escuela de Graduados en Inteligencia Artificial
semiconductores).

## REFERENCIAS

- [1] 'Serie Bg6 (m.2) - kioxia -japón', https://www.kioxia.com/en-jp/business/ssd/client-ssd/bg6.html, consultado: 2025-07-20.
- [2] 'Jetson agx orin para robótica de próxima generación - nvidia', https://www.nvidia.com/en-us/autonomous-machines/embedded-systems/jetson-orin/, consultado: 2025-07-20.
- [3] 'Gpu Nvidia a100 tensor core', https://www.nvidia.com/en-us/datacenter/a100/, consultado: 2025-07-20.
- [4] J. Chen, Z. Lv, S. Wu, K. Q. Lin, C. Song, D. Gao, J.-W. Liu, Z. Gao, D. Mao y M. Z. Shou, 'Videollm-online: Online video large language model for streaming video', en Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, 2024, págs. 18 407-18 418.
- [5] Y. Chen, A. F. AbouElhamayed, X. Dai, Y. Wang, M. Andronic, G. A. Constantinides y M. S. Abdelfattah, 'Bitmod: Bit-serial blend-ofdatatype llm aceleración', en el Simposio internacional IEEE de 2025 sobre arquitectura informática de alto rendimiento (HPCA). IEEE, 2025, págs. 1082-1097.
- [6] S. Di, Z. Yu, G. Zhang, H. Li, TaoZhong, H. Cheng, B. Li, W. He, F. Shu y H. Jiang, 'Transmisión de preguntas y respuestas en video con recuperación de caché KV en video en contexto', en The Thirteenth International Conference on Learning Representations, 2025. [En línea]. Disponible: https://openreview.net/forum?id=8g9fs6mdEG
- [7] S. Ge, Y. Zhang, L. Liu, M. Zhang, J. Han y J. Gao, 'El modelo te dice qué descartar: compresión de caché KV adaptativa para LLM', en The Twelfth International Conference on Learning Representations, 2024. [En línea]. Disponible: https://openreview.net/forum?id=uNrFpDPMyo- [8] A. Grattafiori, A. Dubey, A. Jauhri, A. Pandey, A. Kadian, A. Al-Dahle, A. Letman, A. Mathur, A. Schelten, A. Vaughan, A. Yang, A. Fan, A. Goyal, A. Hartshorn, A. Yang, A. Mitra, A. S. Korenkumar, A. Hivansvark, A. A. Rao, A. Zhang, A. Rodriguez, A. Gregerson, A. Spataru, B. Roziere, B. Biron, B. Tang, B. Chern, C. Caucheteux, C. Nayak, J. Bee, J. Marra, J. McConnell, J. Keller, C. Touret, C. Wu, C. Wong, C. Canton, C. Nicola, D. Nicola, D. Allonsius, D. Song, D. Pintz, D. Livshits, D. Wyatt, D. Esiobu, D. Choudhary, D. Mahajan, D. García-Olano, D. Perino, D. Hupkes, E. Lakomkin, E. AlBadawy, E. Lobanova, E. Dinan, E. M. Smith, F. Raden, F. Raden, F. Guz´m y Guzm. Zhang, G. Synnaeve, G. Lee, G. L. Anderson, G. Thattai, G. Nail, G. Mialon, G. Pang, G. Cucurell, H. Nguyen, H. Korevaar, H. Xu, H. Touvron, I. Zarov, I. Arrieta Ibarra, I. Kloumann, I. Misra, I. Evtimov, J. Copet, J. Zhang, J. Lee, J. Geffert, J. Vranes, J. Park, J. Mahadeokar, J. Shah, J. van der Linde, J. Billock, J. Hong, J. Lee, J. Fu, J. Chi, J. Huang, J. Liu, J. Wang, J. Yu, J. Bitton, J. Spisak, J. Park, J. J. Rocca, J. Rocca, J. Jiaxe, J. Jiaxe, Satun. K. Vasuden Alwala, K. Prasad, K. Upasani, K. Plawiak, K. Li, K. Heafield, K. Stone, K. ElArini, K. Iyer, K. Malik, K. Chiu, K. Bhalla, K. Lakhotia, L. RantalaYeary, L. van der Maaten, L. Chen, L. Jenkins, L. Martin, L. Martin, L. Madaan, L. Malo, L. Blecher, L. Landzaat, L. de Oliveira, M. Muzzi, M. Pasupuleti, M. Singh, M. Paluri, M. Kardas, M. Tsimpoukelli, M. Oldham, M. Rita, M. Pavlova, M. Kambadur, M. Lewis, M. Si, M. Singh, M. K. Habi, N. Gossan, N. Habi, Habi. N. Bashlykov, N. Bogoychev, N. Chatterji, N. Zhang, O. Duchenne, O. C ¸ elebi, P. Alrassy, P. Zhang, P. Li, P. Vasic, P. Weng, P. Bhargava, P. Dubal, P. Krishnan, P. Singh Koura, P. Xu, Qin He, Dong, R. S., R. S. Ganapathy, R. Calderer, R. Silveira Cabral, R. Stojnic, R. Raileanu, R. Maheswari, R. Girdhar, R. Patel, R. Sauvestre, R. Polidoro, R. Sumbaly, R. Taylor, R. Silva, R. Hou, R. Wang, S. Hosseini, S. Chennagh, S. Kim, S. Bell, S. S. Basa, S. S. Edunov, S. Nie, S. Narang, S. Raparthy, S. Shen, S. Wan, S. Bhosale, S. Zhang, S. Vandenhende, S. Batra, S. Whitman, S. Sootla, S. Collot, S. Gururangan, S. Borodinsky, T. Herman, T. Fowler, T. T. Georshalom, T. Sheorgiou y T. Sheorgiou. Speckbacher, 'La manada de modelos Llama 3', arXiv e-prints, pág. arXiv:2407.21783, julio de 2024.
- [9] J. Heo, A. Putra, J. Yoon, S. Yune, H. Lee, J.-H. Kim y J.-Y. Kim, 'Exion: Exploiting inter-and intra-iteration output sparsity for difusion models', en el Simposio internacional IEEE de 2025 sobre arquitectura informática de alto rendimiento (HPCA). IEEE, 2025, págs. 324-337.
- [10] S. Hong, S. Moon, J. Kim, S. Lee, M. Kim, D. Lee y J.-Y. Kim, 'Dfx: un dispositivo multi-fpga de baja latencia para acelerar la generación de texto basada en transformadores', en 2022, 55.º Simposio internacional sobre microarquitectura (MICRO) de IEEE/ACM. IEEE, 2022, págs.616-630.
- [11] J. Kim, 'El futuro de la memoria gráfica y móvil para nuevas aplicaciones', en el Simposio IEEE Hot Chips 28 (HCS) de 2016. IEEE, 2016, págs. 1-25.
- [12] M. Kim, K. Shim, J. Choi y S. Chang, 'Infinipot-v: Compresión de caché kv con restricciones de memoria para la comprensión de la transmisión de video', preimpresión de arXiv arXiv:2506.15745, 2025.- [13] M. Kim, S. Hong, R. Ko, S. Choi, H. Lee, J. Kim, J.-Y. Kim y J. Park, 'Oaken: servicio de llm rápido y eficiente con cuantificación de caché kv híbrida en línea y fuera de línea', en Actas del 52º Simposio internacional anual sobre arquitectura informática, 2025, págs.
- [14] J. Lee, H. Kim, S. Oh, M. Chun, M. Kim y J. Kim, 'Aif: Accelerating on-device llm inference mediante procesamiento in-flash', en Actas del 52º Simposio internacional anual sobre arquitectura informática, 2025, págs.
- [15] S. Lee, Y. Ro, Y. H. Son, H. Cho, N. S. Kim y J. H. Ahn, 'Comprensión de la relación potencia-rendimiento de dispositivos dram modernos energéticamente eficientes', en el Simposio internacional IEEE sobre caracterización de cargas de trabajo (IISWC) de 2017. IEEE, 2017, págs. 110-111.
- [16] W. Lee, J. Lee, J. Seo y J. Sim, ' {InfiniGen}: inferencia generativa eficiente de modelos de lenguaje grandes con gestión dinámica de caché {KV}

ment', en el 18º Simposio de USENIX sobre diseño de sistemas operativos y
Implementación (OSDI 24), 2024, págs. 155-172.

- [17] J. Li, Y. Zhang, M. Y. Hassan, T. Chafekar, T. Cai, Z. Ren, P. Guo, F. Karimzadeh, C. Reed, C. Wang y C. Gan, 'CommVQ: Cuantización vectorial conmutativa para compresión de caché KV', en la cuadragésima segunda conferencia internacional sobre aprendizaje automático, 2025. [En línea]. Disponible: https://openreview.net/forum?id=sbbyCB39HN
- [18] S. Li, Z. Yang, D. Reddy, A. Srivastava y B. Jacob, 'Dramsim3: un simulador de dram con capacidad térmica y precisión de ciclo', IEEE Computer Architecture Letters, vol. 19, núm. 2, págs. 106-109, 2020.
- [19] A. Liu, J. Liu, Z. Pan, Y. He, G. Haffari y B. Zhuang, 'Minicache: compresión de caché Kv en dimensión de profundidad para modelos de lenguaje grandes', en Actas de la 38ª Conferencia Internacional sobre Sistemas de Procesamiento de Información Neural, ser. NIPS '24. Red Hook, Nueva York, EE. UU.: Curran Associates Inc., 2025.
- [20] Y. Liu, H. Li, Y. Cheng, S. Ray, Y. Huang, Q. Zhang, K. Du, J. Yao, S. Lu, G. Ananthanarayanan, M. Maire, H. Hoffmann, A. Holtzman y J. Jiang, 'Cachegen: compresión y transmisión de caché Kv para un servicio rápido de modelos de lenguaje grande', en Actas de la Conferencia ACM SIGCOMM 2024, ser. ACM SIGCOMM '24. Nueva York, NY, EE. UU.: Association for Computing Machinery, 2024, pág. 38-56. [En línea]. Disponible: https://doi.org/10.1145/3651890.3672274
- [21] Z. Liu, J. Yuan, H. Jin, S. H. Zhong, Z. Xu, V. Braverman, B. Chen y X. Hu, 'Kivi: a tuning-free asimetric 2bit quantization for kv cache', en Actas de la 41ª Conferencia Internacional sobre Aprendizaje Automático, ser. ICML'24. JMLR.org, 2024.
- [22] Z. Mo, L. Wang, J. Wei, Z. Zeng, S. Cao, L. Ma, N. Jing, T. Cao, J. Xue, F. Yang y M. Yang, 'Lut tensor core: A software-hardware co-design for lut-based low-bit llm inference', en Actas del 52º Simposio Internacional Anual sobre Arquitectura de Computadores, ser. ISCA '25. Nueva York, NY, EE. UU.: Association for Computing Machinery, 2025, pág. 514-528. [En línea]. Disponible: https://doi.org/10.1145/3695053.3731057
- [23] S. Luna, J.-H. Kim, J. Kim, S. Hong, J. Cha, M. Kim, S. Lim, G. Choi, D. Seo, J. Kim, H. Lee, H. Park, R. Ko, S. Choi, J. Park, J. Lee y J.-Y. Kim, 'Una unidad de procesamiento de latencia: un procesador altamente escalable y optimizado para la latencia para la inferencia de modelos de lenguaje grande', IEEE Micro, vol. 44, núm. 6, págs. 17-33, 2024.
- [24] Z. Ning, G. Liu, Q. Jin, W. Ding, M. Guo y J. Zhao, 'Livevlm: Efficient online video listening via streaming-solving kv cache and retrieval', preimpresión de arXiv arXiv:2505.15269, 2025.- [25] NVIDIA Corporation, 'NVIDIA System Management Interface (nvidiasmi)', https://developer.nvidia.com/system-management-interface, 2008, consultado: 2025-07-20.
- [26] [--, 'Documentación del SDK de Linux de NVIDIA DRIVE OS 5.1', https://docs.nvidia.com/drive/drive os 5.1.6.1L/nvvib docs/index.html# page/DRIVE OS Linux SDK Development Guide/Utilities/util tegrastats.html, 2019, consultado: 2025-07-20.](https://docs.nvidia.com/drive/drive_os_5.1.6.1L/nvvib_docs/index.html#page/DRIVE_OS_Linux_SDK_Development_Guide/Utilities/util_tegrastats.html)
- [27] A. Radford, J. W. Kim, C. Hallacy, A. Ramesh, G. Goh, S. Agarwal, G. Sastry, A. Askell, P. Mishkin, J. Clark, G. Krueger e I. Sutskever, 'Aprendizaje de modelos visuales transferibles a partir de la supervisión del lenguaje natural', en Actas de la 38ª Conferencia Internacional sobre Aprendizaje Automático, ser. Actas de investigación sobre aprendizaje automático, M. Meila y T. Zhang, Eds., vol. 139. PMLR, 18-24 de julio de 2021, págs. 8748-8763. [En línea]. Disponible: https://proceedings.mlr.press/v139/radford21a.html
- [28] B. Schneider, D. Jiang, C. Du, T. Pang y W. Chen, 'Quickvideo: Comprensión de video largo en tiempo real con codiseño de algoritmo del sistema', preimpresión de arXiv arXiv:2505.16175, 2025.
- [29] X. Shen, Y. Xiong, C. Zhao, L. Wu, J. Chen, C. Zhu, Z. Liu, F. Xiao, B. Varadarajan, F. Bordes, Z. Liu, H. Xu, H. J. Kim, B. Soran, R. Krishnamoorthi, M. Elhoseiny y V. Chandra, 'LongVU: Spatiotemporal adaptive listening for long videolanguage listening', en Forty- second International Conferencia sobre aprendizaje automático, 2025. [En línea]. Disponible: https://openreview.net/forum?id=XzZC4gs1mf
- [30] Y. Sheng, L. Zheng, B. Yuan, Z. Li, M. Ryabinin, B. Chen, P. Liang, C. R´ e, I. Stoica y C. Zhang, 'Flexgen: High-throughput generative inference of large language models with a single gpu', en Conferencia internacional sobre aprendizaje automático. PMLR, 2023, págs. 31 094-31 116.
- [31] Y. Tang, D. Ding, Y. Rao, Y. Zheng, D. Zhang, L. Zhao, J. Lu y J. Zhou, 'Coin: Un conjunto de datos a gran escala para un análisis integral de videos instructivos', en Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, 2019, págs.
- [32] Y. Tang, J. Situ, A. Y. Cui, M. Wu y Y. Huang, 'Integración de Llm en realidad extendida: una revisión exhaustiva de las tendencias actuales, los desafíos y las perspectivas futuras', en Actas de la Conferencia CHI 2025 sobre factores humanos en sistemas informáticos, ser. CHI '25. Nueva York, NY, EE. UU.: Association for Computing Machinery, 2025. [En línea]. Disponible: https://doi.org/10.1145/3706598.3714224
- [33] Y. Tang, J. Bi, S. Xu, L. Song, S. Liang, T. Wang, D. Zhang, J. An, J. Lin, R. Zhu, A. Vosoughi, C. Huang, Z. Zhang, P. Liu, M. Feng, F. Zheng, J. Zhang, P. Luo, J. Luo y C. Xu, 'Comprensión de vídeo con modelos de lenguaje grandes: una encuesta', IEEE Transactions on Circuits and Systems for Video Technology, págs.1-1, 2025.
- [34] K. Tao, H. You, Y. Sui, C. Qin y H. Wang, 'Plug-and-play 1. Cuantización de caché kv de x bits para modelos de lenguaje grande de vídeo', preimpresión de arXiv arXiv:2503.16257, 2025.
- [35] A. Tavakkol, J. G´ omez-Luna, M. Sadrosadati, S. Ghose y O. Mutlu, ' { MQSim } : A framework for enable Realistic Studies of modern { Multi-Queue }{ SSD } devices', en 16.ª Conferencia USENIX sobre tecnologías de archivos y almacenamiento (FAST 18), 2018, págs. 49-66.- [36] D. Tu, D. Vashchilenko, Y. Lu y P. Xu, 'VL-cache: Sparsity and modality-aware KV cache compresión para aceleración de inferencia de modelos visión-lenguaje', en The Thirteenth International Conference on Learning Representations, 2025. [En línea]. Disponible: https://openreview.net/forum?id=HMrcv7Q4Ub
- [37] S. Wu, J. Chen, K. Q. Lin, Q. Wang, Y. Gao, Q. Xu, T. Xu, Y. Hu, E. Chen y M. Z. Shou, 'Videollm-mod: Transmisión eficiente de video-lenguaje con cálculo de visión de mezcla de profundidades', Avances en sistemas de procesamiento de información neuronal, vol. 37, págs. 109 922-109 947, 2024.
- [38] H. Xiong, Z. Yang, J. Yu, Y. Zhuge, L. Zhang, J. Zhu y H. Lu, 'Transmisión de comprensión de video e interacción de múltiples rondas con conocimiento mejorado con la memoria', en The Thirteenth International Conference on Learning Representations, 2025. [En línea]. Disponible: https://openreview.net/forum?id=JbPb6RieNC
- [39] J. Xu, J. Pan, Y. Zhou, S. Chen, J. Li, Y. Lian, J. Wu y G. Dai, 'Specee: Accelerating big language model inference with especulative early exiting', en Actas del 52º Simposio Internacional Anual sobre Arquitectura de Computadores, 2025, págs.
- [40] A. Yang, A. Li, B. Yang, B. Zhang, B. Hui, B. Zheng, B. Yu, C. Gao, C. Huang, C. Lv, C. Zheng, D. Liu, F. Zhou, F. Huang, F. Hu, H. Ge, H. Wei, H. Lin, J. Tang, J. Yang, J. Tu, J. Zhang, J. Yang, J. Yang, J. Zhou, J. Zhou, J. Lin, K. Dang, K. Bao, K. Yang, L. Yu, L. Deng, M. Li, M. Xue, M. Li, P. Zhang, P. Wang, Q. Zhu, R. Men, R. Gao, S. Liu, S. Luo, T. Li, T. Tang, W. Yin, X. Ren, X. Wang, X. Zhang, X. Ren, Y. Fan, Y. Su, Y. Zhang, Y. Zhang, Y. Wan, Y. Liu, Z. Wang, Z. Cui, Z. Zhang, Z. Zhou y Z. Qiu, 'Informe técnico de Qwen3', 2025. [En línea]. Disponible: https://arxiv.org/abs/2505.09388
- [41] D. Yang, X. Han, Y. Gao, Y. Hu, S. Zhang y H. Zhao, 'Pyramidinfer: Pyramid kv cache listening for high-throughput llm inference', en ACL (Hallazgos), 2024, págs. 3258-3270. [En línea]. Disponible: https://doi.org/10.18653/v1/2024.findings-acl.195
- [42] S. Yin, C. Fu, S. Zhao, K. Li, X. Sun, T. Xu y E. Chen, 'Una encuesta sobre modelos multimodales de lenguaje grande', National Science Review, vol. 11, núm. 12, pág. nwae403, 2024.
- [43] S. Yoo, H. Kim y J.-Y. Kim, 'Adaptiv: fusión de tokens adaptativos de imágenes basados ​​en similitud de signos para la aceleración del transformador de visión', en 2024, 57.º Simposio internacional sobre microarquitectura IEEE/ACM (MICRO). IEEE, 2024, págs. 64-77.
- [44] X. Zhai, B. Mustafa, A. Kolesnikov y L. Beyer, 'Sigmoid loss for language image pre-training', en Actas de la Conferencia Internacional IEEE/CVF sobre Visión por Computadora (ICCV), octubre de 2023, págs. 11 975-11 986.
- [45] H. Zhang, X. Li y L. Bing, 'Video-llama: un modelo de lenguaje audiovisual adaptado a las instrucciones para la comprensión de videos', en EMNLP (Demos), 2023, págs. 543-553. [En línea]. Disponible: https://doi.org/10.18653/v1/2023.emnlp-demo.49
- [46] Y. Zhang, Y. Hu, R. Zhao, J. C. S. Lui y H. Chen, 'Unificación de la compresión de caché kv para modelos de lenguaje grandes con leankv', 2025. [En línea]. Disponible: https://arxiv.org/abs/2412.03131