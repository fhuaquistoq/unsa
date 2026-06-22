## StreamForest: comprensión eficiente de videos en línea con memoria de eventos persistente

Xiangyu Zeng ∗ 1 , Kefan Qiu ∗ 1 , Qingyu Zhang ∗ 1 , Xinhao Li 1 , Jing
Wang 1, Jiaxin Li 1 Ziang Yan 3, 2, Kun Tian 4, Meng Tian 5, Xinhai
Zhao 4, Yi Wang 2, Limin Wang † 1

1 2 3

Universidad de Nanjing Laboratorio de IA de Shanghai Universidad de Zhejiang 4 Noé
Ark Lab, tecnología inteligente Huawei 5 Yinwang.

https://github.com/MCG-NJU/StreamForest

## Resumen

Los modelos multimodales de lenguaje grande (MLLM) han logrado recientemente
progreso notable en la comprensión del vídeo. Sin embargo, su efectividad
en escenarios de transmisión en tiempo real sigue siendo limitado debido al almacenamiento
limitaciones de las características visuales históricas y tiempo real insuficiente
razonamiento espaciotemporal. Para abordar estos desafíos, proponemos
StreamForest, una novedosa arquitectura diseñada específicamente para streaming
comprensión del vídeo. Lo fundamental para StreamForest es el evento persistente
Memory Forest, un mecanismo de memoria que organiza de forma adaptativa fotogramas de vídeo
en múltiples estructuras de árbol a nivel de evento. Este proceso está guiado por
funciones de penalización basadas en la distancia temporal, la similitud de contenido y
frecuencia de fusión, lo que permite una retención eficiente de la memoria a largo plazo en condiciones
recursos computacionales limitados. Para mejorar la percepción en tiempo real, nosotros
introduce una ventana espaciotemporal detallada, que captura detalles
Señales visuales a corto plazo para mejorar la percepción de la escena actual.
Además, presentamos OnlineIT, un conjunto de datos de ajuste de instrucciones.
Diseñado para tareas de transmisión de vídeo. OnlineIT impulsa significativamente MLLM
rendimiento tanto en la percepción en tiempo real como en la predicción futura. a
evaluar la generalización en aplicaciones prácticas, presentamos ODV-
Bench, un nuevo referente centrado en el vídeo en streaming en tiempo real
comprensión en escenarios de conducción autónoma. Resultados experimentales
demostrar que StreamForest logra el rendimiento más moderno,
con precisiones del 77,3% en StreamingBench, 60,5% en OVBench y 55,6%
en el banco OVO. En particular, incluso bajo una compresión extrema del token visual
(limitado a 1024 tokens), el modelo conserva el 96,8% de su promedio
precisión en ocho puntos de referencia en relación con la configuración predeterminada. Estos
Los resultados subrayan la solidez, eficiencia y generalización de
StreamForest para comprender la transmisión de video.

## 1 Introducción

En los últimos años, los modelos de lenguajes grandes multimodales han hecho importantes
progreso en tareas de comprensión de videos, demostrando una fuerte semántica
capacidades de comprensión y razonamiento en vídeos de diferentes
duraciones y escenarios [30, 52, 31, 35]. Beneficiándose de la gran escala
entrenamiento previo y capacidades mejoradas de modelado intermodal, estos modelos
han sido ampliamente adoptados en diversos ámbitos [28, 42, 55, 59]. Sin embargo,
con la creciente demanda de procesamiento inteligente en tiempo real en línea
aplicaciones como conducción autónoma [49], transmisión de vídeo en vivo [6],
y la robótica [79], los investigadores han cambiado cada vez más su enfoque
desde la comprensión convencional del video fuera de línea hasta la más desafiante
tarea de procesamiento de vídeo en streaming [5, 23, 46].

∗ Igual contribución.

† Autor correspondiente.

Figura 1: StreamForest logra un rendimiento sólido en varios puntos de referencia de evaluación y utiliza una cantidad significativamente menor de tokens visuales. Maneja de manera efectiva tareas clave en escenarios de transmisión de video, incluida la memoria pasada, la percepción en tiempo real y la predicción futura.

<!-- imagen -->

En el campo de la comprensión de la transmisión de video, el almacenamiento en caché eficienteLas funciones de fotogramas de vídeo que llegan continuamente siguen siendo una característica de larga data y
problema desafiante. Para mitigar el almacenamiento y la sobrecarga computacional.
asociados con marcos pasados, el trabajo anterior ha empleado principalmente dos
estrategias para la reducción de características visuales: compresión durante el muestreo [5,
46, 64] y compresión durante el almacenamiento [52, 72, 23]. Compresión durante
El muestreo reduce una gran parte de las características visuales entrantes, lo que
Limita severamente la capacidad del modelo para análisis espaciotemporales de grano fino.
razonamiento. Como resultado, sólo puede realizar operaciones semánticas burdas.
resumen del panorama actual. Por el contrario, la compresión durante
El almacenamiento normalmente implica fusionar o descartar marcos adyacentes según
sobre la similitud entre fotogramas. Si bien es más eficiente en términos de memoria, esta estrategia es
susceptible de omitir acciones críticas en primer plano debido al fondo
ruido. También puede dar lugar a una fusión local excesiva, introduciendo
irregularidades espaciotemporales que degradan la capacidad del modelo para retener
y razonar sobre eventos clave a lo largo del tiempo.

Para abordar los desafíos de la comprensión de la transmisión de video, proponemos un
arquitectura novedosa llamada StreamForest. En su núcleo está el Persistente
Event Memory Forest, un mecanismo diseñado para almacenar y almacenar eficientemente
gestionar información visual a largo plazo. Este sistema de memoria permite un MLLM
para procesar secuencias de vídeo ultralargas a una velocidad constante de 1 fps mediante
organizar dinámicamente segmentos de vídeo en una estructura de árbol basada en
límites del evento. La unión de segmentos se rige por tres sanciones.
funciones que consideran la distancia temporal, la similitud de contenido y la fusión
frecuencia, asegurando una jerarquía de memoria adaptativa y significativa. a
Para mejorar la percepción en tiempo real, presentamos un espacio-temporal detallado
Ventana, que extrae ricas características espaciotemporales locales de lugares cercanos.
marcos. Este módulo permite al MLLM comprender mejor la situación actual.
escena centrándose en el contexto visual temporalmente relevante. También presentamos
OnlineIT, un conjunto de datos de ajuste diseñado específicamente para streaming
comprensión del vídeo. OnlineIT mejora la capacidad de percepción del MLLM
el momento presente y anticipar eventos futuros aprovechando ambos
observaciones recientes y señales históricas a largo plazo. Se dirige a la
problema de las alucinaciones causadas por cambios de distribución espaciotemporal
entre fotogramas pasados y actuales. Además, presentamos ODV-Bench, un
Nuevo punto de referencia para evaluar la comprensión de la transmisión de vídeo en modo autónomo.
escenarios de conducción. ODV-Bench destaca la percepción en tiempo real y el futuro
predicción, proporcionando un marco sistemático para evaluar la
generalización y efectividad en el mundo real de los MLLM de transmisión de video en
tareas posteriores.

Realizamos extensos experimentos con videos tanto en línea como fuera de línea.
comprender los puntos de referencia para validar la eficacia de StreamForest.
Según la configuración predeterminada con un límite de token visual de 8192,
StreamForest supera significativamente el estado del arte anterior
transmisión de video entendiendo MLLM. Alcanza una precisión media de
77,3% en StreamingBench, 60,5% en OVBench y 55,6% en OVO-Bench.
StreamForest también iguala o supera el rendimiento de los principales
Video sin conexión que comprende MLLM en videos largos y cortos.
puntos de referencia, a pesar de operar en una configuración de entrada de video en tiempo real.
Además, StreamForest demuestra una gran resiliencia en condiciones extremas.
compresión. Con un límite de token visual reducido de solo 1024, conserva
96,8% de su rendimiento promedio en ocho puntos de referencia en comparación con elconfiguración predeterminada. Estos resultados resaltan la robustez y eficiencia
de nuestro enfoque en el procesamiento continuo de entradas de vídeo en tiempo real.

## 2 Trabajo relacionado

Modelo de lenguaje grande multimodal. Ampliar las capacidades multimodales desde
imágenes estáticas a secuencias de vídeo dinámicas introducen
Complejidad, lo que requiere que los modelos posean habilidades más fuertes en el modelado.
dependencias de largo alcance y comprensión de eventos [30, 40, 81, 41, 71,
58]. Los avances recientes en MLLM para vídeo han introducido una variedad de
estrategias innovadoras para abordar los desafíos del procesamiento eficiente
y razonamiento sobre entradas de vídeo largas [37, 42, 29, 51, 33]. LongVILA [7]
propone un sistema de paralelismo de secuencia multimodal para contextos largos
modelado, lo que permite un entrenamiento paralelo eficiente y una inferencia en términos extendidos.
contenido de vídeo. Sin embargo, la mayoría de las investigaciones actuales sobre la comprensión del vídeo
permanece centrado en entornos fuera de línea [57, 69, 62, 34], donde el modelo
tiene acceso completo a la secuencia de video completa antes de la inferencia.
Aunque esta configuración facilita el modelado semántico global, cae
corto en escenarios de transmisión, donde la comprensión en tiempo real de
Se requieren escenas en continua evolución. Por lo tanto, el desarrollo de
Los modelos diseñados específicamente para la comprensión de vídeos en línea son de
importancia crítica.

Comprensión de la transmisión de vídeo. En aplicaciones del mundo real, los usuarios
Se espera cada vez más que los MLLM admitan el procesamiento en línea y en tiempo real.
interacción. Esta demanda ha suscitado un creciente interés por la tarea de
Comprensión de transmisión de video. Recientemente, varios trabajos han explorado
esta área emergente [5, 72, 63, 11, 61, 23]. Sin embargo, la mayoría existente
Los enfoques de comprensión de transmisión de video están diseñados principalmente para
transmisión de subtítulos de video densos [5, 60, 32, 47, 12], centrándose únicamente en
resumir contenido semántico a partir de marcos visuales. Como resultado, ellos
luchan por manejar tareas esenciales como la recuperación de recuerdos y el tiempo real.
percepción, que son fundamentales para la transmisión integral de video
comprensión. Además, en la búsqueda de la eficiencia computacional, muchos
Los métodos aplican una compresión agresiva a las secuencias de fotogramas de vídeo [46, 74,
64], lo que los hace inadecuados para tareas complejas y dinámicas que requieren
comprensión espaciotemporal detallada y en tiempo real, como
conducción autónoma. Para abordar estas limitaciones, nuestro objetivo es desarrollar
un enfoque más generalizable y práctico para el vídeo en línea
comprensión. Enfatiza características espaciotemporales de grano fino en el
momento de la consulta y admite almacenamiento de memoria persistente basado en eventos.

## 3 Metodología

## 3.1 Transmisión de vídeo: comprensión de la arquitectura: StreamForest

En esta sección, detallamos nuestro StreamForest propuesto. Específicamente, el
El diseño central de StreamForest reside en la ventana espaciotemporal detallada
y Persistent Memory Forest, que funcionan en conjunto para permitir que el modelo
para retener recuerdos a largo plazo de eventos pasados mientras se admite el tiempo real
percepción.

## 3.1.1 Ventana espaciotemporal detallada

Para cumplir con los requisitos de percepción espaciotemporal en tiempo real de
comprensión de la transmisión de video, presentamos la tecnología de grano fino
Ventana espaciotemporal (FSTW). Observamos que en aplicaciones prácticas,
La mayoría de las pistas que requieren un razonamiento espaciotemporal detallado son
concentrado cerca del momento de la pregunta. Por lo tanto, conservamos sólo
características espaciotemporales de grano fino a corto plazo de segundo nivel.

Específicamente, el FSTW consta de dos componentes: percepción en tiempo real
y memoria espaciotemporal a corto plazo. Percepción en tiempo real directamenteMuestra características visuales de alta resolución del fotograma actual, que
están codificados con información posicional espaciotemporal. Como nuevos marcos
Al llegar, los fotogramas más antiguos se comprimen a lo largo de la dimensión espacial y
transferidos a la memoria espaciotemporal a corto plazo. Al mismo tiempo, el
El modelo calcula la similitud entre fotogramas entre fotogramas nuevos y antiguos para
permitir la segmentación posterior a nivel de evento. el corto plazo
La memoria espaciotemporal mantiene una secuencia de cuadros con una duración de t s.
segundos. Cuando se excede su capacidad, se desbordan las características visuales.
descargado en el bosque de memoria de eventos persistentes. Segmentamos continuo
características visuales en metaeventos identificando la posición con el
Similitud mínima local entre cuadros en la secuencia de cuadros. Esto asegura
que cada metaevento captura una transición espaciotemporal coherente. un
El metaevento se trata como un nodo independiente, que consta de un
colección de fichas visuales de fotogramas consecutivos similares. Estos nodos
forman la base de la memoria a largo plazo del MLLM.

Figura 2: Descripción general de nuestro StreamForest propuesto. La ventana espaciotemporal detallada captura características espaciotemporales a nivel de instancia, mientras que el bosque de memoria de eventos persistentes organiza de forma adaptativa las representaciones a nivel de eventos en un conjunto de estructuras de árbol. Las flechas discontinuas y los tokens de funciones ilustran las posibles operaciones realizadas durante cada iteración de actualización de memoria.

<!-- imagen -->

## 3.1.2 Bosque de memoria de eventos persistente

Para procesar eficientemente características de cuadros de video que llegan continuamente en
escenarios de streaming, proponemos el Bosque de Memoria de Eventos Persistentes
(PEMF), una arquitectura de memoria diseñada específicamente para soportar datos a largo plazo.
memoria en el contexto de la transmisión de vídeo. A diferencia de los métodos anteriores que se basan
sobre compresión directa de similitud entre cuadros [52] o memoria estática
jerarquías [23], PEMF comprime y organiza de forma adaptativa el vídeo
información a nivel de evento. Construye un árbol jerárquico.
memoria estructurada guiada por tres funciones de penalización, lo que permite al modelo
retener contenido semánticamente rico y no redundante mientras se gestiona
memoria de manera eficiente a medida que evoluciona con el tiempo. Para controlar el crecimiento de la memoria,
imponer un límite superior L q al número de tokens de memoria a largo plazo
almacenado en PEMF. Cuando se excede este límite, PEMF realiza operaciones jerárquicas.
Consolidación de la memoria mediante la fusión adaptativa de nodos de eventos adyacentes en
nodos individuales dentro de la estructura de árbol. La selección de nodos para
La fusión se guía por tres funciones de penalización que tienen en cuenta el tiempo.
distancia, similitud de contenido y frecuencia de fusión, asegurando que la
la memoria sigue siendo informativa y compacta.

Penalización por similitud. En vídeos largos, los segmentos de vídeo adyacentes suelen
exhiben una alta similitud visual, lo que resulta en una característica sustancial
redundancia. Por lo tanto, introducimos una penalización por similitud que fomenta
la fusión de nodos de eventos con contenido visual muy similar. debido a
diferencias en la duración de los eventos, dos nodos de eventos candidatos (indicados como x
i , x i +1 ) pueden contener diferentes números de tokens visuales. para manejar
esta discrepancia, inspirados en ToMe [3], adoptamos un gráfico bipartito
enfoque de emparejamiento. En concreto, tratamos las características visuales de los dos.
nodos de eventos como conjuntos en un gráfico bipartito y se calculan por pares
similitudes entre tokens de estos conjuntos. Sea X i ∈ R n i × d
denota las características del token visual del nodo de evento x i, donde n i es
el número de fichas en x i . Calculamos la similitud del coseno por pares.matriz S i = sim ( X i , X i +1 ) ∈ R n i × n i +1 , y seleccione la parte superior
k i puntuaciones de similitud más altas, correspondientes al token más similar
pares entre dos nodos de eventos. La penalización por similitud P s se define como
uno menos el promedio de estas puntuaciones de similitud más altas:

