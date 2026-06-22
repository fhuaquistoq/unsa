## Video-LLaVA: aprendizaje de una representacion visual unificada mediante alineamiento antes de la proyeccion

Bin Lin, Yang Ye, Bin Zhu, Jiaxi Cui, Munang Ning, Peng Jin y Li Yuan.

GitHub: https://github.com/PKU-YuanGroup/Video-LLaVA

## Resumen

Video-LLaVA propone una base simple y robusta para modelos grandes vision-lenguaje que entienden imagenes y videos. El problema que aborda es que muchos modelos codifican imagenes y videos en espacios separados y luego intentan proyectarlos al LLM mediante capas pobres de proyeccion. Esto causa desalineamiento antes de la proyeccion.

La idea principal es alinear imagenes y videos en un espacio visual comun antes de proyectarlos al LLM. Video-LLaVA usa encoders LanguageBind para mapear distintas modalidades al espacio de lenguaje y entrena conjuntamente con datos de imagen y video. El resultado es un modelo que mejora tanto en benchmarks de video como de imagen.

## 1. Introduccion

Los LLM pueden seguir instrucciones en texto, pero la interaccion humana con el mundo tambien incluye informacion visual. Los LVLM extienden los LLM para procesar imagenes y videos. Sin embargo, comprender video es mas dificil que comprender imagenes por la dimension temporal.

Muchos modelos usan encoders separados para imagen y video. El paper sostiene que esto dificulta al LLM aprender interacciones multimodales, porque las features llegan desalineadas al proyector. Video-LLaVA resuelve esto alineando antes de proyectar.

El modelo aprende de imagenes y videos en un entrenamiento conjunto. Esto permite que ambas modalidades se refuercen mutuamente: las imagenes aportan detalle visual y los videos aportan dinamica temporal.

## 2. Trabajo relacionado

Los LLM como LLaMA, Vicuna, Alpaca e InstructGPT se entrenan principalmente para texto. Para volverlos multimodales existen dos enfoques generales:

- Tratar el LLM como planificador que coordina herramientas visuales externas.
- Tratar el LLM como decoder que recibe tokens visuales proyectados.

Video-LLaVA pertenece al segundo grupo. A diferencia de modelos que proyectan modalidades separadas, usa representaciones prealineadas y entrenamiento conjunto.

## 3. Video-LLaVA

### Estructura del modelo

Video-LLaVA contiene encoders LanguageBind, un LLM como Vicuna, capas de proyeccion visual y embeddings de texto. Los encoders transforman imagenes y videos al espacio textual, generando una representacion visual unificada. Luego una proyeccion compartida adapta esa representacion al LLM.

### Representacion visual unificada

La premisa es que la misma informacion puede expresarse como texto, imagen o video. Por ejemplo, un perro corriendo puede describirse con lenguaje, mostrarse en una imagen o verse en un video. Si esas modalidades se mapean a un espacio comun, el LLM puede aprender relaciones mas consistentes.

### Alineamiento antes de proyeccion

LanguageBind parte de OpenCLIP para alinear imagen-lenguaje y luego alinea video con lenguaje usando pares video-texto. Asi, imagen y video convergen a un espacio visual compartido. La proyeccion al LLM se aplica despues de esta alineacion.

## 3.2 Pipeline de entrenamiento

El entrenamiento tiene dos etapas:

1. Entrenamiento de comprension: el modelo aprende a interpretar senales visuales a partir de pares imagen/video-texto.
2. Instruction tuning: el modelo aprende a responder instrucciones mas complejas, incluyendo dialogos multi-turno.

Durante el entrenamiento se mezclan muestras de imagen y video en el mismo batch.

## 4. Experimentos

Video-LLaVA usa Vicuna-7B v1.5, encoders visuales de LanguageBind y proyecciones compartidas de dos capas. En video se muestrean 8 frames por clip. La primera etapa usa datos LAION-CC-SBU y WebVid; la segunda usa instrucciones de LLaVA 1.5 y Video-ChatGPT.

### Video Question Answering

En MSVD-QA, MSRVTT-QA, TGIF-QA y ActivityNet-QA, Video-LLaVA supera a Video-ChatGPT y compite con Chat-UniVi. Esto valida que la representacion visual unificada ayuda a responder preguntas sobre video.

### Image Question Answering

En benchmarks de imagen, Video-LLaVA tambien logra resultados competitivos. Esto muestra que el entrenamiento conjunto con video no degrada la comprension de imagenes; al contrario, puede mejorarla.

### Alucinacion de objetos

El modelo tiene buen desempeno en POPE, lo que sugiere que la representacion visual unificada ayuda a mantener consistencia entre contenido visual y descripcion textual.

## 4.3 Ablaciones

El paper compara representaciones unificadas frente a separadas. Los resultados muestran que alinear antes de proyectar mejora tanto video como imagen. Tambien se evalua el efecto del entrenamiento conjunto: entrenar con imagenes y videos mejora el desempeno en video respecto a entrenar solo con video.

## 5. Limitaciones y futuro

Video-LLaVA tiene limitaciones en videos largos porque usa solo 8 frames uniformemente muestreados. Esto puede perder detalles temporales importantes. El entrenamiento tambien es costoso: requiere varios dias en GPUs A100.

El trabajo futuro incluye proyecciones compartidas mas eficientes, compresion de tokens, extension a modalidades como profundidad e infrarrojo, e incorporacion de embeddings temporales.

## 6. Conclusion

Video-LLaVA demuestra que el alineamiento antes de la proyeccion es una decision arquitectonica importante. Para el tema Video-LLM y VideoQA, este paper funciona como base: no esta centrado en streaming, pero ofrece un backbone multimodal fuerte que trabajos posteriores, como ReKV y rLiVS, pueden usar o comparar.

## Referencias

Las referencias originales se conservan como citas del paper fuente. Incluyen trabajos sobre Flamingo, PaLM, LLaVA, Video-ChatGPT, Video-LLaMA, ImageBind, LanguageBind, Vicuna, MSVD-QA, MSRVTT-QA, TGIF-QA, ActivityNet-QA y POPE.
