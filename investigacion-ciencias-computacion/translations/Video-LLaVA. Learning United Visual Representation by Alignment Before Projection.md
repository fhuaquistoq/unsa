## Video-LLaVA: Aprendizaje de la representación visual unida mediante alineación antes de la proyección

Bin Lin 1, Yang Ye 1, Bin Zhu 1, Jiaxi Cui 4, Munang Ning 1,2,3,
Peng Jin 1,2,3, Li Yuan 1,2,3

1 2 Laboratorio Peng Cheng,

Programa preferido de IA para la ciencia (AI4S), Universidad de Pekín, Shenzhen
escuela de posgrado,

Escuela de Graduados de Shenzhen de la Universidad de Pekín, 3 4 PandaVilla Tech Limited

Correspondencia:

yuanli-ece@pku.edu.cn

GitHub:

https://github.com/PKU-YuanGroup/Video-LLaVA

## Resumen

El modelo de visión-lenguaje de gran tamaño (LVLM) ha mejorado el rendimiento de
diversas tareas posteriores en la comprensión del lenguaje visual. La mayoría existente
Los enfoques codifican imágenes y videos en espacios de características separados, que
Luego se alimentan como entradas a grandes modelos de lenguaje. Sin embargo, debido a la
falta de tokenización unificada para imágenes y videos, es decir, desalineación
antes de la proyección, se vuelve un desafío para un modelo de lenguaje grande
(LLM) para aprender interacciones multimodales a partir de varias proyecciones deficientes
capas. En este trabajo unificamos la representación visual en el lenguaje.
espacio de características para avanzar el LLM fundamental hacia un LVLM unificado. como
Como resultado, establecemos una línea base LVLM simple pero sólida, Video-LLaVA,
que aprende de un conjunto de datos mixto de imágenes y vídeos, mutuamente
mejorándose mutuamente. Como resultado, Video-LLaVA supera a Video-ChatGPT
en un 5,8%, 9,9%, 18,6% y 10,1% en MSRVTT, MSVD, TGIF y ActivityNet,
respectivamente. Además, nuestro Video-LLaVA también logra una calidad superior
Rendimiento en una amplia gama de 9 puntos de referencia de imágenes. Cabe destacar la extensa
experimentos demuestran que Video-LLaVA beneficia mutuamente a las imágenes y
Vídeos dentro de una representación visual unificada, superando a los modelos.
Diseñado específicamente para imágenes o vídeos. Nuestro objetivo es que este trabajo
proporcionar información modesta sobre los insumos multimodales para el LLM.

## 1 Introducción

Recientemente, los LLM han ganado rápidamente popularidad en la comunidad de IA, como
GPT-3.5, GPT-4 (OpenAI, 2023), PaLM (Bi et al., 2020; Anil et al.,
2023) y BLOOM (Scao et al., 2022). Confían en sus poderosos
Habilidades de comprensión del lenguaje para seguir instrucciones proporcionadas por humanos.
y proporcionar las respuestas correspondientes. Normalmente, los LLM solo pueden responder
dentro del ingreso de texto proporcionado por el usuario, el cual es insuficiente
porque la interacción humana con el mundo implica múltiples canales,
como visuales y textuales. Para ello, trabajos recientes (Ye et al., 2023;
Zhu et al., 2023b; Alayrac et al., 2022) han mapeado imágenes en texto.
como tokens, lo que permite que los LLM surjan con la capacidad de comprender
imágenes. A pesar de su eficacia, capacitar a los LLM para comprender
Los videos son más desafiantes que las tareas de comprensión de solo imágenes.
Sin embargo, trabajos recientes (Maaz et al., 2023; Li et al., 2023c; Zhang et al.
al., 2023a) ha dado pasos iniciales para permitir interacciones entre
Vídeo y lenguaje.

Figura 1: Comparación de diferentes paradigmas LVLM. Video-LLaVA alinea imágenes y videos antes de la proyección, lo que permite a LLM aprender a partir de una representación visual unificada y dotar a LLM de la capacidad de comprender imágenes y videos simultáneamente.

<!-- imagen -->

Sin embargo, la mayoría de los LVLM actuales (Li et al., 2023b; Dai et al., 2023; Luo et al.,
otros, 2023; Li y otros, 2023a; Yin y otros, 2023; Fu et al., 2023) pueden
manejan principalmente una única modalidad visual, ya sea lenguaje de imagen o
videolenguaje. Comparamos diferentes paradigmas LVLM como se muestra en la Figura
1, donde VideoChat (Li et al., 2023c) y VideoLLaMA (Zhang et al.,2023a) utilizan un codificador visual compartido para manejar imágenes y vídeos.
Sin embargo, debido a las diferencias inherentes en los tipos de imágenes de los medios.
y videos, es un desafío aprender una representación unificada, y el
El rendimiento cae significativamente por detrás del del vídeo especializado.
modelo experto, Video-ChatGPT. Por lo tanto, X-LLM (Chen et al., 2023) y
Macaw-LLM (Lyu et al., 2023) asigna un codificador específico de modalidad para
cada modalidad, intentando permitir que un LLM comprenda imágenes o videos
a través de varias capas de proyección. Pero sus actuaciones son inferiores.
a modelos expertos en vídeo dedicados como VideoChatGPT (Maaz et al.,
2023). Atribuimos este fenómeno a la falta de alineación antes
proyección. Debido a que las características de imagen y video residen en sus
propios espacios, esto plantea un desafío para un LLM para aprender sus interacciones
de varias capas de proyección deficientes. Algún fenómeno similar como
La alineación antes de la fusión ha sido discutida por ALBEF (Li et al., 2021) y
ViLT (Kim et al., 2021) en modelos multimodelo. Más recientemente,
ImageBindLLM (Han et al., 2023) se centra en permitir que el LLM
procesar simultáneamente múltiples entradas modales prealineando cada una
modalidad a un espacio de características común (Girdhar et al., 2023). Basado en un
modelo de lenguaje de imagen grande, ImageBind-LLM convierte otras modalidades en
las características de imagen más similares recuperándolas de una imagen sin entrenamiento
base de datos en caché. Sin embargo, el enfoque de alineación indirecta de ImageBind-
LLM puede provocar una degradación del rendimiento y el LLM no tiene conocimiento de
datos de vídeo reales.

En este trabajo, presentamos Video-LLaVA, una línea base simple pero poderosa
para que el LVLM maneje simultáneamente imágenes y videos.
Específicamente, como se muestra en la Figura 1, Video-LLaVA inicialmente alinea el
representaciones de imágenes y vídeos en un espacio de características visuales unificado.
Dado que las representaciones visuales ya están alineadas antes de
proyección, empleamos una capa de proyección compartida para mapear el unificado
representación visual para el LLM. Para mejorar la eficiencia computacional,
Video-LLaVA realiza formación conjunta de imágenes y vídeos, logrando
Resultados notables con 1 época de entrenamiento.

Como resultado, el Video-LLaVA propuesto mejora enormemente la capacidad de
el LLM para comprender simultáneamente imágenes y videos. Para imagen
Entendiendo, Video-LLaVA supera a los LVLM avanzados como mPLUG-owl-7B
e InstructBLIP7B en 5 puntos de referencia de imágenes. Además, utilizando 4
Conjuntos de herramientas de referencia para una evaluación más integral, Video-LLaVA-7B
incluso supera a IDEFICS-80B en un 6,4% en MMBench. Además, similares
Se pueden observar tendencias en la comprensión del vídeo, donde Video-LLaVA
supera a VideoChatGPT en un 5,8%, 9,9%, 18,6% y 10,1% respectivamente en
los vídeos de preguntas y respuestas de MSVD, MSRVTT, TGIF y ActivityNet
conjuntos de datos. Amplios experimentos de ablación demuestran que la alineación
antes de que la proyección produzca mayores beneficios. Además, la formación conjunta
de imágenes y vídeos puede facilitar una representación visual unificada en
Comprensión del LLM.

Resumimos nuestras principales contribuciones de la siguiente manera:

- Presentamos Video-LLaVA, una poderosa línea base LVLM. Durante el proceso de capacitación, Video-LLaVA vincula señales visuales al espacio de características del lenguaje, unificando representaciones visuales y propone una solución para alinear antes de la proyección. Permitimos que un LLM realice capacidades de razonamiento visual tanto en imágenes como en videos simultáneamente.- Amplios experimentos demuestran que una representación visual unificada beneficia a los LLM a la hora de aprender a manejar simultáneamente imágenes y vídeos, validando la complementariedad de las modalidades y mostrando una superioridad significativa en comparación con los modelos diseñados específicamente para imágenes o vídeos.

## 2 Trabajo relacionado

## 2.1 Modelos de lenguaje grandes