<!-- fórmula-no-decodificada -->

Penalización por conteo de fusión. Cuando los nodos de eventos participan repetidamente en
memoria jerárquica estructurada fusionándose, sus detalles visuales pueden
se degradan gradualmente debido a la pérdida de información acumulada. Esta degradación
puede conducir a inconsistencias espaciotemporales locales, lo que en última instancia perjudica
la precisión de la comprensión del vídeo a largo plazo. Para mitigar este problema,
Introducimos una penalización por recuento de fusiones como término de regularización. eso
penaliza los nodos demasiado fusionados y fomenta una memoria más equilibrada
proceso de integración, preservando así la fidelidad de cada evento
representación. Sea c i el recuento histórico de fusiones del evento.
nodo x i , con su valor máximo en el momento de la consulta indicado como c max .
Definimos la penalización del recuento de fusiones P m de la siguiente manera:

<!-- fórmula-no-decodificada -->

Penalización por Distancia Temporal. Entendiendo la transmisión de video en el mundo real
escenarios, marcos que están temporalmente más cerca del tiempo de consulta actual
a menudo contienen información más relevante. Esta observación sugiere que
Las características visuales recientes deben preservarse con mayor fidelidad, mientras que
Los rasgos históricos se pueden comprimir de forma más agresiva. para implementar
esta intuición, introducimos una penalización de distancia temporal, que
Alienta al modelo a retener representaciones más detalladas de
eventos temporalmente próximos mientras se promueve el olvido de detalles
de acontecimientos pasados lejanos. Sea t q la consulta actual o
tiempo de interacción, t i denota el momento del evento i. El cálculo de t
i se detalla en el Apéndice A. Definimos la penalización de tiempo P t como
sigue:

<!-- fórmula-no-decodificada -->

Penalización global. Incorporamos las tres sanciones anteriores para guiar el
proceso de fusión adaptativa de nodos de eventos en el PEMF, donde el
La combinación de estos tres factores determina la prioridad de fusión de
pares de nodos de eventos.

<!-- fórmula-no-decodificada -->

Los pesos de penalización w s , w m y w t determinan colectivamente la
comportamiento de PEMF. Cuando sólo se aplica la penalización por similitud, el
La estrategia degenera en una compresión basada en similitudes. Usando la combinación
La penalización por conteo por sí sola conduce a un comportamiento similar a la reducción de resolución uniforme.
Cuando la penalización por distancia temporal se utiliza de forma aislada, el método
se aproxima a FIFO. Al ajustar estos pesos de penalización, nuestro método
permite un equilibrio flexible entre estas estrategias, lo que le permite
adaptarse eficazmente a diversas tareas de streaming, permitiendo un equilibrio entre
ahorro de almacenamiento eficiente y retención de información relevante para la tarea
en diversos escenarios del mundo real.

Los nodos seleccionados para fusionarse se determinan identificando el par
con el puntaje de penalización global más bajo. Empleamos ToMe [3] para el
proceso de fusión, comprimiendo el número de tokens visuales a la mitad
tokens totales del par de nodos seleccionados. Al recibir una consulta de un usuario, el
características visuales de todos los nodos raíz en PEMF, junto con todos los visuales
Las funciones almacenadas en FSTW se introducen en el LLM para soportar el tiempo real,
interacción de transmisión.

## 3.2 Conjunto de datos de ajuste de instrucciones: OnlineIT

Los conjuntos de datos de vídeos largos sin conexión existentes a menudo presentan un sesgo distributivo,
donde normalmente se concentra la evidencia clave para responder las preguntas
en medio del vídeo. Como resultado, los MLLM afinaron dichos datostienden a enfatizar demasiado el contenido histórico, lo que potencialmente conduce a
alucinaciones al interpretar con precisión el momento actual. aunque
Se han publicado algunos conjuntos de datos para la comprensión de la transmisión de video [5,
60, 63, 47], siguen siendo limitados en términos de volumen de datos, calidad y
diversidad de tareas. Para abordar estas limitaciones, construimos OnlineIT, un
conjunto de datos de entrenamiento diseñado específicamente para la transmisión de video
comprensión. OnlineIT se centra en la comprensión detallada de eventos y
comprensión espaciotemporal en tiempo real en entornos de transmisión, y
mejora significativamente el rendimiento de MLLM en transmisión de video
comprensión de las tareas.

OnlineIT-general. Basado en criterios de diversidad, longitud y
dificultad, seleccionamos y refinamos varios archivos finos de alta calidad existentes.
conjuntos de datos de ajuste de la comprensión de la transmisión de video [23, 60, 47]. edificio
A partir de estos, desarrollamos aún más dos nuevos conjuntos de datos que comprenden 32K de alta definición.
Instancias de capacitación en streaming de calidad. Este conjunto de datos presenta una mayor
escala, distribución más amplia y mayor diversidad de tareas, facilitando
el aprendizaje de representaciones de vídeo en streaming más generalizables.

Unidad de TI en línea. Incluye 89.000 instancias de capacitación de control de calidad en streaming desde
Escenarios de conducción autónoma. Este conjunto de datos está diseñado para mejorar los MLLM
rendimiento en tareas posteriores complejas y en tiempo real. Específicamente, por
Integrando la semántica de la escena, las normas de tráfico y la conducción común.
eventos, extraemos elementos clave de escenas de conducción y videoclips para
generar un conjunto de datos de preguntas y respuestas basado en la conducción autónoma
contextos. OnlineIT-Drive cubre principalmente cuatro áreas: (1) tiempo real
localización y conciencia semántica, (2) comprensión del tráfico estático
entidades, (3) comprensión de las entidades de tráfico dinámico y (4) riesgo
Evaluación de eventos y accidentes.

Figura 3: (a) La distribución de los tipos de tareas y el número de pares de control de calidad. (b) El proceso detallado para la construcción del Banco ODV. (c) Ejemplos de tareas típicas en ODV-Bench.

<!-- imagen -->

## 4 Banco ODV

Muchos puntos de referencia existentes para escenarios de transmisión se derivan de
conjuntos de datos de evaluación de video sin conexión [39, 23, 36, 61], y es posible que no
Reflejan adecuadamente las aplicaciones del mundo real de la transmisión de vídeo.
comprensión. Aunque algunos de ellos ya incorporan vídeos Ego4D de
actividades diarias [36, 61], estas muestras de evaluación evalúan principalmente
La capacidad de los MLLM para percibir escenas estáticas y narrar el entorno humano
interacciones de forma escalonada. Por el contrario, la conducción autónoma
presenta entornos dinámicos y de alto riesgo con escenas que cambian rápidamente,
Interacciones complejas de múltiples agentes (vehículos, peatones y tráfico).
señales) y tareas de predicción exigentes (como la evaluación de riesgos y
planificación del movimiento). Estos escenarios requieren modelos que equilibren el largo plazo.
Memoria de eventos con percepción detallada a corto plazo para evitar accidentes.
y tomar decisiones oportunas. Para abordar esta brecha, presentamos ODV-Bench,
un punto de referencia diseñado específicamente para la comprensión de videos en línea en
Escenarios de conducción autónoma.

## 4.1 Formulación de tareas

Como se muestra en la Figura 3 (a), primero exploramos los elementos clave del tráfico en
escenarios de conducción autónoma y resumirlos en tres categorías de
Escenarios de tareas: (1) Tareas estáticas orientadas a objetivos, que implican la
reconocimiento y recuperación de elementos de tráfico parados, como por ejemplo
señales, luces e indicadores viales; (2) Tareas dinámicas orientadas a objetivos,
que se centran en el comportamiento y la predicción de la trayectoria de carreteras dinámicas.participantes como vehículos y peatones; y (3) orientado a eventos
Tareas para la interacción de múltiples agentes, que capturan interacciones complejas.
escenarios de riesgo y accidentes que involucran a múltiples agentes. A continuación, guiado por
señales temporales y las necesidades prácticas de conducir, definimos además
tipos de tareas detalladas basadas en estas categorías para
Evaluar la comprensión del modelo en escenarios realistas de vídeo de conducción en línea.
Para obtener más detalles sobre la formulación de tareas, consulte el Apéndice C.1.

## 4.2 Construcción de referencia

El proceso de construcción de ODV-Bench se ilustra en la Figura 3 (b). Nosotros
Adoptar un enfoque de cuatro etapas para garantizar la calidad de cada generación.
pregunta y luego presente algunos ejemplos de tareas típicas en diferentes
escenarios de conducción en la Figura 3 (c).

Recopilación de datos. (1) Selección de vídeos. Para alinearse con la conducción en el mundo real
escenarios, primero seleccionamos 6 conjuntos de datos [24, 14, 82, 66, 73, 4] de
diferentes escenarios de tareas dentro del ámbito de la conducción autónoma, desde
conducción normal a eventos inesperados. Luego, diseñamos un semiautomático.
canalización que se basa principalmente en el filtrado de anotaciones y basado en YOLO
detección [25], complementada con inspección manual, para seleccionar tareas-
vídeos relevantes del conjunto de datos recopilados. (2) Metaanotación
Generación. Para obtener metanotaciones con detalles espaciotemporales y
información semántica, desarrollamos métodos personalizados basados en
anotaciones del conjunto de datos. Para conjuntos de datos bien anotados, convertimos efectivamente
etiquetas existentes en metaanotaciones específicas de tareas. Para otros, nosotros
Diseñar una tubería semiautomática que comienza con anotaciones gruesas.
generado por VLLM y YOLO [25], seguido de humano estructurado
verificación para garantizar la calidad.

Tabla 1: Resultados de la evaluación en ODV-Bench. Nuestro modelo supera significativamente a los MLLM de video en línea y fuera de línea de última generación en condiciones de prueba de disparo cero y logra mejoras adicionales después de realizar ajustes en los datos del dominio de conducción.| Método | Tamaño | #Marcos | Objetivo estático | Objetivo estático | Objetivo estático | Objetivo estático | Objetivo estático | Objetivo estático | Objetivo estático | Objetivo dinámico | Objetivo dinámico | Objetivo dinámico | Objetivo dinámico | Orientado a eventos | Orientado a eventos | Orientado a eventos | Orientado a eventos | General ||---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|
| Método | Tamaño | #Marcos | RTP | HD | KIE | DCT | DDM | PTM | Promedio                            | AP | LP | PD | Promedio                            | PR | RA | ARA | Promedio                            | General || Humano | Humano | Humano | Humano | Humano | Humano | Humano | Humano | Humano | Humano | Humano | Humano | Humano | Humano | Humano | Humano | Humano | Humano | Humano || Agentes Humanos | - | - | 96,8 | 97,6 | 98,2 | 95,7 | 95,9 | 94,4 | 95,9 | 83,7 | 87,9 | 90,4 | 88,2 | 91,9 | 94,9 | 93,0 | 92,5 | 91,4 || MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto || MiniCPM-V2.6 [65] | 7B | 64 | 20.0 | 87,8 | 15.1 | 49.1 | 26.4 | 20.6 | 27.3 | 71,2 | 73,4 | 47,2 | 60,0 | 73,4 | 33.3 | 16.7 | 53,6 | 49,8 || LargoVA [75] | 7B | 64 | 29,9 | 7.3 | 37,7 | 47,3 | 38,0 | 33,6 | 31,8 | 66,6 | 58,6 | 50,9 | 56,6 | 57,5 | 58.1 | 46,2 | 56,7 | 50,2 || LLaVA-Onevisión [28] | 7B | 64 | 36,0 | 4.9 | 22,6 | 60,0 | 31,4 | 39,0 | 34.2 | 53,6 | 70,3 | 47,4 | 55.1 | 57,9 | 72,2 | 47,4 | 62,2 | 51,6 || PasanteVL2.5 [8] | 8B | 32 | 40.1 | 16.3 | 37,7 | 52,7 | 30,4 | 40,9 | 37,2 | 64.1 | 84,6 | 49,5 | 62,5 | 54,0 | 60,6 | 50,6 | 56.1 | 54,2 || VideoChat-Flash [33] | 7B | 256 | 29,6 | 15,5 | 45,3 | 76,4 | 26.1 | 36.1 | 32.2 | 73,5 | 75,3 | 47,2 | 61,0 | 67,1 | 64,8 | 46,2 | 64,3 | 54,4 || Qwen2.5-VL [2] | 7B | 1fps | 51,8 | 8.1 | 79,3 | 49.1 | 36,0 | 57,3 | 48,3 | 50,4 | 82,6 | 46,9 | 57,5 | 47,6 | 78,6 | 52,6 | 59,4 | 55,6 || MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto || Flash VStream [72] | 7B | 1fps | 25.4 | 1.6 | 11.3 | 50,9 | 36,0 | 22.1 | 24,8 | 25,5 | 39,8 | 47,2 | 40.2 | 32,4 | 48,6 | 30.1 | 38.1 | 35,7 || Dispider [47] | 7B | 1fps | 31.1 | 7.3 | 34,0 | 63,6 | 34,0 | 35,4 | 32,5 | 43,2 | 73.1 | 45,8 | 52,7 | 38,2 55,4 |                                 | 36,5 | 44,3 | 45,2 || VideoChat-Online [23] | 4B | 1fps | 36,9 | 0,8 | 62,3 | 49.1 | 21,5 | 47,0 | 36.1 | 70,2 | 86,7 | 46,4 | 62,9 | 51,2 | 69,4 | 45,5 | 57,4 | 54,5 || StreamBosque | 7B | 1fps | 51,4 | 15,5 | 54,7 | 56,4 | 38,6 | 65,3 | 51,5 | 72,6 | 83,2 | 46,0 | 62,3 | 60,2 | 73,3 | 47,4 | 63,8 | 59,9 || StreamForest (unidad FT) | 7B | 1fps | 70.1 | 17.1 | 100,0 | 60,0 | 32,7 | 83,6 | 64,6 | 64,0 | 96,6 | 59,6 | 70,7 | 71,8 | 93,4 | 58,3 | 78,5 | 71,2 |Construcción de control de calidad. (1) Generación MCQ. Para permitir un control de calidad automático eficiente
generación, primero diseñamos plantillas precisas y diversas adaptadas a
cada tarea definida. Estas plantillas luego se completan con información detallada
y anotaciones precisas para generar pares de control de calidad de alta calidad. A continuación, nosotros
desarrollar un canal de generación de opciones múltiples basado en un conjunto de opciones,
introducir distractores plausibles pero engañosos junto con los correctos
respuesta para garantizar el realismo y la eficacia de las elecciones. (2) Calidad
Controlar. Para garantizar una calidad de referencia, primero realizamos varias rondas
de revisión manual para verificar la claridad y precisión de los pares de control de calidad y la
plausibilidad de las opciones de distractor. Además, para mejorar la diversidad de escenas.
y el equilibrio del tipo de tarea, aplicamos una estrategia de muestreo que asigna
preguntas proporcionalmente a la duración del vídeo, maximizando la cobertura en
escenarios.

## 5 experimentos

