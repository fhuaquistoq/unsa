## Selección de tokens basada en la atención recurrente para LLM de transmisión de video eficiente

Vaggelis Dorovatas 1 , 2

Soroush Seifi 1

Gunshi Gupta 3

Rahaf Aljundi 1

1

Toyota Motor Europa 2 Archimedes RU, Athena RC 3 Universidad de Oxford

## Resumen

Los modelos de lenguaje grande de video (Video-LLM) se destacan en la comprensión de videos
incontext, siempre que tengan acceso completo al vídeo al responder
consultas. Sin embargo, estos modelos enfrentan desafíos en escenarios de streaming.
donde los videos de una hora deben procesarse en línea y las preguntas deben
respuestas oportunas. En este trabajo proponemos un enfoque sin formación.
compatible con Video-LLM estándar, aprovechando tres conceptos clave: 1)
Selección de tokens visuales informada por LLM para identificar aquellos que el LLM
ha atendido y contribuido a la comprensión de cada clip corto.
Nuestra selección basada en la atención nos permite descartar hasta aproximadamente el 95% de
tokens visuales sin importancia con una pérdida mínima de rendimiento; 2) recurrente
procesamiento de tokens seleccionados en el pasado para generar temporalmente coherente
comprensión de cada clip procesado; 3) Pregunta basada en subtítulos
respondiendo para obtener respuestas ligeras y precisas. Nuestro método logra
rendimiento de última generación en pruebas comparativas de transmisión de video, logrando un
equilibrio entre eficiencia y eficacia.

## 1 Introducción

La respuesta y comprensión eficientes y efectivas de preguntas en video son
crucial para implementar modelos de lenguaje grande (LLM) como inteligentes
asistentes en dominios que requieren comprensión continua de información visual.
Esta capacidad es esencial para aplicaciones como las autónomas.
conducción, vigilancia, atención médica y entretenimiento, donde la dinámica
La información visual debe entenderse en tiempo real. Vídeo actual
comprender la investigación con VLM [31, 17, 2] generalmente procesa datos completos
Vídeos en contexto, que presentan fotogramas densamente muestreados junto con el texto.
consultas para generar respuestas, con enfoques de capacitación comunes centrados
en tareas de generación de texto como subtítulos de vídeos o respuesta a preguntas.
Si bien es eficaz para clips cortos, este enfoque de fuerza bruta enfrenta
limitaciones críticas con vídeos más largos: los costos computacionales aumentan a medida que
Los tokens visuales se multiplican y exceder los límites de longitud del contexto fuerza
muestreo escaso que corre el riesgo de perder información. Estos desafíos se convierten
particularmente pronunciado en escenarios de transmisión donde la visualización continua
La entrada hace que el procesamiento de vídeo completo sea insostenible. Esto requiere
técnicas de compresión eficientes que procesan selectivamente fotogramas,
Condensar información visual y mantener una memoria fácilmente recuperable.
estructuras para una respuesta eficaz a las consultas.

Los esfuerzos recientes se han centrado en comprimir la información de breves
videoclips, alejándose del enfoque de fuerza bruta de procesar los
vídeo completo en una sola pasada y, por tanto, ampliando las capacidades del modelo a
Manejar la comprensión de videos largos. Estos enfoques incluyen métodos que
comprimir solo la información visual (antes del LLM) codificada por
un codificador de visión [38, 12, 10], o almacenar sólo descripciones textuales de
clips cortos [1] y recuperar solo aquellos relevantes para la consulta de entrada.
Otra línea de trabajo involucra al LLM en procesamiento de video generando
representaciones comprimidas en forma de tokens de resumen, como
VideoStreaming [24], o almacenamiento de caché KV durante la inferencia, como ReKV
[9], combinando efectivamente las dos modalidades. Mientras todos estos
enfoques marcan avances importantes, enfrentan desafíos amplificados en
configuraciones de transmisión, donde los fotogramas pasados ​​no se pueden volver a visitar y las entradas se puedenhoras de duración. Los enfoques basados en la formación [24, 38] enfrentan problemas de extrapolación
con vídeos arbitrariamente largos, y ampliar su formación puede ser
computacionalmente prohibitivo o sufre de escasez de datos y anotaciones
Problemas para videos largos. Enfoques sin formación como ReKV, mientras
evitando el reentrenamiento, confíe en almacenar grandes cachés KV, lo que se convierte en
consume mucha memoria e introduce una posible redundancia en la transmisión,
en última instancia, afectando el rendimiento general. Por el contrario, los subtítulos en forma de clip
sólo los métodos (por ejemplo, Goldfish [1]) ofrecen eficiencia pero carecen de continuidad,
lo que dificulta el seguimiento de entidades y el razonamiento entre clips, por lo que
obstaculizando la comprensión holística.

*Los dos primeros autores brindan servicios contratados para Toyota.

*Correspondencia: vdorovatas@hotmail.gr

Figura 1: Canalización de rLiVS: (Izquierda): los videos largos se dividen en clips visuales cortos que se procesan en forma de transmisión. A un clip corto se le anteponen tokens visuales seleccionados de clips anteriores en la transmisión para generar una descripción textual. El historial se acumula seleccionando un subconjunto de tokens del clip corto en función de las puntuaciones de atención. (Derecha): dada una consulta sobre el video anterior, las descripciones textuales acumuladas a lo largo de un video largo se comparan con la consulta incrustada y el subconjunto más similar se ingresa al LLM (dentro del videoLLM) para responder preguntas.

<!-- imagen -->

En este trabajo, proponemos un LLM eficiente, sin capacitación y en video.
Solución independiente para la transmisión de video y respuesta a preguntas. Nuestro enfoque
procesa videos largos (sin conexión o en línea) dividiéndolos en cortos
clips y se centra en tres aspectos clave, integrando la percepción visual
con comprensión del lenguaje para una comprensión integral del video: 1)
Comprimir información de vídeo para evitar redundancias e ineficiencias en
uso de la memoria seleccionando un pequeño conjunto de tokens visuales clave por corto
clip, 2) Acceder a tokens visuales seleccionados pasados al procesar nuevos cortos
clips, lo que permite compartir información visual entre clips cortos y
mejorar la comprensión general del video, y 3) realizar videos basados en texto.
recuperación y respuesta, aprovechando la fortaleza probada de los LLM en el razonamiento
en contextos extendidos (como subtítulos de múltiples clips).

Específicamente, durante el procesamiento de cada clip corto (es decir, generar
un pie de foto), nos inspiramos en la neurociencia cognitiva,
particularmente la interacción entre la atención y la memoria [6]. dado
capacidad limitada de la memoria [23, 7], la atención es clave para la selectividad
codificación [6]. En consecuencia, en lugar de muestrear fichas visuales aleatoriamente,
conservamos aquellos que recibieron la mayor atención del LLM,
tratándolos como un rastro de memoria comprimido del clip. Además,
basándose en la idea de que las experiencias pasadas dan forma a la atención actual [6],
Reutilizamos tokens previamente seleccionados al procesar clips posteriores.
Estos tokens pasados se transmiten con nuevas entradas, lo que permite que la memoria
guiar la atención y apoyar la comprensión consciente del contexto. Inspirado por el
procesamiento visual recurrente del cerebro [15], implementamos un simple FIFO
memoria: los tokens recién seleccionados se anteponen y los más antiguos se
descartado una vez que se alcanza el límite de contexto.

Finalmente, al responder una pregunta sobre eventos pasados en una transmisión de video,
recuperamos los K títulos principales según la similitud de la pregunta
a los subtítulos previamente almacenados de clips cortos. Este enfoque es
motivado por la eficiencia de almacenar subtítulos solo y empíricamente
evidencia que sugiere que el razonamiento en video largo es más efectivo contexto representativo. Este enfoque nos permite responder preguntas basadas
en los subtítulos almacenados, en lugar de reprocesar el visual anterior
entrada, que es una práctica común en los trabajos actuales [38, 10]. Figura 1
ilustra nuestro método.

Nuestro enfoque se puede aplicar a cualquier video-LLM previamente entrenado en clips cortos,
permitiendo la comprensión de videos largos sin capacitación adicional y
manteniendo una huella de memoria baja. Evaluamos nuestro método en tres
Puntos de referencia fuera de línea y de transmisión, logrando un rendimiento de última generación.
con requisitos de memoria significativamente menores. La sencillez y la fuerza.
El rendimiento de nuestro diseño estableció una base sólida para videos largos en línea.
respuesta a preguntas.

Nuestras aportaciones son las siguientes:

- Proponemos la selección visual recurrente basada en LLM (rLiVS), un enfoque simple y sin capacitación para comprender videos largos y responder preguntas.
- Nuestro enfoque es independiente de la arquitectura Video-LLM y no requiere ningún módulo externo.
- Logramos un rendimiento de última generación con requisitos de memoria significativamente menores.

A continuación, analizamos el trabajo estrechamente relacionado en la Sección 2 y
Presentamos nuestro método en la Sección 3. Evaluamos nuestro método y realizamos la ablación del
opciones de diseño en la Sección 4 y concluir en la Sección 5.

## 2 Trabajo relacionado

