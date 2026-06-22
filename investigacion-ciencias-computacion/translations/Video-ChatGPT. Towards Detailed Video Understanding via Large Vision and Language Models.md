## Video-ChatGPT: hacia una comprensión detallada del vídeo a través de modelos de lenguaje y visión amplia

Muhammad Maaz 1 *, Hanoona Rasheed 1 *, Salman Khan 1,2, Fahad
Shahbaz Khan 1,3

1 Universidad de AI Mohamed bin Zayed, EAU

2 Universidad Nacional de Australia, Australia 3 Universidad de Linköping,
Suecia

## Resumen

Los agentes de conversación impulsados por modelos de lenguaje grande (LLM) están proporcionando
una nueva forma de interactuar con datos visuales. Si bien ha habido inicialmente
intentos de modelos de conversación basados en imágenes, este trabajo aborda la
campo poco explorado de la conversación basada en video mediante la introducción de Video-
ChatGPT. Es un modelo multimodal que fusiona un visual adaptado a vídeo.
codificador con un LLM. El modelo resultante es capaz de comprender y
generando conversaciones detalladas sobre videos. Presentamos un nuevo
conjunto de datos de 100.000 pares de instrucciones de vídeo utilizados para entrenar Video-ChatGPT
adquirido a través de un proceso manual y semiautomático que es fácilmente escalable
y robusto para etiquetar el ruido. También desarrollamos una evaluación cuantitativa.
marco para modelos de diálogo basados en vídeo para analizar objetivamente
Fortalezas y debilidades de los modelos de diálogo basados en vídeo. Código:
https://github.com/mbzuai-oryx/Video-ChatGPT.

## 1 Introducción

El auge de las aplicaciones de aprendizaje profundo para la comprensión de vídeos ha llevado
a importantes avances en tareas relacionadas con el vídeo. Sin embargo, el vídeo actual
Los modelos de comprensión todavía son incapaces de mantener una conversación abierta.
sobre el contenido del vídeo de forma coherente. Un diálogo basado en vídeo
El modelo puede revolucionar la búsqueda de vídeo, las operaciones de vigilancia y la ayuda.
resumir eventos clave y detección de eventos anormales. Sobre todo, puede
Proporcionar una interfaz unificada comprensible para los humanos para tareas relacionadas con vídeo.
tales como reconocimiento de acciones, localización, detección, segmentación,
recuperación y seguimiento. Además, dicha capacidad es de gran interés.
ya que demostrará la capacidad del modelo para codificar tiempos y
señales espaciales, relaciones contextuales y dependencias a largo plazo.

Los avances recientes en la comprensión multimodal se basan en gran medida en la
combinación de modelos de imágenes previamente entrenados con modelos de lenguaje grandes
(LLM) pero generalmente no consideran entradas de video (Liu et al., 2023; Zhu
et al., 2023; Li et al., 2022, 2023a; Dai et al., 2023). es por lo tanto
interesante aprovechar las amplias capacidades de los LLM para video
comprender las tareas de una manera que no sólo mantuviera el tiempo
y características espaciales, pero también ser experto en generar imágenes similares a las humanas.
conversaciones sobre vídeos. En este artículo, presentamos Video-ChatGPT, un
novedoso modelo multimodal que fusiona las capacidades de representación de un
codificador visual previamente entrenado y los poderes generativos de un LLM, capaz
de entender y conversar sobre vídeos.

1 Contribución igual.

Video-ChatGPT aprovecha un LLM adaptado (Liu et al., 2023) que
integra el codificador visual de CLIP (Radford et al., 2021) con Vicuña
(Chiang et al., 2023) como decodificador de lenguaje, ajustado en generado
pares instructivos imagen-texto. Nuestro enfoque adapta aún más el diseño.
para modelado de vídeo espaciotemporal y afina el modelo en vídeo.
datos de instrucción para capturar la dinámica temporal y cuadro a cuadro
relaciones de coherencia disponibles en los datos de vídeo. A diferencia de otros
trabajos simultáneos para conversaciones basadas en videos (Li et al., 2023b; Zhang
et al., 2023; Su et al., 2023), VideoChatGPT sobresale en temporal
comprensión, coherencia espacial y comprensión contextual como
demostrado por nuestras extensas evaluaciones.Un aporte fundamental de este trabajo es la creación de un conjunto de datos de
100.000 pares de instrucciones en vídeo utilizando una combinación de asistencia humana
y métodos de anotación semiautomáticos. Cada par consta de un vídeo y
su instrucción asociada en forma de pregunta-respuesta. esto
proporciona a VideoChatGPT un conjunto de datos amplio y diverso del que aprender,
aumentando su comprensión específica del video, atención al tiempo
capacidades de relación y conversación.

Además, presentamos la primera videoconversación cuantitativa.
marco de evaluación para la evaluación comparativa, lo que permite una evaluación más precisa
Evaluación del desempeño de modelos de video conversación. esto
El marco evalúa modelos en una variedad de capacidades, como
exactitud de la información, orientación detallada, contextual
comprensión, comprensión temporal y coherencia.

Los aportes de este trabajo son los siguientes,

- Proponemos Video-ChatGPT, un modelo de videoconversación capaz de generar conversaciones significativas sobre vídeos. Combina las capacidades de los LLM con un codificador visual previamente entrenado y adaptado para representaciones de video espaciotemporales.
- Presentamos 100.000 pares de instrucciones en vídeo de alta calidad junto con un marco de anotación novedoso que es escalable y genera una amplia gama de conjuntos de instrucciones específicas en vídeo.
- Desarrollamos el primer marco de evaluación cuantitativa de videoconversaciones para comparar modelos de videoconversación. Demostramos que Video-ChatGPT funciona bien en comparación con motores conversacionales simultáneos para videos como Video Chat (Li et al., 2023b).

## 2 trabajos relacionados

Modelos de lenguaje de visión: avances significativos en el campo de
La visión por computadora se ha observado recientemente debido al desarrollo de
muchos modelos fundamentales de visión y lenguaje. Estos modelos representan una
salto significativo hacia la creación de modelos de visión de propósito general capaces
de abordar varias tareas simultáneamente (Radford et al., 2021; et al,
2022; Gupta y otros, 2022; Maaz et al., 2022). Un buen ejemplo es CLIP
(Radford et al., 2021), que está entrenado en 400 millones de pares de imagen-texto y
ha demostrado un impresionante rendimiento de disparo cero en numerosos
puntos de referencia. Se ha empleado en diversas aplicaciones posteriores,
a partir de la detección y segmentación de objetos basada en imágenes (Rasheed et al., 2022;
Liang et al., 2023) hasta aplicaciones 3D (Rozenberszki et al., 2022; Ni et
al., 2022). También se han realizado numerosos intentos de adaptar CLIP para
aplicaciones de vídeo (Wang et al., 2021; Ni et al., 2022). Similar a nuestro
diseño, ViFiCLIP (Rasheed et al., 2023) sugiere emplear temporal
agrupación de fotogramas de vídeo para adaptar el modelo CLIP basado en imágenes para
Tareas basadas en vídeo.

