## TRANSMISIÓN DE VIDEO COMPRENSIÓN E INTERACCIÓN MULTIROUND CON CONOCIMIENTO MEJORADO EN LA MEMORIA

Haomiao Xiong 1 ∗ , Zongxin Yang 2 ∗ , Jiazuo Yu 1 , Yunzhi Zhuge 1 Lu
Zhang 1, Jiawen Zhu 1, Huchuan Lu 1 1

Universidad Tecnológica de Dalian, 2 Universidad de Harvard

## RESUMEN

Los avances recientes en modelos de lenguajes grandes (LLM) han permitido
desarrollo de Video-LLM, avanzando en el aprendizaje multimodal uniendo
datos de vídeo con tareas de lenguaje. Sin embargo, la comprensión actual del vídeo
Los modelos tienen dificultades para procesar largas secuencias de vídeo, admitir múltiples
convertir diálogos y adaptarse a escenarios dinámicos del mundo real. para dirigirse
estos temas, proponemos STREAMCHAT, un marco sin formación para
streaming de vídeo razonamiento e interacción conversacional. TRANSMITIR
aprovecha un novedoso sistema de memoria jerárquica para procesar y procesar eficientemente
comprimir funciones de vídeo en secuencias extendidas, lo que permite
Diálogo de varios turnos. Nuestro framework incorpora un sistema paralelo
estrategia de programación que mejora la velocidad de procesamiento y reduce la latencia,
asegurando un rendimiento sólido en aplicaciones del mundo real. Además, nosotros
presenta STREAMBENCH, un punto de referencia versátil que evalúa el streaming
Comprensión de vídeo en diversos tipos de medios e interactivos.
escenarios, incluidas interacciones de múltiples turnos y razonamiento complejo
tareas. Evaluaciones exhaustivas sobre STREAMBENCH y otros puntos de referencia públicos
demostrar que STREAMCHAT supera significativamente el estado de
modelos de última generación en términos de precisión y tiempos de respuesta, lo que confirma su
efectividad para la comprensión de la transmisión de video. El código está disponible en
StreamChat.

## 1 INTRODUCCIÓN

Los avances recientes en modelos de lenguaje grande (LLM) [1-3] han llevado a
el desarrollo de VideoLLM [4-9], cuyo objetivo es interpretar imágenes
escenas, acciones y narrativas. Estos modelos representan importantes
Progreso en el aprendizaje multimodal al unir datos de video y lenguaje.
Tareas basadas en tareas, con aplicaciones que van desde el análisis de contenido hasta la gestión humana.
interacción con robots [9].

A pesar de estos avances,

procesa principalmente videos como clips estáticos de modelos fuera de línea actuales y confía
en

Diálogos de un solo turno, que incorporan información visual.

a través de mecanismos como capas de proyección [4, 8] o

estructuras de atención cruzada [2].

Sin embargo, estos modelos

encuentran cuellos de botella computacionales al manejar

Secuencias de vídeo cuidadas, que a menudo tienen dificultades para comprimirlas.

Figura 1: Comparación de rendimiento entre STREAMCHAT y Video-LLM anteriores.

<!-- imagen -->

Funciones de vídeo de larga duración con recursos de memoria limitados [10].
Además, su incapacidad para soportar diálogos de varios turnos reduce
adaptabilidad para escenarios interactivos, y se puede perder información clave
debido a métodos insuficientes de muestreo de video (ver Fig. 2 (a)).

Para abordar estos problemas, han surgido modelos en línea [10, 11]. ellos
utilizar enfoques basados en la memoria e instrucción alineada temporalmente.
sintonización para procesar videos largos y permitir interacciones de múltiples rondas

∗ Igual contribución.

† Autor correspondiente (zgyz@dlut.edu.cn).

†

Figura 2: Comparaciones entre StreamChat y otros métodos (§1). Los métodos sin conexión procesan vídeos completos, lo que provoca pérdida de información y se limita a una sola interacción. Los métodos en línea anteriores [10, 11] permiten interacciones de múltiples rondas pero aún sufren de un procesamiento lento y responden correctamente. El método propuesto logra el procesamiento de video en tiempo real, mejorando la eficiencia y precisión con soporte de memoria.

<!-- imagen -->(ver Fig. 2(b)). Si bien estos modelos permiten interacciones dinámicas del usuario,
Todavía enfrentan desafíos para mantener velocidades de procesamiento rápidas y
desempeño consistente en escenarios desconocidos: factores críticos en
Aplicaciones en tiempo real como navegación robótica y robot humano.
colaboración.

Para superar estas limitaciones, proponemos STREAMCHAT, un programa sin formación
marco para la comprensión de la transmisión de video que ofrece tres claves
innovaciones: (i) Adaptabilidad sin formación, lo que le permite
Procese videos de varios tipos y duraciones sin consumir muchos recursos.
entrenamiento. Esto hace que STREAMCHAT sea adecuado tanto para online como offline.
procesamiento de video mientras se mantiene un rendimiento estable en diversos
escenarios. (ii) Almacenamiento de memoria jerárquica, que gestiona y
comprime la información del vídeo en secuencias largas. Integra cortos
memoria a término para rastrear eventos en curso, memoria a largo plazo para retener
eventos pasados en forma comprimida y memoria de diálogo para mantener
historia conversacional, asegurando un diálogo continuo y coherente
comprensión. (iii) Programación optimizada del sistema, que mejora el modelo.
eficiencia de inferencia al procesar tareas en paralelo en tres
Hilos: el hilo de apilamiento selectivo de marcos identifica y elimina
Marcos redundantes, el hilo de formación de memoria actualiza y refina la memoria.
información, y el hilo de resumen contextual maneja el usuario
solicita y genera respuestas en tiempo real.

Evaluamos STREAMCHAT según los puntos de referencia existentes [12-16] e identificamos sus
dos deficiencias importantes: (i) Contenido de vídeo corto y monótono, que
no logra capturar la complejidad de los medios de transmisión del mundo real, y (ii)
Preguntas simplistas de una sola ronda, que no prueban la eficacia del modelo.
capacidad para entablar un diálogo de múltiples turnos o un razonamiento complejo.

Para abordar estas deficiencias, presentamos STREAMBENCH, un
punto de referencia integral diseñado para la comprensión de la transmisión de video. eso
Incluye una amplia gama de contenido de vídeo, como vídeos egocéntricos, web.
vídeos y escenas de películas, combinados con anotaciones de texto que simulan
interacciones de múltiples rondas. En términos de selección de videos, realizamos
curación manual rigurosa de grandes conjuntos de datos para garantizar alta calidad
contenido y una amplia gama de categorías. En términos de preguntas, nosotros
diseñar seis tipos distintos de consultas, sondeando varias dimensiones de la
capacidades de razonamiento del modelo, desde la simple recuperación de hechos hasta la compleja
inferencia. En comparación con puntos de referencia anteriores, STREAMBENCH no sólo
evalúa la precisión de las respuestas del modelo pero también incorpora latencia
Métricas, que son esenciales para evaluar el rendimiento en tiempo real.
aplicaciones. Este marco de evaluación integral ofrece una visión más
medida realista y confiable de la robustez del modelo y práctica
utilidad.

En resumen, nuestras contribuciones clave son las siguientes:

- Proponemos STREAMCHAT, un método sin formación para la comprensión del vídeo en streaming. Su novedosa estrategia de programación del sistema y almacenamiento de memoria jerárquica permite una gestión sólida de la memoria, procesamiento de vídeo en tiempo real y capacidades de interacción de múltiples rondas. Estas características garantizan una generación de respuestas precisa y eficiente, atendiendo a la naturaleza dinámica de los contextos de vídeo.
- Presentamos STREAMBENCH, el primer punto de referencia integral para evaluar modelos de comprensión de transmisión de video. Este punto de referencia simula interacciones del mundo real a través de diálogos de múltiples turnos y diversos formatos de preguntas, ofreciendo una evaluación detallada del rendimiento del modelo.- STREAMCHAT establece nuevos puntos de referencia (ver Fig. 1), ofreciendo una precisión del 64,7% en STREAMBENCH para configuraciones en línea, lo que representa una mejora del 8,3% con respecto a la mejor marca anterior. En escenarios fuera de línea, supera al método de última generación en un promedio del 2,5% en cuatro puntos de referencia públicos.

Figura 3: Descripción general del punto de referencia (§2). Nuestro punto de referencia cubre 4 dominios clave y 16 tipos de videos de subclases. Estos videos exhiben una distribución más amplia de duración, con 6 tipos diferentes que están distribuidos uniformemente.

<!-- imagen -->

- En términos de eficiencia, STREAMCHAT alcanza una velocidad de procesamiento de 32 FPS, seis veces más que los métodos existentes. Además, mantiene una latencia de generación de texto inferior a 0,9 segundos, lo que muestra avances significativos en el procesamiento de vídeo interactivo.

## 2 COLECCIÓN Y COMPOSICIÓN DE STREAMBENCH

## 2.1 COLECCIÓN DE VIDEOS

Los puntos de referencia de comprensión de videos anteriores [12-14, 1618] se centran principalmente en
escenarios fuera de línea, donde se muestran todos los fotogramas de vídeo y las preguntas de los usuarios.
proporcionado al modelo simultáneamente para generar respuestas. para encontrar un
método más adecuado para evaluar la capacidad de un modelo para comprender en línea
escenarios, presentamos STREAMBENCH, un punto de referencia diseñado principalmente para
simular escenarios de vídeo en línea. Hay dos diferencias distintas
comparado

Tabla 1: Comparaciones de diferentes puntos de referencia. La resonancia magnética denota interacciones de múltiples rondas.| Punto de referencia | SEÑOR | Promedio | Totales | Vídeo | Control de calidad ||------------------|------|-------|---------|---------|----------|
| MSVD [16] | ✗ | 10 | 1,4h | Web | Desc.    || MSRVTT [12] | ✗ | 15 | 12,5h | Web | Desc.    || ActividadNet [14] | ✗ | 112 | 25h | Web | Desc.    || Siguiente control de calidad [15] | ✗ | 40 años | 11h | Web | temporales || PelículaChat [7] | ✗ | 213 | 9h | Película | Película || BANCO DE ARROYO | ✓ | 270 | 25h | Mezclar | En línea |con otros puntos de referencia de comprensión de vídeo. (i) Curación de videos diversos:
Recopilamos cuatro dominios principales y dieciséis subclases de fuentes de vídeo.
incluyendo videos egocéntricos, videos web, videos de trabajo y películas como
la base de datos del índice de referencia. Cada tipo tiene sus características únicas.
y desafíos, que pueden verificar la estabilidad y confiabilidad del
modelo en una amplia gama de escenarios de aplicación. (ii) Tipos de consultas elaboradas
: diseñamos seis tipos de preguntas para satisfacer las necesidades específicas de los usuarios en línea
comprensión del vídeo y garantizar que este tipo de preguntas aparezcan una vez
en un solo vídeo, formando un diálogo de múltiples rondas. Esta sección
presenta cómo recopilamos videos y construimos anotaciones. Más detalles
sobre la diversidad y distribución de nuestro punto de referencia se muestran en la Fig.
3.

