# Análisis previo para la actualización de `document.tex`

Este archivo resume qué exige la consigna, qué faltaba en el entregable actual y cuál es el plan de reestructuración seguido antes de editar el documento principal.

## 1. Qué requiere la consigna

La tarea solicita convertir el borrador actual en un informe académico preliminar en español, conservando la portada y desarrollando al menos estas partes:

- **Introducción** con cuatro párrafos diferenciados:
  1. contexto, motivación y justificación;
  2. trabajos relacionados y cómo abordaron el contexto;
  3. definición del problema;
  4. objetivo del informe.
- **Marco teórico** con:
  - definiciones generales del tema;
  - una **taxonomía del tema** basada en el paper *Video Question Answering via Gradually Refined Attention over Appearance and Motion*;
  - desarrollo detallado de **Video-LLaVA**;
  - desarrollo detallado de **ReKV / Streaming Video Question-Answering with In-Context Video KV-Cache Retrieval**;
  - desarrollo detallado de un **tercer paper complementario**.
- Redacción en forma de **texto académico continuo**, no como lista de apuntes.
- Inclusión de **citas o marcadores de referencia en estilo compatible con LaTeX**, sin romper la compilación del archivo si la bibliografía todavía no está conectada.

## 2. Qué faltaba en `document.tex`

El archivo original tenía una estructura todavía demasiado mínima para la consigna:

- Contenía solo dos secciones breves:
  - `Problema de investigación`
  - `Paper base y papers seminales`
- No tenía una **Introducción** formal con cuatro párrafos diferenciados.
- No tenía **Marco teórico**.
- No desarrollaba ninguna **taxonomía** del tema.
- No explicaba con detalle **Video-LLaVA**, **ReKV** ni un tercer artículo.
- No articulaba el texto como un **informe preliminar académico**, sino como un apunte corto de selección bibliográfica.
- No incorporaba referencias textuales suficientemente visibles para sostener el análisis.

## 3. Fuentes utilizadas

### Fuentes estructurales

- `Estructura artículo.pdf`
- `ITFormer.pdf`

Estas dos fuentes se usaron solo como guía de forma: organización de la introducción, progresión argumental y nivel esperado de explicación metodológica.

### Fuente base para la taxonomía

- `docling/Video Question Answering via Gradually Refined Attention over Appearance and Motion.md`

Se usó como base conceptual para organizar una taxonomía funcional del VideoQA alrededor de la distinción entre **apariencia**, **movimiento** e **integración de ambas evidencias**.

### Papers técnicos principales

- `papers/Video-LLaVA. Learning United Visual Representation by Alignment Before Projection.pdf`
- `papers/STREAMING VIDEO QUESTION-ANSWERING WITH IN-CONTEXT VIDEO KV-CACHE RETRIEVAL.pdf`
- `papers/Video-ChatGPT. Towards Detailed Video Understanding via Large Vision and Language Models.pdf`

## 4. Por qué se eligió Video-ChatGPT como tercer paper

Se eligió **Video-ChatGPT** como tercer artículo porque complementa mejor a los otros dos desde una perspectiva histórica y metodológica:

- **Video-LLaVA** enfatiza la **alineación de representación visual unificada** antes de la proyección al LLM.
- **ReKV** enfatiza la **eficiencia, memoria y recuperación de contexto** para videos largos o en streaming.
- **Video-ChatGPT** cubre de forma clara el paso intermedio hacia los **Video-LLMs conversacionales**, con una arquitectura relativamente interpretable y una contribución fuerte en **datos instruccionales** y **evaluación conversacional**.

Por eso, Video-ChatGPT funciona como buen puente entre el VideoQA clásico y los enfoques modernos basados en LLMs. Además, la propia consigna ya lo sugería como preferencia salvo que hubiese evidencia más fuerte en otra dirección, y la lectura de las fuentes no justificó reemplazarlo.

## 5. Plan propuesto sección por sección antes de editar

### Portada

- Mantener la portada existente.
- Ajustar solo el subtítulo para que refleje mejor el contenido del informe preliminar.

### Introducción

- **Párrafo 1**: establecer el territorio del problema (VideoQA, multimodalidad, relevancia).
- **Párrafo 2**: ubicar trabajos relacionados y la evolución del campo.
- **Párrafo 3**: definir con precisión el problema de investigación actual.
- **Párrafo 4**: declarar el objetivo del informe.

### Marco teórico

- Abrir con una definición general de VideoQA y sus exigencias técnicas.
- Explicar por qué el video añade dificultad frente a imagen + pregunta.
- Introducir una taxonomía funcional basada en Xu et al. (2017).

### Taxonomía

- Presentar tres categorías conceptualmente coherentes y ancladas en la base del paper:
  - preguntas sobre **apariencia**;
  - preguntas sobre **movimiento**;
  - preguntas de **integración apariencia-movimiento**.

### Desarrollo de artículos

- **Video-LLaVA**:
  - problema que intenta resolver;
  - idea de alineación antes de proyección;
  - estructura general y relevancia.
- **ReKV**:
  - paso de VideoQA offline a streaming;
  - uso de sliding window y recuperación de KV-cache;
  - relevancia para contexto largo.
- **Video-ChatGPT**:
  - adaptación de LLaVA al video;
  - rasgos espaciotemporales y capa de proyección;
  - importancia del dataset instruccional.

### Cierre preliminar

- Añadir un breve cierre que conecte las tres líneas técnicas y deje planteada la evolución del área.

## 6. Decisiones de redacción

- Mantener un tono **académico preliminar**, con afirmaciones cuidadosas.
- Evitar inventar resultados o detalles que no aparezcan en las fuentes.
- Usar referencias textuales del tipo **[Referencia preliminar: ...]** para no depender todavía de un `.bib` y mantener el archivo LaTeX compilable.