Modelos de videolenguaje: los modelos recientes de videolenguaje han logrado fuertes
rendimiento en tareas de vídeo de formato corto alineando las características visuales con
texto utilizando transformadores de visión-lenguaje previamente entrenados [30, 16, 19]. Estos
Los modelos suelen muestrear una pequeña cantidad de fotogramas de cada vídeo y
codificarlos en tokens visuales usando una columna vertebral de visión (por ejemplo, ViT),
que luego se introducen en un modelo de lenguaje, ya sea directamente o mediante
fusión modal. Sin embargo, el número de tokens visuales crece linealmente con
el número de fotogramas y parches espaciales (por ejemplo, muestrear 32 fotogramas con un
La columna vertebral basada en ViT (tamaño de parche de 16 × 16) produce más de 1000 tokens, lo que lo convierte en
Procesar vídeos largos de un extremo a otro es prohibitivamente caro [34, 29]. como un
Como resultado, estos modelos se limitan a clips cortos (normalmente menos de 30
segundos), carecen de memoria persistente y no pueden capturar dependencias entre
horizontes temporales más largos sin recursos computacionales y de memoria sustanciales
arriba.

Comprensión de videos largos sin conexión: para superar las limitaciones de los videos cortos
modelos de clips, se han propuesto varios enfoques fuera de línea durante mucho tiempo.
Comprensión del vídeo mediante la introducción de mecanismos para comprimir o resumir.
información a través del tiempo. Algunos métodos se centran en comprimir sólo el
modalidad visual, ya sea mediante selección de fotogramas clave, agrupación de características o
almacenar memoria visual tokenizada [32, 34, 28, 10]. Otros dependen únicamente de
abstracción textual, conversión de clips cortos en lenguaje natural
resúmenes que luego se recuperan y redactan para responder consultas [1].
Más recientemente, métodos híbridos como VideoStreaming [24] entrenan
modelos para generar tokens de resumen entrenables que condensan el
contenido de cada clip en una representación compacta, que se almacena y
recuperado durante la inferencia. Si bien son efectivos, estos métodos a menudo requieren
importante capacitación fuera de línea para alinear el resumen con el downstream
tareas, están vinculados a canales de capacitación previa específicos y, por lo general, procesan
cada clip de forma independiente, lo que limita su capacidad para crear contenidos jerárquicos o
Memoria persistente en vídeos largos.

Comprensión de vídeo en streaming y con memoria aumentada: a diferencia de
enfoques fuera de línea, la comprensión de la transmisión de video tiene como objetivo procesar
fotogramas de vídeo entrantes de forma incremental, lo que permite una reproducción en tiempo real o de baja latencia.inferencia en horizontes temporales prolongados. Métodos de recuperación aumentada
mantener almacenes de memoria externa de incorporaciones pasadas y recuperar datos relevantes
contexto sobre la marcha basado en la entrada actual, evitando la secuencia completa
reprocesamiento [38]. De manera similar, MeMViT [35] incorpora memoria visual.
módulos o atención multimodal sobre tokens almacenados en caché para mejorar el tiempo
coherencia, aunque a menudo se basan en ventanas de contexto de longitud fija o
Mecanismos de atención blanda, que escalan mal o sufren de memoria.
deriva en el tiempo. En la misma dirección de la memoria visual, Video-XL [27]
presenta una arquitectura Video-LLM personalizada que aprende a
comprimir y retener información visual a lo largo del tiempo mediante métodos supervisados.
entrenamiento. Sin embargo, su desempeño está estrechamente ligado a las necesidades específicas.
Arquitectura del modelo y distribución de la duración y compresión del vídeo.
estrategias vistas durante el ajuste fino. Recientemente, ReKV [9] exploró
transmisión de video para responder preguntas almacenando el valor-clave completo del decodificador
cachés y recuperar contexto relevante ya sea a través de interna
mecanismos basados en la atención o similitud externa basada en CLIP,
demostrando que la recuperación a través de incrustaciones almacenadas en caché puede ser a la vez
eficaz y eficiente. Sin embargo, muchos de estos métodos tratan
memoria como un búfer plano o requieren almacenar grandes volúmenes de datos sin procesar
activaciones, lo que limita la escalabilidad y la abstracción a largo plazo. Nuestro trabajo
Se basa en estos conocimientos introduciendo un mecanismo de consolidación de la memoria.
que retiene persistentemente y abstrae jerárquicamente multimodal
contexto a lo largo de una secuencia, lo que permite una evaluación a largo plazo escalable y adaptable a las tareas.
comprensión.

## 3 método

Nuestro objetivo es desarrollar un método eficiente para escenarios de streaming. Nuestro
enfoque utiliza un videoLLM de clip corto como columna vertebral, que es capaz de
Procesamiento multimodal de videoclips cortos y generación de texto.
respuestas. Específicamente, para la tarea de subtitulado general, el modelo,
dada una secuencia de tokens visuales (que representan los fotogramas de entrada, V)
extraído de un codificador visual (por ejemplo, CLIP [25]) y un potencial
instrucción ("Describe lo que está sucediendo en el video"), genera una
descripción textual C, que representa la comprensión del modelo del
clip corto. En lugar de almacenar toda la información (como en [9]) nuestro objetivo
comprimir la información visual aprovechando la relación con el
fichas textuales de cada clip corto, produciendo una imagen comprimida guiada visualmente.
representación por texto, S . Además, apuntamos a un diseño independiente del modelo.
diferente de [11] y [24]). El aspecto clave aquí es que tenemos la
entrada al LLM, V, y su título correspondiente, C, que puede ser
interpretado como la comprensión del modelo expresada en el texto.

## 3.1 Selección de tokens visuales basada en la atención

Varios trabajos han explorado la selección de tokens basada en pesos de atención.
[37, 13, 26]. Estos métodos normalmente implican dejar caer tokens desde el principio.
capas para mejorar la eficiencia del procesamiento de contextos largos en posteriores
capas. La evidencia de estos estudios sugiere que la atención de un modelo
puede servir como un fuerte indicador de la relevancia de los tokens visuales.

Nuestro objetivo es seleccionar globalmente tokens de un clip corto después de que haya sido
sido procesado por VideoLLM, proporcionando contexto para clips posteriores.
Estos tokens guiarán la atención del LLM en el siguiente breve clip para
centrarse en contenidos que sean coherentes con acontecimientos pasados, reduciendo así
atención a las señales de fondo, el ruido y las señales visuales redundantes.

Procesamiento de clips cortos. Empleamos un video-LLM previamente capacitado, que consistede un codificador visual VE, un LLM y un módulo proyector P. Dada una T
-videoclip de fotograma V y una instrucción X I, genera un título textual
del vídeo, C:

<!-- fórmula-no-decodificada -->

donde X V ∈ R TN V × D y N V denota el número de tokens espaciales por
marco, y D es la dimensión de los tokens LLM. El objetivo es seleccionar
un conjunto disperso de tokens S ∈ R T × N S × D donde N S ≪ N V .

Cálculo de coeficientes de atención. Denotaremos X C la incrustación de
el título generado. Por el último token generado, atención
El tensor de coeficiente se estima entre los tokens de texto generados y
los tokens de entrada. Definimos la matriz de atención A l,h ∈ R ( TN V + N I
+ N C ) × ( TN V + N I + N C ) calculado sobre X l = [ X l V , X l I , X l
C ] en una capa dada ly una cabeza de atención dada h donde N I es el
número de tokens de texto en el mensaje de instrucción y N C es el número de
tokens en el número de tokens de subtítulos generados.

Después de transferir X l a la clave K l,h, el valor V l,h y la consulta Q l,h,
las puntuaciones de atención A l,h se calculan utilizando el producto escalar
atención:

<!-- fórmula-no-decodificada -->

donde W l,h Q , W l,h K , W l,h V son las matrices de peso que se pueden aprender para
consultas, claves y valores, respectivamente 1 .

<!-- fórmula-no-decodificada -->

donde d k es la dimensión de las cabezas de atención.

Primero extraemos los coeficientes de atención que representan la cruz.
atención entre el título generado y los tokens visuales de entrada:

<!-- fórmula-no-decodificada -->

1 Nuestro método se centra en pesos de atención, haciendo que las operaciones que involucran
los valores Vl,h son innecesarios y por tanto descartados.

Luego calculamos una puntuación de atención para cada token visual de entrada X V j,
: promediando las puntuaciones de atención de todos los tokens de subtítulos generados N
C. Finalmente, la puntuación de importancia global para un token visual determinado X V j,
: se determina promediando las puntuaciones de atención en diferentes
cabezas de atención H y capas L, que representan la atención general de
el modelo.

<!-- fórmula-no-decodificada -->

En la práctica, para limitar el costo de cómputo, consideramos solo un subconjunto de
capas. Tenga en cuenta que en lugar de promediar considerar otras operaciones como
ya que la agrupación máxima es sencilla.

<!-- fórmula-no-decodificada -->

donde π = argsort ( a ) son los índices que ordenan a en orden descendente
2 .

## 3.2 Procesamiento recurrente de vídeos largos.

Para el primer clip corto X (0) V hemos seleccionado un conjunto de N S
tokens S (0) que reciben la mayor atención del título generado.
Ahora, para el siguiente clip corto X (1) V, proporcionamos como entrada al LLM el
tokens previamente seleccionados S (0) como entrada junto con el conjunto completo de
tokens que comprenden el videoclip actual X (1) V. De manera similar, después
Al generar un título C (1), seleccionamos un conjunto disperso de los más relevantes.
tokens que reciben las puntuaciones de atención más altas de X (1) V: S (1).

Para los siguientes clips de vídeo, creamos una cola FIFO de pasados
tokens seleccionados [ S (0) , S (1) , . . . , S ( t ) ] que se proporcionan
al LLM como contexto un largo con el siguiente clip corto tokens sin procesar X (t
+1) V hasta el límite de la longitud de la ventana de contexto W o calcular
restricción.

## 3.3 Respuesta eficaz a preguntas en vídeo

Hemos ilustrado cómo procesar cada token de videoclip X ( t ) V
representar fotogramas T mientras se tiene acceso a una larga ventana del pasado
fichas seleccionadas. Durante este proceso se genera una leyenda C ( t ),
capturar los pensamientos de LLM y comprender el clip actual
eventos condicionados a clips anteriores proporcionados en la ventana contextual. Nosotrosalmacenar estas incrustaciones de subtítulos generados { X ( t ) C } y dado un
pregunta q, calculamos la similitud coseno promedio entre los
tokens de pregunta X q y los tokens de cada título almacenado { X ( t ) C }
. En lugar de recuperar los K títulos principales basándose únicamente en el promedio
similitud del coseno con la consulta q, usamos Relevancia Marginal Máxima
(MMR) [4], una técnica de recuperación común en la que se calcula una puntuación mediante
equilibrar la relevancia para la consulta (similitud de coseno entre título y
consulta) con diversidad entre los títulos seleccionados (similitud de coseno
entre los títulos candidatos y los ya seleccionados). Utilizando esto
técnica, reducimos efectivamente la posible redundancia que puede surgir en| Consulta | Consulta ||---------|---------------------------------||         | Procesamiento de vídeo en streaming || 1: | M l ← [ ] , M s ← cola (), B ← [ ] || 2: | MAX_MEM ← 16, CLIP_SIZE ← 16 || 3: | mientras que los marcos disponibles sí lo hacen || 4: | B . agregar(get_next_frame()) || 5: | si longitud (B) == CLIP_SIZE entonces || 6: | contexto ← M s + B || 7: | B .claro() || 8: | S , C ← Attn_Selection(contexto) || 9: | si longitud (M s) == MAX_MEM entonces || 10: | Ms .pop_left() || 11: | terminar si || 12: | M s .append( S ) || 13: | M l .append( C ) || 14: | terminar si || 15: | terminar mientras ||         | Respuesta a consultas || 16: | Q ← insertar (consulta) || 17: | C ′ ← Recuperar_TopK ( Q , M l ) || 18: | contexto ← C ′ + Q || 19: | respuesta ← LLM_Generate_Answer(contexto) |generación de subtítulos recurrentes, al tiempo que promueve la cobertura de distintos
información al contestar. El algoritmo 1 resume nuestro enfoque.