Fuentes de datos. Al seleccionar videos para nuestros puntos de referencia, priorizamos
diversidad en tipo y longitud para mantener una alta calidad de los datos. nuestra primaria
Las fuentes son los conjuntos de datos EgoSchema [13] y YouTube-8M [19]. Esquema del ego
ofrece una rica variedad de escenas tanto interiores como exteriores, proporcionando una
amplia gama de perspectivas y acciones egocéntricas, que alinea
perfectamente con nuestras necesidades experimentales. De YouTube-8M, que presenta un
colección completa obtenida en Internet que abarca más de 4000 clases,
filtramos para obtener contenido web, relacionado con el trabajo y cinematográfico de alta calidad.
vídeos. Este marco de selección diverso garantiza que nuestro modelo sea probado.
frente a un amplio espectro de escenarios del mundo real.

Filtrado de vídeos. Es un paso crucial para garantizar la calidad de la
vídeos utilizados en el punto de referencia. Para lograrlo, nuestro filtrado de datos
La canalización consta de selección de máquinas y humanos. En primer lugar, un multimodal
El modelo de lenguaje [20] se utiliza para clasificar los datos originales. el
Las categorías de videos se proporcionan por fuente de datos, las alimentamos con la
videos a la máquina y haga que seleccione la categoría del video.
En segundo lugar, se requiere juicio humano para evaluar la redundancia: la
cambio de escenas en videos. Algunos contenidos de vídeo estáticos (p. ej., visión del ego de
dibujar, mirar televisión) y datos de alto ruido de vídeos web (por ejemplo, vídeos
juegos, anuncios) se eliminan según el criterio humano.

Finalmente, StreamBench consta de 306 vídeos con una duración total de
24,8 horas y una media de 4,5 minutos cada uno, ofreciendo una completa
Colección de vídeos de diferentes categorías y duraciones.

## 2.2 CONSTRUCCIÓN DE TAREAS Y ANOTACIONES

Hemos elaborado seis tareas distintas con anotaciones para simular el
conversación entre el agente y el humano. Cada tarea corresponde a una
escenario diferente del mundo real, asegurando una cobertura integral de
contextos de comunicación potenciales.

- Búsqueda de objetos (OS): los desafíos incluyen describir con precisión la posición de un objeto en un video. Las condiciones de la tarea son que el objeto debe aparecer durante menos de 5 segundos y el intervalo desde su aparición hasta la solicitud del usuario debe exceder los 30 segundos, lo que aumenta la dificultad de la búsqueda.
- Búsqueda de memoria a largo plazo (LM): esta tarea evalúa la memoria del modelo al requerir el recuerdo de eventos que aparecen durante más de 5 segundos, con un retraso superior a 1 minuto desde el final del evento hasta la consulta del usuario, probando la retención de la memoria a largo plazo.
- Búsqueda de memoria a corto plazo (SM): para simular el interés del usuario en eventos recientes, esta tarea establece el intervalo desde la finalización del evento hasta la consulta del usuario en menos de 20 segundos, evaluando la respuesta del modelo a las actividades recientes.- Interacción conversacional (CI): a veces la respuesta a la pregunta actual de un usuario está estrechamente relacionada con el historial de conversaciones. Por tanto, el modelo debe memorizar registros de conversaciones y recuperar el texto más relevante de la memoria como soporte contextual. Este tipo está diseñado para simular escenarios de diálogo de varios turnos. Configuramos la información del diálogo asociada con la solicitud actual del usuario para que provenga de cualquier conversación anterior, con un intervalo de más de 2 diálogos.
- Respuesta a preguntas basada en el conocimiento (KG): este tipo de preguntas evalúa el conocimiento interno del modelo, que es retenido por los modelos de lenguaje grandes base. En este punto de referencia, establecemos que las preguntas deben estar relacionadas con los eventos u objetos que ocurren en el video para que pueda simular escenarios donde los usuarios tienen una necesidad específica de comprender conocimientos previos o enciclopédicos.
- Simple Factual (SF): Este tipo de preguntas se centra en un diálogo amistoso que se inicia entre el usuario y el modelo. Por lo tanto, deben solicitarse dentro de los 30 segundos posteriores al inicio del vídeo. Aunque la pregunta es sencilla, el modelo necesita recordar cosas a corto plazo para responder correctamente.

Para garantizar la calidad de las anotaciones, asignamos adicionalmente
diferentes trabajadores para realizar comentarios humanos para la anotación manual. el
El paso de retroalimentación humana debe centrarse en tres partes: (1) comprobar la
los formatos de las preguntas sean correctos y diversos, (2) asegúrese de que las expresiones sean
claro y consistente con el video, y (3) eliminar temas sensibles
como aquellas cuestiones relacionadas con la nacionalidad o la política. Estos pasos
de anotaciones y comentarios manuales, junto con lenguaje multimodal de gran tamaño
colección de videos asistida por modelos, desde nuestro punto de referencia semiautomático
tubería de construcción (Apéndice § A). Finalmente, STREAMBENCH contiene 1,8K
pares de control de calidad de alta calidad. La distribución de estas anotaciones se muestra en
Fig. 3. Algunos ejemplos del benchmark que ofrecen una visión intuitiva
La observación de nuestros resultados y formatos de anotaciones se muestra en el Apéndice. §B.

## 3 TRANSMISIÓN

Dada la transmisión de video en tiempo real y preguntas con marca de tiempo como entrada,
STREAMCHAT está diseñado para realizar razonamientos y entregar de manera eficiente
respuestas precisas en múltiples rondas. Aprovechando LongVA [20]
(Apéndice § F) como Video-LLM fundamental, nuestro diseño incorpora dos
componentes clave: un sistema de almacenamiento de memoria jerárquico (§ 3.1) que
Aprovecha los recuerdos a largo plazo, a corto plazo y de diálogo para comprimir y
gestionar extensas secuencias de vídeo con recursos limitados,
facilitar el razonamiento eficaz del contenido de vídeo; y un sistema de programación
estrategia (§ 3.2) que desacopla la extracción de características de vídeo de la memoria
actualizaciones, evitando así el crecimiento ilimitado del buffer a medida que el vídeo de entrada
los marcos aumentan.

Tabla 2: Comparaciones de videos recientes. Nuestro método de streaming (S.) con memoria (M.) logra procesar video en tiempo real (R.) y genera una respuesta con baja latencia (L.).

<!-- imagen -->

Figura 4: Descripción general de StreamChat (§3), que comprende tres componentes principales: (i) Apilamiento selectivo de cuadros, que prepara las características de visión para el procesamiento, incluida la codificación de cuadros y el llenado del búfer de visión; (ii) Formación de la memoria, donde las características de la visión se organizan en una memoria estructurada; (iii) Resumen contextual, que utiliza memoria jerárquica para responder a las consultas de los usuarios proporcionando un contexto relevante.

<!-- imagen -->

## 3.1 ALMACENAMIENTO DE MEMORIA JERARQUICA

STREAMCHAT trata los vídeos como depósitos de información dinámicos, utilizandoMemoria jerárquica para analizar y almacenar los diversos contenidos. esto
La sección detalla dos estructuras de memoria especializadas diseñadas para abordar la
Desafíos del almacenamiento y recuperación de información: memoria a largo y corto plazo.
M l ∪ M s = { l i } T/L i =0 ∪ { s i } S i =0 y memoria de diálogo M d =
{ re yo } re yo = 0 . Estas memorias gestionan datos visuales y conversacionales,
donde T es la duración del vídeo, S es la duración de la memoria corta, D cuenta los diálogos,
y L es el tamaño del fragmento para memoria larga. Las siguientes secciones
Introduzca las funciones de los parámetros anteriores.

## 3.1.1 MEMORIA A LARGO-CORTO PLAZO

Apilamiento selectivo de cuadros. Para reducir la sobrecarga de almacenamiento de funciones causada
Para fotogramas redundantes en vídeos, utilizamos Lucas-Kanade Optical Flow.
algoritmo [21] en el módulo de apilamiento selectivo de marcos para ayudar en
determinar la validez de cada cuadro de video { F i ∈ R H × W × 3 } T i
=0. Específicamente, calculamos el vector de movimiento (u, v) entre i -ésimo
fotogramas F i y el último fotograma F i -1 :

donde I x ( i ) , I y ( i ) , It ( i ) representan el parcial
derivadas del marco F i con respecto a la posición i ( x, y ) y
tiempo t. Desarrollamos la magnitud del vector de movimiento || θ || = √ tu 2 + v 2 ∈
[0, 1] para representar la intensidad total del movimiento entre fotogramas. Si || θ|| excede el umbral predefinido t ∈ [0 , 1] , la trama F i serácodificado en la visión incorporando e i ∈ R n × d y empujado al búfer B
visión.

<!-- fórmula-no-decodificada -->

Memoria a corto plazo. Pretendemos diseñar un método de memoria similar al humano que
simula el modelo de Atkinson-Shiffrin [22], que enfatiza el papel de
un almacenamiento a corto plazo para mantener fácilmente accesible, frecuentemente
información actualizada. Específicamente, como se muestra en la Fig. 5 (a), seleccionamos N
Incorporaciones de visión de la visión B como candidatos de visión C. Construyendo sobre
Según la teoría de la curva del olvido de Ebbinghaus [23], manejamos las actualizaciones de la memoria mediante
seleccionando aleatoriamente las incrustaciones de visión S e i de C para construir el
memoria a corto plazo M s :

<!-- fórmula-no-decodificada -->

<!-- fórmula-no-decodificada -->

Memoria a largo plazo. La memoria a largo plazo simula el complejo y
memoria abstracta de los humanos [22]. Por este motivo, diseñamos dos formas de
información en la memoria a largo plazo: pistas de texto, que se utiliza para almacenar
texto declarativo t i que describe eventos que ocurrieron durante un período pasado,
y memoria de visión, que se utiliza para almacenar características visuales comprimidas v
yo ∈ R C × re . Las pistas de texto sirven como índice para recuperar información relevante.
información de la memoria a largo plazo (introducida en §3.1.3). Nuestro sistema

donde σ i es la probabilidad de olvido normalizada de la i -ésima unidad de C,
S representa la longitud.

(a) Memoria a largo plazo

<!-- imagen -->

(b) Memoria del Diálogo

Figura 5: El almacenamiento de memoria jerárquica (§3.1). (a) Largo-corto plazo
memoria, donde el árbol de memoria larga M l y la memoria a corto plazo M s son
construido a lo largo de la línea de tiempo del vídeo. (b) La memoria de diálogo M d es
actualizado después de cada conversación de inferencia para gestionar el diálogo
historias.