Detalles de implementación. Adoptamos SigLiP-so400M[70] como visual
codificador, utilizamos un MLP como cabezal de proyección y empleamos Qwen2-7B como
el LLM. De forma predeterminada, la cantidad de tokens visuales tiene un límite de 8192.
Entre ellos, 729 tokens se asignan a la percepción en tiempo real, mientras que
La memoria espaciotemporal a corto plazo consta de 18 cuadros, cada uno representado
por 128 fichas visuales. Establecemos los pesos de penalización por similitud, fusionamos
recuento y distancia temporal a 0,4, 0,4 y 0,2, respectivamente. el
El modelo se entrena en 32 GPU A100 utilizando nuestro conjunto de datos OnlineIT propuesto.
complementado con datos de vídeo sin conexión de VideoChat-Flash [33] y
LLaVA-Video [76], así como datos de imágenes de LLaVA-OneVision [28]. Nosotros
Adopte una estrategia de capacitación de cinco etapas para capacitar a StreamForest desde cero.
Las tres primeras etapas siguen el paradigma de formación de larga duración fuera de línea.
vídeo MLLM [33]. La cuarta etapa realiza el ajuste fino de la transmisión de video.
para producir el StreamForest base. Además, una quinta etapa opcional puede
incorporarse mediante la formación con el OnlineIT-Drive, que produce la
StreamForest(FTdrive). Durante la fase de evaluación, restringimos la
modelo para procesar cuadros de transmisión a 1 FPS. Para más detalles
detalles de implementación, consulte el Apéndice D.

## 5.1 Resultados de referencia en línea

Evaluamos el rendimiento de nuestro modelo en cuatro puntos de referencia para online
respuesta a preguntas en vídeo: ODV-Bench, StreamingBench [39], OVBench [23],
y OVO-Bench [36]. Estos puntos de referencia siguen el control de calidad de la transmisión de video.
escenarios, donde los VideoLLM deben procesar solo el contenido del video
disponible antes de la marca de tiempo actual.

Banco ODV. Integra estrechamente la información espaciotemporal para
evaluar exhaustivamente la capacidad de los MLLM para comprender
detalles en videos en línea y hacer predicciones futuras basadas en ambos
Contexto histórico y actual en escenarios de conducción autónoma. el
El punto de referencia incluye tareas como identificar objetos o acciones sutiles,
describir posiciones de objetos y relaciones espaciales, y pronosticar
trayectorias de objetos. Estas tareas requieren una sólida capacidad espaciotemporal en tiempo real.
percepción y comprensión contextual. Como se muestra en la Tabla 1,
StreamForest logra una precisión promedio del 59,9% en ODV-Bench sin
recibir formación en OnlineIT-drive y mejora aún más hasta el 71,2% después
entrenando en ello. Esto supera significativamente a todos los existentes en línea y
MLLM fuera de línea, lo que demuestra la fuerte capacidad de generalización de nuestra
Método para realizar tareas de comprensión de vídeo en tiempo real. Estos resultados
resaltar su potencial para aplicaciones del mundo real.Tabla 2: Comparación de nuestro método con enfoques existentes en tareas de respuesta a preguntas en video en varios escenarios. Nuestro enfoque supera significativamente los métodos anteriores en materia de pruebas comparativas de comprensión de vídeo en streaming, al tiempo que mantiene un rendimiento sólido y competitivo en la comprensión de vídeo tanto largo como corto.|                                 |                                 | Vídeo en línea | Vídeo en línea | Vídeo en línea | Vídeo largo | Vídeo largo | Vídeo corto | Vídeo corto ||---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|
| Método | Tamaño | StreamingBench en tiempo real Todo | Promedio del banco OV | Mono de banco OVO | VideoMME sin sub.               | MLVU M-Promedio | Promedio de MVBench | Prueba de percepción Val || MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto || PasanteVL2 [9] | 8B | 63,7 | 48,7 | 50,1 | 54,0 | 64,0 | 65,8 | - || LargoVA [75] | 7B | 60,0 | 43,6 | - | 52,6 | 56,3 | - | - || LLaVA-OneVision [28] | 7B | 71.1 | 49,5 | 52,9 | 58,2 | 64,7 | 56,7 | 57,1 || Qwen2-VL [55] | 7B | 69,0 | 49,7 | 52,7 | 63,3 | -| 67,0 | 66,9 || LongVU [50] | 7B | -| -| 48,5 | 60,6 | 65,4 | 66,9 | -|| Vídeo LLaVA [76] | 7B | - | - | 53.1 | 63,3 | 70,8 | 58,6 | 67,9 || MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto || VideoLLM-en línea [5] | 8B | 36,0 | 9.6 | 12.8 | - | - | - | - || PelículaChat [52] | 7B | - | 30,9 | - | 38,2 | - | 55.1 | - || Flash VStream [72] | 7B | 23.2 | 31.2 | 33.2 | - | - | - | - || VideoChat-Online [23] | 4B | - | 54,9 | - | 52,8 | - | 64,9 | - || Dispider [47] | 7B | 67,6 | -| 41,8 | 57,2 | 61,7 | -| -|| StreamBosque | 7B | 77,3 | 60,5 | 55,6 | 61,4 | 70,0 | 70,2 | 73.1 || StreamForest (unidad FT) | 7B | 76,8 | 61,6 | 55,6 | 61,9 | 69,6 | 68,6 | 71,6 |StreamingBench &amp; OVBanco y Banco OVO. Como se muestra en la Tabla 2, StreamForest
demuestra un sólido rendimiento en la transmisión de código abierto existente
puntos de referencia de comprensión de vídeo. Alcanza una precisión del 77,3% en
StreamingBench, 60,5% en OVBench y 55,6% en OVO-Bench. Estos
Los resultados impresionantes resaltan la solidez de StreamForest en una amplia gama.
gama de escenarios de comprensión de vídeos en línea. El rendimiento superior
de nuestro modelo se puede atribuir a dos innovaciones arquitectónicas clave.
En primer lugar, la ventana espaciotemporal de grano fino permite una visualización espacial precisa.
percepción y modelado temporal responsivo a corto plazo, que son
crítico para la percepción en tiempo real y las tareas de respuesta directa. En segundo lugar,
El bosque de memoria de eventos persistentes organiza de forma adaptativa imágenes visuales a largo plazo.
contenido en un bosque de memoria estructurado y eficiente, significativamente
mejorar la capacidad del MLLM para retener y razonar sobre eventos pasados.
Juntos, estos dos módulos ofrecen capacidades complementarias que permiten
nuestro modelo para manejar entradas de video de streaming dinámicas y de largo horizonte
eficazmente, manteniendo al mismo tiempo una alta coherencia contextual.

## 5.2 Resultados de referencia sin conexión

Evaluamos más a fondo nuestro método en dos videos largos de comprensión.
puntos de referencia (VideoMME[16] y MLVU[77]) y dos conjuntos de datos de vídeo cortos
(MVBench[31] y PerceptionTest[45]). En el entorno fuera de línea, todo el
El vídeo se proporciona como entrada al MLLM. Tomamos muestras de cuadros de video a 1 FPS,
con un límite máximo de 2048 fotogramas. Para vídeos que superen este límite,
los fotogramas se muestrean uniformemente a lo largo de toda la duración. Como se muestra en
Tabla 2, nuestro método demuestra un rendimiento superior tanto en largo como en
Tareas de comprensión de videos cortos en comparación con lo último en tecnología.
MLLM de vídeo en línea. Además, supera a los principales modelos fuera de línea.
en la mayoría de los puntos de referencia, logrando un 61,4% en VideoMME, un 70,0% en MLVU, un 70,2% en
MVBench y 73,1% en PerceptionTest. Este sólido desempeño fuera de línea
escenarios resalta la sólida capacidad de generalización de nuestra
método propuesto.

## 5.3 Ablaciones

Eficacia del Bosque de Memoria de Eventos Persistentes: Sustituimos el
PEMF propuesto con varios métodos utilizados en trabajos anteriores. Para asegurar una
comparación justa, mantenemos los presupuestos de tokens visuales consistentes en todos
métodos y ajustar cada modelo en consecuencia. El modelo de resultados de la ablación
sin FSTW y PEMF

Tabla 3: Comparación entre nuestro PEMF propuesto y otras estrategias de memoria comúnmente utilizadas.| Política de memoria |   Promedio del banco OV |   Mono de banco OVO |   MLVU M-Promedio ||--------------------|---------------|---------------------|--------------|| Muestreo uniforme |          58,2 |                52,7 |         69,4 || Primero en entrar, primero en salir |          58,7 |                52,9 |         56,7 || Fusión de similitud [52] |          60,3 |                53,4 |         68,0 || Banco de memoria piramidal [23] |          60,3 |                53,9 |         68,2 || PEMF (Nuestro) |          60,5 |                55,6 |         70,0 |Tabla 4: Estudio de ablación sobre los componentes clave de StreamForest.

sin FSTW

sin PEMF

sin evento

Nuestro OVBench OVO-Bench MLVU

Promedio general M-Promedio

58.0

59.1

58,9

59,4

60,5

52,5

53,7

53,5

52,6

55,6

51,8

69,4

56,6

69.1

70.0

se muestran en la Tabla 3. La estrategia FIFO muestra el peor desempeño.
Esto es especialmente evidente en el benchmark de vídeo largo MLVU (56,7% vs.
70,0%), donde el método falla debido al descarte sin filtrar de
Características visuales históricas. OVBench enfatiza principalmente el corto plazo,
Percepción espaciotemporal de grano fino. El muestreo uniforme reduce la
resolución de información visual reciente, que es crucial para el tiempo real
comprensión (58,2% frente a 60,5% en OVBench). La fusión de similitud logra
rendimiento comparable a nuestro PEMF en OVBench (60,3% frente a 60,5%).
Sin embargo, sus limitaciones se vuelven claras en tareas que requieren perseverancia.
Memoria y razonamiento a largo plazo. En OVO-Bench, PEMF supera
Combinación de similitud en un +2,2% y en MLVU en un +2,0%. esto es porque
La fusión basada en similitudes puede fusionar demasiado los fotogramas dentro del vídeo local.
segmentos, lo que potencialmente conduce a irregularidades espaciotemporales y la
pérdida de representaciones locales a nivel de eventos. El banco de memoria piramidal
Mantiene la memoria mediante el reemplazo de cuadros. Sin embargo, la capacidad fija
limita su capacidad para capturar características espaciotemporales de largo alcance (53,9%
vs 55,6% en OVO-Bench y 68,2% vs 70,0 en MLVU). En contraste, nuestro
El método evalúa cada evento visual basándose en la similitud a nivel de evento,
recuento de fusiones y distancia temporal. Luego realiza memoria.
consolidación a nivel de eventos. Esta estrategia apoya la eficiencia y
mantenimiento persistente de características visuales históricas.

Efectividad de la Arquitectura General: Realizamos estudios de ablación
en tres componentes arquitectónicos clave. Específicamente, realizamos la ablación del
ventana espaciotemporal granulada y el bosque de memoria de eventos persistentes,
al mismo tiempo que se garantiza que el número total de tokens visuales se mantenga constante
con la configuración original. Además, reemplazamos los basados en eventos.
Construcción de nodos con un enfoque basado en marcos. Como se muestra en la Tabla 4,
La eliminación de ambos módulos provoca la caída de rendimiento más significativa.
El uso solo de FSTW o PEMF mejora el rendimiento en comparación con el
línea de base, pero los mejores resultados se logran cuando ambos componentes son
integrado (+2,5% en OVBench, +3,1% en OVO-Bench y +18,2% en MLVU).
Esta ablación conjunta confirma que FSTW y PEMF proporcionan servicios complementarios.
beneficios. FSTW mejora la percepción espaciotemporal en tiempo real cerca del
marca de tiempo de consulta, mientras que PEMF admite eficiencia y persistencia a largo plazo
memoria, lo que en conjunto produce el rendimiento general más sólido. Además,
La construcción de nodos a nivel de evento previene eficazmente la fusión excesiva dentro
eventos, permitiendo la compresión de características visuales al nivel de
eventos visuales completos en lugar de fotogramas individuales.

<!-- imagen -->

Límite de tokens visuales

Figura 4: Rendimiento con distintos presupuestos de tokens visuales.

<!-- imagen -->

Figura 5: Tiempo de inferencia promedio bajo diferentes presupuestos de tokens visuales (en
GPU A100 única).

Robustez ante diferentes presupuestos de tokens visuales: evaluamos la robustez
de nuestro método bajo diferentes restricciones presupuestarias para tokens visuales,
que van desde 1K a 10K. La Figura 4 ilustra la variación del rendimiento.
en los puntos de referencia MLVU, VideoMME y OVO-Bench bajo estas configuraciones.
En particular, bajo la estricta restricción de solo 1.000 tokens visuales,
StreamForest logra una relación de compresión visual promedio de hasta el 99,8%en pruebas comparativas de videos largos. A pesar de este nivel extremo de compresión, el
El modelo aún mantiene un desempeño competitivo, lo que fuertemente
demuestra la solidez de nuestro enfoque para preservar persistentemente
recuerdos visuales a largo plazo a nivel de evento. También realizamos un directo
Comparación entre nuestro PEMF y Similarity Merge. Los resultados
demuestran claramente dos beneficios principales de nuestro enfoque. Primero, PEMF
exhibe un rendimiento absoluto superior, superando consistentemente
Combinación de similitudes en todos los presupuestos de tokens con una precisión promedio
mejora del 2-3%. En segundo lugar, nuestro método muestra una mayor resiliencia a
compresión extrema. Bajo el presupuesto de tokens de 1K más severo, PEMF retiene
una fracción mayor de su desempeño presupuestario total, logrando un notable
+1,8% de ventaja de retención relativa en VideoMME. Estos experimentales
Los resultados confirman que las mejoras en el rendimiento se derivan del efecto intrínseco.
diseño de PEMF, que consolida de forma adaptativa la memoria a nivel de eventos para
preservar la información semánticamente destacada, garantizando así una alta
precisión y eficiencia de tokens bajo estrictas limitaciones de recursos.
La Figura 5 presenta el tiempo de inferencia promedio de StreamForest bajo
diferentes presupuestos de tokens visuales. La velocidad de inferencia rápida y estable.
destaca la aplicabilidad práctica de StreamForest.

Tabla 5: Análisis cuantitativo del tiempo de ejecución y uso de memoria de
Bosque de corriente.

Tabla 6: Comparación del tiempo de ejecución de PEMF con otros mecanismos de memoria para 500 fotogramas.|   Marcos de entrada |   Memoria (GB) |   FLOP (T) |   Latencia(s) | Método |   Vis. Codificar(es) |   Memoria. Actualización(es) |   LLMinfer (s) |   Total (es) ||----------------|---------------|-------------|---------------|-----------------------|-------------------|-------------------|----------------|-------------|
|             64 |          15.8 |        93.1 |         0,776 | Fusión de similitud [52] |             5.198 |             0,183 |          1.388 |       6.769 ||            256 |          17.1 |       134,1 |         1.126 | PMB [23] |             5.203 |             0,451 |          1.381 |       7.035 ||           1024 |          17.2|       137,3 |         1.497 | PEMF (Nuestro) |             5.218 |             0,172 |          1.394 |       6.784 |Coste computacional: Proporcionamos un análisis cuantitativo del tiempo de ejecución.
y uso de memoria de StreamForest. Aislar el efecto de la memoria.
mecanismo, asumimos que las características visuales a nivel de cuadro ya están
extraído por el codificador de visión en tiempo real. Como se muestra en la Tabla 5, PEMF
impone un límite superior estricto de tokens visuales (8K aquí), asegurando que
El uso de la memoria permanece estable (~ 17 GB) independientemente del número de
fotogramas procesados. En consecuencia, los FLOP y la latencia de inferencia no
crecer significativamente incluso con entradas de transmisión más largas. Además, nosotros
comparar PEMF con otros mecanismos de memoria, incluida la memoria piramidal
Bank [23] y la estrategia Similarity Merge [52]. Como se resume en la tabla
6, el tiempo de ejecución general está dominado por la codificación de visión (5,2 s para 500
fotogramas, ∼ 95 FPS). La actualización de la memoria de PEMF es extremadamente liviana
(0,172 s para 500 fotogramas), lo cual es insignificante en comparación con la eficiencia
ganancias logradas al reducir significativamente el número total de imágenes
fichas.

