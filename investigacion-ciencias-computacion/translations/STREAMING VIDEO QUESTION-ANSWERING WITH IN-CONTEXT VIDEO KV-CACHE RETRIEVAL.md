## TRANSMISIÓN DE PREGUNTAS Y RESPUESTAS DE VIDEO CON RECUPERACIÓN DE CACHE KV DE VIDEO EN CONTEXTO

Shangzhe Di 1, 2 Zhelun Yu 2 Guanghao Zhang 2 Haoyuan Li 2 Tao Zhong 2
Hao Cheng 2 Bolin Li 2 Wanggui He 2 Fangxun Shu 2 Hao Jiang 2

1 Universidad Jiao Tong de Shanghai dishangzhe@sjtu.edu.cn

2 Grupo Alibaba

## RESUMEN

Proponemos ReKV, un novedoso enfoque sin formación que permite una gestión eficiente
transmisión de video de preguntas y respuestas (StreamingVQA), mediante
integrándose con los modelos de lenguaje grande de vídeo (Video-LLM) existentes.
Los sistemas VideoQA tradicionales tienen problemas con vídeos largos, ya que deben
Procese videos completos antes de responder a las consultas y repita esto.
proceso para cada nueva pregunta. Por el contrario, nuestro enfoque analiza a largo plazo
videos en forma de transmisión, lo que permite respuestas rápidas tan pronto como
Se reciben consultas de los usuarios. Partiendo de un Video-LLM común, primero
incorporar un mecanismo de atención de ventana deslizante, asegurando que la entrada
Los fotogramas atienden a un número limitado de fotogramas anteriores, reduciendo así
sobrecarga computacional. Para evitar la pérdida de información, almacenamos procesada
cachés de valores-clave de vídeo (KV-Caches) en RAM y disco, recargándolos en
Memoria GPU según sea necesario. Además, presentamos un método de recuperación que
aprovecha un recuperador externo o los parámetros dentro de Video-LLM para
recuperar solo KV-Caches relevantes para las consultas, lo que garantiza eficiencia y
precisión en la respuesta a las preguntas. ReKV permite la separación de vídeo
codificación y respuesta a preguntas en diferentes procesos y GPU,
mejorando significativamente la eficiencia de StreamingVQA. A través de
experimentación integral, validamos la eficacia y practicidad
de nuestro enfoque, que aumenta significativamente la eficiencia y mejora
Aplicabilidad sobre los modelos VideoQA existentes.

## 1 INTRODUCCIÓN

En la literatura, las tareas de comprensión de videos, como el reconocimiento de acciones
(Caba Heilbron et al., 2015; Goyal et al., 2017; Kay et al., 2017),
seguimiento visual de objetos (Huang et al., 2019; Muller et al., 2018), y
video de preguntas y respuestas (Xu et al., 2017; Jang et al., 2017; Xiao et al.,
otros, 2021; Li et al., 2024b), se han centrado principalmente en clips cortos
dura desde unos pocos segundos hasta minutos. Sin embargo, como modelos de visión
encuentran cada vez más aplicaciones en escenarios del mundo real como la robótica,
Vigilancia y transmisiones en vivo, la investigación en la comunidad de la visión.
ha evolucionado gradualmente hacia la comprensión de transmisiones de vídeo continuas,
donde los contextos a largo plazo y la interacción en tiempo real son cruciales.

En este artículo, consideramos el problema de la transmisión de video.
contestando (StreamingVQA). Como se muestra en la Figura 1 (a), implica
Procesando continuamente largas secuencias de vídeo y respondiendo rápidamente a
consultas sobre el contenido visual en cualquier momento. Puede ser tratado como un
generalización del VideoQA fuera de línea estándar, donde el modelo
procesa todo el vídeo y todas las preguntas simultáneamente. Por
Por definición, dicha tarea de StreamingVQA presenta tres desafíos centrales:
(i) Codificación de video eficiente: a diferencia del VideoQA tradicional sin conexión, donde
los modelos tienen acceso al videoclip completo, StreamingVQA exige real-
Análisis temporal de flujos continuos. Los modelos deben procesar eficientemente
fotogramas entrantes sin acceso a fotogramas futuros o revisiones frecuentes
de fotogramas del pasado lejano. (ii) Preservación del contexto del vídeo: para precisar
responder preguntas planteadas más adelante en la secuencia, los modelos deben preservar
información relevante de marcos anteriores, haciendo que el contexto a largo plazo
la retención es un desafío clave. (iii) Respuesta en tiempo real: el modelo debeproporcionar respuestas precisas con un retraso mínimo, lo que requiere eficiencia
recuperación del contexto del vídeo y respuesta rápida a preguntas.

Los Video-LLM actuales a menudo tienen dificultades para codificar transmisiones de video largas debido a
el gran volumen de tokens de video, lo que obliga a la mayoría de los modelos a procesar solo una
subconjunto escaso de fotogramas (Maaz et al., 2024; Zhang et al., 2024c; Li et al., 2024c;
al., 2024a). Esto da como resultado una duración de video limitada o una pérdida significativa.
de información visual detallada. Mientras que técnicas como la agrupación promedio
(Li et al., 2024c) y compresión de la memoria (Wu et al., 2022; Wang et al.,
2023; Él y otros, 2024; Zhang y otros, 2024a; Qian et al., 2024) reducen
volumen simbólico, tienen el costo de perder detalles, particularmente en
características visuales temporales y de nivel inferior que son esenciales para
respuesta a preguntas.

Figura 1: Descripción general de la tarea StreamingVQA y nuestro ReKV propuesto. (a) StreamingVQA requiere un modelo para procesar continuamente transmisiones de video y responder preguntas sobre contenido visto anteriormente en cualquier momento. (b) Proponemos ReKV para mejorar la eficiencia y precisión en StreamingVQA. Probado con LLaVA-OV-7B en una GPU H800 (80 GB), ReKV mantiene una latencia y un uso de memoria de la GPU estables, lo que evita errores de falta de memoria (OOM) a medida que aumentan los fotogramas. También mejora la precisión en siete puntos de referencia de VideoQA de formato largo en comparación con la línea de base de muestreo uniforme. Se proporcionan más detalles en la Sección 4.

<!-- imagen -->

Para abordar los desafíos, proponemos ReKV (Recuperar video en contexto
KV -Cache), un marco que se integra perfectamente con Video-
LLM (Maaz et al., 2024; Zhang et al., 2024c; Li et al., 2024a) sin
formación adicional. Nuestro método emplea dos estrategias para agregar
información temporal tanto a corto como a largo plazo. Para temporal a corto plazo
contexto, el modelo adopta atención causal con una ventana deslizante
mecanismo (Han et al., 2023), donde los tokens atienden solo a un conjunto limitado
de tokens anteriores durante la codificación. Para recordar información a largo plazo.
, habilitamos el acceso dinámico a cualquier punto dentro de la secuencia de vídeo a través de
recuperación. Específicamente, nuestro método retiene y reutiliza cálculos pasados.
(KV-Cache) para evitar el procesamiento redundante y al mismo tiempo mejorar la seguridad a largo plazo.
razonamiento sin sacrificar los detalles. Para vídeos extremadamente largos, KV-
Los cachés se pueden descargar a la RAM o al disco para evitar el desbordamiento de la memoria.

Para garantizar respuestas precisas y en tiempo real, recuperamos un número fijo
de KV-Caches relevantes para la pregunta actual. Este diseño impacta
equilibrio entre eficiencia y precisión al evitar la necesidad de procesar
todos los fotogramas anteriores, sin dejar de acceder a la información más crítica. Nosotros
experimentó con dos métodos de recuperación: uno usando CLIP externo
modelos (Radford et al., 2021; Zhai et al., 2023) para la coincidencia semántica,
y otro que aprovecha los pesos de atención internos para obtener resultados más rápidos y más
recuperación integrada y potencialmente más sólida (Xiao et al., 2024a; Li
et al., 2024d).

En resumen, ReKV codifica eficientemente secuencias de vídeo largas, conserva y
recupera KVCaches en contexto para abordar preguntas complejas en video.
respondiendo. Además, ReKV separa la codificación de vídeo de las preguntas.
respondiendo a procesos distintos, mejorando aún más la eficiencia. como
Como se muestra en la Figura 1 (b), ReKV mejora la precisión de VideoQA mientras mantiene
latencia de inferencia estable y uso de memoria a medida que aumentan los fotogramas. el
El resto del documento está organizado de la siguiente manera: la Sección 5 proporciona una
descripción general de la literatura relevante. La sección 3 formula laTarea StreamingVQA y describe nuestro método propuesto en detalle. en
Sección 4, presentamos estudios de ablación y comparaciones para validar nuestra
enfoque. En consecuencia, nuestro enfoque no sólo mejora la precisión en largas
Puntos de referencia de VideoQA, incluidos MLVU (Zhou et al., 2024a), QAEGO4D MC (Di
&amp; Xie, 2024), EgoSchema (Mangalam et al., 2023) y ActivityNet-QA (Yu
et al., 2019), así como los puntos de referencia de StreamingVQA (Zhang et al., 2024a),
pero también reduce la latencia de inferencia y el uso de memoria.

## 2 STREAMINGVQA: DEFINICIÓN Y DISCUSIÓN DE TAREAS

Este artículo considera el problema de la transmisión de video de preguntas y respuestas (
StreamingVQA), donde un modelo procesa continuamente una transmisión de video y
Puede responder preguntas sobre contenido visual pasado en cualquier momento. en
En esta sección, definimos formalmente la tarea y delineamos el diseño.
principios para nuestra solución propuesta.