Cuando se lanzó el conocido modelo comercial ChatGPT (OpenAI, 2023)
presentado, la comunidad de IA lanzó lenguaje grande de código abierto
Modelos (LLM) mediante el ajuste de instrucciones y el aumento del tamaño de los modelos. Estos
incluyen LLaMA (Touvron et al., 2023a), Vicuña (Chiang et al., 2023),
Alpaca (Taori et al., 2023), y más recientemente, LLaMA 2 (Touvron et al.,
2023b). Estos modelos están ajustados con conjuntos de instrucciones para emular
Conversaciones entre humanos y asistentes de IA. Además, InstructGPT
(Ouyang et al., 2022) está entrenado en base a GPT-3 (Brown et al., 2020)
con 175 mil millones de parámetros alineándose con las preferencias humanas.
Sin embargo, los LLM solo pueden interactuar dentro del texto. En este trabajo presentamos
Video-LLaVA, que se basa en las poderosas capacidades de razonamiento de
LLM para ampliar la modalidad de interacciones a imágenes y videos.

## 2.2 Modelos de visión-lenguaje de gran tamaño

Al ampliar los LLM a multimodales, especialmente los que involucran imágenes y
videos, los enfoques principales se pueden clasificar en dos tipos en la Tabla
1: i) tratar a LLM como un programador, ii) tratar a LLM como un decodificador.

Tabla 1: Comparación entre diferentes Modelos de Gran Visión-Lenguaje. Para los métodos que tratan a los LLM como programadores, no requieren alineación previa ni capacitación conjunta.| Métodos | Imagen | Vídeo | Prealineado | Formación conjunta ||-----------------------------------|---------|---------|---------------|------------------|
| LLM como planificador |         |         |               |                  || VisualChatGPT (Wu et al., 2023) | ✔ | ✗ | - | - || Abrazando a GPT (Shen et al., 2023) | ✔ | ✗ | - | - || MM-REACT (Yang et al., 2023) | ✔ | ✔ | - | - || ViperGPT (Surís et al., 2023) | ✔ | ✔ | - | - || LLM como decodificador |         |         |               |                  || Mini-GPT4 (Zhu et al., 2023b) | ✔ | ✗ | - | ✗ || LLaVA (Liu et al., 2023b) | ✔ | ✗ | - | ✗ || Video-ChatGPT (Maaz et al., 2023) ✗ | ✔ | - | ✗ || VideoChat (Li et al., 2023c) | ✔ | ✔ | ✗ | ✔ || Vídeo-LLaMA (Zhang et al., 2023a) | ✔ | ✔ | ✗ | ✔ || ImageBind-LLM (Han et al., 2023) | ✔ | ✔ | ✔ | ✗ || Video-LLaVA (Nuestro) | ✔ | ✔ | ✔ | ✔ |## 2.2.1 LLM como planificador

En los métodos basados en programadores, varios modelos visuales se tratan como
módulos plug-and-play. LLM los programa de acuerdo con las necesidades específicas.
requisitos de tareas visuales, como el ensamblaje de bloques de construcción. Algunos de
estos métodos se centran en imágenes, como VisualChatGPT (Wu et al., 2023)
y HuggingGPT (Shen et al., 2023), mientras que MM-REACT (Yang et al., 2023)
y ViperGPT (Surís et al., 2023) también pueden manejar vídeos. una clave
La característica de estos LVLM basados en programadores es que no
requieren capacitación de principio a fin, eliminando así la necesidad de
alineación y entrenamiento conjunto de cada modalidad.

## 2.2.2 LLM como decodificador

En cuanto al enfoque de tratar LLM como un decodificador, este es nuestro principal
enfoque. MiniGPT-4 (Zhu et al., 2023b) alinea tokens de imagen con la entrada de
el modelo de lenguaje grande a través de varias capas de proyección lineal.
Sin embargo, esta alineación es débil y carece de retroalimentación por parte de los humanos.
instrucciones. Posteriormente, mPLUG-Owl (Ye et al., 2023) adopta un sistema de dos
enfoque de entrenamiento por etapas. En la primera etapa, las imágenes se alinean con
lenguaje usando un estilo de preentrenamiento autorregresivo, y la segunda etapa
Implica el ajuste de instrucciones mediante el uso de un conjunto de datos de instrucciones humanas.
Con la creciente escala de backends de modelos de lenguaje grandes, los enfoques
como InstructBLIP (Dai et al., 2023) y la serie LLaVA (Liu et al.,
2023b,a; Lin et al., 2024) recopilan la instrucción humana más amplia
conjuntos de datos para entrenar LVLM más grandes (parámetros 13B). Cada respuesta de
Los conjuntos de datos de instrucciones siguen estrictamente las instrucciones dadas. entonces
se someten a una formación integral utilizando conjuntos de datos de instrucción humana,
permitiendo al LLM capacidades de razonamiento visual. Además, Video-
ChatGPT (Maaz et al., 2023) diseña un conjunto de datos de instrucciones en video de 100k,
capacitar con éxito a los LLM para comprender videos. VideoChat (Li et al.,
2023c) y Video-LLaMA (Zhang et al., 2023a) logran esto realizando
formación conjunta, que permite a los LLM manejar simultáneamente imágenes y
vídeos. Generalmente, ampliar los LLM a modalidades visuales adicionales
requiere una alineación previa, como se ve en LLaMA-Adapter (Zhang et al., 2023b;
Gao et al., 2023) e ImageBind-LLM (Han et al., 2023). Se unen a otros
modalidades al espacio de la imagen a través de ImageBind (Girdhar et al., 2023)
codificador de modalidad. Estos modelos han demostrado que una característica unificada
el espacio es ventajoso para mejorar el razonamiento multimodal de LLM
capacidades. A diferencia de trabajos anteriores, Video-LLaVA no sólo pre-
alinea las características de imagen y video, pero también lleva a cabo capacitación conjunta de
Imágenes y vídeos que facilitan a los LLM el aprendizaje del razonamiento multimodal.
capacidades desde una representación visual unificada.

##3 Vídeo-LLaVA

## 3.1 Estructura del modelo

## 3.1.1 Descripción general del marco

Como se muestra en la Figura 2, Video-LLaVA consta de codificadores LanguageBind f V
(Zhu et al., 2023a) para extraer características de la señal visual sin procesar (im-
edades o videos), un modelo de lenguaje grande f L como Vicuña, visual
capas de proyección f P y una capa de incrustación de palabras f T . Nosotros inicialmente
obtenga características visuales utilizando codificadores LanguageBind. Enlace de idioma
Los codificadores son capaces de mapear diferentes modalidades en el texto.
espacio de características, proporcionándonos así una visión unificada
representación. Posteriormente, se obtiene la representación visual unificada.
codificado por capas de proyección compartidas, que luego se combina con
consultas textuales tokenizadas y alimentadas en un modelo de lenguaje grande para
generar las respuestas correspondientes.

## 3.1.2 Representación visual unidaNuestro objetivo es mapear imágenes y videos en un espacio de funciones compartido para
permitir que el modelo de lenguaje grande aprenda de una visión unificada
representación. Suponemos que se puede transmitir la misma información.
a través de múltiples medios. Por ejemplo, un perro corriendo se puede expresar
a través del lenguaje, una imagen o un vídeo simultáneamente. Por lo tanto, podemos
comprimir información de diferentes modalidades en una característica común
espacio, permitiendo al modelo extraer información de una característica densa
espacio, facilitando las interacciones entre modalidades y la complementariedad. Por lo tanto, nosotros
eligió los codificadores de modalidad de LanguageBind (Zhu et al., 2023a), que
alinear imágenes y videos con el espacio de características textuales.

## 3.1.3 Alineación antes de la proyección

Específicamente, LanguageBind se inicializa desde OpenCLIP (Ilharco et al.,
2021), alineando naturalmente imágenes y lenguaje en un espacio de características compartido.
Posteriormente, alinea las representaciones de vídeo con el espacio del lenguaje.
utilizando 3 millones de pares de vídeo-texto de VIDAL-10M (Zhu et al., 2023a). Por
compartir un espacio de características lingüísticas, las representaciones de imágenes y videos
finalmente convergen en un espacio de características visuales unificado, al que nos referimos
a como alineación emergente de imágenes y videos. Por eso nuestro vídeo
El codificador y el codificador de imágenes se inicializan desde los codificadores LanguageBind.
zoológico, prealineando los insumos para LLM y reduciendo la brecha entre
representaciones de diferentes señales visuales. La visión unificada
La representación se introduce en LLM después de pasar por una proyección compartida.
capa.

## 3.2 Canal de capacitación

En general, el proceso de generación de respuestas por Video-LLaVA es similar
al de un modelo de lenguaje grande (serie GPT). Dada una entrada textual X
T y señales visuales X V, las señales de entrada se codifican en una secuencia
de tokens según la Ecuación 1. Maximizando la probabilidad
probabilidad en la Ecuación 2, el modelo finalmente logra multimodal
capacidades de comprensión.

<!-- fórmula-no-decodificada -->

<!-- fórmula-no-decodificada -->

donde L es la longitud de la secuencia generada X A, y θ es una
parámetro entrenable. Realizamos dinámicamente capacitaciones conjuntas sobre imágenes y
vídeos, en los que un único lote contiene muestras de imágenes y vídeos
simultáneamente.

## 3.2.1 Comprender la formación