supera el cuello de botella del consumo de VRAM y los desafíos de
recuperar unidades de memoria l i mediante la construcción de una estructura de árbol como se muestra en
Figura 5 (a).

La construcción del árbol de la memoria a largo plazo se puede esbozar en el
siguientes pasos: En primer lugar, el búfer de visión se fragmenta y cada fragmento
está agrupado y se le asigna un título:

<!-- fórmula-no-decodificada -->

donde v i ∈ R C × d es el i -ésimo grupo formado a partir de fragmentos de características {K i
} T/L i =0, C son los objetivos de agrupación y ti representa el i -ésimo
título de cada fragmento. Cada fragmento K i contiene L características e i ∈ R n × d
que provienen del buffer B vision. A continuación, una característica agrupada vi y a
título t i juntos forman una unidad de memoria larga l i, que también sirve como
Los nodos básicos de nuestra estructura de árbol:

<!-- fórmula-no-decodificada -->

Finalmente, los nodos básicos se agrupan en nodos de nivel superior [ l 1 0
, l 1 1 , . . . , l 1 k ] en orden cronológico hasta obtener una estructura de árbol
Se forma y todos los nodos básicos están agotados:

<!-- fórmula-no-decodificada -->

## 3.1.2 MEMORIA DEL DIÁLOGO

En nuestro enfoque, cada ronda de preguntas { Q i } D i =0 y respuesta { A i
} D i =0 se ve como un fragmento de memoria, que está precodificado por el
modelo de codificador E ( · ) en una representación contextual d i . Así, el
Todo el historial de diálogo M d está precodificado como se muestra a continuación.
fórmula: M d =

{ re 0 , re 1 , . . . , d i -1 } empujar ←- -d i = E ( &lt; Q i , A i &gt; ) donde
la longitud de M d es igual al número de conversación D , y seleccionamos
MiniLM-L6 [24] como nuestro modelo de codificador.

## 3.1.3 RECUPERACIÓN

Cuando llega una pregunta de usuario Q i, el sistema de memoria buscará la
conocimiento más relevante como complementación por el algoritmo de recuperación.
En el árbol de memoria larga M l, las unidades de pistas Q i y de texto { t i } T/L i =0
están codificados por el tokenizador y la capa de incrustación de LLM. Residencia enla similitud del coseno entre el Qi codificado y la pista del texto ti, la memoria
el sistema buscará los tokens recuperados M s ∪ { v r ∈ R C × d } L r
=0 donde L representa el número de capa de M l. En la memoria de diálogo M d ,
el usuario solicita Q i está codificado por E ( · ) como una consulta para buscar el
contexto &lt; Q recuperada, A recuperada &gt; basado en el índice FAISS [25].
Se muestran más detalles sobre nuestro algoritmo de recuperación en Appen. §C.

## 3.2 PROGRAMACIÓN DEL SISTEMA

Como se muestra en la Fig. 4, nuestro método incluye tres partes diferentes: selectiva
apilamiento de cuadros, formación de memoria y resumen contextual. Estos
Los componentes funcionan como subprocesos independientes para optimizar la inferencia.
velocidad y minimizar la latencia. La programación del sistema es crucial ya que permite
ejecución simultánea de estos hilos sin interferencias,
mejorando significativamente la velocidad de procesamiento.

Específicamente, el hilo de apilamiento de marcos selectivos (i) llena activamente
la visión amortigua la visión B con características e i . Una vez llenos, estos
las características se borran del búfer y se pasan a la (ii) memoria
hilo de formación, que actualiza las estructuras de la memoria construyendo el
árbol de memoria a largo plazo M l y refrescar la memoria a corto plazo M s .
Al mismo tiempo, los registros de diálogo anteriores ( &lt; Qi -1 , A i -1 &gt; ) son
almacenado en la memoria de diálogo M d . Al recibir una nueva consulta Q i , el
(iii) el hilo de resumen contextual recupera información relevante
de la memoria jerárquica para brindar respuestas oportunas. esto
La arquitectura admite latencia inferior a un segundo (&lt;0,9 s) y procesamiento de vídeo de hasta
a 32 FPS.

## 4 EXPERIMENTOS

## 4.1 CONFIGURACIÓN EXPERIMENTAL

Versión t L g C Lenta 0,13 35 15 5 Base 0,35 25 10 5 Rápida 0,58 30 15 5
escenarios, configuramos tres versiones con diferentes configuraciones de memoria:
Estas variantes ajustan los parámetros clave de la memoria,

Configuraciones de memoria. Para adaptar el modelo a varias aplicaciones Base,
Rápido y Lento. incluyendo umbral (t), longitud del fragmento (L), grupo
tamaño ( g ) y objetivos de agrupación ( C ), como se resume en la Tab. 3. El
El modelo rápido está optimizado para

Tabla 3: Configuraciones de memoria para tres modelos.| Versión |    t |   L |   gramo |   C ||-----------|------|-----|-----|-----|
| Lento | 0,13 |  35 |  15 |   5 || Base | 0,35 |  25 |  10 |   5 || Rápido | 0,58 |  30 |  15 |   5 |procesamiento rápido de vídeo, mientras que el modelo Lento prioriza la precisión en
respuestas. El modelo Base equilibra la velocidad de procesamiento y la precisión.

Métricas de Evaluación. Evaluamos la similitud semántica en un solo
conversaciones utilizando el modelo LLaMA-3 [3], que asigna una
puntuación de corrección (Sco.) que va desde [0, 5], donde las puntuaciones más altas
reflejan respuestas que se alinean más estrechamente con las respuestas esperadas. Para
Al evaluar la coherencia en diálogos de varios turnos, calculamos la puntuación.
fluctuaciones entre giros; fluctuaciones más pequeñas (Coh.) indican una
experiencia de diálogo más fluida. Además, medimos la solicitud.
retraso de procesamiento (RPD), definido como el tiempo (en segundos) desde que el usuario
envío de la solicitud hasta el inicio de la generación de respuesta. Un RPD más pequeño
significa una latencia más baja, lo que resulta en tiempos de espera reducidos para los usuarios.
Apéndice. El § D ofrece más detalles.

Detalles de implementación. Utilizamos CLIP-L-P14 [26] como codificador de visión.
y establecemos el número de unidades de memoria seleccionadas S en 5 y el candidato
longitud C a 20. Los experimentos se realizaron en dos GPU NVIDIA Tesla A800
con 80 GB de memoria cada uno (más detalles en el Apéndice § F). Comparamos nuestros
modelo frente a métodos de última generación, incluido Video-LLaVA [4],
LLaMA-VID [2] y etc.

## 4.2 COMPARACIÓN CON MÉTODOS DE ÚLTIMA GENERACIÓN

Escenarios en línea. Como se muestra en la pestaña. 4, nuestros modelos demuestran importantes
mejoras con respecto al mejor método anterior, Videoonline [11].

- Lento: Consigue una precisión un 8,3% mayor y una puntuación un 0,37 mayor que Vídeo-online.
- Rápido: Procesa vídeo a 32 FPS, lo que lo hace mucho más rápido que todos los métodos de transmisión anteriores, al mismo tiempo que mejora la precisión en un 5,3% y obtiene una puntuación 0,17 mayor que [11].
- Base: Alcanza 63,8% Acc. y puntuación de 3,42.
- Mejor modelo: supera a [11] con una mejora de 0,18 en la puntuación de coherencia y reduce la latencia en 0,17 s, lo que ofrece conversaciones más fluidas con tiempos de espera más cortos.

Debido a la programación del sistema, todos los modelos mantienen casi la misma respuesta.
tiempo de aproximadamente 0,9 s. Pestaña. 5 presenta las puntuaciones detalladas en seis
tipos de preguntas. Utilizando almacenamiento de memoria jerárquico, nuestro método sobresale en
objeto

Tabla 4: Resultados cuantitativos en StreamBench. El RPD se mide para los métodos de transmisión. Fr.: cuadros muestreados.| Método | FPS | P.   |   Esco. |   Acc. |   Ah. | RPD ||---------------------|-------|-------|--------|--------|--------|-------|| Desempeño humano | - | - |   4.03 |   79,4 |   1.16 | - || GPT-4o [27] | - | 50 |   3,70 |   71,0 |   1,66 | - || GPT-4o [27] | - | 35 |   3,64 |   69,8 |   1,72 | - || GPT-4o-mini [27] | - | 35 |   3.17 |   59.1 |   2.01 | - || Ajuste de instrucciones |       |       |        |        |        |       || Vídeo-LLaVA [4] | - | 8 |   2,81 |   48,9 |   2.19 | - || LLaMA-VID [2]        | -     | 180   |   2.94 |   51.2 |   2.08 | -     || LLaVA-NExT [28] | - | 8 |   2,65 |   46,2 |   2.18 | - || LLaVA-Sabueso [29] | - | 8 |   3.12 |   54,7 |   1,83 | - || LargoVA [20] | - | 8 |   3.05 |   52,4 |   1,96 | - || MiniCMP-v2.6 [30] | - | 8 |   2,97 |   56,6 |   2.21 | - || VILA1.5 [31] | - | 8 |   3.10 |   57,1 |   2.20 | - || PasanteVL2 [32] | - | 8 |   3.15 |   57,6 |   2.11 | - || PasanteLM-XCP2.5 [33] | - | 8 |   3.21 |   57,7 |   2.12 | - || Sin entrenamiento |       |       |        |        |        |       || PelículaChat [7] | - | 32 |   2.07 |   35,3 |   2,36 | - || LibreVA [8] | - | 4 |   3.10 |   56,3 |   2.11 | - || Transmisión |       |       |        |        |        |       || Vídeo en línea [11] | 5 | - |   3.11 |   56,4 |   1,94 | 1.07 || Flash VStream [10] | 1 | - |   2,89 |   52.1 |   2.21 | 4.15 || TRANSMISIÓN |       |       |        |        |        |       || Lento | 15 | - |   3,48 |   64,7 |   1,76 | 0,90 || Base | 20 | - |   3,42 |   63,8 |   1,79 | 0,89 || Rápido | 32 | - |   3.28 |   61,7 |   1,81 | 0,85 |búsqueda (OS), búsqueda de memoria a largo plazo (LM), búsqueda de memoria a corto plazo
(SM) y tareas de interacción conversacional (CI). En particular, nuestro modelo lento
aumenta la precisión en un 10,3% en OS, un 5,1% en LM, un 4,9% en SM y un 5,8% en
CI comparado con Video-online.

Escenarios sin conexión. Comparamos nuestro modelo base con otros métodos en
puntos de referencia generales de comprensión de video sin conexión, incluido MSRVTT-QA [12],
ActivityNet [14], NExT-QA [15], MSVD-