Dado un flujo de vídeo V T := [ v 1 , v 2 , ..., v T ] que consta de T
marcos y un conjunto de N preguntas Q := { q 1 , q 2 , . . . , q norte } ,
StreamingVQA tiene como objetivo responder una pregunta q i en cualquier momento del paso t (1 ≤ t ≤ T
), usando solo los fotogramas vistos hasta ese punto, V t := [ v 1 , v 2 ,
..., v t ] .

Discusión-I: StreamingVQA frente a OfflineVQA. StreamingVQA implica
Analizar continuamente una transmisión de video entrante y responder preguntas.
en función del contenido visual observado en cualquier momento. En contraste,
modelos convencionales de preguntas y respuestas en vídeo (Yang et al., 2022; Maaz et
otros, 2024; Zhang y otros, 2024c; Li et al., 2024a) operan fuera de línea
modo, denominado OfflineVQA. Los dos paradigmas se diferencian en que: 1)
StreamingVQA procesa una transmisión de video continua, mientras que OfflineVQA
maneja una entrada de video predefinida, y 2) StreamingVQA permite preguntas
que se le solicite en cualquier momento durante la transmisión, mientras que OfflineVQA procesa
preguntas sólo después de haber visto el vídeo completo. En particular,
OfflineVQA puede considerarse un caso especial de StreamingVQA, donde todos
Las preguntas se plantean después de que el video se haya procesado por completo.

Los enfoques convencionales suelen emplear un codificador visual (Radford et
otros, 2021; Zhai y otros, 2023; Fang et al., 2023) y un módulo de proyección
(Zhang et al., 2024c; Li et al., 2023) para procesar fotogramas de vídeo ( V t ).
El resultado se concatena con la pregunta tokenizada para formar un
secuencia [ V t , q i ] 1 , que luego se pasa a un LLM para predecir una
respuesta. Sin embargo, este enfoque no es práctico debido a la alta
Costo computacional asociado con el procesamiento de una gran cantidad de cuadros (
T).

Una solución alternativa común es el muestreo de marcos dispersos (Maaz et al., 2024; Zhang
et al., 2024c; Li et al., 2024a), pero esto introduce nuevos problemas: (i)
pérdida de información visual crítica, lo que lleva a resultados incompletos o inexactos.
respuestas, y (ii) la necesidad de reprocesar marcos para diferentes
preguntas, ya que las preguntas formuladas en diferentes momentos requieren
muestras de marcos distintas. Esto se vuelve cada vez más ineficiente a medida que T y N
crecer.

Dados estos desafíos, los métodos OfflineVQA actuales se quedan cortos cuando
aplicado a escenarios de StreamingVQA. Por lo tanto, diseñar un nuevo enfoque
optimizado para StreamingVQA es crucial para manejar más transmisiones de video
eficientemente, permitiendo responder preguntas en tiempo real y desbloquear más
Aplicaciones interactivas de análisis de vídeo.

Discusión-II: Principios de diseño para una transmisión eficiente VQA. para abordar
los desafíos antes mencionados, podemos explotar la naturaleza causal de la
Decodificador LLM para evitar cálculos redundantes y lograr un equilibrio entre
precisión y velocidad. Durante los cálculos de atención, enmascaramiento causal.evita que el modelo acceda a tokens futuros, asegurando que el video
Las fichas se codifican independientemente de las preguntas. Esto nos permite
desacoplar la codificación de vídeo de la respuesta a preguntas.

Para la codificación de video, aprovechamos la optimización KV-Cache para acelerar
inferencia. Sin embargo, a medida que aumenta el número de fotogramas, el manejo de los
Una gran cantidad de tokens de video se vuelve cada vez más ineficiente y puede
exceder la capacidad del modelo (Han et al., 2023; Xiao et al., 2024b). a
Para abordar esto, adoptamos un mecanismo de atención de ventana deslizante (Han et al.,
2023), lo que limita el alcance de la atención solo a los fotogramas más recientes.

En cuanto a la respuesta a preguntas, los Video KV-Caches se almacenan y se pueden
reutilizado como contexto para responder diferentes preguntas. Sin embargo, video largo.
secuencias producen una cantidad sustancial de KV-Caches, lo que lleva a
Consumo excesivo de memoria de la GPU, sobrecarga computacional y
distracciones innecesarias si se utilizan todas. Para abordar esto, presentamos
un método de recuperación eficiente que selecciona la clave de video más relevante.
vectores de valor del vídeo KV-Caches. Estos vectores seleccionados entonces
Sirve como contexto, permitiendo StreamingVQA eficiente y escalable.

1 Mantenemos la notación original por simplicidad.

Figura 2: Descripción general de ReKV. Modificamos el mecanismo de atención en los Video-LLM basados ​​en decodificadores: (a) La transmisión de video se codifica con atención de ventana deslizante (Ecuación 1), con Video-Caches KV fuera de la ventana descargados a la RAM o al disco. (b) Al recibir una pregunta, los vectores clave-valor relevantes se recuperan en función de la similitud del coseno, con vectores comprimidos para acelerar la recuperación (Ecuación 2). (c) Los vectores clave-valor recuperados se recargan en la GPU y se utilizan para la generación de respuestas autorregresivas (Ecuación 3).

<!-- imagen -->

## 3 REKV: RECUPERAR KV-CACHE DE VIDEO EN CONTEXTO

Esta sección presenta ReKV, un enfoque que se integra perfectamente
con un Video-LLM para permitir StreamingVQA eficiente sin necesidad
formación adicional. En general, ReKV codifica eficientemente la transmisión de video,
mantiene sus KV-Caches, recupera cachés relevantes en función de la información dada
pregunta y los utiliza para responder preguntas con precisión.

Nuestro objetivo es permitir que los Video-LLM, capacitados en fotogramas limitados, realicen
StreamingVQA sin formación adicional. Como se muestra en la Figura 2, el
ReKV propuesto tiene tres componentes: codificación de flujo de video, video KV-
Recuperación de caché y respuesta a preguntas utilizando el valor-clave recuperado
vectores.

Codificación de secuencias de vídeo con atención de ventana deslizante. Codificamos el vídeo.
transmitir VT de forma incremental, procesándolo fragmento a fragmento. En cada paso,
las entradas incluyen vectores de valores clave pasados P = { ( k j , v j ) } l P j =1
y los tokens actuales X = { t i + l P } l X i =1 , donde l P denota
las longitudes de los valores-clave anteriores, y l X se refiere al tamaño del fragmento. el
los vectores clave-valor locales dentro de una ventana l L pueden derivarse entonces como L = P
[ l P -l L +1: l P ] . El cálculo de la atención se formula entonces como:

<!-- fórmula-no-decodificada -->

donde W Q, W K y W V son los parámetros de la capa de atención, L k y L
v corresponden a los vectores clave y de valor en L . Todos los vídeos KV-Caches son
almacenado para su futura recuperación. Para vídeos extremadamente largos, gestionamos la memoria.
restricciones descargando KV-Caches a la RAM o al disco, como en (Xiao et al.,
2024a).

Recuperación de caché KV de vídeo externo. Aquí utilizamos un CLIP externo.
modelo similar (Radford et al., 2021; Zhai et al., 2023) para recuperar
vídeo KV-Cache relevante para preguntas, principalmente como punto de referencia para evaluarsi la recuperación puede mejorar el rendimiento de VideoQA, como se demuestra en
Sección 4. Específicamente, un modelo tipo CLIP transforma cada cuadro de video.
en un vector v = f v ( v ) ∈ R D , donde f v representa lo visual
codificador, D denota la dimensión del vector. Del mismo modo, la pregunta es
codificado como q = f t ( q ) ∈ R D , donde f t es el codificador de texto. nosotros entonces
calcular la similitud del coseno entre las incrustaciones de marco y
pregunta:

<!-- fórmula-no-decodificada -->

donde τ es un parámetro de temperatura que se puede aprender. Esta similitud es
calculado a nivel de marco, en lugar de a nivel de token.
Alternativamente, podemos agrupar b fotogramas consecutivos en bloques mediante
promediando sus vectores de cuadros y luego calculando la similitud a nivel de bloque
puntuaciones. Finalmente, los r fotogramas de vídeo más relevantes o ⌈ r/b ⌉ vídeo
Se recuperan los bloques. El vídeo KV-Cache correspondiente, indicado como R,
Posteriormente se carga en la GPU para responder preguntas.

Recuperación interna de caché KV de vídeo. Aprovechando los recientes avances en
manejo de secuencias largas con LLM (Xiao et al., 2024a; Li et al., 2025;
Fountas et al., 2025), exploramos más a fondo el uso de capas de autoatención
dentro de Video-LLM para su recuperación. Similar a la recuperación externa, interna
la recuperación todavía se realiza a nivel de fotogramas o bloques de vídeo.

Durante el modelado de video, el promedio de los vectores clave de un cuadro es
calculado como su vector de marco representativo: v = 1 N f ∑ N f j =1 k j ∈
R D ′ , donde N f es el número de tokens por cuadro y k j es el j
-ésimo vector clave. Para reducir la sobrecarga computacional, no
diferenciar entre cabezas de atención y en su lugar concatenarlas en
un solo vector, con D ′ como dimensión resultante. De manera similar, el
El vector de pregunta se calcula como q = 1 N q ∑ N q k =1 q k ∈ R D ′ , donde
N q es el número de tokens en la pregunta y q k es su k -ésima consulta
vector. El cálculo de similitud y la recuperación de vídeo KV-Cache son
idéntico al de recuperación externa, excepto que τ se establece en 1.