Nuestro diseño simple y nuestra dependencia de los subtítulos generados para las preguntas en video
la respuesta se deriva de la eficiencia de almacenar los tokens de subtítulos
ID y la utilidad de la estimación de similitud entre tokens textuales
sin depender de ningún codificador integrado externo como CLIP
[25]. Aunque podemos almacenar y proporcionar tokens visuales X (t) V y
subtítulos X ( t ) C al LLM para responder preguntas, nuestros experimentos
muestran que los LLM de video funcionan mejor solo con la entrada de subtítulos.

2 Básicamente, seleccionamos los tokens visuales N S superiores (según la atención
partituras) y almacenarlas en su orden temporal original.

## 4 experimentos

En esta sección, evaluamos nuestro método propuesto comparándolo con el estado de la situación.
enfoques artísticos en streaming y puntos de referencia de comprensión de videos largos,
demostrando un sólido desempeño y una notable eficiencia en todos
experimentos.

Puntos de referencia de evaluación. Evaluamos la efectividad de nuestro método en línea.
escenarios que utilizan el punto de referencia Realtime VStream-QA [38], que incluye
RVS-Movie (que enfatiza la comprensión de la trama) y RVS-Ego (que se centra en
comprensión visual) -ambos con videos de 40 minutos con diversas aperturas-
preguntas terminadas. Para demostrar solidez, también informamos resultados sobre
puntos de referencia sin conexión: MovieChat [28] (170 vídeos con un promedio de 576 segundos
en varios géneros con 510 preguntas que prueban de largo alcance
comprensión), VStream-QA fuera de línea (VS-Movie y VS-Ego) y CG-Bench
[5] (1,219 videos con un promedio de 27 minutos con 12K de opción múltiple
preguntas). Además, realizamos una ablación en NextQA -valset [36]
(570 videos más cortos con un promedio de 44 segundos con 5K de opción múltiple
preguntas) para validar nuestra selección de tokens visuales basada en la atención
enfoque.

Líneas de base. Comparamos nuestro método con los LLM en video establecidos y
enfoques recientes de comprensión de videos largos. Líneas de base requeridas para la capacitación
incluyen: VideoChatGPT [22] (que emplea agrupación temporal-espacial), Chat-
UniVi [14] (que ofrece representaciones unificadas de imagen/vídeo a través de un conjunto
de tokens visuales dinámicos), LLaMA-VID [21] (utilizando aprendizaje basado
compresión con tokens de contexto/contenido por fotograma), VideoScan [20]
(comprimir marcos en tokens semánticos únicos aprendidos preservando
contexto temporal), y Flash-VStream-7B [38] (procesamiento cuadro por cuadro
con memoria jerárquica que incluye memoria espacial FIFO, k-medias ponderadas
consolidación temporal y abstracción de información pasada-actual). Todos
operar en ventanas correderas que no se superpongan. Líneas de base sin entrenamiento
incluyen: MovieChat [28] (usando memoria a corto plazo FIFO y similitud-
consolidación a largo plazo basada), Goldfish [1] (procesamiento independiente
clips cortos con generación y recuperación de subtítulos) y ReKV [9]
(almacenamiento de KV-Cache completo recuperado mediante atención cruzada a consultas).
acuñe nuestro método rLiVS como abreviatura de Visual recurrente informado por LLM
Selección.

Métricas de Evaluación. Para los puntos de referencia con preguntas abiertas, informamos
Tanto la precisión como una puntuación basada en la evaluación GPT-3.5, después de una evaluación previa.
funciona (usamos gpt-3.5-turbo-0613, consistente con la configuración de evaluación
de ReKV [9]). Al modelo se le dan la pregunta, la verdad fundamental y
predicción, y se le pidió que evaluara si la predicción coincide con la
responder ("Sí/No") y asignar una puntuación de compatibilidad. el aviso
La plantilla se proporciona en el Apéndice. Para los puntos de referencia de opción múltiple, nosotros
calcular la precisión comparando directamente las predicciones con las verdades fundamentales.Además, para el punto de referencia de transmisión, informamos la latencia de un extremo a otro,
Uso de GPU (desde la llegada de la consulta hasta la respuesta) y utilización de KV-Cache.
Las evaluaciones comparativas se realizan en una única GPU A100.

Detalles de implementación. Implementamos nuestro método en LLaVA-OneVision [17],
un potente VLM para tareas de imagen y vídeo, que permite la comparación directa con
ReKV (estado actual del arte en puntos de referencia RVS). demostramos
versatilidad al evaluar las variantes 7B y 0.5B, con 7B usado a menos que
especificado de otra manera. Dado que LLaVA-OV se entrena con 32 fotogramas (196
tokens visuales cada uno), asignamos 16 cuadros para clips cortos actuales y
16 para la memoria recurrente a corto plazo. Seleccionamos solo 196 tokens visuales.
de 3.136 disponibles (16×196), conservando sólo el 6,25% del total visual
información por clip corto. Siguiendo trabajos previos, procesamos RVS-
Movie y RVS-Ego a 0,5 FPS [38, 9], MovieChat a 1 FPS y CG-Bench
y VS-Stream sin conexión a 0,5 FPS, con 10.000 tokens de contexto para recuperación
y generación. Promediamos las puntuaciones de atención de 4 (de 28) pilares
capas a lo largo de los experimentos. Para mostrar la generalización en video-LLM
columna vertebral de nuestro canal de transmisión de video, también incorporamos rLIVS para
Qwen2.5-VL-7B [3] y prueba en el punto de referencia de transmisión RVS. Proporcionamos
detalles para esta configuración en el Apéndice A. Finalmente, en el Apéndice
incluir ablaciones en la selección de capas (Apéndice B), así como en
métodos de agregación de atención (promedio versus máximo) y análisis de cómo
la longitud del contexto afecta el rendimiento y la latencia en los Apéndices C y D,
respectivamente.

## 4.1 Resultados

A continuación presentamos los resultados de los puntos de referencia discutidos: comenzando con
una ablación de métodos de selección visual para la comprensión de vídeos cortos,
seguido de evaluaciones sobre el control de calidad de videos largos fuera de línea y concluyendo con
Comprensión de transmisión de video.

Tabla 1: Comparación de pruebas comparativas de vídeos largos sin conexión. La precisión (Acc.) y la puntuación (Sco.) se informan cuando corresponde. Los resultados para ReKV se toman de [9], para VS-Stream de [38], para Moviechat de [28] y [1], mientras que para CG-Bench de [5].| Método | VS-Ego | VS-Ego | Película VS | Película VS | PelículaChat | PelículaChat | Banco CG ||----------------------|----------|----------|------------|------------|-------------|-------------|------------||                       | Acc.     | Esco.     | Acc.       | Esco.       | Acc.        | Esco.        | Acc.       || VideochatGPT [22] | 51,7 | 3.7 | 54,4 | 3.4 | 47,6 | 2.5 | - || PelículaChat [28] | 52,2 | 3.4 | 39.1 | 2.3 | 62,3 | 3.2 | - || Chat-UniVi [14] | 50,9 | 3.8 | 54,0 | 3.4 | - | - | 25,9 || LLaMA-VID [21]        | 54.8     | 3.9      | 51.4       | 3.4        | 53.2        | 3.8         | -          || Pez dorado [1] | - | - | - | - | 67,6 | 4.2 | - || Flash-VStream-7B [38] | 59,0 | 3.9 | 56.1 | 3.4 | - | - | - || rLiVS (Nuestro) | 61,0 | 3.9 | 59,3 | 3.6 | 78,0 | 4.0 | 33.1 |## 4.1.1 Evaluación de estrategias de selección de tokens visuales en conjuntos de datos de videos cortos

La selección visual de tokens es fundamental para nuestro método, ya que supera
información en el procesamiento actual de clips cortos a través de la recurrencia, mejorando
subtitulado y comprensión de la transmisión de video general.

Sostenemos que los métodos ingenuos como el muestreo uniforme o la combinación de medias
corre el riesgo de descartar información importante, mientras que la agrupación tradicional
enfoques como K-Means pueden ser lentos y subóptimos en aplicaciones de alta dimensión
espacios. En cambio, al aprovechar la atención ya calculada del LLM
durante la generación de subtítulos: obtenemos una señal potente y de baja sobrecarga que
Permite una compresión agresiva con una mínima pérdida de rendimiento. a
Para validar esto empíricamente, realizamos experimentos en NextQA-valset,
un breve vídeo comparativo para evaluar la calidad de selección del token
independientemente de los factores de comprensión de vídeos largos. comparamos
el rendimiento del modelo completo manteniendo sólo el 6% o el 12% de la visualización
tokens seleccionados con diferentes métodos:

Tabla 2: Precisión (Acc.) de diferentes métodos de selección evaluados en
SiguienteQA-valset.| Método de selección |   Próximo control de calidad (valset) Acc. ||--------------------|-------------------------|| Modelo completo |                    78,6 || Muestreo uniforme (6%) |                    75,5 || Agrupación media (6%) |                    70,7 || K-Medias ( 6% ) |                    76,8 || Atención (nuestra) ( 6% ) |                    77,0 || Muestreo uniforme ( 12 % ) |                    76,7 || Agrupación media ( 12 % ) |                    75,5 || K-Medias ( 12 % ) |                    78,1 || Atención (nuestra) ( 12% ) |                    78,4 |1) mediante muestreo uniforme, 2) mediante combinación de medias, 3) mediante K-Means y 4) mediante muestreo uniforme
basado en nuestro mecanismo de selección de atención al título generado, como
descrito en la Sección 3.1. La tabla 2 confirma nuestra intuición; mientras uniforme
El muestreo da como resultado una pérdida de rendimiento del 2-3%, nuestra selección basada en la atención
supera el muestreo uniforme incluso al doble de la tasa de compresión.
incurrir en sólo una caída de rendimiento del 1,5% con el 6% de tokens visuales, y
casi igualando el rendimiento del modelo completo con un 12%. Además, significa agrupación,
combinar ingenuamente tokens visuales sin considerar su relevancia,
produce el peor rendimiento, mientras que K-Means tiene un rendimiento comparable al
selección basada en la atención, pero es significativamente más lenta, especialmente cuando
operando en el espacio de entrada de alta dimensión de la introducción del LLM
sobrecarga computacional innecesaria.

## 4.1.2 Conjuntos de datos de respuesta a preguntas en vídeo sin conexión