Impacto de los datos de capacitación: la Tabla 7 presenta el impacto de nuestra capacitación.
estrategia que integra conjuntos de datos tanto en línea como fuera de línea. Los resultados
demostrar claramente que combinar OnlineIT con offline existente
Los conjuntos de datos de VideoQA mejoran significativamente el rendimiento en la transmisión de vídeo
comprensión de los puntos de referencia. OnlineIT está diseñado específicamente para aplicaciones reales.
percepción del tiempo y predicción futura en escenarios de streaming,
mitigar eficazmente las alucinaciones causadas por inconsistencias entre
Contexto histórico espaciotemporal y momento actual.

## 6 Conclusiones

En este trabajo, hemos propuesto StreamForest, una arquitectura novedosa para
comprensión de la transmisión de video que aborda las limitaciones en
Memoria a término y percepción detallada. Al introducir el Persistente
Event Memory Forest, nuestro método gestiona eficazmente imágenes históricas
información a través de la fusión adaptativa guiada por la distancia temporal,
similitud de contenido y penalizaciones por recuento de fusiones. Junto con el fino-
Ventana espaciotemporal granulada, el modelo mantiene una precisión
comprensión del panorama actual. También presentamos OnlineIT, un
transmisión de video comprensión del conjunto de datos de ajuste fino que mitiga
problemas de cambio espaciotemporal y mejora la percepción en tiempo real y
razonamiento. Además de ODV-Bench, un nuevo punto de referencia diseñado para tiempo real
Escenarios de conducción autónoma. Amplios experimentos demuestran que
StreamForest no sólo supera a los MLLM de vídeo en streaming de última generación
pero también compite con los principales MLLM de video fuera de línea bajo estrictas entradas de transmisión
configuraciones, mostrando su robustez y valor práctico en tiempo real
Aplicaciones de comprensión de transmisión de video.

Tabla 7: La contribución de nuestros datos de entrenamiento al rendimiento en el punto de referencia de comprensión de transmisión de video. O.general se refiere a OnlineIT-general, mientras que O.drive se refiere a OnlineIT-drive.| Datos | Banco ODV Banco OVB Banco OVO | Banco ODV Banco OVB Banco OVO | Banco ODV Banco OVB Banco OVO ||-------------------------------|-------------------------------|-------------------------------|-------------------------------||                            | Promedio | Promedio | General || bases | 56,3 | 53,9 | 53,5 || base + O.general | 59,9 | 60,5 | 55,6 || base + O.general + O.drive | 71,2 | 61,6 | 55,6 |## Reconocimiento

Este trabajo cuenta con el apoyo de National Key R &amp; Programa D de China (No.
2022ZD0160900), la Fundación de Ciencias Naturales de la provincia de Jiangsu (No.
BK20250009), y el Centro de innovación colaborativa de software novedoso
Tecnología e Industrialización.

## Referencias

- [1] Antrópico. Soneto de Claude 3.5, 2024. URL https://www.anthropic.com/news/claude-3-5-sonnet.
- [2] Shuai Bai, Keqin Chen, Xuejing Liu, Jialin Wang, Wenbin Ge, Sibo Song, Kai Dang, Peng Wang, Shijie Wang, Jun Tang, et al. Qwen2. Informe técnico de 5 vl. Preimpresión de arXiv arXiv:2502.13923, 2025.
- [3] Daniel Bolya, Cheng-Yang Fu, Xiaoliang Dai, Peizhao Zhang, Christoph Feichtenhofer y Judy Hoffman. Fusión de tokens: tu vit pero más rápido. Preimpresión de arXiv arXiv:2210.09461, 2022.
- [4] Zhengping Che, Guangyu Li, Tracy Li, Bo Jiang, Xuefeng Shi, Xinsheng Zhang, Ying Lu, Guobin Wu, Yan Liu y Jieping Ye. D²-city: un conjunto de datos de vídeo con cámara para salpicadero a gran escala de diversos escenarios de tráfico. Preimpresión de arXiv arXiv:1904.01975, 2019.
- [5] Joya Chen, Zhaoyang Lv, Shiwei Wu, Kevin Qinghong Lin, Chenan Song, Difei Gao, Jia-Wei Liu, Ziteng Gao, Dongxing Mao y Mike Zheng Shou. Videollm-online: Modelo de lenguaje grande de vídeo online para streaming de vídeo. En Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 18407-18418, 2024.
- [6] Joya Chen, Ziyun Zeng, Yiqi Lin, Wei Li, Zejun Ma y Mike Zheng Shou. Livecc: película de vídeo de aprendizaje con transmisión de transcripción de voz a escala. Preimpresión de arXiv arXiv:2504.16030, 2025.
- [7] Yukang Chen, Fuzhao Xue, Dacheng Li, Qinghao Hu, Ligeng Zhu, Xiuyu Li, Yunhao Fang, Haotian Tang, Shang Yang, Zhijian Liu, et al. Longvila: escalamiento de modelos de lenguaje visual de contexto largo para videos largos. Preimpresión de arXiv arXiv:2408.10188, 2024.
- [8] Zhe Chen, Weiyun Wang, Yue Cao, Yangzhou Liu, Zhangwei Gao, Erfei Cui, Jinguo Zhu, Shenglong Ye, Hao Tian, Zhaoyang Liu, et al. Ampliar los límites de rendimiento de los modelos multimodales de código abierto con escalamiento de modelo, datos y tiempo de prueba. Preimpresión de arXiv arXiv:2412.05271, 2024.
- [9] Zhe Chen, Weiyun Wang, Hao Tian, Shenglong Ye, Zhangwei Gao, Erfei Cui, Wenwen Tong, Kongzhi Hu, Jiapeng Luo, Zheng Ma, et al. ¿Qué tan lejos estamos de gpt-4v? cerrando la brecha con los modelos multimodales comerciales con suites de código abierto. Ciencias Ciencias de la Información de China, 67(12):220101, 2024.
- [10] Zesen Cheng, Sicong Leng, Hang Zhang, Yifei Xin, Xin Li, Guanzheng Chen, Yongxin Zhu, Wenqi Zhang, Ziyang Luo, Deli Zhao, et al. Videollama 2: Avance del modelado espacio-temporal y la comprensión del audio en películas de vídeo. Preimpresión de arXiv arXiv:2406.07476, 2024.
- [11] Shangzhe Di, Zhelun Yu, Guanghao Zhang, Haoyuan Li, Tao Zhong, Hao Cheng, Bolin Li, Wanggui He, Fangxun Shu y Hao Jiang. Transmisión de preguntas y respuestas en video con recuperación de caché kv de video en contexto. Preimpresión de arXiv arXiv:2503.00540, 2025.
- [12] Xin Ding, Hao Wu, Yifan Yang, Shiqi Jiang, Donglin Bai, Zhibo Chen y Ting Cao. Streammind: Desbloqueo de diálogos de transmisión de video a velocidad de cuadros completa a través de cognición controlada por eventos. Preimpresión de arXiv arXiv:2503.06220, 2025.
- [13] Heng Fan, Liting Lin, Fan Yang, Peng Chu, Ge Deng, Sijia Yu, Hexin Bai, Yong Xu, Chunyuan Liao y Haibin Ling. Lasot: un punto de referencia de alta calidad para el seguimiento de un solo objeto a gran escala. En CVPR, páginas 5374-5383, 2019.- [14] Jianwu Fang, Lei-lei Li, Junfei Zhou, Junbin Xiao, Hongkai Yu, Chen Lv, Jianru Xue y Tat-Seng Chua. Comprensión del vídeo de accidentes con visión abductiva del ego para una percepción de conducción segura. En Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 22030-22040, 2024.
- [15] Jiajun Fei, Dian Li, Zhidong Deng, Zekun Wang, Gang Liu y Hui Wang. Video-ccam: mejora de la comprensión del videolenguaje con máscaras causales de atención cruzada para videos cortos y largos. Preimpresión de arXiv arXiv:2408.14023, 2024.
- [16] Chaoyou Fu, Yuhan Dai, Yongdong Luo, Lei Li, Shuhuai Ren, Renrui Zhang, Zihan Wang, Chenyu Zhou, Yunhang Shen, Mengdan Zhang, et al. Video-mme: el primer punto de referencia de evaluación integral de películas multimodales en análisis de video. Preimpresión de arXiv arXiv:2405.21075, 2024.

- [17] Jiyang Gao, Chen Sun, Zhenheng Yang y Ram Nevatia. Alto: localización de actividad temporal mediante consulta de idioma. En ICCV, páginas 5267-5275, 2017.
- [18] Chunhui Gu, Chen Sun, Sudheendra Vijayanarasimhan, Caroline Pantofaru, David A. Ross, George Toderici, Yeqing Li, Susanna Ricco, Rahul Sukthankar, Cordelia Schmid y Jitendra Malik. Ava: un conjunto de datos de vídeo de acciones visuales atómicas localizadas espacio-temporalmente. CVPR, 2017.
- [19] Bin Huang, Xin Wang, Hong Chen, Zihan Song y Wenwu Zhu. Vtimellm: permite que llm capte momentos de vídeo. En Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 14271-14280, 2024.
- [20] De-An Huang, Shijia Liao, Subhashree Radhakrishnan, Hongxu Yin, Pavlo Molchanov, Zhiding Yu y Jan Kautz. Lita: Asistente de localización temporal de idiomas. En Conferencia europea sobre visión por computadora, páginas 202-218. Springer, 2024.
- [21] Gabriel Huang, Bo Pang, Zhenhai Zhu, Clara Rivera y Radu Soricut. Entrenamiento previo multimodal para subtítulos de videos densos. En Kam-Fai Wong, Kevin Knight y Hua Wu, editores, Actas de la 1.ª Conferencia del Capítulo Asia-Pacífico de la Asociación de Lingüística Computacional y la 10.ª Conferencia Internacional Conjunta sobre Procesamiento del Lenguaje Natural, diciembre de 2020.
- [22] Lianghua Huang, Xin Zhao y Kaiqi Huang. Got-10k: un gran punto de referencia de alta diversidad para el seguimiento de objetos genéricos en la naturaleza. Transacciones IEEE sobre análisis de patrones e inteligencia artificial, 43(5): 1562-1577, 2021. doi: 10.1109/TPAMI.2019.2957464.
- [23] Zhenpeng Huang, Xinhao Li, Jiaqi Li, Jing Wang, Xiangyu Zeng, Cheng Liang, Tao Wu, Xi Chen, Liang Li y Limin Wang. Comprensión de vídeo online: Ovbench y videochat-online. En CVPR, páginas 3328-3338, 2025.
- [24] Salman Khan, Izzeddin Teeti, Reza Javanmard Alitappeh, Mihaela C Stoian, Eleonora Giunchiglia, Gurkirt Singh, Andrew Bradley y Fabio Cuzzolin. Road-waymo: Concientización de acción a escala para la conducción autónoma. Preimpresión de arXiv arXiv:2411.01683, 2024.
- [25] Rahima Khanam y Muhammad Hussain. Yolov11: una descripción general de las mejoras arquitectónicas clave. Preimpresión de arXiv arXiv:2410.17725, 2024.
- [26] Ranjay Krishna, Kenji Hata, Frederic Ren, Li Fei-Fei y Juan Carlos Niebles. Eventos con subtítulos densos en videos. En ICCV, 2017.
- [27] Ranjay Krishna, Yuke Zhu, Oliver Groth, Justin Johnson, Kenji Hata, Joshua Kravitz, Stephanie Chen, Yannis Kalantidis, Li-Jia Li, David A Shamma, et al. Genoma visual: conectando el lenguaje y la visión mediante anotaciones de imágenes densas de colaboración colectiva. IJCV, 123:32-73, 2017.- [28] Bo Li, Yuanhan Zhang, Dong Guo, Renrui Zhang, Feng Li, Hao Zhang, Kaichen Zhang, Peiyuan Zhang, Yanwei Li, Ziwei Liu, et al. Llama-onevision: Fácil transferencia visual de tareas. Preimpresión de arXiv arXiv:2408.03326, 2024.
- [29] Hongyu Li, Jinyu Chen, Ziyu Wei, Shaofei Huang, Tianrui Hui, Jialin Gao, Xiaoming Wei y Si Liu. Llava-st: un modelo de lenguaje grande multimodal para una comprensión espacial-temporal detallada. Preimpresión de arXiv arXiv:2501.08282, 2025.
- [30] KunChang Li, Yinan He, Yi Wang, Yizhuo Li, Wenhai Wang, Ping Luo, Yali Wang, Limin Wang y Yu Qiao. Videochat: comprensión del vídeo centrada en el chat. Preimpresión de arXiv arXiv:2305.06355, 2023.
- [31] Kunchang Li, Yali Wang, Yinan He, Yizhuo Li, Yi Wang, Yi Liu, Zun Wang, Jilan Xu, Guo Chen, Ping Luo, et al. Mvbench: un punto de referencia integral de comprensión de vídeo multimodal. En Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 22195-22206, 2024.
- [32] Wei Li, Bing Hu, Rui Shao, Leyang Shen y Liqiang Nie. Lion-fs: Rápido y rápido Pensador lento del videolenguaje como asistente de video en línea. Preimpresión de arXiv arXiv:2503.03663, 2025.
- [33] Xinhao Li, Yi Wang, Jiashuo Yu, Xiangyu Zeng, Yuhan Zhu, Haian Huang, Jianfei Gao, Kunchang Li, Yinan He, Chenting Wang, et al. Videochat-flash: compresión jerárquica para modelado de vídeo de contexto largo. Preimpresión de arXiv arXiv:2501.00574, 2024.
- [34] Xinhao Li, Ziang Yan, Desen Meng, Lu Dong, Xiangyu Zeng, Yinan He, Yali Wang, Yu Qiao, Yi Wang y Limin Wang. Videochat-r1: mejora de la percepción espacio-temporal mediante el ajuste del refuerzo. Preimpresión de arXiv arXiv:2504.06958, 2025.