En esta etapa, se requiere que el modelo adquiera la capacidad de interpretar
señales visuales dentro de un extenso conjunto de datos de pares de imagen/video-texto. cada uno
La señal visual corresponde a una única ronda de datos de conversación ( X q ,
X a ), donde XT = X q y X a es la verdad fundamental. el entrenamiento
El objetivo de esta etapa es la pérdida autorregresiva original, donde la
El modelo aprende la capacidad básica de ver la visión. Congelamos el otro
parámetros del modelo durante este proceso.

## 3.2.2 Ajuste de instrucciones

En esta etapa, se requiere que el modelo proporcione respuestas correspondientes.
a diferentes instrucciones. Estas instrucciones a menudo implican tareas más complejas.
tareas de comprensión visual, en lugar de simplemente describir señales visuales.
Tenga en cuenta que los datos de la conversación ( X 1 q , X 1 a , · · · , X N q , X N a
) consta de varias rondas.

<!-- fórmula-no-decodificada -->

donde r representa el número redondo. Como se muestra en la Ecuación 3, cuando r &gt; 1
Concatenamos las conversaciones de todas las rondas anteriores con el
instrucción actual como entrada para esta ronda. El objetivo de la formación
sigue siendo el mismo que en la etapa anterior. Después de esta etapa, el modelo
aprende a generar respuestas correspondientes basadas en diferentes
instrucciones y peticiones. El LLM también participa en la formación en este
escenario.Figura 2: Marco de formación y desempeño. Video-LLaVA exhibe notables capacidades interactivas entre imágenes y videos, a pesar de la ausencia de pares imagen-video en el conjunto de datos. (a) El marco Video-LLaVA demuestra un flujo de datos que genera respuestas correspondientes basadas en instrucciones de entrada. (b) VideoLLaVA logra rendimientos superiores en una amplia gama de 15 conjuntos de datos en imágenes y videos.

<!-- imagen -->

Figura 3: Composición de datos para el entrenamiento de VideoLLaVA. El conjunto de datos de la etapa 1 consta de una conversación de un solo turno, que se centra en descripciones visuales concisas. En la etapa 2, el conjunto de datos comprende conversaciones de varios turnos, enfatizando habilidades complejas de razonamiento visual.

<!-- imagen -->

## 4 experimentos

## 4.1 Configuración experimental

## 4.1.1 Detalles de datos

En 3, para la primera etapa de comprensión del preentrenamiento, utilizamos un subconjunto
de 558K pares de imagen-texto LAION-CC-SBU con BLIP (Li et al., 2022)
subtítulos, que provienen de CC3M (Sharma et al., 2018) y se filtran
por LLaVA (Liu et al., 2023b). Los pares vídeo-texto se derivan de una
subconjunto proporcionado por Valley (Luo et al., 2023), y tenemos acceso a 702k
de un total de 703.000 pares, procedentes de WebVid (Bain et al.,
2021). Para la etapa de ajuste de instrucción, reunimos instrucciones
conjuntos de datos de dos fuentes, incluida una instrucción de texto de imagen de 665k
conjunto de datos de LLaV A 1.5 (Liu et al., 2023a) y un vídeo-texto de 100k
conjunto de datos de instrucciones de Video-ChatGPT (Maaz et al., 2023).

## 4.1.2 Configuración del modelo

Empleamos Vicuna-7B v1.5 como modelo de lenguaje grande. lo visual
Los codificadores se derivan de LanguageBind, inicializados desde OpenCLIP-L/14.
El tokenizador de texto proviene de LLaMA, con aproximadamente 32.000
clases. Las capas de proyección compartida constan de 2 capas completamente conectadas.
con una función activada por GeLU (Hendrycks y Gimpel, 2016).

## 4.1.3 Detalles de la capacitación

En el proceso de capacitación, cambiamos el tamaño y recortamos cada imagen, lo que da como resultado una
tamaño de 224×224 para cada imagen procesada. Tomamos muestras uniformemente de 8 fotogramas.
de cada vídeo, y cada fotograma se somete a un preprocesamiento de la imagen. los datos
en cada lote hay una combinación aleatoria de imágenes y videos. en el primero
etapa, entrenamos para una época con un tamaño de lote de 256, usando AdamW
optimizador con un programa de tasa de aprendizaje del coseno. En la segunda etapa, nosotros
reducir el tamaño del lote a 128. La tasa de aprendizaje inicial para ambas etapas
está establecido en 1e-3, con una relación de calentamiento de 0,03. Hiperparámetro adicional
Los ajustes se pueden encontrar en el apéndice.

## 4.2 Evaluación Cuantitativa

## 4.2.1 Comprensión del vídeo de toma cero

Como se muestra en la Tabla 2, realizamos una evaluación cuantitativa del video.
capacidades de respuesta a preguntas de grandes modelos de videolenguaje en cuatro
conjuntos de datos, incluidos MSVD-QA (Chen y Dolan, 2011), MSRVTT-QA (Xu et
al., 2016), TGIFQA (Jang et al., 2017) y ActivityNet-QA (Yu et al.,
2019). El proceso de evaluación para la comprensión del video sigue Video-
ChatGPT. Informamos la precisión y la puntuación, que se evalúa mediante GPT-
Asistente. Video-LLaVA supera consistentemente a Video-ChatGPT en términos
de precisión en las respuestas a preguntas, que es un gran lenguaje de vídeo avanzado
modelo. Además, Video-LLaVA supera la poderosa línea de base de Video-
ChatGPT en un 5,8%, 9,9%, 18,6% y 10,1% en MSRVTT, MSVD, TGIF y
ActivityNet, respectivamente. Además, realizamos comparaciones con el
modelo SOTA reciente, Chat-UniVi (Jin et al., 2023). A pesar de Chat-UniVi
utilizando más conjuntos de datos como MIMIC-IT (Li et al., 2023a), Video-LLaVAaún demuestra resultados competitivos, superando a ChatUniVi en MSVD,
Conjuntos de datos MSRVTT y TGIF. En resumen, estos resultados validan Video-
La capacidad de LLaVA para comprender vídeos y proporcionarlos contextualmente.
respuestas apropiadas basadas en instrucciones.

Tabla 2: Comparación entre diferentes LVLM en puntos de referencia de razonamiento en video. Empleamos ChatGPTAssistant para evaluar el rendimiento después de Video-ChatGPT (Maaz et al., 2023). La versión de ChatGPT es 'gpt-3.5-turbo'.| Métodos | Tamaño del LLM | MSVD-QA | MSVD-QA | MSRVTT-QA | MSRVTT-QA | TGIF-QA | TGIF-QA | ActivityNet-QA | ActivityNet-QA ||---------------|------------|-----------|-----------|-------------|-------------|-----------|-----------|------------------|------------------|
|               |            | Precisión | Puntuación | Precisión | Puntuación | Precisión | Puntuación | Precisión | Puntuación || CongeladoBiLM | 1B | 32.2 | - | 16.8 | - | 41,0 | - | 24,7 | - || Videochat | 7B | 56,3 | 2.8 | 45,0 | 2.5 | 34,4 | 2.3 | -| 2.2 || Adaptador LLaMA | 7B | 54,9 | 3.1 | 43,8 | 2.7 | -| -| 34.2 | 2.7 || Video-LLaMA   | 7B         | 51.6      | 2.5       | 29.6        | 1.8         | -         | -         | 12.4             | 1.1              || VideochatGPT | 7B | 64,9 | 3.3 | 49,3 | 2.8 | 51,4 | 3.0 | 35,2 | 2.7 || Chat-UniVi | 7B | 65,0 | 3.6 | 54,6 | 3.1 | 60,3 | 3.4 | 45,8 | 3.2 || Vídeo-LLaVA | 7B | 70,7 | 3.9 | 59,2 | 3.5 | 70,0 | 4.0 | 45,3 | 3.3 |Tabla 3: Comparación entre diferentes LVLM en puntos de referencia de comprensión de imágenes. 'Res.', 'L', 'V' representan respectivamente la resolución de la imagen de entrada, LLaMA (Touvron et al., 2023a) y Vicuña (Chiang et al., 2023). Los nombres de los puntos de referencia se abrevian debido a limitaciones de página. VQA-v2 (Goyal et al., 2017); GQA (Hudson y Manning, 2019); VisWiz (Gurari et al., 2018); SQA I: ScienceQA-IMG (Lu et al., 2022); VQA T: TextoVQA (Singh et al., 2019); PAPA (Li et al., 2023d); MMB: MMBench (Liu et al., 2023c); LLaVA W: LLaVA-Bench (In-theWild) (Liu et al., 2023b); MM-Vet (Yu et al., 2023). † dona que reproducimos LLaVA-1.5 con el codificador LanguageBindImage para comparar de manera justa. ∗ señala que existe cierta superposición en los datos de entrenamiento.| Métodos | Máster en Derecho |   Res. | Imagen Respuesta a preguntas | Imagen Respuesta a preguntas | Imagen Respuesta a preguntas | Imagen Respuesta a preguntas | Imagen Respuesta a preguntas | Kit de herramientas de referencia | Kit de herramientas de referencia | Kit de herramientas de referencia | Kit de herramientas de referencia ||--------------|-------|--------|----------------------|----------------------|----------------------|----------------------|----------------------|---------------------|---------------------|---------------------|---------------------||              |       |        | VQA v2 | GQA | VisWiz | SQA I | VQA T | PAPA | MMB | LLaVAW| MM-Veterano || LLaVA-1.5 | V-7B |    336 | -| 62,0 ∗ | -| -| -| -| -| -| 30,5 || BIP-2 | V-13B |    224 | 41,0 | 41,0 | 19.6 | 61,0 | 42,5 | 85,3 | - | 38.1 | 22.4 || InstruirBLIP | V-13B |    224 | - | 49,5 | 33,4 | 63.1 | 50,7 | 78,9 | - | 58,2 | 25,6 || IDEFICS-80B | L-65B |    224 | 60,0 | 45,2 | 36,0 | - | 30,9 | - | 54,5 | - | - || MiniGPT-4 | L-7B |    224 | -| 30,8 | 47,5 | 25.4 | 19.4 | -| 23.0 | -| 22.1 || IDEFICS-9B | L-7B |    224 | 50,9 | 38,4 | 35,5 | -| 25,9 | -| 48,2 | -| -|| mPLUG-Búho | L-7B |    224 | - | 14.0 | 39,0 | 2.8 | 38,8 | - | 46,6 | - | - || Nutria | El 7B |    224 | - | 38.1 | 50,0 | 27.2 | 21.2 - | 32,6 | - | 24,6 || InstruirBLIP | V-7B |    224 | - | 49,2 | 34,5 | 60,5 | 50,1 | - | 36,0 | 60,9 | 26.2 || LLaVA-1.5 † | V-7B |    224 | 72,3 ∗ | 56,9 ∗ | 47,8 | 67,9 | 49,2 | 83,3 | 59,5 | 63,3 | 25,7 || Vídeo-LLaVA | V-7B |    224 | 74,7 ∗ | 60,3 ∗ | 48.1 | 66,4 | 51,8 | 84,4 | 60,9 | 73.1 | 32,0 |## 4.2.2 Respuesta a preguntas con imagen de disparo cero