Modelos de lenguaje grandes: el campo del procesamiento del lenguaje natural ha
Fue testigo de un cambio de paradigma con la llegada de los lenguajes grandes previamente entrenados.
Modelos (LLM) como GPT (Brown et al., 2020), LLaMA (Touvron et al.,
2023), OPT (Zhang et al., 2022) y MOSS (OpenLMLab, 2023). Estos
Los modelos exhiben habilidades extraordinarias como la generación del lenguaje y la comprensión.
Aprendizaje contextual y su habilidad para comprender tareas complejas.
Las indicaciones dadas al usuario de forma inmediata reflejan su impresionante
Adaptabilidad y generalización. Las capacidades probadas de los LLM tienen
alentó a los investigadores a perfeccionarlos para maximizar su competencia.

Una estrategia clave en esta búsqueda es el ajuste de las instrucciones. Este enfoque
se centra en mejorar la alineación del modelo con las intenciones del usuario y
optimizando su calidad de salida. Por ejemplo, InstructGPT (Ouyang et al.,2022) y ChatGPT (OpenAI, 2023) se benefician significativamente de esto
técnica, que muestra mejoras en la interacción conversacional diversa
capacidades y su aptitud para responder a una amplia gama de problemas complejos.
preguntas. Este enfoque eficaz se ha empleado recientemente en el ámbito abierto.
modelos fuente como Alpaca (Taori et al., 2023) y Vicuña (Chiang et al., 2023)
al., 2023), ambos desarrollados utilizando el LLaMA (Touvron et al., 2023)
marco, lo que resulta en mejoras de rendimiento.

LLM previamente capacitados en tareas de visión y lenguaje: los avances recientes en
La comprensión multimodal ha sido impulsada principalmente por la integración.
de modelos de visión basados en imágenes con LLM. Contribuciones fundamentales como
Flamingo (et al, 2022) y BLIP-2 (Li et al., 2023a) han demostrado
el poder de utilizar datos de imágenes y texto a escala web, así como ser pioneros
técnicas en alineación intermodal, para exhibir habilidades dinámicas en
contextos de aprendizaje conversacionales y de pocas tomas. Construyendo sobre esto
Fundación, MiniGPT-4 (Zhu et al., 2023) permite
conversaciones integrando BLIP-2 y Vicuña para una imagen de disparo cero
comprensión.

Igualmente significativa es la aparición de LLaVA (Liu et al., 2023), un
modelo derivado de la arquitectura LLaMa, aprovechando el lenguaje GPT-4
Competencia para generar instrucción multimodal siguiendo datos. con
ajuste de instrucciones aplicado a los datos derivados, LLaVA ha mostrado
interesante capacidad de chat multimodal, que insinúa la escalabilidad
potencial de tal metodología. Además, InstructBLIP (Dai et al.,
2023) ha demostrado sólidas capacidades de diálogo basado en imágenes a través de
Ajuste de la instrucción visión-lenguaje mediante la innovación con reconocimiento de instrucción.
extracción de características visuales.

Más estrechamente relacionado con nuestro trabajo, VideoChat (Li et al., 2023b) emplea
Componentes selectivos de modelos fundamentales de vídeo (Wang et al., 2022).
y modelos de base de imagen (Li et al., 2023a), y los integra con
LLM (Chiang et al., 2023) junto con pocas capas que se puedan aprender,
sintonizado mediante un entrenamiento ligero de dos etapas. Además, ellos
construir un conjunto de datos específicos de video utilizando un lenguaje de visión disponible en el mercado
modelos (Wu et al., 2022; Li et al., 2023a; Huang et al., 2023; Wang et al., 2023a;
al., 2022) para generar descripciones textuales ruidosas y detalladas para mejorar
el entrenamiento de modelos conversacionales videocéntricos.