Tenga en cuenta que la recuperación interna ofrece varias ventajas sobre la externa.
recuperación. Primero, opera independientemente dentro de cada auto-atención.
capa, lo que permite que diferentes capas recuperen diferentes bloques de video. 2
Esto permite una captura más amplia del contexto del vídeo. Además,
La recuperación interna reutiliza representaciones ocultas ya calculadas y
no introduce parámetros adicionales, lo que reduce el cálculo
gastos generales en comparación con la recuperación externa.

Respuesta a preguntas utilizando KV recuperado. Los KV-Caches de vídeo recuperados
Sirve como contexto para las preguntas y respuestas en vídeo. Formalmente, el
El cálculo de la atención se formula como:

<!-- fórmula-no-decodificada -->

donde X representa los tokens de pregunta o el token actual que se está
decodificado, y R k y R v son los vectores clave y de valor del contexto,
que incluye el video recuperado, la pregunta y los generados previamente
fichas.

Codificación posicional. Nuestros Video-LLM básicos emplean la posición rotativa
Incrustaciones (RoPE) (Su et al., 2024), una posición relativa de uso común
método de codificación. Nuestro proceso de codificación de transmisión de video sigue LM-
Infinite (Han et al., 2023), donde RoPE opera normalmente dentro del
ventana local pero está limitada por un "techo de distancia" para más distantes
fichas. Para las respuestas a preguntas, no tenemos en cuenta el original.
posiciones de los KV-Caches recuperados, como el manejo de distancias invisibles entre
tokens presenta desafíos importantes (Han et al., 2023). En cambio, nosotros
Trate estos tokens recuperados como tokens consecutivos normales. Nosotros tambiénexperimentó con una variación estática de Inf-LLM (Xiao et al., 2024a),
donde a todos los tokens recuperados se les asigna la misma posición. Nuestros resultados
muestran que la aplicación de RoPE estándar a los tokens de vídeo recuperados conduce a
mejor rendimiento, probablemente debido a la importancia de capturar datos temporales
Información en video comprensión.

## 4 EXPERIMENTOS

## 4.1 REFERENCIA Y MÉTRICAS

MLVU dev-mc (Zhou et al., 2024a) es el subconjunto de opción múltiple del
Punto de referencia de desarrollo MLVU. Se centra en evaluar el vídeo de formato largo.
comprensión de los MLLM. Los pares de preguntas y respuestas se etiquetan manualmente.
y se puede dividir en 3 grupos: un solo detalle, varios detalles y
holístico. La métrica de evaluación es Precisión.

QAEGO4D test-mc (Di &amp; Xie, 2024) es el subconjunto de opción múltiple del
Prueba comparativa QAEGO4D, centrada en la respuesta a preguntas largas
Vídeos egocéntricos. Incluye anotaciones que marcan segmentos de vídeo.
relevante para cada pregunta. La métrica de evaluación es Precisión.

EgoSchema (Mangalam et al., 2023) es un punto de referencia de diagnóstico a largo plazo.
VideoQA, que presenta más de 5000 preguntas de opción múltiple y tiempo prolongado
longitud del certificado. Desafía los modelos de IA a largo plazo.
comprensión, ya que los modelos actuales de última generación logran significativamente
menor precisión en comparación con el desempeño humano.

ActivityNet-QA (Yu et al., 2019) abarca pares de control de calidad anotados por humanos en
5.800 vídeos derivados de ActivityNet (Caba Heilbron et al., 2015)
conjunto de datos. Este punto de referencia está diseñado para evaluar las capacidades de
Modelos VideoQA en el razonamiento espaciotemporal a largo plazo. Nuestra evaluación
La metodología se alinea con la de Video-ChatGPT (Maaz et al., 2024).
empleando GPT-3.5-turbo-0613 para juzgar la precisión del abierto
Respuestas de control de calidad de vídeo.

2 Por simplicidad, omitimos el índice de capa en la explicación anterior.

RVS-Ego y RVS-Movie (Zhang et al., 2024a) son Streaming VideoQA
puntos de referencia, construidos utilizando 10 videos largos del conjunto de datos Ego4D
(Grauman et al., 2022) y 22 vídeos largos del conjunto de datos MovieNet
(Huang et al., 2020), respectivamente. Estos puntos de referencia cuentan con opciones abiertas.
preguntas combinadas con marcas de tiempo, que se generan inicialmente por
GPT-4V (OpenAI, 2023b) y GPT4 (OpenAI, 2023a), y posteriormente
refinado mediante filtrado manual.

Tabla 1: Resumen de los puntos de referencia de evaluación. MC significa VideoQA de opción múltiple, mientras que OE se refiere a VideoQA abierto.| Punto de referencia | Duración |   #Vídeos |    #QA | Tipo ||-----------------|------------|-----------|--------|--------|
| MLVU dev-mc | 12 minutos |     1.242 |  2.175 | MC || Prueba QAEGO4D-mc | 8,3 minutos |       148 |    500 | MC || Esquema del ego | 3 minutos |     5.031 |  5.031 | MC || ActivityNet-QA | 2 minutos |       800 |  8.000 | Equipo original || RVS-Ego | 60 minutos |        10 |  1.465 | Equipo original || Película RVS | 30 minutos |        22 |  1.905 | Equipo original || CGBenco mc | 27 minutos |     1.219 | 12.129 | MC |CGBench mc (Chen et al., 2025a), el subconjunto de opción múltiple de CGBench,
está diseñado para responder preguntas basadas en pistas en videos largos. eso
se centra en la capacidad de recuperar pistas relevantes para preguntas, haciendo
Es un banco de pruebas ideal para ReKV.

## 4.2 DETALLES DE IMPLEMENTACIÓN

Evaluamos principalmente nuestro enfoque integrándolo en LLaVA-OV-0.5B
y LLaVA-OV-7B (Li et al., 2024a), elegidos por su simplicidad y
fuerte desempeño. En el Apéndice, realizamos experimentos con varios
otros Video-LLM como validaciones adicionales.

Todos los experimentos se realizan en GPU NVIDIA A100 (80 GB) con FP16.
Precisión. Para el modelado de video, procesamos la transmisión de video a 0,5 FPS,
de acuerdo con las pruebas de GPT-4o en MLVU (Zhou et al., 2024a). el local
El tamaño de la ventana está establecido en 15K. Para la recuperación de vídeo externo KV-Cache, utilizamos
SigLIP-SO400M (Zhai et al., 2023) como recuperador. Para KV interno
Recuperación de caché, establecemos el tamaño del bloque ( b ) en 1 y el número de
fotogramas recuperados ( r ) a 64 de forma predeterminada, con hiperparámetro adicional
variaciones exploradas en la Sección 4.3.

A menos que se especifique lo contrario, ReKV se refiere al uso de vídeo interno KV-
Recuperación de caché.

## 4.3 ABLACIONES

En esta sección realizamos estudios de ablación sobre la efectividad de
recuperación de contexto, número de fotogramas recuperados y tamaño del bloque.

Efectividad de la recuperación en contexto. Los experimentos en la prueba QAEGO4D.
mc, como se presenta en la Tabla 2, demuestra los efectos de varios
Métodos de recuperación sobre la precisión y recuperación de VideoQA. La métrica de recuperación,
definido como el porcentaje de fotogramas de vídeo relevantes para la pregunta recuperados,
muestra una fuerte correlación positiva con el rendimiento de VideoQA: mayor
recordar consistentemente conduce a una mayor precisión. Muestreo uniforme, que
selecciona escasamente cuadros, logra la recuperación más baja y, en consecuencia,
la precisión de VideoQA más pobre. Por el contrario, Oracle Retrieval, con
recuperación perfecta, ofrece la mayor precisión de VideoQA, significativamente
superando el muestreo uniforme. Mientras que la recuperación externa e interna
no alcanzan la precisión a nivel de Oracle, ambos superan el muestreo uniforme,
con Internal Retrieval sobresaliendo debido a su mayor recuperación.

Tabla 2: Estudio de ablación en QAEGO4D test-mc. 'Oracle Retrieval' se refiere a un escenario en el que los segmentos de vídeo anotados y relevantes para las preguntas se utilizan como entrada, con un muestreo uniforme de hasta 16 fotogramas. Esta configuración, por definición, tiene una recuperación del 100 % y define el rendimiento superior de VideoQA.| Método de recuperación |   VideoQA Acc. |   Recordar ||--------------------|----------------|----------|
| LLaVA-OV-0.5B      |                |          || Muestreo uniforme |           42,6 |      6.1 || Recuperación externa |           48,0 |     58.1 || Recuperación interna |           50,0 |     63,4 || Recuperación de Oráculo |           52,0 |      100 || LLaVA-OV-7B |                |          || Muestreo uniforme |           53,0 |      6.1 || Recuperación externa |           54,2 |     58.1 || Recuperación interna |           56,0 |     70,5 || Recuperación de Oráculo |           64,4 |      100 |Figura 3: Estudio de ablación de hiperparámetros de recuperación: (a) número de fotogramas recuperados y (b) número de fotogramas por bloque de recuperación. Los experimentos se realizan con LLaVA-OV-7B.

<!-- imagen -->

