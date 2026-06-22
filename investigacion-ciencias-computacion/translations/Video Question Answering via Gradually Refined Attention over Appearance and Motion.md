## Respuesta a preguntas en video a través de una atención gradualmente refinada sobre la apariencia y el movimiento ∗

Dejing Xu ‡, Zhou Zhao ‡, Jun Xiao ‡, Fei Wu ‡, Hanwang Zhang §,
Xiangnan He ¶ , Yueting Zhuang ‡

‡ Universidad de Zhejiang, § Universidad de Columbia, ¶ Universidad Nacional de
Singapur {xudejing, zhaozhou, yzhuang}@zju.edu.cn,
{junx,wufei}@cs.zju.edu.cn {hanwangzhang, xiangnanhe}@gmail.com

## RESUMEN

Recientemente, la respuesta a preguntas sobre imágenes (ImageQA) ha ganado mucha atención.
en la comunidad de investigación. Sin embargo, como extensión natural, el vídeo
la respuesta a preguntas (VideoQA) está menos explorada. Aunque ambas tareas parecen
De manera similar, VideoQA es más desafiante principalmente debido a la complejidad
y diversidad de vídeos. Como tal, simplemente extender los métodos ImageQA
a vídeos es insuficiente y subóptimo. En particular, trabajar con el
El vídeo necesita modelar su estructura temporal inherente y analizar la
diversa información que contiene. En este artículo consideramos explotar
la información de apariencia y movimiento residía en el video con una novela
Mecanismo de atención. Más concretamente, proponemos un modelo de extremo a extremo.
que gradualmente refina su atención sobre la apariencia y el movimiento
características del video usando la pregunta como guía. La pregunta es
procesa palabra por palabra hasta que el modelo genera la optimización final.
atención. La representación ponderada del vídeo, así como otras
información contextual, se utilizan para generar la respuesta. extenso
Los experimentos muestran las ventajas de nuestro modelo en comparación con otros de referencia.
modelos. También demostramos la efectividad de nuestro modelo analizando
los pesos de atención refinados durante el procedimiento de respuesta a preguntas.

## PALABRAS CLAVE

Respuesta a preguntas en vídeo; Mecanismo de Atención; Red neuronal

## 1 INTRODUCCIÓN

Obtener información de vídeos es importante y valioso para numerosos
aplicaciones, especialmente cuando se produce una gran cantidad de vídeos
hoy en día. Es casi imposible para nosotros ver estos videos.
La respuesta a preguntas en vídeo (VideoQA) puede ayudarnos a adquirir rápidamente la
información necesaria que buscamos en los vídeos, lo cual es beneficioso para
varias aplicaciones de la vida real [4, 8, 28, 29, 36].

∗ El autor correspondiente es Jun Xiao.

Permiso para hacer copias digitales o impresas de todo o parte de este trabajo.
para uso personal o en el aula se otorga sin cargo siempre que
Las copias no se hacen ni distribuyen con fines de lucro o ventajas comerciales.
y que las copias lleven este aviso y la cita completa en el primer
página. Derechos de autor de componentes de este trabajo propiedad de terceros distintos del
Los autores deben ser honrados. Se permite realizar resúmenes con crédito. para copiar
de otro modo, o volver a publicar, publicar en servidores o redistribuir en listas,
requiere permiso previo específico y/o una tarifa. Solicitar permisos
de permisos@acm.org.

MM'17, 23-27 de octubre de 2017, Mountain View, CA, EE. UU.

© 2017 Copyright propiedad del propietario/autor(es). Derechos de publicación
con licencia de la Asociación de Maquinaria de Computación.

ACM ISBN 978-1-4503-4906-2/17/10...$15,00

Figura 1: Dado el video y la pregunta, nuestro modelo primero muestra el video a nivel de fotograma y a nivel de clip, luego extrae características de apariencia y movimiento, mientras que la pregunta se procesa a nivel de palabra para refinar la atención gradualmente. Los valores numéricos al lado de las líneas indican la atención refinada después de la última palabra procesada.

<!-- imagen -->

En comparación con la comprensión de imágenes [5, 27, 44, 45], la
La comprensión de los vídeos es más difícil. Los vídeos no solo contienen uno.más dimensión de tiempo, pero también incluyen canales de información adicionales como
como el audio y el texto en la mayoría de los casos. Para medir la comprensión
Capacidad de los modelos en vídeos, se proponen diferentes tareas intermedias.
como clasificación de videos [12, 19, 35, 41] y reconocimiento de acciones
[15, 20, 22, 24, 46]. Recientemente han surgido subtítulos de vídeo donde modelos
Es necesario describir el contenido más informativo de los videoclips utilizando
oraciones en lenguaje natural. La tarea gana mucho interés en el
comunidad de investigación y varios trabajos han mostrado resultados prometedores [17,
25, 40]. Aunque describir el vídeo usando lenguaje natural es cercano a
cómo lo perciben los humanos, los modelos existentes tienden a describir el video de una manera
frase corta, en la que pueden pasarse por alto muchos detalles importantes. Muchas métricas
como BLEU, METEOR, ROUGE y CIDEr han sido propuestos para el
evaluación de oraciones, pero todavía es difícil de evaluar debido a la
ambigüedad y variedad del lenguaje natural, así como las diferencias
entre las evaluaciones subjetivas [43].

Recientemente se propuso VideoQA, que sigue el patrón de imagen.
respuesta a preguntas (ImageQA) [2] excepto el tipo de medio. dado un
video y una pregunta, la tarea requiere que el modelo genere una respuesta.
Debido a que los videos y las preguntas son arbitrarios, el modelo que logra
VideoQA tiene la capacidad de analizar los videos sin restricciones y
preguntas, lo que indica una mejor comprensión de los vídeos. como lo he hecho
mencionado anteriormente, el modelo de subtítulos de vídeo tiende a describir la
video en oraciones generales al que le faltan detalles; en VideoQA, preguntas
puede involucrar diversos detalles del video, y las respuestas no pueden ser tan
generales. Para dar la respuesta, un modelo necesita analizar la pregunta.
cuidadosamente y se centra en la parte importante del vídeo. Además, desde
los detalles son siempre simples y concretos, la respuesta a la pregunta
siempre es mucho más corto que la descripción del vídeo, lo que hace que
la evaluación sea más fácil. Las propiedades anteriores también hacen que VideoQA sea mejor
elección para medir el progreso en la comprensión del vídeo. Un sistema que
tener éxito en ImageQA normalmente necesita una comprensión más detallada del
imagen y razonamiento complejo que un sistema que produce una imagen genérica
captions [2], que es lo mismo cuando se trata de vídeos.

Actualmente, los trabajos existentes sobre VideoQA son escasos ya que la tarea es
relativamente nuevo. [43] presenta cuatro modelos para resolver la tarea VideoQA. Todos
de estos modelos se extienden a partir de modelos para otras tareas como video
subtítulos e ImageQA. Aunque estos modelos han demostrado ser poderosos en su
tarea dedicada, simplemente extenderlos es inapropiado ya que pueden
debilitar o ignorar la información temporal de los videos, que es la más
característica distintiva en comparación con las imágenes estáticas. Además, los vídeos contienen
datos ricos en múltiples canales, y los modelos simplemente extendidos no pueden
aprovecharlos bien. Otra debilidad es que estos modelos siempre codifican
toda la cuestión como una sola característica, que puede no ser expresiva
suficiente para revelar la información contenida en la pregunta. Es más, algunos
Las palabras de la pregunta son importantes para dar la respuesta correcta, y
Estas palabras también pueden estar enterradas en una característica tan tosca.