Como se muestra en la Tabla 3, evaluamos nuestro enfoque para la comprensión de imágenes en
Cinco puntos de referencia de respuesta a preguntas sobre imagen académica. Comparado con el
modelo de última generación InstructBLIP-7B, VideoLLaVA demuestra potente
capacidades de comprensión de imágenes, superando en los cinco
puntos de referencia de respuesta a preguntas. Además, Video-LLaVA exhibe
resultados competitivos en comparación con varios LVLM más potentes, que son
sintonizado en base a 13B o 65B LLM, como superar InstructBLIP-13B por
14,7% en VisWiz, destacando su fuerte capacidad de comprensión en
entornos visuales naturales. Además, para garantizar una comparación justa,
Reemplazamos el codificador de imágenes en LLaVA-1.5 con LanguageBind-Image.
codificador, llamado LLaVA-1.5 † . Esto demuestra que el rendimiento
La mejora observada en Video-LLaVA no se atribuye únicamente a una
Codificador de imágenes más potente. Se pueden encontrar detalles adicionales en la Sección
4.3.6.

Tabla 4: Se informan los resultados de la evaluación de alucinaciones con objetos de disparo cero para tres entornos de evaluación de POPE. 'Sí' indica la proporción de respuestas positivas a la pregunta dada. † dona que reproducimos LLaVA-1.5 con el codificador LanguageBind-Image para comparar de manera justa.| Métodos | Máster en Derecho | Adersarial | Adersarial | Adersarial | Populares | Populares | Populares | Aleatorio | Aleatorio | Aleatorio ||--------------|-------|--------------|--------------|--------------|-----------|-----------|-----------|----------|----------|----------|
| Métodos | Máster en Derecho | Precisión | Puntuación F1 | Sí | Precisión | Puntuación F1 | Sí | Precisión | Puntuación F1 | Sí || MiniGPT-4 | V-13B | 66,6 | 71,4 | 66,7 | 68,3 | 72,2 | 64.1 | 77,8 | 78,9 | 54,8 || InstruirBLIP | V-13B | 74,4 | 78,5 | 69,0 | 81,4 | 83,5 | 62,6 | 88,7 | 89,3 | 55,2 || MM-GPT | L-7B | 50,0 | 66,7 | 100,0 | 50,0 | 66,7 | 100,0 | 50,0 | 66,7 | 100,0 || mPLUG-Búho | L-7B | 50,7 | 66,8 | 98,7 | 50,9 | 66,9 | 98,6 | 54,0 | 66,4 | 95,6 || Chat-UniVi | V-7B | 55,6 | 68,7 | 91,6 | 56,4 | 69,0 | 90,8 | 73,9 | 79,3 | 74,6 || LLaVA-1.5 † | L-7B | 84,3 | 83,2 | 43,5 | 79,8 | 79,4 | 48,0 | 85,7 | 84,8 | 43,0 || Vídeo-LLaVA | V-7B | 81,6 | 80,8 | 45,8 | 85,3 | 84,0 | 42.1 | 86,2 | 85,2 | 42,0 |Evaluación bajo Image Benchmark Toolkits Además, evaluamos
LVLM que utilizan varios kits de herramientas de referencia para el ajuste de instrucciones visuales.
Estos conjuntos de herramientas de referencia proporcionan una evaluación detallada de la eficacia del modelo.
capacidades a través de métricas de evaluación sólidas. Vídeo-LLaVA supera en rendimiento
InstructBLIP-7B en un 24,9%, 12,2% y 5,8% en MMBench, LLaVA-Bench y
MM-Vet, respectivamente. Vale la pena señalar que Video-LLaVA-7B todavía
demuestra un rendimiento avanzado en comparación con modelos LLM más grandes,
superando a InstructBLIP13B en un 6,4% en MM-Vet e IDEFICS-80B (Laurençon
et al., 2023) en un 6,4% en MMBench. Estos resultados demuestran que el vídeo-
LLaVA exhibe una gran comprensión de los aspectos semánticos de las escenas,
permitiéndole responder a un lenguaje natural abierto y de forma libre.
Preguntas sobre imágenes.

## 4.2.3 Evaluación de alucinaciones de objetos

Como se muestra en la Tabla 4, informamos los resultados de la evaluación para objetos de disparo cero.
alucinaciones, utilizando un proceso de evaluación derivado de una encuesta
método de consulta basado en (Li et al., 2023d). Video-LLaVA demuestra
desempeño competitivo en tres subconjuntos: aleatorio, popular y
adversario. Específicamente, en comparación con el modelo de fundación 7B,
VideoLLaVA supera consistentemente a MM-GPT (Gong et al., 2023) en todos
los tres subconjuntos de evaluación de alucinaciones del POPE. Además, cuando
Comparado con el 13B LLM más grande, Video-LLaVA incluso supera a Mini-
GPT4 de forma integral. La exitosa actuación de Video-LLaVA en
La detección de alucinaciones de objetos valida la coherencia entre unificado
representaciones visuales y la generación de descripciones textuales.

## 4.3 Resultados de la ablación

## 4.3.1 Alineación antes de la proyección

Para validar la degradación del rendimiento causada por imágenes separadas.
representación, llevamos a cabo experimentos para explorar el desempeño de
el LLM aprende a partir de diferentes representaciones visuales. Definimos el
uso del codificador de imágenes LanguageBind como representación visual unificada mientras
el codificador MAE (He et al., 2022) utiliza una representación visual separada,
que es un extractor de características de imagen conocido y eficaz.
Además, dado que MAE no interactúa con entradas multimodales durante
En el proceso de formación utilizamos CLIP-L/14, un modelo del mismo tamaño.
Si bien CLIP-L/14 exhibe fuertes capacidades de comprensión multimodal,
no está prealineado con el codificador de vídeo. En consecuencia, esto resulta
en una falta de uniformidad en las características visuales proporcionadas a LLM. nosotros solo
reemplace el codificador de imágenes de la misma escala y mantenga LanguageBind
codificador de vídeo.

## 4.3.2 Para comprender el vídeo

Debido a la sustitución del codificador de imágenes por el codificador MAE, el vídeo
Las características y las características de la imagen ya no están unificadas durante el período inicial de LLM.
aprendizaje de representaciones visuales. En la Figura 4, en comparación con separados
representación visual, la representación visual unida significativamente
mejora el rendimiento en 4 conjuntos de datos de respuesta a preguntas en vídeo.
Las representaciones visuales separadas no sólo exhiben una menor precisión en
respuestas a preguntas, pero también demuestran una tendencia similar en la respuesta
puntuaciones. Estos resultados demuestran que la representación visual unificada
puede ayudar al LLM a aprender y comprender mejor los videos.