Tabla 5: Comparación cuantitativa entre seis tareas. Resultados detallados para
tareas 'OS', 'LM', 'SM', 'CI', 'KG' y 'SF'. Para nombres completos y
definiciones, consulte el § 2.1

.|                      |             |             | SO | SO | LM | LM | SM | SM | CI | CI | kilos | kilos | SF | SF ||---------------------|-------------|-------------|------|------|------|------|------|------|------|------|------|------|------|------|| Método | Publicación | Publicación | Esco. | Acc. | Esco. | Acc. | Esco. | Acc. | Esco. | Acc. | Esco. | Acc. | Esco. | Acc. || Desempeño humano | - - | - - | 3,95 | 71,8 | 3,81 | 69,3 | 4.07 | 81,5 | 4.14 | 82,6 | 4.06 | 80,7 | 4.30 | 80,7 || GPT-4o-50 [27] | Arxiv | 2024 | 3.27 | 60,5 | 3.35 | 61,2 | 3.41 | 64,4 | 3,81 | 72,3 | 4,58 | 93,9 | 3,83 | 74,7 || GPT-4o-35 [27] | Arxiv | 2024 | 3.22 | 59,6 | 3.28 | 58,6 | 3,45 | 65,3 | 3,76 | 71,7 | 4,54 | 93,3 | 3,50 | 66,1 || GPT-4o-mini-35 [27] | Arxiv | 2024 | 2,52 | 46,8 | 2,70 | 45,8 | 2,80 | 51,0 | 3,50 | 64,0 | 4,67 | 95,2 | 2,90 | 53,3 || Ajuste de instrucciones |             |             |      |      |      |      |      |      |      |      |      |      |      |      || Vídeo-LLaVA [4] | EMNLP | 2024 | 2.25 | 31.2 | 2.31 | 35,9 | 2,50 | 41,8 | 3.18 | 56.1 | 3,81 | 74,6 | 2,93 | 54,8 || LLaMA-VID [2]        | ECCV        | 2024        | 2.32 | 33.9 | 2.43 | 38.2 | 2.63 | 44.1 | 3.31 | 58.4 | 3.93 | 76.9 | 3.06 | 57.1 || VILA1.5 [31] | CVPR | 2024 | 2.33 | 36.1 | 2,54 | 44,3 | 2,87 | 50,8 | 3,59 | 68,3 | 3,97 | 78,6 | 3.38 | 65,5 || PasanteVL2 [32] | CVPR | 2024 | 2,49 | 38,5 | 2,70 | 46,6 | 2,89 | 50,9 | 3,61 | 67,6 | 4.02 | 81,0 | 3.29 | 62,2 || LLaVA-NExT [28] | Archivo | 2024 | 2.17 | 35,0 | 2.14 | 31,4 | 2.15 | 36,0 | 2,55 | 42,7 | 3,88 | 76,1 | 3.12 | 57,6 || LLaVA-Sabueso [29] | Arxiv | 2024 | 2,49 | 37,6 | 2,68 | 43,2 | 3.09 | 53,4 | 3.21 | 55,7 | 3,89 | 76,3 | 3.35 | 62,0 || LargoVA [20] | Archivo | 2024 | 2,61 | 41,8 | 2,81 | 47,4 | 3.20 | 57,6 | 3.29 | 59,8 | 4.01 | 80,7 | 3,48 | 66,1 || MiniCMP-v2.6 [30] | Arxiv | 2024 | 2.32 | 37,6 | 2,78 | 51,9 | 2,62 | 43,7 | 3.35 | 65,7 | 3.19 | 66,2 | 3.27 | 64,2 || PasanteLM-XCP2.5 [33] | Arxiv | 2024 | 2,40 | 38,8 | 2,81 | 43,3 | 2,89 | 50,8 | 3,62 | 65,6 | 4.41 | 88,4 | 3.23 | 60,5 || Sin entrenamiento |             |             |      |      |      |      |      |      |      |      |      |      |      |      || PelículaChat [7] | CVPR | 2024 | 1,45 | 18.6 | 1,42 | 20.4 | 1,76 | 26,5 | 2.28 | 42,3 | 3.39 | 67,2 | 2.05 | 35,8 || LibreVA [8] | Arxiv | 2024 | 2.39 | 35,6 | 2.33 | 37,5 | 2,62 | 43,7 | 3.16 | 58,8 | 4.24 | 84,0 | 2,87 | 53,7 || En línea |             |             |      |      |      |      |      |      |      |      |      |      |      |      || Vídeo en línea [11] | CVPR | 2024 | 2,61 | 41,4 | 2,87 | 48,8 | 3.01 | 52,9 | 3.31 | 62,7 | 3,58 | 69,2 | 3.39 | 64.1 || Flash VStream [10] | Arxiv | 2024 | 2,38 | 37.1 | 2,64 | 44,5 | 2,78 | 48,6 | 3.13 | 58.1 | 3.34 | 66,4 | 3.17 | 59,2 || TRANSMISIÓN |             |             |      |      |      |      |      |      |      |      |      |      |      |      || Lento | - - | - - | 3.01 | 51,7 | 2,93 | 53,9 | 3.21 | 57,8 | 3,86 | 68,5 | 4.38 | 88,1 | 3,57 | 69,3 || Base | - - | - - | 2,93 | 50,5 | 2,87 | 52,9 | 3.15 | 56.1 | 3,82 | 67,6 | 4.37 | 87,9 | 3,56 | 68,8 || Rápido | - - | - - | 2,78 | 48.1 | 2,73 | 49,5 | 3.02 | 53,5 | 3,69 | 65,2 | 4.12 | 86,7 | 3,46 | 67,6 |Tabla 6: Comparación de rendimiento de varios modelos en vídeo sin conexión
comprensión del punto de referencia.|                    |             |             | ActNet | ActNet | SIGUIENTE-QA | SIGUIENTE-QA | MSVD | MSVD | MSRVTT | MSRVTT | Promedio | Promedio ||--------------------|-------------|-------------|----------|----------|-----------|-----------|--------|--------|----------|----------|-----------|-----------|
| Método | Publicación | Publicación | Esco.     | Acc.     | Esco.      | Acc.      | Esco.   | Acc.   | Esco.     | Acc.     | Esco.      | Acc.      || Vídeo-LLaVA [4] | EMNLP 2024 | EMNLP 2024 | 1,96 | 35,8 | 2.02 | 34,9 | 2,94 | 57,5 | 2.24 | 42,8 | 2.29 | 42,7 || LLaMA-VID [2]      | ECCV        | 2024        | 2.09     | 36.6     | 2.07      | 36.0      | 2.83   | 56.9   | 2.23     | 42.6     | 2.30      | 43.1      || PelículaChat [7] | CVPR | 2024 | 2.27 | 37,8 | 2.05 | 35,6 | 2,97 | 57,9 | 2.15 | 43,0 | 2,36 | 43,5 || Vídeo en línea [11] | CVPR | 2024 | 2.01 | 36,5 | 2.03 | 35,8 | 2,87 | 54,2 | 2.06 | 38,2 | 2.24 | 41.1 || LargoVA [20] | Archivo | 2024 | 2,48 | 47,1 2,74 | 45,4 | 2,98 | 57,8 | 2.22 | 42,4 | 2,60 | 48.1 || LLaVA-Sabueso [29] | Arxiv | 2024 | 2,69 | 48,7 | 2,56 | 43,7 | 3.07 | 56,8 | 2,42 | 42,7 | 2,68 | 47,9 || LibreVA [8] | Arxiv | 2024 | 2,48 | 46,7 | 2.32 | 41,7 | 3.02 | 58.1 | 2.16 | 38,3 | 2,49 | 46,2 || Flash VStream [10] | Arxiv | 2024 | 2.02 | 37,3 | 2.06 | 36.1 | 2,91 | 56.1 | 2.08 | 39,8 | 2.26 | 42,3 || TRANSMISIÓN | - - | - - | 2,78 | 50,1 | 2,84 | 50,5 | 3.08 | 58,7 | 2,38 | 43,4 | 2,77 | 50,6 |Control de calidad [16]. Dado que estos puntos de referencia implican preguntas abiertas,
evaluar el desempeño utilizando la puntuación y la precisión como métricas, empleando
mismo modelo de puntuación [3] que se utiliza en las pruebas en línea. Cabe señalar que
considerando la duración promedio limitada del video en MSRVTT [12] y MSVD
[16] como se muestra en la Tabla 1, no aplicamos la memoria a largo plazo M l para nuestra
modelo durante la prueba. Además, dado que estas preguntas abiertas
Los puntos de referencia del formato de prueba de respuesta no evalúan el diálogo de varias rondas.
capacidades, eliminamos el componente de memoria de diálogo M d de nuestro
modelo.

- En los puntos de referencia MSVD y MSRVTT, nuestro módulo de memoria a corto plazo permite que el modelo capture detalles visuales más específicos, lo que genera precisiones del 58,7% y 43,4%, respectivamente.
- Con la integración del módulo de memoria a largo plazo, nuestro modelo mejora el rendimiento del vídeo a más largo plazo, superando el mejor método de transmisión anterior Flash-VStream [10] con un 12,8 % y el mejor método fuera de línea LLaVA-Hound [29] con un 1,4 % en el punto de referencia ActivityNet [14]. En NExT-QA [15], nuestro método puede mejorar aún más la precisión del modelo básico LongVA [20] en un 5,1%.
- Aunque el modelo base LongVA [20] ha logrado la mejor precisión promedio en los puntos de referencia fuera de línea, nuestro método la mejora aún más en un 2,5%, lo que demuestra la efectividad del módulo de memoria.

## 4.3 ESTUDIO DE CASO

En la Fig. 6, ilustramos el proceso de razonamiento de STREAMCHAT con g = 2
para simplificar la observación de los mecanismos internos. El escenario
implica que un usuario le pide a STREAMCHAT que identifique una herramienta que cumpla con requisitos específicos
requisitos y describir su entorno. La estructura de la memoria.
consta de una memoria dialogada, M d , con dos entradas históricas, y una
Memoria en capas, M l , con dos niveles. La visualización del árbol de la memoria.
muestra que el sistema busca inicialmente información clave en el nivel 1.
Calcula la similitud del coseno entre la consulta del usuario Q i y dos
unidades de memoria, Resumen (1) y (2), obteniendo puntuaciones de 0,3993 y
0,4751, respectivamente. Con base en estos resultados, STREAMCHAT selecciona el
ruta desde el segundo nodo (v 1) debido a su mayor puntuación de similitud y
continúa por este camino. Posteriormente, el sistema agrega el valor {v
r } 1 r =0 de M s en tokens recuperados que luego se incorporan
en el proceso de razonamiento. Además, una alta puntuación de similitud de
0,6983 entre Qi y la primera conversación histórica ayuda a proporcionar
contexto, mejorando la profundidad y relevancia de la respuesta.