En este artículo, proponemos un modelo de extremo a extremo para VideoQA. el modelo
Primero muestra el vídeo como una serie de fotogramas y clips, a partir de los cuales
Se extraen las características de apariencia y movimiento. Luego el modelo lee el
pregunta palabra por palabra y refina su atención sobre estas características conla interacción entre el nivel de fotograma y el nivel de clip. Cuando todas las palabras
de la pregunta se procesan, el modelo genera la optimización final
Atención que se puede utilizar para fusionar las características de apariencia y movimiento.
como la representación del vídeo. Tanto la pregunta más generalizada
En el procedimiento se utilizan la característica y la característica de palabra detallada. tomando
Como muestra la Figura 1 como ejemplo, el modelo lee la pregunta "¿qué
¿La señora vierte en la olla? secuencialmente y finalmente encuentra que el
El marco presentado en medio de los resultados de la muestra es el más relevante y
importante dar la respuesta "leche".

A modo de resumen, las principales aportaciones del artículo son las siguientes:

- Proponemos resolver la tarea VideoQA utilizando la información de apariencia y movimiento que se encuentra en los videos.
- Nuestro modelo propuesto refina la atención del video utilizando la característica de preguntas generales y la característica de palabras detalladas juntas como guía.
- Generamos dos conjuntos de datos y evaluamos nuestro modelo propuesto. Los extensos experimentos muestran que nuestro modelo logra resultados prometedores.

El resto del documento está organizado de la siguiente manera. Primero presentamos algunos
trabajos relacionados en la Sección 2. Los detalles de nuestro modelo propuesto son
explica en la Sección 3. En la Sección 4, describimos la configuración de
experimentos, presentar varios resultados y analizar los resultados en profundidad.
Finalmente, concluimos nuestro artículo en la Sección 5.

## 2 TRABAJOS RELACIONADOS

En esta sección repasamos brevemente algunos trabajos relacionados con la pregunta en vídeo.
respondiendo (VideoQA) e inspira el diseño de nuestro modelo. Desde VideoQA
Es una tarea bastante nueva, sólo quedan unos pocos trabajos a los que referirme. Aquí también nosotros
Considere dos tareas relacionadas que son los subtítulos de video y la pregunta sobre imágenes.
respondiendo (ImageQA).

## 2.1 Subtítulos de vídeo

Los subtítulos de vídeo tienen como objetivo generar oraciones que describan el contenido.
del vídeo. Existen numerosos trabajos dirigidos a esta tarea. [26]
Muestra varios fotogramas del vídeo y extrae la capa f c 7.
activaciones de la red neuronal convolucional (CNN) como característica para
cada cuadro. Después de la extracción de características, la media del modelo agrupa todas las
estas características en todo el video e ingresa las características agrupadas para
una red de memoria a corto plazo (LSTM) de dos capas. La red LSTM
genera una palabra en cada paso de tiempo, según las características del vídeo y el
palabra anterior hasta que genere la etiqueta de final de oración. [42] propuso
asistir además de las propuestas de objetos espaciotemporales en el vídeo, integrar
con clasificadores de imágenes de última generación, detectores de objetos, alto nivel
características semánticas (SVO) y utilizar la red neuronal recurrente (RNN) para
generar el título del vídeo.[38] propuso una novela 3D CNN-RNN
Arquitectura codificador-decodificador que captura espaciotemporal local.
información mediante el uso de la función 3D CNN. En lugar de significar agrupar los
características de video, el método atiende a cada característica al generar la
siguiente palabra de la oración. [6] propuso un modelo con memoria aumentada
atención que mejora el mecanismo de atención. El modelo utiliza
recuerdos de atenciones pasadas realizadas sobre el vídeo al pensar en
dónde atender en el paso de tiempo actual.

## 2.2 Respuesta a preguntas de imagen

Dada una imagen y una pregunta en lenguaje natural, la respuesta a la pregunta sobre la imagen
(ImageQA) requiere que el modelo proporcione una respuesta correcta. [37] propuesto
un modelo con atención espacial apilada sobre diferentes regiones del
imagen. La pregunta es procesada por la red CNN o LSTM para extraer unavector semántico como su característica. La imagen es procesada por la red VGG.
[23] y las activaciones en la última capa de agrupación se extraen como
características para regiones de imágenes. El modelo utiliza primero la función de pregunta para
consultar las características de la región de la imagen en la primera capa de atención visual,
luego combina la función de pregunta y la función de imagen recuperada para
forme un vector de consulta refinado y finalmente vuelva a consultar los vectores de imagen
en la segunda capa de atención. Dado que algunas preguntas no se pueden responder
Usando solo la imagen, [31] aporta conocimiento externo al modelo.
Además de la imagen y la pregunta, la modelo también recupera algunos datos.
de la base de conocimientos externa. Representaciones vectoriales de región
Se utilizan propuestas, títulos de imágenes junto con el conocimiento recuperado.
para formar una representación fusionada, luego la representación se envía a un LSTM
red que lee la pregunta y genera una respuesta. memoria dinámica
network [14] obtuvo una alta precisión en muchas tareas lingüísticas. [32]
Se mejoró el módulo de entrada de la red de memoria dinámica para consumir imágenes.
regiones como [37] e hizo que el modelo fuera capaz de realizar preguntas
respondiendo en imágenes. El modelo también utiliza un sistema recurrente cerrado bidireccional.
Red de Unidad (GRU) para actualizar las características de la región, lo que hace que la
Flujo de información entre regiones de la imagen. [33] propuso un modelo que
permite que cada palabra de la pregunta seleccione una región de imagen relacionada, luego
reúne la representación ponderada para generar la respuesta.

Figura 2: Nuestro modelo transforma las palabras mediante la capa de incrustación y manipula la atención en la Unidad de Atención y Memoria (AMU). El modelo procesa la pregunta palabra por palabra mientras AMU genera y refina la atención sobre la apariencia y las características de movimiento del video en cada paso de tiempo. Después de procesar todas las palabras, la atención final se utiliza para fusionar ambas características como representación del video. Junto con otros contextos, nuestro modelo genera la respuesta. La línea doble en la figura indica funciones en dos canales.

<!-- imagen -->

## 2.3 Respuestas a preguntas en vídeo