A diferencia del VideoChat, proponemos un novedoso sistema semi-asistido por humanos.
marco de anotación automática para generar instrucción de alta calidad
datos para vídeos. Nuestro diseño de arquitectura simple y escalable utiliza
CLIP previamente entrenado (Radford et al., 2021) para generar espaciotemporal
características que ayudan a Video-ChatGPT a generar videos significativos
conversación. Además, somos los primeros en proponer medidas cuantitativas.
marco para evaluar tareas de conversación por vídeo (consulte la sección "Video
Generación de datos de instrucciones" para más detalles).

## 3 VideochatGPT

Video-ChatGPT es un gran modelo de visión y lenguaje que alinea el video
representaciones con un modelo de lenguaje grande (LLM), mejorando así su
capacidad de generar conversaciones significativas sobre videos. Nuestro enfoque
Se basa en el enfoque empleado en el diseño de visión-lenguaje (VL).
Modelos para el dominio del vídeo. Dada la disponibilidad limitada de videos
pares de subtítulos y los importantes recursos necesarios para la formación en
estos datos desde cero, estos modelos comúnmente adaptan imágenes previamente entrenadas
Modelos VL basados en tareas de vídeo (Ni et al., 2022; Wang et al., 2021;
Rasheed et al., 2023). Adoptamos un enfoque similar, comenzando con laAsistente de visión amplia alineado con el lenguaje (LLaVA) (Liu et al., 2023) como nuestro
fundación.

LLaVA es un LMM que integra el codificador visual de CLIP (Radford et
al., 2021) con el decodificador de lengua Vicuña (Chiang et al., 2023) y es
afinar de extremo a extremo los datos instructivos de visión y lenguaje generados.
Afinamos este modelo utilizando nuestros datos de instrucciones en vídeo, adaptándolo
para tareas de conversación por video. Los datos de las instrucciones en vídeo se obtienen como
combinación de tuberías manuales y automatizadas en nuestra propuesta
Configuración de generación de instrucciones. Esta adaptación en vídeo específico.
Las instrucciones permiten acomodar dinámicas temporales adicionales,
coherencia cuadro a cuadro y relaciones de largo alcance presentes en
datos de vídeo. Como resultado, nuestro Video-ChatGPT sobresale en razonamiento en video,
creatividad y comprensión de lo espacial, temporal y orientado a la acción.
componentes dentro de los videos.

## 3.1 Arquitectura

Usamos CLIP ViT-L/14, que está previamente entrenado usando imágenes visuales a gran escala.
sintonización de instrucciones en LLaVa, como codificador visual. Sin embargo, LLaVa
El codificador visual está diseñado para imágenes, que modificamos para capturar.
Representaciones espaciotemporales en vídeos. Dado un video de muestra V i ∈ R T
× H × W × C con fotogramas T, el codificador visual genera imágenes temporales y
características espaciales. El codificador visual codifica los fotogramas T de forma independiente.
como un lote de imágenes y produce incrustaciones a nivel de fotograma x i ∈ R T × h ×
w × D , donde h = H/p,w = W/p . Aquí p es el tamaño del parche (es decir, 14 para
ViT-L/14), y representamos el número de tokens como N , donde N = h × w
. Las incrustaciones a nivel de marco se agrupan en promedio a lo largo de la dimensión espacial
para obtener una representación temporal a nivel de video t i ∈ R T × D . esto
La operación incorpora implícitamente el aprendizaje temporal a través de la
agregación de múltiples cuadros. De manera similar, las incrustaciones a nivel de marco
se agrupan en promedio a lo largo de la dimensión temporal para producir el nivel de video
representación espacial z i ∈ R N × D . Las características temporales y espaciales.
se concatenan para obtener las características a nivel de vídeo v i ,

<!-- fórmula-no-decodificada -->

Una capa lineal simple y entrenable g proyecta estas características a nivel de video
en el espacio de incrustación del decodificador de lenguaje, transformándolos en
tokens de incrustación de idioma correspondientes Q v ,

<!-- fórmula-no-decodificada -->

Tenga en cuenta que la función g actúa como un adaptador y se puede implementar con
arquitecturas más complicadas también. Sin embargo, optamos por una visión simplista.
Diseño que ofrece un rendimiento competitivo en comparación con modelos más sofisticados.
opciones en nuestros experimentos. Las consultas de texto están tokenizadas en el mismo
dimensiones, Q t ∈ R L × K . Aquí L representa la longitud de la consulta de texto.
Finalmente, Q v se concatena con Q t y se ingresa al decodificador de lenguaje.

## 3.2 Ajuste de instrucciones en vídeo

Empleamos ajuste de instrucciones del LLM en los tokens de predicción,
utilizando su objetivo de entrenamiento autorregresivo original. el preentrenado
El modelo está optimizado con pares de texto y vídeo seleccionados y de alta calidad. Durante
En la fase de ajuste, utilizamos indicaciones predefinidas basadas en lo siguiente
plantilla:Figura 1: Arquitectura de Video-ChatGPT. Video-ChatGPT aprovecha el codificador visual CLIP-L/14 para extraer características de vídeo tanto espaciales como temporales. Esto se logra promediando las características a nivel de marco en las dimensiones temporales y espaciales, respectivamente. Las características espaciotemporales calculadas luego se introducen en una capa lineal que se puede aprender, que las proyecta en el espacio de entrada del LLM. En nuestro enfoque, utilizamos el modelo Vicuna-v1.1, compuesto por 7B parámetros, y lo inicializamos con pesos de LLaVA (Liu et al., 2023).

<!-- imagen -->

USUARIO: &lt;Instrucción&gt; &lt;Vid-tokens&gt; Asistente:

Usando las notaciones, podemos representarlo como,

USUARIO: tv

&lt; Q&gt; &lt; Q&gt; Asistente:

En este mensaje, el mensaje &lt;Instrucción&gt; representa una pregunta relacionada con
el video, tomado aleatoriamente del conjunto de entrenamiento de video-preguntas-
responder en parejas. Las preguntas pueden ser generales, pedir que describan el video o
pueden relacionarse con aspectos temporales, espaciales o creativos específicos de
el contenido del vídeo. La respuesta de predicción &lt;Respuesta&gt; corresponde a la
pregunta específica formulada. A lo largo del entrenamiento, los pesos para ambos
el codificador de vídeo y el LLM permanecen congelados y el modelo maximiza la
probabilidad de predecir tokens que representen la respuesta adaptando el
capa lineal. En consecuencia, las características del vídeo Q v se alinean con
las incrustaciones de palabras LLM previamente entrenadas, equipando VideoChatGPT con el
capacidad de producir respuestas más naturales y confiables.

## 4 Generación de datos de instrucciones en video

En esta sección, analizamos nuestro enfoque centrado en datos, que utiliza tanto
métodos de anotación semiautomáticos y asistidos por humanos para generar
Datos de instrucciones en vídeo de alta calidad. Estos datos son cruciales para la formación.
Video-ChatGPT, que garantiza respuestas precisas y significativas. Nuestros datos
La recopilación implica dos métodos clave. La anotación asistida por humanos,
involucra anotadores expertos que analizan el contenido del video y brindan
descripciones detalladas. Este proceso genera datos ricos en contexto y
Detalle, que ayuda a nuestro modelo a comprender aspectos complejos del vídeo.
contenido. Por otro lado, el marco de anotación semiautomática es
más rentable y escalable. Aprovechando la visión de vanguardia
modelos de lenguaje, este método genera anotaciones amplias y de gran volumen,
aumentando así la cantidad de datos sin comprometer la calidad
sustancialmente. A través de estos métodos combinados, hemos logrado con éxito
acumuló un sólido conjunto de 100.000 pares de instrucciones en vídeo. esto
Un conjunto de datos extenso es crucial para ajustar nuestro modelo para comprender
contenido de video de manera efectiva, integrando señales espaciales y temporales
en su comprensión.

Nuestros datos de instrucción son diversos y completos, incorporando
una amplia gama de tipos de datos. Estos incluyen descripciones detalladas,
resúmenes, pares de preguntas y respuestas, tareas que estimulan la creatividad
o generación de nuevas ideas y tareas conversacionales. Los datos abarcan un
amplio espectro de conceptos, que van desde la apariencia visual y la temporalidad.
relaciones con tareas complejas de razonamiento y más allá, proporcionando una diversidad
campo de entrenamiento para que nuestro modelo aprenda.

<!-- imagen -->

## Anotación del conjunto de datos original:

Una introducción conduce a varias imágenes de herramientas para caballos y una mujer hablando.
a la cámara sosteniendo un pincel. Luego cepilla la crin del caballo.
a fondo mientras sigues mirando a la cámara y hablando.

## Humano anotado:

El vídeo comienza con una diapositiva de introducción, donde hay una imagen del
mujer parada junto a un caballo marrón y blanco. La mujer lleva unchaqueta verde sobre una camisa blanca. Luego, una introducción conduce a varios
Imágenes de herramientas para caballos y una mujer hablando a la cámara sosteniendo un
cepillo. El vídeo trata sobre "cómo preparar a tu caballo para que brille en cuatro sencillos
pasos con una brocha de acabado. Luego cepilla la crin del caballo.
a fondo mientras sigues mirando a la cámara y hablando. el caballo es
negro y el video está filmado dentro de un establo. Ella lleva una chaqueta verde
sobre una camiseta morada de manga larga. Ella está explicando cómo usar un
cepillo de acabado. Ella muestra que el cepillo tiene un borde elevado, que
ayudar a eliminar más suciedad. Utiliza movimientos largos para cepillar el pelo de la
caballo en la dirección del crecimiento del pelo. Ella limpia de arriba a abajo.
piernas. Luego explica que es necesario limpiar el cepillo con una herramienta.
llamado curry. Finalmente, el vídeo termina con una imagen del caballo negro.
todo arreglado y créditos al video.

<!-- imagen -->

## Anotación del conjunto de datos original:

un a doa ' s o a g uu st aa so e d no alrededor del árbol decorando como
además de apagar las luces. Terminan de decorar el árbol y
jugando unos con otros y riendo. Al final primeros planos de los árboles
se muestran así como un oso.

## Humano anotado:

En el vídeo vemos un árbol de Navidad bellamente decorado con exuberantes
ramas verdes adornadas con adornos brillantes y coloridos. como la camara
cacerolas sobre los adornos, brillan a la luz, reflejando el
colores del arcoiris. Luego se muestra a dos personas moviéndose alrededor del árbol.
colgar adornos y colocar luces, colocando cuidadosamente cada adorno
en su lugar designado. Mientras trabajan, charlan y bromean,
disfrutando de la compañía de los demás y del espíritu festivo. después de que terminen
colgando los adornos, dan un paso atrás y admiran su trabajo, riéndose
y abrazándonos unos a otros. La cámara captura primeros planos del acabado.
árbol, mostrando toda su belleza y brillo. Las luces parpadean y
brillan, creando un ambiente cálido y acogedor alrededor de la habitación. La escena es una de
alegría y alegría navideña, una representación perfecta del espíritu navideño.
Finalmente, la cámara se aleja y vemos un lindo osito de juguete bailando.
debajo del árbol, rodeado de regalos bellamente envueltos. En general, el
El vídeo muestra la alegría y la calidez de la temporada festiva, con un
Árbol bellamente decorado, amor y espíritu de dar y compartir.

Figura 2: Ejemplos de enriquecimiento de datos mediante anotaciones asistidas por humanos.
Los anotadores humanos aumentan las descripciones de los vídeos a partir de conjuntos de datos de subtítulos de vídeo.
Los subtítulos se enriquecen al integrar información detallada sobre
Aspectos espaciales y temporales, relaciones objetales, razonamiento, escena.
descripciones y la secuencia cronológica de los acontecimientos.

## 4.1 Anotación asistida por humanos

En este proceso, aprovechamos conjuntos de datos que contienen pares de subtítulos de video y
utilizar la experiencia de los anotadores humanos para enriquecer el terreno original
anotaciones de verdad. Específicamente, utilizamos un subconjunto de ActivityNet-200.
(Fabian Caba Heilbron y Niebles, 2015) que proporciona una base concisa
Descripciones veraces de diversas actividades en distintos segmentos de vídeo.

Los anotadores enriquecen aún más los títulos al agregar información completa.
información sobre las apariencias físicas y espaciales y temporales.
localización, entre otros detalles contextuales críticos. La figura 2 muestra una
ejemplo de cómo se enriquece un título de verdad fundamental utilizando la asistencia humana
anotación.

## 4.2 Marco de anotación semiautomático

Además de las ricas anotaciones asistidas por humanos, también aprovechamos lacapacidades de modelos avanzados de visión y lenguaje de imágenes densas, desarrollando
un marco de anotación semiautomático. Este enfoque es rentable
y escalable, aumentando así la cantidad de datos sin
comprometiendo sustancialmente la calidad.

De manera similar al proceso asistido por humanos, este marco también aprovecha
conjuntos de datos que contienen pares de subtítulos de vídeo. Enriquecemos estos conjuntos de datos utilizando
información contextual extraída de predicciones densas disponibles en el mercado y
Subtitulado de modelos de visión y lenguaje basados en imágenes. Estos modelos proporcionan
predicciones que entregan información contextual adicional,
enriquecer los subtítulos de los vídeos. Desarrollamos un método integral que
combina estas predicciones y utiliza modelos específicos para el propósito
de eliminar el contexto ruidoso o irrelevante de los datos. Esto asegura
que los datos mantengan su exactitud y relevancia.

Aprovechando el uso de modelos disponibles en el mercado, aplicamos modelos previamente entrenados.
como BLIP-2 (Li et al., 2023a) y GRiT (Wu et al., 2022) para fotogramas clave
Análisis en los vídeos. El modelo de subtítulos de imágenes BLIP-2 genera
subtítulos a nivel de fotograma, mientras que el modelo de subtítulos densos GRiT proporciona
leyendas detalladas para los objetos de la escena. Además, los preentrenados
El modelo Tag2Text (Huang et al., 2023) se utiliza para generar etiquetas para cada
fotograma clave del vídeo. A pesar de su utilidad, estos modelos pueden
introducir ruido en los datos.

Figura 3: Ejemplos de generación de datos instructivos utilizando nuestro proceso de anotación semiautomático propuesto. Empleamos modelos de subtítulos y predicción densos disponibles en el mercado para aumentar las descripciones de los videos. BLIP-v2 (Li et al., 2023a) genera subtítulos a nivel de fotograma, mientras que GRIT (Wu et al., 2022) se utiliza para subtítulos de fotogramas densos. Tag2Text (Huang et al., 2023) genera etiquetas para cada fotograma clave, lo que ayuda a eliminar el ruido (por ejemplo, las descripciones de GRiT que contienen patrones de flores y en el teléfono se descartarían ya que no se detectan etiquetas correspondientes). Finalmente, consultamos GPT-3.5 con ejemplos en contexto para generar datos instructivos en video.

<!-- imagen -->

Para garantizar datos de alta calidad y mitigar el ruido, implementamos tres claves
pasos. Primero, mantenemos un umbral de predicción alto para todos los sistemas fuera de línea.
modelos de estantería para mantener la precisión. En segundo lugar, empleamos un especialista
mecanismo de filtrado que elimina cualquier título a nivel de fotograma de BLIP-2 o
GRiT no coincide con las etiquetas a nivel de marco de Tag2Text. este proceso
implica extraer palabras de los subtítulos a nivel de marco que se encuentran dentro
El vocabulario predefinido de etiquetas Tag2Text y elimina cualquier título que
contienen palabras que no están en las etiquetas de un marco determinado. Esta estrategia actúa como
una capa de filtrado adicional y enriquece los subtítulos integrando
predicciones de múltiples modelos.

En el tercer paso, fusionamos subtítulos a nivel de fotograma y utilizamos GPT-3.5.
modelo para generar un título singular y coherente a nivel de vídeo. este paso
aumenta el título de verdad original con contexto de estos
modelos. También ordenamos a GPT-3.5 que descarte información inconsistente.
a través de cuadros, lo que garantiza una instrucción en video precisa y contextualmente rica
conjunto de datos. La Figura 3.4 ilustra cómo se enriquece un título de verdad fundamental.
usando este proceso después de las tres etapas de refinamiento para generar
datos instructivos y título descriptivo detallado. Todos nuestros diseños
Se incluirán indicaciones para el aprendizaje en contexto junto con el conjunto de datos seleccionado.
puesto a disposición del público.

## 4.3 Postprocesamiento asistido por GPT

Por último, implementamos un mecanismo de posprocesamiento asistido por GPT querefina y optimiza las anotaciones enriquecidas, para generar
Datos instructivos en vídeo de alta calidad. Solicitamos al modelo GPT3.5 que cree
pares de preguntas y respuestas de los títulos enriquecidos y detallados que cubren
una amplia variedad de aspectos utilizando el aprendizaje en contexto. Estos aspectos
incluir descripciones detalladas, resúmenes, pares de preguntas y respuestas,
tareas que estimulen la creatividad o la generación de nuevas ideas, y
tareas conversacionales.

Cada uno de estos elementos juega un papel crucial en nuestra estrategia centrada en datos.
enfoque. Nuestro objetivo final es crear una conversación basada en vídeo.
modelo que sea preciso, capaz de comprender el contenido de vídeo de ambos
señales espaciales y temporales, y experto en entablar conversaciones.

Figura 4: Ejemplos de enriquecimiento de datos utilizando nuestra anotación semiautomática propuesta. Empleamos modelos densos de predicción y subtítulos disponibles en el mercado (Li et al., 2023a; Wu et al., 2022; Huang et al., 2023) para aumentar las descripciones de los videos. Todos los elementos de contexto adicionales se combinan con los subtítulos del vídeo y se someten a una etapa de posprocesamiento asistida por GPT, generando la descripción detallada final.

<!-- imagen -->

Tabla 1: Evaluación comparativa del rendimiento de los modelos de generación de texto. Un análisis comparativo en profundidad de VideoChatGPT y Video Chat (Li et al., 2023b) en cinco aspectos de evaluación clave que proponemos en nuestro punto de referencia. Para una comparación justa, se utilizan variantes 7B para todos los modelos. Video-ChatGPT muestra un rendimiento competente en todos los aspectos clave.| Aspecto de evaluación |   Videochat |   Adaptador LLAMA |   Video-LLaMA |   VideochatGPT ||----------------------------|--------------|-----------------|---------------|-----------------|
| Corrección de la información |         2.23 |            2.03 |          1,96 |            2,40 || Orientación detallada |         2,50 |            2.32 |          2.18 |            2,52 || Comprensión contextual |         2,53 |            2.30 |          2.16 |            2,62 || Comprensión temporal |         1,94 |            1,98 |          1,82 |            1,98 || Consistencia |         2.24 |            2.15 |          1,79 |            2.37 |## 5 experimentos

## 5.1 Detalles de implementación

Usamos LLaVA (Liu et al., 2023) como nuestro modelo de referencia y lo ajustamos en
nuestros pares de instrucciones en video de 100K. Solo actualizamos la capa lineal.
proyectar las funciones de video en el espacio de entrada de los LLM, mientras que el resto
de la arquitectura se mantiene congelada. Afinamos el modelo para 3 épocas.
usando una tasa de aprendizaje de 2 e -5 y un tamaño de lote general de 32. Usamos
modelo de parámetros 7B en todos los experimentos y su entrenamiento tomó alrededor de 3
horas en 8 GPU A100 de 40 GB. Durante la inferencia, para la eficiencia de la memoria,
cargar los modelos en modo FP16.

En nuestro marco de anotación semiautomática, utilizamos Katna (KeplerLab,
2019) para extraer fotogramas clave de vídeo. Para Tag2Text disponible en el mercado (Huang et
al., 2023), utilizamos la variante Swin-B con un tamaño de entrada de 384 ×
384 y un umbral de confianza de 0,7. Para GRIT (Wu et al., 2022),
utilice la versión ViT-B con CenterNet2 (Zhou et al., 2021).

## 5.2 Evaluación cuantitativa

En esta sección destacamos una contribución clave de nuestro trabajo: la
Evaluación cuantitativa de VideoChatGPT utilizando métricas avanzadas y
evaluaciones comparativas con modelos de última generación existentes. Realizamos
dos tipos de evaluaciones cuantitativas: i) Generativas basadas en videos
Evaluación comparativa de desempeño y ii) Evaluación de preguntas y respuestas de respuesta cero.