Tabla 3: Estudio de ablación en MLVU dev-mc. Los experimentos se basan en LLaVA-OV-7B.| Tarea | Detalle único | Detalle único | Detalle único | Detalle múltiple | Detalle múltiple | Holístico | Holístico |   Promedio ||--------------------|-----------------|-----------------|-----------------|----------------|----------------|------------|------------|--------|
|                    | Aguja | Yo | PlotQA | Orden | Contar | Tema | Anomalía |        || Muestreo uniforme | 74.1 | 59,7 | 69,8 | 45,9 | 32,0 | 87,9 | 72,0 |   64,7 || Recuperación externa | 78,6 | 69,6 | 71,6 | 40.2 | 37,9 | 84,5 | 63,0 |   66,3 || Recuperación interna | 75,8 | 66,6 | 76,3 | 45,2 | 36,9 | 90.1 | 74,5 |   68,5 |El punto de referencia MLVU (Zhou et al., 2024a) abarca tres tipos de
Tareas de VideoQA: el detalle único requiere identificar un único gráfico crítico
dentro de un vídeo largo, Multi Detail requiere la integración de
múltiples tramas, y Holistic exige una comprensión integral de
todo el vídeo. Esto hace de MLVU una plataforma ideal para evaluar nuestra
método de recuperación en contexto. Como se muestra en la Tabla 3, tanto los externos como los
La recuperación interna mejora la precisión general de VideoQA sobre el uniforme
Línea base de muestreo. Las mejoras son más pronunciadas en Detalle único.
tareas, lo que demuestra que ReKV recupera eficazmente preguntas relevantes
contexto del vídeo. Además, la recuperación interna supera significativamente
Recuperación externa en tareas holísticas, probablemente debido a su capacidad para
Capture un contexto más amplio y aproveche el modelado de video de Video-LLM.
capacidades, como se analiza en la Sección 3.

Número de fotogramas recuperados. Arreglamos el tamaño del bloque ( b = 1 ) y evaluamos
el impacto de variar el número de fotogramas recuperados ( r ∈ { 8 , 16 ,
32, 48, 64, 80}) en los puntos de referencia QAEGO4D y MLVU. Como se ilustra
En la Figura 3 (a), generalmente aumenta el número de fotogramas recuperados.
mejora la precisión de VideoQA, ya que implica capturar imágenes más relevantes
contexto. Sin embargo, en MLVU, esta mejora se estabiliza a medida que se agregan más fotogramas.
recuperarse ya que la información adicional irrelevante dificulta la
proceso posterior de respuesta a preguntas. Además, recuperar más
Los marcos aumentan la sobrecarga computacional del sistema de preguntas y respuestas.
etapa, lo que ralentiza aún más la inferencia.

Tamaño del bloque de recuperación. Al procesar transmisiones de video, agrupamos b
fotogramas consecutivos en bloques para su recuperación a nivel de bloque. Para esto
experimento, fijamos el número de fotogramas recuperados en r = 64 y evaluamos
diferentes tamaños de bloques ( b ∈ 1 , 2 , 4 , 8 , 16 ). Con una r fija,
Los tamaños de bloque más grandes dan como resultado menos bloques recuperados y más concentrados.
La Figura 3 (b) muestra que aumentar el tamaño del bloque afecta negativamente la precisión
en MLVU, mientras que el rendimiento en QAEGO4D se mantiene relativamente estable. esto
sugiere que las tareas MLVU se benefician de la recuperación de elementos visuales más dispersos.
señales, alineándose con su diseño de tareas holísticas y de múltiples detalles (Zhou
et al., 2024a). Por el contrario, QAEGO4D se basa principalmente en un único
clip relevante por pregunta (Di &amp; Xie, 2024).

## 4.4 PREGUNTAS Y RESPUESTAS DE VIDEO SIN CONEXIÓN

La comprensión de la transmisión de video es un área relativamente poco explorada, con
puntos de referencia StreamingVQA limitados disponibles (Zhang et al., 2024a). como
Como se analiza en la Sección 2, OfflineVQA puede considerarse como un caso especial
de StreamingVQA. Por lo tanto, primero evaluamos nuestro método fuera de línea.
configuración utilizando cuatro puntos de referencia VideoQA de formato largo ampliamente adoptados,
comparando nuestros resultados con los métodos VideoQA de última generación. un resumen
de estos puntos de referencia se pueden encontrar en la Tabla 1.

Tabla 4: Preguntas y respuestas en vídeo sin conexión sobre cuatro puntos de referencia de formato largo. 'Acc.' denota precisión y 'Puntuación' es la calificación de respuesta abierta de gpt-3.5-turbo-0613 en una escala del 1 al 5.| Método | MLVU | QAEGO4D | Esquema del ego | ActivityNet-QA | ActivityNet-QA ||-------------------------------------|-------------|-------------|-------------|------------------|------------------||                                           | desarrollador Acc.    | prueba Acc.   | Acc.        | Acc.             | Puntuación || GPT-4V (OpenAI, 2023b) | 49,2 | - | - | 57,0 | - || GPT-4o (OpenAI, 2024) | 64,6 | - | - | - | - || Gemini-1.5-Flash (Equipo et al., 2023) | - | - | 65,7 | 55,3 | - || Gemini-1.5-Pro ​​​​(Equipo et al., 2023) | - | - | 72,2 | 57,5 | - || Video-ChatGPT-7B (Maaz et al., 2024) | 31.3 | - | - | - | - || LLaMA-VID-7B (Li et al., 2024c) | 33.2 | -| -| 47,4 | 3.30 || MiniGPT4-Video-7B (Ataallah et al., 2024) | 44,5 | -| -| 44,3 | 3.35 || Vídeo-LLaVA-7B (Lin et al., 2024) | 47,3 | -| -| -| -|| LongVA-7B (Zhang et al., 2024b) | 56,3 | - | - | 50,0 | - || VideoStreaming (Qian et al., 2024) | - | - | 44.1 | - | - || Flash-VStream-7B (Zhang et al., 2024a) | 50,2 | 38,2 | 38.1 | 51,9 | 3.40 || LLaVA-OV-0.5B (Li et al., 2024a)          | 53.2        | 42.6        | 29.6        | 50.5             | 3.02             || +ReKV (0,5 FPS → 64 fotogramas) | 56,1 (+2,9) | 50,0 (+7,4) | 31,0 (+1,4) | 52,1 (+1,6) | 3,15 (+,13) || LLaVA-OV-7B (Li et al., 2024a) | 64,7 | 52,8 | 59,8 | 56,6 | 3.29 || +ReKV (0,5 FPS → 64 fotogramas) | 68,5 (+3,8) | 56,0 (+3,2) | 60,7 (+0,9) | 60,4 (+3,8) | 3,52 (+,23) |Tabla 5: Resultados del benchmark StreamingVQA. Todos los métodos se prueban en condiciones idénticas. 'Video adjunto.' Son fotogramas codificados por segundo. La "latencia" se mide desde la entrada de la pregunta hasta la finalización de la respuesta. 'GPU' indica el uso máximo de memoria de la GPU y 'KV-Cache' se refiere al tamaño de KV-Cache de vídeo descargado por hora.| Método de recuperación | RVS-Ego | RVS-Ego | Película RVS | Película RVS | Velocidad de carrera | Velocidad de carrera | Uso de la memoria | Uso de la memoria ||--------------------|-----------|-----------|-------------|-------------|-----------------|-----------------|----------------|----------------|
|                    | Acc.      | Puntuación | Acc.        | Puntuación | Vídeo adjunto.      | Latencia | GPU | Caché KV || Flash-VStream-7B | 57,3 | 4.0 | 53.1 | 3.3 | 14 FPS | 2,4s | 20GB | - || Muestreo uniforme | 56,2 | 3.7 | 43,0 | 3.3 | - | 2,9s | 21GB | - || Recuperación externa | 62,4 | 3.9 | 53,6 | 3.5 | 11 FPS | 5,8s | 55 GB | 18,8 GB/h || Recuperación interna | 63,7 | 4.0 | 54,4 | 3.6 | 11 FPS | 3,3s | 38GB | 18,8 GB/h || Muestreo uniforme | 51,8 | 3.7 | 37,2 | 3.2 | - | 2,5 s | 7 GB | - || Recuperación externa | 54.1 | 3.8 | 44,7 | 3.4 | 17 FPS | 4,1s | 37 GB | 4,0 GB/h || Recuperación interna | 54,7 | 3.9 | 44,6 | 3.4 | 17 FPS | 1,6s | 19 GB | 4,0 GB/h |Como se muestra en la Tabla 4, nuestro ReKV propuesto siempre mejora el rendimiento
de LLaVA-OV-0.5B y LLaVA-OV-7B sin formación adicional. En particular,
LLaVA-OV-7B +ReKV supera a dos modelos StreamingVQA basados en memoria
(VideoStreaming (Qian et al., 2024) y FlashVStream (Zhang et al.,
2024a)) por un amplio margen. Si bien el modelo base ya demuestra
desempeño sólido, y no reclamamos el crédito por este logro, nuestro
El método puede integrarse perfectamente con Video-LLM, beneficiándose de su
avances continuos.

## 4.5 PREGUNTAS-RESPUESTAS DE VIDEO EN TRANSMISIÓN

Luego evaluamos nuestro método en la configuración de transmisión usando el RVS-Ego.
y puntos de referencia RVS-Movie. Durante el modelado de secuencias de video, las preguntas son
entrada inmediatamente después de sus marcas de tiempo de finalización anotadas y respondidas
basado en el contenido del vídeo anterior.

Rendimiento de respuesta a preguntas. La Tabla 5 presenta el StreamingVQA
rendimiento. Tanto los métodos de recuperación externos como internos reducen significativamente
superar la línea de base de muestreo uniforme. Además, nuestro enfoque
permite que LLaVA-OV-7B supere a Flash-VStream (Zhang et al., 2024a),
demostrando la efectividad de ReKV para StreamingVQA.

Velocidad de carrera y uso de memoria. También examinamos la velocidad de carrera y
Uso de la memoria en condiciones controladas. Específicamente, una hora, 1080P
Se utiliza un vídeo de RVS-Ego con 100 preguntas dispersas. cada pregunta
se completa con 64 tokens y las respuestas generadas se fijan en 128
fichas de longitud. Los fotogramas de vídeo se extraen previamente a 0,5 FPS (1.800
cuadros en total) y se transmiten al Video-LLM cuadro por cuadro.