Figura 6: Un ejemplo de inferencia de StreamChat (§4.3). Dada una pregunta, nuestro sistema recupera la información más relacionada en un largo árbol de memoria e historiales de diálogo basados ​​en la mayor similitud de coseno.

<!-- imagen -->

Tabla 7: Análisis de la memoria jerárquica. Esta tabla muestra el impacto de varias configuraciones de memoria.|       |         | SO | SO | LM | LM | SM | SM | CI | CI | kilos | kilos | SS | SS | Promedio | Promedio ||-------|---------|------|------|------|------|------|------|------|------|------|------|------|------|-----------|-----------|
| M l ✗ | M s M d | Esco. | Acc. | Esco. | Acc. | Esco. | Acc. | Esco. | Acc. | Esco. | Acc. | Esco. | Acc. | Esco.      | Acc.      || ✗ | ✗ | 2,54 | 41,6 | 2,55 | 45,5 | 2,93 | 52,5 | 3.30 | 60.1 | 4.44 | 89,9 | 3,79 | 72,6 | 3.27 | 60,3 || ✗ | ✗ ✓ | 2,55 | 41,9 | 2,55 | 45,7 | 2,94 | 52,5 | 3,66 | 64,2 | 4.44 | 88,7 | 3,78 | 72,4 | 3.32 | 60,9 || ✗ | ✓ ✗ | 2,58 | 43,3 | 2,62 | 46,6 | 3.09 | 55,7 | 3.31 | 60,7 | 4.39 | 88,1 | 3,68 | 69,8 | 3.28 | 60,7 || ✓ | ✗ ✗ | 2,85 | 49,5 | 2,78 | 51,7 | 2,96 | 53,5 | 3.32 | 61.1 | 4.42 | 88,4 | 3,65 | 69,4 | 3.33 | 62,2 || ✓ | ✓ ✗ | 2,91 | 50,4 | 2,88 | 53,0 | 3.10 | 56,0 | 3,55 | 63,4 | 4.36 | 87,6 | 3,58 | 68,7 | 3.39 | 63.1 || ✓ | ✓ ✓ | 2,93 | 50,5 | 2,87 | 52,9 | 3.15 | 56.1 | 3,82 | 67,6 | 4.37 | 87,9 | 3,56 | 68,8 | 3,42 | 63,8 |## 4.4 ESTUDIO DE ABLACIÓN

Explorando los efectos de la memoria jerárquica. Realizamos ablación
experimentos utilizando el modelo Base para evaluar el impacto de diferentes
componentes de la memoria sobre el rendimiento. Como se muestra en la Tabla 7, agregando M d a la
El modelo base mejoró el rendimiento en la tarea de CI en un 4,1 % sin afectar
otras tareas. Agregar M l mejoró el rendimiento de la tarea LM en un 6,2%, mientras que
el uso de M aumenta el rendimiento de las tareas SM en un 3,2%. Los resultados indican
que el desempeño del modelo en cada subtarea se alinee con la inclusión
de atributos de memoria específicos. Además, observamos que diferentes
Los componentes de la memoria pueden complementarse entre sí. Cuando tanto a largo plazo M l como
La memoria a corto plazo M s se aplica simultáneamente, la precisión promedio
aumenta un 0,9%.

Compensaciones en la configuración de velocidad y umbral. El umbral del Lucas-
El algoritmo Kanade Optical Flow influye significativamente en el procesamiento de vídeo
velocidades. Como se ilustra en la Fig. 7 (a), aumentar el umbral inicialmente
acelera la velocidad de procesamiento. Sin embargo, este aumento se satura cuando
t llega a 0,55, estabilizándose en 32 FPS. Es importante destacar que un mayor procesamiento
Se desaconsejan las velocidades debido a su impacto perjudicial en el modelo.
rendimiento (64,0% → 60,7%). Elevar los umbrales conduce a más
cambios pronunciados en las diferencias de fotogramas y pérdida de datos originales,
limitando así la capacidad del modelo para utilizar eficazmente todo el
espectro de información de vídeo.

Diseño de Árbol de Memoria Larga. La longitud del trozo (L), el tamaño del grupo (g),
y el objetivo de agrupamiento ( C ) impactan significativamente la efectividad del
árbol de memoria ( M l ). En la Fig. 7 (b-d), evaluamos cómo estos factores
influir en las tareas de comprensión de vídeos en línea, utilizando el modelo base con t
=0,35.

- Como se muestra en la Fig. 7 (b), aumentar L de 15 a 30 conduce a un mejor rendimiento (61,2% → 64,0%). Sin embargo, aumentar aún más L a 40 da como resultado una ligera disminución (64,0 % → 63,1 %) y aumenta sustancialmente la latencia (0,84 s → 1,26 s), debido a las demandas computacionales del algoritmo de agrupación.
- Aumentar g de 2 a 12, lo que representa una menor compresión de la información visual y aumenta la longitud de la secuencia de entrada C × L, mejora el rendimiento (62,0% → 63,9%) a medida que se logra una mayor diversidad en el conocimiento en cada nodo del árbol de memoria larga M l. Sin embargo, intensifica la carga en la recuperación, lo que lleva a un aumento en el RDP (0,76 s → 1,02 s), como se ilustra en la Fig. 7 (c).
- El objetivo de agrupamiento ( C ) influye principalmente en el número de tokens ( vi ) almacenados en el M l . La Fig. 7 (d) muestra que aumentar la dimensión de vi (3 → 10) mejora el rendimiento del modelo (59,4% → 64,0%) al enriquecer el conocimiento almacenado, lo que también exacerba las limitaciones de VRAM (20 → 56 GB).

Precisión

65.0

64.0

63.0

62.0

61.0

Precisión

60.0

59.0

58.0

65.0

65.0

64.0

64.0

63.0

63.0

62.0

62.0

61.0

61.0

60.0

Precisión

60.0

59.0

59.0

58.0

58.0

0,15 0,25 0,35

<!-- imagen -->

70

60

50

40

30

ES

20

10

Memoria

Memoria

0

15

15

Figura 7: Análisis de parámetros de memoria. (a) La influencia entre
velocidad y umbral; Impacto de (b) la longitud del fragmento y (c) el tamaño del grupo en
rendimiento y latencia; (d) Efecto del objetivo de agrupamiento en el desempeño
y VRAM.

## 5 TRABAJOS RELACIONADOS

Modelos de lenguaje multimodal (MLM). Los recientes acontecimientos de grandes
Los modelos de lenguaje [3, 34-38] y las técnicas de alineación multimodal tienen
avanzó significativamente la capacidad de los MLM. La serie LLaVA [39, 40]
utiliza capas de mapeo sencillas y ajuste de instrucciones visuales paraampliar las tareas de comprensión de imágenes a datos de vídeo. Retos en vídeo
El procesamiento implica principalmente comprimir video de manera eficiente dentro de
ventanas contextuales limitadas. Innovaciones como el uso de ChatUniVi [5] de un
El algoritmo de agrupamiento K-NN comprime dinámicamente los tokens visuales, mientras
LLaMA-VID [2] reduce imágenes individuales a dos tokens mediante atención cruzada,
y MovieChat [7] aprovecha los marcos de memoria a largo y corto plazo para
amplio manejo de datos. A pesar de estos avances, la transición hacia
Comprensión efectiva de transmisión de video en tiempo real en la práctica.
las aplicaciones siguen sin abordarse lo suficiente. Nuestra investigación introduce una
Solución robusta diseñada para satisfacer las demandas en tiempo real del vídeo en línea.
comprensión, con el objetivo de llenar este vacío crítico.

Comprensión de la transmisión de vídeo. Demandas de comprensión de la transmisión de video
respuestas en tiempo real de modelos a consultas de usuarios, incluso como duraciones de videos
potencialmente extenderse indefinidamente. Esto es particularmente desafiante para
puntos de referencia tradicionales como reconocimiento de acciones [41], video de rondas múltiples
diálogo [42] y respuesta a preguntas en primera persona [13] que se basan en
muestreo de marco uniforme. En respuesta a estas limitaciones, existe una
creciente cambio hacia modelos en línea que procesan sólo datos actuales y pasados.
fotogramas de vídeo para formular respuestas [10, 11]. A pesar de estos
avances, estos modelos a menudo tienen problemas con velocidades de procesamiento lentas
y capacidades de generalización inadecuadas, lo que subraya una necesidad crítica
para una mayor exploración y mejora en este campo.

Recuperación-Generación Aumentada (RAG). RAG combina la recuperación de información
y generación de texto para producir respuestas más precisas e informativas
incorporar conocimiento externo en modelos de lenguaje [43-53]. esto
La técnica se ha vuelto cada vez más popular para abordar el conocimiento.
desafíos de retención y acceso a la información en tiempo real. Banco de memoria [54]
mejora la interacción al almacenar conversaciones en tiempo real y aprovechar
búsqueda de similitud para recuperar información contextualmente relevante,
enriquecer la profundidad y la coherencia del diálogo. Este enfoque
Mejora significativamente la capacidad de un modelo para mantener la continuidad en
conversaciones, particularmente en interacciones largas o de varios turnos donde
Mantener el contexto es crucial. Inspirándonos en la eficiencia de RAG,
introducir un sistema de memoria multimodal que integra y actualiza
datos textuales y visuales en tiempo real. Usando una recuperación inspirada en RAG
mecanismo, este sistema accede eficientemente a los más relevantes
información de nuestro banco de memoria, permitiendo el lenguaje multimodal
modelo para ofrecer respuestas precisas y específicas de consultas para vídeo mejorado
comprensión del lenguaje.

## 6 CONCLUSIÓN

En este trabajo, presentamos STREAMBENCH, un punto de referencia integral
diseñado específicamente para evaluar la comprensión de la transmisión de video, cubriendo un
gama más amplia de duraciones y tipos de videos con seis formatos de preguntas para
simular interacciones entre humanos y robots del mundo real. Este alcance más amplio
mejora nuestra capacidad para evaluar el rendimiento del modelo en situaciones complejas y
escenarios dinámicos. Además, presentamos STREAMCHAT, un programa de formación gratuito.
método diseñado para la comprensión eficiente de la transmisión de video, que
trata los fotogramas de vídeo como unidades comprimibles y almacenables y los gestiona
a través de una estructura de memoria jerárquica. Con sistema avanzado
programación, STREAMCHAT logra velocidades de procesamiento en tiempo real y reducción
latencia de interacción, lo que demuestra un rendimiento sólido tanto en línea
y configuraciones fuera de línea en nuestros extensos experimentos.

20

20

70

70

60

60

50

50

40

40

30

30

ES

20

20

10

10

0

0ES