Evaluación comparativa del rendimiento de generación de texto basada en video: presentamos una
punto de referencia para evaluar el rendimiento de generación de texto de videos
modelos de conversación. Para hacer esto, seleccionamos un conjunto de pruebas basado en el
Conjunto de datos ActivityNet-200 (Fabian Caba Heilbron y Niebles, 2015),
con vídeos con subtítulos descriptivos ricos y densos y asociados
pares de preguntas y respuestas a partir de anotaciones humanas. También desarrollamos un
canal de evaluación utilizando el modelo GPT-3.5. Este oleoducto evalúa
varias capacidades del modelo y asigna una puntuación relativa a las
generó predicciones en una escala del 1 al 5, en los siguientes cinco aspectos:

Tabla 2: Comparación de preguntas y respuestas de Zeroshot de Video-ChatGPT con otros modelos generativos de vídeo. Para una comparación justa, se utilizan variantes 7B para todos los modelos. Video-ChatGPT tiene un rendimiento competitivo en todos los conjuntos de datos.| Modelo | MSVD-QA | MSVD-QA | MSRVTT-QA | MSRVTT-QA | TGIF-QA | TGIF-QA | Actividad Net-QA | Actividad Net-QA ||---------------|-----------|-----------|-------------|-------------|-----------|-----------|-------------------|-------------------|
|               | Precisión | Puntuación | Precisión | Puntuación | Precisión | Puntuación | Precisión | Puntuación || CongeladoBiLM | 32.2 | - | 16.8 | - | 41,0 | - | 24,7 | - || Videochat | 56,3 | 2.8 | 45,0 | 2.5 | 34,4 | 2.3 | 26,5 | 2.2 || Adaptador LLAMA | 54,9 | 3.1 | 43,8 | 2.7 | - | - | 34.2 | 2.7 || Video LLaMA   | 51.6      | 2.5       | 29.6        | 1.8         | -         | -         | 12.4              | 1.1               || VideochatGPT | 64,9 | 3.3 | 49,3 | 2.8 | 51,4 | 3.0 | 35,2 | 2.8 |- (i) Corrección de la información: verificamos la exactitud del texto generado, asegurándonos de que se alinee con el contenido del video y no malinterprete ni desinforme.
- (ii) Orientación detallada: evaluamos la profundidad de las respuestas del modelo, buscando tanto la integridad, es decir, la respuesta del modelo cubre todos los puntos principales del video, como la especificidad, que denota la inclusión de detalles específicos en lugar de solo puntos genéricos en la respuesta del modelo.
- (iii) Comprensión contextual: Evaluamos la comprensión del modelo del contexto del video, verificando si sus respuestas se alinean con el contexto general del contenido del video.
- (iv) Comprensión temporal: examinamos la comprensión del modelo de la secuencia temporal de eventos en el video al responder preguntas.
- (v) Coherencia: Evaluamos la coherencia del modelo en preguntas diferentes pero similares o en diferentes secciones del video.