La respuesta a preguntas en vídeo (VideoQA) es una tarea relativamente nueva, en la que un
Se proporciona un video y una pregunta en lenguaje natural y el modelo necesita
para dar la respuesta correcta. [51] presenta un enfoque codificador-decodificador para
responder las preguntas de opción múltiple del video, pero la forma del
La pregunta es "completar espacios en blanco", que es diferente de la pregunta abierta.
Respuesta a las preguntas propuestas aquí. [43] es el primer trabajo en VideoQA que
nuestro conocimiento. El trabajo simplemente amplía varios modelos existentes desde
otras tareas como subtítulos de vídeo e ImageQA. Todo lo extendido
Los modelos extraen la característica de la pregunta utilizando la red LSTM.
La característica extraída es tosca y carece de la capacidad de revelar la
información contenida en palabras clave ya que el vector semántico único puede
ocultar la característica de palabras específicas. Además, la apariencia y
Las características de movimiento del video se extraen pero solo se fusionan de manera ingenua.
puesta en común. Creemos que los dos canales de funciones deberían explotarse con
interacción de manera más efectiva. Artículos recientes [39, 49, 50] también emplean el
Mecanismo de atención sobre vídeos. A diferencia de estos estudios, nuestro artículo
propone afinar la atención gradualmente utilizando tanto el enfoque grueso
función de pregunta y función de palabra detallada.

## 3 MÉTODOS

En esta sección, primero definimos varias anotaciones necesarias, luego
Presentar el marco de nuestro modelo propuesto en detalle.

Actualmente nos centramos en vídeos de corta duración y que tienenSólo unas pocas escenas. Dado el video V y la pregunta Q, el objetivo de
la respuesta a preguntas en video (VideoQA) es para dar la respuesta adecuada
Un. Dado que el vídeo siempre contiene muchos fotogramas por segundo, que son
redundante, se muestrea en cuadros y clips distribuidos uniformemente que
Puede pensarse como una representación comprimida de todo el vídeo. el
El clip consta de 16 fotogramas consecutivos que contienen el movimiento básico.
información. El número de marcos se elige para que sea compatible con el
extractor de funciones. Después del proceso de muestreo, el vídeo V es
representado como una serie de fotogramas y clips. El simplemente comprimido
La representación no sólo reduce los fotogramas redundantes que pueden aumentar
la carga de cálculo del modelo pero también mantiene la información de
el vídeo en todos los pasos de tiempo tanto como sea posible. La pregunta Q es
representados como una serie de palabras respectivamente. Para el mismo video, hay
Puede haber muchas preguntas diferentes, por lo que el modelo debe darse cuenta de lo que
hace la pregunta y encuentra la información necesaria en el vídeo. el
La longitud de la respuesta A no es muy larga ya que lo que plantea la pregunta es
Siempre específico y se puede responder en pocas palabras. La respuesta concisa
también facilita la evaluación, lo cual es importante para comparar
diferentes modelos y medir el progreso en la tarea.

Para superar la debilidad de los modelos propuestos en [43], proponemos un
modelo de extremo a extremo para VideoQA. Nuestro modelo utiliza la apariencia y
información de movimiento en el video y analiza la pregunta más
cuidadosamente. Como muestra la Figura 2, para un vídeo determinado, la apariencia y
Primero se extraen las características de movimiento del video, luego la pregunta es
Se analiza palabra por palabra y se refina la atención sobre estas características.
por Unidad de Memoria de Atención (AMU) en cada paso de tiempo. Después de la última palabra de
Se procesa la pregunta, el modelo genera la atención refinada para la
vídeo que es más relevante y valioso para responder a las preguntas específicas
pregunta. El modelo utiliza esta atención para fusionar apariencia y movimiento.
características y obtener la representación del vídeo. Para generar el
respuesta, otros contextos como información de preguntas y atención
La historia también se utiliza como referencia. Los detalles del modelo son
explicado como Extracción de funciones, Unidad de memoria de atención y Respuesta
Generación en la siguiente.

Figura 3: Los bloques de operación en AMU se despliegan según su orden de ejecución de izquierda a derecha en (a), y los detalles de la operación REF se presentan en (b). La línea doble en la figura indica funciones en dos canales.

<!-- imagen -->

## 3.1 Extracción de funciones

La extracción de características se ha estudiado en muchos campos de investigación [47, 48].
Los fotogramas del vídeo son imágenes estáticas que contienen diferentes objetos,
que ocupan gran parte de la información del vídeo. podemos extraer
características de apariencia de los marcos. Además de los objetos estáticos, el movimiento es
Otro canal de información contenida en vídeos que marca la diferencia.
entre vídeos e imágenes. Las características de movimiento se pueden extraer del
clips del vídeo que se componen de 16 fotogramas consecutivos. Nosotros
extraiga características en el nivel de fotograma y en el nivel de clip para obtener una serie de vectores
representaciones del vídeo. Existen otras características que podemos extraer.
del vídeo, como funciones de audio y funciones de texto. Actualmente, nosotros
Solo considere los canales de apariencia y movimiento ya que son
Disponible en todos los videos y suficiente para explicar nuestro modelo. Para preguntas,
Utilizamos la técnica de incrustación que se explica a continuación.Apariencia. Trabajos recientes [4, 11] muestran que las activaciones en lo más profundo
La capa de CNN se puede generalizar bien a otras tareas, lo que significa que
Las activaciones pueden representar la semántica de la imagen. muchos relacionados
Tareas como la detección y localización de objetos también se abordan con
este tipo de característica [7, 21]. En nuestro modelo, seleccionamos usar la red VGG.
[23] como extractor de características de apariencia a nivel de fotograma porque es
ampliamente utilizado y muestra resultados prometedores en la literatura. para un determinado
video, representamos sus características de apariencia como F a = [ f a 1 , f a 2 , .
. . , f a N ], donde N es el número de fotogramas muestreados en el vídeo y
el superíndice a indica apariencia.

Movimiento. [10] propone la red C3D para realizar reconocimiento de acciones y
muestra resultados prometedores en varios conjuntos de datos. Las activaciones en el
La capa más profunda de la red C3D también se utiliza en muchas tareas relacionadas con
videos [38, 42, 43] y muestran su capacidad para capturar la dinámica
información de vídeos. Por la misma razón que antes, utilizamos C3D.
Network como extractor de funciones de movimiento a nivel de clip. Para un vídeo determinado,
sus clips muestreados son procesados por la red C3D y el movimiento extraído
las características se representan como F m = [ f m 1 , f m 2 , . . . , f metro norte ] ,
donde N es el número de clips muestreados en el vídeo y el superíndice m
indica movimiento.

Pregunta. La pregunta se puede representar como una serie de palabras anotadas como
Q = [ q 1 , q 2 , . . . , q T ] . Hay muchas formas de codificar palabras.
como la codificación one-hot y la codificación de bolsa de palabras. Trabajos recientes [16,
18] representan una palabra mediante un vector continuo de longitud fija que se llama
incrustación de palabras. La representación es compacta y puede capturar la alta
Nivel de significado semántico de la palabra. En nuestro modelo, también utilizamos el
capa de incrustación para transformar la palabra q t a su incrustación semántica x t
.

Después de extraer las características del video y la pregunta, se crea una novela.
Se aplica un mecanismo de atención para generar atención sobre el vídeo.
características basadas en la pregunta.

## 3.2 Unidad de memoria de atención

En esta sección presentamos el proceso de atención, que es el núcleo de
nuestro modelo. Primero presentamos el proceso de atención en un contexto global.
perspectiva, luego explicamos los bloques de operación dentro de la Memoria de Atención
Unidad (AMU) en detalles.