En la Tabla 1 presentamos los resultados sobre la comprensión de vídeos largos sin conexión.
puntos de referencia. rLiVS incorporado en LLaVA-OV-7B logra lo último en tecnología
rendimiento en los puntos de referencia VS-Ego y VS-Movie, superando al
mejor anterior en un 2% y un 3%, respectivamente. En MovieChat y CG-Bench, nuestro
método logra resultados sólidos, superando enfoques anteriores,
demostrando la efectividad de comprimir fuertemente tokens visuales
sin perjudicar el rendimiento. En general, estos resultados indican que nuestra
El método sirve como una base sólida, manejando eficientemente entradas largas,
No requiere capacitación adicional ni ajustes, mientras se mantiene
Requisitos mínimos de almacenamiento de memoria.

Tabla 3: Evaluación del punto de referencia de transmisión Realtime VStream-QA [38], que consta de los subconjuntos RVS-Ego y RVS-Movie. Se informa el uso de precisión (Acc.), puntuación (Sco.), latencia, VRAM y KV-Cache. Los resultados de las líneas de base se toman de [20].| Método | RVS-Ego | RVS-Ego | Película RVS | Película RVS | Latencia | VRAM | Caché KV ||--------------------------------|-----------|-----------|-------------|-------------|-----------|--------|------------|
|                                | Acc.      | Esco.      | Acc.        | Esco.        |           |        |            || PelículaChat [28] | 50,7 | 3.4 | 36,0 | 2.3 | - | - | - || LLaMA-VID [21]                 | 53.4      | 3.9       | 48.6        | 3.3         | -         | -      | -          || Flash-VStream-7B [38] | 57,3 | 4.0 | 53.1 | 3.3 | 2,1s | 19GB | - || Escaneo de vídeo [20] | 60,9 | 4.0 | 54.1 | 3.5 | 2,1s | 18GB | - || LLAVE-OV 0.5B ↪ → ReKV [9] | 54,7 | 3.7 | 44,6 | 3.4 | 1,6s | 19GB | 4,0 GB/h || ↪ → rLiVS (Nuestro) | 57,6 | 3.8 | 51,3 | 3.4 | 1,5 s | 11GB | - || LLaVA-OV 7B ↪ → ReKV [9] | 63,7 | 4.0 | 54,4 | 3.6 | 2,7s | 36GB | 18,8 GB/h || ↪ → rLiVS (Nuestro) | 65,3 | 4.0 | 57,7 | 3.6 | 1,9s | 25GB | - || Qwen2.5-VL 7B ↪ → rLiVS (Nuestro) | 68.1 | 4.0 | 56.1 | 3.6 | 2,7s | 19GB | - |## 4.1.3 Transmisión de conjuntos de datos de respuesta a preguntas en vídeo

Finalmente, presentamos nuestros resultados sobre RVS-Ego y RVS-Movie, vídeo largo
puntos de referencia de transmisión que representan el enfoque principal de este trabajo y
el escenario principal para evaluar nuestro método propuesto. rLiVS demuestra
gran idoneidad para este entorno, como se muestra en la Tabla 3, superando
el mejor anterior, ReKV, con el mismo modelo de red troncal, en un 2-3% y
lograr nuevos resultados de vanguardia en el punto de referencia, al mismo tiempo que
siendo casi 1 segundo más rápido que ReKV. Es importante destacar que nuestra atención
técnica de compresión basada en combinación con recurrencia y guiada por subtítulos.
recuperación y respuesta-resulta sólido en ambos subconjuntos del
punto de referencia de transmisión. A diferencia de ReKV, rLiVS no requiere memoria externa
descarga y opera dentro de una ventana de contexto de 10K, lo que permite
rendimiento eficiente y efectivo, al tiempo que resulta en 11 GB menos de pico
Uso de VRAM con la misma red troncal de video-LLM. Curiosamente, cuando se combina
Con el modelo liviano 0.5B, nuestro método supera al anterior.
segundo mejor enfoque que utiliza un modelo 7B, así como ReKV con el mismo
0.5B en un 2,9% en RVS-Ego y un 6,7% en RVS-Movie, destacando el
Fortaleza de la respuesta basada en subtítulos incluso en variantes de modelos más pequeños
enfatizando el aspecto eficiente de nuestras elecciones de diseño. Finalmente, por
acoplando nuestro método con el reciente y fuerte LLM multimodal Qwen2.5-VL, podemos
Mejore aún más el rendimiento en RVS-Ego, alcanzando una precisión del 68,1%. esto
destaca la naturaleza independiente del modelo y plug-and-play de nuestra propuesta
rLiVS.

Una fortaleza clave de nuestro marco es su capacidad para adaptar la selección de tokens.
basado dinámicamente en instrucciones de tareas, que sirven como señales de arriba hacia abajo.
A través de la atención guiada por instrucciones, el modelo selecciona un subconjunto disperso
de tokens visuales semánticamente relevantes, lo que permite un uso eficiente y contextual.
comprensión consciente. Como se muestra en el Apéndice H, la selección de tokens es altamente
sensible a la instrucción, con diferentes tareas que producen distintos
Patrones de atención. Esta adaptabilidad admite una amplia gama de videos.
comprender tareas sin reentrenamiento o cambios arquitectónicos,
destacando direcciones prometedoras para largo plazo escalables y generalizables.
modelos de vídeo.

## 4.1.4 Ablación de opciones de diseño de rLiVS

A continuación, eliminamos las opciones de diseño principales de nuestro método mediante
experimentos, demostrando lo siguiente: (1) la importancia de
recurrencia al procesar vídeos largos divididos en clips cortos; (2) el
superioridad de los subtítulos sobre tokens visuales seleccionados, o su
combinación: para recuperar y responder en secuencias de vídeo largas, (3) el
Beneficios de rendimiento de LLM- Tabla 4: Ablación sobre el efecto de
acceder a información visual pasada a través de la recurrencia y la atención
selección.| Método | RVS-Ego | RVS-Ego | Película RVS | Película RVS | PelículaChat | PelículaChat ||----------------|-----------|-----------|-------------|-------------|-------------|-------------|
|                | Acc.      | Esco.      | Acc.        | Esco.        | Acc.        | Esco.        || rLiVS | 65,3 | 4.0 | 57,7 | 3.6 | 78,0 | 4.0 || sin recurrencia | 62,5 | 3.9 | 53,7 | 3.5 | 74.1 | 3.9 |selección informada basada en la atención en comparación con el muestreo uniforme ingenuo en
escenarios de transmisión; y (4) el efecto de la tasa de compresión en
rendimiento aguas abajo.

La importancia de la recurrencia en flujos largos. Como se discutió anteriormente,
[1] propone procesar vídeos largos como clips cortos independientes, almacenando
solo los resultados textuales (subtítulos) generados por un video-LLM y
recuperar información relevante basada en la similitud del coseno con la entrada
pregunta. Si bien este enfoque es simple y eficiente, adolece de un
Falta de continuidad que se vuelve cada vez más problemática a medida que aumenta la duración del vídeo.
aumenta. Específicamente, la ausencia de información visual compartida entre
Los clips y la dependencia exclusiva de representaciones textuales pueden dificultar la
capacidad del modelo para realizar un seguimiento consistente de entidades, como personas o
objetos a lo largo del tiempo, especialmente cuando los subtítulos carecen de distinción
contexto. Estas limitaciones naturalmente afectan el rendimiento. empíricamente
Para demostrarlo, presentamos resultados (Tabla 4) en RVS-Ego, RVS-Movie y
MovieChat, que demuestra que la introducción de la recurrencia mejora constantemente
rendimiento en un 3-4%, en condiciones por lo demás idénticas. Es importante destacar que
La recurrencia en nuestro método tiene un doble propósito: (1) mejora
Continuidad y coherencia en clips cortos, impulsando vídeos largos.
comprensión, y (2) guía la atención del LLM durante el token visual
selección, reforzando su propio impacto.

Subtítulos versus tokens visuales para recuperación y respuesta. un natural
La pregunta es si la información textual (títulos) o la imagen seleccionada
Los tokens proporcionan una mejor señal para recuperar y responder al usuario.
consultas, es decir, qué representación es la mejor opción para realizar
Preservar información crucial al tiempo que permite la compresión. Para recuperación
, la consulta de entrada está integrada en el espacio de entrada del LLM. Fichas visuales,
proyectado ad hoc desde el codificador visual, falta semántica aprendida
alineación con las consultas, lo que lleva a puntuaciones de similitud menos significativas (como
ilustrado en el Apéndice F). La similitud adecuada entre ellos requeriría
proyectar consultas y tokens visuales en un espacio común (como CLIP
codificadores [25]), agregando pasos computacionales adicionales a nuestra canalización.

Por el contrario, los subtítulos comparten la misma modalidad que las consultas y pueden ser
directamente integrado en el espacio de entrada del LLM, haciéndolos naturalmente mejores
adecuado para la recuperación mediante similitud de coseno simple. Para responder, tabla
5 presenta resultados utilizando tokens visuales, subtítulos o su combinación
como información recuperada para cada clip corto. Entre los enfoques,
utilizar sólo subtítulos logra el mejor rendimiento. A pesar de las señales visuales
teóricamente ofrecen información más rica, probablemente tienen un rendimiento inferior debido
al

Tabla 5: Comparación de las modalidades de visión y lenguaje para la recuperación de información y respuesta a consultas en los puntos de referencia RVSEgo/Movie.| Modalidad recuperada | RVS-Ego | RVS-Ego | Película RVS | Película RVS ||-----------------------|-----------|-----------|-------------|-------------||                        | Acc.      | Esco.      | Acc.        | Esco.        || Fichas visuales seleccionadas | 58,2 | 3.9 | 48,4 | 3.5 || Subtítulos | 65,1 | 4.0 | 57,7 | 3.6 || Combinación | 63,0 | 4.0 | 54,3 | 3.5 |discrepancia entre los datos de entrenamiento de video-LLM (predominantemente clips cortos de un
pocos segundos) y contextos de evaluación (minutos/hora de vídeos en streaming).
Los video-LLM actuales luchan por comprender la información visual en general
escalas de tiempo, incluso cuando computacionalmente se pueden representar como entrada. esto
se alinea con los hallazgos de [33], que demostró que a pesar de la
Existencia de optimizaciones arquitectónicas que permiten vídeos largos.
procesamiento (a través de estructuras de memoria o representaciones comprimidas),
Los video-LLM todavía demuestran una degradación significativa del rendimiento en
estos contextos. Por el contrario, la investigación de LLM ha explorado ampliamente
Adaptación al contexto, desarrollo de modelos y capacitación efectivos.
Técnicas que permiten el razonamiento sobre secuencias extensas de textos.
entradas, como subtítulos de múltiples clips. Por lo tanto, al utilizar sólo los subtítulos
como información pasada durante la recuperación y respuesta, efectivamente
transformar el problema en un problema de control de calidad de contexto largo basado en texto,
aprovechar la fortaleza probada de los LLM en el razonamiento sobre estos extendidos
contextos.