Presentamos los resultados de la evaluación de nuestro modelo propuesto, Video-ChatGPT,
utilizando el marco de evaluación comparativa cuantitativa de la Tabla 1. Los resultados
revelar su desempeño competente en todos los aspectos clave en comparación con
los modelos contemporáneos de video conversación recientemente introducidos, Video
Chat (Li et al., 2023b), Adaptador LLaMA (Gao et al., 2023) y Video-
LLaMA (Zhang et al., 2023). Video-ChatGPT muestra un buen rendimiento,
en gran parte debido al ajuste de instrucciones que realizamos y su sencilla
arquitectura que aprovecha los LLM con un codificador visual previamente entrenado
sintonizado para datos de vídeo. Esto le proporciona la sólida capacidad de
generar texto contextualmente relevante, detallado y temporalmente preciso
desde la entrada de vídeo.

Evaluación de preguntas y respuestas de respuesta cero: llevamos a cabo una evaluación integral
Evaluación cuantitativa utilizando varias preguntas abiertas de uso común.
conjuntos de datos de respuesta: MSRVTT-QA (Xu et al., 2017), MSVD-QA (Xu et al., 2017),
TGIF-QA FrameQA (Jang et al., 2017) y ActivityNetQA (Yu et al.,
2019). Estas evaluaciones se llevaron a cabo de manera cero,
empleando evaluación asistida por GPT para evaluar las capacidades del modelo.
Este proceso de evaluación mide la precisión de los datos generados por el modelo.
predicciones y asigna una puntuación relativa en una escala del 1 al 5.