Las palabras de la pregunta se procesan secuencialmente y una novela
Se aplica un mecanismo de atención a lo largo del procedimiento. Como podemos ver en
Figura 2, el modelo primero usa una capa de incrustación para transformar la entrada
palabra a su incrustación x t , que contiene la información semántica de la
palabra actual. La palabra incrustación x t luego se introduce en el LSTMq, el
estado oculto del cual se considera recordar la información sobre
la parte de la pregunta procesada. Tanto la palabra incrustación x t como la palabra oculta
El estado h q t de LSTMq se ingresa a AMU para generar y refinar el
atención sobre la apariencia y las características de movimiento.

Como se muestra en la Figura 2, AMU toma la incrustación de palabras actual, pregunta
información y funciones de video como entradas, luego realiza varios pasos
para refinar la atención sobre las funciones de video. Para mayor claridad, utilizamos el
Línea doble para representar las entidades que contienen dos canales. allí
Hay cuatro bloques de operación principales en AMU que son Atención (ATT), Canal
Fusión (CF), Memoria (LSTMa) y Refinado (REF). Junto con varios
operaciones de transformación, constituyen el proceso gradualmente refinado
Mecanismo de atención de nuestro modelo propuesto. Primero damos una visión general de
el mecanismo de atención realizado en AMU, luego mostrar los detalles concretosde las operaciones involucradas.

En la Figura 3, desenrollamos los bloques de operación dentro de AMU de acuerdo con su
orden de ejecución para mayor claridad. ATT1 realiza la atención inicial sobre F
basado en la incrustación de palabras actual x t y asiste a las funciones de video
relacionado con la palabra actual. La suma ponderada de las características de apariencia p a t
y las características de movimiento p m t son luego fusionadas por CF, lo que asigna importancia
puntuación para cada canal y obtener la representación fusionada intermedia de
vídeo u t . El estado oculto h q t de LSTMq, el generado previamente
representación de vídeo v t -1 y la presentación de vídeo intermedia u t
se suman para formar la entrada de LSTMa, que recuerda todos
realizó operaciones de atención. ATT2 usa h a t para realizar el segundo
atención sobre F . La primera atención pesa 1 t y la segunda
atención los pesos de 2 t están refinados en REF y en la representación en vídeo
Se genera v t que se utilizará en el siguiente paso de tiempo. A continuación,
Explicaremos los bloques de cada operación de forma concreta.

Atención. Dada una pregunta sobre el video, solo un subconjunto de los fotogramas.
o clips están relacionados la mayor parte del tiempo. Son estas características las que
Es más útil dar la respuesta. El mecanismo de atención tiene como objetivo asignar
pesos a las características de apariencia y movimiento del video por separado y
atiende a características útiles mediante una combinación ponderada de ellas. hay
dos operaciones de atención ATT1 y ATT2 en AMU. Tomamos ATT1 como
Ejemplo para explicar la formulación de la operación de atención. Como podamos
Como se ve en la Figura 2, ATT1 usa la palabra incrustación x t para realizar la
atención sobre las funciones de vídeo F . Aquí omitimos la notación de
apariencia o movimiento por simplicidad. La operación se realiza en cada
canal de las características. El mecanismo de atención se puede formular como
sigue:

<!-- fórmula-no-decodificada -->

Los pesos a i reflejan la relevancia entre la palabra actual y la
En la característica, W f y W x se utilizan para transformar incrustaciones de palabras y
funciones de vídeo en el mismo espacio de incrustación subyacente. con el
peso de atención a i, la característica fusionada pt se calcula de la siguiente manera:

<!-- fórmula-no-decodificada -->

El p t es la representación del vídeo al que asiste la palabra actual de
la pregunta. El ATT1 aquí mejora la influencia de la palabra actual cuando
respondiendo la pregunta. Más tarde ATT2 usará h a t para realizar otro
operación de atención que también genera segundos pesos de atención.

Fusión de canales. Después de obtener las características p t, que en realidad es
compuesto de p a t y p m t , las dos características se fusionan para formar una
representación de vídeo intermedia u t . Dado que las palabras en una pregunta pueden
se correlacionan con la apariencia y el movimiento en diferentes intensidades, el modelo
utiliza la palabra actual para asignar puntuaciones a ambos canales de características y
los fusiona de la siguiente manera:

<!-- fórmula-no-decodificada -->

Las intensidades calculadas son dos escalares s a t , s m t y el fusionado
representación u t absorbe información tanto de la apariencia como del movimiento
Canales del vídeo según la palabra actual.

Memoria. El modelo procesa una palabra de la pregunta y realiza dos
operaciones de atención ATT1 y ATT2 en cada paso de tiempo. Usamos LSTMa para
controlar la entrada de la operación de segunda atención y recordar el
Historial de atención. El modelo ya ha generado el intermedio.
representación u t del vídeo basada en la palabra actual. ahora traemos
la información de la pregunta en consideración. Desde el estado oculto h q
t del LSTMq contiene parte de la pregunta que ha sido procesadaya, el modelo lo usa al igual que el video intermedio
representación u t y la representación de vídeo refinada v t -1 del último
paso de tiempo para formar la entrada de LSTMa. La salida h a t se utiliza para realizar
segunda operación de atención ATT2.

Refinar. Después de ejecutar ATT2, el modelo genera el segundo
peso a 2 t sobre F . Tanto el de 1 t como el de 2 t se utilizan para refinar el
atención. El detalle de REF se representa en la Figura 3 y se puede
formulado de la siguiente manera:

<!-- fórmula-no-decodificada -->

t

Aquí д t en realidad incluye д a t y д m t de apariencia y movimiento,
y v t es la representación fusionada final del vídeo en el paso de tiempo t.

Con el proceso de atención anterior, el modelo utiliza palabras detalladas
información e información de preguntas generales para refinar gradualmente
la atención sobre la apariencia y las características de movimiento del video. el
La atención con la incorporación de palabras actuales puede mejorar la palabra clave.
información que puede estar oculta en una característica de vector único del
pregunta, y la información de la pregunta puede brindar una orientación más general
al fusionar estos rasgos y afinar la atención. Después de la UMA
procesó todas las palabras de la pregunta, la representación refinada del video
se genera cuál es más relevante y significativo para responder a las
pregunta.

## 3.3 Generación de respuestas

En esta sección, presentamos los dos tipos de métodos de generación de respuestas.
que se utilizan comúnmente en la literatura.

En el paso de tiempo T, después de procesar la última palabra de la pregunta,
obtenga la representación fusionada de video v T. También tenemos dos más.
información de contexto. El vector de memoria c q T de la pregunta LSTMq
contiene información sobre la pregunta y el vector de memoria c a T de
AMU contiene información sobre el historial de atención. Usamos estos tres
aspectos de la información para generar la respuesta.

Podemos preparar un conjunto de respuestas predefinido y el Generador puede ser un simple
clasificador softmax. La respuesta se elige de la siguiente manera:

<!-- fórmula-no-decodificada -->

t

El Generador también puede ser la red LSTM que se usa comúnmente en
tarea de generación de oraciones. La pregunta información c q T y atención.
El historial c a T se puede utilizar para inicializar la red LSTM mientras el
La representación de vídeo refinada v T sirve como primera entrada. Cada palabra de
la respuesta se puede generar como en la Ecuación (5), excepto que la elección ha terminado
todo el vocabulario.