Selección visual uniforme versus basada en la atención en escenarios de transmisión. Nosotros
demostrado previamente (Sec. 4.1.1) que nuestra selección basada en la atención
supera el muestreo uniforme en el control de calidad de videos cortos, incluso al doble de
tasa de compresión, lo que permite una reducción agresiva de tokens con un mínimo
pérdida de rendimiento. Ampliamos ahora esta valoración al streaming
configuración, donde la selección impulsa los subtítulos (a través de la recurrencia)

Tabla 6: Comparación de métodos de selección dentro de nuestra cartera en RVS-Ego y RVS-Movie.| Método de selección | RVS-Ego | RVS-Ego | Película RVS | Película RVS ||--------------------|-----------|-----------|-------------|-------------|
|                    | Acc.      | Esco.      | Acc.        | Esco.        || Muestreo uniforme | 64,2 | 3.9 | 56,0 | 3.5 || Atención (nuestra) | 65,1 | 4.0 | 57,7 | 3.6 |y por lo tanto juega un papel crítico en el desempeño general. Nuestros resultados en
La Tabla 6 es consistente con las del punto de referencia de clips cortos:
selección basada en la atención, aprovechando la capacidad de LLM para atender a temas cruciales
La información, que filtra eficazmente el ruido y la redundancia, es superior.
en ambos subconjuntos entre un 1% y un 2%, sin introducir ningún coste adicional,
ya que la generación de subtítulos es parte de nuestro proceso. Tenga en cuenta que el uniforme
El muestreo está integrado en nuestra línea completa, reemplazando solo el
estrategia de selección. Este enfoque sirve como una manera significativamente más fuerte
línea de base en comparación con la línea de base de muestreo uniforme de ReKV [9], como
demostrado en el Apéndice.

## Tasa de compresión y rendimiento descendente.

Nuestra elección de compresión visual agresiva de tokens (por ejemplo, retener solo
6%) puede generar preocupaciones sobre la posible pérdida de información y
degradación del rendimiento. Sin embargo, esto refleja una compensación práctica en
Comprensión de vídeo en tiempo real. Los vídeos de formato largo suelen contener
Redundancia visual sustancial, lo que permite un filtrado de tokens eficaz.
sin perjudicar el rendimiento. Para validar esto, la Tabla 7 muestra cómo
La precisión varía con diferentes tasas de compresión en NextQA.
conjunto de validación. Retener el 12% de los tokens visuales casi coincide con el total
rendimiento, mientras que el 6 % conduce a pérdidas insignificantes 3 . Nuestro marco sigue siendo
Flexible, con una relación de compresión ajustable a los requisitos de la tarea.

Tabla 7: Acc. en NextQA valset para diferentes % de tokens visuales seleccionados
con nuestro método basado en la atención propuesto.| Fichas seleccionadas |   Acc.(%) ||-------------------|-----------|
| 1% |      68,0 || 6% |      77,0 || 12% |      78,4 || 19% |      78,7 || 25% |      79,0 || 100% |      78,6 |De cara al futuro, compresión adaptativa y retención que ajusta dinámicamente
basado en la complejidad del contenido visual, es una dirección prometedora. mientras
puede mejorar el rendimiento en escenas visualmente ricas, introduce
sobrecarga computacional, que actualmente evitamos mantener en línea
eficiencia. Los resultados cualitativos en el Apéndice J demuestran aún más la
robustez de nuestro método bajo compresión extrema.

## 5 Discusión

La transmisión de video para responder preguntas y comprenderlas es una tendencia emergente.
panorama que puede permitir el despliegue confiable de agentes inteligentes
en diversos dominios que requieren comprensión visual. Un diseño clave
El requisito para soluciones prácticas es el bajo uso de memoria y computación. en
En este trabajo, proponemos un enfoque simple basado en dos opciones de diseño:
Primero, la selección instantánea de tokens visuales basados en los LLM.
atención, lo que da como resultado una tasa de compresión de aproximadamente 95 % del contenido de clips cortos;
En segundo lugar, el uso de procesamiento recurrente y preguntas basadas en subtítulos.
Responder para permitir sin problemas un razonamiento de nivel superior durante más tiempo.
se extiende. Con estos componentes conseguimos un rendimiento de última generación en
Pruebas comparativas de transmisión de video. Nuestro enfoque es agnóstico al video-LLM.
desplegado y no requiere capacitación, estableciendo una base sólida para el futuro
trabajo.

Más allá de la respuesta a preguntas, nuestro marco muestra potencial como general.
columna vertebral de propósito para tareas de comprensión y procesamiento de videos de formato largo
como resumen, recuperación y seguimiento de objetos. preliminar
Los resultados de los resúmenes de vídeo largos (Apéndice I) proporcionan una información inicial.
evidencia de la generalización del método. Además, nuestro análisis en
El Apéndice H demuestra que el mecanismo de selección de memoria es a la vez
sensible a las tareas y condicionado a la instrucción, lo que subraya su flexibilidad
y adaptabilidad. Consideramos que esto es una dirección prometedora para el futuro.
investigación, donde las descripciones de tareas en lenguaje natural pueden guiar dinámicamente
Selección de memoria y razonamiento.

Limitaciones. A pesar del buen desempeño, nuestro método prioriza
eficiencia sobre integridad al atender solo al contenido seleccionado
durante el procesamiento de clips cortos. Esto puede pasar por alto detalles detallados y
daña la continuidad, ya que el LLM carece de acceso al historial de video completo.
Mientras que nuestro búfer de memoria FIFO ofrece contexto y subtítulos a corto plazo
servir como memoria a largo plazo, es posible que este mecanismo temporal no siempre
capturar información semánticamente destacada. Una dirección prometedora es
explorar una selección de memoria más semántica, en lugar de puramente temporal
estrategias que siguen siendo eficientes.

Nuestro enfoque tampoco requiere formación y depende totalmente de personal previamente formado.
columna vertebral, heredando sus limitaciones en la comprensión visual y
razonamiento temporal. El proceso de subtitulación recurrente, condicionado a
marcos e instrucciones anteriores, pueden introducir redundancia entre subtítulos,
lo que puede afectar el rendimiento de la recuperación. Abordamos esto parcialmente mediante
considerando tanto la novedad como la relevancia durante la recuperación. Trabajo futuro
incluye integrar el método en los canales de capacitación y evaluar
en una gama más amplia de modelos y puntos de referencia para comprender mejor su
fortalezas y limitaciones.

3 Un beneficio adicional es que seleccionar solo el 6% de los tokens permite más
ranuras de memoria en la cola FIFO, mejorando la comprensión a largo plazo.

## Referencias- [1] Kirolos Ataallah, Xiaoqian Shen, Eslam Abdelrahman, Essam Sleiman, Mingchen Zhuge, Jian Ding, Deyao Zhu, Jürgen Schmidhuber y Mohamed Elhoseiny. Goldfish: comprensión visión-lenguaje de videos arbitrariamente largos. En Conferencia europea sobre visión por computadora, páginas 251-267. Springer, 2024.
- [2] Jinze Bai, Shuai Bai, Yunfei Chu, Zeyu Cui, Kai Dang, Xiaodong Deng, Yang Fan, Wenbin Ge, Yu Han, Fei Huang, et al. Informe técnico de Qwen. Preimpresión de arXiv arXiv:2309.16609, 2023.
- [3] Shuai Bai, Keqin Chen, Xuejing Liu, Jialin Wang, Wenbin Ge, Sibo Song, Kai Dang, Peng Wang, Shijie Wang, Jun Tang, et al. Qwen2. Informe técnico de 5 vl. Preimpresión de arXiv arXiv:2502.13923, 2025.
- [4] Kamil Bennani-Smires, Claudiu Musat, Andreea Hossmann, Michael Baeriswyl y Martin Jaggi. Extracción simple de frases clave sin supervisión mediante incrustaciones de oraciones. En Anna Korhonen e Ivan Titov, editores, Actas de la 22.ª Conferencia sobre aprendizaje computacional de lenguajes naturales, páginas 221-229, Bruselas, Bélgica, octubre de 2018. Asociación de Lingüística Computacional.
- [5] Guo Chen, Yicheng Liu, Yifei Huang, Yuping He, Baoqi Pei, Jilan Xu, Yali Wang, Tong Lu y Limin Wang. Cg-bench: punto de referencia de respuesta a preguntas basado en pistas para la comprensión de vídeos largos. Preimpresión de arXiv arXiv:2412.12075, 2024.
- [6] Marvin M Chun y Nicholas B Turk-Browne. Interacciones entre atención y memoria. Opinión actual en neurobiología, 17(2):177-184, 2007. Neurociencia cognitiva.
- [7] Nelson Cowan. El mágico número 4 en la memoria a corto plazo: una reconsideración de la capacidad de almacenamiento mental. Ciencias del comportamiento y del cerebro, 24:87 - 114, 2001.
- [8] Tri Dao. Flashattention-2: Atención más rápida con mejor paralelismo y partición del trabajo. Preimpresión de arXiv arXiv:2307.08691, 2023.
- [9] Shangzhe Di, Zhelun Yu, Guanghao Zhang, Haoyuan Li, TaoZhong, Hao Cheng, Bolin Li, Wanggui He, Fangxun Shu y Hao Jiang. Transmisión de preguntas y respuestas en video con recuperación de caché KV de video incontextual. En La Decimotercera Conferencia Internacional sobre Representaciones del Aprendizaje, 2025.
- [10] Gueter Josmy Faure, Jia-Fong Yeh, Min-Hung Chen, Hung-Ting Su, Shang-Hong Lai y Winston H Hsu. Hermes: comprensión temporal coherente de forma larga con episodios y semántica. Preimpresión de arXiv arXiv:2408.17443, 2024.
- [11] Tao Ge, Jing Hu, Lei Wang, Xun Wang, Si-Qing Chen y Furu Wei. Codificador automático en contexto para la compresión de contexto en un modelo de lenguaje grande. Preimpresión de arXiv arXiv:2307.06945, 2023.
- [12] Bo He, Hengduo Li, Young Kyun Jang, Menglin Jia, Xuefei Cao, Ashish Shah, Abhinav Shrivastava y Ser-Nam Lim. Ma-lmm: modelo multimodal grande con memoria aumentada para comprensión de videos a largo plazo. En Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 13504-13514, 2024.
- [13] Xiaohu Huang, Hao Zhou y Kai Han. Llm-vtp: poda de tokens visuales basada en Llm para una comprensión eficiente del vídeo multimodal.
- [14] Peng Jin, Ryuichi Takanobu, Wancai Zhang, Xiaochun Cao y Li Yuan. Chat-univi: la representación visual unificada potencia los modelos de lenguaje grandes con comprensión de imágenes y videos. En Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 13700-13710, 2024.
- [15] Victor A.F. Lamme y Pieter R. Roelfsema. Los distintos modos de visión que ofrecen el procesamiento feedforward y recurrente. Tendencias en Neurociencias, 23(11):571-579, 2000.- [16] Jie Lei, Linjie Li, Luowei Zhou, Zhe Gan, Tamara L Berg, Mohit Bansal y Jingjing Liu. Menos es más: Clipbert para el aprendizaje de idiomas y vídeo mediante muestreos dispersos. En Actas de la conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 7331-7341, 2021.