Figura 4: Ejemplos cualitativos de StreamingVQA. El ejemplo está extraído del punto de referencia QAEGO4D. La transmisión de vídeo se procesa cuadro por cuadro. y marque las marcas de tiempo en las que se plantean las preguntas. □ y □ indican los contextos de vídeo relevantes que respaldan la respuesta a estas preguntas.

<!-- imagen -->

Como se ilustra en la Tabla 5, ambos métodos de recuperación mantienen un alto nivel de vídeo.
velocidades de codificación, con LLaVA-OV-7B alcanzando 11 FPS y LLaVA-OV-0.5B
logrando 17 FPS. Además, la descarga de KVCache sigue siendo manejable, con
LLaVA-OV-7B a 18,8 GB/h y LLaVA-OV-0.5B a 4,0 GB/h (consulte el apéndice para
más detalles). La recuperación externa, sin embargo, introduce una mayor latencia
y uso de memoria de GPU debido a cálculos adicionales en el externo
retriever, mientras que la recuperación interna reduce significativamente ambos. Figura
1 también ha demostrado que la latencia y el uso de la memoria de la GPU se mantienen estables.
a medida que se procesan más fotogramas. Flash-VStream también muestra una buena eficiencia.
Sin embargo, sólo mantiene una huella de memoria relativamente pequeña (681
tokens) (Zhang et al., 2024a), lo que lleva a una posible pérdida de información
cuando se trata de vídeos extremadamente largos.

Ejemplos cualitativos. La Figura 4 presenta un ejemplo de transmisión de video.
respuesta a preguntas. Nuestro enfoque procesa continuamente transmisiones de video.
mientras responde a preguntas planteadas en diferentes momentos. para mejorar
eficiencia, almacena y recupera videos KV-Caches relevantes como
información contextual para responder estas preguntas.

Proporcionamos detalles de implementación adicionales y resultados experimentales en
el Apéndice.

## 5 TRABAJOS RELACIONADOS

LLM para la comprensión de videos. En los últimos años ha habido un aumento de
Interés en aprovechar los modelos de lenguaje grande (LLM) para vídeo.
comprensión, lo que lleva al desarrollo de varias soluciones innovadoras.
enfoques (Maaz et al., 2024; Zhang et al., 2024c; Li et al., 2024a).
Estos modelos suelen utilizar un codificador Vision para extraer funciones de vídeo,seguido de un paso de mapeo con proyección lineal, MLP o Q-Former (Li
et al., 2023). Las características mapeadas se combinan con datos textuales y
introducido en modelos de lenguaje grandes (LLM) para generar una salida de texto. Estos
Los modelos tienen arquitecturas relativamente simples, que requieren menos capacitación.
datos y recursos computacionales, pero logran un sólido rendimiento en
Puntos de referencia de comprensión de videos cortos (Xu et al., 2017; Xiao et al.,
2021; Li et al., 2024b). Sin embargo, emplean muestras dispersas o token
técnicas de compresión para reducir el número de tokens, lo que puede resultar
en una pérdida significativa de información cuando se trata de más o más
vídeos ricos en contenido. Como resultado, no son adecuados por mucho tiempo.
comprensión de vídeo o comprensión de vídeo en streaming.

Comprensión de videos largos. Un desafío central en un vídeo largo
comprender es comprimir efectivamente la información de largos
vídeos. Muchos enfoques utilizan el lenguaje como puente, condensando vídeos en
subtítulos densos (Zhang et al., 2023; Islam et al., 2024; Zhou et al.,
2024b). Si bien esto logra buenos resultados en algunos casos, comprimir
contenido de vídeo en texto a menudo conduce a la pérdida de elementos visuales cruciales.
detalles. Además, como enfoque pionero en la transmisión de vídeo
comprensión, VideoLLM-Online (Chen et al., 2024) emplea un sistema de datos
Metodología centrada en la intercalación de vídeo y texto durante la formación. en
Por el contrario, nuestro enfoque no requiere capacitación, lo que permite una integración perfecta.
con varios Video-LLM existentes para ampliar su StreamingVQA
capacidades. Además, VideoLLM-Online conserva solo un token
por cuadro para manejar videos largos, lo que puede resultar en información visual
pérdida. Nuestro método conserva información visual completa y aprovecha In-
Recuperación de caché KV de contexto para mejorar la eficiencia.

Otra línea de investigación se centra en comprimir vídeos largos en un
banco de memoria (Wu et al., 2019; 2022; Wang et al., 2023). MC-ViT
(Balazevic et al., 2024) adapta transformadores de vídeo previamente entrenados
afinándolos para atender a recuerdos visuales condensados. se relaciona
estrechamente con el video basado en memoria, poda y fusión de tokens
métodos de comprensión. En comparación, proponemos un método sin formación.
específicamente diseñado para la tarea StreamingVQA. Incorporación de MC-ViT
en la tarea StreamingVQA podría ser una vía interesante para el futuro
investigación y reconocemos su potencial en este ámbito. Este enfoque
se ha integrado en Video-LLM para la comprensión de la transmisión de video,
como se muestra en trabajos como VideoStreaming (Qian et al., 2024) y Flash-
VStream (Zhang et al., 2024a). Estos métodos actualizan dinámicamente el
memoria durante el procesamiento de vídeo y utilizarla para tareas posteriores.
A pesar de su innovación, una limitación importante de estos métodos es su
no tener en cuenta la duración del vídeo y la densidad de la información, especialmente
cuando se utiliza un tamaño de memoria fijo. Por ejemplo, Flash-VStream comprime
Tanto clips de 10 segundos como vídeos de una hora de duración en los mismos 681 tokens.
Además, estos métodos carecen de interpretabilidad, lo que dificulta
determinar cuánta información se está comprimiendo en la memoria o
si la información relevante del vídeo se recupera con precisión durante
tareas posteriores.

En la búsqueda de una mayor interpretabilidad en la comprensión de vídeos largos,
métodos como GroundVQA (Di &amp; Xie, 2024) y GeLM (Chen et al., 2025b)
Abogar por localizar clips de video relevantes mientras responde al usuario.
consultas. Inspirándose en ellos, este trabajo se abstiene decondensar excesivamente la información del vídeo. Aprovechando la causalidad
capacidades de Video-LLM, conserva todo el Video KV-Cache,
permitiendo la recuperación de información relevante cuando sea necesario. esto
La estrategia mitiga eficazmente la pérdida sustancial de contenido de vídeo.
al mismo tiempo que mejora la interpretabilidad.

Manejo de contexto largo para LLM. El manejo de secuencias de texto largas en LLM ha
ha sido un gran desafío debido a los altos costos computacionales y de memoria,
lo que lleva a limitaciones de entrenamiento en secuencias más cortas. Técnicas como
Uso de StreamingLLM (Xiao et al., 2024b) y LM-Infinite (Han et al., 2023)
Atención de ventana deslizante para procesar secuencias largas de forma incremental, pero
descartar fichas distantes, lo que limita la capacidad del modelo para capturar
dependencias de rango. Enfoques recientes (Xiao et al., 2024a; Li et al.,
2025; Fountas et al., 2025) abordan esto almacenando y recuperando
KV-Caches previamente calculados, lo que permite una mejor recuperación de datos distantes
contextos.

Recuperación-Generación Aumentada. Generación aumentada de recuperación (RAG)
combina mecanismos de recuperación con modelos generativos para mejorar
Rendimiento en diversas tareas de PNL mediante la incorporación de conocimientos externos.
(Guu et al., 2020; Lewis et al., 2020; Borgeaud et al., 2022) y
mejorar el rendimiento en tareas de visión y lenguaje (Xu et al., 2024). En-
recuperación de contexto, propuesta recientemente para manejar entradas largas (Ram et al.
al., 2023), recupera información del propio documento de entrada en lugar de
que una base de conocimientos externa. Recuperación adicional de KV-Cache en contexto
mejora la eficiencia al precodificar documentos largos, evitando redundancia
codificaciones y aprovechar las capacidades de recuperación propias del LLM para
rendimiento más rápido y eficaz.

## 6 CONCLUSIÓN

En conclusión, este artículo presenta un enfoque sin formación, ReKV,
Diseñado para mejorar la eficiencia de los modelos de lenguaje grande de vídeo.
(Video-LLM) para transmisión de preguntas y respuestas en video (StreamingVQA).
A diferencia de los sistemas convencionales de preguntas y respuestas en vídeo (VideoQA), que deben
Procese videos completos antes de responder, ReKV permite una respuesta rápida y en tiempo real.
respuestas. Al emplear un mecanismo de atención de ventana deslizante, garantiza
que el modelo solo considera un subconjunto de fotogramas anteriores mientras codifica
la transmisión de vídeo, lo que reduce significativamente las demandas computacionales.
Para conservar el contexto clave del vídeo, desarrollamos un KV-Cache en contexto
Método de recuperación que almacena y recarga eficientemente vectores clave-valor.
que sea relevante para cada consulta. Esta estrategia de recuperación dirigida, combinada
con la capacidad de realizar modelos de video y responder preguntas en
procesos y GPU separados, da como resultado una transmisión altamente eficiente
Sistema de control de calidad de vídeo. Amplios experimentos muestran que ReKV no sólo supera
modelos VideoQA existentes en rendimiento, pero también mejora su
practicidad para aplicaciones de streaming del mundo real.

Expresiones de gratitud. Este trabajo cuenta con el apoyo del Programa Nacional Clave de I+D de
China (nº 2022ZD0161400). Agradecemos a Yikun Liu por las discusiones y
realizando experimentos en CGBench.

## REFERENCIAS