Nuestro modelo propuesto es un modelo de extremo a extremo que refina su atención.
sobre la apariencia y las características de movimiento del video basándose en el análisis de la
pregunta con cuidado. Adoptamos varios experimentos para evaluar nuestra
modelo propuesto y la siguiente sección describe el modelo concreto
configuración del experimento.

## 4 EXPERIMENTOS

En esta sección, adoptamos varios experimentos sobre preguntas en video.
conjuntos de datos de respuesta (VideoQA) construidos a partir de videos disponibles públicamente
subtitular conjuntos de datos. Los resultados muestran la efectividad de la propuesta.
modelo.

## 4.1 Preparación de datos

Dado que VideoQA es una tarea relativamente nueva, no hay información pública disponible.
conjunto de datos. [9] presenta un método que puede generar pares de preguntas y respuestas.
de las descripciones automáticamente. Generamos dos conjuntos de datos basados en esto.
método mediante la conversión de subtítulos de vídeo en conjuntos de datos existentes para cuestionar
parejas de respuesta.

MSVD-QA. El conjunto de datos se basa en la descripción del vídeo de Microsoft Research.
Corpus [3] que se utiliza en muchos experimentos de subtítulos de vídeo. El MSVD-
El conjunto de datos de control de calidad tiene un número total de 1970 videoclips y 50 505 preguntas.
responder en parejas. Dividimos el conjunto de datos en función de los vídeos del conjunto de entrenamiento.toma el 61%, el conjunto de validación toma el 13% y el conjunto de prueba toma el 26% del total
número de vídeos. La Tabla 1 muestra las estadísticas del conjunto de datos MSVD-QA.

MSRVTT-QA. El conjunto de datos se basa en el conjunto de datos MSR-VTT [34] que es
más grande y tiene escenas más complejas. El conjunto de datos contiene 10.000 videoclips.
y 243.000 pares de preguntas y respuestas. Seguimos la división de datos en MSR-VTT
conjunto de datos que es 65% para el conjunto de entrenamiento, 5% para el conjunto de validación y 30% para
conjunto de prueba. La Tabla 2 muestra las estadísticas del conjunto de datos MSRVTT-QA.

## 4.2 Detalles del modelo

Ya presentamos el marco de nuestro modelo propuesto en la Sección 3,
Aquí mostramos las configuraciones concretas del modelo adoptado en los experimentos.

Usamos la red VGG para extraer funciones del canal de apariencia y
Red C3D para extraer características del canal de movimiento. Dado un vídeo,
Primero tomamos una muestra de 20 fotogramas y clips distribuidos uniformemente, respectivamente.
Luego se aplican las redes VGG y C3D a estos fotogramas y clips, desde
que las activaciones de la última capa completamente conectada se extraen como
características correspondientes. El número de funciones en ambos canales es 20.
y la dimensión de las características es 4.096.

La pregunta se transforma por la capa de incrustación, que en realidad es una
Matriz que contiene representaciones vectoriales de todas las palabras del vocabulario.
Utilizamos la incrustación GloVe de 300 dimensiones previamente entrenada [18] para
inicializamos nuestra capa de incrustación. El guante utilizado está entrenado según
Wikipedia 2014 y Gigaword 5, que contiene 400.000 vocabulario. podamos
la incrustación de GloVe para que coincida con el tamaño del vocabulario. Para palabras en nuestro
El vocabulario que no apareció en GloVe, promediamos todas las demás palabras existentes.
incrustaciones como sus incrustaciones. Elegimos que el tamaño de LSTMq sea 300
que coincide con la dimensión de la palabra incrustación.

Tabla 1: Estadística del MSVD-QA.|       |   Vídeo |   Par de control de calidad | Tipo de pregunta | Tipo de pregunta | Tipo de pregunta | Tipo de pregunta | Tipo de pregunta ||-------|---------|-----------|-----------------|-----------------|-----------------|-----------------|-----------------|
|       |         |           | qué | quien | cómo | cuando | donde || Tren |   1.200 |    30.933 | 19.485 | 10.479 | 736 | 161 | 72 || vale |     250 |     6.415 | 3.995 | 2.168 | 185 | 51 | 16 || Prueba |     520 |    13.157 | 8.149 | 4.552 | 370 | 58 | 28 || Todo |   1.970 |    50.505 | 31.629 | 17.199 | 1.291 | 270 | 116 |## Tabla 2: Estadística del MSRVTT-QA.|       |   Vídeo |   Par de control de calidad | Tipo de pregunta | Tipo de pregunta | Tipo de pregunta | Tipo de pregunta | Tipo de pregunta ||-------|---------|-----------|-----------------|-----------------|-----------------|-----------------|-----------------|
|       |         |           | qué | quien | cómo | cuando | donde || Tren |   6.513 |   158.581 | 108.792 | 43.592 | 4.067 | 1.626 | 504 || vale |     497 |    12.278 | 8.337 | 3.439 | 344 | 106 | 52 || Prueba |   2.990 |    72.821 | 49.869 | 20.385 | 1.640 | 677 | 250 || Todo |  10.000 |   243.680 | 166.998 | 67.416 | 6.051 | 2.409 | 806 |La AMU realiza operaciones de atención basadas en representaciones de la
vídeo y la pregunta. Dado que los dos tipos de representación pueden variar
En tamaño, elegimos 256 como tamaño de dimensión común para AMU. Ambos vídeos
Las características y las incrustaciones de palabras se asignan a este espacio común subyacente.
antes de realizar más cálculos. El tamaño del LSTMa también se establece en 256.

Aunque la respuesta abierta es más natural, requiere más tiempo y
recursos de cálculo para adoptar el experimento. Por lo tanto usamos 1000 vías
selección de softmax del conjunto de respuestas predefinido para generar la respuesta
como lo hacen muchos trabajos anteriores [2, 43] al responder preguntas.

## 4.3 Métodos de referencia

A modo de comparación, elegimos ampliar tres modelos básicamente como [43]
con algunos cambios menores como base. Estos modelos reflejan la
poder continuamente fortalecido.

Modelo VQA extendido (E-VQA). El modelo utiliza una red LSTM para codificar.
todas las palabras de la pregunta y otra red LSTM diferente para codificar
los fotogramas del vídeo. La representación de la pregunta y el vídeo.
Luego se fusionan como una representación uniforme, que se utiliza para decodificar la
respuesta. El modelo considera la naturaleza secuenciada del vídeo y
pregunta.

Modelo de Atención Suave Extendida (E-SA). El modelo primero codifica las palabras.
en la pregunta usando una red LSTM, entonces la representación codificada es
Se utiliza para atender a las características de los fotogramas de vídeo. Tanto la pregunta como
Se utilizan representaciones de vídeo ponderadas para generar la respuesta. el modelo
agrega la capacidad de seleccionar cuadros importantes según la pregunta.

Redes de memoria extendidas de extremo a extremo (E-MN). El modelo utiliza el bidireccional.
Red LSTM para actualizar las representaciones de cuadros del video. el
Las representaciones actualizadas se asignan a la memoria y la pregunta.
La representación se utiliza para realizar múltiples pasos de inferencia para generar.
la respuesta. El modelo no sólo tiene las habilidades de los dos primeros modelos sino que
También aumenta y mejora el procedimiento de inferencia.