- [17] Bo Li, Yuanhan Zhang, Dong Guo, Renrui Zhang, Feng Li, Hao Zhang, Kaichen Zhang, Peiyuan Zhang, Yanwei Li, Ziwei Liu, et al. Llama-onevision: Fácil transferencia visual de tareas. Preimpresión de arXiv arXiv:2408.03326, 2024.
- [18] Haopeng Li, Andong Deng, Qiuhong Ke, Jun Liu, Hossein Rahmani, Yulan Guo, Bernt Schiele y Chen Chen. Sports-qa: ​​un punto de referencia de respuesta a preguntas en vídeo a gran escala para deportes complejos y profesionales. Preimpresión de arXiv arXiv:2401.01505, 2024.
- [19] Junnan Li, Dongxu Li, Caiming Xiong y Steven Hoi. Blip: entrenamiento previo de imágenes de lenguaje de arranque para la comprensión y generación unificadas de visión y lenguaje. En Conferencia internacional sobre aprendizaje automático, páginas 12888-12900. PMLR, 2022.
- [20] Ruanjun Li, Yuedong Tan, Yuanming Shi y Jiawei Shao. Videoscan: permite la comprensión eficiente de la transmisión de video a través de portadores semánticos a nivel de cuadro. Preimpresión de arXiv arXiv:2503.09387, 2025.
- [21] Yanwei Li, Chengyao Wang y Jiaya Jia. Llama-vid: Una imagen vale 2 tokens en modelos de lenguaje grandes. En Conferencia europea sobre visión por computadora, páginas 323-340. Springer, 2024.
- [22] Muhammad Maaz, Hanoona Rasheed, Salman Khan y Fahad Shahbaz Khan. Video-chatgpt: Hacia una comprensión detallada del vídeo a través de grandes modelos de visión y lenguaje. Preimpresión de arXiv arXiv:2306.05424, 2023.
- [23] George A. Miller. El mágico número siete, más o menos dos: unos límites a nuestra capacidad de procesar información. The Psychoological Review, 63(2):81-97, marzo de 1956.
- [24] Rui Qian, Xiaoyi Dong, Pan Zhang, Yuhang Zang, Shuangrui Ding, Dahua Lin y Jiaqi Wang. Transmisión de comprensión de vídeos largos con modelos de lenguaje de gran tamaño. Avances en los sistemas de procesamiento de información neuronal, 37:119336-119360, 2024.
- [25] Alec Radford, Jong Wook Kim, Chris Hallacy, Aditya Ramesh, Gabriel Goh, Sandhini Agarwal, Girish Sastry, Amanda Askell, Pamela Mishkin, Jack Clark, et al. Aprendizaje de modelos visuales transferibles a partir de la supervisión del lenguaje natural. En Conferencia internacional sobre aprendizaje automático, páginas 8748-8763. PMLR, 2021.
- [26] Zhenmei Shi, Yifei Ming, Xuan-Phi Nguyen, Yingyu Liang y Shafiq Joty. Descubriendo las gemas en las primeras capas: acelerando películas de contexto largo con una reducción de token de entrada de 1000x. Preimpresión de arXiv arXiv:2409.17422, 2024.
- [27] Yan Shu, Zheng Liu, Peitian Zhang, Minghao Qin, Junjie Zhou, Zhengyang Liang, Tiejun Huang y Bo Zhao. Video-xl: modelo de lenguaje de visión extralarga para comprensión de videos a escala de horas. En Actas de la Conferencia sobre visión por computadora y reconocimiento de patrones, páginas 26160-26169, 2025.
- [28] Enxin Song, Wenhao Chai, Guanhong Wang, Yucheng Zhang, Haoyang Zhou, Feiyang Wu, Haozhe Chi, Xun Guo, Tian Ye, Yanting Zhang, et al. Moviechat: desde token denso hasta memoria escasa para una comprensión prolongada de vídeos. En Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 18221-18232, 2024.
- [29] Zhan Tong, Yibing Song, Jue Wang y Limin Wang. Videomae: los codificadores automáticos enmascarados son aprendices eficientes en datos para el entrenamiento previo de video autosupervisado. Avances en sistemas de procesamiento de información neuronal, 35:10078-10093, 2022.- [30] María Tsimpoukelli, Jacob L Menick, Serkan Cabi, SM Eslami, Oriol Vinyals y Felix Hill. Aprendizaje multimodal de pocas oportunidades con modelos de lenguaje congelados. Avances en los sistemas de procesamiento de información neuronal, 34:200-212, 2021.
- [31] Jiawei Wang, Liping Yuan, Yuchen Zhang y Haomiao Sun. Tarsero: Recetas para entrenar y evaluar modelos de descripción de videos grandes. Preimpresión de arXiv arXiv:2407.00634, 2024.
- [32] Limin Wang, Bingkun Huang, Zhiyu Zhao, Zhan Tong, Yinan He, Yi Wang, Yali Wang y Yu Qiao. Videomae v2: Escalado de codificadores automáticos de vídeo enmascarados con enmascaramiento dual. En Actas de la conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 14549-14560, 2023.

- [33] Weihan Wang, Zehai He, Wenyi Hong, Yean Cheng, Xiaohan Zhang, Ji Qi, Xiaotao Gu, Shiyu Huang, Bin Xu, Yuxiao Dong, et al. Lvbench: un punto de referencia de comprensión de videos extremadamente largo. Preimpresión de arXiv arXiv:2406.08035, 2024.
- [34] Yi Wang, Kunchang Li, Yizhuo Li, Yinan He, Bingkun Huang, Zhiyu Zhao, Hongjie Zhang, Jilan Xu, Yi Liu, Zun Wang, et al. Internvideo: Modelos básicos de vídeo general a través del aprendizaje generativo y discriminativo. Preimpresión de arXiv arXiv:2212.03191, 2022.
- [35] Chao-Yuan Wu, Yanghao Li, Karttikeya Mangalam, Haoqi Fan, Bo Xiong, Jitendra Malik y Christoph Feichtenhofer. Memvit: transformador de visión multiescala con memoria aumentada para un reconocimiento de vídeo eficiente a largo plazo, 2022.
- [36] Junbin Xiao, Xindi Shang, Angela Yao y Tat-Seng Chua. Next-qa: Siguiente fase de preguntas y respuestas para explicar las acciones temporales. En Actas de la conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 9777-9786, 2021.
- [37] Weihao Ye, Qiong Wu, Wenhao Lin y Yiyi Zhou. Ajustar y podar: poda de tokens visual rápida y sin capacitación para modelos de lenguaje grandes multimodales. En Actas de la Conferencia AAAI sobre Inteligencia Artificial, volumen 39, páginas 22128-22136, 2025.
- [38] Haoji Zhang, Yiqin Wang, Yansong Tang, Yong Liu, Jiashi Feng, Jifeng Dai y Xiaojie Jin. Flash-vstream: comprensión en tiempo real basada en memoria para transmisiones de video largas. Preimpresión de arXiv arXiv:2406.08085, 2024.
- [39] Junjie Zhou, Yan Shu, Bo Zhao, Boya Wu, Zhengyang Liang, Shitao Xiao, Minghao Qin, Xi Yang, Yongping Xiong, Bo Zhang, et al. Mlvu: Evaluación comparativa de la comprensión de vídeos largos multitarea. En Actas de la Conferencia sobre visión por computadora y reconocimiento de patrones, páginas 13691-13701, 2025.

## Apéndices técnicos y material complementario

## Detalles experimentales

En esta sección le ofrecemos una descripción detallada de nuestra evaluación.
configuración y opciones metodológicas para garantizar la reproducibilidad de nuestros
experimentos:- Selección de tokens visuales: como se describe en el artículo principal, nuestro método de selección basado en la atención selecciona k de N tokens visuales a nivel mundial. El video-LLM principal que utilizamos (LLaVA-OV) se entrena en 32 cuadros de video, cada uno de los cuales contribuye con 196 tokens, lo que da como resultado un total de 6272 tokens visuales. Dividimos la ventana de contexto de 6272 tokens en dos mitades: la primera se usa para codificar información pasada (inyectando tokens visuales previamente seleccionados) y la segunda se reserva para los tokens visuales sin procesar del clip corto actual. Elegimos k de modo que divida uniformemente los 3136 tokens de la segunda mitad (clip corto actual) para utilizar completamente el contexto. Durante la selección, consideramos solo la segunda mitad, correspondiente al clip corto actual (es decir, 3136 tokens). Para las pruebas comparativas de transmisión y fuera de línea de videos largos, seleccionamos 196 tokens de este conjunto, que es 1,16 (o aproximadamente el 6,25%) del total. Para simplificar, informamos esto como 6% s en todo el documento. En nuestra línea de base uniforme, todo sigue igual y solo tomamos una muestra de 196 de los 3136 tokens del clip corto actual.
- Recursos: en todos los experimentos, utilizamos una o dos GPU NVIDIA A100 con 40 GB de memoria. Las mediciones de latencia y VRAM se realizan en una sola GPU.
- Evaluación de LLM: como se informa en el artículo principal, seguimos el trabajo anterior [9, 38] para la evaluación basada en LLM y utilizamos gpt-3.5-turbo. Las plantillas de indicaciones empleadas se proporcionan en la Sección K.
- Configuración Qwen2.5-VL: este modelo incorpora dinámicamente fotogramas según la resolución de entrada, lo que da como resultado un número diferente de tokens por fotograma. En nuestros experimentos, utilizamos la misma cantidad de tokens por clip corto que LLaVA-OV para garantizar una comparación justa.

## Selección de capa B para selección de token visual basada en la atención