Limitaciones y trabajos futuros. Nuestro algoritmo de recuperación actual se basa en
técnicas básicas de emparejamiento, lo que en ocasiones conduce a respuestas incorrectas.
Mejorar esto con mecanismos de recuperación más detallados es una
siguiente paso esencial. Además, las limitaciones de VRAM de nuestro árbol-
El almacenamiento estructurado podría limitar la escalabilidad a medida que la duración del vídeo y
la complejidad crece aún más. Investigar más eficiente o adaptativo
Las técnicas de compresión abordarán estas limitaciones. Además, a
Para lograr una latencia más baja, planeamos explorar una integración de hardware más estrecha.
y la posible adopción de servicios distribuidos multimodales y de servicio rápido
sistemas para adaptarse a parámetros de modelo más grandes y mayor usuario
demandas.

## REFERENCIAS

- [1] OpenAI. Chatgpt: Optimización de modelos de lenguaje para el diálogo. https://openai.com/blog/chatgpt, 2022. Consultado el: 30 de noviembre de 2022.
- [2] Yanwei Li, Chengyao Wang y Jiaya Jia. Llama-vid: Una imagen vale 2 tokens en modelos de lenguaje grandes. Preimpresión de arXiv arXiv:2311.17043, 2023.
- [3] Abhimanyu Dubey, Abhinav Jauhri, Abhinav Pandey, Abhishek Kadian, Ahmad Al-Dahle, Aiesha Letman, Akhil Mathur, Alan Schelten, Amy Yang, Angela Fan, et al. La manada de modelos llama 3. Preimpresión de arXiv arXiv:2407.21783, 2024.
- [4] Bin Lin, Bin Zhu, Yang Ye, Munan Ning, Peng Jin y Li Yuan. Video-llava: Aprendizaje de la representación visual unida por alineación antes de la proyección. Preimpresión de arXiv arXiv:2311.10122, 2023.
- [5] Peng Jin, Ryuichi Takanobu, Wancai Zhang, Xiaochun Cao y Li Yuan. Chat-univi: la representación visual unificada potencia los modelos de lenguaje grandes con comprensión de imágenes y videos. En Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 13700-13710, 2024.
- [6] Muhammad Maaz, Hanoona Rasheed, Salman Khan y Fahad Shahbaz Khan. Video-chatgpt: Hacia una comprensión detallada del vídeo a través de grandes modelos de visión y lenguaje. Preimpresión de arXiv arXiv:2306.05424, 2023.
- [7] Enxin Song, Wenhao Chai, Guanhong Wang, Yucheng Zhang, Haoyang Zhou, Feiyang Wu, Haozhe Chi, Xun Guo, Tian Ye, Yanting Zhang, et al. Moviechat: desde token denso hasta memoria escasa para una comprensión prolongada de vídeos. En Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 18221-18232, 2024.
- [8] Wenhao Wu. Freeva: mllm sin conexión como asistente de vídeo sin formación. Preimpresión de arXiv arXiv:2405.07798, 2024.
- [9] Lin Xu, Yilin Zhao, Daquan Zhou, Zhijie Lin, See Kiong Ng y Jiashi Feng. Pllava: Extensión de llava sin parámetros de imágenes a videos para subtítulos de video densos. Preimpresión de arXiv arXiv:2404.16994, 2024.
- [10] Haoji Zhang, Yiqin Wang, Yansong Tang, Yong Liu, Jiashi Feng, Jifeng Dai y Xiaojie Jin. Flash-vstream: comprensión en tiempo real basada en memoria para transmisiones de video largas. Preimpresión de arXiv arXiv:2406.08085, 2024.
- [11] Joya Chen, Zhaoyang Lv, Shiwei Wu, Kevin Qinghong Lin, Chenan Song, Difei Gao, Jia-Wei Liu, Ziteng Gao, Dongxing Mao y Mike Zheng Shou. Videollm-online: Modelo de lenguaje grande de vídeo online para streaming de vídeo. En Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 18407-18418, 2024.
- [12] Jun Xu, Tao Mei, Ting Yao y Yong Rui. Msr-vtt: un gran conjunto de datos de descripción de video para unir video e idioma. En Actas de la conferencia IEEE sobre visión por computadora y reconocimiento de patrones, páginas 5288-5296, 2016.- [13] Karttikeya Mangalam, Raiymbek Akshulakov y Jitendra Malik. Egoschema: un punto de referencia de diagnóstico para la comprensión del lenguaje de vídeos de muy larga duración. Avances en los sistemas de procesamiento de información neuronal, 36, 2024.
- [14] Zhou Yu, Dejing Xu, Jun Yu, Ting Yu, Zhou Zhao, Yueting Zhuang y Dacheng Tao. Activitynet-qa: un conjunto de datos para comprender videos web complejos mediante la respuesta a preguntas. En Actas de la Conferencia AAAI sobre Inteligencia Artificial, volumen 33, páginas 9127-9134, 2019.
- [15] Junbin Xiao, Xindi Shang, Angela Yao y Tat-Seng Chua. Next-qa: Siguiente fase de preguntas y respuestas para explicar las acciones temporales. En Actas de la conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 9777-9786, 2021.

- [16] Dejing Xu, Zhou Zhao, Jun Xiao, Fei Wu, Hanwang Zhang, Xiangnan He y Yueting Zhuang. Respuestas a preguntas en vídeo mediante una atención refinada gradualmente sobre la apariencia y el movimiento. En Actas de la 25ª conferencia internacional ACM sobre multimedia, páginas 1645-1653, 2017.
- [17] Chaoyou Fu, Yuhan Dai, Yondong Luo, Lei Li, Shuhuai Ren, Renrui Zhang, Zihan Wang, Chenyu Zhou, Yunhang Shen, Mengdan Zhang, et al. Video-mme: el primer punto de referencia de evaluación integral de películas multimodales en análisis de video. Preimpresión de arXiv arXiv:2405.21075, 2024.
- [18] Junjie Zhou, Yan Shu, Bo Zhao, Boya Wu, Shitao Xiao, Xi Yang, Yongping Xiong, Bo Zhang, Tiejun Huang y Zheng Liu. Mlvu: un punto de referencia integral para la comprensión de vídeos largos multitarea. Preimpresión de arXiv arXiv:2406.04264, 2024.
- [19] Sami Abu-El-Haija, Nisarg Kothari, Joonseok Lee, Paul Natsev, George Toderici, Balakrishnan Varadarajan y Sudheendra Vijayanarasimhan. Youtube-8m: un punto de referencia de clasificación de vídeos a gran escala. Preimpresión de arXiv arXiv:1609.08675, 2016.
- [20] Peiyuan Zhang, Kaichen Zhang, Bo Li, Guangtao Zeng, Jingkang Yang, Yuanhan Zhang, Ziyue Wang, Haoran Tan, Chunyuan Li y Ziwei Liu. Larga transferencia de contexto del lenguaje a la visión. Preimpresión de arXiv arXiv:2406.16852, 2024.
- [21] Bruce D Lucas y Takeo Kanade. Una técnica iterativa de registro de imágenes con aplicación a la visión estéreo. En IJCAI'81: 7ma conferencia conjunta internacional sobre inteligencia artificial, volumen 2, páginas 674-679, 1981.
- [22] Richard C. Atkinson. Una propuesta de sistema y sus procesos de control. La Psicología del Aprendizaje y la Motivación, 2, 1968.
- [23] Hermann Ebbinghaus. Memoria: una contribución a la psicología experimental. Anales de neurociencias, 20(4):155, 2013.
- [24] Wenhui Wang, Furu Wei, Li Dong, Hangbo Bao, Nan Yang y Ming Zhou. Minilm: Destilación profunda de autoatención para la compresión independiente de la tarea de transformadores previamente entrenados. Avances en los sistemas de procesamiento de información neuronal, 33:5776-5788, 2020.
- [25] Jeff Johnson, Matthijs Douze y Hervé Jégou. Búsqueda de similitudes a escala de mil millones con gpus. Transacciones IEEE sobre Big Data, 7(3):535-547, 2019.
- [26] Alec Radford, Jong Wook Kim, Chris Hallacy, Aditya Ramesh, Gabriel Goh, Sandhini Agarwal, Girish Sastry, Amanda Askell, Pamela Mishkin, Jack Clark, et al. Aprendizaje de modelos visuales transferibles a partir de la supervisión del lenguaje natural. En Conferencia internacional sobre aprendizaje automático, páginas 8748-8763. PMLR, 2021.
- [27] Aaron Hurst, Adam Lerer, Adam P Goucher, Adam Perelman, Aditya Ramesh, Aidan Clark, AJ Ostrow, Akila Welihinda, Alan Hayes, Alec Radford, et al. Tarjeta del sistema gpt-4o. Preimpresión de arXiv arXiv:2410.21276, 2024.- [28] Haotian Liu, Chunyuan Li, Yuheng Li, Bo Li, Yuanhan Zhang, Sheng Shen y Yong Jae Lee. Llava-next: Mejora del razonamiento, ocr y conocimiento del mundo, enero de 2024.
- [29] Ruohong Zhang, Liangke Gui, Zhiqing Sun, Yihao Feng, Keyang Xu, Yuanhan Zhang, Di Fu, Chunyuan Li, Alexander Hauptmann, Yonatan Bisk, et al. Optimización de preferencias directas de modelos multimodales de gran tamaño de vídeo a partir de la recompensa del modelo de lenguaje. Preimpresión de arXiv arXiv:2404.01258, 2024.
- [30] Yuan Yao, Tianyu Yu, Ao Zhang, Chongyi Wang, Junbo Cui, Hongji Zhu, Tianchi Cai, Haoyu Li, Weilin Zhao, Zhihui He, et al. Minicpm-v: Un mllm de nivel gpt-4v en tu teléfono. Preimpresión de arXiv arXiv:2408.01800, 2024.
- [31] Ji Lin, Hongxu Yin, Wei Ping, Pavlo Molchanov, Mohammad Shoeybi y Song Han. Vila: Sobre el entrenamiento previo para modelos de lenguaje visual. En Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 26689-26699, 2024.
- [32] Zhe Chen, Jiannan Wu, Wenhai Wang, Weijie Su, Guo Chen, Sen Xing, Muyan Zhong, Qinglong Zhang, Xizhou Zhu, Lewei Lu, et al. Internvl: Ampliar los modelos básicos de la visión y alinearlos para tareas visual-lingüísticas genéricas. En Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 24185-24198, 2024.
- [33] Pan Zhang, Xiaoyi Dong, Yuhang Zang, Yuhang Cao, Rui Qian, Lin Chen, Qipeng Guo, Haodong Duan, Bin Wang, Linke Ouyang, et al. Internlm-xcomposer-2.5: Un versátil