Para comparar Video-ChatGPT, comparamos su rendimiento con otros
modelos significativos, como FrozenBiLM (Yang et al., 2022) y el
modelo de video generativo, Video Chat, Adaptador LLaMA y Video-LLaMA.
FrozenBiLM es un modelo que adapta modelos de lenguaje bidireccional congelados
previamente entrenado en datos de solo texto a escala web para entradas multimodales, que muestran
resultados prometedores en configuraciones de VideoQA de disparo cero. A pesar de lo sólido
base establecida por estos modelos, Video-ChatGPT consistentemente
los superó, logrando un rendimiento de vanguardia (SOTA) en todo
todos los conjuntos de datos. Estos resultados indican la capacidad de Video-ChatGPT para
comprender el contenido de vídeo y generar contenido preciso y contextualmente rico
respuestas a preguntas.

## 5.3 Ablaciones

Impacto de las anotaciones semiautomáticas: entrenamos Video-ChatGPT en dos
subconjuntos: uno con anotaciones humanas (30% de nuestros datos) y otro con semi-
anotaciones automáticas (70%). Los resultados en la Tabla. 3 indican que
entrenar únicamente con datos anotados por humanos o generados semiautomáticamente
Los datos producen un buen rendimiento. El rendimiento general cuando se utiliza sólo
Los datos generados por humanos son los más bajos debido al número limitado de etiquetas.
(30% de todos los datos) disponibles en este escenario. Sin embargo, el óptimo
Los resultados se logran cuando se utiliza un conjunto de datos combinado para el entrenamiento.Tabla 3: Datos anotados por humanos versus datos anotados semiautomáticamente: el entrenamiento utilizando datos anotados por humanos y anotados semiautomáticamente logra el mejor rendimiento.| Métrica |   Sólo humanos |   Sólo automático |   Combinado ||--------------------------|--------------|------------------|------------|
| Corrección |         2.27 |             2,35 |       2,40 || Orientación detallada |         2,49 |             2,49 |       2,52 || Comprensión contextual |         2,50 |             2,56 |       2,62 || Comprensión temporal |         1,85 |             1,92 |       1,98 || Consistencia |         2.21 |             2,38 |       2.37 || Promedio |         2.28 |             2.34 |       2,38 |Evaluación Cuantitativa con GPT-3.5: Considerando las limitaciones planteadas
mediante el uso de GPT3.5, al que se accede a través de API y no es de código abierto,
Realizamos evaluaciones utilizando el LLM de código abierto, Vicuña-1.5 (13B)
(Chiang et al., 2023). Los resultados en la Tabla. 4 muestran una tendencia similar en
corrección, detalle, comprensión contextual y temporal y
consistencia en comparación con la evaluación inicial GPT-3.5. Esto asegura
Nuestro método de evaluación sigue siendo accesible y replicable.

Tabla 4: Evaluación utilizando el modelo Vicuna-1.5 (13B): Observamos una tendencia similar al evaluar el uso del modelo Vicuna1.5 (13B) de código abierto versus GPT-3.5-Turbo.| Métrica |   Videochat |   Video-LLaMA |   VideochatGPT ||--------------------------|--------------|---------------|-----------------|
| Corrección |         2.32 |          2.10 |            2,49 || Orientación detallada |         2,50 |          2.18 |            2,52 || Comprensión contextual |         2,76 |          2.41 |            2,85 || Comprensión temporal |         2.27 |          2.17 |            2,38 || Consistencia |         2,95 |          2,67 |            3.09 |Garantizar la coherencia del canal de anotaciones automáticas: para garantizar
coherencia entre nuestro proceso de evaluación automática y el humano.
evaluaciones, realizamos una prueba ciega comparando pares de control de calidad de humanos y
Fuentes anotadas de forma semiautomática utilizando 50 vídeos muestreados al azar. un
La tasa de precisión del 52% al distinguir entre los dos demostró la
confiabilidad de nuestros datos semiautomáticos, confirmando que nuestra calidad
El control alinea eficazmente las evaluaciones automáticas con el juicio humano.
estándares.

## 6 Conclusión

En este trabajo presentamos Video-ChatGPT, un modelo multimodal que fusiona
un codificador visual previamente entrenado con un modelo de lenguaje grande (LLM) para permitir
comprensión de videos y conversaciones basadas en videos. VideochatGPT
aprovecha un adaptador encima de LLM previamente entrenado y de las redes troncales de visión y
se ajusta con precisión los datos de instrucciones en vídeo para capturar la dinámica temporal y
Relaciones de consistencia espacial en secuencias espaciotemporales. Un conjunto de datos
de 100.000 pares de video-instrucción se crea para mejorar Video-
Capacidades de conversación y comprensión específicas de video de ChatGPT.
El trabajo también introdujo una evaluación cuantitativa de videoconversaciones.
marco para la evaluación comparativa, la evaluación de modelos en un conjunto diverso de
capacidades que incluyen respuesta a preguntas en video convencional, así como
descripciones abiertas.

## 7 limitaciones

Si bien el modelo se desempeña de manera competitiva en varios escenarios, lo observamos
le resulta difícil comprender las relaciones temporales sutiles en
vídeos (&gt; 2 min), lo que puede comprometer su rendimiento predictivo.
Además, tiene dificultades para reconocer los detalles de pequeños
objetos, a menudo faltando información adicional incluida en estos detalles.

## 8 riesgos potenciales

Video-ChatGPT, como cualquier otro modelo de IA, debe manejarse con la debida precaución
prevenir el uso indebido y garantizar que se respeten los principios de equidad,
transparencia y respeto a la privacidad de los usuarios.

Hicimos un esfuerzo concertado para minimizar el sesgo durante la creación del conjunto de datos.
fase para Video-ChatGPT. A pesar de estos esfuerzos, es importante
reconocer la posibilidad de que persista el sesgo residual. El uso de nuestro
modelo debe tener en cuenta estos posibles sesgos, que pueden sutilmente
influyen en la comprensión y respuesta del modelo al contenido visual. Nosotros
Animar a todos los usuarios a considerar estas limitaciones en su aplicación.
de Video-ChatGPT y esforzarnos por lograr un uso ético y responsable en todos
contextos.

## 9 Uso de datos y asistente de IA

Seleccionamos nuestro conjunto de datos en función de un subconjunto del conjunto de datos ActivityNet-200
(Fabian Caba Heilbron y Niebles, 2015), distribuido bajo LICENCIA MIT,
disponibles para su uso en investigaciones. Además, el uso de modelos GPT cumple con
(OpenAI). Respetando la información de la licencia de origen, divulgaremos todos
conjuntos de datos creados en este trabajo bajo LICENCIA MIT.