Tabla 5: Efecto de la alineación antes de la proyección sobre la imagen. "Unidos" se refiere a la representación visual unificada, mientras que "Separados" se refiere a la representación visual separada. Los nombres de los puntos de referencia se abrevian debido a limitaciones de página.| Métodos |   Imagen Respuesta a preguntas VQA v2 GQA VisWiz SQA I VQA T |   Imagen Respuesta a preguntas VQA v2 GQA VisWiz SQA I VQA T |   Imagen Respuesta a preguntas VQA v2 GQA VisWiz SQA I VQA T |   Imagen Respuesta a preguntas VQA v2 GQA VisWiz SQA I VQA T |   Imagen Respuesta a preguntas VQA v2 GQA VisWiz SQA I VQA T |   Kit de herramientas de referencia POPE MMB LLaVA W MM-Vet |   Kit de herramientas de referencia POPE MMB LLaVA W MM-Vet |   Kit de herramientas de referencia POPE MMB LLaVA W MM-Vet |   Kit de herramientas de referencia POPE MMB LLaVA W MM-Vet ||----------------|----------------------------------------------------------|----------------------------------------------------------|----------------------------------------------------------|----------------------------------------------------------|----------------------------------------------------------|---------------------------------------------|---------------------------------------|---------------------------------------------|---------------------------------------------|| Separado-MAE |                                                     66,0 |                                                     55,4 |                                                     42,5 |                                                     65,0 |                                                     44,2 |                                        80,8 |                                        45,7 |                                        35,9 |                                        20.0 || CLIP separado |                                                     74,6 |                                                     59,9 |                                                     47,8 |                                                     67,3 |                                                     51,5 |                                        84,4 |                                        60,2 |                                        68,9 |                                        30,6 || Unidos |                                                     74,7 |                                                     60,3 |                                                     48.1 |                                                     66,4 |                                                     51,8 |                                        84,4 |                                        60,9 |                                        73.1 |                                        32,0 || ∆ Acc.         |                                                     +0,1 |                                                     +0,4 |                                                     +0,3 |                                                     -0,9 |                                                     +0,3 |                                        +0,0 |                                        +0,7 |                                        +4.2 |                                        +1,4 |Tabla 6: Efecto del entrenamiento conjunto en vídeo. Evaluamos en cuatro videos.
conjuntos de datos de preguntas y respuestas. ∗ denota que utilizamos solo datos de video
tanto en la primera como en la segunda etapa.

Figura 4: Efecto de alineación antes de la proyección en video. Validamos e informamos la precisión y la puntuación de cuatro conjuntos de datos de preguntas y respuestas en vídeo.| Métodos |   Puntuación de precisión de MSVD-QA |   Puntuación de precisión de MSVD-QA | Precisión MSRVTT-QA | Puntuación |   Puntuación de precisión de TGIF-QA |   Puntuación de precisión de TGIF-QA |   Precisión de control de calidad de ActivityNet | Precisión de control de calidad de ActivityNet ||------------------|--------------------------|--------------------------|----------------------|---------|--------------------------|--------------------|---------------------|---------------------|| Métodos |                     64,8 |                      3.2 | Precisión MSRVTT-QA | Puntuación |                          |                      3.4 |                           | Puntuación || Vídeo-LLaVA ∗ |                          |                          | 58,3 | 3.4 |                     67,8 |                          |                      40,7 | 2.0 || Conjunto con Imagen |                     70,7 |                      3.9 | 59,2 | 3.5 |                     70,0 |                      4.0 |                      45,3 | 3.3 || ∆ Acc.           |                     +5,9 |                     +0,7 | +0,9 | +0,1 |                     +2.2 |                     +0,6 |                      +4,6 | +1,3 |<!-- imagen -->

<!-- imagen -->

## 4.3.3 Para la comprensión de imágenes

La representación visual unificada demuestra un rendimiento sólido,
superando la representación visual separada de manera integral en 5
conjuntos de datos de preguntas y respuestas sobre imágenes y 4 kits de herramientas de referencia en la Tabla 5.
Además, observamos un margen significativo de mejora del rendimiento.
en la representación visual unificada en MM-Bench, LLaVA-Bench y
Conjuntos de herramientas de referencia de MM-Vet. Esto resalta que la visión unificada
La representación no sólo mejora el rendimiento en la respuesta a preguntas sobre imágenes.
pero también proporciona beneficios en otros aspectos de la comprensión de imágenes, como
como reducir la alucinación de objetos y mejorar las capacidades de OCR.

Figura 5: Efecto del entrenamiento conjunto sobre la imagen. † dona que reproducimos los resultados de LLaVA-1.5 a una resolución de 224×224 con el codificador LanguageBind-Image para una comparación justa.

<!-- imagen -->

## 4.3.4 Capacitación conjunta

Esta subsección tiene como objetivo validar la complementariedad de imágenes y
videos durante la capacitación conjunta, que pueden mejorar mutuamente el LLM
Comprensión de imágenes y vídeos basada en una visión unificada.
representación.

## 4.3.5 Para comprender el vídeo

Para comparar el rendimiento en pruebas comparativas de vídeo, eliminamos los datos de la imagen.
durante la formación de VideoLLaVA, que se llama Video-LLaVA ∗. Nosotros
comparar con Video-LLaVA ∗ para evaluar las ganancias de rendimiento de la articulación
entrenamiento de imágenes en pruebas de video. En la Tabla 6, evaluamos nuestro modelo en
cuatro conjuntos de datos de preguntas y respuestas en vídeo. Comparado con Vídeo-LLaVA ∗
sin imagen en el entrenamiento, el modelo entrenado con imágenes conjuntas y
videos logra mejoras integrales en los cuatro videos
conjuntos de datos. Estos resultados demuestran que el entrenamiento conjunto de imágenes y
Los videos facilitan la comprensión de LLM de las representaciones visuales.

## 4.3.6 Para la comprensión de imágenes

Al comparar el rendimiento en pruebas comparativas de imágenes, resulta complicado
encuentre un LVLM basado en imágenes con la misma configuración que VideoLLaVA. a
Para solucionar este problema, reemplazamos el codificador de imágenes en LLaVA-1.5 con el
codificador LanguageBind-Image y reproducir los resultados a una resolución de
224×224 utilizando la misma configuración de entrenamiento, llamada LLaVA-1.5 †.
Como se muestra en la Figura 5, en comparación con LLaVA-1.5 †, que utiliza el mismo
configuración del codificador de imágenes, observamos mejoras de rendimiento en 8
de 9 puntos de referencia, lo que demuestra una mejora mutua en visual
comprensión. Video-LLaVA supera a LLaVA-1.5 † en POPE, lo que indica
que el entrenamiento conjunto con vídeos alivia la alucinación objetual en
imágenes. Se observa una tendencia similar en algunos otros conjuntos de herramientas de referencia,
como LLaVA-Bench y MMBench, donde los datos de vídeo mejoran significativamente
Desempeño del LLM en tareas complejas de razonamiento y conversación de imágenes.

## 5 Limitaciones y direcciones futuras

## 5.1 Limitación

Si bien Video-LLaVA exhibe una fuerte competitividad tanto en imágenes como
vídeos, todavía observamos algunas limitaciones de Video-LLaVA. para empezar
Con, Video-LLaVA tiene un rendimiento moderado en la comprensión de vídeos largos. en
Tabla 2, Chat-UniVi supera 0.5 en ActivityNet-QA porque Video-LLaVA
sólo utiliza 8 fotogramas muestreados uniformemente para comprender el vídeo, lo que
resulta en la pérdida de información detallada de videos largos.
Además, entrenar Video-LLaVA es computacionalmente costoso,
requiriendo de 3 a 4 días para completar el proceso de capacitación en 8 GPU A100-80G.

## 5.2 Direcciones futuras

En el futuro, tal vez podamos explorar una proyección compartida más eficiente.modo que puede comprimir tokens mientras preserva las características de los datos. esto seria
Apoye Video-LLaVA para comprender mejor los videos largos. Además, Video-
LLaVA puede servir como punto de partida para ampliar a otros aspectos relacionados con la visión.
modalidades, como imágenes de profundidad e infrarrojas. Además, podríamos
explorar cómo incorporar incrustaciones de marcas de tiempo de manera efectiva, permitiendo
Grandes modelos de lenguaje visual para responder preguntas relacionadas con el tiempo.
relaciones.

## 6 Conclusión

En este trabajo presentamos Video-LLaVA, un sencillo pero potente software de gran tamaño.
modelo de referencia del lenguaje visual. Proponemos un marco novedoso para abordar
el problema de la desalineación antes de la proyección, utilizando un LanguageBind
codificador para vincular previamente señales visuales en el espacio de características del lenguaje. a
permitir que un LLM comprenda imágenes y videos simultáneamente, nosotros
Realizar capacitaciones conjuntas sobre imágenes y videos, permitiendo al LLM aprender.
Interacciones multimodales a partir de una representación visual unificada. extenso
Los experimentos demuestran que el entrenamiento conjunto sobre imágenes y vídeos
beneficia mutuamente el desempeño. Además, validamos que alinear
Las representaciones visuales antes de la proyección ayudan al aprendizaje de LLM. Sorprendentemente,
LLM, después de aprender de una representación visual unificada, exhibe la
notable capacidad para interactuar simultáneamente con imágenes y vídeos,
mostrando una poderosa comprensión de conceptos visuales unificados. Estos
Los resultados demuestran colectivamente la efectividad del Video-LLaVA.
marco de formación. Como marco de entrenamiento visual unificado, el
El rendimiento de Video-LLaVA incluso supera el de los modelos expertos diseñados
específicamente para imágenes o vídeos.

## Agradecimientos

Este trabajo fue financiado en parte por la Fundación de Ciencias Naturales de
China (Nº 62202014, 62332002, 62425101), Investigación Básica de Shenzhen
Programa (No.JCYJ20220813151736001).