- modelo de lenguaje de visión amplia que admite entradas y salidas contextuales largas. Preimpresión de arXiv arXiv:2407.03320, 2024.
- [34] Tom Brown, Benjamin Mann, Nick Ryder, Melanie Subbiah, Jared D Kaplan, Prafulla Dhariwal, Arvind Neelakantan, Pranav Shyam, Girish Sastry, Amanda Askell, et al. Los modelos de lenguaje son aprendices de pocas oportunidades. Avances en los sistemas de procesamiento de información neuronal, 33:1877-1901, 2020.
- [35] Susan Zhang, Stephen Roller, Naman Goyal, Mikel Artetxe, Moya Chen, Shuohui Chen, Christopher Dewan, Mona Diab, Xian Li, Xi Victoria Lin, et al. Optar: Abrir modelos de lenguaje transformador previamente entrenados. Preimpresión de arXiv arXiv:2205.01068, 2022.
- [36] Hyung Won Chung, Le Hou, Shayne Longpre, Barret Zoph, Yi Tay, William Fedus, Yunxuan Li, Xuezhi Wang, Mostafa Dehghani, Siddhartha Brahma, et al. Escalar modelos de lenguaje ajustados a la instrucción. Revista de investigación sobre aprendizaje automático, 25(70):1-53, 2024.
- [37] Rohan Anil, Andrew M Dai, Orhan Firat, Melvin Johnson, Dmitry Lepikhin, Alexandre Passos, Siamak Shakeri, Emanuel Taropa, Paige Bailey, Zhifeng Chen, et al. Informe técnico de Palm 2. Preimpresión de arXiv arXiv:2305.10403, 2023.
- [38]R OpenAI. Informe técnico del Gpt-4. arxiv2303.08774. Ver en el artículo 2(5), 2023.
- [39] Haotian Liu, Chunyuan Li, Qingyang Wu y Yong Jae Lee. Ajuste de instrucciones visuales. Avances en sistemas de procesamiento de información neuronal, 36, 2024.
- [40] Haotian Liu, Chunyuan Li, Yuheng Li y Yong Jae Lee. Líneas de base mejoradas con ajuste de instrucciones visuales. En Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 26296-26306, 2024.
- [41] Joao Carreira y Andrés Zisserman. Quo vadis, ¿reconocimiento de la acción? un nuevo modelo y el conjunto de datos cinéticos. En actas de la Conferencia IEEE sobre visión por computadora y reconocimiento de patrones, páginas 6299-6308, 2017.- [42] Huda Alamri, Vincent Cartillier, Abhishek Das, Jue Wang, Anoop Cherian, Irfan Essa, Dhruv Batra, Tim K Marks, Chiori Hori, Peter Anderson, et al. Diálogo audiovisual con reconocimiento de escena. En Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 7558-7567, 2019.
- [43] Penghao Zhao, Hailin Zhang, Qinhan Yu, Zhengren Wang, Yunteng Geng, Fangcheng Fu, Ling Yang, Wentao Zhang y Bin Cui. Generación de recuperación aumentada para contenido generado por IA: una encuesta. Preimpresión de arXiv arXiv:2402.19473, 2024.
- [44] Xinbei Ma, Yeyun Gong, Pengcheng He, Hai Zhao y Nan Duan. Reescritura de consultas para modelos de lenguaje grandes con recuperación aumentada. Preimpresión de arXiv arXiv:2305.14283, 2023.
- [45] Wenhao Yu, Dan Iter, Shuohang Wang, Yichong Xu, Mingxuan Ju, Soumya Sanyal, Chenguang Zhu, Michael Zeng y Meng Jiang. Generar en lugar de recuperar: los modelos de lenguaje grandes son potentes generadores de contexto. Preimpresión de arXiv arXiv:2209.10063, 2022.
- [46] Zhihong Shao, Yeyun Gong, Yelong Shen, Minlie Huang, Nan Duan y Weizhu Chen. Mejora de los modelos de lenguaje grande con recuperación aumentada con sinergia iterativa de recuperación y generación. Preimpresión de arXiv arXiv:2305.15294, 2023.
- [47] Xintao Wang, Qianwen Yang, Yongting Qiu, Jiaqing Liang, Qianyu He, Zhouhong Gu, Yanghua Xiao y Wei Wang. Knowledgpt: mejora de modelos de lenguaje grandes con acceso de recuperación y almacenamiento en bases de conocimiento. Preimpresión de arXiv arXiv:2308.11761, 2023.
- [48] Zhuyun Dai, Vincent Y Zhao, Ji Ma, Yi Luan, Jianmo Ni, Jing Lu, Anton Bakalov, Kelvin Guu, Keith B Hall y Ming-Wei Chang. Promptagator: recuperación densa de pocas tomas de 8 ejemplos. Preimpresión de arXiv arXiv:2209.11755, 2022.
- [49] Zhiqing Sun, Xuezhi Wang, Yi Tay, Yiming Yang y Denny Zhou. Modelos de lenguaje aumentados por recitación. Preimpresión de arXiv arXiv:2210.01296, 2022.
- [50] Akari Asai, Zeqiu Wu, Yizhong Wang, Avirup Sil y Hannaneh Hajishirzi. Auto-rago: Aprender a recuperar, generar y criticar a través de la autorreflexión. Preimpresión de arXiv arXiv:2310.11511, 2023.
- [51] Xi Victoria Lin, Xilun Chen, Mingda Chen, Weijia Shi, María Lomeli, Rich James, Pedro Rodríguez, Jacob Kahn, Gergely Szilvasy, Mike Lewis, et al. Ra-dit: ajuste de instrucción dual con recuperación aumentada. Preimpresión de arXiv arXiv:2310.01352, 2023.

- [52] Oded Ovadia, Menachem Brief, Moshik Mishaeli y Oren Elisha. ¿Ajuste o recuperación? comparando la inyección de conocimiento en llms. Preimpresión de arXiv arXiv:2312.05934, 2023.
- [53] Tong Chen, Hongwei Wang, Sihao Chen, Wenhao Yu, Kaixin Ma, Xinran Zhao, Dong Yu y Hongming Zhang. Recuperación densa x: ¿Qué granularidad de recuperación deberíamos utilizar? Preimpresión de arXiv arXiv:2312.06648, 2023.
- [54] Wanjun Zhong, Lianghong Guo, Qiqi Gao, He Ye y Yanlin Wang. Memorybank: mejora de modelos lingüísticos grandes con memoria a largo plazo. En Actas de la Conferencia AAAI sobre Inteligencia Artificial, volumen 38, páginas 19724-19731, 2024.

## RESUMEN DEL APÉNDICE

Este apéndice contiene detalles adicionales para la presentación de ICLR 2025,
titulado 'Comprensión de transmisión de video e interacción de múltiples rondas con
Conocimiento mejorado con la memoria', que se organiza de la siguiente manera:

- §A presenta el proceso de recopilación de conjuntos de datos.
- §B visualiza más casos de nuestro benchmark.
- §C ofrece detalles sobre el algoritmo de recuperación.
- §D presenta los detalles y el método de cálculo de las métricas.
- §E muestra casos de falla y análisis.
- §F analiza las estrategias de selección y despliegue de modelos.
- §G describe nuestros planes para la expansión del benchmark.

## UN CANAL DE DATOSPaso 1: Clasificación Paso 2: Juez humano

Figura 8: Proceso de preparación de fechas utilizado en STREAMBENCH.

<!-- imagen -->

La figura 8 presenta nuestro canal de recopilación de videos. Consta de 3 partes:
(1) Clasificación; (2) Juez humano; y (3) verificación de anotaciones. Primero, un
MLLM [4] se utiliza para completar la clasificación de videos según nuestra
requisitos. El siguiente mensaje se utiliza durante los primeros datos.
paso de filtrado:

- 'Según la información del video observado, categorice el video en una de las categorías predefinidas que figuran en { Todas\_Clases }. Responda exclusivamente en el formato de una cadena de diccionario de Python con las claves 'pred' y 'score'. La clave 'pred' debe contener la CADENA mayúscula de la categoría elegida. Abstenerse de proporcionar cualquier texto adicional o salida explicativa. Su respuesta debe seguir estrictamente este ejemplo: { 'pred': 'A' }.'

{ All\_Class } es la formación de opciones. Al tratar con diferentes
conjuntos de datos, necesitamos cambiar las opciones. Por ejemplo, cuando se trata de
Youtube-8M [19], es { A: Drama, B: Acción, C: Dibujos animados, D: Romance, E:
Ciencia ficción, F: Otros } y { A: Cocina, B: Construcción, C: Room-Tour, D:
Gardening, E: Otros } para el conjunto de datos EgoSchema [13]. Guardamos la salida en
un archivo JSON y luego busque categorías del archivo según sea necesario. es
Vale la pena señalar que también utilizamos la información de la categoría original en el
Datos de YouTube. El proceso de clasificación anterior se utiliza principalmente para
Clasificación secundaria de datos de MovieClips. Para EgoSchema, necesitamos
Clasifique todos los videos originales ya que no contienen anotaciones de categoría.

##B MÁS VISUALIZACIONES

En la Fig. 9-12, visualizamos varios casos de STREAMCHAT aplicados a diferentes
tipos de vídeos. Específicamente, la Fig. 9 ilustra un video egocéntrico.
anotación, donde el sistema realiza preguntas interactivas basadas en
la información visual capturada desde una perspectiva en primera persona. en esto
En la figura, se muestran varios tipos de preguntas de anotación, incluidas
identificación de objetos, recuperación de recuerdos e interacciones basadas en conocimientos.
Cada ejemplo contiene seis preguntas distintas, con el Inicio simple (SS)
pregunta colocada primero, mientras que la secuencia de las cinco restantes
Las preguntas varían aleatoriamente a lo largo del video. Las preguntas exploran
aspectos como la memoria a corto plazo, la memoria a largo plazo, la búsqueda de objetos y
interacción conversacional, lo que permite una amplia gama de análisis. a
Para mejorar aún más la claridad, se resaltan marcos específicos junto con el
ubicaciones de los objetos clave a los que se hace referencia en las preguntas, lo que permite una mejor
comprensión de cómo el sistema interactúa con el contexto visual en
diferentes momentos.

Figura 9: Visualización del análisis de vídeo egocéntrico.

<!-- imagen -->

Figura 10: Visualización del análisis de vídeo web.

<!-- imagen -->

Figura 11: Visualización del análisis de vídeo de trabajo.

<!-- imagen -->

Figura 12: Visualización del análisis de vídeo de películas.

<!-- imagen -->

## C ALGORITMO DE RECUPERACIÓN