Los modelos anteriores no tienen una forma dedicada de procesar la apariencia.
e información de movimiento del vídeo. Las características de los diferentes canales.
son medias agrupadas entre los fotogramas y clips correspondientes.

## 4.4 Detalles de la formación

Implementamos nuestros modelos y modelos de referencia utilizando TensorFlow [1], un
marco de gráfico de cálculo de flujo de datos que sirve para el aprendizaje profundo
método muy bien.

Para ambos conjuntos de datos, elegimos las K = 1000 respuestas más frecuentes como
el conjunto de respuestas, que sigue la configuración en [2]. También seleccionamos varios
palabras más frecuentes del conjunto de entrenamiento del conjunto de datos como vocabulario.
El tamaño del vocabulario de MSVD-QA es 4000, mientras que el de MSRVTT-QA es 8000.

Utilizamos descenso de gradiente estocástico de mini lotes para optimizar los modelos y
Adam [13] con su tasa de aprendizaje predeterminada de 0,001 como optimizador. Nosotros
utilice el tamaño de lote 32 para MSVD-QA y 64 para MSRVTT-QA. todos los
Los modelos se entrenan como máximo 30 épocas con parada temprana. para manejar el
preguntas de diferente longitud de manera eficiente, dividimos las preguntas en
varios grupos según la longitud de la pregunta. El número de cubos utilizados.
en MSVD-QA es 4. En MSRVTT-QA, el número es 5 ya que MSRVTT-QA tiene más
preguntas más largas. En cada cubo, las preguntas se rellenan hasta el final.
de la pregunta más larga en ese grupo. La función de pérdida de todos los modelos.
se define de la siguiente manera:

<!-- fórmula-no-decodificada -->

La primera parte es la pérdida logarítmica, donde N es el tamaño del lote, M es
el número de respuestas posibles, y i j es un indicador binario de sio no la respuesta j es la respuesta correcta, por ejemplo i, y p i j es la
probabilidad de asignar la respuesta j al ejemplo i mediante el modelo. el segundo
parte es la regularización L2 en mínimos cuadrados donde w k representa la
peso del modelo y λ 1 es el hiperparámetro que controla la importancia de
la regularización. El término de regularización se utiliza para evitar que el modelo
por sobreajuste.

Para nuestro modelo propuesto, dado que utiliza los dos canales explícitamente,
También agregue otro elemento en la Ecuación (6) para alentar al modelo a utilizar
las características de todos los canales de videos. El artículo se puede definir como:

<!-- fórmula-no-decodificada -->

Donde N es el tamaño del lote, s a i y s m i denota la puntuación de importancia
asignado a cada canal por ejemplo i finalmente. El artículo se agrega al
función de pérdida original con coeficiente λ 2 al entrenar nuestra propuesta
modelo específicamente.

## 4.5 Resultados y Análisis

Evaluamos tres modelos de referencia y nuestro modelo propuesto en MSVD-QA y
MSRVTT-QA. Las precisiones se presentan en la Tabla 3 y la Tabla 4.

Para ambos conjuntos de datos, nuestro modelo propuesto logra precisiones generales más altas.
que otros modelos de referencia, lo que indica la eficacia de nuestro
modelo. Hay cinco tipos de preguntas en ambos conjuntos de datos, por lo que también
informar la precisión en cada tipo de pregunta. Nuestro modelo logra mayores
Las precisiones en cuestión escriben qué y quién en ambos conjuntos de datos, pero el
El rendimiento en otros tipos de preguntas es ligeramente diferente. Como se muestra en
Tabla 1 y Tabla 2, preguntas de tipo qué y quién componen la mayor parte de los
preguntas en ambos conjuntos de datos y contienen las respuestas más diversas. el otro
Tres tipos de preguntas tienen respuestas muy limitadas que no son generales.
suficiente para reflejar el rendimiento de nuestro modelo.

Tabla 3: Resultados del experimento con el conjunto de datos MSVD-QA.| Métodos |   qué |   quien |   cómo |   cuando |   donde |   TODOS ||-----------|--------|-------|-------|--------|---------|-------|
| E-VQA     |  0.097 | 0.422 | 0.838 |  0.724 |   0.536 | 0.233 || E-SA |  0,150 | 0,451 | 0,838 |  0,655 |   0,322 | 0,276 || E-MN |  0,129 | 0,465 | 0,803 |  0,707 |   0,500 | 0,267 || Nuestro modelo |  0,206 | 0,475 | 0,835 |  0,724 |   0,536 | 0,320 |Tabla 4: Resultados del experimento con el conjunto de datos MSRVTT-QA.| Métodos |   qué |   quien |   cómo |   cuando |   donde |   TODOS ||-----------|--------|-------|-------|--------|---------|-------|
| E-VQA     |  0.189 | 0.387 | 0.835 |  0.705 |   0.292 | 0.264 || E-SA |  0,220 | 0,416 | 0,796 |  0,731 |   0,332 | 0,293 || E-MN |  0,234 | 0,418 | 0,837 |  0,708 |   0,276 | 0,304 || Nuestro modelo |  0,262 | 0,430 | 0,802 |  0,725 |   0,300 | 0,325 |Para el análisis cualitativo, presentamos varios ejemplos de ambos
conjuntos de datos. En la Figura 4, las dos primeras filas muestran cuatro ejemplos correctamente.
respondido por nuestro modelo de MSVD-QA, y las dos últimas filas muestran cuatro
ejemplos de MSRVTT-QA. El vídeo está representado en 5 concatenados.
fotogramas que se muestrean distribuidos uniformemente en el vídeo original para
ahorrando espacio. Podemos ver que el modelo logra responder preguntas que
involucran objetos y acciones. Como se mencionó anteriormente en la Sección 4, el
Los videos en MSRVTT-QA tienen más cambios de escena en comparación con los videos en MSVD-
Control de calidad, que también se puede verificar visualmente en los ejemplos.

Tener una comprensión intuitiva de cómo el modelo propuesto responde a las
pregunta, también analizamos la atención de dos ejemplos de MSRVTT-QA
en la Figura 5. El vídeo se presenta en 20 fotogramas y 20 clips en cada uno.
canal que coincida exactamente con la configuración del experimento. buceamos
en el interior del modelo y visualizar las puntuaciones asignadas a cada
canal a la izquierda del video. También presentamos los pesos de atención.
dentro de cada canal generado por el modelo. En el primer ejemplo, el
La pregunta es "¿Qué está recogiendo Calamardo?" y el video es una caricatura.
Nuestro modelo da la respuesta correcta "teléfono" para esta pregunta mediante
centrarse en el canal de apariencia y generar altos pesos de atención
en marcos que contienen un teléfono. En el segundo ejemplo, la pregunta es
"¿Qué está haciendo un hombre?" y el vídeo se compone de tres escenas principales
que son un auto en movimiento, un tablero y un hombre sentado en el auto. Nosotros
Podemos ver que dado que la pregunta es sobre acción, nuestro modelo
asigna una puntuación más alta al canal de movimiento que a la apariencia
canal. En el canal de movimiento, nuestro modelo se centra en los tres primeros
clips que contienen el movimiento de un coche en movimiento. Incluso la apariencia
El canal es menos utilizado, nuestro modelo todavía se centra en fotogramas que contienen el
hombre que es el tema de la pregunta.