En nuestro método, los tokens visuales se seleccionan en función de su atención.
puntuaciones a los subtítulos generados. En lugar de utilizar la atención de todos
capas, calculamos puntuaciones de un pequeño subconjunto. Atención rápida moderna
implementaciones (por ejemplo, FlashAttention-2 [8]) evitan explícitamente
materializando la matriz de atención N 2 completa (con N como la secuencia
longitud) para reducir la memoria y el costo computacional. Para mantener la eficiencia
y evitar gastos generales adicionales, utilizamos la atención desde solo un pequeño ℓ de distancia
de L número total de capas (28 en LLaVA-OV). en el periódico principal
experimentos, establecemos l = 4 y seleccionamos estas capas mediante uniforme
muestreo a lo largo de la profundidad de la red. Como se muestra en la Tabla 8, este
Una estrategia simple proporciona un rendimiento sólido comparable al uso de todos
capas, con una variación mínima. Por el contrario, seleccionar capas de
Las regiones localizadas (por ejemplo, temprana, media o tardía) producen rendimientos menos estables.
resultados, y el uso de una sola capa da como resultado resultados significativamente peores.
rendimiento.

## Métodos C para agregar puntuaciones de atención en la selección de tokens

En nuestro método, optamos por la puntuación media sobre diferentes puntos de atención.
para selección de tokens; sin embargo, observamos que la puntuación alternativa
También podrían explorarse estrategias de agregación. Aquí presentamos resultados
en NextQA-valset usando max como método de agregación y seleccionando
6% de las fichas visuales (para comparar directamente con los resultados de la
artículo principal). Como se muestra en la Tabla 9, ambos métodos funcionan de manera similar, con una
diferencia insignificante de sólo 0 . 1% en precisión.

## D Longitud del contexto para recuperar y recuperar. Respondiendo

En el artículo principal, utilizamos una longitud de contexto de 10k tokens para recuperar
y responder consultas de los usuarios. Esta elección está motivada por dos factores clave:(1) Latencia: En aplicaciones de vídeo en streaming, donde llegan las consultas de los usuarios
Además de los fotogramas en tiempo real, las respuestas de baja latencia son fundamentales.
Aumentar la longitud del contexto ralentiza sustancialmente tanto la recuperación
e inferencia LLM. (2) Filtrado de ruido: las transmisiones de vídeo largas son altamente
Los contextos redundantes y más amplios aumentan el riesgo de incluir información irrelevante.
información que puede degradar el rendimiento. Validamos empíricamente
estas consideraciones en la Tabla 10 sobre RVSMovie y RVS-Ego streaming
puntos de referencia, lo que demuestra que si bien se aumenta el contexto de 6k a 10k
mejora la precisión, un aumento adicional a 20k no ofrece más
beneficio -y en algunos casos, incluso perjudica el rendimiento- al tiempo que incurre en mayores
costos computacionales. En general, una longitud de contexto de 10k ofrece una ventaja
equilibrio entre eficacia y eficiencia. Finalmente, notamos que
ReKV [9] recupera 64 fotogramas (cada uno consta de 196 tokens, desde el
Se utiliza el mismo modelo), lo que resulta en un contexto más amplio y, por lo tanto, en mayor
latencia (2,7 s) como se informa en el artículo principal.

4 Estas capas se utilizan en todos los experimentos del artículo principal.

Tabla 8: Precisión en el conjunto de validación NextQA para diferentes selecciones de capas, manteniendo el 6 % de los tokens visuales.| Capas |   Acc. en NextQA-valset (%) ||------------------|-----------------------------|
| Modelo completo |                        78,6 || todas las capas |                        76,8 || [5, 9, 14, 20] 4 |                          77 || [3, 12, 18, 24] |                        76,8 || [1, 10, 15, 25] |                        77,2 || [4, 5, 16, 28] |                        76,7 || [3, 4, 5, 6] |                        75,2 || [13, 14, 15, 16] |                        76,6 || [24, 25, 26, 27] |                        76,6 || [3] |                        73,3 || [15] |                        76,4 || [26] |                        75,7 |Tabla 9: Precisión en el conjunto de validación NextQA para diferentes métodos de agregación de capas de cabezas de atención, manteniendo el 6 % de los tokens visuales.| Método de agregación |   Acc. en NextQA-valset (%) ||---------------------|--------------------------------|| Modelo completo |                        78,6 || Promedio                 |                        77,0 || Máximo |                        77,1 |Tabla 10: Comparación de precisión y puntuación en diferentes contextos de duración en RVS-Movie y RVS-Ego.| Longitud del contexto | RVS-Ego | RVS-Ego | Película RVS | Película RVS | Latencia ||------------------|-----------|-----------|-------------|-------------|-----------|
|                  | Acc.      | Puntuación | Acc.        | Puntuación |           || 20k | 65,3 | 4.0 | 56,0 | 3.5 | 3,9s || 10k | 65,3 | 4.0 | 57,7 | 3.6 | 1,9s || 6k | 62,7 | 3.9 | 57,0 | 3.5 | 1,6s |## E Información posicional

Una posible preocupación con nuestro método propuesto es la manipulación o pérdida
de información posicional espacial durante la selección de token visual. en
rLiVS, conservamos sólo el orden temporal de los tokens seleccionados, potencialmente
alterando su disposición espacial original al reinsertarlos en el
Contexto del LLM. Sin embargo, nuestros experimentos iniciales sugieren que este diseño
La elección no obstaculiza el desempeño. Como se muestra en la Tabla 7, seleccionando solo
19% de los tokens visuales, según nuestra puntuación y preservación de relevancia
orden temporal: logra un rendimiento comparable al uso del conjunto completo de
tokens visuales en el conjunto de validación NextQA. Curiosamente, seleccionando el 25%
de tokens incluso supera la línea base de token completo, lo que indica que
reducir la redundancia y filtrar el contenido menos informativo es más
beneficioso que preservar estrictamente la estructura espacial. este hallazgo
se alinea con observaciones anteriores (por ejemplo,

[13]) que los datos de vídeo contienen una redundancia sustancial, que puede degradar
rendimiento si no se mitiga. Además, dado que los modelos visión-lenguaje
(VLM) están principalmente pre-entrenados para la generación de subtítulos, postulamos que
Aprovechar este objetivo como paso intermedio actúa como un medio eficaz
filtro de ruido. Durante el preentrenamiento, estos modelos aprenden a centrarse en el
tokens visuales más informativos: aquellos que se alinean con la semántica de
subtítulos. Nuestro método se basa directamente en esta propiedad seleccionando
tokens que más contribuyen a la generación de subtítulos, mejorando así
relevancia y reducción del ruido. Finalmente, observamos que al optar por no
abordar el desafío de inyectar con precisión información posicional,
Nuestro método sigue siendo ortogonal a cualquier arquitectura de vídeo-LLM específica.
Desarrollar estrategias mejoradas para retener o reintroducir explícitamente
información espacial y, por lo tanto, modelar mejor la dinámica espacio-temporal
durante el procesamiento, sigue siendo una dirección de investigación abierta, que dejamos para
trabajo futuro.

## F Tokens visuales versus recuperación basada en subtítulos

En nuestros estudios de ablación, sostenemos que los tokens visuales proyectados ad hoc
del codificador visual falta alineación semántica aprendida con el usuario
preguntas. Como resultado, producen puntuaciones de similitud no informativas y
conducir a un comportamiento de recuperación confuso. Por el contrario, los subtítulos están en el
La misma modalidad que las consultas de los usuarios; naturalmente, se encuentran dentro de la misma entrada de LLM.
espacio, haciéndolos más adecuados para su recuperación. La figura 2 ilustra la
distribución de puntuaciones de similitud de cosenos entre tokens visuales y
consultas, así como entre subtítulos y consultas, agregadas en
vídeos y preguntas en RVS-Movie. Observamos que la similitud
la distribución entre tokens visuales y consultas está estrechamente centrada
alrededor de cero, con valores que van desde aproximadamente 0 . 02 a 0. 06.
Por el contrario, las puntuaciones de similitud entre subtítulos y consultas abarcan un
rango mucho más amplio, desde aproximadamente 0 . 4 a 0. 9 -casi una orden de
magnitud más amplia, lo que indica potencialmente una mayor capacidad para distinguir
relevante del contenido irrelevante durante la recuperación.

Para verificar este punto, presentamos resultados de rendimiento tanto en RVS-Ego como en
RVS-Movie utiliza tres métodos de recuperación basados en similitudes: tokens visuales,
subtítulos y selección aleatoria 5 . Como se muestra en la Tabla 11, la recuperación utilizando
Los tokens visuales funcionan a la par con la selección aleatoria, reforzando la
afirman que los tokens visuales proporcionan un valor limitado. Por el contrario, subtítulos
la recuperación basada produce consistentemente mejoras de aproximadamente 2-3%,
subrayando su eficacia.Figura 2: Distribución de similitudes de cosenos entre tokens visuales y consultas (izquierda) y entre subtítulos y consultas (derecha), promediadas sobre videos y preguntas en RVS-Movie.

<!-- imagen -->

## G Nuestra línea de base uniforme frente a la de ReKV [9]

En el artículo principal, comparamos token visual uniforme y basado en la atención.
selección en los benchmarks de streaming RVS-Ego y RVS-Movie, mostrando
que nuestro método basado en la atención supera consistentemente al uniforme
muestreo en aproximadamente 1-2%. En particular, la línea de base uniforme en ese
La comparación se fortaleció con dos componentes clave de nuestro enfoque:
recurrencia y selección de tokens a nivel de clip corto, los cuales
mejorar significativamente el rendimiento. Aquí presentamos un uniforme más simple.
línea de base de muestreo más alineada con la configuración de [9], donde los tokens son
muestreado uniformemente en todo el vídeo. Para adaptar esto a nuestra
marco, seleccionamos uniformemente 32 cuadros globalmente para llenar el contexto,
consistente con la longitud del cuadro que nuestra red troncal de video-LLM fue
entrenado. Como se muestra en la Tabla 12, esta línea base ingenua realiza
significativamente peor (alrededor de un 10 % menos) que nuestro uniforme reforzado
variante, destacando la importancia de nuestras opciones de diseño propuestas.

5 En todos los métodos, se recuperan subtítulos para responder.