Inspirado en el sistema de argumentación de recuperación [43], nuestro enfoque
mejora la capacidad del modelo para abordar consultas complejas mediante la recuperación
la información más relevante de la memoria a largo plazo para el contexto
apoyo. Como se describe en el Algoritmo 1, calculamos la similitud, Sim,
entre la solicitud del usuario, Q, y las entradas T n en la memoria. esto
El proceso identifica la ruta óptima para acceder a la información más pertinente.
conocimiento almacenado, T mejor y C mejor. Aprovechando nuestro almacenamiento en forma de árbol
estructura, nos centramos eficientemente en los nodos de mayor similitud en cadacapa, minimizando la carga computacional al evitar subnodos exhaustivos
cálculos. El conocimiento seleccionado, T mejor y C mejor, es entonces
integrado con Q en un formato de solicitud mixto para servir como entrada final
para el modelo de lenguaje multimodal, facilitando una respuesta precisa
generación.```
Algorithm 1 Knowledge Retrieval from Long-Term Memory 1: Input: User request Q 2: Output: Best matching knowledge T and C 3: Initialize similarity Sim max ←-∞ 4: Initialize best path knowledge T best , C best ←∅ , ∅ 5: for each node n in the tree structure do 6: Compute similarity Sim ( Q,T n ) where T n is the caption at node n 7: if Sim ( Q,T n ) > Sim max then 8: Sim max ← Sim ( Q,T n ) 9: T best , C best ← T n , C n ▷ Update best match knowledge 10: end if 11: if node n has children then 12: Continue to next level 13: end if 14: end for 15: Reconstruct input for MLLM using T best , C best , and Q 16: return T best , C best
```
##D DETALLES DE LAS MÉTRICAS

Diseñamos las siguientes métricas para medir la capacidad del modelo para transmitir
comprensión del vídeo:

- (1) Puntuación y precisión: para evaluar la corrección semántica de un diálogo de un solo turno, el uso de modelos de lenguaje es un enfoque generalizado [12, 14, 16-18]. También utilizamos esto como una métrica clave en nuestro punto de referencia. En nuestra prueba comparativa, utilizamos el modelo de lenguaje de código abierto LLaMA-3 8B [3] versión Instruct como nuestro modelo de puntuación f. Este es el mensaje que utilizamos durante la puntuación:

Prompt = [ { "role": "system", "content": "Eres un inteligente
chatbot diseñado para evaluar la exactitud de los resultados generativos
para pares de preguntas y respuestas. Tu tarea es comparar la respuesta prevista.
con la respuesta correcta y determinar si coinciden

significativamente. Así es como puede realizar la tarea:

## INSTRUCCIONES:

- Centrarse en la coincidencia significativa entre la respuesta prevista y la respuesta correcta.
- Considerar sinónimos o paráfrasis como coincidencias válidas.
- Evaluar la exactitud de la predicción frente a la respuesta. "}

{ "role": "system", "content": "Evalúe el siguiente vídeo.
par de preguntas y respuestas basadas: Pregunta: pregunta; Respuesta correcta: respuesta;
Respuesta prevista: predicción

Proporcione su evaluación sólo como sí/no y puntúe donde la puntuación sea un
valor entero entre 0 y 5, donde 5 indica el significado más alto
partido.

Genere la respuesta en forma de cadena de diccionario de Python.
con las claves 'llama pred' y 'score', donde el valor de 'llama pred' es un
cadena de 'sí' o 'no' y el valor de 'puntuación' está en INTEGER, no
CUERDA. NO PROPORCIONE NINGÚN OTRO TEXTO DE SALIDA O EXPLICACIÓN. Sólo
proporcione la cadena del diccionario de Python. Por ejemplo, su respuesta debe
se ve así: {'llama pred': TU JUEZ, 'puntuación': TU PUNTUACIÓN.}" } ]

Tabla 8: Indicación dada al modelo LLaMA-3 para su evaluación.

Organizamos la pregunta Q, la respuesta de referencia R y la respuesta del modelo M.
en la pestaña. 8 formación y envío al modelo de puntuación, que luego proporciona
una puntuación en el rango de 0 a 5 y evalúa si la respuesta del modelo
es semánticamente correcto:

<!-- fórmula-no-decodificada -->

Una puntuación más alta Si y Acc indica que la respuesta se acerca más a la
respuesta de referencia.

(2) Coherencia: dado que un solo vídeo puede implicar múltiples rondas de
diálogo, necesitamos evaluar la capacidad del modelo para proporcionar una respuesta coherente
experiencia en diferentes rondas. Introducimos la métrica de coherencia,
que calcula el valor absoluto de la diferencia entre el
puntuaciones semánticas Si de diferentes diálogos dentro de un mismo escenario. el
El promedio de todas estas diferencias se utiliza como métrica de coherencia. el
La fórmula de cálculo es la siguiente:

<!-- fórmula-no-decodificada -->

donde C es la puntuación de coherencia, N es el número total de turnos de diálogo
en el escenario, Si representa la puntuación semántica del i -ésimo diálogo
girar y | . | es la diferencia absoluta entre las puntuaciones semánticas de
turnos de diálogo consecutivos. Es evidente que una C más pequeña indica
que el modelo proporcione una mejor experiencia de coherencia para el usuario.

(3) Retraso en el procesamiento de solicitudes: para escenarios en línea, latencia del sistema
consta de dos partes: 1. Retraso en el procesamiento de la solicitud; 2. Retraso generacional.
El retraso generacional está influenciado principalmente por factores como el contexto.
longitud, parámetros del modelo de lenguaje y métodos de implementación, y pueden ser
ajustado a través de varios métodos. En este punto de referencia, evaluamos principalmente
(1) retraso en el procesamiento de la solicitud, que se calcula como el tiempo desde queel usuario completa la entrada de la solicitud hasta que el modelo comienza a generar
la respuesta. La fórmula de cálculo es la siguiente:

<!-- fórmula-no-decodificada -->

## E CASO DE FALLA Y ANALIZAR

Presentamos algunos casos de fallas que ocurrieron durante las pruebas y explicamos
por qué ocurrieron. La mayoría de estos casos provienen de búsquedas de objetos,
memoria a término, memoria a corto plazo y tareas de interacción conversacional. el
Los problemas ocurridos se agrupan principalmente en cuatro tipos:

- Temporal detallado: en la tarea de búsqueda de objetos, nuestro método todavía tiene dificultades para identificar información clave cuando los objetos o eventos consultados aparecen demasiado breve o esporádicamente. Por ejemplo, como se demuestra en el caso (1), la pregunta del usuario se refiere a una vela. Sin embargo, debido a la aparición poco frecuente de la vela y su pequeño tamaño, el modelo no proporciona una respuesta precisa.
- Espacial de grano fino: ya sea en la búsqueda de objetos o en tareas de memoria a corto plazo, nuestro método enfrenta limitaciones cuando el objetivo del usuario es demasiado pequeño o se mezcla con el fondo, incluso si el objeto aparece varias veces en el video. Por ejemplo, en el caso de la tarea de memoria a corto plazo (2) y en el caso de la tarea de búsqueda de objetos (3), los objetos objetivo (tazón de porcelana y taza roja) son demasiado pequeños en relación con el primer plano, lo que dificulta que el modelo los detecte y localice con precisión. Continuaremos mejorando nuestro método para mejorar la percepción de objetos pequeños.
- Movimiento del objetivo: durante el proceso de razonamiento, observamos que en algunos casos, incluso cuando el modelo identificó correctamente al objetivo, su interpretación de las acciones y relaciones del objetivo con los objetos circundantes aún era inexacta. Por ejemplo, en el caso de la tarea de memoria a largo plazo (4), el modelo no reconoció la acción y la asociación de posición entre la "persona" y la "caja", lo que llevó a una respuesta incorrecta.
- Inducción de contexto: en la tarea de interacción conversacional, el desempeño del modelo está influenciado por la precisión de sus respuestas a preguntas relacionadas previas. Por ejemplo, en el caso 5, el modelo recuperó información del historial de diálogo, pero cuando esa información histórica era incorrecta, al modelo le resultaba difícil proporcionar la respuesta correcta.
- Pérdida de información: Según los resultados experimentales de la Tab. 5, aunque nuestro método muestra un rendimiento equilibrado en varias tareas de StreamChat, nuestro almacenamiento de memoria jerárquico todavía tiene el riesgo potencial de perder información. Dado que nuestro método existente depende demasiado de la precisión del algoritmo de recuperación, continuaremos actualizándolo para minimizar la pérdida de información.

## SELECCIÓN E IMPLEMENTACIÓN DEL MODELO F

Nuestra investigación indica que un modelo adecuado debe poseer la
siguientes atributos clave:

- Comprensión de los vídeos de formato largo: el procesamiento eficaz de vídeos largos es crucial. Si bien utilizamos K-Means para la compresión de características, la información recuperada por nuestro mecanismo de memoria permanece relativamente larga, lo que requiere un modelo capaz de manejar secuencias extendidas.
- Robustez ante las variaciones de las indicaciones: para que las conversaciones de varios turnos sean precisas y coherentes, el modelo debe ser sólido ante los cambios en la redacción de las indicaciones. Esta robustez es esencial para evitar inconsistencias.

Figura 13: Visualización de casos de falla.

<!-- imagen -->

o alucinaciones en la salida del modelo cuando las indicaciones se ajustan a
incorporar información del mecanismo de la memoria.

Al integrar LongVA con nuestro sistema propuesto, ampliamos con éxitosus capacidades para abarcar el procesamiento de video en streaming y multi-turn
conversaciones preservando al mismo tiempo estas características críticas. como nosotros
introducido en §4, utilizamos 2 GPUS para completar la implementación de nuestro
método. La razón principal es que durante la programación del sistema, necesitamos
utilizar el paralelismo tensorial para distribuir la carga computacional para
ejecución eficiente. Específicamente, el (i) apilamiento selectivo de cuadros
El hilo y (iii) el hilo de resumen de contexto se ejecutan en GUP1 mientras
(ii) el hilo de formación de memoria se ejecuta en GPU2. Por lo tanto, el
Los tensores de video comprimido deben transmitirse entre diferentes GPU.
para garantizar el funcionamiento estable del sistema.

##G PLAN DE EXPANSIÓN DE STREAMBENCH

- Escala de vídeo: estamos intentando ampliar la cantidad de vídeos contenidos en StreamBench para alcanzar un estándar más alto. Estamos trabajando para ampliar la cantidad de videos a miles mientras mantenemos la diversidad de longitudes y tipos de videos.
- Escala de anotaciones: Seguimos promoviendo el desarrollo de anotaciones de alta calidad. Según sus sugerencias, utilizaremos métodos de anotación manual para ampliar la anotación de los puntos de referencia existentes al orden de diez mil niveles y también utilizaremos la inspección manual para filtrar etiquetas tóxicas e información errónea.
- Tareas diversas: dado que el punto de referencia actual solo tiene un tipo de tarea, continuamos ampliando los tipos de tareas incluidas en el punto de referencia, que incluyen, entre otras, preguntas de opción múltiple, subtítulos de video y conexión a tierra de video, etc.