## 10 anotaciones humanas

La curación semiautomática del conjunto de datos implica anotaciones humanas.
Los anotadores reciben información básica sobre subtítulos de vídeo concisos.
Se dan instrucciones específicas para enriquecer el título con información completa.
descripciones del contenido del vídeo, con especial atención a la temporalidad.
y detalles espaciales. Se les dan instrucciones específicas para neutralizar
el tono y los prejuicios durante el proceso de corrección.

## 11 Evaluación Cualitativa

Realizamos una evaluación exhaustiva de nuestro modelo en una variedad de
Finalizó tareas de respuesta a preguntas en video, utilizando diversos videos obtenidos
de ActivityNet y YouTube. Las tareas de evaluación incluyeron vídeo.
razonamiento (Figura 5), tareas creativas y generativas (ver Figura 6),comprensión espacial (Figura 7), reconocimiento de acciones (Figura 8), video
conversación (Figura 9), respuesta a preguntas (Figura 10) y temporal.
comprensión (Figura 11). Nuestro modelo demuestra competencia en
comprender el contenido de los videos y generar información precisa
respuestas a través de múltiples tareas basadas en video. Nuestro modelo puede efectivamente
comprender la información visual presente en los videos y proporcionar
respuestas precisas (ver Figuras 5 a 11).

## Referencias

- Tom B Brown, Benjamin Mann, Nick Ryder, Melanie Subbiah, Jared Kaplan, Prafulla Dhariwal, Arvind Neelakantan, Pranav Shyam, Girish Sastry, Amanda Askell, et al. 2020. Los modelos lingüísticos aprenden con pocas posibilidades. Preimpresión de arXiv arXiv:2005.14165.
- Wei-Lin Chiang, Zhuohan Li, Zi Lin, Ying Sheng, Zhanghao Wu, Hao Zhang, Lianmin Zheng, Siyuan Zhuang, Yonghao Zhuang, Joseph E. González, Ion Stoica y Eric P. Xing. 2023. Vicuña: un chatbot de código abierto que impresiona a gpt-4 con una calidad de chatgpt del 90%*.
- Wenliang Dai, Junnan Li, Dongxu Li, Anthony Meng Huat Tiong, Junqi Zhao, Weisheng Wang, Boyang Li, Pascale Fung y Steven Hoi. 2023. Instructblip: Hacia modelos de visión y lenguaje de propósito general con ajuste de instrucciones.
- Jean-Baptiste Alayrac et al. 2022. Flamingo: un modelo de lenguaje visual para el aprendizaje en pocas tomas.
- Bernard Ghanem Fabián Caba Heilbron, Víctor Escorcia y Juan Carlos Niebles. 2015. Activitynet: un video de referencia a gran escala para comprender la actividad humana. En Actas de la Conferencia IEEE sobre visión por computadora y reconocimiento de patrones, páginas 961-970.
- Peng Gao, Jiaming Han, Renrui Zhang, Ziyi Lin, Shijie Geng, Aojun Zhou, Wei Zhang, Pan Lu, Conghui He, Xiangyu Yue, Hongsheng Li y Yu Qiao. 2023. Llama-adaptador v2: modelo de instrucción visual eficiente en parámetros. arXiv preimpresión arXiv:2304.15010 .
- Tanmay Gupta, Amita Kamath, Aniruddha Kembhavi y Derek Hoiem. 2022. Hacia sistemas de visión de propósito general: una arquitectura de lenguaje de visión independiente de las tareas de un extremo a otro. En actas de la conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones.
- Xinyu Huang, Youcai Zhang, Jinyu Ma, Weiwei Tian, ​​Rui Feng, Yuejie Zhang, Yaqian Li, Yandong Guo y Lei Zhang. 2023. Tag2text: modelo guía de visión y lenguaje mediante etiquetado de imágenes. arXiv preimpresión arXiv:2303.05657 .
- Yunseok Jang, Yale Song, Youngjae Yu, Youngjin Kim y Gunhee Kim. 2017. Tgif-qa: Hacia el razonamiento espaciotemporal en la respuesta visual a preguntas. En Actas de la conferencia IEEE sobre visión por computadora y reconocimiento de patrones, páginas 2758-2766.
- Laboratorio Kepler. 2019. Katna: Herramienta para automatizar tareas de extracción de fotogramas clave de vídeo, compresión de vídeo, recorte automático de imágenes y cambio de tamaño de imágenes inteligentes. https://github.com/keplerlab/katna.
- Junnan Li, Dongxu Li, Silvio Savarese y Steven Hoi. 2023a. Blip-2: Arranque del entrenamiento previo de imágenes y lenguaje con codificadores de imágenes congeladas y modelos de lenguaje grandes. arXiv preimpresión arXiv:2301.12597 .
- Junnan Li, Dongxu Li, Caiming Xiong y Steven Hoi. 2022. Blip: Arranque del entrenamiento previo del lenguaje-imagen para la comprensión y generación unificadas del lenguaje-visión. En Conferencia internacional sobre aprendizaje automático, páginas 12888-12900. PMLR.
- Kunchang Li, Yinan He, Yi Wang, Yizhuo Li, Wenhai Wang, Ping Luo, Yali Wang, Limin Wang y Yu Qiao. 2023b. Videochat: comprensión del vídeo centrada en el chat. arXiv preimpresión arXiv:2305.06355 .- Feng Liang, Bichen Wu, Xiaoliang Dai, Kunpeng Li, Yinan Zhao, Hang Zhang, Peizhao Zhang, Peter Vajda y Diana Marculescu. 2023. Segmentación semántica de vocabulario abierto con clip adaptado a máscara. En actas de la conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones.
- Haotian Liu, Chunyuan Li, Qingyang Wu y Yong Jae Lee. 2023. Ajuste de instrucciones visuales.
- Muhammad Maaz, Hanoona Rasheed, Salman Khan, Fahad Shahbaz Khan, Rao Muhammad Anwer y Ming-Hsuan Yang. 2022. Detección de objetos independiente de clase con transformador multimodal. En La Conferencia Europea sobre Visión por Computador. Saltador.
- Bolin Ni, Houwen Peng, Minghao Chen, Songyang Zhang, Gaofeng Meng, Jianlong Fu, Shiming Xiang y Haibin Ling. 2022. Ampliación de modelos previamente entrenados en lenguaje-imagen para el reconocimiento de vídeo general. En La Conferencia Europea sobre Visión por Computador.
- OpenAI. Condiciones de uso de Openai. https://openai.com/policies/terms-of-use.
- OpenAI. 2023. Chatgpt. Modelo de lenguaje grande para conversaciones de estilo humano https://chat.openai.com.
- OpenLMLab. 2023. Moss: Código base para el proyecto Moss. Un modelo de lenguaje conversacional de código abierto mejorado con complementos, https://github.com/OpenLMLab/MOSS.
- Long Ouyang, Jeffrey Wu, Xu Jiang, Diogo Almeida, Carroll Wainwright, Pamela Mishkin, Chong Zhang, Sandhini Agarwal, Katarina Slama, Alex Ray, et al. 2022. Entrenamiento de modelos de lenguaje para seguir instrucciones con retroalimentación humana. Avances en sistemas de procesamiento de información neuronal, 35:27730-27744.
- Alec Radford, Jong Wook Kim, Chris Hallacy, Aditya Ramesh, Gabriel Goh, Sandhini Agarwal, Girish Sastry, Amanda Askell, Pamela Mishkin, Jack Clark, et al. 2021. Aprendizaje de modelos visuales transferibles a partir de la supervisión del lenguaje natural. En Conferencia Internacional sobre Aprendizaje Automático.
- Hanoona Rasheed, Muhammad Uzair khattak, Muhammad Maaz, Salman Khan y Fahad Shahbaz Khan. 2023. Los modelos de clips ajustados aprenden en vídeo de forma eficiente. En actas de la conferencia IEEE/CVF sobre visión por computadora y reconocimiento de patrones.
- Hanoona Rasheed, Muhammad Maaz, Muhammad Uzair Khattak, Salman Khan y Fahad Shahbaz Khan. 2022. Cerrar la brecha entre las representaciones a nivel de objetos y de imágenes para la detección de vocabulario abierto. En Avances en sistemas de procesamiento de información neuronal.
- David Rozenberszki, O Letanía y Angela Dai. 2022. Segmentación semántica 3D en interiores basada en el lenguaje en la naturaleza. En La Conferencia Europea sobre Visión por Computador.
- Yixuan Su, Tian Lan, Huayang Li, Jialu Xu, Yan Wang y Deng Cai. 2023. Pandagpt: un modelo para las instrucciones: síguelos a todos. arXiv preimpresión arXiv:2305.16355 .
- Rohan Taori, Ishaan Gulrajani, Tianyi Zhang, Yann Dubois, Xuechen Li, Carlos Guestrin, Percy Liang y Tatsunori B. Hashimoto. 2023. Alpaca de Stanford: un modelo de llama que sigue instrucciones. https://github.com/tatsu-lab/stanford\_alpaca.
- Hugo Touvron, Thibaut Lavril, Gautier Izacard, Xavier Martinet, Marie-Anne Lachaux, Timothée Lacroix, Baptiste Rozière, Naman Goyal, Eric Hambro, Faisal Azhar, Aurelien Rodriguez, Armand Joulin, Edouard Grave y Guillaume Lample. 2023. Llama: Modelos lingüísticos básicos abiertos y eficientes. arXiv preimpresión arXiv:2302.13971 .
- Mengmeng Wang, Jiazheng Xing y Yong Liu. 2021. Actionclip: un nuevo paradigma para el reconocimiento de acciones de vídeo. Preimpresión de arXiv arXiv:2109.08472.- Yi Wang, Kunchang Li, Yizhuo Li, Yinan He, Bingkun Huang, Zhiyu Zhao, Hongjie Zhang, Jilan Xu, Yi Liu, Zun Wang, et al. 2022. Internvideo: Modelos básicos de vídeo general mediante aprendizaje generativo y discriminativo. Preimpresión de arXiv arXiv:2212.03191.
- Jialian Wu, Jianfeng Wang, Zhengyuan Yang, Zhe Gan, Zicheng Liu, Junsong Yuan y Lijuan Wang. 2022. Grit: un transformador generativo de región a texto para la comprensión de objetos. arXiv preimpresión arXiv:2212.00280 .
- Dejing Xu, Zhou Zhao, Jun Xiao, Fei Wu, Hanwang Zhang, Xiangnan He y Yueting Zhuang. 2017. Respuesta a preguntas en vídeo mediante una atención refinada gradualmente sobre la apariencia y el movimiento. En Actas de la 25ª conferencia internacional ACM sobre multimedia, páginas 1645-1653.
- Antoine Yang, Antoine Miech, Josef Sivic, Ivan Laptev y Cordelia Schmid. 2022. Respuesta a preguntas en vídeo de toma cero mediante modelos de lenguaje bidireccional congelados. arXiv preimpresión arXiv:2206.08155 .
- Zhou Yu, Dejing Xu, Jun Yu, Ting Yu, Zhou Zhao, Yueting Zhuang y Dacheng Tao. 2019. Activitynet-qa: un conjunto de datos para comprender videos web complejos mediante la respuesta a preguntas. En Actas de la Conferencia AAAI sobre Inteligencia Artificial, volumen 33, páginas 9127-9134.
- Hang Zhang, Xin Li y Lidong Bing. 2023. Videollama: un modelo de lenguaje audiovisual adaptado a instrucciones para la comprensión de vídeos. arXiv preimpresión arXiv:2306.02858 .
- Susan Zhang, Stephen Roller, Naman Goyal, Mikel Artetxe, Moya Chen, Shuohui Chen, Christopher Dewan, Mona Diab, Xian Li, Xi Victoria Lin, et al. 2022. Opción: modelos abiertos de lenguaje transformador previamente entrenados. arXiv preimpresión arXiv:2205.01068 .
- Xingyi Zhou, Vladlen Koltun y Philipp Krähenbühl. 2021. Detección probabilística en dos etapas. En la preimpresión de arXiv arXiv:2103.07461 .
- Deyao Zhu, Jun Chen, Xiaoqian Shen, Xiang Li y Mohamed Elhoseiny. 2023. Minigpt-4: Mejora de la comprensión del lenguaje visual con modelos avanzados de lenguaje grande. Preimpresión de arXiv arXiv:2304.10592.