- Kirolos Ataallah, Xiaoqian Shen, Eslam Abdelrahman, Essam Sleiman, Deyao Zhu, Jian Ding y Mohamed Elhoseiny. Minigpt4-video: avance de películas multimodales para la comprensión de videos con tokens visual-textuales intercalados. En Taller CVPR, 2024.
- Ivana Balazevic, Yuge Shi, Pinelopi Papalampidi, Rahma Chaabouni, Skanda Koppula y Olivier J Henaff. La consolidación de la memoria permite la comprensión de vídeos de contexto prolongado. En ICML, 2024.- Sebastian Borgeaud, Arthur Mensch, Jordan Hoffmann, Trevor Cai, Eliza Rutherford, Katie Millican, George Bm Van Den Driessche, Jean-Baptiste Lespiau, Bogdan Damoc, Aidan Clark, et al. Mejorar los modelos de lenguaje recuperando billones de tokens. En ICML, 2022.
- Fabián Caba Heilbron, Víctor Escorcia, Bernard Ghanem y Juan Carlos Niebles. Activitynet: un video de referencia a gran escala para comprender la actividad humana. En CVPR, 2015.
- Guo Chen, Yicheng Liu, Yifei Huang, Yuping He, Baoqi Pei, Jilan Xu, Yali Wang, Tong Lu y Limin Wang. Cg-bench: punto de referencia de respuesta a preguntas basado en pistas para la comprensión de vídeos largos. En ICLR, 2025a.
- Joya Chen, Zhaoyang Lv, Shiwei Wu, Kevin Qinghong Lin, Chenan Song, Difei Gao, Jia-Wei Liu, Ziteng Gao, Dongxing Mao y Mike Zheng Shou. Videollm-online: Modelo de lenguaje grande de vídeo online para streaming de vídeo. En CVPR, 2024.
- Qirui Chen, Shangzhe Di y Weidi Xie. Videoqa de múltiples saltos fundamentado en videos egocéntricos de larga duración. En AAAI, 2025b.
- Shangzhe Di y Weidi Xie. Preguntas y respuestas fundamentadas en vídeos largos y egocéntricos. En CVPR, 2024.
- Yuxin Fang, Wen Wang, Binhui Xie, Quan Sun, Ledell Wu, Xinggang Wang, Tiejun Huang, Xinlong Wang y Yue Cao. Eva: Explorando los límites del aprendizaje a escala de representaciones visuales enmascaradas. En CVPR, 2023.
- Zafeirios Fountas, Martin A Benfeghoul, Adnan Oomerjee, Fenia Christopoulou, Gerasimos Lampouras, Haitham Bou-Ammar y Jun Wang. Memoria episódica similar a la humana para películas de contexto infinito. En ICLR, 2025.
- Raghav Goyal, Samira Ebrahimi Kahou, Vincent Michalski, Joanna Materzynska, Susanne Westphal, Heuna Kim, Valentin Haenel, Ingo Fruend, Peter Yianilos, Moritz Mueller-Freitag, et al. La base de datos de vídeos 'algo, algo' para aprender y evaluar el sentido común visual. En ICCV, 2017.
- Kristen Grauman, Andrew Westbury, Eugene Byrne, Zachary Chavis, Antonino Furnari, Rohit Girdhar, Jackson Hamburger, Hao Jiang, Miao Liu, Xingyu Liu, et al. Ego4d: La vuelta al mundo en 3.000 horas de vídeo egocéntrico. En CVPR, 2022.
- Kelvin Guu, Kenton Lee, Zora Tung, Panupong Pasupat y Mingwei Chang. Preentrenamiento del modelo de lenguaje aumentado de recuperación. En ICML, 2020.
- Chi Han, Qifan Wang, Wenhan Xiong, Yu Chen, Heng Ji y Sinong Wang. Lm-infinite: generalización de longitud simple sobre la marcha para modelos de lenguaje grandes. Preimpresión de arXiv arXiv:2308.16137, 2023.
- Bo He, Hengduo Li, Young Kyun Jang, Menglin Jia, Xuefei Cao, Ashish Shah, Abhinav Shrivastava y Ser-Nam Lim. Ma-lmm: modelo multimodal grande con memoria aumentada para comprensión de videos a largo plazo. En CVPR, 2024.
- Lianghua Huang, Xin Zhao y Kaiqi Huang. Got-10k: un gran punto de referencia de alta diversidad para el seguimiento de objetos genéricos en la naturaleza. TPAMI, 2019.
- Qingqiu Huang, Yu Xiong, Anyi Rao, Jiaze Wang y Dahua Lin. Movienet: un conjunto de datos holístico para la comprensión de películas. En ECCV, 2020.

- Md Mohaiminul Islam, Ngan Ho, Xitong Yang, Tushar Nagarajan, Lorenzo Torresani y Gedas Bertasius. Resumen de vídeo: subtítulos recursivos de vídeos de una hora de duración. En CVPR, 2024.
- Yunseok Jang, Yale Song, Youngjae Yu, Youngjin Kim y Gunhee Kim. Tgif-qa: Hacia el razonamiento espaciotemporal en la respuesta visual a preguntas. En CVPR, 2017.
- Will Kay, Joao Carreira, Karen Simonyan, Brian Zhang, Chloe Hillier, Sudheendra Vijayanarasimhan, Fabio Viola, Tim Green, Trevor Back, Paul Natsev, et al. El conjunto de datos de vídeo de acción humana cinética. arXiv:1705.06950, 2017.- Patrick Lewis, Ethan Perez, Aleksandra Piktus, Fabio Petroni, Vladimir Karpukhin, Naman Goyal, Heinrich K¨ uttler, Mike Lewis, Wen-tau Yih, Tim Rockt¨ aschel, et al. Generación aumentada de recuperación para tareas de PNL con uso intensivo de conocimiento. En NeurIPS, 2020.
- Bo Li, Yuanhan Zhang, Dong Guo, Renrui Zhang, Feng Li, Hao Zhang, Kaichen Zhang, Yanwei Li, Ziwei Liu y Chunyuan Li. Llama-onevision: Fácil transferencia visual de tareas. Preimpresión de arXiv arXiv:2408.03326, 2024a.
- Jingyao Li, Han Shi, Xin Jiang, Zhenguo Li, Hong Xu y Jiaya Jia. Quickllama: aceleración de inferencia basada en consultas para modelos de lenguaje grandes. En COLING, 2025.
- Junnan Li, Dongxu Li, Silvio Savarese y Steven Hoi. Blip-2: Arranque del entrenamiento previo de imágenes y lenguaje con codificadores de imágenes congeladas y modelos de lenguaje grandes. En ICML, 2023.
- Kunchang Li, Yali Wang, Yinan He, Yizhuo Li, Yi Wang, Yi Liu, Zun Wang, Jilan Xu, Guo Chen, Ping Luo, et al. Mvbench: una comprensión integral de video multimodal de referencia. En CVPR, 2024b.
- Yanwei Li, Chengyao Wang y Jiaya Jia. Llama-vid: Una imagen vale 2 tokens en modelos de lenguaje grandes. En ECCV, 2024c.
- Yuhong Li, Yingbing Huang, Bowen Yang, Bharat Venkitesh, Acyr Locatelli, Hanchen Ye, Tianle Cai, Patrick Lewis y Deming Chen. Snapkv: Llm sabe lo que buscas antes de la generación. En NeurIPS, 2024d.
- Bin Lin, Yang Ye, Bin Zhu, Jiaxi Cui, Munan Ning, Peng Jin y Li Yuan. Video-LLaVA: Aprendizaje de la representación visual unida por alineación antes de la proyección. En EMNLP, 2024.
- Muhammad Maaz, Hanoona Rasheed, Salman Khan y Fahad Shahbaz Khan. Video-chatgpt: Hacia una comprensión detallada del vídeo a través de grandes modelos de visión y lenguaje. En ACL, 2024.
- Karttikeya Mangalam, Raiymbek Akshulakov y Jitendra Malik. Egoschema: un punto de referencia de diagnóstico para la comprensión del lenguaje de vídeos de muy larga duración. NeurIPS, 2023.
- Matthias Muller, Adel Bibi, Silvio Giancola, Salman Alsubaihi y Bernard Ghanem. Trackingnet: un conjunto de datos a gran escala y un punto de referencia para el seguimiento de objetos en la naturaleza. En ECCV, 2018.
- OpenAI. Gpt-4, marzo de 2023a. URL https://cdn.openai.com/papers/gpt-4-system-card.pdf.

OpenAI. Gpt-4v, septiembre de 2023b. URL https://openai.com/index/
tarjeta-sistema-gpt-4v/ .

OpenAI. Gpt-4o, mayo de 2024. URL https://openai.com/index/hello-gpt-4o/.

- Rui Qian, Xiaoyi Dong, Pan Zhang, Yuhang Zang, Shuangrui Ding, Dahua Lin y Jiaqi Wang. Transmisión de comprensión de vídeos largos con modelos de lenguaje de gran tamaño. En NeurIPS, 2024.
- Alec Radford, Jong Wook Kim, Chris Hallacy, Aditya Ramesh, Gabriel Goh, Sandhini Agarwal, Girish Sastry, Amanda Askell, Pamela Mishkin, Jack Clark, et al. Aprendizaje de modelos visuales transferibles a partir de la supervisión del lenguaje natural. En ICML, 2021.
- Ori Ram, Yoav Levine, Itay Dalmedigos, Dor Muhlgay, Amnon Shashua, Kevin Leyton-Brown y Yoav Shoham. Modelos de lenguaje aumentados de recuperación en contexto. En ACL, 2023.