Tabla 11: Comparación de precisión y puntuación bajo diferentes estrategias de recuperación (basadas en subtítulos, fichas visuales o aleatorias) en RVS-Movie y RVS-Ego.| Recuperación | RVS-Ego | RVS-Ego | Película RVS | Película RVS ||---------------|-----------|-----------|-------------|-------------|
|               | Acc.      | Puntuación | Acc.        | Puntuación || Aleatorio | 64,6 | 3.9 | 55,2 | 3.5 || Fichas visuales | 63,7 | 3.9 | 55,0 | 3.5 || Subtítulos | 65,3 | 4.0 | 57,7 | 3.6 |Tabla 12: Comparación de la precisión y la puntuación del muestreo uniforme global (marcos de muestreo) frente a nuestra línea base recurrente basada en subtítulos de clips cortos en RVS-Movie y RVS-Ego.| Línea de base | RVS-Ego | RVS-Ego | Película RVS | Película RVS ||--------------------------------------------|-----------|-----------|-------------|-------------|
|                                            | Acc.      | Puntuación | Acc.        | Puntuación || Uniforme mundial | 56,2 | 3.7 | 44.1 | 3.1 || Uniforme recurrente basado en subtítulos de clips cortos | 64,2 | 3.9 | 56,0 | 3.5 |## H Selección basada en la atención según la instrucción de entrada

## Mecanismo central H.1

Proponemos un mecanismo flexible de atención de arriba hacia abajo en el que la entrada
Las instrucciones guían la selección de tokens visuales a través de un título o
paso de generación de respuesta. El pipeline general está estructurado de la siguiente manera:

Instrucción → Título/respuesta generado → Token basado en atención
Selección

Los tokens se seleccionan en función de sus puntuaciones de atención durante el texto.
generación, alineándose con la relevancia semántica específica de la tarea. como
discutido, este mecanismo se inspira en los sistemas de memoria humanos,
donde la atención está modulada por objetivos impulsados por tareas en lugar de uniformes
procesamiento en todas las entradas.

Si bien trabajos anteriores han aprovechado las puntuaciones de atención para la reducción de tokens [13,
26, 37], estos enfoques normalmente operan dentro de capas individuales,
tokens de poda basados en la atención localizada durante el procesamiento temprano
etapas. Por el contrario, nuestro método introduce varias innovaciones clave.
Primero, calculamos la agregación de atención entre capas, identificando tokens.
que reciben constantemente una alta atención en múltiples capas. En segundo lugar,
Aplicamos una retención escasa de tokens y mantenemos solo un pequeño subconjunto de elementos visuales.
tokens (aproximadamente 6%) según las puntuaciones agregadas. En tercer lugar, a través de
reutilización recurrente de tokens, los tokens seleccionados se propagan a través
segmentos de vídeo cortos sucesivos, lo que permite la continuidad temporal sin
cualquier modelo de puesta a punto. Este diseño no sólo reduce el coste computacional
pero también preserva la coherencia semántica entre los clips, facilitando
razonamiento en vídeo eficiente y contextual.

## H.2 Adaptabilidad de tareas

Aunque nuestros experimentos principales se centran en la selección de tokens guiada por subtítulos
para la respuesta visual a preguntas (VQA), el marco propuesto es
inherentemente adaptable a la instrucción y capaz de generalizar a través de un
Variedad de tareas de comprensión de vídeo mediante la modificación de la entrada.
instrucción. Por ejemplo:

- Seguimiento de personas: 'Sigue el movimiento de la persona con la camiseta roja a lo largo de esta secuencia de vídeo'.

- Reconocimiento de acciones: 'Identificar y describir los movimientos atléticos específicos realizados'.
- Detección de objetos: 'Localiza y describe todos los vehículos que aparecen en esta escena'.

Cada instrucción específica de la tarea provoca distintos resultados del modelo, lo que lleva
a patrones de atención únicos y selecciones de tokens que reflejan la
prioridades semánticas de la tarea. Para evaluar cualitativamente este comportamiento,
Realizamos un análisis comparativo de las superposiciones de selección de tokens en
instrucciones variables.

Estudio de caso: Sensibilidad de la instrucción Seleccionamos dos representantes
videos y evaluó la superposición de tokens visuales seleccionados usando un
Mensaje de subtítulos genérico versus instrucciones específicas de la tarea:

- Vídeo 1: Una chica con gafas de sol es el objeto principal en primer plano.
- -Instrucción: 'Rastree las ubicaciones de la chica que usa gafas de sol en el video'.
- -Superposición de tokens con aviso genérico: 44%
- Vídeo 2: Dos personas jugando a las cartas, con el interés centrado en el fondo.
- -Instrucción: 'Ubica y describe los objetos que aparecen en el fondo del vídeo.'
- -Superposición de tokens con aviso genérico: 8%

El mensaje genérico utilizado en ambos casos fue: 'Describe lo que está sucediendo en
el vídeo", consistente con el utilizado durante los subtítulos principales
experimentos. Los resultados demuestran que la selección de tokens es altamente
sensible a la instrucción dada, validando la adaptabilidad dinámica
de nuestro mecanismo basado en la atención.

## I Tarea de resumen de video largo

En esta sección, llevamos a cabo una evaluación preliminar de la eficacia de nuestro método.Generalización de tareas a través de largos resúmenes en vídeo. Usando el MLVU
división de validación [39], calculamos la puntuación holística del resumen de vídeo
y compare nuestro enfoque con Video-XL [27], un programa reciente basado en capacitación
modelo de comprensión de transmisión de video que también emplea
mecanismos. Como se muestra en la Tabla 13, rLiVS supera a Video-XL en términos holísticos.
calidad del resumen.

Tabla 13: Puntuaciones de resumen de vídeo holístico en MLVU (validación).| Método | Tamaño del modelo |   Puntuación de resumen de MLVU ||----------------------|--------------|----------------------|| Vídeo-XL | 7B |                       3.40 || rLiVS (Nuestro) | 7B |                       3,65 |Estos resultados demuestran que rLiVS no solo iguala sino que supera
arquitecturas especializadas y entrenadas como Video-XL sin dejar de ser
sin formación y fácilmente extensible. Incluimos una más detallada
desglose de esta comparación en la versión revisada del artículo.

## J Análisis cualitativo

Evaluar la efectividad de nuestra agresiva compresión de tokens en
escenarios complejos y dinámicos, realizamos una evaluación cualitativa sobre
un subconjunto de videos del conjunto de datos SportsQA [18]. aunque lleno
Los resultados cuantitativos están pendientes debido a restricciones de acceso al conjunto de datos.
compare los resultados de los subtítulos generados utilizando nuestro método (6% de retención de tokens)
contra los generados utilizando todos los tokens visuales.

A pesar de la reducción significativa en el recuento de tokens, el modelo conserva su
capacidad de producir descripciones semánticamente ricas y detalladas, estrechamente
coincidiendo con la línea de base del token completo. A continuación se muestran ejemplos seleccionados de la
categoría de gimnasia aeróbica:

- Completo: 'El vídeo muestra una rutina de gimnasia sincronizada realizada por un grupo de atletas en el X Campeonato Europeo de Gimnasia Aeróbica.'
- 6% : 'El vídeo trata sobre una rutina de gimnasia realizada por el equipo italiano en el décimo Campeonato de Europa.'

- Completo: 'El vídeo muestra una actuación de gimnasia rítmica de tres atletas.' 6% : 'El vídeo muestra una rutina de gimnasia rítmica realizada por tres atletas.'
- Completo: 'El vídeo muestra una rutina de gimnasia sincronizada realizada por dos atletas en el V Campeonato Asiático de Gimnasia Aeróbica.'
- 6%: 'El vídeo trata sobre dos gimnastas realizando una rutina sincronizada en el V Campeonato Asiático de Gimnasia Aeróbica.'
- Completo: 'El vídeo trata sobre una actuación de gimnasia rítmica realizada por dos atletas en la séptima Competición Mundial de Grupos de Edad de Aeróbic de la FIG en Incheon, Corea.' 6%: 'El vídeo muestra una rutina de gimnasia rítmica realizada por dos atletas.'

Estos ejemplos indican que nuestro modelo mantiene una alta fidelidad semántica,
incluso bajo compresión agresiva y se generaliza bien a cambios rápidos.
escenas con múltiples actores y acciones detalladas. Esto sugiere la
viabilidad de nuestro enfoque de selección de tokens fijo pero eficiente en
dominios de alta complejidad como los deportes.

## K Plantillas de mensajes

Mostramos aquí las indicaciones que utilizamos para la generación de subtítulos y para el LLM.
evaluación:

Generación de subtítulos. "Describe lo que está sucediendo en el video".

LLMevaluación. Para ello, seguimos trabajos anteriores [38, 9] y utilizamos
la siguiente plantilla de uso común:```
messages=[ { "role": "system", "content": "You are an intelligent chatbot designed for evaluating the correctness of generative outputs for question-answer pairs. " "Your task is to compare the predicted answer with the correct answer and determine if they match meaningfully. Here's how you can accomplish the task:" "---" "##INSTRUCTIONS: " "-Focus on the meaningful match between the predicted answer and the correct answer.\n" "-Consider synonyms or paraphrases as valid matches.\n" "-Evaluate the correctness of the prediction compared to the answer." }, { "role": "user", "content": "Please evaluate the following video-based question-answer pair:\n\n" f"Question: {question}\n" f"Correct Answer: {answer}\n" f"Predicted Answer: {pred}\n\n" "Provide your evaluation only as a yes/no and score where the score is an integer value between 0 and 5, with 5 indicating the highest meaningful match. " "Please generate the response in the form of a Python dictionary string with keys 'pred' and 'score', where value of 'pred' is a string of 'yes' or 'no' and value of 'score' is in INTEGER, not STRING." "DO NOT PROVIDE ANY OTHER OUTPUT TEXT OR EXPLANATION. Only provide the Python dictionary string. " "For example, your response should look like this: {'pred': 'yes', 'score': 4.8}." } ]
```
## L Impacto más amplio

El impacto más amplio de nuestro enfoque se puede discutir desde tres puntos principales.
aspectos: el impacto ambiental, la accesibilidad y la ética.
consideración. Nuestro método reduce significativamente los costos computacionales y
consumo de energía, reduciendo posteriormente la huella de carbono
asociado con la comprensión de videos largos. Junto con la eficiencia, nuestra
Este enfoque hace que las respuestas a las preguntas en streaming sean fácilmente accesibles para
profesionales e investigadores con recursos limitados. Sin embargo, como en el
En el caso de la mayoría de las investigaciones sobre IA, nuestro enfoque puede potencialmente ser mal utilizado, y
es crucial garantizar una aplicación responsable. Como tal, prevenir
su uso en vigilancia no autorizada o violaciones de la privacidad es importante.
En general, desarrollar directrices y regulaciones integrales sobre la
El uso ético de grandes modelos de lenguaje de vídeo es esencial para mitigar
estos riesgos.