## Referencias

- Jean-Baptiste Alayrac, Jeff Donahue, Pauline Luc, Antoine Miech, Iain Barr, Yana Hasson, Karel Lenc, Arthur Mensch, Katherine Millican, Malcolm Reynolds, et al. 2022. Flamingo: un modelo de lenguaje visual para el aprendizaje en pocas tomas. Avances en sistemas de procesamiento de información neuronal, 35:23716-23736.
- Rohan Anil, Andrew M Dai, Orhan Firat, Melvin Johnson, Dmitry Lepikhin, Alexandre Passos, Siamak Shakeri, Emanuel Taropa, Paige Bailey, Zhifeng Chen, et al. 2023. Informe técnico de Palm 2. arXiv preimpresión arXiv:2305.10403 .
- Max Bain, Arsha Nagrani, Gül Varol y Andrew Zisserman. 2021. Congelado en el tiempo: un codificador conjunto de vídeo e imágenes para recuperación de un extremo a otro. En Actas de la Conferencia Internacional IEEE/CVF sobre Visión por Computadora, páginas 1728-1738.
- Bin Bi, Chenliang Li, Chen Wu, Ming Yan, Wei Wang, Songfang Huang, Fei Huang y Luo Si. 2020. Palm: Entrenamiento previo de un modelo de lenguaje autorregresivo y de codificación automática para la generación condicionada por el contexto. Preimpresión de arXiv arXiv:2004.07159.
- Tom Brown, Benjamin Mann, Nick Ryder, Melanie Subbiah, Jared D Kaplan, Prafulla Dhariwal, Arvind Neelakantan, Pranav Shyam, Girish Sastry, Amanda Askell, et al. 2020. Los modelos lingüísticos aprenden con pocas posibilidades. Avances en sistemas de procesamiento de información neuronal, 33:1877-1901.
- David Chen y William B. Dolan. 2011. Recopilación de datos altamente paralelos para la evaluación de paráfrasis. En Actas de la 49ª reunión anual de la asociación de lingüística computacional: tecnologías del lenguaje humano, páginas 190-200.- Feilong Chen, Minglun Han, Haozhi Zhao, Qingyang Zhang, Jing Shi, Shuang Xu y Bo Xu. 2023. Xllm: Arranque de modelos avanzados de lenguajes grandes al tratar las multimodalidades como lenguajes extranjeros. arXiv preimpresión arXiv:2305.04160 .
- Wei-Lin Chiang, Zhuohan Li, Zi Lin, Ying Sheng, Zhanghao Wu, Hao Zhang, Lianmin Zheng, Siyuan Zhuang, Yonghao Zhuang, Joseph E González, et al. 2023. Vicuña: un chatbot de código abierto que impresiona a gpt-4 con una calidad de chatgpt del 90 %*. Véase https://vicuna.lmsys.org (consultado el 14 de abril de 2023).
- Wenliang Dai, Junnan Li, Dongxu Li, Anthony Meng Huat Tiong, Junqi Zhao, Weisheng Wang, Boyang Li, Pascale Fung y Steven Hoi. 2023. Instructblip: Hacia modelos de lenguaje visual de uso general con ajuste de instrucciones. Preimpresión, arXiv:2305.06500.
- Chaoyou Fu, Peixian Chen, Yunhang Shen, Yulei Qin, Mengdan Zhang, Xu Lin, Jinrui Yang, Xiawu Zheng, Ke Li, Xing Sun, Yunsheng Wu y Rongrong Ji. 2023. Mme: Un punto de referencia de evaluación integral para modelos de lenguajes grandes multimodales. Preimpresión de arXiv arXiv:2306.13394.
- Peng Gao, Jiaming Han, Renrui Zhang, Ziyi Lin, Shijie Geng, Aojun Zhou, Wei Zhang, Pan Lu, Conghui He, Xiangyu Yue, et al. 2023. Llama-adaptador v2: modelo de instrucción visual eficiente en parámetros. arXiv preimpresión arXiv:2304.15010 .
- Rohit Girdhar, Alaaeldin El-Nouby, Zhuang Liu, Mannat Singh, Kalyan Vasudev Alwala, Armand Joulin e Ishan Misra. 2023. Imagebind: un espacio de incrustación para vincularlos a todos. En Actas de la Conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 15180-15190.
- Tao Gong, Chengqi Lyu, Shilong Zhang, Yudong Wang, Miao Zheng, Qian Zhao, Kuikun Liu, Wenwei Zhang, Ping Luo y Kai Chen. 2023. Multimodal-gpt: Un modelo de visión y lenguaje para el diálogo con humanos. arXiv preimpresión arXiv:2305.04790 .
- Yash Goyal, Tejas Khot, Douglas Summers-Stay, Dhruv Batra y Devi Parikh. 2017. Hacer que la v en vqa importe: elevar el papel de la comprensión de imágenes en la respuesta visual a preguntas. En Actas de la conferencia IEEE sobre visión por computadora y reconocimiento de patrones, páginas 6904-6913.
- Danna Gurari, Qing Li, Abigale J Stangl, Anhong Guo, Chi Lin, Kristen Grauman, Jiebo Luo y Jeffrey P Bigham. 2018. Gran desafío de Vizwiz: Responder preguntas visuales de personas ciegas. En Actas de la conferencia IEEE sobre visión por computadora y reconocimiento de patrones, páginas 3608-3617.
- Jiaming Han, Renrui Zhang, Wenqi Shao, Peng Gao, Peng Xu, Han Xiao, Kaipeng Zhang, Chris Liu, Song Wen, Ziyu Guo, et al. 2023. Imagebind-llm: Ajuste de instrucciones multimodal. Preimpresión de arXiv arXiv:2309.03905.
- Kaiming He, Xinlei Chen, Saining Xie, Yanghao Li, Piotr Dollár y Ross Girshick. 2022. Los codificadores automáticos enmascarados aprenden con visión escalable. En Actas de la conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 16000-16009.
- Dan Hendrycks y Kevin Gimpel. 2016. Unidades lineales de error gaussiano (gelus). arXiv preimpresión arXiv:1606.08415 .
- Drew A Hudson y Christopher D Manning. 2019. Gqa: un nuevo conjunto de datos para el razonamiento visual del mundo real y la respuesta a preguntas de composición. En Actas de la conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 6700-6709.
- Gabriel Ilharco, Mitchell Wortsman, Ross Wightman, Cade Gordon, Nicholas Carlini, Rohan Taori, Achal Dave, Vaishaal Shankar, Hongseok Namkoong, John Miller, Hannaneh Hajishirzi, Ali Farhadi y Ludwig Schmidt. 2021. Clip abierto. Si utiliza este software, cítelo como se muestra a continuación.- Yunseok Jang, Yale Song, Youngjae Yu, Youngjin Kim y Gunhee Kim. 2017. Tgif-qa: Hacia el razonamiento espaciotemporal en la respuesta visual a preguntas. En Actas de la conferencia IEEE sobre visión por computadora y reconocimiento de patrones, páginas 2758-2766.
- Peng Jin, Ryuichi Takanobu, Caiwan Zhang, Xiaochun Cao y Li Yuan. 2023. Chat-univi: la representación visual unificada potencia los modelos de lenguaje grandes con comprensión de imágenes y videos. arXiv preimpresión arXiv:2311.08046 .
- Wonjae Kim, Bokyung Son e Ildoo Kim. 2021. Vilt: Transformador de visión y lenguaje sin convolución ni supervisión de región. En Conferencia internacional sobre aprendizaje automático, páginas 5583-5594. PMLR.
- Hugo Laurençon, Lucile Saulnier, Léo Tronchon, Stas Bekman, Amanpreet Singh, Anton Lozhkov, Thomas Wang, Siddharth Karamcheti, Alexander M. Rush, Douwe Kiela, Matthieu Cord y Victor Sanh. 2023. Obelics: un conjunto de datos filtrados y abiertos a escala web de documentos de imagen y texto entrelazados. Preimpresión, arXiv:2306.16527.
- Bo Li, Yuanhan Zhang, Liangyu Chen, Jinghao Wang, Jingkang Yang y Ziwei Liu. 2023a. Otter: un modelo multimodal con ajuste de instrucciones en contexto. arXiv preimpresión arXiv:2305.03726 .
- Junnan Li, Dongxu Li, Silvio Savarese y Steven Hoi. 2023b. Blip-2: Arranque del entrenamiento previo de imágenes y lenguaje con codificadores de imágenes congeladas y modelos de lenguaje grandes. arXiv preimpresión arXiv:2301.12597 .
- Junnan Li, Dongxu Li, Caiming Xiong y Steven Hoi. 2022. Blip: Arranque del entrenamiento previo del lenguaje-imagen para la comprensión y generación unificadas del lenguaje-visión. En Conferencia internacional sobre aprendizaje automático, páginas 12888-12900. PMLR.
- Junnan Li, Ramprasaath Selvaraju, Akhilesh Gotmare, Shafiq Joty, Caiming Xiong y Steven Chu Hong Hoi. 2021. Alinear antes de fusionar: aprendizaje de representación de visión y lenguaje con destilación de impulso. Avances en sistemas de procesamiento de información neuronal, 34:9694-9705.
- KunChang Li, Yinan He, Yi Wang, Yizhuo Li, Wenhai Wang, Ping Luo, Yali Wang, Limin Wang y Yu Qiao. 2023c. Videochat: comprensión del vídeo centrada en el chat. arXiv preimpresión arXiv:2305.06355 .
- Yifan Li, Yifan Du, Kun Zhou, Jinpeng Wang, Wayne Xin Zhao y Ji-Rong Wen. 2023d. Evaluación de alucinaciones de objetos en modelos grandes de visión y lenguaje. arXiv preimpresión arXiv:2305.10355 .
- Bin Lin, Zhenyu Tang, Yang Ye, Jiaxi Cui, Bin Zhu, Peng Jin, Junwu Zhang, Munan Ning y Li Yuan. 2024. Moe-llava: Mezcla de expertos para grandes modelos de visión y lenguaje. arXiv preimpresión arXiv:2401.15947 .
- Haotian Liu, Chunyuan Li, Yuheng Li y Yong Jae Lee. 2023a. Líneas de base mejoradas con ajuste de instrucciones visuales. arXiv preimpresión arXiv:2310.03744 .
- Haotian Liu, Chunyuan Li, Qingyang Wu y Yong Jae Lee. 2023b. Ajuste de instrucciones visuales. arXiv preimpresión arXiv:2304.08485 .
- Yuan Liu, Haodong Duan, Yuanhan Zhang, Bo Li, Songyang Zhang, Wangbo Zhao, Yike Yuan, Jiaqi Wang, Conghui He, Ziwei Liu, et al. 2023c. Mmbench: ¿Su modelo multimodal es un jugador versátil? arXiv preimpresión arXiv:2307.06281 .
- Pan Lu, Swaroop Mishra, Tanglin Xia, Liang Qiu, KaiWei Chang, Song-Chun Zhu, Oyvind Tafjord, Peter Clark y Ashwin Kalyan. 2022. Aprenda a explicar: razonamiento multimodal a través de cadenas de pensamiento para responder preguntas científicas. Avances en sistemas de procesamiento de información neuronal, 35:2507-2521.
- Ruipu Luo, Ziwang Zhao, Min Yang, Junwei Dong, Minghui Qiu, Pengcheng Lu, Tao Wang y Zhongyu Wei. 2023. Valley: asistente de vídeo con capacidad mejorada de modelo de lenguaje grande. arXiv preimpresión arXiv:2306.07207 .- Chenyang Lyu, Minghao Wu, Longyue Wang, Xinting Huang, Bingshuai Liu, Zefeng Du, Shuming Shi y Zhaopeng Tu. 2023. Macaw-llm: Modelado de lenguaje multimodal con integración de imágenes, audio, video y texto. arXiv preimpresión arXiv:2306.09093 .
- Muhammad Maaz, Hanoona Rasheed, Salman Khan y Fahad Shahbaz Khan. 2023. Video-chatgpt: hacia una comprensión detallada del video a través de modelos de lenguaje y visión amplios. arXiv preimpresión arXiv:2306.05424 .
- OpenAI. 2023. Informe técnico Gpt-4. Preimpresión, arXiv:2303.08774.
- Long Ouyang, Jeffrey Wu, Xu Jiang, Diogo Almeida, Carroll Wainwright, Pamela Mishkin, Chong Zhang, Sandhini Agarwal, Katarina Slama, Alex Ray, et al. 2022. Entrenamiento de modelos de lenguaje para seguir instrucciones con retroalimentación humana. Avances en sistemas de procesamiento de información neuronal, 35:27730-27744.
- Teven Le Scao, Angela Fan, Christopher Akiki, Ellie Pavlick, Suzana Ili´ c, Daniel Hesslow, Roman Castagné, Alexandra Sasha Luccioni, François Yvon, Matthias Gallé, et al. 2022. Bloom: un modelo de lenguaje multilingüe de acceso abierto de 176 parámetros. arXiv preimpresión arXiv:2211.05100 .
- Piyush Sharma, Nan Ding, Sebastian Goodman y Radu Soricut. 2018. Subtítulos conceptuales: un conjunto de datos de texto alternativo de imágenes limpio e hipernímico para subtítulos automáticos de imágenes. En Actas de la 56ª Reunión Anual de la Asociación de Lingüística Computacional (Volumen 1: Artículos extensos), páginas 2556-2565.
- Yongliang Shen, Kaitao Song, Xu Tan, Dongsheng Li, Weiming Lu y Yueting Zhuang. 2023. Hugginggpt: resolución de tareas de inteligencia artificial con chatgpt y sus amigos en huggingface. arXiv preimpresión arXiv:2303.17580 .
- Amanpreet Singh, Vivek Natarajan, Meet Shah, Yu Jiang, Xinlei Chen, Dhruv Batra, Devi Parikh y Marcus Rohrbach. 2019. Hacia modelos vqa que saben leer. En Actas de la conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones, páginas 8317-8326.
- Dídac Surís, Sachit Menon y Carl Vondrick. 2023. Vipergpt: inferencia visual mediante ejecución en Python para el razonamiento. arXiv preimpresión arXiv:2303.08128 .
- Rohan Taori, Ishaan Gulrajani, Tianyi Zhang, Yann Dubois, Xuechen Li, Carlos Guestrin, Percy Liang y Tatsunori B Hashimoto. 2023. Alpaca de Stanford: un modelo de llama que sigue instrucciones.
- Hugo Touvron, Thibaut Lavril, Gautier Izacard, Xavier Martinet, Marie-Anne Lachaux, Timothée Lacroix, Baptiste Rozière, Naman Goyal, Eric Hambro, Faisal Azhar, et al. 2023a. Llama: Modelos de lenguaje básico abiertos y eficientes. arXiv preimpresión arXiv:2302.13971 .
- Hugo Touvron, Louis Martin, Kevin Stone, Peter Albert, Amjad Almahairi, Yasmine Babaei, Nikolay Bashlykov, Soumya Batra, Prajjwal Bhargava, Shruti Bhosale, et al. 2023b. Llama 2: base abierta y modelos de chat optimizados. Preimpresión de arXiv arXiv:2307.09288.
- Chenfei Wu, Shengming Yin, Weizhen Qi, Xiaodong Wang, Zecheng Tang y Nan Duan. 2023. Chatgpt visual: hablar, dibujar y editar con modelos de base visual. arXiv preimpresión arXiv:2303.04671 .
- Jun Xu, Tao Mei, Ting Yao y Yong Rui. 2016. Msrvtt: un gran conjunto de datos de descripción de videos para unir video y lenguaje. En Actas de la conferencia IEEE sobre visión por computadora y reconocimiento de patrones, páginas 5288-5296.
- Zhengyuan Yang, Linjie Li, Jianfeng Wang, Kevin Lin, Ehsan Azarnasab, Faisal Ahmed, Zicheng Liu, Ce Liu, Michael Zeng y Lijuan Wang. 2023. Mmreact: Impulsar chatgpt para el razonamiento y la acción multimodal. arXiv preimpresión arXiv:2303.11381 .- Qinghao Ye, Haiyang Xu, Guohai Xu, Jiabo Ye, Ming Yan, Yiyang Zhou, Junyang Wang, Anwen Hu, Pengcheng Shi, Yaya Shi, et al. 2023. mplug-owl: La modularización potencia los grandes modelos de lenguaje con multimodalidad. preimpresión de arXiv arXiv:2304.14178.
- Shukang Yin, Chaoyou Fu, Sirui Zhao, Ke Li, Xing Sun, Tong Xu y Enhong Chen. 2023. Una encuesta sobre modelos de lenguajes grandes multimodales. Preimpresión de arXiv arXiv:2306.13549.
- Weihao Yu, Zhengyuan Yang, Linjie Li, Jianfeng Wang, Kevin Lin, Zicheng Liu, Xinchao Wang y Lijuan Wang. 2023. Mm-vet: Evaluación de grandes modelos multimodales para capacidades integradas. Preimpresión de arXiv arXiv:2308.02490.
- Zhou Yu, Dejing Xu, Jun Yu, Ting Yu, Zhou Zhao, Yueting Zhuang y Dacheng Tao. 2019. Activitynet-qa: un conjunto de datos para comprender videos web complejos mediante la respuesta a preguntas. En Actas de la Conferencia AAAI sobre Inteligencia Artificial, volumen 33, páginas 9127-9134.
- Hang Zhang, Xin Li y Lidong Bing. 2023a. Videollama: un modelo de lenguaje audiovisual adaptado a las instrucciones para la comprensión de vídeos. arXiv preimpresión arXiv:2306.02858 .
- Renrui Zhang, Jiaming Han, Aojun Zhou, Xiangfei Hu, Shilin Yan, Pan Lu, Hongsheng Li, Peng Gao y Yu Qiao. 2023b. Llama-adapter: ajuste eficiente de modelos de lenguaje con atención de inicio cero. arXiv preimpresión arXiv:2303.16199 .
- Bin Zhu, Bin Lin, Munan Ning, Yang Yan, Jiaxi Cui, HongFa Wang, Yatian Pang, Wenhao Jiang, Junwu Zhang, Zongwei Li, et al. 2023a. Languagebind: Ampliación del preentrenamiento del lenguaje en vídeo a la modalidad n mediante alineación semántica basada en el lenguaje. arXiv preimpresión arXiv:2310.01852 .
- Deyao Zhu, Jun Chen, Xiaoqian Shen, Xiang Li y Mohamed Elhoseiny. 2023b. Minigpt-4: mejora de la comprensión visión-lenguaje con modelos avanzados de lenguaje grande. Preimpresión de arXiv arXiv:2304.10592.