- Jianlin Su, Murtadha Ahmed, Yu Lu, Shengfeng Pan, Wen Bo y Yunfeng Liu. Roformer: Transformador mejorado con incrustación de posición giratoria. Neurocomputación, 2024.
- Equipo Gemini, Rohan Anil, Sebastian Borgeaud, Yonghui Wu, Jean-Baptiste Alayrac, Jiahui Yu, Radu Soricut, Johan Schalkwyk, Andrew M Dai, Anja Hauth, et al. Gemini: una familia de modelos multimodales de gran capacidad. Preimpresión de arXiv arXiv:2312.11805, 2023.
- Jiahao Wang, Guo Chen, Yifei Huang, Limin Wang y Tong Lu. Transformador de memoria y anticipación para la comprensión de la acción en línea. En ICCV, 2023.- Chao-Yuan Wu, Christoph Feichtenhofer, Haoqi Fan, Kaiming He, Philipp Krahenbuhl y Ross Girshick. Bancos de funciones a largo plazo para una comprensión detallada del vídeo. En CVPR, 2019.
- Chao-Yuan Wu, Yanghao Li, Karttikeya Mangalam, Haoqi Fan, Bo Xiong, Jitendra Malik y Christoph Feichtenhofer. Memvit: transformador de visión multiescala con memoria aumentada para un reconocimiento de vídeo eficiente a largo plazo. En CVPR, 2022.
- Chaojun Xiao, Pengle Zhang, Xu Han, Guangxuan Xiao, Yankai Lin, Zhengyan Zhang, Zhiyuan Liu y Maosong Sun. Infllm: extrapolación de contexto largo sin entrenamiento para películas con una memoria de contexto eficiente. En Taller ICML, 2024a.
- Guangxuan Xiao, Yuandong Tian, ​​Beidi Chen, Song Han y Mike Lewis. Modelos de lenguaje de transmisión eficientes con captadores de atención. En ICLR, 2024b.
- Junbin Xiao, Xindi Shang, Angela Yao y Tat-Seng Chua. Next-qa: siguiente fase de preguntas y respuestas para explicar las acciones temporales. En CVPR, 2021.
- Dejing Xu, Zhou Zhao, Jun Xiao, Fei Wu, Hanwang Zhang, Xiangnan He y Yueting Zhuang. Respuestas a preguntas en vídeo mediante una atención refinada gradualmente sobre la apariencia y el movimiento. En ACM Multimedia, 2017.
- Jilan Xu, Yifei Huang, Junlin Hou, Guo Chen, Yuejie Zhang, Rui Feng y Weidi Xie. Subtítulos de vídeo egocéntricos recuperados y aumentados. En CVPR, 2024.
- Antoine Yang, Antoine Miech, Josef Sivic, Ivan Laptev y Cordelia Schmid. Respuestas a preguntas en video de toma cero a través de modelos de lenguaje bidireccional congelados. En NeurIPS, 2022.
- X Ye. Calflops: una herramienta de cálculo de flops y parámetros para redes neuronales en el marco de pytorch, 2023.
- Zhou Yu, Dejing Xu, Jun Yu, Ting Yu, Zhou Zhao, Yueting Zhuang y Dacheng Tao. Activitynetqa: un conjunto de datos para comprender videos web complejos mediante la respuesta a preguntas. En AAAI, 2019.
- Xiaohua Zhai, Basil Mustafa, Alexander Kolesnikov y Lucas Beyer. Pérdida de sigmoide para el preentrenamiento de imágenes del lenguaje. En ICCV, 2023.
- Ce Zhang, Taixi Lu, Md Mohaiminul Islam, Ziyang Wang, Shoubin Yu, Mohit Bansal y Gedas Bertasius. Un marco de cine simple para responder preguntas en video de largo alcance. Preimpresión de arXiv arXiv:2312.17235, 2023.
- Haoji Zhang, Yiqin Wang, Yansong Tang, Yong Liu, Jiashi Feng, Jifeng Dai y Xiaojie Jin. Flash-vstream: comprensión en tiempo real basada en memoria para transmisiones de video largas. Preimpresión de arXiv arXiv:2406.08085, 2024a.
- Peiyuan Zhang, Kaichen Zhang, Bo Li, Guangtao Zeng, Jingkang Yang, Yuanhan Zhang, Ziyue Wang, Haoran Tan, Chunyuan Li y Ziwei Liu. Larga transferencia de contexto del lenguaje a la visión. Preimpresión de arXiv arXiv:2406.16852, 2024b.
- Yuanhan Zhang, Bo Li, haotian Liu, Yong jae Lee, Liangke Gui, Di Fu, Jiashi Feng, Ziwei Liu y Chunyuan Li. Llava-next: Un sólido modelo de comprensión de video de toma cero, abril de 2024c. URL https://llava-vl.github.io/blog/2024-04-30-llava-next-video/.
- Junjie Zhou, Yan Shu, Bo Zhao, Boya Wu, Shitao Xiao, Xi Yang, Yongping Xiong, Bo Zhang, Tiejun Huang y Zheng Liu. Mlvu: un punto de referencia integral para la comprensión de vídeos largos multitarea. Preimpresión de arXiv arXiv:2406.04264, 2024a.
- Xingyi Zhou, Anurag Arnab, Shyamal Buch, Shen Yan, Austin Myers, Xuehan Xiong, Arsha Nagrani y Cordelia Schmid. Transmisión de subtítulos de vídeo densos. En CVPR, 2024b.

En el apéndice, proporcionamos detalles adicionales de implementación,
experimentos y discusiones sobre limitaciones y trabajos futuros.

## A DETALLES ADICIONALES DE IMPLEMENTACIÓN

## A.1 SERVICIO MULTIPROCESAMIENTO

Como se discutió en la Sección 2, nuestro enfoque permite la separación de videosmodelado y respuesta a preguntas en diferentes procesos y GPU,
mejorando significativamente la eficiencia en aplicaciones del mundo real.
Específicamente, dedicamos un proceso principal a la codificación de transmisiones de video,
utilizando la atención de la ventana deslizante para analizar el vídeo y almacenar el
caché calculado en RAM. Si se excede la capacidad de RAM, los datos pueden ser
descargado al disco. Además, se mantiene un pool de procesos, con el
número de procesos determinado por la frecuencia de consultas y disponibles
recursos. Cada proceso carga los mismos parámetros de Video-LLM pero opera
de forma independiente. El procesamiento del vídeo continúa ininterrumpidamente, sin
esperando que se completen las tareas de respuesta a preguntas. Cuando se plantea una consulta,
Registramos su marca de tiempo para garantizar que la información del video después de este punto
queda excluido de la respuesta. Un proceso disponible del grupo es entonces
activado para recuperar vectores clave-valor de vídeo relevantes utilizando nuestro método,
cargándolos en su GPU para responder preguntas. Este enfoque permite
aplicaciones StreamingVQA eficientes, con importante potencial en áreas
como robótica, vigilancia, realidad aumentada y en vivo
radiodifusión.

## A.2 PLANTILLAS INDICADORAS PARA VIDEOQA