Del análisis detallado de las atenciones refinadas en ambos ejemplos, podemos
encontramos que nuestro modelo propuesto explota ambos canales de videos y
selecciona el canal y las funciones útiles al responder la pregunta.

Figura 4: Los ejemplos respondidos correctamente de ambos conjuntos de datos.

<!-- imagen -->

Figura 5: Visualización de la atención para dos ejemplos. a representa apariencia y m representa movimiento.

<!-- imagen -->

<!-- imagen -->

## 5 CONCLUSIONES

En este artículo, desarrollamos un modelo de extremo a extremo que explota la
información de los canales de apariencia y movimiento en el video cuando
realizando la tarea VideoQA. Para responder a la pregunta, el modelo extrae
características de apariencia y movimiento del video y refina gradualmente su
atención a estas características basándose tanto en preguntas generales
característica y característica de palabra detallada. Nuestro modelo se puede ampliar fácilmente
incorporar más canales de información como el texto y el audio
canales. Para evaluar nuestro modelo, realizamos experimentos extensos en dos
conjuntos de datos. Los resultados muestran que nuestro modelo puede lograr un mejor rendimiento.
en comparación con otros modelos básicos. También realizamos varios detalles
análisis, mostrando que nuestro modelo efectivamente se centra en las necesidades
información de diferentes canales. En el futuro combinaremos nuestra
Método de comprensión de videos con comentarios de los usuarios (por ejemplo, calificaciones y clics).
a aplicaciones posteriores orientadas al usuario, como aplicaciones personalizadas
recomendación [8, 30].

##AGRADECIMIENTOS

Este trabajo fue apoyado por el programa 973 (2015CB352302), ZhejiangFundación de Ciencias Naturales (LZ17F020001), Fundación Nacional de Ciencias Naturales
Fundación de China (61572431, 61602405, U1611461, U1611461), China
Centro de Conocimiento de Ciencias de la Ingeniería y Programa Clave de Zhejiang
Provincia (2015C01027), Programa Nacional Clave de Investigación y Desarrollo de
China (SQ2017YFGX030023).

## REFERENCIAS