- [35] Yanwei Li, Chengyao Wang y Jiaya Jia. Llama-vid: Una imagen vale 2 tokens en modelos de lenguaje grandes. En Conferencia europea sobre visión por computadora, páginas 323-340. Springer, 2024.
- [36] Yifei Li, Junbo Niu, Ziyang Miao, Chunjiang Ge, Yuanhang Zhou, Qihao He, Xiaoyi Dong, Haodong Duan, Shuangrui Ding, Rui Qian, et al. Ovo-bench: ¿Qué tan lejos están tus películas de video de la comprensión de los videos en línea del mundo real? Preimpresión de arXiv arXiv:2501.05510, 2025.
- [37] Bin Lin, Yang Ye, Bin Zhu, Jiaxi Cui, Munan Ning, Peng Jin y Li Yuan. Video-llava: Aprendizaje de la representación visual unida por alineación antes de la proyección. Preimpresión de arXiv arXiv:2311.10122, 2023.
- [38] Ji Lin, Hongxu Yin, Wei Ping, Pavlo Molchanov, Mohammad Shoeybi y Song Han. Vila: Sobre el entrenamiento previo para modelos de lenguaje visual. En Actas de la conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 26689-26699, 2024.
- [39] Junming Lin, Zheng Fang, Chi Chen, Zihao Wan, Fuwen Luo, Peng Li, Yang Liu y Maosong Sun. Streamingbench: Evaluación de la brecha entre los mllms para lograr la comprensión de la transmisión de video. Preimpresión de arXiv arXiv:2411.03628, 2024.
- [40] Jiajun Liu, Yibing Wang, Hanghang Ma, Xiaoping Wu, Xiaoqi Ma, Xiaoming Wei, Jianbin Jiao, Enhua Wu y Jie Hu. Canguro: un potente modelo de lenguaje de vídeo que admite entrada de vídeo de contexto largo. Preimpresión de arXiv arXiv:2408.15542, 2024.
- [41] Zhijian Liu, Ligeng Zhu, Baifeng Shi, Zhuoyang Zhang, Yuming Lou, Shang Yang, Haocheng Xi, Shiyi Cao, Yuxian Gu, Dacheng Li, et al. Nvila: Modelos eficientes de lenguaje visual de frontera. Preimpresión de arXiv arXiv:2412.04468, 2024.
- [42] Zuyan Liu, Yuhao Dong, Ziwei Liu, Winston Hu, Jiwen Lu y Yongming Rao. Oryx mllm: comprensión espacio-temporal bajo demanda con resolución arbitraria. Preimpresión de arXiv arXiv:2409.12961, 2024.- [43] Andreea-Maria Oncescu, Joao F Henriques, Yang Liu, Andrew Zisserman y Samuel Albanie. Queryd: un conjunto de datos de vídeo con texto de alta calidad y narraciones de audio. En ICASSP, páginas 2265-2269. IEEE, 2021.
- [44] OpenAI. Hola gpt-4o. https://openai.com/index/hello-gpt-4o, 2024.
- [45] Viorica P˘ atr˘ aucean, Lucas Smaira, Ankush Gupta, Adri` a Recasens Continente, Larisa Markeeva, Dylan Banarse, Skanda Koppula, Joseph Heyward, Mateusz Malinowski, Yi Yang, Carl Doersch, Tatiana Matejovicova, Yury Sulsky, Antoine Miech, Alex Frechette, Hanna Klimczak, Raphael Koster, Junlin Zhang, Stephanie Winkler, Yusuf Aytar, Simon Osindero, Dima Damen, Andrew Zisserman y Jo˜ ao Carreira. Prueba de percepción: un punto de referencia de diagnóstico para modelos de vídeo multimodal. En Avances en sistemas de procesamiento de información neuronal, 2023. URL https://openreview.net/forum?id=HYEGXFnPoq.
- [46] Rui Qian, Xiaoyi Dong, Pan Zhang, Yuhang Zang, Shuangrui Ding, Dahua Lin y Jiaqi Wang. Transmisión de comprensión de vídeos largos con modelos de lenguaje de gran tamaño. Avances en los sistemas de procesamiento de información neuronal, 37:119336-119360, 2024.
- [47] Rui Qian, Shuangrui Ding, Xiaoyi Dong, Pan Zhang, Yuhang Zang, Yuhang Cao, Dahua Lin y Jiaqi Wang. Dispider: Habilitar películas de video con interacción activa en tiempo real a través de percepción, decisión y reacción desenredadas. Preimpresión de arXiv arXiv:2501.03218, 2025.
- [48] Shuhuai Ren, Linli Yao, Shicheng Li, Xu Sun y Lu Hou. Timechat: un modelo de lenguaje grande multimodal sensible al tiempo para la comprensión de videos largos. En Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 14313-14323, 2024.
- [49] Hao Shao, Yuxuan Hu, Letian Wang, Guanglu Song, Steven L Waslander, Yu Liu y Hongsheng Li. Lmdrive: conducción de circuito cerrado de un extremo a otro con modelos de lenguaje grandes. En Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 15120-15130, 2024.
- [50] Xiaoqian Shen, Yunyang Xiong, Changsheng Zhao, Lemeng Wu, Jun Chen, Chenchen Zhu, Zechun Liu, Fanyi Xiao, Balakrishnan Varadarajan, Florian Bordes, et al. Longvu: compresión adaptativa espaciotemporal para la comprensión de videolenguaje de larga duración. Preimpresión de arXiv arXiv:2410.17434, 2024.
- [51] Yan Shu, Zheng Liu, Peitian Zhang, Minghao Qin, Junjie Zhou, Zhengyang Liang, Tiejun Huang y Bo Zhao. Video-xl: modelo de lenguaje de visión extralarga para comprensión de videos a escala de horas. Preimpresión de arXiv arXiv:2409.14485, 2024.
- [52] Enxin Song, Wenhao Chai, Guanhong Wang, Yucheng Zhang, Haoyang Zhou, Feiyang Wu, Haozhe Chi, Xun Guo, Tian Ye, Yanting Zhang, et al. Moviechat: desde token denso hasta memoria escasa para una comprensión prolongada de vídeos. En Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 18221-18232, 2024.

- [53] Yansong Tang, Dajun Ding, Yongming Rao, Yu Zheng, Danyang Zhang, Lili Zhao, Jiwen Lu y Jie Zhou. COIN: un conjunto de datos a gran escala para un análisis integral de videos instructivos. En CVPR, páginas 1207-1216, 2019.
- [54] Equipo Gemini, Petko Georgiev, Ving Ian Lei, Ryan Burnell, Libin Bai, Anmol Gulati, Garrett Tanzer, Damien Vincent, Zhufeng Pan, Shibo Wang, et al. Géminis 1.5: desbloquear la comprensión multimodal a través de millones de tokens de contexto. Preimpresión de arXiv arXiv:2403.05530, 2024.- [55] Peng Wang, Shuai Bai, Sinan Tan, Shijie Wang, Zhihao Fan, Jinze Bai, Keqin Chen, Xuejing Liu, Jialin Wang, Wenbin Ge, et al. Qwen2-vl: Mejora de la percepción del mundo del modelo de visión-lenguaje en cualquier resolución. Preimpresión de arXiv arXiv:2409.12191, 2024.
- [56] Weiyun Wang, Yiming Ren, Haowen Luo, Tiantong Li, Chenxiang Yan, Zhe Chen, Wenhai Wang, Qingyun Li, Lewei Lu, Xizhou Zhu, et al. El proyecto que todo lo ve v2: Hacia la comprensión general de las relaciones del mundo abierto. Preimpresión de arXiv arXiv:2402.19474, 2024.
- [57] Xidong Wang, Dingjie Song, Shunian Chen, Chen Zhang y Benyou Wang. Longllava: escalar películas multimodales a 1000 imágenes de manera eficiente a través de una arquitectura híbrida. Preimpresión de arXiv arXiv:2409.02889, 2024.
- [58] Yi Wang, Kunchang Li, Xinhao Li, Jiashuo Yu, Yinan He, Guo Chen, Baoqi Pei, Rongkun Zheng, Zun Wang, Yansong Shi, Tianxiang Jiang, Songze Li, Jilan Xu, Hongjie Zhang, Yifei Huang, Yu Qiao, Yali Wang y Limin Wang. Internvideo2: Escalamiento de modelos básicos para la comprensión de videos multimodales. En ECCV, páginas 396-416, 2024.
- [59] Yi Wang, Xinhao Li, Ziang Yan, Yinan He, Jiashuo Yu, Xiangyu Zeng, Chenting Wang, Changlian Ma, Haian Huang, Jianfei Gao, et al. Vídeo interno2. 5: Potenciar los videos mllms con modelos de contexto extensos y ricos. Preimpresión de arXiv arXiv:2501.12386, 2025.
- [60] Yueqian Wang, Xiaojun Meng, Yuxuan Wang, Jianxin Liang, Jiansheng Wei, Huishhuai Zhang y Dongyan Zhao. Videollm sabe cuándo hablar: mejora de la comprensión de videos urgentes con formato de interacción de dúo de video-texto, 2024. URL https://arxiv.org/abs/2411.17991.
- [61] Haomiao Xiong, Zongxin Yang, Jiazuo Yu, Yunzhi Zhuge, Lu Zhang, Jiawen Zhu y Huchuan Lu. Transmisión de comprensión de video e interacción de múltiples rondas con conocimiento mejorado con memoria. Preimpresión de arXiv arXiv:2501.13468, 2025.
- [62] Ziang Yan, Zhilin Li, Yinan He, Chenting Wang, Kunchang Li, Xinhao Li, Xiangyu Zeng, Zilei Wang, Yali Wang, Yu Qiao, et al. Optimización de preferencias de tareas: mejora de modelos de lenguaje grande multimodal con alineación de tareas de visión. Preimpresión de arXiv arXiv:2412.19326, 2024.
- [63] Zhenyu Yang, Yuhang Hu, Zemin Du, Dizhan Xue, Shengsheng Qian, Jiahong Wu, Fan Yang, Weiming Dong y Changsheng Xu. Svbench: un punto de referencia con diálogos temporales de múltiples turnos para la comprensión de video en streaming. Preimpresión de arXiv arXiv:2502.10810, 2025.
- [64] Linli Yao, Yicheng Li, Yuancheng Wei, Lei Li, Shuhuai Ren, Yuanxin Liu, Kun Ouyang, Lean Wang, Shicheng Li, Sida Li, et al. Timechat-online: el 80% de los tokens visuales son naturalmente redundantes en la transmisión de videos. Preimpresión de arXiv arXiv:2504.17343, 2025.
- [65] Yuan Yao, Tianyu Yu, Ao Zhang, Chongyi Wang, Junbo Cui, Hongji Zhu, Tianchi Cai, Haoyu Li, Weilin Zhao, Zhihui He, et al. Minicpm-v: Un mllm de nivel gpt-4v en tu teléfono. Preimpresión de arXiv arXiv:2408.01800, 2024.
- [66] Fisher Yu, Haofeng Chen, Xin Wang, Wenqi Xian, Yingying Chen, Fangchen Liu, Vashisht Madhavan y Trevor Darrell. Bdd100k: un conjunto de datos de conducción diverso para un aprendizaje multitarea heterogéneo. En Actas de la conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 2636-2645, 2020.
- [67] Licheng Yu, Patrick Poirson, Shan Yang, Alexander C Berg y Tamara L Berg. Modelado del contexto en expresiones referentes. En ECCV, páginas 69-85. Saltador, 2016.
- [68] Abhay Zala, Jaemin Cho, Satwik Kottur, Xilun Chen, Barlas Oguz, Yashar Mehdad y Mohit Bansal. Recuperación jerárquica de momentos de vídeo y subtítulos por pasos. En CVPR, páginas 23056-23065, 2023.- [69] Xiangyu Zeng, Kunchang Li, Chenting Wang, Xinhao Li, Tianxiang Jiang, Ziang Yan, Songze Li, Yansong Shi, Zhengrong Yue, Yi Wang, et al. Timesuite: mejora de mllms para la comprensión de vídeos largos mediante un ajuste basado en tierra. Preimpresión de arXiv arXiv:2410.19702, 2024.
- [70] Xiaohua Zhai, Basil Mustafa, Alexander Kolesnikov y Lucas Beyer. Pérdida de sigmoide para el preentrenamiento de imágenes del lenguaje. En Actas de la conferencia internacional IEEE/CVF sobre visión por computadora, páginas 11975-11986, 2023.

- [71] Boqiang Zhang, Kehan Li, Zesen Cheng, Zhiqiang Hu, Yuqian Yuan, Guanzheng Chen, Sicong Leng, Yuming Jiang, Hang Zhang, Xin Li, et al. Videollama 3: Fundación de modelos multimodales de frontera para la comprensión de imágenes y videos. Preimpresión de arXiv arXiv:2501.13106, 2025.
- [72] Haoji Zhang, Yiqin Wang, Yansong Tang, Yong Liu, Jiashi Feng, Jifeng Dai y Xiaojie Jin. Flash-vstream: comprensión en tiempo real basada en memoria para transmisiones de video largas. Preimpresión de arXiv arXiv:2406.08085, 2024.
- [73] Jun-Bo Zhang, Wei Feng, Meng-Biao Zhao, Fei Yin, Xu-Yao Zhang y Cheng-Lin Liu. Detección de texto en vídeo con representación sólida de funciones. Transacciones IEEE sobre circuitos y sistemas para tecnología de vídeo, 34(6):4407-4420, 2023.
- [74] Pan Zhang, Xiaoyi Dong, Yuhang Cao, Yuhang Zang, Rui Qian, Xilin Wei, Lin Chen, Yifei Li, Junbo Niu, Shuangrui Ding, et al. Internlm-xcomposer2. 5-omnilive: Un sistema multimodal integral para interacciones de audio y video en streaming a largo plazo. Preimpresión de arXiv arXiv:2412.09596, 2024.
- [75] Peiyuan Zhang, Kaichen Zhang, Bo Li, Guangtao Zeng, Jingkang Yang, Yuanhan Zhang, Ziyue Wang, Haoran Tan, Chunyuan Li y Ziwei Liu. Larga transferencia de contexto del lenguaje a la visión. Preimpresión de arXiv arXiv:2406.16852, 2024.
- [76] Yuanhan Zhang, Jinming Wu, Wei Li, Bo Li, Zejun Ma, Ziwei Liu y Chunyuan Li. Ajuste de instrucciones en vídeo con datos sintéticos. Preimpresión de arXiv arXiv:2410.02713, 2024.
- [77] Junjie Zhou, Yan Shu, Bo Zhao, Boya Wu, Shitao Xiao, Xi Yang, Yongping Xiong, Bo Zhang, Tiejun Huang y Zheng Liu. Mlvu: un punto de referencia integral para la comprensión de vídeos largos multitarea. Preimpresión de arXiv arXiv:2406.04264, 2024.
- [78] Luowei Zhou, Chenliang Xu y Jason J. Corso. Hacia el aprendizaje automático de procedimientos a partir de vídeos instructivos web. En Conferencia AAAI sobre Inteligencia Artificial, 2017. URL https://api.semanticscholar.org/CorpusID:19713015.
- [79] Haoyi Zhu, Honghui Yang, Yating Wang, Jiange Yang, Limin Wang y Tong He. Spa: la conciencia espacial 3D permite una representación corporal efectiva. Preimpresión de arXiv arXiv:2410.08208, 2024.
- [80] Zhe Zhu, Dun Liang, Songhai Zhang, Xiaolei Huang, Baoli Li y Shimin Hu. Detección y clasificación de señales de tráfico en la naturaleza. En Actas de la conferencia IEEE sobre visión por computadora y reconocimiento de patrones, páginas 2110-2118, 2016.
- [81] Orr Zohar, Xiaohan Wang, Yann Dubois, Nikhil Mehta, Tong Xiao, Philippe Hansen-Estruch, Licheng Yu, Xiaofang Wang, Felix Juefei-Xu, Ning Zhang, et al. Apollo: una exploración de la comprensión del vídeo en grandes modelos multimodales. Preimpresión de arXiv arXiv:2412.10360, 2024.
- [82] Jannik Zürn, Paul Gladkov, Sof´ıa Dudas, Fergal Cotter, Sofi Toteva, Jamie Shotton, Vasiliki Simaiaki y Nikhil Mohan. Wayvescenes101: un conjunto de datos y punto de referencia para la síntesis de vistas novedosas en la conducción autónoma. Preimpresión de arXiv arXiv:2407.08280, 2024.

## A Detalles de la marca de tiempo del nodo

La marca de tiempo de cada nodo de evento se inicializa como el tiempo promedio delfotogramas que representa (por ejemplo, si un nodo de evento abarca fotogramas de 10 a
14s, su marca de tiempo se inicializa como 12s). Cuando dos nodos de eventos están
fusionado, la marca de tiempo del nuevo nodo se calcula como un recuento de tokens.
promedio ponderado de los nodos originales:

<!-- fórmula-no-decodificada -->

donde ti, t j son las marcas de tiempo de los nodos de eventos originales, y n i
, n j son los números de tokens visuales contenidos en cada nodo,
respectivamente. Este esquema ponderado evita la deriva de la marca de tiempo durante
múltiples rondas de fusión, especialmente cuando los nodos fusionados contienen
cantidades significativamente diferentes de fichas visuales.

## B Detalles de OnlineIT

En esta sección, proporcionamos una descripción completa de la tarea.
categorización y distribución de datos del conjunto de datos OnlineIT. es
Diseñado específicamente para mejorar la comprensión de la transmisión de video.
capacidades de los MLLM en términos de percepción en tiempo real, futuro
predicción y continuidad de eventos. Como se muestra en la Tabla 8, el conjunto de datos es
dividido en dos componentes principales: OnlineIT-general, que apunta
comprensión general de transmisión de video y OnlineIT-drive, que se enfoca
sobre escenarios de conducción autónoma. Cada subconjunto está cuidadosamente diseñado para
cubrir una amplia gama de tareas de percepción y razonamiento detalladas
con anotaciones de alta calidad.

Tabla 8: Tipos de tareas y volúmenes de datos de OnlineIT.| Conjunto de datos | Categorías | Tarea | Fuente | Número de instancia ||------------------|---------------------|-----------------------------------------------------------------|----------------------------------------------|----------------|| OnlineIT-general | Percepción espacial | Conexión a tierra espacial | RefCOCO [67] | ∼ 43k || OnlineIT-general |                           | Comprensión espacial de múltiples rondas | Allseeing-V2 [56] Genoma visual [27] | ∼ 45 000 ∼ 43 000 || OnlineIT-general |                           | VQA espacial con conexión a tierra | Todo lo ve-V2 [56] | ∼ 43k || OnlineIT-general |                           | Localización espacial relativa | LaSOT [13] | ∼ 19k ||                  |                           |                                                                 | Charadas-STA [17] | ∼ 11k ||                  | Percepción temporal | Puesta a tierra temporal | HiREST [68] QuerYD [43] | ∼ 0,4k ∼ 13k ||                  |                           | Razonamiento Localización temporal Conexión a tierra temporal multiformato | ActividadNet-RTL [20] | ∼ 10 000 ∼ 20 000 ||                  |                           |                                                                 | InternVid-VTime [19] |                ||                  |                           | Localización de la acción espaciotemporal | AVÁ [18] | ∼ 6k ||                  | Percepción espaciotemporal | Seguimiento hacia atrás de objetos | LaSOT [13] | ∼ 51 000 ∼ 58 000 ||                  |                           |                                                                 | TENGO [22] LaSOT [13] | ∼ 14k ||                  |                           | Detección espaciotemporal | ActivityNet-Captions ViTT [78] Youcook2 [21] | ∼ 10k ∼ 5k ||                  | Percepción del evento | Subtítulos de vídeo densos | [26] | ∼ 1k ||                  |                           | Memoria Pasada | D²-Ciudad [4] | ∼ 9k ||                  | Objetivo estático | Percepción en tiempo real | TT100k [80] | ∼ 46k || Unidad de TI en línea | Objetivo dinámico | Predicción de localización | D²-Ciudad [4] | ∼ 13k ∼ 1k ||                  |                           | Predicción de distancia de movimiento | Camino-waymo [24] Camino-waymo [24] | ∼ 7k ||                  |                           | Razonamiento del accidente | MM-AU [14] | ∼ 6k ||                  | Orientado a eventos |                                                                 |                                              | ∼ 7k ||                  |                           | Análisis de Riesgos | MM-AU [14] |                |## B.1 TI en línea-general

OnlineIT-general abarca una amplia gama de tareas diseñadas para fomentar una
Comprensión integral del contenido de vídeo espaciotemporal en streaming.
ajustes. Como se muestra en la Tabla 8, el conjunto de datos se clasifica en cuatro
tipos de tareas primarias: percepción espacial, percepción temporal,
percepción espaciotemporal y percepción de eventos. Para garantizar la diversidad,
solidez y cobertura de tareas detallada, compilamos y refinamos datos
de una amplia gama de fuentes. En total, OnlineIT-general comprende más de
400.000 instancias que abarcan varios niveles de dificultad y duraciones de vídeo.

Percepción espacial. Este tipo de tarea incluye cuatro subtareas. espacial
La conexión a tierra requiere que el modelo genere el cuadro delimitador que indica el
Ubicación de un objeto consultado. La comprensión espacial de múltiples rondas implica
identificar la ubicación espacial del objeto a través de un diálogo de múltiples turnos o
generar un título para el objeto dentro de una región espacial especificada.
Spatial Grounded VQA combina la respuesta visual a preguntas con tecnología espacial.
localización, lo que requiere que el modelo proporcione el cuadro delimitador del
área relevante al responder la pregunta. Espacial relativo
La localización desafía al modelo a determinar la posición de un
objeto especificado en relación con la escena general. Estas tareas enfatizan
fundamento y razonamiento espacial, que son cruciales para mejorar una
Percepción espacial detallada del modelo en transmisión de video en tiempo real
escenarios.

Percepción temporal. Esta categoría consta de tres subtareas. temporales
La conexión a tierra implica interpretar una consulta en lenguaje natural e identificar
las marcas de tiempo de inicio y finalización del segmento de vídeo correspondiente. en
escenarios de transmisión, el modelo también debe evaluar si lo descrito
El evento está actualmente en curso. El razonamiento de la localización temporal requiere
Identificar el lapso de tiempo relevante de un evento mientras se responde una
pregunta basada en el razonamiento. Localización temporal multiformato
incorpora diálogos de un solo turno y de múltiples turnos, cubriendo un
amplia gama de formatos de preguntas. Estas tareas se centran en fortalecer
la capacidad del MLLM para rastrear y razonar sobre dependencias temporales,
mejorar su comprensión de los momentos actuales y pasados en un video
corriente.

Percepción espaciotemporal. Este tipo de tarea integra espacios y
razonamiento temporal e incluye tres subtareas. Acción espaciotemporal
La localización requiere que el modelo prediga tanto la ubicación espacial como la
la acción que realiza un objetivo en un momento de consulta específico. Objeto
El seguimiento hacia atrás asigna al modelo la tarea de identificar la ubicación actual
de un objeto y rastrear su posición en puntos de tiempo anteriores, como
uno o dos segundos antes. La detección espaciotemporal opera sobre
ventanas temporales más amplias, preguntando si un objeto visible en el
El fotograma actual existía hace varios segundos o requería que el modelo
localizar un objeto en un momento histórico específico y determinar su
duración de la existencia. Estas tareas combinan señales espaciotemporales para
Capture acciones, movimientos y transiciones, lo que permite que el modelo realice un seguimiento.
trayectorias de objetos y anticipar estados futuros basados en el pasado y
contexto actual.

Percepción de eventos. Esta categoría incluye dos subtareas. Vídeo denso
Los subtítulos implican detectar una secuencia de eventos en un vídeo y
generando marcas de tiempo correspondientes junto con descripciones de alto nivel.
La localización de pasos y los subtítulos se diferencian al centrarse en la segmentación y
narrando pasos procesales clave en videos de formato largo. Estas tareas sondestinado a mejorar la comprensión del modelo de complejos, de varios pasos
eventos, permitiendo la interpretación estructurada de secuencias dinámicas en
Comprensión de transmisión de video.

## B.2 Unidad de TI en línea

OnlineIT-drive está diseñado específicamente para el ámbito del streaming
Comprensión del vídeo en la conducción autónoma. El conjunto de datos incluye 89k
instancias, que están organizadas en tres categorías principales de tareas.
En conjunto, estas tareas apuntan a fortalecer no sólo el tiempo real
capacidades de percepción, sino también el razonamiento temporal y la toma de decisiones.
capacidades de creación de MLLM en situaciones de alto riesgo y en rápida evolución
ambientes.

Comprensión del objetivo estático. Para mejorar la capacidad del modelo para estática.
comprensión de la escena, se introducen dos tipos de tareas. Percepción en tiempo real
Requiere que el modelo perciba e interprete con precisión la semántica.
y atributos espaciales de objetivos relacionados con el tráfico tal como aparecen en realidad
tiempo. Past Memory evalúa la capacidad del modelo para retener y recuperar
la semántica y las características espaciotemporales de los objetivos de tráfico que
fueron observados en un momento anterior. Estas tareas en conjunto mejoran
La capacidad del modelo para percibir, comprender y recordar estática.
Elementos del tráfico y contexto ambiental, como la infraestructura vial.
y señalización reglamentaria.

Comprensión dinámica del objetivo. Incluye dos tipos de tareas que tienen como objetivo
mejorar la comprensión predictiva de los participantes del tráfico dinámico.
La predicción de ubicación requiere que el modelo estime la posición futura
de un objetivo en movimiento en función de su trayectoria de movimiento histórica. Mover
La predicción de distancia se centra en predecir la distancia recorrida entre
el vehículo del ego y otros agentes en movimiento, dados los relacionados con el movimiento.
observaciones. Estas tareas están diseñadas para mejorar la capacidad del modelo para
rastrear objetos en continuo movimiento y anticipar trayectorias futuras.

Razonamiento orientado a eventos. Se pretende fomentar el desarrollo de
Habilidades de razonamiento necesarias para la evaluación de riesgos y accidentes.
interpretación. El análisis de riesgos requiere que el modelo detecte posibles
fuentes de peligro en la escena del tráfico actual y evaluar la
probabilidad de que ocurra un accidente. El razonamiento del accidente implica post hoc
análisis, donde el modelo debe inferir las causas de un accidente observado
y articular estrategias preventivas plausibles. Estas tareas están diseñadas
mejorar la capacidad del modelo para razonar sobre relaciones causales y
anticipar o reflexionar sobre los riesgos del tráfico con conciencia contextual.

## C Detalles de ODV-Bench

En esta sección, detallamos la taxonomía de tareas y la formulación del ODV-
Bench, así como las estadísticas del conjunto de datos. Clasificamos escenarios de tareas
basado en entidades objetivo y derivar percepción clave y tarea de razonamiento
tipos para cada escenario en la Tabla 9.

Tabla 9: Descripción general de las categorías de tareas, sus subcategorías y plantillas de preguntas.| Escenario objetivo de tarea | Subtarea | Ejemplos de consultas ||-------------------------------|------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|| Objetivo estático | Percepción del tráfico en tiempo real | 1) ¿Cuál es el significado de la señal de tráfico en [0.61,0.31,0.64,0.38] en la imagen actual? 2) ¿Cuáles son las coordenadas de posición de la señal de tráfico que indica "Paso de peatones" en la imagen actual? 3) ¿Cuál es el significado del cartel de la carretera en [0.77,0.08,0.88,0.2] en la imagen actual? 4) Según el cartel de la carretera en [0.09,0.46,0.19,0.52] actualmente, si vamos en dirección izquierda, ¿a dónde iremos? ¿A qué distancia está? 5) ¿Cuál es el color del semáforo en [0.42,0.01,0.45,0.13] en la imagen actual? ¿Y cuál es su indicación? 6) Según el cartel de la carretera en [0.65,0.03,0.76,0.16] actualmente, ¿a qué distancia está de Fengle? || Objetivo estático | Memoria del tráfico pasado | 1) ¿Cuáles eran las coordenadas de posición de la señal de tráfico que indicaba "Prohibido girar a la izquierda" en la escena hace 3 segundos? 2) ¿Cuál era el significado de la señal de tráfico en [0.39,0.13,0.41,0.17] en la escena hace 1 segundo? 3) La señal de tráfico se encuentra actualmente en [0.92,0.02,0.96,0.09]. ¿Cuáles eran sus coordenadas hace 2 segundos? 4) ¿Cuál era el color del semáforo en [0.35,0.1,0.38,0.22] en la escena hace 2 segundos?                                                                                                                                                                                                                                                 || Objetivo estático | Impulsando la toma de decisiones | 1) Según el cartel de la carretera en [0.37,0.18,0.47,0.31] en la imagen tomada hace 2 segundos, si queremos ir a Renhe, ¿en qué dirección debemos ir y qué tan lejos está? 2) Según el cartel de la carretera en [0.51,0.18,0.61,0.3] actualmente, si queremos ir a la avenida Libai, ¿en qué dirección debemos ir y qué tan lejos está? 3) Según el cartel de la carretera en [0.38,0.04,0.63,0.23] actualmente, si queremos girar a la izquierda, ¿en qué carril debemos estar?                                                                                                                                                                                                               || Objetivo estático | Extracción de información clave | 1) Si queremos ir a Suzhou, ¿a qué objetivo deberíamos prestarle más atención actualmente? Proporcione el tipo y las coordenadas.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       || Objetivo estático | Detección de alucinaciones | 1) Según el cartel de la carretera en [0.38,0.31,0.52,0.48], ¿a qué distancia está actualmente de la ciudad de Qingpu? 2) ¿Cuál es el significado de la señal de tráfico en [0.34,0.29,0.38,0.38] en la imagen actual? 3) ¿Cuál es el color del semáforo en [0.9,0.86,0.95,0.92] en la imagen actual?                                                                                                                                                                                                                                                                                                                                                                                || Objetivo estático | Detección de cambios de tráfico | 1) En este momento, ¿el semáforo que indica "girar a la derecha" se ha vuelto completamente verde? 2) En este momento, ¿el semáforo de delante se ha puesto completamente rojo?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   ||                               | Predicción de acción | 1) ¿Cuál será el estado de movimiento posterior del automóvil que se encuentra actualmente en la ubicación [0.993, 0.615, 1.0, 0.63]?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         || Objetivo dinámico | Predicción de ubicación | 1) ¿Cómo será el cuadro de posición del peatón en la ubicación [0.544, 0.561, 0.613, 0.895] en el próximo segundo?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            ||                               | Predicción de distancia | 1) ¿La distancia entre nuestro automóvil y el automóvil en [0.488, 0.488, 0.501, 0.494] está aumentando o acercándose?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       ||                               | Predicción de riesgos | 1) ¿Existe una alta probabilidad de que ocurran accidentes de tránsito dentro de un período determinado en el futuro? 2) ¿Habrá riesgos de tráfico importantes dentro de un período determinado en el futuro?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              || Evento de interacción multiagente | Análisis de Riesgos | 1) Actualmente existe un alto riesgo de accidentes de tráfico. Según el entorno, ¿qué tipos de accidentes es probable que ocurran y cuál es la base de esta predicción? 2) Actualmente existen importantes riesgos de tráfico. Según el entorno, ¿cuáles son las fuentes de estos riesgos y qué tipos de accidentes podrían provocar?                                                                                                                                                                                                                                                                                                                                           || Evento de interacción multiagente | Respuesta al motivo del accidente | 1) ¿Cuál es la causa del accidente en el video? ¿Qué medidas se pueden tomar para evitarlo?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |## C.1 Taxonomía y formulación de tareas

Primero identificamos las categorías principales de entidades de tráfico relevantes para
conducción autónoma y organizar escenarios de tareas en tres grupos: (1)
Tareas para objetivos estáticos, que implican el reconocimiento y recuperación
de elementos de tráfico estacionarios como señales de tráfico, semáforos y carreteras.
indicadores; (2) Tareas para objetivos dinámicos, que se centran en el comportamiento.
predicción y localización de entidades en movimiento como vehículos y
peatones; y (3) Tareas para eventos de interacción multiobjetivo, que
capturar interacciones complejas, escenarios de riesgo y accidentes que involucran
múltiples agentes. Basado en estas categorías y guiado por señales temporales
y las necesidades prácticas de la conducción, definimos con más detalle las tareas detalladas
tipos para evaluar exhaustivamente la comprensión del modelo de forma realista en línea
Escenarios de vídeo de conducción.

Figura 6: Ejemplos de cada tarea en ODV-Bench. Las 12 tareas se dividen en tres modos de percepción diferentes para la comprensión de vídeos en línea para la conducción autónoma.

<!-- imagen -->

## C.1.1 Tareas para objetivos estáticos

Los elementos estáticos del tráfico, como las señales de tráfico y los indicadores de la carretera, juegan
un papel crucial en la toma de decisiones y la prevención de peligros en condiciones normales.
condiciones de conducción. Evaluar la capacidad del modelo para recuperar y
reconocer estos elementos en transmisiones de video en línea, diseñamos un dedicado
conjunto de tareas. Específicamente, distinguimos entre percepción básica
tareas y tareas de razonamiento más avanzadas, y refinarlas aún más según
sobre señales temporales y necesidades prácticas de conducción: (1) Tráfico en tiempo real
Percepción: Percibir e interpretar la semántica y ubicaciones espaciales.
de elementos de tráfico estáticos en tiempo real; (2) Memoria de tráfico anterior: recuperación
y rastrear la semántica y los estados espaciotemporales de observaciones previamente observadas.
elementos estáticos; (3) Toma de decisiones de conducción: tomar decisiones de conducción
basado en la información percibida; (4) Extracción de información clave:
Identificar y localizar elementos clave del tráfico críticos para las decisiones de conducción;
(5) Detección de alucinaciones: identificar preguntas irrelevantes para el
entrada de vídeo existente; y (6) Detección de cambios de tráfico: detectar
marcas de tiempo para cambios en elementos de tráfico, como semáforos.

## C.1.2 Tareas para objetivos dinámicos

La posición y el comportamiento de otros participantes en la carretera, como vehículos.
y peatones, son factores de referencia cruciales que influyen en la autonomía
decisiones de conducción y seguridad. La capacidad de predecir la posición y
El comportamiento de los objetos de tráfico dinámicos es esencial para garantizar la seguridad de los vehículos.
conducción autónoma. Por lo tanto, nos centramos en las siguientes tres tareas para
evaluar eficazmente esta capacidad: (1) Predicción de acción: predecir
la próxima acción de vehículos y peatones basada en continua
señales espaciotemporales; (2) Predicción de distancia: predecir la relativa
cambio de distancia entre el ego-vehículo y otros vehículos basado en
información de movimiento; y (3) Predicción de ubicación: predecir el futuro
Posición espacial de objetivos de tráfico dinámicos en función de su movimiento.
trayectorias.

## C.1.3 Tareas para eventos de interacción con múltiples objetivos

Para lograr una conducción autónoma segura y fiable, el sistema debe poder
Identificar riesgos y analizar accidentes en interacciones viales complejas.
escenarios. En el contexto de las transmisiones de vídeo en línea, esta capacidad implica
el reconocimiento dinámico y el análisis de interacciones entre múltiples agentes, como
así como la predicción razonable de los riesgos de tráfico. Para evaluar esto
capacidad, diseñamos las siguientes tres categorías de tareas: (1) RiesgoPredicción: predecir la aparición de riesgos de tráfico importantes y
responder proactivamente; (2) Análisis de Riesgos: detectar las fuentes de
riesgos de tráfico actuales y análisis de las posibles causas de accidentes;
y (3) Respuesta al motivo del accidente: análisis posterior al accidente, proporcionando
Causas potenciales del incidente y resumen de lecciones prácticas.
aprendido.

## C.2 Estadísticas del conjunto de datos

ODV-Bench comprende 1.190 videoclips únicos de conducción en primera persona,
abarcando una amplia gama de escenarios de conducción en diferentes
países, desde condiciones de conducción rutinarias hasta peligros potenciales y
accidentes. La duración de los videos varía de 5 segundos a 90 segundos,
capturar eficazmente la diversidad de la conducción en streaming en el mundo real
experiencias. El punto de referencia incluye 6.322 pares de preguntas y respuestas, con un
marca de tiempo de consulta promedio de 18,9 segundos. Específicamente, la estática
La categoría orientada a objetos comprende 247 vídeos con un total de 1.639.
preguntas; la categoría orientada a objetos dinámicos incluye 162 vídeos y
2.973 preguntas; y la categoría orientada a eventos consta de 781 vídeos.
con 1.710 preguntas. Todas las preguntas están en formato de opción múltiple, con
El número de opciones varía entre 2 y 4 dependiendo de la pregunta.
tipo.

## D Más detalles de implementación

Tabla 10: Configuración de parámetros para el preentrenamiento fuera de línea de tres etapas.|          |                                                            | Etapa-1 | Etapa 2 | Etapa 3 ||----------|------------------------------------------------------|-------------------------------|-------------------------------|--------------------------------|| Visión | Resolución × Núm. marcos #Tokens | 384 64 × 4 | 384 × 8 64 × 8 | Máx. 384 × 512 Máx. 16 × 512 || Datos | Conjunto de datos #Muestras | Imagen y vídeo corto 0,6 millones y 0,5 millones | Imagen y vídeo corto 3,8 millones y 3,4 millones | Imagen y vídeo corto/largo 0,5 M y 2,8 M || Modelo | #parámetros entrenables | Proyector 16,98 MB | Modelo completo 8030.35MB | Modelo completo 8030.35MB || Formación | Tamaño de lote LR del codificador de visión LR del conector & Época LLM | 512 1×10-3 1×10-3 1 | 256 2×10-6 1×10-5 1 | 256 2×10-6 1×10-5 1 |Tabla 11: Configuración de parámetros para el ajuste fino en línea de la cuarta etapa y el ajuste fino de la quinta etapa.|                                                                                                                                                                        | Etapa 4 | Etapa5 ||---------------------------------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------|---------------------------------------------|| Conjunto de datos #Muestras | Imagen y (corto/largo/en línea) -Vídeo 0,4 millones y 1,3 millones | Imagen y (corto/largo/en línea) -Vídeo 0,2M y 0,5M || #parámetros entrenables | Proyector&LLM 7632.60MB | Proyector&LLM 7632.60MB || Resolución Marcos FPS | 384 × 384 2 ∼ 512 1 | 384 × 384 2 ∼ 512 1 || Percepción en tiempo real Qouta Cuota de memoria espaciotemporal Total Límites de token visual Similitud Peso de penalización Recuento de fusión Peso de penalización Distancia temporal Peso de penalización | 729 128 × 18 8192 0,4 0,4 ​​0,2 | 729 128 × 18 8192 0,4 0,4 ​​0,2 || Tamaño de lote Época LR | 256 1×10-5 | 256 || Relación de calentamiento de caída de peso optimizador | 1 | 1 × 10 - 5 1 ||                                                                                                                                                                        | Adán W | AdánW 0 ||                                                                                                                                                                        | 0 0,03 |                                             ||                                                                                                                                                                        |                                             | 0,03 || Horario LR | coseno | coseno || Visión Seleccionar capa | -2 | -2 || Números de GPU | 32 | 32 |Adoptamos una estrategia de capacitación en cinco etapas para capacitar sistemáticamente a los
modelo StreamForest propuesto, con el objetivo de explotar plenamente su potencial para
Tareas de comprensión de vídeo en streaming. En las tres primeras etapas,
seguir y ampliar el paradigma de formación de VideoChat-Flash [33],
Emplear capacitación fuera de línea para dotar al modelo de capacidades sólidas.
en comprensión de videos de formato largo y alineación intermodal. Estas etapas
están diseñados progresivamente, cubriendo diversas escalas de datos y tareas.
objetivos, permitiendo que el modelo adquiera gradualmente competencias básicas
tales como alineación básica visión-lenguaje, modelado temporal a largo plazo,
y razonamiento de escenas complejas. Procedimientos de formación detallados y
Las configuraciones de hiperparámetros para estas etapas se proporcionan en la Tabla 10.

En la cuarta y quinta etapa, realizamos ajustes en línea para mejorar
la capacidad del modelo para procesar entradas de streaming en escenarios realistas.
Al alimentar continuamente secuencias de fotogramas durante el entrenamiento, el modelo
aprende a retener una percepción detallada del momento actual mientras
Mantener la memoria a largo plazo de eventos pasados, incluso bajo alta compresión.
restricciones. La configuración completa y los ajustes de parámetros para el
fase de ajuste en línea se enumeran en la Tabla 11. Estas etapas son
crítico para la transición del modelo de la comprensión fuera de línea a la realidad.
razonamiento del tiempo, mejorando significativamente su robustez y practicidad.
efectividad en aplicaciones del mundo real.

## E Actuaciones completas

En las siguientes partes, presentamos los resultados completos y comparamos
StreamForest con modelos líderes propietarios y de código abierto. a
evaluamos exhaustivamente la efectividad de StreamForest, llevamos a cabo
Experimento sobre tres puntos de referencia de comprensión de videos en línea:
StreamingBench, OVBench y OVO-Bench.

## E.1 Banco de transmisión

La Tabla 12 presenta los resultados completos de la evaluación en StreamingBench.
que cubre 12 tareas de comprensión de video en tiempo real. StreamForest logra
la puntuación media más alta (77,26%) entre todos los modelos evaluados, tanto
de código abierto y propietario, mientras opera eficientemente a 1 fps.
En particular, StreamForest supera a los principales MLLM propietarios, como
GPT-4o (73,28%) y Gemini 1.5 Pro (75,69%). También es significativamente
supera los principales modelos fuera de línea de código abierto como LLaV A-OneVision
(71,12%) y Qwen2.5-VL (73,68%), lo que subraya su robusta multimodal
capacidades de representación y razonamiento. En el vídeo online MLLM
categoría, StreamForest establece una nueva tecnología abierta de última generación y con un rendimiento superior.
sus homólogos de origen Dispider (67,63%) por un amplio margen. es consistente
La precisión y la eficiencia en tiempo real demuestran un gran potencial para
Implementación práctica en aplicaciones de streaming.

Tabla 12: Resultados completos de la evaluación de tareas de comprensión en tiempo real en StreamingBench.| Método | Tamaño | #Marcos | OP | RC | informática | ATP | UE | TR | relaciones públicas | SU | ACP | TC | TODOS ||---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|
| Humano | - | - | 89,47 | 92,00 | 93,60 | 91,47 | 95,65 | 92,52 | 88,00 | 88,75 | 89,74 | 91,30 | 91,46 || MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios || Géminis 1.5 pro [54] | - | 1fps | 79,02 | 80,47 | 83,54 | 79,67 | 80,00 | 84,74 | 77,78 | 64,23 | 71,95 | 48,70 | 75,69 || GPT-4o [44] | - | 64 | 77.11 | 80,47 | 83,91 | 76,47 | 70,19 | 83,80 | 66,67 | 62,19 | 69,12 | 49,22 | 73,28 || Soneto de Claude 3.5 [1] | - | 20 | 73,33 | 80,47 | 84,09 | 82.02 | 75,39 | 79,53 | 61.11 | 61,79 | 69,32 | 43.09 | 72,44 || MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto || Vídeo-LLaMA2 [10] | 7B | 32 | 55,86 | 55,47 | 57,41 | 58,17 | 52,80 | 43,61 | 39,81 | 42,68 | 45,61 | 35,23 | 49,52 || RESTO-1.5 [38] | 8B | 14 | 53,68 | 49,22 | 70,98 | 56,86 | 53,42 | 53,89 | 54,63 | 48,78 | 50,14 | 17,62 | 52,32 || Vídeo-CCAM [15] | 14B | 96 | 56,40 | 57,81 | 65,30 | 62,75 | 64,60 | 51,40 | 42,59 | 47,97 | 49,58 | 31,61 | 53,96 || LargoVA [75] | 7B | 128 | 70,03 | 63,28 | 61,20 | 70,92 | 62,73 | 59,50 | 61.11 | 53,66 | 54,67 | 34,72 | 59,96 || PasanteVL2 [9] | 8B | 16 | 68,12 | 60,94 | 69,40 | 77,12 | 67,70 | 62,93 | 59,26 | 53,25 | 54,96 | 56,48 | 63,72 || Canguro [40] | 7B | 64 | 71.12 | 84,38 | 70,66 | 73,20 | 67,08 | 61,68 | 56,48 | 55,69 | 62.04 | 38,86 | 64,60 || Vídeo LLaVA-NeXT [76] | 32B | 64 | 78,20 | 70,31 | 73,82 | 76,80 | 63,35 | 69,78 | 57,41 | 56,10 | 64,31 | 38,86 | 66,96 || MiniCPM-V2.6 [65] | 8B | 32 | 71,93 | 71,09 | 77,92 | 75,82 | 64,60 | 65,73 | 70,37 | 56,10 | 62,32 | 53,37 | 67,44 || LLaVA-OneVision [28] | 7B | 32 | 80,38 | 74,22 | 76,03 | 80,72 | 72,67 | 71,65 | 67,59 | 65,45 | 65,72 | 45.08 | 71.12 || Qwen2.5-VL [2] | 7B | 1fps | 78,32 | 80,47 | 78,86 | 80,45 | 76,73 | 78,50 | 79,63 | 63,41 | 66,19 | 53,19 | 73,68 || MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto || Flash VStream [72] | 7B | - | 25,89 | 43,57 | 24,91 | 23,87 | 27.33 | 13.08 | 18,52 | 25.20 | 23,87 | 48,70 | 23.23 || VideoLLM-en línea [5] | 8B | 2 fps | 39.07 | 40.06 | 34,49 | 31.05 | 45,96 | 32,40 | 31,48 | 34.16 | 42,49 | 27,89 | 35,99 || Dispider [47] | 7B | 1fps | 74,92 | 75,53 | 74,10 | 73,08 | 74,44 | 59,92 | 76,14 | 62,91 | 62,16 | 45,80 | 67,63 || StreamForest(Nuestro) | 7B | 1fps | 83.11 | 82,81 | 82,65 | 84,26 | 77,50 | 78,19 | 76,85 | 69.11 | 75,64 | 54,40 | 77,26 |## E.2 OVBanco

Tabla 13: Resultados completos de la evaluación en OVBench.| Nombre de la tarea | Tamaño | PF | PF | PF | THV | THV | THV | PM | PM | PM | SP | SP | SP | STP TP | STP TP | STP TP | AVG ||---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|
| Nombre del subconjunto | Tamaño | AA | SGP | diputado | AP | SV | OP | RA | relaciones públicas | TR | Alabama | OP | EN | OT COMO | SL | OES | AVG || MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios || Géminis-1.5-Flash [54] | - | 71,4 | 53,6 | 21.9 | 56,5 | 60,8 | 40,6 | 36,7 | 47,9 | 62,5 | 32,3 | 37,5 | 87,0 50,0 | 83,3 | 22.3 | 46,9 | 50,7 || MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto || PasanteVL2 [9] | 7B | 52,6 | 60,2 | 27,6 | 57,5 | 52,0 | 58,5 | 38,8 | 67,1 | 58,3 | 38.1 | 31,3 87,4 | 37,0 | 75,4 | 31,4 | 5.9 | 48,7 || PasanteVL2 [9] | 4B | 57,7 | 57,0 | 14.4 | 59,2 | 49,4 | 60,0 | 30.3 | 61,8 | 46,3 | 30,9 | 20,1 83,0 | 32,3 | 70,7 | 29.4 | 3.4 | 44.1 || LLaMA-VID [35] | 7B | 43,6 | 50,9 | 19.6 | 64,0 | 47,5 | 46,8 | 29.4 | 48,9 | 51,2 | 31,9 | 11,2 75,7 | 24,8 | 59.1 | 26.0 | 40,0 | 41,9 || LLaVA-Onevisión [28] | 7B | 68,0 | 62,7 | 35,9 | 58,4 | 50,3 | 46,5 | 29.4 | 60,7 | 58,0 | 43.1 | 14,2 86,5 | 49,7 | 70,7 | 28.1 | 30.2 | 49,5 || LargoVA [75] | 7B | 64.1 | 56,5 | 29,5 | 54,9 | 51,9 | 34,8 | 35,3 | 55,6 | 57,7 | 31,6 | 67,4 | 44,7 | 80,0 | 26,7 | 4.0 | 43,6 || MiniCPM-V2.6 [65] | 7B | 33.3 | 35,9 | 15.0 | 59,2 | 50,8 | 55.1 | 25.0 | 37,4 | 41,7 | 26,6 | 3,4 11,8 | 98,3 36,3 | 66,1 | 26.4 | 6.2 | 39.1 || Qwen2-VL [55] | 7B | 60,3 | 66,1 | 22.1 | 54,9 | 51,5 | 51.1 | 37,8 | 64,4 | 69,3 | 35,3 | 97,0 |                                 | 65,1 | 30,8 | 11.7 | 49,7 || LITA [20] | 7B | 19.2 | 24,5 | 19,9 | 40,8 | 48,9 | 24,9 | 3.1 | 27.3 | 6.4 | 6.9 | 28,5 14,6 | 49,4 35,2 23,9 | 27,4 | 0,5 | 3.4 | 20.4 || TimeChat [48] | 7B | 7.7 | 15.3 | 18.7 | 20.6 | 15.7 | 11.7 | 9.1 | 14.7 | 9.8 | 7.5 | 19,5 | 13,9 10,3 | 9.3 | 10.1 | 10.8 | 12.8 || VTimeLLM [19] | 7B | 37,2 | 23.4 | 15.0 | 64,8 | 43,8 | 53,2 | 25,9 | 38,8 | 32,5 | 25,9 | 20,4 40,9 | 6.8 | 48,4 | 43,5 | 8.6 | 33.1 || MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto || VídeoLLM-Online [5] | 7B | 0 | 1.8 | 20,9 | 5.2 | 5.9 | 32,6 | 0 | 2.3 | 26,7 | 0,6 | 26,6 | 0,9 | 19,9 0,9 | 1.7 | 8.3 | 9.6 || PelículaChat [52] | 7B | 23.1 | 27,5 | 23,6 | 58,4 | 43,9 | 40,3 | 25,6 | 31.1 | 23,9 | 26,9 | 39,6 | 24,4 28,9 | 29.3 | 25,5 | 21.9 | 30,9 || Flash-Vstream [72] | 7B | 26,9 | 37,6 | 23,9 | 60.1 | 41,9 | 40,0 | 23.4 | 35,3 | 26.1 | 24,7 | 28,8 27,0 |                                 | 29,8 | 25,6 | 26,8 | 31.2 || Videochat-Online [23] | 4B | 64.1 | 59,7 | 16.6 | 63.1 | 58,3 | 62,8 | 42,2 | 54,4 | 70,6 | 54.1 | 24,8 88,7 | 21,4 48,5 | 73,0 | 25,9 | 71,7 | 54,9 || StreamForest (Nuestro) | 7B | 69,2 | 60,0 | 34,4 | 69,1 | 54,0 | 72,9 | 50,9 | 64,9 | 82,2 | 56,6 | 87,9 95,2 | 61,2 | 64,2 | 30,6 | 92,6 | 60,5 |La Tabla 13 muestra los resultados completos de OVBench, que abarcan seis
diversas categorías de tareas (FP, THV, PM, SP, STP, TP). corrientebosque
logra la puntuación media más alta del 60,5%, superando a todos los de código abierto
MLLM de vídeo en línea y fuera de línea. Supera significativamente a otros abiertos.
modelos de origen en línea, por ejemplo, Videochat-Online (54,9%) y Flash-VStream
(31,2%), así como modelos offline como Qwen2-VL (49,7%) y LLaVA-
OneVision (49,5%). En comparación con Gemini-1.5-Flash (50,7%), StreamForest
ofrece casi 10 puntos más de precisión en promedio, afirmando su
capacidad de equilibrar la eficiencia en tiempo real con un alto rendimiento.

## E.3 OVO-Banco

La Tabla 14 detalla el rendimiento en OVO-Bench, donde nuevamente StreamForest
lidera entre los MLLM de video en línea de código abierto con un promedio general de
55,57%, superando a Dispidier-7B (41,78%) y Flash-VStream-7B
(33,15%). Destaca en áreas críticas como la visualización en tiempo real.
percepción (61,20%), rastreo hacia atrás (52,02%) y avance activo
respondiendo (53,49%), mostrando un razonamiento temporal sólido en ambos
acontecimientos pasados y futuros. Estos resultados posicionan a StreamForest como un
Solución práctica y potente para videolenguaje en tiempo real.
comprensión.

Tabla 14: Resultados detallados de la evaluación en OVO-Bench.

<!-- imagen -->|                                                                                              | # Marcos | Percepción visual en tiempo real | Percepción visual en tiempo real | Percepción visual en tiempo real | Percepción visual en tiempo real | Percepción visual en tiempo real | Percepción visual en tiempo real | Percepción visual en tiempo real | Seguimiento hacia atrás | Seguimiento hacia atrás | Seguimiento hacia atrás | Seguimiento hacia atrás | Adelante | Adelante | Adelante | Adelante | Promedio general                    ||----------------------------------------------------------------------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|---------------------------------|
| Modelo |                                 | OCR | ACR | ATR | UTE | FPD | DOJ | Promedio                            | EPM | ASÍ | DAN | Promedio                            | REC | RSS | RRC | Promedio                            | Promedio general                    || Humano | - | 93,96 | 92,57 | 94,83 | 92,70 | 91,09 | 94,02 | 93,20 | 92,59 | 93,02 | 91,37 | 92,33 | 95,48 | 89,67 | 93,56 | 92,90 | 92,81 || MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios | MLLM propietarios || Géminis 1.5 Pro [54] GPT-4o [44] | 1fps 64 | 85,91 69,80 | 66,97 64,22 | 79,31 71,55 | 58,43 51,12 | 63,37 70,30 | 61,96 59,78 | 69,32 64,46 | 58,59 57,91 | 76,35 75,68 | 52,64 48,66 | 62,54 60,75 | 35,53 27,58 | 74,24 73,21 | 61,67 59,40 | 57,15 53,40 | 63,00 59,54 || MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto | MLLM de vídeo sin conexión de código abierto || LLaVA-Video-7B [76] LLaVA-OneVision-7B [28] Qwen2-VL-7B [55] InternVL2-8B [9] LongVU-7B [50] | 64 64 64 64 1fps | 69,80 67,11 69,13 68,46 55,70 | 59,63 58,72 53,21 58,72 49,54 | 66,38 69,83 63,79 68,97 59,48 | 50,56 49,44 50,56 44,94 48,31 | 72,28 71,29 66,34 67,33 68,32 | 61,41 60,33 60,87 55,98 63,04 | 63,34 62,79 60,65 60,73 57,40 | 51,18 52,53 44,44 43,10 43,10 | 64,19 58,78 66,89 61,49 66,22 | 9,68 23,66 34,41 27,41 9,14 | 41,68 44,99 48,58 44,00 39,49 | 34,10 24,79 30,09 25,79 16,62 | 67,57 66,93 65,66 57,55 69,00 | 60,83 60,83 50,83 52,92 60,00 | 54,17 50,85 48,86 45,42 48,54 | 53,06 52,88 52,70 50,05 48,48 || MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto | MLLM de vídeo en línea de código abierto || VideoLLM-online-8B [5] Flash-VStream-7B [72] Dispider-7B [47] | 2 fps 1 fps 1 fps | 8,05 25,50 57,72 | 23,85 32,11 49,54 | 12,07 29,31 62,07 | 14,04 33,71 44,94 | 45,54 29,70 61,39 | 21,20 28,80 51,63 | 20,79 29,86 54,55 | 22,22 36,36 48,48 | 18,80 33,78 55,41 | 12,18 5,91 4,30 | 17,73 25,35 36,06 | - 5,44 18,05 | - 67,25 37,36 | - 60,00 48,75 | - 44,23 34,72 | 12,84 33,15 41,78 || StreamForest-7B (Nuestro) | 1fps | 68,46 | 53.21 | 71,55 | 47,75 | 65,35 | 60,87 | 61,20 | 58,92 | 64,86 | 32,26 | 52.02 | 32,81 | 70,59 | 57.08 | 53,49 | 55,57 |## F Comparación cualitativa

Figura 7: Comparación cualitativa entre StreamForest y otro método.

<!-- imagen -->

La Figura 7 presenta una comparación cualitativa entre nuestro modelo y otros.
método. En el ejemplo principal, StreamForest demuestra una capacidad superior
para capturar detalles visuales detallados y mantener la memoria persistente
con el tiempo, permitiendo inferencias más coherentes e informadas. el medio
El ejemplo destaca la sólida base espaciotemporal de StreamForest.
capacidades, localizando con precisión objetos y eventos en el espacio y
tiempo. El ejemplo inferior ilustra el potencial del modelo en
Escenarios de conducción inteligentes, donde ofrece información precisa en tiempo real.
percepción y apoya predicciones futuras basadas en datos históricos y
observaciones actuales.

## G Más ablaciones

Figura 8: Experimentos de ablación de tres pesos de penalización.

<!-- imagen -->

Evaluar el efecto de cada sanción sobre la efectividad de las medidas a largo plazo.
memoria, realizamos estudios de ablación variando los pesos de los
penalización por similitud, penalización por conteo de fusión y penalización por distancia temporal.
Los resultados se presentan en la Figura 8, que informa la precisión de
VideoMME y OVBench bajo diferentes configuraciones. Demuestra que
una combinación equilibrada de pesos de penalización es más eficaz.
Específicamente, 0,4 para penalizaciones por recuento de similitud y fusión, y 0,2
para la penalización de distancia temporal produce la memoria más efectiva
construcción. Esta configuración logra un equilibrio favorable entre
Preservar la coherencia semántica, mantener la diversidad en la memoria.
representación y garantizar una continuidad temporal razonable.

## H Eficiencia de la inferencia de rondas múltiples

Evaluamos la eficiencia de respuesta de múltiples rondas de StreamForest mediante transmisión
un vídeo de 600 segundos al modelo a una velocidad constante de 1 FPS. aislar
procesamiento del rendimiento a partir de la latencia de generación de texto, el modelo fue
restringido para producir una respuesta de un solo token para cada cuadro. bajo
En esta configuración rigurosa, StreamForest logró una velocidad de procesamiento promedio
de 9,9 FPS, que compite con VideoLLMOnline (12,3 FPS), un modelo
Reconocido por sus capacidades en tiempo real. Fundamentalmente, StreamForest
ofrece esta alta eficiencia sin comprometer su sustancial
superioridad en la precisión del razonamiento sobre VideoLLM-Online. en crudo
Por el contrario, Qwen2-VL, otro modelo que también prioriza el razonamiento
precisión, demostró graves cuellos de botella en el rendimiento. Su procesamiento
La velocidad cayó por debajo de 1 FPS en un video de solo dos minutos de duración, y
encontré errores de falta de memoria (OOM) en una única GPU A100-80G después
procesando sólo 79 fotogramas.

Tabla 15: Comparación de la velocidad de inferencia de múltiples rondas.| Método |   Resolución | FPS ||-------------------|--------------|-------|
| Qwen2.5-VL |          384 | OOM || VídeoLLM-Online |          384 | 12.3 || StreamForest (1k) |          384 | 9.9 |## I Discusiones

## I.1 Limitaciones

A pesar de la efectividad de nuestro método propuesto, varias limitaciones
todavía hay motivos que justifican una mayor investigación. Nuestro enfoque sólo puede basarse en
calcular la similitud entre cuadros para determinar los momentos en los que el modelo
deberían producir resultados de manera proactiva. Específicamente, el método identifica
mínimos locales en puntuaciones de similitud para detectar transiciones. Sin embargo, esto
La técnica captura principalmente cambios burdos de escena y a menudo no logra
detectar con precisión verdaderos límites de eventos semánticos. Para abordar esto
limitación, una posible solución es incorporar un MLLM ligero
como módulo recordatorio auxiliar. Este módulo podría proporcionar información semántica.
Guía de nivel para respaldar resultados más precisos y conscientes del contexto.
decisiones. Estas limitaciones sugieren direcciones prometedoras para el futuro.
trabajo.

## I.2 Impactos más amplios

Nuestro método propuesto muestra un gran potencial para la transmisión en el mundo real
Comprensión de vídeo, especialmente en aplicaciones críticas como las autónomas.
conduciendo. Con ajustes específicos de dominio, se puede adaptar a varios
tareas posteriores que requieren un procesamiento visual continuo. Como se muestra en
El texto principal, el modelo se desempeña bien en escenarios de conducción autónoma,
donde la percepción precisa y oportuna es crucial para la seguridad y la toma de decisiones.
haciendo. Puede procesar eficientemente transmisiones de video en vivo mientras preserva
percepción detallada y memoria contextual a largo plazo. Esta capacidad
es particularmente valioso bajo recursos computacionales limitados, ayudando
mejorar la confiabilidad y capacidad de respuesta de los sistemas inteligentes en
entornos dinámicos.

Sin embargo, como ocurre con muchos modelos de visión y lenguaje, las posibles consecuencias sociales negativas
También se deben considerar los impactos. Si se despliega sin las salvaguardias adecuadas,
Los modelos pueden heredar o amplificar los sesgos presentes en los datos de entrenamiento, lo que lleva a
a un comportamiento poco fiable. Por ejemplo, las disparidades de desempeño entre
diferentes entornos o condiciones (por ejemplo, clima, iluminación o
ubicación geográfica) podría afectar la solidez de StreamForest. a
mitigar tales riesgos, debemos explorar técnicas para mejorar
Interpretabilidad y controlabilidad de modelos de streaming de vídeo en condiciones de seguridad.
contextos.