Usamos la misma plantilla de mensajes para todos los VideoQA de opción múltiple.
puntos de referencia. El texto en rojo indica entradas variables.```
System: You are a helpful assistant. User: <video> Question: <question> Options: (A) <Option_A> (B) <Option_B> (C) <Option_C> (D) <Option_D> (E) <Option_E> Answer with the option's letter from the given choices directly. Assistant:
```
## La plantilla de solicitud para VideoQA abierto es bastante más simple:```
System: You are a helpful assistant. User: <video> <question> Assistant:
```
## A. CÁLCULO DEL TAMAÑO DE CACHÉ DE 3 KV

El tamaño del KV-Cache se puede calcular utilizando la siguiente fórmula,
asumiendo precisión FP16:```
2 × L layers × T frames × M tokens/frame × H heads × D dimension × 2 bytes .
```
Para LLaVA-OV-7B (Li et al., 2024a), con L = 28, M = 196, H = 4 y
D = 128 , procesar un vídeo de 1 hora a 0,5 FPS ( T = 1800 ) da como resultado un
tamaño total de caché KV de 18,8 GB.

De manera similar, para LLaVA-OV-0.5B (Li et al., 2024a), con L = 24, M = 196,
H = 2 y D = 64, procesar un vídeo de 1 hora a 0,5 FPS da como resultado un
tamaño total de caché KV de 4,0 GB.

Estos cálculos teóricos son consistentes con los experimentales.
Los resultados se muestran en la Tabla 5.

## B EXPERIMENTOS ADICIONALES

## B.1 EXPERIMENTOS CON MÁS VIDEO-LLMS Y BENCHMARK

Para evaluar más a fondo la generalización de nuestro enfoque, lo probamos en
VideoLLM adicionales: Video-LLaVA-7B (Lin et al., 2024), LongVA-7B
(Zhang et al., 2024b) y LLaVA-OV-72B (Li et al., 2024a). Usamos
fragmentación del modelo para LLaVA-OV-72B, lo que ralentiza significativamente la inferencia. a
Para mitigar esto, redujimos el FPS a 0,1 y el número de imágenes recuperadas.
fotogramas a 32, lo que garantiza una evaluación eficiente. Como se muestra en la Tabla 6, ReKV
rendimiento constantemente mejorado en varios modelos y puntos de referencia,
destacando su robustez y adaptabilidad.

Tabla 6: Experimentos adicionales con más Video-LLM y puntos de referencia.
'Acc.' denota precisión. 'X Frames' se refiere al muestreo uniforme de X
fotogramas del vídeo. 'Y FPS → Cuadros' indica un vídeo de entrada con un
velocidad de cuadros de Y FPS, de la cual se recuperan X cuadros.

incógnita| Método | Muestreo | Desarrollador MLVU | QAEGO4D | Esquema del ego | Acc. CGBench.                                                             ||----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------|---------------------------------------------------|--------------------------------------------------------------------|--------------------------------------------------------------------------|--------------------------------------------------------------------------||                                                                                                                                                                                           |                                                                                                                         | Acc.                                              | prueba Acc.                                                                | Acc.                                                                     |                                                                          || Video-LLaVA-7B (Lin et al., 2024) +ReKV LongVA-7B (Zhang et al., 2024b) +ReKV LLaVA-OV-0.5B (Li et al., 2024a) +ReKV LLaVA-OV-7B (Li et al., 2024a) +ReKV LLaVA-OV-72B (Li et al., 2024a) | 8 fotogramas 0,5 FPS → 8 fotogramas 32 fotogramas 0,5 FPS → 32 64 fotogramas 0,5 FPS → 64 64 fotogramas 0,5 FPS → 64 32 fotogramas 0,1 FPS → 32 | 46,5 47,2 57,3 58,6 53,2 56,1 64,7 68,5 71,9 72,6 | 37,0 37,9 (+0,9) 42,8 45,6 (+2,8) 42,6 50,0 (+7,4) 52,8 56,0 (+3,2) 53,6 | 41,4 42,2 (+0,8) 42,5 42,7 (+0,2) 29,6 31,0 (+1,4) 59,8 60,7 (+0,9) 59,6 | 18,7 19,2 (+0,5) 26,1 26,4 (+0,3) 21,4 21,7 (+0,3) 31,1 33,9 (+2,8) 37,2 ||                                                                                                                                                                                           |                                                                                                                         | (+0,7) |                                                                          |                                                                          |                                                                          ||                                                                                                                                                                                           | Marcos | (+1,3) |                                                                          |                                                                          |                                                                          ||                                                                                                                                                                                           | Marcos | (+2,9) |                                                                          |                                                                          |                                                                          ||                                                                                                                                                                                           | Marcos | (+3,8) |                                                                          |                                                                          |                                                                          || +ReKV | Marcos | (+0,7) | 57,0 (+3,4) | 62,0 (+2,4) | 40,5 (+3,3) |## B.2 COMPARACIONES JUSTAS CON FLASH-VSTREAM

Las tablas 4 y 5 compararon LLaVA-OneVision+ReKV con Flash-VStream.
Sin embargo, estas comparaciones pueden ser injustas debido a la diferente arquitectura.
y datos de entrenamiento. Por lo tanto, aquí realizamos comparaciones justas utilizando los mismos
Red troncal Video-LLM, incluido el codificador visual idéntico (CLIP-
ViT-L/14), proyector (MLP de 2 capas), LLM (Vicuna-7B-v1.5), capacitación
datos y canales de tren/evaluación.

Debido a la inaccesibilidad de los vídeos WebVid 3 utilizados en Flash-VStream
capacitación original, utilizamos 232K videos InternVid 4 muestreados aleatoriamente como
sustituto. Esto garantiza entornos experimentales comparables. Entrenamos a un
modelo básico Video-LLM ( Base ) y una versión mejorada con Flash-VStream (
Base+Flash). De manera similar, integramos ReKV en la misma línea base (
Base+ReKV ) para comparaciones justas. Para mantener la paridad, la base
Muestra uniformemente 16 fotogramas por vídeo, redimensionados a 224 × 224. visuales
las características de forma ( T, 16 , 16 , D ) se agrupan en promedio para ( T, 8 , 8 ,
D ) antes de pasar a través del proyector MLP y al LLM. ambos
Flash-VStream y ReKV procesan vídeo a 0,5 FPS, y ReKV recupera 16
marcos.

Tabla 7: Comparaciones justas con Flash-VStream. 'Original Flash' es el punto de control publicado oficialmente por Flash-VStream mientras que 'Base+Flash' es nuestra versión reproducida.| Método | MLVU dev-mc | Prueba QAEGO4D-mc | Esquema del ego | Película RVS | RVS-Ego ||----------------|---------------|-------------------|-------------|-------------|-------------|
| Base | 49,8 | 39,0 | 42,6 | 47,2 | 54.1 || Base+Flash | 51,0 | 37,4 | 41.2 | 50,1 | 55,4 || Base+ReKV | 51,9 (+0,9) | 40,5 (+3,1) | 43,7 (+2,5) | 51,9 (+1,8) | 54,7 (-0,7) || Flash original | 50,2 | 38,2 | 38.1 | 53.1 | 57,3 |Como se muestra en la Tabla 7, Base+ReKV supera consistentemente a la base Video-
LLM Base y supera a Base+Flash en la mayoría de los casos, destacando su
superioridad en condiciones comparativas justas. Además, ReKV ofrece
usabilidad mejorada, integrándose perfectamente con Video-LLM existentes
sin requerir un reentrenamiento extenso.

3 https://github.com/m-bain/webvid

4 https://huggingface.co/datasets/OpenGVLab/InternVid

Por el contrario, el Base+Flash reproducido no coincide consistentemente
supera a Base. Destaca en StreamingVQA (RVS-Movie y RVS-Ego) y
MLVU pero tiene un rendimiento inferior en QAEGO4D y EgoSchema. Esta discrepancia es
probablemente debido a una pérdida significativa de información visual: el modelo base
procesa 1024 tokens visuales (16 × 64), mientras que Base+Flash usa solo 681
fichas de memoria.

Para contexto adicional, incluimos resultados del Flash-
VStream (Original Flash) usando puntos de control de su versión oficial
depósito 5 . Nuestra Base+Flash reproducida muestra desviaciones de rendimiento,
Probablemente debido a diferencias en los datos de entrenamiento y posibles cambios ambientales.
factores.

## B.3 COMPLEJIDAD COMPUTACIONAL

Garantizamos comparaciones justas mediante el uso de la misma red troncal de Video-LLM
(Sec. B.2) bajo condiciones de transmisión controlada (Sec. 4.5).
Específicamente, medimos los FLOP y MAC del Video-LLM base,
Flash-VStream y nuestros métodos de recuperación internos y externos. Nosotros
analizó los TFLOP y TMAC promedio por control de calidad en varias preguntas
frecuencias en un video de 1 hora, aprovechando la biblioteca calflops (Ye,
2023).

Como se muestra en las Tablas 8 y 9, la eficiencia de ReKV mejora significativamente
con una frecuencia cada vez mayor de control de calidad. La transmisión de video se codifica solo una vez y
Los resultados calculados se reutilizan en los controles de calidad, lo que lleva a una reducción por consulta.
complejidad a medida que aumenta la frecuencia de control de calidad. Flash-VStream supera a ReKV en niveles bajos
Frecuencias de control de calidad (por ejemplo, 100 controles de calidad). Sin embargo, la complejidad de ReKV disminuye
más rápidamente con una mayor frecuencia QA, principalmente debido a Flash-
Alta sobrecarga de actualización de memoria de VStream. Por lo tanto, ReKV es más adecuado para
escenarios de alta concurrencia, como transmisión en vivo, y no requiere
formación adicional.

Además, la recuperación interna supera consistentemente a la externa.
recuperación, reduciendo los FLOP promedio en un 15,5% y los MAC en un 15,2%. Estos
Los resultados subrayan la capacidad de ReKV para equilibrar la eficiencia computacional.
y eficacia, especialmente en entornos dinámicos y con muchas consultas.
Esto posiciona a ReKV como una solución práctica y escalable para streaming.
comprensión del vídeo.

Tabla 8: TFLOP/QA.|   #QA |   Línea de base |   Flash-VStream |   ReKV (externo) |   ReKV (interno) ||--------|------------|-----------------|-------------------|-------------------|
|    100 |       22.4 |            15,5 |              21.7 |              18,5 ||    200 |       12.7 |            14.1 |              11.4 |               9.6 ||    360 |        8.5 |            13.8 |               6.8 |               5.6 |Tabla 9: TMAC/QA.|   #QA |   Línea de base |   Flash-VStream |   ReKV (externo) |   ReKV (interno) ||--------|------------|-----------------|-------------------|-------------------|
|    100 |       11.2 |             7.8 |              10.8 |               9.2 ||    200 |        6.4 |             7.1 |               5.7 |               4.8 ||    360 |        4.3 |             6.8 |               3.3 |               2.8 |## C LIMITACIONES Y TRABAJO FUTURO

Si bien ReKV mejora la precisión y eficiencia de Video-LLM en el
Tarea StreamingVQA, todavía tiene varias limitaciones que merecen un futuro
Investigación: Primero, aunque el KV-Cache se descarga a la RAM o al disco
es manejable, como se muestra en la Tabla 5, manejando videos extremadamente largos
corrientes, como las que se encuentran bajo vigilancia, pueden dar lugar a una situación insostenible.
aumento del tamaño de la caché. Este problema se puede mitigar integrando
técnicas como cuantificación, poda de tokens y compresión. segundo
, el uso de un tamaño de bloque constante para agrupar fotogramas consecutivos
durante la recuperación puede interrumpir la continuidad del vídeo. Una solución más refinada
Implicaría segmentar vídeos en bloques semánticamente coherentes. tercero
, nuestro método recupera un número fijo de fotogramas. El trabajo futuro podría
explorar estrategias de recuperación dinámicas que ajusten el número de fotogramas
según el contexto del vídeo y los requisitos de consulta. Finalmente, StreamingVQA
sigue siendo una tarea poco explorada y con pocos puntos de referencia disponibles. Desarrollando
puntos de referencia de alta calidad con anotaciones temporales precisas es crucial para
avanzar en futuras investigaciones.

5 https://github.com/IVGSZ/Flash-VStream