- [1] Martin Abadi, Ashish Agarwal, Paul Barham, Eugene Brevdo, Zhifeng Chen, Craig Citro, Greg S Corrado, Andy Davis, Jeffrey Dean, Matthew Devin, et al. 2016. Tensorflow: aprendizaje automático a gran escala en sistemas distribuidos heterogéneos. Preimpresión de arXiv arXiv:1603.04467 (2016).
- [2] Stanislaw Antol, Aishwarya Agrawal, Jiasen Lu, Margaret Mitchell, Dhruv Batra, C Lawrence Zitnick y Devi Parikh. 2015. Vqa: Respuesta visual a preguntas. En ICCV.
- [3] David L. Chen y William B. Dolan. 2011. Recopilación de datos altamente paralelos para la evaluación de paráfrasis. En LCA.
- [4] Jingyuan Chen, Hanwang Zhang, Xiangnan He, Liqiang Nie, Wei Liu y TatSeng Chua. 2017. Filtrado colaborativo atento: recomendación multimedia con atención a nivel de elementos y componentes. En SIGIR
- [5] Long Chen, Hanwang Zhang, Jun Xiao, Liqiang Nie, Jian Shao y Tat-Seng Chua. 2017. SCA-CNN: Atención espacial y por canales en redes convolucionales para subtítulos de imágenes. En CVPR
- [6] Rasool Fakoor, Abdel-Rahman Mohammed, Margaret Mitchell, Sing Bing Kang y Pushmeet Kohli. 2016. Modelado de atención con memoria aumentada para vídeos. Preimpresión de arXiv arXiv:1611.02261 (2016).
- [7] Ross Girshick, Jeff Donahue, Trevor Darrell y Jitendra Malik. 2014. Jerarquías de funciones ricas para una detección precisa de objetos y segmentación semántica. En CVPR
- [8] Xiangnan He, Lizi Liao, Hanwang Zhang, Liqiang Nie, Xia Hu y Tat-Seng Chua. 2017. Filtrado colaborativo neuronal. En WWW
- [9] Michael Heilman y Noah A. Smith. 2009. Generación de consultas mediante clasificaciones y transformaciones sobregeneradas. Informe técnico. CARNEGIE-MELLON UNIV PITTSBURGH PA INSTITUTO DE TECNOLOGÍAS DE IDIOMAS.
- [10] Shuiwang Ji, Wei Xu, Ming Yang y Kai Yu. 2013. Redes neuronales convolucionales 3D para el reconocimiento de acciones humanas. TPAMI (2013).
- [11] Yangqing Jia, Evan Shelhamer, Jeff Donahue, Sergey Karayev, Jonathan Long, Ross Girshick, Sergio Guadarrama y Trevor Darrell. 2014. Caffe: Arquitectura convolucional para una rápida incorporación de funciones. En ACMMM.
- [12] Andrew Karpathy, George Toderici, Sanketh Shetty, Thomas Leung, Rahul Sukthankar y Li Fei-Fei. 2014. Clasificación de vídeos a gran escala con redes neuronales convolucionales. En CVPR
- [13] Diederick Kingma y Jimmy Ba. 2014. Adam: un método para la optimización estocástica. Preimpresión de arXiv arXiv:1412.6980 (2014).
- [14] Ankit Kumar, Ozan Irsoy, Peter Ondruska, Mohit Iyyer, James Bradbury, Ishaan Gulrajani, Victor Zhong, Romain Paulus y Richard Socher. 2016. Pregúntame cualquier cosa: redes de memoria dinámica para el procesamiento del lenguaje natural. En ICML.
- [15] Quoc V Le, Will Y Zou, Serena Y Yeung y Andrew Y Ng. 2011. Aprendizaje de características espacio-temporales invariantes jerárquicas para el reconocimiento de acciones con análisis de subespacio independiente. En CVPR
- [16] Thomas Mikolov, Ilya Sutskever, Kai Chen, Greg S Corrado y Jeff Dean. 2013. Representaciones distribuidas de palabras y frases y su composicionalidad. En NIPS
- [17] Pingbo Pan, Zhongwen Xu, Yi Yang, Fei Wu y Yueting Zhuang. 2016. Codificador neuronal recurrente jerárquico para representación de video con aplicación a subtítulos. En CVPR- [18] Jeffrey Pennington, Richard Socher y Christopher D Manning. 2014. Guante: Vectores globales para representación de palabras. En EMNLP.
- [19] Matthew J. Roach, JD Mason y Mark Pawlewski. 2001. Clasificación de géneros de vídeo mediante dinámica. En ICASSP.
- [20] Paul Scovanner, Saad Ali y Mubarak Shah. 2007. Un descriptor de tamizado tridimensional y su aplicación al reconocimiento de acciones. En ACMMM.
- [21] Pierre Sermanet, David Eigen, Xiang Zhang, Michael Mathieu, Rob Fergus y Yann LeCun. 2013. Overfeat: reconocimiento, localización y detección integrados mediante redes convolucionales. Preimpresión de arXiv arXiv:1312.6229 (2013).
- [22] Karen Simonyan y Andrew Zisserman. 2014. Redes convolucionales de dos flujos para el reconocimiento de acciones en videos. En NIPS.
- [23] Karen Simonyan y Andrew Zisserman. 2014. Redes convolucionales muy profundas para el reconocimiento de imágenes a gran escala. Preimpresión de arXiv arXiv:1409.1556 (2014).
- [24] Khurram Soomro, Amir Roshan Zamir y Mubarak Shah. 2012. UCF101: Un conjunto de datos de 101 clases de acciones humanas tomadas de videos en la naturaleza. Preimpresión de arXiv arXiv:1212.0402 (2012).
- [25] Subhashini Venugopalan, Marcus Rohrbach, Jeffrey Donahue, Raymond Mooney, Trevor Darrell y Kate Saenko. 2015. Secuencia a secuencia-vídeo a texto. En ICCV.
- [26] Subhashini Venugopalan, Huijuan Xu, Jeff Donahue, Marcus Rohrbach, Raymond Mooney y Kate Saenko. 2014. Traducir vídeos a lenguaje natural utilizando redes neuronales recurrentes profundas. Preimpresión de arXiv arXiv:1412.4729 (2014).
- [27] Meng Wang, Weijie Fu, Shijie Hao, Hengchang Liu y Xindong Wu. 2017. Aprendizaje en Big Graph: inferencia de etiquetas y regularización con jerarquía de anclaje. TKDE (2017).
- [28] Meng Wang, Richang Hong, Guangda Li, Zheng-Jun Zha, Shuicheng Yan y TatSeng Chua. 2012. Resumen de vídeos web impulsado por eventos mediante localización de etiquetas e identificación de teclas. TMM (2012).
- [29] Meng Wang, Xian-Sheng Hua, Richang Hong, Jinhui Tang, Guo-Jun Qi y Yan Song. 2009. Anotación de vídeo unificada mediante aprendizaje multigráfico. TCSVT (2009).
- [30] Xiang Wang, Xiangnan He, Liqiang Nie y Tat-Seng Chua. 2017. Item Silk Road: Recomendar elementos de dominios de información a usuarios sociales. En SIGIR.
- [31] Qi Wu, Peng Wang, Chunhua Shen, Anthony Dick y Anton van den Hengel. 2016. Pregúntame cualquier cosa: respuesta visual de preguntas de forma libre basada en conocimientos de fuentes externas. En CVPR.
- [32] Caiming Xiong, Stephen Merity y Richard Socher. 2016. Redes de memoria dinámica para responder preguntas visuales y textuales. En ICML.
- [33] Huijuan Xu y Kate Saenko. 2016. Pregunte, atienda y responda: Explorando la atención espacial guiada por preguntas para responder preguntas visualmente. En ECCV.
- [34] Jun Xu, Tao Mei, Ting Yao y Yong Rui. 2016. MSR-VTT: un gran conjunto de datos de descripción de vídeo para unir vídeo y lenguaje. En CVPR.
- [35] Li-Qun Xu y Yongmin Li. 2003. Clasificación de vídeos mediante características espacio-temporales y PCA. En ICME.
- [36] Xun Yang, Meng Wang, Richang Hong, Qi Tian y Yong Rui. 2017. Mejora de la reidentificación de personas en un subespacio autodidacta. Preimpresión de arXiv arXiv:1704.06020 (2017).
- [37] Zichao Yang, Xiaodong He, Jianfeng Gao, Li Deng y Alex Smola. 2016. Redes de atención apiladas para responder preguntas sobre imágenes. En CVPR.
- [38] Li Yao, Atousa Torabi, Kyunghyun Cho, Nicolas Ballas, Christopher Pal, Hugo Larochelle y Aaron Courville. 2015. Describir videos explotando la estructura temporal. En ICCV.- [39] Yunan Ye, Zhou Zhao, Yimeng Li, Long Chen, Jun Xiao y Yueting Zhuang. 2017. Respuesta a preguntas en vídeo mediante el aprendizaje en red de atención aumentada y atribuida. En SIGIR.
- [40] Haonan Yu, Jiang Wang, Zhiheng Huang, Yi Yang y Wei Xu. 2016. Subtítulos de párrafos de vídeo utilizando redes neuronales recurrentes jerárquicas. En CVPR.
- [41] Joe Yue-Hei Ng, Matthew Hausknecht, Sudheendra Vijayanarasimhan, Oriol Vinyals, Rajat Monga y George Toderici. 2015. Más allá de fragmentos breves: redes profundas para la clasificación de vídeos. En CVPR.
- [42] Mihai Zanfir, Elisabeta Marinoiu y Cristian Sminchisescu. 2016. Modelos de atención espaciotemporal para subtítulos de vídeo fundamentados. En ACCV.
- [43] Kuo-Hao Zeng, Tseng-Hung Chen, Ching-Yao Chuang, Yuan-Hong Liao, Juan Carlos Niebles y Min Sun. 2017. Aprovechar las descripciones de los vídeos para aprender a responder preguntas en vídeo. En AAAI.
- [44] Hanwang Zhang, Xindi Shang, Huanbo Luan, Meng Wang y Tat-Seng Chua. 2016. Aprender de la inteligencia colectiva: aprendizaje de funciones utilizando imágenes y etiquetas sociales. TOM (2016).
- [45] Hanwang Zhang, Zheng-Jun Zha, Yang Yang, Shuicheng Yan, Yue Gao y TatSeng Chua. 2013. Jerarquía semántica aumentada por atributos: hacia la reducción de la brecha semántica y la brecha de intención en la recuperación de imágenes. En ACMMM.
- [46] Songyang Zhang, Xiaoming Liu y Jun Xiao. 2017. Sobre características geométricas para el reconocimiento de acciones basado en esqueletos utilizando redes LSTM multicapa. En WACV.
- [47] Zhou Zhao, Lu Hanqing, Deng Cai, Xiaofei He y Yueting Zhuang. 2016. Codificación dispersa multimodal parcial mediante regularización de estructura de similitud adaptativa. En ACMMM.
- [48] Zhou Zhao, Xiaofei He, Deng Cai, Lijun Zhang, Wilfred Ng y Yueting Zhuang. 2016. Selección de funciones regularizadas gráficamente con reconstrucción de datos. En TKDE.
- [49] Zhou Zhao, Jinghao Lin, Xinghua Jiang, Deng Cai, Xiaofei He y Yueting Zhuang. 2017. Respuesta a preguntas en vídeo a través del aprendizaje en red jerárquica de atención de doble nivel. En ACMMM.
- [50] Zhou Zhao, Qifan Yang, Deng Cai, Xiaofei He y Yueting Zhuang. 2017. Respuesta a preguntas en vídeo a través de redes jerárquicas de atención espacio-temporal. En IJCAI.
- [51] Linchao Zhu, Zhongwen Xu, Yi Yang y Alexander G Hauptmann. 2015. Descubriendo el contexto temporal para preguntas y respuestas en vídeo. Preimpresión de arXiv arXiv:1511.04670 (2015).