## Un apéndice de ejemplo

## A.1 Entorno de entrenamiento

Mostramos algunas configuraciones de entrenamiento como se muestra en la Tabla 7. codificador de video y
El codificador de imágenes no está entrenado en ambas etapas. La capa de proyección
consta de 2 capas lineales con un GeLU (Hendrycks y Gimpel, 2016)
función de activación entre ellos. Imagen y vídeo comparten la proyección.
capa.

Tabla 7: Entorno de entrenamiento.| Configuración | Preentrenamiento | Ajuste de instrucciones ||-------------------------------------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------|| Codificador de video Codificador de imágenes Optimizador Épocas de velocidad profunda Capa de selección de visión Decaimiento de peso Relación de calentamiento Programación de tasa de aprendizaje Tasa de aprendizaje Tamaño de lote | LanguageBind-Video-LoRA LanguageBind-Image AdamW Zero2 1 -2 0.0 0.03 desintegración del coseno 1e-3 2e-5 256 128 | LanguageBind-Video-LoRA LanguageBind-Image AdamW Zero2 1 -2 0.0 0.03 desintegración del coseno 1e-3 2e-5 256 128 |## A.2 Tablero de exhibición

Mostramos algunas muestras no seleccionadas aquí, y estos videos provienen de
Video-ChatGPT (Maaz et al., 2023).