Figura 5: Tarea de razonamiento en vídeo. Esta figura ilustra un ejemplo de la demostración de Video-ChatGPT que muestra su desempeño en tareas de razonamiento por video.

<!-- imagen -->

Figura 6: Tareas creativas y generativas. Los ejemplos ilustrativos de la demostración de Video-ChatGPT destacan su desempeño en tareas creativas y generativas basadas en video, como la elaboración de una historia, un poema o un anuncio.

<!-- imagen -->

Figura 7: Tareas de comprensión espacial. La figura muestra ejemplos de la demostración de Video-ChatGPT, enfatizando su capacidad en tareas de comprensión espacial basadas en video, incluida la identificación de ubicaciones reconocidas o el recuento del número de objetos en una escena.

<!-- imagen -->

Figura 8: Tarea de reconocimiento de acciones. Esta figura ilustra ejemplos de la demostración de Video-ChatGPT que muestra su desempeño en tareas de reconocimiento de acciones de video, como tocar la batería y acicalar al caballo.

<!-- imagen -->

Figura 9: Tareas de conversación y comprensión del vídeo. Esta figura ilustra ejemplos de la demostración de Video-ChatGPT que muestran su desempeño en tareas de conversación y comprensión de video.

<!-- imagen -->

Figura 10: Tarea de respuesta a preguntas. La figura muestra ejemplos de demostración de Video-ChatGPT que muestran su desempeño en tareas de respuesta a preguntas.

<!-- imagen -->

Figura 11: Tarea de comprensión temporal. La figura proporciona ejemplos de la demostración de Video-ChatGPT, destacando su desempeño en tareas de comprensión temporal, particularmente en la comprensión de secuencias de eventos.

<!-- imagen -->