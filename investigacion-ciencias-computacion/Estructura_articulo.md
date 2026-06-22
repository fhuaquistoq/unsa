# ITFormer: Bridging Time Series and Natural Language for Multi-Modal QA with Large-Scale Multitask Dataset

Wang, Y., Lei, P., Song, J., Hao, Y., Chen, T., Zhang, Y., ... & Wei, Z. (2025). *ITFormer: Bridging time series and natural language for multi-modal QA with large-scale multitask dataset*. arXiv preprint arXiv:2506.20093.

Lo primero que tenemos que responder al leer un paper son estas 4 preguntas:

1. ¿Qué problema aborda?
2. ¿Por qué es difícil?
3. ¿Qué propone el paper?
4. ¿Cómo demuestra que funciona?

## Problema

Integrar series temporales complejas (sensores, señales) con lenguaje natural para responder preguntas.

## Es difícil porque

- Las series temporales son señales numéricas de alta dimensión.
- Los modelos de lenguaje trabajan con texto.

## Propuesta: idea del paper

Proponer **ITFormer**, un framework que conecta un encoder de series temporales con un LLM.

Crear el dataset **EngineMT-QA**, un benchmark multimodal para este problema (propuesta adicional).

# Análisis de la estructura del paper científico

## I. Título

El título debe ser claro, específico, incluye el nombre del método y describe el problema.

**Método + Problema + dominio**

> **ITFormer: Bridging Time Series and Natural Language for Multi-Modal QA**

- Indica la técnica: ITFormer.
- Indica el problema: time series + Natural Language.
- Indica la tarea: Multimodal QA.

**Revisor:** con el título infiere área del paper, técnica y aplicación.

Un buen título responde a:

- ¿Qué hiciste?
- ¿En qué problema?

## II. Abstract

Es la parte más crítica del paper.

**Estructura:**

- Contexto.
- Problema.
- Método.
- Resultados.

**Contexto + Problema + Método + Resultados**

- **Contexto:** se describe el crecimiento de modelos multimodales.
- **Problema:** los LLMs no manejan bien series temporales numéricas.
- **Método:** se propone ITFormer, que integra señales temporales con lenguaje.
- **Resultados:** se introduce un dataset nuevo y se muestran mejoras experimentales.

**Revisor:** analiza que el abstract describa un problema claro, contribución clara, mencionar dataset, mencionar resultados. Muchos revisores prefieren ver números concretos en el abstract. Por ejemplo, *improves accuracy by X%*, esto aumenta credibilidad.

**Plantilla:**

```text
Existing methods for X suffer from Y.
In this paper we propose Z.
We evaluate our approach on dataset D.
Results show improvement in metric M.
```

## III. Introducción

En la introducción se evalúa la motivación científica.

**Estructura:** aplicando el modelo CARS (*Create A Research Space*):

1. Establecer el territorio.
2. Identificar la brecha.
3. Ocupar la brecha.

### a) Establecer el territorio

- Multimodal learning.
- Integración de diferentes tipos de datos.

### b) Identificar la brecha

- Los LLMs trabajan con texto.
- Las series temporales son señales numéricas.

Por lo tanto:

- Hay una brecha entre ambos dominios.

### c) Ocupar la brecha

- ITFormer.

Además:

- Nuevo dataset.
- Nuevas tareas.

La introducción debe contar una historia científica:

- Contexto general: área importante.
- Problema no resuelto.
- Propuesta.
- Contribuciones.

### Párrafo 1: Contexto general

- Multimodal AI.
- Series temporales.

### Párrafo 2: Problema

No existe integración efectiva con LLMs.

### Párrafo 3: Propuesta

Proponer ITFormer.

### Párrafo 4: Contribuciones

- A new framework (ITFormer).
- A new dataset (engineMT-QA).
- Extensive experiments.

El revisor analiza si existen trabajos previos similares que resuelvan el problema. Por lo tanto se deben mencionar en la introducción.

**Plantilla:**

```text
Problema: Current solutions fail because...
Propuesta: We propose...
Key insight
Results
Impact
```

## IV. Trabajos relacionados

**Estructura:** debe estar organizado por categorías temáticas y citar trabajos relevantes.

**Regla:** no se debe escribir paper por paper. Se debe escribir por categorías.

El paper agrupa trabajos previos en categorías como:

- Modelos de series temporales.
- LLMs para razonamiento.
- Multimodal learning.

El revisor evalúa si el autor conoce el estado del arte y si posiciona su propuesta dentro de una categoría, realizando una comparación conceptual clara.

**Errores comunes:** muchos escriben: "El Paper A hace esto...", "El paper B hace esto...", etc.

Debe ser:

- Categoría 1: ...
- Categoría 2: ...
- Etc.

También puede incluir:

- Tabla comparativa por atributos.

## V. Metodología

Se responde a la pregunta:

> **¿La idea es realmente nueva?**

La metodología debe incluir:

- Diagramas.
- Ecuaciones.
- Explicación clara.

ITFormer incluye componentes como:

- **Time Token Position Encoding:** codificación temporal.
- **Instruct Time Attention:** mecanismo de atención adaptado.
- **Learnable Instruct Tokens:** tokens instructivos.

Estos mecanismos permiten alinear señales temporales con texto dentro de un modelo tipo transformer.

### Dataset

- Introduce el dataset: **EngineMT-QA**.
- Características:
  - Más de 87k preguntas y respuestas.
  - Datos de sensores de motores.
  - Múltiples tareas cognitivas.

La metodología debe permitir:

- Reproducibilidad.
- Claridad.
- Formalización.

Debe incluir:

- Ecuaciones.
- Diagramas.
- Pseudocódigo.

### Formato del experimento

- Se define baselines, datasets y métricas.
- Mínimo debe tener 2 baselines, 3 experimentos.

El revisor debe responder a la pregunta:

- ¿Podría implementar esto?
- ¿Los baselines son realmente el estado del arte?

## VI. Resultados

Se demuestra si el paper realmente aporta algo.

- ¿Vale la pena el método comparado con el existente?

ITFormer demostró cómo:

- Mejora accuracy.
- Mejora F1.
- Usa menos parámetros entrenables.

Compara entre ITFormer y modelos existentes.

## VII. Discusión

Se analiza cuándo funciona mejor y posibles limitaciones.

- No ocultar limitaciones.

El paper explica:

- Cuándo funciona mejor.
- Qué limitaciones existen.

## VIII. Conclusión

Resume:

- Qué se propuso.
- Qué resultados se lograron.
- Impacto.
- Qué trabajo futuro existe.

## IX. Referencias bibliográficas

## Validar y modificar con la IA

- CARS (*Create A Research Space*).
- Gap.
- Tight framing: plantea tensión y pregunta de investigación clara.
- Golden thread.
- Hook: describe la relevancia del tema.

Herramientas sugeridas:

- **Thesify.ai:** mapear argumentos, identifica claim.
- **Scite.ai:** reference check.