<!-- imagen -->

Figura 6: Muestras de Video-LLaVA en comprensión de video.

<!-- imagen -->

[Instrucción de entrada]

¿Qué tiene de extraño este vídeo?

El video es extraño porque muestra a un hombre sentado al lado de un león y

al lado de un hombre. También se ve al hombre acariciando a un león en la hierba.

[Instrucción de entrada]

¿Qué tiene de inusual este vídeo?

El aspecto inusual de este vídeo es que se ve a la jirafa nadando.

No es una vista común. También se ve a la jirafa saltando de una boa sumergible.

bajo el agua, lo que aumenta la singularidad del vídeo.

[Instrucción de entrada]

¿Puedes escribir un poema corto inspirado en

Claro, aquí tienes un breve poema inspirado en el vídeo:

Las flores florecen, una gracia suave,

Sus pétalos suaves, sus colores trazan.

Su belleza, un espectáculo digno de contemplar,

Un momento de paz, un momento para sostener.

Sus colores, una sinfonía de matices,

Una celebración de la vida y de todo lo que ésta puede hacer.

Su belleza, un recordatorio para apreciar,

Las cosas simples, que nos traen felicidad.

Así que tomemos un momento para apreciar,

La belleza de las flores y todo lo que pueden crear.

[Instrucción de entrada]

Describe el vídeo en detalle.

El vídeo comienza con un coche cubierto de nieve y se ve a una persona usándolo.

Quitar la nieve de las ventanillas del coche. Se ve a la persona raspando

parabrisas y las ventanillas laterales del coche. La persona continúa s

el coche, asegurándose de quitar toda la nieve. El video luego muestra

limpiando la nieve del parabrisas y las ventanillas laterales del coche. la persona

paño para limpiar la nieve de las ventanillas del coche, asegurándose de que el coche

limpio. El video termina con la persona terminando el trabajo y el auto bei

En general, el vídeo muestra a una persona quitando nieve de la ventanilla de un coche.

y luego limpiando la nieve de las ventanillas del coche para asegurarse de que esté limpia.

[Instrucción de entrada]

Describe el vídeo en detalle.

En el vídeo vemos a un hombre caminando por una pista mientras empuña una jabalina.

por la pista y lanza la jabalina. El video luego muestra al hombre.

el punto de partida, y vemos la jabalina aterrizar en el suelo. el m

lejos de la cámara. En el vídeo podemos ver que el hombre es un ja.

él está practicando sus lanzamientos. La pista parece estar bien mantenida, un

Los alrededores parecen ser tranquilos y pacíficos. La forma y la técnica del hombre.

Impresionante, y está claro que tiene experiencia en el deporte.
En general,

un vistazo al mundo del lanzamiento de jabalina y la dedicación y habilidad

en el deporte.

Figura 7: Muestras de Video-LLaVA en comprensión de video.

[Instrucción de entrada]

Describe la actividad

El video muestra a un hombre tocando el dr.

sus manos y sus palos, y continúa

[Instrucción de entrada]

Describe brevemente t

El vídeo muestra a una mujer acicalándose.

pelo y limpia sus pezuñas, y el hor

El vídeo también incluye tomas de la w

[Instrucción de entrada]

cual es el principal

El principal desafío que enfrenta el pueblo

visto luchando por mantenerse erguido y ma

adelante.

[Instrucción de entrada]

¿Dónde está este vi?

se muestra en el video?

El vídeo está tomado desde la parte superior de la

Ciudad de Nueva York. La estatua se muestra fr

desde lo alto de la propia estatua.

[Instrucción de entrada]

¿Puedes describir

ventana como se muestra en el video

¡Claro! El hombre en el video demuestra

abajo con un trapo. Luego aplica un cle

otra vez. Finalmente, muestra cómo secarlo]

¿Por qué es gracioso este vídeo?

curioso porque el bebé está jugando con un mando de Wii, que no es un juguetecitado con bebés. Las acciones del bebé también son divertidas, como
apuntando

y hacer muecas, lo que aumenta la diversión general del vídeo.

norte]

¿Puedes identificar algún peligro de seguridad en este video?

está jugando con un camión de juguete y un coche de juguete, lo que podría suponer un riesgo para la seguridad.
riesgo si

supervisado. Sin embargo, el bebé está sentado en el suelo, por lo que hay menos
riesgo de

rojo a si estuvieran jugando con los juguetes en una superficie más alta.

en]

Crea un cuento de hadas corto con una lección moral inspirada

En un tiempo, en un reino muy lejano, vivía una bella princesa llamada
Rosa.

wn por su amabilidad y belleza, y todos la amaban.

Un día,

un

Desde entonces llamado Leo vino a visitar el reino. Estaba fascinado por la

Decidió quedarse en el reino. Sin embargo, la celosa hermana de Rose, Lily, sí lo hizo.
no

ción que Leo le estaba dando a su hermana. Intentó hacerle daño a Rose, pero Leo la salvó.
ella

enamorado El reino celebró su amor y vivieron felices para siempre.
después.

La historia es que el amor verdadero lo conquista todo y que los celos pueden llevar a
dañino

.

Describe el vídeo.

norte]

Oye una persona preparando un plato con pimientos y naranjas. Ellos cortan el

naranjas y colóquelas sobre una mesa. Luego la persona procede a cortar el

Corta las naranjas en tiras y colócalas sobre un trozo de papel encerado. ellos usan
un

Corta los pimientos y las naranjas en tiras y colócalas sobre un trozo de cera.
papel.

Luego usa una cuchara para mezclar los pimientos y las naranjas. El vídeo termina

en cortar los pimientos y las naranjas en tiras y colocarlas sobre una
pieza

.

norte]

¿Cuál es el enfoque principal del video?

nosotros del video es el proceso de crear una pintura de un árbol usando
varios

d colores. El vídeo muestra la mano del artista sosteniendo un pincel y
usándolo

libre, así como el uso de diferentes colores y técnicas para crear el

en]

¿Qué usa la mujer para partir los troncos y cómo lo hace?

Utiliza un hacha para partir los troncos. Ella sostiene el hacha en su mano y la balancea.
abajo

Ella repite este proceso varias veces, dividiendo el tronco en trozos más pequeños.
piezas.

<!-- imagen -->

El video muestra a una joven realizando una rutina de gimnasia en una
barra de equilibrio. ella

ejecuta varios giros y trucos antes de desmontar y aterrizar en un
estera.

El vídeo trata sobre un hombre al que se ve girando y lanzando un disco en un
campo. el entonces

¿Qué actividades realizan la mujer y el perro?

La mujer y el perro juegan al frisbee en el jardín. la mujer tira
el disco volador,

El video muestra a dos hombres peleando sobre una mesa mientras son observados por
un grupo de

Describe paso a paso la receta de cocción que se muestra en el

El video muestra a una persona cocinando un plato de fideos. ellos comienzan por
agua hirviendo en una olla

y agregarle fideos. Luego, añaden aceite y algunos ingredientes a la
olla. Después de eso,

mezclan los fideos y los ingredientes. Finalmente sirven
el plato en un plato.

Figura 8: Muestras de Video-LLaVA en comprensión de video.

Figura 9: Muestras de Video-LLaVA en comprensión de video.

<!-- imagen -->