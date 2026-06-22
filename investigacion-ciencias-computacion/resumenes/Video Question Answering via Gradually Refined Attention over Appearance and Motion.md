## Respuesta a preguntas sobre video mediante atencion gradualmente refinada sobre apariencia y movimiento

Dejing Xu, Zhou Zhao, Jun Xiao, Fei Wu, Hanwang Zhang, Xiangnan He y Yueting Zhuang.

## Resumen

Este paper aborda Video Question Answering cuando la tarea aun era relativamente nueva. Propone un modelo end-to-end que usa informacion de apariencia y movimiento del video. La pregunta se procesa palabra por palabra y cada palabra ayuda a refinar la atencion sobre frames y clips.

La contribucion central es Attention Memory Unit (AMU), un mecanismo que combina atencion sobre apariencia, atencion sobre movimiento, fusion de canales y memoria de atencion. El modelo muestra mejores resultados que baselines extendidos desde ImageQA y video captioning en datasets MSVD-QA y MSRVTT-QA.

## Palabras clave

Video Question Answering; mecanismo de atencion; red neuronal.

## 1. Introduccion

La cantidad de videos disponibles crece rapidamente. VideoQA busca responder preguntas especificas sobre un video y permite evaluar comprension visual con mas precision que el subtitulado generico. A diferencia del captioning, donde una descripcion breve puede omitir detalles, una pregunta obliga al modelo a localizar informacion concreta.

VideoQA es mas dificil que ImageQA porque el video tiene dimension temporal y puede incluir informacion de varios canales. Los metodos que simplemente extienden ImageQA tienden a debilitar o ignorar la estructura temporal.

El paper propone procesar el video como frames y clips. De los frames extrae apariencia; de los clips extrae movimiento. Luego procesa la pregunta palabra por palabra para refinar la atencion sobre esos canales.

Contribuciones principales:

- Usar conjuntamente apariencia y movimiento para VideoQA.
- Refinar la atencion con informacion gruesa de la pregunta e informacion fina de cada palabra.
- Generar dos datasets, MSVD-QA y MSRVTT-QA, para evaluar el modelo.

## 2. Trabajo relacionado

### Video captioning

El subtitulado de video genera frases que describen el contenido. Muchos enfoques usan CNN, LSTM, RNN, 3D CNN y mecanismos de atencion. Aunque es util, suele producir descripciones generales.

### Image Question Answering

ImageQA requiere responder preguntas en lenguaje natural sobre imagenes. Los trabajos previos usan atencion espacial, conocimiento externo, redes de memoria dinamica y atencion guiada por palabras.

### Video Question Answering

VideoQA requiere responder sobre un video y una pregunta. Los primeros trabajos extendian modelos de captioning o ImageQA, pero normalmente codificaban la pregunta como un solo vector y fusionaban apariencia/movimiento con promedio simple. Este paper propone una atencion refinada gradualmente y una fusion mas explicita de canales.

## 3. Metodo

El modelo recibe un video V y una pregunta Q, y genera una respuesta A. El video se muestrea en frames y clips de 16 frames. Los frames capturan apariencia; los clips capturan movimiento.

### 3.1 Extraccion de caracteristicas

Para apariencia se usa VGG y se extraen activaciones profundas de frames. Para movimiento se usa C3D y se extraen caracteristicas de clips. La pregunta se representa como una secuencia de palabras transformadas mediante embeddings.

### 3.2 Attention Memory Unit

AMU es el nucleo del modelo. En cada paso temporal, el modelo procesa una palabra de la pregunta y actualiza la atencion sobre el video.

AMU contiene cuatro bloques:

- ATT: calcula atencion sobre caracteristicas de video.
- CF: fusiona canales de apariencia y movimiento.
- Memory: usa una LSTM para recordar la historia de atenciones.
- REF: refina las atenciones anteriores y genera una representacion del video.

La primera atencion usa la palabra actual. La fusion de canales decide cuanto peso dar a apariencia y movimiento. Luego una memoria de atencion incorpora informacion de la pregunta procesada y de la representacion anterior. La segunda atencion refina el resultado.

Este diseno permite que palabras clave de la pregunta tengan impacto directo. Por ejemplo, una pregunta sobre "que esta haciendo" puede aumentar la importancia del canal de movimiento, mientras que una pregunta sobre "que objeto" puede concentrarse en apariencia.

### 3.3 Generacion de respuesta

Al finalizar la pregunta, el modelo combina la representacion refinada del video, la memoria de la pregunta y la historia de atencion. La respuesta puede generarse con un clasificador softmax sobre un conjunto predefinido de respuestas o con una LSTM generadora.

## 4. Experimentos

El paper construye MSVD-QA y MSRVTT-QA a partir de datasets de video captioning.

MSVD-QA contiene 1,970 clips y 50,505 pares pregunta-respuesta. MSRVTT-QA contiene 10,000 clips y 243,680 pares.

El modelo usa 20 frames y 20 clips por video. Las caracteristicas de VGG y C3D tienen dimension 4096. Las palabras usan embeddings GloVe de 300 dimensiones. La respuesta se selecciona con softmax entre las 1,000 respuestas mas frecuentes.

### Baselines

Se comparan tres modelos extendidos:

- E-VQA: codifica pregunta y video con LSTM.
- E-SA: usa soft attention sobre frames.
- E-MN: usa redes de memoria end-to-end.

Estos baselines no tratan de forma dedicada la apariencia y el movimiento; fusionan canales con promedio.

### Resultados

El modelo propuesto obtiene mayor precision global en MSVD-QA y MSRVTT-QA. Mejora especialmente en preguntas "what" y "who", que son las mas frecuentes y diversas.

El analisis cualitativo muestra que el modelo atiende correctamente a frames con objetos relevantes y a clips con acciones. En ejemplos de movimiento, el canal de movimiento recibe mas peso; en preguntas sobre objetos, el canal de apariencia domina.

## 5. Conclusion

El paper introduce una arquitectura temprana pero importante para VideoQA. Su valor para el tema actual es historico y conceptual: muestra por que VideoQA necesita atencion temporal y multimodal, no solo transferir tecnicas de ImageQA. Aunque no aborda streaming ni Video-LLM modernos, sus ideas sobre apariencia, movimiento y atencion guiada por pregunta siguen siendo fundamentos para entender Video Question Answering.

## Referencias

Las referencias originales se conservan como citas del paper fuente. Incluyen trabajos sobre TensorFlow, VQA, MSVD, MSR-VTT, VGG, C3D, GloVe, atencion visual, redes de memoria, video captioning y VideoQA temprano.
